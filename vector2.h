#pragma once

#include <math.h>

// i don't believe in more data types...
// also no fixed-width floating points?? seriously??
struct vector2 {
    double x;
    double y;
};

typedef struct vector2 vector2;

vector2 vector2_new(double, double);

// add element-wise
vector2 vector2_add(vector2, vector2);

// my.x * other.x + my.y * other.y
double vector2_dot(vector2, vector2);

// subtract element-wise
vector2 vector2_sub(vector2, vector2);

// get a unit vector from a radian input
vector2 vector2_unit(double);