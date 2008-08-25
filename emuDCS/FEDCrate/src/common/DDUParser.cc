/*****************************************************************************\
* $Id: DDUParser.cc,v 3.7 2008/08/25 12:25:49 paste Exp $
*
* $Log: DDUParser.cc,v $
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

#include "ChamberParser.h"
#include "DDU.h"

emu::fed::DDUParser::DDUParser(xercesc::DOMNode *pNode, int crate, char *fileName):
	slot_(0)
{
	parseNode(pNode);

	fillInt("slot", slot_);
	
	if(slot_ == 0) {
		std::cerr << "No slot specified for DDU! " << std::endl;
	} else { 
		ddu_ = new DDU(slot_);
		//fillInt("skip_vme_load", ddu_->skip_vme_load_); 
		fillInt("gbe_prescale", ddu_->gbe_prescale_); 
		fillHex("killfiber", (int &) ddu_->killfiber_);

		ChamberParser CP = ChamberParser(fileName, crate, slot_);
		ddu_->setChambers(CP.getChambers());
	}
}


