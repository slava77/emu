#ifndef __EMU_FCRATE_MANAGER_H__
#define __EMU_FCRATE_MANAGER_H__

#include "EmuFCrate.h"

class EmuFCrateManager : public EmuFCrate
{
public:
	XDAQ_INSTANTIATOR();

	EmuFCrateManager(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);
};

#endif  // ifndef __EMU_FCRATE_MANAGER_H__
// vim: set ai sw=4 ts=4:
