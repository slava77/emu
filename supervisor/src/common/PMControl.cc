#include "emu/supervisor/PMControl.h"

emu::supervisor::PMControl::PMControl( xdaq::Application *parent, 
				       xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
				       xdata::String partition )
  : emu::supervisor::TCDSControl::TCDSControl( parent, tcdsApplicationDescriptor, partition, "PM" ){}


emu::supervisor::PMControl& emu::supervisor::PMControl::configureSequence(){
  switch ( runType_ ){
  case global:
    // In global, we're not in control of the PM, therefore:
    return *this;
  case local:
    // First of all, wait for PM to complete 'Configure' transition
    waitForState( "Configured", 20 );
    break;
  case AFEBcalibration:
    // First of all, wait for PM to complete 'Configure' transition
    waitForState( "Configured", 20 );
    break;
  case CFEBcalibration:
    // First of all, wait for PM to complete 'Configure' transition
    waitForState( "Configured", 20 );
    break;
  default:
    XCEPT_RAISE( xcept::Exception, "Unknown run type." );
  }
  return *this;
}

emu::supervisor::PMControl::PMControl& emu::supervisor::PMControl::enableSequence(){
  switch ( runType_ ){
  case global:
    // In global, we're not in control of the PM, therefore:
    return *this;
  case local:           // fall through
  case AFEBcalibration: // fall through
  case CFEBcalibration:
    // First of all, wait for PM to complete 'Enable' transition
    waitForState( "Enabled", 20 );
    break;
  default:
    XCEPT_RAISE( xcept::Exception, "Unknown run type." );
  }
  return *this;
}

emu::supervisor::PMControl::PMControl& emu::supervisor::PMControl::stopSequence(){
  switch ( runType_ ){
  case global:
    // In global, we're not in control of the PM, therefore:
    return *this;
  case local:
    // First of all, wait for PM to complete the 'Stop' transition
    // waitForState( "Halted|Configured", 20 );
    waitForState( "Configured", 20 );
    break;
  case AFEBcalibration:
    // First of all, wait for PM to complete the 'Stop' transition
    waitForState( "Configured", 20 );
    break;
  case CFEBcalibration:
    // First of all, wait for PM to complete the 'Stop' transition
    waitForState( "Configured", 20 );
    break;
  default:
    XCEPT_RAISE( xcept::Exception, "Unknown run type." );
  }
  return *this;
}
