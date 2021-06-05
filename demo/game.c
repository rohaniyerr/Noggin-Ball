#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "body.h"
#include "forces.h"
#include "polygon.h"
#include "player.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_wrapper.h"

const size_t WINDOW_WIDTH_ = 1000;
const size_t WINDOW_HEIGHT_ = 500;
const vector_t MIN_POINT = {0, 0};
const vector_t MAX_POINT = {WINDOW_WIDTH_, WINDOW_HEIGHT_};

const size_t DZLI_ID = 1;
const double DZLI_JUMP_SCALAR = 200;
const double DZLI_SPEED_SCALAR = 300;
const vector_t DZLI_GRAVITY = {0, -400};
const char *DZLI_PIC = "images/dzli.png";

const size_t DING_ID = 2;
const double DING_JUMP_SCALAR = 300;
const double DING_SPEED_SCALAR = 200;
const vector_t DING_GRAVITY = {0, -600};
const char *DING_PIC = "images/ding.png";

const size_t RIIYER_ID = 3;
const double RIIYER_JUMP_SCALAR = 250;
const double RIIYER_SPEED_SCALAR = 250;
const vector_t RIIYER_GRAVITY = {0, -500};
const char *RIIYER_PIC = "images/riiyer.png";

const size_t ESEINER_ID = 4;
const double ESEINER_JUMP_SCALAR = 350;
const double ESEINR_SPEED_SCALAR = 350;
const vector_t ESEINER_GRAVITY = {0, -700};
const char *ESEINER_PIC = "images/eseiner.png";

const size_t SDUNBAR_ID = 5;
const double SDUNBAR_JUMP_SCALAR = 500;
const double SDUNBAR_SPEED_SCALAR = 800;
const vector_t SDUNBAR_GRAVITY = {0, -900};
const char *SDUNBAR_PIC = "images/sdunbar.png";

const size_t P1_BODY_ID = 0;
const size_t P1_LEG_ID = 1;
const size_t P2_BODY_ID = 2;
const size_t P2_LEG_ID = 3;
const size_t BALL_ID = 4;
const size_t LEFT_WALL_ID = 5;
const size_t RIGHT_WALL_ID = 6;
const size_t CEILING_ID = 7;
const size_t FLOOR_ID = 8;
const size_t VERT_GOAL1_ID = 9;
const size_t HOR_GOAL1_ID = 10;
const size_t VERT_GOAL2_ID = 11;
const size_t HOR_GOAL2_ID = 12;

const vector_t LEFT_WALL_SPAWN = {5, 250};
const vector_t RIGHT_WALL_SPAWN = {995, 250};
const vector_t CEILING_SPAWN = {500, 495};
const vector_t FLOOR_SPAWN = {500, 5};

const int GOAL_VERT_LENGTH = 10;
const int GOAL_VERT_HEIGHT = 125;
const vector_t GOAL1_VERT_SPAWN = {20, 40.5};
const vector_t GOAL2_VERT_SPAWN = {980, 40.5};
const int GOAL_HOR_LENGTH = 25;
const int GOAL_HOR_HEIGHT = 10;
const vector_t GOAL1_HOR_SPAWN = {55, 155};
const vector_t GOAL2_HOR_SPAWN = {945, 155};
const double GOAL1_HOR_ANGLE = -M_PI / 69;
const double GOAL2_HOR_ANGLE = M_PI / 69;
const int GOAL1_GOAL_LINE = 85;
const int GOAL2_GOAL_LINE = 915;
const double CROSSBAR_HEIGHT = 145.5;

const double GAMMA = 1.5;
const vector_t GRAVITY = {0, -400};
const double WALL_ELASTICITY = 0.15;
const double PLAYER_ELASTICITY = 0.4;

const int GAME_TIME = 60;
const char *BKG_PIC = "images/stadium.png";
const char *TITLE_PIC = "images/titlescreen.png";
const char *CHAR_SELECT_PIC = "images/charselect.png";
const char *FONT_PATH = "fonts/OpenSans-Bold.ttf";
const int FONT_SIZE = 24;

const char *BKG_SOUND = "sounds/crowd.wav";
const int SOUND_FREQUENCY = 44100;
const int SOUND_CHANNELS = 2;
const int SOUND_CHUNKSIZE = 512;
const int SOUND_LOOPS = -1;

