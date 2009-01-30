#ifndef __EMU_DAQ_READER_SPY_H__
#define __EMU_DAQ_READER_SPY_H__

#include "emu/daq/reader/Base.h"
#include "emu/daq/reader/Clock.h"

namespace emu { namespace daq { namespace reader {

  /// An emu::daq::reader for reading from spy channel.
  class Spy : public emu::daq::reader::Base
  {
  public:

    /// error types
    enum Status_t {
      EndOfEventMissing = 0x0001,	///< end of event missing
      Timeout           = 0x0002,	///< timeout occurred while waiting for data
      PacketsMissing    = 0x0004,	///< one or more ethernet packets didn't make it
      LoopOverwrite     = 0x0008,	///< packet info ring buffer has bitten its own tail
      BufferOverwrite   = 0x0010,	///< data ring buffer has bitten its own tail
      Oversized         = 0x0020	///< too big an event
    };

    /// constructor

    /// @param filename Linux device to read from
    /// @param format format [ emu::daq::reader::Base::DDU | emu::daq::reader::Base::DCC ]
    /// @param debug if \c TRUE , prints debug messages to stdout
    ///
    Spy( std::string filename, int format, bool debug=false );

    /// destructor
    ~Spy();

    /// Opens device for reading.
    ///
    /// @param filename Linux device to read from
    ///
    void open( std::string filename );

    /// Resets and enables device for reading.
    virtual void resetAndEnable();

    /// Unmaps and closes device.
    void close();

    /// Resets device for reading.
    int  reset();
    /// Enables device for reading.
    int  enableBlock();
    /// not used in memory mapped readout
    int  disableBlock();
    /// not used in memory mapped readout
    int  endBlockRead();
    /// not used in memory mapped readout
    int  chunkSize();

  protected:
    int               theFileDescriptor;
    //all below needed for ddu2004 only
    // new additions for MemoryMapped DDU
    char *buf_data;		///< pointer to data to be read from ring buffer
    char *buf_start;		///< pointer to start of data ring buffer
    unsigned long int buf_pnt;	///< read pointer (index; number of bytes) w.r.t. the beginning of data ring buffer
    unsigned long int buf_end;	///< end of data ring buffer w.r.t its beginning
    unsigned long int buf_eend;	///< index in data ring buffer beyond which an event may not fit any more
    unsigned long int buf_pnt_kern; ///< kernel's write pointer (index; number of bytes) w.r.t. the beginning of data ring buffer

    char *ring_start;		///< pointer to start of packet info ring buffer 
    unsigned long int ring_size;	///< size of packet info ring buffer
    unsigned long int ring_pnt;	///< read pointer (index; number of bytes) w.r.t. the beginning of packet info ring buffer
    unsigned short ring_loop;		///< the number of times the reading of the data ring buffer has looped back
    unsigned short ring_loop_kern;         ///< the number of times the writing of the data ring buffer has looped back as obtained from the current entry of the packet info ring
    unsigned short ring_loop_kern2;        ///< the number of times the writing of the data ring buffer has looped back as obtained from the first entry of the packet info ring
    unsigned short timeout;  ///< timeout waiting for event
    unsigned short packets; ///< number of packets in event
    unsigned short pmissing;    ///< packets are  missing at beginning
    unsigned short pmissing_prev; ///< packets are missing at end
    unsigned short end_event;   ///< end of event seen
    unsigned short overwrite;   ///< overwrite

    char *tail_start;		///< not used

    // DEBUG START
    int visitCount;		///< the number of times readDDU has been called
    int oversizedCount;		///< the number of oversized events
    int pmissingCount;		///< the number of times packets were missing
    int loopOverwriteCount;	///< the number of times packet info ring buffer overwrite has occurred
    int bufferOverwriteCount;	///< the number of times data ring buffer overwrite has occurred
    int timeoutCount;		///< the number of times timeout has occurred
    int endEventCount;		///< the number of times end of event has been seen
    emu::daq::reader::Clock *ec;			///< used for timing periodic debug prints
    emu::daq::reader::Clock *ec1;			///< used for timing periodic debug prints
    emu::daq::reader::Clock *ec2;			///< used for timing periodic debug prints
    emu::daq::reader::Clock *ec3;			///< used for timing periodic debug prints
    emu::daq::reader::Clock *ec4;			///< used for timing periodic debug prints
    // DEBUG END
    bool insideEventWithMissingPackets; ///< \c TRUE if this event has had packets missing but not ended yet

    /// Reads DDU data.

    /// @param buf pointer to be set to point to the data
    ///
    /// @return number of bytes read
    ///
    int   readDDU(unsigned short*& buf);

    /// Reads DCC data (<b>not implemented</b>).

    /// @param buf pointer to be set to point to the data
    ///
    /// @return number of bytes read
    ///
    int   readDCC(unsigned short*& buf);

    /// Strips data of ethernet filler words.

    /// @param data pointer to start of data
    /// @param dataLength data length in bytes
    ///
    /// @return length in bytes of data stripped of ethernet filler words
    ///
    int   dataLengthWithoutPadding( const unsigned short* data, const int dataLength );
  };
}}} // namespace emu::daq::reader

#endif
