/*****************************************************************************\
* $Id: DataTable.cc,v 1.1 2008/08/25 13:35:26 paste Exp $
*
* $Log: DataTable.cc,v $
* Revision 1.1  2008/08/25 13:35:26  paste
* Missed another one...
*
*
\*****************************************************************************/
#include "DataTable.h"

#include <iostream>

#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"

emu::fed::DataElement::DataElement(std::stringstream myValue, std::string className):
	class_(className)
{
	*value << myValue;
}



emu::fed::DataElement::DataElement(std::string myValue, std::string klass):
	class_(klass)
{
	value = new std::stringstream();
	*value << myValue;
}



std::string emu::fed::DataElement::toHTML(bool breaks)
{
	std::stringstream *out = new std::stringstream();
	if (breaks) {
		*out << cgicc::td(value->str())
			.set("class",class_)
			.set("style","border-right: 3px double #000;")<< std::endl;
	} else {
		*out << cgicc::td(value->str())
			.set("class",class_) << std::endl;
	}
	return out->str();
}



emu::fed::DataRow::DataRow(std::stringstream myName, unsigned int cols, unsigned long int breaks)
	throw (FEDException):
	breaks_(breaks)
{
	if (cols < 1) XCEPT_RAISE(FEDException, "DataRow requires 1 or more column.");
	for (unsigned int iele = 0; iele < cols; iele++) {
		DataElement *newEle = new DataElement();
		elements_.push_back(newEle);
	}
	*(elements_[0]->value) << myName;
}



emu::fed::DataRow::DataRow(std::string myName, unsigned int cols, unsigned long int breaks)
	throw (FEDException):
	breaks_(breaks)
{
	if (cols < 1) XCEPT_RAISE(FEDException, "DataRow requires 1 or more column.");
	for (unsigned int iele = 0; iele < cols; iele++) {
		DataElement *newEle = new DataElement();
		elements_.push_back(newEle);
	}
	*(elements_[0]->value) << myName;
}



emu::fed::DataElement *emu::fed::DataRow::operator[] (unsigned int element)
	throw (FEDException)
{
	if (element < elements_.size()) return elements_[element];
	else {
		for (unsigned int ielement = elements_.size(); ielement <= element; ielement++) {
			DataElement *newElement = new DataElement("");
			elements_.push_back(newElement);
		}
		return elements_[element];
	}
}



std::string emu::fed::DataRow::toHTML()
{
	std::stringstream *out = new std::stringstream();
	*out << cgicc::tr()
		.set("style","border-top: solid 1px #000;") << std::endl;
	for (unsigned int icol = 0; icol < elements_.size(); icol++) {
		*out << elements_[icol]->toHTML( (breaks_ & (1 << icol)) );
	}
	*out << cgicc::tr() << std::endl;
	return out->str();
}



std::string emu::fed::DataRow::makeForm(std::string target, unsigned int crate, unsigned int ddu, unsigned int val, std::string buttonText)
	throw (FEDException)
{

	if (elements_.size() < 2) XCEPT_RAISE(FEDException, "DataRow::makeForm assumes the second element is the value of the data, so at least 2 elements are required in the DataRow instance before this method can be called.");

	std::stringstream *out = new std::stringstream();

	*out << cgicc::form()
		.set("method","GET")
		.set("action",target) << std::endl;
	// The DDU
	std::stringstream dduText;
	dduText << ddu;
	// The Crate
	std::stringstream crateText;
	crateText << crate;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateText.str()) << std::endl;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","ddu")
		.set("value",dduText.str()) << std::endl;
	// The legacy val parameter
	std::stringstream valText;
	valText << val;
	*out << cgicc::input()
		.set("type","hidden")
		.set("name","command")
		.set("value",valText.str()) << std::endl;
	// The current value
	*out << cgicc::input()
		.set("type","text")
		.set("name","textdata")
		.set("size","10")
		.set("value",elements_[1]->value->str()) << std::endl;
	// Submit
	*out << cgicc::input()
		.set("type","submit")
		.set("value",buttonText) << std::endl;
	*out << cgicc::form() << std::endl;

	return out->str();
}



