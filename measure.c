#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <stdbool.h> 
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>

#include <papi.h>

#include <time.h>

#include "measure.h"

#define NUMBER_OF_EVENTS 11
#define MAX_COUNTERS     2

__attribute__ ((visibility("default"))) 
const int N_EVENTS                  = NUMBER_OF_EVENTS+1;
static int events[NUMBER_OF_EVENTS+1][MAX_COUNTERS+1];
static long long meas[MAX_COUNTERS] = {0, 0};
static int event_set                = PAPI_NULL;

__attribute__ ((visibility("default")))
int measure(char *test, char *name, char *size, testfunc fp, void *up, int ups) {
	uint8_t  i, j;
	uint64_t total_ns, total_s, total;
	struct timespec begin, end;
	bool time = false; 
	bool warmedup   = false;
	bool OK  = true;

	printf("%s,%s,%s", test, name, size);

	for (i = 0; i <= N_EVENTS; i++) {

		for (j = 1; j <= events[i-warmedup][0]; j++) {
			meas[j-1] = 0;
			if ( PAPI_query_event(events[i-warmedup][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				if ( warmedup ) {
					printf(",-");
				}
			}
			continue;
		}

		PAPI_add_events(event_set, &events[i-warmedup][1], events[i-warmedup][0]);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &begin);
		}

		PAPI_start(event_set);

		for (j = 0; j < ups; j++) {
			(fp)(up);
		}

		PAPI_stop(event_set, meas);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &end);

			if (begin.tv_nsec <= end.tv_nsec) {
				total_ns = end.tv_nsec - begin.tv_nsec;
				total_s  = end.tv_sec  - begin.tv_sec;
			} else {
				total_ns = end.tv_nsec + (1e9 - begin.tv_nsec);
				total_s  = end.tv_sec - begin.tv_sec - 1;
			}

			total = total_s * 1e9 + total_ns;
			if ( warmedup ) {
				printf(",%f", (double)total/ups);
				time = true;
			}
		}

		if ( warmedup ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				printf(",%lld", meas[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	printf("\n");

	return 1;
}

__attribute__ ((visibility("default")))
int measure_with_sideeffects(char *test, char *name, char *size, 
		testfunc fp, void **up, int ups) {
	uint8_t  i, j;
	uint64_t total_ns, total_s, total;
	struct timespec begin, end;
	bool time = false; 
	bool warmedup   = false;
	bool OK   = true;

	printf("%s,%s,%s", test, name, size);

	for (i = 0; i <= N_EVENTS; i++) {

		for (j = 1; j <= events[i-warmedup][0]; j++) {
			meas[j-1] = 0;
			if ( PAPI_query_event(events[i-warmedup][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				if ( warmedup ) {
					printf(",-");
				}
			}
			continue;
		}

		PAPI_add_events(event_set, &events[i-warmedup][1], events[i-warmedup][0]);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &begin);
		}

		PAPI_start(event_set);

		for (j = 0; j < ups; j++) {
			(fp)(up[MEASURE_IDX(ups, i, j)]);
		}

		PAPI_stop(event_set, meas);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &end);

			if (begin.tv_nsec <= end.tv_nsec) {
				total_ns = end.tv_nsec - begin.tv_nsec;
				total_s  = end.tv_sec  - begin.tv_sec;
			} else {
				total_ns = end.tv_nsec + (1e9 - begin.tv_nsec);
				total_s  = end.tv_sec - begin.tv_sec - 1;
			}

			total = total_s * 1e9 + total_ns;
			if ( warmedup ) {
				printf(",%f", (double)total/ups);
				time = true;
			}
		}

		if ( warmedup ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				printf(",%lld", meas[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	printf("\n");

	return 1;
}


__attribute__ ((visibility("default")))
int measure_with_sideeffects_and_values(char *test, char *name, char *size, 
		testfunc fp, void **up, int ups, void **cleanup) {
	uint8_t  i, j;
	uint64_t total_ns, total_s, total;
	struct timespec begin, end;
	bool time = false; 
	bool warmedup   = false;
	bool OK   = true;

	printf("%s,%s,%s", test, name, size);

	for (i = 0; i <= N_EVENTS; i++) {

		for (j = 1; j <= events[i-warmedup][0]; j++) {
			meas[j-1] = 0;
			if ( PAPI_query_event(events[i-warmedup][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				if ( warmedup ) {
					printf(",-");
				}
			}
			continue;
		}

		PAPI_add_events(event_set, &events[i-warmedup][1], events[i-warmedup][0]);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &begin);
		}

		PAPI_start(event_set);

		for (j = 0; j < ups; j++) {
			cleanup[MEASURE_IDX(ups, i, j)] = 
				(fp)(up[MEASURE_IDX(ups, i, j)]);
		}

		PAPI_stop(event_set, meas);

		if ( !time ) {
			clock_gettime(CLOCK_REALTIME, &end);

			if (begin.tv_nsec <= end.tv_nsec) {
				total_ns = end.tv_nsec - begin.tv_nsec;
				total_s  = end.tv_sec  - begin.tv_sec;
			} else {
				total_ns = end.tv_nsec + (1e9 - begin.tv_nsec);
				total_s  = end.tv_sec - begin.tv_sec - 1;
			}

			total = total_s * 1e9 + total_ns;
			if ( warmedup ) {
				printf(",%f", (double)total/ups);
				time = true;
			}
		}

		if ( warmedup ) {
			for (j = 1; j <= events[i-warmedup][0]; j++) {
				printf(",%lld", meas[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	printf("\n");

	return 1;
}

__attribute__ ((visibility("default")))
int measure_init() {
	uint8_t i, j;
	char buf[1024];
	
	events[0][0] = MAX_COUNTERS;
	events[0][1] = PAPI_TOT_INS;
	events[0][2] = PAPI_TOT_CYC;

	events[1][0] = MAX_COUNTERS;
	events[1][1] = PAPI_BR_NTK;
	events[1][2] = PAPI_BR_TKN;

	events[2][0] = MAX_COUNTERS;
	events[2][1] = PAPI_BR_MSP;
	events[2][2] = PAPI_BR_PRC;

	events[3][0] = MAX_COUNTERS;
	events[3][1] = PAPI_L1_DCM;
	events[3][2] = PAPI_L1_ICM;

	events[3][0] = MAX_COUNTERS;
	events[3][1] = PAPI_L1_DCM;
	events[3][2] = PAPI_L1_ICM;

	events[4][0] = MAX_COUNTERS;
	events[4][1] = PAPI_L2_DCM;
	events[4][2] = PAPI_L2_ICM;

	events[5][0] = MAX_COUNTERS;
	events[5][1] = PAPI_TLB_DM;
	events[5][2] = PAPI_TLB_IM;

	events[6][0] = 1;
	events[6][1] = PAPI_L1_DCH;

	events[7][0] = 1;
	events[7][1] = PAPI_L2_DCH;

	events[8][0] = 1;
	events[8][1] = PAPI_L1_ICH;

	events[9][0] = 1;
	events[9][1] = PAPI_L2_ICH;

	events[10][0] = 1;
	events[10][1] = PAPI_REF_CYC;


	if ( PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		return 0;
	}

	printf("Test,Name,Method,Nanoseconds");
	for (i = 0; i < NUMBER_OF_EVENTS; i++) {
		for (j = 1; j <= events[i][0]; j++) {
			PAPI_event_code_to_name(events[i][j], buf);
			printf(",%s", buf);
		}
	}
	printf("\n");

	if ( PAPI_create_eventset(&event_set) != PAPI_OK) {
		return 0;
	}

	return 1;
}

__attribute__ ((visibility("default")))
int measure_destroy() {

	if ( PAPI_destroy_eventset(&event_set) != PAPI_OK ) {
		return 0;
	}

	PAPI_shutdown();
	return 1;
}
