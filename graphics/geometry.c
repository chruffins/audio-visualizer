#include "geometry.h"

void ch_model_init(ch_model* model, ALLEGRO_VERTEX* init_vertices, int* index_buffer, unsigned int num_vertices, unsigned int num_indices, 
    ALLEGRO_BITMAP* texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_type, ALLEGRO_PRIM_TYPE prim_type) {
    model->vertex_data = init_vertices;
    model->buffer_type = buffer_type;
    model->prim_type = prim_type;
    model->num_vertices = num_vertices;
    model->num_indices = num_indices;
    model->texture = texture;
    model->vertex_buffer = al_create_vertex_buffer(NULL, init_vertices, num_vertices, buffer_type);
    model->index_buffer = al_create_index_buffer(4, index_buffer, num_indices, buffer_type);
}

void ch_model_draw(ch_model* model) {
    int result = al_draw_indexed_buffer(model->vertex_buffer, model->texture, model->index_buffer, 0, model->num_indices, model->prim_type);
    //printf("%d\n", result);
}

void ch_model_free(ch_model* model) {
    al_destroy_vertex_buffer(model->vertex_buffer);
    al_destroy_index_buffer(model->index_buffer);
}

void ch_model_init_cube(ch_model* model, double lwh, double x, double y, double z) {
    static ALLEGRO_BITMAP* bmp = NULL;
    if (!bmp) bmp = al_load_bitmap("assets/texture1.png");

    int cube_indices[] = {
        0, 1, 2, 2, 3, 0,   // Front face
        4, 5, 6, 6, 7, 4,   // Back face
        8, 9,10,10,11, 8,   // Left face
        12,13,14,14,15,12,  // Right face
        16,17,18,18,19,16,  // Top face
        20,21,22,22,23,20   // Bottom face
    };

    ALLEGRO_VERTEX cube_vertices[] = {
        // Front face
        { -1, -1,  1, 0, 0, al_map_rgb(255, 255, 255) },
        {  1, -1,  1, 1, 0, al_map_rgb(255, 255, 255) },
        {  1,  1,  1, 1, 1, al_map_rgb(255, 255, 255) },
        { -1,  1,  1, 0, 1, al_map_rgb(255, 255, 255) },
    
        // Back face
        { -1, -1, -1, 1, 0, al_map_rgb(255, 255, 255) },
        {  1, -1, -1, 0, 0, al_map_rgb(255, 255, 255) },
        {  1,  1, -1, 0, 1, al_map_rgb(255, 255, 255) },
        { -1,  1, -1, 1, 1, al_map_rgb(255, 255, 255) },
    
        // Left face
        { -1, -1, -1, 0, 0, al_map_rgb(255, 255, 255) },
        { -1, -1,  1, 1, 0, al_map_rgb(255, 255, 255) },
        { -1,  1,  1, 1, 1, al_map_rgb(255, 255, 255) },
        { -1,  1, -1, 0, 1, al_map_rgb(255, 255, 255) },
    
        // Right face
        {  1, -1, -1, 1, 0, al_map_rgb(255, 255, 255) },
        {  1, -1,  1, 0, 0, al_map_rgb(255, 255, 255) },
        {  1,  1,  1, 0, 1, al_map_rgb(255, 255, 255) },
        {  1,  1, -1, 1, 1, al_map_rgb(255, 255, 255) },
    
        // Top face
        { -1,  1, -1, 0, 0, al_map_rgb(255, 255, 255) },
        {  1,  1, -1, 1, 0, al_map_rgb(255, 255, 255) },
        {  1,  1,  1, 1, 1, al_map_rgb(255, 255, 255) },
        { -1,  1,  1, 0, 1, al_map_rgb(255, 255, 255) },
    
        // Bottom face
        { -1, -1, -1, 0, 0, al_map_rgb(255, 255, 255) },
        {  1, -1, -1, 1, 0, al_map_rgb(255, 255, 255) },
        {  1, -1,  1, 1, 1, al_map_rgb(255, 255, 255) },
        { -1, -1,  1, 0, 1, al_map_rgb(255, 255, 255) },
    };

    ALLEGRO_VERTEX* vertices = malloc(sizeof(ALLEGRO_VERTEX)*24);

    int* indices = malloc(sizeof(int)*36);
    for (int i = 0; i < 36; i++) {
        indices[i] = cube_indices[i];
    }

    for (int i = 0; i < 24; i++) {
        vertices[i] = cube_vertices[i];

        vertices[i].x = (vertices[i].x * lwh / 2) + x;
        vertices[i].y = (vertices[i].y * lwh / 2) + y;
        vertices[i].z = (vertices[i].z * lwh / 2) + z;
        vertices[i].u *= 424;
        vertices[i].v *= 424;
    }

    ch_model_init(model, vertices, indices, 24, 36, bmp, ALLEGRO_PRIM_BUFFER_DYNAMIC, ALLEGRO_PRIM_TRIANGLE_LIST);
}