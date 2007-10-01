// EmuFCrateManager.cc

#include "EmuFCrateManager.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrateManager);

EmuFCrateManager::EmuFCrateManager(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuFCrate(stub)
{
}

// End of file
// vim: set ai sw=4 ts=4:
