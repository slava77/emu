#include "emu/supervisor/TCDSControl.h"

#include "emu/utils/String.h"
#include "toolbox/string.h"

#include <unistd.h> // for usleep

emu::supervisor::TCDSControl::TCDSControl( xdaq::Application *parent, 
					   xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
					   xdata::String partition, 
					   string tcdsDeviceName )
  : parentApplication_( parent )
  , messenger_( new emu::soap::Messenger( parent ) )
  , partition_( partition )
  , actionRequestorId_( tcdsDeviceName + "-" + partition.toString() )
  , tcdsApplicationDescriptor_( tcdsApplicationDescriptor )
  , runType_( UNKNOWN ){
}

emu::supervisor::TCDSControl::~TCDSControl(){
  delete messenger_;
}

emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::setRunType( xdata::String& runType ){
  if ( runType.toString().find("ALCT") != string::npos    ) runType_ = AFEBcalibration;
  if ( runType.toString().find("CFEB") != string::npos    ) runType_ = CFEBcalibration;
  if ( toolbox::tolower( runType.toString() ) == "local"  ) runType_ = local;
  if ( toolbox::tolower( runType.toString() ) == "global" ) runType_ = global;
  return *this;
}

emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::configure( xdata::String& hardwareConfigurationString ){
  hardwareConfiguration_ = hardwareConfigurationString;
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Halted" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Configure",
			     emu::soap::Parameters().add( "hardwareConfigurationString", &hardwareConfiguration_ ), 
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
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::enable( xdata::UnsignedInteger& runNumber ){
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Configured" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Enable",
			     emu::soap::Parameters().add( "runNumber", &runNumber ), 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Enable' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                   << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "            << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"      << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::halt(){
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState != "" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Halt",
			     emu::soap::Parameters::none, 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Halt' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                 << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "          << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"    << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  hardwareLease_.reset();
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::stop(){
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Enabled" || initialState == "Paused" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Stop",
			     emu::soap::Parameters::none, 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Stop' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                 << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "          << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"    << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::pause(){
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Enabled" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Pause",
			     emu::soap::Parameters::none, 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Pause' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                  << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "           << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"     << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::resume(){
  string initialState( waitForASteadyState( 20 ) );
  if ( initialState == "Paused" ){
    messenger_->sendCommand( tcdsApplicationDescriptor_, "Resume",
			     emu::soap::Parameters::none, 
			     emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  }
  else{
    ostringstream oss;
    oss << "Attempted to 'Resume' " << tcdsApplicationDescriptor_->getClassName() 
	<< " of "                   << tcdsApplicationDescriptor_->getAttribute( "service" ) 
	<< " (instance "            << tcdsApplicationDescriptor_->getInstance()
	<< ") when it is in '"      << initialState << "' state.";
    XCEPT_RAISE( xcept::Exception, oss.str() );
  }
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendL1A(){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendL1A",
			   emu::soap::Parameters::none, 
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendBgo( xdata::String& bgoName ){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendBgo",
			   emu::soap::Parameters().add( "bgoName", &bgoName ),
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendBgo( xdata::UnsignedInteger& bgoNumber ){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendBgo",
			   emu::soap::Parameters().add( "bgoNumber", &bgoNumber ),
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendBgoTrain( xdata::String& bgoTrainName ){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendBgoTrain",
			   emu::soap::Parameters().add( "bgoTrainName", &bgoTrainName ),
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendBCommand( xdata::UnsignedInteger& bcommandData,
									  xdata::String& bcommandType ){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendBCommand",
			   emu::soap::Parameters().add( "bcommandData", &bcommandData ).add( "bcommandType", &bcommandType ),
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::sendBCommand( xdata::UnsignedInteger& bcommandData,
									  xdata::String& bcommandType,
									  xdata::UnsignedInteger& bcommandAddress,
									  xdata::UnsignedInteger& bcommandSubAddress,
									  xdata::String& bcommandAddressType
									  ){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "SendBCommand",
			   emu::soap::Parameters()
			   .add( "bcommandData", &bcommandData ).add( "bcommandType", &bcommandType )
			   .add( "bcommandAddress", &bcommandAddress ).add( "bcommandSubAddress", &bcommandSubAddress )
			   .add( "bcommandAddressType", &bcommandAddressType ),
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
emu::supervisor::TCDSControl& emu::supervisor::TCDSControl::initCyclicGenerators(){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "InitCyclicGenerators",
			   emu::soap::Parameters::none, 
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return *this;
}
string emu::supervisor::TCDSControl::readHardwareConfiguration(){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "ReadHardwareConfiguration",
			   emu::soap::Parameters::none, 
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
  return string();
}

string emu::supervisor::TCDSControl::getState(){
  xdata::String stateName;
  messenger_->getParameters( tcdsApplicationDescriptor_, emu::soap::Parameters().add( "stateName", &stateName ) );
  return stateName.toString();
}

string emu::supervisor::TCDSControl::getSteadyState(){
  return waitForASteadyState( 30 );
}

bool emu::supervisor::TCDSControl::waitForState( const string& targetState, const int timeoutSeconds ){
  set<string> targetStates( emu::utils::csvTo< set<string> >( targetState, '|' ) );
  for ( set<string>::iterator ts = targetStates.begin(); ts != targetStates.end(); ++ts ){
    if ( !isSteadyState( *ts ) ){
      XCEPT_RAISE( xcept::Exception, "Target state " + *ts + "is not a steady state." );
    }
  }
  string state( waitForASteadyState( timeoutSeconds ) );
  return targetStates.find( state ) != targetStates.end();
}

string emu::supervisor::TCDSControl::waitForASteadyState( const int timeoutSeconds ){
  // If timeoutSeconds is negative, no timeout.
  for ( int i=0; i<=timeoutSeconds || timeoutSeconds<0; ++i ){
    string state(  getState() );
    if ( isSteadyState( state ) ) return state;
    ::sleep( 1 );
  }
  return string();
}

void emu::supervisor::TCDSControl::mSleep( unsigned int milliseconds ) const {
  ::usleep( 1000 * milliseconds );
}

bool emu::supervisor::TCDSControl::isSteadyState( const string& state ){
  if ( state == "Halted"     ) return true;
  if ( state == "Configured" ) return true;
  if ( state == "Enabled"    ) return true;
  if ( state == "Paused"     ) return true;
  if ( state == "Failed"     ) return true;
  return false;
}
