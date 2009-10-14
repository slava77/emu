/*****************************************************************************\
* $Id: Table.cc,v 1.2 2009/10/14 20:02:50 paste Exp $
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
columnVector_(myTable.columnVector_)
{
}



emu::fed::Table::Table(Table &myTable):
CSSElement(myTable),
rowVector_(myTable.rowVector_),
columnVector_(myTable.columnVector_)
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
	columnVector_ = myTable.columnVector_;
	return *this;
}



emu::fed::Table &emu::fed::Table::operator=(Table &myTable)
{
	setID(myTable.getID());
	setClasses(myTable.getClasses());
	rowVector_ = myTable.rowVector_;
	columnVector_ = myTable.columnVector_;
	return *this;
}



unsigned int emu::fed::Table::rowSize()
{
	return rowVector_.size();
}



emu::fed::TableRow *emu::fed::Table::getRow(const unsigned int &row)
{
	if (row >= rowSize()) {
		for (unsigned int iRow = rowSize(); iRow <= row; ++iRow) {
			insertRow(new TableRow());
		}
	}
	return rowVector_[row];
}



emu::fed::TableRow *emu::fed::Table::operator[](const unsigned int &row)
{
	return getRow(row);
}



std::vector<emu::fed::TableRow *> emu::fed::Table::getRows()
{
	return rowVector_;
}



unsigned int emu::fed::Table::columnSize()
{
	return columnVector_.size();
}



emu::fed::TableColumn *emu::fed::Table::getColumn(const unsigned int &col)
{
	if (col >= columnSize()) {
		for (unsigned int iCol = columnSize(); iCol <= col; ++iCol) {
			insertColumn(new TableColumn());
		}
	}
	return columnVector_[col];
}



emu::fed::TableColumn *emu::fed::Table::operator()(const unsigned int &col)
{
	return getColumn(col);
}



std::vector<emu::fed::TableColumn *> emu::fed::Table::getColumns()
{
	return columnVector_;
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
	if (myRow->size() < columnSize()) {
		// If the row being inserted is smaller than the current number of columns, increase its size.
		myRow->insertCell(new TableCell(), columnSize() - 1);
	}
	
	if (rowNumber < 0 || rowNumber == (int) rowSize()) {
		// Tack the row onto the end.
		rowVector_.push_back(myRow);
	} else if (rowNumber > (int) rowSize()) {
		// Insert empty rows until we get to the point where we can insert this row.
		for (int iRow = rowSize(); iRow <= rowNumber - 1; ++iRow) {
			// Make a new row with the same number of elements as the row being inserted.
			TableRow *newRow = new TableRow();
			newRow->insertCell(new TableCell(), myRow->size() - 1);
			rowVector_.push_back(newRow);
		}
		rowVector_.push_back(myRow);
	} else {
		// Insert the row and move everything down one.
		rowVector_.insert(rowVector_.begin() + rowNumber, myRow);
	}
	
	// Now we need to rebuild the columns.
	rebuildColumns();
	
	return *this;
}



emu::fed::Table &emu::fed::Table::insertRow(TableRow &myRow, const int &rowNumber)
{
	return insertRow(&myRow, rowNumber);
}



emu::fed::Table &emu::fed::Table::replaceRow(TableRow *myRow, const unsigned int &rowNumber)
{
	if (rowNumber >= rowSize()) {
		insertRow(myRow, rowNumber);
	} else {
		rowVector_.erase(rowVector_.begin() + rowNumber);
		rowVector_.insert(rowVector_.begin() + rowNumber, myRow);
		rebuildColumns();
	}
	
	return *this;
}



emu::fed::Table &emu::fed::Table::replaceRow(TableRow &myRow, const unsigned int &rowNumber)
{
	return replaceRow(&myRow, rowNumber);
}



emu::fed::Table &emu::fed::Table::insertColumn(TableColumn *myColumn, const int &colNumber)
{
	if (myColumn->size() < rowSize()) {
		// If the row being inserted is smaller than the current number of columns, increase its size.
		myColumn->insertCell(new TableCell(), rowSize() - 1);
	}
	
	if (colNumber < 0 || colNumber == (int) columnSize()) {
		// Tack the row onto the end.
		columnVector_.push_back(myColumn);
	} else if (colNumber > (int) columnSize()) {
		// Insert empty columns until we get to the point where we can insert this column.
		for (int iCol = columnSize(); iCol <= colNumber - 1; ++iCol) {
			// Make a new row with the same number of elements as the row being inserted.
			TableColumn *newColumn = new TableColumn();
			newColumn->insertCell(new TableCell(), myColumn->size() - 1);
			columnVector_.push_back(newColumn);
		}
		columnVector_.push_back(myColumn);
	} else {
		// Insert the row and move everything down one.
		columnVector_.insert(columnVector_.begin() + colNumber, myColumn);
	}
	
	// Now we need to rebuild the rows.
	rebuildRows();
	
	return *this;
}



emu::fed::Table &emu::fed::Table::insertColumn(TableColumn &myColumn, const int &colNumber)
{
	return insertColumn(&myColumn, colNumber);
}



emu::fed::Table &emu::fed::Table::replaceColumn(TableColumn *myColumn, const unsigned int &colNumber)
{
	if (colNumber >= columnSize()) {
		insertColumn(myColumn, colNumber);
	} else {
		columnVector_.erase(columnVector_.begin() + colNumber);
		columnVector_.insert(columnVector_.begin() + colNumber, myColumn);
		rebuildRows();
	}
	
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


emu::fed::Table &emu::fed::Table::clear()
{
	rowVector_.clear();
	columnVector_.clear();
	return *this;
}



std::string emu::fed::Table::toHTML()
{
	std::ostringstream out;
	
	out << cgicc::table()
		.set("id", getID())
		.set("class", getClass());
	out << cgicc::colgroup();
	
	for (std::vector<TableColumn *>::const_iterator iCol = columnVector_.begin(); iCol != columnVector_.end(); ++iCol) {
		out << cgicc::col()
			.set("id", (*iCol)->getID())
			.set("class", (*iCol)->getClass());
	}
	
	out << cgicc::colgroup();
	
	for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
		out << (*iRow)->toHTML();
	}
	
	out << cgicc::table();
	
	return out.str();
}



std::string emu::fed::Table::toText()
{
	std::ostringstream out;
	std::ostringstream border;
	std::ostringstream breaker;
	
	// Make sure the elements in a single column have the same widths.
	for (std::vector<TableColumn *>::iterator iColumn = columnVector_.begin(); iColumn != columnVector_.end(); ++iColumn) {
		unsigned int width = 0;
		std::vector<TableCell *> cells = (*iColumn)->getCells();
		for (std::vector<TableCell *>::iterator iCell = cells.begin(); iCell != cells.end(); ++iCell) {
			if ((*iCell)->str().length() > width) width = (*iCell)->str().length();
		}
		for (std::vector<TableCell *>::iterator iCell = cells.begin(); iCell != cells.end(); ++iCell) {
			(*iCell)->fill(' ');
			(*iCell)->width(width);
		}
	}
	
	border << "+";
	breaker << "+";
	// Make the border and breakers.
	std::vector<TableCell *> cells = getRow(0)->getCells();
	for (std::vector<TableCell *>::const_iterator iCell = cells.begin(); iCell != cells.end(); ++iCell) {
		unsigned int length = (*iCell)->str().length() + 2;
		for (unsigned int iDash = 0; iDash < length; ++iDash) {
			border << "=";
			breaker << "-";
		}
		border << "+";
		breaker << "+";
	}
	
	out << border.str() << std::endl;
	// Print the rows
	for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
		out << (*iRow)->toText() << std::endl;
		if (iRow + 1 != rowVector_.end()) out << breaker.str();
	}
	
	out << border.str();
	
	return out.str();
}



void emu::fed::Table::rebuildColumns()
{
	// Rebuild my column vector from scratch.
	std::vector<TableColumn *> columns;
	for (unsigned int iColumn = 0; iColumn != columnSize(); ++iColumn) {
		TableColumn *newColumn = new TableColumn(*getColumn(iColumn));
		newColumn->clear();
		for (std::vector<TableRow *>::const_iterator iRow = rowVector_.begin(); iRow != rowVector_.end(); ++iRow) {
			newColumn->insertCell((*iRow)->getCell(iColumn));
		}
		columns.push_back(newColumn);
	}
	columnVector_ = columns;
}



void emu::fed::Table::rebuildRows()
{
	// Rebuild my row vector from scratch.
	std::vector<TableRow *> rows;
	for (unsigned int iRow = 0; iRow != rowSize(); ++iRow) {
		TableRow *newRow = new TableRow(*getRow(iRow));
		newRow->clear();
		for (std::vector<TableColumn *>::const_iterator iCol = columnVector_.begin(); iCol != columnVector_.end(); ++iCol) {
			newRow->insertCell((*iCol)->getCell(iRow));
		}
		rows.push_back(newRow);
	}
	rowVector_ = rows;
}
