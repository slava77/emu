/*****************************************************************************\
* $Id: definitions.js,v 1.1 2009/07/03 23:33:29 paste Exp $
\*****************************************************************************/

function ReloadElement() {
	this.id = null;
	this.reloadFunction = null;
	this.callbackSuccess = null;
	this.callbackError = null;
	this.timeSinceReload = 0;
	this.timeToReload = 10;
	this.timeToError = 60;
	this.stop = false;
	this.lastReloadTime = "";
	function tickFunction(now) {
		var updateElement;
		if (updateElement = $(this.id + "_loadtime")) {
			var min = parseInt(this.timeSinceReload / 60).toString();
			var sec = (this.timeSinceReload % 60).toString();
			if (sec.length == 1) sec = "0" + sec;
			var time = min + ":" + sec;
			if (this.timeSinceReload == 0) {
				var date = new Date();
				this.lastReloadTime = date.toLocaleString();
			}
			updateElement.update(this.lastReloadTime + " (" + time + " ago)");
		}
		this.timeSinceReload++;
		if (this.timeSinceReload % this.timeToReload == 0 || now == true) {
			if (this.timeSinceReload > this.timeToError) {
				if (updateElement && !updateElement.hasClassName("Failed")) {
					updateElement.addClassName("Failed");
				}
			}
			if (this.stop) return;
			$(this.id + "_loadicon").setAttribute("src", "/emu/emuDCS/FEDApps/images/ajax-loader.gif");
			this.reloadFunction();
		} else if (updateElement && updateElement.hasClassName("Failed")) updateElement.removeClassName("Failed");
	}
	this.tick = tickFunction;
	function resetFunction() {
		var updateElement;
		if (updateElement = $(this.id + "_loadicon")) updateElement.setAttribute("src", "/emu/emuDCS/FEDApps/images/empty.gif");
		this.timeSinceReload = 0;
		this.tick();
	}
	this.reset = resetFunction;
}

var reloadElements = new Array();
