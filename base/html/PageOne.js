var panels = null;
var tmbPanel = null;


function onLoad() {

    var URLs = {
	PCMan:    "http://csc-sv.cms:20010/urn:xdaq-application:lid=60/ForEmuPage1",
	BlueP:    "http://csc-dcs-pc1.cms:20070/urn:xdaq-application:lid=70/ForEmuPage1",
	BlueM:    "http://csc-dcs-pc2.cms:20070/urn:xdaq-application:lid=70/ForEmuPage1",
	PCMonP:   "http://csc-dcs-pc1.cms:20040/urn:xdaq-application:lid=30/ForEmuPage1",
	PCMonM:   "http://csc-dcs-pc2.cms:20040/urn:xdaq-application:lid=30/ForEmuPage1",
	DAQDisks: "http://srv-c2d04-25.cms:8845/urn:xdaq-application:lid=400/retrieveCollection",
	CSCTF:    "http://l1ts-csctf.cms:5005/urn:xdaq-application:service=las/retrieveCollection",
	TTC:      "http://srv-c2d04-27.cms:9945/urn:xdaq-application:lid=400/retrieveCollection",
	FED:      "http://csc-sv.cms:20101/urn:xdaq-application:lid=66/ForEmuPage1",
	DAQ:      "http://csc-daq00.cms:20200/urn:xdaq-application:class=emu::daq::manager::Application,instance=0/ForEmuPage1",
	DQM:      "http://csc-dqm.cms:20550/urn:xdaq-application:lid=1450/ForEmuPage1"
    };
    var tmbURLs = {
	minus:'http://csc-dcs-pc2.cms:20040/urn:xdaq-application:lid=30/XmlOutput', 
	plus: 'http://csc-dcs-pc1.cms:20040/urn:xdaq-application:lid=30/XmlOutput'
    };

    var divs = document.getElementsByTagName('div');
    panels = new Array();
    var period = 10000;
    for ( var i=0; i<divs.length; i++ ){
	if ( divs[i].id == 'TMB' ){
	    new XSLTLoader('TMB_XSL.xml','RUI-to-chamber_mapping.xml',
			   function(){
			       $('#TMB').append( this.result );
			       tmbPanel = new TMBPanel( tmbURLs );
			   }
			  );
	}
	else{
	    panels.push( new Panel( divs[i].id, period, URLs[divs[i].id] ) );
	}
    }

    document.onmousemove = mouseMove;
    document.onmouseup = onMouseUp;
    
}

function mouseMove(ev){
    ev.preventDefault();
    //alert( panels.length );
    for ( var i=0; i<panels.length; i++ ){
	panels[i].mouseMove(ev);
    }
}

function onMouseUp(e){
    e.preventDefault();
    //alert( panels.length );
    for ( var i=0; i<panels.length; i++ ){
	panels[i].onMouseUp(e);
    }
}

//
// Utility methods
//

function formatNumber( number ){
    var n = Number( number );
    var a = Math.abs( n );
    return n.toFixed( ( a<1 ? 3 : ( a<10 ? 2 : ( a<100 ? 1 : 0 ) ) ) );
}


function printTransforms( transformList ){
    var printedList = '';
    for ( var i=0; i<transformList.numberOfItems; i++ ){
	printedList = printedList + printSvgMatrix( transformList.getItem(i).matrix ) + ' ';
    }
    return printedList;
}

function printSvgMatrix( matrix ){
    if ( matrix != null ){
	return printedList =  
	    '(' + matrix.a +
	    ' ' + matrix.b +
	    ' ' + matrix.c +
	    ' ' + matrix.d +
	    ' ' + matrix.e +
	    ' ' + matrix.f +
	    ')';
    }
    return 'NO MATRIX';
}

function toUnixTime( dateTime ){
    var d = new Date();
    if ( dateTime.length == 19 ){
	 // YYYY-MM-DD hh:mm:ss
	 d.setFullYear( Number( dateTime.substr( 0,4) )     );
	 d.setMonth   ( Number( dateTime.substr( 5,2) ) - 1 );
	 d.setDate    ( Number( dateTime.substr( 8,2) )     );
	 d.setHours   ( Number( dateTime.substr(11,2) )     );
	 d.setMinutes ( Number( dateTime.substr(14,2) )     );
	 d.setSeconds ( Number( dateTime.substr(17,2) )     );
    }
    else {
	 var months = { 'Jan':1, 'Feb':2, 'Mar':3, 'Apr':4, 'May':5, 'Jun':6, 'Jul':7, 'Aug':8, 'Sep':9, 'Oct':10, 'Nov':11, 'Dec':12 }
	 // Wed, Jun 10 2009 21:47:35 GMT
	 d.setUTCFullYear( Number( dateTime.substr(12,4) )     );
	 d.setUTCMonth   ( months[ dateTime.substr( 5,3) ] - 1 );
	 d.setUTCDate    ( Number( dateTime.substr( 9,2) )     );
	 d.setUTCHours   ( Number( dateTime.substr(17,2) )     );
	 d.setUTCMinutes ( Number( dateTime.substr(20,2) )     );
	 d.setUTCSeconds ( Number( dateTime.substr(23,2) )     );
    }
    return d.getTime();
}

function zeroPadTo2( number ){
     return ( number < 10 ? '0' : '' )+number.toString();
}

function timeToString( time ){
     var d = new Date();
     d.setTime( time );
     return    zeroPadTo2( d.getFullYear() )
	  +'-'+zeroPadTo2( d.getMonth()+1  )
	  +'-'+zeroPadTo2( d.getDate()     )
	  +' '+zeroPadTo2( d.getHours()    )
	  +':'+zeroPadTo2( d.getMinutes()  )
	  +':'+zeroPadTo2( d.getSeconds()  );
}


//
// XSLTLoader object
//

function XSLTLoader( xslURL, xmlURL, onCompletion ){
    // Load the XSL asynchronously. When that's loaded, load the XML asynchronously. 
    // When that, too, is loaded, perform the transformation and call onCompletion.

    var self = this;
    this.xslURL = xslURL;
    this.xmlURL = xmlURL;
    this.onCompletion = onCompletion; // user's callback
    this.xsltProcessor = new XSLTProcessor();
    this.result = null;

    this.xslRequestStateChanged = function(e){
	if ( this.readyState == 4 ){
	    self.xsltProcessor.importStylesheet( this.responseXML );
	    self.loadXML();
	}
    };
    
    this.loadXSL = function(){
	var request = new XMLHttpRequest();
	request.open("GET", this.xslURL, true);
	request.onreadystatechange=this.xslRequestStateChanged
	request.send(null);
    };
    
    this.xmlRequestStateChanged = function(e){
	if ( this.readyState == 4 ){
	    var xml = this.responseXML;
	    self.result = self.xsltProcessor.transformToFragment(xml, document);
	    // console.log( (new XMLSerializer()).serializeToString( self.result ) );
	    self.onCompletion(); // invoke user's callback
	}
    };
    
    this.loadXML = function(){
	var request = new XMLHttpRequest();
	request.open("GET", xmlURL, true);
	request.onreadystatechange=this.xmlRequestStateChanged
	request.send(null);
    };

    this.loadXSL();
}

//
// Disk object
//

function Disk( host, mount, time, state, usage, free ){
  this.host  = host ;
  this.mount = mount;
  this.time  = time ;
  this.state = state;
  this.usage = usage; // [%]
  this.free  = free ; // [MB]
}

//
// Panel object
//

