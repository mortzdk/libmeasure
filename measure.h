#ifndef H_MEASURE
#define H_MEASURE

#include <inttypes.h>

#define MEASURE_IDX(WIDTH, x, y) ((WIDTH) * (x)) + (y)

// Testfunction is the argument that needs to be tested. Should only take a
// single argument.
typedef void *(*testfunc)(void *, ...);

int measure_init();
int measure(char *test, char *name, char *testfile, testfunc fp, void *up, 
		int ups);
int measure_with_sideeffects(char *test, char* name, char *testfile, 
		testfunc fp, void **up, int ups);
int measure_with_sideeffects_and_values(char *test, char* name, char *testfile, 
		testfunc fp, void **up, int ups, void **cleanup);
int measure_destroy();
#endif
