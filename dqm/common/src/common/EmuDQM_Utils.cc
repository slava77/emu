#include "emu/dqm/common/EmuDQM_Utils.h"

namespace emu {
namespace dqm {
namespace utils {

std::string now(time_t tstamp, const char* format)
{
  char buf[255];
  time_t now = tstamp;
  if (tstamp == 0) now=time(NULL);
  const struct tm * timeptr = localtime(&now);
  strftime(buf, sizeof(buf),format, timeptr);
  // strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", timeptr);
  std::string time = std::string(buf);
  if (time.find("\n",0) != std::string::npos)
    time = time.substr(0,time.find("\n",0));
  else {
    if (time.length() == 0)
      time = "---";
  }
  return time;

};


std::string now()
{
  return now(0);
}


std::string getDateTime(time_t tstamp)
{
  time_t t = tstamp;
  struct tm *tm;

  if (t == 0) time ( &t );
  tm = gmtime ( &t ); // Unversal Coordinated Time

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << tm->tm_year%100
     << std::setfill('0') << std::setw(2) << tm->tm_mon+1
     << std::setfill('0') << std::setw(2) << tm->tm_mday      << "_"
     << std::setfill('0') << std::setw(2) << tm->tm_hour
     << std::setfill('0') << std::setw(2) << tm->tm_min
     << std::setfill('0') << std::setw(2) << 0       << "_UTC";

  return ss.str();
}

int getNumStrips(std::string cscID)
{
  if ((cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0 )
      || (cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") == 0 )) return 64;
  else if ((cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0 )
      || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") == 0 )) return 112;
  else return 80;
}

int getNumCFEBs(std::string cscID)
{
  if ((cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0 )
      || (cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") ==0 )) return 4;
  else if ((cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0 )
      || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0 )) return 7;
  else return 5;
}

int getNumWireGroups(std::string cscID)
{
  if ( (cscID.find("ME+4/1") == 0) || (cscID.find("ME-4/1") ==0)
       || (cscID.find("ME+3/1") == 0) || (cscID.find("ME-3/1") == 0)
       || (cscID.find("ME+4.1") == 0) || (cscID.find("ME-4.1") ==0)
       || (cscID.find("ME+3.1") == 0) || (cscID.find("ME-3.1") == 0)) return 96;
  else if ( (cscID.find("ME+2/1") == 0) || (cscID.find("ME-2/1") ==0)
            || (cscID.find("ME+2.1") == 0) || (cscID.find("ME-2.1") ==0)) return 112;
  else if ( (cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0)
            || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0)) return 48;
  else if ( (cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0)
            || (cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") ==0)) return 32;
  else return 64;
}

int getNumAFEBs(std::string cscID)
{
  return 3*(getNumWireGroups(cscID)/8);
}

int getAFEBPos(int ilayer, int iwire)
{
  int icolumn;
  icolumn = iwire/8;
  return (ilayer/2 + 3 * icolumn);
}

std::vector< std::pair<int,int> > getHVSegmentsMap(std::string cscID)
{
  std::vector< std::pair<int,int> > hvSegMap;
  hvSegMap.clear();
  if ((cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0 )) {
    hvSegMap.push_back(std::make_pair(1,48));
  } else if ( (cscID.find("ME+1/2") == 0) || (cscID.find("ME-1/2") ==0)) {
    hvSegMap.push_back(std::make_pair(1,24));
    hvSegMap.push_back(std::make_pair(25,48));
    hvSegMap.push_back(std::make_pair(49,64));
  } else if ( (cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0)) {
    hvSegMap.push_back(std::make_pair(1,12));
    hvSegMap.push_back(std::make_pair(13,22));
    hvSegMap.push_back(std::make_pair(23,32));
  } else if ( (cscID.find("ME+2/1") == 0) || (cscID.find("ME-2/1") ==0)) {
    hvSegMap.push_back(std::make_pair(1,44));
    hvSegMap.push_back(std::make_pair(45,80));
    hvSegMap.push_back(std::make_pair(81,112));
  } else if ( (cscID.find("ME+3/1") == 0) || (cscID.find("ME-3/1") ==0)
              ||  (cscID.find("ME+4/1") == 0) || (cscID.find("ME-4/1") ==0 ) ) {
    hvSegMap.push_back(std::make_pair(1,32));
    hvSegMap.push_back(std::make_pair(33,64));
    hvSegMap.push_back(std::make_pair(65,96));
  } else if ( (cscID.find("ME+2/2") == 0) || (cscID.find("ME-2/2") ==0)
              ||  (cscID.find("ME+3/2") == 0) || (cscID.find("ME-3/2") ==0 ) ) {
    hvSegMap.push_back(std::make_pair(1,16));
    hvSegMap.push_back(std::make_pair(17,28));
    hvSegMap.push_back(std::make_pair(29,40));
    hvSegMap.push_back(std::make_pair(41,52));
    hvSegMap.push_back(std::make_pair(53,64));
  }

  return hvSegMap;

}

int getHVSegmentNumber(std::string cscID, uint32_t aseg)
{
  std::vector< std::pair<int,int> > hvSegMap = getHVSegmentsMap(cscID);
  int first_wire = aseg*8+1;
  for (uint32_t i=0; i<hvSegMap.size(); i++) {
    if ( (first_wire >= hvSegMap[i].first)
         && (first_wire < hvSegMap[i].second) )
      return i;
  }
  return -1;

}

bool isME11(std::string cscID)
{
  if ((cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0 ) 
   || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0 )) {
    return true;
  } else return false;
}

bool isME42(std::string cscID)
{
  if ((cscID.find("ME+4/2") == 0) || (cscID.find("ME-4/2") ==0 )) {
    return true;
  } else return false;
}

std::map<std::string, int> getCSCTypeToBinMap()
{
  std::map<std::string, int> tmap;
  tmap["ME-4/2"] = 0;
  tmap["ME-4/1"] = 1;
  tmap["ME-3/2"] = 2;
  tmap["ME-3/1"] = 3;
  tmap["ME-2/2"] = 4;
  tmap["ME-2/1"] = 5;
  tmap["ME-1/3"] = 6;
  tmap["ME-1/2"] = 7;
  tmap["ME-1/1"] = 8;
  tmap["ME+1/1"] = 9;
  tmap["ME+1/2"] = 10;
  tmap["ME+1/3"] = 11;
  tmap["ME+2/1"] = 12;
  tmap["ME+2/2"] = 13;
  tmap["ME+3/1"] = 14;
  tmap["ME+3/2"] = 15;
  tmap["ME+4/1"] = 16;
  tmap["ME+4/2"] = 17;
  return tmap;

}

std::string getCSCTypeName(int id)
{

  std::map<std::string, int> tmap = getCSCTypeToBinMap();
  for (std::map<std::string,int>::iterator it= tmap.begin(); it != tmap.end(); ++it) {
    if (it->second == id) return it->first;
  }
  return "";
}


std::string getCSCTypeLabel(int endcap, int station, int ring )
{
  std::string label = "Unknown";
  std::ostringstream st;
  if ((endcap > 0) && (station>0) && (ring>0)) {
    if (endcap==1) {
      st << "ME+" << station << "/" << ring;
      label = st.str();
    } else if (endcap==2) {
      st << "ME-" << station << "/" << ring;
      label = st.str();
    } else {
      label = "Unknown";
    }
  }
  return label;
}

std::string genCSCTitle(std::string tag)
{
  std::string title = " " + tag;
  if (tag.find("DDU_") != std::string::npos ) {
    int ddu=0;
    int n = sscanf(tag.c_str(), "DDU_%02d", &ddu);
    if (n==1) {
      title = Form(" DDU = %02d", ddu);
    }

  } else if (tag.find("CSC_") != std::string::npos ) {
    int crate=0, slot=0;
    int n = sscanf(tag.c_str(), "CSC_%03d_%02d", &crate, &slot );
    if (n==2) {
      title = Form(" Crate ID = %02d. DMB ID = %02d", crate, slot);
    }

  } else if (tag.find("EMU") != std::string::npos ) {
    title = "";
  }

  return title;
}

// TODO: !!! Should put proper mappings
std::string getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition )
{
  return cscID;
  //  int crate=0, slot=0;
  std::string cscName="";
  if (sscanf(cscID.c_str(), "CSC_%03d_%02d", &crate , &slot) == 2) {
    // cscName=getCSCFromMap(crate,slot, CSCtype, CSCposition );
  }
  return cscName;
}

bool isValidDDUmapping(int dduID, int crate, int slot)
{
  std::map<std::pair<int, int>, int> CSCtoDDUmap;
  return true;
}

}
}
}

