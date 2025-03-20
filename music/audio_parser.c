#include "audio_parser.h"

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

static int import_file_callback(ALLEGRO_FS_ENTRY *entry, void *vdb) {
    sqlite3* db = (sqlite3*) vdb;
    printf("hello from %s\n", al_get_fs_entry_name(entry));
    return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
}

static int import_album_callback(ALLEGRO_FS_ENTRY *entry, void *valbum) {
    ch_album *album = (ch_album*)valbum;
    const char *fname = al_get_fs_entry_name(entry);
    
    if (al_get_fs_entry_mode(entry) == ALLEGRO_FILEMODE_ISDIR) {
        printf("skipping subdirectory %s\n", al_get_fs_entry_name(entry));
        return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
    }

    // appears we're on a file then, hopefully it's a compatible one?
    if (is_audio_file(fname)) {
        // song import logic
        printf("found music file %s\n", fname);
        ch_song_vec_push(&album->songs, ch_song_load(fname));
    } else if (is_image_file(fname)) {
        // album image import logic
        album->picture_path = fname;
    }

    return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
}

static int import_file_recursive_callback(ALLEGRO_FS_ENTRY *entry, void *vdb) {
    sqlite3* db = (sqlite3*) vdb;


    return ALLEGRO_FOR_EACH_FS_ENTRY_OK;
}

bool is_audio_file(const char* fpath) {
    return str_ends_with(fpath, "flac") || str_ends_with(fpath, "mp3") || str_ends_with(fpath, "ogg") || str_ends_with(fpath, "wav");
}

bool is_image_file(const char* ipath) {
    return str_ends_with(ipath, "jpg") || str_ends_with(ipath, "jpeg") || str_ends_with(ipath, "png");
}

int import_music_from_file(sqlite3* db, const char* file_path) {
    printf("just imported %s\n", file_path);
    return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
}

/* returns the number of files imported or -1 if ANY fail...? */
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

    printf("imported %d songs\n", album.songs.size);

    return 0;
}

int import_music_from_folder_recursive(sqlite3* db, const char* dir_path) {

}