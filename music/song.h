#pragma once
#include<taglib-2/tag_c.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../util/vector.h"

typedef struct ch_metadata ch_metadata;
typedef struct ch_song ch_song;
DEF_VECTOR(ch_song)

struct ch_metadata {
    const char *title;
    const char *artist;
    const char *album;
    const char *album_artist;
    const char *genre;
    const char *comment;
    unsigned int track;
    unsigned int disc;
    unsigned int year;
    unsigned int duration;
    // above are basic tags

    // below are more specialized tags
    const char *composer;
    const char *conductor;
    float bpm;
};

struct ch_song {
    int id;
    int order; // this is used for sorting by order for albums and playlists...
    char* filename;
    ch_metadata metadata;
};

ch_metadata ch_metadata_create();

ch_song* ch_song_load(const char* filename);

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

inline void ch_song_vec_reserve(ch_song_vec *vec, size_t capacity) {
    VECTOR_RESERVE(ch_song*, *vec, capacity);
}

inline void ch_song_vec_free(ch_song_vec *vec) {
    VECTOR_FREE(*vec);
}

void ch_song_print(ch_song* song);

int ch_song_track_comparator(const void* a, const void* b);