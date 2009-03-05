/*****************************************************************************\
* $Id: reload.js,v 1.1 2009/03/05 16:18:23 paste Exp $
*
* $Log: reload.js,v $
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

// Set a 10-second reload of the current page (after removing all the GET commands
// in the URL)

setTimeout("window.location.href=window.location.href.replace(/[\\?].*$/,\"\");", 10000);
