#include "geometry.h"
#include <math.h>

struct tri_helper { // tri_helper doesn't own of its pointers
    ch_obj* obj;
    ALLEGRO_VERTEX* vertices;
    int* indices;
    int face_vertex_start; // this and vertex_start should be equal at the beginning and end of the loop
    int vertex_start;
    int index_start;
};

static ch_model ch_model_from_ch_obj(ch_obj* obj, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags);
static void add_triangle_to_indices(int vi1, int vi2, int vi3, void* helper);
static void triangulate_polygon(CH_VERTEX* vertices, int n_vertices, void (*emit_triangle)(int vi1, int vi2, int vi3, void* user_data), void* userdata);
static void fan_triangulation(CH_VERTEX* vertices, int n_vertices, void (*emit_triangle)(int vi1, int vi2, int vi3, void* userdata), void* userdata);

ch_model ch_model_create(ALLEGRO_VERTEX *vertices, unsigned int n_vertices, int *indices, unsigned int n_indices, 
    ALLEGRO_BITMAP *texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type) {
    ch_model model;
    model.vertex_data = vertices;
    model.num_vertices = n_vertices;
    model.num_indices = n_indices;
    model.texture = texture;
    model.buffer_flags = buffer_flags;
    model.prim_type = prim_type;
    model.position = (vector3) {0};
    model.rotation = (vector3) {0};
    model.scale = vector3_new(1, 1, 1);
    ch_model_recalculate_transform(&model);

    if (texture) {
        // need to go through all of the vertices and set uv properly

    }
    printf("creating buffers now\n");
    model.vertex_buffer = al_create_vertex_buffer(NULL, vertices, n_vertices, buffer_flags);
    model.index_buffer = al_create_index_buffer(sizeof(int), indices, n_indices, buffer_flags);
    printf("made a model\n");
    return model;
}

void ch_model_init(ch_model *model, ALLEGRO_VERTEX *init_vertices, int *index_buffer, unsigned int num_vertices, unsigned int num_indices,
                   ALLEGRO_BITMAP *texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type)
{
    model->vertex_data = init_vertices;
    model->buffer_flags = buffer_flags;
    model->prim_type = prim_type;
    model->num_vertices = num_vertices;
    model->num_indices = num_indices;
    model->texture = texture;
    model->vertex_buffer = al_create_vertex_buffer(NULL, init_vertices, num_vertices, buffer_flags);
    model->index_buffer = al_create_index_buffer(4, index_buffer, num_indices, buffer_flags);
    model->position = vector3_new(0, 0, 0);
    model->rotation = vector3_new(0, 0, 0);
    model->scale = vector3_new(1, 1, 1);
    model->transform_dirty = 1;
}

void ch_model_draw(ch_model* model) {
    al_draw_indexed_buffer(model->vertex_buffer, model->texture, model->index_buffer, 0, model->num_indices, model->prim_type);
}

void ch_model_free(ch_model* model) {
    al_destroy_vertex_buffer(model->vertex_buffer);
    al_destroy_index_buffer(model->index_buffer);
}

void ch_model_init_cube(ch_model* model, ALLEGRO_BITMAP* texture, double lwh, double x, double y, double z) {
    if (!texture) texture = al_load_bitmap("assets/texture1.png");

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

        vertices[i].x = (vertices[i].x / 2);
        vertices[i].y = (vertices[i].y / 2);
        vertices[i].z = (vertices[i].z / 2);
        vertices[i].u *= al_get_bitmap_width(texture);
        vertices[i].v *= al_get_bitmap_height(texture);
    }

    ch_model_init(model, vertices, indices, 24, 36, texture, ALLEGRO_PRIM_BUFFER_DYNAMIC, ALLEGRO_PRIM_TRIANGLE_LIST);
    model->position = vector3_new(x, y, z);
    model->scale = vector3_new(lwh, lwh, lwh);
}

ch_model ch_model_load(char* obj_filename, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags) {
    return ch_model_from_ch_obj(ch_obj_load(obj_filename), buffer_flags);
}

