/*****************************************************************************\
* $Id: common.js,v 1.3 2009/11/06 13:48:34 paste Exp $
\*****************************************************************************/

Event.observe(window, "load", function(event) {
	
	// Begin auto-refreshing
	reloadElements.each(function(e) {
		e.tick(true);
		var reloadFunction = e.tick.bind(e);
		new PeriodicalExecuter(reloadFunction, 1);
	});

});

function ignoreSOAP(talf) {
	
	if (talf) {
		$$("button.statechange").invoke("show");
	} else {
		$$("button.statechange").invoke("hide");
	}

	var url = URL + "/IgnoreSOAP";
	var params = {"ignoreSOAP": (talf ? 1 : 0)};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		//onSuccess: noAction,
		onFailure: reportError
	});
}

function reportError(transport) {
	//this.stop = true;
	alert("The following error has occurred: " + transport.statusText + ".  An expert should be contacted.");
}

function reportErrorAndStop(transport) {
	this.stop = true;
	$(this.id + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
	alert("The following error has occurred: " + transport.statusText + ".  An expert should be contacted.\nAutomatic reload has now been disabled to avoid further errors.");
}

function doCommand(command, reloadElement) {
	// Bind the special callbacks
	var successCallback = reloadElement.callbackSuccess.bind(reloadElement);
	var errorCallback = reloadElement.callbackError.bind(reloadElement);

	// Set loading icon
	$(reloadElement.id + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");

	var url = URL + "/Fire";
	var params = {"action": command, "ajax": 1};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}
