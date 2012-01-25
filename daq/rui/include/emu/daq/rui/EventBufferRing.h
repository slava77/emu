#ifndef __emu_daq_rui_EventBufferRing_h__
#define __emu_daq_rui_EventBufferRing_h__

#include "emu/daq/rui/Ring.h"
#include "emu/daq/rui/EventBuffer.h"

using namespace std;

namespace emu { namespace daq { namespace rui {

      class EventBufferRing{
      public:

	friend ostream& operator<<( ostream& os, emu::daq::rui::EventBufferRing& ebr );

	EventBufferRing();
	EventBufferRing( const size_t size );
	~EventBufferRing();
	size_t setSize( size_t size );
	bool addData( const size_t dataSize, const char* data, const bool startsEvent );
	void emptyEventBuffers();
	list<const emu::daq::rui::EventBuffer*> getEventBuffers() const;

      private:
	Ring<emu::daq::rui::EventBuffer*> ring_; ///< Circular list of event buffers. 
	Ring<emu::daq::rui::EventBuffer*>::iterator current_; ///< Points to the event buffer that's currently being filled
      };

      ostream& operator<<( ostream& os, emu::daq::rui::EventBufferRing& ebr );

}}} // namespace emu::daq::rui

#endif
