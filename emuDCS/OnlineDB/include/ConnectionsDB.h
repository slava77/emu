#ifndef ConnectionsDB_h
#define ConnectionsDB_h

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace emu{
  namespace db{

class ConnectionsDB
{
public:
  ConnectionsDB();
  ~ConnectionsDB();

  // General Parser Routines
  void parseNode(xercesc::DOMNode * pNode); 
  bool fillInt(std::string item, int & target);
  bool fillIntX(std::string item, int & target);
  bool fillLongLongIntX(std::string item, long long int & target);
  bool fillString(std::string item, std::string & target);
  bool fillFloat(std::string item, float & target);

  // Parser Routines for RUI-to-chamber_mapping.xml
  void parseFile(const std::string name);
  void MapParser(xercesc::DOMNode *pNode);
  void RUIParser(xercesc::DOMNode *pNode);
  void DDUParser(xercesc::DOMNode *pNode);
  void inputParser(xercesc::DOMNode *pNode);
  void PeripheralCrateParser(xercesc::DOMNode *pnode);
  void ChamberParser(xercesc::DOMNode *pnode);
  void FiberCassetteParser(xercesc::DOMNode *pnode); 

  // Parser Routines for SwitchConnections.xml
  void sparseFile(const std::string name);
  void sDataParser(xercesc::DOMNode *pNode);
  void sIDParser(xercesc::DOMNode *pNode);
  void sVMEParser(xercesc::DOMNode *pNode);
  void sFCTCParser(xercesc::DOMNode *pNode);
  void sMACParser(xercesc::DOMNode *pNode);
  void sSwitchParser(xercesc::DOMNode *pNode);

  // Translation Routines for RUI-to-chamber_mapping.xml
  std::string DDUtoChamber(int crate,int slot,int input);
  std::string PCratetoChamber(std::string endcap,int station,int crate,int slot);
  std::string chamber_str(int i);
  std::string ddu_str(int i);
  std::string cassette_str(int i);
  std::string pcrate_str(int i);
  std::string pcrate_name_str(int i);
  std::string DDUtoDump(std::string ddu);
  std::string Pcrate_nametoDump(std::string vme);
  std::string CassettetoDump(std::string cassette);
  std::string ChambertoDump(std::string chamber);
  std::string NametoDump(std::string name);

  // Translation Routines for SwitchConnections.xml
  std::string sVMEtoDump(std::string svme);
  std::string sFCTCtoDump(std::string sfctc);
  std::string sMACtoDump(std::string smac);
  std::string sSwitchtoDump(std::string sswitch);
  std::string sNametoDump(std::string name);

  std::string ConnectionsDBXMLFile_;  // file:RUI-to-chamber_mapping.xml 
  std::string sConnectionsDBXMLFile_; // file:SwitchConnections.xml 
  std::string HOMEDIR_;

  // variables/structures for RUI-to-chamber_mapping.xml
  int RUI_instance;
  std::string chamber_endcap;
  int chamber_station;
  int chamber_type;
  int chamber_number;
  int vme_id;
  int vme_crate;
  int vme_slot;
  int ddu_crate;
  int ddu_slot;
  int ddu_input;
  int cassette_crate;
  int cassette_pos;
  int cassette_socket;
  typedef struct db{
    int RUI_instance;
    std::string chamber_endcap;
    int chamber_station;
    int chamber_type;
    int chamber_number;
    int vme_id;
    int vme_crate;
    int vme_slot;
    int ddu_crate;
    int ddu_slot;
    int ddu_input;
    int cassette_crate;
    int cassette_pos;
    int cassette_socket;
  }DB;
  DB db[540];
  int idb;

 // variables/structures for SwitchConnections.xml 
  std::string svme;
  std::string sfctc;
  std::string smac;
  std::string sswitch;
  typedef struct sdb{
    std::string svme;
    std::string sfctc;
    std::string smac;
    std::string sswitch;
  }SDB;
  SDB sdb[60];
  int isdb;

protected:

  xercesc::DOMNamedNodeMap * pAttributes_;
};

  }
}

#endif
