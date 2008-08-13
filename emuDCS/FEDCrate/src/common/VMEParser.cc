//-----------------------------------------------------------------------
// $Id: VMEParser.cc,v 3.4 2008/08/13 14:20:42 paste Exp $
// $Log: VMEParser.cc,v $
// Revision 3.4  2008/08/13 14:20:42  paste
// Massive update removing "using namespace" code and cleaning out stale header files as preparation for RPMs.
//
// Revision 3.3  2008/06/10 13:52:12  gilmore
// improved FED Crate HyperDAQ operability
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
//#include "Crate.h"

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


























