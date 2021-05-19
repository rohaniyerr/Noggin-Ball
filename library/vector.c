#include "vector.h"
#include "math.h"
#include <stdlib.h>

const vector_t VEC_ZERO = {.x = 0, .y = 0};

vector_t vec_add(vector_t v1, vector_t v2) {
    vector_t v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    return v;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
    return vec_add(v1, vec_negate(v2));
}

vector_t vec_negate(vector_t v) {
    return vec_multiply(-1, v);
}

vector_t vec_multiply(double scalar, vector_t v) {
    v.x = v.x * scalar;
    v.y = v.y * scalar;
    return v;
}

double vec_dot(vector_t v1, vector_t v2) {
    return (v1.x*v2.x + v1.y*v2.y);
}

double vec_cross(vector_t v1, vector_t v2) {
    return (v1.x*v2.y - v2.x*v1.y);
}

vector_t vec_rotate(vector_t v, double angle) {
    vector_t v_new;
    double x = v.x * cos(angle) - v.y*sin(angle);
    double y = v.x * sin(angle) + v.y*cos(angle);
    v_new.x = x;
    v_new.y = y;
    return v_new;
}

double vec_magnitude(vector_t v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}