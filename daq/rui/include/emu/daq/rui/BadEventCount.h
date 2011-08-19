#ifndef __emu_daq_rui_BadEventCount_h__
#define __emu_daq_rui_BadEventCount_h__

#include "xdata/UnsignedLong.h"

namespace emu { namespace daq { namespace rui {

      class BadEventCount : public xdata::UnsignedLong{

      public:
	/// 
	/// Constructor.
	/// @param i 
	///
	/// @return 
	///
	BadEventCount( const unsigned long i = 0 ) : xdata::UnsignedLong( i ){}
	/// 
	/// Progressive prescaler used to prevent writing too many bad events to file and filling up the disk.
	///
	/// @return TRUE if this bad event is to be written to file.
	///
	bool passesPrescaling() const;
  };

}}} // namespace emu::daq::rui

#endif
