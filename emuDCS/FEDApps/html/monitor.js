/*****************************************************************************\
* $Id: monitor.js,v 1.5 2009/05/20 18:18:37 paste Exp $
\*****************************************************************************/

var reloadElements = new Array();

Event.observe(window, "load", function(event) {

	// Update DDU and DCC status separately
	getDDUStatus();
	new PeriodicalExecuter(getDDUStatus, reloadSecs);
	getDCCStatus();
	new PeriodicalExecuter(getDCCStatus, reloadSecs);

	// Don't know these a priori
	$$(".loadtime").each(function(element) {
		var id = element.readAttribute("id");
		id = id.substr(0, id.length - 9);
		reloadElements.push(id);
		timeSinceReload[id] = 0;
	});
	
	// Starting and stopping
	$$(".pause_button").each(function(element) {
		element.observe("click", function(event) {
			$$(".start_button").each(function(element) { element.disabled = false; });
			$$(".pause_button").each(function(element) { element.disabled = true; });
			stop = true;
		});
	});
	$$(".start_button").each(function(element) {
		element.observe("click", function(event) {
			$$(".start_button").each(function(element) { element.disabled = true; });
			$$(".pause_button").each(function(element) { element.disabled = false; });
			stop = false;
			getStatus();
		});
	});
	
	// Updating monitor
	$$(".monitor_select").each(function(element) {
		element.observe("change", function(event) {
			getStatus();
		});
	});
	
});

function getStatus(pe) {
	if (stop) {
		return;
	}
	// Loop through crates
	$$(".monitor_select").each(function(element) {
		var crateNumber = element.readAttribute("crate");
		var url = URL + "/Get" + element.value;
		var params = {"CrateNumber": crateNumber};
		
		$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
		
		new Ajax.Request(url, {
			method: "get",
			parameters: params,
			onSuccess: updateStatus,
			onFailure: reportError
		});
		
	});
}

function updateStatus(transport) {
	var data = transport.responseJSON;
	
	var crateNumber = data["crate"];
	
	var what = data["action"];
	
	if (what == "Temperatures") updateTemperatures(data);
	if (what == "Voltages") updateVoltages(data);
	if (what == "Occupancies") updateOccupancies(data);
	if (what == "Counts") updateCounts(data);
	if (what == "FiberStatus") updateFiberStatus(data);
	
	$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
	
	timeSinceReload["FED_Monitor_" + crateNumber] = 0;
	updateTimes();
}

function updateTemperatures(data) {
	var crateNumber = data["crate"];
	
	data["ddus"].each(function(board) {
		var slot = board["slot"];
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Temperatures") drawDDUCartilage(crateNumber, slot, "Temperatures");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board["temperatures"].each(function(temperature) {
			$(crateSlot + "_temp_" + temperature["number"]).update(temperature["temperature"]);
			$(crateSlot + "_temp_" + temperature["number"] + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(temperature["status"]).update(temperature["message"]);
		});
	});
}

function updateVoltages(data) {
	var crateNumber = data["crate"];
	
	data["ddus"].each(function(board) {
		var slot = board["slot"];
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Voltages") drawDDUCartilage(crateNumber, slot, "Voltages");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board["voltages"].each(function(voltage) {
			$(crateSlot + "_volt_" + voltage["number"]).update(voltage["voltage"]);
			$(crateSlot + "_volt_" + voltage["number"] + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(voltage["status"]).update(voltage["message"]);
		});
	});
}

function updateOccupancies(data) {
	var crateNumber = data["crate"];
	
	data["ddus"].each(function(board) {
		var slot = board["slot"];
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Occupancies") drawDDUCartilage(crateNumber, slot, "Occupancies");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;

		board["fibers"].each(function(fiber) {
			crateSlotFiber = crateSlot + "_fiber_" + fiber["fiber"];
			$(crateSlotFiber + "_name").update(fiber["name"]);
			
			fiber["occupancies"].each(function(occupancy) {
				$(crateSlotFiber + "_" + occupancy["type"] + "_count").update(occupancy["count"]);
				$(crateSlotFiber + "_" + occupancy["type"] + "_percent").update(occupancy["percent"]);
			});
		});
	});
}

function updateCounts(data) {
	var crateNumber = data["crate"];
	
	data["ddus"].each(function(board) {
		var slot = board["slot"];
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "Counts") drawDDUCartilage(crateNumber, slot, "Counts");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board["counts"].each(function(count) {
			$(crateSlot + "_register_" + count["name"] + "_count").update(count["count"]);
		});
	});
}

