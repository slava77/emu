/*****************************************************************************\
* $Id: formChecker.js,v 1.1 2009/03/05 16:18:23 paste Exp $
*
* $Log: formChecker.js,v $
* Revision 1.1  2009/03/05 16:18:23  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
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
