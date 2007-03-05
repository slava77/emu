#include "EmuServer.h"
#include <sstream>

EmuServer::EmuServer( xdaq::Application                    *parentApp,
		      const string                          clientName,
		      const unsigned int                    clientInstance,
		      xdata::Serializable                  *prescaling,
		      xdata::Serializable                  *onRequest,
		      xdata::Serializable                  *creditsHeld,
		      const Logger                         *logger )
  throw( xcept::Exception )
  :parentApp_                    (  parentApp ),
   clientName_                   (  clientName ),
   clientInstance_               (  clientInstance ),
   prescaling_                   (  dynamic_cast<xdata::UnsignedLong*>( prescaling  ) ),
   sendDataOnRequestOnly_        (  dynamic_cast<xdata::Boolean*>     ( onRequest   ) ),
   nEventCreditsHeld_            (  dynamic_cast<xdata::UnsignedLong*>( creditsHeld ) ),
   logger_                       ( *logger )
{
  name_            = createName();
  appDescriptor_   = parentApp->getApplicationDescriptor();
  appContext_      = parentApp->getApplicationContext();
  zone_            = appContext_->getDefaultZone();

  dataIsPendingTransmission_ = false;

}

// EmuServer::~EmuServer(){} this must go to the .h file, otherwise the executive won't load this lib module

string EmuServer::createName(){
  ostringstream oss;
  
  oss << parentApp_->getApplicationDescriptor()->getClassName()
      << parentApp_->getApplicationDescriptor()->getInstance()
      << "-to-"
      << clientName_ << clientInstance_;

  return oss.str();
}

void EmuServer::findClientDescriptor()
{
  try
    {
      clientDescriptor_ =
	zone_->getApplicationDescriptor( clientName_, clientInstance_ );
    }
  catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
      clientDescriptor_ = 0;
      LOG4CPLUS_WARN(logger_, 
		     name_ << 
		     " server could not find its client " << clientName_ <<
		     " instance "<< clientInstance_ );
    }
}


void EmuServer::addCredits( const int nCredits, const int prescaling ){
  nEventCreditsHeld_->value_ += nCredits;
  *prescaling_                = prescaling;
  LOG4CPLUS_INFO(logger_, 
		 name_    << " server received credits for " << 
		 nCredits << " events prescaled by 1/" << 
		 prescaling << ". Now holding " << 
		 *nEventCreditsHeld_ );
}
