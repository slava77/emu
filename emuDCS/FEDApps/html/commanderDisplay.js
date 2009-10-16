/*****************************************************************************\
* $Id: commanderDisplay.js,v 1.1 2009/10/16 20:32:35 paste Exp $
\*****************************************************************************/

// Make sure the parameters are global
var params = window.location.href.parseQuery();

Event.observe(window, "load", function(event) {

	// Read the values immediately
	readValues();
	
});



function readValues() {
	
	var url = URL + "/Read" + params.board.toUpperCase() + "Registers";
	
	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: updateValues,
		onFailure: reportError
	});
}



function updateValues(transport) {

	var data = transport.responseJSON;
	if (data.ddus) {
		updateDDUs(data);
	} else if (data.dccs) {
		updateDCCs(data);
	} else {
		$("content").update("Bad result!  Nothing to display!");
	}

}


function updateDDUs(data) {

	var debug = true;
	if (data.ddus.size() > 1 || !$("radio_html").checked) debug = false;
	var table = new Table("data_display", ["data_table"]);
	var header = new Row("data_header", ["data_header"]);
	
	if (debug) {
		header.cells.push(new Cell("board_name", ["data_board"]).update("DDU " + data.ddus[0].rui));
		header.cells.push(new Cell("data_title", ["data_title"]).update("Register Value"));
		header.cells.push(new Cell("data_debug", ["data_debug"]).update("Description"));
	} else {
		header.cells.push(new Cell("empty_cell", ["data_board"]));
		data.ddus.each(function(ddu) {
			header.cells.push(new Cell("board_name_" + ddu.rui, ["data_board"]).update("DDU " + ddu.rui));
		});
	}
	table.rows.push(header);
	
	// Entries are always in order.
	data.entries.each(function(entry, i) {
		var row = new Row("data_" + i, ["data_entry"]);
		row.cells.push(new Cell("entry_name_" + i, ["entry_name"]).update(entry.name));

		entry.values.each(function(val) {
			var value = val.value;
			var base = val.base;
			if (base) value = value.toString(base);
			
			row.cells.push(new Cell("entry_value_" + i + "_" + val.rui, ["entry_value", "base" + base]).update(value));
			if (debug) return;
		});
		if (debug) {
			if (entry.descriptions.size()) {
				row.cells.push(new Cell("entry_debug_" + i, ["entry_debug"]).update(entry.descriptions[0].value));
			} else {
				row.cells.push(new Cell("entry_debug_" + i, ["entry_debug"]));
			}
		}
		table.rows.push(row);
	});
	
	if ($("radio_text").checked) $("content").update(new Element("pre").update(table.toText()));
	else $("content").update(table.toElement());
}

function updateDCCs(data) {

}
