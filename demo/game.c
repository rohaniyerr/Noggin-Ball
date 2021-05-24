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
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};

const double RIGHT_VELOCITY = 200;
const double LEFT_VELOCITY = -200;
const double JUMP_VELOCITY = 200;

const vector_t GRAVITY = {0, -500};
const double WALL_ELASTICITY = 0.2;
const double PLAYER_ELASTICITY = 0.4;
const size_t BALL_NUMBER_IN_SCENE = 4;
const size_t FLOOR_NUMBER = 8;

const double LEG_ROTATION_SPEED = 5;
const double PLAYER_RADIUS = 25;
const double PLAYER1_ANGLE = 19*M_PI/12;
const double PLAYER2_ANGLE = 5*M_PI/12;
const vector_t PLAYER1_BODY_SPAWN = {200, 88};
const vector_t PLAYER2_BODY_SPAWN = {800, 88};
const vector_t PLAYER1_LEG_SPAWN = {200, 35};
const vector_t PLAYER2_LEG_SPAWN = {800, 35};
const double PLAYER_MAJOR_AXIS = 1;
const double PLAYER_MINOR_AXIS = 1.5;
const vector_t PLAYER1_LEG_TOP_RIGHT = {200, 30};
const vector_t PLAYER2_LEG_TOP_RIGHT = {800, 30};
const size_t LEG_SCALING = 3;

const double CIRCLE_POINTS = 40;
const double CIRCLE_MASS = 1;
const double BALL_RADIUS = 15;
const vector_t BALL_SPAWN = {WINDOW_WIDTH_/2 , WINDOW_HEIGHT_/2};


void on_key_player(char key, key_event_type_t type, void *scene) {
    body_t *body1 = scene_get_body((scene_t*)scene, 2);
    body_t *leg1 = scene_get_body((scene_t*)scene, 3);
    body_set_rotation_speed(leg1, 0);
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                if(true == true) {
                    vector_t v = {.x = LEFT_VELOCITY, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case RIGHT_ARROW:
                if(true == true) {
                    vector_t v = {RIGHT_VELOCITY, body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case UP_ARROW:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body1), body_get_shape(scene_get_body(scene, 8)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body1).x, .y = JUMP_VELOCITY};
                        body_set_velocity(body1, v);
                        body_set_velocity(leg1, v);
                    }
                }
            case SDLK_COMMA:
                body_set_rotation_speed(leg1, -LEG_ROTATION_SPEED);
        }
    }
    else if (type == KEY_RELEASED) {
        switch (key) {
            case SDLK_COMMA:
                body_set_rotation_speed(leg1, 0);
                body_set_rotation(leg1, PLAYER1_ANGLE);
            case LEFT_ARROW:
                if (true == true) {
                    vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, NO_X2);
                    body_set_velocity(leg1, NO_X2);
                    break;
                }
            case RIGHT_ARROW:
                if (true == true) {
                    vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, NO_X2);
                    body_set_velocity(leg1, NO_X2);
                    break;
                }

        }
    }

    body_t *body2 = scene_get_body((scene_t*)scene, 0);
    body_t *leg2 = scene_get_body((scene_t*)scene, 1);
    body_set_rotation_speed(leg2, 0);
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_d:
                if(true == true) {
                    vector_t v = {.x = RIGHT_VELOCITY, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case SDLK_a:
                if(true == true) {
                    vector_t v = {.x = LEFT_VELOCITY, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case SDLK_w:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body2), body_get_shape(scene_get_body(scene, 8)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body2).x, .y = JUMP_VELOCITY};
                        body_set_velocity(body2, v);
                        body_set_velocity(leg2, v);
                    }
                    break;
                }
            case SDLK_c:
                body_set_rotation_speed(leg2, -LEG_ROTATION_SPEED);
        }
    }
    else if (type == KEY_RELEASED) {
        switch (key) {
            case SDLK_c:
                body_set_rotation_speed(leg2, 0);
                body_set_rotation(leg2, PLAYER2_ANGLE);
            case SDLK_a:
                if (true == true) {
                    vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, NO_X2);
                    body_set_velocity(leg2, NO_X2);
                    break;
                }
            case SDLK_d:
                if (true == true) {
                    vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, NO_X2);
                    body_set_velocity(leg2, NO_X2);
                    break;
                }

        }
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
    body_set_mass(body, 1);
    return body;
}

