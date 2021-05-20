// TODO: IMPLEMENT YOUR TESTS IN THIS FILE
#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "color.h"

list_t *make_shape() {
    list_t *shape = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){-1, +1};
    list_add(shape, v);
    return shape;
}

void test_drag() {
  const double M = 10;
  const double k = rand() % 10 / 10.0;
  const double init_vel = 3;
  const double DT = 1e-6;
  const int STEPS = 1000000;
  double elapsed_time = 0;
  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), M, (rgb_color_t) {0, 0, 0});
  body_set_velocity(mass, (vector_t) {init_vel, 0});
  scene_add_body(scene, mass);
  create_drag(scene, k, mass);
  for (int i = 0; i < STEPS; i++) {
      elapsed_time= i * DT;
      assert(vec_isclose(
          body_get_velocity(mass),
          (vector_t) {init_vel * pow(M_E, -k * elapsed_time / M), 0}
      ));
      scene_tick(scene, DT);
  }
  scene_free(scene);
}

double spring_energy(body_t *b1, body_t *b2, double k){
    vector_t vec_distance = vec_subtract(body_get_centroid(b1), body_get_centroid(b2));
    return .5 * k * vec_dot(vec_distance, vec_distance);

}
double kinetic_energy(body_t *body) {
    vector_t v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

void test_spring_energy(){
    const double M = 10;
    const double K = 2;
    const double A = 3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), M, (rgb_color_t) {0, 0, 0});
    body_set_centroid(mass1, (vector_t) {A, 0});
    scene_add_body(scene, mass1);
    body_t *mass2 = body_init(make_shape(), M, (rgb_color_t) {0, 0, 0});
    body_set_centroid(mass2, (vector_t) {-A, 0});
    scene_add_body(scene, mass2);
    create_spring(scene, K, mass1, mass2);
    double initial_energy = spring_energy(mass1, mass2, K);
    for(int i = 0; i < STEPS; i++){
        double new_energy = spring_energy(mass1, mass2, K) + kinetic_energy(mass1) + kinetic_energy(mass2);
        assert(within(1e-1, new_energy, initial_energy));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

void test_gravity(){
    const double M1 = 1, M2 = 730000;
    const double G = 1e3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *black_hole = body_init(make_shape(), M2, (rgb_color_t) {0, 0, 0});
    scene_add_body(scene, black_hole);
    body_t *planet = body_init(make_shape(), M1, (rgb_color_t) {0, 0, 0});
    scene_add_body(scene, planet);
    create_newtonian_gravity(scene, G, black_hole, planet);
    for(int i = 0; i < STEPS; i++){
        vector_t v = vec_subtract(body_get_centroid(black_hole), body_get_centroid(planet));
        double magnitude = pow(vec_dot(v,v), 0.5);
        assert(within(1e-1, magnitude, 0));
        scene_tick(scene, DT);
    }
    scene_free(scene); 
}

int main(int argc, char *argv[]) {
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
      read_testname(argv[1], testname, sizeof(testname));
  }
    DO_TEST(test_drag)
    DO_TEST(test_spring_energy)
    DO_TEST(test_gravity)
    return 0;
}
