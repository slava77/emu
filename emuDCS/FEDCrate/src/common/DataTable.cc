/*****************************************************************************\
* $Id: DataTable.cc,v 1.5 2009/02/10 21:08:55 paste Exp $
*
* $Log: DataTable.cc,v $
* Revision 1.5  2009/02/10 21:08:55  paste
* Fixed a memory leak in EmuFCrateHyperDAQ's use of the DataTable class
*
* Revision 1.4  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.3  2008/11/03 23:33:47  paste
* Modifications to fix "missing stylesheet/javascript" problem.
*
* Revision 1.2  2008/10/13 11:56:40  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 1.1  2008/08/25 13:35:26  paste
* Missed another one...
*
*
\*****************************************************************************/
#include "DataTable.h"

#include <iostream>

#include "cgicc/Cgicc.h"

emu::fed::DataElement::DataElement(std::stringstream myValue, std::string className):
class_(className)
{
	(*this) << myValue.str();
	if (class_ != "") element_.set("class", class_);
}



emu::fed::DataElement::DataElement(std::string myValue, std::string className):
class_(className)
{
	(*this) << myValue;
	if (class_ != "") element_.set("class", class_);
}



emu::fed::DataElement::DataElement(const DataElement &myElement)
{
	(*this) << myElement.str();
	class_ = myElement.class_;
	if (class_ != "") element_.set("class", class_);
}



emu::fed::DataElement &emu::fed::DataElement::operator=(const DataElement &myElement)
{
	if (this == myElement) return (*this);
	str("");
	(*this) << myElement.str();
	class_ = myElement.class_;
	if (class_ != "") element_.set("class", class_);
	return (*this);
}



emu::fed::DataElement &emu::fed::DataElement::setClass(const std::string &myClass)
{
	element_.set("class", myClass);
	class_ = myClass;
	return (*this);
}



std::string emu::fed::DataElement::toHTML()
{
	std::stringstream out;
	out << element_ << str() << cgicc::td() << std::endl;
	return out.str();
}



emu::fed::DataRow::DataRow(unsigned int cols, DataElement *myElement)
{
	for (unsigned int ielement = 0; ielement < cols; ielement++) {
		push_back(myElement);
	}
}



emu::fed::DataElement &emu::fed::DataRow::operator[] (unsigned int element)
{
	if (element < size()) return *(at(element));
	else {
		for (unsigned int ielement = size(); ielement <= element; ielement++) {
			DataElement *newElement = new DataElement();
			push_back(newElement);
		}
		return *(at(element));
	}
}



std::string emu::fed::DataRow::toHTML()
{
	std::stringstream out;
	out << element_ << std::endl;
	for (std::vector<DataElement *>::iterator iElement = begin(); iElement != end(); iElement++) {
		out << (*iElement)->toHTML();
	}
	out << cgicc::tr() << std::endl;
	return out.str();
}


/*
std::string emu::fed::DataRow::makeForm(std::string target, unsigned int crate, unsigned int ddu, unsigned int val, std::string buttonText)
{

	if (size() < 2) return "";

	std::stringstream out;

	out << cgicc::form()
		.set("method","GET")
		.set("action",target) << std::endl;
	// The DDU
	std::stringstream dduText;
	dduText << ddu;
	// The Crate
	std::stringstream crateText;
	crateText << crate;
	out << cgicc::input()
		.set("type","hidden")
		.set("name","crate")
		.set("value",crateText.str()) << std::endl;
	out << cgicc::input()
		.set("type","hidden")
		.set("name","ddu")
		.set("value",dduText.str()) << std::endl;
	// The legacy val parameter
	std::stringstream valText;
	valText << val;
	out << cgicc::input()
		.set("type","hidden")
		.set("name","command")
		.set("value",valText.str()) << std::endl;
	// The current value
	out << cgicc::input()
		.set("type","text")
		.set("name","textdata")
		.set("size","10")
		.set("value",(*this)[1].str()) << std::endl;
	// Submit
	out << cgicc::input()
		.set("type","submit")
		.set("value",buttonText) << std::endl;
	out << cgicc::form() << std::endl;

	return out.str();
}
*/


emu::fed::DataTable::DataTable(unsigned int rows, DataRow *myRow)
{
	for (unsigned int irow = 0; irow < rows; irow++) {
		push_back(myRow);
	}
}



emu::fed::DataRow &emu::fed::DataTable::operator[] (unsigned int row)
{
	if (row < size()) return *(at(row));
	else {
		for (unsigned int irow = size(); irow <= row; irow++) {
			DataRow *newRow = new DataRow();
			push_back(newRow);
		}
		return *(at(row));
	}
}



emu::fed::DataElement &emu::fed::DataTable::operator() (unsigned int row, unsigned int col)
{
	return (*this)[row][col];
}



std::string emu::fed::DataTable::toHTML() {
	std::stringstream out;
	out << element_ << std::endl;
	for (std::vector<DataRow *>::iterator iRow = begin(); iRow != end(); iRow++) {
		out << (*iRow)->toHTML();
	}
	out << cgicc::table() << std::endl;

	return out.str();
}



std::map<std::string, unsigned int> emu::fed::DataTable::countClasses() {
	std::map<std::string, unsigned int> classNameCounts;
	for (std::vector<DataRow *>::iterator iRow = begin(); iRow != end(); iRow++) {
		for (std::vector<DataElement *>::iterator iElement = (*iRow)->begin(); iElement != (*iRow)->end(); iElement++) {
			classNameCounts[(*iElement)->getClass()]++;
		}
	}
	return classNameCounts;
}



std::string emu::fed::DataTable::printSummary() {
	std::stringstream out;

	// Grab the classes.
	std::map<std::string, unsigned int> classes = countClasses();

	// Print "OK" first:
	std::map<std::string,unsigned int>::iterator iFound = classes.find("ok");
	if (iFound != classes.end()) {
		out << cgicc::span()
			.set("class", iFound->first);
		out << iFound->second << "/" << size() << " " << iFound->first;
		out << cgicc::span() << std::endl;
	}
	
	std::map<std::string,unsigned int>::iterator iClass;
	for (iClass = classes.begin(); iClass != classes.end(); iClass++) {
		if (iClass->first == "ok") continue;
		out << cgicc::span()
			.set("class",iClass->first);
		out << iClass->second << "/" << size() << " " << iClass->first;
		out << cgicc::span() << std::endl;
	}
	
	return out.str();
}
