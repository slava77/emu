#ifndef DDUParser_h
#define DDUParser_h
/*
 *  class DDUParser
 *  author Stan Durkin 1/25/05
 *     
 */
class DDU;
#include <vector>
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE


class DDUParser
{

public:
  DDUParser(){}
  explicit DDUParser(DOMNode * pNode, int crate);
    
  /// the last one parsed
  DDU * ddu() const {return ddu_;}

private:
  DDU * ddu_;//last one parsed
  EmuParser parser_;
};

#endif






