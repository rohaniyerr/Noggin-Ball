#ifndef __COLOR_H__
#define __COLOR_H__

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} rgb_color_t;

rgb_color_t choose_rand_color();

double get_r_color(rgb_color_t color);
double get_g_color(rgb_color_t color);
double get_b_color(rgb_color_t color);
rgb_color_t *rgb_color_init(double r, double g, double b);

#endif // #ifndef __COLOR_H__