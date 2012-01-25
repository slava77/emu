#include "emu/daq/fu/EmuEventHeaderTrailer.h"

emu::daq::fu::EmuEventHeaderTrailer::EmuEventHeaderTrailer( bool calibration, bool normalTrigger, bool TFPresent ){

  // Initialize with fixed bits inherited from DCC

  header_[0] = 0x005F; // 15 -  0
  header_[1] = 0x0000; // 31 - 16
  header_[2] = 0x0000; // 47 - 32
  header_[3] = 0x5000; // 63 - 48

  header_[4] = 0x0000; // 15 -  0
  header_[5] = 0x0000; // 31 - 16
  header_[6] = 0x0000; // 47 - 32
  header_[7] = 0xD900; // 63 - 48

  trailer_[0] = 0x0000; // 15 -  0
  trailer_[1] = 0x0000; // 31 - 16
  trailer_[2] = 0x0000; // 47 - 32
  trailer_[3] = 0xEF00; // 63 - 48

  trailer_[4] = 0x0003; // 15 -  0
  trailer_[5] = 0x0000; // 31 - 16
  trailer_[6] = 0x0000; // 47 - 32
  trailer_[7] = 0xAF00; // 63 - 48

  // Set Emu event bit
  header_[3] |= 0x0400;

  // Set calibration bit
  if ( calibration ) header_[3] |= 0x0200;

  // Set normal trigger bit
  if ( normalTrigger ) header_[3] |= 0x0100;

  // Set version number
  header_[1] |= 0xFF00 & ( version_ << 8 );

  // Set TF present bit
  if ( TFPresent ) header_[1] |= 0x0080;
}

void emu::daq::fu::EmuEventHeaderTrailer::setL1ACounter( uint32_t L1ACounter ){
  header_[2]  = (uint16_t) ( 0x0000FFFF &   L1ACounter         );
  header_[3] &= 0xFF00; // zero L1ACounter part, keeping the rest
  header_[3] |= (uint16_t) ( 0x000000FF & ( L1ACounter >> 16 ) );
}

void emu::daq::fu::EmuEventHeaderTrailer::setDDUCount( uint16_t DDUCount ){
  header_[1] &= 0xFF80; // zero DDU count part
  header_[1] |= 0x007F & DDUCount;
}

void emu::daq::fu::EmuEventHeaderTrailer::setCSCConfigId( uint32_t CSCConfigId ){
  trailer_[0] = (uint16_t) ( 0x0000FFFF &   CSCConfigId         );
  trailer_[1] = (uint16_t) ( 0x0000FFFF & ( CSCConfigId >> 16 ) );
}

void emu::daq::fu::EmuEventHeaderTrailer::setTFConfigId( uint32_t TFConfigId ){
  trailer_[4] &= 0x000F; // zero non-fixed part
  trailer_[4] |= (uint16_t) ( 0x0000FFF0 & ( TFConfigId <<  4 ) );
  trailer_[5]  = (uint16_t) ( 0x0000FFFF & ( TFConfigId >> 12 ) );
  trailer_[6]  = (uint16_t) ( 0x0000FFFF & ( TFConfigId >> 28 ) );
}
