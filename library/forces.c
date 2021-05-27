#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "forces.h"
#include "color.h"

typedef struct aux {
    body_t *body1;
    body_t *body2;
    void *c;
} aux_t;

aux_t *aux_init(body_t *body1, body_t *body2, void *c) {
    aux_t *aux = malloc(sizeof(aux_t));
    aux->body1 = body1;
    aux->body2 = body2;
    aux->c = c;
    return aux;
}

typedef struct collision_aux {
    body_t *body1;
    body_t *body2;
    void *aux;
    collision_handler_t handler;
    bool collided;
} collision_aux_t;

collision_aux_t *collision_info_init(body_t *body1, body_t *body2, void *aux, collision_handler_t handler) {
    collision_aux_t *c = malloc(sizeof(collision_aux_t));
    c->body1 = body1;
    c->body2 = body2;
    c->aux = aux;
    c->handler = handler;
    c->collided = false;
    return c;
}

void force_creator_spring(void *aux) {
    aux_t *a = (aux_t*) aux;
    body_t *body1 = a->body1;
    body_t *body2 = a->body2;
    double *c = (double *) a->c;
    vector_t displacement = vec_subtract(body_get_centroid(body1) , body_get_centroid(body2));
    double distance = vec_magnitude(displacement);
    double F = *c * distance;
    vector_t direction = vec_multiply(1/distance, displacement);
    body_add_force(body1, vec_multiply(-F, direction));
    body_add_force(body2, vec_multiply(F, direction));
}

void force_creator_drag(void *aux) {
    aux_t *a = (aux_t*) aux;
    body_t *body = a->body1;
    double *c = (double *) a->c;
    vector_t velocity = body_get_velocity(body);
    body_add_force(body, vec_multiply(-(*c), velocity));
}

void force_creator_newtonian_gravity(void *aux) {
    aux_t *a = (aux_t*) aux;
    body_t *body1 = a->body1;
    body_t *body2 = a->body2;
    double *c = (double *) a->c;

    vector_t displacement = vec_subtract(body_get_centroid(body1) , body_get_centroid(body2));
    double distance = vec_magnitude(displacement);
    if (distance < 2) { distance = 2; } 
    double F = *c * body_get_mass(body1) * body_get_mass(body2) / pow(distance, 2);
    vector_t direction = vec_multiply(1/distance, displacement);
    body_add_force(body1, vec_multiply(-F, direction));
    body_add_force(body2, vec_multiply(F, direction));
}

void force_creator_planet_gravity(void *aux) {
    aux_t *a = (aux_t*) aux;
    body_t *body = a->body1;
    vector_t *g = (vector_t *) a->c;
    vector_t velocity = body_get_velocity(body);
    body_add_force(body, vec_add(*g, velocity));
}

void handler_destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body1);
    body_remove(body2);
}

void handler_color_change(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    list_t *colors = (list_t *) aux;
    rgb_color_t *brick_color = malloc(sizeof(rgb_color_t));
    *brick_color = body_get_color(body1);
    for (size_t i = 0; i < list_size(colors); i++) {
        rgb_color_t *color = list_get(colors, i);
        if ((color->r == brick_color->r) && (color->g == brick_color->g) && (color->b == brick_color->b)) {
            if (i == 0) {
                body_remove(body1);
            }
            else {
                rgb_color_t *new_color = list_get(colors, i - 1);
                body_set_color(body1, new_color);
                break;
            }
        }
    }
    free(brick_color);
}

void handler_physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    double *elasticity = (double*) aux;

    double reduced_mass = body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2));
    double u_b = vec_dot(body_get_velocity(body2), axis);
    double u_a = vec_dot(body_get_velocity(body1), axis);
    if (body_get_mass(body1) == INFINITY) {
        reduced_mass = body_get_mass(body2);
    }
    if (body_get_mass(body2) == INFINITY) {
        reduced_mass = body_get_mass(body1);
    }
    double impulse = reduced_mass * (1 + *elasticity) * (u_b - u_a);
    if (body_get_mass(body1) == INFINITY && body_get_mass(body2) == INFINITY) {impulse = 0;}
    body_add_impulse(body2, (vector_t) vec_multiply(-impulse, axis));
    body_add_impulse(body1, (vector_t) vec_multiply(impulse, axis));
}

