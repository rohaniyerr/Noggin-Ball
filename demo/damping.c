#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "forces.h"
 
#define WINDOW_TITLE "CS 3"
#define CIRC_NPOINTS 1000
 
const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
const int RADIUS = 10;
const int CIRCLE_MASS = 1;
const long INVISIBLE_MASS = 1000000;
const double GAMMA = 0.50;
const double K = 10;
const rgb_color_t WHITE = {1, 1, 1};
const double RIGHT_VELOCITY_POW = 1.50;
const double RIGHT_VELOCITY_NORM = 75;
const double LEFT_VELOCITY_POW = 1.69;
const double LEFT_VELOCITY_NORM = 35;

body_t *make_circle(vector_t spawn) {
    list_t *c = list_init(CIRC_NPOINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {RADIUS * cos(angle), RADIUS * sin(angle)};
        *v = vec_add(*v, spawn);
        list_add(c, v);
    }
    return body_init(c, CIRCLE_MASS, choose_rand_color());
}

body_t *make_invisible_circle(vector_t spawn) {
    list_t *c = list_init(CIRC_NPOINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {cos(angle), sin(angle)};
        *v = vec_add(*v, spawn);
        list_add(c, v);
    }
    return body_init(c, INVISIBLE_MASS, WHITE);
}
 
void spawn_circles(scene_t *scene) {
    size_t num_circles = WINDOW_WIDTH_ / (2*RADIUS);
    for (size_t i = 0; i < num_circles; i++){ 
        vector_t spawn = {.x = i*2*RADIUS + RADIUS, .y = WINDOW_HEIGHT_ / 2};
        body_t *circle = make_circle(spawn);
        scene_add_body(scene, circle);
    }
}

void spawn_invisible_circles(scene_t *scene) {
    size_t num_circles = WINDOW_WIDTH_ / (2*RADIUS);
    for (size_t i = 0; i < num_circles; i++){ 
        vector_t spawn = {.x = i*2*RADIUS + RADIUS, .y = WINDOW_HEIGHT_ / 2};
        body_t *invisible_circle = make_invisible_circle(spawn);
        scene_add_body(scene, invisible_circle);
    }
}
 
void set_left_circle_velocity(scene_t* scene) {
    size_t num_circles = WINDOW_WIDTH_ / (2*RADIUS);
    for (size_t i = 0; i < num_circles/2; i++) {
        body_t *body = scene_get_body(scene, i + num_circles);
        vector_t velocity = {.x = 0, .y = (pow(fabs(body_get_centroid(body).x - WINDOW_WIDTH_/2), LEFT_VELOCITY_POW)/LEFT_VELOCITY_NORM)};
        body_set_velocity(body, velocity);
    }
}
 
void set_right_circle_velocity(scene_t* scene) {
    size_t num_circles = WINDOW_WIDTH_ / (2*RADIUS);
    for (size_t i = num_circles/2; i < num_circles; i++) {
        body_t *body = scene_get_body(scene, i + num_circles);
        vector_t velocity = {.x = 0, .y = (pow(fabs(body_get_centroid(body).x - WINDOW_WIDTH_/2), RIGHT_VELOCITY_POW)/RIGHT_VELOCITY_NORM)};
        body_set_velocity(body, velocity);
    }
}
 
int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    scene_t *scene = scene_init();
    size_t num_circles = WINDOW_WIDTH_ / (2*RADIUS);
    spawn_invisible_circles(scene);
    spawn_circles(scene);
    set_left_circle_velocity(scene);
    set_right_circle_velocity(scene);
    for (size_t i = 0; i < num_circles; i++) {
        create_drag(scene, GAMMA, scene_get_body(scene, i + num_circles));
        create_spring(scene, K, scene_get_body(scene, i), scene_get_body(scene, i + num_circles));
    }
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    return 0;
}