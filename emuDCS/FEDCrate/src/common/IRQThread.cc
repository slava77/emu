#include "IRQThread.h"

IRQThread::IRQThread(IRQData mydata):
	is_started(false),threadID(0)
{
	cout << "...Constructing Thread..." << endl;
	data = mydata;
}

IRQThread::~IRQThread()
{
	// kill the attributes
	//pthread_attr_destroy(attr);
	// if started, kill the thread
	if (is_started) this->end();
}

void IRQThread::setCrateNumber(int number) {
	data.crate_number = number;
}

void IRQThread::start(long unsigned int runnumber)
{
	if (is_started) {
		cout << " Thread already started.  Killing old thread and logger (unsafe)" << endl;
		this->kill();
	}
	cout << "**about to launch pthread" << endl;
	status = 0;
	time(&(data.start_time));
	int error = pthread_create(&threadID, NULL, IRQ_Interrupt_Handler, &data);
	cout << "**pthread launched with status " << error << endl;
	if (error) {
		cout << " Incountered error " << error << " when attempting to start thread." << endl;
		exit(1);
	} else {
		is_started = true;
		// start logger
		
		data.count=0;
		data.count_fmm=0;
		data.last_ddu = 0;
		data.last_errs[0]=0;
		data.last_errs[1]=0;
		data.last_errs[2]=0;
		data.last_count_fmm=0;
		data.last_status=status;
		
		for (int i=0; i<21; i++) {
			data.last_error[i] = 0;
			data.last_fmm_stat[i] = 0;
			data.acc_error[i] = 0;
			data.ddu_count[i] = 0;
			data.previous_problem[i] = 0;
		}
		
		cout << "**Create unique Logger for EmuFEDVME" << endl;

		time_t theTime;
		char datebuf[55];
		char filebuf[255];
		theTime=time(NULL);
		
		// log file format: EmuFEDYYYY-DOY-HHMMSS_rRUNNUMBER.log
		strftime(datebuf, sizeof(datebuf), "%Y-%j-%H%M%S", localtime(&theTime));
		sprintf(filebuf,"EmuFED%s_r%05d.log",datebuf,runnumber);
		
		//printf("  MyDateString = %s,  LogName=%s\n",datebuf,filebuf);
		log4cplus::SharedAppenderPtr myAppend = new FileAppender(filebuf);
		myAppend->setName("EmuFEDappender");

		//Appender Layout
		std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
		// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
		// only need error data from Log lines with "ErrorData" tag
		myAppend->setLayout( myLayout );

		log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
		logger.addAppender(myAppend);
	}
}

void IRQThread::end()
{
	if (!is_started) {
		cout << " Thread not started.  Start thread before attempting to end it." << endl;
		return;
	}
	cout << "**about to end pthread" << endl;
	//cout << "Interrupt handler sleeping for 6 seconds...." << endl;
	//usleep(6000000);
	unsigned long mask=0x00000001;
	data.exit = 1;
	sleep((unsigned int) 6);
	CAENVME_IRQDisable(data.Handle,mask);
	is_started = false;
	cout << "**pthread ended" << endl;
	cout << "**Killing EmuFEDVME log Appender" << endl;
	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
	logger.removeAppender("EmuFEDappender");
	cout << "**EmuFEDVME log Appender killed" << endl;
}

void IRQThread::kill()
{
	if (!is_started) {
		cout << " Thread not started.  Star thread before attempting to kill it" << endl;
		return;
	}
	cout << "**about to kill pthread" << endl;
	int error = pthread_cancel(threadID);
	if (error) {
		cout << " Incountered error " << error << " when attempting to cancel thread." << endl;
		exit(error);
	} else {
		is_started = false;
		//cout << "Interrupt handler sleeping for 6 seconds...." << endl;
		//usleep(6000000);
		unsigned long mask=0x00000001;
		sleep((unsigned int) 6);
		CAENVME_IRQDisable(data.Handle,mask);
		cout << "**pthread killed" << endl;
		cout << "**Killing EmuFEDVME log Appender" << endl;
		log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
		logger.removeAppender("EmuFEDappender");
		cout << "**EmuFEDVME log Appender killed" << endl;
	}
}

int IRQThread::test(int ival)
{
	switch (ival) {
		case 0:
			return data.count;
			break;
		case 1:
			return data.crate_number;
			break;
		case 2:
			return data.last_ddu;
			break;
		case 3:
			return data.last_status;
			break;
		case 4:
		case 5:
		case 6:
			return data.last_errs[ival-4];
			break;
		case 7:
			return data.last_count_fmm;
			break;
		default:
			cout << " I don't understand ival=" << ival << endl;
			return -1;
	}
}

void IRQThread::info()
{
	cout << "VME INTERRUPT INFORMATION" << endl;
	cout << "  Last Interrupt:" << endl;
	printf("  SLOT %d FIBER %04x ERR %d SYNC %d FMM %d  NUM_ERR %d \n",data.last_ddu,data.last_status,data.last_errs[0],data.last_errs[1],data.last_errs[2],data.last_count_fmm);
}

