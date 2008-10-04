/*****************************************************************************\
* $Id: formChecker.js,v 1.1 2008/10/04 18:44:05 paste Exp $
*
* $Log: formChecker.js,v $
* Revision 1.1  2008/10/04 18:44:05  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
\*****************************************************************************/

// Check to see if there is any data in a given form element.  Turn it red and return
// false, otherwise return true.

function formCheck(id) {
	var element = document.getElementById(id);
	if (element.value == "") {
		element.style.backgroundColor = "#FFCCCC";
		return false;
	} else {
		return true;
	}
}
