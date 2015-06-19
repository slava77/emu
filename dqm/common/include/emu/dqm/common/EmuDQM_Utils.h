#ifndef _EmuDQM_Utils_h_
#define _EmuDQM_Utils_h_

#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <iomanip>

#include <TString.h>
#include <TPRegexp.h>

// Function shortcuts
#define REMATCH(pat, str)         (TPRegexp(pat).MatchB(str))
#define REREPLACE(pat, str, rep)  { TString s(str); TPRegexp(pat).Substitute(s, rep); str = s; }

#define MAX_DDU 36
#define POST_LS1_ME11A_STRIP_START_INDEX 252289

namespace emu
{
namespace dqm
{
namespace utils
{

std::string now(time_t tstamp, const char* format="%Y-%m-%d %H:%M:%S %Z");
std::string now();
std::string getDateTime(time_t tstamp = 0);
int getNumStrips(std::string cscID, uint16_t fFormatVersion = 2005);
int getNumCFEBs(std::string cscID, uint16_t fFormatVersion = 2005);
int getNumWireGroups(std::string cscID);
int getNumAFEBs(std::string cscID);
int getAFEBPos(int ilayer, int iwire);
std::vector< std::pair<int,int> > getHVSegmentsMap(std::string cscID);
int getHVSegmentNumber(std::string cscID, uint32_t iseg);
bool isME11(std::string cscID);
bool isME42(std::string cscID);
int getME11a_first_strip_index(std::string cscID, uint16_t fFormatVersion = 2013);
bool isValidDDUmapping(int dduID, int crate, int slot); /// Check that chamber (crate ID, slot) belongs to this DDU ID
std::string getCSCTypeName(int id);
std::string getCSCTypeLabel(int endcap, int station, int ring );
std::string getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition );
std::map<std::string, int> getCSCTypeToBinMap();
std::string genCSCTitle(std::string tag);
int getRUIfromDDUId(unsigned ddu_id);
int getDDUfromRUIId(unsigned ddu_id);
std::string replaceRUIwithDDUId(std::string rui);
uint32_t getRunNumberFromFilename(std::string datafile, std::string prefix = "csc_"); 
    
    
}
}
}


#endif
