#ifndef __EMU_PERIPHERAL_CRATE_MANAGER_H__
#define __EMU_PERIPHERAL_CRATE_MANAGER_H__

#include "EmuPeripheralCrate.h"

class EmuPeripheralCrateManager : public EmuPeripheralCrate
{
public:
		XDAQ_INSTANTIATOR();

		EmuPeripheralCrateManager(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);
};

#endif  // ifndef __EMU_PERIPHERAL_CRATE_MANAGER_H__
// vim: set ai sw=4 ts=4:
