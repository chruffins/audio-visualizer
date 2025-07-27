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
#include "../graphics/skybox.h"

#include "inputstate.h"

typedef struct {
    // Window/display properties
    char* filename;
    const int width;
    const int height;
    ALLEGRO_DISPLAY* display;

    // Graphics/models/textures
    ALLEGRO_BITMAP* test_texture;
    ALLEGRO_BITMAP* info_cube_texture;
    ALLEGRO_TRANSFORM identity_transform;
    ALLEGRO_FONT* def;
    ALLEGRO_VERTEX triangles[3*1000];
    int triangles_n;
    ch_vis_buffer *vb;
    camera cam;

    // Geometry/particles
    ch_skybox skybox;
    ch_model_vec models;
    particle* particles;
    int particles_ptr;

    // audio
    ch_song* song;
    ALLEGRO_VOICE* voice;
    ALLEGRO_MIXER* mixer;
    ALLEGRO_AUDIO_STREAM* stream;

    // music data in-memory
    ch_song_vec songs;

    // Timing/performance
    double fps;
    uint64_t frames_done;
    double old_time;

    // Input/database
    input_state input;
    sqlite3* db;
} ch_app_state;

void init_app_state();
ch_app_state* get_app_state();