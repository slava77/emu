#include "Utilities/Configuration/interface/Architecture.h"
//#include "TrigDAQ/TrackFinderDataFormat/include/SRSPEventData.h"
//#include "SPReadout/include/FileReaderSP.h"
#include "Trigger/L1CSCTBRawFormat/interface/FileReaderSP.h"
#include "Trigger/L1CSCTBRawFormat/interface/L1MuCSCEventData.h"
#include "TFile.h"
#include "EmuSPMonitor.h"
#include <iomanip>
#include <iostream>

// g++ -g -DL1CSC_STANDALONE -I/home/dqm/ORCA -c -o ../src/SPMonitorTest.o SPMonitorTest.cpp
// g++ -o SPMonitorTest ../src/SPMonitorTest.o  -lL1CSCTBRawFormat -lL1CSCTrackFinder -lL1CSCTrigger -lm -ldl -rdynamic -L/home/dqm/ORCA/lib/Linux__2.2
int main(int argc, char *argv[]) {
	EmuSPMonitor spMonitor;
/*	FileReaderSP SRSPData; SRSPData.openFile(argv[1]);
	SRSPEventData::setCDF(false);
	while( SRSPData.readNextEvent() ) spMonitor.fill(reinterpret_cast<const unsigned short*>(SRSPData.data()), SRSPData.dataLength()/2);
	TFile f("qqq.root","RECREATE");
	list<TCanvas*> canvases = spMonitor.getListOfCanvases();
	list<TCanvas*>::const_iterator cnv = canvases.begin();
	while( cnv != canvases.end() ){ (*cnv)->Write(); cnv++; }
	f.Close();
*/
	FileReaderSP sp;
	sp.openFile(argv[1]/*"/home/dqm/EmuDAQ/EmuMonitor/test/AddressTable_SP2002.xml"*/);
	L1MuCSCEventData::setTriggerBX(4);
	int nevent = 0;

	while( sp.readNextEvent() /*&& nevent++<70*/ )
		spMonitor.fill(reinterpret_cast<unsigned short*>(sp.data()));
	TFile f("qqq.root","RECREATE");
	list<TCanvas*> canvases = spMonitor.getListOfCanvases();
	list<TCanvas*>::const_iterator cnv = canvases.begin();
	while( cnv != canvases.end() ){ /*cout<<(void*)(*cnv)<<endl; if(*cnv)cout<<(*cnv)->GetName()<<endl;*/ (*cnv)->Write(); cnv++; }
	f.Close();

	return 0;
};

