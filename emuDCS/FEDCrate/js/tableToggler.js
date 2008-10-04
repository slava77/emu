/*****************************************************************************\
* $Id: tableToggler.js,v 1.1 2008/10/04 18:44:05 paste Exp $
*
* $Log: tableToggler.js,v $
* Revision 1.1  2008/10/04 18:44:05  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
\*****************************************************************************/

// Toggle an element between displayed and not

function toggle(id) {
	var elem = document.getElementById(id);
	elem.style.display = (elem.style.display != 'none' ? 'none' : '' );
}