emu::fed::DataTable::DataTable(std::string id):
	cols_(0),
	id_(id),
	hidden_(false),
	breaks_(0)
{
	//std::cout << "Making a new DataTable at " << this << " with id " << id << std::endl;
	// Everything is done dynamically later.
}



emu::fed::DataRow *emu::fed::DataTable::operator[] (unsigned int row)
{
	if (row < rows_.size()) return rows_[row];
	else {
		for (unsigned int irow = rows_.size(); irow <= row; irow++) {
			DataRow *newRow = new DataRow("",cols_,breaks_);
			rows_.push_back(newRow);
		}
		return rows_[row];
	}
}



emu::fed::DataElement *emu::fed::DataTable::operator() (unsigned int row, unsigned int col)
{
	DataRow *thisRow = (*this)[row];
	return (*thisRow)[col];
}



void emu::fed::DataTable::addColumn(std::string title) {
	headers_.push_back(title);
	cols_++;
	rows_.clear();
}



void emu::fed::DataTable::addBreak()
	throw (FEDException)
{
	if (cols_ == 0) XCEPT_RAISE(FEDException, "DataTable::addBreak requires at least one column.");
	breaks_ |= (1 << (cols_ - 1));
	for (std::vector< DataRow * >::iterator iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
		(*iRow)->setBreaks(breaks_);
	}
}



void emu::fed::DataTable::addRow(DataRow *row) {
	row->setBreaks(breaks_);
	rows_.push_back(row);
}



std::string emu::fed::DataTable::toHTML(bool tableTags) {
	std::stringstream *out = new std::stringstream();

	if (hidden_ && tableTags) {
		*out << cgicc::table()
			.set("id",id_)
			.set("class","data")
			.set("style","display: none;") << std::endl;
	} else if (tableTags) {
		*out << cgicc::table()
			.set("id",id_)
			.set("class","data") << std::endl;
	}
	*out << cgicc::tr()
		.set("style","font-weight: bold;") << std::endl;
	std::vector< std::string >::iterator iCol;
	for (unsigned int icol = 0; icol != headers_.size(); icol++) {
		if (breaks_ & (1 << icol)) {
			*out << cgicc::td(headers_[icol])
				.set("style","border-right: 3px double #000;") << std::endl;
		} else {
			*out << cgicc::td(headers_[icol]) << std::endl;
		}
	}
	*out << cgicc::tr() << std::endl;
	std::vector< DataRow *>::iterator iRow;
	for (iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
		*out << (*iRow)->toHTML() << std::endl;
	}
	if (tableTags) *out << cgicc::table();
	return out->str();
}



unsigned int emu::fed::DataTable::countClass(std::string className) {
	unsigned int returnVal = 0;
	for (std::vector<DataRow *>::iterator iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
		for (unsigned int icol = 0; icol < headers_.size(); icol++) {
			if (headers_[icol] == "Value" && (*(*iRow))[icol]->getClass() == className) returnVal++;
		}
	}
	return returnVal;
}



std::string emu::fed::DataTable::printSummary() {
	std::stringstream *out = new std::stringstream();

	unsigned int nTotal = 0;

	// Grab the classes.  Ignore "none".
	std::map<std::string,unsigned int> classes;
	for (unsigned int irow = 0; irow != rows_.size(); irow++) {
		for (unsigned int icol = 0; icol != cols_; icol++) {
			if (headers_[icol] != "Value" || (*this)(irow,icol)->getClass() == "none") continue;
			classes[(*this)(irow,icol)->getClass()]++;
			nTotal++;
		}
	}

	// Print "OK" first:
	std::map<std::string,unsigned int>::iterator iFound = classes.find("ok");
	if (iFound != classes.end()) {
		*out << cgicc::span()
			.set("class",iFound->first);
		*out << iFound->second << "/" << nTotal << " " << iFound->first;
		*out << cgicc::span() << std::endl;
	}
	
	std::map<std::string,unsigned int>::iterator iClass;
	for (iClass = classes.begin(); iClass != classes.end(); iClass++) {
		if (iClass->first == "ok") continue;
		*out << cgicc::span()
			.set("class",iClass->first);
		*out << iClass->second << "/" << nTotal << " " << iClass->first;
		*out << cgicc::span() << std::endl;
	}
	
	return out->str();
}
