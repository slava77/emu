/*****************************************************************************\
* $Id: DataTable.h,v 1.10 2008/10/13 11:56:40 paste Exp $
*
* $Log: DataTable.h,v $
* Revision 1.10  2008/10/13 11:56:40  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 1.9  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
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

		/** @class DataElement An individual element of a DataRow object.  Contains the actual value
		*	to be displayed (as a stringstream) and the CSC class name of the element
		*	for formatting purposes.
		**/
		class DataElement: public std::stringstream
		{
			friend class DataRow;
			
		public:
			
			/** Standard constructor.
			*
			*	@param myValue is the value of the pacticular element.
			*	@param className is the class of the particular element.
			**/
			DataElement(std::stringstream myValue, std::string className = "none");
		
			/** Standard constructor with std::strings.
			*
			*	@param myValue is the value of the pacticular element.
			*	@param className is the class of the particular element.
			**/
			DataElement(std::string myValue = "", std::string klass = "none");

			/** Copy constructor, for the stupid stringstream **/
			DataElement(const DataElement &myElement);

			DataElement &operator= (const DataElement &myElement);
		
			~DataElement() {}
		
			/** Set the CSS class. **/
			inline void setClass(std::string className) { class_ = className; }
		
			/** @returns CSS class. **/
			inline std::string getClass() { return class_; }
		
			/** Display as an HTML table element \<td\>. **/
			std::string toHTML(bool breaks = false);
		
		private:
			std::string class_;
		};
		
	
		/** @class DataRow An individual row of a DataTable object.  These contain DataElements. **/
		class DataRow: public std::vector<DataElement>
		{
			friend class DataTable;
		public:
			/** Standard constructor.
			*
			*	@param myName is the name of the row, or the "title" of the element of data to be displayed.
			*	@param cols is the number of columns in the row to begin with.  Can be dynamically updated later.
			*	@param breaks is a set of bits with each high bit corresponding to placing a break in the table after that column.
			**/
			DataRow(std::stringstream myName, unsigned int cols = 3, unsigned long int breaks = 0)
				throw (FEDException);
		
			/** Standard constructor with std::strings. **/
			DataRow(std::string myName, unsigned int cols = 3, unsigned long int breaks = 0)
				throw (FEDException);
		
			~DataRow() {}
		
			/** Access a given element.  Expand the vector as needed.  **/
			DataElement &operator[] (unsigned int element)
				throw (FEDException);
		
			/** Set which elements to draw a line after (using borders). 
			*
			*	@param breaks is a set of bits with each high bit corresponding to placing a break in the table after that column.
			**/
			inline void setBreaks(unsigned long int breaks) { breaks_ = breaks; }
		
			/** Set the CSS class name of the \e second element.
			*	The second element is sometimes used as the data value, so it might be
			*	important to have an easy way to set its class.
			**/
			inline void setClass(std::string className)
				throw (FEDException)
			{
				if (size() < 2) XCEPT_RAISE(FEDException, "DataRow::getClass assumes that the second element is the data element.  You need at least two elements in the DataRow to call this method.");
				(*this)[1].setClass(className);
			}
		
			/** @returns a string representing the row in and HTML table row \<tr\>.
			
				@sa @class DataElement method toHTML.
			**/
			std::string toHTML();
		
			/** Make a form for editing the present value.
			*
			*	@param target is the target URL of the form
			*	@param crate is the target crate that is being addressed
			*	@param ddu is the index of the ddu that is being addressed
			*	@param val is the legacy index number for DDU/DCCTextLoad to parse.
			*	@param buttonText is the text you want to appear in the submit button.
			**/
			std::string makeForm(std::string target, unsigned int crate, unsigned int ddu, unsigned int val, std::string buttonText = "Load")
				throw (FEDException);
		
		private:
			//std::vector< DataElement > elements_;
			unsigned long int breaks_;
		};
		
		/** @class DataTable A class for building, maintaining, and easily displaying tables of data
		*	used in the EmuFCrateHyperDAQ application.
		*
		*	@sa @class EmuFCrateHyperDAQ
		**/
		class DataTable: public std::vector<DataRow>
		{
		public:
			/** Standard constructor.
			*
			*	@param id the HTML id tag of the table.  Should be unique.
			**/
			DataTable(std::string id);
		
			~DataTable() {};
		
			/** Access a given DataRow.  Increase the DataRow vector as needed.  **/
			DataRow &operator[] (unsigned int row);
		
			/** Access a given DataElement.  Will automatically expand DataRows with
			*	new DataElements and expand with more DataRows as needed.
			*
			*	@param row the DataRow to access.
			*	@param col the DataElement in that row to access.
			**/
			DataElement &operator() (unsigned int row, unsigned int col);
		
			/** Add a column to the table.  Will expand the DataRows as needed.
			*
			*	@param title the title of the column to appear at the top of the HTML version of the table.
			**/
			void addColumn(std::string title);
		
			/** Put a border-defined break to the right of the last defined column. **/
			void addBreak()
				throw (FEDException);
		
			/** Add a pre-constructed DataRow to the table. **/
			void addRow(DataRow row);
			
			/** @return the number of DaraRows in the table. **/
			inline unsigned int countRows() { return size(); }
		
			/** @return the table formatted as an HTML table \<table\>.
			*
			*	@param tableTags if true will display the table with the opening and closing \<table\> tags.  Useful if you want to combine tables under the same set of tags.
			**/
			std::string toHTML(bool tableTags = true);
		
			/** @return the number of times a particular class name appears in the classes
			*	of the DataElements.
			*
			*	@param className the CSS class name for which to search.
			**/
			unsigned int countClass(std::string className);
		
			/** @returns a string summarizing the classes of the table in the format
			*	"#withClassName1/#Elements ClassName1 #withClassName2/#Elements ClassName2..."
			*	Will automatically skip class names that are empty or "none", and wraps each
			*	or the class summaries in an HTML \<span\> tag with the same class as
			*	that which is being described.
			**/
			std::string printSummary();
		
			/** @returns whether or not the table is set to not display (via the HTML
			*	parameter style="display: none;".
			**/
			inline bool isHidden() { return hidden_; }
			
			/** Sets whether or not the table is set to not display (via the HTML
			*	parameter style="display: none;".
			**/
			inline void setHidden(bool hidden) { hidden_ = hidden; }
		
		private:
			//std::vector< DataRow > rows_;
			std::vector< std::string > headers_;
			unsigned int cols_;
			std::string id_;
			bool hidden_;
			unsigned long int breaks_;
		};

	}
}


#endif
