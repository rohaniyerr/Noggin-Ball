#include "sdl_wrapper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "forces.h"
#include "polygon.h"
#include "SDL2/SDL.h"

#define WINDOW_TITLE "CS 3"
#define CIRC_NPOINTS 20
 
const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
const vector_t PLAYER_SPAWN = {500, -10};
const vector_t BALL_SPAWN = {500, 250};
const vector_t RIGHT_VELOCITY = {1000, 0};
const vector_t LEFT_VELOCITY = {-1000, 0};
const rgb_color_t RECT_COLOR = {1,0,1};
const int BRICK_LENGTH = 45;
const int BRICK_HEIGHT = 20;
const double BALL_RADIUS = 10.0;
const int PLAYER_LENGTH = 40;
const int PLAYER_HEIGHT = 20;
const int Y_SPACE_BETWEEN = 25;
const double X_SPACE_BETWEEN = 4.3;
const int ROWS = 3;
const int COLS = 10;
const double INITIAL_INVADER_MASS = 1;
const double CIRCLE_MASS = 1;
const double EDGE_DAMPING = -0.5;
const int CIRCLE_POINTS = 20;
const double VELOCITY = 500;
const double ELASTICITY = 1;


list_t *color_list() {
    list_t *colors = list_init(10, (free_func_t)free);
    rgb_color_t *RED = rgb_color_init(1, 0, 0);
    list_add(colors, RED);
    rgb_color_t *ORANGE = rgb_color_init(1, .5, 0);
    list_add(colors, ORANGE);
    rgb_color_t *YELLOW = rgb_color_init(1, 1, 0);
    list_add(colors, YELLOW);
    rgb_color_t *GREEN = rgb_color_init(0, 1, 0);
    list_add(colors, GREEN);
    rgb_color_t *SPRING_GREEN = rgb_color_init(0, .75, .25);
    list_add(colors, SPRING_GREEN);
    rgb_color_t *CYAN = rgb_color_init(0, 1, 1);
    list_add(colors, CYAN);
    rgb_color_t *AZURE = rgb_color_init(0, .25, .75);
    list_add(colors, AZURE);
    rgb_color_t *BLUE = rgb_color_init(0, 0, 1);
    list_add(colors, BLUE);
    rgb_color_t *VIOLET = rgb_color_init(.25, 0, .75);
    list_add(colors, VIOLET);
    rgb_color_t *MAGENTA = rgb_color_init(.5, 0, .5);
    list_add(colors, MAGENTA);
    return colors;
}

