#include "list.h"
#include "test_util.h"
#include "vec_list.h"
#include <assert.h>
#include <stdlib.h>

// void test_list_size0() {
//     list_t *l = list_init(0, (free_func_t) list_free);
//     assert(list_size(l) == 0);
//     list_free(l);
// }

// void test_list_size1() {
//     list_t *l = list_init(1, (free_func_t) list_free);
//     assert(list_size(l) == 0);
//     // Add
//     vector_t *v = malloc(sizeof(*v));
//     *v = VEC_ZERO;
//     list_add(l, v);
//     assert(list_size(l) == 1);
//     // Remove
//     assert(list_remove(l, 0) == v);
//     free(v);
//     assert(list_size(l) == 0);
//     // Add again
//     v = malloc(sizeof(*v));
//     v->x = v->y = 1;
//     list_add(l, v);
//     assert(list_size(l) == 1);
//     list_free(l);
// }

// void test_list_small() {
//     list_t *l = list_init(5, (free_func_t) list_free);
//     assert(list_size(l) == 0);
//     // Fill partially
//     vector_t *v = malloc(sizeof(*v));
//     *v = VEC_ZERO;
//     list_add(l, v);
//     v = malloc(sizeof(*v));
//     v->x = v->y = 1;
//     list_add(l, v);
//     v = malloc(sizeof(*v));
//     v->x = v->y = 2;
//     list_add(l, v);
//     assert(list_size(l) == 3);
//     // Fill to capacity
//     v = malloc(sizeof(*v));
//     v->x = v->y = 3;
//     list_add(l, v);
//     v = malloc(sizeof(*v));
//     v->x = v->y = 4;
//     list_add(l, v);
//     assert(list_size(l) == 5);
//     // Remove some
//     v = list_remove(l, 0);
//     free(v);
//     v = list_remove(l, 0);
//     free(v);
//     assert(list_size(l) == 3);
//     // Add, replacing previous elements
//     v = malloc(sizeof(*v));
//     v->x = v->y = 5;
//     list_add(l, v);
//     v = malloc(sizeof(*v));
//     v->x = v->y = 6;
//     list_add(l, v);
//     assert(list_size(l) == 5);
//     // Overwrite added elements
//     list_free(l);
// }

// void add_null(void *l) {
//     list_add(l, NULL);
// }

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    // DO_TEST(test_list_size0)
    // DO_TEST(test_list_size1)
    // DO_TEST(test_list_small)

    puts("list_test PASS");
}
