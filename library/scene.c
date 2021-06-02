#include "scene.h"
#include <stdio.h>

const int guess = 13;

typedef struct scene{
    list_t *bodies;
    size_t size;
    size_t capacity;
    list_t *force_creator_list;
    char info;
    size_t p1_num;
    size_t p2_num;
    player_t *player1;
    player_t *player2;
    list_t *textures;
    SDL_Texture *bkg;
    SDL_Surface *bkg_image;
    Mix_Music *bkg_sound;
} scene_t;

typedef struct forcer {
    force_creator_t force;
    void *aux;
    free_func_t freer;
    list_t *bodies;
} forcer_t;

void forcer_free(forcer_t *force) {
    if (force->freer != NULL) {
        force->freer(force->aux);
    }
    list_free_wo_data(force->bodies);
    free(force);
}

void scene_set_info(scene_t *scene, char info) {
    scene->info = info;
}

char scene_get_info(scene_t *scene) {
    return scene->info;
}

scene_t *scene_init(void) {
    scene_t *scene = malloc(sizeof(scene_t));
    scene->bodies = list_init(guess, (free_func_t) body_free);
    scene->size = 0;
    scene->capacity = guess;
    scene->force_creator_list = list_init(1, free);
    scene->textures = list_init(1, free);
    scene->info = ' ';
    scene->bkg = NULL;
    scene->bkg_image = NULL;
    scene->bkg_sound = NULL;
    scene->p1_num = 1;
    scene->p2_num = 4;
    scene->player1 = NULL;
    scene->player2 = NULL;
    assert(scene != NULL);
    assert(scene->bodies != NULL);
    return scene;
}

void scene_set_p1(scene_t *scene, size_t player) {
    scene->p1_num = player;
}

size_t scene_get_p1(scene_t *scene) {
    return scene->p1_num;
}

void scene_set_p2(scene_t *scene, size_t player) {
    scene->p2_num = player;
}

size_t scene_get_p2(scene_t *scene) {
    return scene->p2_num;
}

void scene_set_player1(scene_t *scene, player_t *player) {
    scene->player1 = player;
}

player_t *scene_get_player1(scene_t *scene) {
    return scene->player1;
}

void scene_set_player2(scene_t *scene, player_t *player) {
    scene->player2 = player;
}

player_t *scene_get_player2(scene_t *scene) {
    return scene->player2;
}

void scene_free(scene_t *scene) {
    for(size_t i = 0; i < list_size(scene->force_creator_list); i++){
        list_t *new_force_creator_list = (list_t *) scene->force_creator_list;
        forcer_t *forcers = list_get(new_force_creator_list, i);
        if(forcers->freer != NULL){
            forcers->freer(forcers->aux);
        }
    }
    Mix_CloseAudio();
    Mix_FreeMusic(scene_get_bkg_sound(scene));
    list_free(scene->force_creator_list);
    list_free(scene->bodies);
    if (scene->player1 != NULL) {
        free(scene->player1);
    }
    if (scene->player2 != NULL) {
        free(scene->player2);
    }
    free(scene);
}

void scene_set_bkg(scene_t *scene, SDL_Texture *texture) {
  scene->bkg = texture;
}

SDL_Texture *scene_get_bkg(scene_t *scene) {
  return scene->bkg;
}

void scene_set_bkg_image(scene_t *scene, const char *filename) {
  SDL_Surface *image = IMG_Load(filename);
  scene->bkg_image = image;
}

SDL_Surface *scene_get_bkg_image(scene_t *scene) {
  return scene->bkg_image;
}

void scene_set_bkg_sound(scene_t *scene, Mix_Music *sound) {
  scene->bkg_sound = sound;
}

Mix_Music *scene_get_bkg_sound(scene_t *scene) {
  return scene->bkg_sound;
}

size_t scene_bodies(scene_t *scene) {
    return scene->size;
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    assert(index < scene->size);
    return (body_t *) list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
    assert(body != NULL);
    if (scene->size >= scene->capacity) {
        scene->bodies = (list_t*) realloc(scene->bodies, scene->capacity * 2 * sizeof(body_t*));
        scene->capacity *= 2;
    }
    list_add(scene->bodies, body);
    scene->size++;
}

void scene_remove_body(scene_t *scene, size_t index) {
    assert(index < scene->size);
    body_remove(scene_get_body(scene, index));
}

void scene_tick(scene_t *scene, double dt) {
    for (size_t i = 0; i < list_size(scene->force_creator_list); i++) {
        forcer_t *force = (forcer_t*) list_get(scene->force_creator_list, i);
        force_creator_t force_creator = force->force;
        force_creator(force->aux);
    }
    for (size_t k = 0; k < scene->size; k++) {
        body_t *body = list_get(scene->bodies, k);
        if (body_is_removed(body)) {
            for (size_t i = 0; i < list_size(scene->force_creator_list); i++) {
                forcer_t *force = list_get(scene->force_creator_list, i);
                for (size_t j = 0; j < list_size(force->bodies); j++) {
                    if (list_get(force->bodies, j) == body) {
                        list_remove(scene->force_creator_list, i);
                        forcer_free(force);
                        i--;
                        break;
                    }
                }
            }
            body_free(list_remove(scene->bodies, k));
            scene->size--;
            k--;
        }
        else { body_tick(body, dt); }
    }
}

forcer_t *forcer_t_init(force_creator_t forcer, void* aux, free_func_t freer, list_t *bodies) {
    forcer_t *force = malloc(sizeof(forcer_t));
    force->force = forcer;
    force->aux = aux;
    force->freer = freer;
    force->bodies = bodies;
    return force;
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer) {
    forcer_t *force = forcer_t_init(forcer, aux, freer, bodies);
    list_add(scene->force_creator_list, force);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux, free_func_t freer) {
    list_t *bodies = list_init(0, (free_func_t) free);
    forcer_t *force = forcer_t_init(forcer, aux, freer, bodies);
    list_add(scene->force_creator_list, force);
}

list_t *scene_textures_list(scene_t *scene) {
  return scene->textures;
}

void scene_add_texture(scene_t *scene, SDL_Texture *texture) {
  list_add(scene->textures, texture);
}