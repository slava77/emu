/*****************************************************************************\
* $Id: Table.cc,v 1.1 2009/10/13 20:29:18 paste Exp $
\*****************************************************************************/
#include "emu/fed/Table.h"

#include "cgicc/Cgicc.h"
#include "emu/fed/TableRow.h"
#include "emu/fed/TableColumn.h"
#include "emu/fed/TableCell.h"
#include "cgicc/HTMLClasses.h"



emu::fed::Table::Table(const std::string &id, const std::string &classes):
CSSElement(id, classes)
{
}



emu::fed::Table::Table(const std::string &id, const std::vector<std::string> &classes):
CSSElement(id, classes)
{
}



emu::fed::Table::Table(const std::vector<std::string> &classes, const std::string &id):
CSSElement(id, classes)
{
}



emu::fed::Table::Table(const Table &myTable):
CSSElement(myTable),
rowVector_(myTable.rowVector_),
columnIDs_(myTable.columnIDs_),
columnClasses_(myTable.columnClasses_)
{
}



emu::fed::Table::~Table()
{
}



emu::fed::Table &emu::fed::Table::operator=(const Table &myTable)
{
	setID(myTable.getID());
	setClasses(myTable.getClasses());
	rowVector_ = myTable.rowVector_;
	columnIDs_ = myTable.columnIDs_;
	columnClasses_ = myTable.columnClasses_;
	return *this;
}



unsigned int emu::fed::Table::rowSize()
{
	return rowVector_.size();
}



emu::fed::TableRow *emu::fed::Table::getRow(const unsigned int &row)
{
	if (row >= rowVector_.size()) {
		for (unsigned int iRow = rowVector_.size(); iRow <= row; ++iRow) {
			rowVector_.push_back(new TableRow());
		}
	}
	return rowVector_[row];
}



emu::fed::TableRow *emu::fed::Table::operator[](const unsigned int &row)
{
	return getRow(row);
}



unsigned int emu::fed::Table::columnSize()
{
	unsigned int maxCols = 0;
	for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
		if ((*iRow)->size() > maxCols) maxCols = (*iRow)->size();
	}
	return maxCols;
}



emu::fed::TableColumn *emu::fed::Table::getColumn(const unsigned int &col)
{
	// Columns are fake, so I have to build one myself.
	TableColumn *column = new TableColumn();
	for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
		column->insertCell((*iRow)->getCell(col));
	}
	if (col >= columnIDs_.size()) {
		for (unsigned int id = columnIDs_.size(); id <= col; ++id) {
			columnIDs_.push_back("");
		}
	}
	if (col >= columnClasses_.size()) {
		for (unsigned int id = columnClasses_.size(); id <= col; ++id) {
			columnClasses_.push_back(std::vector<std::string> ());
		}
	}
	column->setID(columnIDs_[col]).setClasses(columnClasses_[col]);
	return column;
}



emu::fed::TableColumn *emu::fed::Table::operator()(const unsigned int &col)
{
	return getColumn(col);
}



emu::fed::TableCell *emu::fed::Table::getCell(const unsigned int &row, const unsigned int &col)
{
	return getRow(row)->getCell(col);
}



emu::fed::TableCell *emu::fed::Table::operator()(const unsigned int &row, const unsigned int &col)
{
	return getCell(row, col);
}



emu::fed::Table &emu::fed::Table::insertRow(TableRow *myRow, const int &rowNumber)
{
	if (rowNumber < 0) {
		rowVector_.push_back(myRow);
	} else if ((unsigned int) rowNumber >= rowVector_.size()) {
		for (int iRow = rowVector_.size(); iRow <= rowNumber - 1; iRow++) {
			rowVector_.push_back(new TableRow());
		}
		rowVector_.push_back(myRow);
	} else {
		rowVector_.insert(rowVector_.begin() + rowNumber, myRow);
	}
	return *this;
}



