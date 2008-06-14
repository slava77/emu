#ifndef DataTable_h
#define DataTable_h

#include "FEDException.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "cgicc/Cgicc.h"

using namespace std;


class DataRow
{
	friend class DataTable;
public:
	/** Standard constructor.
	*
	*	@param cols is the number of columns in the row.
	**/
	DataRow(ostringstream myName, unsigned int cols = 3)
		throw (FEDException):
		cols_(cols),
		class_("ok")
	{
		if (cols < 2) XCEPT_RAISE(FEDException, "DataRow requires 2 or more columns.");
		if (cols > 3) other.resize(cols - 3,"");
		name = new stringstream();
		value = new stringstream();
		comments = new stringstream();
		*name << myName;
	}

	/** Standard constructor with strings. **/
	DataRow(string myName, unsigned int cols = 3)
		throw (FEDException):
		cols_(cols),
		class_("ok")
	{
		if (cols < 2) XCEPT_RAISE(FEDException, "DataRow requires 2 or more columns.");
		if (cols > 3) other.resize(cols - 3);
		name = new stringstream();
		value = new stringstream();
		comments = new stringstream();
		*name << myName;
	}

	/** Standard destructor **/
	~DataRow() {}

	inline string className() { return class_; }
	inline void setClassName(string className) { class_ = className; }

	string toHTML()
	{
		ostringstream *out = new ostringstream();
		*out << cgicc::tr()
			.set("style","border-top: solid 1px #000;") << endl;
		for (unsigned int icol = 0; icol < cols_; icol++) {
			if (icol == 1) *out << cgicc::td().set("class",class_);
			else *out << cgicc::td();
			if (icol == 0) *out << name->str();
			else if (icol == 1) {
				*out << value->str() << dec;
			}
			else if (icol == 2) *out << comments->str();
			else *out << other[icol - 3];
			*out << cgicc::td() << endl;
		}
		*out << cgicc::tr();

		return out->str();
	}

	/** Make a form for editing the present value.
	*
	*	@param target is the target URL of the form
	*	@param ddu is the index of the ddu that is being addressed
	*	@param val is the legacy index number for DDU/DCCTextLoad to parse.
	*	@param buttonText is the text you want to appear in the submit button.
	**/
	string makeForm(string target, unsigned int ddu, unsigned int val, string buttonText = "Load")
	{
		ostringstream *out = new ostringstream();

		*out << cgicc::form()
			.set("method","GET")
			.set("action",target) << endl;
		// The DDU
		ostringstream dduText;
		dduText << ddu;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","ddu")
			.set("value",dduText.str()) << endl;
		// The legacy val parameter
		ostringstream valText;
		valText << val;
		*out << cgicc::input()
			.set("type","hidden")
			.set("name","val")
			.set("value",valText.str()) << endl;
		// The current value
		*out << cgicc::input()
			.set("type","text")
			.set("name","textdata")
			.set("size","10")
			.set("ENCTYPE","multipart/form-data")
			.set("value",value->str()) << endl;
		// Submit
		*out << cgicc::input()
			.set("type","submit")
			.set("value",buttonText) << endl;
		*out << cgicc::form() << endl;

		return out->str();
	}

	stringstream *name;
	stringstream *value;
	stringstream *comments;
	vector< string > other;

private:
	unsigned int cols_;
	string class_;
};


class DataTable
{
public:
	/** Standard constructor.
	*
	*	@param id is the HTML id tag of the table.  Should be unique.
	**/
	DataTable(string id)
		throw (FEDException):
		cols_(0),
		id_(id),
		hidden_(false)
	{
		// Everything is done dynamically later.
	}
	/** Standard destructor. **/
	~DataTable() {};

	/** Access a given row. **/
	DataRow *operator[] (unsigned int irow)
		throw (FEDException)
	{
		if (irow < rows_.size()) return rows_[irow];
		else {
			for (unsigned int row = rows_.size(); row <= irow; row++) {
				DataRow *newRow = new DataRow("",cols_);
				rows_.push_back(newRow);
			}
			return rows_[irow];
		}
	}

	void addColumn(string title) {
		headers_.push_back(title);
		cols_++;
		rows_.clear();
	}

	void addRow(DataRow *row) {
		rows_.push_back(row);
	}
	
	inline unsigned int countRows() { return rows_.size(); }

	string toHTML() {
		ostringstream *out = new ostringstream();

		if (hidden_) {
			*out << cgicc::table()
				.set("id",id_)
				.set("class","data")
				.set("style","display: none;") << endl;
		} else {
			*out << cgicc::table()
				.set("id",id_)
				.set("class","data") << endl;
		}
		*out << cgicc::tr()
			.set("style","font-weight: bold;") << endl;
		vector< string >::iterator iCol;
		for (iCol = headers_.begin(); iCol != headers_.end(); iCol++) {
			*out << cgicc::td(*iCol) << endl;
		}
		*out << cgicc::tr() << endl;
		vector< DataRow *>::iterator iRow;
		for (iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			*out << (*iRow)->toHTML() << endl;
		}
		*out << cgicc::table();
		return out->str();
	}

	unsigned int countClass(string className) {
		unsigned int returnVal = 0;
		vector<DataRow *>::iterator iRow;
		for (iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			if ((*iRow)->className() == className) returnVal++;
		}
		return returnVal;
	}

	string printSummary() {
		ostringstream *out = new ostringstream();
		
		std::map<string,unsigned int> classes;
		vector<DataRow *>::iterator iRow;
		for (iRow = rows_.begin(); iRow != rows_.end(); iRow++) {
			classes[(*iRow)->className()]++;
		}
		
		// Print "OK" first:
		std::map<string,unsigned int>::iterator iFound = classes.find("ok");
		if (iFound != classes.end()) {
			*out << cgicc::span()
				.set("class",iFound->first);
			*out << iFound->second << " " << iFound->first;
			*out << cgicc::span() << endl;
		}
		
		std::map<string,unsigned int>::iterator iClass;
		for (iClass = classes.begin(); iClass != classes.end(); iClass++) {
			if (iClass->first == "ok") continue;
			*out << cgicc::span()
				.set("class",iClass->first);
			*out << iClass->second << " " << iClass->first;
			*out << cgicc::span() << endl;
		}
		
		return out->str();
	}

	inline bool isHidden() { return hidden_; }
	inline void setHidden(bool hidden) { hidden_ = hidden; }

private:
	vector< DataRow *> rows_;
	vector< string > headers_;
	unsigned int cols_;
	string id_;
	bool hidden_;
};



#endif
