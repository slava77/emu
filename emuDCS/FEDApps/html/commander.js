/*****************************************************************************\
* $Id: commander.js,v 1.2 2009/08/15 19:17:42 paste Exp $
\*****************************************************************************/

Event.observe(window, "load", function(event) {
	
	var statusReloadElement = new ReloadElement();
	statusReloadElement.id = "FED_Commander_Select";
	statusReloadElement.reloadFunction = getStatus;
	statusReloadElement.callbackSuccess = updateStatus;
	statusReloadElement.callbackError = reportErrorAndStop;
	statusReloadElement.timeToReload = 10;
	statusReloadElement.timeToError = 60;
	reloadElements.push(statusReloadElement);
	
	var configurationReloadElement = new ReloadElement();
	configurationReloadElement.id = "FED_Commander_Configuration";
	configurationReloadElement.reloadFunction = getConfiguration;
	configurationReloadElement.callbackSuccess = updateConfiguration;
	configurationReloadElement.callbackError = reportErrorAndStop;
	configurationReloadElement.timeToReload = 10;
	configurationReloadElement.timeToError = 60;
	reloadElements.push(configurationReloadElement);
	
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
	
	// Reconfigure
	// Firefox is an idiot when it comes to refreshing the DOM, so I need this here.
	$("reconfigure_button").disabled = false;
	$("reconfigure_button").observe("click", function(ev) {
		ev.element().disabled = true;
		reconfigure(configurationReloadElement);
	});
	
	// Selection buttons
	$$(".all_ddus").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".ddu_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = true;
			});
			$$(".ddu_hidden").invoke("show");
			$("ddu_displayed").hide();
		});
	});
	$$(".no_ddus").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".ddu_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = false;
			});
			$$(".ddu_hidden").invoke("hide");
			$("ddu_displayed").show();
		});
	});
	$$(".all_dccs").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".dcc_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = true;
			});
		});
	});
	$$(".no_dccs").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".dcc_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = false;
			});
		});
	});

	// Hide firmware section until a crate is selected
	// Count the number of checked crates and make the firmware section appear if there are any
	var firmwareChecked = 0;
	$$(".crate_checkbox").each(function(el2) {
		if (el2.checked) firmwareChecked++;
	});
	if (firmwareChecked) {
		$("firmware_hidden").show();
		$("firmware_displayed").hide();
	} else {
		$("firmware_hidden").hide();
		$("firmware_displayed").show();
	}
	$$(".crate_checkbox").each(function(element) {
		element.observe("change", function(el) {
			// Ensure that only one check box is selected at a time (but they can't be radios, which cannot be unselected)
			if (el.checked) {
				// Bleh
				$$(".crate_checkbox").each(function(el2) {
					if (el2.id != el.id) el2.checked = false;
				});
			}
			// Count the number of checked crates and make the firmware section appear if there are any
			var totalChecked = 0;
			$$(".crate_checkbox").each(function(el2) {
				if (el2.checked) totalChecked++;
			});
			if (totalChecked) {
				$("firmware_hidden").show();
				$("firmware_displayed").hide();
			} else {
				$("firmware_hidden").hide();
				$("firmware_displayed").show();
			}
		});
	});

	// Hide DDU section until a DDU is selected
	// Count the number of checked DDUs and make the DDU section appear if there are any
	var dduChecked = 0;
	$$(".ddu_checkbox").each(function(el2) {
		if (el2.checked) dduChecked++;
	});
	if (dduChecked) {
		$$(".ddu_hidden").invoke("show");
		$("ddu_displayed").hide();
	} else {
		$$(".ddu_hidden").invoke("hide");
		$("ddu_displayed").show();
	}
	$$(".ddu_checkbox").each(function(element) {
		element.observe("change", function(el) {
			// Count the number of checked ddus and make the ddu section appear if there are any
			var totalChecked = 0;
			$$(".ddu_checkbox").each(function(el2) {
				if (el2.checked) totalChecked++;
			});
			if (totalChecked) {
				$$(".ddu_hidden").invoke("show");
				$("ddu_displayed").hide();
			} else {
				$$(".ddu_hidden").invoke("hide");
				$("ddu_displayed").show();
			}
		});
	});
	
	$("all_ddu_registers").observe("click", function(ev) {
		$$(".ddu_registers_checkbox").each(function(element) {
			element.checked = true;
		});
	});
	$("no_ddu_registers").observe("click", function(ev) {
		$$(".ddu_registers_checkbox").each(function(element) {
			element.checked = false;
		});
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
	
	data.crates.each(function(crate) {
		var crateNumber = crate.number;
		crate.ddus.each(function(ddu) {
			var slotNumber = ddu.slot;
			var dduElement;
			if (dduElement = $("crate_" + crateNumber + "_slot_" + slotNumber)) {
				dduElement.removeClassName("ok").removeClassName("error").removeClassName("warning").removeClassName("error_black").removeClassName("questionable").addClassName(ddu.status);
			}
			
			ddu.fibers.each(function(fiber) {
				var fiberNumber = fiber.number;
				var fiberElement;
				if (fiberElement = $("crate_" + crateNumber + "_slot_" + slotNumber + "_fiber_" + fiberNumber)) {
					fiberElement.removeClassName("ok").removeClassName("error").removeClassName("warning").removeClassName("error_black").removeClassName("questionable").addClassName(fiber.status);
				}
			});
		});
		crate.dccs.each(function(dcc) {
			var slotNumber = dcc.slot;
			var dccElement;
			if (dccElement = $("crate_" + crateNumber + "_slot_" + slotNumber)) {
				dccElement.removeClassName("ok").removeClassName("error").removeClassName("warning").removeClassName("error_black").removeClassName("questionable").addClassName(dcc.status);
			}
			
			dcc.fifos.each(function(fifo) {
				var fifoNumber = fifo.number;
				
				var fifoElement;
				if (fifoElement = $("crate_" + crateNumber + "_slot_" + slotNumber + "_fifo_" + fifoNumber)) {
					fifoElement.removeClassName("ok").removeClassName("error").removeClassName("warning").removeClassName("error_black").removeClassName("questionable").addClassName(fifo.status);
				}
			});
			
			dcc.slinks.each(function(slink) {
				var slinkNumber = slink.number;
				var slinkElement;
				if (slinkElement = $("crate_" + crateNumber + "_slot_" + slotNumber + "_slink_" + slinkNumber)) {
					slinkElement.removeClassName("ok").removeClassName("error").removeClassName("warning").removeClassName("error_black").removeClassName("questionable").addClassName(slink.status);
				}
			});
		});
	});
	
	// Finish by resetting the countdown
	this.reset();
}
