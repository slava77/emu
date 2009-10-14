/*****************************************************************************\
* $Id: TableCell.h,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_TABLECELL_H__
#define __EMU_FED_TABLECELL_H__

#include <string>
#include <vector>
#include <sstream>

#include "emu/fed/CSSElement.h"

namespace emu {
	namespace fed {
		
		/** @class TableCell An individual cell of a Table object. **/
		class TableCell: public CSSElement, public std::ostringstream
		{
			public:
				
				/** Standard constructor.
				*
				* @param id the CSS ID of the table.
				* @param classes the CSS class of the table.
				**/
				TableCell(const std::string &id = "", const std::string &classes = "");
				
				/** Standard constructor.
				*
				* @param id the CSS ID of the table.
				* @param classes a vector of CSS classes of the table.
				**/
				TableCell(const std::string &id, const std::vector<std::string> &classes);
				
				/** Standard constructor.
				*
				* @param id the CSS ID of the table.
				* @param classes a vector of CSS classes of the table.
				**/
				TableCell(const std::vector<std::string> &classes, const std::string &id = "");
				
				/** Copy constructor. **/
				TableCell(const TableCell &myCell);
				TableCell(TableCell &myCell);
				
				/** Destructor. **/
				~TableCell();
				
				/** Assignment operator. **/
				TableCell &operator=(const TableCell &myCell);
				TableCell &operator=(TableCell &myCell);
				
				/** @returns a string representing the cell in an HTML table element \<td\>. **/
				virtual std::string toHTML();
				
				/** @returns a string representing the cell in formatted text. **/
				virtual std::string toText();
				
		};
		
	}
}


#endif
