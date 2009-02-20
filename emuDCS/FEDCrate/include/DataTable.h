/*****************************************************************************\
* $Id: DataTable.h,v 1.13 2009/02/20 15:25:46 paste Exp $
*
* $Log: DataTable.h,v $
* Revision 1.13  2009/02/20 15:25:46  paste
* Fixed include path problem with DataTable.
*
* Revision 1.12  2009/02/10 21:08:55  paste
* Fixed a memory leak in EmuFCrateHyperDAQ's use of the DataTable class
*
* Revision 1.11  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
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
#ifndef __EMU_FED_DATATABLE_H__
#define __EMU_FED_DATATABLE_H__

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "cgicc/HTMLClasses.h"

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
			**/
			DataElement(std::stringstream myValue, std::string myClass = "");
		
			/** Standard constructor with std::strings.
			*
			*	@param myValue is the value of the pacticular element.
			*	@param className is the class of the particular element.
			**/
			DataElement(std::string myValue = "", std::string myClass = "");

			/** Copy constructor, for the stupid stringstream **/
			DataElement(const DataElement &myElement);

			/** Standard defining operator **/
			DataElement &operator= (const DataElement &myElement);

			/** Arrow operator for accessing the cgicc element stored within **/
			inline cgicc::td *operator-> () { return &element_; }

			/** Set the class of the HTML \<td\> element.  This is required because the HTMLAttributeList
			*	class does not allow for reading back of already set attributes (a SERIOUSLY short-sighted
			*	maneuver if you ask me).
			*
			*	@param myClass is new class name of the element.
			*	@returns itself so that this can be chained with other method calls.
			**/
			DataElement &setClass(const std::string &myClass);

			/** Get the stored class of the element **/
			inline std::string getClass() { return class_; }
		
			/** Display as an HTML table element \<td\>. **/
			std::string toHTML();
		
		private:
		
			cgicc::td element_;
			std::string class_;
			
		};
		
	
		/** @class DataRow An individual row of a DataTable object.  These contain DataElements. **/
		class DataRow: public std::vector<DataElement *>
		{
			friend class DataTable;
			
		public:
			/** Standard constructor.
			*
			*	@param cols is the number of columns in the row to begin with.
			*	@param myElement is the default DataElement object that will be used as a template for the elements of the row.
			**/
			DataRow(unsigned int cols = 0, DataElement *myElement = new DataElement());
		
			/** Access a given element.  Expand the vector as needed.  **/
			DataElement &operator[] (unsigned int element);

			/** Arrow operator for accessing the cgicc element stored within **/
			inline cgicc::tr *operator-> () { return &element_; }
		
			/** @returns a string representing the row in and HTML table row \<tr\>.
			*	@sa DataElement method toHTML.
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
			//std::string makeForm(std::string target, unsigned int crate, unsigned int ddu, unsigned int val, std::string buttonText = "Load");
		
		private:
			//std::vector< DataElement > elements_;
			//unsigned long int breaks_;
			cgicc::tr element_;
		};
		
		/** @class DataTable A class for building, maintaining, and easily displaying tables of data
		*	used in the EmuFCrateHyperDAQ application.
		*
		*	@sa EmuFCrateHyperDAQ
		**/
		class DataTable: public std::vector<DataRow *>
		{
		public:
			/** Standard constructor.
			*
			*	@param row is the number of rows the table will begin with.
			*	@param myRow is the default DataRow object that will be used as a template for the rows of the table.
			**/
			DataTable(unsigned int rows = 0, DataRow *myRow = new DataRow());
		
			/** Access a given DataRow.  Increase the DataRow vector as needed.  **/
			DataRow &operator[] (unsigned int row);
		
			/** Access a given DataElement.  Will automatically expand DataRows with
			*	new DataElements and expand with more DataRows as needed.
			*
			*	@param row the DataRow to access.
			*	@param col the DataElement in that row to access.
			**/
			DataElement &operator() (unsigned int row, unsigned int col);

			/** Arrow operator for accessing the cgicc element stored within **/
			inline cgicc::table *operator->() { return &element_; }
		
			/** @return the table formatted as an HTML table \<table\>.
			*
			*	@param tableTags if true will display the table with the opening and closing \<table\> tags.  Useful if you want to combine tables under the same set of tags.
			**/
			std::string toHTML();
		
			/** @return a map of the the number of times a particular class name appears in the classes
			*	of the DataElements.
			*
			*	@param className the CSS class name for which to search.
			*	@returns a class-name/multiplicity map
			**/
			std::map<std::string, unsigned int> countClasses();
		
			/** @returns a string summarizing the classes of the table in the format
			*	"#withClassName1/#Elements ClassName1 #withClassName2/#Elements ClassName2..."
			*	Will automatically skip class names that are empty or "none", and wraps each
			*	or the class summaries in an HTML \<span\> tag with the same class as
			*	that which is being described.
			**/
			std::string printSummary();
		
		private:
			cgicc::table element_;

		};

	}
}


#endif
