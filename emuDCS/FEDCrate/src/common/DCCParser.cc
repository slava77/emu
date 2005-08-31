#include <vector>
#include "DCCParser.h"
#include "DCC.h"
#include <xercesc/dom/DOM.hpp>

DCCParser::DCCParser(DOMNode * pNode, int crateNumber)
{

  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    cerr << "No slot specified for DCC! " << endl;
  } else { 
   dcc_ = new DCC(crateNumber,slot);  
  }
}



























