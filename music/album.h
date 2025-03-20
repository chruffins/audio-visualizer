#pragma once
#include "song.h"

typedef struct ch_album ch_album;

struct ch_album {
    int id;
    const char *name;
    int year;
    const char *picture_path;
    const char *album_artist;
    ch_song_vec songs;
};

inline ch_album ch_album_create() {
    return (ch_album) { 0 };
}