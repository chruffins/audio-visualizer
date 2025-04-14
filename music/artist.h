#pragma once
#include<stdlib.h>

typedef struct ch_artist ch_artist;

struct ch_artist {
    int id;
    const char *name;
    const char *picture_path;
    const char *description;
};

inline ch_artist ch_artist_create(int artist_id, const char *name, const char *picture_path, const char *description) {
    return (ch_artist) { .id = artist_id, .name = name, .picture_path = picture_path, .description = description };
}

void ch_artist_print(ch_artist artist);