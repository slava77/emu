/*****************************************************************************\
* $Id: CSSElement.cc,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#include "emu/fed/CSSElement.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>



emu::fed::CSSElement::CSSElement(const std::string &id, const std::string &classes):
id_(id)
{
	setClass(classes);
}



emu::fed::CSSElement::CSSElement(const std::string &id, const std::vector<std::string> &classes):
id_(id),
classes_(classes)
{
}



emu::fed::CSSElement::CSSElement(const std::vector<std::string> &classes, const std::string &id):
id_(id),
classes_(classes)
{
}



emu::fed::CSSElement::CSSElement(const CSSElement &myCSSElement):
id_(myCSSElement.id_),
classes_(myCSSElement.classes_)
{
}



emu::fed::CSSElement::CSSElement(CSSElement &myCSSElement):
id_(myCSSElement.id_),
classes_(myCSSElement.classes_)
{
}




emu::fed::CSSElement::~CSSElement()
{
}



emu::fed::CSSElement &emu::fed::CSSElement::operator=(const CSSElement &myCSSElement)
{
	id_ = myCSSElement.id_;
	classes_ = myCSSElement.classes_;
	return *this;
}



emu::fed::CSSElement &emu::fed::CSSElement::operator=(CSSElement &myCSSElement)
{
	id_ = myCSSElement.id_;
	classes_ = myCSSElement.classes_;
	return *this;
}



std::string emu::fed::CSSElement::getID() const
{
	return id_;
}



emu::fed::CSSElement &emu::fed::CSSElement::setID(const std::string &id)
{
	id_ = id;
	return *this;
}



std::string emu::fed::CSSElement::getClass() const
{
	std::ostringstream classes;
	if (classes_.size()) {
		for (std::vector<std::string>::const_iterator iClass = classes_.begin(); iClass != (classes_.end() - 1); ++iClass) {
			classes << (*iClass) << " ";
		}
		classes << classes_.back();
	}
	return classes.str();
}



std::vector<std::string> emu::fed::CSSElement::getClasses() const
{
	return classes_;
}



emu::fed::CSSElement &emu::fed::CSSElement::setClass(const std::string &classes)
{
	classes_.clear();
	boost::split(classes_, classes, boost::algorithm::is_any_of(", "));
	return *this;
}



emu::fed::CSSElement &emu::fed::CSSElement::setClasses(const std::vector<std::string> &classes)
{
	classes_.clear();
	classes_ = classes;
	return *this;
}
