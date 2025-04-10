#include "menu.h"

static ALLEGRO_MENU_INFO default_menu_info[] = {
    ALLEGRO_START_OF_MENU("&File", 1),
        { "&Open", FILE_OPEN_ID, 0, NULL },
        { "Open Fol&der", FOLDER_OPEN_ID, 0, NULL },
        { "E&xit", FILE_EXIT_ID, 0, NULL },
        ALLEGRO_END_OF_MENU,
    ALLEGRO_START_OF_MENU("&Help", 7),
        {"&About", 8, 0, NULL },
        ALLEGRO_END_OF_MENU,
    ALLEGRO_END_OF_MENU
};

ALLEGRO_MENU* get_menu() {
    return al_build_menu(default_menu_info);
}

char* choose_audio_file() {
    // need to change this to remember last directory or something
    ALLEGRO_FILECHOOSER* restrict fc = al_create_native_file_dialog("./", "Select Audio", "*.wav;*.ogg;*.flac;*.opus;*.mp3", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
    char* restrict copied_path = NULL;
    const char* restrict path;
    al_show_native_file_dialog(NULL, fc);

    int result = al_get_native_file_dialog_count(fc);
    if (result) {
        path = al_get_native_file_dialog_path(fc, 0);
        if (path) copied_path = strdup(path);
    }

    return copied_path;
}

char* choose_album_folder() {
    // temporary function cause we're switching to root music folder later
    ALLEGRO_FILECHOOSER* restrict fc = al_create_native_file_dialog("/home/chris/Music", "Select Album Folder", NULL, ALLEGRO_FILECHOOSER_FOLDER);
    char* restrict copied_path = NULL;
    const char* restrict path;
    al_show_native_file_dialog(NULL, fc);

    int result = al_get_native_file_dialog_count(fc);
    if (result) {
        path = al_get_native_file_dialog_path(fc, 0);
        if (path) copied_path = strdup(path);
    }

    return copied_path;
}