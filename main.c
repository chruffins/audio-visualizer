#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<complex.h>
#include<memory.h>
#include<sqlite3.h>
#include<fftw3.h>

#include "core/menu.h"
#include "core/event_handler.h"
#include "core/app_state.h"
#include "core/inputstate.h"

#include "graphics/particle.h"
#include "graphics/font.h"
#include "graphics/camera.h"
#include "graphics/geometry.h"
#include "graphics/visualization.h"

#include "music/song.h"
#include "music/database.h"
#include "music/audio_parser.h"
#include "music/play_queue.h"

#include<allegro5/allegro.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_native_dialog.h>
#include<allegro5/allegro_color.h>
#include<allegro5/allegro_image.h>

#define PI 3.14159265
#define PARTICLE_LIMIT 8192
#define SAMPLES 1024

void must_init(int test, const char* name) {
    if (test) return;

    printf("%s failed to init!\n", name);
    exit(EXIT_FAILURE);
}

void do_inits() {
    must_init(al_init(), "allegro");
    must_init(al_init_acodec_addon(), "allegro acodec");
    must_init(al_init_primitives_addon(), "allegro primitives");
    must_init(al_init_native_dialog_addon(), "allegro dialog/menu");
    must_init(al_init_font_addon(), "allegro font");
    must_init(al_init_ttf_addon(), "allegro ttf");
    must_init(al_install_audio(), "allegro audio");
    must_init(al_init_image_addon(), "allegro image");
   
    must_init(al_install_mouse(), "mouse");
    must_init(al_install_keyboard(), "keyboard");

    init_app_state();
}

void update_buffer(void* buf, unsigned int samples, void* data) {
    //static int pos = 0;
    float* fbuf = (float*)buf;

    // assume stereo i.e. 2 channels
    ch_vis_buffer_add_samples(get_app_state()->vb, fbuf);
    /*
    for (int i = 0; i < samples; i++) {
        waveform_buffer[pos++] = fbuf[i * 2];
        if (pos == SAMPLES) {
            memcpy(waveform, waveform_buffer, SAMPLES * sizeof(float));
            pos = 0;
            break;
        }
    }
    */
   ch_vis_buffer_do_fft(get_app_state()->vb);
}

void handle_input() {
    double x = 0, y = 0, z = 0, xyz = 0;
    ch_app_state* app = get_app_state();

    if (app->input.key_down[ALLEGRO_KEY_W]) z += 1;
    if (app->input.key_down[ALLEGRO_KEY_S]) z -= 1;
    if (app->input.key_down[ALLEGRO_KEY_A]) x += 1;
    if (app->input.key_down[ALLEGRO_KEY_D]) x -= 1;
    if (app->input.key_down[ALLEGRO_KEY_E]) y += 1;
    if (app->input.key_down[ALLEGRO_KEY_Q]) y -= 1;

    xyz = sqrt(x * x + y * y + z * z);
    if (xyz > 0) {
        x /= xyz;
        y /= xyz;
        z /= xyz;

        camera_move(&app->cam, -0.1 * x, -0.1 * y,
            -0.1 * z);
    }

    if (app->input.mouse_buttons[1]) {
        // printf("%d %d\n", input.mouse_dx, input.mouse_dy);
        camera_rotate_around_axis(&app->cam, app->cam.x,
            -0.01 * app->input.mouse_dy);
        camera_rotate_around_axis(&app->cam, (vector3){0, 1, 0},
            -0.01 * app->input.mouse_dx);
    }
}

void add_vertex(double x, double y, double z, double u, double v, ALLEGRO_COLOR color) {
    int i = get_app_state()->triangles_n++;

    get_app_state()->triangles[i] = (ALLEGRO_VERTEX){ .x = x, .y = y, .z = z, .u = u, .v = v, .color = color };
}

void add_quad(double x, double y, double z, double u, double v,
   double ux, double uy, double uz, double uu, double uv,
   double vx, double vy, double vz, double vu, double vv,
   ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
    add_vertex(x, y, z, u, v, c1);
    add_vertex(x + ux, y + uy, z + uz, u + uu, v + uv, c1);
    add_vertex(x + vx, y + vy, z + vz, u + vu, v + vv, c2);
    add_vertex(x + vx, y + vy, z + vz, u + vu, v + vv, c2);
    add_vertex(x + ux, y + uy, z + uz, u + uu, v + uv, c1);
    add_vertex(x + ux + vx, y + uy + vy, z + uz + vz, u + uu + vu,
        v + uv + vv, c2);
}

