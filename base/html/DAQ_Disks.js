function Disk( host, mount, time, state, usage, free ){
  this.host  = host ;
  this.mount = mount;
  this.time  = time ;
  this.state = state;
  this.usage = usage; // [%]
  this.free  = free ; // [MB]
}

function byHost( a, b ){
  return a.host > b.host;
}

function jsonToTable( jsonURL ){

  // Get data disk info
  $.getJSON( jsonURL, function(json) {
    var disks  = new Array();
    $.each( json.table.rows, function(i,contextRow){
      if ( contextRow.context.lastIndexOf(':9999') > 0 ){
	$.each( contextRow.diskUsage.rows, function(j,fsRow){ 
	  if ( fsRow.fileSystem == '/data' ){
	    var host = contextRow.context.match('^http://([^:]+):[0-9]+')[1];
	    disks.push( new Disk( host, '/data', fsRow.sampleTime, fsRow.state, 
				  fsRow.usePercent, (1.-0.01*fsRow.usePercent)*fsRow.totalMB ) );
	  }
	});
      }
    });
    // Create pop-up table
    var popupTable = '<table><tr><th>disk</th><th>usage [%]</th><th>free [MB]</th><th>state</th><th>time</th></tr>';
    $.each( disks.sort(byHost), function(i,disk){
      var usageClass = 'ON';
      if      ( disk.usage > 80 ) usageClass = 'WARN';
      else if ( disk.usage > 95 ) usageClass = 'OFF';
      var stateClass = ( disk.state == 'OK' ? 'ON' : 'OFF' );
      popupTable += '<tr>'  + 
	'<td>'                            + disk.host+':'+disk.mount + '</td>' +
	'<td class="' + usageClass + '">' + disk.usage.toFixed(0)    + '</td>' +
	'<td>'                            + disk.free.toFixed(0)     + '</td>' +
	'<td class="' + stateClass + '">' + disk.state               + '</td>' +
	'<td>'                            + disk.time                + '</td>' +
	'</tr>';
    });
    popupTable += '</table>';
    $('body').append( popupTable );
  });

}
