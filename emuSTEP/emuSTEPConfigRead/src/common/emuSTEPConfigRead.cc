#include "xercesc/util/PlatformUtils.hpp"
#include "emuSTEPConfigRead.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace XERCES_CPP_NAMESPACE;

#define FILL_LAST_USED_MAP \
map_last["endcap"]     = &endcap; \
map_last["chamtype"]   = &chamtype; \
map_last["chamnum"]    = &chamnum; \
map_last["run_type"]   = &run_type; \
map_last["run_number"] = &run_number_str; \
map_last["ddu_in"]     = &ddu_in_str;

void emuSTEPConfigRead::write_last_used
(
	string endcap, 
	string chamtype, 
	string chamnum, 
	string run_type, 
	unsigned long run_number, 
	bool ddu_in_[16]
)
{
	char* step_path_ch = getenv("STEPHOME");
	if (step_path_ch == NULL) return;
	
	string step_last_used = string(step_path_ch) + "/data/xml/test_config/last_used.xml";

	fstream f (step_last_used.c_str(), fstream::out);

	// convert non-strings into strings
	ostringstream run_number_s;
	run_number_s << run_number;
	string run_number_str = run_number_s.str();

	ostringstream ddu_in_s;
	for (int i = 0; i < 16; i++) ddu_in_s << (ddu_in_[i] ? "1" : "0");
	string ddu_in_str = ddu_in_s.str();

	std::map <std::string, string*> map_last; // parameter map
	std::map <std::string, string*>::iterator it;

	// fill the map
	FILL_LAST_USED_MAP;

	// write the file
	f << "<STEP_last_used>" << endl;
	f << "\t<params>" << endl;
	
	for (it = map_last.begin(); it != map_last.end(); it++)
		f << "\t\t<" << it->first << ">" << *(it->second) <<  "</" << it->first << ">" << endl;

	f << "\t</params>" << endl;
	f << "</STEP_last_used>" << endl;

	f.close();
}


int emuSTEPConfigRead::read_last_used
(
	string &endcap, 
	string &chamtype, 
	string &chamnum, 
	string &run_type, 
	unsigned long &run_number, 
	bool *ddu_in_
)
{

	// set defaults, in case some params are not in the file
	endcap = "p";
	chamtype = "2.1";
	chamnum = "01";
	run_type = "11";
	run_number = 0;
	memset(ddu_in_, 0, sizeof (ddu_in_));


	char* step_path_ch = getenv("STEPHOME");
	if (step_path_ch == NULL) return -1;
	
	string last_used_file = string(step_path_ch) + "/data/xml/test_config/last_used.xml";

	// cannot figure out how to detect if the file does not exist - XercesDOMParser::parse just crashes with segmentation fault
	// so see if we can open the file
	fstream fs(last_used_file.c_str(), fstream::in);
	if (fs.fail())
	{
		// defaults are already set, just get out
		return -1;
	}
	fs.close();

	std::map <std::string, string*> map_last; // parameter map

	string run_number_str, ddu_in_str; // strings for non-string parameters

	// fill the map
	FILL_LAST_USED_MAP;

	cout << "Loading last used parameters from " << last_used_file << endl;;

	XMLPlatformUtils::Initialize();
	XercesDOMParser *parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(false); // this is default
	parser->setCreateEntityReferenceNodes(true);  // this is default
	parser->setIncludeIgnorableWhitespace (false);

	parser->parse(last_used_file.c_str());
	DOMDocument *doc = parser->getDocument();
	DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("STEP_last_used") );
	if( l->getLength() != 1 )
	{
		cout << "There is not exactly one STEP_last_used node in configuration" << endl;
		return 1;
	}
	DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("params") );
	if( itemList->getLength() == 0 )
	{
		cout << "There are no parameter sections" << endl;
		return 1;
	}

	DOMNode* info;

	for(unsigned int j=0; j<itemList->getLength(); j++)
	{
		info = itemList->item(j); // parameter section
		
		std::map<std::string, std::string> obj_info;
		DOMNodeList *children = info->getChildNodes();

		string nodename = string(XMLString::transcode(info->getNodeName()));
		cout <<  "node: " << trim(nodename) << endl;

		// param node found. Decode parameters
		for(unsigned int i=0; i<children->getLength(); i++)
		{
			std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
			trim(paramname);

			if ( children->item(i)->hasChildNodes() ) 
			{
				std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
				trim(param);

				cout <<  paramname << " = " << param << endl;

				*(map_last[paramname]) = param; // assign to structure item
			}
		}
	}

	// convert non-string params to required form
	run_number = atol(run_number_str.c_str());
	for (int k = 0; k < 16; k++) ddu_in_[k] = ddu_in_str[k] == '1';

	delete parser;
	return 0;
}


// macro to fill the parameter map
#define map_add(tnum, parname) map##tnum[#parname] = &(tcs->t##tnum.parname)

