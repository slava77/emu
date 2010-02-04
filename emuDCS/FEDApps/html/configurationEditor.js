/*****************************************************************************\
* $Id: configurationEditor.js,v 1.5 2010/02/04 21:03:45 paste Exp $
\*****************************************************************************/

// Global variables follow

// Make this global.  It saves on DB queries if we have an evil user
var keyMap = new Hash();

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
	nameInput.success = function(d) {
		this.element.value = d.value;
	}
	newTR2.insert(new Element("td").insert(nameInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("DB key"));
	var keyInput = new TextInput("input_system_key", {value: data.system.key});
	keyInput.type = "System";
	keyInput.action = "update_key";
	keyInput.success = function(d) {
		this.element.value = d.value;
	}
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
		$$("[crate='"+d.previous+"']").each(function(el) {el.writeAttribute("crate", d.value)});
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
		$("dcc_add").writeAttribute("slot", d.nextSlot);
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
		$$("[rui='"+d.previous+"']").each(function(el) { el.writeAttribute("rui", d.value); });
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
		$("dcc_add").writeAttribute("slot", d.nextSlot);
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
		$$("[fiber='"+d.previous+"']").each(function(el) { el.writeAttribute("fiber", d.value); });
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
	nameInput.success = function(d) {
		this.element.value = d.value;
	}
	newTR3.insert(new Element("td").insert(nameInput.element));
	newTable.insert(newTR3);

	var newTR5 = new Element("tr");
	newTR5.insert(new Element("td", {class: "bold"}).update("Kill"));
	var killedInput = new CheckBoxInput("input_fiber_kill", {crate: data.crate, rui: data.rui, fiber: data.fiber.fiber}, data.fiber.killed);
	killedInput.type = "Fiber";
	killedInput.action = "update_killed";
	newTR5.insert(new Element("td").insert(killedInput.element));
	newTable.insert(newTR5);

	var newTR10 = new Element("tr");
	var deleteButton = new ButtonInput("input_fiber_delete", {value: data.fiber.fiber, crate: data.crate, rui: data.rui, fiber: data.fiber.fiber});
	deleteButton.type = "Fiber";
	deleteButton.action = "delete";
	deleteButton.success = function(d) {
		$("fiber_selector").select("[value='"+d.value+"']").invoke("remove");
		$("table_area").descendants().invoke("remove")
		$("table_area").update("");
		$("fiber_add").writeAttribute("value", d.nextFiber).writeAttribute("fiber", d.nextFiber);
	}
	deleteButton.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	deleteButton.element.insert("Delete Fiber");
	newTR10.insert(new Element("td", {colspan: 2}).insert(deleteButton.element));
	newTable.insert(newTR10);

	return newTable;

}



// Display the DCC table for editing and update the fiber numbers in the selector
function updateDCC(data) {

	var insertElement = $("table_area");

	// Eliminate everything in the insert location
	insertElement.descendants().invoke("remove")
	insertElement.update("");

	// Catch errors
	if (data.error) {
		insertElement.insert(data.error);
		return;
	}

	// Show the table elements corresponding to fifos
	$$(".fiber_cell").invoke("hide");
	$$(".fifo_cell").invoke("show");

	// Clear the fifo selectors
	$("fifo_selector").fire("reset");

	// Wipe the entries from the selector
	$("fifo_selector").descendants().invoke("remove");

	// Add the fifo numbers to the selectors
	$("fifo_selector").insert(new Element("option", {value: ""}).update("Select FIFO"));

	data.dcc.fifos.each(function(myfifo) {
		$("fifo_selector").insert(new Element("option", {value: myfifo.fifo, fifo: myfifo.fifo, fmmid: data.dcc.fmmid, crate: data.crate}).update("FIFO " + myfifo.fifo));
	});

	// Clear the add fifo button
	$("fifo_add").fire("reset");

	// Update the value of the add fifo button
	$("fifo_add").writeAttribute("value", data.nextFIFO).writeAttribute("crate", data.crate).writeAttribute("fmmid", data.dcc.fmmid).writeAttribute("fifo", data.nextFIFO);

	// Build and insert the DCC table
	var dccTable = makeDCCTable(data);
	insertElement.insert(dccTable);
}


