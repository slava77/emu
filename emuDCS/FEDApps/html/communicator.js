/*****************************************************************************\
* $Id: communicator.js,v 1.3 2009/07/01 14:54:02 paste Exp $
\*****************************************************************************/

var defaultConfigurationMode = "XML";

Event.observe(window, "load", function(event) {
	
	var statusReloadElement = new ReloadElement();
	statusReloadElement.id = "FED_Communicator_Status";
	statusReloadElement.reloadFunction = getStatus;
	statusReloadElement.callbackSuccess = updateStatus;
	statusReloadElement.callbackError = reportErrorAndStop;
	statusReloadElement.timeToReload = 10;
	statusReloadElement.timeToError = 60;
	reloadElements.push(statusReloadElement);
	
	var configurationReloadElement = new ReloadElement();
	configurationReloadElement.id = "FED_Communicator_Configuration";
	configurationReloadElement.reloadFunction = getConfiguration;
	configurationReloadElement.callbackSuccess = updateConfiguration;
	configurationReloadElement.callbackError = reportErrorAndStop;
	configurationReloadElement.timeToReload = 10;
	configurationReloadElement.timeToError = 60;
	reloadElements.push(configurationReloadElement);
	
	// enable buttons with check-box
	$("enable_buttons").observe("change", function(ev) {
		var element = ev.element();
		if (element.checked && confirm("Enabling manual state changes will cause this FED Communicator application to ignore any SOAP messages it receives from the FED Manager.  If this FED Communicator application is involved in a global run, this will result in the run crashing.\n\nClick \"OK\" if you are certain you want to ignore SOAP messages.  Otherwise, click \"Cancel\".")) {
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
			doCommand(ev.element().readAttribute("command"), statusReloadElement);
		});
	});
	
	// Change configuration settings
	$$(".config_type").each(function(element) {
		element.observe("change", function(ev) {
			var el = ev.element();
			if (el.checked) {
				changeConfigMode(el.value, configurationReloadElement);
			}
		});
	});
	
	// Make the configuration file change automatically submit.
	$("xml_file_select").observe("change", function(ev) {
		changeXMLFile($("xml_file_select").value, configurationReloadElement);
	});
	
	// Make the DB key change automatically submit.
	$("db_key_select").observe("change", function(ev) {
		changeDBKey($("db_key_select").value, configurationReloadElement);
	});
	
});

function getStatus() {
	// Bind the special callbacks
	var successCallback = this.callbackSuccess.bind(this);
	var errorCallback = this.callbackError.bind(this);
	
	var url = URL + "/GetStatus";
	
	new Ajax.Request(url, {
		method: "get",
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}

function updateStatus(transport) {
	var data = transport.responseJSON;
	
	$$(".deleteme").invoke("remove");
	
	var state = data.state ? data.state : "Unknown";
	var classesToRemove = $("communicator_state").classNames();
	classesToRemove.each(function(name) {
		$("communicator_state").removeClassName(name);
	});
	$("communicator_state").update(state).addClassName(state);
	
	if (state == "Unknown") {
		$("status_description").update("The Communicator was unable to properly determine its own FSM state.  If sending a Halt command does not fix this, call an expert.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-error.png");
		$("halt_button").disabled = false;
	} else if (state == "Failed") {
		$("status_description").update("The Communicator detected an error when attempting to transition between states.  If sending a Halt command does not fix this, call an expert.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-error.png");
		$("halt_button").disabled = false;
	} else if (state == "Halted") {
		$("status_description").update("The FED crate hardware under command of this application is ready to be configured.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("configure_button").disabled = false;
	} else if (state == "Configured") {
		$("status_description").update("The FED crate hardware under command of this application has been configured and is ready to take data.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("configure_button").disabled = false;
		$("enable_button").disabled = false;
	} else if (state == "Enabled") {
		$("status_description").update("The FED crate hardware under command of this application is monitoring data in real-time and will report errors to the CSC Hotspot.");
		$("statusicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/dialog-information.png");
		$("halt_button").disabled = false;
		$("disable_button").disabled = false;
	}
	
	var monitorURL = data.monitorURL;
	$("monitorURL").update(monitorURL).setAttribute("href", monitorURL);
	var commanderURL = data.commanderURL;
	$("commanderURL").update(commanderURL).setAttribute("href", commanderURL);
	
	var systemName = data.systemName;
	$("application_title").update("FED Crate Communicator (" + systemName + ")");
	
	this.reset();
}


