#ifndef EmuParser_h
#define EmuParser_h

#include <xercesc/dom/DOM.hpp>
#include <string>

XERCES_CPP_NAMESPACE_USE
using namespace std;

class EmuParser 
{
public:
  EmuParser() {}
  virtual ~EmuParser() {}

  void parseNode(DOMNode * pNode);
  void fillInt(string item, int & target);
  void fillFloat(string item, float & target);
  void fillString(string item, string & target);
     
  //void fillHex(string item, int & target);


protected:
  DOMNamedNodeMap * pAttributes_;
  int size_;

};

#endif

