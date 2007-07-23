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
#include "DDU.h"
#include "Crate.h"

XERCES_CPP_NAMESPACE_USE


class DDUParser: public EmuParser
{

public:
	DDUParser(){}
	explicit DDUParser(DOMNode * pNode, int crate=0);
		
	/// the last one parsed
	DDU * ddu() const {return ddu_;}

private:
	DDU * ddu_;//last one parsed
};

#endif






