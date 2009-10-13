/*****************************************************************************\
* $Id: Table.h,v 1.1 2009/10/13 20:29:18 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_TABLE_H__
#define __EMU_FED_TABLE_H__

#include <string>
#include <vector>

#include "emu/fed/CSSElement.h"

namespace emu {
	namespace fed {
		
		class TableRow;
		class TableCell;
		typedef TableRow TableColumn;

		/** @class Table A class for building, maintaining, and easily displaying tables of data
		**/
		class Table: public CSSElement
		{
		public:
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes the CSS class of the table.
			**/
			Table(const std::string &id = "", const std::string &classes = "");
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			Table(const std::string &id, const std::vector<std::string> &classes);
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			Table(const std::vector<std::string> &classes, const std::string &id = "");
			
			/** Standard constructor.
			*
			* @param classes a vector of CSS classes of the table.
			**/
			Table(const std::vector<std::string> &classes);
			
			/** Copy constructor. **/
			Table(const Table &myTable);
			
			/** Destructor. **/
			~Table();
			
			/** Assignment operator. **/
			Table &operator=(const Table &myTable);
			
			/** Get the number of rows. **/
			inline unsigned int rowSize();
		
			/** Access a given TableRow.  Automatically resize the TableRow vector as needed.  **/
			TableRow *getRow(const unsigned int &row);
			
			/** Access a given TableRow.  Automatically resize the TableRow vector as needed.  **/
			TableRow *operator[](const unsigned int &row);
			
			// Note for the following:  in HTML, columns are not real structural elements, so I have to be tricky about their display.
			
			/** Get the number of columns. **/
			inline unsigned int columnSize();
			
			/** Access a given TableColumn.  Automatically resize the TableRows as needed.  **/
			TableColumn *getColumn(const unsigned int &col);
			
			/** Access a given TableColumn.  Automatically resize the TableRows as needed.  **/
			TableColumn *operator()(const unsigned int &col);
			
			/** Access a given TableCell.  Automatically resize the TableRows as needed.  **/
			TableCell *getCell(const unsigned int &row, const unsigned int &col);
			
			/** Access a given TableCell.  Automatically resize the TableRows as needed.  **/
			TableCell *operator()(const unsigned int &row, const unsigned int &col);
			
			/** Add a TableRow in a given position.
			*
			*	@param myRow the TableRow object to add.
			*	@param rowNumber the row number of the new TableRow.  All following rows will be moved down one.
			**/
			Table &insertRow(TableRow *myRow, const int &rowNumber = -1);
			Table &insertRow(TableRow &myRow, const int &rowNumber = -1);
			
			/** Replace a TableRow.
			*
			*	@param myRow the TableRow object to add.
			*	@param rowNumber the row number of the TableRow to replace.
			**/
			Table &replaceRow(TableRow *myRow, const unsigned int &rowNumber);
			Table &replaceRow(TableRow &myRow, const unsigned int &rowNumber);
			
			/** Add a TableColumn in a given position.
			*
			*	@param myColumn the TableColumn object to add.
			*	@param colNumber the column number of the new TableColumn.  All following columns will be moved right one.
			**/
			Table &insertColumn(TableColumn *myColumn, const int &colNumber = -1);
			Table &insertColumn(TableColumn &myColumn, const int &colNumber = -1);
			
			/** Replace a TableColumn.
			*
			*	@param myColumn the TableColumn object to add.
			*	@param colNumber the column number of the TableColumn to replace.
			**/
			Table &replaceColumn(TableColumn *myColumn, const unsigned int &colNumber);
			Table &replaceColumn(TableColumn &myColumn, const unsigned int &colNumber);
			
			/** Replace a TableCell. 
			*
			*	@param myCell the TableCell object to add.
			*	@param rowNumber the row number of the TableCell to replace.
			*	@param colNumber the column number of the TableCell to replace.
			**/
			Table &replaceCell(TableCell *myCell, const unsigned int &rowNumber, const unsigned int &colNumber);
			Table &replaceCell(TableCell &myCell, const unsigned int &rowNumber, const unsigned int &colNumber);
			
			/** @returns a string representing the row in an HTML table \<table\>. **/
			virtual std::string toHTML() const;
			
			/** @returns a string representing the row in formatted text. **/
			virtual std::string toText() const;
		
		private:
			
			std::vector<TableRow *> rowVector_;
			std::vector<std::string> columnIDs_;
			std::vector<std::vector<std::string> > columnClasses_;

		};

	}
}

#endif
