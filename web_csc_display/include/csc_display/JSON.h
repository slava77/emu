#ifndef _CSC_DISPLAY_JSON_H_
#define _CSC_DISPLAY_JSON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string>

class JSON {
    public:
    
    JSON();
    ~JSON();
    
    void addInt(const char* name, int val);
    void addStr(const char* name, char* val);
    void addObj(const char* name, JSON* val);
    void addArr(const char* name, int* val, int len); // only array of int that is supported
    
    const char* str();
    
    private:
    
    std::string midString;
    std::string str_temp;
    int added;
};

#endif
