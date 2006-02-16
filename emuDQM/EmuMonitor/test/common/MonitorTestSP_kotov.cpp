// #include "Muon/METBRawFormat/interface/FileReaderDDU.h"
#include "EmuDAQ/DDUReadout/include/FileReaderDDU.h"
#include "Muon/METBRawFormat/interface/MuEndEventData.h"
#include "TrigDAQ/TrackFinderDataFormat/include/SRSPEventData.h"
#include "SPReadout/include/FileReaderSP.h"
#include "EmuLocalPlotter.h"
#include "EmuSPMonitor.h"
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
	if( argc < 2 ){ cout<<"Specify path to datafile at least"<<endl; exit(0); }
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
	ddu.openFile(argv[1]);

	int runNumber = atoi( RegExSubstitute(".*[dN][du][um](\\w+).*","$1",argv[1]).c_str() );
	cout<<"Proceed run #"<<runNumber<<"  ( "<<argv[0]<<" )"<<endl;

	if( runNumber>=400 ){
		MuEndDMBHeader::setDDU2004(true);
		MuEndDDUTrailer::setDDU2004(true);
	}

	// This checker will filter out all suspicious DDU events
	dduBinExaminer dduExaminer;
	dduExaminer.output1().hide();
	dduExaminer.output2().hide();

	while( ddu.readNextEvent() ){
		// Check data for errors before
		const unsigned short* dduData = reinterpret_cast<unsigned short*>(ddu.data());
		long dduDataLength = ddu.dataLength()/sizeof(unsigned short);
		if( dduExaminer.check(dduData, dduDataLength)!=0 || dduExaminer.errors() ){
			cout<<"Bad Event: "<<dduExaminer.errors()<<endl;
			continue;
		}
		plotter.fill((unsigned char*)ddu.data(), ddu.dataLength(), ddu.errorFlag);
	}
	plotter.save("dqm_.root");
	ddu.closeFile();

	SRSPEventData::setCDF(false);
	if( argc>=3 ){
		TFile f("dqm.root","UPDATE");
		const char *basename = strrchr(argv[2],'/');
		if( basename==NULL ) basename=argv[2]; else basename++;
		//f.mkdir(basename);
		//f.cd(basename);

		FileReaderSP sp; sp.openFile(argv[1]);
		EmuSPMonitor spMonitor;

		while( sp.readNextEvent() )
			spMonitor.fill(reinterpret_cast<const unsigned short*>(sp.data()));
		list<TCanvas*> canvases = spMonitor.getListOfCanvases();
		list<TCanvas*>::const_iterator cnv = canvases.begin();
		while( cnv != canvases.end() ){
			char buff[512];
			sprintf(buff,"%s\/%s",basename,(*cnv)->GetName());
			(*cnv)->SetName(buff);
			(*cnv)->Write();
			cnv++;
		}
		f.Close();
	}
	if( argc>=4 ){
		TFile f("dqm.root","UPDATE");
		const char *basename = strrchr(argv[3],'/');
		if( basename==NULL ) basename=argv[3]; else basename++;
		//f.mkdir(basename);
		//f.cd(basename);
		FileReaderSP sp; sp.openFile(argv[1]);
		EmuSPMonitor spMonitor;
		while( sp.readNextEvent() )
			spMonitor.fill(reinterpret_cast<const unsigned short*>(sp.data()));
		list<TCanvas*> canvases = spMonitor.getListOfCanvases();
		list<TCanvas*>::const_iterator cnv = canvases.begin();
		while( cnv != canvases.end() ){
			char buff[512];
			sprintf(buff,"%s\/%s",basename,(*cnv)->GetName());
			(*cnv)->SetName(buff);
			(*cnv)->Write();
			cnv++;
		}
		f.Close();
	}

	return 0;
};
