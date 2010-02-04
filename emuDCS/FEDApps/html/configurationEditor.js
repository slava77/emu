/*****************************************************************************\
* $Id: configurationEditor.js,v 1.4 2010/02/04 10:39:51 paste Exp $
\*****************************************************************************/

// Global variables follow

// Make this global.  It saves on DB queries if we have an evil user
var keyMap = new Hash();

// An array of crate numbers to save me the look-up time
var crateNumbers = new Array();

// Global count so we know if we are ready to upload/build the XML or not.
var systemsToUpdate = 0;
var cratesToUpdate = 0;
var controllersToUpdate = 0;
var ddusToUpdate = 0;
var dccsToUpdate = 0;
var fibersToUpdate = 0;
var fifosToUpdate = 0;

var allClear = true;

// Class definitions

var Input = Class.create({

	initialize: function(klass, id, properties) {
		this.element = new Element(klass, properties);
		this.id = id;
		this.element.writeAttribute("id", id);
		this.type = "";
		this.action = "update";
		this.parameters = new Hash();
		this.success = function(data) {};
		this.element.observe("reset", this.reset.bind(this));
		this.insignia = true;
		this.element.observe("trigger", this.modFunction_.bind(this));
	},

	modFunction_: function(event) {

		this.reset();

		this.parameters.set("action", this.action);

		// My parameters might be different if I am a selector
		if (this.element.match("select")) {
			var i = this.element.selectedIndex;
			var el = $(this.element.options[i]);

			this.parameters.set("value", el.value);
			this.parameters.set("crate", el.readAttribute("crate"));
			this.parameters.set("slot", el.readAttribute("slot"));
			this.parameters.set("rui", el.readAttribute("rui"));
			this.parameters.set("fmmid", el.readAttribute("fmmid"));
			this.parameters.set("fiber", el.readAttribute("fiber"));
			this.parameters.set("fifo", el.readAttribute("fifo"));
		} else {
			if (this.element.match("[type='checkbox']]")) this.parameters.set("value", (this.element.checked ? "on" : "off"));
			else this.parameters.set("value", this.element.value);
			this.parameters.set("crate", this.element.readAttribute("crate"));
			this.parameters.set("slot", this.element.readAttribute("slot"));
			this.parameters.set("rui", this.element.readAttribute("rui"));
			this.parameters.set("fmmid", this.element.readAttribute("fmmid"));
			this.parameters.set("fiber", this.element.readAttribute("fiber"));
			this.parameters.set("fifo", this.element.readAttribute("fifo"));
		}
		var url = URL + '/' + this.type;

		new Ajax.Request(url, {
			method: "get",
			parameters: this.parameters,
			onSuccess: this.callback.bind(this),
			onFailure: reportError
		});
	},

	callback: function(transport) {

		var data = transport.responseJSON;

		if (data.error) {
			this.element.writeAttribute("title", data.error);
			this.element.writeAttribute("alt", data.error);
			this.element.addClassName("error");
			if (this.insignia) this.element.insert({after: new Element("img", {"id": this.id + "_insignia", "src": "/emu/emuDCS/FEDApps/images/error.png"})});
			return;
		} else if (data.warning) {
			this.element.writeAttribute("title", data.warning);
			this.element.writeAttribute("alt", data.warning);
			this.element.addClassName("warning");
			if (this.insignia) this.element.insert({after: new Element("img", {"id": this.id + "_insignia", "src": "/emu/emuDCS/FEDApps/images/warning.png"})});
		} else if (data.ok) {
			if (this.insignia) this.element.insert({after: new Element("img", {"id": this.id + "_insignia", "src": "/emu/emuDCS/FEDApps/images/dialog-ok-apply.png"})});
		}

		this.success(data);
	},

	reset: function() {

		if ($(this.id + "_insignia"))  {
			$(this.id + "_insignia").remove();
		}
		this.element.writeAttribute("title", "");
		this.element.writeAttribute("alt", "");
		this.element.removeClassName("error").removeClassName("warning");
	}
});



var TextInput = Class.create(Input, {

	initialize: function($super, id, properties) {
		var newProperties;
		if (properties) {
			newProperties = properties;
		}
		newProperties.type = "text";
		$super("input", id, newProperties);
		this.element.observe("change", this.modFunction_.bind(this));
	}

});



var ButtonInput = Class.create(Input, {

	initialize: function($super, id, properties) {
		$super("button", id, properties);
		this.element.observe("click", this.modFunction_.bind(this));
	}

});



var CheckBoxInput = Class.create(Input, {

	initialize: function($super, id, properties, checked) {
		var newProperties;
		if (properties) {
			newProperties = properties;
		}
		newProperties.type = "checkbox";
		$super("input", id, properties);
		this.element.checked = checked;
		this.element.observe("change", this.modFunction_.bind(this));
	}

});



var SelectInput = Class.create(Input, {

	initialize: function($super, id, properties) {
		$super("select", id, properties);
		this.element.observe("change", this.modFunction_.bind(this));
		this.insignia = false;
	}

});



// Global utility functions follow

// Display the system table for editing and update the crate numbers in the selector
function updateSystem(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Hide the table elements corresponding to boards and inputs
	$$(".crate_cell").invoke("show");
	$$(".board_cell").invoke("hide");
	$$(".fiber_cell").invoke("hide");
	$$(".fifo_cell").invoke("hide");

	// Clear the crate selector
	$("crate_selector").fire("reset");

	// Wipe the entries from the selectors
	$("crate_selector").descendants().invoke("remove");
	$("ddu_selector").descendants().invoke("remove");
	$("dcc_selector").descendants().invoke("remove");
	$("fiber_selector").descendants().invoke("remove");
	$("fifo_selector").descendants().invoke("remove");

	// Add the crate numbers to the crate selector
	$("crate_selector").insert(new Element("option", {value: ""}).update("Select crate"));

	data.system.crates.each(function(crate) {
		$("crate_selector").insert(new Element("option", {value: crate.number, crate: crate.number}).update("Crate " + crate.number));
	});

	// Clear the add crate button
	$("crate_add").fire("reset");

	// Update the value of the add crate button
	$("crate_add").writeAttribute("value", data.nextCrate).writeAttribute("crate", data.nextCrate);

	// Build and insert the system table
	var systemTable = makeSystemTable(data);
	insertElement.insert(systemTable);
}


