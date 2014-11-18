#include "emu/supervisor/TCDSHardwareLease.h"

#include "toolbox/task/TimerFactory.h"

emu::supervisor::TCDSHardwareLease::TCDSHardwareLease( xdaq::Application *parent, 
						       xdaq::ApplicationDescriptor* tcdsApplicationDescriptor, 
						       const xdata::String& actionRequestorId,
						       toolbox::TimeInterval const& interval )
  : parentApplication_( parent )
  , messenger_( new emu::soap::Messenger( parent ) )
  , actionRequestorId_( actionRequestorId )
  , tcdsApplicationDescriptor_( tcdsApplicationDescriptor )
  , interval_(interval)
  , timer_( toolbox::task::getTimerFactory()->createTimer( actionRequestorId_.toString() ) ){
  timer_->addExceptionListener( dynamic_cast<toolbox::exception::Listener*>( parentApplication_ ) );
  toolbox::TimeVal start = toolbox::TimeVal::gettimeofday();
  timer_->scheduleAtFixedRate( start, this, interval_, NULL, "" );
}

emu::supervisor::TCDSHardwareLease::~TCDSHardwareLease(){
  toolbox::task::getTimerFactory()->removeTimer( actionRequestorId_.toString() );
  delete messenger_;
}

void emu::supervisor::TCDSHardwareLease::timeExpired( toolbox::task::TimerEvent& event ){
  renew();
}

void emu::supervisor::TCDSHardwareLease::renew(){
  messenger_->sendCommand( tcdsApplicationDescriptor_, "RenewHardwareLease",
			   emu::soap::Parameters::none, 
			   emu::soap::Attributes().add( "actionRequestorId", &actionRequestorId_ ) );
}
