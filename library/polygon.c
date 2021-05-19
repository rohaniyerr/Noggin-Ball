#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "polygon.h"


double polygon_area(list_t *polygon) {
    double area = 0;
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        vector_t *v2 = list_get(polygon, (i+1) % list_size(polygon));
        area += (v1->x*v2->y);
        area -= (v2->x*v1->y);
    }
    return fabs(area)/2.0;
}


vector_t polygon_centroid(list_t *polygon) {
    double area = polygon_area(polygon);
    vector_t centroid;
    centroid.x = 0;
    centroid.y = 0;
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        vector_t *v2 = list_get(polygon, (i+1) % list_size(polygon));
        centroid.x += (v1->x + v2->x)*(vec_cross(*v1, *v2));
        centroid.y += (v1->y + v2->y)*(vec_cross(*v1, *v2));
    }
    centroid.x = centroid.x / (6*area);
    centroid.y = centroid.y / (6*area);
    return centroid;
}


void polygon_translate(list_t *polygon, vector_t translation) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get(polygon, i);
        v->x += translation.x;
        v->y += translation.y;
    }
}


void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get(polygon, i);
        *v = vec_subtract(*v, point);
        *v = vec_rotate(*v, angle);
        *v = vec_add(*v, point);
    }
}