// Make the table for editing the system
function makeSystemTable(data) {
	var newTable = new Element("table", {id: "system_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("System Properties"));
	newTable.insert(newTR1);

	var newTR2 = new Element("tr");
	newTR2.insert(new Element("td", {class: "bold"}).update("System name"));
	var nameInput = new TextInput("input_system_name", {value: data.system.name});
	nameInput.type = "System";
	nameInput.action = "update_name";
	newTR2.insert(new Element("td").insert(nameInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("DB key"));
	var keyInput = new TextInput("input_system_key", {value: data.system.key});
	keyInput.type = "System";
	keyInput.action = "update_key";
	newTR3.insert(new Element("td").insert(keyInput.element));
	newTable.insert(newTR3);

	var newTR4 = new Element("tr");
	newTR4.insert(new Element("td", {class: "bold"}).update("Timestamp"));
	newTR4.insert(new Element("td").insert(data.system.timestamp));
	newTable.insert(newTR4);

	return newTable;
}


// Display the crate table for editing and update the board numbers in the selector
function updateCrate(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Hide the table elements corresponding to inputs, show the element corresponding to boards
	$$(".board_cell").invoke("show");
	$$(".fiber_cell").invoke("hide");
	$$(".fifo_cell").invoke("hide");

	// Clear the board selectors
	$("ddu_selector").fire("reset");
	$("dcc_selector").fire("reset");
	$("controller_selector").fire("reset");

	// Wipe the entries from the selectors
	$("ddu_selector").descendants().invoke("remove");
	$("dcc_selector").descendants().invoke("remove");
	$("fiber_selector").descendants().invoke("remove");
	$("fifo_selector").descendants().invoke("remove");

	// Add the board numbers to the selectors
	$("ddu_selector").insert(new Element("option", {value: ""}).update("Select DDU"));

	data.crate.ddus.each(function(ddu) {
		$("ddu_selector").insert(new Element("option", {value: ddu.rui, rui: ddu.rui, crate: data.crate.number}).update("DDU " + ddu.rui));
	});

	// Clear the add DDU button
	$("ddu_add").fire("reset");

	// Update the value of the add DDU button
	$("ddu_add").writeAttribute("value", data.nextRUI).writeAttribute("crate", data.crate.number).writeAttribute("rui", data.nextRUI).writeAttribute("slot", data.nextSlot);

	$("dcc_selector").insert(new Element("option", {value: ""}).update("Select DCC"));

	data.crate.dccs.each(function(dcc) {
		$("dcc_selector").insert(new Element("option", {value: dcc.fmmid, fmmid: dcc.fmmid, crate: data.crate.number}).update("DCC " + dcc.fmmid));
	});

	// Clear the add DCC button
	$("dcc_add").fire("reset");

	// Update the value of the add DCC button
	$("dcc_add").writeAttribute("value", data.nextFMMID).writeAttribute("crate", data.crate.number).writeAttribute("fmmid", data.nextFMMID).writeAttribute("slot", data.nextSlot);

	// Update the value of the Controller button
	$("controller_selector").writeAttribute("value", data.crate.number).writeAttribute("crate", data.crate.number);

	// Build and insert the crate table
	var crateTable = makeCrateTable(data);
	insertElement.insert(crateTable);
}


// Make the table for editing a crate
function makeCrateTable(data) {

	var newTable = new Element("table", {id: "crate_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("Crate Properties"));
	newTable.insert(newTR1);

	var newTR2 = new Element("tr");
	newTR2.insert(new Element("td", {class: "bold"}).update("Crate number"));
	var nameInput = new TextInput("input_crate_number", {value: data.crate.number, crate: data.crate.number});
	nameInput.type = "Crate";
	nameInput.action = "update_number";
	nameInput.success = function(d) {
		$("crate_selector").select("[value='"+d.previous+"']").each(function(el) {
			el.writeAttribute("value", d.value).writeAttribute("crate", d.value).update("Crate " + d.value);
		});
		this.parameters.set("crate", d.value);
		$("input_crate_delete").writeAttribute("value", d.value).writeAttribute("crate", d.value);
		$("crate_add").writeAttribute("value", d.nextCrate).writeAttribute("crate", d.nextCrate);
	}
	newTR2.insert(new Element("td").insert(nameInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	var deleteButton = new ButtonInput("input_crate_delete", {value: data.crate.number, crate: data.crate.number});
	deleteButton.type = "Crate";
	deleteButton.action = "delete";
	deleteButton.success = function(d) {
		$("crate_selector").select("[value='"+d.value+"']").invoke("remove");
		$$(".board_cell").invoke("hide");
		$$(".fiber_cell").invoke("hide");
		$$(".fifo_cell").invoke("hide");
		$("table_area").descendants().invoke("remove")
		$("table_area").update("");
		$("crate_add").writeAttribute("value", d.nextCrate).writeAttribute("crate", d.nextCrate);
	}
	deleteButton.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	deleteButton.element.insert("Delete crate");
	newTR3.insert(new Element("td", {colspan: 2}).insert(deleteButton.element));
	newTable.insert(newTR3);

	return newTable;

}


// Display the controller table for editing
function updateController(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Hide the table elements corresponding to inputs, show the element corresponding to boards
	$$(".board_cell").invoke("show");
	$$(".fiber_cell").invoke("hide");
	$$(".fifo_cell").invoke("hide");

	// Clear the board selectors
	$("ddu_selector").fire("reset");
	$("dcc_selector").fire("reset");

	// Build and insert the controller table
	var controllerTable = makeControllerTable(data.controller);
	insertElement.insert(controllerTable);
}


// Make the table for editing a controller
function makeControllerTable(controller) {

	var newTable = new Element("table", {id: "controller_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("Controller Properties"));
	newTable.insert(newTR1);

	var newTR2 = new Element("tr");
	newTR2.insert(new Element("td", {class: "bold"}).update("CAEN device"));
	var deviceInput = new TextInput("input_controller_device", {value: controller.device, crate: controller.crate});
	deviceInput.type = "Controller";
	deviceInput.action = "update_device";
	deviceInput.parameters.set("device", controller.device);
	newTR2.insert(new Element("td").insert(deviceInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("CAEN link"));
	var linkInput = new TextInput("input_controller_link", {value: controller.link, crate: controller.crate});
	linkInput.type = "Controller";
	linkInput.action = "update_link";
	linkInput.parameters.set("link", controller.link);
	newTR3.insert(new Element("td").insert(linkInput.element));
	newTable.insert(newTR3);

	return newTable;

}


// Display the DDU table for editing and update the fiber numbers in the selector
function updateDDU(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Show the table elements corresponding to fibers
	$$(".fiber_cell").invoke("show");
	$$(".fifo_cell").invoke("hide");

	// Clear the fiber selectors
	$("fiber_selector").fire("reset");

	// Wipe the entries from the selector
	$("fiber_selector").descendants().invoke("remove");

	// Add the fiber numbers to the selectors
	$("fiber_selector").insert(new Element("option", {value: ""}).update("Select fiber"));

	data.ddu.fibers.each(function(myfiber) {
		$("fiber_selector").insert(new Element("option", {value: myfiber.fiber, fiber: myfiber.fiber, rui: data.ddu.rui, crate: data.crate}).update("Fiber " + myfiber.fiber));
	});

	// Clear the add fiber button
	$("fiber_add").fire("reset");

	// Update the value of the add fiber button
	$("fiber_add").writeAttribute("value", data.nextFiber).writeAttribute("crate", data.crate).writeAttribute("rui", data.ddu.rui).writeAttribute("fiber", data.nextFiber);

	// Build and insert the DDU table
	var dduTable = makeDDUTable(data);
	insertElement.insert(dduTable);
}


// Make the table for editing a DDU
function makeDDUTable(data) {

	var newTable = new Element("table", {id: "ddu_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("DDU Properties"));
	newTable.insert(newTR1);

	var newTR0 = new Element("tr");
	newTR0.insert(new Element("td", {class: "bold"}).update("Slot"));
	var slotInput = new TextInput("input_ddu_slot", {value: data.ddu.slot, crate: data.crate, rui: data.ddu.rui});
	slotInput.type = "DDU";
	slotInput.action = "update_slot";
	slotInput.success = function(d) {
		$("ddu_add").writeAttribute("slot", d.nextSlot);
	}
	newTR0.insert(new Element("td").insert(slotInput.element));
	newTable.insert(newTR0);

	var newTR2 = new Element("tr");
	newTR2.insert(new Element("td", {class: "bold"}).update("RUI"));
	var ruiInput = new TextInput("input_ddu_rui", {value: data.ddu.rui, crate: data.crate, rui: data.ddu.rui});
	ruiInput.type = "DDU";
	ruiInput.action = "update_rui";
	ruiInput.success = function(d) {
		$("ddu_selector").select("[value='"+d.previous+"']").each(function(el) {
			el.writeAttribute("value", d.value).writeAttribute("rui", d.value).update("DDU " + d.value);
		});
		this.parameters.set("rui", d.value);
		$("input_ddu_delete").writeAttribute("value", d.value).writeAttribute("rui", d.value);
		$("ddu_add").writeAttribute("value", d.nextRUI).writeAttribute("rui", d.nextRUI);
	}
	newTR2.insert(new Element("td").insert(ruiInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("FMM ID"));
	var fmmidInput = new TextInput("input_ddu_fmmid", {value: data.ddu.fmmid, crate: data.crate, rui: data.ddu.rui});
	fmmidInput.type = "DDU";
	fmmidInput.action = "update_fmmid";
	newTR3.insert(new Element("td").insert(fmmidInput.element));
	newTable.insert(newTR3);

	var newTR4 = new Element("tr");
	newTR4.insert(new Element("td", {class: "bold"}).update("GbE prescale"));
	var gbeInput = new TextInput("input_ddu_gbe", {value: data.ddu.gbe_prescale, crate: data.crate, rui: data.ddu.rui});
	gbeInput.type = "DDU";
	gbeInput.action = "update_gbe";
	newTR4.insert(new Element("td").insert(gbeInput.element));
	newTable.insert(newTR4);

	var newTR5 = new Element("tr");
	newTR5.insert(new Element("td", {class: "bold"}).update("Force all data checks"));
	var forceAllInput = new CheckBoxInput("input_ddu_forceAll", {crate: data.crate, rui: data.ddu.rui}, data.ddu.force_checks);
	forceAllInput.type = "DDU";
	forceAllInput.action = "update_force_checks";
	forceAllInput.success = function(d) {
		if (d.value) this.element.checked = true;
		else this.element.checked = false;
	}
	newTR5.insert(new Element("td").insert(forceAllInput.element));
	newTable.insert(newTR5);

	var newTR6 = new Element("tr");
	newTR6.insert(new Element("td", {class: "bold"}).update("Force ALCT checks"));
	var forceALCTInput = new CheckBoxInput("input_ddu_forceALCT", {crate: data.crate, rui: data.ddu.rui}, data.ddu.force_alct);
	forceALCTInput.type = "DDU";
	forceALCTInput.action = "update_force_alct";
	newTR6.insert(new Element("td").insert(forceALCTInput.element));
	newTable.insert(newTR6);

	var newTR7 = new Element("tr");
	newTR7.insert(new Element("td", {class: "bold"}).update("Force TMB checks"));
	var forceTMBInput = new CheckBoxInput("input_ddu_forceTMB", {crate: data.crate, rui: data.ddu.rui}, data.ddu.force_tmb);
	forceTMBInput.type = "DDU";
	forceTMBInput.action = "update_force_tmb";
	newTR7.insert(new Element("td").insert(forceTMBInput.element));
	newTable.insert(newTR7);

	var newTR8 = new Element("tr");
	newTR8.insert(new Element("td", {class: "bold"}).update("Force CFEB checks"));
	var forceCFEBInput = new CheckBoxInput("input_ddu_forceCFEB", {crate: data.crate, rui: data.ddu.rui}, data.ddu.force_cfeb);
	forceCFEBInput.type = "DDU";
	forceCFEBInput.action = "update_force_cfeb";
	newTR8.insert(new Element("td").insert(forceCFEBInput.element));
	newTable.insert(newTR8);

	var newTR9 = new Element("tr");
	newTR9.insert(new Element("td", {class: "bold"}).update("Force DMB checks"));
	var forceDMBInput = new CheckBoxInput("input_ddu_forceDMB", {crate: data.crate, rui: data.ddu.rui}, data.ddu.force_dmb);
	forceDMBInput.type = "DDU";
	forceDMBInput.action = "update_force_dmb";
	newTR9.insert(new Element("td").insert(forceDMBInput.element));
	newTable.insert(newTR9);

	var newTR10 = new Element("tr");
	var deleteButton = new ButtonInput("input_ddu_delete", {value: data.ddu.rui, crate: data.crate, rui: data.ddu.rui});
	deleteButton.type = "DDU";
	deleteButton.action = "delete";
	deleteButton.success = function(d) {
		$("ddu_selector").select("[value='"+d.value+"']").invoke("remove");
		$$(".fiber_cell").invoke("hide");
		$$(".fifo_cell").invoke("hide");
		$("table_area").descendants().invoke("remove")
		$("table_area").update("");
		$("ddu_add").writeAttribute("value", d.nextRUI).writeAttribute("rui", d.nextRUI).writeAttribute("slot", d.nextSlot);
	}
	deleteButton.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	deleteButton.element.insert("Delete DDU");
	newTR10.insert(new Element("td", {colspan: 2}).insert(deleteButton.element));
	newTable.insert(newTR10);

	return newTable;

}



// Display the Fiber table for editing and update the fiber numbers in the selector
function updateFiber(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Build and insert the fiber table
	var fiberTable = makeFiberTable(data);
	insertElement.insert(fiberTable);
}


// Make the table for editing a fiber
function makeFiberTable(data) {

	var newTable = new Element("table", {id: "fiber_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("Fiber Properties"));
	newTable.insert(newTR1);

	var newTR0 = new Element("tr");
	newTR0.insert(new Element("td", {class: "bold"}).update("Number"));
	var fiberInput = new TextInput("input_fiber_number", {value: data.fiber.fiber, crate: data.crate, rui: data.rui, fiber: data.fiber.fiber});
	fiberInput.type = "Fiber";
	fiberInput.action = "update_number";
	fiberInput.success = function(d) {
		$("fiber_selector").select("[value='"+d.previous+"']").each(function(el) {
			el.writeAttribute("value", d.value).writeAttribute("fiber", d.value).update("Fiber " + d.value);
		});
		this.parameters.set("fiber", d.value);
		$("input_fiber_delete").writeAttribute("value", d.value).writeAttribute("fiber", d.value);
		$("fiber_add").writeAttribute("value", d.nextFiber).writeAttribute("fiber", d.nextFiber);
	}
	newTR0.insert(new Element("td").insert(fiberInput.element));
	newTable.insert(newTR0);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("Chamber/SP name"));
	var nameInput = new TextInput("input_fiber_name", {value: data.fiber.name, crate: data.crate, rui: data.rui, fiber: data.fiber.fiber});
	nameInput.type = "Fiber";
	nameInput.action = "update_name";
	newTR3.insert(new Element("td").insert(nameInput.element));
	newTable.insert(newTR3);

	var newTR5 = new Element("tr");
	newTR5.insert(new Element("td", {class: "bold"}).update("Kill"));
	var killedInput = new CheckBoxInput("input_fiber_kill", {crate: data.crate, rui: data.rui, fiber: data.fiber.fiber}, data.fiber.killed);
	killedInput.type = "Fiber";
	killedInput.action = "update_killed";
	killedInput.success = function(d) {
		if (d.value) this.element.checked = true;
		else this.element.checked = false;
	}
	newTR5.insert(new Element("td").insert(killedInput.element));
	newTable.insert(newTR5);

	return newTable;

}



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

function isPosInt(x) {
	var y = parseInt(x);
	if (isNaN(y)) return false;
	return x == y && x.toString() == y.toString() && y >= 0;
}

function findChamber(needle) {

	var found = false;
	// Find the first needle and make that visible and selected
	($$(".input_fiber_name")).each(function(element) {
		if (element.value.indexOf(needle) >= 0) {

			var toggleElements = new Array();

			var crate = element.getAttribute("crate");
			var rui = element.getAttribute("rui");
			var fiber = element.getAttribute("fiber");

			($$("[fiber]")).each(function(ele) {
				if (ele.getAttribute("fiber") == fiber && ele.getAttribute("rui") == rui && ele.getAttribute("crate") == crate) {
					ele.writeAttribute("fibers_hidden", 0);
					ele.writeAttribute("ddu_hidden", 0);
					ele.writeAttribute("ddus_hidden", 0);
					ele.writeAttribute("crate_hidden", 0);
					ele.writeAttribute("crates_hidden", 0);
					toggleElements.push(ele);
				}
			});

			($$("[rui]")).each(function(ele) {
				if (ele.getAttribute("rui") == rui && ele.getAttribute("crate") == crate) {
					ele.writeAttribute("ddu_hidden", 0);
					ele.writeAttribute("ddus_hidden", 0);
					ele.writeAttribute("crate_hidden", 0);
					ele.writeAttribute("crates_hidden", 0);
					toggleElements.push(ele);
				}
			});

			($$("[crate]")).each(function(ele) {
				if (ele.getAttribute("crate") == crate) {
					ele.writeAttribute("crate_hidden", 0);
					ele.writeAttribute("crates_hidden", 0);
					toggleElements.push(ele);
				}
			});

			toggleHidden(toggleElements);

			($("find_a_fiber_error")).hide();
			found = true;

			element.focus();
			element.select();

			throw $break;
		}
	});

	if (!found) {
		$("find_a_fiber_error").show();
	}
}



Event.observe(window, "load", function(event) {

	// Hide all hidden elements
	($$(".hidden")).invoke("hide");

	// Update the DB system names and keys
	getDBKeys();

	// Make the drop-downs for selecting things
	var systemButton = new ButtonInput("system_selector");
	systemButton.type = "System";
	systemButton.action = "get";
	systemButton.insignia = false;
	systemButton.success = updateSystem.bind(systemButton);
	systemButton.element.update("System");
	$("system_cell").insert(systemButton.element);

	var crateDrop = new SelectInput("crate_selector");
	crateDrop.type = "Crate";
	crateDrop.action = "get";
	crateDrop.success = updateCrate.bind(crateDrop);
	crateDrop.element.insert(new Element("option", {value: ""}).update("Select crate"));
	$("crate_cell").insert(crateDrop.element);

	var dduDrop = new SelectInput("ddu_selector");
	dduDrop.type = "DDU";
	dduDrop.action = "get";
	dduDrop.success = updateDDU.bind(dduDrop);
	dduDrop.element.insert(new Element("option", {value: ""}).update("Select DDU"));
	$("board_cell").insert(dduDrop.element);

	var dccDrop = new SelectInput("dcc_selector");
	dccDrop.type = "DCC";
	dccDrop.action = "get";
	dccDrop.success = function(data) {
		updateDCC(data);
	}
	dccDrop.element.insert(new Element("option", {value: ""}).update("Select DCC"));
	$("board_cell").insert(new Element("br")).insert(dccDrop.element);

	var controllerButton = new ButtonInput("controller_selector");
	controllerButton.type = "Controller";
	controllerButton.action = "get";
	controllerButton.insignia = false;
	controllerButton.success = updateController.bind(controllerButton);
	controllerButton.element.update("Controller");
	$("board_cell").insert(new Element("br")).insert(controllerButton.element);

	var fiberDrop = new SelectInput("fiber_selector");
	fiberDrop.type = "Fiber";
	fiberDrop.action = "get";
	fiberDrop.success = updateFiber.bind(fiberDrop);
	fiberDrop.element.insert(new Element("option", {value: ""}).update("Select fiber"));
	$("fiber_cell").insert(fiberDrop.element);

	var fifoDrop = new SelectInput("fifo_selector");
	fifoDrop.type = "FIFO";
	fifoDrop.action = "get";
	fifoDrop.success = function(data) {
		updateFIFO(data);
	}
	fifoDrop.element.insert(new Element("option", {value: ""}).update("Select FIFO"));
	$("fifo_cell").insert(fifoDrop.element);

	// Make add buttons for adding things
	var newCrate = new ButtonInput("crate_add", {class: "add"});
	newCrate.type = "Crate";
	newCrate.action = "add";
	newCrate.success = function(data) {
		this.element.writeAttribute("value", data.nextCrate).writeAttribute("crate", data.nextCrate);
		$("crate_selector").insert(new Element("option", {value: data.value, crate: data.value}).update("Crate " + data.value));
	}
	newCrate.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newCrate.element.insert("Add crate");
	$("crate_cell").insert(new Element("br")).insert(newCrate.element);

	var newDDU = new ButtonInput("ddu_add", {class: "add"});
	newDDU.type = "DDU";
	newDDU.action = "add";
	newDDU.success = function(data) {
		this.element.writeAttribute("value",  data.nextRUI).writeAttribute("rui", data.nextRUI).writeAttribute("slot", data.nextSlot);
		$("ddu_selector").insert(new Element("option", {value: data.value, rui: data.value, crate: data.crate}).update("DDU " + data.value));
	}
	newDDU.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newDDU.element.insert("Add DDU");
	$("board_cell").insert(new Element("br")).insert(newDDU.element);

	var newDCC = new ButtonInput("dcc_add", {class: "add"});
	newDCC.type = "DCC";
	newDCC.action = "add";
	newDCC.success = function(data) {
		this.element.writeAttribute("value",  data.value + 1).writeAttribute("fmmid", data.value + 1);
		$("ddu_selector").insert(new Element("option", {value: data.value, fmmid: data.value, crate: data.crate}).update("DCC " + data.value));
	}
	newDCC.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newDCC.element.insert("Add DCC");
	$("board_cell").insert(new Element("br")).insert(newDCC.element);

	var newFiber = new ButtonInput("fiber_add", {class: "add"});
	newFiber.type = "Fiber";
	newFiber.action = "add";
	newFiber.success = function(data) {
		this.element.writeAttribute("value",  data.nextFiber).writeAttribute("fiber", data.nextFiber);
		$("fiber_selector").insert(new Element("option", {value: data.value, fiber: data.value, rui: data.rui, crate: data.crate}).update("Fiber " + data.value));
	}
	newFiber.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newFiber.element.insert("Add fiber");
	$("fiber_cell").insert(new Element("br")).insert(newFiber.element);

	var newFIFO = new ButtonInput("fifo_add", {class: "add"});
	newFIFO.type = "FIFO";
	newFIFO.action = "add";
	newFIFO.success = function(data) {
		this.element.writeAttribute("value",  data.value + 1).writeAttribute("fifo", data.value + 1);
		$("fifo_selector").insert(new Element("option", {value: data.value, fifo: data.calue, fmmid: data.fmmid, crate: data.crate}).update("FIFO " + data.value));
	}
	newFIFO.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newFIFO.element.insert("Add fifo");
	$("fifo_cell").insert(new Element("br")).insert(newFIFO.element);


	($("xml_file_button")).disabled = false;

	($("xml_file_button")).observe("click", function(ev) {
		ev.element().update("Uploading...").insert(new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif"}));
		ev.element().disabled = true;
		($("xml_file_form")).submit();
		window.location.reload();
	});


	($("load_button")).disabled = false;
	($("load_button")).observe("click", function(ev) {

		var key = ($("configuration_key")).value;
		if (key == "") return;

		ev.element().update("Loading from database...").insert(new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif"}));
		ev.element().disabled = true;
		var url = URL + "/LoadFromDB";
		var params = {"key": key}

		new Ajax.Request(url, {
			method: "get",
			parameters: params,
			onSuccess: function(transport) { window.location.reload(); },
			onFailure: reportError
		});

	});

	($("create_button")).disabled = false;
	($("create_button")).observe("click", function(ev) {
		ev.element().update("Creating new configuration...").insert(new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif"}));
		ev.element().disabled = true;
		var url = URL + "/CreateNew";

		new Ajax.Request(url, {
			method: "get",
			onSuccess: function(transport) { window.location.reload(); },
			onFailure: reportError
		});
	});
	/*
	($$(".write_xml")).each(function(element) {
		element.disabled = false;
		element.observe("click", function(ev) {
			($$(".action_button")).each(function(el) { el.disabled = true; });
			updateConfiguration(ev.element(), false);
			new PeriodicalExecuter(checkXMLUpdate, 1);
		});
	});

	($$(".upload_to_db")).each(function(element) {
		element.disabled = false;
		element.observe("click", function(ev) {
			($$(".action_button")).each(function(el) { el.disabled = true; });
			updateConfiguration(ev.element(), true);
			new PeriodicalExecuter(checkUploadUpdate, 1);
		});
	});

	($$(".find_a_fiber")).each(function(element) {
		element.observe("keypress", function(ev) {
			if (ev.keyCode == Event.KEY_RETURN) {
				findChamber(($("find_a_fiber")).value);
			}
		});
	});

	($$(".find_button")).each(function(element) {
		element.observe("click", function(ev) {
			findChamber(($("find_a_fiber")).value);
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

	// There is only one of these, but this ensures that the function is not called if the element does not exist.
	($$(".add_crate")).each(function(element) {

		// Make me a function to handle clicks
		element.clickFunction = function(event) {

			if (!element.number) {
				// Figure out what your new number should be
				var newNumber = -1;
				($$("[crate]")).each(function(el) {
					var a = parseInt(el.readAttribute("crate"));
					if (a > newNumber) newNumber = a;
				});

				newNumber++;
				element.number = newNumber;
			}

			// Try adding the crate
			var params = new Hash();
			params.set("action", "add");
			params.set("fakeCrate", this.number);
			params.set("number", this.number);
			ajaxCrate(params, this.callback.bind(this));
		}

		// Make me a function to handle callbacks
		element.callback = function(transport) {

			var data = transport.responseJSON;

			if (data.error) {
				setError(this, data.error);
				return;
			} else if (data.warning) {
				if (!$("add_crate_symbol")) {
					this.insert(new Element("img", {"id": "add_crate_symbol", "src": "/emu/emuDCS/FEDApps/images/warning.png"}), {position: "after"});
				}
				this.writeAttribute("title", data.warning);
				this.writeAttribute("alt", data.warning);
			} else if ($("add_crate_symbol")) {
				$("add_crate_symbol").remove();
				this.writeAttribute("title", "");
				this.writeAttribute("alt", "");
			}

			var newCrateNumber = this.number;
			this.number = newCrateNumber + 1;
			var newTable = newCrate(newCrateNumber);

			this.up().insert({"before": newTable});
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

		}

		element.observe("click", element.clickFunction.bind(element));

	});

	updateCrateTriggers();
	updateDDUTriggers();
	updateDCCTriggers();
	updateFiberTriggers();
	updateFIFOTriggers();
*/
});





function ajaxCrate(params, callback) {

	var url = URL + '/BuildCrate';

	new Ajax.Request(url, {
		method: "get",
		parameters: params,
		onSuccess: callback,
		onFailure: reportError
	});

}

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

			element.clickFunction = function(event) {

				var crateNumber = this.readAttribute("crate");
				// Try deleting the crate
				var params = new Hash();
				params.set("action", "delete");
				params.set("fakeCrate", crateNumber);
				params.set("number", crateNumber);
				ajaxCrate(params, this.callback.bind(this));
			}

			element.callback = function(transport) {

				var data = transport.responseJSON;

				var crateNumber = data.number;

				if (data.error) {
					if (!$("delete_crate_symbol_" + crateNumber)) {
						this.insert(new Element("img", {"id": "delete_crate_symbol_" + crateNumber, "src": "/emu/emuDCS/FEDApps/images/error.png"}), {position: "after"});
					}
					this.writeAttribute("title", data.error);
					this.writeAttribute("alt", data.error);
					return;
				} else if ($("delete_crate_symbol_" + crateNumber)) {
					$("delete_crate_symbol_" + crateNumber).remove();
					this.writeAttribute("title", "");
					this.writeAttribute("alt", "");
				}

				$$("[crate='"+crateNumber+"']").invoke("remove");
				this.remove();

			}


			element.observe("click", element.clickFunction.bind(element));
		}
	});

	$$(".input_crate_number").each(function(element) {
		if (number == null || element.readAttribute("crate") == number) {

			element.changeFunction = function(event) {

				var crateNumber = this.readAttribute("crate");
				// Try deleting the crate
				var params = new Hash();
				params.set("action", "update");
				params.set("fakeCrate", crateNumber);
				params.set("number", this.value);
				ajaxCrate(params, this.callback.bind(this));
			}

			element.callback = function(transport) {

				var data = transport.responseJSON;

				var crateNumber = this.readAttribute("crate");
				if ($("update_crate_symbol_" + crateNumber))  {
					$("update_crate_symbol_" + crateNumber).remove();
				}

				if (data.error) {
					if (!$("update_crate_symbol_" + crateNumber)) {
						this.up().insert(new Element("img", {"id": "update_crate_symbol_" + crateNumber, "src": "/emu/emuDCS/FEDApps/images/error.png"}), {position: "after"});
					}
					this.writeAttribute("title", data.error);
					this.writeAttribute("alt", data.error);
					return;
				} else if (data.warning) {
					if (!$("update_crate_symbol_" + data.number)) {
						this.up().insert(new Element("img", {"id": "update_crate_symbol_" + data.number, "src": "/emu/emuDCS/FEDApps/images/warning.png"}), {position: "after"});
					}
					this.writeAttribute("title", data.warning);
					this.writeAttribute("alt", data.warning);
				} else if ($("update_crate_symbol_" + data.number))  {

					$("update_crate_symbol_" + data.number).remove();
					this.writeAttribute("title", "");
					this.writeAttribute("alt", "");
				}

				$$("[crate='"+crateNumber+"']").each(function(el) {
					el.writeAttribute("crate", data.number);
				});

			}


			element.observe("change", element.changeFunction.bind(element));
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
	newTR4.insert(new Element("td").insert(new Element("input", {"class": "input_dcc_fmm_id", "type": "text", "value": fmmid, "crate": crate, "fmmid": fmmid})));
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
		imgEle.writeAttribute("src", "/emu/emuDCS/FEDApps/images/minus.png");
		hidden.each(function(el) {
			($(el)).writeAttribute(class, 0);
		});
	} else {
		imgEle.writeAttribute("src", "/emu/emuDCS/FEDApps/images/plus.png");
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
		keyMap.set(name, ($A(system.keys)));
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

function updateConfiguration(element, doingUpload) {
	allClear = true;

	element.insert(new Element("img", {"src": "/emu/emuDCS/FEDApps/images/ajax-loader.gif", "id": "spinner"}));

	// Do the checks here.
	// Clear all the errors
	($$("input,select")).invoke("removeClassName", "inputError");
	($$("input,select")).invoke("removeClassName", "inputWarning");

	// Count everything so we know how long to wait before doing the uploads
	systemsToUpdate = 1;
	cratesToUpdate = $$(".input_crate_number").size();
	controllersToUpdate = $$(".input_controller_device").size();
	ddusToUpdate = $$("input_ddu_rui").size();
	dccsToUpdate = $$("input_dcc_fmm_id").size();
	fibersToUpdate = $$("input_fiber_number").size();
	fifosToUpdate = $$("input_fifo_number").size();

	// Enforce a non-null system name
	var systemName = ($("input_database_name")).value.strip();
	($("input_database_name")).value = systemName;
	if (systemName == "") {
		($("input_database_name")).addClassName("inputError");
		alert("Please enter a system name before attempting to upload the configuration.");
		allClear = false;
		return;
	}

	if (doingUpload) {
		var checkKey = ($("input_database_key")).value;
		// Enforce integer key
		if (!isPosInt(checkKey)) {
			($("input_database_key")).addClassName("inputError");
			alert("Configuration key must be a positive integer.");
			allClear = false;
			return;
		}
	}

	// Make sure the key is unique
	keyMap.each(function(pair) {
		if (pair.value.detect(function(key) { return key == checkKey; })) {
			($("input_database_key")).addClassName("inputError");
			alert("The configuration key you chose is already in use by the system \""+pair.key+"\".\n\nPlease change the key to an unused value before attempting to upload the configuration.");
			allClear = false;
			throw $break;
		}
	});
	if (!allClear) return;

	// Make sure each crate number is unique
	// I need to go through each one to know which one is the problem
	($$(".input_crate_number")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("Crate numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
		($$(".input_crate_number")).each(function(elj, j) {
			if (i == j) return;
			if (eli.value == elj.value) {
				elj.addClassName("inputError");
				alert("Crate numbers must be unique per configuration.\n\nPlease change the crate numbers before attempting to upload the configuration.");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Make sure controller device and link are integers
	($$(".input_controller_device,.input_controller_link")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("Controler device and link numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
	});
	if (!allClear) return;

	// Make sure that only one board is defined per slot
	($$(".input_ddu_slot,.input_dcc_slot")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("Slot numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
		var intValue = parseInt(eli.value);
		if (intValue < 2 || intValue > 20) {
			eli.addClassName("inputError");
			alert("Slot numbers must be integers between 2 and 20 (inclusive).\n\nPlease check the slot numbers before attempting to upload the configuration.");
			allClear = false;
			throw $break;
		}
		($$(".input_ddu_slot,.input_dcc_slot")).each(function(elj, j) {
			if (i == j) return;
			if (eli.value == elj.value && eli.readAttribute("crate") == elj.readAttribute("crate")) {
				elj.addClassName("inputError");
				alert("Only one board can occupy a given slot per crate.\n\nPlease check the slot numbers before attempting to upload the configuration.");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Warn on bad DCC slot
	($$(".input_dcc_slot")).each(function(el) {
		var slotValue = parseInt(el.value);
		if (slotValue != 8 && slotValue != 17 && !confirm("DCCs are typically only able to function in slots 8 and 17.\n\nAre you sure you want the DCC in crate "+el.readAttribute("crate")+" to be in slot "+slotValue+"?  Click \"OK\" to continue, \"Cancel\" to abort the upload.")) {
			el.addClassName("inputWarning");
			allClear = false;
			throw $break;
		}
	});
	if (!allClear) return;

	// Make sure the DDU RUIs are unique
	($$(".input_ddu_rui")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("DDU RUI numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
		($$(".input_ddu_rui")).each(function(elj, j) {
			if (i == j) return;
			if (eli.value == elj.value) {
				elj.addClassName("inputError");
				alert("DDU RUI numbers need to be unique.\n\nPlease check the DDU RUI numbers before attempting to upload the configuration.");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Make sure the FMM IDs are unique
	($$(".input_ddu_fmm_id,.input_dcc_fmm_id")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("FMM ID numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
		($$(".input_ddu_fmm_id,.input_dcc_fmm_id")).each(function(elj, j) {
			if (i == j) return;
			if (eli.value == elj.value) {
				elj.addClassName("inputError");
				alert("FMM ID numbers need to be unique.\n\nPlease check the FMM ID numbers before attempting to upload the configuration.");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Make sure GbE prescale is reasonable
	($$(".input_ddu_gbe_prescale")).each(function(eli) {
		var val = eli.value;
		if (!isPosInt(val)) {
			eli.addClassName("inputError");
			alert("GbE Prescale numbers must be positive integers, and \""+val+"\" is not.");
			allClear = false;
			throw $break;
		}
		if (val > 15) {
			if (!confirm("Only the first 4 bits of the GbE Prescale register is used, so the number "+val+" will be truncated to "+(parseInt(val) & 15)+" before uploading.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
				eli.addClassName("inputWarning");
				allClear = false;
				throw $break;
			} else {
				eli.value = (parseInt(val) & 15);
			}
		}
	});
	if (!allClear) return;

	// Check CCB command signal against RUI
	($$(".input_ddu_invert_ccb")).each(function(eli) {

		var crate = eli.readAttribute("crate");
		var rui = eli.readAttribute("rui");

		($$(".input_ddu_rui")).each(function(elj) {
			if (elj.readAttribute("crate") == crate && elj.readAttribute("rui") == rui) {
				if (eli.checked && elj.value != 192) {
					if (!confirm("The DDU with RUI number "+elj.value+" has the \"Invert CCB command signals\" option selected.  The RUI number of this DDU will automatically be set to 192.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
						elj.addClassName("inputWarning");
						allClear = false;
						throw $break;
					} else {
						elj.value = 192;
					}
				} else if (!eli.checked && elj.value == 192) {
					if (!confirm("A DDU with RUI number 192 does not have the \"Invert CCB command signals\" option selected.  This RUI is special, so the \"Invert CCB command signals\" option will automatically be selected at this time.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
						elj.addClassName("inputWarning");
						allClear = false;
						throw $break;
					} else {
						eli.checked = true;
					}
				}
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Count the number of fibers per DDU
	($$(".input_ddu_rui")).each(function(eli) {

		var crate = eli.readAttribute("crate");
		var rui = eli.readAttribute("rui");
		var printRUI = eli.value;

		// All of a sudden union is not working.  Yargh!
		var theseFibers = new Array;
		($$(".input_fiber_number")).each(function(elj) {
			if (elj.readAttribute("crate") == crate && elj.readAttribute("rui") == rui) theseFibers.push(elj);
		});

		if (theseFibers.size() < 15 && !confirm("The DDU with RUI number "+printRUI+" has "+theseFibers.size()+" fiber"+(theseFibers.size() == 1 ? "" : "s")+" defined in this configuration.  A DDU has 15 fiber inputs.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
			allClear = false;
			throw $break;
		} else if (theseFibers.size() > 15) {
			alert("The DDU with RUI number "+printRUI+" has "+theseFibers.size()+" fibers defined in this configuration.  A DDU has only 15 fiber inputs.")
			allClear = false;
			throw $break;
		}

		theseFibers.each(function(idi, i) {
			var number = ($(idi)).value;
			if (!isPosInt(number)) {
				($(idi)).addClassName("inputError");
				alert("Fiber numbers must be positive integers, and \""+number+"\" is not.");
				allClear = false;
				throw $break;
			}
			if (parseInt(number) > 14) {
				($(idi)).addClassName("inputError");
				alert("Fiber numbers must be between 0 and 14, and \""+number+"\" is not.");
				allClear = false;
				throw $break;
			}

			theseFibers.each(function(idj, j) {
				if (i == j) return;
				if (number == ($(idj)).value) {
					($(idj)).addClassName("inputError");
					alert("Fiber numbers need to be unique per DDU.\n\nPlease check the Fiber numbers for the DDU with RUI number "+printRUI+" before attempting to upload the configuration.");
					allClear = false;
					throw $break;
				}
			});
			if (!allClear) throw $break;
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Make sure fiber names are sane
	($$(".input_fiber_name")).each(function(elj) {
		var name = elj.value.toString().strip();
		elj.value = name;
		if (!name.match(/^[\+\-]\d\/\d\/[0-3]?\d$/) && !name.match(/^SP[0-1]?\d$/) && !confirm("The fiber name \""+name+"\" is not a recognized chamber or SP name.  Chamber names have the format [+-]\\d/\\d/[0-3]?\\d, and SPs have the format SP[0-1]?\\d.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
			elj.addClassName("inputWarning");
			allClear = false;
			throw $break;
		}
	});
	if (!allClear) return;

	// All SLink IDs must be unique
	($$(".input_dcc_slink1,.input_dcc_slink2")).each(function(eli, i) {
		if (!isPosInt(eli.value)) {
			eli.addClassName("inputError");
			alert("SLink ID numbers must be positive integers, and \""+eli.value+"\" is not.");
			allClear = false;
			throw $break;
		}
		($$(".input_dcc_slink1,.input_dcc_slink2")).each(function(elj, j) {
			if (i == j) return;
			if (eli.value == elj.value) {
				elj.addClassName("inputError");
				alert("SLink ID numbers need to be unique.\n\nPlease check the Slink ID numbers before attempting to upload the configuration.");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Warn about FMM/Slink mismatch
	($$(".input_dcc_fmm_id")).each(function(eli, i) {
		var crate = eli.readAttribute("crate");
		var fmmid = eli.readAttribute("fmmid");
		var printFMMID = eli.value;

		($$(".input_dcc_slink1")).each(function(elj, j) {
			if (elj.readAttribute("crate") == crate && elj.readAttribute("fmmid") == fmmid && elj.value != printFMMID && !confirm("The DCC with FMM ID "+printFMMID+" has an SLink 1 ID of "+elj.value+".  Typically these two numbers are equal.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
				elj.addClassName("inputWarning");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
		($$(".input_dcc_slink2")).each(function(elj, j) {
			if (elj.readAttribute("crate") == crate && elj.readAttribute("fmmid") == fmmid && parseInt(elj.value) != parseInt(printFMMID) + 1 && !confirm("The DCC with FMM ID "+printFMMID+" has an SLink 2 ID of "+elj.value+".  Typically the SLink 2 ID has a value of one greater than the FMM ID.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
				elj.addClassName("inputWarning");
				allClear = false;
				throw $break;
			}
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Count the number of FIFOs per DCC
	($$(".input_dcc_fmm_id")).each(function(eli) {

		var crate = eli.readAttribute("crate");
		var fmmid = eli.readAttribute("fmmid");
		var printFMMID = eli.value;

		// All of a sudden union is not working.  Yargh!
		var theseFIFOs = new Array;
		($$(".input_fifo_number")).each(function(elj) {
			if (elj.readAttribute("crate") == crate && elj.readAttribute("fmmid") == fmmid) theseFIFOs.push(elj);
		});

		if (theseFIFOs.size() < 10 && !confirm("The DCC with FMM ID number "+printFMMID+" has "+theseFIFOs.size()+" FIFO"+(theseFIFOs.size() == 1 ? "" : "s")+" defined in this configuration.  A DCC has 10 FIFO inputs.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
			allClear = false;
			throw $break;
		} else if (theseFIFOs.size() > 10) {
			alert("The DCC with FMM ID number "+printFMMID+" has "+theseFIFOs.size()+" FIFOs defined in this configuration.  A DCC has only 10 FIFO inputs.")
			allClear = false;
			throw $break;
		}

		theseFIFOs.each(function(idi, i) {
			var number = ($(idi)).value;
			if (!isPosInt(number)) {
				($(idi)).addClassName("inputError");
				alert("FIFO numbers must be positive integers, and \""+number+"\" is not.");
				allClear = false;
				throw $break;
			}
			if (parseInt(number) > 9) {
				($(idi)).addClassName("inputError");
				alert("FIFO numbers must be between 0 and 9, and \""+number+"\" is not.");
				allClear = false;
				throw $break;
			}

			theseFIFOs.each(function(idj, j) {
				if (i == j) return;
				if (number == ($(idj)).value) {
					($(idj)).addClassName("inputError");
					alert("FIFO numbers need to be unique per DCC.\n\nPlease check the FIFO numbers for the DCC with FMM ID number "+printFMMID+" before attempting to upload the configuration.");
					allClear = false;
					throw $break;
				}
			});
			if (!allClear) throw $break;
		});
		if (!allClear) throw $break;
	});
	if (!allClear) return;

	// Check for sane RUIs
	// Make sure fiber names are sane
	($$(".input_fifo_rui")).each(function(eli) {
		var rui = eli.value;
		if (!isPosInt(rui)) {
			eli.addClassName("inputError");
			alert("FIFO RUI numbers must be positive integers, and \""+rui+"\" is not.");
			allClear = false;
			throw $break;
		}

		var crate = eli.readAttribute("crate");
		var foundMatch = false;
		($$(".input_ddu_rui")).each(function(elj) {
			if (elj.readAttribute("crate") == crate && elj.value == rui) {
				foundMatch = true;
				throw $break;
			}
		});

		if (!foundMatch && !confirm("There is no RUI "+rui+" in crate "+crate+", but a FIFO in that crate is attempting to reference it.\n\nPress \"OK\" to continue, \"Cancel\" to abort the upload.")) {
			eli.addClassName("inputWarning");
			allClear = false;
			throw $break;
		}

	});
	if (!allClear) return;

	// Build a new system
	buildSystem();
}

function buildSystem() {
	var params = new Hash();
	params.set("systemName", $("input_database_name").value);
	params.set("key", $("input_database_key").value);

	var url = URL + "/BuildSystem";

	new Ajax.Request(url, {
		method: "get",
		asynchronous: false,
		parameters: params,
		onSuccess: buildCrates,
		onFailure: reportError
	});
}

function buildCrates(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new system: " + data.error);
		return;
	}

	systemsToUpdate--;

	// Crates in parallel

	var url = URL + "/BuildCrate";

	($$(".input_crate_number")).each(function(crateElement) {
		var params = new Hash();
		params.set("number", crateElement.value);
		params.set("fakeCrate", crateElement.readAttribute("crate"));

		new Ajax.Request(url, {
			method: "get",
			asynchronous: false,
			parameters: params,
			onSuccess: buildController,
			onFailure: reportError
		});
	});
}

function buildController(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new crate: " + data.error);
		return;
	}

	cratesToUpdate--;

	var params = new Hash({"fakeCrate": data.fakeCrate, "crate": data.crate});
	($$(".input_controller_device")).each(function(controllerElement) {
		if (controllerElement.readAttribute("crate") == data.fakeCrate) {
			params.set("device", controllerElement.value);
			throw $break;
		}
	});
	($$(".input_controller_link")).each(function(controllerElement) {
		if (controllerElement.readAttribute("crate") == data.fakeCrate) {
			params.set("link", controllerElement.value);
			throw $break;
		}
	});

	var url = URL + "/BuildController";
	new Ajax.Request(url, {
		method: "get",
		asynchronous: false,
		parameters: params,
		onSuccess: buildBoards,
		onFailure: reportError
	});
}

function buildBoards(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new controller: " + data.error);
		return;
	}

	controllersToUpdate--;

	// DDUs in parallel

	var url = URL + "/BuildDDU";

	($$(".input_ddu_rui")).each(function(dduElement) {
		if (dduElement.readAttribute("crate") != data.fakeCrate) return;
		var params = new Hash();
		var fakeRUI = dduElement.readAttribute("rui");
		params.set("fakeCrate", data.fakeCrate);
		params.set("fakeRUI", fakeRUI);
		params.set("crate", data.crate);

		($$("[class^='input_ddu_']")).each(function(newElement) {
			if (newElement.readAttribute("rui") == fakeRUI && newElement.readAttribute("crate") == data.fakeCrate) {
				if (newElement.checked) params.set(newElement.readAttribute("class"), 1);
				else params.set(newElement.readAttribute("class"), newElement.value);
			}
		});

		new Ajax.Request(url, {
			method: "get",
			asynchronous: false,
			parameters: params,
			onSuccess: buildFibers,
			onFailure: reportError
		});
	});

	// DCCs in parallel

	url = URL + "/BuildDCC";

	($$(".input_dcc_fmm_id")).each(function(dccElement) {
		if (dccElement.readAttribute("crate") != data.fakeCrate) return;
		var params = new Hash();
		var fakeFMMID = dccElement.readAttribute("fmmid");
		params.set("fakeCrate", data.fakeCrate);
		params.set("fakeFMMID", fakeFMMID);
		params.set("crate", data.crate);

		($$("[class^='input_dcc_']")).each(function(newElement) {
			if (newElement.readAttribute("fmmid") == fakeFMMID && newElement.readAttribute("crate") == data.fakeCrate) {
				if (newElement.checked) params.set(newElement.readAttribute("class"), 1);
				else params.set(newElement.readAttribute("class"), newElement.value);
			}
		});

		new Ajax.Request(url, {
			method: "get",
			asynchronous: false,
			parameters: params,
			onSuccess: buildFIFOs,
			onFailure: reportError
		});
	});
}

function buildFibers(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new DDU: " + data.error);
		return;
	}

	ddusToUpdate--;

	// Fibers in parallel

	var url = URL + "/BuildFiber";

	($$(".input_fiber_number")).each(function(fiberElement) {
		if (fiberElement.readAttribute("crate") != data.fakeCrate || fiberElement.readAttribute("rui") != data.fakeRUI) return;
		var params = new Hash();
		var fakeNumber = fiberElement.readAttribute("fiber");
		params.set("crate", data.crate);
		params.set("rui", data.rui);

		($$("[class^='input_fiber_']")).each(function(newElement) {
			if (newElement.readAttribute("rui") == data.fakeRUI && newElement.readAttribute("crate") == data.fakeCrate && newElement.readAttribute("fiber") == fakeNumber) {
				if (newElement.checked) params.set(newElement.readAttribute("class"), 1);
				else params.set(newElement.readAttribute("class"), newElement.value);
			}
		});

		new Ajax.Request(url, {
			method: "get",
			asynchronous: false,
			parameters: params,
			onSuccess: checkFiberError,
			onFailure: reportError
		});
	});
}

function buildFIFOs(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new DCC: " + data.error);
		return;
	}

	dccsToUpdate--;

	// FIFOs in parallel

	var url = URL + "/BuildFIFO";

	($$(".input_fifo_number")).each(function(fifoElement) {
		if (fifoElement.readAttribute("crate") != data.fakeCrate || fifoElement.readAttribute("fmmid") != data.fakeFMMID) return;
		var params = new Hash();
		var fakeNumber = fifoElement.readAttribute("fifo");
		params.set("crate", data.crate);
		params.set("fmmid", data.fmmid);

		($$("[class^='input_fifo_']")).each(function(newElement) {
			if (newElement.readAttribute("fmmid") == data.fakeFMMID && newElement.readAttribute("crate") == data.fakeCrate && newElement.readAttribute("fifo") == fakeNumber) {
				if (newElement.checked) params.set(newElement.readAttribute("class"), 1);
				else params.set(newElement.readAttribute("class"), newElement.value);
			}
		});

		new Ajax.Request(url, {
			method: "get",
			asynchronous: false,
			parameters: params,
			onSuccess: checkFIFOError,
			onFailure: reportError
		});
	});
}

function checkFiberError(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new fiber: " + data.error);
		return;
	}

	fibersToUpdate--;
}

function checkFIFOError(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to create a new FIFO: " + data.error);
		return;
	}

	fifosToUpdate--;
}

function checkXMLUpdate(pe) {
	if (!allClear) {
		($$(".action_button")).each(function(el) { el.disabled = false; });
		$("spinner").remove();
		pe.stop();
		return;
	}
	if (systemsToUpdate + cratesToUpdate + controllersToUpdate + ddusToUpdate + dccsToUpdate + fibersToUpdate + fifosToUpdate > 0) return;
	pe.stop();
	getXML();
}

function getXML() {
	($$(".action_button")).each(function(el) { el.disabled = false; });
	$("spinner").remove();
	window.location.href = URL + '/WriteXML';
}

function checkUploadUpdate(pe) {
	if (!allClear) {
		($$(".action_button")).each(function(el) { el.disabled = false; });
		$("spinner").remove();
		pe.stop();
		return;
	}
	if (systemsToUpdate + cratesToUpdate + controllersToUpdate + ddusToUpdate + dccsToUpdate + fibersToUpdate + fifosToUpdate > 0) return;
	pe.stop();
	doUpload();
}

function doUpload() {
	($$(".action_button")).each(function(el) { el.disabled = false; });
	$("spinner").remove();

	var url = URL + '/UploadToDB';

	new Ajax.Request(url, {
		method: "get",
		onSuccess: finalUploadCheck,
		onFailure: reportError
	});
}

function finalUploadCheck(transport) {
	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to upload to the database: " + data.error);
		return;
	} else {
		alert("Your database configuration has been uploaded.\n\nThe configuration system name is \""+data.systemName+"\"\nThe configuration key is "+data.key);
		getDBKeys();
	}
}
