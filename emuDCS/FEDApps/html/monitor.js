/*****************************************************************************\
* $Id: monitor.js,v 1.7 2009/07/01 14:54:02 paste Exp $
\*****************************************************************************/

Event.observe(window, "load", function(event) {

	// Don't know these a priori because the crate numbers are encoded herein
	$$(".loadtime").each(function(element) {
		var id = element.readAttribute("id");
		if (id == "FED_Monitor_Configuration_loadtime") return;
		id = id.substr(0, id.length - 9);
		
		var reloadElement = new ReloadElement();
		reloadElement.id = id;
		reloadElement.reloadFunction = getDDUMonitor;
		reloadElement.callbackSuccess = updateStatuses;
		// Special function
		reloadElement.updateDDUMonitor = updateDDUMonitor;
		// Special function
		reloadElement.updateDDUStatus = updateDDUStatus;
		// Special function
		reloadElement.updateDCCStatus = updateDCCStatus;
		reloadElement.callbackError = reportErrorAndStop;
		reloadElement.timeToReload = 10;
		reloadElement.timeToError = 60;
		reloadElements.push(reloadElement);
	});
	
	// Starting and stopping
	$$(".pause_button").each(function(element) {
		element.observe("click", function(event) {
			$$(".start_button").each(function(e) {
				if (e.name == element.name) e.disabled = false;
			});
			element.disabled = true;
			reloadElements.each(function(e) {
				if (element.name == e.id) e.stop = true;
			});
		});
	});
	$$(".start_button").each(function(element) {
		element.observe("click", function(event) {
			$$(".pause_button").each(function(e) {
				if (e.name == element.name) e.disabled = false;
			});
			element.disabled = true;
			reloadElements.each(function(e) {
				if (element.name == e.id) {
					e.stop = false;
					e.tick(true);
				}
			});
		});
	});
	
	// Updating monitor
	$$(".monitor_select").each(function(element) {
		element.observe("change", function(event) {
			reloadElements.each(function(e) {
				if (element.name == e.id) {
					e.tick(true);
				}
			});
		});
	});
	
	var configurationReloadElement = new ReloadElement();
	configurationReloadElement.id = "FED_Monitor_Configuration";
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
	
});

function getDDUMonitor() {
	
	// Bind the special callbacks
	var successCallback = this.updateDDUMonitor.bind(this);
	var errorCallback = this.callbackError.bind(this);
	
	// Loop through crates and get the DDU status based on what is selected
	$$(".monitor_select").each(function(element) {
		var crateNumber = element.readAttribute("crate");
		var url = URL + "/Get" + element.value;
		var params = {"CrateNumber": crateNumber};
		
		new Ajax.Request(url, {
			method: "get",
			parameters: params,
			onSuccess: successCallback,
			onFailure: errorCallback
		});
		
	});
}

function updateDDUMonitor(transport) {
	// Bind the special callbacks
	var successCallback = this.updateDDUStatus.bind(this);
	var errorCallback = this.callbackError.bind(this);
	
	var data = transport.responseJSON;
	
	var crateNumber = data.crate;
	
	var what = data.action;
	
	if (what == "Temperatures") updateTemperatures(data);
	if (what == "Voltages") updateVoltages(data);
	if (what == "Occupancies") updateOccupancies(data);
	if (what == "Counts") updateCounts(data);
	if (what == "FiberStatus") updateFiberStatus(data);
	
	// Now jump to updating the common DDU information
	var params = {"CrateNumber": crateNumber};
	var url = URL + "/GetDDUStatus";
	
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}

function updateDDUStatus(transport) {
	// Bind the special callbacks
	var successCallback = this.updateDCCStatus.bind(this);
	var errorCallback = this.callbackError.bind(this);
	
	var data = transport.responseJSON;
	
	var crateNumber = data.crate;
	data.ddus.each(function(board) {
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		$(crateSlot + "_slot").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(board.fmmStatus);
		
		$(crateSlot + "_l1a").update(board.L1A);
	});
	
	// Now jump to updating the DCC information
	var params = {"CrateNumber": crateNumber};
	var url = URL + "/GetDCCStatus";
	
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}

