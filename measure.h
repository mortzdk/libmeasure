#ifndef H_MEASURE
#define H_MEASURE

#include <inttypes.h>

#define MEASURE_IDX(WIDTH, x, y) ((WIDTH) * (x)) + (y)

// Testfunction is the argument that needs to be tested. Should only take a
// single argument.
typedef void *(*testfunc)(void *);

int measure_init();
int measure(char *restrict test, char *restrict name, char *restrict size, 
		testfunc fp, void *restrict up, const int ups);
int measure_with_sideeffects(char *restrict test, char *restrict name, 
		char *restrict size, testfunc fp, void **restrict up, const int ups);
int measure_with_sideeffects_and_values(char *restrict test, 
		char *restrict name, char *restrict size, testfunc fp, 
		void **restrict up, const int ups, void **restrict cleanup);
int measure_destroy();
#endif
