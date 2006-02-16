using namespace std;
#include <string>
#include "EmuDAQ/DDUReadout/FileReaderDDU.cc"
//#include "Muon/METBRawFormat/interface/MuEndEventData.h"
#include "MuEndEventData.h"
#include "EmuLocalPlotter.h"
#include "TStyle.h"
#include "RegEx.cc"
#include <iostream>

#define SIGNAL(s, handler){ \
sa.sa_handler = handler; \
if (sigaction(s, &sa, NULL) < 0) { \
    fprintf(stderr, "Couldn't establish signal handler (%d): %m", s); \
    exit(1); \
} \
}

bool wait = false;

void bad_signal(int signal){ printf("\n!!!!!!Program died because of %d signal\n",signal); exit(0); }
void control_signal(int signal){ switch(signal){ case SIGHUP: wait=false; break; case SIGINT: wait=true; } }

int main(int argc, char **argv) {
	if( argc != 2 ){ cout<<"Specify path to datafile only"<<endl; exit(0); }
	// Setup signals handling in case ORCA crashes
	struct sigaction sa;
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGSEGV);
	sigaddset(&mask, SIGABRT);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGINT);
	sa.sa_mask = mask;
	sa.sa_flags = 0;
	SIGNAL(SIGABRT, bad_signal);
	SIGNAL(SIGSEGV, bad_signal);
	SIGNAL(SIGHUP,control_signal);
	SIGNAL(SIGINT,control_signal);

	// end signals

	EmuLocalPlotter plotter;
	plotter.book();

	FileReaderDDU ddu;
	ddu.open(argv[1]);

	plotter.SetDDUCheckMask(0xFFFFDFFF);

	int runNumber = atoi( RegExSubstitute(".*[dN][du][um](\\w+).*","$1",argv[1]).c_str() );
	cout<<"Proceed run #"<<runNumber<<"  ( "<<argv[0]<<" )"<<endl;

	if( runNumber>=400 ){
		//MuEndDMBHeader::setDDU2004(true);
		//MuEndDDUTrailer::setDDU2004(true);
		plotter.SetDDU2004(1);
	} else {
		//MuEndDMBHeader::setDDU2004(false);
		//MuEndDDUTrailer::setDDU2004(false);
		plotter.SetDDU2004(0);
	}

	// This checker will filter out all suspicious DDU events
	dduBinExaminer dduExaminer;
	dduExaminer.output1().hide();
	dduExaminer.output2().hide();
///int nevents =0 ;

        size_t dduDataLength;
        const unsigned short *dduData=0;

	while( (dduDataLength = ddu.next(dduData)) ){
		do {
			FILE *fd = fopen("wait","rt");
			if( fd == NULL ) wait=false; else { wait=true; fclose(fd); sleep(1); }
		} while( wait );
		// Check data for errors before
		//if( dduExaminer.check(dduData, dduDataLength)!=0 || dduExaminer.errors() ){
		//	cout<<"Bad Event: "<<dduExaminer.errors()<<endl;
		//	//continue;
		//}
		int errorStat = 0;
		if( ddu.status()==FileReaderDDU::Type2 ) errorStat |= 0x8000;
		if( ddu.status()==FileReaderDDU::Type3 ) errorStat |= 0x4000;
		if( ddu.status()==FileReaderDDU::Type4 ) errorStat |= 0x2000;
		if( ddu.status()==FileReaderDDU::Type5 ) errorStat |= 0x1000;
		if( ddu.status()==FileReaderDDU::Type6 ) errorStat |= 0x0800;
		plotter.fill((unsigned char*)dduData, dduDataLength*2, errorStat);
		///nevents++;
	}

	plotter.save("dqm.root");
	return 0;
};
