//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 3.2 2008/04/22 09:31:11 geurts Exp $
// $Log: VMEParser.cc,v $
// Revision 3.2  2008/04/22 09:31:11  geurts
// New FEDCrate Control software by Jason and Phillip.
//
// Revision 3.1  2007/07/23 05:03:31  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.3  2006/01/27 16:04:50  gilmore
// *** empty log message ***
//
// Revision 1.8  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
//
//-----------------------------------------------------------------------
#include "VMEParser.h"
#include "VMEController.h"
#include "Crate.h"

VMEParser::VMEParser(DOMNode * pNode, int crate)
{
	int Link;
	int Device;
	parseNode(pNode);
	fillInt("Device",Device); 
	fillInt("Link",Link);
	controller_ = new VMEController(Device, Link);
	//fillInt("vmeirq_start", controller_->start_thread_on_init); 
}


























