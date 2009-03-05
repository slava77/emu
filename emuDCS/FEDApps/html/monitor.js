/*****************************************************************************\
* $Id: monitor.js,v 1.1 2009/03/05 16:18:23 paste Exp $
*
* $Log: monitor.js,v $
* Revision 1.1  2009/03/05 16:18:23  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
\*****************************************************************************/

// Make a variable in global scope that tells us whether we should or shouldn't monitor
var monitor = false;
var enabled = false;

Event.observe(window, "load", function(event) {
	
	// Toggle monitoring
	//$("start_stop").enable();
	enabled = true;
	$("start_stop").observe("click", function(ev) {
		if (enabled) {
			if (monitor) {
				monitor = false;
				ev.element().update("Stopping Monitoring");
				enabled = false;
			} else {
				monitor = true;
				new PeriodicalExecuter(ajaxCrates, 10);
				ev.element().update("Starting Monitoring");
				enabled = false;
			}
		}
	});
	
});

function ajaxCrates(pe) {

	if (!monitor) {
		pe.stop();
		$("start_stop").update("Start Monitoring");
		var image = new Element("img", {"src": "/emu/images/playarrow.png", "width": "59", "height": "67", "style": "vertical-align: middle"});
		$("start_stop").insert({"top": image});
		enabled = true;
		return;
	}
	$("start_stop").update("Stop Monitoring");
	var image = new Element("img", {"src": "/emu/images/pausebars.png", "width": "59", "height": "67", "style": "vertical-align: middle"});
	$("start_stop").insert({"top": image});
	enabled = true;
	
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
	$("start_stop").update("Stopping Monitoring");
	enabled = false;
}

function updateCrate(transport) {
	//$("debug").update(transport.responseText);
	var data = transport.responseJSON;
	if (!data) return errorFunction(transport);
	
	var crateNumber = data.number;
	data.boards.each( function(board) {
		$("crate_" + crateNumber + "_slot_" + board.slot + "_status").removeClassName("ok").removeClassName("warning").removeClassName("error").removeClassName("caution").removeClassName("undefined").addClassName(board.fmmStatus);
		
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
	var table = new Element("table", {"class": "board_data"});
	var slotRow = new Element("tr", {"class": "headers"});
	var dataRow = new Element("tr", {"class": "data"});
	var labelRow = new Element("tr", {"class": "footers"});
	board.ddurates.each(function(ddurate) {
		var header = new Element("td").update("slot " + ddurate.slot);
		var body = new Element("td").update(ddurate.rate);
		var footer = new Element("td", {"class": ddurate.status}).update(ddurate.message);
		slotRow.insert(header);
		dataRow.insert(body);
		labelRow.insert(footer);
	});
	board.slinkrates.each(function(slinkrate) {
		var style = "";
		if (slinkrate.slink == 1) style = "border-left: 2px solid #000";
		var header = new Element("td", {"style": style}).update("SLink " + slinkrate.slink);
		var body = new Element("td", {"style": style}).update(slinkrate.rate);
		var footer = new Element("td", {"style": style, "class": slinkrate.status}).update(slinkrate.message);
		slotRow.insert(header);
		dataRow.insert(body);
		labelRow.insert(footer);
	});
	table.insert(slotRow);
	table.insert(dataRow);
	table.insert(labelRow);
	$(crateSlot + "_data").update("").insert(table);
	
}

function updateFibers(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		var table = new Element("table", {"class": "board_data"});
		var slotRow = new Element("tr", {"class": "headers"});
		var dataRow = new Element("tr", {"class": "data"});
		var labelRow = new Element("tr", {"class": "footers"});
		board.fibers.each(function(fiber) {
			var header = new Element("td").update(fiber.fiber);
			var body = new Element("td", {"style": "width: 6.67%"}).update(fiber.name);
			var footer = new Element("td", {"class": fiber.status}).update(fiber.message);
			slotRow.insert(header);
			dataRow.insert(body);
			labelRow.insert(footer);
		});
		table.insert(slotRow);
		table.insert(dataRow);
		table.insert(labelRow);
		$(crateSlot + "_data").update("").insert(table);
	});
}