const double PLAYER_RADIUS = 45;
const double PLAYER1_ANGLE = -M_PI/5;
const double PLAYER2_ANGLE = M_PI/5;
const vector_t PLAYER1_BODY_SPAWN = {200, 200};
const vector_t PLAYER2_BODY_SPAWN = {800, 200};
const vector_t PLAYER1_LEG_SPAWN = {240, 200};
const vector_t PLAYER2_LEG_SPAWN = {760, 200};
const double PLAYER_MAJOR_AXIS = 0.7;
const double PLAYER_MINOR_AXIS = 0.9;
const vector_t PLAYER1_LEG_TOP_LEFT = {215, 92};
const vector_t PLAYER2_LEG_TOP_RIGHT = {800, 30};
const double LEG_SCALING = 3.5;
const double PLAYER_MASS = 1;
const double KICK_ANGLE = M_PI / 3;
const double BODY_LEG_MAX_X_DIFF = 40;
const double BODY_LEG_MIN_X_DIFF = 38;
const double BODY_LEG_MAX_Y_DIFF = 30;

const char *BALL_PIC = "images/ball.png";
const double CIRCLE_POINTS = 40;
const double BALL_MASS = 1;
const double BALL_RADIUS = 25;
const vector_t BALL_SPAWN = {500, 400};
const double BALL_MAX_VELOCITY = 1000;
const double BALL_MAX_VELOCITY_SCALE = 0.5;

//ding player_t model
player_t *make_ding(body_t *body, body_t *leg) {
    return player_init(body, leg, DING_JUMP_SCALAR, DING_SPEED_SCALAR, DING_GRAVITY, DING_PIC);
}

//riiyer player_t model
player_t *make_riiyer(body_t *body, body_t *leg) {
    return player_init(body, leg, RIIYER_JUMP_SCALAR, RIIYER_SPEED_SCALAR, RIIYER_GRAVITY, RIIYER_PIC);
}

//dzli player_t model
player_t *make_dzli(body_t *body, body_t *leg) {
    return player_init(body, leg, DZLI_JUMP_SCALAR, DZLI_SPEED_SCALAR, DZLI_GRAVITY, DZLI_PIC);
}

//eseiner player_t model
player_t *make_eseiner(body_t *body, body_t *leg) {
    return player_init(body, leg, ESEINER_JUMP_SCALAR, ESEINR_SPEED_SCALAR, ESEINER_GRAVITY, ESEINER_PIC);
}

//sdunbar player_t model
player_t *make_sdunbar(body_t *body, body_t *leg) {
    return player_init(body, leg, SDUNBAR_JUMP_SCALAR, SDUNBAR_SPEED_SCALAR, SDUNBAR_GRAVITY, SDUNBAR_PIC);
}

//make and set player1
void make_player1(scene_t *scene, size_t index) {
    body_t *p1_body = scene_get_body(scene, P1_BODY_ID);
    body_t *p1_leg = scene_get_body(scene, P1_LEG_ID);
    if (index == DZLI_ID) { 
        player_t *dzli = make_dzli(p1_body, p1_leg);
        scene_set_player1(scene, dzli);
    }
    else if (index == DING_ID) {
        player_t *ding = make_ding(p1_body, p1_leg);
        scene_set_player1(scene, ding);
    }
    else if (index == RIIYER_ID) {
        player_t *riiyer = make_riiyer(p1_body, p1_leg);
        scene_set_player1(scene, riiyer);
    }
    else if (index == ESEINER_ID) {
        player_t *eseiner = make_eseiner(p1_body, p1_leg);
        scene_set_player1(scene, eseiner);
    }
    else if (index == SDUNBAR_ID) {
        player_t *sdunbar = make_sdunbar(p1_body, p1_leg);
        scene_set_player1(scene, sdunbar);
    }
    scene_set_p1(scene, index);
}

