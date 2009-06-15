/*****************************************************************************\
* $Id: manager.js,v 1.3 2009/06/15 17:25:44 paste Exp $
\*****************************************************************************/

var reloadElements = ["FED_System_Status", "FED_Communicator_Status"];

Event.observe(window, "load", function(event) {
	
	// enable buttons with check-box
	$("enable_buttons").observe("change", function(ev) {
		var element = ev.element();
		if (element.checked && confirm("Enabling manual state changes will cause the FED system to ignore any SOAP messages it receives from the CSCSupervisor.  If the FED system is involved in a global run, this will result in the run crashing.\n\nClick \"OK\" if you are certain you want to ignore SOAP messages.  Otherwise, click \"Cancel\".")) {
			ignoreSOAP(true);
		} else if (element.checked) {
			element.checked = false;
		} else {
			ignoreSOAP(false);
		}
	});

	// put button fuctions in place
	$$("button.statechange").each(function(element) {
		if ($("enable_buttons").checked) {
			element.show();
		} else {
			element.hide();
		}
		element.observe("click", function(ev) {
			$$("button.statechange").each(function(el) { el.disabled = true; });
			doCommand(ev.element().readAttribute("command"));
		});
	});
	
	testForPage1();
});
/*
var errors = 0;
var trial = 0;

function testForPage1() {
	
	if (errors >= 5) return;
	trial++;
	
	var url = URL + "/ForEmuPage1";
	new Ajax.Request(url, {
		method: "get",
		onSuccess: updateTestForPage1,
		onFailure: reportErrorPage1
	});
}

function reportErrorPage1(transport) {
	errors++;
	$("updateMe").update(new Element("div").update("Trial " + trial + ": Error " + errors + " " + transport.status + ": " + transport.statusText));
	testForPage1();
}

function updateTestForPage1(transport) {
	if (!transport.responseText || transport.responseText == "") reportErrorPage1(transport);
	errors = 0;
	$("updateMe").update(new Element("div").update("Trial " + trial + ": " + transport.status + ": " + transport.statusText));
	testForPage1();
}
*/


function updateStates(transport) {
	var data = transport.responseJSON;
	
	$$(".deleteme").invoke("remove");
	
	var state = data["state"] ? data["state"] : "Unknown";
	var classesToRemove = $("manager_state").classNames();
	classesToRemove.each(function(name) {
		$("manager_state").removeClassName(name);
	});
	$("manager_state").update(state).addClassName(state);
	
	if (state == "Unknown") {
		$("status_description").update("The Manager was unable to properly determine the states of the underlying Communicator applications.  If sending a Halt command does not fix this, call an expert.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-error.png");
		$("halt_button").disabled = false;
	} else if (state == "Failed") {
		$("status_description").update("Either one of the underlying Communicator applications is in the Failed state, or the Manager detected an error when attempting to transition between states.  If sending a Halt command does not fix this, call an expert.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-error.png");
		$("halt_button").disabled = false;
	} else if (state == "Indefinite") {
		$("status_description").update("The underlying Communicator applications are not in the same state.  This can be expected if the system is being debugged by an expert.  If sending a Halt command does not fix this, call an expert.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-warning.png");
		$("halt_button").disabled = false;
	} else if (state == "Halted") {
		$("status_description").update("The FED system is ready to be configured.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("configure_button").disabled = false;
	} else if (state == "Configured") {
		$("status_description").update("The FED system hardware has been configured and is ready to take data.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("configure_button").disabled = false;
		$("enable_button").disabled = false;
	} else if (state == "Enabled") {
		$("status_description").update("The FED system is monitoring data in real-time and will report errors to the CSC Hotspot.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("disable_button").disabled = false;
	}
	
	var communicators = data["communicators"];
	
	communicators.each(function(communicator) {
	
		var instance = communicator["instance"];
		
		if (!$("systemName" + instance)) {
			var cName = new Element("div", {"class": "category", "id": "systemName" + instance});
			var cTable = new Element("table", {"class": "noborder dialog tier1"});
			
			var cStateRow = new Element("tr");
			cStateRow.insert(new Element("td").update("State: "));
			cStateRow.insert(new Element("td").insert(new Element("span", {"id": "state" + instance})));
			cTable.insert(cStateRow);
			
			var cConfigRow = new Element("tr");
			cConfigRow.insert(new Element("td").update("Configuration mode: "));
			cConfigRow.insert(new Element("td").insert(new Element("span", {"id": "configMode" + instance})));
			cTable.insert(cConfigRow);
			
			var cSOAPRow = new Element("tr");
			cSOAPRow.insert(new Element("td").update("Ignore SOAP commands: "));
			cSOAPRow.insert(new Element("td").insert(new Element("span", {"id": "ignoreSOAP" + instance})));
			cTable.insert(cSOAPRow);
			
			var cAppRow = new Element("tr");
			cAppRow.insert(new Element("td").update("Communicator URL: "));
			cAppRow.insert(new Element("td").insert(new Element("a", {"id": "communicatorURL" + instance})));
			cTable.insert(cAppRow);
			
			var cCommRow = new Element("tr");
			cCommRow.insert(new Element("td").update("Commander URL: "));
			cCommRow.insert(new Element("td").insert(new Element("a", {"id": "commanderURL" + instance})));
			cTable.insert(cCommRow);
			
			var cMonRow = new Element("tr");
			cMonRow.insert(new Element("td").update("Monitor URL: "));
			cMonRow.insert(new Element("td").insert(new Element("a", {"id": "monitorURL" + instance})));
			cTable.insert(cMonRow);
			
			var insertHere = $("FED_Communicator_Status_dialog");
			insertHere.insert(cName);
			insertHere.insert(cTable);
		}
		
		$("systemName" + instance).update(communicator["systemName"] + " (instance " + instance + ")");
		
		var stateElement = $("state" + instance);
		var removeMe = stateElement.classNames();
		removeMe.each(function(name) {
			stateElement.removeClassName(name);
		});
		stateElement.update(communicator["state"]).addClassName(communicator["state"]);
		
		$("configMode" + instance).update(communicator["configMode"]);
		
		var ignoreSOAPElement = $("ignoreSOAP" + instance);
		ignoreSOAPElement.update(communicator["ignoreSOAP"]);
		if (communicator["ignoreSOAP"] == true) {
			if (!ignoreSOAPElement.hasClassName("error") && !ignoreSOAPElement.hasClassName("error_black")) {
				ignoreSOAPElement.addClassName("error");
			}
		} else {
			ignoreSOAPElement.removeClassName("error").removeClassName("error_black");
		}
		
		$("communicatorURL" + instance).update(communicator["url"]).setAttribute("href", communicator["url"]);
		
		$("commanderURL" + instance).update(communicator["commanderURL"] ? communicator["commanderURL"] : "none found").setAttribute("href", communicator["ommanderURL"]);
		
		$("monitorURL" + instance).update(communicator["monitorURL"] ? communicator["monitorURL"] : "none found").setAttribute("href", communicator["monitorURL"]);
		
	});
	
	// Unset loading icons
	reloadElements.each(function(str) {
		$(str + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
	});
	
	// Update times since reload
	reloadElements.each(function(str) {
		timeSinceReload[str] = 0;
	});
	updateTimes();
	
	stop = false;

}
