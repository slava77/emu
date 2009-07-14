
      //////////////////////
      // START LOADING PART
      //////////////////////

var Url = document.URL;

var Xml = document.implementation.createDocument("", "", null);

Xml.addEventListener("load", xmlLoaded, false);

var Timer = null;

function showHTML(){
    var s = new XMLSerializer();
    var str = s.serializeToString(document);
    document.getElementById("htmlListed").innerHTML = str.replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

function loadCountsXML(){
    Xml.load( Url );
}

function autoLoadCountsXML(){
    //alert(document.getElementById("refreshPeriod").value+' '+document.getElementById("autoRefresh").checked);
    var refreshPeriod = document.getElementById("refreshPeriod").value;
    var autoRefresh = document.getElementById("autoRefresh").checked;
    clearTimeout(Timer);
    if ( refreshPeriod != 0 && autoRefresh ){
	loadCountsXML();
	Timer = setTimeout('autoLoadCountsXML()', refreshPeriod*1000);
	//alert(functionToCall+' '+refreshPeriod);
    }
 }


function removeAllDescendants( node ){
    if ( node.hasChildNodes() ){
	while ( node.childNodes.length >= 1 ){
	    node.removeChild( node.firstChild );       
	} 
    }
}


function xmlLoaded(e){
    //alert('xmlLoaded');
    valuesFromXmlToTable( Xml );
    scale();
}


function on_load()
{
    attachListeners();
    autoLoadCountsXML();
}

function onSampleOrCounterSwitch(){
    valuesFromXmlToTable( Xml );
    scale();
}

function valuesFromXmlToTable( xmlDoc ){
    try{
	//var str = '';
	var sampleName=getSelectedRadio('sampleSelector');
	var counterName=getSelectedRadio('counterSelector');
	if ( !counterName ) counterName = getSelectedListItem('counterSelector');
	//alert(counterName);
	var samples=xmlDoc.getElementsByTagName('sample');
	var delta_t=0;
	for (s=0;s<samples.length;s++){
	    if ( samples[s].getAttribute('name') == sampleName ){
		delta_t=samples[s].getAttribute('delta_t');
		var counts=samples[s].getElementsByTagName('count');
		for (c=0;c<counts.length;c++){
		    var td = document.getElementById( 'td_'+counts[c].getAttribute('chamber') );
		    //str += td.innerHTML +' :  '+ counts[c].getAttribute(counterName) + ' :  '+ counts[c].getAttribute('chamber') + '\n';
		    td.innerHTML = counts[c].getAttribute(counterName);
		}
	    }
	}
	dateTime = xmlDoc.getElementsByTagName('emuCounters')[0].getAttribute('dateTime').replace('T',String.fromCharCode(160)).substring(0,19);
	document.getElementsByTagName('title')[0].innerHTML = 'Emu counts at '+dateTime;
	document.getElementById('sampleTime').innerHTML = dateTime;
	document.getElementById('sampleDeltaT').innerHTML = delta_t;
	//alert(str);
    }catch(err){ 
      //alert( err );
    }
}

function getSelectedRadio( name ){
    var radioButtons = document.getElementsByName(name);
    for (r=0;r<radioButtons.length;r++){
	if ( radioButtons[r].checked ){
	    return radioButtons[r].getAttribute('value');
	}
    }
}

function getSelectedListItem( id ){
    var list = document.getElementById(id);
    return list.options[list.selectedIndex].value;
}


////////////////////
// END LOADING PART
////////////////////

////////////////////////
// START RESCALING PART
////////////////////////

var Chambers; // polygons
var Counts;   // count table's cells
var Colors;   // palette table's cells

function attachListeners(){
    Counts = document.getElementsByName("td_count");
    Colors = document.getElementById('paletteTable').getElementsByTagName("td");
    ReadoutFailedColor = Colors.item(0).style.backgroundColor;
    // Attach listeners
    Chambers = document.getElementsByTagName("svg:polygon"); // Firefox 3
    if ( Chambers.length == 0 ) Chambers = document.getElementsByTagName("polygon"); // Firefox 2
    for (i=0;i<Chambers.length;i++){
	if ( Chambers.item(i).hasAttribute('name') ){
	    Chambers.item(i).addEventListener("mouseover", onMouseOverChamber, false);
	    Chambers.item(i).addEventListener("mouseout" , onMouseOutChamber,  false);
	}
    }
    for (i=0;i<Counts.length;i++){
	Counts.item(i).addEventListener("mouseover", onMouseOverCell, false);
	Counts.item(i).addEventListener("mouseout" , onMouseOutCell,  false);
    }
}

function onScale(evt){
    var rescaleFactor = 2;
    var maxPalette = Number(Colors.item(Colors.length-1).innerHTML);
    var t = evt.target;
    if ( t.getAttribute('id') == 'scaleUp' ){
	maxPalette = Math.ceil( maxPalette*rescaleFactor );
    }
    else{
	maxPalette = Math.ceil( maxPalette/rescaleFactor );
    }
    scalePaletteTo( maxPalette );
    reColor();
}

function onLogScale(){
    scalePaletteTo( Colors.item( Colors.length-1 ).innerHTML ); // keep max
    reColor();
}

function onAutoScale(evt){
    var t = evt.target;
    document.getElementById("scaleDown").disabled=t.checked;
    document.getElementById("scaleUp").disabled=t.checked;
    scale();
}

function scale(){
    if ( document.getElementById("autoScale").checked ){
	document.getElementById("scaleDown").disabled=true;
	document.getElementById("scaleUp").disabled=true;
	var max = getMaxCount();
	scalePaletteTo( max );
    }
    reColor();
}

function scalePaletteTo( count ){
    var maxVal = 1;
    if ( count > 0 ) maxVal = count;
    if ( document.getElementById("logScale").checked ){
	var factor = Math.pow(maxVal,1./(Colors.length-3));
	for (i=2;i<Colors.length;i++){
	    Colors.item(i).innerHTML = Math.ceil( Math.pow(factor,i-2) );
	}
    }
    else{
	var increment = (maxVal-1)/(Colors.length-3);
	for (i=2;i<Colors.length;i++){
	    Colors.item(i).innerHTML = Math.ceil( 1+(i-2)*increment ) ;
	}
    }
}

function getMaxCount(){
    var maxCount = 0;
    for (i=0;i<Counts.length;i++){
	if ( maxCount - Counts.item(i).innerHTML < 0 ){ maxCount = Counts.item(i).innerHTML; }
    }
    //alert(maxCount);
    return maxCount;
}

function reColor(){
    //alert('reColor '+Counts.length);
    var str='';
    for (i=0;i<Counts.length;i++){
    //for (i=0;i<3;i++){
        var bgcolor = '#000000';
        var color = ''; 
	if ( Number(Counts.item(i).innerHTML) < 0 ){
	  bgcolor = Colors.item(0).style.backgroundColor;
	  color = Colors.item(0).style.backgroundColor; // readout failed
	} else {
	  if ( Number(Counts.item(i).innerHTML) > 0 ){
	    bgcolor = findBackgroundColorFor( Counts.item(i).innerHTML );
	  }
	}
        Counts.item(i).style.backgroundColor = bgcolor;
        Counts.item(i).style.color = color;
	var chamberName=Counts.item(i).id.substr(3);
	//alert(chamberName+' '+color+' '+document.getElementById(chamberName));
	//str += i+' '+chamberName+' '+color+' '+color.substring(18,color.length-1)+' '+document.getElementById(chamberName)+' '+Counts.item(i).nodeName+'\n';
	var ch=document.getElementById(chamberName);
	if ( ch ){
	  ch.setAttribute('fill', bgcolor);
	  ch.setAttribute('name', Counts.item(i).innerHTML);
	}
    }
    //alert(str);
}

function findBackgroundColorFor( value ){
    for (j=Colors.length-1;j>=0;j--){
	if ( Number(value) >= Number(Colors.item(j).innerHTML) ){
	    return Colors.item(j).style.backgroundColor;
	}
    }
    return '#0000ff';
}


function onMouseOverChamber(evt){
    highlightChamber( evt.target );
}

function onMouseOutChamber(evt){
    dullChamber( evt.target );
}

function highlightChamber( chamber ){
    chamber.setAttribute('opacity','1.');
    var label = document.getElementById( 'label_'+chamber.id );
    var labelColor = '#0000ff';
    if ( chamber.getAttribute('fill') == Colors.item(0).style.backgroundColor ) labelColor = '#ffff00';
    label.setAttribute('fill',labelColor);
    label.setAttribute('font-size','80');

    // Display focused chamber label
    document.getElementById('focusedChamberBox').setAttribute('visibility','visible');
    var t = document.getElementById('focusedChamberText')
    t.setAttribute('visibility','visible');
    var chamber_name = chamber.getAttribute('id');
    //t.firstChild.nodeValue = chamber_name.replace('-','&#8211;');// not translated in CDATA section
    t.firstChild.nodeValue = chamber_name.replace('-',String.fromCharCode(0x2013)); // quote en dash literally

    // Display count label
    var b = document.getElementById('countLabelBox');
    b.setAttribute('visibility','visible');
    b.setAttribute('fill',chamber.getAttribute('fill'));
    var t = document.getElementById('countLabelText')
    t.setAttribute('visibility','visible');
    t.setAttribute('fill',labelColor);
    var c = chamber.getAttribute('name');
    if ( c == '-1' ){ t.firstChild.nodeValue = 'no access'; }
    else            { t.firstChild.nodeValue = c; }
}

function dullChamber( chamber ){
    chamber.setAttribute('opacity','0.8');
    var label = document.getElementById( 'label_'+chamber.id );
    label.setAttribute('fill','#777777');
    label.setAttribute('font-size','60');
    // Hide focused chamber label
    document.getElementById('focusedChamberBox' ).setAttribute('visibility','hidden');
    document.getElementById('focusedChamberText').setAttribute('visibility','hidden');
    // Hide count label
    document.getElementById('countLabelBox' ).setAttribute('visibility','hidden');
    document.getElementById('countLabelText').setAttribute('visibility','hidden');
}

function onMouseOverCell(evt){
    var cell=evt.target;
    var chamber_name=cell.id.substr(3);
    var chamber=document.getElementById(chamber_name);
//     alert(cell+' '+chamber_name+' '+chamber);
    highlightChamber( chamber );
    tooltip(evt,'mapping_'+chamber_name);
}

function onMouseOutCell(evt){
    var cell=evt.target;
    var chamber_name=cell.id.substr(3);
    var chamber=document.getElementById(chamber_name);
    dullChamber( chamber );
    tooltip(evt,'mapping_'+chamber_name);
}

//////////////////////
// END RESCALING PART
//////////////////////
