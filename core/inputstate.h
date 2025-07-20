#pragma once
#include<allegro5/allegro.h>

#include<stdint.h>

typedef struct input_state input_state;

struct input_state {
    uint8_t key_down[ALLEGRO_KEY_MAX];
    uint8_t key_state[ALLEGRO_KEY_MAX];
    uint8_t mouse_buttons[10];
    int mouse_dx;
    int mouse_dy;
};