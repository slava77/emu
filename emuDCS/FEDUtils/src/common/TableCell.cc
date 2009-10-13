/*****************************************************************************\
* $Id: TableCell.cc,v 1.1 2009/10/13 20:29:18 paste Exp $
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



std::string emu::fed::TableCell::toHTML() const
{
	std::ostringstream out;
	
	return out.str();
}



std::string emu::fed::TableCell::toText() const
{
	std::ostringstream out;
	
	return out.str();
}
