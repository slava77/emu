/* Global variables for DDU IRQ interrupt */
#define NUMCRATES 1
#define NUM_THREADS 1

struct IRQData{
  int threadid;
  long Handle;
  int exit;
  int count;
  int count_fmm;
  int last_ddu;
  unsigned short int last_status;
  int last_fiber;
  int last_errs[3];
  int last_count_fmm;

};

