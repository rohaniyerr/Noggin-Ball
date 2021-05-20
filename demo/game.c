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

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t RIGHT_VELOCITY = {500, 0};
const vector_t LEFT_VELOCITY = {-500, 0};
const vector_t JUMP_VELOCITY = {0, 200};
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};
const vector_t GRAVITY = {0, -100};
const double WALL_ELASTICITY = 0.8;
const double PLAYER_ELASTICITY = 1.2;
const size_t BALL_NUMBER_IN_SCENE = 4;
const double LEG_ROTATION_SPEED = 1.5;
const double PLAYER_RADIUS = 25;
const vector_t PLAYER1_BODY_SPAWN = {200, 32};
const vector_t PLAYER1_LEG_SPAWN = {200 + 25, 15};
const double PLAYER1_ANGLE = 19*M_PI/12;
const vector_t PLAYER2_BODY_SPAWN = {800, 32};
const vector_t PLAYER2_LEG_SPAWN = {800 - 25, 15};
const double PLAYER2_ANGLE = 5*M_PI/12;
const double CIRCLE_POINTS = 40;
const double CIRC_NPOINTS = 40;
const double CIRCLE_MASS = 1;


void on_key_player(char key, key_event_type_t type, void *scene) {
    body_t *body = scene_get_body((scene_t*)scene, 0);
    body_t *leg = scene_get_body((scene_t*)scene, 1);
    if (type == KEY_PRESSED) {
        switch (key) {
            case RIGHT_ARROW:
                body_set_velocity(body, RIGHT_VELOCITY);
                body_set_velocity(leg, RIGHT_VELOCITY);
                break;
            case LEFT_ARROW:
                body_set_velocity(body, LEFT_VELOCITY);
                body_set_velocity(leg, LEFT_VELOCITY);
                break;
            case UP_ARROW:
                body_set_velocity(body, JUMP_VELOCITY);
                body_set_velocity(leg, JUMP_VELOCITY);
                break;
            case SDLK_COMMA:
                body_set_rotation_speed(leg, LEG_ROTATION_SPEED);
        }
    }
    else {
        body_set_velocity(body, VEC_ZERO);
        body_set_velocity(leg, VEC_ZERO);
        if (body_get_rotation(body) <= PLAYER1_ANGLE){ body_set_rotation_speed(leg,-LEG_ROTATION_SPEED); }
        else { body_set_rotation_speed(leg, 0); }
    }

    body_t *body2 = scene_get_body((scene_t*)scene, 2);
    body_t *leg2 = scene_get_body((scene_t*)scene, 3);
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_d:
                body_set_velocity(body2, RIGHT_VELOCITY);
                body_set_velocity(leg2, RIGHT_VELOCITY);
                break;
            case SDLK_a:
                body_set_velocity(body2, LEFT_VELOCITY);
                body_set_velocity(leg2, LEFT_VELOCITY);
                break;
            case SDLK_w:
                body_set_velocity(body2, JUMP_VELOCITY);
                body_set_velocity(leg2, JUMP_VELOCITY);
                break;
            case SDLK_c:
                body_set_rotation_speed(leg2, LEG_ROTATION_SPEED);
        }
    }
    else {
        vector_t NO_X = {.x = 0, .y = body_get_velocity(body2).y};
        body_set_velocity(body2, NO_X);
        body_set_velocity(leg2, NO_X);
        if (body_get_rotation(body2) >= PLAYER2_ANGLE){ body_set_rotation_speed(leg2,-LEG_ROTATION_SPEED); }
        else { body_set_rotation_speed(leg2, 0); }
    }
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
    if (body_get_centroid(body).x + PLAYER_RADIUS + body_get_velocity(body).x * dt >= WINDOW_WIDTH_) {return true;}
    else if (body_get_centroid(body).x - PLAYER_RADIUS + body_get_velocity(body).x * dt <= 0) {return true;}
    return false;
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

body_t *make_ball(scene_t *scene, double radius) {
    list_t *circ = circle_init(radius);
    body_t *ball = body_init(circ, CIRCLE_MASS, choose_rand_color());
    scene_add_body(scene, ball);
    return ball;
}

body_t *make_oval(rgb_color_t *color) {
    list_t *c = list_init(CIRC_NPOINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {PLAYER_RADIUS * cos(angle), 1.5 * PLAYER_RADIUS * sin(angle)};
        list_add(c, v);
    }
    return body_init(c, INFINITY, *color);
}

body_t *make_player_body(scene_t *scene, rgb_color_t *color, vector_t spawn) {
    body_t *player_body = make_oval(color);
    body_set_color(player_body, color);
    body_set_centroid(player_body, spawn);
    scene_add_body(scene, player_body);
    return player_body;
}

body_t *make_player_leg(scene_t *scene, rgb_color_t *color, vector_t spawn, double angle) {
    body_t *leg = make_rectangle(PLAYER_RADIUS, 5, spawn);
    body_set_color(leg, color);
    body_set_rotation(leg, angle);
    scene_add_body(scene, leg);
    return leg;
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    
    rgb_color_t *GREEN = rgb_color_init(0,1,0);
    rgb_color_t *BLUE = rgb_color_init(0,0,1);

    scene_t *scene = scene_init();

    body_t *player1_body = make_player_body(scene, GREEN, PLAYER1_BODY_SPAWN);
    body_t *player1_leg = make_player_leg(scene, GREEN, PLAYER1_LEG_SPAWN, PLAYER1_ANGLE);

    body_t *player2_body = make_player_body(scene, BLUE, PLAYER2_BODY_SPAWN);
    body_t *player2_leg = make_player_leg(scene, BLUE, PLAYER2_LEG_SPAWN, PLAYER2_ANGLE);

    body_t *ball = make_ball(scene, 10);

    sdl_on_key((key_handler_t) on_key_player);
    //add gravity to ball and players
    for (size_t i = 0; i < 5; i++) { 
        create_planet_gravity(scene, GRAVITY, scene_get_body(scene, i));
    }
    //add physics collisions between players and ball
    for (size_t i = 0; i < 4; i++) { 
        create_physics_collision(scene, PLAYER_ELASTICITY, scene_get_body(scene, BALL_NUMBER_IN_SCENE), scene_get_body(scene, i));
    }
    //add physics collisions between the ball and the walls
    //add physics collisions between players and walls
    for (size_t i = 5; i < scene_bodies(scene); i++) { 
        create_physics_collision(scene, WALL_ELASTICITY, scene_get_body(scene, BALL_NUMBER_IN_SCENE), scene_get_body(scene, i));
        for (size_t j = 0; j < 4; j++) {
            create_physics_collision(scene, PLAYER_ELASTICITY, scene_get_body(scene, j), scene_get_body(scene, i));
        }
    }

    double time = 0;
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
        time += dt;
    }
    scene_free(scene);
    return 0;
}
