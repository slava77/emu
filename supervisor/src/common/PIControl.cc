#include "emu/supervisor/PIControl.h"

emu::supervisor::PIControl::PIControl( xdaq::Application *parent, 
				       xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
				       xdata::String partition )
  : emu::supervisor::TCDSControl::TCDSControl( parent, tcdsApplicationDescriptor, partition, "PI" ){}

emu::supervisor::PIControl& emu::supervisor::PIControl::setRunType( xdata::String& runType ){
  this->emu::supervisor::TCDSControl::setRunType( runType );
  return *this;
}

emu::supervisor::PIControl& emu::supervisor::PIControl::configure( xdata::String& hardwareConfigurationString, xdata::Boolean& usePrimaryTCDS ){
  hardwareConfiguration_ = hardwareConfigurationString;
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Halted" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Configure",
			     emu::soap::Parameters()
			     .add( "hardwareConfigurationString", &hardwareConfiguration_ )
			     .add( "usePrimaryTCDS"             , &usePrimaryTCDS         ), 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Configure' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                      << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "               << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"         << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  toolbox::TimeInterval interval;
  interval.fromString( "PT5S" );
  hardwareLease_.reset( new TCDSHardwareLease( parentApplication_,
					       tcdsApplicationDescriptor_,
					       actionRequestorId_,
					       interval                    ) );
  return *this;
}
