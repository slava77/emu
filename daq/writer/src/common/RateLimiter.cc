#include <iostream>
#include <emu/daq/writer/RateLimiter.h>

emu::daq::writer::RateLimiter::RateLimiter( const int limitInHz, const int sampleSize ) :
  limitInHz_( limitInHz ),
  sampleSize_( sampleSize ),
  timeStamps_( new timeval[sampleSize] ),
  firstTimeIndex_( 0 ),
  lastTimeIndex_( 0 ),
  eventCount_( 0 )
{
  if ( limitInHz_ >= 0 ) minTimeSpanInMicrosec_ = int( 1000000 * double(sampleSize) / double(limitInHz) );
  else                   minTimeSpanInMicrosec_ = 0; // This means no limit.
  endOfVetoPeriod_.tv_sec  = 0;
  endOfVetoPeriod_.tv_usec = 0;
}

emu::daq::writer::RateLimiter::~RateLimiter(){
  delete [] timeStamps_;
}

bool emu::daq::writer::RateLimiter::acceptEvent(){
  if ( minTimeSpanInMicrosec_ == 0 ) return true;
  if ( gettimeofday( &now_, &dummy_ ) > 0 ) return false;
  
  timeStamps_[lastTimeIndex_] = now_; // the current time
  
  struct timeval earliestEndOfSample = {
    timeStamps_[firstTimeIndex_].tv_sec  + minTimeSpanInMicrosec_ / 1000000,
    timeStamps_[firstTimeIndex_].tv_usec + minTimeSpanInMicrosec_ % 1000000
  };

  //   if ( eventCount_ % 1000 == 0 ) std::cout 
  // 				   << now_.tv_sec  << " s   "
  // 				   << now_.tv_usec << " us   "
  // 				   << endOfVetoPeriod_.tv_sec  << " s   "
  // 				   << endOfVetoPeriod_.tv_usec << " us  ["
  // 				   << firstTimeIndex_ << ","
  // 				   << lastTimeIndex_  << "]"
  // 				   << "   eventCount " << eventCount_
  // 				   << std::endl;

  if ( eventCount_ == 0x8fffffff ){ eventCount_ = 0; } // assumes 32-bit integer
  else                            { eventCount_++; }

  lastTimeIndex_++;
  lastTimeIndex_ = ( lastTimeIndex_ % sampleSize_ );

  if ( eventCount_ >= sampleSize_ ){
    // Once we have a sample of sampleSize_ events, we decide 
    // on the basis of the time between its first and last events.

    // Move the index of the first event in the ring buffer.
    firstTimeIndex_++;
    firstTimeIndex_ = ( firstTimeIndex_ % sampleSize_ );

    if ( endOfVetoPeriod_.tv_sec > 0 ){
      // A veto period is specified. Reject events until endOfVetoPeriod_.
      if ( ( now_.tv_sec  > endOfVetoPeriod_.tv_sec                                            ) || 
	   ( now_.tv_sec == endOfVetoPeriod_.tv_sec && now_.tv_usec > endOfVetoPeriod_.tv_usec )    ){
	// Veto period is already over. Unset it.
	endOfVetoPeriod_.tv_sec  = 0;
	endOfVetoPeriod_.tv_usec = 0;
	// Reset eventCount_ so that we can start accepting unconditionally 
	// all events until sampleSize_ is reached.
	eventCount_ = 1;
	return true;
      }
      else{
	// Veto is still in effect. Therefore
	return false; 
      }
    }
    else if ( ( now_.tv_sec  > earliestEndOfSample.tv_sec                                               ) || 
	      ( now_.tv_sec == earliestEndOfSample.tv_sec && now_.tv_usec > earliestEndOfSample.tv_usec )    ){
      // Time diff between the first sample and the current sample is long enough 
      // for the rate to be below the limit, therefore:
      return true;
    }
    else{
      // Time diff between the first sample and the current sample is too short,
      // i.e., the rate is too high. Set a veto period, and reject this event.
      endOfVetoPeriod_ = earliestEndOfSample;
      return false;
    }
  }
  else{
    // We accept unconditionally all events until sampleSize_ of them is accumulated.
    return true;
  }

}
