#include<stdbool.h>
#include<stdio.h>
#include<math.h>
#include<complex.h>
#include<memory.h>

#include<fftw3.h>
#include "menu.h"

#include<allegro5/allegro.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_native_dialog.h>

#define SAMPLES 8192
#define N SAMPLES / 2 + 1

static const char* filename = "hummingbird.ogg";
static float rms_l = 0.0;
static float rms_r = 0.0;
static fftwf_complex fft_output[N];
static float frequencies[N];
static float phases[N];
static float sample_buffer[SAMPLES];
//static volatile cbuffer sample_buffer;
static ALLEGRO_DISPLAY* display;
static ALLEGRO_VERTEX vertices[SAMPLES];
static fftwf_plan plan;
static const int width = 1920;
static const int height = 1080;

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

void update_meter(void* buf, unsigned int samples, void* data) {
    float *fbuf = (float *)buf;
    float sum_l = 0.0;
    float sum_r = 0.0;
    unsigned int i;
    //memmove(sample_buffer + (samples), sample_buffer, (SAMPLES-samples));
    memmove(sample_buffer, sample_buffer + samples, (SAMPLES-samples)*sizeof(float));
    memcpy(sample_buffer + (SAMPLES-samples), fbuf, samples*sizeof(float));

    fftwf_execute(plan);

    for (i = samples; i > 0; i--) {
        sum_l += fbuf[0] * fbuf[0];
        sum_r += fbuf[1] * fbuf[1];
        fbuf += 2;
    }

    for (int i = 0; i < N; i++) {
        float real = crealf(fft_output[i]);
        float imag = cimagf(fft_output[i]);
        frequencies[i] = sqrt(real * real + imag * imag);
        phases[i] = atan2(imag, real);
    }

    rms_l = sqrt(sum_l / samples);
    rms_r = sqrt(sum_r / samples);
}

void draw_frequency_bins() {
    float w = width / 300.0f;
    for (int i = 0; i < 50; i++) {
        float h = 2 + frequencies[100-i];
        al_draw_filled_rectangle(i*w, (height / 2) - h, i*w + w, (height / 2) + h, hsv_to_color(i % 300 / 300.0, 1, 1));
    }
    for (int i = 50; i < 300; i++) {
        float h = 2 + frequencies[i-50];
        al_draw_filled_rectangle(i*w, (height / 2) - h, i*w + w, (height / 2) + h, hsv_to_color(i % 300 / 300.0, 1, 1));
    }
}

void draw_spinning_oscilloscope() {
    static float angle = 0;
    static float hue = 0;

    for (int i = 0; i < 900; i++) {
        float x = (i-450);
        float y = (sample_buffer[i] * 100);

        vertices[i] = (ALLEGRO_VERTEX){ .x = (x * cosf(angle) - y * sinf(angle)) + (width / 4), .y = (x * sinf(angle) + y * cosf(angle)) + (height / 2), .color = hsv_to_color(hue, 1, 1) };
        vertices[i + 900] = (ALLEGRO_VERTEX){ .x = (x * cosf(angle) - y * sinf(angle)) + (3 * width / 4), .y = (x * sinf(angle) + y * cosf(angle)) + (height / 2), .color = hsv_to_color(hue, 1, 1) };
    }

    al_draw_prim(vertices, NULL, NULL, 0, 900, ALLEGRO_PRIM_POINT_LIST);
    al_draw_prim(vertices, NULL, NULL, 900, 1800, ALLEGRO_PRIM_POINT_LIST);
    angle += 0.02;
    hue += 0.001;
    
}

void draw_oscilloscope() {
    const ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    for (int i = 0; i < width; i++) {
        vertices[i] = (ALLEGRO_VERTEX){ .x = i, .y = (height / 2) + sample_buffer[(int)(i / (float)(width) * 1024)]*200, .z = 0, .color = green };
        //al_draw_pixel(width - i, 200+sample_buffer[(width - i)*2]*100, al_map_rgb(0, 255, 0));
    }
    al_draw_prim(vertices, NULL, NULL, 0, width, ALLEGRO_PRIM_POINT_LIST);
    //al_draw_line(1024, 500, 1024, 900, green, 1);
}

void draw_audio_levels() {
    float db_l, db_r, db;

    db_l = 20 * log10(rms_l / 20e-6);
    db_r = 20 * log10(rms_r / 20e-6);
    db = (db_l + db_r) / 2.0;

    al_draw_filled_rectangle(40, 0, 80, db_l*2, al_map_rgb(80, 0, 255));
    al_draw_filled_rectangle(90, 0, 130, db_r*2, al_map_rgb(255, 0, 80));
    al_draw_filled_rectangle(140, 0, 180, db*2, al_map_rgb(80, 255, 80));
}

void run_main_loop() {
    plan = fftwf_plan_dft_r2c_1d(SAMPLES, sample_buffer, fft_output, FFTW_MEASURE);

    display = al_create_display(width, height);

    ALLEGRO_MENU* menu = get_menu();
    // al_set_display_menu(display, menu);

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT event;

    ALLEGRO_VOICE* voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    ALLEGRO_MIXER* mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

    al_attach_mixer_to_voice(mixer, voice);
    al_reserve_samples(32);

    ALLEGRO_AUDIO_STREAM* stream = al_load_audio_stream(filename, 4, 4096);
    if (!stream) {
        printf("Failed to create stream...\n");
    }
    al_attach_audio_stream_to_mixer(stream, mixer);

    bool redraw;
    bool finished = true;
    int mode = 0;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_set_mixer_postprocess_callback(mixer, update_meter, NULL);

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
            redraw = true;
            break;
        default:
            break;
        }

        if (redraw) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            switch (mode)
            {
            case 0: 
                draw_frequency_bins();
                break;
            case 1:
                draw_oscilloscope();
                break;
            case 2:
                draw_spinning_oscilloscope();
                break;
            case 3:
                draw_audio_levels();
                break;
            default:
                break;
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