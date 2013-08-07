#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "csc_display/JSON.h"

JSON::JSON() {
    midString = "";
    str_temp = "";
    added = 0;
}

JSON::~JSON() {
    // ???
}

void JSON::addInt(const char* name, int val) {
    if (added++ > 0) midString = midString + ",";
    
    char valBuf[10];
    sprintf(valBuf, "%d", val);
    
    midString = midString + "\"" + name + "\"" + ":";
    midString = midString + valBuf;
}

void JSON::addStr(const char* name, char* val) {
    if (added++ > 0) midString = midString + ",";
    midString = midString + "\"" + name + "\"" + ":";
    midString = midString + "\"" + val  + "\"";
}

void JSON::addObj(const char* name, JSON* val) {
    if (added++ > 0) midString = midString + ",";
    midString = midString + "\"" + name + "\"" + ":";
    midString = midString + val->str();
}

void JSON::addArr(const char* name, int* val, int len) {
    if (added++ > 0) midString = midString + ",";
    midString = midString + "\"" + name + "\"" + ":";
    
    midString = midString + "[";
    for (int i = 0; i < len; i++) {
        char valBuf[10];
        sprintf(valBuf, "%d", val[i]);
        
        if (i > 0) midString = midString + ",";
        midString = midString + valBuf;
    }
    midString = midString + "]";
}

const char* JSON::str() {
    str_temp = "{" + midString + "}";
    return str_temp.c_str();
}
