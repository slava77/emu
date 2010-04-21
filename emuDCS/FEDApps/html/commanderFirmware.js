/*****************************************************************************\
* $Id: commanderFirmware.js,v 1.5 2010/04/21 13:19:25 paste Exp $
\*****************************************************************************/

// Global for the required pause in uploading
//var uploading = false;

function reportError(transport) {
	//this.stop = true;
	alert("The following error has occurred: " + transport.statusText + ".  An expert should be contacted.");
}

var Button = Class.create({

	initialize: function(id) {
		this.element = $(id);
		this.text = this.element.innerHTML;
		this.success = function(data) {};
		this.element.observe("reset", this.reset.bind(this));
	},

	callback: function(transport) {

		var data = transport.responseJSON;

		if (data.error) {
			this.element.update("Error!");
			this.element.writeAttribute("title", data.error);
			this.element.writeAttribute("alt", data.error);
			this.element.addClassName("error");
			return;
		} else if (data.warning) {
			this.element.update("Warning!");
			this.element.writeAttribute("title", data.warning);
			this.element.writeAttribute("alt", data.warning);
			this.element.addClassName("warning");
		} else {
			this.reset();
		}

		this.success(data);
	},

	reset: function() {
		this.element.writeAttribute("title", "").writeAttribute("alt", "");
		this.element.removeClassName("error").removeClassName("warning").update(this.text);
	}
});

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
			ev.element().update("Uploading.  Patience, please...");
			$$(".upload").each(function(ele) {
				ele.disabled = true;
			});
			// Make sure the updater knows that we are uploading
			//uploading = true;
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

	if ($("reset")) {
	
		var button = new Button("reset");
		
		
		($("reset")).observe("click", function(event) {
			event.element().update("Resetting").disabled = true;
			var url = URL + "/DDUReset";
			var params = new Hash();
			params.set("crate", crateNumber);
			
			new Ajax.Request(url, {
				method: "get",
				parameters: params,
				onSuccess: button.callback.bind(button),
				onFailure: reportError
			});
		});
		
		button.success = function(data) {
			this.element.update(this.text).disabled = false;
			data.boards.each(function(board) {
				var slot = board.slot;
				board.chips.each(function(chip) {
					var name = chip.chip;
					$(name + "_label_" + slot).update(chip.version);
				});
			});
		}
	}

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
	var disable = (alldone != percents.size() * data.boards.size());

	// Now update the submit buttons
	percents.each(function(it) {

		if (it.value != 100) {
			$(it.key + "_upload").update("Installing " + it.value + "%").disabled = true;
		} else {
			$(it.key + "_upload").update("Upload and install").disabled = disable;
		}
	});

}

function updateReset(transport)
{
	var data = transport.responseJSON;

	if (data.error) {
		
	}

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
	var disable = (alldone != percents.size() * data.boards.size());

	// Now update the submit buttons
	percents.each(function(it) {

		if (it.value != 100) {
			$(it.key + "_upload").update("Installing " + it.value + "%").disabled = true;
		} else {
			$(it.key + "_upload").update("Upload and install").disabled = disable;
		}
	});
}