void make_walls(scene_t *scene) {
    int size = 10;
    vector_t spawn1 = {5, 2*WINDOW_HEIGHT_/3};
    body_t *wall1 = make_rectangle(size, 3*WINDOW_HEIGHT_/4, spawn1);
    body_set_mass(wall1, INFINITY);
    rgb_color_t *BLACK = rgb_color_init(0,0,0);
    body_set_color(wall1, BLACK);
    scene_add_body(scene, wall1);

    vector_t spawn2 = {995, WINDOW_HEIGHT_/2};
    body_t *wall2 = make_rectangle(size, WINDOW_HEIGHT_, spawn2);
    body_set_mass(wall2, INFINITY);
    body_set_color(wall2, BLACK);
    scene_add_body(scene, wall2);

    vector_t spawn3 = {WINDOW_WIDTH_/2, 495};
    body_t *wall3 = make_rectangle(WINDOW_WIDTH_, size, spawn3);
    body_set_mass(wall3, INFINITY);
    body_set_color(wall3, BLACK);
    scene_add_body(scene, wall3);

    vector_t spawn4 = {WINDOW_WIDTH_/2, 5};
    body_t *wall4 = make_rectangle(WINDOW_WIDTH_, size, spawn4);
    body_set_mass(wall4, INFINITY);
    body_set_color(wall4, BLACK);
    scene_add_body(scene, wall4);
}

void make_goals(scene_t *scene) {
    rgb_color_t *BROWN = rgb_color_init(.6,.15,.15);

    vector_t spawn_vert_1 = {25, WINDOW_HEIGHT_/8 - 22}; 
    body_t *vert_goal1 = make_rectangle(10, WINDOW_HEIGHT_/4 - 20, spawn_vert_1);
    body_set_color(vert_goal1, BROWN);
    body_set_mass(vert_goal1, INFINITY);
    scene_add_body(scene, vert_goal1);
    
    vector_t spawn_hor_1 = {60, WINDOW_HEIGHT_/4 + 10};
    body_t *hor_goal1 = make_rectangle(25, 10, spawn_hor_1);
    body_set_color(hor_goal1, BROWN);
    body_set_mass(hor_goal1, INFINITY);
    scene_add_body(scene, hor_goal1);

    vector_t spawn_vert_2 = {975, WINDOW_HEIGHT_/8 - 22}; 
    body_t *vert_goal2 = make_rectangle(10, WINDOW_HEIGHT_/4 - 20, spawn_vert_2);
    body_set_color(vert_goal2, BROWN);
    body_set_mass(vert_goal2, INFINITY);
    scene_add_body(scene, vert_goal2);
    
    vector_t spawn_hor_2 = {940, WINDOW_HEIGHT_/4 + 10};
    body_t *hor_goal2 = make_rectangle(25, 10, spawn_hor_2);
    body_set_color(hor_goal2, BROWN);
    body_set_mass(hor_goal2, INFINITY);
    scene_add_body(scene, hor_goal2);
}

bool hits_edge_player(body_t *body, double dt) {
    if (body_get_centroid(body).x + PLAYER_RADIUS + body_get_velocity(body).x * dt >= WINDOW_WIDTH_) {return true;}
    else if (body_get_centroid(body).x - PLAYER_RADIUS + body_get_velocity(body).x * dt <= 0) {return true;}
    return false;
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
    body_set_centroid(ball, BALL_SPAWN);
    body_set_mass(ball, 1);
    scene_add_body(scene, ball);
    return ball;
}

body_t *make_oval(rgb_color_t *color, double radius, double x_scalar, double y_scalar) {
    list_t *c = list_init(CIRCLE_POINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRCLE_POINTS; i++) {
        double angle = 2 * M_PI * i / CIRCLE_POINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {x_scalar * radius * cos(angle), y_scalar * radius * sin(angle)};
        list_add(c, v);
    }
    return body_init(c, INFINITY, *color);
}

body_t *make_p2_leg(scene_t *scene, rgb_color_t *color, vector_t spawn, vector_t top_right){
    
    list_t *leg = list_init(5, free);
    
    vector_t *v = malloc(sizeof(*v));
    double curr_x = top_right.x;
    double curr_y = top_right.y;
    v->x = top_right.x;
    v->y = top_right.y;

    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x -= LEG_SCALING * 5;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_y -= LEG_SCALING * 7;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x -= LEG_SCALING * 5;
    curr_y -= LEG_SCALING * 3;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x += LEG_SCALING * 10;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    body_t *body = body_init(leg, 1, *color);
    body_set_rotation(body, PLAYER2_ANGLE);
    body_set_centroid(body, spawn);
    scene_add_body(scene, body);
    return body;
}

body_t *make_p1_leg(scene_t *scene, rgb_color_t *color, vector_t spawn, vector_t top_left){
    
    list_t *leg = list_init(5, free);
    
    vector_t *v = malloc(sizeof(*v));
    double curr_x = top_left.x;
    double curr_y = top_left.y;
    v->x = top_left.x;
    v->y = top_left.y;

    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x += LEG_SCALING * 5;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_y -= LEG_SCALING * 7;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x += LEG_SCALING * 5;
    curr_y -= LEG_SCALING * 3;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x -= LEG_SCALING * 10;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);
    
    body_t *body = body_init(leg, 1, *color);
    body_set_rotation(body, PLAYER1_ANGLE);
    body_set_centroid(body, spawn);
    scene_add_body(scene, body);
    return body;
}

body_t *make_player_body(scene_t *scene, rgb_color_t *color, vector_t spawn) {
    body_t *player_body = make_oval(color, PLAYER_RADIUS, PLAYER_MAJOR_AXIS, PLAYER_MINOR_AXIS);
    body_set_color(player_body, color);
    body_set_centroid(player_body, spawn);
    body_set_mass(player_body, 1);
    scene_add_body(scene, player_body);
    return player_body;
}

void check_edge(scene_t *scene) {
    for (size_t i = 0; i < 5; i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t centroid = body_get_centroid(body);
        if (centroid.y <= 32) {
            vector_t new_centroid = {.x = centroid.x, .y = 50};
            body_set_centroid(body, new_centroid);
        }
    }
    body_t *ball = scene_get_body(scene, 4);
    vector_t centroid = body_get_centroid(ball);
    if (centroid.x <= 15) {
        vector_t new_centroid = {.x = 50, .y = centroid.y};
        body_set_centroid(ball, new_centroid);
    }
    else if (centroid.x >= 1015) {
        vector_t new_centroid = {.x = 950, .y = centroid.y};
        body_set_centroid(ball, new_centroid);
    }
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    
    rgb_color_t *GREEN = rgb_color_init(0,1,0);
    rgb_color_t *BLUE = rgb_color_init(0,0,1);
    rgb_color_t *BLACK = rgb_color_init(0,0,0);

    scene_t *scene = scene_init();

    make_player_body(scene, GREEN, PLAYER1_BODY_SPAWN); //0
    make_p1_leg(scene, BLACK, PLAYER1_LEG_SPAWN, PLAYER1_LEG_TOP_RIGHT);
    make_player_body(scene, BLUE, PLAYER2_BODY_SPAWN); //2
    make_p2_leg(scene, BLACK, PLAYER2_LEG_SPAWN, PLAYER2_LEG_TOP_RIGHT);

    make_ball(scene, BALL_RADIUS); //4

    make_walls(scene); //5,6,7,8
    make_goals(scene); //9,10,11,12

    sdl_on_key((key_handler_t) on_key_player);

    //add gravity to ball and players
    for (size_t i = 0; i < 5; i++) { 
        create_planet_gravity(scene, GRAVITY, scene_get_body(scene, i));
        create_normal_force(scene, scene_get_body(scene, i), scene_get_body(scene, FLOOR_NUMBER));
    }
    //add physics collisions between players and ball
    for (size_t i = 0; i < 4; i++) { 
        create_physics_one_collision(scene, PLAYER_ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, BALL_NUMBER_IN_SCENE));
    }
    //add physics collisions between the ball and the walls
    //add physics collisions between players and walls
    for (size_t i = 5; i < scene_bodies(scene); i++) { 
        create_physics_collision(scene, WALL_ELASTICITY, scene_get_body(scene, BALL_NUMBER_IN_SCENE), scene_get_body(scene, i));
        for (size_t j = 0; j < 4; j++) {
            create_physics_collision(scene, WALL_ELASTICITY, scene_get_body(scene, j), scene_get_body(scene, i));
        }
    }

    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        check_edge(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    return 0;
}
