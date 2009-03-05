/*****************************************************************************\
* $Id: bitFlipper.js,v 1.1 2009/03/05 16:18:23 paste Exp $
*
* $Log: bitFlipper.js,v $
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

// Flip a bit from a form element that contains a number, then display in hex.

function toggleBit(id,bit) {
	var elem = document.getElementById(id);
	var oldValue = parseInt(elem.value);
	var newValue = (oldValue ^ (1 << bit));
	elem.value = "0x" + newValue.toString(16);
}

// Set a bit in a form element that contains a number, then display in hex.
function setBit(id,bit) {
	var elem = document.getElementById(id);
	var oldValue = parseInt(elem.value);
	var newValue = (oldValue | (1 << bit));
	elem.value = "0x" + newValue.toString(16);
}

// Clear a bit in a form element that contains a number, then display in hex
function clearBit(id,bit) {
	var elem = document.getElementById(id);
	var oldValue = parseInt(elem.value);
	var newValue = (oldValue & ~(1 << bit));
	elem.value = "0x" + newValue.toString(16);
}