body_t *make_rectangle(int length, int height, vector_t spawn){
    list_t *rectangle = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {-length, -height};
    list_add(rectangle, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {+length, -height};
    list_add(rectangle, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {+length, +height};
    list_add(rectangle, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {-length, +height};
    list_add(rectangle, v);
    body_t *body = body_init(rectangle, 1, choose_rand_color());
    body_set_centroid(body, spawn);
    body_set_mass(body, INFINITY);
    return body;
}

bool hits_edge_player(body_t *body, double dt) {
    if (body_get_centroid(body).x + PLAYER_LENGTH + body_get_velocity(body).x * dt >= WINDOW_WIDTH_) {return true;}
    else if (body_get_centroid(body).x - PLAYER_LENGTH + body_get_velocity(body).x * dt <= 0) {return true;}
    return false;
}

void on_key(char key, key_event_type_t type, double held_time, void *scene) {
    body_t *block = scene_get_body((scene_t*)scene, 0);
    if (type == KEY_PRESSED) {
        switch (key) {
            case RIGHT_ARROW:
                body_set_velocity(block, RIGHT_VELOCITY);
                break;
            case LEFT_ARROW:
                body_set_velocity(block, LEFT_VELOCITY);
                break;
        }
    }
    else {
        body_set_velocity(block, VEC_ZERO);
    }
}

void make_walls(scene_t *scene) {
    int size = 1;
    vector_t spawn1 = {-1, WINDOW_HEIGHT_/2};
    body_t *wall1 = make_rectangle(size, WINDOW_HEIGHT_, spawn1);
    body_set_mass(wall1, INFINITY);
    scene_add_body(scene, wall1);

    vector_t spawn2 = {1001, WINDOW_HEIGHT_/2};
    body_t *wall2 = make_rectangle(size, WINDOW_HEIGHT_, spawn2);
    body_set_mass(wall2, INFINITY);
    scene_add_body(scene, wall2);

    vector_t spawn3 = {WINDOW_WIDTH_/2, 501};
    body_t *wall3 = make_rectangle(WINDOW_WIDTH_, size, spawn3);
    body_set_mass(wall3, INFINITY);
    scene_add_body(scene, wall3);
}

void make_bricks(scene_t *scene, list_t *color) {
    double y_position = WINDOW_HEIGHT_ - BRICK_HEIGHT;
    double x_position = BRICK_LENGTH + 10;
    double shift_down = BRICK_HEIGHT + Y_SPACE_BETWEEN;
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            body_t *brick = make_rectangle(BRICK_LENGTH, BRICK_HEIGHT, (vector_t) {x_position + 2 * (BRICK_LENGTH+X_SPACE_BETWEEN) * j, y_position});
            body_set_color(brick, (rgb_color_t *) list_get(color, j));
            scene_add_body(scene, brick);
        }
        y_position -= shift_down;
    }
}

list_t *circle_init(double radius) {
    list_t *circle = list_init(CIRCLE_POINTS, free);
    double arc_angle = 2 * M_PI / CIRCLE_POINTS;
    vector_t point = {.x = radius, .y = 0.0};
    for (size_t i = 0; i < CIRCLE_POINTS; i++) {
        vector_t *v = malloc(sizeof(*v));
        *v = point;
        list_add(circle, v);
        point = vec_rotate(point, arc_angle);
    }
    return circle;
}

body_t *make_ball(scene_t *scene) {
    list_t *circ = circle_init(BALL_RADIUS);
    body_t *ball = body_init(circ, CIRCLE_MASS, choose_rand_color());
    body_set_centroid(ball, BALL_SPAWN);
    double direction = M_PI / 6 * (rand() % 10);
    body_set_velocity(ball, (vector_t) {VELOCITY * cos(direction), VELOCITY * fabs(sin(direction)) + 50});
    scene_add_body(scene, ball);
    return ball;
}

bool ball_off_screen(scene_t *scene, body_t *ball){
    if(body_get_centroid(ball).y < 0){
        body_remove(ball);
        return true;
    }
    return false;
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    list_t *colors = color_list();
    scene_t *scene = scene_init();
    body_t *player = make_rectangle(PLAYER_LENGTH, PLAYER_HEIGHT, PLAYER_SPAWN);
    
    scene_add_body(scene, player);
    make_walls(scene);
    make_bricks(scene, colors);
    body_t *ball = make_ball(scene);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        create_physics_collision(scene, ELASTICITY, ball, scene_get_body(scene, i));
        create_color_changer(scene, colors, scene_get_body(scene, i), ball);
    }
    sdl_on_key((key_handler_t) on_key);
    double time = 0;
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        time += dt;
        if (hits_edge_player(scene_get_body(scene, 0), dt)) { body_set_velocity(scene_get_body(scene, 0), vec_multiply(EDGE_DAMPING, body_get_velocity(scene_get_body(scene, 0)))); }
        if (ball_off_screen(scene, ball)) {
            for (size_t i = 1; i < scene_bodies(scene); i++) {
                scene_remove_body(scene, i);
            }
            make_walls(scene);
            make_bricks(scene, colors);
            ball = make_ball(scene);
            for (size_t i = 0; i < scene_bodies(scene); i++) {
                create_physics_collision(scene, ELASTICITY, ball, scene_get_body(scene, i));
                create_color_changer(scene, colors, scene_get_body(scene, i), ball);
            }
        }
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    list_free(colors);
    scene_free(scene);
    return 0;
}