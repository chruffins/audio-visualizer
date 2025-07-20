#pragma once
#include<stdlib.h>

#include "../util/vector.h"
#include "song.h"

/**
 * @file album.h
 * @brief Defines the album struct and functions for album management.
 */

/**
 * @typedef ch_album
 * Struct representing a music album.
 */
typedef struct ch_album ch_album;

/**
 * @brief Defines a vector type for ch_album.
 */
DEF_VECTOR(ch_album)

/**
 * @struct ch_album
 * @brief Represents a music album with metadata.
 */
struct ch_album {
    int id;                   /**< Unique album ID */
    const char *title;        /**< Album title */
    int year;                 /**< Release year */
    const char *picture_path; /**< Path to album artwork */
    int artist_id;            /**< Artist ID */
};

/**
 * @brief Creates a default-initialized ch_album struct.
 * @return Initialized ch_album
 */
inline ch_album ch_album_create() {
    return (ch_album) { 0 };
}

/**
 * @brief Prints album information to stdout.
 * @param album Pointer to ch_album
 */
void ch_album_print(ch_album* album);
//void ch_album_sort(ch_album* album);
//void ch_album_shuffle(ch_album* album);

/**
 * @brief Initializes a ch_album_vec.
 * @return Initialized ch_album_vec
 */
inline ch_album_vec ch_album_vec_init() {
    ch_album_vec vec;
    VECTOR_INIT(vec);
    return vec;
}

/**
 * @brief Returns a pointer to the album at the given index.
 * @param vec Pointer to ch_album_vec
 * @param index Index of the album
 * @return Pointer to ch_album
 */
inline ch_album* ch_album_vec_at(ch_album_vec *vec, size_t index) {
    return VECTOR_AT(*vec, index);
}

/**
 * @brief Pushes an album onto the vector.
 * @param vec Pointer to ch_album_vec
 * @param album Album to add
 */
inline void ch_album_vec_push(ch_album_vec *vec, ch_album album) {
    VECTOR_PUSH(ch_album, *vec, album);
}

/**
 * @brief Reserves capacity for the vector.
 * @param vec Pointer to ch_album_vec
 * @param capacity Number of elements to reserve
 */
inline void ch_album_vec_reserve(ch_album_vec *vec, size_t capacity) {
    VECTOR_RESERVE(ch_album, *vec, capacity);
}

/**
 * @brief Frees memory used by the vector.
 * @param vec Pointer to ch_album_vec
 */
inline void ch_album_vec_free(ch_album_vec *vec) {
    VECTOR_FREE(*vec);
}