function updateDCCStatus(transport) {
	var data = transport.responseJSON;
	
	var crateNumber = data.crate;
	
	data.dccs.each(function(board) {
		var slot = board.slot;
		if (!$("dcc_cartilage_" + crateNumber + "_" + slot)) drawDCCCartilage(crateNumber, slot);
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		
		$(crateSlot + "_slot").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(board.fmmStatus);
		
		$(crateSlot + "_l1a").update(board.L1A);
		
		board.ddurates.each(function(ddurate) {
			var dduSlot = ddurate["slot"];
			$(crateSlot + "_fifo_" + dduSlot + "_rate").update(ddurate["rate"]);
			$(crateSlot + "_fifo_" + dduSlot + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(ddurate["status"]).update(ddurate["message"]);
		});
		board.slinkrates.each(function(slinkrate) {
			var slink = slinkrate["slink"];
			$(crateSlot + "_slink_" + slink + "_rate").update(slinkrate["rate"]);
			$(crateSlot + "_slink_" + slink + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(slinkrate["status"]).update(slinkrate["message"]);
		});
	});
	
	// Finally, we are done!
	var updateStatuses = this.callbackSuccess.bind(this);
	updateStatuses();
}

function updateStatuses() {
	this.reset();
}

function updateTemperatures(data) {
	var crateNumber = data.crate;
	
	data.ddus.each(function(board) {
		var slot = board.slot;
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Temperatures") drawDDUCartilage(crateNumber, slot, "Temperatures");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board.temperatures.each(function(temperature) {
			$(crateSlot + "_temp_" + temperature.number).update(temperature.temperature);
			$(crateSlot + "_temp_" + temperature.number + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(temperature.status).update(temperature.message);
		});
	});
}

function updateVoltages(data) {
	var crateNumber = data.crate;
	
	data.ddus.each(function(board) {
		var slot = board.slot;
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Voltages") drawDDUCartilage(crateNumber, slot, "Voltages");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board.voltages.each(function(voltage) {
			$(crateSlot + "_volt_" + voltage.number).update(voltage.voltage);
			$(crateSlot + "_volt_" + voltage.number + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(voltage.status).update(voltage.message);
		});
	});
}

function updateOccupancies(data) {
	var crateNumber = data.crate;
	
	data.ddus.each(function(board) {
		var slot = board.slot;
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Occupancies") drawDDUCartilage(crateNumber, slot, "Occupancies");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;

		board.fibers.each(function(fiber) {
			crateSlotFiber = crateSlot + "_fiber_" + fiber.fiber;
			$(crateSlotFiber + "_name").update(fiber.name);
			
			fiber.occupancies.each(function(occupancy) {
				$(crateSlotFiber + "_" + occupancy.type + "_count").update(occupancy.count);
				$(crateSlotFiber + "_" + occupancy.type + "_percent").update(occupancy.percent);
			});
		});
	});
}

function updateCounts(data) {
	var crateNumber = data.crate;
	
	data.ddus.each(function(board) {
		var slot = board.slot;
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Counts") drawDDUCartilage(crateNumber, slot, "Counts");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board.counts.each(function(count) {
			$(crateSlot + "_register_" + count.name + "_count").update(count.count);
		});
	});
}

function updateFiberStatus(data) {
	var crateNumber = data.crate;
	
	data.ddus.each(function(board) {
		var slot = board.slot;
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "FiberStatus") drawDDUCartilage(crateNumber, slot, "FiberStatus");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board.fibers.each(function(fiber) {
			$(crateSlot + "_fiber_" + fiber.number + "_name").update(fiber.name);
			$(crateSlot + "_fiber_" + fiber.number + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(fiber.status).update(fiber.message);
		});
	});
}

