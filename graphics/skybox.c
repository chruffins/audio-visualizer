#include "skybox.h"

static const ALLEGRO_COLOR white = (ALLEGRO_COLOR) { .r = 1, .g = 1, .b = 1, };

static const ALLEGRO_VERTEX skybox_vertices[] = {
// Front face (mirrored horizontally and vertically)
{ -1, -1,  1, 1, 1, white },
{  1, -1,  1, 0, 1, white },
{  1,  1,  1, 0, 0, white },
{ -1,  1,  1, 1, 0, white },

// Back face
{ -1, -1, -1, 0, 1, white },
{  1, -1, -1, 1, 1, white },
{  1,  1, -1, 1, 0, white },
{ -1,  1, -1, 0, 0, white },

// Left face
{ -1, -1, -1, 1, 1, white },
{ -1, -1,  1, 0, 1, white },
{ -1,  1,  1, 0, 0, white },
{ -1,  1, -1, 1, 0, white },

// Right face
{  1, -1, -1, 0, 1, white },
{  1, -1,  1, 1, 1, white },
{  1,  1,  1, 1, 0, white },
{  1,  1, -1, 0, 0, white },

// Top face
{ -1,  1, -1, 1, 1, white },
{  1,  1, -1, 0, 1, white },
{  1,  1,  1, 0, 0, white },
{ -1,  1,  1, 1, 0, white },

// Bottom face
{ -1, -1, -1, 1, 1, white },
{  1, -1, -1, 0, 1, white },
{  1, -1,  1, 0, 0, white },
{ -1, -1,  1, 1, 0, white },
};

ch_skybox ch_skybox_load(const char *front, const char *right, const char *back, const char *left, const char *down, const char *up) {
    ch_skybox skybox;
    skybox.faces[SKYBOX_FRONT] = al_load_bitmap(front);
    skybox.faces[SKYBOX_RIGHT] = al_load_bitmap(left);
    skybox.faces[SKYBOX_BACK] = al_load_bitmap(back);
    skybox.faces[SKYBOX_LEFT] = al_load_bitmap(right);
    skybox.faces[SKYBOX_UP] = al_load_bitmap(up);
    skybox.faces[SKYBOX_DOWN] = al_load_bitmap(down);

    ch_model_init_cube(&skybox.box, NULL, 100, 0, 0, 0);

    int width = al_get_bitmap_width(skybox.faces[SKYBOX_FRONT]);
    int height = al_get_bitmap_height(skybox.faces[SKYBOX_FRONT]);

    // now we have to fix the UVs....
    for (int i = 0; i < 24; i++) {
        skybox.box.vertex_data[i] = skybox_vertices[i];
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
