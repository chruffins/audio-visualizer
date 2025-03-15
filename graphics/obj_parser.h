#pragma once
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct ch_obj ch_obj;
typedef struct ch_obj_vertex ch_obj_vertex;
typedef struct ch_obj_normal ch_obj_normal;
typedef struct ch_obj_vt ch_obj_vt;
typedef struct ch_obj_face_vertex ch_obj_face_vertex;

struct ch_obj_vertex {
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
};

struct ch_obj_normal {
    float x;
    float y;
    float z;
};

struct ch_obj_vt {
    float u;
    float v; // no w
};

struct ch_obj_face_vertex {
    unsigned int v; // what is the vertex in the list?
    unsigned int vt; // what vt do we use?
    unsigned int vn; // what normal do we use? (uhh are we using this lol)
};

struct ch_obj {
    unsigned int v_count;
    ch_obj_vertex* vertices;

    unsigned int n_count;
    ch_obj_normal* normals;

    unsigned int vt_count;
    ch_obj_vt* vts;

    unsigned int face_count; // note: this is the number of lines starting with "f"
    unsigned int triangle_count; // we can calculate this assuming that all faces are simple (fuck)
    unsigned int face_vertex_sum; // the SUM of number of face vertices
    unsigned int* face_vertex_counts; // array containing in each element the number of vertices in each face
    ch_obj_face_vertex* face_vertices; // pretty much raw container, requires us to use face_vertex_counts to delimit
};

void ch_obj_destroy(ch_obj* obj); // free all non-null resources and the obj itself...?
ch_obj* ch_obj_load(char* filename);