// this will consume and destroy the obj
ch_model ch_model_from_ch_obj(ch_obj* obj, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags) {
    if (obj == NULL) {
        // TODO: address this lol
        printf("you passed in a bad model\n");
        exit(1);
    }
    int vertex_count;

    ALLEGRO_VERTEX* vertices = calloc(sizeof(ALLEGRO_VERTEX), obj->face_vertex_sum);
    int* indices = calloc(sizeof(int), obj->triangle_count * 3);
    printf("we allocated %d indices\n", obj->triangle_count * 3);
    struct tri_helper helper = {.obj = obj, .vertices = vertices, .indices = indices, .face_vertex_start = 0, .vertex_start = 0};
    ch_model model;

    for (int i = 0; i < obj->face_count; i++) {
        vertex_count = obj->face_vertex_counts[i];
        // pull out the VERTICES!!!
        for (int j = 0; j < vertex_count; j++) {
            ch_obj_face_vertex fv = obj->face_vertices[helper.face_vertex_start + j];
            ch_obj_vertex v = obj->vertices[fv.v];
            ch_obj_vt vt = obj->vts[fv.vt];

            vertices[helper.vertex_start] = (ALLEGRO_VERTEX){ .x = v.x, .y = v.y, .z = v.z, .u = vt.u, .v = vt.v, .color = al_map_rgb_f(v.r - 0.3, v.g - 0.5, v.b - 0.5)};
            helper.vertex_start++;
        }
        // triangulation gives us INDICES!!
        triangulate_polygon(vertices + helper.vertex_start - vertex_count, vertex_count, add_triangle_to_indices, &helper);
        helper.face_vertex_start += vertex_count;
    }
    printf("we now have %d vertices and %d indices\n", helper.vertex_start, helper.index_start);
    model = ch_model_create(vertices, obj->face_vertex_sum, indices, obj->triangle_count * 3, NULL, buffer_flags, ALLEGRO_PRIM_TRIANGLE_LIST);

    ch_obj_destroy(obj);
    return model;
}

void add_triangle_to_indices(int vi1, int vi2, int vi3, void* vhelper) {
    // so the ints are indices... we need to offset by index in helper to get the face vertices
    //static int calls = 0;
    struct tri_helper* helper = (struct tri_helper*)vhelper;
    //printf("%d %d %d\n", vi1, vi2, vi3);

    helper->indices[helper->index_start] = vi1 + helper->face_vertex_start;
    helper->indices[helper->index_start + 1] = vi2 + helper->face_vertex_start;
    helper->indices[helper->index_start + 2] = vi3 + helper->face_vertex_start;
    helper->index_start += 3;
}

void triangulate_polygon(CH_VERTEX *vertices, int n_vertices, void (*emit_triangle)(int vi1, int vi2, int vi3, void *userdata), void *userdata) {
    fan_triangulation(vertices, n_vertices, emit_triangle, userdata);
}

void fan_triangulation(CH_VERTEX* vertices, int n_vertices, void (*emit_triangle)(int vi1, int vi2, int vi3, void* userdata), void* userdata) {
    if (n_vertices < 3) {
        printf("%s:%d why\n", __FILE__, __LINE__);
        return;
    }

    for (int i = 1; i < n_vertices - 1; i++) {
        emit_triangle(0, i, i + 1, userdata);
    }
}

void ch_model_move(ch_model* model, vector3 offset) {
    model->position.x += offset.x;
    model->position.y += offset.y;
    model->position.z += offset.z;
    model->transform_dirty = 1;
}

void ch_model_set_position(ch_model* model, vector3 position) {
    model->position = position;
    model->transform_dirty = 1;
}

void ch_model_rotate(ch_model* model, vector3 angles) {
    model->rotation.x += angles.x;
    model->rotation.y += angles.y;
    model->rotation.z += angles.z;
    model->transform_dirty = 1;
}

void ch_model_set_rotation(ch_model* model, vector3 rotation) {
    model->rotation = rotation;
    model->transform_dirty = 1;
}

void ch_model_recalculate_transform(ch_model* model) {
    if (!model->transform_dirty) return;

    al_identity_transform(&model->transform);

    // Apply scaling
    al_scale_transform_3d(&model->transform, model->scale.x, model->scale.y, model->scale.z);

    // Apply rotations (order: Z, Y, X)
    al_rotate_transform_3d(&model->transform, 1, 0, 0, model->rotation.x);
    al_rotate_transform_3d(&model->transform, 0, 1, 0, model->rotation.y);
    al_rotate_transform_3d(&model->transform, 0, 0, 1, model->rotation.z);

    // Apply translation
    al_translate_transform_3d(&model->transform, model->position.x, model->position.y, model->position.z);

    model->transform_dirty = 0;
}