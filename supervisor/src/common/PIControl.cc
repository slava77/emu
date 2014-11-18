#include "emu/supervisor/PIControl.h"

emu::supervisor::PIControl::PIControl( xdaq::Application *parent, 
				       xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
				       xdata::String partition )
  : emu::supervisor::TCDSControl::TCDSControl( parent, tcdsApplicationDescriptor, partition, "PI" ){}


emu::supervisor::TCDSControl& emu::supervisor::PIControl::setUsePrimaryTCDS( xdata::Boolean& usePrimaryTCDS ){
  messenger_->setParameters( tcdsApplicationDescriptor_, emu::soap::Parameters().add( "usePrimaryTCDS", &usePrimaryTCDS ) );
  return *this;
}
