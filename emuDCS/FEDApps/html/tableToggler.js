/*****************************************************************************\
* $Id: tableToggler.js,v 1.1 2009/03/05 16:18:23 paste Exp $
*
* $Log: tableToggler.js,v $
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

// Toggle an element between displayed and not

function toggle(id) {
	var elem = document.getElementById(id);
	elem.style.display = (elem.style.display != 'none' ? 'none' : '' );
}
