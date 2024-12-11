#pragma once

#include <math.h>

// i don't believe in more data types...
// also no fixed-width floating points?? seriously??
struct vector2 {
    double x;
    double y;
};

struct vector3 {
    double x;
    double y;
    double z;
};

typedef struct vector2 vector2;
typedef struct vector3 vector3;

/* VECTOR2 FUNCTIONS!!! */

vector2 vector2_new(double, double);

// add element-wise
vector2 vector2_add(vector2, vector2);

// my.x * other.x + my.y * other.y
double vector2_dot(vector2 a, vector2 b);

// vector2 vector2_cross(vector2 a, vector2 b);

// subtract element-wise
vector2 vector2_sub(vector2, vector2);

// get a unit vector from a radian input
vector2 vector2_from_rad(double);

// get a normalized vector
vector2 vector2_norm(vector2);

// multiply a vector by a scalar
vector2 vector2_mul(vector2, double f);

// get magnitude aka norm
double vector2_mag(vector2 a);

/* and now for vector3 functions */

vector3 vector3_new(double x, double y, double z);

vector3 vector3_add(vector3 a, vector3 b);
vector3 vector3_sub(vector3 a, vector3 b);
double vector3_dot(vector3 a, vector3 b);
vector3 vector3_cross(vector3 a, vector3 b);
vector3 vector3_mul(vector3 a, double f);
vector3 vector3_norm(vector3 a);
double vector3_mag(vector3 a);

void vector3_iadd(vector3* a, vector3 b);