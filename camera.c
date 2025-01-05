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