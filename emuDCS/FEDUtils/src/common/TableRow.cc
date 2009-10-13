/*****************************************************************************\
* $Id: TableRow.cc,v 1.1 2009/10/13 20:29:18 paste Exp $
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



std::string emu::fed::TableRow::toHTML() const
{
	std::ostringstream out;
	
	return out.str();
}



std::string emu::fed::TableRow::toText() const
{
	std::ostringstream out;
	
	return out.str();
}