//make and set player2
void make_player2(scene_t *scene, size_t index) {
    body_t *p2_body = scene_get_body(scene, P2_BODY_ID);
    body_t *p2_leg = scene_get_body(scene, P2_LEG_ID);
    if (index == DZLI_ID) { 
        player_t *dzli = make_dzli(p2_body, p2_leg);
        scene_set_player2(scene, dzli);
    }
    else if (index == DING_ID) {
        player_t *ding = make_ding(p2_body, p2_leg);
        scene_set_player2(scene, ding);
    }
    else if (index == RIIYER_ID) {
        player_t *riiyer = make_riiyer(p2_body, p2_leg);
        scene_set_player2(scene, riiyer);
    }
    else if (index == ESEINER_ID) {
        player_t *eseiner = make_eseiner(p2_body, p2_leg);
        scene_set_player2(scene, eseiner);
    }
    else if (index == SDUNBAR_ID) {
        player_t *sdunbar = make_sdunbar(p2_body, p2_leg);
        scene_set_player2(scene, sdunbar);
    }
    scene_set_p2(scene, index);
}

//key controls for both characters
void on_key_player(char key, key_event_type_t type, void *scene) {
    body_t *body2 = scene_get_body((scene_t*)scene, P2_BODY_ID);
    body_t *leg2 = scene_get_body((scene_t*)scene, P2_LEG_ID);
    player_t *p2 = scene_get_player2((scene_t *)scene);
    double p2_speed = player_get_speed(p2);
    double p2_jump = player_get_jump(p2);
    body_set_rotation(leg2, PLAYER2_ANGLE);
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                if(true == true) {
                    vector_t v = {.x = -p2_speed, .y = body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case RIGHT_ARROW:
                if(true == true) {
                    vector_t v = {p2_speed, body_get_velocity(body2).y};
                    body_set_velocity(body2, v);
                    body_set_velocity(leg2, v);
                    break;
                }
            case UP_ARROW:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body2), body_get_shape(scene_get_body(scene, FLOOR_ID)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body2).x, .y = p2_jump};
                        body_set_velocity(body2, v);
                        body_set_velocity(leg2, v);
                    }
                    break;
                }
            case SDLK_COMMA:
                if (true == true) {
                    body_set_rotation(leg2, -KICK_ANGLE);
                    break;
                }
        }
    }
    else {
        vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body2).y};
        body_set_velocity(body2, NO_X2);
        body_set_velocity(leg2, NO_X2);
    }
    player_t *p1 = scene_get_player1((scene_t *)scene);
    double p1_speed = player_get_speed(p1);
    double p1_jump = player_get_jump(p1);
    body_t *body1 = scene_get_body((scene_t*)scene, P1_BODY_ID);
    body_t *leg1 = scene_get_body((scene_t*)scene, P1_LEG_ID);
    body_set_rotation(leg1, PLAYER1_ANGLE);    
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_d:
                if(true == true) {
                    vector_t v = {.x = p1_speed, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case SDLK_a:
                if(true == true) {
                    vector_t v = {.x = -p1_speed, .y = body_get_velocity(body1).y};
                    body_set_velocity(body1, v);
                    body_set_velocity(leg1, v);
                    break;
                }
            case SDLK_w:
                if(true == true) {
                    collision_info_t info = find_collision(body_get_shape(body1), body_get_shape(scene_get_body(scene, FLOOR_ID)));
                    if (info.collided) {
                        vector_t v = {.x = body_get_velocity(body1).x, .y = p1_jump};
                        body_set_velocity(body1, v);
                        body_set_velocity(leg1, v);
                    }
                    break;
                }
            case SDLK_c:
                if (true == true){
                    body_set_rotation(leg1, KICK_ANGLE);
                    break;
                }
        }
    }
    else {
        vector_t NO_X2 = {.x = 0, .y = body_get_velocity(body1).y};
        body_set_velocity(body1, NO_X2);
        body_set_velocity(leg1, NO_X2);
    }
}

//make sounds for scenes
void make_sounds(scene_t *scene) {
    if(!Mix_PlayingMusic()){
        SDL_Init(SDL_INIT_AUDIO);
        Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
        Mix_OpenAudio(SOUND_FREQUENCY, MIX_DEFAULT_FORMAT, SOUND_CHANNELS, SOUND_CHUNKSIZE);
        Mix_Music *sound = Mix_LoadMUS(BKG_SOUND);
        scene_set_bkg_sound(scene, sound);
        Mix_PlayMusic(sound, SOUND_LOOPS);
    }
}

