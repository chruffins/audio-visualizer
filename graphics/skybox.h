#pragma once

#include <allegro5/allegro.h>
#include "geometry.h"

typedef struct ch_skybox ch_skybox;

enum CH_SKYBOX_FACES {
    SKYBOX_FRONT = 0,
    SKYBOX_BACK,
    SKYBOX_LEFT,
    SKYBOX_RIGHT,
    SKYBOX_UP,
    SKYBOX_DOWN
};

struct ch_skybox {
    ALLEGRO_BITMAP* faces[6];
    ch_model box;
};

ch_skybox ch_skybox_load(const char* front, const char* right, const char* back, 
    const char* left, const char* down, const char* up);

void ch_skybox_draw(ch_skybox *skybox);