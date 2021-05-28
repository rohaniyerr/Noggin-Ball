#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "body.h"
#include "polygon.h"

typedef struct body {
    list_t *shape;
    rgb_color_t color;
    double mass;
    vector_t velocity;
    vector_t force;
    vector_t acceleration;
    vector_t impulse;
    vector_t centroid;
    double rotation_speed;
    SDL_Texture *texture;
    SDL_Surface *image;
    double rotation;
    void *info;
    free_func_t info_freer;
    bool remove;
    double radius;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer) {
    body_t *body = malloc(sizeof(body_t));
    body->shape = shape;
    body->color = color;
    body->mass = mass;
    body->velocity = VEC_ZERO;
    body->centroid = polygon_centroid(shape);
    body->rotation_speed = 0;
    body->rotation = 0;
    body->force = VEC_ZERO;
    body->acceleration = VEC_ZERO;
    body->impulse = VEC_ZERO;
    body->info = info;
    body->info_freer = (free_func_t) info_freer;
    body->remove = false;
    body->image = NULL;
    body->texture = NULL;
    body->radius = 0;
    return body;
}


void body_free(body_t *body){
    list_free(body->shape);
    SDL_FreeSurface(body->image);
    free_func_t info_freer = (free_func_t) body->info_freer;
    if (info_freer != NULL) {
        info_freer(body->info);
    }
    free(body);
}

list_t *body_get_shape(body_t *body) {
    list_t *new_list = list_init(list_size(body->shape), (free_func_t) free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *v = malloc(sizeof(vector_t));
        *v = *(vector_t *)list_get(body->shape, i);
        list_add(new_list, v);
    }
    return new_list;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

void body_set_rotation_speed(body_t *body, double rotation){
    body->rotation_speed = rotation;
}

double body_get_rotation_speed(body_t *body) {
    return body->rotation_speed;
}

double body_get_rotation(body_t *body){
    return body->rotation;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

void body_set_color(body_t *body, rgb_color_t *color) {
    body->color = *color;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

void *body_get_info(body_t *body) {
    return body->info;
}

void body_set_centroid(body_t *body, vector_t x) {
    vector_t old_centroid = body->centroid;
    body->centroid = x;
    list_t *shape = body->shape;
    polygon_translate(shape, vec_subtract(x, old_centroid));
}

void body_set_velocity(body_t * body, vector_t velocity) {
    body->velocity = velocity;
}

void body_set_rotation(body_t *body, double angle) {
    list_t *shape = body->shape;
    double rotation = angle - body->rotation;
    polygon_rotate(shape, rotation, body->centroid);
    body->rotation = angle;
}

void body_add_force(body_t *body, vector_t force) {
    body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    body->impulse = vec_add(body->impulse, impulse);
}

void body_set_mass(body_t *body, double mass) {
    body->mass = mass;
}

void body_tick(body_t *body, double dt) {
    list_t *shape = body->shape;
    vector_t velocity_old = body->velocity;
    body->acceleration = vec_multiply(1/body->mass, body->force);

    vector_t velocity_new = vec_add(velocity_old, vec_multiply(dt, body->acceleration));
    body->velocity = vec_add(velocity_new, vec_multiply(1/body->mass, body->impulse));

    vector_t average = vec_multiply(0.5, vec_add(velocity_old, body->velocity));

    body_set_centroid(body, vec_add(body->centroid, vec_multiply(dt, average)));
    polygon_rotate(shape, body->rotation_speed * dt, body->centroid);
    body->force = VEC_ZERO;
    body->impulse = VEC_ZERO;
}

void body_remove(body_t *body) {
    body->remove = true;
}

bool body_is_removed(body_t *body) {
    return body->remove;
}

vector_t body_get_force(body_t *body) {
    return body->force;
}

vector_t body_get_impulse(body_t *body) {
    return body->impulse;
}

void body_set_image(body_t *body, const char *filename) {
  SDL_Surface *image = IMG_Load(filename);
  body->image = image;
}

SDL_Surface *body_get_image(body_t *body) {
  return body->image;
}

void body_set_texture(body_t *body, SDL_Texture *texture) {
  body->texture = texture;
}

SDL_Texture *body_get_texture(body_t *body) {
  return body->texture;
}

double body_get_radius(body_t *body) {
  return body->radius;
}

void body_set_radius(body_t *body, double radius) {
  body->radius = radius;
}