// #include "Muon/METBRawFormat/interface/FileReaderDDU.h"
// #include "FileReaderDDU.h"
// #include "CSCEventData.h"
#include "EmuFileReader.h"
#include "EmuSpyReader.h"
#include "EmuLocalPlotter.h"
#include <iomanip>
#include <iostream>

#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"
#include "coloredlayout.h"

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

int main(int argc, char **argv) {
  
//	For time performance	
	long t0, t1;
	t0 = time(0);
	Logger logger = Logger::getRoot();  
	// Initialize log system
        SharedAppenderPtr appender(new ConsoleAppender());
	appender->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );
        logger.addAppender(appender);
	logger.setLogLevel(INFO_LOG_LEVEL);


	EmuLocalPlotter plotter(logger);
	plotter.book();

	int NumberOfEvents = 1000000;
	int startEvent = 0;
	string datafile = "";
  	string histofile = "dqm_results.root";
        // EmuFileReader ddu; //( inputDeviceName_.toString(), inputDataFormatInt_ );
	// FileReaderDDU ddu;
	int dduCheckMask=0xFFFFDFFF;
	int binCheckMask=0xFFFFFFFF;
	// int dduCheckMask = 0x0;
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

	plotter.SetHistoFile(histofile.c_str());	
	if (dduCheckMask >= 0) {
        	plotter.SetDDUCheckMask(dduCheckMask);
	}
	if (binCheckMask >= 0) {
		plotter.SetBinCheckMask(binCheckMask);
	}
	plotter.SetDDU2004(1);
	
	if(debug_printout)  
		LOG4CPLUS_WARN (logger,  "Opening file " << datafile << " from event = " << startEvent 
			<< " to event = " << NumberOfEvents);
	EmuFileReader ddu(datafile.c_str(), EmuReader::DDU);
	ddu.open(datafile.c_str());
	if(debug_printout)  LOG4CPLUS_WARN (logger, "File " << datafile << " is opened");
	int i=0;

	while (ddu.readNextEvent()) {
		i++;
		short data = *((short *) ddu.data());
		int status = 0;
//KK
                if( ddu.getErrorFlag()==EmuFileReader::Type2 ) status |= 0x8000;
                if( ddu.getErrorFlag()==EmuFileReader::Type3 ) status |= 0x4000;
                if( ddu.getErrorFlag()==EmuFileReader::Type4 ) status |= 0x2000;
                if( ddu.getErrorFlag()==EmuFileReader::Type5 ) status |= 0x1000;
                if( ddu.getErrorFlag()==EmuFileReader::Type6 ) status |= 0x0800;
if(status) continue;
//KKend
		if ((i>=startEvent) && (i<=(startEvent+NumberOfEvents))) { 
//			cout << hex << data << endl; 
			// plotter.fill((unsigned char*) ddu.data(), ddu.dataLength(), status);			
			if(debug_printout) 
			LOG4CPLUS_WARN (logger, "Event#"<< dec << i<< " **** Buffer size: " << ddu.dataLength() << " bytes");
			plotter.fill((unsigned char*) ddu.data(), ddu.dataLength(), status);
			if (i%20000 == 0)
                                plotter.save(histofile.c_str());
		}
	
		
		if (i+1>(startEvent+NumberOfEvents)) break;  
	} 


	t1 = time(0);
        LOG4CPLUS_WARN (logger, "Total time: " << t1-t0);

	LOG4CPLUS_WARN (logger, "Events: " << i);
	plotter.save(histofile.c_str());
	plotter.SaveImages("images", "png" , 1600, 1200);
	ddu.close();

	// delete plotter;
	
	return 0;
};

