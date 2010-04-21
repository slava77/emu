/*****************************************************************************\
* $Id: commander.js,v 1.9 2010/04/21 13:19:25 paste Exp $
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

	// Selection buttons
	$$(".all_ddus").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".ddu_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = true;
			});
			$$(".ddu_button").each( function(e) { e.disabled = false; });
		});
	});
	$$(".no_ddus").each(function(element) {
		element.observe("click", function(ev) {
			var el = ev.element();
			var crate = el.readAttribute("crate");
			$$(".ddu_checkbox").each(function(element2) {
				if (element2.readAttribute("crate") == crate) element2.checked = false;
			});
			$$(".ddu_button").each( function(e) { e.disabled = true; });
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
		$$(".firmware_button").each( function(e) { e.disabled = false; });
	} else {
		$$(".firmware_button").each( function(e) { e.disabled = true; });
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
				$$(".firmware_button").each( function(e) { e.disabled = false; });
			} else {
				$$(".firmware_button").each( function(e) { e.disabled = true; });
			}
		});
	});

	// Make the firmware buttons do something
	($$(".ddu_firmware")).each(function(el) {
		el.observe("click", function(ev) {
			// The crates to manipulate
			window.location.href = URL + "/DDUFirmwareManager?crate=" + ev.element().readAttribute("crate");
		});
	});
	($$(".dcc_firmware")).each(function(el) {
		el.observe("click", function(ev) {
			// The crates to manipulate
			window.location.href = URL + "/DCCFirmwareManager?crate=" + ev.element().readAttribute("crate");
		});
	});

	// Disable DDU buttons until a DDU is selected
	// Count the number of checked DDUs
	var dduChecked = 0;
	$$(".ddu_checkbox").each(function(el2) {
		if (el2.checked) dduChecked++;
	});
	if (dduChecked) {
		$$(".ddu_button").each( function(e) { e.disabled = false; });
	} else {
		$$(".ddu_button").each( function(e) { e.disabled = true; });
	}
	$$(".ddu_checkbox").each(function(element) {
		element.observe("change", function(el) {
			// Count the number of checked ddus and make the ddu section appear if there are any
			var totalChecked = 0;
			$$(".ddu_checkbox").each(function(el2) {
				if (el2.checked) totalChecked++;
			});
			if (totalChecked) {
				$$(".ddu_button").each( function(e) { e.disabled = false; });
			} else {
				$$(".ddu_button").each( function(e) { e.disabled = true; });
			}
		});
	});

	// Disable DCC buttons until a DCC is selected
	// Count the number of checked DCCs
	var dccChecked = 0;
	$$(".dcc_checkbox").each(function(el2) {
		if (el2.checked) dccChecked++;
	});
	if (dccChecked) {
		$$(".dcc_button").each( function(e) { e.disabled = false; });
	} else {
		$$(".dcc_button").each( function(e) { e.disabled = true; });
	}
	$$(".dcc_checkbox").each(function(element) {
		element.observe("change", function(el) {
			// Count the number of checked ddus and make the ddu section appear if there are any
			var totalChecked = 0;
			$$(".dcc_checkbox").each(function(el2) {
				if (el2.checked) totalChecked++;
			});
			if (totalChecked) {
				$$(".dcc_button").each( function(e) { e.disabled = false; });
			} else {
				$$(".dcc_button").each( function(e) { e.disabled = true; });
			}
		});
	});

	// TODO FIXME
	$$(".ddu_button").each( function(e) { e.disabled = false; });
	$$(".dcc_button").each( function(e) { e.disabled = false; });

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
	$("all_dcc_registers").observe("click", function(ev) {
		$$(".dcc_registers_checkbox").each(function(element) {
			element.checked = true;
		});
	});
	$("no_dcc_registers").observe("click", function(ev) {
		$$(".dcc_registers_checkbox").each(function(element) {
			element.checked = false;
		});
	});

	// Print data in selected format (in a new window)
	$("ddu_display_button").observe("click", function(ev) {
		// The crates/slots to read
		var ruis = new Array();
		$$(".ddu_checkbox").each( function(e) {
			if (e.checked == true) {
				ruis.push(e.readAttribute("rui"));
			}
		});
		// The registers to read
		var registers = new Array();
		$$(".ddu_registers_checkbox").each( function(e) {
			if (e.checked == true) {
				registers.push(e.readAttribute("name"));
			}
		});
		
		// Build a URL from the registers
		var params = new Hash();
		params.set("board", "ddu");
		params.set("rui", ruis);
		params.set("reg", registers);
		var url = URL + "/DisplayRegisters?" + params.toQueryString();
		document.location.href = url;
	});

	$("dcc_display_button").observe("click", function(ev) {
		// The crates/slots to read
		var fmmids = new Array();
		$$(".dcc_checkbox").each( function(e) {
			if (e.checked == true) {
				fmmids.push(e.readAttribute("fmmid"));
			}
		});
		// The registers to read
		var registers = new Array();
		$$(".dcc_registers_checkbox").each( function(e) {
			if (e.checked == true) {
				registers.push(e.readAttribute("name"));
			}
		});
		// Can I submit a form without that form actually existing?
		($$(".fake_form")).invoke("remove");
		var time = new Date();
		var fakeForm = new Element("form", {"class": "fake_form", "style": "display:none", "method": "get", "action": URL + "/DisplayRegisters?board=dcc", "target": "commanderDisplay" + time.valueOf()});
		fmmids.each(function(fmmid) {
			fakeForm.insert(new Element("input", {"name": "fmmid", "value": fmmid}));
		});
		registers.each(function(register) {
			fakeForm.insert(new Element("input", {"name": "reg", "value": register}));
		});
		$("FED_Commander_Select_dialog").insert(fakeForm);
		fakeForm.submit();
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
