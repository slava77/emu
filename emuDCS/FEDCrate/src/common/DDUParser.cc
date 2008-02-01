#include "DDUParser.h"

using namespace std;

DDUParser::DDUParser(DOMNode * pNode, int crate, char *fileName): slot_(0)
{
	parseNode(pNode);

	fillInt("slot", slot_);
	
	if(slot_ == 0) {
		cerr << "No slot specified for DDU! " << endl;
	} else { 
		ddu_ = new DDU(slot_);
		fillInt("skip_vme_load", ddu_->skip_vme_load_); 
		fillInt("gbe_prescale", ddu_->gbe_prescale_); 
		fillHex("killfiber", ddu_->killfiber_);

		ChamberParser CP = ChamberParser(fileName, crate, slot_);
		ddu_->setChambers(CP.chamberVector());
	}
}


