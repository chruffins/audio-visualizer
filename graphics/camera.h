#pragma once
#include<allegro5/allegro.h>

#include "vector.h"
#include "ray.h"

/**
 * @struct camera
 * @brief Represents a 3D camera with position and orientation axes.
 */
struct camera {
    vector3 pos;   /**< Camera position in 3D space */
    vector3 x;     /**< X-axis (right/left direction) */
    vector3 y;     /**< Y-axis (up/down direction) */
    vector3 z;     /**< Z-axis (forward/backward direction) */
    double fov;    /**< Field of view in degrees */
    ALLEGRO_TRANSFORM transform;    /**< Internal storage of the camera matrix */
};

/**
 * @typedef camera
 * Alias for struct camera.
 */
typedef struct camera camera;

/**
 * @brief Sets up the 3D projection using the camera parameters.
 * @param cam Pointer to camera
 */
void setup_3d_projection(camera* cam);

/**
 * @brief Returns the normalized forward vector of the camera (y=0).
 * @param cam Pointer to camera
 * @return Normalized forward vector
 */
inline vector3 camera_forward_vector(camera* cam) {
    vector3 forward = cam->z;
    forward.y = 0;
    return vector3_norm(forward);
}

/**
 * @brief Returns the normalized right vector of the camera (y=0).
 * @param cam Pointer to camera
 * @return Normalized right vector
 */
inline vector3 camera_right_vector(camera* cam) {
    vector3 right = cam->x;
    right.y = 0;
    return vector3_norm(right);
}

/**
 * @brief Builds and applies the camera transform for rendering.
 * @param cam Pointer to camera
 */
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

/**
 * @brief Build and use a translation-invariant camera transform for skybox.
 * @param cam Pointer to camera
 */
inline ALLEGRO_TRANSFORM get_skybox_camera_transform(camera* cam) {
    ALLEGRO_TRANSFORM t;
    al_build_camera_transform(&t, 
        0, 0, 0,
        -cam->z.x, -cam->z.y, -cam->z.z,
        cam->y.x, cam->y.y, cam->y.z);
    return t;
}

/**
 * @brief Builds the camera transform for rendering.
 * @param cam Pointer to camera
 */
inline ALLEGRO_TRANSFORM update_camera_transform(camera* cam) {
    ALLEGRO_TRANSFORM t;
    al_build_camera_transform(&t, 
        cam->pos.x, cam->pos.y, cam->pos.z,
        cam->pos.x - cam->z.x, 
        cam->pos.y - cam->z.y, 
        cam->pos.z - cam->z.z,
        cam->y.x, cam->y.y, cam->y.z);

    cam->transform = t;
    return t;
}

/**
 * @brief Calculates the pitch (vertical angle) of the camera.
 * @param c Pointer to camera
 * @return Pitch angle in radians
 */
inline double camera_get_pitch(camera *c)
{
   vector3 f = camera_forward_vector(c);
   return asin(vector3_dot(f, c->y));
}

/**
 * @brief Calculates the yaw (compass direction) of the camera.
 * @param c Pointer to camera
 * @return Yaw angle in radians
 */
inline double camera_get_yaw(camera *c)
{
   return atan2(c->z.x, c->z.z);
}

/**
 * @brief Calculates the roll (tilt) of the camera.
 * @param c Pointer to camera
 * @return Roll angle in radians
 */
inline double camera_get_roll(camera *c)
{
   vector3 r = camera_right_vector(c);
   return asin(vector3_dot(r, c->y));
}

/**
 * @brief Sets the camera position.
 * @param cam Pointer to camera
 * @param pos New position vector
 */
void camera_set_position(camera* cam, vector3 pos);

/**
 * @brief Moves the camera along its axes.
 * @param cam Pointer to camera
 * @param right Amount to move right
 * @param up Amount to move up
 * @param forward Amount to move forward
 */
void camera_move(camera* cam, double right, double up, double forward);

/**
 * @brief Rotates the camera around a given axis.
 * @param c Pointer to camera
 * @param axis Axis to rotate around
 * @param radians Angle in radians
 */
void camera_rotate_around_axis(camera *c, vector3 axis, double radians);

/**
 * @brief Converts a 2D screen coordinate (mouse click) to a 3D ray in world space.
 * @param cam Pointer to camera
 * @param mouse_x Mouse X coordinate (screen space)
 * @param mouse_y Mouse Y coordinate (screen space)
 * @param screen_width Width of the screen in pixels
 * @param screen_height Height of the screen in pixels
 * @param out_ray Output: resulting ray in world space
 */
void camera_screen_point_to_ray(const camera* cam, double mouse_x, double mouse_y, double screen_width, double screen_height, ch_ray* out_ray);