#ifndef _EmuDQM_Utils_h_
#define _EmuDQM_Utils_h_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <sstream>

#include <TString.h>

namespace emu
{
namespace dqm
{
namespace utils
{

std::string now(time_t tstamp);
std::string now();
int getNumStrips(std::string cscID);
int getNumCFEBs(std::string cscID);
int getNumWireGroups(std::string cscID);
std::vector< std::pair<int,int> > getHVSegmentsMap(std::string cscID);
bool isME11(std::string cscID);
std::string getCSCTypeLabel(int endcap, int station, int ring );
std::string getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition );
std::map<std::string, int> getCSCTypeToBinMap();
std::string genCSCTitle(std::string tag);
    
    
}
}
}


#endif
