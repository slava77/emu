#ifndef BoardsDB_h
#define BoardsDB_h

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace emu {
  namespace db {

class BoardsDB
{
public:
  BoardsDB();
  ~BoardsDB();

  // General Parser Routines
  void parseNode(xercesc::DOMNode * pNode); 
  bool fillInt(std::string item, int & target);
  bool fillIntX(std::string item, int & target);
  bool fillLongLongIntX(std::string item, long long int & target);
  bool fillString(std::string item, std::string & target);
  bool fillFloat(std::string item, float & target);

  // Parser Routines for RUI-to-chamber_mapping.xml
  void parseFile(int type,const std::string name);
  void MainParser(int type,xercesc::DOMNode *pNode);
  void FillParser(int type,xercesc::DOMNode *pNode);
  void FillCFEBDB();
  int CrateToDMBID(std::string crate,int slot);
  int ChamberToCFEBID(std::string chamber,int cfeb);

  std::string PCBoardsDBXMLFile_;
  std::string CFEBBoardsDBXMLFile_;
  std::string CSC2ChamberDBXMLFile_;
  std::string HOMEDIR_;



protected:

  int iPCdb;
  typedef struct pcdb{
    int slot;
    std::string crate;
    std::string type;
    std::string board;
  }PCDB;
  PCDB pcDB[1492];

  int iCFEBdb;
  typedef struct tcfebdb{
    int cfebID;
    std::string csctype;
    int cscid;
    int cscnumber;
  }T_CFEBDB;
  T_CFEBDB t_cfebdb[2368];

  int iCSCIDdb;
  typedef struct tcsciddb{
    std::string csctype;
    int cscid;
    std::string chamber;
  }T_CSCIDDB;
  T_CSCIDDB t_csciddb[468];

  typedef struct cfebdb{
    std::string chamber;
    int cfeb;
    int cfebID;
  }CFEBDB;
  CFEBDB cfebDB[2368];


  xercesc::DOMNamedNodeMap * pAttributes_;
};

  }
}

#endif
