#ifndef XMLParser_h
#define XMLParser_h

#include <xercesc/dom/DOM.hpp>
#include <string>

class Crate;
class Chamber;
class EmuEndcap;

class XMLParser 
{
public:
  XMLParser();
  virtual ~XMLParser() {}

  EmuEndcap * GetEmuEndcap() { return emuEndcap_; }
  void parseNode(xercesc::DOMNode * pNode);
  bool fillInt(std::string item, int & target);
  bool fillIntX(std::string item, int & target);
  bool fillLongLongIntX(std::string item, long long int & target);
  bool fillFloat(std::string item, float & target);
  bool fillString(std::string item, std::string & target);
  void CCBParser(xercesc::DOMNode * pNode, Crate * crate);
  void MPCParser(xercesc::DOMNode * pNode, Crate * crate);
  void TMBParser(xercesc::DOMNode * pNode, Crate * crate, Chamber * chamber, xercesc::DOMNode * pNodeGlobal);
  void DAQMBParser(xercesc::DOMNode * pNode, Crate * crate, Chamber * chamber);
  void CSCParser(xercesc::DOMNode * pNode, Crate * crate, xercesc::DOMNode * pNodeGlobal);
  Crate * VMEParser(xercesc::DOMNode * pNode);
  void PeripheralCrateParser(xercesc::DOMNode *pNode,EmuEndcap * endcap,xercesc::DOMNode *pNodeGlobal);
  EmuEndcap * EmuEndcapParser(xercesc::DOMNode *pNode);
  void parseFile(const std::string name);

protected:
  xercesc::DOMNamedNodeMap * pAttributes_;
  EmuEndcap * emuEndcap_;
};

#endif