void draw_scene() {
    ALLEGRO_TRANSFORM projection = *al_get_current_projection_transform();
    double pitch, yaw, roll;
    ch_app_state* app = get_app_state();

    draw_song_status(app->display, app->info_cube_texture, app->def, app->song, 0, 0);

    // all of our 3d elements go here
    setup_3d_projection(&app->cam);
    al_clear_to_color(al_map_rgb(25, 25, 25));

    al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
    al_clear_depth_buffer(1);

    update_camera_transform(&app->cam);

    ALLEGRO_TRANSFORM rot_only = get_skybox_camera_transform(&app->cam);
    ALLEGRO_TRANSFORM combined = app->skybox.box.transform;
    al_compose_transform(&combined, &rot_only);
    al_use_transform(&combined);
    ch_skybox_draw(&app->skybox);
    
    for (int i = 0; i < app->models.size; i++) {
        ch_model* model = ch_model_vec_at(&app->models, i);

        if (model->transform_dirty) ch_model_recalculate_transform(model);

        combined = model->transform;
        al_compose_transform(&combined, &app->cam.transform);
        al_use_transform(&combined);
        ch_model_draw(model);
    }

    al_use_transform(&app->cam.transform);
    al_draw_prim(app->triangles, NULL, app->test_texture, 0, app->triangles_n, ALLEGRO_PRIM_TRIANGLE_LIST);

    // back to 2d we go
    al_use_transform(&app->identity_transform);
    al_use_projection_transform(&projection);
    al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

    al_hold_bitmap_drawing(true);
    int lh = al_get_font_line_height(app->def);
    al_draw_textf(app->def, al_map_rgb(255, 0, 0), 0, lh * 0, 0, "%.0f FPS", app->fps);
    al_draw_textf(app->def, al_map_rgb(255, 0, 0), 0, lh * 1, 0, "pos: %.1f, %.1f, %.1f", app->cam.pos.x, app->cam.pos.y, app->cam.pos.z);
    
    pitch = camera_get_pitch(&app->cam) * 180 / PI;
    yaw = camera_get_yaw(&app->cam) * 180 / PI;
    roll = camera_get_roll(&app->cam) * 180 / PI;
    al_draw_textf(app->def, al_map_rgb(255, 0, 0), 0, lh * 2, 0, "pitch: %+4.0f, yaw: %+4.0f, roll: %+4.0f", pitch, yaw, roll);
    al_hold_bitmap_drawing(false);
    draw_frequency_bins(app->display, app->test_texture, app->vb);

    al_flip_display();
}

void add_checkerboard(void)
{
   int x, y;
   ALLEGRO_COLOR c1 = al_color_name("white");
   ALLEGRO_COLOR c2 = al_color_name("black");

   for (y = 0; y < 20; y++) {
      for (x = 0; x < 20; x++) {
         double px = x - 20 * 0.5;
         double py = 0.2;
         double pz = y - 20 * 0.5;
         ALLEGRO_COLOR c = c1;
         if ((x + y) & 1) {
            c = c2;
            py -= 0.1;
         }
         add_quad(px, py, pz, 0, 0,
            1, 0, 0, 512, 0,
            0, 0, 1, 0, 512,
            c, c);
      }
   }
}

