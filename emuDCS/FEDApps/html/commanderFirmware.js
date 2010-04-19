/*****************************************************************************\
* $Id: commanderFirmware.js,v 1.4 2010/04/19 15:30:35 paste Exp $
\*****************************************************************************/

// Global for the required pause in uploading
var uploading = false;

function reportError(transport) {
	//this.stop = true;
	alert("The following error has occurred: " + transport.statusText + ".  An expert should be contacted.");
}

// This is universal between DDUs and DCCs.  Be careful!

Event.observe(window, "load", function(event) {

	// Hide targets
	$$(".hidden").invoke("hide");

	// When the broadcast checkbox is checked, disable non-broadcast checkboxes of the same chip type
	$$("input.broadcast").each(function(e) {
		e.checked = false;
		e.observe("change", function(ev) {
			if (ev.element().checked) {
				$$("input[chip=\"" + ev.element().readAttribute("chip") + "\"]").each(function(el) {
					if (!el.hasClassName("broadcast")) el.disabled = true;
				});
			} else {
				$$("input[chip=\"" + ev.element().readAttribute("chip") + "\"]").each(function(el) {
					if (!el.hasClassName("broadcast")) el.enable();
				});
			}
		});
	});

	// When any checkmark is checked, make sure that there is a place to upload a file.  If not, show it.
	// Clear all checkboxes, too.
	$$("input[type=\"checkbox\"]").each(function(e) {
		e.disabled = false;
		e.checked = false;
		e.observe("change", function(ev) {
			if (ev.element().checked && !$(ev.element().readAttribute("chip") + "_target").visible()) {
				$(ev.element().readAttribute("chip") + "_target").show()
			} else if (!ev.element().checked && $(ev.element().readAttribute("chip") + "_target").visible()) {
				// Make sure no other elements with that chip are checked before hiding
				var nChecked = false;
				$$("input[chip=\"" + ev.element().readAttribute("chip") + "\"]").each(function(el) {
					if (el.checked) {
						nChecked = true;
						throw $break;
					}
				});
				// Hide the element
				if (!nChecked) $(ev.element().readAttribute("chip") + "_target").hide();
			}
		});
	});

	$$(".upload").each(function(e) {

		// Disable all upload buttons to begin with.  The updater will enable them.
		e.disabled = true;

		var chip = e.readAttribute("chip");

		// Intercept clicks, add form data and disable buttons appropriately.
		e.observe("click", function(ev) {
			// Make sure the updater knows that we are uploading
			uploading = true;
			$$(".form_hidden").invoke("remove");
			var form = $(chip + "_form");
			var crateEle = new Element("input", {"class": "form_hidden", name: "crate", value: crateNumber, type: "hidden"});
			form.insert(crateEle);
			var boardEle = new Element("input", {"class": "form_hidden", name: "board", value: boardType, type: "hidden"});
			form.insert(boardEle);
			var chipEle = new Element("input", {"class": "form_hidden", name: "chip", value: chip, type: "hidden"});
			form.insert(chipEle);
			$$("input[chip=\"" + chip + "\"]").each(function(el) {
				if (el.checked) {
					var slotEle = new Element("input", {"class": "form_hidden", name: "slot", value: el.readAttribute("slot"), type: "hidden"});
					form.insert(slotEle);
				}
			});
			var broadcast = $("broadcast_" + chip).checked ? 1 : 0;
			var broadcastEle = new Element("input", {"class": "form_hidden", name: "broadcast", value: broadcast, type: "hidden"});
			form.insert(broadcastEle);
			form.submit();
		});

	});

	// Start an updater to make sure uploads do not collide
	new PeriodicalExecuter(firmwareCheck, 1);

});

function firmwareCheck(pe)
{

	// Get the firmware completion data
	var url = URL + "/FirmwareCheck";
	var params = new Hash();
	params.set("crate", crateNumber);
	params.set("board", boardType);

	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: updateFirmware,
		onFailure: reportError
	});

}

