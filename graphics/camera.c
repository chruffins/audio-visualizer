#include "camera.h"

void setup_3d_projection(camera* cam) {
   ALLEGRO_TRANSFORM projection;
   ALLEGRO_DISPLAY *display = al_get_current_display();
   double dw = al_get_display_width(display);
   double dh = al_get_display_height(display);
   double f;
   al_identity_transform(&projection);
   al_translate_transform_3d(&projection, 0, 0, -1);
   f = tan(cam->fov / 2);
   al_perspective_transform(&projection, -1 * dw / dh * f, f,
      1,
      f * dw / dh, -f, 1000);
   al_use_projection_transform(&projection);
}

void camera_set_position(camera *cam, vector3 pos) {
   cam->pos = pos;
}

void camera_move(camera *cam, double right, double up, double forward) {
   vector3_iadd(&cam->pos, vector3_mul(cam->x, right));
   vector3_iadd(&cam->pos, vector3_mul(cam->y, up));
   vector3_iadd(&cam->pos, vector3_mul(cam->z, forward));
}

void camera_rotate_around_axis(camera *c, vector3 axis, double radians)
{
   ALLEGRO_TRANSFORM t;
   float x, y, z;

   al_identity_transform(&t);
   al_rotate_transform_3d(&t, axis.x, axis.y, axis.z, radians);

   x = c->y.x;
   y = c->y.y;
   z = c->y.z;

   al_transform_coordinates_3d(&t, &x, &y, &z);

   c->y.x = x;
   c->y.y = y;
   c->y.z = z;

   x = c->z.x;
   y = c->z.y;
   z = c->z.z;

   al_transform_coordinates_3d(&t, &x, &y, &z);

   c->z.x = x;
   c->z.y = y;
   c->z.z = z;

   /* Make sure the axes remain orthogonal to each other. */
   c->z = vector3_norm(c->z);
   c->x = vector3_cross(c->y, c->z);
   c->x = vector3_norm(c->x);
   c->y = vector3_cross(c->z, c->x);
}

void camera_screen_point_to_ray(const camera* cam, double mouse_x, double mouse_y, double screen_width, double screen_height, ch_ray* out_ray) {
    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    double ndc_x = (2.0 * mouse_x) / screen_width - 1.0;
    double ndc_y = 1.0 - (2.0 * mouse_y) / screen_height; // y is usually inverted

    // Calculate aspect ratio and FOV in radians
    double aspect = screen_width / screen_height;
    double fov_rad = cam->fov * (M_PI / 180.0);

    // Calculate the size of the image plane at z = -1
    double px = ndc_x * aspect * tan(fov_rad / 2.0);
    double py = ndc_y * tan(fov_rad / 2.0);

    // Ray direction in camera space
    vector3 ray_dir = vector3_norm(
      vector3_add(
         vector3_add(
            vector3_mul(cam->x, px),
            vector3_mul(cam->y, py)
         ),
         vector3_mul(cam->z, -1.0)
      )
   );

   ch_ray_init(out_ray, &cam->pos, &ray_dir);
}