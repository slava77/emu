#ifndef _csc_display_utils_h
#define _csc_display_utils_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>

#include "xgi/Method.h"
#include "cgicc/Cgicc.h"

std::string getFormValue(const std::string& elmt_name, xgi::Input * in);
int getFormIntValue(const std::string& elmt_name, xgi::Input * in);
void getFileContent(const char* fileType, const char* filename, char** content, int* size=NULL);

#endif
