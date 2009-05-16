/*****************************************************************************\
* $Id: common.js,v 1.1 2009/05/16 18:53:09 paste Exp $
\*****************************************************************************/

var stop = false;
var timeSinceReload = new Object;
var reloadSecs = 10;
var reloadError = 60;

Event.observe(window, "load", function(event) {
	
	// begin auto-refreshing
	getStatus(null);
	new PeriodicalExecuter(getStatus, reloadSecs);
	new PeriodicalExecuter(tickSeconds, 1);
	
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
	stop = !confirm("The following error has occurred: " + transport.statusText + ".  An expert should be contacted.\nClick \"OK\" to continue automatic refreshing of the application status in spite of this error.  Click \"Cancel\" to stop automatic refreshing.");
}

function updateTimes() {
	reloadElements.each(function(str) {
		var updateElement = $(str + "_loadtime");
		if (timeSinceReload[str] > reloadError) {
			if (!updateElement.hasClassName("Failed")) {
				updateElement.addClassName("Failed");
			}
		} else if (updateElement.hasClassName("Failed")) updateElement.removeClassName("Failed");
		var min = parseInt(timeSinceReload[str] / 60).toString();
		var sec = (timeSinceReload[str] % 60).toString();
		if (sec.length == 1) sec = "0" + sec;
		var time = min + ":" + sec;
		updateElement.update(time);
	});
}

function tickSeconds(pe) {
	// Update times since reload
	reloadElements.each(function(str) {
		timeSinceReload[str]++;
	});
	updateTimes();
}

function getStatus(pe) {
	if (stop) {
		return;
	}

	stop = true;

	reloadElements.each(function(str) {
		$(str + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
	});

	var url = URL + "/GetStatus";
	new Ajax.Request(url, {
		method: "get",
		onSuccess: updateStates,
		onFailure: reportError
	});
}

function doCommand(command) {
	stop = true;

	// Set loading icons
	reloadElements.each(function(str) {
		$(str + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
	});

	var url = URL + "/Fire";
	var params = {"action": command, "ajax": 1};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: finishCommand,
		onFailure: reportError
	});
}

function finishCommand(transport) {
	var data = transport.responseJSON;
	stop = false;
	getStatus();
}