function updateCounts(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		var table = new Element("table", {"class": "board_data"});
		var slotRow = new Element("tr", {"class": "headers"});
		var dataRow = new Element("tr", {"class": "data"});
		board.counts.each(function(count) {
			var header = new Element("td").update(count.name);
			var body = new Element("td", {"style": "width: 20%"}).update(count.count);
			slotRow.insert(header);
			dataRow.insert(body);
		});
		table.insert(slotRow);
		table.insert(dataRow);
		$(crateSlot + "_data").update("").insert(table);
	});
}

function updateOccupancies(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		var table = new Element("table", {"class": "board_data"});
		var headRow = new Element("tr", {"class": "headers"});
		headRow.insert(new Element("td", {"style": "width: 10%"}).update("fiber"));
		headRow.insert(new Element("td", {"style": "width: 10%"}).update("chamber"));
		headRow.insert(new Element("td", {"style": "width: 20%"}).update("DMB"));
		headRow.insert(new Element("td", {"style": "width: 20%"}).update("TMB"));
		headRow.insert(new Element("td", {"style": "width: 20%"}).update("ALCT"));
		headRow.insert(new Element("td", {"style": "width: 20%"}).update("CFEB"));
		table.insert(headRow);
		
		board.occupancies.each(function(occupancy) {
			var dataRow = new Element("tr", {"class": "occupancy_data"});
			dataRow.insert(new Element("td", {"style": "width: 10%"}).update(occupancy.fiber));
			dataRow.insert(new Element("td", {"style": "width: 10%"}).update(occupancy.name));
			dataRow.insert(new Element("td", {"style": "width: 20%"}).update(occupancy.DMBcount + "<br />" + occupancy.DMBpercent));
			dataRow.insert(new Element("td", {"style": "width: 20%"}).update(occupancy.TMBcount + "<br />" + occupancy.TMBpercent));
			dataRow.insert(new Element("td", {"style": "width: 20%"}).update(occupancy.ALCTcount + "<br />" + occupancy.ALCTpercent));
			dataRow.insert(new Element("td", {"style": "width: 20%"}).update(occupancy.CFEBcount + "<br />" + occupancy.CFEBpercent));
			table.insert(dataRow);
		});
		$(crateSlot + "_data").update("").insert(table);
	});
}

function updateTemperatures(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		var table = new Element("table", {"class": "board_data"});
		var slotRow = new Element("tr", {"class": "headers"});
		var dataRow = new Element("tr", {"class": "data"});
		var labelRow = new Element("tr", {"class": "footers"});
		board.temperatures.each(function(temperature) {
			var header = new Element("td").update(temperature.name);
			var body = new Element("td", {"style": "width: 25%"}).update(temperature.temperature);
			var footer = new Element("td", {"class": temperature.status}).update(temperature.message);
			slotRow.insert(header);
			dataRow.insert(body);
			labelRow.insert(footer);
		});
		table.insert(slotRow);
		table.insert(dataRow);
		table.insert(labelRow);
		$(crateSlot + "_data").update("").insert(table);
	});
}

function updateVoltages(data) {
	var crateNumber = data.number;
	data.boards.each( function(board) {
		if (board.type != "DDU") return;
		var crateSlot = "crate_" + crateNumber + "_slot_" + board.slot;
		var table = new Element("table", {"class": "board_data"});
		var slotRow = new Element("tr", {"class": "headers"});
		var dataRow = new Element("tr", {"class": "data"});
		var labelRow = new Element("tr", {"class": "footers"});
		board.voltages.each(function(voltage) {
			var header = new Element("td").update(voltage.name);
			var body = new Element("td", {"style": "width: 25%"}).update(voltage.voltage);
			var footer = new Element("td", {"class": voltage.status}).update(voltage.message);
			slotRow.insert(header);
			dataRow.insert(body);
			labelRow.insert(footer);
		});
		table.insert(slotRow);
		table.insert(dataRow);
		table.insert(labelRow);
		$(crateSlot + "_data").update("").insert(table);
	});
}