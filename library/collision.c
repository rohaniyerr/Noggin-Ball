#include "collision.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void calculate_slope(list_t *shape, list_t *slopes) {
    for (size_t i = 0 ; i < list_size(shape); i++) {
        vector_t *vec_i = (vector_t*)list_get(shape, i);
        vector_t *vec_i_next = (vector_t*)list_get(shape, (i+1)%list_size(shape));
        vector_t diff = vec_subtract(*vec_i, *vec_i_next);
        vector_t *slope = malloc(sizeof(vector_t));
        slope->x = diff.y/vec_magnitude(diff); slope->y = -diff.x/vec_magnitude(diff); //unit vector
        list_add(slopes, slope);
    }
}

vector_t find_projection(list_t *shape, vector_t *slope) {
    vector_t min_max = {.x = INFINITY, .y = -INFINITY};
    for (size_t i = 0; i < list_size(shape); i++) {
        vector_t *vec_i = (vector_t *)list_get(shape, i);
        double dot = vec_dot(*vec_i, *slope);
        if (min_max.x > dot) {min_max.x = dot;}
        if (min_max.y < dot) {min_max.y = dot;}
    }
    return min_max;
}

bool intersecting_projections(vector_t s1, vector_t s2) {
    if (s1.x <= s2.x && s1.y >= s2.x) {return true;}
    else if (s1.x <=s2.y && s1.y>=s2.y) {return true;}
    else if (s2.x <= s1.x && s2.y >= s1.x) {return true;}
    else if (s2.x >=s1.x && s2.y<=s1.y) { return true;}
    return false;
}

vector_t find_collision_axis(list_t *shape1, list_t *shape2) {
    list_t *slopes = list_init(list_size(shape1) + list_size(shape2), (free_func_t)free);
    calculate_slope(shape1, slopes);
    calculate_slope(shape2, slopes);
    double overlap = INFINITY;
    vector_t axis = VEC_ZERO;
    for (size_t i = 0; i < list_size(slopes); i++) {
        vector_t s1 = find_projection(shape1, list_get(slopes, i));
        vector_t s2 = find_projection(shape2, list_get(slopes, i));
        double temp = 0;
        if (s1.x <= s2.x && s1.y >= s2.y) {
            temp = fabs(s2.y-s2.x);
        }
        else if (s2.x <= s1.x && s2.y >= s1.y) {
            temp = fabs(s1.y-s1.x);
        }
        else if (s1.x >= s2.x && s1.x <= s2.y){
            temp = fabs(s2.y - s1.x);
        }
        else if (s1.x <= s2.x && s2.x <= s1.y) {
            temp = fabs(s1.y-s2.x);
        }
        else {
            if (temp == 0) {
                return VEC_ZERO;
            }
        }
        if (temp < overlap) {
            overlap = temp;
            vector_t *temp_axis = (vector_t *) list_get(slopes, i);
            axis = *temp_axis;
        }
    }
    return axis;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
    vector_t collision_axis = find_collision_axis(shape1, shape2);
    if (collision_axis.x == 0 && collision_axis.y == 0) {
        collision_info_t collision_info;
        collision_info.collided = false;
        return collision_info;
    }
    collision_info_t collision_info;
    collision_info.collided = true;
    collision_info.axis = collision_axis;
    return collision_info;
}