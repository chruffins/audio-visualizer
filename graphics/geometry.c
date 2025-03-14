#include "geometry.h"

static int get_vertex_index_count_from_obj(char* obj_filename, unsigned int *vcount, unsigned int *icount);
static int read_data_from_obj(char *obj_filename, ALLEGRO_VERTEX* vertices, unsigned int vcount, int* indices, unsigned int icount);

ch_model ch_model_create(ALLEGRO_VERTEX *vertices, unsigned int n_vertices, int *indices, unsigned int n_indices, 
    ALLEGRO_BITMAP *texture, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags, ALLEGRO_PRIM_TYPE prim_type) {
    ch_model model;
    model.vertex_data = vertices;
    model.num_vertices = n_vertices;
    model.num_indices = n_indices;
    model.texture = texture;
    model.buffer_flags = buffer_flags;
    model.prim_type = prim_type;

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
}

void ch_model_draw(ch_model* model) {
    int result = al_draw_indexed_buffer(model->vertex_buffer, model->texture, model->index_buffer, 0, model->num_indices, model->prim_type);
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

ch_model ch_model_load_obj(char* obj_filename, ALLEGRO_PRIM_BUFFER_FLAGS buffer_flags) {
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;

    ALLEGRO_VERTEX* vertices;
    int* indices;

    printf("pulling vertex and index count now\n");
    get_vertex_index_count_from_obj(obj_filename, &vertex_count, &index_count);
    printf("got %d vertices and %d indices\n", vertex_count, index_count);

    vertices = calloc(sizeof(ALLEGRO_VERTEX), vertex_count);
    indices = calloc(sizeof(int), index_count);

    printf("pulling data now\n");
    read_data_from_obj(obj_filename, vertices, vertex_count, indices, index_count);
    return ch_model_create(vertices, vertex_count, indices, index_count, NULL, buffer_flags, ALLEGRO_PRIM_TRIANGLE_LIST);
}

int get_vertex_index_count_from_obj(char* obj_filename, unsigned int *vcount, unsigned int *icount) {
    FILE* file = fopen(obj_filename, "r");
    if (file == NULL) {
        return 1;
    }

    char line_buffer[256];

    while (fgets(line_buffer, 256, file)) {
        if (strncmp(line_buffer, "v ", 2) == 0) {
            (*vcount)++;
        } else if (strncmp(line_buffer, "f ", 2) == 0) {
            (*icount) += 3;
        }
    }

    fclose(file);
    return 0;
}

int read_data_from_obj(char *obj_filename, ALLEGRO_VERTEX* vertices, unsigned int vcount, int* indices, unsigned int icount) {
    FILE* file = fopen(obj_filename, "r");
    char line_buffer[256];

    unsigned v = 0;
    unsigned i = 0;
    unsigned vt = 0;

    int rc = 0;

    // TODO: fix assumption that vcount and icount arent overran via like an overflow (this is like probably the fastest way ever to fuck this program up)

    while (fgets(line_buffer, 256, file)) {
        if (strncmp(line_buffer, "v ", 2) == 0) { // indicates a vertex
            float vs[6];

            rc = sscanf(line_buffer, "v %f %f %f %f %f %f", vs, vs+1, vs+2, vs+3, vs+4, vs+5);
            vertices[v].color = al_map_rgb_f(1, 1, 1);
            switch (rc)
            {
            case 6:
                // interpret as x y z r g b (x y z gets read in the next bits)
                vertices[v].color = al_map_rgb_f(vs[3], vs[4], vs[5]);
            case 3:
            case 4:
                // interpret as x y z [w] (no support for w)
                vertices[v].x = vs[0];
                vertices[v].y = vs[1];
                vertices[v].z = vs[2];
                break;
            default:
                // invalid vertex declaration, just blow up
                fprintf(stderr, "bad vertex format: %s\n", line_buffer);
                return 1;
                break;
            }
            v++;
        } else if (strncmp(line_buffer, "vt ", 3) == 0) { // looking at UV coordinates right now
            float uv[2];

            rc = sscanf(line_buffer, "vt %f %f", uv, uv+1);
            switch (rc)
            {
            case 2:
                //vertices[vt].v = uv[1];
            case 1:
                //vertices[vt].u = uv[0];
                break;
            default:
                fprintf(stderr, "bad vertex texture format: %s\n", line_buffer);
                return 1;
            }
            vt++;
        } else if (strncmp(line_buffer, "f ", 2) == 0) { // defining indices right now (in terms of faces which we consider triangles)
            int vi[3]; // assuming we only get apssed in triangles (bad assumption :sob:)
            int vti[3];
            int vni[3]; // lol we dont use these at all yet
            if ((rc = sscanf(line_buffer + 2, "%d %d %d", vi, vi + 1, vi + 2)) == 3) {
                indices[i] = vi[0] < 0 ? vcount + vi[0] : vi[0] - 1;
                indices[i+1] = vi[1] < 0 ? vcount + vi[1] : vi[1] - 1;
                indices[i+2] = vi[2] < 0 ? vcount + vi[2] : vi[2] - 1;
            } else if ((rc = sscanf(line_buffer + 2, "%d/%d %d/%d %d/%d", vi, vti, vi + 1, vti + 1, vi + 2, vti + 2)) == 6) {
                indices[i] = vi[0] < 0 ? vcount + vi[0] : vi[0] - 1;
                indices[i+1] = vi[1] < 0 ? vcount + vi[1] : vi[1] - 1;
                indices[i+2] = vi[2] < 0 ? vcount + vi[2] : vi[2] - 1;
            } else if ((rc = sscanf(line_buffer + 2, "%d//%d %d//%d %d//%d", vi, vni, vi + 1, vni + 1, vi + 2, vni + 2)) == 6) {
                indices[i] = vi[0] < 0 ? vcount + vi[0] : vi[0] - 1;
                indices[i+1] = vi[1] < 0 ? vcount + vi[1] : vi[1] - 1;
                indices[i+2] = vi[2] < 0 ? vcount + vi[2] : vi[2] - 1;
            } else if ((rc = sscanf(line_buffer + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", vi, vti, vni, vi + 1, vti + 1, vni + 1, vi + 2, vti + 2, vni + 2)) == 9) {
                indices[i] = vi[0] < 0 ? vcount + vi[0] : vi[0] - 1;
                indices[i+1] = vi[1] < 0 ? vcount + vi[1] : vi[1] - 1;
                indices[i+2] = vi[2] < 0 ? vcount + vi[2] : vi[2] - 1;
            } else {
                fprintf(stderr, "bad face format: %s\n", line_buffer);
                return 1;
            }

            i += 3;
        }
    }
    printf("ended up pulling out %d (w/ %d vts) vertices and %d indices\n", v, vt, i);

    fclose(file);

    return 0;
}