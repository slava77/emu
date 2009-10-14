/*****************************************************************************\
* $Id: TableRow.cc,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#include "emu/fed/TableRow.h"

#include "cgicc/Cgicc.h"
#include "emu/fed/TableCell.h"
#include "cgicc/HTMLClasses.h"



emu::fed::TableRow::TableRow(const std::string &id, const std::string &classes):
CSSElement(id, classes)
{
}



emu::fed::TableRow::TableRow(const std::string &id, const std::vector<std::string> &classes):
CSSElement(id, classes)
{
}



emu::fed::TableRow::TableRow(const std::vector<std::string> &classes, const std::string &id):
CSSElement(id, classes)
{
}



emu::fed::TableRow::TableRow(const TableRow &myRow):
CSSElement(myRow),
cellVector_(myRow.cellVector_)
{
}



emu::fed::TableRow::TableRow(TableRow &myRow):
CSSElement(myRow),
cellVector_(myRow.cellVector_)
{
}



emu::fed::TableRow::~TableRow()
{
}



emu::fed::TableRow &emu::fed::TableRow::operator=(const TableRow &myRow)
{
	setID(myRow.getID());
	setClasses(myRow.getClasses());
	cellVector_ = myRow.cellVector_;
	return *this;
}



emu::fed::TableRow &emu::fed::TableRow::operator=(TableRow &myRow)
{
	setID(myRow.getID());
	setClasses(myRow.getClasses());
	cellVector_ = myRow.cellVector_;
	return *this;
}



unsigned int emu::fed::TableRow::size()
{
	return cellVector_.size();
}



emu::fed::TableCell *emu::fed::TableRow::getCell(const unsigned int &col)
{
	if (col >= cellVector_.size()) {
		for (unsigned int iCol = cellVector_.size(); iCol <= col; ++iCol) {
			cellVector_.push_back(new TableCell());
		}
	}
	return cellVector_[col];
}



std::vector<emu::fed::TableCell *> emu::fed::TableRow::getCells()
{
	return cellVector_;
}



emu::fed::TableCell *emu::fed::TableRow::operator[](const unsigned int &col)
{
	return getCell(col);
}



emu::fed::TableRow &emu::fed::TableRow::insertCell(TableCell *myCell, const int &colNumber)
{
	if (colNumber < 0) {
		cellVector_.push_back(myCell);
	} else if ((unsigned int) colNumber >= cellVector_.size()) {
		for (int iCol = cellVector_.size(); iCol <= colNumber - 1; iCol++) {
			cellVector_.push_back(new TableCell());
		}
		cellVector_.push_back(myCell);
	} else {
		cellVector_.insert(cellVector_.begin() + colNumber, myCell);
	}
	return *this;
}



emu::fed::TableRow &emu::fed::TableRow::insertCell(TableCell &myCell, const int &colNumber)
{
	return insertCell(&myCell, colNumber);
}



emu::fed::TableRow &emu::fed::TableRow::replaceCell(TableCell *myCell, const unsigned int &colNumber)
{
	if (colNumber >= cellVector_.size()) {
		insertCell(myCell, colNumber);
	} else {
		cellVector_.erase(cellVector_.begin() + colNumber);
		cellVector_.insert(cellVector_.begin() + colNumber, myCell);
	}
	return *this;
}



emu::fed::TableRow &emu::fed::TableRow::replaceCell(TableCell &myCell, const unsigned int &colNumber)
{
	return replaceCell(&myCell, colNumber);
}



emu::fed::TableRow &emu::fed::TableRow::clear()
{
	cellVector_.clear();
	return *this;
}



std::string emu::fed::TableRow::toHTML()
{
	std::ostringstream out;
	
	out << cgicc::tr()
		.set("id", getID())
		.set("class", getClass());
	
	for (std::vector<TableCell *>::const_iterator iCell = cellVector_.begin(); iCell != cellVector_.end(); ++iCell) {
		out << (*iCell)->toHTML();
	}

	out << cgicc::tr();
	
	return out.str();
}



std::string emu::fed::TableRow::toText()
{
	std::ostringstream out;
	
	out << "|";
	for (std::vector<TableCell *>::const_iterator iCell = cellVector_.begin(); iCell != cellVector_.end(); ++iCell) {
		out << (*iCell)->toText() << "|";
	}
	
	return out.str();
}
