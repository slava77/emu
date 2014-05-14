
    enum Status_t {
      EndOfEventMissing = 0x0001,	///< end of event missing
      Timeout           = 0x0002,	///< timeout occurred while waiting for data
      PacketsMissing    = 0x0004,	///< one or more ethernet packets didn't make it
      LoopOverwrite     = 0x0008,	///< packet info ring buffer has bitten its own tail
      BufferOverwrite   = 0x0010,	///< data ring buffer has bitten its own tail
      Oversized         = 0x0020	///< too big an event
    };


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

