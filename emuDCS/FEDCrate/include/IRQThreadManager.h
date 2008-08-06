#ifndef IRQThreadManager_H
#define IRQThreadManager_H

using namespace std;
#include <utility>
#include <vector>
#include <map>
#include <queue>
#include "Crate.h"
#include "VMEController.h"
#include "DDU.h"

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

class IRQThreadManager {

public:

	IRQThreadManager();
	~IRQThreadManager();
	
	void attachCrate(Crate *crate);
	void startThreads(unsigned long int runNumber = 0);
	void endThreads();

	static void *IRQThread(void *data);
	
	inline IRQData *data() { return data_; }

private:

	vector< pair<Crate *, pthread_t> > threadVector_;
	IRQData *data_;

};

#endif
