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

inline vector3 camera_forward_vector(camera* cam) {
    vector3 forward = cam->z;
    forward.y = 0;
    return vector3_norm(forward);
}

inline vector3 camera_right_vector(camera* cam) {
    vector3 right = cam->x;
    right.y = 0;
    return vector3_norm(right);
}

inline void build_and_use_camera_transform(camera* cam) {
    ALLEGRO_TRANSFORM t;
    al_build_camera_transform(&t, 
        cam->pos.x, cam->pos.y, cam->pos.z,
        cam->pos.x - cam->z.x, 
        cam->pos.y - cam->z.y, 
        cam->pos.z - cam->z.z,
        cam->y.x, cam->y.y, cam->y.z);
    al_use_transform(&t);
}

inline double camera_get_pitch(camera *c)
{
   vector3 f = camera_forward_vector(c);
   return asin(vector3_dot(f, c->y));
}

/* Calculate the yaw of the camera. This is basically the compass direction.
 */
inline double camera_get_yaw(camera *c)
{
   return atan2(c->z.x, c->z.z);
}

/* Calculate the roll of the camera. This is the angle between the x axis
 * vector and its project on the y = 0 plane.
 */
inline double camera_get_roll(camera *c)
{
   vector3 r = camera_right_vector(c);
   return asin(vector3_dot(r, c->y));
}

void camera_set_position(camera* cam, vector3 pos);
void camera_move(camera* cam, double right, double up, double forward);
void camera_rotate_around_axis(camera *c, vector3 axis, double radians);