//make rectangle (for goals/walls/floor)
body_t *make_rectangle(int length, int height, vector_t spawn) {
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

//make walls using make_rectangle
void make_walls(scene_t *scene) {
    int size = 5;
    rgb_color_t *BLACK = rgb_color_init(0,0,0);

    body_t *wall1 = make_rectangle(size, WINDOW_HEIGHT_, LEFT_WALL_SPAWN);
    body_set_mass(wall1, INFINITY);
    body_set_color(wall1, BLACK);
    scene_add_body(scene, wall1);

    body_t *wall2 = make_rectangle(size, WINDOW_HEIGHT_, RIGHT_WALL_SPAWN);
    body_set_mass(wall2, INFINITY);
    body_set_color(wall2, BLACK);
    scene_add_body(scene, wall2);

    body_t *wall3 = make_rectangle(WINDOW_WIDTH_, size, CEILING_SPAWN);
    body_set_mass(wall3, INFINITY);
    body_set_color(wall3, BLACK);
    scene_add_body(scene, wall3);

    body_t *wall4 = make_rectangle(WINDOW_WIDTH_, size, FLOOR_SPAWN);
    body_set_mass(wall4, INFINITY);
    body_set_color(wall4, BLACK);
    scene_add_body(scene, wall4);
}

//make goals using make_rectangle
void make_goals(scene_t *scene) {
    rgb_color_t *white = rgb_color_init(1, 1, 1);
 
    body_t *vert_goal1 = make_rectangle(GOAL_VERT_LENGTH, GOAL_VERT_HEIGHT, GOAL1_VERT_SPAWN);
    body_set_color(vert_goal1, white);
    body_set_mass(vert_goal1, INFINITY);
    scene_add_body(scene, vert_goal1);
    
    body_t *hor_goal1 = make_rectangle(GOAL_HOR_LENGTH, GOAL_HOR_HEIGHT, GOAL1_HOR_SPAWN);
    body_set_color(hor_goal1, white);
    body_set_mass(hor_goal1, INFINITY);
    body_set_rotation(hor_goal1, GOAL1_HOR_ANGLE);
    scene_add_body(scene, hor_goal1);

    body_t *vert_goal2 = make_rectangle(GOAL_VERT_LENGTH, GOAL_VERT_HEIGHT, GOAL2_VERT_SPAWN);
    body_set_color(vert_goal2, white);
    body_set_mass(vert_goal2, INFINITY);
    scene_add_body(scene, vert_goal2);
    
    body_t *hor_goal2 = make_rectangle(GOAL_HOR_LENGTH, GOAL_HOR_HEIGHT, GOAL2_HOR_SPAWN);
    body_set_color(hor_goal2, white);
    body_set_mass(hor_goal2, INFINITY);
    body_set_rotation(hor_goal2, GOAL2_HOR_ANGLE);
    scene_add_body(scene, hor_goal2);
}

//make circle
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

//make ball using make_circle
body_t *make_ball(scene_t *scene, double radius) {
    list_t *circ = circle_init(radius);
    body_t *ball = body_init(circ, BALL_MASS, choose_rand_color());
    body_set_centroid(ball, BALL_SPAWN);
    body_set_radius(ball, BALL_RADIUS);
    body_set_image(ball, BALL_PIC);
    scene_add_body(scene, ball);
    return ball;
}

//make oval for body
body_t *make_oval(rgb_color_t *color, double radius, double x_scalar, double y_scalar) {
    list_t *c = list_init(CIRCLE_POINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRCLE_POINTS; i++) {
        double angle = 2 * M_PI * i / CIRCLE_POINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {x_scalar * radius * cos(angle), y_scalar * radius * sin(angle)};
        list_add(c, v);
    }
    return body_init(c, PLAYER_MASS, *color);
}

//make leg for player2
body_t *make_p2_leg(scene_t *scene, rgb_color_t *color, vector_t spawn, vector_t top_right) {
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

    body_t *body = body_init(leg, PLAYER_MASS, *color);
    body_set_rotation(body, PLAYER2_ANGLE);
    body_set_centroid(body, spawn);
    scene_add_body(scene, body);
    return body;
}

//make leg for player1
body_t *make_p1_leg(scene_t *scene, rgb_color_t *color, vector_t spawn, vector_t top_left) {
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
    curr_x -= LEG_SCALING * 5;
    curr_y += LEG_SCALING * 3;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);

    
    v = malloc(sizeof(*v));
    curr_y += LEG_SCALING * 7;
    v->x = curr_x;
    v->y = curr_y;
    list_add(leg, v);
    

    body_t *body = body_init(leg, PLAYER_MASS, *color);
    body_set_rotation(body, PLAYER1_ANGLE);
    body_set_centroid(body, spawn);
    scene_add_body(scene, body);
    return body;
}

