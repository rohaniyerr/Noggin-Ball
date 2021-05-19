#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "polygon.h"

#define WINDOW_TITLE "CS 3"
const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;

const double ROTATION = 4;
const int RADIUS = 100;
const int NUM_POINTS = 12;
const vector_t MIN_POINT = {0,0};
const size_t X_VELOCITY = 100;
const size_t Y_VELOCITY = 100;

list_t *make_star(int points, int radius, vector_t center) {
    list_t *star = list_init(2 * points, (free_func_t) free);
    double angle = 2 * M_PI / points;

    //find the first point on the outside (point above centroid)
    vector_t outside_first = {.x = center.x, .y = center.y + radius};

    //find the first point on the inside (point to right of centroid)
    double inner_angle_start = (M_PI / points) + (M_PI/2);
    double inner_radius = RADIUS / 2;

    vector_t inside_first = {.x = inner_radius * cos(inner_angle_start),
                                        .y = inner_radius * sin(inner_angle_start)};
    inside_first = vec_add(inside_first, center);

    //given the first point, add each vector rotated into star
    for (size_t i = 0; i < points; i++) {
        double angle_rot = angle * i;
        vector_t *outside = malloc(sizeof(vector_t));
        *outside = vec_subtract(outside_first, center);
        *outside = vec_rotate(*outside, angle_rot);
        *outside = vec_add(*outside, center);

        vector_t *inside = malloc(sizeof(vector_t));
        *inside = vec_subtract(inside_first, center);
        *inside = vec_rotate(*inside, angle_rot);
        *inside = vec_add(*inside, center);

        list_add(star, outside);
        list_add(star, inside);
    }
    return star;
}

vector_t update_position (vector_t velocity, double dt, double radius, list_t *polygon) {
    vector_t center = polygon_centroid(polygon);
    vector_t dxy = vec_multiply(dt, velocity);
    double dx = dxy.x;
    double dy = dxy.y;

    if (dxy.x > 0 && center.x + dxy.x + radius >= WINDOW_WIDTH_) {
        velocity.x *= -1;
        dx = WINDOW_WIDTH_ - center.x - radius - dxy.x;
    }
    else if (dxy.x < 0 && center.x + dxy.x - radius <= 0) {
        velocity.x *= -1;
        dx = radius - center.x - dxy.x;
    }

    if (dxy.y > 0 && center.y + dxy.y + radius >= WINDOW_HEIGHT_) {
        velocity.y *= -1;
        dy = WINDOW_HEIGHT_- center.y - radius - dxy.y;
    }
    else if (dxy.y < 0 && center.y +dxy.y - radius <= 0) {
        velocity.y *= -1;
        dy = radius - center.y - dxy.y;
    }

    vector_t displacement = {.x = dx, .y = dy};
    polygon_translate(polygon, displacement);
    polygon_rotate(polygon, ROTATION * dt, center);
    return velocity;
}

int main () {
    const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
    rgb_color_t color = choose_rand_color();
    vector_t velocity = {.x = X_VELOCITY, .y = Y_VELOCITY};
    vector_t center = {.x = WINDOW_WIDTH_/2, .y = WINDOW_HEIGHT_/2};
    list_t *star = make_star(NUM_POINTS, RADIUS, center);
    sdl_init(MIN_POINT, MAX_POINT);
    scene_t *scene = scene_init();
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        sdl_clear();
        velocity = update_position(velocity, dt, RADIUS, star);
        sdl_draw_polygon(star, color);
        sdl_show();
    }
    list_free(star);
    scene_free(scene);
    return 0;
}