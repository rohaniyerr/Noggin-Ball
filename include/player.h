#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "vector.h"
#include "body.h"

typedef struct player player_t;

player_t *player_init(body_t *body, body_t *leg, double jump, double speed, vector_t gravity, const char *filename);

body_t *player_get_body(player_t *player);

body_t *player_get_leg(player_t *player);

double player_get_speed(player_t *player);

double player_get_jump(player_t *player);

int player_get_score(player_t *player);

void player_set_score(player_t *player, int score);

vector_t player_get_gravity(player_t *player);

void player_set_velocity(player_t *player, vector_t velocity);

#endif // #ifndef __PLAYER_H__