/*****************************************************************************\
* $Id: FakeVMEModule.cc,v 1.1 2009/11/23 07:24:31 paste Exp $
\*****************************************************************************/
#include "emu/fed/FakeVMEModule.h"

#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

#include "emu/fed/VMELock.h"

emu::fed::FakeVMEModule::FakeVMEModule(const unsigned int &mySlot):
VMEModule(mySlot),
slot_(mySlot)
{
}



uint16_t emu::fed::FakeVMEModule::readVME(const uint32_t &Address, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	srand((unsigned) time(NULL));
	return rand();
}



void emu::fed::FakeVMEModule::writeVME(const uint32_t &Address, const uint16_t &data, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	return;
}
