#pragma once
#include<allegro5/allegro.h>
#include<allegro5/allegro_native_dialog.h>

#define FILE_OPEN_ID 2
#define FILE_EXIT_ID 6

ALLEGRO_MENU* get_menu();

// responsibility of destroying the string is on the user
const char* choose_audio_file();