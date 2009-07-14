var prevTooltip;

function getWindowWidth() {
    
    if(window.innerWidth) {
        return window.innerWidth;
    } 
    
    return document.body.clientWidth
}

function mouseX(e) {

    if(e.pageX) {
        return e.pageX;
    }
    
    return e.clientX + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft);
}

function mouseY(e) {
   
   if(e.pageY) {
        return e.pageY;
    } 
    
    return e.clientY + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop);    
}

function tooltip(e, o) {

    var windowWidth = getWindowWidth();

    o = document.getElementById(o);
    
    if(prevTooltip && prevTooltip != o) {
        prevTooltip.style.visibility = 'hidden';
    }

    if(o.style.visibility == 'visible') {

        o.style.visibility = 'hidden';
    } else {
    
        if(o.offsetWidth) {
            ew = o.offsetWidth;
        } else if(o.clip.width) {
            ew = o.clip.width;
        }
    
        y = mouseY(e) + 16;
        x = mouseX(e) - (ew / 4);
        
        if (x < 2) {
            x = 2;
        } else if(x + ew > windowWidth) {
//             x = windowWidth - ew - 4;
            x = windowWidth - ew - 20;
        }
        
        o.style.left = x + 'px';
        o.style.top = y + 'px';    
        
        o.style.visibility = 'visible';
        
        prevTooltip = o;
    }
}