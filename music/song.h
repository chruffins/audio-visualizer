#pragma once
#include<taglib/tag_c.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../util/vector.h"

typedef struct ch_metadata ch_metadata;
typedef struct ch_song ch_song;
DEF_VECTOR(ch_song)

struct ch_metadata {
    char *title;
    char *artist;
    char *album;
    // char *album_artist;
    // char *composer;
    char *genre;
    char *comment;
    unsigned int track;
    unsigned int disc;
    unsigned int year;
    float bpm;
};

struct ch_song {
    char* filename;
    ch_metadata metadata;
};

ch_metadata ch_metadata_create();

ch_song* ch_song_create(char* filename);

ch_song* ch_song_load(char* filename);

inline ch_song_vec ch_song_vec_init() {
    ch_song_vec vec;
    VECTOR_INIT(vec);
    return vec;
}

inline ch_song* ch_song_vec_at(ch_song_vec *vec, size_t index) {
    return VECTOR_AT(*vec, index);
}

inline void ch_song_vec_push(ch_song_vec *vec, ch_song *song) {
    VECTOR_PUSH(ch_song*, *vec, song);
}

inline void ch_song_vec_free(ch_song_vec *vec) {
    VECTOR_FREE(*vec);
}

void ch_song_print(ch_song* song);