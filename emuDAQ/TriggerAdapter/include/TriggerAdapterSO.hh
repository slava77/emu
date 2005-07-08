//-----------------------------------------------------------------------
// $Id: TriggerAdapterSO.hh,v 2.0 2005/07/08 12:15:40 geurts Exp $
// $Log: TriggerAdapterSO.hh,v $
// Revision 2.0  2005/07/08 12:15:40  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
// Description: Library responsible for instantiating application objects.
#ifndef __TriggerAdapterSO_h__
#define __TriggerAdapterSO_h__
 
#include "xdaq.h"
class TriggerAdapterSO : public xdaqSO
{
public:
    void init();
    void shutdown();
 
private:
    xdaqPluggable* createXdaqApp(const char *classname);
};
 
#endif
 
