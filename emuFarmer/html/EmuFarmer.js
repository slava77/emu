function appsVisible(e){
    // toggles visibility of application name list
    var targetId=e.target.id;
    var styleValue='display:none';
    if ( e.target.checked ) { styleValue=''; }
    var spans=document.getElementsByTagName('span');
    for (i=0; i<spans.length; i++){
	if ( spans.item(i).id.indexOf( targetId, 0 ) >= 0 ){
	    spans.item(i).setAttribute('style',styleValue);
	}
    }
}

function changeRowColor(){
    // changes color of the (un)checked row
    var inputs=document.getElementsByTagName('input');
    for (i=0; i<inputs.length; i++){
	if ( inputs.item(i).name.indexOf( ':', 0 ) > 0 ){
	    var tds = inputs.item(i).parentNode.parentNode.childNodes;
	    for (j=0; j<tds.length; j++){
		if ( tds.item(j).nodeName == 'TD' ){
		    var styleValue='';
		    if ( ! inputs.item(i).checked ){
// 			styleValue='background-color:transparent';
			styleValue='background-color:#8a7f58';
		    }
		    tds.item(j).setAttribute('style',styleValue);
		}
	    }
	}
    }
}

function multiCheck(e){
    // checks or unchecks all checkboxes in this table
    var targetId=e.target.id;
    var checkThem=false;
    if ( e.target.title.indexOf( 'all', 0 ) >= 0 ){
	checkThem=true;
	// make sure it's always checked
	e.target.checked=true;
    }
    else{
	// make sure it's never checked
	e.target.checked=false;
    }
    var inputs=document.getElementsByTagName('input');
    for (i=0; i<inputs.length; i++){
	if ( inputs.item(i).id == targetId && inputs.item(i).name.indexOf( ':', 0 ) >= 0 ){
	    inputs.item(i).checked=checkThem;
	}
    }
    changeRowColor();
}

function validateSelection(e){
    // create array of group names
    var groups=new Array();
    var processTables=document.getElementsByName("processTable");
    for (i=0; i<processTables.length; i++){
	groups.push( processTables[i].id );
    }
    // create a counter object containing a counter for each group
    counter = new Object();
    for (i=0; i<groups.length; i++){    
	eval('counter.'+groups[i]+'=0;');
    }
    // count checked process selector checkboxes for each group
    var inputs=document.getElementsByTagName("input");
    for (i=0; i<inputs.length; i++){
	if ( inputs.item(i).getAttribute('type')=='checkbox' &&
	     inputs.item(i).name.indexOf( ':', 0 )>0 &&
	     inputs.item(i).checked ){
	    counter[inputs.item(i).id]++;
	}
    }
    // prepare message
    var totalCount=0;
    var message='You are about to '+ e.target.getAttribute('value') + "\n";
    for (i=0; i<groups.length; i++){
	totalCount += counter[groups[i]];
	if ( counter[groups[i]] > 0 ) message += counter[groups[i]] + ' ' + groups[i] + '\n';
    }
    message += 'process';
    if ( totalCount > 1 ) message += 'es';
    message += '.';
    if ( totalCount == 0 ){
	alert('You have not selected any process.');
	return;
    }
    // prompt user to confirm
    if ( confirm( message ) )
    {
	// set the value of the action button to that of the button that was clicked on
	document.getElementById('action').value=e.target.getAttribute('value');
	e.target.form.submit();
    }
}

function validateConfigFileChange(e){
    // Find out whether there is already a config file used.
    var linkToMergedConfigFile = document.getElementById('mergedConfigFileURL');
    // If no config file is used yet, let the user go ahead and create one.
    if ( linkToMergedConfigFile.innerHTML.indexOf( 'xml', 0 ) < 0 ){
	document.getElementById('action').value='create config';
	e.target.form.submit();
	return;
    }
    // Apparently there is one. Check for job id's and warn the user about them.
    var runningProcessesCount = 0
    var tds = document.getElementsByTagName('td');
    for (i=0; i<tds.length; i++){
	if ( tds.item(i).id == 'jobId' && tds.item(i).innerHTML.indexOf( '-', 0 ) < 0 ){
	    runningProcessesCount++;
	}
    }
    if ( runningProcessesCount > 0 ){
	var message = 'You are about to recreate the configuration file for the Emu processes, but there seem';
	if ( runningProcessesCount == 1 ) message += 's';
	message += ' to be ' + runningProcessesCount;
	message += ' of them still running.\nIt is recommended that you stop';
	if ( runningProcessesCount == 1 ) message += ' it ';
	else                              message += ' them ';
	message += 'first.';
	if ( confirm( message ) ){
	    document.getElementById('action').value='create config';
	    e.target.form.submit();
	}
	return;
    }
    document.getElementById('action').value='create config';
    e.target.form.submit();
    return;
}
