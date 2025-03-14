#pragma once
#include<taglib/tag_c.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct ch_metadata ch_metadata;
typedef struct ch_song ch_song;

struct ch_metadata {
    char *title;
    char *artist;
    char *album;
    // char *album_artist;
    char *composer;
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
void ch_song_print(ch_song* song);