//make body for players
body_t *make_player_body(scene_t *scene, rgb_color_t *color, vector_t spawn) {
    body_t *player_body = make_oval(color, PLAYER_RADIUS, PLAYER_MAJOR_AXIS, PLAYER_MINOR_AXIS);
    body_set_color(player_body, color);
    body_set_centroid(player_body, spawn);
    body_set_radius(player_body, PLAYER_RADIUS);
    scene_add_body(scene, player_body);
    return player_body;
}

//check edge for ball, bodies, and legs
void check_edge(scene_t *scene) {
    body_t *ball = scene_get_body(scene, BALL_ID);
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
        if (i == P1_LEG_ID || i == P2_LEG_ID) {
            if (c.y <= 35) {
                body_set_centroid(body, (vector_t) {c.x, 39});
                body_set_velocity(body, (vector_t) {body_get_velocity(body).x, 0});
            }
        }
        if (c.x <= 15) {
            vector_t new_centroid = {.x = 20, .y = c.y};
            body_set_centroid(body, new_centroid);
        }
        else if (c.x >= 985) {
            vector_t new_centroid = {.x = 980, .y = c.y};
            body_set_centroid(body, new_centroid);
        }
    }
}

//check if a goal has been scored
bool check_goal(body_t *ball, player_t *p1, player_t *p2) {
    vector_t centroid = body_get_centroid(ball);
    if (centroid.y + BALL_RADIUS <= CROSSBAR_HEIGHT) {
        if (centroid.x + BALL_RADIUS <= GOAL1_GOAL_LINE) {
            player_set_score(p2, player_get_score(p2) + 1);
            return true;
        }
        if (centroid.x - BALL_RADIUS >= GOAL2_GOAL_LINE) {
            player_set_score(p1, player_get_score(p1) + 1);
            return true;
        }
    }
    return false;
}

//play sound if a goal is scored
void play_goal_sound() {
    Mix_Chunk *goal = Mix_LoadWAV("sounds/ronaldo.wav");
    Mix_PlayChannel(1, goal, 0);
}

//closes sound
void channel_done(int channel) {
    Mix_FreeChunk(Mix_GetChunk(channel));
}

