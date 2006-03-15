// #include "Muon/METBRawFormat/interface/FileReaderDDU.h"
#include "FileReaderDDU.h"
// #include "CSCEventData.h"
#include "EmuLocalPlotter.h"
#include <iomanip>
#include <iostream>

int main(int argc, char **argv) {
  
//	For time performance	
	long t0, t1;
	t0 = time(0);
  
	EmuLocalPlotter plotter;
	plotter.book();

	int NumberOfEvents = 100000;
	int startEvent = 0;
	string datafile = "";
  	string histofile = "dqm_stnd.root";
	FileReaderDDU ddu;
	// int dduCheckMask=0xFFFFDFFF;
	int binCheckMask=0xFFFFFFFF;
	int dduCheckMask = 0x0;
//	int binCheckMask = 0x0;
	switch (argc) {
		case 7: binCheckMask = atoi(argv[7]);	
		case 6: dduCheckMask = atoi(argv[6]);
		case 5: startEvent = atoi(argv[4]);
		case 4: histofile = argv[3];
		case 3: NumberOfEvents = atoi(argv[2]);
		case 2:	datafile = argv[1];
		break;
	}
	
	if (dduCheckMask >= 0) {
        	plotter.SetDDUCheckMask(dduCheckMask);
	}
	if (binCheckMask >= 0) {
		plotter.SetBinCheckMask(binCheckMask);
	}
	plotter.SetDDU2004(1);
	
	if(debug_printout) cout << "D**MonitorTest> Opening file " << datafile << " from event = " << startEvent << " to event = " << NumberOfEvents << endl;
	ddu.openFile(datafile.c_str());
	if(debug_printout) cout << "D**MonitorTest> File " << datafile << " is opened" << endl;
	int i=0;

	while (ddu.readNextEvent()) {
		i++;
		short data = *((short *) ddu.data());
		if ((i>=startEvent) && (i<=(startEvent+NumberOfEvents))) { 
//			cout << hex << data << endl; 
			plotter.fill((unsigned char*) ddu.data(), ddu.dataLength(), ddu.status());			
			if(debug_printout) cout << "***debug>  Event#"<< dec << i<< " **** Buffer size: " << ddu.dataLength() << " bytes" << endl;
		}
	
		
		if (i+1>(startEvent+NumberOfEvents)) break;  
	} 


	t1 = time(0);
        cout << "Total time: " << t1-t0 << endl;

	cout << "Events: " << i << endl;
	plotter.save(histofile.c_str());
	ddu.closeFile();

	// delete plotter;
	
	return 0;
};

