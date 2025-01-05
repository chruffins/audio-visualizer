#pragma once
#include<allegro5/allegro.h>

#include "vector.h"

struct camera {
    vector3 pos;
    vector3 x; // x-axis -- right / left
    vector3 y; // y-axis -- up / down
    vector3 z; // z-axis -- north / south
    double fov;
};

typedef struct camera camera;

void setup_3d_projection(camera* cam);
inline void build_and_use_camera_transform(camera* cam) {
    ALLEGRO_TRANSFORM t;
    al_build_camera_transform(&t, 
      cam->pos.x, cam->pos.x, cam->pos.z,
      cam->pos.x - cam->z.x, cam->pos.y - cam->z.y, cam->pos.z - cam->z.z,
      cam->y.x, cam->y.y, cam->y.z);
    al_use_transform(&t);
}