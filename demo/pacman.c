#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"


#define WINDOW_TITLE "CS 3"
#define CIRC_NPOINTS 1000

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
const vector_t PACMAN_SPAWN = {500, 250};
const int PACMAN_RADIUS = 50;
const int PELLET_RADIUS = 10;
const double INITIAL_PACMAN_MASS = 1;
const double CIRCLE_MASS = 1;
const rgb_color_t PACMAN_COLOR = {1, 1, 0};
const double THETA = M_PI / 6;
const double SPAWN_RATE = 3;
const double ACCELERATION = 300;
const size_t INITIAL_PELLETS = 10;

vector_t random_spawn() {
    vector_t spawn = {(rand() % WINDOW_WIDTH_), (rand() % WINDOW_HEIGHT_)};
    return spawn;
}

body_t *make_circle() {
    vector_t spawn = random_spawn();
    list_t *c = list_init(CIRC_NPOINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {PELLET_RADIUS * cos(angle), PELLET_RADIUS * sin(angle)};
        *v = vec_add(*v, spawn);
        list_add(c, v);
    }
    return body_init(c, CIRCLE_MASS, choose_rand_color());
}

void pacman_eat(scene_t *scene) {
    body_t *pacman = scene_get_body(scene, 0);
    if (scene_bodies(scene) > 1) {
        for (size_t i = 1; i < scene_bodies(scene); i++) {
            body_t *circle = scene_get_body(scene, i);
            vector_t pac_center = body_get_centroid(pacman);
            double circle_x = body_get_centroid(circle).x;
            double circle_y = body_get_centroid(circle).y;
            if (PACMAN_RADIUS / 2 > fabs(circle_x - pac_center.x) && PACMAN_RADIUS / 2 > fabs(circle_y - pac_center.y)) {
                body_set_mass(pacman, body_get_mass(pacman) + 1);
                scene_remove_body(scene, i);
            }
        }
    }
}

void wrap_around(body_t *pacman) {
    if (body_get_centroid(pacman).x - PACMAN_RADIUS > WINDOW_WIDTH_) {
        double new_x = body_get_centroid(pacman).x - WINDOW_WIDTH_;
        vector_t new_centroid = {new_x, body_get_centroid(pacman).y};
        body_set_centroid(pacman, new_centroid);
    }
    else if (body_get_centroid(pacman).x + PACMAN_RADIUS < 0) {
        double new_x = WINDOW_WIDTH_ - body_get_centroid(pacman).x;
        vector_t new_centroid = {new_x, body_get_centroid(pacman).y};
        body_set_centroid(pacman, new_centroid);
    }
    if (body_get_centroid(pacman).y + PACMAN_RADIUS < 0) {
        double new_y = WINDOW_HEIGHT_ - body_get_centroid(pacman).y;
        vector_t new_centroid = {body_get_centroid(pacman).x, new_y};
        body_set_centroid(pacman, new_centroid);
    }
    else if (body_get_centroid(pacman).y - PACMAN_RADIUS > WINDOW_HEIGHT_) {
        double new_y = body_get_centroid(pacman).y - WINDOW_HEIGHT_;
        vector_t new_centroid = {body_get_centroid(pacman).x, new_y};
        body_set_centroid(pacman, new_centroid);
    }
}

body_t *make_pacman() {
    list_t *shape = list_init(CIRC_NPOINTS + 1, (free_func_t) free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = THETA + 2 * (M_PI - THETA) * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {PACMAN_RADIUS * cos(angle), PACMAN_RADIUS * sin(angle)};
        *v = vec_add(*v, PACMAN_SPAWN);
        list_add(shape, v);
    }
    vector_t *SPAWN = malloc(sizeof(vector_t));
    *SPAWN = PACMAN_SPAWN;
    list_add(shape, SPAWN);
    return body_init(shape, INITIAL_PACMAN_MASS, PACMAN_COLOR);
}

void spawn_pellets(scene_t *scene, size_t INITIAL_PELLETS) {
    for (size_t i = 0; i < INITIAL_PELLETS; i++){ 
        body_t *pellet = make_circle();
        scene_add_body(scene, pellet);
    }
}

void on_key(char key, key_event_type_t type, double held_time, void *scene) {
    body_t *pacman = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
        switch (key) {
            case UP_ARROW:
                body_set_rotation(pacman, M_PI / 2.0);
                vector_t up = {0, held_time * ACCELERATION};
                body_set_velocity(pacman, up);
                break;
            case RIGHT_ARROW:
                body_set_rotation(pacman, 0);
                vector_t right = {held_time * ACCELERATION, 0};
                body_set_velocity(pacman, right);
                break;
            case LEFT_ARROW:
                body_set_rotation(pacman, M_PI);
                vector_t left = {-held_time * ACCELERATION, 0};
                body_set_velocity(pacman, left);
                break;
            case DOWN_ARROW:
                body_set_rotation(pacman, 3*M_PI / 2.0);
                vector_t down = {0, -held_time * ACCELERATION};
                body_set_velocity(pacman, down);
                break;
        }
    }
    else {
        body_set_velocity(pacman, VEC_ZERO);
    }
}

int main() {
    body_t *pacman = make_pacman();
    sdl_init(MIN_POINT, MAX_POINT);
    scene_t *scene = scene_init();
    scene_add_body(scene, pacman);
    spawn_pellets(scene, INITIAL_PELLETS);
    double time = 0;
    sdl_on_key((key_handler_t) on_key);
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        time += dt;
        if (time > SPAWN_RATE) {
            body_t *circle = make_circle();
            scene_add_body(scene, circle);
            time = 0;
        }
        scene_tick(scene, dt);
        wrap_around(pacman);
        pacman_eat(scene);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    return 0;
}