function updateFirmware(transport)
{

	var data = transport.responseJSON;

	var percents = new Hash();

	var alldone = 0;

	// Loop through the returned boards
	data.boards.each(function(board) {

		var slot = board.slot;

		// Loop through the returned chips
		board.chips.each(function(chip) {

			// Update the text in the table
			$(chip.name + "_label_" + slot).update(chip.text);

			// Check to see if this is the smallest % done for all chips of this type
			if (!percents.get(chip.name) || chip.percent < percents.get(chip)) {
				percents.set(chip.name, chip.percent);
			}

			if (chip.percent == 100) alldone++;

		});

	});

	// Only allow more uploads if this is 100% of everybody.
	var disable = (alldone != percents.size());

	// Now update the submit buttons
	percents.each(function(it) {

		if (uploading && it.value == 100) {
			$(it.key + "_upload").update("Uploading...").disabled = true;
		} else if (it.value == 100) {
			$(it.key + "_upload").update("Upload and install").disabled = disable;
		} else {
			$(it.key + "_upload").update("Installing " + it.value + "%").disabled = true;
			uploading = false;
		}
	});

}


/*
function readValues()
{
	// Bind the special callbacks
	var successCallback = this.callbackSuccess.bind(this);
	var errorCallback = this.callbackError.bind(this);

	var url = URL + "/Read" + this.params.board.toUpperCase() + "Registers";

	new Ajax.Request(url, {
		method: "get",
		parameters: this.params,
		onSuccess: successCallback,
		onFailure: errorCallback
	});
}



function updateValues(transport)
{
	var data = transport.responseJSON;
	if (data.ddus) {
		if ($("display_select").value == "text" || this.makeTable) {
			this.makeTable = false;
			makeDDUTable(data);
		} else updateDDUs(data);
	} else if (data.dccs) {
		if ($("display_select").value == "text" || this.makeTable) {
			this.makeTable = false;
			makeDCCTable(data);
		} else updateDCCs(data);
	} else {
		$("content").update("Bad result!  Nothing to display!");
	}

	this.reset();
}



function makeDDUTable(data)
{
	var debug = true;
	if (data.ddus.size() > 1 || $("display_select").value == "text") debug = false;
	var table = new Table({"id": "data_display", "class": "data_table"});
	var header = new Row({"id": "data_header", "class": "data_header"});

	if (debug) {
		header.cells.push(new Cell({"id": "board_name", "class": "data_board"}).update("DDU " + data.ddus[0].rui));
		header.cells.push(new Cell({"id": "data_title", "class": "data_title"}).update("Register Value"));
		header.cells.push(new Cell({"id": "data_debug", "class": "data_debug"}).update("Description"));
	} else {
		header.cells.push(new Cell({"id": "empty_cell", "class": "empty"}));
		data.ddus.each(function(ddu) {
			header.cells.push(new Cell({"id": "board_name_" + ddu.rui, "class": "data_board", "rui": ddu.rui}).update("DDU " + ddu.rui));
		});
	}
	table.rows.push(header);

	// Entries are always in order.
	data.entries.each(function(entry, i) {
		var row = new Row({"id": "data_" + i, "class": "data_entry"});
		row.cells.push(new Cell({"id": "entry_name_" + i, "class": "entry_name"}).update(entry.name));

		entry.values.each(function(val) {
			var value = val.value;
			var base = val.base;
			if (base) value = value.toString(base);
			if (base == 16) value = "0x" + value.toUpperCase();
			else if (base == 8) value = "0" + value;

			row.cells.push(new Cell({"id": "entry_value_" + i + "_" + val.rui, "class": "entry_value", "base": base}).update(value));
			if (debug) return;
		});
		if (debug) {
			var cell = new Cell({"id": "entry_debug_" + i, "class": "entry_debug"});
			if (entry.descriptions.size()) cell.update(entry.descriptions[0].value);
			row.cells.push(cell);
		}
		table.rows.push(row);
	});

	if ($("display_select").value == "text") $("content").update(new Element("pre").update(table.toText()));
	else {
		$("content").update(table.toElement());

		// Add DDU-selecting functions
		if (!debug) {
			$$(".data_board").each(function(element) {
				element.observe("click", function(event) {
					reloadElements.each(function(re) {
						var newParams = new Object();
						newParams.rui = event.element().readAttribute("rui");
						newParams.reg = re.params.reg; // An array.
						newParams.board = re.params.board;
						re.params = newParams;
						re.makeTable = true;
						re.tick(true);
					});
				});
			});
		} else {
			$$(".data_board").each(function(element) {
				element.observe("click", function(event) {
					reloadElements.each(function(re) {
						re.params = window.location.href.parseQuery();
						re.params["reg"] = regs; // Defined in Commander.cc
						re.params["rui"] = ruis; // Defined in Commander.cc
						re.params["fmmid"] = fmmids; // Defined in Commander.cc
						re.makeTable = true;
						re.tick(true);
					});
				});
			});
		}
	}

}



function updateDDUs(data)
{
	if ($("display_select").value == "text") return makeDDUTable(data);
	var debug = true;
	if (data.ddus.size() > 1) debug = false;

	// Entries are always in order.
	data.entries.each(function(entry, i) {

		entry.values.each(function(val) {
			var value = val.value;
			var base = val.base;
			if (base) value = value.toString(base);
			if (base == 16) value = "0x" + value.toUpperCase();
			else if (base == 8) value = "0" + value;

			$("entry_value_" + i + "_" + val.rui).update(value);
			if (debug) return;
		});
		if (debug && entry.descriptions.size()) {
			$("entry_debug_" + i).update(entry.descriptions[0].value);
		}

	});

}

function makeDCCTable(data)
{
	var debug = true;
	if (data.ddus.size() > 1 || $("display_select").value == "text") debug = false;
	var table = new Table({"id": "data_display", "class": "data_table"});
	var header = new Row({"id": "data_header", "class": "data_header"});

	if (debug) {
		header.cells.push(new Cell({"id": "board_name", "class": "data_board"}).update("DCC " + data.dccs[0].fmmid));
		header.cells.push(new Cell({"id": "data_title", "class": "data_title"}).update("Register Value"));
		header.cells.push(new Cell({"id": "data_debug", "class": "data_debug"}).update("Description"));
	} else {
		header.cells.push(new Cell({"id": "empty_cell", "class": "empty"}));
		data.dccs.each(function(ddu) {
			header.cells.push(new Cell({"id": "board_name_" + dcc.fmmid, "class": "data_board", "fmmid": dcc.fmmid}).update("DCC " + dcc.fmmid));
		});
	}
	table.rows.push(header);

	// Entries are always in order.
	data.entries.each(function(entry, i) {
		var row = new Row({"id": "data_" + i, "class": "data_entry"});
		row.cells.push(new Cell({"id": "entry_name_" + i, "class": "entry_name"}).update(entry.name));

		entry.values.each(function(val) {
			var value = val.value;
			var base = val.base;
			if (base) value = value.toString(base);
			if (base == 16) value = "0x" + value.toUpperCase();
			else if (base == 8) value = "0" + value;

			row.cells.push(new Cell({"id": "entry_value_" + i + "_" + val.fmmid, "class": "entry_value", "base": base}).update(value));
			if (debug) return;
		});
		if (debug) {
			var cell = new Cell({"id": "entry_debug_" + i, "class": "entry_debug"});
			if (entry.descriptions.size()) cell.update(entry.descriptions[0].value);
			row.cells.push(cell);
		}
		table.rows.push(row);
	});

	if ($("display_select").value == "text") $("content").update(new Element("pre").update(table.toText()));
	else {
		$("content").update(table.toElement());

		// Add DDU-selecting functions
		if (!debug) {
			$$(".data_board").each(function(element) {
				element.observe("click", function(event) {
					reloadElements.each(function(re) {
						var newParams = new Object();
						newParams.fmmid = event.element().readAttribute("fmmid");
						newParams.reg = re.params.reg; // An array.
						newParams.board = re.params.board;
						re.params = newParams;
						re.makeTable = true;
						re.tick(true);
					});
				});
			});
		} else {
			$$(".data_board").each(function(element) {
				element.observe("click", function(event) {
					reloadElements.each(function(re) {
						re.params = window.location.href.parseQuery();
						re.params["reg"] = regs; // Defined in Commander.cc
						re.params["rui"] = ruis; // Defined in Commander.cc
						re.params["fmmid"] = fmmids; // Defined in Commander.cc
						re.makeTable = true;
						re.tick(true);
					});
				});
			});
		}
	}
}

function updateDCCs(data)
{

}
*/
