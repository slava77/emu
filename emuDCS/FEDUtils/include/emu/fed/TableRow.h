/*****************************************************************************\
* $Id: TableRow.h,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_TABLEROW_H__
#define __EMU_FED_TABLEROW_H__

#include <string>
#include <vector>

#include "emu/fed/CSSElement.h"

namespace emu {
	namespace fed {
	
		class TableCell;
		
		/** @class TableRow An individual row of a Table object.  These contain TableCells. **/
		class TableRow: public CSSElement
		{
		public:
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes the CSS class of the table.
			**/
			TableRow(const std::string &id = "", const std::string &classes = "");
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			TableRow(const std::string &id, const std::vector<std::string> &classes);
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			TableRow(const std::vector<std::string> &classes, const std::string &id = "");
			
			/** Copy constructor. **/
			TableRow(const TableRow &myRow);
			TableRow(TableRow &myRow);
			
			/** Destructor. **/
			~TableRow();
			
			/** Assignment operator. **/
			TableRow &operator=(const TableRow &myRow);
			TableRow &operator=(TableRow &myRow);
			
			/** Count the number of cells in the row. **/
			unsigned int size();
			
			/** Access a given element.  Expand the vector as needed.  **/
			TableCell *getCell(const unsigned int &col);

			/** Access a given element.  Expand the vector as needed.  **/
			TableCell *operator[](const unsigned int &col);
			
			/** Access all cells. **/
			std::vector<TableCell *> getCells();
			
			/** Insert a TableCell. 
			*
			*	@param myCell the TableCell object to add.
			*	@param colNumber the column number of the TableCell to add.  All following elements will be pushed right.
			**/
			TableRow &insertCell(TableCell *myCell, const int &colNumber = -1);
			TableRow &insertCell(TableCell &myCell, const int &colNumber = -1);
			
			/** Replace a TableCell. 
			*
			*	@param myCell the TableCell object to add.
			*	@param colNumber the column number of the TableCell to replace.
			**/
			TableRow &replaceCell(TableCell *myCell, const unsigned int &colNumber);
			TableRow &replaceCell(TableCell &myCell, const unsigned int &colNumber);
			
			/** Clear the cells **/
			TableRow &clear();
		
			/** @returns a string representing the row in an HTML table row \<tr\>. **/
			virtual std::string toHTML();
			
			/** @returns a string representing the row in formatted text. **/
			virtual std::string toText();
		
		private:
			std::vector<TableCell *> cellVector_;
			
		};

	}
}


#endif
