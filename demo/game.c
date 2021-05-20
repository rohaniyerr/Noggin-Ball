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
const double PLAYER1_ANGLE = 5*M_PI/3;
const double PLAYER2_ANGLE = M_PI/3;


void on_key_player1(char key, key_event_type_t type, void *scene) {
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
}

void on_key_player2(char key, key_event_type_t type, void *scene) {
    body_t *body = scene_get_body((scene_t*)scene, 2);
    body_t *leg = scene_get_body((scene_t*)scene, 3);
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
        if (body_get_rotation(body) >= PLAYER2_ANGLE){ body_set_rotation_speed(leg,-LEG_ROTATION_SPEED); }
        else { body_set_rotation_speed(leg, 0); }
    }
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    
    sdl_on_key((key_handler_t) on_key_player1);
    sdl_on_key((key_handler_t) on_key_player2);

    scene_t *scene = scene_init();
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
