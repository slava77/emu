/*****************************************************************************\
* $Id: Parser.cc,v 3.1 2009/01/29 15:31:24 paste Exp $
*
* $Log: Parser.cc,v $
* Revision 3.1  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "Parser.h"

emu::fed::Parser::Parser(xercesc::DOMNode *pNode):
pAttributes_(pNode->getAttributes())
{
	// That's all, folks!
}
