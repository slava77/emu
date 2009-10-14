/*****************************************************************************\
* $Id: TableCell.cc,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#include "emu/fed/TableCell.h"

#include "cgicc/Cgicc.h"
#include "emu/fed/TableCell.h"
#include "cgicc/HTMLClasses.h"



emu::fed::TableCell::TableCell(const std::string &id, const std::string &classes):
CSSElement(id, classes)
{
}



emu::fed::TableCell::TableCell(const std::string &id, const std::vector<std::string> &classes):
CSSElement(id, classes)
{
}



emu::fed::TableCell::TableCell(const std::vector<std::string> &classes, const std::string &id):
CSSElement(id, classes)
{
}



emu::fed::TableCell::TableCell(const TableCell &myCell):
CSSElement(myCell)
{
	*this << myCell.str();
}



emu::fed::TableCell::TableCell(TableCell &myCell):
CSSElement(myCell)
{
	*this << myCell.str();
}



emu::fed::TableCell::~TableCell()
{
}



emu::fed::TableCell &emu::fed::TableCell::operator=(const TableCell &myCell)
{
	setID(myCell.getID());
	setClasses(myCell.getClasses());
	this->copyfmt(myCell);
	return *this;
}



emu::fed::TableCell &emu::fed::TableCell::operator=(TableCell &myCell)
{
	setID(myCell.getID());
	setClasses(myCell.getClasses());
	this->copyfmt(myCell);
	return *this;
}



std::string emu::fed::TableCell::toHTML()
{
	std::ostringstream out;
	
	out << cgicc::td(this->str())
		.set("id", getID())
		.set("class", getClass());
	
	return out.str();
}



std::string emu::fed::TableCell::toText()
{
	std::ostringstream out;
	
	out << " " << this->str() << " ";
	
	return out.str();
}
