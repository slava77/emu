/*****************************************************************************\
* $Id: Parser.cc,v 1.2 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/Parser.h"

emu::fed::Parser::Parser(xercesc::DOMNode *pNode):
pAttributes_(pNode->getAttributes())
{
	// That's all, folks!
}
