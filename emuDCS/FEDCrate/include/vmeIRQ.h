/* Global variables for DDU IRQ interrupt */
#ifndef vmeIRQ_h
#define vmeIRQ_h

#define NUMCRATES 1
#define NUM_THREADS 1

#include <time.h>

struct IRQData {
	long Handle;
	
	int exit;
	int count;
	int count_fmm;
	
	int last_ddu;
	
	int last_error[21]; // Per DDU slot
	int last_fmm_stat[21]; // Per DDU slot
	int acc_error[21]; // Per DDU slot
	int ddu_count[21]; // Per DDU slot
	time_t last_error_time[21]; // Per DDU slot
	
	int previous_problem[21];
	
	unsigned short int last_status;
	int last_errs[3];
	int last_count_fmm;
	
	int crate_number;
	
	time_t start_time;
};

#endif
