#include "vector.h"

vector2 vector2_new(double x, double y) {
    return (vector2) { x, y };
}

vector2 vector2_add(vector2 a, vector2 b) {
    return (vector2) { a.x + b.x, a.y + b.y };
}

double vector2_dot(vector2 a, vector2 b) {
    return (a.x * b.x) + (a.y * b.y); 
}

vector2 vector2_sub(vector2 a, vector2 b) {
    return (vector2) { a.x - b.x, a.y - b.y }; 
}

vector2 vector2_from_rad(double radians) {
    return (vector2) { cos(radians), sin(radians) };
}

vector2 vector2_mul(vector2 x, double scalar) {
    return (vector2) { x.x * scalar, x.y * scalar }; 
}

vector3 vector3_new(double x, double y, double z) {
    return (vector3){ x, y, z };
}

vector3 vector3_add(vector3 a, vector3 b) {
    return (vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

vector3 vector3_sub(vector3 a, vector3 b) {
    return (vector3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

double vector3_dot(vector3 a, vector3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

vector3 vector3_cross(vector3 a, vector3 b) {
    return (vector3){ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x } ;
}

vector3 vector3_mul(vector3 a, double f) {
    return (vector3) {a.x * f, a.y * f, a.z * f};
}

vector3 vector3_norm(vector3 a) {
    double mag = vector3_mag(a);

    return mag == 0 ? a : vector3_mul(a, 1 / mag);
}

double vector3_mag(vector3 a) {
    return sqrt(vector3_dot(a, a));
}

void vector3_iadd(vector3* restrict a, vector3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}
