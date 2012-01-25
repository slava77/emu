#ifndef __emu_daq_rui_BadEventCount_h__
#define __emu_daq_rui_BadEventCount_h__

#include <stdint.h>

#include "xdata/UnsignedInteger64.h"

namespace emu { namespace daq { namespace rui {

      class BadEventCount : public xdata::UnsignedInteger64{

      public:
	/// 
	/// Constructor.
	/// @param i 
	///
	/// @return 
	///
	BadEventCount( const uint64_t i = 0 ) : xdata::UnsignedInteger64( i ){}
	/// 
	/// Progressive prescaler used to prevent writing too many bad events to file and filling up the disk.
	///
	/// @return TRUE if this bad event is to be written to file.
	///
	bool passesPrescaling() const;
  };

}}} // namespace emu::daq::rui

#endif
