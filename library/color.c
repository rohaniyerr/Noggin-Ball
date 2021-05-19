#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "color.h"

rgb_color_t choose_rand_color() {
    double r = (float)((rand() % 100) / 100.0);
    double g = (float)((rand() % 100) / 100.0);
    double b = (float)((rand() % 100) / 100.0);
    rgb_color_t color = {r, g, b};
    return color;
}

double get_r_color(rgb_color_t color){
    return color.r;
}
double get_g_color(rgb_color_t color){
    return color.g;
}
double get_b_color(rgb_color_t color){
    return color.b;
}

rgb_color_t *rgb_color_init(double r, double g, double b){
    rgb_color_t *color = malloc(sizeof(rgb_color_t));
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}