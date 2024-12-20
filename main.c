#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<complex.h>
#include<memory.h>

#include<fftw3.h>
#include "menu.h"
#include "particle.h"

#include<allegro5/allegro.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_native_dialog.h>

static const char* filename = "hummingbird.ogg";
static const int width = 960;
static const int height = 540;
static ALLEGRO_DISPLAY* display;

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
    must_init(al_install_audio(), "allegro audio");

    must_init(al_install_mouse(), "mouse");
    must_init(al_install_keyboard(), "keyboard");

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    // al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_GTK_TOPLEVEL | ALLEGRO_PROGRAMMABLE_PIPELINE);
}

ALLEGRO_COLOR hsv_to_color(float h, float s, float v) {
    int hi = (int)(h * 6) % 6;
    float f = h * 6 - hi;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (hi)
    {
    case 0: return al_map_rgb_f(v, t, p);
    case 1: return al_map_rgb_f(q, v, p);
    case 2: return al_map_rgb_f(p, v, t);
    case 3: return al_map_rgb_f(p, q, v);
    case 4: return al_map_rgb_f(t, p, v);
    case 5: return al_map_rgb_f(v, p, q);
    }

    return al_map_rgb(0,0,0);
}

void run_main_loop() {
    display = al_create_display(width, height);

    particle* particles = malloc(sizeof(particle)*5000);
    int particles_ptr = 0;

    ALLEGRO_MENU* menu = get_menu();
    al_set_display_menu(display, menu);

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT event;

    ALLEGRO_VOICE* voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    ALLEGRO_MIXER* mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

    ALLEGRO_MOUSE_STATE mouse_state;

    al_attach_mixer_to_voice(mixer, voice);
    al_reserve_samples(32);

    ALLEGRO_AUDIO_STREAM* stream = al_load_audio_stream(filename, 4, 4096);
    if (!stream) {
        printf("Failed to create stream...\n");
    }
    al_attach_audio_stream_to_mixer(stream, mixer);

    bool redraw;
    bool finished = true;
    bool mousedown = false;
    int mode = 0;

    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    while (finished) {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            finished = false;
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
            } else if (event.keyboard.keycode == ALLEGRO_KEY_Q) {
                al_set_audio_stream_pan(stream, al_get_audio_stream_pan(stream)-0.1 < -1 ? -1 : al_get_audio_stream_pan(stream)-0.1);
                printf("new pan: %f\n", al_get_audio_stream_pan(stream));
            } else if (event.keyboard.keycode == ALLEGRO_KEY_E) {
                al_set_audio_stream_pan(stream, al_get_audio_stream_pan(stream)+0.1 > 1 ? 1 : al_get_audio_stream_pan(stream)+0.1);
                printf("new pan: %f\n", al_get_audio_stream_pan(stream));
            }
            break;
        case ALLEGRO_EVENT_TIMER:
            for (int i = 0; i < 5000; i++) {
                particle_update(&particles[i]);
            }
            redraw = true;
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.button == 1) {
                printf("mouse down @ %d!\n", event.mouse.x);
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
                particles[particles_ptr] = particle_create_params( (vector2){ mouse_state.x, mouse_state.y }, (vector2){ (rand() % 4) - 4, (rand() % 4) - 4 }, 
                    (vector2){ 0, 0.5 }, al_map_rgb(rand() % 255, rand() % 255, rand() % 255), 300);
                particles_ptr = (particles_ptr + 1) % 5000;
            }
        }

        if (redraw) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            for (int i = 0; i < 5000; i++) {
                particle_draw(&particles[i]);
            }

            al_flip_display();
        }
    }

    al_destroy_audio_stream(stream);
    al_destroy_mixer(mixer);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        filename = argv[1];
        printf("Filename set: %s\n", filename);
    }

    do_inits();

    run_main_loop();

    return 0;
}