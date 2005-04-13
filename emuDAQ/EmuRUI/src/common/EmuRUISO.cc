//-----------------------------------------------------------------------
// $Id: EmuRUISO.cc,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUISO.cc,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#include "EmuRUISO.h"
#include "EmuRUI.h"

void EmuRUISO::init()
{
	instance_ = new EmuRUI(); 
	xdaq::load(instance_);
}

void EmuRUISO::shutdown()
{
	delete instance_;
}

extern "C" void *init_EmuRUI()
{
	return ((void *)new EmuRUISO());
}
