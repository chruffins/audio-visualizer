#include<stdbool.h>
#include<stdio.h>
#include<math.h>
#include<complex.h>

#include<fftw3.h>

#include<allegro5/allegro.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_primitives.h>

static const char* filename = "hummingbird.ogg";
static volatile float rms_l = 0.0;
static volatile float rms_r = 0.0;
static fftwf_complex freq_bins[1024 / 2 + 1];

void must_init(int test, const char* name) {
    if (test) return;

    printf("%s failed to init!\n", name);
    exit(EXIT_FAILURE);
}

void do_inits() {
    must_init(al_init(), "allegro");
    must_init(al_init_acodec_addon(), "allegro acodec");
    must_init(al_init_primitives_addon(), "allegro primitives");
    must_init(al_install_audio(), "allegro audio");

    must_init(al_install_mouse(), "mouse");
    must_init(al_install_keyboard(), "keyboard");

    al_set_new_display_flags(ALLEGRO_WINDOWED);
}

void update_meter(void* buf, unsigned int samples, void* data) {
    static fftwf_plan plan;
    if (!plan) plan = fftwf_plan_dft_r2c_1d(samples, buf, freq_bins, FFTW_ESTIMATE);
    float *fbuf = (float *)buf;
    float sum_l = 0.0;
    float sum_r = 0.0;
    unsigned int i;

    fftwf_execute(plan);

    for (i = samples; i > 0; i--) {
        sum_l += fbuf[0] * fbuf[0];
        sum_r += fbuf[1] * fbuf[1];
        fbuf += 2;
    }

    rms_l = sqrt(sum_l / samples);
    rms_r = sqrt(sum_r / samples);
}

void run_main_loop() {
    ALLEGRO_DISPLAY* display = al_create_display(640, 480);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT event;

    ALLEGRO_VOICE* voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    ALLEGRO_MIXER* mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

    al_attach_mixer_to_voice(mixer, voice);
    al_reserve_samples(8);

    ALLEGRO_AUDIO_STREAM* stream = al_load_audio_stream(filename, 4, 2048);
    if (!stream) {
        printf("Failed to create stream...\n");
    }
    al_attach_audio_stream_to_mixer(stream, mixer);

    bool redraw;
    bool finished = true;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_set_mixer_postprocess_callback(mixer, update_meter, NULL);

    al_start_timer(timer);

    float db_l, db_r;
    float db;

    while (finished) {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            finished = false;
            break;
        case ALLEGRO_EVENT_TIMER:
            redraw = true;
            break;
        default:
            break;
        }

        if (redraw) {
            redraw = false;

            db_l = 20 * log10(rms_l / 20e-6);
            db_r = 20 * log10(rms_r / 20e-6);
            db = (db_l + db_r) / 2.0;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_filled_rectangle(40, 0, 80, db_l, al_map_rgb(80, 0, 255));
            al_draw_filled_rectangle(90, 0, 130, db_r, al_map_rgb(255, 0, 80));
            al_draw_filled_rectangle(140, 0, 180, db, al_map_rgb(80, 255, 80));
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