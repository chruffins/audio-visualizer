#pragma once
#include "vector.h"

typedef struct ch_ray ch_ray;

struct ch_ray {
    vector3 origin;
    vector3 direction;
    double length;
};

// Initializes a ch_ray with origin and (normalized) direction
void ch_ray_init(ch_ray* ray, const vector3* origin, const vector3* direction);

// Computes the point along the ray at distance t
void ch_ray_at(const ch_ray* ray, double t, vector3* out_point);

// Returns 1 if the ray intersects the plane, 0 otherwise.
// If there is an intersection, 't' is set to the distance along the ray.
// Plane is defined by a point and a normal.
int ch_ray_plane_intersect(const ch_ray* ray, const vector3* plane_point, const vector3* plane_normal, double* t);

// Returns 1 if the ray intersects the sphere, 0 otherwise.
// If there is an intersection, 't' is set to the nearest positive distance along the ray.
// Sphere is defined by center and radius.
int ch_ray_sphere_intersect(const ch_ray* ray, const vector3* sphere_center, double sphere_radius, double* t);