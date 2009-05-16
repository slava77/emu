/*****************************************************************************\
* $Id: communicator.js,v 1.1 2009/05/16 18:53:09 paste Exp $
\*****************************************************************************/

var reloadElements = ["FED_Communicator_Status", "FED_Communicator_Configuration"];
var defaultConfigurationMode = "XML";

Event.observe(window, "load", function(event) {
	
	// enable buttons with check-box
	$("enable_buttons").observe("change", function(ev) {
		var element = ev.element();
		if (element.checked && confirm("Enabling manual state changes will cause this FED Communicator application to ignore any SOAP messages it receives from the FED Manager.  If this FED Communicator application is involved in a global run, this may result in the run crashing.\n\nClick \"OK\" if you are certain you want to ignore SOAP messages.  Otherwise, click \"Cancel\".")) {
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
	
	// Change configuration settings
	$$(".config_type").each(function(element) {
		element.observe("change", function(ev) {
			var el = ev.element();
			if (el.checked) {
				changeConfigMode(el.value);
			}
		});
	});
	
	// Make the configuration file change automatically submit.
	$("xml_file_select").observe("change", function(ev) {
		changeXMLFile();
	});
	
});

function changeConfigMode(mode) {

	if (mode != defaultConfigurationMode && !confirm("Changing the configuration mode to something else other than the default is not recommended, and will likely have undesired consequences in future runs.\n\nIf you are sure you wish to change the configuration mode, click \"OK\".  If you want to use the default setting, click \"Cancel\".")) {
		$$(".config_type").each(function(element) {
			if (element.value == defaultConfigurationMode) element.checked = true;
		});
		return;
	}

	stop = true;

	var url = URL + "/ChangeConfigMode";
	var params = {"configMode": mode};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: finishConfigMode,
		onFailure: reportError
	});
}

function changeXMLFile() {
	stop = true;

	var url = URL + "/ChangeXMLFile";
	var params = {"xmlFile": $("xml_file_select").value};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: finishXMLFile,
		onFailure: reportError
	});
}

function finishConfigMode(transport) {
	var data = transport.responseJSON;
	stop = false;
	getStatus();
}

function finishXMLFile(transport) {
	var data = transport.responseJSON;
	stop = false;
	getStatus();
}

function updateStates(transport) {
	var data = transport.responseJSON;
	
	$$(".deleteme").invoke("remove");
	
	var state = data["state"] ? data["state"] : "Unknown";
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
	
	var monitorURL = data["monitorURL"];
	$("monitorURL").update(monitorURL).setAttribute("href", monitorURL);
	var commanderURL = data["commanderURL"];
	$("commanderURL").update(commanderURL).setAttribute("href", commanderURL);
	
	var configMode = data["configMode"];
	if (configMode == "XML") {
		$("config_type_xml").checked = true;
		$("xml_file_select").disabled = false;
	} else if (configMode == "Database") {
		$("config_type_database").checked = true;
		$("xml_file_select").disabled = true;
	} else if (configMode == "Autodetect") {
		$("config_type_autodetect").checked = true;
		$("xml_file_select").disabled = true;
	}
	
	var xmlFile = data["xmlFile"];
	$("xml_file_select").childElements().each(function(element) {
		if (element.value == data["xmlFile"]) element.selected = true;
		else element.selected = false;
	});
	
	var systemName = data["systemName"];
	$("application_title").update("FED Crate Communicator (" + systemName + ")");
	
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

