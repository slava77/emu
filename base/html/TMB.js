function TMBPanel( dataURLs ) {
    var self = this; // This will be needed in member methods that work as callback functions.

    this.dataURL = dataURLs;
    this.xmlDoc = { minus:null, plus:null };
    this.timer = null;

    this.counts   = null;
    this.colors   = null;
    this.chambers = null;

    this.counterSelector  = null;
    this.slidingSample    = null;
    this.cumulativeSample = null;
    this.countUnits       = null;
    this.densityUnits     = null;
    this.autoRefresh      = null;
    this.refreshNow       = null;
    this.autoScale        = null;
    this.scaleDown        = null;
    this.scaleUp          = null;
    this.logScale         = null;


    this.getXML = function(){
	$.get( self.dataURL.minus, {}, function(xml){ self.xmlDoc.minus = xml; } );
	$.get( self.dataURL.plus , {}, function(xml){ self.xmlDoc.plus  = xml; } );
    };

    this.chamberArea = function( chamberName ){
	// See Muon TDR, p145
	var station = chamberName.substr(3,1);
	var ring    = chamberName.substr(5,1);
	if ( station == '1' ){
	    if ( ring == '1' ) return 0.51772; // 1.505 * ( 0.201 + 0.487 ) / 2 m^2
	    if ( ring == '2' ) return 1.08727; // 1.635 * ( 0.511 + 0.819 ) / 2 m^2
	    if ( ring == '3' ) return 1.35590; // 1.735 * ( 0.630 + 0.933 ) / 2 m^2
	    return 0.; // should never happen
	}
	
	if ( ring == '2' ) return 3.11212; // 3.215 * ( 0.666 + 1.270 ) / 2 m^2
	
	if ( station == '2' ) return 1.69860; // 1.900 * ( 0.534 + 1.254 ) / 2 m^2
	if ( station == '3' ) return 1.57164; // 1.680 * ( 0.617 + 1.254 ) / 2 m^2
	if ( station == '4' ) return 1.45425; // 1.500 * ( 0.685 + 1.254 ) / 2 m^2
	return 0.; // should never happen
    }
    
    this.zeroPaddedChamberName = function( chamberName ){
	if ( chamberName.length == 9 ) return chamberName;
	// console.log( chamberName.substr(0,7)+'0'+chamberName.substr(7) );
	return chamberName.substr(0,7)+'0'+chamberName.substr(7);
    }

    this.onSampleOrCounterSwitch = function(){
	self.valuesFromXmlToTable();
	self.scale();
    }

    this.onScale = function(evt){
	var rescaleFactor = 2;
	var maxPalette = Number(self.colors.item(self.colors.length-1).innerHTML);
	var t = evt.target;
	if ( t.getAttribute('id') == 'scaleUp' ){
	    maxPalette = Math.ceil( maxPalette*rescaleFactor );
	}
	else{
	    maxPalette = Math.ceil( maxPalette/rescaleFactor );
	}
	self.scalePaletteTo( maxPalette );
	self.reColor();
    }

    this.onLogScale = function(){
	self.scalePaletteTo( self.colors.item( self.colors.length-1 ).innerHTML ); // keep max
	self.reColor();
    }

    this.onAutoScale = function(evt){
	var t = evt.target;
	document.getElementById("scaleDown").disabled=t.checked;
	document.getElementById("scaleUp").disabled=t.checked;
	self.scale();
    }

    this.scale = function(){
	if ( document.getElementById("autoScale").checked ){
	    document.getElementById("scaleDown").disabled=true;
	    document.getElementById("scaleUp").disabled=true;
	    var max = self.getMaxCount();
	    self.scalePaletteTo( max );
	}
	self.reColor();
    }

    this.scalePaletteTo = function( count ){
	var maxVal = 1;
	if ( count > 0 ) maxVal = count;
	if ( document.getElementById("logScale").checked ){
	    var factor = Math.pow(maxVal,1./(self.colors.length-3));
	    for (i=2;i<self.colors.length;i++){
		self.colors.item(i).innerHTML = Math.ceil( Math.pow(factor,i-2) );
	    }
	}
	else{
	    var increment = (maxVal-1)/(self.colors.length-3);
	    for (i=2;i<self.colors.length;i++){
		self.colors.item(i).innerHTML = Math.ceil( 1+(i-2)*increment ) ;
	    }
	}
    }

    this.getMaxCount = function(){
	var maxCount = 0;
	for (i=0;i<self.counts.length;i++){
	    if ( maxCount - self.counts.item(i).innerHTML < 0 ){ maxCount = self.counts.item(i).innerHTML; }
	}
	//alert(maxCount);
	return maxCount;
    }

    this.reColor = function(){
	//alert('reColor '+self.counts.length);
	var str='';
	for (i=0;i<self.counts.length;i++){
	    //for (i=0;i<3;i++){
            var bgcolor = '#000000';
            var color = ''; 
	    if ( Number(self.counts.item(i).innerHTML) < 0 ){
		bgcolor = self.colors.item(0).style.backgroundColor;
		color = self.colors.item(0).style.backgroundColor; // readout failed
	    } else {
		if ( Number(self.counts.item(i).innerHTML) > 0 ){
		    bgcolor = self.findBackgroundColorFor( self.counts.item(i).innerHTML );
		}
	    }
            self.counts.item(i).style.backgroundColor = bgcolor;
            self.counts.item(i).style.color = color;
	    var chamberName=self.counts.item(i).id.substr(3);
	    //alert(chamberName+' '+color+' '+document.getElementById(chamberName));
	    //str += i+' '+chamberName+' '+color+' '+color.substring(18,color.length-1)+' '+document.getElementById(chamberName)+' '+self.counts.item(i).nodeName+'\n';
	    var ch=document.getElementById(chamberName);
	    if ( ch ){
		ch.setAttribute('fill', bgcolor);
		ch.setAttribute('name', self.counts.item(i).innerHTML);
	    }
	}
	//alert(str);
    }

    this.findBackgroundColorFor = function( value ){
	for (j=self.colors.length-1;j>=0;j--){
	    if ( Number(value) >= Number(self.colors.item(j).innerHTML) ){
		return self.colors.item(j).style.backgroundColor;
	    }
	}
	return '#0000ff';
    }


    this.onMouseOverChamber = function(evt){
	self.highlightChamber( evt.target );
    }
    
    this.onMouseOutChamber = function(evt){
	self.dullChamber( evt.target );
    }
    
    this.highlightChambersInSameRUI = function( chamber ){
	var allChambers = document.getElementById('div_RUIs').getElementsByTagName('div');
	for ( var i=0; i < allChambers.length; i++ ){
	    if ( allChambers[i].getAttribute('id').substr(4) == chamber.id ){
		// This is the chamber we're looking for. Loop over its siblings in the list:
		var siblingChambers = allChambers[i].parentNode.getElementsByTagName('div');
		for ( var j=0; j < siblingChambers.length; j++ ){
		    // Paint this chamber's frame yellow, its siblings' orangered:
		    var chamberId = siblingChambers[j].getAttribute('id').substr(4);		    
		    document.getElementById( chamberId ).setAttribute('stroke',(chamberId == chamber.id ? '#ffff00' : 'OrangeRed'));
		}
		document.getElementById('focusedRUIBox').setAttribute('visibility','visible');
		var t = document.getElementById('focusedRUIText');
		// console.log(allChambers[i].parentNode.getAttribute('id').substr(7));
		t.firstChild.nodeValue = 'RUI '+allChambers[i].parentNode.getAttribute('id').substr(7);
		t.setAttribute('visibility','visible');		
		break;
	    }
	}
    }

    this.dullChambersInSameRUI = function( chamber ){
	var allChambers = document.getElementById('div_RUIs').getElementsByTagName('div');
	for ( var i=0; i < allChambers.length; i++ ){
	    if ( allChambers[i].getAttribute('id').substr(4) == chamber.id ){
		// This is the chamber we're looking for. Loop over its siblings in the list:
		var siblingChambers = allChambers[i].parentNode.getElementsByTagName('div');
		for ( var j=0; j < siblingChambers.length; j++ )
			document.getElementById( siblingChambers[j].getAttribute('id').substr(4) ).setAttribute('stroke','');
		document.getElementById('focusedRUIText').setAttribute('visibility','hidden');
		document.getElementById('focusedRUIBox' ).setAttribute('visibility','hidden');
		break;
	    }
	}
    }

    this.highlightChambersInSameVME = function( chamber ){
	var DMBSlotToNumber = { 'slot3':1, 'slot5':2, 'slot7':3, 'slot9':4, 'slot11':5, 'slot15':6, 'slot17':7, 'slot19':8, 'slot21':9 };
	var allChambers = document.getElementById('div_VMEs').getElementsByTagName('div');
	for ( var i=0; i < allChambers.length; i++ ){
	    if ( allChambers[i].getAttribute('id').substr(4) == chamber.id ){
		// This is the chamber we're looking for. Loop over its siblings in the list:
		var siblingChambers = allChambers[i].parentNode.getElementsByTagName('div');
		for ( var j=0; j < siblingChambers.length; j++ )
		    document.getElementById( 'label_'+siblingChambers[j].getAttribute('id').substr(4) ).setAttribute('fill','#00ff00');
		document.getElementById('focusedVMEBox').setAttribute('visibility','visible');
		var t = document.getElementById('focusedVMEText');
		//console.log(allChambers[i].parentNode.getAttribute('id').substr(6));
		// t.firstChild.nodeValue = 'VME '+allChambers[i].parentNode.getAttribute('id').substr(7);
		t.firstChild.nodeValue = allChambers[i].parentNode.getAttribute('VMEName')+' ['+allChambers[i].parentNode.getAttribute('id').substr(7)+']';
		t.setAttribute('visibility','visible');		
		document.getElementById('focusedDMBBox').setAttribute('visibility','visible');
		var t = document.getElementById('focusedDMBText');
		//console.log(allChambers[i].parentNode.getAttribute('id').substr(6));
		t.firstChild.nodeValue = 'TMB/DMB '+DMBSlotToNumber[ allChambers[i].textContent ];
		t.setAttribute('visibility','visible');		
		break;
	    }
	}
    }

    this.dullChambersInSameVME = function( chamber ){
	var allChambers = document.getElementById('div_VMEs').getElementsByTagName('div');
	for ( var i=0; i < allChambers.length; i++ ){
	    if ( allChambers[i].getAttribute('id').substr(4) == chamber.id ){
		// This is the chamber we're looking for. Loop over its siblings in the list:
		var siblingChambers = allChambers[i].parentNode.getElementsByTagName('div');
		for ( var j=0; j < siblingChambers.length; j++ )
		    document.getElementById( 'label_'+siblingChambers[j].getAttribute('id').substr(4) ).setAttribute('fill','#777777');
		document.getElementById('focusedVMEText').setAttribute('visibility','hidden');
		document.getElementById('focusedVMEBox' ).setAttribute('visibility','hidden');
		document.getElementById('focusedDMBText').setAttribute('visibility','hidden');
		document.getElementById('focusedDMBBox' ).setAttribute('visibility','hidden');
		break;
	    }
	}
    }

    this.highlightChamber = function( chamber ){
	self.highlightChambersInSameRUI( chamber );
	self.highlightChambersInSameVME( chamber );
	chamber.setAttribute('opacity','1');
	var label = document.getElementById( 'label_'+chamber.id );
	var labelColor = '#ffff00';
	label.setAttribute('fill','#ffff00');
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
	var c = chamber.getAttribute('name');
	if ( c == '-1' ){ t.firstChild.nodeValue = 'no access'; }
	else            { t.firstChild.nodeValue = c; }

	// Display Sector Processor label
	var b = document.getElementById('sectorProcessorLabelBox');
	b.setAttribute('visibility','visible');
	var t = document.getElementById('sectorProcessorLabelText')
	t.setAttribute('visibility','visible');
 	var allChambers = document.getElementById('div_SPs').getElementsByTagName('div');
	for ( var i=0; i < allChambers.length; i++ ){
	  if ( allChambers[i].getAttribute('id').substr(4) == chamber.id ){
	    // This is the chamber we're looking for. Get its parent's id, which is the Sector Processor number.
	    t.firstChild.nodeValue = 'SP '+allChambers[i].parentNode.getAttribute('id').substr(6); // e.g.: div_SP11
	    break;
	  }
	}
   }
    
    this.dullChamber = function( chamber ){
	self.dullChambersInSameRUI( chamber );
	self.dullChambersInSameVME( chamber );
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
	// Hide Sector Processor label
	document.getElementById('sectorProcessorLabelBox' ).setAttribute('visibility','hidden');
	document.getElementById('sectorProcessorLabelText').setAttribute('visibility','hidden');
    }
    
    this.onMouseOverCell = function(evt){
	var cell=evt.target;
	var chamber_name=cell.id.substr(3);
	var chamber=document.getElementById(chamber_name);
	//console.log(cell+' '+chamber_name+' '+chamber);
	self.highlightChamber( chamber );
	tooltip(evt,'mapping_'+chamber_name);
    }
    
    this.onMouseOutCell = function(evt){
	var cell=evt.target;
	var chamber_name=cell.id.substr(3);
	var chamber=document.getElementById(chamber_name);
	self.dullChamber( chamber );
	tooltip(evt,'mapping_'+chamber_name);
    }
    
    this.attachListeners = function(){
	var paletteTable      = document.getElementById('paletteTable');
	var countsTableParent = document.getElementById('countsTableParent');
	// Do this only once the page has been created by XSLT
	if ( paletteTable && countsTableParent ){

	    this.counts = countsTableParent.getElementsByTagName("td");
	    this.colors = paletteTable.getElementsByTagName("td");
	    ReadoutFailedColor = this.colors.item(0).style.backgroundColor;
	    // Attach listeners
	    this.chambers = document.getElementsByTagName("svg:polygon"); // Firefox 3
	    if ( this.chambers.length == 0 ) this.chambers = document.getElementsByTagName("polygon"); // Firefox 2
	    for (i=0;i<this.chambers.length;i++){
		if ( this.chambers.item(i).id.substr(0,2) == 'ME' ){
		    this.chambers.item(i).addEventListener("mouseover", this.onMouseOverChamber, false);
		    this.chambers.item(i).addEventListener("mouseout" , this.onMouseOutChamber,  false);
		}
	    }
	    for (i=0;i<this.counts.length;i++){
		this.counts.item(i).addEventListener("mouseover", this.onMouseOverCell, false);
		this.counts.item(i).addEventListener("mouseout" , this.onMouseOutCell,  false);
	    }

	    this.counterSelector  = document.getElementById('counterSelector' );
	    this.slidingSample    = document.getElementById('slidingSample'   );
	    this.cumulativeSample = document.getElementById('cumulativeSample');
	    this.countUnits       = document.getElementById('countUnits'      );
	    this.densityUnits     = document.getElementById('densityUnits'    );
	    this.autoRefresh      = document.getElementById('autoRefresh'     );
	    this.refreshPeriod    = document.getElementById('refreshPeriod'   );
	    this.refreshNow       = document.getElementById('refreshNow'      );
	    this.autoScale        = document.getElementById('autoScale'       );
	    this.scaleDown        = document.getElementById('scaleDown'       );
	    this.scaleUp          = document.getElementById('scaleUp'         );
	    this.logScale         = document.getElementById('logScale'        );

	    // Attachment of onclick callback doesn't work in Firefox 10.0.7 (but mouseout does...).
	    // this.counterSelector .addEventListener('onchange', this.onSampleOrCounterSwitch, false);
	    // this.slidingSample   .addEventListener('onclick',  this.onSampleOrCounterSwitch, false);
	    // this.cumulativeSample.addEventListener('onclick',  this.onSampleOrCounterSwitch, false);
	    // this.countUnits      .addEventListener('onclick',  this.onSampleOrCounterSwitch, false);
	    // this.densityUnits    .addEventListener('onclick',  this.onSampleOrCounterSwitch, false);
	    // this.autoRefresh     .addEventListener('onclick',  this.autoLoadData,       false);
	    // this.refreshNow      .addEventListener('onclick',  this.loadData,           false);
	    // this.autoScale       .addEventListener('onclick',  this.onAutoScale,             false);
	    // this.scaleDown       .addEventListener('onclick',  this.onScale,                 false);
	    // this.scaleUp         .addEventListener('onclick',  this.onScale,                 false);
	    // this.logScale        .addEventListener('onclick',  this.onLogScale,              false);
	}
    }

    this.sumOverRingsChambers = function(){
	var unitName=$('input:radio[name=unitSelector]:checked').val();
	var sum_endcap = { minus:0, plus:0 };
	var countsTableParent = document.getElementById('countsTableParent');
	if ( countsTableParent ){
	    var th = countsTableParent.getElementsByTagName('th');
	    for (var ith=0;ith<th.length;ith++){
		if ( th[ith].id.substr(0,9) == 'sum_ring_' ){
		    var sum_ring = 0;
		    var td = th[ith].parentNode.getElementsByTagName('td'); // td cells in the same row
		    for ( var itd=0; itd<td.length; itd++ ) sum_ring += Math.max(0,Number(td[itd].innerHTML)); // Exclude negative values (no access) from sum.
		    th[ith].innerHTML = ( unitName == 'density' ? ( sum_ring / td.length ).toPrecision(2) : sum_ring );
		    if      ( th[ith].id.substr(11,1) == '-' ) sum_endcap.minus += sum_ring;
		    else if ( th[ith].id.substr(11,1) == '+' ) sum_endcap.plus  += sum_ring;
		}
	    }
	    document.getElementById('sum_endcap_minus').innerHTML = ( unitName == 'density' ? '' : sum_endcap.minus );
	    document.getElementById('sum_endcap_plus' ).innerHTML = ( unitName == 'density' ? '' : sum_endcap.plus  );
	}
    }

    this.valuesFromXmlToTable = function(){
	var selectedUnit   = $('input:radio[name=unitSelector]:checked').val();
	var selectedSample = $('input:radio[name=sampleSelector]:checked').val();
	//console.log( selectedUnit+' '+selectedSample );
	$.each( this.xmlDoc, function(side,xml){
	    //if (xml) console.log( (new XMLSerializer()).serializeToString( xml ) );
	    // var th_sum_endcap = document.getElementById( 'sum_endcap_'+side );
	    // th_sum_endcap.innerHTML = 0;
	    $('emuCounters',xml).each( function(){ 
		$('#sampleTime').text( $(this).attr('dateTime').replace('T',String.fromCharCode(160)).substring(0,19) );
		$('sample',this).each( function(){
		    if ( $(this).attr('name') == selectedSample ){
			$('#sampleDeltaT').text( $(this).attr('delta_t') );
			$('count',this).each( function(){
			    // jQuery is confused by the - and / in the chamber name: 
			    // $('#td_'+$(this).attr('chamber')).text( $(this).attr($("#counterSelector").val()));
			    // Use plain old selector method instead:
			    var td_chamber = document.getElementById( 'td_'+self.zeroPaddedChamberName( $(this).attr('chamber') ) );
			    // var th_sum_ring = document.getElementById( 'sum_ring_'+$(this).attr('chamber').substr(0,6) );
			    if ( td_chamber ){
				if ( selectedUnit == 'density' )
				    td_chamber.innerHTML = ( $(this).attr($("#counterSelector").val()) / self.chamberArea( $(this).attr('chamber') ) ).toPrecision(2);
				else
				    td_chamber.innerHTML = $(this).attr($("#counterSelector").val());
			    }
			});
		    }
		});
	    });
	});
	this.sumOverRingsChambers();
    };

    this.loadData = function(){
	self.getXML();
	// console.log( 'XML +: '+self.xmlDoc.plus +' XML -: '+self.xmlDoc.minus );
	self.valuesFromXmlToTable();
	self.scale();
	// if ( self.xmlDoc.minus ) console.log( (new XMLSerializer()).serializeToString( self.xmlDoc.minus ) );
    }

    this.autoReloadData = function (){
	clearTimeout( self.timer );
	if ( self.refreshPeriod ){
	    // console.log( 'self.refreshPeriod.value '+self.refreshPeriod.value +' self.autoRefresh.checked '+self.autoRefresh.checked );
	    if ( self.refreshPeriod.value != 0 && self.autoRefresh.checked ){
		self.loadData();
	    }
	}
	self.timer = setTimeout( function(){ self.autoReloadData(); }, self.refreshPeriod.value * 1000 );
    };

    this.attachListeners();
    this.loadData();
    this.timer = setTimeout( 
    	function(){
    	    self.autoReloadData(); 
    	}, 
    	3000 ); // Wait 3s before the first update to allow data XMLs to be loaded.
    // this.autoReloadData();
}
