#include "skybox.h"

ch_skybox ch_skybox_load(const char *front, const char *right, const char *back, const char *left, const char *down, const char *up) {
    ch_skybox skybox;
    skybox.faces[SKYBOX_FRONT] = al_load_bitmap(front);
    skybox.faces[SKYBOX_RIGHT] = al_load_bitmap(right);
    skybox.faces[SKYBOX_BACK] = al_load_bitmap(back);
    skybox.faces[SKYBOX_LEFT] = al_load_bitmap(left);
    skybox.faces[SKYBOX_UP] = al_load_bitmap(down);
    skybox.faces[SKYBOX_DOWN] = al_load_bitmap(up);

    ch_model_init_cube(&skybox.box, NULL, 100, 0, 0, 0);

    int width = al_get_bitmap_width(skybox.faces[SKYBOX_FRONT]);
    int height = al_get_bitmap_height(skybox.faces[SKYBOX_FRONT]);

    // now we have to fix the UVs....
    for (int i = 0; i < 24; i++) {
        skybox.box.vertex_data[i].u *= width;
        skybox.box.vertex_data[i].v *= height;
    }

    al_destroy_vertex_buffer(skybox.box.vertex_buffer);
    skybox.box.vertex_buffer = al_create_vertex_buffer(NULL, skybox.box.vertex_data, skybox.box.num_vertices, skybox.box.buffer_flags);

    ch_model_recalculate_transform(&skybox.box);

    return skybox;
}

void ch_skybox_draw(ch_skybox *skybox) {
    // it's actually front back left right top bottom lol
    for (int i = 0; i < 6; i++) {
        al_draw_indexed_buffer(skybox->box.vertex_buffer, skybox->faces[i], skybox->box.index_buffer, 6*i, (6*i)+6, skybox->box.prim_type);
    }
}
