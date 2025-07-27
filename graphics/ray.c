#include "ray.h"
#include <math.h>

// Initializes a ch_ray with origin and (normalized) direction
void ch_ray_init(ch_ray* ray, const vector3* origin, const vector3* direction) {
    ray->origin = *origin;
    ray->direction = vector3_norm(*direction);
    ray->length = 0.0; // Optional: set to 0 or some default, can be ignored if unused
}

// Computes the point along the ray at distance t
void ch_ray_at(const ch_ray* ray, double t, vector3* out_point) {
    *out_point = vector3_add(ray->origin, vector3_mul(ray->direction, t));
}

// Returns 1 if the ray intersects the plane, 0 otherwise.
int ch_ray_plane_intersect(const ch_ray* ray, const vector3* plane_point, const vector3* plane_normal, double* t) {
    double denom = vector3_dot(ray->direction, *plane_normal);
    if (fabs(denom) < 1e-8) {
        return 0; // Parallel, no intersection
    }
    vector3 diff = vector3_sub(*plane_point, ray->origin);
    *t = vector3_dot(diff, *plane_normal) / denom;
    return (*t >= 0.0);
}

// Returns 1 if the ray intersects the sphere, 0 otherwise.
// Sphere is defined by center and radius.
int ch_ray_sphere_intersect(const ch_ray* ray, const vector3* sphere_center, double sphere_radius, double* t) {
    vector3 oc = vector3_sub(ray->origin, *sphere_center);
    double a = vector3_dot(ray->direction, ray->direction);
    double b = 2.0 * vector3_dot(oc, ray->direction);
    double c = vector3_dot(oc, oc) - sphere_radius * sphere_radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return 0;
    }
    double sqrt_disc = sqrt(discriminant);
    double t0 = (-b - sqrt_disc) / (2.0 * a);
    double t1 = (-b + sqrt_disc) / (2.0 * a);
    // Find the nearest positive t
    if (t0 >= 0.0) {
        *t = t0;
        return 1;
    } else if (t1 >= 0.0) {
        *t = t1;
        return 1;
    }
    return 0;
}

