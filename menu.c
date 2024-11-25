#include "menu.h"

static ALLEGRO_MENU_INFO default_menu_info[] = {
   ALLEGRO_START_OF_MENU("&File", 1),
      { "&Open", 2, 0, NULL },
      ALLEGRO_START_OF_MENU("Open &Recent...", 3),
         { "Recent 1", 4, 0, NULL },
         { "Recent 2", 5, 0, NULL },
         ALLEGRO_END_OF_MENU,
      ALLEGRO_MENU_SEPARATOR,
      { "E&xit", 6, 0, NULL },
      ALLEGRO_END_OF_MENU,
   ALLEGRO_START_OF_MENU("&Help", 7),
      {"&About", 8, 0, NULL },
      ALLEGRO_END_OF_MENU,
   ALLEGRO_END_OF_MENU
};

ALLEGRO_MENU* get_menu() {
    return al_build_menu(default_menu_info);
}
