#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<complex.h>
#include<memory.h>

#include<fftw3.h>
#include "menu.h"
#include "particle.h"
#include "font.h"
#include "camera.h"

#include<allegro5/allegro.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_native_dialog.h>
#include<allegro5/allegro_color.h>

#define PI 3.14159265
#define PARTICLE_LIMIT 8192
#define SAMPLES 1024

static char* filename = NULL;
static const int width = 1920;
static const int height = 1080;
static ALLEGRO_DISPLAY* display;

static float waveform_buffer[SAMPLES] = { 0 };
static float waveform[SAMPLES] = { 0 };
static ALLEGRO_VERTEX waveform_visual_buffer[SAMPLES] = { 0 };

static ALLEGRO_VERTEX triangles[3*1000] = { 0 };
static int triangles_n = 0;

static camera cam = { 0 };

static particle* particles = NULL;
static int particles_ptr = 0;

// FPS tracking
static double fps = 0;
static uint64_t frames_done = 0;
static double old_time = 0;

static ALLEGRO_TRANSFORM identity_transform;

static ALLEGRO_FONT* def = NULL;

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
   
    must_init(al_install_mouse(), "mouse");
    must_init(al_install_keyboard(), "keyboard");

    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_GTK_TOPLEVEL | ALLEGRO_PROGRAMMABLE_PIPELINE);

    particles = malloc(sizeof(particle)*PARTICLE_LIMIT);
    al_identity_transform(&identity_transform);

    cam.x.x = 1;
    cam.y.y = 1;
    
    cam.z = vector3_new(0, 0, 0);
    cam.pos = vector3_new(0, 100, 0);

    cam.fov = 60 * ALLEGRO_PI / 180;
}

void update_buffer(void* buf, unsigned int samples, void* data) {
    static int pos = 0;
    float* fbuf = (float*)buf;

    // assume stereo i.e. 2 channels
    for (int i = 0; i < samples; i++) {
        waveform_buffer[pos++] = fbuf[i * 2];
        if (pos == SAMPLES) {
            memcpy(waveform, waveform_buffer, SAMPLES * sizeof(float));
            pos = 0;
            break;
        }
    }
}

void handle_input() {

}

void add_vertex(double x, double y, double z, double u, double v, ALLEGRO_COLOR color) {
    int i = triangles_n++;

    triangles[i] = (ALLEGRO_VERTEX){ .x = x, .y = y, .z = z, .u = u, .v = v, .color = color };
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

    // all of our 3d elements go here
    setup_3d_projection(&cam);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
    al_clear_depth_buffer(1);

    build_and_use_camera_transform(&cam);

    //al_hold_bitmap_drawing(true);
    for (int i = 0; i < PARTICLE_LIMIT; i++) {
        particle_draw(&particles[i]);
    }

    al_draw_prim(waveform_visual_buffer, NULL, NULL, 0, SAMPLES, ALLEGRO_PRIM_LINE_LIST);
    al_draw_prim(triangles, NULL, NULL, 0, triangles_n, ALLEGRO_PRIM_TRIANGLE_LIST);

    // back to 2d we go
    al_use_transform(&identity_transform);
    al_use_projection_transform(&projection);
    al_set_render_state(ALLEGRO_DEPTH_TEST, 0);

    int lh = al_get_font_line_height(def);
    al_draw_textf(def, al_map_rgb(255, 0, 0), 0, lh * 0, 0, "%.0f FPS", fps);
    al_draw_textf(def, al_map_rgb(255, 0, 0), 0, lh * 1, 0, "pos: %.1f, %.1f, %.1f", cam.pos.x, cam.pos.y, cam.pos.z);
    al_draw_textf(def, al_map_rgb(255, 0, 0), 0, lh * 2, 0,
      "look: %+3.1f/%+3.1f/%+3.1f",
         -cam.z.x, -cam.z.y, -cam.z.z);
    //al_hold_bitmap_drawing(false);

    al_flip_display();
}

