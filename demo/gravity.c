#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"

#define WINDOW_TITLE "CS 3"

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};

const vector_t GRAVITY = {0, -600};
const size_t X_VELOCITY = 200;
const size_t Y_VELOCITY = 100;

const double BOUNCE_FACTOR = 0.8;
const double SPAWN_RATE = 1.2;
const double ROTATION = 2;
const int RADIUS = 50;


list_t *make_star(int points, int radius, vector_t center)
{
    list_t *star = list_init(2 * points, (free_func_t) free);
    double angle = 2 * M_PI / points;

    //find the first point on the outside (point above centroid)
    vector_t outside_first = {.x = center.x, .y = center.y + radius};

    //find the first point on the inside (point to right of centroid)
    double inner_angle_start = (M_PI / points) + (M_PI / 2);
    double inner_radius = RADIUS / 2;

    vector_t inside_first = {.x = inner_radius * cos(inner_angle_start), .y = inner_radius * sin(inner_angle_start)};
    inside_first = vec_add(inside_first, center);

    //given the first point, add each vector rotated into star
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

        list_add(star, outside);
        list_add(star, inside);
    }
    return star;
}



vector_t update_velocity(double dt, double radius, body_t *body) {
    vector_t velocity = body_get_velocity(body);
    vector_t center = body_get_centroid(body);
    velocity = vec_add(velocity, vec_multiply(dt, GRAVITY));
    vector_t dxy = vec_multiply(dt, velocity);
    double dy = dxy.y;
    if (dy > 0 && center.y + dy + radius >= WINDOW_HEIGHT_)
    {
        velocity.y *= -1 * BOUNCE_FACTOR;
    }
    else if (dy < 0 && center.y + dy - radius <= 0)
    {
        velocity.y *= -1 * BOUNCE_FACTOR;
    }
    return velocity;
}

int main() {
    double time = 0;
    int points = 2; //initial numbers of the points for the first polygon
    int num_stars = 0;
    int max_stars = WINDOW_WIDTH_ / X_VELOCITY / SPAWN_RATE;
    vector_t initial_velocity = {.x = X_VELOCITY, .y = Y_VELOCITY};
    vector_t spawn = {.x = RADIUS, .y = WINDOW_HEIGHT_ - RADIUS};
    sdl_init(MIN_POINT, MAX_POINT);
    list_t *stars = list_init(max_stars, (free_func_t) body_free);
    scene_t *scene = scene_init();
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        time += dt;
        if (time > SPAWN_RATE || num_stars == 0) {
            body_t *star = body_init(make_star(points, RADIUS, spawn), 1, choose_rand_color());
            body_set_rotation_speed(star, ROTATION);
            body_set_velocity(star, initial_velocity);
            body_set_centroid(star, spawn);
            time = 0;
            scene_add_body(scene, star);
            points++;
            num_stars++;
        }
        for (size_t i = 0; i < num_stars; i++) {
            body_t *star_i = scene_get_body(scene, i);
            if (body_get_centroid(star_i).x - RADIUS > WINDOW_WIDTH_) {
                scene_remove_body(scene, 0);
                num_stars--;
                assert(num_stars >= 0);
            }
            else {
                body_set_velocity(star_i, update_velocity(dt, RADIUS, star_i));
                body_tick(star_i, dt);
            }
        }
        sdl_render_scene(scene);
    }
    list_free(stars);
    scene_free(scene);
    return 0;
}
