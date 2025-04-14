#pragma once
#include<stdlib.h>

typedef struct ch_genre ch_genre;

struct ch_genre {
    int id;
    const char* name;
};

inline ch_genre ch_genre_create(int id, const char* name) {
    return (ch_genre) {.id = id, .name = name};
}
