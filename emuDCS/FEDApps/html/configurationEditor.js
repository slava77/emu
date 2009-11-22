/*****************************************************************************\
* $Id: configurationEditor.js,v 1.1 2009/11/22 22:52:21 paste Exp $
\*****************************************************************************/

// Make this global.  It saves on DB queries if we have an evil user
var keyMap = new Hash();

function union() {
	var cnt = new Hash();
	var argArray = $A(arguments);
	argArray.each(function(arr) {
		var arr2 = $A(arr).map(function(e){ return e.readAttribute("id"); });
		arr2.each(function(el) {
			if (cnt.get(el)) cnt.set(el, cnt.get(el) + 1);
			else cnt.set(el, 1);
		});
	});
	var ret = new Array();
	cnt.each(function(pair) {
		if (pair.value == argArray.size()) ret.push(pair.key);
	});
	
	return ret;
}

Event.observe(window, "load", function(event) {

	($$(".hidden")).invoke("hide");

	($("xml_file_button")).disabled = false;

	($("xml_file_button")).observe("click", function(ev) {
		ev.element().update("Uploading...").insert({"after": new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif"})});
		ev.element().disabled = true;
		($("xml_file_form")).submit();
		window.location.reload();
	});
	
	// Update the DB keys and descriptions
	getDBKeys();
	
	($("load_button")).observe("click", function(ev) {
	
		var key = ($("configuration_key")).value;
		if (key == "") return;
	
		ev.element().update("Loading from database...").insert({"after": new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif"})});
		ev.element().disabled = true;
		var url = URL + "/LoadFromDB";
		var params = {"key": key}
		
		new Ajax.Request(url, {
			method: "post",
			parameters: params,
			onSuccess: updateDBKeys,
			onFailure: reportError
		});
	
	});

	($$(".crates_open_close")).each(function(element) {
		element.observe("click", function(ev) {
		
			var imgEle = $("crates_open_close");
			var hidden = $$("[crates_hidden]");
			
			popOpen("crates_hidden", hidden, imgEle);
			toggleHidden(hidden);
		
		});
	});
	
	($$(".add_crate")).each(function(element) {
		element.observe("click", function(ev) {
			
			var newCrateNumber = -1;
			($$("[crate]")).each(function(el) {
				var a = parseInt(el.readAttribute("crate"));
				if (a > newCrateNumber) newCrateNumber = a;
			});
			
			newCrateNumber++;
			var newTable = newCrate(newCrateNumber);
			
			element.up().insert({"before": newTable});
			updateCrateTriggers(newCrateNumber);
			
			var newDiv2 = new Element("div", {"id": "config_dccs_" + newCrateNumber, "class": "bold tier3 hidden", "crate": newCrateNumber, "crates_hidden": 0, "crate_hidden": 1});
			newDiv2.insert(new Element("img", {"class": "dccs_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": newCrateNumber}));
			newDiv2.insert(new Element("span", {"class": "dccs_open_close pointer", "crate": newCrateNumber}).update("DCCs"));
			newTable.insert({"after": newDiv2});
			
			var newButton2 = new Element("button", {"class": "add_dcc", "crate": newCrateNumber}).insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-add.png"})).insert("Add DCC");
			newDiv2.insert({"after": new Element("div", {"id": "config_dcc_add_" + newCrateNumber, "class": "tier4 hidden", "crate": newCrateNumber, "dccs_hidden": 1, "crate_hidden": 1, "crates_hidden": 0}).insert(newButton2)});
			
			updateDCCTriggers(newCrateNumber);
			
			var newDiv = new Element("div", {"id": "config_ddus_" + newCrateNumber, "class": "bold tier3 hidden", "crate": newCrateNumber, "crates_hidden": 0, "crate_hidden": 1});
			newDiv.insert(new Element("img", {"class": "ddus_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": newCrateNumber}));
			newDiv.insert(new Element("span", {"class": "ddus_open_close pointer", "crate": newCrateNumber}).update("DDUs"));
			newTable.insert({"after": newDiv});
			
			var newButton = new Element("button", {"class": "add_ddu", "crate": newCrateNumber}).insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-add.png"})).insert("Add DDU");
			newDiv.insert({"after": new Element("div", {"id": "config_ddu_add_" + newCrateNumber, "class": "tier4 hidden", "crate": newCrateNumber, "ddus_hidden": 1, "crate_hidden": 1, "crates_hidden": 0}).insert(newButton)});
			
			updateDDUTriggers(newCrateNumber);
			
			newTable.insert({"after": newController(newCrateNumber)});
			
			toggleHidden($$("[crate_hidden]"));
		});
	});
	
	updateCrateTriggers();
	updateDDUTriggers();
	updateDCCTriggers();
	updateFiberTriggers();
	updateFIFOTriggers();

});

function updateCrateTriggers(number) {
	($$(".crate_open_close")).each(function(element) {
		if (number == null || element.readAttribute("crate") == number) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var imgEle;
				($$("img.crate_open_close")).each(function(el) {
					if (el.readAttribute("crate") == crate) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[crate_hidden]"), $$("[crate='"+crate+"']"));
				
				popOpen("crate_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});
	
	$$(".delete_crate").each(function(element) {
		if (number == null || element.readAttribute("crate") == number) {
			element.observe("click", function(ev) {
				
				var crate = element.readAttribute("crate");
				$$("[crate='"+crate+"']").invoke("remove");
				
			});
		}
	});
}

function updateDDUTriggers(crateNumber, ruiNumber) {
	$$(".ddus_open_close").each(function(element) {
		if (crateNumber == null || (ruiNumber == null && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var imgEle;
				$$("img.ddus_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[ddus_hidden]"), $$("[crate='"+crate+"']"));
				
				popOpen("ddus_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});

	$$(".ddu_open_close").each(function(element) {
		if (crateNumber == null || (ruiNumber == element.readAttribute("rui") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var rui = element.readAttribute("rui");
				var imgEle;
				$$("img.ddu_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate && el.readAttribute("rui") == rui) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[ddu_hidden]"), $$("[crate='"+crate+"']"), $$("[rui='"+rui+"']"));

				popOpen("ddu_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});
	
	$$(".delete_ddu").each(function(element) {
		if (crateNumber == null || (ruiNumber == element.readAttribute("rui") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var crate = element.readAttribute("crate");
				var rui = element.readAttribute("rui");
				var deleteMe = union($$("[crate='"+crate+"']"), $$("[rui='"+rui+"']"));
				deleteMe.each(function(id) {
					if ($(id)) ($(id)).remove();
				});
				
			});
		}
	});
	
	$$(".add_ddu").each(function(element) {
		if (crateNumber == null || (ruiNumber == null && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var newRUI = -1;
				$$("[rui]").each(function(el) {
					var a = parseInt(el.readAttribute("rui"));
					if (a > newRUI) newRUI = a;
				});
				
				newRUI++;
				
				var crate = element.readAttribute("crate");
				var newTable = newDDU(crate, newRUI);
			
				element.up().insert({"before": newTable});
				updateDDUTriggers(crate, newRUI);
				
				var newDiv2 = new Element("div", {"id": "config_fibers_" + crate + "_" + newRUI, "class": "bold tier4 hidden", "crate": crate, "rui": newRUI, "crates_hidden": 0, "crate_hidden": 0, "ddus_hidden": 0, "ddu_hidden": 1});
				newDiv2.insert(new Element("img", {"class": "fibers_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": crate, "rui": newRUI}));
				newDiv2.insert(new Element("span", {"class": "fibers_open_close pointer", "crate": crate, "rui": newRUI}).update("Fibers"));
				newTable.insert({"after": newDiv2});
				
				var newButton2 = new Element("button", {"class": "add_fiber", "crate": crate, "rui": newRUI}).insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-add.png"})).insert("Add Fiber");
				newDiv2.insert({"after": new Element("div", {"id": "config_fiber_add_" + crate + "_" + newRUI, "class": "tier5 hidden", "crate": crate, "rui": newRUI, "fibers_hidden": 1, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0}).insert(newButton2)});
				
				updateFiberTriggers(crate, newRUI);
				
				toggleHidden($$("[ddu_hidden]"));
			});
		}
	});
}

function updateDCCTriggers(crateNumber, fmmNumber) {
	$$(".dccs_open_close").each(function(element) {
		if (crateNumber == null || (fmmNumber == null && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var imgEle;
				$$("img.dccs_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[dccs_hidden]"), $$("[crate='"+crate+"']"));
				
				popOpen("dccs_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});

	$$(".dcc_open_close").each(function(element) {
		if (crateNumber == null || (fmmNumber == element.readAttribute("fmmid") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var fmmid = element.readAttribute("fmmid");
				var imgEle;
				$$("img.dcc_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate && el.readAttribute("fmmid") == fmmid) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[dcc_hidden]"), $$("[crate='"+crate+"']"), $$("[fmmid='"+fmmid+"']"));

				popOpen("dcc_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});
	
	$$(".delete_dcc").each(function(element) {
		if (crateNumber == null || (fmmNumber == element.readAttribute("fmmid") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var crate = element.readAttribute("crate");
				var fmmid = element.readAttribute("fmmid");
				var deleteMe = union($$("[crate='"+crate+"']"), $$("[fmmid='"+fmmid+"']"));
				deleteMe.each(function(id) {
					if ($(id)) ($(id)).remove();
				});
				
			});
		}
	});
	
	$$(".add_dcc").each(function(element) {
		if (crateNumber == null || (fmmNumber == null && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var newFMM = -1;
				$$("[fmmid]").each(function(el) {
					var a = parseInt(el.readAttribute("fmmid"));
					if (a > newFMM) newFMM = a;
				});
				
				newFMM++;
				
				var crate = element.readAttribute("crate");
				var newTable = newDCC(crate, newFMM);
			
				element.up().insert({"before": newTable});
				updateDCCTriggers(crate, newFMM);
				
				var newDiv2 = new Element("div", {"id": "config_fifos_" + crate + "_" + newFMM, "class": "bold tier4 hidden", "crate": crate, "fmmid": newFMM, "crates_hidden": 0, "crate_hidden": 0, "dccs_hidden": 0, "dcc_hidden": 1});
				newDiv2.insert(new Element("img", {"class": "fifos_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": crate, "fmmid": newFMM}));
				newDiv2.insert(new Element("span", {"class": "fifos_open_close pointer", "crate": crate, "fmmid": newFMM}).update("FIFOs"));
				newTable.insert({"after": newDiv2});
				
				var newButton2 = new Element("button", {"class": "add_fifo", "crate": crate, "fmmid": newFMM}).insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-add.png"})).insert("Add FIFO");
				newDiv2.insert({"after": new Element("div", {"id": "config_fifo_add_" + crate + "_" + newFMM, "class": "tier5 hidden", "crate": crate, "fmmid": newFMM, "fifos_hidden": 1, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0}).insert(newButton2)});
				
				updateFIFOTriggers(crate, newFMM);
				
				toggleHidden($$("[dcc_hidden]"));
			});
		}
	});
}

function updateFiberTriggers(crateNumber, ruiNumber, fiberNumber) {
	$$(".fibers_open_close").each(function(element) {
		if (crateNumber == null || (fiberNumber == null && ruiNumber == element.readAttribute("rui") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var rui = element.readAttribute("rui");
				var imgEle;
				$$("img.fibers_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate && el.readAttribute("rui") == rui) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[fibers_hidden]"), $$("[crate='"+crate+"']"), $$("[rui='"+rui+"']"));

				popOpen("fibers_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});
	
	$$(".delete_fiber").each(function(element) {
		if (crateNumber == null || (fiberNumber == element.readAttribute("fiber") && ruiNumber == element.readAttribute("rui") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var crate = element.readAttribute("crate");
				var rui = element.readAttribute("rui");
				var fiber = element.readAttribute("fiber");
				var deleteMe = union($$("[crate='"+crate+"']"), $$("[rui='"+rui+"']"), $$("[fiber='"+fiber+"']"));
				deleteMe.each(function(id) {
					if ($(id)) ($(id)).remove();
				});
				
			});
		}
	});
	
	$$(".add_fiber").each(function(element) {
		if (crateNumber == null || (fiberNumber == null && ruiNumber == element.readAttribute("rui") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var newF = -1;
				var crate = element.readAttribute("crate");
				var rui = element.readAttribute("rui");
				union($$("[fiber]"), $$("[rui='"+rui+"']"), $$("[crate='"+crate+"']")).each(function(el) {
					var em = $(el);
					var a = parseInt(em.readAttribute("fiber"));
					if (a > newF) newF = a;
				});
				
				newF++;
				
				var newTable = newFiber(crate, rui, newF);
			
				element.up().insert({"before": newTable});
				updateFiberTriggers(crate, rui, newF);
				
				toggleHidden($$("[fiber_hidden]"));
			});
		}
	});
}

function updateFIFOTriggers(crateNumber, fmmNumber, fifoNumber) {
	$$(".fifos_open_close").each(function(element) {
		if (crateNumber == null || (fifoNumber == null && fmmNumber == element.readAttribute("fmmid") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
			
				var crate = element.readAttribute("crate");
				var fmmid = element.readAttribute("fmmid");
				var imgEle;
				$$("img.fifos_open_close").each(function(el) {
					if (el.readAttribute("crate") == crate && el.readAttribute("fmmid") == fmmid) {
						imgEle = el;
						throw $break;
					}
				});
				var hidden = union($$("[fifos_hidden]"), $$("[crate='"+crate+"']"), $$("[fmmid='"+fmmid+"']"));

				popOpen("fifos_hidden", hidden, imgEle);
				toggleHidden(hidden);
			
			});
		}
	});
	
	$$(".delete_fifo").each(function(element) {
		if (crateNumber == null || (fifoNumber == element.readAttribute("fifo") && fmmNumber == element.readAttribute("fmmid") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var crate = element.readAttribute("crate");
				var fmmid = element.readAttribute("fmmid");
				var fifo = element.readAttribute("fifo");
				var deleteMe = union($$("[crate='"+crate+"']"), $$("[fmmid='"+fmmid+"']"), $$("[fifo='"+fifo+"']"));
				deleteMe.each(function(id) {
					if ($(id)) ($(id)).remove();
				});
				
			});
		}
	});
	
	$$(".add_fifo").each(function(element) {
		if (crateNumber == null || (fifoNumber == null && fmmNumber == element.readAttribute("fmmid") && crateNumber == element.readAttribute("crate"))) {
			element.observe("click", function(ev) {
				
				var newF = -1;
				var crate = element.readAttribute("crate");
				var fmmid = element.readAttribute("fmmid");
				union($$("[fifo]"), $$("[fmmid='"+fmmid+"']"), $$("[crate='"+crate+"']")).each(function(el) {
					var em = $(el);
					var a = parseInt(em.readAttribute("fifo"));
					if (a > newF) newF = a;
				});
				
				newF++;
				
				var newTable = newFIFO(crate, fmmid, newF);
			
				element.up().insert({"before": newTable});
				updateFIFOTriggers(crate, fmmid, newF);
				
				toggleHidden($$("[fifo_hidden]"));
			});
		}
	});
}

function newCrate(number) {
	var newTable = new Element("table", {"id": "config_crate_table_" + number, "class": "tier2 config_crate_table hidden", "crate": number, "crates_hidden": 1});
	
	var newTR1 = new Element("tr");
	
	var newTD1 = new Element("td", {"crate": number, "class": "bold config_crate_name crate_open_close pointer"});
	newTD1.insert(new Element("img", {"class": "crate_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": number}));
	newTD1.insert("Crate " + number);
	newTR1.insert(newTD1);
	
	var newTD2 = new Element("td");
	var newButton = new Element("button", {"class": "delete_crate", "crate": number});
	newButton.insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	newButton.insert("Delete crate");
	newTD2.insert(newButton);
	newTR1.insert(newTD2);
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr", {"id": "config_crate_number_row_" + number, "class": "hidden", "crate": number, "crate_hidden": 1, "crates_hidden": 0});
	
	var newTD3 = new Element("td").update("Number");
	newTR2.insert(newTD3);
	
	var newTD4 = new Element("td").insert(new Element("input", {"class": "input_crate_number", "type": "text", "value": number, "crate": number}));
	newTR2.insert(newTD4);
	newTable.insert(newTR2);
	
	return newTable;
}

function newController(number) {
	var newTable = new Element("table", {"id": "config_controller_table_" + number, "class": "tier3 controller_table hidden", "crate": number, "crates_hidden": 0, "crate_hidden": 1});
	
	var newTR1 = new Element("tr").insert(new Element("td", {"crate": number, "class": "bold controller_name"}).update("VME Controller"));
	newTR1.insert(new Element("td"));
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr").insert(new Element("td").update("Device"));
	var newTD1 = new Element("td").insert(new Element("input", {"class": "input_controller_device", "type": "text", "value": 0, "crate": number}));
	newTR2.insert(newTD1);
	newTable.insert(newTR2);
	
	var newTR3 = new Element("tr").insert(new Element("td").update("Link"));
	var newTD2 = new Element("td").insert(new Element("input", {"class": "input_controller_link", "type": "text", "value": 0, "crate": number}));
	newTR3.insert(newTD2);
	newTable.insert(newTR3);
	
	return newTable;
}

function newDDU(crate, rui) {
	var newTable = new Element("table", {"id": "config_ddu_" + crate + "_" + rui, "class": "tier4 ddu_table hidden", "crate": crate, "rui": rui, "crates_hidden": 0, "crate_hidden": 0, "ddus_hidden": 0});
	
	var newTR1 = new Element("tr");
	
	var newTD1 = new Element("td", {"crate": crate, "class": "bold ddu_name ddu_open_close pointer", "rui": rui});
	newTD1.insert(new Element("img", {"class": "ddu_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": crate, "rui": rui}));
	newTD1.insert("DDU " + rui);
	newTR1.insert(newTD1);
	
	var newTD2 = new Element("td");
	var newButton = new Element("button", {"class": "delete_ddu", "crate": crate, "rui": rui});
	newButton.insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	newButton.insert("Delete DDU");
	newTD2.insert(newButton);
	newTR1.insert(newTD2);
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr", {"id": "config_ddu_row_slot_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR2.insert(new Element("td").update("Slot"));
	newTR2.insert(new Element("td").insert(new Element("input", {"class": "input_ddu_slot", "type": "text", "value": 0, "crate": crate, "rui": rui})));
	newTable.insert(newTR2);
	
	var newTR3 = new Element("tr", {"id": "config_ddu_row_rui_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR3.insert(new Element("td").update("RUI"));
	newTR3.insert(new Element("td").insert(new Element("input", {"class": "input_ddu_rui", "type": "text", "value": rui, "crate": crate, "rui": rui})));
	newTable.insert(newTR3);
	
	var newTR4 = new Element("tr", {"id": "config_ddu_row_fmmid_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR4.insert(new Element("td").update("FMM ID"));
	newTR4.insert(new Element("td").insert(new Element("input", {"class": "input_ddu_fmmid", "type": "text", "value": 0, "crate": crate, "rui": rui})));
	newTable.insert(newTR4);
	
	var newTR5 = new Element("tr", {"id": "config_ddu_row_fec_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR5.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_force_checks_" + crate + "_" + rui}).update("Enable forced error checks")));
	newTR5.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_force_checks_" + crate + "_" + rui, "class": "input_ddu_force_checks", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR5);
	
	var newTR6 = new Element("tr", {"id": "config_ddu_row_alct_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR6.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_force_alct_" + crate + "_" + rui}).update("Enable forced ALCT checks")));
	newTR6.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_force_alct_" + crate + "_" + rui, "class": "input_ddu_force_alct", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR6);
	
	var newTR7 = new Element("tr", {"id": "config_ddu_row_tmb_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR7.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_force_tmb_" + crate + "_" + rui}).update("Enable forced TMB checks")));
	newTR7.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_force_tmb_" + crate + "_" + rui, "class": "input_ddu_force_tmb", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR7);
	
	var newTR8 = new Element("tr", {"id": "config_ddu_row_cfeb_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR8.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_force_cfeb_" + crate + "_" + rui}).update("Enable forced CFEB checks")));
	newTR8.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_force_cfeb_" + crate + "_" + rui, "class": "input_ddu_force_cfeb", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR8);
	
	var newTR9 = new Element("tr", {"id": "config_ddu_row_dmb_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR9.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_force_dmb_" + crate + "_" + rui}).update("Enable forced normal DMB")));
	newTR9.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_force_dmb_" + crate + "_" + rui, "class": "input_ddu_force_dmb", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR9);
	
	var newTR10 = new Element("tr", {"id": "config_ddu_row_gbe_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR10.insert(new Element("td").update("Gigabit ethernet prescale"));
	newTR10.insert(new Element("td").insert(new Element("input", {"class": "input_ddu_gbe", "type": "text", "value": 0, "crate": crate, "rui": rui})));
	newTable.insert(newTR10);
	
	var newTR11 = new Element("tr", {"id": "config_ddu_row_ccb_" + crate + "_" + rui, "class": "hidden", "crate": crate, "rui": rui, "ddu_hidden": 1, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR11.insert(new Element("td").insert(new Element("label", {"for": "input_ddu_invert_ccb_" + crate + "_" + rui}).update("Invert CCB command signals")));
	newTR11.insert(new Element("td").insert(new Element("input", {"id": "input_ddu_invert_ccb_" + crate + "_" + rui, "class": "input_ddu_invert_ccb", "type": "checkbox", "crate": crate, "rui": rui})));
	newTable.insert(newTR11);
	
	return newTable;
}

function newDCC(crate, fmmid) {
	var newTable = new Element("table", {"id": "config_dcc_" + crate + "_" + fmmid, "class": "tier4 dcc_table hidden", "crate": crate, "fmmid": fmmid, "crates_hidden": 0, "crate_hidden": 0, "dccs_hidden": 0});
	
	var newTR1 = new Element("tr");
	
	var newTD1 = new Element("td", {"crate": crate, "class": "bold dcc_name dcc_open_close pointer", "fmmid": fmmid});
	newTD1.insert(new Element("img", {"class": "dcc_open_close pointer", "src": "/emu/emuDCS/FEDApps/images/plus.png", "crate": crate, "fmmid": fmmid}));
	newTD1.insert("DCC " + fmmid);
	newTR1.insert(newTD1);
	
	var newTD2 = new Element("td");
	var newButton = new Element("button", {"class": "delete_dcc", "crate": crate, "fmmid": fmmid});
	newButton.insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	newButton.insert("Delete DCC");
	newTD2.insert(newButton);
	newTR1.insert(newTD2);
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr", {"id": "config_dcc_row_slot_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR2.insert(new Element("td").update("Slot"));
	newTR2.insert(new Element("td").insert(new Element("input", {"class": "input_dcc_slot", "type": "text", "value": 0, "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR2);
	
	var newTR4 = new Element("tr", {"id": "config_dcc_row_fmmid_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR4.insert(new Element("td").update("FMM ID"));
	newTR4.insert(new Element("td").insert(new Element("input", {"class": "input_dcc_fmmid", "type": "text", "value": fmmid, "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR4);
	
	var newTR5 = new Element("tr", {"id": "config_dcc_row_slink1_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR5.insert(new Element("td").update("SLink 1 ID"));
	newTR5.insert(new Element("td").insert(new Element("input", {"class": "input_dcc_slink1", "type": "text", "value": fmmid, "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR5);
	
	var newTR6 = new Element("tr", {"id": "config_dcc_row_slink2_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR6.insert(new Element("td").update("SLink 2 ID"));
	newTR6.insert(new Element("td").insert(new Element("input", {"class": "input_dcc_slink2", "type": "text", "value": (parseInt(fmmid) + 1), "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR6);
	
	var newTR7 = new Element("tr", {"id": "config_dcc_row_sw_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR7.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_sw_switch_" + crate + "_" + fmmid}).update("Enable software switch")));
	newTR7.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_sw_switch_" + crate + "_" + fmmid, "class": "input_dcc_sw_switch", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR7);
	
	var newTR8 = new Element("tr", {"id": "config_dcc_row_ttc_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR8.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_ignore_ttc_" + crate + "_" + fmmid}).update("Ignore TTCRx-not-ready signal")));
	newTR8.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_ignore_ttc_" + crate + "_" + fmmid, "class": "input_dcc_itnore_ttc", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR8);
	
	var newTR9 = new Element("tr", {"id": "config_dcc_row_slinkbp_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR9.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_ignore_backpressure_" + crate + "_" + fmmid}).update("Ignore SLink backpressure")));
	newTR9.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_ignore_backpressure_" + crate + "_" + fmmid, "class": "input_dcc_ignore_backpressure", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR9);
	
	var newTR10 = new Element("tr", {"id": "config_dcc_row_slinknp_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR10.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_ignore_slink_" + crate + "_" + fmmid}).update("Ignore Slink-not-present")));
	newTR10.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_ignore_slink_" + crate + "_" + fmmid, "class": "input_dcc_ignore_slink", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR10);
	
	var newTR11 = new Element("tr", {"id": "config_dcc_row_sw4_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR11.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_sw4_" + crate + "_" + fmmid}).update("Switch bit 4")));
	newTR11.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_sw4_" + crate + "_" + fmmid, "class": "input_dcc_sw4", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR11);
	
	var newTR12 = new Element("tr", {"id": "config_dcc_row_sw5_" + crate + "_" + fmmid, "class": "hidden", "crate": crate, "fmmid": fmmid, "dcc_hidden": 1, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR12.insert(new Element("td").insert(new Element("label", {"for": "input_dcc_sw5_" + crate + "_" + fmmid}).update("Switch bit 5")));
	newTR12.insert(new Element("td").insert(new Element("input", {"id": "input_dcc_sw5_" + crate + "_" + fmmid, "class": "input_dcc_sw5", "type": "checkbox", "crate": crate, "fmmid": fmmid})));
	newTable.insert(newTR12);
	
	return newTable;
}

function newFiber(crate, rui, fiber) {

	var newTable = new Element("table", {"id": "config_fiber_" + crate + "_" + rui + "_" + fiber, "class": "tier5 fiber_table hidden", "crate": crate, "rui": rui, "fiber": fiber, "crates_hidden": 0, "crate_hidden": 0, "ddus_hidden": 0, "ddu_hidden": 0, "fibers_hidden": 0});
	
	var newTR1 = new Element("tr");
	var newTD1 = new Element("td", {"crate": crate, "class": "bold fiber_name", "rui": rui, "fiber": fiber});
	newTD1.insert("Fiber " + fiber);
	newTR1.insert(newTD1);
	
	var newTD2 = new Element("td");
	var newButton = new Element("button", {"class": "delete_fiber", "crate": crate, "rui": rui, "fiber": fiber});
	newButton.insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	newButton.insert("Delete Fiber");
	newTD2.insert(newButton);
	newTR1.insert(newTD2);
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr", {"id": "config_fiber_row_number_" + crate + "_" + rui + "_" + fiber, "class": "hidden", "crate": crate, "rui": rui, "fiber": fiber, "fibers_hidden": 0, "ddu_hidden": 0, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR2.insert(new Element("td").update("Number"));
	newTR2.insert(new Element("td").insert(new Element("input", {"class": "input_fiber_number", "type": "text", "value": fiber, "crate": crate, "rui": rui, "fiber": fiber})));
	newTable.insert(newTR2);
	
	var newTR3 = new Element("tr", {"id": "config_fiber_row_name_" + crate + "_" + rui + "_" + fiber, "class": "hidden", "crate": crate, "rui": rui, "fiber": fiber, "fibers_hidden": 0, "ddu_hidden": 0, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR3.insert(new Element("td").update("Name"));
	newTR3.insert(new Element("td").insert(new Element("input", {"class": "input_fiber_name", "type": "text", "crate": crate, "rui": rui, "fiber": fiber})));
	newTable.insert(newTR3);
	
	var newTR4 = new Element("tr", {"id": "config_fiber_row_killed_" + crate + "_" + rui + "_" + fiber, "class": "hidden", "crate": crate, "rui": rui, "fiber": fiber, "fibers_hidden": 0, "ddu_hidden": 0, "ddus_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR4.insert(new Element("td").insert(new Element("label", {"for": "input_fiber_killed_" + crate + "_" + rui + "_" + fiber}).update("Killed")));
	newTR4.insert(new Element("td").insert(new Element("input", {"id": "input_fiber_killed_" + crate + "_" + rui + "_" + fiber, "class": "input_fiber_killed", "type": "checkbox", "crate": crate, "rui": rui, "fiber": fiber})));
	newTable.insert(newTR4);
	
	return newTable;
}

function newFIFO(crate, fmmid, fifo) {

	var newTable = new Element("table", {"id": "config_fifo_" + crate + "_" + fmmid + "_" + fifo, "class": "tier5 fifo_table hidden", "crate": crate, "fmmid": fmmid, "fifo": fifo, "crates_hidden": 0, "crate_hidden": 0, "dccs_hidden": 0, "dcc_hidden": 0, "fifos_hidden": 0});
	
	var newTR1 = new Element("tr");
	var newTD1 = new Element("td", {"crate": crate, "class": "bold fifo_name", "fmmid": fmmid, "fifo": fifo});
	newTD1.insert("FIFO " + fifo);
	newTR1.insert(newTD1);
	
	var newTD2 = new Element("td");
	var newButton = new Element("button", {"class": "delete_fifo", "crate": crate, "fmmid": fmmid, "fifo": fifo});
	newButton.insert(new Element("img", {"class": "icon", "src": "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	newButton.insert("Delete FIFO");
	newTD2.insert(newButton);
	newTR1.insert(newTD2);
	newTable.insert(newTR1);
	
	var newTR2 = new Element("tr", {"id": "config_fifo_row_number_" + crate + "_" + fmmid + "_" + fifo, "class": "hidden", "crate": crate, "fmmid": fmmid, "fifo": fifo, "fifos_hidden": 0, "dcc_hidden": 0, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR2.insert(new Element("td").update("Number"));
	newTR2.insert(new Element("td").insert(new Element("input", {"class": "input_fifo_number", "type": "text", "value": fifo, "crate": crate, "fmmid": fmmid, "fifo": fifo})));
	newTable.insert(newTR2);
	
	var newTR3 = new Element("tr", {"id": "config_fifo_row_rui_" + crate + "_" + fmmid + "_" + fifo, "class": "hidden", "crate": crate, "fmmid": fmmid, "fifo": fifo, "fifos_hidden": 0, "dcc_hidden": 0, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR3.insert(new Element("td").update("RUI"));
	newTR3.insert(new Element("td").insert(new Element("input", {"class": "input_fifo_rui", "type": "text", "crate": crate, "fmmid": fmmid, "fifo": fifo})));
	newTable.insert(newTR3);
	
	var newTR4 = new Element("tr", {"id": "config_fifo_row_used_" + crate + "_" + fmmid + "_" + fifo, "class": "hidden", "crate": crate, "fmmid": fmmid, "fifo": fifo, "fifos_hidden": 0, "dcc_hidden": 0, "dccs_hidden": 0, "crate_hidden": 0, "crates_hidden": 0});
	newTR4.insert(new Element("td").insert(new Element("label", {"for": "input_fifo_used_" + crate + "_" + fmmid + "_" + fifo}).update("Used")));
	newTR4.insert(new Element("td").insert(new Element("input", {"id": "input_fifo_used_" + crate + "_" + fmmid + "_" + fifo, "class": "input_fifo_used", "type": "checkbox", "crate": crate, "fmmid": fmmid, "fifo": fifo})));
	newTable.insert(newTR4);

	return newTable;
}

function popOpen(class, hidden, imgEle) {
	if (imgEle.readAttribute("src") == "/emu/emuDCS/FEDApps/images/plus.png") {
		imgEle.setAttribute("src", "/emu/emuDCS/FEDApps/images/minus.png");
		hidden.each(function(el) {
			($(el)).writeAttribute(class, 0);
		});
	} else {
		imgEle.setAttribute("src", "/emu/emuDCS/FEDApps/images/plus.png");
		hidden.each(function(el) {
			($(el)).writeAttribute(class, 1);
		});
	}
}

function toggleHidden(hidden) {
	hidden.each(function(id) {
		var el = $(id);
		if ((el.readAttribute("crates_hidden") == 1 || el.readAttribute("crate_hidden") == 1 || el.readAttribute("ddus_hidden") == 1 || el.readAttribute("ddu_hidden") == 1 || el.readAttribute("fibers_hidden") == 1 || el.readAttribute("fiber_hidden") == 1 || el.readAttribute("dccs_hidden") == 1 || el.readAttribute("dcc_hidden") == 1 || el.readAttribute("fifos_hidden") == 1 || el.readAttribute("fifo_hidden") == 1)) {
			el.hide();
		} else {
			el.show();
		}
	});
}

function getDBKeys() {
	var url = URL + "/GetDBKeys";
	
	new Ajax.Request(url, {
		method: "get",
		onSuccess: updateDBKeys,
		onFailure: reportError
	});
}

function updateDBKeys(transport) {
	var data = transport.responseJSON;
	($("configuration_description")).childElements().invoke("remove");
	
	data.systems.each(function(system) {
		var name = system.name;
		keyMap.set(name, $A(system.keys));
		
		($("configuration_description")).insert(new Element("option", {"value": name}).update(name));
	
	});

	($("configuration_description")).observe("change", updateKeySelector);
	updateKeySelector();
}

function updateKeySelector() {
	var keySelect = $("configuration_key");
	var keyName = ($("configuration_description")).value;
	keySelect.childElements().invoke("remove");
	keyMap.each(function(pair) {
		if (pair.key != keyName) return;
		pair.value.each(function(dbkey) {
			keySelect.insert(new Element("option", {"value": dbkey}).update(dbkey));
		});
		throw $break;
	});
}
