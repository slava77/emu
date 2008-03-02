#include "EmuFileReader.h"
#include "EmuSpyReader.h"
#include "EmuPlotter.h"
#include <iomanip>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

// == Show usage info
void usage() {
}


int main(int argc, char **argv) {
/*
	if (argc <2) {
                std::cerr << argv[0] << ": Not enough parameters." << std::endl;
		usage();
                exit(-1);
        }
*/
  
	Logger logroot = Logger::getRoot();  
	Logger logger = Logger::getInstance("runEmuPlotter");

        SharedAppenderPtr appender(new ConsoleAppender());
	appender->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );

	logroot.addAppender(appender);
	
	logger.setLogLevel(INFO_LOG_LEVEL);
	

	if (argc <2) {
                LOG4CPLUS_FATAL(logger, argv[0] << ": Not enough parameters.");
                usage();
                exit(-1);
        }


	uint32_t NumberOfEvents = 0xFFFFFFFF;
        uint32_t startEvent = 0;

	std::string cfgDir="";
        if (getenv("HOME") && string(getenv("HOME")).size())
        cfgDir=getenv("HOME"); //+"/config/";
        cfgDir+="/config/";
        std::string xmlHistosBookingCfg = "file://" + cfgDir + "emuDQMBooking.xml";
        std::string xmlCanvasesCfg = "file://" + cfgDir + "emuDQMCanvases.xml";
        std::string cscMapFile = cfgDir+"csc_map.txt";

	std::string datafile = "";
  	std::string histofile = "dqm_results.root";
	std::string filter = ""; // folder filter

	std::string plotsdir = "images";	// Output images path
	std::string imgFormat = "png"; 	// Output image format
	uint32_t imgWidth = 1200;	// Output image width
	uint32_t imgHeight = 900;	// Putput image height	
	

	uint32_t dduCheckMask=0xFFFFDFFF;
	uint32_t binCheckMask=0xFFFB3BF6;
	// uint32_t binCheckMask=0xF778FFF6; // ignore CFEB DAV error and Sample Count Error
	uint32_t node=0;

	if (argc>=2) {
		datafile = argv[1];
		if (datafile.find(".root") != std::string::npos) {
			switch (argc) {
                                case 3: filter = argv[2];
                                case 2: datafile = argv[1];
                                break;
                       }
		} else {
			switch (argc) {	
				case 7: binCheckMask = strtoul(argv[6], NULL, 0);	
				case 6: dduCheckMask = strtoul(argv[5], NULL, 0);
				case 5: startEvent = atoi(argv[4]);
				case 4: histofile = argv[3];
				case 3: NumberOfEvents = atoi(argv[2]);
				case 2:	datafile = argv[1];
				break;
			}
		}

	}

	struct stat stats;
	if (stat(datafile.c_str(), &stats)<0) {
		LOG4CPLUS_FATAL(logger, datafile << ": " << 
			strerror(errno));
		exit(-1);	
	}
	EmuFileReader ddu(datafile.c_str(), EmuReader::DDU);
        ddu.open(datafile.c_str());
        LOG4CPLUS_INFO (logger, "Opened data file " << datafile);

	EmuPlotter* plotter = new EmuPlotter();
	plotter->setLogLevel(WARN_LOG_LEVEL);
	plotter->setUnpackingDebug(false);
	plotter->setUnpackingLogLevel(OFF_LOG_LEVEL);
	plotter->setCSCMapFile(cscMapFile);
        plotter->setXMLHistosBookingCfgFile(xmlHistosBookingCfg);
        plotter->setXMLCanvasesCfgFile(xmlCanvasesCfg);
        plotter->book();

	
	if (datafile.find(".bin") != std::string::npos) {	
		histofile = datafile;
		if (histofile.rfind("/") != std::string::npos) 
			histofile.erase(0, histofile.rfind("/")+1);
		plotsdir = histofile;
                plotsdir = plotsdir.replace(plotsdir.find(".bin"), 4, ".plots");
		histofile = histofile.replace(histofile.find(".bin"), 4, ".root");
	}
        if (datafile.find(".raw") != std::string::npos) {
                histofile = datafile;
                if (histofile.rfind("/") != std::string::npos)
                        histofile.erase(0, histofile.rfind("/")+1);
		plotsdir = histofile;
		plotsdir = plotsdir.replace(plotsdir.find(".raw"), 4, ".plots");
                histofile = histofile.replace(histofile.find(".raw"), 4, ".root");
        }

	// Try to extract Node ID from data file name (should match pattern EmuRUInn)
	if (datafile.find("EmuRUI") != std::string::npos) {
		std::string nodestr = datafile.substr(datafile.find("EmuRUI"), 8);
		nodestr.erase(0,6); // remove "EmuRUI"
		node = atoi(nodestr.c_str());
		LOG4CPLUS_INFO (logger, "Found Node ID " << node);
        }

	if (datafile.find(".root") != std::string::npos) {
		LOG4CPLUS_INFO (logger, "Load MEs from ROOT file " << datafile);
		histofile = datafile;
                if (histofile.rfind("/") != std::string::npos)
                        histofile.erase(0, histofile.rfind("/")+1);
                plotsdir = histofile;
		std::string runname = histofile; 
		runname = runname.replace(runname.find(".root"), 5, "");
                plotsdir = plotsdir.replace(plotsdir.find(".root"), 5, ".plots");
		plotter->convertROOTToImages(datafile, plotsdir.c_str(), imgFormat , imgWidth, imgHeight, runname, filter);
		// plotter->loadFromROOTFile(datafile);
		// plotter->saveCanvasImages(plotsdir.c_str(), imgFormat , imgWidth, imgHeight, runname);
		// plotter->generateLayout("csc-layouts.py", "EMU");
		
		// LOG4CPLUS_INFO (logger, "Cleanup before exit");
		// plotter->cleanup();
		delete plotter;
		return 0;
	}
	LOG4CPLUS_INFO (logger,  "Reading Events from " << startEvent
                        << " to " << NumberOfEvents);

	plotter->setHistoFile(histofile.c_str());	
	if (dduCheckMask >= 0) {
        	plotter->setDDUCheckMask(dduCheckMask);
	}
	if (binCheckMask >= 0) {
		plotter->setBinCheckMask(binCheckMask);
	}

	long t0, t1;
        t0 = time(0);

	uint32_t cnt=0;

	while (ddu.readNextEvent()) {
		cnt++;
		int status = 0;
                if( ddu.getErrorFlag()==EmuFileReader::Type2 ) status |= 0x8000;
                if( ddu.getErrorFlag()==EmuFileReader::Type3 ) status |= 0x4000;
                if( ddu.getErrorFlag()==EmuFileReader::Type4 ) status |= 0x2000;
                if( ddu.getErrorFlag()==EmuFileReader::Type5 ) status |= 0x1000;
                if( ddu.getErrorFlag()==EmuFileReader::Type6 ) status |= 0x0800;
		if(status) continue;

		if ((cnt>=startEvent) && (cnt<=(startEvent+NumberOfEvents))) { 
			LOG4CPLUS_DEBUG (logger, "Event#"<< dec << cnt << " **** Buffer size: " << ddu.dataLength() << " bytes");
			plotter->processEvent(ddu.data(), ddu.dataLength(), status, node);
			if (cnt%1000 == 0) LOG4CPLUS_INFO (logger, "Processed Events: "<< dec << cnt);
		}
		if (cnt+1>(startEvent+NumberOfEvents)) break;  
	} 
	t1 = time(0);

	LOG4CPLUS_INFO (logger, "Total Events: " << plotter->getTotalEvents() << ", Readout Rate: " << (plotter->getTotalEvents()/(t1-t0)) << " Events/sec" );
	LOG4CPLUS_INFO (logger, "Good Events: " << plotter->getGoodEventsCount() <<  ", Bad Events: " << plotter->getBadEventsCount());
	LOG4CPLUS_INFO (logger, "Unpacked CSCs Events: " << plotter->getTotalUnpackedCSCs() <<  ", Unpacking Rate: " << (plotter->getTotalUnpackedCSCs()/(t1-t0)) << " CSCs/sec");
	std::map<std::string, ME_List >::iterator itr;
	LOG4CPLUS_INFO (logger, "List of MEs:");
	for (itr=plotter->GetMEs().begin(); itr != plotter->GetMEs().end(); ++itr) {
		LOG4CPLUS_INFO (logger, itr->first);
	}
 	LOG4CPLUS_INFO (logger, "Run time: " << t1-t0 << " seconds");

	plotter->saveToROOTFile(histofile.c_str());
	ddu.close();
	delete plotter;
	
	return 0;
};

