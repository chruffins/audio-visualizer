#include "audio_parser.h"

static int import_file_callback(ALLEGRO_FS_ENTRY *entry, void *vdb) {
    sqlite3* db = (sqlite3*) vdb;
    printf("hello from %s\n", al_get_fs_entry_name(entry));
    return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
}

int import_music_from_file(sqlite3* db, const char* file_path) {
    printf("just imported %s\n", file_path);
    return ALLEGRO_FOR_EACH_FS_ENTRY_SKIP;
}

/* returns the number of files imported or -1 if ANY fail...? */
int import_music_from_folder(sqlite3* db, const char* dir_path) {
    ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(dir_path);
    if (dir == NULL) {
        return -1;
    }
    if (!al_fs_entry_exists(dir) || al_get_fs_entry_mode(dir) == ALLEGRO_FILEMODE_ISFILE) {
        printf("couldn't find folder!\n");
        al_destroy_fs_entry(dir);
        return -1;
    }

    al_for_each_fs_entry(dir, import_file_callback, db);
}

int import_music_from_folder_recursive(sqlite3* db, const char* dir_path) {

}