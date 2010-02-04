/*****************************************************************************\
* $Id: configurable.js,v 1.3 2010/02/04 10:39:51 paste Exp $
\*****************************************************************************/

var oldConfigMode = "";

Event.observe(window, "load", function(event) {
	
	var configurationReloadElement = new ReloadElement();

	($$(".configurable_hidden")).invoke("hide");

	// There is only one of these
	$$(".configuration_dialog").each(function(el) {
		configurationReloadElement.id = el.readAttribute("name");
		configurationReloadElement.reloadFunction = getConfiguration;
		configurationReloadElement.callbackSuccess = updateConfiguration;
		configurationReloadElement.callbackError = reportErrorAndStop;
		configurationReloadElement.timeToReload = 10;
		configurationReloadElement.timeToError = 60;
		reloadElements.push(configurationReloadElement);
	});

	// Change configuration settings
	$$(".config_type").each(function(element) {
		// Get the previous configuration mode for warnings
		if (element.checked) oldConfigMode = element.value;
		element.observe("change", function(ev) {
			var el = ev.element();
			if (el.checked) {
				changeConfigMode(el.value, configurationReloadElement);
			}
		});
	});
	
	// Make the configuration file change automatically submit.
	if ($("xml_file_select")) {
		$("xml_file_select").observe("change", function(ev) {
			changeXMLFile($("xml_file_select").value, configurationReloadElement);
		});
	}
	
	// Make the correct DB key selector show up (if it hasn't already)
	if ($("system_name_select")) {
		var element = ($("system_name_select"));
		var selectedName = element.value;
		($$(".db_key_select")).each(function(el) {
			if (el.readAttribute("system") == selectedName) el.show();
			else el.hide();
		});
	
		// Make the correct DB key selector show up when the name changes
		element.observe("change", function(ev) {
			($$(".db_key_select")).each(function(el) {
				if (el.readAttribute("system") == ev.element().value) el.show();
				else el.hide();
			});
		});
	}

	// Make the DB key change automatically submit.
	($$(".db_key_select")).each(function(el) {
		el.observe("change", function(ev) {
			changeDBKey(ev.element().value, configurationReloadElement);
		});
	});

	// Reconfigure
	// Firefox is an idiot when it comes to refreshing the DOM, so I need this here.
	$("reconfigure_button").disabled = false;
	$("reconfigure_button").observe("click", function(ev) {
		ev.element().disabled = true;
		reconfigure(configurationReloadElement);
	});

});

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
	
	if (!confirm("Changing the configuration mode to something else other than the default is not recommended, and will likely have undesired consequences in future runs.\n\nIf you are sure you wish to change the configuration mode, click \"OK\".  If you want to use the default setting, click \"Cancel\".")) {
		$$(".config_type").each(function(element) {
			if (element.value == oldConfigMode) element.checked = true;
		});
		return;
	}
	
	// Change the previous configuration mode
	oldConfigMode = mode;

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
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = true;
		});
	} else if (configMode == "Database") {
		$("config_type_database").checked = true;
		$("xml_file_select").disabled = true;
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = false;
		});
	} else if (configMode == "Autodetect") {
		$("config_type_autodetect").checked = true;
		$("xml_file_select").disabled = true;
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = true;
		});
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
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = true;
		});
	} else if (configMode == "Database") {
		$("config_type_database").checked = true;
		$("xml_file_select").disabled = true;
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = false;
		});
	} else if (configMode == "Autodetect") {
		$("config_type_autodetect").checked = true;
		$("xml_file_select").disabled = true;
		$$(".db_key_select", ".system_name_select").each(function(el) {
			el.disabled = true;
		});
	}
	
	var xmlFile = data.xmlFile;
	$("xml_file_select").childElements().each(function(element) {
		if (element.value == xmlFile) element.selected = true;
		else element.selected = false;
	});
	
	var dbKey = data.dbKey;
	var systemName = "";
	$$(".db_key_select").each(function(el) {
		el.childElements().each(function(element) {
			if (element.value == dbKey) {
				element.selected = true;
				systemName = el.readAttribute("system");
			} else element.selected = false;
		});
	});
	
	$$(".system_name_select").each(function(el) {
		el.childElements().each(function(element) {
			if (element.value == systemName) element.selected = true;
			else element.selected = false;
		});
	});
	
	this.reset();
}

function finishReconfigure(transport) {
	window.location.reload();
}
