#pragma once
#include<taglib-2/tag_c.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#include "../util/vector.h"

/**
 * @typedef ch_metadata
 * Struct holding metadata information for a song.
 */
typedef struct ch_metadata ch_metadata;

/**
 * @typedef ch_song
 * Struct representing a song entry.
 */
typedef struct ch_song ch_song;

/**
 * @brief Defines a vector type for ch_song.
 */
DEF_VECTOR(ch_song)

/**
 * @struct ch_metadata
 * @brief Holds metadata tags for a music file.
 */
struct ch_metadata {
    const char *filepath;      /**< Path to the music file */
    const char *title;         /**< Song title */
    const char *artist;        /**< Song artist */
    const char *album;         /**< Album name */
    const char *album_artist;  /**< Album artist */
    const char *genre;         /**< Genre */
    const char *comment;       /**< Comment */
    unsigned int track;        /**< Track number */
    unsigned int disc;         /**< Disc number */
    unsigned int year;         /**< Year of release */
    unsigned int duration;     /**< Duration in seconds */
    // above are basic tags

    // below are more specialized tags
    const char *composer;      /**< Composer */
    const char *conductor;     /**< Conductor */
    float bpm;                 /**< Beats per minute */
};

/**
 * @struct ch_song
 * @brief Represents a song entry in the library.
 */
struct ch_song {
    int id;                    /**< Unique song ID */
    const char *filename;      /**< File name */
    const char *title;         /**< Song title */
    int album_id;              /**< Album ID */
    int track;                 /**< Track number */
    const char *comment;       /**< Comment */
    int duration;              /**< Duration in seconds */
    //ch_metadata metadata;
};

/**
 * @brief Creates a default-initialized ch_metadata struct.
 * @return ch_metadata instance
 */
ch_metadata ch_metadata_create();

/**
 * @brief Populates a ch_metadata struct from a file.
 * @param md Pointer to ch_metadata to populate
 * @param filename Path to the file
 * @return 0 on success, nonzero on failure
 */
int ch_metadata_populate(ch_metadata* md, char* filename);

/**
 * @brief Initializes a ch_song_vec.
 * @return Initialized ch_song_vec
 */
inline ch_song_vec ch_song_vec_init() {
    ch_song_vec vec;
    VECTOR_INIT(vec);
    return vec;
}

/**
 * @brief Returns a pointer to the song at the given index.
 * @param vec Pointer to ch_song_vec
 * @param index Index of the song
 * @return Pointer to ch_song
 */
inline ch_song* ch_song_vec_at(ch_song_vec *vec, size_t index) {
    return VECTOR_AT(*vec, index);
}

/**
 * @brief Pushes a song onto the vector.
 * @param vec Pointer to ch_song_vec
 * @param song Song to add
 */
inline void ch_song_vec_push(ch_song_vec *vec, ch_song song) {
    VECTOR_PUSH(ch_song, *vec, song);
}

/**
 * @brief Reserves capacity for the vector.
 * @param vec Pointer to ch_song_vec
 * @param capacity Number of elements to reserve
 */
inline void ch_song_vec_reserve(ch_song_vec *vec, size_t capacity) {
    VECTOR_RESERVE(ch_song, *vec, capacity);
}

/**
 * @brief Frees memory used by the vector.
 * @param vec Pointer to ch_song_vec
 */
inline void ch_song_vec_free(ch_song_vec *vec) {
    VECTOR_FREE(*vec);
}

/**
 * @brief Prints song information to stdout.
 * @param song Pointer to ch_song
 */
void ch_song_print(ch_song* song);

/**
 * @brief Returns a pointer to a random song within the song_vec
 * @param vec Pointer to ch_song_vec
 */
inline ch_song* ch_song_vec_rand(ch_song_vec* vec) {
    srandom(time(NULL));
    if (vec->size == 0) return NULL;
    size_t t = random() % vec->size;
    return &vec->data[t];
}

/**
 * @brief Comparator for sorting songs by track number.
 * @param a Pointer to first song
 * @param b Pointer to second song
 * @return Comparison result
 */
int ch_song_track_comparator(const void* a, const void* b);