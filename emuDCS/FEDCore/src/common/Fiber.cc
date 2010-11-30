/*****************************************************************************\
* $Id: Fiber.cc,v 1.8 2010/11/30 10:08:11 cvuosalo Exp $
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
killed_(false),
ignoreErr_(false)
{

}



emu::fed::Fiber::Fiber(const unsigned int &fiberNumber, const std::string &name, const bool &killed, const bool &ignoreErr):
fiberNumber_(fiberNumber),
name_("???"),
endcap_("?"),
plusMinus_(0),
station_(0),
ring_(0),
number_(0),
killed_(killed),
ignoreErr_(ignoreErr)
{
	parseName_(name);
}



emu::fed::Fiber::Fiber(const unsigned int &fiberNumber, const unsigned int &plusMinus, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed, const bool &ignoreErr):
fiberNumber_(fiberNumber),
name_("???"),
endcap_("?"),
plusMinus_(plusMinus),
station_(station),
ring_(ring),
number_(number),
killed_(killed),
ignoreErr_(ignoreErr)
{
	if (plusMinus == 1) endcap_ = "+";
	else if (plusMinus == 2) endcap_ = "-";
	generateName_();
}



emu::fed::Fiber::Fiber(const unsigned int &fiberNumber, const std::string &endcap, const unsigned int &station, const unsigned int &ring, const unsigned int &number, const bool &killed, const bool &ignoreErr):
fiberNumber_(fiberNumber),
name_("???"),
endcap_(endcap),
plusMinus_(0),
station_(station),
ring_(ring),
number_(number),
killed_(killed),
ignoreErr_(ignoreErr)
{
	if (endcap == "+") plusMinus_ = 1;
	else if (endcap == "-") plusMinus_ = 2;
	else endcap_ = "?";
	generateName_();
}



void emu::fed::Fiber::parseName_(const std::string &name)
{
	// Check normal station name first
	if (sscanf(name.c_str(), "-%1u/%1u/%02u", &station_, &ring_, &number_) == 3) {
		endcap_ = "-";
		// CGICC does not understand that %2B is a plus-sign, so check for that here
	} else if (sscanf(name.c_str(), "+%1u/%1u/%02u", &station_, &ring_, &number_) == 3) {
		endcap_ = "+";
	} else if (sscanf(name.c_str(), "%1u/%1u/%02u", &station_, &ring_, &number_) == 3) {
		endcap_ = "+";
		// Else it's probably an SP, so check that
	} else if (sscanf(name.c_str(), "SP%02u", &number_) == 1) {
		station_ = 0;
		ring_ = 0;
		endcap_ = (number_ <= 6) ? "+" : "-";
	}

	generateName_();
}



void emu::fed::Fiber::generateName_()
{
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
