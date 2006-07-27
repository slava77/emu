#include "EmuPlotter.h"

EmuPlotter::EmuPlotter():
	logger_(Logger::getInstance("EmuPlotter"))
{
	xmlCfgFile = "";
	setParameters();
}


EmuPlotter::EmuPlotter(Logger logger):
        logger_(logger)
{
	xmlCfgFile = "";
	setParameters();
}

void EmuPlotter::setParameters() 
{
	nEvents = 0;
	L1ANumber = 0;
	BXN = 0;
	dduCheckMask = 0xFFFFFFFF;
	//binCheckMask = 0xFFFFFFFF;
	binCheckMask = 0xDFCB7BF6;
        bin_checker.output1().hide();
        bin_checker.output2().hide();
        bin_checker.crcALCT(true);
        bin_checker.crcTMB (true);
        bin_checker.crcCFEB(true);
        bin_checker.crcDDU (true);
}

void EmuPlotter::clearMECollection(std::map<std::string, EmuMonitoringObject*> & collection) 
{
	
	std::map<string, EmuMonitoringObject*>::iterator itr;
	if (collection.size() > 0) {
	/*
		for (itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
        	}
	*/
		collection.clear();
	}

}
void EmuPlotter::printMECollection(std::map<std::string, EmuMonitoringObject*> & collection)
{
        std::map<string, EmuMonitoringObject*>::iterator itr;
	int i = 0;
        for (itr = collection.begin();itr != collection.end(); ++itr) {
		LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


EmuPlotter::~EmuPlotter() {
	std::map<std::string, std::map<std::string, EmuMonitoringObject*> >::iterator itr;	
	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
		clearMECollection(itr->second);
	}

	MEs.clear();
	clearMECollection(commonMEfactory);
	clearMECollection(chamberMEfactory);
	clearMECollection(globalMEfactory);
}

void EmuPlotter::book(int node) {
	if (MEs.size() > 0) {
		std::map<std::string, std::map<std::string, EmuMonitoringObject*> >::iterator itr;
        	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
                	clearMECollection(itr->second);
        	}
        	MEs.clear();
	}
/*
        clearMECollection(commonMEfactory);
        clearMECollection(chamberMEfactory);
        clearMECollection(globalMEfactory);
*/
//	LOG4CPLUS_INFO(logger_, "XML Config file: "  <<  xmlCfgFile)
	loadXMLBookingInfo(xmlCfgFile);
	setParameters();
        
/*
	if (loadXMLBookingInfo(xmlCfgFile) == 0) 

	{
		string common = Form("DDU%d",node);
		MEs[common] = bookCommon(node);
		LOG4CPLUS_INFO(logger_,"=== ME Collection for " << common);
	        printMECollection(MEs[common]);

	}
*/
}



