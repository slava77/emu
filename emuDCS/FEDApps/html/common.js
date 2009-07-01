/*****************************************************************************\
* $Id: common.js,v 1.2 2009/07/01 14:54:02 paste Exp $
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

function getConfiguration() {
	// Bind the special callbacks
	var successCallback = this.callbackSuccess.bind(this);
	var errorCallback = this.callbackError.bind(this);
	
	var url = URL + "/GetConfiguration";
	
	new Ajax.Request(url, {
		method: "get",
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}

function changeConfigMode(mode, reloadElement) {
	
	if (window.defaultConfigurationMode !== undefined && mode != defaultConfigurationMode && !confirm("Changing the configuration mode to something else other than the default is not recommended, and will likely have undesired consequences in future runs.\n\nIf you are sure you wish to change the configuration mode, click \"OK\".  If you want to use the default setting, click \"Cancel\".")) {
		$$(".config_type").each(function(element) {
			if (element.value == defaultConfigurationMode) element.checked = true;
		});
		return;
	}
	
	// Bind functions
	reloadElement.changeConfigModeSuccessCallback = finishConfigMode;
	var successCallback = reloadElement.changeConfigModeSuccessCallback.bind(reloadElement);
	
	var url = URL + "/ChangeConfigMode";
	var params = {"configMode": mode};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: reportError
	});
}

function changeXMLFile(file, reloadElement) {
	// Bind functions
	reloadElement.changeXMLSuccessCallback = finishXMLFile;
	var successCallback = reloadElement.changeXMLSuccessCallback.bind(reloadElement);
	
	var url = URL + "/ChangeXMLFile";
	var params = {"xmlFile": file};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: reportError
	});
}

function changeDBKey(key, reloadElement) {
	// Bind functions
	reloadElement.changeDBKeySuccessCallback = finishDBKey;
	var successCallback = reloadElement.changeDBKeySuccessCallback.bind(reloadElement);
	
	var url = URL + "/ChangeDBKey";
	var params = {"dbKey": key};
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: reportError
	});
}

function reconfigure(reloadElement) {
	reloadElement.stop = true;
	$(reloadElement.id + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
	
	var url = URL + "/Reconfigure";
	new Ajax.Request(url, {
		method: "get",
		onSuccess: finishReconfigure,
		onFailure: reportError
	});
}

function finishConfigMode(transport) {
	var data = transport.responseJSON;
	var configMode = data.configMode;
	if (configMode == "XML") {
		$("config_type_xml").checked = true;
		$("xml_file_select").disabled = false;
		$("db_key_select").disabled = true;
	} else if (configMode == "Database") {
		$("config_type_database").checked = true;
		$("xml_file_select").disabled = true;
		$("db_key_select").disabled = false;
	} else if (configMode == "Autodetect") {
		$("config_type_autodetect").checked = true;
		$("xml_file_select").disabled = true;
		$("db_key_select").disabled = true;
	}
	this.tick(true);
}

function finishXMLFile(transport) {
	var data = transport.responseJSON;
	var xmlFile = data.xmlFile;
	$("xml_file_select").childElements().each(function(element) {
		if (element.value == data.xmlFile) element.selected = true;
		else element.selected = false;
	});
	this.tick(true);
}

function finishDBKey(transport) {
	var data = transport.responseJSON;
	var dbKey = data.dbKey;
	$("db_key_select").childElements().each(function(element) {
		if (element.value == data.dbKey) element.selected = true;
		else element.selected = false;
	});
	this.tick(true);
}

function updateConfiguration(transport) {
	var data = transport.responseJSON;
	
	var configMode = data.configMode;
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
	
	var xmlFile = data.xmlFile;
	$("xml_file_select").childElements().each(function(element) {
		if (element.value == data.xmlFile) element.selected = true;
		else element.selected = false;
	});
	
	var dbKey = data.dbKey;
	$("db_key_select").childElements().each(function(element) {
		if (element.value == data.dbKey) element.selected = true;
		else element.selected = false;
	});
	
	this.reset();
}

function finishReconfigure(transport) {
	window.location.reload();
}
