#pragma once

#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>

#include "vector.h"
#include "stdint.h"

/**
 * @file particle.h
 * @brief Defines the particle struct and functions for particle simulation and rendering.
 */

/**
 * @struct particle
 * @brief Represents a single particle with position, velocity, acceleration, color, and lifetime.
 */
struct particle {
    vector2 pos;           /**< Position of the particle */
    vector2 vel;           /**< Velocity of the particle */
    vector2 acc;           /**< Acceleration of the particle */
    ALLEGRO_COLOR color;   /**< Color of the particle */
    int32_t lifetime;      /**< Lifetime in update ticks */
    bool enabled;          /**< Whether the particle is active */
};

/**
 * @typedef particle
 * Alias for struct particle.
 */
typedef struct particle particle;

/**
 * @brief Creates a default-initialized particle.
 * @return Initialized particle
 */
particle particle_create();

/**
 * @brief Creates a particle with specified parameters.
 * @param pos Initial position
 * @param vec Initial velocity
 * @param acc Initial acceleration
 * @param color Particle color
 * @param lifetime Lifetime in update ticks
 * @return Initialized particle
 */
particle particle_create_params(vector2 pos, vector2 vec, vector2 acc, ALLEGRO_COLOR color, int32_t lifetime);

/**
 * @brief Updates the particle's state (position, velocity, lifetime).
 * @param p Pointer to particle
 */
void particle_update(particle*);

/**
 * @brief Draws the particle using Allegro primitives.
 * @param p Pointer to particle
 */
void particle_draw(particle*);