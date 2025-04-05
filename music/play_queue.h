#pragma once
#include<allegro5/allegro.h>

#include "../util/queue.h"

#include "song.h"
#include "album.h"
#include "playlist.h"

typedef enum CH_LOOP_MODE CH_LOOP_MODE;
typedef enum CH_CONTAINER_MODE CH_CONTAINER_MODE;
typedef union ch_music_container ch_music_container;
typedef struct ch_play_queue ch_play_queue;

enum CH_LOOP_MODE {
    CH_LOOP_NONE,
    CH_LOOP_YEAH,
    CH_LOOP_SONG
};

enum CH_CONTAINER_MODE {
    CH_CONTAINER_NONE,
    CH_CONTAINER_SONG,
    CH_CONTAINER_PLAYLIST,
    CH_CONTAINER_ALBUM
};

union ch_music_container {
    ch_song* song;
    ch_playlist* playlist;
    ch_album* album;
};

struct ch_play_queue {
    ch_queue priority_songs;
    ch_music_container current;
    CH_LOOP_MODE loop_mode;
    CH_CONTAINER_MODE container_mode;

    // need some things to track shuffle...
    size_t current_song_index;
};

ch_play_queue* ch_play_queue_create();
ch_song* ch_play_queue_get_next_song(ch_play_queue* play_queue);
void ch_play_queue_destroy(ch_play_queue* play_queue);