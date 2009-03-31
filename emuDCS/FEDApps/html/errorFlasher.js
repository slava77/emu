/*****************************************************************************\
* $Id: errorFlasher.js,v 1.2 2009/03/31 22:12:02 paste Exp $
*
* $Log: errorFlasher.js,v $
* Revision 1.2  2009/03/31 22:12:02  paste
* Version bump.
* Reduced demand on client-side browser by changing Monitor javascript functions.
* Added update countdown timer to Monitor.
* Made updates on changing of Monitor selection instantaneous.
*
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

// Flash anything with a class of "error" between red-on-black and black-on-red.
new PeriodicalExecuter(function(pe) {

	// Get all the elements with the class "error"
	var toBlack = $$(".error");
	var toBlackBorder = $$(".errorOutline");
	var toRed = $$(".error_black");
	var toRedBorder = $$(".error_blackOutline");
	toBlack.each(function(element) {
		element.removeClassName("error").addClassName("error_black");
	});
	toRed.each(function(element) {
		element.removeClassName("error_black").addClassName("error");
	});
	toBlackBorder.each(function(element) {
		element.removeClassName("errorOutline").addClassName("error_blackOutline");
	});
	toRed.each(function(element) {
		element.removeClassName("error_blackOutline").addClassName("errorOutline");
	});
}, 1);
