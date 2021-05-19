#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "forces.h"

#define WINDOW_TITLE "CS 3"

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
const size_t num_stars = 100;
const vector_t initial_velocity = {0, 0};
const double rotate = 0;
const double gravity = 1000;
const int points = 4;
const double angle =  2 * M_PI / points;

body_t *make_star()
{
    vector_t center = {.x=rand() % (WINDOW_WIDTH_/2) + (WINDOW_WIDTH_/4), .y = rand() % (WINDOW_HEIGHT_/2) + (WINDOW_HEIGHT_/4)};
    list_t *star_list = list_init(2 * points, (free_func_t) free);
    double radius = (rand() % 25) + 1;
    vector_t outside_first = {.x = center.x, .y = center.y + radius};
    double inner_angle_start = (M_PI / points) + (M_PI / 2);
    double inner_radius = radius / 2;

    vector_t inside_first = {.x = inner_radius * cos(inner_angle_start), .y = inner_radius * sin(inner_angle_start)};
    inside_first = vec_add(inside_first, center);
    for (size_t i = 0; i < points; i++)
    {
        double angle_rot = angle * i;
        vector_t *outside = malloc(sizeof(vector_t));
        *outside = vec_subtract(outside_first, center);
        *outside = vec_rotate(*outside, angle_rot);
        *outside = vec_add(*outside, center);

        vector_t *inside = malloc(sizeof(vector_t));
        *inside = vec_subtract(inside_first, center);
        *inside = vec_rotate(*inside, angle_rot);
        *inside = vec_add(*inside, center);

        list_add(star_list, outside);
        list_add(star_list, inside);
    }
    body_t *star = body_init(star_list, radius, choose_rand_color());
    body_set_rotation_speed(star, rotate);
    return star;
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    scene_t *scene = scene_init();
    for(size_t i = 0; i < num_stars; i++){
        scene_add_body(scene, make_star());
    }
    for(size_t i = 0; i < num_stars; i++){
        for(size_t j = 0; j < num_stars; j++){
            if(j != i){
                create_newtonian_gravity(scene, gravity, scene_get_body(scene, i), scene_get_body(scene, (j)));
            }
        }
    }
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    return 0;
}
