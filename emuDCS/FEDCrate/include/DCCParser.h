#ifndef DCCParser_h
#define DCCParser_h
/*
 *  class DCCParser
 *  author Stan Durkin 1/25/05
 *     
 */
class DCC;
#include <vector>
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE


class DCCParser
{

public:
  DCCParser(){}
  explicit DCCParser(DOMNode * pNode, int crate);
    
  /// the last one parsed
  DCC * dcc() const {return dcc_;}

private:
  DCC * dcc_;//last one parsed
  EmuParser parser_;
};

#endif






