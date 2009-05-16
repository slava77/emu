/*****************************************************************************\
* $Id: monitor.js,v 1.4 2009/05/16 18:53:09 paste Exp $
\*****************************************************************************/

// Make a variable in global scope that tells us whether we should or shouldn't monitor
var monitor = false;
var enabled = false;
var ticks = 1;

Event.observe(window, "load", function(event) {
	
	// Toggle monitoring
	//$("start_stop").enable();
	enabled = true;
	$("start_stop").observe("click", function(ev) {
		if (enabled) {
			if (monitor) {
				monitor = false;
				enabled = false;
				ev.element().update("Stopping Monitoring");
			} else {
				$$(".crate_selection").each(function(element) {
					drawCartilage(element.readAttribute("crate"), element.value);
				});
				monitor = true;
				enabled = false;
				ticks = 1;
				new PeriodicalExecuter(countdown, 1);
				ev.element().update("Starting Monitoring");
			}
		}
	});

	$$(".crateSelection").each(function(element) {
		drawCartilage(element.readAttribute("crate"), element.value);
		drawDCCCartilage(element.readAttribute("crate"));
		element.observe("change", function(ev) {
			ticks = 1;
			var el = ev.element();
			drawCartilage(el.readAttribute("crate"), el.value);
		});
	});
});

function countdown(pe) {

	if (!monitor) {
		pe.stop();
		$("start_stop").update("Start Monitoring");
		var image = new Element("img", {"src": "/emu/emuDCS/FEDApps/images/playarrow.png", "width": "59", "height": "67", "style": "vertical-align: middle"});
		$("start_stop").insert({"top": image});
		enabled = true;
		if ($("countdown")) $("countdown").remove();
		ticks = 1;
		return;
	}
	if (!enabled) {
		$("start_stop").update("Stop Monitoring");
		var image = new Element("img", {"src": "/emu/emuDCS/FEDApps/images/pausebars.png", "width": "59", "height": "67", "style": "vertical-align: middle"});
		$("start_stop").insert({"top": image});
		enabled = true;
	}
	if (!$("countdown")) {
		var cd = new Element("div", {"id": "countdown"});
		$("start_stop").insert({"after": cd}); 
	}

	ticks--;
	if (ticks > 0) {
		$("countdown").update("Next update in " + ticks + " second" + (ticks == 1 ? "" : "s"));
	} else {
		ticks = 10;
		$("countdown").update("Updating now...");
		ajaxCrates();
	}
	
}

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

function drawCartilage(crate, what) {
	$$("tr.ddu td.data").each(function(element) {
		if (element.readAttribute("crate") != crate) return;
		var slot = element.readAttribute("slot");
		element.update("");
		var table = new Element("table", {"class": "board_data", "crate": crate, "slot": slot});

		if (what == "fibers") {
			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});
			for (var fiber = 0; fiber < 15; fiber++) {
				var header = new Element("td").update(fiber);
				var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_name", "style": "width: 6.67%"});
				var footer = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_status"});
				slotRow.insert(header);
				dataRow.insert(body);
				labelRow.insert(footer);
			}
			table.insert(slotRow);
			table.insert(dataRow);
			table.insert(labelRow);
			
		} else if (what == "counts") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			[{name: "DDUFPGA", title: "DDUFPGA"}, {name: "INFPGA01", title: "INFPGA0 (1-3)"}, {name: "INFPGA02", title: "INFPGA0 (4-7)"}, {name: "INFPGA11", title: "INFPGA1 (8-11)"}, {name: "INFPGA12", title: "INFPGA1 (12-14)"}].each(function (register) {
				var header = new Element("td").update(register.title);
				var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_register_" + register.name + "_count", "style": "width: 20%"});
				slotRow.insert(header);
				dataRow.insert(body);
			});

			table.insert(slotRow);
			table.insert(dataRow);
			
		} else if (what == "occupancies") {
		
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
				dataRow.insert(new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_name", "style": "width: 11%"}));

				["DMB", "TMB", "ALCT", "CFEB"].each(function(name) {
					var ele = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_" + name, "style": "width: 21%"});
					ele.insert(new Element("div", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_" + name + "_count"}));
					ele.insert(new Element("div", {"id": "crate_" + crate + "_slot_" + slot + "_fiber_" + fiber + "_" + name + "_percent"}));
					dataRow.insert(ele);
				});

				table.insert(dataRow);
			}
			
		} else if (what == "temperatures") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});

			for (var temp = 0; temp < 4; temp++) {
				var header = new Element("td").update("sensor " + temp);
				var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_temp_" + temp, "style": "width: 25%"});
				var footer = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_temp_" + temp + "_status"});
				slotRow.insert(header);
				dataRow.insert(body);
				labelRow.insert(footer);
			}

			table.insert(slotRow);
			table.insert(dataRow);
			table.insert(labelRow);

		} else if (what == "voltages") {

			var slotRow = new Element("tr", {"class": "headers"});
			var dataRow = new Element("tr", {"class": "data"});
			var labelRow = new Element("tr", {"class": "footers"});

			[{number: 0, name: "1.5V"},{number: 1, name: "2.5V (1)"},{number: 2, name: "2.5V (2)"},{number: 3, name: "3.3V"}].each(function(volt) {
				var header = new Element("td").update(volt.name);
				var body = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_volt_" + volt.number, "style": "width: 25%"});
				var footer = new Element("td", {"id": "crate_" + crate + "_slot_" + slot + "_volt_" + volt.number + "_status"});
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

function drawDCCCartilage(crate) {
	$$("tr.dcc td.data").each(function(element) {
		if (element.readAttribute("crate") != crate) return;
		var slot = element.readAttribute("slot");
		element.update("");
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

