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

#define MAX_DDU 36

namespace emu
{
namespace dqm
{
namespace utils
{

std::string now(time_t tstamp, const char* format="%Y-%m-%d %H:%M:%S %Z");
std::string now();
std::string getDateTime(time_t tstamp = 0);
int getNumStrips(std::string cscID);
int getNumCFEBs(std::string cscID);
int getNumWireGroups(std::string cscID);
int getNumAFEBs(std::string cscID);
int getAFEBPos(int ilayer, int iwire);
std::vector< std::pair<int,int> > getHVSegmentsMap(std::string cscID);
int getHVSegmentNumber(std::string cscID, uint32_t iseg);
bool isME11(std::string cscID);
bool isME42(std::string cscID);
bool isValidDDUmapping(int dduID, int crate, int slot); /// Check that chamber (crate ID, slot) belongs to this DDU ID
std::string getCSCTypeName(int id);
std::string getCSCTypeLabel(int endcap, int station, int ring );
std::string getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition );
std::map<std::string, int> getCSCTypeToBinMap();
std::string genCSCTitle(std::string tag);
    
    
}
}
}


#endif
