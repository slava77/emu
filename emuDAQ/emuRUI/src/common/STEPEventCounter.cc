#include "emuRUI/STEPEventCounter.h"

#include <sstream>
#include <iostream>
#include <iomanip>

namespace emuRUI{

  STEPEventCounter::STEPEventCounter(){
    for ( int i=0; i<maxDDUInputs_; ++i ) isMaskedInput_[i] = false;
    reset();
  }

  void STEPEventCounter::initialize( const unsigned int requestedEvents, char* const DDUHeader ){
    // Sets number of requested events, live DDU inputs and zeros all counters.
    requestedEvents_ = requestedEvents;
    neededEvents_ = 0;
    short* liveDDUInputs = (short*)( DDUHeader + offsetLiveDDUInputsField_ );
    // std::cout << "LLLL: " << std::hex << *liveDDUInputs << " " << std::dec << *liveDDUInputs << std::endl; 
    short bitMask = 0x0001;
    for ( int i=0; i<maxDDUInputs_; ++i ){
      isLiveInput_[i] = bool( bitMask & *liveDDUInputs );
      bitMask <<= 1;
      count_[i] = 0;
    }
    isInitialized_ = true;
  }

  void STEPEventCounter::reset(){
    // Zeros number of requested events and all counters, and unsets live DDU inputs.
    // Does NOT unmask inputs.
    requestedEvents_ = 0;
    neededEvents_ = 0;
    for ( int i=0; i<maxDDUInputs_; ++i ){
      isLiveInput_[i] = false;
      count_[i] = 0;
    }
    isInitialized_ = false;
  }

  bool STEPEventCounter::isNeededEvent( char* const DDUHeader ){
    // This event is needed if it contains data from a DDU input that has not yet produced
    // data in the requested number of events.
    // If this event is needed, increment counters for DDU inputs with data in this event.

    // Need this event?
    bool isNeeded = false;
    short* nonEmptyDDUInputs = (short*)( DDUHeader + offsetNonEmptyDDUInputsField_ );
    short bitMask = 0x0001;
    for ( int i=0; i<maxDDUInputs_; ++i ){
      if ( bool(bitMask & *nonEmptyDDUInputs) && 
	   ! isMaskedInput_[i]                &&
	   count_[i] < requestedEvents_          ){
	isNeeded = true;
	break;
      }
      bitMask <<= 1;
    }

    // If so, increment counters.
    if ( isNeeded ){
      bitMask = 0x0001;
      for ( int i=0; i<maxDDUInputs_; ++i ){
	if ( bitMask & *nonEmptyDDUInputs && 
	     ! isMaskedInput_[i]             ) ++count_[i];
	bitMask <<= 1;
      }
      ++neededEvents_;
    }

    return isNeeded;
  }

  unsigned int STEPEventCounter::getLowestCount() const {
    if ( isInitialized_ ){
      unsigned int lowestCount = 0x7fffffff; // When cast to int, this should still be positive.
      bool allExcluded = true;
      for ( int i=0; i<maxDDUInputs_; ++i ){
	allExcluded &= ( !isLiveInput_[i] || isMaskedInput_[i]);
	if ( isLiveInput_[i] && ! isMaskedInput_[i] )
	  if ( count_[i] < lowestCount ) lowestCount = count_[i];
      }
      if ( allExcluded ) return 0x7fffffff; // When cast to int, this should still be positive.
      return lowestCount;
    }
    return 0;
  }

  unsigned int STEPEventCounter::getCount( const int dduInputIndex ) const {
    if ( 0 <= dduInputIndex && dduInputIndex < maxDDUInputs_ ) return count_[dduInputIndex];
    return 0;
  }

  bool STEPEventCounter::isLiveInput( const int dduInputIndex ) const {
    if ( 0 <= dduInputIndex && dduInputIndex < maxDDUInputs_ ) return isLiveInput_[dduInputIndex];
    return false;
  }

  void STEPEventCounter::maskInput( const int dduInputIndex ){
    if ( 0 <= dduInputIndex && dduInputIndex < maxDDUInputs_ ) isMaskedInput_[dduInputIndex] = true;
  }

  void STEPEventCounter::unmaskInput( const int dduInputIndex ){
    if ( 0 <= dduInputIndex && dduInputIndex < maxDDUInputs_ ) isMaskedInput_[dduInputIndex] = false;
  }

  bool STEPEventCounter::isMaskedInput( const int dduInputIndex ) const {
    if ( 0 <= dduInputIndex && dduInputIndex < maxDDUInputs_ ) return isMaskedInput_[dduInputIndex];
    return false;
  }

  std::string STEPEventCounter::print() const {
    // Prints comma-separated list of counts. 
    // Prints '-' for inputs that are not alive.
    // Prints '_' for inputs that are masked.
    // Prints '=' for inputs that are masked and not alive.
    std::stringstream counters;
    for ( int i=0; i<maxDDUInputs_; ++i ){
      if      (   isLiveInput_[i] && ! isMaskedInput_[i] ) counters << count_[i];
      else if ( ! isLiveInput_[i] && ! isMaskedInput_[i] ) counters << "-";
      else if (   isLiveInput_[i] &&   isMaskedInput_[i] ) counters << "_";
      else if ( ! isLiveInput_[i] &&   isMaskedInput_[i] ) counters << "=";
      if ( i+1 != maxDDUInputs_ ) counters << ",";
    }
    return counters.str();
  }

}
