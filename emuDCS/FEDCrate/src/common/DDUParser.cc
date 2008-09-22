/*****************************************************************************\
* $Id: DDUParser.cc,v 3.10 2008/09/22 14:31:54 paste Exp $
*
* $Log: DDUParser.cc,v $
* Revision 3.10  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.9  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.8  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.7  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DDUParser.h"

#include <iostream>

//#include "ChamberParser.h"
#include "DDU.h"

emu::fed::DDUParser::DDUParser(xercesc::DOMElement *pNode):
	slot_(0)
{

	parseNode(pNode);

	fillInt("Slot", slot_);
	
	if(slot_ == 0) {
		std::cerr << "No slot specified for DDU! " << std::endl;
	} else {

		ddu_ = new DDU(slot_);
		//fillInt("skip_vme_load", ddu_->skip_vme_load_);

		fillHex("GbE_prescale", (int &) ddu_->gbe_prescale_);
		//fillHex("killfiber", (int &) ddu_->killfiber_);

		// Kill Fiber is a little more complicated now.
		fillHex("Options", options_);

		//ChamberParser CP = ChamberParser(fileName, crate, slot_);
		//ddu_->setChambers(CP.getChambers());
	}
}


