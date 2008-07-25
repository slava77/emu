#ifndef DataTable_h
#define DataTable_h

#include "FEDException.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "cgicc/Cgicc.h"

using namespace std;


class DataElement
{
	friend class DataRow;
public:
	/** Standard constructor.
	*
	*	@param value is the value of the pacticular element.
	*	@param class is the class of the particular element.
	**/
	DataElement(ostringstream myValue, string className = "none"):
		class_(className)
	{
		*value << myValue;
	}

	/** Standard constructor with strings.
	*
	*	@param value is the value of the pacticular element.
	*	@param class is the class of the particular element.
	**/
	DataElement(string myValue = "", string klass = "none"):
		class_(klass)
	{
		value = new stringstream();
		*value << myValue;
	}

	/** Standard destructor **/
	~DataElement() {}

	/** Set the value straight away. **/
	void operator= (string myValue)
	{
		value->clear();
		*value << value;
	}

	/** Also set the value straight away **/
	inline void setValue(stringstream myValue)
	{
		value->clear();
		*value << value;
	}

	/** Also set the value straight away **/
	inline void setValue(string myValue)
	{
		value->clear();
		*value << value;
	}

	/** Set the CSS class **/
	inline void setClass(string className) { class_ = className; }

	/** Get the CSS class **/
	inline string getClass() { return class_; }

	/** Display as an HTML table element **/
	string toHTML(bool breaks = false)
	{
		ostringstream *out = new ostringstream();
		if (breaks) {
			*out << cgicc::td(value->str())
				.set("class",class_)
				.set("style","border-right: 3px double #000;")<< endl;
		} else {
			*out << cgicc::td(value->str())
				.set("class",class_) << endl;
		}
		return out->str();
	}

	stringstream *value;

private:
	string class_;
};

class DataRow
{
	friend class DataTable;
public:
	/** Standard constructor.
	*
	*	@param cols is the number of columns in the row.
	**/
	DataRow(ostringstream myName, unsigned int cols = 3, unsigned long int breaks = 0)
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

	/** Standard constructor with strings. **/
	DataRow(string myName, unsigned int cols = 3, unsigned long int breaks = 0)
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

	/** Standard destructor **/
	~DataRow() {}

	/** Access a given element. **/
	DataElement *operator[] (unsigned int element)
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

	/** Set which elements to draw a line after (using borders). **/
	inline void setBreaks(unsigned long int breaks) { breaks_ = breaks; }

	/** Get the class of the second element.  This is assumed to be the data
	*	element.
	**/
/*
	std::string getClass()
		throw (FEDException)
	{
		for (std::vector< DataElement * >::iterator iElem = elements_.begin(); iElem != elements_.end(); iElem++) {
			if ((*iElem)->getClass() != "none" && (*iElem)->getClass() != "") return (*iElem)->getClass();
		}
		return "none";
	}
*/

	/** Set the class of the second element. **/
	void setClass(string className)
		throw (FEDException)
	{
		if (elements_.size() < 2) XCEPT_RAISE(FEDException, "DataRow::getClass assumes that the second element is the data element.  You need at least two elements in the DataRow to call this method.");
		elements_[1]->setClass(className);
	}

	string toHTML()
	{
		ostringstream *out = new ostringstream();
		*out << cgicc::tr()
			.set("style","border-top: solid 1px #000;") << endl;
		for (unsigned int icol = 0; icol < elements_.size(); icol++) {
			*out << elements_[icol]->toHTML( (breaks_ & (1 << icol)) );
		}
		*out << cgicc::tr() << endl;
		return out->str();
	}

	/** Make a form for editing the present value.
	*
	*	@param target is the target URL of the form
	*	@param ddu is the index of the ddu that is being addressed
	*	@param val is the legacy index number for DDU/DCCTextLoad to parse.
	*	@param buttonText is the text you want to appear in the submit button.
	**/
	string makeForm(string target, unsigned int crate, unsigned int ddu, unsigned int val, string buttonText = "Load")
		throw (FEDException)
	{

		if (elements_.size() < 2) XCEPT_RAISE(FEDException, "DataRow::makeForm assumes the second element is the value of the data, so at least 2 elements are required in the DataRow instance before this method can be called.");

		ostringstream *out = new ostringstream();

		*out << cgicc::form()
			.set("method","GET")
			.set("action",target) << endl;
		// The DDU
		ostringstream dduText;
		dduText << ddu;
		// The Crate
		ostringstream crateText;
		crateText << crate;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","crate")
			.set("value",crateText.str()) << endl;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","ddu")
			.set("value",dduText.str()) << endl;
		// The legacy val parameter
		ostringstream valText;
		valText << val;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","command")
			.set("value",valText.str()) << endl;
		// The current value
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("size","10")
			.set("value",elements_[1]->value->str()) << endl;
		// Submit
		*out << cgicc::input()
			.set("type","submit")
			.set("value",buttonText) << endl;
		*out << cgicc::form() << endl;

		return out->str();
	}

private:
	vector< DataElement * > elements_;
	unsigned long int breaks_;
};


