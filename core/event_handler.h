#pragma once
#include "menu.h"

void handle_display_resize(ALLEGRO_EVENT *event);
void handle_display_close(ALLEGRO_EVENT *event);
void handle_audio_stream_finished(ALLEGRO_EVENT *event);
void handle_key_down(ALLEGRO_EVENT *event);
void handle_key_up(ALLEGRO_EVENT *event);
void handle_timer(ALLEGRO_EVENT *event);
void handle_menu_click(ALLEGRO_EVENT *event);
void handle_mouse_button_down(ALLEGRO_EVENT *event);
void handle_mouse_button_up(ALLEGRO_EVENT *event);
void handle_mouse_axes(ALLEGRO_EVENT *event);