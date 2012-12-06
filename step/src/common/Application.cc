#include "emu/step/Application.h"

#include "xdaq/NamespaceURI.h"

emu::step::Application::Application( xdaq::ApplicationStub *s ) :
  xdaq::WebApplication( s ),
  bsem_( toolbox::BSem::EMPTY ), // locked
  logger_( Logger::getInstance( generateLoggerName() ) )
{

  fsm_.addState('H', "Halted",         this, &emu::step::Application::stateChanged);
  fsm_.addState('C', "Configured",     this, &emu::step::Application::stateChanged);
  fsm_.addState('E', "Enabled",        this, &emu::step::Application::stateChanged);
  fsm_.setFailedStateTransitionChanged(this, &emu::step::Application::stateChanged);
  
  fsm_.addStateTransition('H', 'C', "Configure", this, &emu::step::Application::configureAction);
  fsm_.addStateTransition('C', 'E', "Enable",    this, &emu::step::Application::enableAction);
  fsm_.addStateTransition('E', 'C', "Stop",      this, &emu::step::Application::stopAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &emu::step::Application::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &emu::step::Application::haltAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &emu::step::Application::noAction);
  fsm_.addStateTransition('C', 'C', "Stop",      this, &emu::step::Application::noAction);
  fsm_.addStateTransition('H', 'H', "Enable",    this, &emu::step::Application::noAction);    
  fsm_.addStateTransition('H', 'H', "Stop",      this, &emu::step::Application::noAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &emu::step::Application::noAction);
  fsm_.addStateTransition('E', 'E', "Configure", this, &emu::step::Application::noAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &emu::step::Application::noAction);
  fsm_.setFailedStateTransitionAction(           this, &emu::step::Application::failAction);
  // Define "Fail" transition explicitly for moveToFailedState to work:
  fsm_.addStateTransition('H', 'F', "Fail",      this, &emu::step::Application::failAction);
  fsm_.addStateTransition('C', 'F', "Fail",      this, &emu::step::Application::failAction);
  fsm_.addStateTransition('E', 'F', "Fail",      this, &emu::step::Application::failAction);
  // Make "Failed" state impervious to all commands but "Reset":
  fsm_.addStateTransition('F', 'H', "Reset",     this, &emu::step::Application::resetAction);
  fsm_.addStateTransition('F', 'F', "Configure", this, &emu::step::Application::noAction);    
  fsm_.addStateTransition('F', 'F', "Enable",    this, &emu::step::Application::noAction);
  fsm_.addStateTransition('F', 'F', "Stop",      this, &emu::step::Application::noAction);
  fsm_.addStateTransition('F', 'F', "Halt",      this, &emu::step::Application::noAction);
  
  fsm_.setStateName('F', "Failed");
  
  fsm_.setInitialState( 'H' );
  fsm_.reset();


  // SOAP callbacks
  xoap::bind(this, &emu::step::Application::onConfigure,     "Configure",     XDAQ_NS_URI);
  xoap::bind(this, &emu::step::Application::onEnable,        "Enable",        XDAQ_NS_URI);
  xoap::bind(this, &emu::step::Application::onHalt,          "Halt",          XDAQ_NS_URI);
  xoap::bind(this, &emu::step::Application::onReset,         "Reset",         XDAQ_NS_URI);

  
  state_ = fsm_.getStateName( fsm_.getCurrentState() );
  getApplicationInfoSpace()->fireItemAvailable( "State"    , &state_ );
  getApplicationInfoSpace()->fireItemAvailable( "state"    , &state_ );
  getApplicationInfoSpace()->fireItemAvailable( "stateName", &state_ );
  reasonForFailure_ = "";
  getApplicationInfoSpace()->fireItemAvailable("reasonForFailure", &reasonForFailure_);

  bsem_.give();
}

void emu::step::Application::noAction(toolbox::Event::Reference e){
}