emu::fed::Table &emu::fed::Table::insertRow(TableRow &myRow, const int &rowNumber)
{
	return insertRow(&myRow, rowNumber);
}



emu::fed::Table &emu::fed::Table::replaceRow(TableRow *myRow, const unsigned int &rowNumber)
{
	if (rowNumber >= rowVector_.size()) {
		insertRow(myRow, rowNumber);
	} else {
		rowVector_.erase(rowVector_.begin() + rowNumber);
		rowVector_.insert(rowVector_.begin() + rowNumber, myRow);
	}
	return *this;
}



emu::fed::Table &emu::fed::Table::replaceRow(TableRow &myRow, const unsigned int &rowNumber)
{
	return replaceRow(&myRow, rowNumber);
}



emu::fed::Table &emu::fed::Table::insertColumn(TableColumn *myColumn, const int &colNumber)
{
	// Columns are fake, so I have to build the rows manually.
	// A column is one element per row at a specific location.
	int maxCol = colNumber;
	if (colNumber < 0) {
		// Find the largest row.
		for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
			if ((int) (*iRow)->size() > maxCol) maxCol = (*iRow)->size();
		}
	}
	// It is important that the number of rows in the table be maintained, and that all rows are shifted.
	// This mimics the behavior of inserting a row even if the row does not match the columns of the table.
	for (unsigned int iRow = 0; iRow < rowVector_.size(); ++iRow) {
		getRow(iRow)->insertCell(myColumn->getCell(iRow), maxCol);
	}
	// The ID and classes of the column must be stored.
	if ((unsigned int) maxCol >= columnIDs_.size()) {
		for (unsigned int id = columnIDs_.size(); id <= (unsigned int) maxCol; ++id) {
			columnIDs_.push_back("");
		}
	}
	if ((unsigned int) maxCol >= columnClasses_.size()) {
		for (unsigned int id = columnClasses_.size(); id <= (unsigned int) maxCol; ++id) {
			columnClasses_.push_back(std::vector<std::string> ());
		}
	}
	columnIDs_[maxCol] = myColumn->getID();
	columnClasses_[maxCol] = myColumn->getClasses();
	return *this;
}



emu::fed::Table &emu::fed::Table::insertColumn(TableColumn &myColumn, const int &colNumber)
{
	return insertColumn(&myColumn, colNumber);
}



emu::fed::Table &emu::fed::Table::replaceColumn(TableColumn *myColumn, const unsigned int &colNumber)
{
	for (unsigned int iRow = 0; iRow < myColumn->size(); ++iRow) {
		getRow(iRow)->replaceCell(myColumn->getCell(iRow), colNumber);
	}
	// The ID and classes of the column must be stored.
	if (colNumber >= columnIDs_.size()) {
		for (unsigned int id = columnIDs_.size(); id <= colNumber; ++id) {
			columnIDs_.push_back("");
		}
	}
	if (colNumber >= columnClasses_.size()) {
		for (unsigned int id = columnClasses_.size(); id <= colNumber; ++id) {
			columnClasses_.push_back(std::vector<std::string> ());
		}
	}
	columnIDs_[colNumber] = myColumn->getID();
	columnClasses_[colNumber] = myColumn->getClasses();
	return *this;
}



emu::fed::Table &emu::fed::Table::replaceColumn(TableColumn &myColumn, const unsigned int &colNumber)
{
	return replaceColumn(&myColumn, colNumber);
}



emu::fed::Table &emu::fed::Table::replaceCell(TableCell *myCell, const unsigned int &rowNumber, const unsigned int &colNumber)
{
	getRow(rowNumber)->replaceCell(myCell, colNumber);
	
	return *this;
}



std::string emu::fed::Table::toHTML() const
{
	std::ostringstream out;
	
	return out.str();
}



std::string emu::fed::Table::toText() const
{
	std::ostringstream out;
	
	return out.str();
}
