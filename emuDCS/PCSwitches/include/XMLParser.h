#ifndef XMLParser_h
#define XMLParser_h

#include <xercesc/dom/DOM.hpp>
#include <string>

namespace emu {
  namespace pcsw {

class Switch;

class XMLParser 
{
public:
  XMLParser();
  virtual ~XMLParser() {}

  void parseNode(xercesc::DOMNode * pNode);
  bool fillInt(std::string item, int & target);
  bool fillIntX(std::string item, int & target);
  bool fillLongIntX(std::string item, long int & target);
  bool fillLongLongIntX(std::string item, long long int & target);
  bool fillString(std::string item, std::string & target);
  bool fillFloat(std::string item, float & target);

  void ConnectionParser(xercesc::DOMNode *pNode, Switch * S, int Counter);
  void FillSwitchParser(xercesc::DOMNode *pNode, Switch * S, int Counter);
  void PCParser(xercesc::DOMNode *pNode, Switch * S, int Counter);
  void EndcapSideParser(xercesc::DOMNode *pNode,Switch * S,xercesc::DOMNode *pNodeGlobal);
  Switch * SwitchParser(xercesc::DOMNode *pNode);
  void parseFile(const std::string name);

  inline Switch * GetSwitch() { return S_; }

protected:
  xercesc::DOMNamedNodeMap * pAttributes_;
  Switch * S_;
};

  } // namespace emu::pcsw
  } // namespace emu

#endif
