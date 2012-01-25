#ifndef __emu_daq_fu_EmuEventHeaderTrailer_h__
#define __emu_daq_fu_EmuEventHeaderTrailer_h__

#include <stdint.h>

#include "emu/daq/server/Base.h"

namespace emu { namespace daq { namespace fu {

  /// Emu event header and trailer.

  /// Events built by the local DAQ are wrapped in Emu event header and trailer.
  /// The size and the fixed bits of Emu event header and trailer are the same as
  /// that of the DCC header and trailer.
  class EmuEventHeaderTrailer{
  public:
    /// constructor
    
    /// @param calibration TRUE if it is a calibration run
    /// @param normalTrigger TRUE if it is normal trigger
    /// @param TFPresent TRUE if the Track Finder DDU data is part of the event
    EmuEventHeaderTrailer( bool calibration, bool normalTrigger, bool TFPresent );
    /// accessor of header
    
    /// @return pointer to the header
    ///
    uint16_t* header(){ return header_; }
    /// accessor of trailer
    
    /// @return pointer to the trailer
    ///
    uint16_t* trailer(){ return trailer_; }
    /// accessor of header size
    
    /// @return header size
    ///
    size_t    headerSize(){ return headerSize_; }
    /// accessor of trailer size
    
    /// @return trailer size
    ///
    size_t    trailerSize(){ return trailerSize_; }
    /// modifier of DDU count
    
    /// @param DDU count
    ///
    void setDDUCount( uint16_t DDUCount );
    /// modifier of L1A counter
    
    /// @param L1A counter
    ///
    void setL1ACounter( uint32_t L1ACounter );
    /// modifier of CSC configuration id
    
    /// @param CSC configuration id
    /// CSC configuration id can in theory be 67 bit long (20 decimal digits);
    /// in practice it's unlikely to ever grow beyond ~20 bits, so the 56-bit field 
    /// should be more than enough for it.
    ///  
    void setCSCConfigId( uint32_t CSCConfigId );
    /// modifier of Track Finder configuration id
    
    /// @param Track Finder configuration id
    /// Track Finder  configuration id can in theory be 67 bit long (20 decimal digits);
    /// in practice it's unlikely to ever grow beyond ~20 bits, so the 52-bit field 
    /// should be more than enough for it.
    ///  
    void setTFConfigId( uint32_t TFConfigId );
  private:
    static const unsigned int version_ = 1; ///< version number to be incremented by 1 every time something is changed
    static const size_t headerSize_ = 16; ///< header size in bytes
    static const size_t trailerSize_ = 16; ///< trailer size in bytes
    uint16_t header_[headerSize_/2]; ///< the header
    uint16_t trailer_[trailerSize_/2]; ///< the trailer

    struct Client{
      emu::daq::server::Base *server;
      Client(emu::daq::server::Base* S=NULL){
	server = S;
      }
    };
  };

}}} // namespace emu::daq::fu

#endif // #ifndef __emu_daq_fu_EmuEventHeaderTrailer_h__
