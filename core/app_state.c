#include "app_state.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static ch_app_state app_state = (ch_app_state) {
    .filename = NULL,
    .width = 1920,
    .height = 1080,
    .display = NULL,
    .vb = NULL,
    .song = NULL,
    .test_texture = NULL,
    .info_cube_texture = NULL,
    .triangles_n = 0,
    .particles = NULL,
    .particles_ptr = 0,
    .fps = 0.0,
    .frames_done = 0,
    .old_time = 0.0,
    .def = NULL,
    .db = NULL,
};

ch_app_state* get_app_state() {
    return &app_state;
}

void init_app_state() {
    // Move Allegro display option/flag setup here
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MIPMAP);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_GTK_TOPLEVEL | ALLEGRO_PROGRAMMABLE_PIPELINE);

    app_state.vb = ch_vis_buffer_create();
    app_state.models = ch_model_vec_init();
    app_state.particles = malloc(sizeof(particle) * 8192);
    al_identity_transform(&app_state.identity_transform);

    app_state.cam.x.x = 1;
    app_state.cam.y.y = 1;
    app_state.cam.z = vector3_new(0, 0, 1);
    app_state.cam.pos = vector3_new(0, 0, 0);
    app_state.cam.fov = 60 * ALLEGRO_PI / 180;

    int rc = sqlite3_open("music.db", &app_state.db);
    if (rc) {
        fprintf(stderr, "can't open database: %s\n", sqlite3_errmsg(app_state.db));
    }
    rc = init_database(app_state.db);
    if (rc) {
        exit(1);
    }

    app_state.songs = get_songs(app_state.db);
    get_albums(app_state.db);
    get_artists(app_state.db);
    get_genres(app_state.db);

    app_state.voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    if (!app_state.voice) {
        fprintf(stderr, "Failed to create voice\n");
        exit(1);
    }

    app_state.mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    if (!app_state.mixer) {
        fprintf(stderr, "Failed to create mixer\n");
        al_destroy_voice(app_state.voice);
        exit(1);
    }
}