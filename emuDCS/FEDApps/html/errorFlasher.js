/*****************************************************************************\
* $Id: errorFlasher.js,v 1.3 2009/05/16 18:53:09 paste Exp $
\*****************************************************************************/

// Flash anything with a class of "error" between red-on-black and black-on-red.
new PeriodicalExecuter(function(pe) {
	// Get all the elements with the class "error"
	var toBlack = $$(".error");
	var toRed = $$(".error_black");
	toBlack.each(function(element) {
		element.removeClassName("error").addClassName("error_black");
	});
	toRed.each(function(element) {
		element.removeClassName("error_black").addClassName("error");
	});
}, 1);
