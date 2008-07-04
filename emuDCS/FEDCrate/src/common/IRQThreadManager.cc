
#include "IRQThreadManager.h"

IRQThreadManager::IRQThreadManager() {
	threadVector_.clear();
	data_ = new IRQData();
	//cout << "I have constructed an IRQData object, and this is what exit is: " << data_->exit << endl;
}



IRQThreadManager::~IRQThreadManager() {

	//endThreads();

}



void IRQThreadManager::attachCrate(Crate *crate) {

	pthread_t threadID;
	//cout << "IRQThreadManager::attachCrate Attaching crate with number " << crate->number() << endl;
	threadVector_.push_back(pair<Crate *, pthread_t>(crate, threadID));

}




void IRQThreadManager::startThreads(unsigned long int runNumber) {

	//cout << "IRQThreadManager::startThreads Create unique Logger for EmuFEDVME" << endl;

	char datebuf[55];
	char filebuf[255];
	time_t theTime = time(NULL);

	// log file format: EmuFEDYYYY-DOY-HHMMSS_rRUNNUMBER.log
	strftime(datebuf, sizeof(datebuf), "%Y-%j-%H%M%S", localtime(&theTime));
	sprintf(filebuf,"EmuFED%s_r%05u.log",datebuf,(unsigned int) runNumber);

	log4cplus::SharedAppenderPtr myAppend = new FileAppender(filebuf);
	myAppend->setName("EmuFEDappender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
	logger.addAppender(myAppend);

	//cout << "IRQThreadManager::startThreads Clearing shared data" << endl;
	
	data_->runNumber = runNumber;
	data_->exit = 0;
	
	for (unsigned int i=0; i<threadVector_.size(); i++) {
		//cout << "IRQThreadManager::startThread Adding and clearing data for crate number " << threadVector_[i].first->number() << endl;

		int crateNumber = threadVector_[i].first->number();
		data_->crateNumbers.push(crateNumber);
		data_->crate[crateNumber] = threadVector_[i].first;
		data_->Handles[crateNumber] = threadVector_[i].first->vmeController()->theBHandle;
		data_->startTime[crateNumber] = 0;
		data_->count[crateNumber] = 0;
		data_->countFMM[crateNumber] = 0;
		data_->countSync[crateNumber] = 0;
		data_->ticks[crateNumber] = 0;
		data_->tickTime[crateNumber] = 0;

		data_->lastDDU[crateNumber] = 0;
		data_->lastStatus[crateNumber] = 0;
		data_->lastErrs[crateNumber][0] = 0;
		data_->lastErrs[crateNumber][1] = 0;
		data_->lastErrs[crateNumber][2] = 0;
		data_->lastCountFMM[crateNumber] = 0;
		
		for (int j=0; j<21; j++) {
			data_->lastError[crateNumber][j] = 0; // Per DDU slot
			data_->lastFMMStat[crateNumber][j] = 0; // Per DDU slot
			data_->accError[crateNumber][j] = 0; // Per DDU slot
			data_->dduCount[crateNumber][j] = 0; // Per DDU slot
			data_->lastErrorTime[crateNumber][j] = 0; // Per DDU slot
			
			data_->previousProblem[crateNumber][j] = 0;
		}
	}

	for (unsigned int i=0; i<threadVector_.size(); i++) {
		//cout << "IRQThreadManager::startThread Starting thread for crate number " << threadVector_[i].first->number() << endl;

		// Start the thread (as a static function)
		//int error = pthread_create(&(threadVector_[i].second), NULL, IRQThread, data_);
		pthread_create(&(threadVector_[i].second), NULL, IRQThread, data_);
		//cout << "IRQThreadManager::startThread pthread launched with status " << error << endl;
	}
}



void IRQThreadManager::endThreads() {
	if (data_->exit == 1) {
		//cout << "IRQThreadManager::endThreads Threads already stopped." << endl << flush;
	} else {
		//cout << "IRQThreadManager::endThreads Gracefully killing off all threads." << endl << flush;
		//cout << "<PGK> Before exit=1" << endl << flush;
		data_->exit = 1;
		//cout << "<PGK> After exit=1" << endl << flush;
		sleep((unsigned int) 6);
		//cout << "<PGK> After sleep" << endl << flush;
	}
}



void IRQThreadManager::killThreads() {

	if (data_->exit == 1) {
		//cout << "IRQThreadManager::killThreads Threads already stopped." << endl;
	} else {
		//cout << "IRQThreadManager::killThreads Brutally killing off all threads." << endl;
		for (unsigned int i=0; i<threadVector_.size(); i++) {
			pthread_t threadID = threadVector_[i].second;
			int error = pthread_cancel(threadID);
			if (error) {
				//cout << " Incountered error " << error << " when attempting to cancel thread." << endl;
				exit(error);
			}
		}
		
		sleep((unsigned int) 6);
	}

}



/// The big one
void *IRQThreadManager::IRQThread(void *data)
{

	IRQData *locdata = (IRQData *)data;
	
	int crateNumber = locdata->crateNumbers.front();
	locdata->crateNumbers.pop();
	
	long BHandle = locdata->Handles[crateNumber];
	unsigned char Data[2];
	CVDataWidth DW=cvD16;
	CVIRQLevels IRQLevel=cvIRQ1;
	unsigned long mask=0x00000001;


	char buf[300];
	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
	
	vector<DDU *> dduVector = locdata->crate[crateNumber]->ddus();

	time(&(locdata->startTime[crateNumber]));

	while (1) {
		if(locdata->exit==1) break;
		
		locdata->ticks[crateNumber]++;
		time(&(locdata->tickTime[crateNumber]));
		
		CAENVME_IRQEnable(BHandle,mask);
		int noerr=CAENVME_IRQWait(BHandle,mask,5000);


		if(noerr && locdata->count[crateNumber] > 0){
			int slot = locdata->lastDDU[crateNumber];
			DDU *myDDU;
			for (unsigned int iDDU=0; iDDU<dduVector.size(); iDDU++) {
				if (dduVector[iDDU]->slot() == slot) {
					myDDU = dduVector[iDDU];
					break;
				}
			}
			unsigned short int status = myDDU->vmepara_CSCstat();
			if (!status) {
				sprintf(buf," ** EmuFEDVME, Crate %01d:  RESET detected, clearing old IRQ data...\n ",crateNumber);
				LOG4CPLUS_ERROR(logger,buf);
				sprintf(buf," ErrorData RESET Detected");
				LOG4CPLUS_ERROR(logger,buf << endl);

				locdata->count[crateNumber] = 0;
				locdata->countFMM[crateNumber] = 0;
				locdata->countSync[crateNumber] = 0;
				locdata->lastDDU[crateNumber] = 0;
				locdata->lastErrs[crateNumber][0] = 0;
				locdata->lastErrs[crateNumber][1] = 0;
				locdata->lastErrs[crateNumber][2] = 0;
				locdata->lastCountFMM[crateNumber] = 0;
				locdata->lastStatus[crateNumber] = status;
				
				for (int i=0; i<21; i++) {
					locdata->lastError[crateNumber][i] = 0;
					locdata->lastFMMStat[crateNumber][i] = 0;
					locdata->accError[crateNumber][i] = 0;
					locdata->dduCount[crateNumber][i] = 0;
					//locdata->previous_problem[i] = 0;
				}
				
			}
		}
		
		if(noerr) continue;

		// We have an error!

		unsigned int ERR,SYNC,FMM,NUM_ERR,NUM_SYNC,SLOT;
		Data[0]=0x00;Data[1]=0x00;
		CAENVME_IACKCycle(BHandle,IRQLevel,Data,DW);
		ERR=0;SYNC=0;FMM=0;SLOT=0;NUM_ERR=0;
		if(Data[1]&0x80)ERR=1;  // DDU Board Error set (OR of all chambers + DDU)
		if(Data[1]&0x40)SYNC=1; // DDU Board SyncErr set (OR of all chambers + DDU)
		if(Data[1]&0x20)FMM=1;  // FMM Error or SyncErr set (DDU wants a reset)
		SLOT=Data[1]&0x1f;		// slot of DDU sending the error
		NUM_ERR=((Data[0]>>4)&0x0f); // # CSCs on DDU with Error set
		NUM_SYNC=(Data[0]&0x0f);     // # CSCs on DDU with SyncErr set

		sprintf(buf," ** EmuFEDVME: Interrupt detected for Crate/Slot %d/%d, 0x%02x%02x ** ",crateNumber,SLOT,Data[1]&0xff,Data[0]&0xff);
		LOG4CPLUS_ERROR(logger,buf);
		
		if(Data[1]&0xe0){
			//printf(" ** EmuFEDVME %d: ",SLOT);
			sprintf(buf," ** EmuFEDVME %d: ",SLOT);
			if(ERR){
				//printf("  DDU has Error");
				strcat(buf,"  DDU has Error");
			}
			if(SYNC){
				//printf("  DDU Lost Sync");
				strcat(buf,"  DDU Lost Sync");
			}
			if(ERR){
				//printf("  FMM Reset requested");
				strcat(buf,"  FMM Reset requested");
			}
			LOG4CPLUS_ERROR(logger,buf);
		}

		DDU *myDDU;
		for (unsigned int iDDU=0; iDDU<dduVector.size(); iDDU++) {
			if ((unsigned int) dduVector[iDDU]->slot() == SLOT) {
				myDDU = dduVector[iDDU];
				break;
			}
		}
		unsigned short int status = myDDU->vmepara_CSCstat();

		// Log all errors in persisting array...
		locdata->count[crateNumber]++;
		locdata->lastDDU[crateNumber] = SLOT;
		
		// we need an exclusive or...
		unsigned int xorstatus = status^(locdata->accError[crateNumber][SLOT]);
		
		if (!xorstatus) continue; // Same error as before, I guess...
		
		locdata->lastError[crateNumber][SLOT] = xorstatus;
		locdata->accError[crateNumber][SLOT] = status; // This is the thing I want to count!
		time(&(locdata->lastErrorTime[crateNumber][SLOT]));
		locdata->dduCount[crateNumber][SLOT]++;

		locdata->countFMM[crateNumber]=NUM_ERR;
		locdata->countSync[crateNumber]=NUM_SYNC;
		locdata->lastErrs[crateNumber][0]=ERR;
		locdata->lastErrs[crateNumber][1]=SYNC;
		locdata->lastErrs[crateNumber][2]=FMM;
		locdata->lastCountFMM[crateNumber]=NUM_ERR;

		sprintf(buf," ** EmuFEDVME %d:   %d CSCs w/Error, %d w/SyncErr. CSCtroubleFlags 0x%02x%02x",SLOT,NUM_ERR,NUM_SYNC,Data[1],Data[0]);
		LOG4CPLUS_INFO(logger,buf << endl);
// JRG here: really need to show only the _most recent_ IRQ CSCs status change,
//   use XOR with accumStatus[iSLOT] for the individual Slots (that is, NOT
//   locadata->last_status! ) to get it.
		time_t theTime = time(NULL);
		sprintf(buf," ErrorData %d %d %04X %ju",crateNumber,SLOT,status,(uintmax_t)theTime);
		LOG4CPLUS_ERROR(logger,buf << endl);
		
		// Check if we have sufficient error conditions.
		// Loop over all the slots in all the crates
		unsigned int statusCount = 0;
		vector<Crate *> resetCrates;
		for (map<int,Crate *>::iterator iter = locdata->crate.begin(); iter != locdata->crate.end(); iter++) {
			bool crateCounted = false;
			for (int islot = 0; islot < 21; islot++) {
				bitset<16> bs(locdata->accError[iter->first][islot]);
				statusCount += bs.count();
				if (bs.count() && !crateCounted) {
					resetCrates.push_back(locdata->crate[iter->first]);
					crateCounted = true;
					cout << "--> Crate " << iter->first << " has been counted for resetting." << endl;
				}
			}
		}
		//cout << "--> Total Resets requested: " << statusCount << endl;
		if (statusCount > 1 ) {
			//cout << "    Requesting resets via FMM" << endl;
			// Send a reset to everybody who needs a reset
			int broadcastSlot = 28;
			for (unsigned int iCrate = 0; iCrate < resetCrates.size(); iCrate++) {
				DDU *broadcastDDU;
				vector<DDU *> myDDUVector = resetCrates[iCrate]->ddus();
				for (unsigned int iDDU=0; iDDU<myDDUVector.size(); iDDU++) {
					if (myDDUVector[iDDU]->slot() == broadcastSlot) {
						broadcastDDU = myDDUVector[iDDU];
						//cout << "--> Requesting reset on crate " << resetCrates[iCrate]->number() << " broadcast slot " << broadcastDDU->slot() << endl;
						break;
					}
				}
				broadcastDDU->vmepara_wr_fmmreg(0xFED8);
				//cout << "<-- Done!" << endl;
			}
			
		}
		
	}

	//cout << " IRQ_Int call pthread_exit" << endl;
	pthread_exit(NULL);
}