void run_main_loop() {
    ch_app_state* app = get_app_state();
    app->display = al_create_display(app->width, app->height);

    app->skybox = ch_skybox_load("assets/1.png", "assets/2.png", "assets/3.png", "assets/4.png", "assets/5.png", "assets/6.png");

    char* filepath;

    app->def = get_default_font();

    ALLEGRO_MENU* menu = get_menu();
    al_set_display_menu(app->display, menu);

    ALLEGRO_MOUSE_STATE mouse_state;
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT event;

    al_attach_mixer_to_voice(app->mixer, app->voice);
    al_reserve_samples(32);

    app->song = ch_song_vec_rand(&app->songs);
    if (app->song) {
        app->stream = al_load_audio_stream(app->song->filename, 4, 4096);
        if (!app->stream) {
            printf("Failed to create stream...\n");
        }
        al_attach_audio_stream_to_mixer(app->stream, app->mixer);
        al_set_mixer_postprocess_callback(app->mixer, update_buffer, NULL);
    }

    bool redraw = false;
    bool unfinished = true;
    bool mousedown = false;
    int mode = 0;
    
    // const vector2 center = { width / 2, height / 2 };
    // TODO: actually make something for rendering geometry
    // song = ch_song_load(filename);

    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(app->display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_default_menu_event_source());

    if (app->stream) {
        al_register_event_source(queue, al_get_audio_stream_event_source(app->stream));
    }

    al_start_timer(timer);
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    app->test_texture = al_create_bitmap(512, 512);
    app->info_cube_texture = al_create_bitmap(512, 512);
    al_set_target_bitmap(app->test_texture);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_set_target_backbuffer(app->display);

    ch_model info_cube;
    //ch_model_move(&info_cube, vector3_new(0, 10, 0));
    info_cube.texture = app->info_cube_texture;
    ch_model_init_cube(&info_cube, app->info_cube_texture, 5, 0, 10, 0);
    ch_model_rotate(&info_cube, vector3_new(0, PI / 4., 0));

    ch_model_vec_push(&app->models, ch_model_load("assets/teapot.obj", ALLEGRO_PRIM_BUFFER_STATIC));
    ch_model_vec_push(&app->models, info_cube);
    //ch_model_init_cube(&model, 40, 0, 0, 0);
    
    // al_load_bitmap("assets/plank.jpeg");
    /*
    al_create_bitmap(512, 512);
    al_set_target_bitmap(test_texture);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_set_target_backbuffer(display);
    */
    if (app->test_texture) {
        //model.texture = test_texture;
    } else {
        printf("failed to add texture!\n");
    }

    add_quad(0, 0, 0, 0, 0, 
        0, 0, 3, 512, 0,
        3, 0, 0, 0, 512,
        al_map_rgb(255, 0, 0), al_map_rgb(0, 0, 255));
    add_quad(0, 3, 0, 0, 0, 
        0, 0, 3, 1, 0,
        3, 0, 0, 0, 1,
        al_map_rgb(0, 255, 0), al_map_rgb(0, 0, 255));

    add_checkerboard();

    while (unfinished) {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            handle_display_resize(&event);
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            unfinished = false;
            break;
        case ALLEGRO_EVENT_AUDIO_STREAM_FINISHED:
            handle_audio_stream_finished(&event);
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            handle_key_down(&event);
            break;
        case ALLEGRO_EVENT_KEY_UP:
            handle_key_up(&event);
            break;
        case ALLEGRO_EVENT_TIMER:
            for (int i = 0; i < SAMPLES; i++) {
                app->particles_ptr = (app->particles_ptr + 1) % PARTICLE_LIMIT;

                // particles[particles_ptr] = particle_create_params(vector2_new((i*((double)width / SAMPLES)), (center.y / 2.0) + (waveform[i] * 250)), vector2_new(0,0), 
                // vector2_new(0,0), al_map_rgb(0, 255, 0), 2);
            }

            for (int i = 0; i < PARTICLE_LIMIT; i++) {
                particle_update(&app->particles[i]);
            }
            handle_input();

            for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                if (app->input.key_state[i] == 0) app->input.key_down[i] = 0;
            }
            app->input.mouse_dx = 0;
            app->input.mouse_dy = 0;
            redraw = true;
            break;
        case ALLEGRO_EVENT_MENU_CLICK:
            if (event.user.data1 == MENU_FILE_FOLDER) {
                filepath = choose_album_folder();
                printf("user picked folder %s\n", filepath);
                if (filepath) {
                    import_music_from_folder_recursive(app->db, filepath);
                    //print_music_files(db, filepath);
                    free(filepath);
                }
            }
            handle_menu_click(&event);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.button == 1) {
                printf("mouse down @ %d %d!\n", event.mouse.x, event.mouse.y);
                mousedown = true;
            }
            app->input.mouse_buttons[event.mouse.button] = 1;
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (event.mouse.button == 1) {
                mousedown = false;
            }
            app->input.mouse_buttons[event.mouse.button] = 0;
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            app->input.mouse_dx += event.mouse.dx;
            app->input.mouse_dy += event.mouse.dy;
            break;
        default:
            break;
        }
        if (event.type == ALLEGRO_EVENT_TIMER && mousedown) {
            al_get_mouse_state(&mouse_state);
            for (int i = 0; i < 5; i++) {
                app->particles[app->particles_ptr] = particle_create_params(
                    (vector2){ mouse_state.x, mouse_state.y },
                    (vector2){ (rand() % 9) - 4, (rand() % 9) - 4 },
                    (vector2){ 0, 0.3 },
                    al_map_rgb(rand() % 255, rand() % 255, rand() % 255),
                    300
                );
                app->particles_ptr = (app->particles_ptr + 1) % PARTICLE_LIMIT;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            draw_scene();

            app->frames_done++;
        }

        double game_time = al_get_time();
        if (game_time - app->old_time >= 0.2) {
            app->fps = app->frames_done / (game_time - app->old_time);

            app->frames_done = 0;
            app->old_time = game_time;
        }
    }

    al_destroy_audio_stream(app->stream);
    al_destroy_mixer(app->mixer);
    al_destroy_voice(app->voice);
    al_destroy_display(app->display);
    al_destroy_event_queue(queue);
}

int main(int argc, char **argv) {
    FILE* fp;
    ch_app_state* app = get_app_state();

    fp = fopen("recent.txt", "r");
    if (fp) {
        app->filename = malloc(sizeof(char)*300);
        fgets(app->filename, 300, fp);
        fclose(fp);
    } else {
        app->filename = "./bitterend.ogg";
    }

    do_inits();
    run_main_loop();

    sqlite3_close(app->db);

    return 0;
}