// Make the table for editing a DCC
function makeDCCTable(data) {

	var newTable = new Element("table", {id: "dcc_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("DCC Properties"));
	newTable.insert(newTR1);

	var newTR0 = new Element("tr");
	newTR0.insert(new Element("td", {class: "bold"}).update("Slot"));
	var slotInput = new TextInput("input_dcc_slot", {value: data.dcc.slot, crate: data.crate, fmmid: data.dcc.fmmid});
	slotInput.type = "DCC";
	slotInput.action = "update_slot";
	slotInput.success = function(d) {
		$("dcc_add").writeAttribute("slot", d.nextSlot);
		$("ddu_add").writeAttribute("slot", d.nextSlot);
	}
	newTR0.insert(new Element("td").insert(slotInput.element));
	newTable.insert(newTR0);

	var newTR2 = new Element("tr");
	newTR2.insert(new Element("td", {class: "bold"}).update("FMM ID"));
	var fmmidInput = new TextInput("input_dcc_fmmid", {value: data.dcc.fmmid, crate: data.crate, fmmid: data.dcc.fmmid});
	fmmidInput.type = "DCC";
	fmmidInput.action = "update_fmmid";
	fmmidInput.success = function(d) {
		$("dcc_selector").select("[value='"+d.previous+"']").each(function(el) {
			el.writeAttribute("value", d.value).writeAttribute("fmmid", d.value).update("DCC " + d.value);
		});
		$$("[fmmid='"+d.previous+"']").each(function(el) { el.writeAttribute("fmmid", d.value); });
		this.parameters.set("fmmid", d.value);
		$("input_dcc_delete").writeAttribute("value", d.value).writeAttribute("fmmid", d.value);
		$("dcc_add").writeAttribute("value", d.nextFMMID).writeAttribute("fmmid", d.nextFMMID);
	}
	newTR2.insert(new Element("td").insert(fmmidInput.element));
	newTable.insert(newTR2);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("SLink 1 ID"));
	var slink1Input = new TextInput("input_dcc_slink1", {value: data.dcc.slink1, crate: data.crate, fmmid: data.dcc.fmmid});
	slink1Input.type = "DCC";
	slink1Input.action = "update_slink_1";
	newTR3.insert(new Element("td").insert(slink1Input.element));
	newTable.insert(newTR3);

	var newTR4 = new Element("tr");
	newTR4.insert(new Element("td", {class: "bold"}).update("SLink 2 ID"));
	var slink2Input = new TextInput("input_dcc_slink2", {value: data.dcc.slink2, crate: data.crate, fmmid: data.dcc.fmmid});
	slink2Input.type = "DCC";
	slink2Input.action = "update_slink_2";
	newTR4.insert(new Element("td").insert(slink2Input.element));
	newTable.insert(newTR4);

	var newTR5 = new Element("tr");
	newTR5.insert(new Element("td", {class: "bold"}).update("Activate software switch"));
	var swswitchInput = new CheckBoxInput("input_dcc_swswitch", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.swswitch);
	swswitchInput.type = "DCC";
	swswitchInput.action = "update_sw_switch";
	newTR5.insert(new Element("td").insert(swswitchInput.element));
	newTable.insert(newTR5);

	var newTR6 = new Element("tr");
	newTR6.insert(new Element("td", {class: "bold"}).update("Ignore TTC fiber"));
	var ignoreTTCInput = new CheckBoxInput("input_dcc_ignoreTTC", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.ignore_ttc);
	ignoreTTCInput.type = "DCC";
	ignoreTTCInput.action = "update_sw_ttc";
	newTR6.insert(new Element("td").insert(ignoreTTCInput.element));
	newTable.insert(newTR6);

	var newTR7 = new Element("tr");
	newTR7.insert(new Element("td", {class: "bold"}).update("Ignore SLink backpressure"));
	var ignoreBackpressureInput = new CheckBoxInput("input_dcc_ignoreBackpressure", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.ignore_backpressure);
	ignoreBackpressureInput.type = "DCC";
	ignoreBackpressureInput.action = "update_sw_backpressure";
	ignoreBackpressureInput.success = function(d) {
		if (d.value) $("input_dcc_ignoreSLink").checked = false;
	}
	newTR7.insert(new Element("td").insert(ignoreBackpressureInput.element));
	newTable.insert(newTR7);

	var newTR8 = new Element("tr");
	newTR8.insert(new Element("td", {class: "bold"}).update("Ignore all SLink errors"));
	var ignoreSLinkInput = new CheckBoxInput("input_dcc_ignoreSLink", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.ignore_slink);
	ignoreSLinkInput.type = "DCC";
	ignoreSLinkInput.action = "update_sw_slink";
	ignoreSLinkInput.success = function(d) {
		if (d.value) $("input_dcc_ignoreBackpressure").checked = false;
	}
	newTR8.insert(new Element("td").insert(ignoreSLinkInput.element));
	newTable.insert(newTR8);

	var newTR9 = new Element("tr");
	newTR9.insert(new Element("td", {class: "bold"}).update("Software switch bit 4"));
	var bit4Input = new CheckBoxInput("input_dcc_bit4", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.bit4);
	bit4Input.type = "DCC";
	bit4Input.action = "update_sw_bit4";
	newTR9.insert(new Element("td").insert(bit4Input.element));
	newTable.insert(newTR9);

	var newTR10 = new Element("tr");
	newTR10.insert(new Element("td", {class: "bold"}).update("Software switch bit 5"));
	var bit5Input = new CheckBoxInput("input_dcc_bit4", {crate: data.crate, fmmid: data.dcc.fmmid}, data.dcc.bit5);
	bit5Input.type = "DCC";
	bit5Input.action = "update_sw_bit5";
	newTR10.insert(new Element("td").insert(bit5Input.element));
	newTable.insert(newTR10);

	var newTR11 = new Element("tr");
	var deleteButton = new ButtonInput("input_dcc_delete", {value: data.dcc.fmmid, crate: data.crate, fmmid: data.dcc.fmmid});
	deleteButton.type = "DCC";
	deleteButton.action = "delete";
	deleteButton.success = function(d) {
		$("dcc_selector").select("[value='"+d.value+"']").invoke("remove");
		$$(".fiber_cell").invoke("hide");
		$$(".fifo_cell").invoke("hide");
		$("table_area").descendants().invoke("remove")
		$("table_area").update("");
		$("dcc_add").writeAttribute("value", d.nextFMMID).writeAttribute("fmmid", d.nextFMMID).writeAttribute("slot", d.nextSlot);
		$("ddu_add").writeAttribute("slot", d.nextSlot);
	}
	deleteButton.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	deleteButton.element.insert("Delete DCC");
	newTR11.insert(new Element("td", {colspan: 2}).insert(deleteButton.element));
	newTable.insert(newTR11);

	return newTable;

}



// Display the FIFO table for editing and update the fiber numbers in the selector
function updateFIFO(data) {

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
	var fifoTable = makeFIFOTable(data);
	insertElement.insert(fifoTable);
}


// Make the table for editing a fiber
function makeFIFOTable(data) {

	var newTable = new Element("table", {id: "fiber_table", class: "tier1"});

	var newTR1 = new Element("tr", {class: "title"});
	newTR1.insert(new Element("td", {colspan: 2}).update("FIFO Properties"));
	newTable.insert(newTR1);

	var newTR0 = new Element("tr");
	newTR0.insert(new Element("td", {class: "bold"}).update("Number"));
	var fifoInput = new TextInput("input_fifo_number", {value: data.fifo.fifo, crate: data.crate, fmmid: data.fmmid, fifo: data.fifo.fifo});
	fifoInput.type = "FIFO";
	fifoInput.action = "update_number";
	fifoInput.success = function(d) {
		$("fifo_selector").select("[value='"+d.previous+"']").each(function(el) {
			el.writeAttribute("value", d.value).writeAttribute("fifo", d.value).update("FIFO " + d.value);
		});
		$$("[fifo='"+d.previous+"']").each(function(el) { el.writeAttribute("fifo", d.value); });
		this.parameters.set("fifo", d.value);
		$("input_fifo_delete").writeAttribute("value", d.value).writeAttribute("fifo", d.value);
		$("fifo_add").writeAttribute("value", d.nextFIFO).writeAttribute("fifo", d.nextFIFO);
	}
	newTR0.insert(new Element("td").insert(fifoInput.element));
	newTable.insert(newTR0);

	var newTR3 = new Element("tr");
	newTR3.insert(new Element("td", {class: "bold"}).update("RUI"));
	var ruiInput = new TextInput("input_fifo_rui", {value: data.fifo.rui, crate: data.crate, fmmid: data.fmmid, fifo: data.fifo.fifo});
	ruiInput.type = "FIFO";
	ruiInput.action = "update_rui";
	newTR3.insert(new Element("td").insert(ruiInput.element));
	newTable.insert(newTR3);

	var newTR5 = new Element("tr");
	newTR5.insert(new Element("td", {class: "bold"}).update("FIFO in use"));
	var useInput = new CheckBoxInput("input_fifo_use", {crate: data.crate, fmmid: data.fmmid, fifo: data.fifo.fifo}, data.fifo.use);
	useInput.type = "FIFO";
	useInput.action = "update_use";
	newTR5.insert(new Element("td").insert(useInput.element));
	newTable.insert(newTR5);

	var newTR10 = new Element("tr");
	var deleteButton = new ButtonInput("input_fifo_delete", {value: data.fifo.fifo, crate: data.crate, fmmid: data.fmmid, fifo: data.fifo.fifo});
	deleteButton.type = "FIFO";
	deleteButton.action = "delete";
	deleteButton.success = function(d) {
		$("fifo_selector").select("[value='"+d.value+"']").invoke("remove");
		$("table_area").descendants().invoke("remove")
		$("table_area").update("");
		$("fifo_add").writeAttribute("value", d.nextFIFO).writeAttribute("fifo", d.nextFIFO);
	}
	deleteButton.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-remove.png"}));
	deleteButton.element.insert("Delete FIFO");
	newTR10.insert(new Element("td", {colspan: 2}).insert(deleteButton.element));
	newTable.insert(newTR10);

	return newTable;

}



function findChamber(needle) {

	// Send for cgicc to do the dirty work
	var url = URL + '/FindChamber';

	var parameters = new Hash();
	parameters.set("needle", needle);
	if (needle.indexOf('+') >= 0) {
		parameters.set("plusPos", needle.indexOf('+'));
	}

	new Ajax.Request(url, {
		method: "get",
		parameters: parameters,
		onSuccess: findChamberCallback,
		onFailure: reportError
	});
}

function findChamberCallback(transport) {

	var data = transport.responseJSON;

	if (data.error) {
		alert(data.error);
		return;
	}

	// This is just like a click on the fiber selector
	updateFiber(data);

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
	dccDrop.success = updateDCC.bind(dccDrop);
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
	fifoDrop.success = updateFIFO.bind(fifoDrop);
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
		$("dcc_add").writeAttribute("slot", data.nextSlot);
		$("ddu_selector").insert(new Element("option", {value: data.value, rui: data.value, crate: data.crate}).update("DDU " + data.value));
	}
	newDDU.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newDDU.element.insert("Add DDU");
	$("board_cell").insert(new Element("br")).insert(newDDU.element);

	var newDCC = new ButtonInput("dcc_add", {class: "add"});
	newDCC.type = "DCC";
	newDCC.action = "add";
	newDCC.success = function(data) {
		this.element.writeAttribute("value",  data.nextFMMID).writeAttribute("fmmid", data.nextFMMID).writeAttribute("slot", data.nextSlot);
		$("ddu_add").writeAttribute("slot", data.nextSlot);
		$("dcc_selector").insert(new Element("option", {value: data.value, fmmid: data.value, crate: data.crate}).update("DCC " + data.value));
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
		this.element.writeAttribute("value",  data.nextFIFO).writeAttribute("fifo", data.nextFIFO);
		$("fifo_selector").insert(new Element("option", {value: data.value, fifo: data.value, fmmid: data.fmmid, crate: data.crate}).update("FIFO " + data.value));
	}
	newFIFO.element.insert(new Element("img", {class: "icon", src: "/emu/emuDCS/FEDApps/images/list-add.png"}));
	newFIFO.element.insert("Add FIFO");
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

	$("find_a_fiber").observe("keypress", function(ev) {
		if (ev.keyCode == Event.KEY_RETURN) {
			findChamber(($("find_a_fiber")).value);
		}
	});

	$("find_button").observe("click", function(ev) {
		findChamber(($("find_a_fiber")).value);
	});


	$("write_xml").disabled = false;
	$("write_xml").observe("click", function(ev) {
		window.location.href = URL + '/WriteXML';
	});

	$("upload_to_db").disabled = false;
	$("upload_to_db").observe("click", function(ev) {
		($$(".action_button")).each(function(el) { el.disabled = true; });
		ev.element().update("Uploading to database...").insert(new Element("img", {src: "/emu/emuDCS/FEDApps/images/ajax-loader.gif", id: "spinner"}));

		var url = URL + '/UploadToDB';

		new Ajax.Request(url, {
			method: "get",
			onSuccess: finalUploadCheck,
			onFailure: reportError
		});

	});

});

function finalUploadCheck(transport) {

	($$(".action_button")).each(function(el) { el.disabled = false; });
	$("upload_to_db").update("Upload configuration to database");

	var data = transport.responseJSON;
	if (data.error) {
		alert("The following error occured trying to upload to the database: " + data.error);
		return;
	} else {
		alert("Your database configuration has been uploaded.\n\nThe configuration system name is \""+data.systemName+"\"\nThe configuration key is "+data.key);
		getDBKeys();
	}
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
