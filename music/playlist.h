#pragma once
#include<stdlib.h>

#include<allegro5/allegro.h>

#include "database.h"
#include "song.h"

typedef struct ch_playlist ch_playlist;

struct ch_playlist {
    int id; // IMPORTANT: negative id implies that it was NOT loaded from database
    const char* name;
    const char* picture_path;
    const char* description;
    
    ALLEGRO_BITMAP* thumbnail;
    ch_song_vec songs;
};

ch_playlist* ch_playlist_create(const char* name, const char* thumbnail_path, const char* description);
ch_playlist* ch_playlist_load(sqlite3* db, int id);
int ch_playlist_save(sqlite3* db, ch_playlist* playlist);

int ch_playlist_add_song(ch_playlist* playlist, ch_song* song);

ALLEGRO_BITMAP* ch_playlist_get_thumbnail(ch_playlist* playlist);
int ch_playlist_load_thumbnail(ch_playlist* playlist);

void ch_playlist_print(ch_playlist* playlist, int print_songs);