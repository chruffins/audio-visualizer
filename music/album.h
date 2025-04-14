#pragma once
#include<stdlib.h>

#include "../util/vector.h"
#include "song.h"

typedef struct ch_album ch_album;
DEF_VECTOR(ch_album)

struct ch_album {
    int id;
    const char *title;
    int year;
    const char *picture_path;
    int artist_id;
};

inline ch_album ch_album_create() {
    return (ch_album) { 0 };
}

void ch_album_print(ch_album* album);
//void ch_album_sort(ch_album* album);
//void ch_album_shuffle(ch_album* album);

inline ch_album_vec ch_album_vec_init() {
    ch_album_vec vec;
    VECTOR_INIT(vec);
    return vec;
}

inline ch_song* ch_album_vec_at(ch_album_vec *vec, size_t index) {
    return VECTOR_AT(*vec, index);
}

inline void ch_album_vec_push(ch_album_vec *vec, ch_album album) {
    VECTOR_PUSH(ch_album, *vec, album);
}

inline void ch_album_vec_reserve(ch_album_vec *vec, size_t capacity) {
    VECTOR_RESERVE(ch_song, *vec, capacity);
}

inline void ch_album_vec_free(ch_album_vec *vec) {
    VECTOR_FREE(*vec);
}