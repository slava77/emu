#ifndef _emuSTEPConfigRead_h_
#define _emuSTEPConfigRead_h_

#include <iostream>
#include <string>
#include <map>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "test_config_struct.h"

using namespace std;

class emuSTEPConfigRead
{
 public:

	void write_last_used
	(
		string endcap, 
		string chamtype, 
		string chamnum, 
		string run_type, 
		unsigned long run_number, 
		bool ddu_in_[16]
	);

	int read_last_used
	(
		string &endcap, 
		string &chamtype, 
		string &chamnum, 
		string &run_type, 
		unsigned long &run_number, 
		bool *ddu_in_
	);

	int read_test_config(char* xmlFile, test_config_struct * tcs);

	string& trim(string &str);
};

#endif