//resets scene, called only when goal is scored
void reset_scene(scene_t *scene, player_t *player1, player_t *player2) {
    body_t *p1_body = player_get_body(player1);
    body_t *p1_leg = player_get_leg(player1);
    body_t *p2_body = player_get_body(player2);
    body_t *p2_leg = player_get_leg(player2);
    body_t *ball = scene_get_body(scene, BALL_ID);
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

//cap the velocity of the ball
void ball_too_fast(body_t *ball) {
    if (vec_magnitude(body_get_velocity(ball)) > BALL_MAX_VELOCITY) {
        body_set_velocity(ball, vec_multiply(BALL_MAX_VELOCITY_SCALE, body_get_velocity(ball)));
    }
}

//initialize forces for every body in the scene
//gravity on ball and bodies
//normal force on ball and bodies
//drag on ball
//elasticity between ball/bodies on the walls/floor
void make_forces(scene_t *scene) {
    body_t *ball = scene_get_body(scene, BALL_ID);
    player_t *player1 = scene_get_player1(scene);
    player_t *player2 = scene_get_player2(scene);
    vector_t P1_GRAVITY = player_get_gravity(player1);
    vector_t P2_GRAVITY = player_get_gravity(player2);
    //add drag to ball so it doesn't accelerate
    create_drag(scene, GAMMA, ball);
    //add gravity to ball and players
    for (size_t i = 0; i < LEFT_WALL_ID; i++) { 
        if (i == 4) {create_planet_gravity(scene, GRAVITY, scene_get_body(scene, i)); }
        else if (i < 2) {create_planet_gravity(scene, P1_GRAVITY, scene_get_body(scene, i)); }
        else {create_planet_gravity(scene, P2_GRAVITY, scene_get_body(scene, i)); }
         //add normal force between players/ball and top of goals/floor
        create_normal_force(scene, scene_get_body(scene, i), scene_get_body(scene, FLOOR_ID));
        create_normal_force(scene, scene_get_body(scene, i), scene_get_body(scene, HOR_GOAL1_ID));
        create_normal_force(scene, scene_get_body(scene, i), scene_get_body(scene, HOR_GOAL2_ID));
    }
    //add physics collisions between players and ball
    for (size_t i = 0; i < BALL_ID; i++) { 
        create_physics_one_collision(scene, PLAYER_ELASTICITY, scene_get_body(scene, i), ball);
    }
    //add physics collisions between the ball and the walls
    //add physics collisions between players and walls
    for (size_t i = LEFT_WALL_ID; i < scene_bodies(scene); i++) { 
        create_physics_one_collision(scene, WALL_ELASTICITY, scene_get_body(scene, i), ball);
        for (size_t j = 0; j < BALL_ID; j++) {
            if (j % 2 == 0 || i == FLOOR_ID) {create_physics_collision(scene, WALL_ELASTICITY, scene_get_body(scene, j), scene_get_body(scene, i)); }
        }
    }
}

//makes all the sprites in the game (walls/floor/bodies/ball)
void make_shapes(scene_t *soccer_scene) {
    rgb_color_t *GREEN = rgb_color_init(0,1,0);
    rgb_color_t *BLUE = rgb_color_init(0,0,1);
    rgb_color_t *BLACK = rgb_color_init(0,0,0);

    make_player_body(soccer_scene, GREEN, PLAYER1_BODY_SPAWN);
    make_p1_leg(soccer_scene, BLACK, PLAYER1_LEG_SPAWN, PLAYER1_LEG_TOP_LEFT);
    make_player_body(soccer_scene, BLUE, PLAYER2_BODY_SPAWN);
    make_p2_leg(soccer_scene, BLACK, PLAYER2_LEG_SPAWN, PLAYER2_LEG_TOP_RIGHT);

    make_ball(soccer_scene, BALL_RADIUS);

    make_walls(soccer_scene);
    make_goals(soccer_scene);
}

//on_key function for title screen
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
            case SDLK_q:
                exit(0);
        }
    }
}

//on_key function for character selection screen
void on_key_char(char key, key_event_type_t type, void *scene) {
    if (type == KEY_PRESSED) {
        switch (key) {
            case SDLK_RETURN:
                scene_set_info(scene, 'g');
                break;
            case SDLK_1:
                scene_set_p1(scene, DZLI_ID);
                break;
            case SDLK_2:
                scene_set_p1(scene, DING_ID);
                break;
            case SDLK_3:
                scene_set_p1(scene, RIIYER_ID);
                break;
            case SDLK_4:
                scene_set_p1(scene, ESEINER_ID);
                break;
            case SDLK_5:
                scene_set_p2(scene, DZLI_ID);
                break;
            case SDLK_6:
                scene_set_p2(scene, DING_ID);
                break;
            case SDLK_7:
                scene_set_p2(scene, RIIYER_ID);
                break;
            case SDLK_8:
                scene_set_p2(scene, ESEINER_ID);
                break;
            case SDLK_h:
                scene_set_p2(scene, SDUNBAR_ID);
                break;
            case SDLK_g:
                scene_set_p1(scene, SDUNBAR_ID);
                break;
            case SDLK_q:
                exit(0);
        }
    }
}