function updateFiberStatus(data) {
	var crateNumber = data["crate"];
	
	data["ddus"].each(function(board) {
		var slot = board["slot"];
		if (!$("ddu_cartilage_" + crateNumber + "_" + slot) || $("ddu_cartilage_" + crateNumber + "_" + slot).readAttribute("what") != "FiberStatus") drawDDUCartilage(crateNumber, slot, "FiberStatus");
		
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		board["fibers"].each(function(fiber) {
			$(crateSlot + "_fiber_" + fiber["number"] + "_name").update(fiber["name"]);
			$(crateSlot + "_fiber_" + fiber["number"] + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(fiber["status"]).update(fiber["message"]);
		});
	});
}

function getDDUStatus(pe) {
	if (stop) {
		return;
	}
	// Loop through crates
	$$(".monitor_select").each(function(element) {
		var crateNumber = element.readAttribute("crate");
		var url = URL + "/GetDDUStatus";
		var params = {"CrateNumber": crateNumber};
		
		$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
		
		new Ajax.Request(url, {
			method: "get",
			parameters: params,
			onSuccess: updateDDU,
			onFailure: reportError
		});
		
	});
}

function getDCCStatus(pe) {
	if (stop) {
		return;
	}
	// Loop through crates
	$$(".monitor_select").each(function(element) {
		var crateNumber = element.readAttribute("crate");
		var url = URL + "/GetDCCStatus";
		var params = {"CrateNumber": crateNumber};
		
		$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
		
		new Ajax.Request(url, {
			method: "get",
			parameters: params,
			onSuccess: updateDCC,
			onFailure: reportError
		});
		
	});
}

function updateDDU(transport) {
	var data = transport.responseJSON;
	
	var crateNumber = data["crate"];
	data["ddus"].each(function(board) {
		var crateSlot = "crate_" + crateNumber + "_slot_" + board["slot"];
		$(crateSlot + "_slot").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(board["fmmStatus"]);
		
		$(crateSlot + "_l1a").update(board["L1A"]);
	});
	
	$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
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

			[{number: 0, name: "1.5V"},{number: 1, name: "2.5V (1)"},{number: 2, name: "2.5V (2)"},{number: 3, name: "3.3V"}].each(function(volt) {
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

function updateDCC(transport) {
	var data = transport.responseJSON;
	
	var crateNumber = data["crate"];
	
	data["dccs"].each(function(board) {
		var slot = board["slot"];
		if (!$("dcc_cartilage_" + crateNumber + "_" + slot)) drawDCCCartilage(crateNumber, slot);
		var crateSlot = "crate_" + crateNumber + "_slot_" + slot;
		
		$(crateSlot + "_slot").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(board["fmmStatus"]);
		
		$(crateSlot + "_l1a").update(board["L1A"]);
		
		board["ddurates"].each(function(ddurate) {
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
	
	$("FED_Monitor_" + crateNumber + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
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

/*
function ajaxCrates() {
	
	var url = URL + "/getAJAX";
	$$(".crate_form").each(function(crateForm) {
		var params = crateForm.serialize(true);
		new Ajax.Request(url,
			{
				method: "get",
				parameters: params,
				onSuccess: updateCrate,
				onFailure: errorFunction
			}
		);
	});
}

function errorFunction(transport) {
	$$("td.data").each(function(element) {
		element.update("Error in AJAX communication");
	});
	monitor = false;
	enabled = false;
	ticks = 10;
	$("start_stop").update("Stopping Monitoring");
}

function updateCrate(transport) {
	//$("debug").update(transport.responseText);
	var data = transport.responseJSON;
	if (!data) return errorFunction(transport);
	
	var crateNumber = data.number;
	data.boards.each( function(board) {
		$("crate_" + crateNumber + "_slot_" + board.slot + "_slot").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(board.fmmStatus);
		
		$("crate_" + crateNumber + "_slot_" + board.slot + "_l1a").update(board.L1A);
		
		if (board.type == "DCC") updateDataRates(crateNumber, board);
	});
	
	if (data.what == "fibers") return updateFibers(data);
	else if (data.what == "counts") return updateCounts(data);
	else if (data.what == "occupancies") return updateOccupancies(data);
	else if (data.what == "temperatures") return updateTemperatures(data);
	else if (data.what == "voltages") return updateVoltages(data);
	else return errorFunction(transport);
}

function updateDataRates(crateNumber, board) {
	var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
	board.ddurates.each(function(ddurate) {
		$(crateSlot + "_fifo_" + ddurate.slot + "_rate").update(ddurate.rate);
		$(crateSlot + "_fifo_" + ddurate.slot + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(ddurate.status).update(ddurate.message);
	});
	board.slinkrates.each(function(slinkrate) {
		$(crateSlot + "_slink_" + slinkrate.slink + "_rate").update(slinkrate.rate);
		$(crateSlot + "_slink_" + slinkrate.slink + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(slinkrate.status).update(slinkrate.message);
	});
}

function updateFibers(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		board.fibers.each(function(fiber) {
			if (fiber.fiber == null) return;
			$(crateSlot + "_fiber_" + fiber.fiber + "_name").update(fiber.name);
			$(crateSlot + "_fiber_" + fiber.fiber + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("error_black").removeClassName("caution").removeClassName("undefined").addClassName(fiber.status).update(fiber.message);
		});
	});
}

function updateCounts(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		board.counts.each(function(count) {
			if (count.count == null) return;
			$(crateSlot + "_register_" + count.name + "_count").update(count.count);
		});
	});
}

function updateOccupancies(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;

		board.occupancies.each(function(occupancy) {
			$(crateSlot + "_fiber_" + occupancy.fiber + "_name").update(occupancy.name);
			occupancy.numbers.each(function(number) {
				if (number.count == null) return;
				$(crateSlot + "_fiber_" + occupancy.fiber + "_" + number.type + "_count").update(number.count);
				$(crateSlot + "_fiber_" + occupancy.fiber + "_" + number.type + "_percent").update(number.percent);
			});
		});
	});
}

function updateTemperatures(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		board.temperatures.each(function(temperature) {
			if (temperature.temperature == null) return;
			$(crateSlot + "_temp_" + temperature.number).update(temperature.temperature);
			$(crateSlot + "_temp_" + temperature.number + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(temperature.status).update(temperature.message);
		});
	});
}

function updateVoltages(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		board.voltages.each(function(voltage) {
			if (voltage.voltage == null) return;
			$(crateSlot + "_volt_" + voltage.number).update(voltage.voltage);
			$(crateSlot + "_volt_" + voltage.number + "_status").removeClassName("green").removeClassName("orange").removeClassName("red").removeClassName("yellow").removeClassName("blue").removeClassName("undefined").addClassName(voltage.status).update(voltage.message);
		});
	});
}




*/