/// The big one
void *IRQThread::IRQ_Interrupt_Handler(void *data)
{
	IRQData *locdata = (IRQData *)data;
	long BHandle;
	unsigned long Address;
	unsigned char Data[2];
	CVAddressModifier AM=cvA24_U_DATA;
	CVDataWidth DW=cvD16;
	CVIRQLevels IRQLevel=cvIRQ1;
	unsigned long mask=0x00000001;

	int crate_number = locdata->crate_number;

	int noerr;
	unsigned int ERR,SYNC,FMM,SLOT,NUM_ERR,NUM_SYNC;
	unsigned int status;
	char buf[300];
	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");

	while (1) {
		if(locdata->exit==1) break;
		
		BHandle=locdata->Handle;
		CAENVME_IRQEnable(BHandle,mask);
		noerr=CAENVME_IRQWait(BHandle,mask,5000);


		if(noerr && locdata->count > 0){
			SLOT=(locdata->last_ddu)&0x001f;
			Address=0x00034000|((SLOT<<19)&0x00f80000);
			Data[0]=0xFF;
			Data[1]=0xFF;
		// JRG, now get CSC_Stat: 16-bit word for which CSCs have a problem
			CAENVME_ReadCycle(BHandle,Address,Data,AM,DW);
			status = ( (Data[1]<<8)&0xff00 ) | (Data[0]&0xff);
			if (!status) {
				sprintf(buf," ** EmuFEDVME, Crate %01d:  RESET detected, clearing old IRQ data...\n ",locdata->crate_number);
				LOG4CPLUS_ERROR(logger,buf);
				sprintf(buf," ErrorData RESET Detected");
				LOG4CPLUS_ERROR(logger,buf << endl);

				locdata->count=0;
				locdata->count_fmm=0;
				locdata->last_ddu = 0;
				locdata->last_errs[0]=0;
				locdata->last_errs[1]=0;
				locdata->last_errs[2]=0;
				locdata->last_count_fmm=0;
				locdata->last_status=status;
				
				for (int i=0; i<21; i++) {
					locdata->last_error[i] = 0;
					locdata->last_fmm_stat[i] = 0;
					locdata->acc_error[i] = 0;
					locdata->ddu_count[i] = 0;
					//locdata->previous_problem[i] = 0;
				}
				
			}
		}
		
		if(noerr) continue;

		// We have an error!

		Data[0]=0x00;Data[1]=0x00;
		CAENVME_IACKCycle(BHandle,IRQLevel,Data,DW);
		ERR=0;SYNC=0;FMM=0;SLOT=0;NUM_ERR=0;
		if(Data[1]&0x80)ERR=1;  // DDU Board Error set
		if(Data[1]&0x40)SYNC=1; // DDU Board SyncErr set
		if(Data[1]&0x20)FMM=1;  // FMM Error or SyncErr set
		SLOT=Data[1]&0x1f;
		NUM_ERR=((Data[0]>>4)&0x0f); // # CSCs on DDU with Error set
		NUM_SYNC=(Data[0]&0x0f);     // # CSCs on DDU with SyncErr set

		sprintf(buf," ** EmuFEDVME: Interrupt detected for Crate/Slot %d/%d, 0x%02x%02x ** ",crate_number,SLOT,Data[1]&0xff,Data[0]&0xff);
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

		Address=0x00034000|((SLOT<<19)&0x00f80000);
		Data[0]=0xFF;
		Data[1]=0xFF;
		// JRG, now get CSC_Stat: 16-bit word for which CSCs have a problem
		CAENVME_ReadCycle(BHandle,Address,Data,AM,DW);
		status=((Data[1]<<8)&0xff00)|(Data[0]&0xff);

		// Log all errors in persisting array...
		locdata->count++;
		locdata->last_ddu = SLOT;
		
		// we need an exclusive or...
		unsigned int xorstatus = status^(locdata->acc_error[SLOT]);
		locdata->last_error[SLOT] = xorstatus;
		locdata->acc_error[SLOT] = status;
		time(&(locdata->last_error_time[SLOT]));
		locdata->ddu_count[SLOT]++;

		locdata->count_fmm=NUM_ERR;
		locdata->last_errs[0]=ERR;
		locdata->last_errs[1]=SYNC;
		locdata->last_errs[2]=FMM;
		locdata->last_count_fmm=NUM_ERR;

		sprintf(buf," ** EmuFEDVME %d:   %d CSCs w/Error, %d w/SyncErr. CSCtroubleFlags 0x%02x%02x",SLOT,NUM_ERR,NUM_SYNC,Data[1],Data[0]);
		LOG4CPLUS_INFO(logger,buf << endl);
// JRG here: really need to show only the _most recent_ IRQ CSCs status change,
//   use XOR with accumStatus[iSLOT] for the individual Slots (that is, NOT
//   locadata->last_status! ) to get it.
		time_t theTime = time(NULL);
		sprintf(buf," ErrorData %d %d %04X %ju",crate_number,SLOT,status,(uintmax_t)theTime);
		LOG4CPLUS_ERROR(logger,buf << endl);
	}

	printf(" IRQ_Int call pthread_exit \n");
	pthread_exit(NULL);
}

