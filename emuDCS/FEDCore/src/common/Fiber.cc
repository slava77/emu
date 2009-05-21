/*****************************************************************************\
* $Id: Fiber.cc,v 1.1 2009/05/21 15:33:44 paste Exp $
\*****************************************************************************/
#include "emu/fed/Fiber.h"

#include <sstream>
#include <iomanip>



emu::fed::Fiber::Fiber():
name_("???"),
endcap_("?"),
plusMinus_(0),
station_(0),
ring_(0),
number_(0)
{

}



emu::fed::Fiber::Fiber(unsigned int plusMinus, unsigned int station, unsigned int ring, unsigned int number):
name_("???"),
endcap_("?"),
plusMinus_(plusMinus),
station_(station),
ring_(ring),
number_(number)
{
	if (plusMinus == 1) endcap_ = "+";
	else if (plusMinus == 2) endcap_ = "-";
	// Generate the name, but only if it makes sense
	// Check for a sensible normal chamber name first, then an SP
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
	} else if (endcap_ != "?" && station_ == 0 && ring_ == 0 && number_ != 0) {
		std::ostringstream nameStream;
		nameStream << "SP" << std::setw(2) << std::setfill('0') << number_;
	}
}



emu::fed::Fiber::Fiber(std::string endcap, unsigned int station, unsigned int ring, unsigned int number):
name_("???"),
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
	// Check for a sensible normal chamber name first, then an SP
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
	} else if (endcap_ != "?" && station_ == 0 && ring_ == 0 && number_ != 0) {
		std::ostringstream nameStream;
		nameStream << "SP" << std::setw(2) << std::setfill('0') << number_;
	}
}
