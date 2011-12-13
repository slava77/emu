#ifndef __emu_daq_rui_EventBuffer_h__
#define __emu_daq_rui_EventBuffer_h__

#include <iostream> // for size_t and setw,too

using namespace std;

namespace emu { namespace daq { namespace rui {

      class EventBuffer{
      public:

	friend ostream& operator<<( ostream& os, const emu::daq::rui::EventBuffer& eb );

	EventBuffer();
	~EventBuffer();
	bool appendData( const size_t dataSize, const char* data );
	const char* getEvent() const;
	size_t getEventSize() const;
	void empty();

      private:
	void grow( const size_t newBufferSize );

	char* buffer_;
	size_t bufferSize_;
	size_t eventSize_;
	static const size_t initialBufferSize_ = 0xd0000; ///< Initial size of the buffer. (128 kB)
	static const size_t maxBufferSize_ = 0x100000; ///< Maximum size the buffer may grow to. (1 MB)
      };

      ostream& operator<<( ostream& os, const emu::daq::rui::EventBuffer& eb );

}}} // namespace emu::daq::rui

#endif
