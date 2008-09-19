/*****************************************************************************\
* $Id: DCCParser.cc,v 3.6 2008/09/19 16:53:52 paste Exp $
*
* $Log: DCCParser.cc,v $
* Revision 3.6  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DCCParser.h"

#include <iostream>

#include "DCC.h"

emu::fed::DCCParser::DCCParser(xercesc::DOMElement *pNode)
{
	parseNode(pNode);
	
	int slot = 0;
	fillInt("Slot", slot);
	if(slot == 0) {
		std::cerr << "No slot specified for DCC! " << std::endl;
	} else { 
		dcc_ = new DCC(slot);
		fillHex("FIFO_in_use", dcc_->fifoinuse_);
		//fillHex("softwareswitch", dcc_->softsw_);
	}
}



