function Panel( name, refreshPeriod, dataURL ) {
    var self = this; // This will be needed in member methods that work as callback functions.

    //console.log( name+' '+refreshPeriod+' '+dataURL );

    this.name = name;
    this.state = '';
    this.pageRefreshPeriod = refreshPeriod; // ms
    this.svg_element = null;
    this.g_element = null;
    this.title_element = null;
    this.value_element = null;
    this.graph_element = document.getElementById(this.name+"-graph");
    this.pad_element = null;
    this.age_element = null;
    this.coords_element = null;
    this.follow_element = null;
    this.zoom_element = null;
    this.progressTable_fragment = document.createDocumentFragment(); // To cache progress table in non-calib runs.
    this.stopTime_fragment = document.createDocumentFragment(); // To cache stop time on calib runs.
    this.dragging = false;
    this.dragStart = { x:0, y:0 };
    this.cursorOverGraph = false;
    this.following   = true;
    this.autoZooming = true;
    this.oldTransform = null;
    this.trends = new Array();
    
    this.xAxis = null;
    this.yAxis = null;
    
    this.XmlDoc    = null;
    this.Clock     = null;
    this.Subsystem = null;
    this.DataURL   = dataURL;
    
    if ( this.graph_element ){
	this.svg_element = document.getElementById(this.name+"-svgRoot");
	this.g_element = document.getElementById(this.name+"-transformer");
	if ( !this.g_element ) g_element = document.getElementById(this.name+"-scaler");
	this.title_element = document.getElementById(this.name+"-title");
	this.value_element = document.getElementById(this.name+"-lastValue");
	this.pad_element = document.getElementById(this.name+"-pad");
	this.age_element = document.getElementById(this.name+"-ageOfGraph");
	this.follow_element = document.getElementById(this.name+"-follow");
	this.zoom_element = document.getElementById(this.name+"-zoom");
    }
	
    //
    // Member methods
    //

    this.mouseMove = function (ev){
	if ( this.cursorOverGraph ) this.coords_element.firstChild.nodeValue = '('+this.xAxis.pixelToValue(ev.pageX,true)+', '+this.yAxis.pixelToValue(ev.pageY,true)+')';
	if ( this.dragging ){
	    var newTransform = " translate(" + (ev.pageX-this.dragStart.x)*this.xAxis.svgPerPixel + " " + (ev.pageY-this.dragStart.y)*this.yAxis.svgPerPixel + ") ";
	    if ( this.oldTransform ) newTransform = this.oldTransform + newTransform;
	    this.graph_element.setAttribute("transform", newTransform );
	    try{
		this.redrawAxes();
	    } catch(ex) {
		//alert( ex );
	    }
	    // var msg = 'mousemove: ';
	    // msg += ' readout: ' + this.graph_element.getAttribute("transform") + ' or ' + printTransforms( this.graph_element.transform.baseVal ) +'   ' + printTransforms( this.graph_element.transform.animVal );
	    // msg += this.xAxis.print() + '   ' + this.yAxis.print();
	    // if (this.span2_element) this.span2_element.innerHTML = msg; 
	}
    };
    
    this.onMouseUp = function (e){
	if ( document.body ) document.body.style.cursor = 'default';
	this.dragging = false;
	//var msg = 'mouseup: ';
	if ( this.graph_element ){
	    this.graph_element.transform.baseVal.consolidate();
	}
	//msg += ' readout: ' + this.graph_element.getAttribute("transform") + ' or ' + printTransforms( this.graph_element.transform.baseVal ) +'   ' + printTransforms( this.graph_element.transform.animVal );
	// if (this.span2_element) this.span2_element.innerHTML = this.xAxis.print() + '   ' + this.yAxis.print(); 
    };
    
    this.attachListeners = function (){
    	this.coords_element = document.getElementById(this.name+"-pointerCoords");
    	this.pad_element = document.getElementById(this.name+"-pad");
    	this.pad_element.onmousedown = this.onMouseDownOnPad;
    	this.graph_element.onmouseover = this.onMouseOverGraph;
    	this.graph_element.onmouseout = this.onMouseOffGraph;
    	this.pad_element.addEventListener('DOMMouseScroll', this.onMouseWheelOnPad, false);
    	this.follow_element.onmousedown = this.onMouseDownOnFollowButton;
    	this.zoom_element.onmousedown = this.onMouseDownOnZoomButton;
    };

    this.onMouseDownOnPad = function (e){
	//alert(self.graph_element + ' ' + self.dragStart.x );
	self.dragStart = { x:e.pageX, y:e.pageY };
	if ( document.body ) document.body.style.cursor = 'move';
	self.dragging = true;
	self.graph_element.transform.baseVal.consolidate();
	self.oldTransform = self.graph_element.getAttribute( "transform" );
	// self.printSvgSvg();
    };

    this.onMouseDownOnFollowButton = function (){
	//alert( self.following );
	var follow_rect  = document.getElementById(self.name+'-follow_rect');
	var follow_text  = document.getElementById(self.name+'-follow_text');
	var follow_edge1 = document.getElementById(self.name+'-follow_edge1');
	var follow_edge2 = document.getElementById(self.name+'-follow_edge2');
	if ( !self.following ){
	    // Start following
	    follow_rect.setAttribute( 'fill', '#333333' );
	    follow_edge1.setAttribute( 'fill', '#111111' );
	    follow_edge2.setAttribute( 'fill', '#888888' );
	    follow_text.setAttribute( 'fill', '#eeeeee' );
	    follow_text.setAttribute( 'title', 'click to stop following last point' );
	    follow_rect.setAttribute( 'title', 'click to stop following last point' );
	    follow_text.firstChild.nodeValue = 'freeze'
	    self.following = true;
	} else {
	    // Stop following
	    follow_rect.setAttribute( 'fill', '#ffffaa' );
	    follow_edge1.setAttribute( 'fill', '#996644' );
	    follow_edge2.setAttribute( 'fill', '#ffffee' );
	    follow_text.setAttribute( 'fill', '#ffaa55' );
	    follow_text.setAttribute( 'title', 'click to keep last point in view' );
	    follow_rect.setAttribute( 'title', 'click to keep last point in view' );
	    follow_text.firstChild.nodeValue = 'follow'
	    self.following = false;
	}
    };

    this.onMouseDownOnZoomButton = function (){
	//alert( self.autoZooming );
	var zoom_rect = document.getElementById(self.name+'-zoom_rect');
	var zoom_text = document.getElementById(self.name+'-zoom_text');
	if ( self.autoZooming ){
	    // Disable automatic zoom
	    zoom_text.setAttribute( 'title', 'click to let vertical range adjust automatically' );
	    zoom_rect.setAttribute( 'title', 'click to let vertical range adjust automatically' );
	    zoom_text.firstChild.nodeValue = 'auto range'
	    self.autoZooming = false;
	} else {
	    // Enable automatic zoom
	    zoom_text.setAttribute( 'title', 'click to fix vertical range' );
	    zoom_rect.setAttribute( 'title', 'click to fix vertical range' );
	    zoom_text.firstChild.nodeValue = 'fix range'
	    self.autoZooming = true;
	}
    };
    
    this.onMouseOffGraph = function (e){
	if ( document.body ) document.body.style.cursor = 'default';
	self.coords_element.style.visibility = 'hidden';
	self.cursorOverGraph = false;
    };
    
    this.onMouseOverGraph = function (e){
	if ( document.body ) document.body.style.cursor = 'crosshair';
	self.coords_element.style.visibility = 'visible';
	self.cursorOverGraph = true;
    };

    this.onMouseWheelOnPad = function (event){
	var newTransform;
	var delta = 0;
	// Delta is multiple of +-3.
	// Or +-1 if Alt is pressed.
	delta = event.detail;
	if (delta){
	    switch(delta){
	    case -1:
		// Scale up in X
		if ( document.body ) document.body.style.cursor = 'w-resize';
 		self.scale( 1.25, 1. );
		break;
	    case 1:
		// Scale down in X
		if ( document.body ) document.body.style.cursor = 'e-resize';
 		self.scale( 0.8, 1. );
		break;
	    case -3:
		// Scale up in Y
		if ( document.body ) document.body.style.cursor = 'n-resize';
		self.scale( 1., 1.25 );
		break;    
	    case 3:
		// Scale down in Y
		if ( document.body ) document.body.style.cursor = 's-resize';
		self.scale( 1., 0.8 );
		break;
	    default:
		if ( document.body ) document.body.style.cursor = 'default';
	    }
	}
	// Prevent default actions caused by mouse wheel.
	event.preventDefault();
	event.returnValue = false;
    };

    this.translate = function ( xDistSVG, yDistSVG ){
	var newTransformation = ' translate('+xDistSVG+','+yDistSVG+') ';
	var oldTransformation = this.graph_element.getAttribute("transform");
	if ( oldTransformation ) newTransformation = oldTransformation + newTransformation;
	this.graph_element.setAttribute("transform", newTransformation );
	this.redrawAxes();
    };
    
    this.scale = function ( xFactor, yFactor ){
	var xToOrigin = 0;
	var yToOrigin = 0;
	if ( this.graph_element.transform.animVal.numberOfItems ){
	    this.graph_element.transform.baseVal.consolidate();
	    try{ 
		var matrix = this.graph_element.transform.animVal.getItem(0).matrix;
		xToOrigin = matrix.e/matrix.a;
		yToOrigin = matrix.f/matrix.d;
	    } catch(ex){}// { alert(ex); }
	}
	// Go to the origin, scale, and get back
	var newTransformation = ' translate('+(-xToOrigin)+','+(-yToOrigin)+') scale( '+xFactor+','+yFactor+' ) translate('+xToOrigin+','+yToOrigin+') ';
	var oldTransformation = this.graph_element.getAttribute("transform");
	if ( oldTransformation ) newTransformation = oldTransformation + newTransformation;
	this.graph_element.setAttribute("transform", newTransformation );
	this.redrawAxes();    
    };
    
    this.relabelAxes = function (){
	
	var labelElements = document.getElementById(this.name+"-xAxis").getElementsByTagNameNS('http://www.w3.org/2000/svg','text');
	for ( var i=0; i<labelElements.length / 2 ; i++ ){
	    labelElements[i                           ].firstChild.nodeValue = this.xAxis.labels[i].stringValue1;
	    labelElements[i + labelElements.length / 2].firstChild.nodeValue = this.xAxis.labels[i].stringValue2;
	}
	
	labelElements = document.getElementById(this.name+"-yAxis").getElementsByTagNameNS('http://www.w3.org/2000/svg','text');
	for ( var i=0; i<labelElements.length; i++ ) labelElements[i].firstChild.nodeValue = this.yAxis.labels[i].stringValue1;
	
    };
    
    this.redrawAxes = function (){
	if ( this.graph_element ){
	    this.graph_element.transform.baseVal.consolidate();
	    if ( this.graph_element.transform.animVal.numberOfItems ){
		var matrix = this.graph_element.transform.animVal.getItem(0).matrix
		this.xAxis.transform( matrix );
		this.yAxis.transform( matrix );
	    } else {
		this.xAxis.transform( null ); // just for updating everything (will do identity transformation)
		this.yAxis.transform( null ); // just for updating everything (will do identity transformation)
	    }
	    this.relabelAxes();
	}
    };
    
    this.printSvgSvg = function (){
	var msg = 
	    '<table>' +
	    '  <tr><td>currentScale </td><td>' + this.svg_element.currentScale  + '</td></tr>' +
	    // 	'  <tr><td>pixelUnitToMillimeterX </td><td>' + this.svg_element.pixelUnitToMillimeterX  + '</td></tr>' +
	    // 	'  <tr><td>pixelUnitToMillimeterY </td><td>' + this.svg_element.pixelUnitToMillimeterY  + '</td></tr>' +
	    // 	'  <tr><td>screenPixelToMillimeterX </td><td>' + this.svg_element.screenPixelToMillimeterX  + '</td></tr>' +
	    // 	'  <tr><td>screenPixelToMillimeterY </td><td>' + this.svg_element.screenPixelToMillimeterY  + '</td></tr>' +
	    '  <tr><td>width </td><td>' + this.svg_element.width.animVal.value  + '</td></tr>' +
	    '  <tr><td>height </td><td>' + this.svg_element.height.animVal.value  + '</td></tr>' +
	    '  <tr><td>CTM </td><td>' + printSvgMatrix( this.svg_element.getCTM() ) + '</td></tr>' +
	    '  <tr><td>ScreenCTM </td><td>' + printSvgMatrix( this.svg_element.getScreenCTM() ) + '</td></tr>' +
	    '</table>';
	if (this.span3_element) this.span3_element.innerHTML = msg;
    };

    this.followLastPoint = function ( xSVGLastPoint ){
	if ( xSVGLastPoint > this.xAxis.hiSVG ) this.translate( this.xAxis.hiSVG-xSVGLastPoint, 0 );
    };

    this.centerXAxisOnValue = function ( xSVG ){
	this.translate( 0.5*(this.xAxis.loSVG+this.xAxis.hiSVG) - xSVG, 0 );
    };
    
    this.transformYToFit = function (){
	// Zoom and pan in y so that each point in range can be seen.
	var minYSVG =  0.1*Number.MAX_VALUE;
	var maxYSVG = -0.1*Number.MAX_VALUE;
	var noPointInRange = true;
	for ( var i=0; i<this.graph_element.points.numberOfItems; i++ ){
	    if ( this.xAxis.loSVG <= this.graph_element.points.getItem(i).x && this.graph_element.points.getItem(i).x <= this.xAxis.hiSVG ){
		if ( this.graph_element.points.getItem(i).y < minYSVG ) minYSVG = this.graph_element.points.getItem(i).y;
		if ( this.graph_element.points.getItem(i).y > maxYSVG ) maxYSVG = this.graph_element.points.getItem(i).y;
		noPointInRange = false;
	    }
	}
	if ( noPointInRange ) return;
	var yShiftSVG = 0.5* ( ( this.yAxis.loSVG + this.yAxis.hiSVG ) - ( minYSVG + maxYSVG ) ); // midpoint shift
	var yFactor   = ( this.yAxis.loSVG - this.yAxis.hiSVG ) / Math.max( 300., 1.1 * ( maxYSVG -  minYSVG ) );
	this.translate( 0., yShiftSVG );
	this.scale( 1., yFactor );
    };

    this.ageOfPageClock = function (ageOfPage){
    	hours=Math.floor(ageOfPage/3600);
    	minutes=Math.floor(ageOfPage/60)%60;
    	// graph
    	var age="";
    	if (hours) age+=hours+"h ";
    	if (minutes) age+=minutes+"m ";
    	age+=ageOfPage%60+"s ";
	//alert(age);
    	if ( this.age_element ){
    	    this.value_element.firstChild.nodeValue = this.value_element.firstChild.nodeValue; // Heh?! *This* prevents age_element from starting inheriting background color from the containing td element when it is updated!?
    	    this.age_element.firstChild.nodeValue = age+'ago';
    	    //age_element.setAttribute( 'style', 'background-color: #333333;' ); // Apparently, this has no effect. It keeps on inheriting the bkg color of the td element. 
    	    this.value_element.firstChild.nodeValue = this.value_element.firstChild.nodeValue; // Heh?! *This* prevents age_element from starting inheriting background color from the containing td element when it is updated!?
    	}
    	// table
    	try{
    	    age="";
    	    if (hours) age+=hours+"h&nbsp;";
    	    if (minutes) age+=minutes+"m&nbsp;";
    	    age+=ageOfPage%60+"s&nbsp;";
    	    document.getElementById(this.name+'-td_ageOfPage').innerHTML='Loaded&nbsp;'+age+'ago';
    	    var mainTableElem = document.getElementById(this.name+'-fadingTable');
    	    if      ( ageOfPage < 0.003 * this.pageRefreshPeriod ) mainTableElem.className = 'fresh';
    	    else if ( ageOfPage < 0.010 * this.pageRefreshPeriod ) mainTableElem.className = 'aging';
    	    else                                                   mainTableElem.className = 'stale';
    	} catch(ex){}// { alert(ex); }
    	ageOfPage=ageOfPage+1;
	// Two methods to pass the reference to this object:
	// * Method 1: Save this in another variable that we can use inside setTimeout.
	// var self = this;
    	this.Clock = setTimeout( function(){ self.ageOfPageClock( ageOfPage ); }, 1000 );
	// * Method 2: Pass this as the third argument. This is said not to work in IE...
    	//this.Clock = setTimeout( function( thisObj ){ thisObj.ageOfPageClock( ageOfPage ); }, 1000, this );
    };

    this.appendPoint = function ( p ){
	if ( !this.graph_element ) return;

	var xSVG = this.xAxis.toSVG( p.time );
	var ySVG;
	ySVG = this.yAxis.toSVG( p.value );
	this.title_element.firstChild.nodeValue = p.name;
	this.value_element.firstChild.nodeValue = formatNumber( p.value );

	var oldPoints = this.graph_element.getAttribute("points");
	if ( oldPoints ) this.graph_element.setAttribute("points", oldPoints+' '+xSVG+','+ySVG );
	else             this.graph_element.setAttribute("points", xSVG+','+ySVG );
	//var newPoints = graph_element.getAttribute("points");
	// Jump to the first point (in case the client's clock is not set correctly or is in another time zone):
	if ( oldPoints.length == 0 ) this.centerXAxisOnValue( xSVG );
	if ( this.following ) this.followLastPoint( xSVG );
	if ( this.autoZooming ) this.transformYToFit();
    };

    this.TrackFinderFromJson = function (){
	// Get info on Track Finder from csctf_emupageone flashlist
	var state = null;
	$.getJSON( this.DataURL+'?fmt=json&flash=urn:xdaq-flashlist:csctf_emupageone', function(json){
	    var time = toUnixTime( json.table.properties.LastUpdate );
	    $('#'+self.name+'-td_localDateTime').text( timeToString( time ) );
	    var msg = '';
	    $.each( json.table.rows, function(i,row){
		if ( i == 0 ){
		    msg += 'EMUPAGEONE_FSM_t.rows.length='+row.EMUPAGEONE_FSM_t.rows.length+'   EMUPAGEONE_RATES_t.rows.length='+row.EMUPAGEONE_RATES_t.rows.length;
		    var validConfPatterns = ['^EmuLocal$','^Configuration$'];
		    var foundGlobalConf = false;
		    var foundLocalConf  = false;
		    for ( var p=0; p<validConfPatterns.length; p++ ){
			$.each( row.EMUPAGEONE_FSM_t.rows, function(j,configRow){
			    if ( configRow['id'].search(validConfPatterns[p])==0 ){
				$('#'+self.name+'-td_value_state').attr( 'class', configRow['state'] );
				$('#'+self.name+'-a_value_state').text( configRow['state'] );
				$('#'+self.name+'-a_value_state').attr( 'title', 'Operation \"'+configRow['id']+'\" is in '+configRow['state']+' state.' );
				$('#'+self.name+'-td_value_confkey').attr( 'class', configRow['conf_key'] );
				$('#'+self.name+'-a_value_confkey').text( configRow['conf_key'] );
				$('#'+self.name+'-a_value_confkey').attr( 'title', 'Operation \"'+configRow['id']+'\" has configuration key '+configRow['conf_key']+'.' );
				if ( p==0 ) foundGlobalConf = true;
				else        foundLocalConf  = true;
				state = configRow['state'];
			    }
			});
		    }
		    if ( !foundGlobalConf && !foundLocalConf ){
			$('#'+self.name+'-td_value_state').attr( 'class', 'UNKNOWN' );
			$('#'+self.name+'-a_value_state').text( 'UNKNOWN' );
			$('#'+self.name+'-a_value_state').attr( 'title', (row.EMUPAGEONE_FSM_t.rows.length==0?'No operation found. (This is normal IF the TF Cell has been restarted in this run.)':' Only invalid operation found.') );
			$('#'+self.name+'-td_value_confkey').attr( 'class', 'UNKNOWN' );
			$('#'+self.name+'-a_value_confkey').text( 'UNKNOWN' );
			$('#'+self.name+'-a_value_confkey').attr( 'title', (row.EMUPAGEONE_FSM_t.rows.length==0?'No operation found. (This is normal IF the TF Cell has been restarted in this run.)':' Only invalid operation found.') );
		    }
		    $.each( row.EMUPAGEONE_RATES_t.rows, function(j,ratesRow){ 
			if ( j == 0 ){
			    var graphPoint = { name:'total SP input rate [Hz]', time:time, value:ratesRow['Total SPs Rate'] };
			    self.appendPoint( graphPoint );
			    $('#'+self.name+'-a_value_min').text( formatNumber( ratesRow['Min Single SP Rate'] ) + ' Hz' );
			    if ( ratesRow['Min Single SP Rate'] == 0 && ratesRow['Total SPs Rate'] > 100 ){
				$('#'+self.name+'-td_value_min').attr('class', 'WARN' );
				$('#'+self.name+'-a_value_min').attr('title', 'One or more SPs may be dead. Click to check.' );
			    }
			    else{
				$('#'+self.name+'-td_value_min').attr('class', '');
				$('#'+self.name+'-a_value_min').attr('title', '');
				       }
			    $('#'+self.name+'-a_value_total').text( formatNumber( ratesRow['Total SPs Rate'] ) + ' Hz' );
			}
		    });
		}
	    });
	}).success( function(){
	    // Get info on Track Finders' AFD registers from csctf_csr_af_wordcount flashlist. They only work as a canary in the enabled state.
	    if ( state == 'enabled' ){
		$.getJSON( self.DataURL+'?fmt=json&flash=urn:xdaq-flashlist:csctf_csr_af_wordcount', function(json){
		    var time = toUnixTime( json.table.properties.LastUpdate );
		    var msg = '';
		    var title = '';
		    var maxAllowedChange = 2;
		    var maxChange = 0;
		    $.each( json.table.rows, function(i,row){
			$.each( row.CSR_AF_WORDCOUNT_t.rows, function(j,afCountRow){ 
			    if ( self.trends[j]==undefined || self.trends[j]==null ) self.trends[j] = new Trend(10);
			    self.trends[j].add( time, afCountRow.VALUE );
			    var change = self.trends[j].difference(10);
			    if ( Math.abs( change ) > maxChange ) maxChange = Math.abs( change );
			    if ( Math.abs( change ) > maxAllowedChange ){
				title += '('+afCountRow.NUM_ASP+','+afCountRow.NUM_LINK+','+afCountRow.VALUE+','+change+')\n';
			    }

			    //if ( Math.abs( change ) > 0 ) console.log( 'CSCTF: '+self.trends[j].print()+' self.trends['+j+'].difference(10)='+self.trends[j].difference(10) );	    
			    msg += afCountRow.NUM_ASP+' '+afCountRow.NUM_LINK+' '+afCountRow.VALUE+' '+self.trends[j].difference(10)+'\n';
			});
			if ( title.length > 1 ){
			    title = "These links' AFD register changed more than "+maxAllowedChange+' (SP,link,value,change):\n'+title;
			    $('#'+self.name+'-a_value_afd').attr('title',title).text( maxChange );
			    $('#'+self.name+'-td_value_afd').attr('class','WARN');
			}
			else{
			    $('#'+self.name+'-a_value_afd').attr('title','All looking OK now.').text( maxChange );
			    $('#'+self.name+'-td_value_afd').attr('class','');
			}
		    });
		}).success( function(){
		    clearTimeout(self.Clock);
		    self.ageOfPageClock(0);
		});
	    }
	    else{
		$('#'+self.name+'-a_value_afd').attr('title','Not enabled now therefore no reading reported.').text( '0' );
		$('#'+self.name+'-td_value_afd').attr('class','');
		clearTimeout(self.Clock);
		self.ageOfPageClock(0);
	    }
	});
	
    };

    this.TTCFromJson = function (){
	// Get info on Track Finder from csctf_emupageone flashlist
	$.getJSON( self.DataURL+'?fmt=json&flash=urn:xdaq-flashlist:ttcci_conf', function(json){
	    var combinedState = null;
	    $.each( json.table.rows, function(i,row){
		if ( row.ComponentKey.substr(0,3) == 'CSC' ){
		    if ( combinedState && combinedState != row.FSMState ) combinedState = 'INDEFINITE';
		    else                                                  combinedState = row.FSMState;
		    //console.log( row.ComponentKey+' '+row.FSMState+' '+combinedState );
		}
	    });
	    $('#'+self.name+'-td_value_State').attr( 'class', combinedState );
	    $('#'+self.name+'-a_value_State').text( combinedState );
	    $('#'+self.name+'-a_value_State').attr( 'title', (combinedState == 'INDEFINITE' ? 'Not all TTC applications are in the same FSM state.' : 'All TTC applications are '+combinedState) );
	    
	}).success( function(){
	    $.getJSON( self.DataURL+'?fmt=json&flash=urn:xdaq-flashlist:ttcci_scalers', function(json){
		var time = toUnixTime( json.table.properties.LastUpdate );
		$('#'+self.name+'-td_localDateTime').text( timeToString( time ) );
		var minL1ARate = Number.POSITIVE_INFINITY;
		$.each( json.table.rows, function(i,row){
		    if ( row.ComponentKey.substr(row.ComponentKey.length-4) == 'CSC+' ){
			$('#'+self.name+'-a_value_L1APlus').text( formatNumber( row.L1ARateHz )+' Hz' );
			if ( row.L1ARateHz < minL1ARate ) minL1ARate = row.L1ARateHz;
			//console.log( row.ComponentKey+' '+row.L1ARateHz );
		    }
		    else if ( row.ComponentKey.substr(row.ComponentKey.length-4) == 'CSC-' ){
			$('#'+self.name+'-a_value_L1AMinus').text( formatNumber( row.L1ARateHz )+' Hz' );
			if ( row.L1ARateHz < minL1ARate ) minL1ARate = row.L1ARateHz;
			//console.log( row.ComponentKey+' '+row.L1ARateHz );
		    }
		    else if ( row.ComponentKey.substr(row.ComponentKey.length-5) == 'CSCTF' ){
			$('#'+self.name+'-a_value_L1ATF').text( formatNumber( row.L1ARateHz )+' Hz' );
			if ( row.L1ARateHz < minL1ARate ) minL1ARate = row.L1ARateHz;
			//console.log( row.ComponentKey+' '+row.L1ARateHz );
		    }
		});
		var graphPoint = { name:'TTC L1A rate [Hz]', time:time, value:minL1ARate };
		self.appendPoint( graphPoint );
	    }).success( function(){
		clearTimeout(self.Clock);
		self.ageOfPageClock(0);
	    });
	});
    }


    this.diskUsageFromJson = function(){
	// Get data disk info
	$.getJSON( this.DataURL+'?fmt=json&flash=urn:xdaq-flashlist:diskInfo', function(json){
	    var time = toUnixTime( json.table.properties.LastUpdate );
	    $('#'+self.name+'-td_localDateTime').text( timeToString( time ) );
	    var disks  = new Array();
	    $.each( json.table.rows, function(i,contextRow){
		if ( contextRow.context.lastIndexOf(':9999') > 0 ){
		    $.each( contextRow.diskUsage.rows, function(j,fsRow){ 
			if ( fsRow.fileSystem == '/data' ){
			    try{
				// Select local DAQ farm machines that read out DDUs
				var matches = contextRow.context.match('^http://([cC][sS][cC]-[cC]2[dD]08-(0[1-9]|10)(.cms)?):[0-9]+');
				if ( matches.length > 1 ){
				    disks.push( new Disk( matches[1], '/data', fsRow.sampleTime, fsRow.state, 
							  fsRow.usePercent, (1.-0.01*fsRow.usePercent)*fsRow.totalMB ) );
				}
			    }
			    catch(e){}
			}
		    });
		}
	    });
	    // Display disk of highest usage
	    var d = disks.sort( function( a, b ){ return b.usage - a.usage; } )[0]; // Sort function must return a pos/neg number. With boolean, e.g. a>b, it doesn't work in WebKit (e.g., in rekonq).
	    var klass = 'ON';
	    if      ( d.usage > 80 ) klass = 'WARN';
	    else if ( d.usage > 95 ) klass = 'OFF';
	    $('#'+self.name+'-td_value_0').attr('class',klass);
	    $('#'+self.name+'-a_value_0').attr('title',d.host+':'+d.mount+' has '+formatNumber(d.free)+' MB free left at '+d.time+'.').text(d.usage.toFixed(0)+' %');
	})
	    .success( function(){
		clearTimeout(self.Clock);
		self.ageOfPageClock(0);
	    });

    }


    this.getXML = function(){
	var msg = 'getXML ';
	
	// $.ajax({ url:     self.DataURL, 
	// 	 success: function(xml){
	// 	     $('monitorable',xml).each( 
	// 		 function(m){
	// 		     msg = msg + $(this).attr('name') + ', ';
	// 		 }
	// 	     );
	// 	 }
	//        })
	//     .complete( function(){ alert( msg ); } );

	//msg = msg + ' ' + self.name+'-td_localDateTime ' + document.getElementById( 'blabla' );
	//alert( msg );

	$.get( self.DataURL, {},
	       function(xml) {
		   var time;
		   $('ForEmuPage1',xml).each( 
		       function(){
			   $('#'+self.name+'-td_localDateTime').text( $(this).attr('localDateTime') );
			   time = $(this).attr('localUnixTime')* 1000; // ms in JavaScript!
			   if ( isNaN( time ) ) time = toUnixTime( $(this).attr('localDateTime') ); // PC Manager doesn't report localUnixTime...
		       }
		   );
		   $('monitorable',xml).each( 
		       function(){
			   if ( self.name == 'FED' ){
			       if ( self.trends.length == 0 ){
				   self.trends[0] = new Trend(6); // for + side errors
				   self.trends[1] = new Trend(6); // for - side errors
				   self.trends[2] = new Trend(6); // for TF     errors
			       }
			       if      ( $(this).attr('name') == 'title' ){
				   $('#FED-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'State' ){
				   $('#FED-a_name_State'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#FED-a_value_State' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#FED-td_value_State').attr('class',$(this).attr('value'   ));
				   self.state = $(this).attr('value');
			       }
			       else if ( $(this).attr('name') == 'ME+ Errors' ){
				   self.trends[0].add( time, Number($(this).attr('value')) );
				   $('#FED-td_value_ErrorsPlus').attr('class',( self.trends[0].rate(6) <= 0 ? '' : 'WARN' ));
				   $('#FED-a_name_ErrorsPlus'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('ME+');
			           $('#FED-a_value_ErrorsPlus' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'ME- Errors' ){
				   self.trends[1].add( time, Number($(this).attr('value')) );
				   $('#FED-td_value_ErrorsMinus').attr('class',( self.trends[1].rate(6) <= 0 ? '' : 'WARN' ));
				   $('#FED-a_name_ErrorsMinus'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('ME−');
			           $('#FED-a_value_ErrorsMinus' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'TF Errors' ){
				   self.trends[2].add( time, Number($(this).attr('value')) );
				   $('#FED-td_value_ErrorsTF').attr('class',( self.trends[2].rate(6) <= 0 ? '' : 'WARN' ));
				   $('#FED-a_name_ErrorsTF'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('TF');
			           $('#FED-a_value_ErrorsTF' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'ME+ Configuration' ){
				   $('#FED-a_name_ConfigPlus'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('ME+');
			           $('#FED-a_value_ConfigPlus' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'ME- Configuration' ){
				   $('#FED-a_name_ConfigMinus'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('ME−');
			           $('#FED-a_value_ConfigMinus' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'TF Configuration' ){
				   $('#FED-a_name_ConfigTF'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('TF');
			           $('#FED-a_value_ConfigTF' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'DCC Total Output Rate' ){
			           $('#FED-td_name_DCC_out' ).attr('class', ( $(this).attr('value') == 0 && self.state == 'Enabled' ? 'WARN' : '' ));
				   $('#FED-a_name_DCC_out'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('Total Out');
			           $('#FED-a_value_DCC_out' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text( formatNumber(Number($(this).attr('value'))*0.001)+' kB/s' );
			       }
			       else if ( $(this).attr('name') == 'DCC Total Input Rate' ){
			           $('#FED-td_name_DCC_in' ).attr('class', ( $(this).attr('value') == 0 && self.state == 'Enabled' ? 'WARN' : '' ));
				   $('#FED-a_name_DCC_in'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('Total In');
			           $('#FED-a_value_DCC_in' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text( formatNumber(Number($(this).attr('value'))*0.001)+' kB/s' );
				   if ( time > 0 ){
				       var graphPoint = { name: 'DCC input [kB/s]', 
							  time:  time, 
							  value: Number($(this).attr('value'))*0.001 // B --> kB
							};
				       self.appendPoint( graphPoint );
				   }
				   
			       }
			   }
			   else if ( self.name == 'DAQ' ){
			       if ( self.trends.length == 0 ){
				   self.trends[0] = new Trend(2); // for min events
				   self.trends[1] = new Trend(2); // for max events
			       }
			       if      ( $(this).attr('name') == 'title' ){
				   $('#DAQ-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'state' ){
				   $('#DAQ-a_name_State'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DAQ-a_value_State' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#DAQ-td_value_State').attr('class',$(this).attr('value'   ));
				   self.state = $(this).attr('value');
			       }
			       else if ( $(this).attr('name') == '#' ){
				   $('#DAQ-a_name_RunNum'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DAQ-a_value_RunNum' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'type' ){
				   $('#DAQ-a_name_RunType'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DAQ-a_value_RunType' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'ctrl' ){
				   $('#DAQ-a_name_RunCtrl'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DAQ-a_value_RunCtrl' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'start' ){
				   $('#DAQ-a_name_Start'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DAQ-a_value_Start' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'stop' ){
				   var runType = $('monitorable[name="type"]',xml).attr('value');
				   // if ( Math.random() > 0.5 ){
				   if ( runType.indexOf('Calib') == 0 ){
				       // Save the stop-time anchor and remove it:
			               if ( $('#DAQ-a_value_Stop' ).length > 0 ){
					   self.stopTime_fragment.appendChild( document.getElementById('DAQ-a_value_Stop') );
					   $('#DAQ-a_value_Stop' ).remove();
				       }
				       // Reinsert the calib progress table from the doc fragment cache:
				       if ( $('#DAQ-table_Progress' ).length == 0 ){
					   $('#DAQ-td_value_Stop').append( self.progressTable_fragment );
				       }
				       $('#DAQ-a_name_Stop'  ).attr('href' ,'').attr('title','Progress of the calibration run sequence.').text('progress');
				       var iR = $('monitorable[name="calib runIndex"]' ,xml).attr('value');
				       var nR = $('monitorable[name="calib nRuns"]'    ,xml).attr('value');
				       var iS = $('monitorable[name="calib stepIndex"]',xml).attr('value');
				       var nS = $('monitorable[name="calib nSteps"]'   ,xml).attr('value');
				       $('#DAQ-progress_Runs' ).attr('value',iR).attr('max',nR)
					   .attr('title','Finished '+iR+' runs in a sequence of '+nR+' calibration runs.');
				       $('#DAQ-progress_Steps').attr('value',iS).attr('max',nS)
					   .attr('title','Done step '+iS+' of '+nS+'.');
				   }
				   else{
				       // Save the calib progress table and remove it:
			               if ( $('#DAQ-table_Progress' ).length > 0 ){
					   self.progressTable_fragment.appendChild( document.getElementById('DAQ-table_Progress') );
					   $('#DAQ-table_Progress' ).remove();
				       }
				       // Reinsert the stop-time anchor from the doc fragment cache:
				       if ( $('#DAQ-a_value_Stop' ).length == 0 ){
					   $('#DAQ-td_value_Stop').append( self.stopTime_fragment );
				       }
				       $('#DAQ-a_name_Stop'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			               $('#DAQ-a_value_Stop' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
				   }

			       }
			       else if ( $(this).attr('name') == 'min events' ){
				   var klass = '';
				   if ( time > 0 ){
				       self.trends[0].add( time, Number($(this).attr('value')) );
				       if ( self.trends[0].rate(2) == 0 && self.state == 'Enabled' ) klass = 'WARN';
				   }
				   $('#DAQ-td_value_Min').attr('class',klass);
				   $('#DAQ-a_name_Min'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('min');
			           $('#DAQ-a_value_Min' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'max events' ){
				   var klass = '';
				   if ( time > 0 ){
				       self.trends[1].add( time, Number($(this).attr('value')) );
				       if ( self.trends[1].rate(2) == 0 && self.state == 'Enabled' ) klass = 'WARN';
				       var graphPoint = { name: 'max rate [event/s]', 
							  time:  time, 
							  value: self.trends[1].rate(2)
							};
				       self.appendPoint( graphPoint );
				   }
				   $('#DAQ-td_value_Max').attr('class',klass);
				   $('#DAQ-a_name_Max'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('max');
			           $('#DAQ-a_value_Max' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			   }
			   else if ( self.name == 'DQM' ){
			       if      ( $(this).attr('name') == 'title' ){
				   $('#DQM-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'state' ){
				   $('#DQM-a_name_State'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DQM-a_value_State' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#DQM-td_value_State').attr('class',$(this).attr('value'   ));
				   self.state = $(this).attr('value');
			       }
			       else if ( $(this).attr('name') == 'cscrate' ){
				   $('#DQM-a_name_CSC'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('CSC');
			           $('#DQM-a_value_CSC' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
				   $('#DQM-td_value_CSC').attr('class',( $(this).attr('value') == 0 && self.state == 'Enabled' ? 'WARN' : '' ));
			       }
			       else if ( $(this).attr('name') == 'evtrate' ){
				   $('#DQM-a_name_Event'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text('event');
			           $('#DQM-a_value_Event' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
				   $('#DQM-td_value_Event').attr('class',( $(this).attr('value') == 0 && self.state == 'Enabled' ? 'WARN' : '' ));
			       }
			       else if ( $(this).attr('name') == 'quality' ){
				   $('#DQM-a_name_Quality'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ));
			           $('#DQM-a_value_Quality' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
				   if ( time > 0 ){
				       var graphPoint = { name: 'quality', 
							  time:  time, 
							  value: Number($(this).attr('value'))
							};
				       self.appendPoint( graphPoint );
				   }
			       }
			   }
			   else if ( self.name == 'PCMan' ){
			       if      ( $(this).attr('name') == 'title' ){
				   $('#PCMan-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value').replace(' ',' '));
			       }
			       else if ( $(this).attr('name') == 'State' ){
				   $('#PCMan-a_name_State'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMan-a_value_State' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#PCMan-td_value_State').attr('class',$(this).attr('value'   ));
			       }
			       else if ( $(this).attr('name') == 'Key+' ){
				   $('#PCMan-a_name_KeyP'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMan-a_value_KeyP' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			       else if ( $(this).attr('name') == 'Key-' ){
				   $('#PCMan-a_name_KeyM'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMan-a_value_KeyM' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			       }
			   }
			   else if ( self.name == 'PCMonP' ){
			       if ( self.trends.length == 0 ) self.trends[0] = new Trend(3); // for heartbeat
			       if      ( $(this).attr('name') == 'title' ){
				   $('#PCMonP-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value').replace(' ',' '));
			       }
			       else if ( $(this).attr('name') == 'VME Access' ){
				   $('#PCMonP-a_name_Access'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMonP-a_value_Access' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#PCMonP-td_value_Access').attr('class',$(this).attr('value'   ));
			       }
			       else if ( $(this).attr('name') == 'Heartbeat' ){
				   self.trends[0].add( time, Number($(this).attr('value')) );
				   $('#PCMonP-td_value_Heart').attr('class',( self.trends[0].rate(3) == 0 ? 'WARN' : '' ));
				   $('#PCMonP-a_name_Heart'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMonP-a_value_Heart' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text( formatNumber( self.trends[0].rate(3) )+' Hz');
				   // alert( self.trends[0].print() );
			       }
			   }
			   else if ( self.name == 'PCMonM' ){
			       if ( self.trends.length == 0 ) self.trends[0] = new Trend(3); // for heartbeat
			       if      ( $(this).attr('name') == 'title' ){
				   $('#PCMonM-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value').replace(' ',' '));
			       }
			       else if ( $(this).attr('name') == 'VME Access' ){
				   $('#PCMonM-a_name_Access'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMonM-a_value_Access' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#PCMonM-td_value_Access').attr('class',$(this).attr('value'   ));
			       }
			       else if ( $(this).attr('name') == 'Heartbeat' ){
				   self.trends[0].add( time, Number($(this).attr('value')) );
				   $('#PCMonM-td_value_Heart').attr('class',( self.trends[0].rate(3) == 0 ? 'WARN' : '' ));
				   $('#PCMonM-a_name_Heart'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#PCMonM-a_value_Heart' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text( formatNumber( self.trends[0].rate(3) )+' Hz');
			       }
			   }
			   else if ( self.name == 'BlueP' ){
			       if      ( $(this).attr('name') == 'title' ){
				   $('#BlueP-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value').replace(' ',' '));
			       }
			       else if ( $(this).attr('name') == 'VME Access' ){
				   $('#BlueP-a_name_Access'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#BlueP-a_value_Access' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#BlueP-td_value_Access').attr('class',$(this).attr('value'   ));
			       }
			   }
			   else if ( self.name == 'BlueM' ){
			       if      ( $(this).attr('name') == 'title' ){
				   $('#BlueM-a_value_Title').attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value').replace(' ',' '));
			       }
			       else if ( $(this).attr('name') == 'VME Access' ){
				   $('#BlueM-a_name_Access'  ).attr('href' ,$(this).attr('nameURL' )).attr('title',$(this).attr('nameDescription' )).text($(this).attr('name' ).replace(' ',' '));
			           $('#BlueM-a_value_Access' ).attr('href' ,$(this).attr('valueURL')).attr('title',$(this).attr('valueDescription')).text($(this).attr('value'));
			           $('#BlueM-td_value_Access').attr('class',$(this).attr('value'   ));
			       }
			   }
		       }
		   );
	       }
	     )
	    .success( function(){
		clearTimeout(self.Clock);
		self.ageOfPageClock(0);
	    });
	    //.complete( function(){ alert( msg ); } );

    }

    this.autoReloadData = function (){
	if      ( self.name == 'DAQDisks' ) this.diskUsageFromJson();
	else if ( self.name == 'CSCTF'    ) this.TrackFinderFromJson();
	else if ( self.name == 'TTC'      ) this.TTCFromJson();
	else                                this.getXML();

	setTimeout( function(){ self.autoReloadData(); }, this.pageRefreshPeriod );
    };

    //
    // End of member methods' definitions
    //

    // Time development graph, if any
    if ( this.graph_element ){
	try{	
	    var d = new Date();
	    var t = d.getTime();
	    // Times in millisec!
	    this.xAxis = new Axis( this, t-300000, t+300000, 5, true,
				   Number(this.pad_element.getAttribute('x')),
				   Number(this.pad_element.getAttribute('x'))+Number(this.pad_element.getAttribute('width')),
				   1./this.svg_element.getScreenCTM().a );
	    this.yAxis = new Axis( this, 0., 1., 5, false,
				   Number(this.pad_element.getAttribute('y'))+Number(this.pad_element.getAttribute('height')),
				   Number(this.pad_element.getAttribute('y')),
				   1./this.svg_element.getScreenCTM().d );
	    this.redrawAxes();
	} catch(ex) {
	    //alert( ex );
	}

	this.attachListeners();
    }


    this.ageOfPageClock(0);
    
    // setTimeout( function(){ self.autoReloadData(); }, refreshOffset );

    this.autoReloadData();
}



//
// Axis object
//

function Axis(panel,loValue,hiValue,nLabels,isX,loSVG,hiSVG,svgPerPixel){
    this.panel = panel;
    this.loValueRef = loValue; // initial value to be preserved for reference
    this.hiValueRef = hiValue; // initial value to be preserved for reference
    this.loValue = loValue; // current value of lower end
    this.hiValue = hiValue; // current value of higher end

    this.nLabels = nLabels;
    this.isX = isX;

    this.loSVGRef = loSVG; // initial SVG coord to be preserved for reference
    this.hiSVGRef = hiSVG; // initial SVG coord  to be preserved for reference
    this.loSVG = loSVG; // current SVG coord of lower end
    this.hiSVG = hiSVG; // current SVG coord of higher end

    this.svgPerPixelRef   = svgPerPixel;
    this.lengthSVGRef     = this.hiSVGRef - this.loSVGRef;
    this.valuePerPixelRef = ( this.hiValueRef - this.loValueRef ) / this.lengthSVGRef * this.svgPerPixelRef;
    this.svgPerPixel   = this.svgPerPixelRef;
    this.lengthSVG     = this.lengthSVGRef;
    this.valuePerPixel = this.valuePerPixelRef;

    this.labels = new Array(this.nLabels);

    this.updateLabels = function(){
	var increment = ( this.hiValue - this.loValue ) / ( this.nLabels - 1 );
    	for ( var i=0; i<this.labels.length; i++ ){
	    if ( this.labels[i] ) this.labels[i].setValue( this.loValue + i * increment );
	    else this.labels[i] = new Label( this.loValue + i * increment, this.isX );
	}
    }

    this.updateScaling = function(){
	this.lengthSVG     = this.hiSVG - this.loSVG;
	this.svgPerPixel   = this.svgPerPixelRef * this.lengthSVG / this.lengthSVGRef;
	this.valuePerPixel = ( this.hiValue - this.loValue ) / this.lengthSVG * this.svgPerPixel;
    }

    this.toSVG = function( value ){
	return this.loSVGRef + ( value - this.loValueRef ) * ( this.hiSVGRef - this.loSVGRef ) / ( this.hiValueRef - this.loValueRef );
    }

    this.toValue = function( SVG ){
	return this.loValueRef + ( SVG - this.loSVGRef ) / ( this.hiSVGRef - this.loSVGRef ) * ( this.hiValueRef - this.loValueRef );
    }

    this.transform = function( svgMatrix ){
	if ( svgMatrix ){
	    if ( this.isX ){
		this.loSVG = ( this.loSVGRef - svgMatrix.e ) / svgMatrix.a;
		this.hiSVG = ( this.hiSVGRef - svgMatrix.e ) / svgMatrix.a;
	    } else {
		this.loSVG = ( this.loSVGRef - svgMatrix.f ) / svgMatrix.d;
		this.hiSVG = ( this.hiSVGRef - svgMatrix.f ) / svgMatrix.d;
	    }
	}
	this.loValue = this.toValue( this.loSVG );
	this.hiValue = this.toValue( this.hiSVG );
	this.updateScaling();
	this.updateLabels();
    }

    this.formatValue = function( value ){
	if ( this.isX ){
	    var d = new Date();
	    d.setTime( value );
	    return d.getFullYear()+'-'+(d.getMonth()+1)+'-'+d.getDate()+' '+d.getHours()+':'+d.getMinutes()+':'+d.getSeconds();
	}
	else { return formatNumber( value ); }
    }

    this.pixelToValue = function( pixel, formatted ){
	var svgMatrix = this.panel.svg_element.getScreenCTM();
	this.panel.g_element.transform.baseVal.consolidate();
	var gMatrix = this.panel.g_element.transform.baseVal.getItem(0).matrix;
	var graphMatrix = null;
	this.panel.graph_element.transform.baseVal.consolidate();
	if ( this.panel.graph_element.transform.animVal.numberOfItems){
	    graphMatrix = this.panel.graph_element.transform.animVal.getItem(0).matrix
	}
	var SVG;
	if ( this.isX ){
	    // Transform from screen to svg:svg...
	    SVG = ( pixel - svgMatrix.e ) / svgMatrix.a;
	    // ...then from svg:svg to svg:g...
	    SVG = ( SVG - gMatrix.e ) / gMatrix.a;
	    // ...then from svg:g to svg:polyline (the graph)...
	    if ( graphMatrix ) SVG = ( SVG - graphMatrix.e ) / graphMatrix.a;
	} else {
	    // Transform from screen to svg:svg...
	    SVG = ( pixel - svgMatrix.f ) / svgMatrix.d;
	    // ...then from svg:svg to svg:g...
	    SVG = ( SVG - gMatrix.f ) / gMatrix.d;
	    // ...then from svg:g to svg:polyline (the graph)...
	    if ( graphMatrix ) SVG = ( SVG - graphMatrix.f ) / graphMatrix.d;
	}
	// ... and finally to value:
	if ( formatted ) return this.formatValue( this.toValue( SVG ) );
	return this.toValue( SVG );
    }

    this.print = function(){
	msg = 
	    '<table>' +
	    '  <tr><td>isX           </td><td>' + this.isX            + '</td></tr>' +
	    '  <tr><td>loValue       </td><td>' + this.loValue        + '</td></tr>' +
	    '  <tr><td>hiValue       </td><td>' + this.hiValue        + '</td></tr>' +
	    '  <tr><td>loValueRef    </td><td>' + this.loValueRef     + '</td></tr>' +
	    '  <tr><td>hiValueRef    </td><td>' + this.hiValueRef     + '</td></tr>' +
	    '  <tr><td>loSVG         </td><td>' + this.loSVG          + '</td></tr>' +
	    '  <tr><td>hiSVG         </td><td>' + this.hiSVG          + '</td></tr>' +
	    '  <tr><td>loSVGRef      </td><td>' + this.loSVGRef       + '</td></tr>' +
	    '  <tr><td>hiSVGRef      </td><td>' + this.hiSVGRef       + '</td></tr>' +
	    '  <tr><td>lengthSVG     </td><td>' + this.lengthSVG      + '</td></tr>' +
	    '  <tr><td>svgPerPixel   </td><td>' + this.svgPerPixel    + '</td></tr>' +
	    '  <tr><td>valuePerPixel </td><td>' + this.valuePerPixel  + '</td></tr>' +
	    '</table>';
	return msg;
    }

}


function Label(value,isTime){
    this.value = 0;
    this.isTime = isTime;
    this.stringValue1 = '';
    this.stringValue2 = ''; // YYYY-MM-DD if isTime; null otherwise

    this.zeroPadTo2 = function( number ){
	if ( number <= 9 ) return '0'+number.toString();
	return number.toString();
    }

    this.setValue = function(v){
	this.value = v;
  	if ( isTime ){
//  	if ( false ){
	    var d = new Date();
	    d.setTime(this.value);
	    this.stringValue1 = this.zeroPadTo2(d.getHours())+':'+this.zeroPadTo2(d.getMinutes())+':'+this.zeroPadTo2(d.getSeconds());
	    this.stringValue2 = d.getFullYear().toString().substr(2,2)+'-'+this.zeroPadTo2(d.getMonth()+1)+'-'+this.zeroPadTo2(d.getDate());
	} else {
	    this.stringValue1 = formatNumber( this.value );
	}
    }

    this.setValue( value );
}


function Trend( maxSize ){
    this.maxSize = Math.max( 2, maxSize ); // at least 2
    this.points = new Array();
    this.add = function( t, v ){
	if ( this.points.push( { time:t, value:v } ) > this.maxSize ) while( this.points.length > this.maxSize ) this.points.shift(); // keep size at maxSize at most
    };
    this.rate = function( sampleSize ){
	if ( this.points.length < 2 ) return 0;
	var iLast = Math.min( this.points.length, Math.max( 2, sampleSize ) ) - 1;
	if ( this.points[iLast].time <= this.points[0].time ) return 0;
	return ( this.points[iLast].value - this.points[0].value ) * 1000. / ( this.points[iLast].time - this.points[0].time ); // ms --> s
    };
    this.difference = function( sampleSize ){
	if ( this.points.length < 2 ) return 0;
	var iLast = Math.min( this.points.length, Math.max( 2, sampleSize ) ) - 1;
	return ( this.points[iLast].value - this.points[0].value );
    };
    this.print = function(){
	var str = 'maxSize '+this.maxSize+' length '+this.points.length+' ';
	for ( var i=0; i<this.points.length; i++ ) str = str+'( '+this.points[i].time+','+this.points[i].value+') ';
	return str;
    }
}


function appendPoint( p ){
    if ( !graph_element ) return;

    var xSVG = xAxis.toSVG( p.time );
    var ySVG;
    ySVG = yAxis.toSVG( p.value );
    title_element.firstChild.nodeValue = p.name;
    value_element.firstChild.nodeValue = formatNumber( p.value );

    var oldPoints = graph_element.getAttribute("points");
    if ( oldPoints ) graph_element.setAttribute("points", oldPoints+' '+xSVG+','+ySVG );
    else             graph_element.setAttribute("points", xSVG+','+ySVG );
    //var newPoints = graph_element.getAttribute("points");
    previousPoint.time  = p.time;
    previousPoint.value = p.value;
    // Jump to the first point (in case the client's clock is not set correctly or is in another time zone):
    if ( oldPoints.length == 0 ) centerXAxisOnValue( xSVG );
    if ( following ) followLastPoint( xSVG );
    if ( autoZooming ) transformYToFit();
}

function Monitorable( time, name, value, nameDescr, valueDescr, nameURL, valueURL ){
  this.time       = time;
  this.name       = name;
  this.value      = value;
  this.nameDescr  = nameDescr;
  this.valueDescr = valueDescr;
  this.nameURL    = nameURL;
  this.valueURL   = valueURL;
  
  this.previousTime  = this.time;
  this.previousValue = this.value;
  this.prevPrevTime  = this.time;
  this.prevPrevValue = this.value;
  
  this.set = function( time, name, value, nameDescr, valueDescr, nameURL, valueURL ){
    this.prevPrevTime  = this.previousTime;
    this.prevPrevValue = this.previousValue;
    this.previousTime  = this.time;
    this.previousValue = this.value;

    this.time       = time;
    this.value      = value;
    this.nameDescr  = nameDescr;
    this.valueDescr = valueDescr;
    this.nameURL    = nameURL;
    this.valueURL   = valueURL;
  };

  this.rate = function(){
    if ( this.time == this.previousTime ) return 0;
    return Math.max( 0, (this.value-this.previousValue) * 1000 / (this.time-this.previousTime) ); // ms --> s
  };

  this.rate2 = function(){ // rate calculated over two samplings
    if ( this.time == this.prevPrevTime ) return 0;
    return Math.max( 0, (this.value-this.prevPrevValue) * 1000 / (this.time-this.prevPrevTime) ); // ms --> s
  };
}
