#include "DDUParser.h"


DDUParser::DDUParser(DOMNode * pNode, int crate)
{
	parseNode(pNode);
	
	int slot = 0;
	fillInt("slot", slot);
	
	if(slot == 0) {
		cerr << "No slot specified for DDU! " << endl;
	} else { 
		ddu_ = new DDU(slot);
		fillInt("skip_vme_load", ddu_->skip_vme_load_); 
		fillInt("gbe_prescale", ddu_->gbe_prescale_); 
		fillHex("killfiber", ddu_->killfiber_);
	}
}



























