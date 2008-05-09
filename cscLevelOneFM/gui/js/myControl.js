// globals
var myAnim=null;
var _blackboardSelected;


// takes an element
function setMyParameter(formName,type,textElement)
{
    // set the new value in the form element and update the
    // FM.
	document.FMPilotForm.globalParameterName4.value=formName;
	document.FMPilotForm.globalParameterValue4.value=textElement.value;
	document.FMPilotForm.globalParameterType4.value=type;
	
	submitForm('FMPilotForm', 'SET_GLOBAL_PARAMETERS');   
}

 

// takes a value
function setMyParameterValue(formName,type,value)
{
    // set the new value in the form element and update the
    // FM.
	document.FMPilotForm.globalParameterName3.value=formName;
	document.FMPilotForm.globalParameterValue3.value=value;
	document.FMPilotForm.globalParameterType3.value=type;
	
	submitForm('FMPilotForm', 'SET_GLOBAL_PARAMETERS');   
}


 
/* update global parameters */
function myUpdateParameters(message) {

	var pArray = message.getElementsByTagName('PARAMETER');
	var pDebug = document.getElementById('DEBUG_MSG');

    var debugString = '';

	var pObject = null;
	if (pArray != null) {
			
		for (i = 0; i < pArray.length; i++) {
				
			if (pArray[i] != null) {
				pNameNode = pArray[i].getElementsByTagName('NAME')[0];
				pValueNode = pArray[i].getElementsByTagName('VALUE')[0];
				if (pNameNode != null && pValueNode != null) {
					pName = pNameNode.childNodes[0].nodeValue;

					if ( pValueNode.childNodes[0] != null) {
						pValue = pValueNode.childNodes[0].nodeValue;

						var pObject = document.getElementById(pName);
						// identification can be only by id...
						// arbitrary atributes don't work in mozilla...
						//
						if ( pName == "HEARTBEAT" ) { 
							doAnim(); 
						}
						else if (pObject != null) {
								pObject.innerHTML = pValue;							
							if ( pName == "STATUS" ) { 
								pObject.className = pValue; 
							}
						}
							  
					}
				}
			}
		}
	}
}

function myInit()
{
	_blackboardSelected=false;
	 onLoad(); 
	 var onloadSound = document.getElementById('onloadSound'); 
	 if (onloadSound != null) {
		playSound(onloadSound.value);
	 }
}

function initAnim() {
	myAnim = new YAHOO.util.ColorAnim('HEARTBEAT', {backgroundColor: { from: '#11aa00' , to: '#ffffff' } }); 
}
 
function doAnim() { 
  if (myAnim==null) initAnim();
  myAnim.animate();
}

function nothing() {
 	// do nothing
 	return;
}


		
