#pragma once
#include<allegro5/allegro.h>
#include<allegro5/allegro_native_dialog.h>

#define MENU_FILE_TOPLEVEL 10
#define MENU_FILE_OPEN 11
#define MENU_FILE_FOLDER 12
#define MENU_FILE_EXIT 13

#define DEBUG_MENU_TOPLEVEL 20
#define DEBUG_MENU_LIST_ALBUMS 21
#define DEBUG_MENU_LIST_ARTISTS 22
#define DEBUG_MENU_LIST_GENRES 23

ALLEGRO_MENU* get_menu();

// responsibility of destroying the string is on the user
char* choose_audio_file();

// responsibility of destroying the string is on the user
char* choose_album_folder();

