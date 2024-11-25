#include "particle.h"

particle particle_create() {
    return (particle) { (vector2){ 0, 0 }, (vector2){ 0, 0 }, (vector2){ 0, 0 }, al_map_rgb_f(1.0, 1.0, 1.0), 300, true};
}

particle particle_create_params(vector2 pos, vector2 vec, vector2 acc, ALLEGRO_COLOR color, int32_t lifetime) {
    return (particle) { pos, vec, acc, color, lifetime, true };
}

void particle_update(particle* restrict this) {
    this->vel = vector2_add(this->vel, this->acc);
    this->pos = vector2_add(this->pos, this->vel);

    this->lifetime -= 1;

    if (this->lifetime == 0) {
        this->enabled = false;
    }
}

void particle_draw(particle* restrict this) {
    if (!this->enabled) return;

    al_draw_circle(this->pos.x, this->pos.y, 3, this->color, 1);
}