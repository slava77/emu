#include "EmuPlotter.h"

EmuPlotter::EmuPlotter():
	logger_(Logger::getInstance("EmuPlotter")),
	dduCheckMask(0xFFFFFFFF),
	binCheckMask(0xFFFFFFFF)
{
	xmlHistosBookingCfgFile = "";
	xmlCanvasesCfgFile = "";
	setParameters();
}


EmuPlotter::EmuPlotter(Logger logger):
        logger_(logger),
	dduCheckMask(0xFFFFFFFF),
        binCheckMask(0xFFFFFFFF)
{
	xmlHistosBookingCfgFile = "";
        xmlCanvasesCfgFile = "";
	setParameters();
}

void EmuPlotter::setParameters() 
{
	nEvents = 0;
	L1ANumber = 0;
	BXN = 0;
	// dduCheckMask = 0xFFFFFFFF;
	// binCheckMask = 0xFFFFFFFF;
        bin_checker.output1().hide();
        bin_checker.output2().hide();
        bin_checker.crcALCT(true);
        bin_checker.crcTMB (true);
        bin_checker.crcCFEB(true);
}

void EmuPlotter::clearMECollection(ME_List & collection) 
{

	if (collection.size() > 0) {
		for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
        	}
		collection.clear();
	}

}
void EmuPlotter::printMECollection(ME_List & collection)
{
	int i = 0;
        for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
		LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


void EmuPlotter::clearCanvasesCollection(MECanvases_List & collection)
{

        if (collection.size() > 0) {
                for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
                }
                collection.clear();
        }

}
void EmuPlotter::printCanvasesCollection(MECanvases_List & collection)
{
        int i = 0;
        for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


EmuPlotter::~EmuPlotter() {
	std::map<std::string, ME_List >::iterator itr;	
	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
		clearMECollection(itr->second);
	}

	MEs.clear();
	clearMECollection(commonMEfactory);
	clearMECollection(chamberMEfactory);
	clearMECollection(dduMEfactory);
}

void EmuPlotter::book() {
	if (MEs.size() > 0) {
		std::map<std::string, ME_List >::iterator itr;
        	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
                	clearMECollection(itr->second);
        	}
        	MEs.clear();
	}
	if (MECanvases.size() > 0) {
                std::map<std::string, MECanvases_List >::iterator itr;
                for (itr = MECanvases.begin(); itr != MECanvases.end(); ++itr) {
                        clearCanvasesCollection(itr->second);
                }
                MECanvases.clear();
        }

/*
        clearMECollection(commonMEfactory);
        clearMECollection(chamberMEfactory);
        clearMECollection(globalMEfactory);
*/
//	LOG4CPLUS_INFO(logger_, "XML Config file: "  <<  xmlCfgFile)
//	loadXMLBookingInfo(xmlCfgFile);
//	setParameters();
        

	if (loadXMLBookingInfo(xmlHistosBookingCfgFile) == 0) 
	{
		setParameters();
/*
		string common = Form("EMU_%d",node);
		MEs[common] = bookCommon(node);
		LOG4CPLUS_INFO(logger_,"=== ME Collection for " << common);
	        printMECollection(MEs[common]);
*/
	}
	loadXMLCanvasesInfo(xmlCanvasesCfgFile);

}

bool EmuPlotter::isMEvalid(ME_List& MEs, string name, EmuMonitoringObject*& me)
{
	ME_List_iterator res = MEs.find(name);
	if (res != MEs.end() && (res->second != 0)) {
		me = res->second;
		// cout << "Found " << me->getName() << endl;
		return true;
	} else {
		LOG4CPLUS_WARN(logger_, "Can not find ME " << name);
		me = 0;
		return false;
	}
	
} 

