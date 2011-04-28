 ageOfPage=0;
 function countSeconds(){
    hours=Math.floor(ageOfPage/3600);
    minutes=Math.floor(ageOfPage/60)%60;
    age='';
    if (hours) age+=hours+' h ';
    if (minutes) age+=minutes+' m ';
    age+=ageOfPage%60+' s ';
    document.getElementById('ageOfPage').innerHTML=age;
    ageOfPage=ageOfPage+1;
    setTimeout('countSeconds()',1000);
 }

function onRecycle( ruiInstance ){
    document.forms['countTable'][ruiInstance].value = ruiInstance;
    document.forms['countTable'][ruiInstance].name = 'recycle';
}
