#pragma once
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_color.h>
#include<stdlib.h>
#include<stdio.h>

typedef struct ch_model ch_model;

struct ch_model {
    ALLEGRO_VERTEX* vertex_data;
    ALLEGRO_VERTEX_BUFFER* vertex_buffer;
    ALLEGRO_INDEX_BUFFER* index_buffer;
    ALLEGRO_BITMAP* texture;
    unsigned int num_vertices;
    unsigned int num_indices;
    ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags;
    ALLEGRO_PRIM_TYPE prim_type;
};

ch_model ch_model_create(ALLEGRO_VERTEX* vertices, unsigned int n_vertices, int* indices, unsigned int n_indices, 
    ALLEGRO_BITMAP* texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type);
void ch_model_init(ch_model* model, ALLEGRO_VERTEX* init_vertices, int* index_buffer, unsigned int num_vertices, unsigned int num_indices, 
    ALLEGRO_BITMAP* texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type);
void ch_model_draw(ch_model* model);
void ch_model_free(ch_model* model); // free resources

void ch_model_init_cube(ch_model* model, double lwh, double x, double y, double z);

ch_model ch_model_load_obj(char* obj_filename, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags);