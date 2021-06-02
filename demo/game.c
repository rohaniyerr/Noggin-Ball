#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "forces.h"
#include "polygon.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_wrapper.h"
#include "player.h"

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};

const double RIGHT_VELOCITY = 200;
const double LEFT_VELOCITY = -200;
const double JUMP_VELOCITY = 200;
const double VELOCITY = 200;

const double GAMMA = 2;
const vector_t GRAVITY = {0, -200};
const vector_t PLAYER_GRAVITY = {0, -500};
const double WALL_ELASTICITY = 0.15;
const double PLAYER_ELASTICITY = 0.4;
const size_t BALL_NUMBER_IN_SCENE = 4;
const size_t FLOOR_NUMBER = 8;

const double LEG_ROTATION_SPEED = 8;
const double PLAYER_RADIUS = 45;
const double PLAYER1_ANGLE = -M_PI/5;
const double PLAYER2_ANGLE = M_PI/5;
const vector_t PLAYER1_BODY_SPAWN = {200, 70};
const vector_t PLAYER2_BODY_SPAWN = {800, 70};
const vector_t PLAYER1_LEG_SPAWN = {240, 70};
const vector_t PLAYER2_LEG_SPAWN = {760, 70};
const double PLAYER_MAJOR_AXIS = 0.7;
const double PLAYER_MINOR_AXIS = 0.9;
const vector_t PLAYER1_LEG_TOP_LEFT = {215, 92};
const vector_t PLAYER2_LEG_TOP_RIGHT = {800, 30};
const double LEG_SCALING = 3.5;

const double CIRCLE_POINTS = 40;
const double CIRCLE_MASS = 1;
const double BALL_RADIUS = 25;
const vector_t BALL_SPAWN = {WINDOW_WIDTH_/2 , 400};
double BALL_MAX_VELOCITY = 1000;

player_t *make_ding(size_t player_idx) {
    player_init(250, 150);
}

player_t *make_riiyer(size_t player_idx) {
    player_init(200, 200);
}

player_t *make_dzli(size_t player_idx) {
    player_init(150, 250);
}

player_t *make_eseiner(size_t player_idx) {
    player_init(300, 300);
}

void on_key_player(char key, key_event_type_t type, void *scene) {
    body_t *body1 = scene_get_body((scene_t*)scene, 2);
    body_t *leg1 = scene_get_body((scene_t*)scene, 3);
    size_t p2_idx = scene_get_player2((scene_t *)scene);
    double p2_speed = player_get_speed(p2);
    double p2_jump = player_get_jump(p2);
    body_set_rotation_speed(leg1, 0);
    body_set_rotation(leg1, M_PI/5);
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                if(true == true) {
                    vector_t v = {.x = -p2_speed, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case RIGHT_ARROW:
                if(true == true) {
                    vector_t v = {p2_speed, body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case UP_ARROW:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body1), body_get_shape(scene_get_body(scene, 8)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body1).x, .y = p2_jump};
                        body_set_velocity(body1, v);
                        body_set_velocity(leg1, v);
                    }
                    break;
                }
            case SDLK_COMMA:
                if (true == true) {
                    body_set_rotation(leg1, -M_PI / 3);
                    break;
                }
        }
    }
    else {
        vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body1).y};
        body_set_velocity(body1, NO_X2);
        body_set_velocity(leg1, NO_X2);
    }
    player_t *p1 = scene_get_player1((scene_t *)scene);
    double p1_speed = player_get_speed(p1);
    double p1_jump = player_get_jump(p1);
    body_t *body2 = scene_get_body((scene_t*)scene, 0);
    body_t *leg2 = scene_get_body((scene_t*)scene, 1);
    body_set_rotation_speed(leg2, 0);
    body_set_rotation(leg2, - M_PI / 5);    
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_d:
                if(true == true) {
                    vector_t v = {.x = p1_speed, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case SDLK_a:
                if(true == true) {
                    vector_t v = {.x = -p1_speed, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case SDLK_w:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body2), body_get_shape(scene_get_body(scene, 8)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body2).x, .y = p1_jump};
                        body_set_velocity(body2, v);
                        body_set_velocity(leg2, v);
                    }
                    break;
                }
            case SDLK_c:
                if (true == true){
                    body_set_rotation(leg2, M_PI / 3);
                    break;
                }
        }
    }
    else {
        vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body2).y};
        body_set_velocity(body2, NO_X2);
        body_set_velocity(leg2, NO_X2);
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
    int size = 5;
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
    rgb_color_t *BROWN = rgb_color_init(1,1,1);

    vector_t spawn_vert_1 = {20, WINDOW_HEIGHT_/8 - 22}; 
    body_t *vert_goal1 = make_rectangle(10, WINDOW_HEIGHT_/4 - 20, spawn_vert_1);
    body_set_color(vert_goal1, BROWN);
    body_set_mass(vert_goal1, INFINITY);
    body_set_rotation(hor_goal1, -M_PI / 69);
    scene_add_body(scene, vert_goal1);
    
    vector_t spawn_hor_1 = {55, WINDOW_HEIGHT_/4 + 10};
    body_t *hor_goal1 = make_rectangle(25, 10, spawn_hor_1);
    body_set_color(hor_goal1, BROWN);
    body_set_mass(hor_goal1, INFINITY);
    scene_add_body(scene, hor_goal1);

    vector_t spawn_vert_2 = {980, WINDOW_HEIGHT_/8 - 22}; 
    body_t *vert_goal2 = make_rectangle(10, WINDOW_HEIGHT_/4 - 20, spawn_vert_2);
    body_set_color(vert_goal2, BROWN);
    body_set_mass(vert_goal2, INFINITY);
    scene_add_body(scene, vert_goal2);
    
    vector_t spawn_hor_2 = {945, WINDOW_HEIGHT_/4 + 10};
    body_t *hor_goal2 = make_rectangle(25, 10, spawn_hor_2);
    body_set_color(hor_goal2, BROWN);
    body_set_mass(hor_goal2, INFINITY);
    body_set_rotation(hor_goal1, M_PI / 69);
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
    body_set_radius(ball, BALL_RADIUS);
    body_set_image(ball, "images/ball.png");
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
    curr_x -= LEG_SCALING * 4;
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
    // body_set_radius(body, 25);
    // body_set_image(body, "images/leg2.png");
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
    curr_y -= LEG_SCALING * 10;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);
    
    v = malloc(sizeof(*v));
    curr_x += LEG_SCALING * 9;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    v = malloc(sizeof(*v));
    curr_x -= LEG_SCALING * 4;
    curr_y += LEG_SCALING * 3;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    
    v = malloc(sizeof(*v));
    curr_y += LEG_SCALING * 7;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    
    
    body_t *body = body_init(leg, 1, *color);
    body_set_rotation(body, PLAYER1_ANGLE);
    body_set_centroid(body, spawn);
    // body_set_radius(body, 25);
    // body_set_image(body, "images/leg1.png");
    scene_add_body(scene, body);
    return body;
}

body_t *make_player_body(scene_t *scene, rgb_color_t *color, vector_t spawn, char *filename) {
    body_t *player_body = make_oval(color, PLAYER_RADIUS, PLAYER_MAJOR_AXIS, PLAYER_MINOR_AXIS);
    body_set_color(player_body, color);
    body_set_centroid(player_body, spawn);
    body_set_mass(player_body, 1);
    body_set_radius(player_body, PLAYER_RADIUS);
    body_set_image(player_body, filename);
    scene_add_body(scene, player_body);
    return player_body;
}

void check_edge(scene_t *scene) {
    body_t *ball = scene_get_body(scene, 4);
    vector_t centroid = body_get_centroid(ball);
    if (centroid.y <= 30) {
        vector_t new_centroid = {.x = centroid.x, .y = 33};
        vector_t velocity = {.x = body_get_velocity(ball).x, .y = fabs(body_get_velocity(ball).y)/2};
        body_set_velocity(ball, velocity);
        body_set_centroid(ball, new_centroid);
    }
    for (size_t i = 0; i < 5; i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t c = body_get_centroid(body);
        if (c.x <= 15) {
            vector_t new_centroid = {.x = 20, .y = c.y};
            body_set_centroid(body, new_centroid);
        }
        else if (c.x >= 1015) {
            vector_t new_centroid = {.x = 1010, .y = c.y};
            body_set_centroid(body, new_centroid);
        }
    }
}

void stop_bkg_sound(scene_t *scene){
    Mix_CloseAudio();
    Mix_FreeMusic(scene_get_bkg_sound(scene));
}

bool check_goal(body_t *ball, player_t *p1, player_t *p2) {
    double y_lim = (WINDOW_HEIGHT_/8 - 22) + (WINDOW_HEIGHT_/4 - 20) - 2 * 10;
    double x_left = 60 + 25;
    double x_right = 940 - 25;
    vector_t centroid = body_get_centroid(ball);
    if (centroid.y + BALL_RADIUS <= y_lim) {
        if (centroid.x + BALL_RADIUS <= x_left) {
            p2->score++;
            return true;
        }
        if (centroid.x - BALL_RADIUS >= x_right) {
            p1->score++;
            return true;
        }
    }
    return false;
}

void play_goal_sound(){
    Mix_Chunk *goal = Mix_LoadWAV("sounds/ronaldo.wav");
    Mix_PlayChannel(1, goal, 0);
}

void channel_done(int channel){
    Mix_FreeChunk(Mix_GetChunk(channel));
}


void reset_scene(scene_t *scene, player_t *player1, player_t *player2){
    body_t *p1_body = player_get_body(player1);
    body_t *p1_leg = player_get_leg(player1);
    body_t *p2_body = player_get_body(player2);
    body_t *p2_leg = player_get_leg(player2);
    body_t *ball = scene_get_body(scene, BALL_NUMBER_IN_SCENE);
    if (check_goal(ball, player1, player2)) {
        play_goal_sound();
        Mix_ChannelFinished(channel_done);
        body_set_centroid(p1_body, PLAYER1_BODY_SPAWN);
        body_set_centroid(p1_leg, PLAYER1_LEG_SPAWN);
        body_set_centroid(p2_body, PLAYER2_BODY_SPAWN);
        body_set_centroid(p2_leg, PLAYER2_LEG_SPAWN);
        body_set_centroid(ball, BALL_SPAWN);
        body_set_velocity(ball, VEC_ZERO);
    }
}

void ball_too_fast(body_t *body) {
    if (vec_magnitude(body_get_velocity(body)) > BALL_MAX_VELOCITY) {
        body_set_velocity(body, vec_multiply(.5, body_get_velocity(body)));
    }
}

void make_forces(scene_t *scene) {
    body_t *ball = scene_get_body(scene, BALL_NUMBER_IN_SCENE);
    //add drag to ball so it doesn't accelerate
    create_drag(scene, GAMMA, ball);
    //add gravity to ball and players
    for (size_t i = 0; i < 5; i++) { 
        if (i == 4) {create_planet_gravity(scene, GRAVITY, scene_get_body(scene, i)); }
        else {create_planet_gravity(scene, PLAYER_GRAVITY, scene_get_body(scene, i)); }
        create_normal_force(scene, scene_get_body(scene, i), scene_get_body(scene, FLOOR_NUMBER));
    }
    //add physics collisions between players and ball
    for (size_t i = 0; i < 4; i++) { 
        create_physics_one_collision(scene, PLAYER_ELASTICITY, scene_get_body(scene, i), ball);
    }
    //add physics collisions between the ball and the walls
    //add physics collisions between players and walls
    for (size_t i = 5; i < scene_bodies(scene); i++) { 
        create_physics_one_collision(scene, WALL_ELASTICITY, scene_get_body(scene, i), ball);
        for (size_t j = 0; j < 4; j++) {
            if (j % 2 == 0 || i == FLOOR_NUMBER) {create_physics_collision(scene, WALL_ELASTICITY, scene_get_body(scene, j), scene_get_body(scene, i)); }
        }
    }
}

void make_shapes(scene_t *soccer_scene) {
    rgb_color_t *GREEN = rgb_color_init(0,1,0);
    rgb_color_t *BLUE = rgb_color_init(0,0,1);
    rgb_color_t *BLACK = rgb_color_init(0,0,0);

    make_player_body(soccer_scene, GREEN, PLAYER1_BODY_SPAWN, "images/riiyer.png"); //0
    make_p1_leg(soccer_scene, BLACK, PLAYER1_LEG_SPAWN, PLAYER1_LEG_TOP_LEFT); //1
    make_player_body(soccer_scene, BLUE, PLAYER2_BODY_SPAWN, "images/ding.png"); //2
    make_p2_leg(soccer_scene, BLACK, PLAYER2_LEG_SPAWN, PLAYER2_LEG_TOP_RIGHT); //3

    make_ball(soccer_scene, BALL_RADIUS); //4

    make_walls(soccer_scene); //5,6,7,8
    make_goals(soccer_scene); //9,10,11,12
}


void on_key_title(char key, key_event_type_t type, void *scene) {
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_0:
                if (true == true) {
                    scene_set_info(scene, 'c');
                    break;
                }
            case SDLK_SPACE:
                if (true == true) {
                    scene_set_info(scene, 't');
                    break;
                }
        }
    }
}

void on_key_char(char key, key_event_type_t type, void *scene) {
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_q:
                scene_set_info(scene, 'g');
            case SDLK_1:
                scene_set_player1(scene, 1);
            case SDLK_2:
                scene_set_player1(scene, 2);
            case SDLK_3:
                scene_set_player1(scene, 3);
            case SDLK_4:
                scene_set_player1(scene, 4);
            case SDLK_5:
                scene_set_player2(scene, 1);
            case SDLK_6:
                scene_set_player2(scene, 2);
            case SDLK_7:
                scene_set_player2(scene, 3);
            case SDLK_8:
                scene_set_player2(scene, 4);
        }
    }
}



void end_game(player_t *player1, player_t *player2) {
    int player1_score = player_get_score(player1);
    int player2_score = player_get_score(player2);
    if (player1_score > player2_score) {
        printf("Player 1 wins! %d-%d\n", player1_score, player2_score);
    }
    else if (player2_score > player1_score) {
        printf("Player 2 wins! %d-%d\n", player2_score, player1_score);
    }
    else {
        printf("Tie! %d-%d\n", player1_score, player2_score);
    }
}

void check_player_legs(scene_t *scene, player_t *p1, player_t *p2) {
    body_t *p1_body = player_get_body(p1);
    body_t *p1_leg = player_get_leg(p1);
    body_t *p2_body = player_get_body(p2);
    body_t *p2_leg = player_get_leg(p2);
    if (fabs(body_get_centroid(p1_leg).x-body_get_centroid(p1_body).x) > 40 || fabs(body_get_centroid(p1_leg).x-body_get_centroid(p1_body).x) < 38) {
            vector_t v1 = {.x = 40, .y = 0};
            vector_t new_leg1 = vec_add(body_get_centroid(p1_body), v1);
            body_set_centroid(p1_leg, new_leg1);
        }
    // if (fabs(body_get_centroid(p1_leg).y-body_get_centroid(p1_body).y) > 40 || fabs(body_get_centroid(p1_leg).y-body_get_centroid(p1_body).y) < 38) {
    //     vector_t v2 = {.x = 0, .y = -10};
    //     vector_t new_leg2 = vec_add(body_get_centroid(p1_body), v2);
    //     body_set_centroid(p1_leg, new_leg2);
    // }    

    if (fabs(body_get_centroid(p2_body).x-body_get_centroid(p2_leg).x) > 40 || fabs(body_get_centroid(p2_body).x-body_get_centroid(p2_leg).x) < 38) {
            vector_t v3 = {.x = -40, .y = 0};
            vector_t new_leg3 = vec_add(body_get_centroid(p2_body), v3);
            body_set_centroid(p2_leg, new_leg3);
        }
    // if (fabs(body_get_centroid(p2_body).y-body_get_centroid(p2_leg).y) > 40 || fabs(body_get_centroid(p2_body).y-body_get_centroid(p2_leg).y) < 38) {
    //         vector_t v4 = {.x = 0, .y = 0};
    //         vector_t new_leg4 = vec_add(body_get_centroid(p2_body), v4);
    //         body_set_centroid(p2_leg, new_leg4);
    //     }
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    scene_t *title = scene_init();
    scene_t *soccer_scene = scene_init();
    scene_t *char_scene = scene_init();
    
    sdl_on_key((key_handler_t) on_key_title);
    
    make_SDL_image();

    scene_set_bkg_image(title, "images/titlescreen.png");
    scene_set_bkg_image(char_scene, "images/charselect.png");
    sdl_init_textures(title);

    while (!sdl_is_done(title)) {
        double dt = time_since_last_tick();
        scene_tick(title, dt);
        sdl_render_scene(title);   
        if (scene_get_info(title) == 't') {scene_free(title); break; }
        else if (scene_get_info(title) == 'c') {
            scene_free(title);
            sdl_on_key((key_handler_t) on_key_char);

            sdl_init_textures(char_scene);

            while (!sdl_is_done(char_scene)) {
                double dt = time_since_last_tick();
                scene_tick(char_scene, dt);
                sdl_render_scene(char_scene);
                if (scene_get_info(char_scene) == 'g') { scene_free(char_scene); break; }
            }
            break;
        }
    }

    scene_add_body(title, make_rectangle(100, 100, BALL_SPAWN));
    while (!sdl_is_done(title)) {
        double dt = time_since_last_tick();
        scene_tick(title, dt);
        sdl_render_scene(title);
        if (!((bool)scene_get_info(title))) { break; }
    }
    scene_free(title);

    make_shapes(soccer_scene);
    make_forces(soccer_scene);

    player_t *player1 = player_init(scene_get_body(soccer_scene, 0), scene_get_body(soccer_scene, 1), 200, 200);
    player_t *player2 = player_init(scene_get_body(soccer_scene, 2), scene_get_body(soccer_scene, 3), 200, 200);
    scene_set_p1(soccer_scene, player1);
    scene_set_p2(soccer_scene, player2);

    sdl_on_key((key_handler_t) on_key_player);

    scene_set_bkg_image(soccer_scene, "images/stadium.png");
    // scene_set_bkg_image(title, "image/title.png"); //todo
    scene_set_bkg_sound(soccer_scene, "sounds/crowd.mp3");

    make_SDL_image();
    sdl_init_textures(soccer_scene);

    TTF_Font *font = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 24);
    SDL_Rect *p1_scoreboard = sdl_rect_init(340, 20, 50, 70);
    SDL_Rect *p2_scoreboard = sdl_rect_init(600, 20, 50, 70);
    SDL_Rect *time_board = sdl_rect_init(450, 20, 80, 70);
    SDL_Color black = {0, 0, 0, 255};

    char *p1 = malloc(sizeof(char) * 3);
    char *p2 = malloc(sizeof(char) * 3);
    char *timer = malloc(sizeof(char) * 3);
    int p1_score = player_get_score(player1);
    int p2_score = player_get_score(player2);
    double time = 0;
    sprintf(p1, "%d", p1_score);
    sprintf(p2, "%d", p2_score);
    int seconds = 60 - time;
    sprintf(timer, "%d", seconds);

    while (!sdl_is_done(soccer_scene)) {
        sdl_render_scene(soccer_scene);
        double dt = time_since_last_tick();
        time += dt;
        if (time > 59.8) {
            break;
        }
        check_edge(soccer_scene);
        check_player_legs(soccer_scene, player1, player2);
        ball_too_fast(scene_get_body(soccer_scene, BALL_NUMBER_IN_SCENE));
        scene_tick(soccer_scene, dt);
        reset_scene(soccer_scene, player1, player2);
        sdl_render_game_scene(soccer_scene, p1, p2, timer, p1_scoreboard, p2_scoreboard, time_board, font, black);
        if (player_get_score(player1) != p1_score) {
            p1_score = player_get_score(player1);
            sprintf(p1, "%d", p1_score);
        }
        if (player_get_score(player2) != p2_score) {
            p2_score = player_get_score(player2);
            sprintf(p2, "%d", p2_score);
        }
        int temp_secs = 60 - time;
        if (temp_secs != seconds) {
            int seconds = temp_secs;
            sprintf(timer, "%d", seconds);
        }
    }
    stop_bkg_sound(soccer_scene);
    end_game(player1, player2);
    scene_free(soccer_scene);
    free(p1_scoreboard);
    free(p2_scoreboard);
    free(time_board);
    TTF_CloseFont(font);
    free(p1);
    free(p2);
    free(timer);
    return 0;
}
