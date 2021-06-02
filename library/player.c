#include "player.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct player {
    body_t *body;
    body_t *leg;
    int score;
    double jump_scalar;
    double speed_scalar;
    double kick_speed;
} player_t;

player_t *player_init(body_t *body, body_t *leg, double jump, double speed) {
    player_t *player = malloc(sizeof(player_t));
    player->body = body;
    player->leg = leg;
    player->score = 0;
    player->jump_scalar = jump;
    player->speed_scalar = speed;
    return player;
}

body_t *player_get_body(player_t *player) {
    return player->body;
}

body_t *player_get_leg(player_t *player) {
    return player->leg;
}

int player_get_score(player_t *player) {
    return player->score;
}

double player_get_speed(player_t *player) {
    return player->speed_scalar;
}

double player_get_jump(player_t *player) {
    return player->jump_scalar;
}

void player_set_score(player_t *player, int score) {
    player->score = score;
}

void player_set_velocity(player_t *player, vector_t velocity) {
    body_set_velocity(player->body, velocity);
    body_set_velocity(player->leg, velocity);
}