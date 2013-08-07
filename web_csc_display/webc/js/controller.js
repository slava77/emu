
function getEventNum() { // return int
    return parseInt($("#event_bar").val());
}

function setEventNum(evt_num) { // evt_num is int
    var evt_num_str = evt_num.toString();
    $("#event_num").html(evt_num_str);
    $("#event_text").val(evt_num_str);
    $("#event_bar").val(evt_num);
}


function getTotalEventNum() { // return int
    return parseInt($("#total_event_num").html());
}

function setTotalEventNum(total_evt_num) { // total_evt_num is int
    var totalEvtNumStr = total_evt_num.toString();
    $("#total_event_num").html(totalEvtNumStr);
    $("#event_bar").attr("max", totalEvtNumStr);
}


function getSpeed() { // return int
    return parseInt($("#speed_bar").val());
}

function setSpeed(speed_num) { // speed_num is int
    $("#speed_bar").val(speed_num);
}


function getFileName() { // return string
    return $("#filename_input").val();
}

function setFileName(filename) { // filename is string
    $("#filename").hide();
	$("#filename_input").val(filename);
    $("#filename_disp").html(filename).show();
    $("#filename_close").show();
}

function changeFileName() { // no in-out, just void
    $("#filename_disp").hide();
    $("#filename").show();
    $("#filename_close").hide();
}


function getPlayStatus() { // return bool
    return parseInt($("#play_status").val()) == 1;
}

function setPlayStatus(play) { // play is bool
    // check ready status
    var ready = getReadyStatus();
    if (play && (!ready)) {
        $("#filename").focus();
        alert("Please type a file name and press enter");
        return;
    }
    
    var play_str = play ? "1" : "0";
    $("#play_status").val(play_str);
    
    // set the display of play/pause button according to its play status
    if (play) {
        $("#play_btn").attr("src", "img?f=play-dark.png");
        $("#pause_btn").attr("src", "img?f=pause.png");
    }
    else {
        $("#play_btn").attr("src", "img?f=play.png");
        $("#pause_btn").attr("src", "img?f=pause-dark.png");
    }
    
}


function getReadyStatus() { // return bool
    return parseInt($("#ready_status").val()) == 1;
}

function setReadyStatus(ready) { // ready is bool
    var ready_str = ready ? "1" : "0";
    $("#ready_status").val(ready_str);
}


function getHackMode() { // return bool
    return $("#hackmode").is(":checked");
}

function setHackMode(hack) { // hack is bool
    if (getHackMode() != hack)
        $("#hackmode").click();
}


function getAutoScale() { // return bool
    return $("#autoscale").is(":checked");
}

function setAutoScale(ascale) { // ascale is bool
    if (getAutoScale() != ascale)
        $("#autoscale").click();
}


function getConfMode() { // return string
    var confLabel = $("#conf_mode").html();
    return $(".conf_menu:contains('"+confLabel+"')").attr("conf");
}

function setConfMode(conf_mode) { // conf_mode is string
    var confLabel = $(".conf_menu[conf='"+conf_mode+"']").html();
    $("#conf_mode").html(confLabel);
}


function getChamberID() { // return int
    return parseInt($("#chamber_id").html());
}

function setChamberID(chamberID) { // chamberID is int
    $("#chamber_id").html(chamberID.toString());
}


function getCurrentGraphURL(header) {
    // set the default value of header
    header = (typeof header !== "undefined") ? header : false;
    
    // getting the parameter
    var evt_num = getEventNum().toString();
    var filename = getFileName();
    var chamberID = getChamberID().toString();
    var confMode = getConfMode();
    var hackMode = getHackMode() ? "1" : "0";
    var autoscale = getAutoScale() ? "1" : "0";
    var hdr = header ? "1" : "0";
    
    // getting the url of the graphic
    var enc = encodeURIComponent;
    var url = "graph";
    url += "?evt=" + enc(evt_num) + "&cid=" + enc(chamberID) + "&f=" + enc(filename) + "&hdr=" + enc(hdr);
    url += "&conf=" + enc(confMode) + "&hack=" + enc(hackMode) + "&ascale=" + enc(autoscale);
    
    return url;
}

function showCurrentGraph(callback) {
    if (typeof callback === "undefined") callback = function() {}; // do nothing
    
    var url = getCurrentGraphURL();
    
    // change the graph to the new graph
    $("#graph").load(callback).attr("src", url);
    $("#graph-link").attr("href", url);
}

// other functions (setup the unchanged variables, e.g. total event numbers, chambers ID, etc)

// add chambers menu
function addChambersID(ids) { // ids is array of chamber ID (array of int)
    for (var i = 0; i < ids.length; i++) {
        var id = ids[i].toString();
        $("#chambers_menu").append('<li><a href="#" onclick="setChamberID('+id+');showCurrentGraph()">Chamber ' + id + '</a></li>');
    }
}

// remove all chambers menu
function removeAllChambersID() {
    $("#chambers_menu").html("");
}

// function to print the current graphic by 
function printGraphic() {
    // set the content of new printing window
    var graphURL = getCurrentGraphURL(true);
    var content = "<img src='" + graphURL + "' onload='setTimeout(function(){window.print()},100)'/>";
    
    // open new window for printing
    var openWindow = window.open("", "print-w", "");
    openWindow.document.write(content);
}

// initial function, to set all the handler into corresponding HTML elements
$(document).ready(function() {
    $(document).ready();
    
    // configuration menu
    $(".conf_menu").click(function() {setConfMode($(this).attr("conf"));showCurrentGraph();});
    
    // // run menu
    // $("#run_play").click(function() {setPlayStatus(1)});
    // $("#run_pause").click(function() {setPlayStatus(0)});
    // $("#run_rewind").click(function() {setEventNum(1)});
    
    $("#graph-link").click(function() {setPlayStatus(0);return true});
    
    // for chambers menu, their handlers are automatically added in addChambersID function
    
    // buttons handlers and run menu
    $("#play_btn, #run_play").click(function() {setPlayStatus(1)});
    $("#pause_btn, #run_pause").click(function() {setPlayStatus(0)});
    $("#rewind_btn, #run_rewind").click(function() {setPlayStatus(0);setEventNum(1);showCurrentGraph()});
    $("#print_btn").click(function() {printGraphic();setPlayStatus(0);});
    
    // event handler
    $("#event_text").keyup(function(e) {
        if (e.keyCode == 13) {
            setEventNum(parseInt($(this).val())); // if pressed enter
            showCurrentGraph();
        }
    });
    $("#event_bar").mouseup(function() {
        setEventNum($(this).val());
        showCurrentGraph();
    });
    
    // no need for speed handler, because changing the set speed function only change the position in speed bar
    
    $("#filename").keyup(function(e) {
        if (e.keyCode == 13) { // if press "enter"
            setFileName($(this).val());
            initProcess();
        }
    });
    $("#filename_close").click(function() {
        changeFileName();
        setReadyStatus(0);
        setPlayStatus(0);
    });
});

