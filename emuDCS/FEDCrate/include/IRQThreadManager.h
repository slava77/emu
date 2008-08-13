#ifndef __IRQTHREADMANAGER_H__
#define __IRQTHREADMANAGER_H__

//using namespace std;
#include <utility>
#include <vector>
#include <map>
#include <queue>
#include "FEDCrate.h"
#include "VMEController.h"
#include "DDU.h"
#include "EmuFEDLoggable.h"

#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <bitset>

#include <unistd.h>

#include <pthread.h>
#include "IRQData.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>

class IRQThreadManager: public EmuFEDLoggable {

public:

	IRQThreadManager();
	~IRQThreadManager();
	
	void attachCrate(FEDCrate *crate);
	void startThreads(unsigned long int runNumber = 0);
	void endThreads();

	static void *IRQThread(void *data);
	
	inline IRQData *data() { return data_; }

private:

	std::vector< std::pair<FEDCrate *, pthread_t> > threadVector_;
	IRQData *data_;

};

#endif