int emuSTEPConfigRead::read_test_config(char* xmlFile, test_config_struct * tcs) 
{
	char* step_path_ch = getenv("STEPHOME");
	if (step_path_ch == NULL){
	  step_path_ch = "/home/cscme42/STEP";
	}
	if(step_path_ch==NULL){return -1;}
	
	string step_config = string(step_path_ch) + "/data/xml/test_config/" + xmlFile;

	memset(tcs, 0, sizeof(test_config_struct));

	// maps of parameters for each test
	std::map <std::string, int*> map11, map12, map13, map14, map15, map16, map17, map17b, map18, map19, map21, map25, map30;
	std::map <std::string, int*>* cur_map; // currently used map

	// fill the maps (see macro above)
	map_add(11, events_total);

	map_add(12, events_per_strip);
	map_add(12, alct_test_pulse_amp_11);
	map_add(12, alct_test_pulse_amp_12);
	map_add(12, alct_test_pulse_amp_13);
	map_add(12, alct_test_pulse_amp_21);
	map_add(12, alct_test_pulse_amp_22);
	map_add(12, alct_test_pulse_amp_31);
	map_add(12, alct_test_pulse_amp_32);
	map_add(12, alct_test_pulse_amp_41);
	map_add(12, alct_test_pulse_amp_42);

	map_add(13, events_per_threshold);
	map_add(13, thresholds_per_tpamp);
	map_add(13, threshold_first);	 
	map_add(13, threshold_step);	 
	map_add(13, tpamps_per_run);	 
	map_add(13, tpamp_first);     
	map_add(13, tpamp_step);	     

	map_add(14, alct_test_pulse_amp);
	map_add(14, events_per_delay);
	map_add(14,	delays_per_run); 
	map_add(14,	delay_first);	 
	map_add(14,	delay_step);	 

	map_add(15, events_total);

	map_add(16, events_per_layer);
	map_add(16, alct_test_pulse_amp);

	map_add(17, dmb_test_pulse_amp);
	map_add(17, events_per_delay); 
	map_add(17, delays_per_strip); 
	map_add(17, delay_first);      
	map_add(17, delay_step);       
	map_add(17, strips_per_run);   
	map_add(17, strip_first);      
	map_add(17, strip_step);  

	map_add(17b, dmb_test_pulse_amp);
	map_add(17b, events_per_pulsedac); 
	map_add(17b, pulse_dac_settings); 
	map_add(17b, dac_first);      
	map_add(17b, dac_step);       
	map_add(17b, strips_per_run);   
	map_add(17b, strip_first);      
	map_add(17b, strip_step);      

	map_add(18, events_total);

	map_add(19, scale_turnoff);
	map_add(19, range_turnoff);
	map_add(19, events_per_thresh);
	map_add(19, threshs_per_tpamp);
	map_add(19, thresh_first);
	map_add(19, thresh_step);
	map_add(19, dmb_tpamps_per_strip);
	map_add(19, dmb_tpamp_first);
	map_add(19, dmb_tpamp_step);
	map_add(19, strips_per_run);
	map_add(19, strip_first);
	map_add(19, strip_step);

	map_add(21, dmb_test_pulse_amp);
	map_add(21, cfeb_threshold);    
	map_add(21, events_per_hstrip); 
	map_add(21, hstrips_per_run);   
	map_add(21, hstrip_first);      
	map_add(21, hstrip_step);       

	map_add(25, trig_settings);
	map_add(25, events_per_trig_set);

	map_add(30, events_per_delay);
	map_add(30, tmb_l1a_delays_per_run);
	map_add(30, tmb_l1a_delay_first);
	map_add(30, tmb_l1a_delay_step);

    // map of test parameter maps
	std::map <std::string, std::map <std::string, int*>* > test_map; 
	test_map["11"] = &map11;
	test_map["12"] = &map12;
	test_map["13"] = &map13;
	test_map["14"] = &map14;
	test_map["15"] = &map15;
	test_map["16"] = &map16;
	test_map["17"] = &map17;
	test_map["17b"] = &map17b;
	test_map["18"] = &map18;
	test_map["19"] = &map19;
	test_map["21"] = &map21;
	test_map["25"] = &map25;
	test_map["30"] = &map30;

	cout << "Loading test configuration from XML file: "  <<  step_config << endl;

	if (step_config == "") 
	{
		cout << "Invalid configuration file: " << step_config << endl;
		return 1;
	}

	XMLPlatformUtils::Initialize();
	XercesDOMParser *parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(false); // this is default
	parser->setCreateEntityReferenceNodes(true);  // this is default
	parser->setIncludeIgnorableWhitespace (false);

	parser->parse(step_config.c_str());
	DOMDocument *doc = parser->getDocument();
	DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("STEP_tests") );
	if( l->getLength() != 1 )
	{
		cout << "There is not exactly one STEP_tests node in configuration" << endl;
		return 1;
	}
	DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("test_config") );
	if( itemList->getLength() == 0 )
	{
		cout << "There are no test configuration sections" << endl;
		return 1;
	}

	DOMNode* info;

	for(unsigned int j=0; j<itemList->getLength(); j++)
	{
		info = itemList->item(j); // test config section
		
		std::map<std::string, std::string> obj_info;
		DOMNodeList *children = info->getChildNodes();

		string nodename = string(XMLString::transcode(info->getNodeName()));
		cout <<  "node: " << trim(nodename) << endl;

		// test_config node found. Decode parameters
		for(unsigned int i=0; i<children->getLength(); i++)
		{
			std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
			trim(paramname);

			if ( children->item(i)->hasChildNodes() ) 
			{
				std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
				trim(param);

				cout <<  paramname << " = " << param << endl;
				if (paramname.compare("test") == 0) // test number
					cur_map = test_map[param]; // select map of that test
				else // one of the parameters
					*((*cur_map)[paramname]) = atol(param.c_str()); // assign to structure item
				
			}
		}


	}

	delete parser;
	return 0;
}

// removes leading and trailing spaces from a string
string& emuSTEPConfigRead::trim(string &str)
{
	string whitespaces (" \t\f\v\n\r");
	size_t found;
  
	// trailing
	found = str.find_last_not_of(whitespaces);
	if (found != string::npos)
		str.erase(found+1);
	else
		str.clear();            // str is all whitespace

	// leading
	found = str.find_last_of(whitespaces);
	if (found != string::npos)
		str.erase(0, found+1);

	return str;

}

