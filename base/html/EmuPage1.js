var XmlDoc = document.implementation.createDocument("", "", null);
XmlDoc.addEventListener("load", xmlDocLoaded, false);

var Clock     = null;
var Subsystem = null;
var XmlUrl    = null;

function onLoad(subsystem)
{
    Subsystem = subsystem;
    ageOfPageClock(0);
    XmlUrl = 'ForEmuPage1';
    autoReloadXmlDoc();
}

function autoReloadXmlDoc(){
    XmlDoc.load( XmlUrl );
    setTimeout('autoReloadXmlDoc()', 8000);
}

function xmlDocLoaded(e){
    //alert('xmlDocLoaded: event type '+e.type+' target '+e.target);
    var xmlIsOK = false;
    try{
      xmlIsOK = valuesFromXmlToTable();
    } catch(e) {
      //alert('Caught exception thrown from valuesFromXmlToTable: '+e.message);
      return;
    }
    if ( xmlIsOK ){
      clearTimeout(Clock);
      ageOfPageClock(0);
    }
}

function valuesFromXmlToTable(){
    var rootElement = XmlDoc.getElementsByTagName('ForEmuPage1');
    if ( !rootElement.length ) return false;
    document.getElementById('td_localDateTime').innerHTML = rootElement[0].getAttribute('localDateTime');
    var monitorables = XmlDoc.getElementsByTagName('monitorable');
    if ( !monitorables.length ) return false;
    for ( m=0; m<monitorables.length; m++ ){
        // name
	var th_name = document.getElementById( 'th_'+monitorables[m].getAttribute('name') );
	if ( th_name ) th_name.setAttribute( 'class', monitorables[m].getAttribute('name') );
	var a_name = document.getElementById( 'a_'+monitorables[m].getAttribute('name') );
	if ( a_name ){
	  a_name.setAttribute('title',monitorables[m].getAttribute('nameDescription'));
	  a_name.innerHTML = monitorables[m].getAttribute('name');
	}
        // value
	var td_value = document.getElementById( 'td_value_of_'+monitorables[m].getAttribute('name') );
	if ( td_value ) td_value.setAttribute( 'class', monitorables[m].getAttribute('value') );
	var a_value = document.getElementById( 'a_value_of_'+monitorables[m].getAttribute('name') );
	if ( a_value ){
	  a_value.setAttribute('title',monitorables[m].getAttribute('valueDescription'));
	  a_value.innerHTML = monitorables[m].getAttribute('value');
	}
    }
    return true;
}

function ageOfPageClock(ageOfPage){
    hours=Math.floor(ageOfPage/3600);
    minutes=Math.floor(ageOfPage/60)%60;
    age="";
    if (hours) age+=hours+"&nbsp;h&nbsp;";
    if (minutes) age+=minutes+"&nbsp;m&nbsp;";
    age+=ageOfPage%60+"&nbsp;s&nbsp;";
    document.getElementById('td_ageOfPage').innerHTML='Loaded&nbsp;'+age+'ago';
    var mainTableElem = document.getElementById('table_'+Subsystem);
    if      ( ageOfPage < 16 ) mainTableElem.setAttribute('class','fresh');
    else if ( ageOfPage < 60 ) mainTableElem.setAttribute('class','aging');
    else                       mainTableElem.setAttribute('class','stale');
    ageOfPage=ageOfPage+1;
    Clock = setTimeout('ageOfPageClock('+ageOfPage+')',1000);
}
