#include "vector2.h"

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

vector2 vector2_unit(double radians) {
    return (vector2) { cos(radians), sin(radians) };
}