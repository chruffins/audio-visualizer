#include "audio_parser.h"

struct import_state {
    sqlite3 *db;
    int last_song_id;

    int last_album_id;
    const char* last_album_title;
};

static int import_file_callback(ALLEGRO_FS_ENTRY *entry, void *vdb);
static int import_album_callback(ALLEGRO_FS_ENTRY *entry, void *vdb);
static int import_file_recursive_callback(ALLEGRO_FS_ENTRY *entry, void *vdb);

static bool str_ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) return false;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len) {
        return 0;
    }

    return strcasecmp(str + str_len - suffix_len, suffix) == 0;
}

//static int import_folder_as_album(sqlite3 *db, ALLEGRO_FS_ENTRY *dir);
static int print_filename_callback(ALLEGRO_FS_ENTRY *entry, void *_) {
    //printf("%d\n", al_get_fs_entry_mode(entry));
    if (al_get_fs_entry_mode(entry) & ALLEGRO_FILEMODE_ISDIR) {
        printf("entering directory: %s\n", al_get_fs_entry_name(entry));
    } else {
        printf("%s\n", al_get_fs_entry_name(entry));
    }
    return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
}

static int import_file_callback(ALLEGRO_FS_ENTRY *entry, void *vdb) {
    sqlite3* db = (sqlite3*) vdb;
    printf("hello from %s\n", al_get_fs_entry_name(entry));
    return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
}

static int import_file_recursive_callback(ALLEGRO_FS_ENTRY *entry, void *vis) {
    struct import_state* state = (struct import_state*)vis;
    sqlite3* db = state->db;

    if (al_get_fs_entry_mode(entry) == ALLEGRO_FILEMODE_ISDIR) {
        return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
    }

    const char *fname = al_get_fs_entry_name(entry);
    ch_metadata song_data = {0};
    int rc;
    int artist_id = 0;
    int album_artist_id = 0;
    int genre_id = 0;
    int album_id = 0;
    int song_id = 0;

    if (is_audio_file(fname)) {
        rc = ch_metadata_populate(&song_data, fname);
    }

    if (rc != 0) {
        // TODO: add to some list of "broken" files
        return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
    }

    // first check artist because it's the most independent
    if (song_data.album_artist) album_artist_id = add_artist(db, song_data.album_artist, NULL, NULL);
    if (song_data.artist) artist_id = add_artist(db, song_data.artist, NULL, NULL);

    // then we do genre
    if (song_data.genre) genre_id = add_genre(db, song_data.genre);

    // then we do album
    if (song_data.album && (artist_id || album_artist_id)) {
        // figure out a way to infer album art...
        if (state->last_album_title && song_data.album && strcmp(state->last_album_title, song_data.album) == 0) {
            album_id = state->last_album_id;
        } else {
            printf("using a new album now...\n");
            album_id = add_album(db, song_data.album, album_artist_id ? album_artist_id : artist_id, NULL);
            state->last_album_title = strdup(song_data.album);
            state->last_album_id = album_id;
        }
    } else if (song_data.album) {
        printf("did not add album %s\n", song_data.album);
    }

    // finally do song...
    song_id = add_song(db, fname, song_data.title, album_id, song_data.track, song_data.comment, song_data.duration);

    // TODO: cleanup metadata

    return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
}

bool is_audio_file(const char* fpath) {
    return al_identify_sample(fpath) != NULL;
}

bool is_image_file(const char* ipath) {
    return al_identify_bitmap(ipath) != NULL;
}

int print_music_files(sqlite3* db, const char* root_dir_path) {
    printf("reading from %s\n", root_dir_path);

    ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(root_dir_path);
    if (dir == NULL) {
        return -1;
    }
    if (!al_fs_entry_exists(dir) || al_get_fs_entry_mode(dir) == ALLEGRO_FILEMODE_ISFILE) {
        printf("couldn't find folder!\n");
        al_destroy_fs_entry(dir);
        return -1;
    }

    al_for_each_fs_entry(dir, print_filename_callback, NULL);

    return 0;
}

/* returns the number of files imported or -1 if ANY fail...? */
/*
int import_album_from_folder(sqlite3* db, const char* dir_path) {
    ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(dir_path);
    ch_album album = ch_album_create();
    if (dir == NULL) {
        return -1;
    }
    if (!al_fs_entry_exists(dir) || al_get_fs_entry_mode(dir) == ALLEGRO_FILEMODE_ISFILE) {
        printf("couldn't find folder!\n");
        al_destroy_fs_entry(dir);
        return -1;
    }

    al_for_each_fs_entry(dir, import_album_callback, &album);
    if (album.songs.size > 0) {
        album.album_artist = strdup(album.songs.data[0]->metadata.album_artist);
    }

    ch_album_sort(&album);
    ch_album_shuffle(&album);
    ch_album_print(&album);
    for (int i = 0; i < album.songs.size; i++) {
        ch_song_print(album.songs.data[i]);
    }

    int artist_id = get_artist_id_by_name(db, album.album_artist);
    if (artist_id == -1) {
        artist_id = add_artist(db, album.album_artist, NULL, NULL);
        printf("created new artist %d\n", artist_id);
    } else {
        printf("found artist %d\n", artist_id);
    }

    int album_id = add_album(db, album.title, artist_id, album.picture_path);

    return 0;
}
*/
int import_music_from_folder_recursive(sqlite3* db, const char* dir_path) {
    //printf("%s\n", dir_path);
    ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(dir_path);
    if (dir == NULL) {
        return -1;
    }
    if (!al_fs_entry_exists(dir)) {
        printf("couldn't find folder %s!\n", dir_path);
        al_destroy_fs_entry(dir);
        return -1;
    }
    struct import_state state = {
        .db = db,
        .last_album_id = 0,
        .last_album_title = NULL
    };

    al_for_each_fs_entry(dir, import_file_recursive_callback, &state);

    return 0;
}