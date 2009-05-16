/*****************************************************************************\
* $Id: Chamber.cc,v 1.4 2009/05/16 18:54:26 paste Exp $
\*****************************************************************************/
#include "emu/fed/Chamber.h"

#include <sstream>
#include <iomanip>



emu::fed::Chamber::Chamber():
name_("???"),
peripheralCrateName_("???"),
endcap_("?"),
plusMinus_(0),
station_(0),
ring_(0),
number_(0)
{

}



emu::fed::Chamber::Chamber(unsigned int plusMinus, unsigned int station, unsigned int ring, unsigned int number, unsigned int pCrateTriggerSector):
name_("???"),
peripheralCrateName_("???"),
endcap_("?"),
plusMinus_(plusMinus),
station_(station),
ring_(ring),
number_(number)
{
	if (plusMinus == 1) endcap_ = "+";
	else if (plusMinus == 2) endcap_ = "-";
	// Generate the name, but only if it makes sense
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
		
		// Generate the peripheral crate name only if it makes sense
		if (pCrateTriggerSector > 0 && pCrateTriggerSector <= 6) {
			std::stringstream nameStream;
			nameStream << "VME" << (endcap_ == "+" ? "p" : "m") << station_ << "_" << pCrateTriggerSector;
		}
	}
}



emu::fed::Chamber::Chamber(std::string endcap, unsigned int station, unsigned int ring, unsigned int number, unsigned int pCrateTriggerSector):
name_("???"),
peripheralCrateName_("???"),
endcap_(endcap),
plusMinus_(0),
station_(station),
ring_(ring),
number_(number)
{
	if (endcap == "+") plusMinus_ = 1;
	else if (endcap == "-") plusMinus_ = 2;
	else endcap = "?";
	// Generate the name, but only if it makes sense
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
		
		// Generate the peripheral crate name only if it makes sense
		if (pCrateTriggerSector > 0 && pCrateTriggerSector <= 6) {
			std::stringstream nameStream;
			nameStream << "VME" << (endcap_ == "+" ? "p" : "m") << station_ << "_" << pCrateTriggerSector;
		}
	}
}
