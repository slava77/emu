#ifndef IRQThread_h
#define IRQThread_h

using namespace std;
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#include <pthread.h>
#include "vmeIRQ.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <xdaq/Application.h>

class IRQThread {
public:

	IRQThread(IRQData mydata);
	~IRQThread();

	static void *IRQ_Interrupt_Handler(void *data);
	void setCrateNumber(int number);
	void start(long unsigned int runnumber);
	void end();
	void kill();
	void info();
	int test(int ival);
	int count() { return this->test(0); }
	int crate_number() { return this->test(1); }
	int last_ddu() { return this->test(2); }
	int last_status() { return this->test(3); }
	int last_errs0() { return this->test(4); }
	int last_errs1() { return this->test(5); }
	int last_errs2() { return this->test(6); }
	int last_count_fmm() { return this->test(7); }
	
	int ddu_count(int slot) { return data.ddu_count[slot]; }
	int ddu_last_error(int slot) { return data.last_error[slot]; }
	int last_error() { return data.last_error[last_ddu()]; }
	int acc_error(int slot) { return data.acc_error[slot]; }
	time_t ddu_last_error_time(int slot) { return data.last_error_time[slot]; }
	time_t last_error_time() { return data.last_error_time[last_ddu()]; }
	time_t start_time() { return data.start_time; }
	
	// Debug only!
	pthread_t ID() { return threadID; }

	bool is_started;

	IRQData data;
	int status;

private:
	pthread_t threadID;
	pthread_attr_t *attr;

};

#endif
