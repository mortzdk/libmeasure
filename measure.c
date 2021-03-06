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

#define NUMBER_OF_EVENTS 10
#define MAX_COUNTERS     3

__attribute__ ((visibility("default"))) 
const int N_EVENTS                  = NUMBER_OF_EVENTS+2;
static int events[NUMBER_OF_EVENTS][MAX_COUNTERS+1];
static long long meas[MAX_COUNTERS] = {0, 0, 0};
static long long cnt[MAX_COUNTERS] = {0, 0, 0};
static int event_set                = PAPI_NULL;
static FILE *fd;

__attribute__ ((visibility("default")))
int measure(char *restrict test, char *restrict name, char *restrict size, 
		testfunc fp, void *restrict up, const int ups) {
	uint8_t  i, j, k;
	uint32_t idx;
	uint64_t total_ns = 0, total_s = 0, total;
	struct timespec begin, end;
	bool time     = false; 
	bool warmedup = false;
	bool OK       = true;

	fprintf(fd, "%s,%s,%s", test, name, size);

	for (i = 0; i < N_EVENTS; i++) {
		idx = i - warmedup - time;
		if ( warmedup && !time ) {
			for (j = 0; j < ups; j++) {
				clock_gettime(CLOCK_REALTIME, &begin);
				(fp)(up);
				clock_gettime(CLOCK_REALTIME, &end);
				if (begin.tv_nsec <= end.tv_nsec) {
					total_ns += end.tv_nsec - begin.tv_nsec;
					total_s  += end.tv_sec  - begin.tv_sec;
				} else {
					total_ns += end.tv_nsec + (1e9 - begin.tv_nsec);
					total_s  += end.tv_sec - begin.tv_sec - 1;
				}
			}

			total = total_s * 1e9 + total_ns;
			fprintf(fd, ",%f", (double)total/ups);
			time = true;
			continue;
		}

		for (j = 1; j <= events[idx][0]; j++) {
			if ( PAPI_query_event(events[idx][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[idx][0]; j++) {
				if ( warmedup ) {
					fprintf(fd, ",-");
				}
			}
			OK = true;
			continue;
		}

		for (j = 0; j < MAX_COUNTERS; j++) {
			meas[j] = 0;
			cnt[j] = 0;
		}

		PAPI_add_events(
				event_set, 
				&events[idx][1], 
				events[idx][0]
		);

		for (j = 0; j < ups; j++) {
			PAPI_start(event_set);
			(fp)(up);
			PAPI_stop(event_set, meas);

			for (k = 1; k <= events[idx][0]; k++) {
				cnt[j-1] += meas[k-1];
			}

			PAPI_reset(event_set);
		}

		if ( warmedup ) {
			for (j = 1; j <= events[idx][0]; j++) {
				fprintf(fd, ",%f", (double)cnt[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	fprintf(fd, "\n");

	return 1;
}

__attribute__ ((visibility("default")))
int measure_with_sideeffects(char *restrict test, char *restrict name, 
		char *restrict size, testfunc fp, void **restrict up, const int ups) {
	uint8_t  i, j, k;
	uint32_t idx;
	uint64_t total_ns = 0, total_s = 0, total;
	struct timespec begin, end;
	bool time     = false; 
	bool warmedup = false;
	bool OK       = true;

	fprintf(fd, "%s,%s,%s", test, name, size);

	for (i = 0; i < N_EVENTS; i++) {
		idx = i - warmedup - time;
		if ( warmedup && !time ) {
			for (j = 0; j < ups; j++) {
				clock_gettime(CLOCK_REALTIME, &begin);
				(fp)(up[MEASURE_IDX(ups, i, j)]);
				clock_gettime(CLOCK_REALTIME, &end);
				if (begin.tv_nsec <= end.tv_nsec) {
					total_ns += end.tv_nsec - begin.tv_nsec;
					total_s  += end.tv_sec  - begin.tv_sec;
				} else {
					total_ns += end.tv_nsec + (1e9 - begin.tv_nsec);
					total_s  += end.tv_sec - begin.tv_sec - 1;
				}
			}

			total = total_s * 1e9 + total_ns;
			fprintf(fd, ",%f", (double)total/ups);
			time = true;
			continue;
		}

		for (j = 1; j <= events[idx][0]; j++) {
			if ( PAPI_query_event(events[idx][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[idx][0]; j++) {
				if ( warmedup ) {
					fprintf(fd, ",-");
				}
			}
			OK = true;
			continue;
		}

		for (j = 0; j < MAX_COUNTERS; j++) {
			meas[j] = 0;
			cnt[j] = 0;
		}

		PAPI_add_events(
				event_set, 
				&events[idx][1], 
				events[idx][0]
		);

		for (j = 0; j < ups; j++) {
			PAPI_start(event_set);
			(fp)(up[MEASURE_IDX(ups, i, j)]);
			PAPI_stop(event_set, meas);

			for (k = 1; k <= events[idx][0]; k++) {
				cnt[k-1] += meas[k-1];
			}

			PAPI_reset(event_set);
		}

		if ( warmedup ) {
			for (j = 1; j <= events[idx][0]; j++) {
				fprintf(fd, ",%f", (double)cnt[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	fprintf(fd, "\n");

	return 1;
}


__attribute__ ((visibility("default")))
int measure_with_sideeffects_and_values(char *restrict test, 
		char *restrict name, char *restrict size, testfunc fp, 
		void **restrict up, const int ups, void **restrict cleanup) {
	uint8_t  i, j, k;
	uint32_t idx;
	uint64_t total_ns = 0, total_s = 0, total;
	struct timespec begin, end;
	bool time     = false; 
	bool warmedup = false;
	bool OK       = true;

	fprintf(fd, "%s,%s,%s", test, name, size);

	for (i = 0; i < N_EVENTS; i++) {
		idx = i - warmedup - time;
		if ( warmedup && !time ) {
			for (j = 0; j < ups; j++) {
				clock_gettime(CLOCK_REALTIME, &begin);
				cleanup[MEASURE_IDX(ups, i, j)] = 
					(fp)(up[MEASURE_IDX(ups, i, j)]);
				clock_gettime(CLOCK_REALTIME, &end);
				if (begin.tv_nsec <= end.tv_nsec) {
					total_ns += end.tv_nsec - begin.tv_nsec;
					total_s  += end.tv_sec  - begin.tv_sec;
				} else {
					total_ns += end.tv_nsec + (1e9 - begin.tv_nsec);
					total_s  += end.tv_sec - begin.tv_sec - 1;
				}
			}

			total = total_s * 1e9 + total_ns;
			fprintf(fd, ",%f", (double)total/ups);
			time = true;
			continue;
		}

		for (j = 1; j <= events[idx][0]; j++) {
			if ( PAPI_query_event(events[idx][j]) != PAPI_OK ) {
				OK = false;
			}
		}

		if ( !OK ) {
			for (j = 1; j <= events[idx][0]; j++) {
				if ( warmedup ) {
					fprintf(fd, ",-");
				}
			}
			OK = true;
			continue;
		}

		for (j = 0; j < MAX_COUNTERS; j++) {
			meas[j] = 0;
			cnt[j] = 0;
		}

		PAPI_add_events(
				event_set, 
				&events[idx][1], 
				events[idx][0]
		);

		for (j = 0; j < ups; j++) {
			PAPI_start(event_set);

			cleanup[MEASURE_IDX(ups, i, j)] = 
				(fp)(up[MEASURE_IDX(ups, i, j)]);

			PAPI_stop(event_set, meas);
			for (k = 1; k <= events[idx][0]; k++) {
				cnt[k-1] += meas[k-1];
			}
			PAPI_reset(event_set);
		}

		if ( warmedup ) {
			for (j = 1; j <= events[idx][0]; j++) {
				fprintf(fd, ",%f", (double)cnt[j-1]/ups);
			}
		}

		PAPI_cleanup_eventset(event_set);

		warmedup = true;
	}

	fprintf(fd, "\n");

	return 1;
}

__attribute__ ((visibility("default")))
int measure_init(char *filename) {
	uint8_t i, j;
	char buf[1024];
	
	events[0][0] = MAX_COUNTERS;
	events[0][1] = PAPI_TOT_INS;
	events[0][2] = PAPI_TOT_CYC;
	events[0][3] = PAPI_REF_CYC;

	events[1][0] = 2;
	events[1][1] = PAPI_BR_NTK;
	events[1][2] = PAPI_BR_TKN;

	events[2][0] = 2;
	events[2][1] = PAPI_BR_MSP;
	events[2][2] = PAPI_BR_PRC;

	events[3][0] = 2;
	events[3][1] = PAPI_L1_DCM;
	events[3][2] = PAPI_L1_ICM;

	events[3][0] = 2;
	events[3][1] = PAPI_L1_DCM;
	events[3][2] = PAPI_L1_ICM;

	events[4][0] = 2;
	events[4][1] = PAPI_L2_DCM;
	events[4][2] = PAPI_L2_ICM;

	events[5][0] = 2;
	events[5][1] = PAPI_TLB_DM;
	events[5][2] = PAPI_TLB_IM;

	events[6][0] = 1;
	events[6][1] = PAPI_L1_DCA;

	events[7][0] = 1;
	events[7][1] = PAPI_L2_DCA;

	events[8][0] = 1;
	events[8][1] = PAPI_L1_ICA;

	events[9][0] = 1;
	events[9][1] = PAPI_L2_ICA;

	if ( (fd = fopen(filename, "a")) == NULL ) {
		return 0;
	}

	if ( PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		return 0;
	}

	fprintf(fd, "Test,Name,Method,Nanoseconds");
	for (i = 0; i < NUMBER_OF_EVENTS; i++) {
		for (j = 1; j <= events[i][0]; j++) {
			PAPI_event_code_to_name(events[i][j], buf);
			fprintf(fd, ",%s", buf);
		}
	}
	fprintf(fd, "\n");

	if ( PAPI_create_eventset(&event_set) != PAPI_OK) {
		return 0;
	}

	return 1;
}

__attribute__ ((visibility("default")))
int measure_destroy() {
	if ( fclose(fd) != 0 ) {
		return 0;
	}

	if ( PAPI_destroy_eventset(&event_set) != PAPI_OK ) {
		return 0;
	}

	PAPI_shutdown();
	return 1;
}
