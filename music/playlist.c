#include "playlist.h"

ch_playlist* ch_playlist_create(const char* name, const char* thumbnail_path, const char* description) {
    ch_playlist* new_playlist = malloc(sizeof(ch_playlist));
    if (!new_playlist) return NULL;

    new_playlist->id = -1;
    new_playlist->name = name;
    new_playlist->picture_path = thumbnail_path;
    new_playlist->description = description;
    //new_playlist->songs = ch_song_vec_init();
}

/*
ch_playlist* ch_playlist_load(sqlite3* db, int id) {

}
*/

int ch_playlist_add_song(ch_playlist* playlist, ch_song* song) {
    //ch_song_vec_push(&playlist->songs, song);

    return 0; // yeah hmm uhh
}

/*
int ch_playlist_save(sqlite3* db, ch_playlist* playlist) {
    // wait... we can separate playlist saves here because of how they um...
    int is_preexisting = playlist->id < 0 ? 1 : 0;
    int new_playlist_id;

    if (is_preexisting) {
        // UPDATE the playlist instead of saving
    } else {
        new_playlist_id = add_playlist(db, playlist->name, playlist->picture_path, playlist->description);
        if (new_playlist_id > 0) {
            playlist->id = new_playlist_id;
        } else {
            return -1;
        }
    }
    return 0;
}
*/

ALLEGRO_BITMAP* ch_playlist_get_thumbnail(ch_playlist* playlist) {
    if (playlist->thumbnail) {
        return playlist->thumbnail;
    }

    if (playlist->picture_path) {
        playlist->thumbnail = al_load_bitmap(playlist->picture_path);
        return playlist->thumbnail;
    }

    return NULL;
}

int ch_playlist_load_thumbnail(ch_playlist* playlist) {

}

void ch_playlist_print(ch_playlist* playlist, int print_songs) {
    if (!playlist) return;
    
    printf("Playlist: %s", playlist->name ? playlist->name : "(empty)");
    printf("Thumbnail: %s", playlist->picture_path ? playlist->picture_path : "(empty)");
    printf("Description: %s", playlist->description ? playlist->description : "(empty)");
    /*
    printf("# of Songs: %d", playlist->songs.size);

    if (print_songs) {
        for (size_t i = 0; i < playlist->songs.size; i++) {
            printf("Song %d: %s", i, playlist->songs.data[i]->metadata.title);
        }
    }
    */
}