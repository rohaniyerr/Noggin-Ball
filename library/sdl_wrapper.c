#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "sdl_wrapper.h"
#include "body.h"
#include "polygon.h"

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

bool is_SDL_image = false;

double IMG_SCALE = 1.0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:  return LEFT_ARROW;
        case SDLK_UP:    return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN:  return DOWN_ARROW;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    TTF_Init();
    renderer = SDL_CreateRenderer(window, -1, 0);
}

bool sdl_is_done(void *scene) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                free(event);
                return true;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(key, type, held_time, scene);
                break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

vector_t change_coordinate(vector_t v, vector_t new_center, double center_x, double center_y, double scale) {
    vector_t pos = vec_multiply(scale, vec_subtract(v, new_center));
    return (vector_t){center_x + pos.x, center_y - pos.y};
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}


bool on_screen(vector_t v, double radius) {
    if (v.x - radius > WINDOW_WIDTH)  return false;
    if (v.x + radius < -WINDOW_WIDTH)   return false;
    if (v.y - radius > WINDOW_HEIGHT) return false;
    if (v.y + radius < -WINDOW_HEIGHT)  return false;
    return true;
}

void sdl_draw_polygon_body(body_t *body, rgb_color_t color) { // changed this to take a body instead of list
    list_t *points = body_get_shape(body);
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    // Make the pic fit the scene
    int *width = malloc(sizeof(int)),*height = malloc(sizeof(int));
    // assert(width);
    // assert(height);
    SDL_GetWindowSize(window, width, height);
    double center_x = *width / 2.0,
           center_y = *height / 2.0;
    free(width);
    free(height);
    double x_scale = center_x / max_diff.x,
           y_scale = center_y / max_diff.y;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    // Convert each vertex to a point on screen
    short *x_points = malloc(sizeof(short) * n);
    short *y_points = malloc(sizeof(short) * n); //idk i just stack overflowed this
    // assert(x_points);
    // assert(y_points);
    vector_t new_center = center;

    //make body if on screen
    vector_t adjusted_centroid = polygon_centroid(points);
    double radius = body_get_radius(body) * scale * sqrt(2); // pythag
    vector_t pos = vec_multiply(scale, vec_subtract(adjusted_centroid, new_center));
    if (!on_screen((vector_t){center_x + pos.x, center_y - pos.y}, radius)) {
        return;
    }
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t point = change_coordinate(*vertex, new_center, center_x, center_y, scale);
        x_points[i] = point.x;
        y_points[i] = point.y;
    }

    if (!body_get_image(body) || !is_SDL_image) {
        // Draw polygon with the given color
        filledPolygonRGBA(
            renderer,
            x_points, y_points, n,
            color.r * 255, color.g * 255, color.b * 255, 255
        );
        free(x_points);
        free(y_points);
        list_free(points);
    }

    else if (is_SDL_image) {
        vector_t centroid = body_get_centroid(body);
        SDL_Rect *dest = malloc(sizeof(SDL_Rect));
        int radius = (int) body_get_radius(body);
        vector_t corner = VEC_ZERO;
        corner = (vector_t) {(centroid.x - radius), (centroid.y + radius)};
        corner = change_coordinate(corner, new_center, center_x, center_y, scale);
        dest->x = corner.x;
        dest->y = corner.y;
        dest->w = 2 * scale * radius * IMG_SCALE;
        dest->h = 2 * scale * radius * IMG_SCALE;

        // Initializes and renders texture from body->image.
        SDL_Texture *texture = body_get_texture(body);
        SDL_RenderCopy(renderer, texture, NULL, dest);
        free(dest);
    }
}

void sdl_show(void) {
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff), min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center), min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);
    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene) {
    sdl_clear();
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        list_t *shape = body_get_shape(body);
        sdl_draw_polygon(shape, body_get_color(body));
        list_free(shape);
    }
    if (is_SDL_image) {
      SDL_Texture *bkg = scene_get_bkg(scene);
      SDL_Rect *dest = malloc(sizeof(SDL_Rect));
      dest->x = 0;
      dest->y = 0;
      dest->w = 1000;
      dest->h = 1000;
      SDL_RenderCopy(renderer, bkg, NULL, dest);
      free(dest);
    }
    sdl_show();
}

void sdl_render_game_scene(scene_t *scene, char *p1, char *p2, char *timer, char *winner, SDL_Rect *p1_scoreboard, SDL_Rect *p2_scoreboard, SDL_Rect *time_board, SDL_Rect *disp_winner, TTF_Font *font, SDL_Color color) {
    sdl_clear();
    if (is_SDL_image) {
      SDL_Texture *bkg = scene_get_bkg(scene);
      SDL_Rect *dest = malloc(sizeof(SDL_Rect));
      dest->x = 0;
      dest->y = 0;
      dest->w = 1000;
      dest->h = 1000;
      SDL_RenderCopy(renderer, bkg, NULL, dest);
      free(dest);
    }
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t original_body_position = body_get_centroid(body);
        sdl_draw_polygon_body(body, body_get_color(body));
        body_set_centroid(body, original_body_position);
    }
    make_scoreboard(p1, p2, timer, p1_scoreboard, p2_scoreboard, time_board, font, color);
    if(strcmp(timer, "0") == 0){
        display_winner(winner, disp_winner, font, color);
    }
    sdl_show();
}

void sdl_init_textures(scene_t *s) {
    int *width = malloc(sizeof(int)), *height = malloc(sizeof(int));
    SDL_GetWindowSize(window, width, height);
    // double center_x = *width / 2.0, center_y = *height / 2.0;
    // double x_scale = center_x / max_diff.x, y_scale = center_y / max_diff.y;
    // double scale = x_scale < y_scale ? x_scale : y_scale;
    if (scene_get_bkg_image(s)) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, scene_get_bkg_image(s));
        scene_set_bkg(s, texture);
    }
    for (int i = 0; i < scene_bodies(s); i ++) {
        body_t *body = scene_get_body(s, i);
        if (body_get_image(body)) {
            SDL_Rect *dest = malloc(sizeof(SDL_Rect));
            // Make texture.
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, body_get_image(body));
            body_set_texture(body, texture);
            free(dest);
        }
    }
}

void make_SDL_image(void) {
  is_SDL_image = !is_SDL_image;
}

void sdl_on_key(key_handler_t handler) {
    key_handler = handler;
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}

SDL_Rect *sdl_rect_init(int x, int y, int w, int h) {
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
    return rect;
}

void generate_text(char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect) {
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_RenderCopy(renderer, message, NULL, rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(message);
}

void make_scoreboard(char *p1, char *p2, char *timer, SDL_Rect *p1_scoreboard, SDL_Rect *p2_scoreboard, SDL_Rect *time_board, TTF_Font *font, SDL_Color color) {
    generate_text(p1, font, color, p1_scoreboard);
    generate_text(p2, font, color, p2_scoreboard);
    generate_text(timer, font, color, time_board);
}

void display_winner(char *winner_message, SDL_Rect *winner_disp, TTF_Font *font, SDL_Color color){
    generate_text(winner_message, font, color, winner_disp);
}