function drawDDUCartilage(crate, slot, what) {
	$$("tr.ddu td.data").each(function(element) {
		if (element.readAttribute("crate") != crate || element.readAttribute("slot") != slot) return;

		element.update("");
		
		element.insert(new Element("span", {"id": "ddu_cartilage_" + crate + "_" + slot, "style": "display: none;", "what": what}));
		
		var crateSlot = "crate_" + crate + "_slot_" + slot;
		
		var table = new Element("table", {"class": "board_data", "crate": crate, "slot": slot});

		if (what == "FiberStatus") {
			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});
			for (var fiber = 0; fiber < 15; fiber++) {
				var header = new Element("td").update(fiber);
				var body = new Element("td", {"id": crateSlot + "_fiber_" + fiber + "_name", "style": "width: 6.67%"});
				var footer = new Element("td", {"id": crateSlot + "_fiber_" + fiber + "_status"});
				slotRow.insert(header);
				dataRow.insert(body);
				labelRow.insert(footer);
			}
			table.insert(slotRow);
			table.insert(dataRow);
			table.insert(labelRow);
			
		} else if (what == "Counts") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			[{name: "DDUFPGA", title: "DDUFPGA"}, {name: "INFPGA01", title: "INFPGA0 (1-3)"}, {name: "INFPGA02", title: "INFPGA0 (4-7)"}, {name: "INFPGA11", title: "INFPGA1 (8-11)"}, {name: "INFPGA12", title: "INFPGA1 (12-14)"}].each(function (register) {
				var header = new Element("td").update(register.title);
				var body = new Element("td", {"id": crateSlot + "_register_" + register.name + "_count", "style": "width: 20%"});
				slotRow.insert(header);
				dataRow.insert(body);
			});

			table.insert(slotRow);
			table.insert(dataRow);
			
		} else if (what == "Occupancies") {
		
			var headRow = new Element("tr", {"class": "headers"});
			headRow.insert(new Element("td", {"style": "width: 5%"}).update("fiber"));
			headRow.insert(new Element("td", {"style": "width: 11%"}).update("chamber"));
			headRow.insert(new Element("td", {"style": "width: 21%"}).update("DMB"));
			headRow.insert(new Element("td", {"style": "width: 21%"}).update("TMB"));
			headRow.insert(new Element("td", {"style": "width: 21%"}).update("ALCT"));
			headRow.insert(new Element("td", {"style": "width: 21%"}).update("CFEB"));
			table.insert(headRow);

			for (var fiber = 0; fiber < 15; fiber++) {
				var dataRow = new Element("tr", {"class": "occupancy_data"});
				dataRow.insert(new Element("td", {"style": "width: 5%"}).update(fiber));
				dataRow.insert(new Element("td", {"id": crateSlot + "_fiber_" + fiber + "_name", "style": "width: 11%"}));

				["DMB", "TMB", "ALCT", "CFEB"].each(function(name) {
					var ele = new Element("td", {"id": crateSlot + "_fiber_" + fiber + "_" + name, "style": "width: 21%"});
					ele.insert(new Element("div", {"id": crateSlot + "_fiber_" + fiber + "_" + name + "_count"}));
					ele.insert(new Element("div", {"id": crateSlot + "_fiber_" + fiber + "_" + name + "_percent"}));
					dataRow.insert(ele);
				});

				table.insert(dataRow);
			}
			
		} else if (what == "Temperatures") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});

			for (var temp = 0; temp < 4; temp++) {
				var header = new Element("td").update("sensor " + temp);
				var body = new Element("td", {"id": crateSlot + "_temp_" + temp, "style": "width: 25%"});
				var footer = new Element("td", {"id": crateSlot + "_temp_" + temp + "_status"});
				slotRow.insert(header);
				dataRow.insert(body);
				labelRow.insert(footer);
			}

			table.insert(slotRow);
			table.insert(dataRow);
			table.insert(labelRow);

		} else if (what == "Voltages") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});

			[{number: 0, name: "1500 mV"},{number: 1, name: "2500 mV"},{number: 2, name: "2500 mV"},{number: 3, name: "3300 mV"}].each(function(volt) {
				var header = new Element("td").update(volt.name);
				var body = new Element("td", {"id": crateSlot + "_volt_" + volt.number, "style": "width: 25%"});
				var footer = new Element("td", {"id": crateSlot + "_volt_" + volt.number + "_status"});
				slotRow.insert(header);
				dataRow.insert(body);
				labelRow.insert(footer);
			});

			table.insert(slotRow);
			table.insert(dataRow);
			table.insert(labelRow);

		}

		element.insert(table);
	});
	
}

function drawDCCCartilage(crate, slot) {
	$$("tr.dcc td.data").each(function(element) {
		if (element.readAttribute("crate") != crate || element.readAttribute("slot") != slot) return;
		
		element.update("");
		
		element.insert(new Element("span", {"id": "dcc_cartilage_" + crate + "_" + slot, "style": "display: none;"}));
		
		var table = new Element("table", {"class": "board_data", "crate": crate, "slot": slot});

		var table = new Element("table", {"class": "board_data"});
		var slotRow = new Element("tr", {"class": "headers"});
		var dataRow = new Element("tr", {"class": "data"});
		var labelRow = new Element("tr", {"class": "footers"});

		var firstfifo = (slot == 17) ? 14 : 3;
		var lastfifo = (slot == 17) ? 21 : 14;

		for (var fifo = firstfifo; fifo < lastfifo; fifo++) {
			if (fifo == 8 || fifo == 17) continue;

			var header = new Element("td", {"style": "width: 8.3333%;"}).update("slot " + fifo);
			var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fifo_" + fifo + "_rate"});
			var footer = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fifo_" + fifo + "_status"});
			slotRow.insert(header);
			dataRow.insert(body);
			labelRow.insert(footer);
		}

		for (var slink = 1; slink < 3; slink++) {
			var style = "";
			if (slink == 1) style = "border-left: 2px solid #000";
			var header = new Element("td", {"style": "width: 8.3333%;" + style}).update("SLink " + slink);
			var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_slink_" + slink + "_rate", "style": style});
			var footer = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_slink_" + slink + "_status", "style": style});
			slotRow.insert(header);
			dataRow.insert(body);
			labelRow.insert(footer);
		}
		table.insert(slotRow).insert(dataRow).insert(labelRow);
		element.insert(table);
	});
}
