#include <sys/time.h>

namespace emu{
  namespace daq{
    namespace writer{

      /// RateLimiter limits rate by rejecting events, if necessary.
      ///
      /// It starts by accepting \ref sampleSize events, and if they're all in before the time needed
      /// for the rate to be below \ref limitInHz, it rejects all subsequent events until that time 
      /// (that is, until \ref endOfVetoPeriod_). Then it starts accepting them again.
      class RateLimiter{
	int             limitInHz_;             ///< max rate in Hz
	int             sampleSize_;            ///< number of events in the sample
	int             minTimeSpanInMicrosec_; ///< min time the sample must span for the rate to be below the limit
	struct timeval *timeStamps_;            ///< a ring buffer of time stamps and of size sampleSize_
	int             firstTimeIndex_;        ///< index of the oldest time stamp in the timeStamps_ array
	int             lastTimeIndex_;         ///< index of the most recent time stamp in the timeStamps_ array
	struct timeval  endOfVetoPeriod_;       ///< all events are rejected until the time specified by this
	int             eventCount_;            ///< number of events since start or last lift of veto
	bool            debug_;

	struct timeval  now_;                   ///< gettimeofday will set this to the current time
	struct timezone dummy_;                 ///< gettimeofday will set this; not used

      public:
	/// Constructor.
	///
	/// @param limitInHz rate limit in Hertz 
	/// @param sampleSize sample size (number of events)
	///
	RateLimiter( const int limitInHz, const int sampleSize );

	/// Destructor.
	///
	///
	~RateLimiter();

	/// Finds out whether or not an event can be accepted now.
	/// Should be invoked for every event.
	///
	/// @return \c true if an event can now be accepted, \c false otherwise 
	///
	bool acceptEvent();
      };

    }
  }
}
