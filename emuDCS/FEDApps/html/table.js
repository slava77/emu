/*****************************************************************************\
* $Id: table.js,v 1.2 2009/10/26 19:17:20 paste Exp $
\*****************************************************************************/

/** Javascript object for generating an HTML table **/

function Table(properties)
{
	this.properties = properties;
	
	this.rows = new Array();
	this.cols = new Array();

	this.toElement = function() {
	
		var eTable = new Element("table", this.properties);
		
		if (this.cols.size()) {
			var eColgroup = new Element("colgroup");
			this.cols.each(function(col) {
				var eCol = new Element("col", col.properties);
				eColgroup.insert(eCol);
			});
			eTable.insert(eColgroup);
		}
	
		this.rows.each(function(row) {
			eTable.insert(row.toElement());
		});
		
		return eTable;
	}
	
	this.toText = function() {
	
		if (!this.rows.size()) return "";
	
		// For printing
		var border = "+";
		var breaker = "+";
		var output = "";
	
		// Calculate the size of each column
		var maxCol = this.rows[0].cells.size();
		for (var iCol = 0; iCol < maxCol; ++iCol) {
			var colWidth = 0;
			this.rows.each(function(row) {
				if (!row.cells.size()) return;
				var w = row.cells[iCol].content.toString().length;
				if (w && w > colWidth) colWidth = w;
			});
			
			
			// Set the width of the elements in this column to the maximum.
			this.rows.each(function(row) {
				var cell = row.cells[iCol];
				cell.content = (" ").times(colWidth - cell.content.toString().length) + cell.content;
			});
			
			border += ("=").times(colWidth + 2) + "+";
			breaker += ("-").times(colWidth + 2) + "+";

		}
		
		output += border + "\n";
		// Print the rows	
		this.rows.each(function(row, iRow) {
			if (iRow != 0) output += breaker + "\n";
			output += "|" + row.toText() + "\n";
		});
		
		return output + border;
	}
}

function Row(properties)
{
	this.properties = properties;
	
	this.cells = new Array();
	
	this.toElement = function() {
	
		var eTr = new Element("tr", this.properties);
		
		this.cells.each(function(cell) {
			eTr.insert(cell.toElement());
		});
		
		return eTr;
	
	}
	
	this.toText = function() {
	
		var output = "";
		
		this.cells.each(function(cell) {
			output += " " + cell.content + " |";
		});
		
		return output;
	}
}

function Cell(properties)
{
	this.properties = properties;
	
	this.content = "";
	
	this.toElement = function() {
	
		var eTd = new Element("td", this.properties);
		eTd.update(this.content);
		return eTd;
	}
	
	this.update = function(content) {
		this.content = content;
		return this;
	}
}
