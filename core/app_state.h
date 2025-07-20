#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <sqlite3.h>

#include "../graphics/visualization.h"
#include "../graphics/camera.h"
#include "../graphics/geometry.h"
#include "../graphics/particle.h"
#include "../music/song.h"
#include "../music/database.h"
#include "../music/audio_parser.h"
#include "../music/play_queue.h"

#include "inputstate.h"

typedef struct {
    char* filename;
    const int width;
    const int height;
    ALLEGRO_DISPLAY* display;
    ch_vis_buffer *vb;
    ch_song* song;
    ch_model model;
    ch_model info_cube;
    ALLEGRO_BITMAP* test_texture;
    ALLEGRO_BITMAP* info_cube_texture;
    ALLEGRO_VERTEX triangles[3*1000];
    int triangles_n;
    camera cam;
    particle* particles;
    int particles_ptr;
    double fps;
    uint64_t frames_done;
    double old_time;
    ALLEGRO_TRANSFORM identity_transform;
    ALLEGRO_FONT* def;
    input_state input;
    sqlite3* db;
    ALLEGRO_VOICE* voice;
    ALLEGRO_MIXER* mixer;
    ALLEGRO_AUDIO_STREAM* stream;
} ch_app_state;

void init_app_state();
ch_app_state* get_app_state();