#ifndef ChamberParser_h
#define ChamberParser_h
/*
 *  class ChamberParser
 *  author Phillip Killewald 1/31/08
 *     
 */
class Chamber;
#include <vector>
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>
#include "Chamber.h"
#include <stdlib.h> // getenv
#include <sstream>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

XERCES_CPP_NAMESPACE_USE

using namespace std;

class ChamberParser: public EmuParser
{

public:
	ChamberParser(){}
	explicit ChamberParser(char *fileName, int crate, int slot);

	inline vector<Chamber *> chamberVector() const { return chamberVector_; }

private:
	// This parsing is annoying.  Use Stan's idea of parsing every step with a different method.
	vector<DOMNode *> parseMaps(DOMNode *pDoc);
	vector<DOMNode *> parseRUIs(DOMNode *pMap);
	vector<DOMNode *> parseDDUs(DOMNode *pRUI, int crate, int slot);
	void parseInput(DOMNode *pDDU);

	vector<Chamber *> chamberVector_;
};

#endif
