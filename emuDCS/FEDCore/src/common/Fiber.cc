/*****************************************************************************\
* $Id: Fiber.cc,v 1.5 2009/07/01 14:17:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/Fiber.h"

#include <sstream>
#include <iomanip>



emu::fed::Fiber::Fiber():
fiberNumber_(0),
name_("???"),
endcap_("?"),
plusMinus_(0),
station_(0),
ring_(0),
number_(0),
killed_(false)
{

}



emu::fed::Fiber::Fiber(const unsigned int &fiberNumber, const unsigned int &plusMinus, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed):
fiberNumber_(fiberNumber),
name_("???"),
endcap_("?"),
plusMinus_(plusMinus),
station_(station),
ring_(ring),
number_(number),
killed_(killed)
{
	if (plusMinus == 1) endcap_ = "+";
	else if (plusMinus == 2) endcap_ = "-";
	// Generate the name, but only if it makes sense
	// Check for a sensible normal chamber name first, then an SP
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
		name_ = nameStream.str();
	} else if (endcap_ != "?" && station_ == 0 && ring_ == 0 && number_ != 0) {
		std::ostringstream nameStream;
		nameStream << "SP" << std::setw(2) << std::setfill('0') << number_;
		name_ = nameStream.str();
	}
}



emu::fed::Fiber::Fiber(const unsigned int &fiberNumber, const std::string &endcap, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed):
fiberNumber_(fiberNumber),
name_("???"),
endcap_(endcap),
plusMinus_(0),
station_(station),
ring_(ring),
number_(number),
killed_(killed)
{
	if (endcap == "+") plusMinus_ = 1;
	else if (endcap == "-") plusMinus_ = 2;
	else endcap_ = "?";
	// Generate the name, but only if it makes sense
	// Check for a sensible normal chamber name first, then an SP
	if (endcap_ != "?" && (station_ > 0 && station_ <= 4) && (ring_ > 0 && ring_ <= 4) && (number_ > 0 && number_ <= 36)) {
		std::ostringstream nameStream;
		nameStream << endcap_ << station_ << "/" << ring_ << "/" << std::setw(2) << std::setfill('0') << number_;
		name_ = nameStream.str();
	} else if (endcap_ != "?" && station_ == 0 && ring_ == 0 && number_ != 0) {
		std::ostringstream nameStream;
		nameStream << "SP" << std::setw(2) << std::setfill('0') << number_;
		name_ = nameStream.str();
	}
}
