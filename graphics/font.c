#include "font.h"
#include "stdio.h"

ALLEGRO_FONT* get_default_font() {
    static ALLEGRO_FONT* default_font = NULL;
    if (!default_font) {
        default_font = al_create_builtin_font();
        // TODO: crash the program in a predictable way if this also fails
        if (!default_font) printf("default font failed to load\n");
    }

    return default_font;
}