void run_main_loop() {
    display = al_create_display(width, height);

    char* filepath;

    def = get_default_font();

    ALLEGRO_MENU* menu = get_menu();
    al_set_display_menu(display, menu);

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT event;

    ALLEGRO_VOICE* voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    ALLEGRO_MIXER* mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

    ALLEGRO_MOUSE_STATE mouse_state;
    FILE* fp = NULL;

    al_attach_mixer_to_voice(mixer, voice);
    al_reserve_samples(32);

    ALLEGRO_AUDIO_STREAM* stream = al_load_audio_stream(filename, 4, 4096);
    if (!stream) {
        printf("Failed to create stream...\n");
    }
    al_attach_audio_stream_to_mixer(stream, mixer);
    al_set_mixer_postprocess_callback(mixer, update_buffer, NULL);

    bool redraw = false;
    bool unfinished = true;
    bool mousedown = false;
    int mode = 0;
    
    // const vector2 center = { width / 2, height / 2 };

    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_default_menu_event_source());

    al_start_timer(timer);

    add_quad(0, 0, 0, 0, 0, 
        0, 0, 3, 0, 0,
        3, 0, 0, 0, 0,
        al_map_rgb(255, 0, 0), al_map_rgb(0, 0, 255));

    while (unfinished) {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(display);
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            unfinished = false;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                mode = (mode + 1) % 4;
                printf("mode changed: %d\n",mode);
            } else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                al_set_audio_stream_playing(stream, !al_get_audio_stream_playing(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                al_set_audio_stream_gain(stream, al_get_audio_stream_gain(stream)+0.1);
                printf("new gain: %f\n", al_get_audio_stream_gain(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                al_set_audio_stream_gain(stream, al_get_audio_stream_gain(stream)-0.1);
                printf("new gain: %f\n", al_get_audio_stream_gain(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_O) {
                al_set_audio_stream_pan(stream, al_get_audio_stream_pan(stream)-0.1 < -1 ? -1 : al_get_audio_stream_pan(stream)-0.1);
                printf("new pan: %f\n", al_get_audio_stream_pan(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_P) {
                al_set_audio_stream_pan(stream, al_get_audio_stream_pan(stream)+0.1 > 1 ? 1 : al_get_audio_stream_pan(stream)+0.1);
                printf("new pan: %f\n", al_get_audio_stream_pan(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_A) {
                cam.pos.x -= 1;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_D) {
                cam.pos.x += 1;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_W) {
                cam.pos.z -= 1;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_S) {
                cam.pos.z += 1;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_Q) {
                cam.pos.z -= 1;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_E) {
                cam.pos.z += 1;
            }
            break;
        case ALLEGRO_EVENT_TIMER:
            for (int i = 0; i < SAMPLES; i++) {
                particles_ptr = (particles_ptr + 1) % PARTICLE_LIMIT;

                // particles[particles_ptr] = particle_create_params(vector2_new((i*((double)width / SAMPLES)), (center.y / 2.0) + (waveform[i] * 250)), vector2_new(0,0), 
                // vector2_new(0,0), al_map_rgb(0, 255, 0), 2);

                waveform_visual_buffer[i] = (ALLEGRO_VERTEX){ .x = (i*((double)width / SAMPLES)), .y = waveform[i] * 250, .z = 0, .color = al_map_rgb(0, 255, 0)};
            }

            for (int i = 0; i < PARTICLE_LIMIT; i++) {
                particle_update(&particles[i]);
            }
            redraw = true;
            break;
        case ALLEGRO_EVENT_MENU_CLICK:
            if (event.user.data1 == FILE_EXIT_ID) {
                unfinished = false;
            } else if (event.user.data1 == FILE_OPEN_ID) {
                filepath = choose_audio_file();
                if (filepath) {
                    fp = fopen("recent.txt", "w");
                    if (fp) {
                        fprintf(fp, "%s", filepath);
                        fclose(fp);
                    }
                    al_destroy_audio_stream(stream);
                    stream = al_load_audio_stream(filepath, 4, 4096);
                    if (stream) al_attach_audio_stream_to_mixer(stream, mixer);
                    free(filepath);
                }
            }
            
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.button == 1) {
                printf("mouse down @ %d %d!\n", event.mouse.x, event.mouse.y);
                mousedown = true;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (event.mouse.button == 1) {
                mousedown = false;
            }
            break;
        default:
            break;
        }

        if (event.type == ALLEGRO_EVENT_TIMER && mousedown) {
            al_get_mouse_state(&mouse_state);
            for (int i = 0; i < 5; i++) {
                particles[particles_ptr] = particle_create_params( (vector2){ mouse_state.x, mouse_state.y }, (vector2){ (rand() % 9) - 4, (rand() % 9) - 4 }, 
                    (vector2){ 0, 0.3 }, al_map_rgb(rand() % 255, rand() % 255, rand() % 255), 300);
                particles_ptr = (particles_ptr + 1) % PARTICLE_LIMIT;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            draw_scene();

            frames_done++;
        }

        double game_time = al_get_time();
        if (game_time - old_time >= 0.2) {
            fps = frames_done / (game_time - old_time);

            frames_done = 0;
            old_time = game_time;
        }
    }

    al_destroy_audio_stream(stream);
    al_destroy_mixer(mixer);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
}

int main(int argc, char **argv) {
    FILE* fp;

    fp = fopen("recent.txt", "r");
    if (fp) {
        filename = malloc(sizeof(char)*300);
        fgets(filename, 300, fp);
        fclose(fp);
    } else {
        filename = "./bitterend.ogg";
    }

    do_inits();

    run_main_loop();

    return 0;
}