#pragma once
#include<sqlite3.h>
#include<taglib/tag_c.h>
#include<allegro5/allegro.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "database.h"

int import_music_from_file(sqlite3* db, const char* file_path);
int import_music_from_folder(sqlite3* db, const char* dir_path);
int import_music_from_folder_recursive(sqlite3* db, const char* dir_path);