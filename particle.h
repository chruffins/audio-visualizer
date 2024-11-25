#pragma once

#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>

#include "vector2.h"
#include "stdint.h"

struct particle {
    vector2 pos;
    vector2 vel;
    vector2 acc;
    ALLEGRO_COLOR color;
    int32_t lifetime; // these are measured in updates (uhh)
    bool enabled;
};

typedef struct particle particle;

particle particle_create();
particle particle_create_params(vector2 pos, vector2 vec, vector2 acc, ALLEGRO_COLOR color, int32_t lifetime);

void particle_update(particle*);
void particle_draw(particle*);