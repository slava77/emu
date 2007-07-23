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
#include "DCC.h"
#include "Crate.h"

XERCES_CPP_NAMESPACE_USE


class DCCParser: public EmuParser
{

public:
	DCCParser(){}
	explicit DCCParser(DOMNode * pNode, int crate = 0);
		
	/// the last one parsed
	DCC * dcc() const {return dcc_;}

private:
	DCC * dcc_;//last one parsed
};

#endif






