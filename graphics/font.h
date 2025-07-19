#pragma once
/*
"what is my purpose?"
"you're a header for a singleton"
"oh my god"
*/

/**
 * @file font.h
 * @brief Provides access to a singleton default font for rendering text.
 */

#include<allegro5/allegro_font.h>

/**
 * @brief Returns a pointer to the default font used for rendering.
 *        The font is managed as a singleton.
 * @return Pointer to ALLEGRO_FONT instance
 */
ALLEGRO_FONT* get_default_font();