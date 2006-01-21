/* Global variables for DDU IRQ interrupt */
#define NUMCRATES 1
#define NUM_THREADS 1

struct IRQData{
  int threadid;
  long Handle;
  int exit;
  int sums[15][16];
  int errs[3];
  int count;
  int count_fmm;
  int last_ddu;
  int last_fiber;
  int last_errs[3];
  int last_count_fmm;
};

