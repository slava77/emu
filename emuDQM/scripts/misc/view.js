function view (layout,scale){
	var width  = new Object();
	var height = new Object();
	var nCols=0, nRows=0;

	var location = new Array(10);
	for(var i=0; i<10; i++) location[i] = new Array(10);
	
	for(var item in layout){
		var w_h = layout[item].replace(/\+.*$/g, "");
		var x_y = layout[item].replace(/^.*?\+/g, "");
		var w   = w_h.replace(/x.*$/g, "");
		var h   = w_h.replace(/^.*x/g, "");
		var x   = x_y.replace(/\+.*$/g, "");
		var y   = x_y.replace(/^.+\+/g, "");
		width [item] = w*scale;
		height[item] = h*scale;
		if( nCols < x ) nCols = x;
		if( nRows < y ) nRows = y;
		location[x][y] = item;
	}

	document.writeln("<table width=100% cellspacing=0 cellpadding=0 border=0>\n");
	for(var y=1; y<=nRows; y++){
		document.writeln("<tr>\n");
		for(var x=1; x<=nCols; x++){
                        var title = location[x][y].replace(/^.*?\//g, "").replace(/\/\w+.png$/g, ":");
			document.writeln("<td>"+title+"</td>\n");
		}
		document.writeln("</tr>\n");
		document.writeln("<tr>\n");
		for(var x=1; x<=nCols; x++){
			document.writeln("<td><a href="+location[x][y]+"><img alt='qwe' border=1 width="+width[location[x][y]]+" vspace=10 hspace=10 height="+height[location[x][y]]+" src="+location[x][y]+"></a></td>\n");
		}
		document.writeln("</tr>\n");
	}
	document.writeln("</table>\n");
}
