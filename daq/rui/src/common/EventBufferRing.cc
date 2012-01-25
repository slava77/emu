#include "emu/daq/rui/EventBufferRing.h"
#include <ostream> // for size_t, too
#include <iomanip>

emu::daq::rui::EventBufferRing::EventBufferRing(){
  // Ring must have at least one element:
  ring_.push_back( new emu::daq::rui::EventBuffer() );
  current_ = ring_.begin();
}

emu::daq::rui::EventBufferRing::EventBufferRing( const size_t size ){
  for ( size_t i=0; i<size; ++i ){
    ring_.push_back( new emu::daq::rui::EventBuffer() );
  }
  current_ = ring_.begin();
}

emu::daq::rui::EventBufferRing::~EventBufferRing(){
  Ring<emu::daq::rui::EventBuffer*>::iterator it = current_;
  do {
    delete *it;
  } while ( ++it != current_ );
}

size_t
emu::daq::rui::EventBufferRing::setSize( size_t size ){
  // It can only grow or remain the same size:
  for ( size_t i=ring_.size(); i<size; ++i ){
    ring_.push_back( new emu::daq::rui::EventBuffer() );
  }
  return ring_.size();
}

void
emu::daq::rui::EventBufferRing::emptyEventBuffers(){
  Ring<emu::daq::rui::EventBuffer*>::iterator it = current_;
  do {
    (*it)->empty();
  } while ( ++it != current_ );
}

bool 
emu::daq::rui::EventBufferRing::addData( const size_t dataSize, const char* data, const bool startsEvent ){
  // If this data block starts the event, shift the current event pointer to the next event buffer,
  // and empty it:
  if ( startsEvent ){
    ++current_;
    (*current_)->empty();
  }
  // Now write into it:
  bool successful = (*current_)->appendData( dataSize, data );
  return successful;
}

list<const emu::daq::rui::EventBuffer*>
emu::daq::rui::EventBufferRing::getEventBuffers() const {
  list<const emu::daq::rui::EventBuffer*> listOfNonEmptyBuffers;

  // Start collecting non-empty event buffers with the oldest one, 
  // i.e., the one after the current one:
  Ring<emu::daq::rui::EventBuffer*>::iterator it = current_;
  ++it;
  do {
    if ( (*it)->getEventSize() > 0 ){
      listOfNonEmptyBuffers.push_back( *it );
    }
  } while ( it++ != current_ );

  return listOfNonEmptyBuffers;
}

ostream& emu::daq::rui::operator<<( ostream& os, emu::daq::rui::EventBufferRing& ebr ){
  os << "EventBufferRing has " << setw(4) << ebr.ring_.size() << " elements (arrow is write pointer):" << endl;
  Ring<emu::daq::rui::EventBuffer*>::iterator it = ebr.ring_.begin();
  for ( size_t i=0; i<ebr.ring_.size(); ++i ){
    os << ( it == ebr.current_ ? "     -->" : "        " )
       << setw(4) << i << "  " << **it;
    ++it;
  }
  return os;
}

///////////////////
// Example and test
///////////////////

// Uncomment main() below and test it with
// g++ -g -Wall -I$BUILD_HOME/emu/daq/rui/include EventBuffer.cc EventBufferRing.cc && ./a.out

// int main( int argc, char** argv ){
//   char *data = "abcd";
//   size_t size = 4;

//   emu::daq::rui::EventBufferRing ebr;
//   ebr.setSize(5);
//   cout << ebr.addData( size, data, true ) << endl;
//   cout << ebr.addData( size, data, false) << endl;
//   cout << ebr.addData( size, data, false) << endl;

//   cout << ebr.addData( size, data, true ) << endl;

//   cout << ebr.addData( size, data, true ) << endl;

//   cout << ebr.addData( size, data, true ) << endl;

//   // cout << ebr.addData( size, data, true ) << endl;
//   // cout << ebr.addData( size, data, false) << endl;

//   // cout << ebr.addData( size, data, true ) << endl;
//   // cout << ebr.addData( size, data, false) << endl;
//   // cout << ebr.addData( size, data, false) << endl;
//   // cout << ebr.addData( size, data, false) << endl;
//   // cout << ebr.addData( size, data, false) << endl;


//   cout << ebr;

//   cout << endl << "List of non-empty buffers, oldest first:" << endl;
//   list<const emu::daq::rui::EventBuffer*> ebl( ebr.getEventBuffers() );
//   for ( list<const emu::daq::rui::EventBuffer*>::const_iterator i=ebl.begin(); i!=ebl.end(); ++i ) cout << **i;

//   cout << endl << "Circular buffer after having been emptied:" << endl;
//   ebr.emptyEventBuffers();
//   cout << ebr;

//   cout << endl << "List of non-empty buffers, oldest first:" << endl;
//   ebl = ebr.getEventBuffers();
//   for ( list<const emu::daq::rui::EventBuffer*>::const_iterator i=ebl.begin(); i!=ebl.end(); ++i ) cout << **i;

//   return 0;
// }
