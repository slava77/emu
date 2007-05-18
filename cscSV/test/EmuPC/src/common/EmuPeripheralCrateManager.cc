// EmuPeripheralCrateManager.cc

#include "EmuPeripheralCrateManager.h"

XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateManager);

EmuPeripheralCrateManager::EmuPeripheralCrateManager(xdaq::ApplicationStub *stub)
                throw (xdaq::exception::Exception) :
				EmuPeripheralCrate(stub)
{
}

// End of file
// vim: set ai sw=4 ts=4:
