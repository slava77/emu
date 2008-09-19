/*****************************************************************************\
* $Id: VMEControllerParser.cc,v 3.4 2008/09/19 16:53:52 paste Exp $
*
* $Log: VMEControllerParser.cc,v $
* Revision 3.4  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.3  2008/08/19 14:51:03  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEControllerParser.h"

#include "VMEController.h"

emu::fed::VMEControllerParser::VMEControllerParser(xercesc::DOMElement* pNode)
{
	int Link;
	int Device;
	parseNode(pNode);
	fillInt("Device",Device);
	fillInt("Link",Link);
	vmeController_ = new VMEController(Device, Link);
	
}