void force_creator_normal_force(void *aux) {
    aux_t *a = (aux_t*) aux;
    body_t *body = a->body1;
    body_t *floor = a->body2;
    collision_info_t info = find_collision(body_get_shape(body), body_get_shape(floor));
    if (info.collided) {
        vector_t force = body_get_force(body);
        vector_t normal_f = {.x = 0, .y = -force.y};
        body_add_force(body, normal_f);
        vector_t impulse = body_get_impulse(body);
        vector_t normal_i = {.x = 0, .y = -impulse.y};
        body_add_impulse(body, normal_i);
    }
}

void handler_physics_one_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    double *elasticity = (double*) aux;
    double reduced_mass = body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2));
    double u_b = vec_dot(body_get_velocity(body2), axis);
    double u_a = vec_dot(body_get_velocity(body1), axis);
    if (body_get_mass(body1) == INFINITY) {
        reduced_mass = body_get_mass(body2);
    }
    if (body_get_mass(body2) == INFINITY) {
        reduced_mass = body_get_mass(body1);
    }
    double impulse = reduced_mass * (1 + *elasticity) * (u_b - u_a);
    if (fabs(u_b - u_a) < 25) {
        if (impulse < 0) {
            impulse = -5000;
            body_add_force(body2, (vector_t) {-100, 0});
        }
        else {
            impulse = 5000;
            body_add_force(body2, (vector_t) {100, 0});
        }
    }
    if (body_get_mass(body1) == INFINITY && body_get_mass(body2) == INFINITY) {impulse = 0;}
    body_add_impulse(body2, (vector_t) vec_multiply(-2*impulse, axis));
} 

void force_creator_collision(void *aux) {
    collision_aux_t *info = (collision_aux_t*) aux;
    body_t *body1 = info->body1;
    body_t *body2 = info->body2;
    void *c = info->aux;
    collision_handler_t handler = info->handler;
    collision_info_t collision_info = find_collision(body_get_shape(body1), body_get_shape(body2));
    if (collision_info.collided && !info->collided) {
        info->collided = true;
        handler(body1, body2, collision_info.axis, c);
    }
    else if (!collision_info.collided) {
        info->collided = false;
    }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2) {
    double *gr = malloc(sizeof(double));
    *gr = G;
    aux_t *aux = aux_init(body1, body2, gr);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, force_creator_newtonian_gravity, aux, bodies, NULL);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    double *K = malloc(sizeof(double));
    *K = k;
    aux_t *aux = aux_init(body1, body2, (void*) K);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, force_creator_spring, aux, bodies, NULL);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    double *g = malloc(sizeof(double));
    *g = gamma;
    aux_t *aux = aux_init(body, NULL, (void *) g);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body);
    scene_add_bodies_force_creator(scene, force_creator_drag, aux, bodies, free);
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2, collision_handler_t handler, void *aux, free_func_t freer) {
    collision_aux_t *info = collision_info_init(body1, body2, aux, handler);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, force_creator_collision, info, bodies, freer);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2) {
    double *e = malloc(sizeof(double));
    *e = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t) handler_physics_collision, (void *) e, free);
}

void create_physics_one_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2) {
    double *e = malloc(sizeof(double));
    *e = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t) handler_physics_one_collision, (void *) e, free);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    create_collision(scene, body1, body2, (collision_handler_t) handler_destructive_collision, NULL, NULL);
}

void create_color_changer(scene_t *scene, list_t *colors, body_t *body1, body_t *body2) {
    create_collision(scene, body1, body2, (collision_handler_t) handler_color_change, (void *) colors, NULL);
}

void create_planet_gravity(scene_t *scene, vector_t gravity, body_t *body) {
    vector_t *g = malloc(sizeof(vector_t));
    *g = gravity;
    aux_t *aux = aux_init(body, NULL, (void *) g);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body);
    scene_add_force_creator(scene, force_creator_planet_gravity, aux, (free_func_t) free);
}

void create_normal_force(scene_t *scene, body_t *body, body_t *floor) {
    aux_t *aux = aux_init(body, floor, NULL);
    list_t *bodies = list_init(2, (free_func_t) free);
    list_add(bodies, body);
    scene_add_force_creator(scene, force_creator_normal_force, aux, (free_func_t) free);
}