#ifndef H_MEASURE
#define H_MEASURE

#include <inttypes.h>

// Testfunction is the argument that needs to be tested. Should only take a
// single argument.
typedef void *(*testfunc)(void *, ...);

int measure_init();
int measure(char *test, char *name, char *testfile, testfunc fp, void *up, int ups);
int measure_multi(char *test, char* name, char *testfile, testfunc fp, void ***up, int ups);
int measure_multi_cleanup(char *test, char* name, char *testfile, testfunc fp, void ***up, int ups, void ***cleanup);
void measure_destroy();
#endif
