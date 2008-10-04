/*****************************************************************************\
* $Id: errorFlasher.js,v 1.1 2008/10/04 18:44:05 paste Exp $
*
* $Log: errorFlasher.js,v $
* Revision 1.1  2008/10/04 18:44:05  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
\*****************************************************************************/

// Flash anything with a class of "error" between red-on-black and black-on-red.

var red = 1;

function getElementsByClassName(classname) {
	var node = document.getElementsByTagName("body")[0];
	var a = [];
	var re = new RegExp("\\\\b" + classname + "\\\\b");
	var els = node.getElementsByTagName("*");
	for ( i=0,j=els.length; i<j; i++)
		if (re.test(els[i].className))
			a.push(els[i]);
	return a;
}

function setcolor() {
	var x = getElementsByClassName("error");
	var c1;
	var c2;
	if (red) {
		red = 0;
		c1 = "red";
		c2 = "black";
	} else {
		red = 1;
		c1 = "black";
		c2 = "red";
	}
	for ( i=0,j=x.length; i<j; i++) {
		x[i].style.backgroundColor = c1;
		x[i].style.color = c2;
	}
	
	setTimeout("setcolor();",1000);
}

setTimeout("setcolor();",1000);
