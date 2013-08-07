var routineProcessID = 0;

function initProcess() {
    // cancel the routine process if there is one
    if (routineProcessID != 0) clearTimeout(routineProcessID);
    
    var filename = getFileName();
    $.ajax({
        
        // send data to "init" handler to get basic parameters of the file
        url: "init",
        dataType: "json",
        type: "GET",
        data: { f: filename },
        
        success: function(json) {
            // if successfully open the file
            if (json.status == 1) {
                removeAllChambersID();
                addChambersID(json.data.chambersID);
                setChamberID(parseInt(json.data.chambersID[0]));
                setTotalEventNum(json.data.totalEventNum);
                setEventNum(1);
                setReadyStatus(1);
                routineProcessID = setTimeout(routineProcess, 10);
            }
            // if the file is failed to open
            else {
                alert(json.msg);
                changeFileName();
            }
        }
    });
}

function routineProcess() {
    var play = getPlayStatus();
    
    // determine its delay time between routine process
    var speed = getSpeed();
    var delay;
    switch (speed) {
        case 1: delay = 2000; break;
        case 2: delay = 1500; break;
        case 3: delay = 1000; break;
        case 4: delay = 700 ; break;
        default:delay = 500 ;
    }
    
    if (play) {
        
        // update the graph based on current set parameters
        showCurrentGraph();
        
        // set the routine process after the image has been loaded
        routineProcessID = setTimeout(function() {
            
            // update the event number based on its current playing status
            if (getPlayStatus()) {
                var nextEventNum = getEventNum() + 1;
                if (nextEventNum > getTotalEventNum()) nextEventNum = 1;
                setEventNum(nextEventNum);
            }
            routineProcess();
            
        }, delay);
    }
    else {
        routineProcessID = setTimeout(routineProcess, delay);
    }
}

$(document).ready(function() {
    $(document).ready();
    setPlayStatus(0);
});
