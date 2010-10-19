#include "emu/daq/rui/EventBuffer.h"
#include <algorithm> // for std::max
#include <iomanip> // for std::max
#include <string.h> // for memcpy

emu::daq::rui::EventBuffer::EventBuffer() :
  bufferSize_( initialBufferSize_ ),
  eventSize_( 0 )
{
  if ( bufferSize_ > maxBufferSize_ ) bufferSize_ = maxBufferSize_;
  buffer_ = new char[bufferSize_];
}

emu::daq::rui::EventBuffer::~EventBuffer(){
  delete [] buffer_;
}

bool
emu::daq::rui::EventBuffer::appendData( const size_t dataSize, const char* data ){
  // Return true if all of the data (dataSize bytes) could be appended
  bool successful = true;

  // Double the buffer size if necessary
  size_t newEventSize = eventSize_+dataSize;
  if ( newEventSize > bufferSize_ ){ 
    try{
      grow( std::max( newEventSize, 2*bufferSize_ ) );
    } 
    catch( std::exception &e ) {
      successful = false;
    }
  }

  if ( newEventSize > bufferSize_ ) successful = false;

  // Copy as much of the data as there's free space left
  size_t copiableDataSize = std::min( dataSize, bufferSize_ - eventSize_ );
  ::memcpy( buffer_+eventSize_, data, copiableDataSize );
  // Update event size
  eventSize_ += copiableDataSize;

  return successful;
}

const char* 
emu::daq::rui::EventBuffer::getEvent() const { return buffer_; }

size_t
emu::daq::rui::EventBuffer::getEventSize() const { return eventSize_; }

void
emu::daq::rui::EventBuffer::empty(){ eventSize_ = 0; }

void
emu::daq::rui::EventBuffer::grow( const size_t newBufferSize ){
  // Nothing to do if it's already max size
  if ( bufferSize_ == maxBufferSize_ ) return;

  // Make sure it doesn't exceed max size
  size_t newSize = newBufferSize;
  if ( newSize > maxBufferSize_ ) newSize = maxBufferSize_;
  
  // Remember old buffer
  char *oldBuffer = buffer_;

  // Create new buffer
  buffer_ = new char[newSize];
  // Copy old buffer's content to new, and delete it
  ::memcpy( buffer_, oldBuffer, eventSize_ );
  delete [] oldBuffer;
  // Update buffer size
  bufferSize_ = newSize;
}

ostream& emu::daq::rui::operator<<( ostream& os, const emu::daq::rui::EventBuffer& eb ){
  os << "size: " << setw(10) << eb. getEventSize() << ", data: ";
  for ( size_t i=0; i<eb.getEventSize(); ++i ){
    os << *(eb.getEvent()+i);
  }
  os << endl;
  return os;
}
