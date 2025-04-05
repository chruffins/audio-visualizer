#include "play_queue.h"

ch_play_queue* ch_play_queue_create() {
    ch_play_queue* play_queue = malloc(sizeof(ch_play_queue));
    play_queue->priority_songs = ch_queue_init();
    play_queue->current.album = NULL;
    play_queue->loop_mode = CH_LOOP_NONE;
    play_queue->container_mode = CH_CONTAINER_NONE;

    return play_queue;
}

ch_song* ch_play_queue_get_next_song(ch_play_queue* play_queue) {
    switch (play_queue->loop_mode)
    {
    case CH_LOOP_NONE:
        if (!ch_queue_is_empty(&play_queue->priority_songs)) {
            return ch_queue_pop(&play_queue->priority_songs);
        }
        if (play_queue->container_mode == CH_CONTAINER_SONG) {
            return play_queue->current.song;
        } else if (play_queue->container_mode == CH_CONTAINER_ALBUM) {
            play_queue->current_song_index = (play_queue->current_song_index + 1);
            return ch_song_vec_at(&play_queue->current.album->songs, play_queue->current_song_index);
        } else if (play_queue->container_mode == CH_CONTAINER_PLAYLIST) {
            play_queue->current_song_index = (play_queue->current_song_index + 1);
            return ch_song_vec_at(&play_queue->current.playlist->songs, play_queue->current_song_index);
        }
        break;
    case CH_LOOP_SONG:
        if (!ch_queue_is_empty(&play_queue->priority_songs)) {
            return ch_queue_peek(&play_queue->priority_songs);
        }
        if (play_queue->container_mode == CH_CONTAINER_SONG) {
            return play_queue->current.song;
        } else if (play_queue->container_mode == CH_CONTAINER_ALBUM) {
            return ch_song_vec_at(&play_queue->current.album->songs, play_queue->current_song_index);
        } else if (play_queue->container_mode == CH_CONTAINER_PLAYLIST) {
            return ch_song_vec_at(&play_queue->current.playlist->songs, play_queue->current_song_index);
        }
        break;
    case CH_LOOP_YEAH:
        // implement the looping, i guess
        if (play_queue->container_mode == CH_CONTAINER_SONG) {
            return play_queue->current.song;
        } else if (play_queue->container_mode == CH_CONTAINER_ALBUM) {
            play_queue->current_song_index = (play_queue->current_song_index + 1) % play_queue->current.album->songs.size;
            return ch_song_vec_at(&play_queue->current.album->songs, play_queue->current_song_index);
        } else if (play_queue->container_mode == CH_CONTAINER_PLAYLIST) {
            play_queue->current_song_index = (play_queue->current_song_index + 1) % play_queue->current.playlist->songs.size;
            return ch_song_vec_at(&play_queue->current.playlist->songs, play_queue->current_song_index);
        }
        break;
    }
}

void ch_play_queue_destroy(ch_play_queue* play_queue) {

}