void emu::step::Application::failAction(toolbox::Event::Reference event)
{
    try
    {
      if ( typeid(*event) == typeid(toolbox::fsm::FailedEvent) ){
        toolbox::fsm::FailedEvent &failedEvent = dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
        xcept::Exception exception = failedEvent.getException();
	reasonForFailure_ = xhtmlformat_exception_history( exception );

        stringstream ss;
        ss << "Failure occurred when performing transition from "
	   << fsm_.getStateName( failedEvent.getFromState() ) 
	   << " to " 
	   << fsm_.getStateName( failedEvent.getToState() )
	   << ": " << xcept::stdformat_exception_history( exception );
        LOG4CPLUS_FATAL(logger_, ss.str() );
      }
    }catch( xcept::Exception& e ){
      stringstream ss;
      ss <<  "Caught exception while moving to Failed state: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_FATAL(logger_, ss.str() );
    }catch( std::exception& e ){
      stringstream ss;
      ss <<  "Caught exception while moving to Failed state: " << e.what();
      LOG4CPLUS_FATAL(logger_, ss.str() );
    }catch(...){
      stringstream ss;
      ss <<  "Caught an unknown exception while moving to Failed state.";
      LOG4CPLUS_FATAL( logger_, ss.str() );
    }
}

void emu::step::Application::resetAction( toolbox::Event::Reference event ){
  // To be only used to get out of 'Failed' state.
  fsm_.reset();
}

void emu::step::Application::stateChanged( toolbox::fsm::FiniteStateMachine &fsm ){
  state_ = fsm.getStateName( fsm.getCurrentState() );
  LOG4CPLUS_INFO( logger_, string( "State changed to " ) + (string)state_ );
}

void emu::step::Application::fireEvent( string name ){
  toolbox::Event::Reference event( new toolbox::Event(name, this) );
  // fsm_.fireEvent( event ) expects a toolbox::fsm::exception::Exception to be thrown.
  // It catches it but instead of rethrowing it, it sets the state to 'F' (Failed).
  fsm_.fireEvent( event );
}

void emu::step::Application::moveToFailedState( xcept::Exception exception ){
  // Use this from inside the work loop to force the FSM to Failed state 
  bsem_.take();
  try{
    toolbox::Event::Reference evtRef( new toolbox::Event( "Fail", this ) );
    reasonForFailure_ = xhtmlformat_exception_history( exception );
    fsm_.fireEvent( evtRef );
  } 
  catch( xcept::Exception &e ){
    LOG4CPLUS_FATAL( logger_, "Failed to move to the Failed state : " << xcept::stdformat_exception_history(e) );
  }
  bsem_.give();
}

string emu::step::Application::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    unsigned long               appInstance    = appDescriptor->getInstance();
    stringstream                ss;
    string                      loggerName;

    ss << appClass << appInstance;
    loggerName = ss.str();

    return loggerName;
}

string emu::step::Application::xhtmlformat_exception_history( xcept::Exception& e ){
  stringstream ss;
  ss << "<table class=\"Xcept\">";
  ss << "<tr><th colspan=\"3\">Exception stack</th></tr>";
  ss << "<tr><th style=\"width: 1%;\">layer</th><th>message</th><th>raised at</th></tr>";

  std::vector<xcept::ExceptionInformation> & history = e.getHistory();
  std::vector<xcept::ExceptionInformation>::reverse_iterator i = history.rbegin();
  int iLayer = history.size();
  while ( i != history.rend() ){
    ss << "<tr>";
    ss << "<td style=\"width: 1%;\">" << iLayer << "</td>";
    // Do not put message in a CDATA section as the message may already contain one, and CDATA sections are not allowed to be nested.
    // ss << "<td style=\"background-color: #dddd11;\"><![CDATA[" << i->getProperty("message") << "]]></td>";
    ss << "<td style=\"background-color: #dddd11;\">" << i->getProperty("message") << "</td>";
    ss << "<td style=\"text-align: left;\">" << i->getProperty("function") << " (" << i->getProperty("module") << ":" << i->getProperty("line") << ")</td>";
    ss << "</tr>";
    ++i;
    --iLayer;
  }

  ss << "</table>";

  return ss.str();
}