//ensure players legs do not separate too far from the bodies
void check_player_legs(scene_t *scene, player_t *p1, player_t *p2) {
    body_t *p1_body = player_get_body(p1);
    body_t *p1_leg = player_get_leg(p1);
    body_t *p2_body = player_get_body(p2);
    body_t *p2_leg = player_get_leg(p2);
    if (fabs(body_get_centroid(p1_leg).x-body_get_centroid(p1_body).x) > BODY_LEG_MAX_X_DIFF || fabs(body_get_centroid(p1_leg).x-body_get_centroid(p1_body).x) < BODY_LEG_MIN_X_DIFF) {
            vector_t v1 = {.x = BODY_LEG_MAX_X_DIFF, .y = 0};
            vector_t new_leg1 = vec_add(body_get_centroid(p1_body), v1);
            body_set_centroid(p1_leg, new_leg1);
        }
    if (fabs(body_get_centroid(p1_leg).y-body_get_centroid(p1_body).y) > BODY_LEG_MAX_Y_DIFF) {
        vector_t v2 = {.x = 0, .y = -BODY_LEG_MAX_Y_DIFF};
        vector_t new_leg2 = vec_add(body_get_centroid(p1_body), v2);
        body_set_centroid(p1_leg, new_leg2);
    }    

    if (fabs(body_get_centroid(p2_body).x-body_get_centroid(p2_leg).x) > BODY_LEG_MAX_X_DIFF || fabs(body_get_centroid(p2_body).x-body_get_centroid(p2_leg).x) < BODY_LEG_MIN_X_DIFF) {
            vector_t v3 = {.x = -BODY_LEG_MAX_X_DIFF, .y = 0};
            vector_t new_leg3 = vec_add(body_get_centroid(p2_body), v3);
            body_set_centroid(p2_leg, new_leg3);
        }
    if (fabs(body_get_centroid(p2_leg).y-body_get_centroid(p2_body).y) > BODY_LEG_MAX_Y_DIFF) {
        vector_t v4 = {.x = 0, .y = -BODY_LEG_MAX_Y_DIFF};
        vector_t new_leg4 = vec_add(body_get_centroid(p2_body), v4);
        body_set_centroid(p2_leg, new_leg4);
    }   
}

//stop players from passing through each other
void prevent_intersection(player_t *player1, player_t *player2) {
    vector_t p1_cent = body_get_centroid(player_get_body(player1));
    vector_t p2_cent = body_get_centroid(player_get_body(player2));

    if (p1_cent.x > p2_cent.x) {
        if (p2_cent.x + PLAYER_RADIUS >= p1_cent.x && fabs(p1_cent.y - p2_cent.y) < 2*PLAYER_RADIUS) {
            if (fabs(body_get_velocity(player_get_body(player2)).x) > fabs(body_get_velocity(player_get_body(player1)).x)) {
                vector_t new_p1_pos = {.x = p2_cent.x + PLAYER_RADIUS, .y = p1_cent.y};
                body_set_centroid(player_get_body(player1), new_p1_pos);
            }
            else {
                vector_t new_p2_pos = {.x = p1_cent.x - PLAYER_RADIUS, .y = p2_cent.y};
                body_set_centroid(player_get_body(player2), new_p2_pos);
            }
        }
    }
    else if (p2_cent.x > p1_cent.x) {
        if (p1_cent.x + PLAYER_RADIUS >= p2_cent.x && fabs(p1_cent.y - p2_cent.y) < 2*PLAYER_RADIUS) {
            if (fabs(body_get_velocity(player_get_body(player1)).x) > fabs(body_get_velocity(player_get_body(player2)).x)) {
                vector_t new_p2_pos = {.x = p1_cent.x + PLAYER_RADIUS, .y = p2_cent.y};
                body_set_centroid(player_get_body(player2), new_p2_pos);
            }
            else {
                vector_t new_p1_pos = {.x = p2_cent.x - PLAYER_RADIUS, .y = p1_cent.y};
                body_set_centroid(player_get_body(player1), new_p1_pos);
            }
        }
    }
}

//updates the scoreboard
int update_player_scoreboard(player_t *player, int score, char *score_str) {
    if (player_get_score(player) != score) {
        score = player_get_score(player);
        sprintf(score_str, "%d", score);
    }
    return score;
}

//updates the timer
int update_timer(double time, int seconds, char *timer) {
    int temp_secs = GAME_TIME - time;
    if (temp_secs != seconds) {
        seconds = temp_secs;
        sprintf(timer, "%d", seconds);
    }
    return seconds;
}

//frees the scoreboard
void free_scoreboard(SDL_Rect *p1_board, SDL_Rect *p2_board, SDL_Rect *time_board, SDL_Rect *disp_winner, char *p1, char *p2, char *timer, char *winner, TTF_Font *font) {
    free(p1_board);
    free(p2_board);
    free(time_board);
    free(disp_winner);
    TTF_CloseFont(font);
    free(p1);
    free(p2);
    free(timer);
    free(winner);
}

//prints the winner (when timer runs out)
void print_winner(char *winner, int p1_score, int p2_score) {
    if(p1_score > p2_score){
        strcpy(winner, "Player 1 wins!");
    }
    else if(p2_score > p1_score){
        strcpy(winner, "Player 2 wins!");
    }
    else{
        strcpy(winner, "It's a tie!");
    }
}