class DataTable
{
public:
	/** Standard constructor.
	*
	*	@param id is the HTML id tag of the table.  Should be unique.
	**/
	DataTable(string id):
		cols_(0),
		id_(id),
		hidden_(false),
		breaks_(0)
	{
		//cout << "Making a new DataTable at " << this << " with id " << id << endl;
		// Everything is done dynamically later.
	}

	/** Standard destructor. **/
	~DataTable() {};

	/** Access a given row. **/
	DataRow *operator[] (unsigned int row)
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

	/** Access a given element. **/
	DataElement *operator() (unsigned int row, unsigned int col)
	{
		DataRow *thisRow = (*this)[row];
		return (*thisRow)[col];
	}

	void addColumn(string title) {
		headers_.push_back(title);
		cols_++;
		rows_.clear();
	}

	/** Put a border-defined break to the right of the last defined column. **/
	void addBreak()
		throw (FEDException)
	{
		if (cols_ == 0) XCEPT_RAISE(FEDException, "DataTable::addBreak requires at least one column.");
		breaks_ |= (1 << (cols_ - 1));
		for (std::vector< DataRow * >::iterator iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			(*iRow)->setBreaks(breaks_);
		}
	}

	void addRow(DataRow *row) {
		row->setBreaks(breaks_);
		rows_.push_back(row);
	}
	
	inline unsigned int countRows() { return rows_.size(); }

	string toHTML(bool tableTags = true) {
		ostringstream *out = new ostringstream();

		if (hidden_ && tableTags) {
			*out << cgicc::table()
				.set("id",id_)
				.set("class","data")
				.set("style","display: none;") << endl;
		} else if (tableTags) {
			*out << cgicc::table()
				.set("id",id_)
				.set("class","data") << endl;
		}
		*out << cgicc::tr()
			.set("style","font-weight: bold;") << endl;
		vector< string >::iterator iCol;
		for (unsigned int icol = 0; icol != headers_.size(); icol++) {
			if (breaks_ & (1 << icol)) {
				*out << cgicc::td(headers_[icol])
					.set("style","border-right: 3px double #000;") << endl;
			} else {
				*out << cgicc::td(headers_[icol]) << endl;
			}
		}
		*out << cgicc::tr() << endl;
		vector< DataRow *>::iterator iRow;
		for (iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			*out << (*iRow)->toHTML() << endl;
		}
		if (tableTags) *out << cgicc::table();
		return out->str();
	}

	unsigned int countClass(string className) {
		unsigned int returnVal = 0;
		for (vector<DataRow *>::iterator iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			for (unsigned int icol = 0; icol < headers_.size(); icol++) {
				if (headers_[icol] == "Value" && (*(*iRow))[icol]->getClass() == className) returnVal++;
			}
		}
		return returnVal;
	}

	string printSummary() {
		ostringstream *out = new ostringstream();

		unsigned int nTotal = 0;

		// Grab the classes.  Ignore "none".
		std::map<string,unsigned int> classes;
		for (unsigned int irow = 0; irow != rows_.size(); irow++) {
			for (unsigned int icol = 0; icol != cols_; icol++) {
				if (headers_[icol] != "Value" || (*this)(irow,icol)->getClass() == "none") continue;
				classes[(*this)(irow,icol)->getClass()]++;
				nTotal++;
			}
		}

		// Print "OK" first:
		std::map<string,unsigned int>::iterator iFound = classes.find("ok");
		if (iFound != classes.end()) {
			*out << cgicc::span()
				.set("class",iFound->first);
			*out << iFound->second << "/" << nTotal << " " << iFound->first;
			*out << cgicc::span() << endl;
		}
		
		std::map<string,unsigned int>::iterator iClass;
		for (iClass = classes.begin(); iClass != classes.end(); iClass++) {
			if (iClass->first == "ok") continue;
			*out << cgicc::span()
				.set("class",iClass->first);
			*out << iClass->second << "/" << nTotal << " " << iClass->first;
			*out << cgicc::span() << endl;
		}
		
		return out->str();
	}

	inline bool isHidden() { return hidden_; }
	inline void setHidden(bool hidden) { hidden_ = hidden; }

private:
	std::vector< DataRow *> rows_;
	std::vector< std::string > headers_;
	unsigned int cols_;
	std::string id_;
	bool hidden_;
	unsigned long int breaks_;
};




#endif
