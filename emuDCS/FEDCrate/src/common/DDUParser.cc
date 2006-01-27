#include <vector>
#include "DDUParser.h"
#include "DDU.h"
#include <xercesc/dom/DOM.hpp>

DDUParser::DDUParser(DOMNode * pNode, int crateNumber)
{

  parser_.parseNode(pNode);

  int slot = 0;
  parser_.fillInt("slot", slot);
  
  if(slot == 0) {
    cerr << "No slot specified for DDU! " << endl;
  } else { 
   ddu_ = new DDU(crateNumber,slot);
   parser_.fillInt("gbe_prescale", ddu_->gbe_prescale_); 

  }
}



