int main() {
    sdl_init(MIN_POINT, MAX_POINT);
    //initialize scenes
    scene_t *title = scene_init();
    scene_t *soccer_scene = scene_init();
    scene_t *char_scene = scene_init();
    
    //default player1 and player2 indices
    size_t p1_idx = scene_get_p1(char_scene);
    size_t p2_idx = scene_get_p2(char_scene);

    sdl_on_key((key_handler_t) on_key_title);

    make_SDL_image();

    scene_set_bkg_image(title, TITLE_PIC);
    scene_set_bkg_image(char_scene, CHAR_SELECT_PIC);
    sdl_init_textures(title);

    //running the title screen
    while (!sdl_is_done(title)) {
        double dt = time_since_last_tick();
        scene_tick(title, dt);
        sdl_render_scene(title);   
        if (scene_get_info(title) == 't') { scene_free(title); break;}
        else if (scene_get_info(title) == 'c') {
            scene_free(title);
            sdl_on_key((key_handler_t) on_key_char);

            sdl_init_textures(char_scene);
            //running the character selection screen
            while (!sdl_is_done(char_scene)) {
                double dt = time_since_last_tick();
                scene_tick(char_scene, dt);
                sdl_render_scene(char_scene);
                if (scene_get_info(char_scene) == 'g') { break;}
            }
            p1_idx = scene_get_p1(char_scene);
            p2_idx = scene_get_p2(char_scene);
            scene_free(char_scene);
            break;
        }
    }

    //make shapes and players for game scene
    make_shapes(soccer_scene);
    make_player1(soccer_scene, p1_idx);
    make_player2(soccer_scene, p2_idx);
    make_forces(soccer_scene);

    player_t *player1 = scene_get_player1(soccer_scene);
    player_t *player2 = scene_get_player2(soccer_scene);
    
    sdl_on_key((key_handler_t) on_key_player);

    scene_set_bkg_image(soccer_scene, BKG_PIC);

    //scoreboard
    TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    SDL_Rect *p1_scoreboard = sdl_rect_init(340, 20, 50, 70);
    SDL_Rect *p2_scoreboard = sdl_rect_init(600, 20, 50, 70);
    SDL_Rect *time_board = sdl_rect_init(450, 20, 80, 70);
    SDL_Rect *disp_winner = sdl_rect_init(160, 150, 700, 90);
    SDL_Color black = {0, 0, 0, 255};

    //timer and score
    char *p1 = malloc(sizeof(char) * 3);
    char *p2 = malloc(sizeof(char) * 3);
    char *timer = malloc(sizeof(char) * 3);
    char *winner  = malloc(sizeof(char) * 15);

    int p1_score = player_get_score(player1);
    int p2_score = player_get_score(player2);
    double time = 0;
    sprintf(p1, "%d", p1_score);
    sprintf(p2, "%d", p2_score);
    int seconds = GAME_TIME - time;
    sprintf(timer, "%d", seconds);

    sdl_init_textures(soccer_scene);
    make_sounds(soccer_scene);

    while (!sdl_is_done(soccer_scene)) {
        double dt = time_since_last_tick();
        time += dt;
        if (time > GAME_TIME) {
            break;
        }
        check_edge(soccer_scene);
        check_player_legs(soccer_scene, player1, player2);
        prevent_intersection(player1, player2);
        ball_too_fast(scene_get_body(soccer_scene, BALL_ID));
        reset_scene(soccer_scene, player1, player2);
        scene_tick(soccer_scene, dt);
        sdl_render_game_scene(soccer_scene, p1, p2, timer, winner, p1_scoreboard, p2_scoreboard, time_board, disp_winner, font, black);
        p1_score = update_player_scoreboard(player1, p1_score, p1);
        p2_score = update_player_scoreboard(player2, p2_score, p2);
        seconds = update_timer(time, seconds, timer);
        if(strcmp(timer, "0") == 0){
            print_winner(winner, p1_score, p2_score);
        }
    }
    free_scoreboard(p1_scoreboard, p2_scoreboard, time_board, disp_winner, p1, p2, timer, winner, font);
    scene_free(soccer_scene);
    return 0;
}
