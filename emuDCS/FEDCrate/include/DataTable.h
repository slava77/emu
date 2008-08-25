/*****************************************************************************\
* $Id: DataTable.h,v 1.7 2008/08/25 12:25:49 paste Exp $
*
* $Log: DataTable.h,v $
* Revision 1.7  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.6  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DATATABLE_H__
#define __DATATABLE_H__

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "FEDException.h"

namespace emu {
	namespace fed {

		class DataElement
		{
			friend class DataRow;
		public:
			/** Standard constructor.
			*
			*	@param value is the value of the pacticular element.
			*	@param class is the class of the particular element.
			**/
			DataElement(std::stringstream myValue, std::string className = "none");
		
			/** Standard constructor with std::strings.
			*
			*	@param value is the value of the pacticular element.
			*	@param class is the class of the particular element.
			**/
			DataElement(std::string myValue = "", std::string klass = "none");
		
			/** Standard destructor **/
			~DataElement() {}
		
			/** Set the value straight away. **/
			inline std::string operator= (std::string myValue)
			{
				value->clear();
				*value << value;
				return value->str();
			}
		
			/** Also set the value straight away **/
			inline void setValue(std::stringstream myValue)
			{
				value->clear();
				*value << value;
			}
		
			/** Also set the value straight away **/
			inline void setValue(std::string myValue)
			{
				value->clear();
				*value << value;
			}
		
			/** Set the CSS class **/
			inline void setClass(std::string className) { class_ = className; }
		
			/** Get the CSS class **/
			inline std::string getClass() { return class_; }
		
			/** Display as an HTML table element **/
			std::string toHTML(bool breaks = false);
		
			std::stringstream *value;
		
		private:
			std::string class_;
		};
		
		class DataRow
		{
			friend class DataTable;
		public:
			/** Standard constructor.
			*
			*	@param cols is the number of columns in the row.
			**/
			DataRow(std::stringstream myName, unsigned int cols = 3, unsigned long int breaks = 0)
				throw (FEDException);
		
			/** Standard constructor with std::strings. **/
			DataRow(std::string myName, unsigned int cols = 3, unsigned long int breaks = 0)
				throw (FEDException);
		
			/** Standard destructor **/
			~DataRow() {}
		
			/** Access a given element. **/
			DataElement *operator[] (unsigned int element)
				throw (FEDException);
		
			/** Set which elements to draw a line after (using borders). **/
			inline void setBreaks(unsigned long int breaks) { breaks_ = breaks; }
		
		
			/** Set the class of the second element. **/
			inline void setClass(std::string className)
				throw (FEDException)
			{
				if (elements_.size() < 2) XCEPT_RAISE(FEDException, "DataRow::getClass assumes that the second element is the data element.  You need at least two elements in the DataRow to call this method.");
				elements_[1]->setClass(className);
			}
		
			/** What it looks like **/
			std::string toHTML();
		
			/** Make a form for editing the present value.
			*
			*	@param target is the target URL of the form
			*	@param ddu is the index of the ddu that is being addressed
			*	@param val is the legacy index number for DDU/DCCTextLoad to parse.
			*	@param buttonText is the text you want to appear in the submit button.
			**/
			std::string makeForm(std::string target, unsigned int crate, unsigned int ddu, unsigned int val, std::string buttonText = "Load")
				throw (FEDException);
		
		private:
			std::vector< DataElement * > elements_;
			unsigned long int breaks_;
		};
		
		
		class DataTable
		{
		public:
			/** Standard constructor.
			*
			*	@param id is the HTML id tag of the table.  Should be unique.
			**/
			DataTable(std::string id);
		
			/** Standard destructor. **/
			~DataTable() {};
		
			/** Access a given row. **/
			DataRow *operator[] (unsigned int row);
		
			/** Access a given element. **/
			DataElement *operator() (unsigned int row, unsigned int col);
		
			void addColumn(std::string title);
		
			/** Put a border-defined break to the right of the last defined column. **/
			void addBreak()
				throw (FEDException);
		
			void addRow(DataRow *row);
			
			inline unsigned int countRows() { return rows_.size(); }
		
			std::string toHTML(bool tableTags = true);
		
			unsigned int countClass(std::string className);
		
			std::string printSummary();
		
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

	}
}


#endif
