#ifndef __DDUPARSER_H__
#define __DDUPARSER_H__
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
//#include "Crate.h"
#include "ChamberParser.h"

XERCES_CPP_NAMESPACE_USE


class DDUParser: public EmuParser
{

public:
	DDUParser(){}
	explicit DDUParser(DOMNode * pNode, int crate=0, char *fileName=0);

	/// the last one parsed
	DDU * ddu() const { return ddu_; }

private:
	DDU * ddu_;//last one parsed
	int slot_;
};

#endif






