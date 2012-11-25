// Application.cc

#include "emu/supervisor/Application.h"
#include "emu/base/Stopwatch.h"

#include <sstream>
#include <set>
#include <map>
#include <cstdlib>  // strtol()
#include <iomanip>
#include <sys/time.h>  // gettimeofday()

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"
#include "xoap/domutils.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"
#include "xdata/soap/Serializer.h"

#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"

#include "xcept/tools.h"
#include "xdaq2rc/RcmsStateNotifier.h"
#include "toolbox/fsm/FailedEvent.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationDescriptorImpl.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/XceptSerializer.h"
#include "xdata/Float.h" 
#include "xdata/Double.h" 
#include "xdata/Boolean.h"

#include "emu/supervisor/exception/Exception.h"
#include "emu/supervisor/alarm/Alarm.h"
#include "emu/base/Alarm.h"
#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"

using namespace std;
using namespace cgicc;

XDAQ_INSTANTIATOR_IMPL(emu::supervisor::Application)

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";
static const unsigned int N_LOG_MESSAGES = 50;
static const string STATE_UNKNOWN = "unknown";

void emu::supervisor::Application::CalibParam::registerFields(xdata::Bag<CalibParam> *bag)
{
  key_ = "";
  command_ = "";
  loop_ = 1U;
  delay_ = 1U;
  ltc_ = "LTCConfiguration.txt";
  ttcci_ = "TTCciConfiguration_ListenCSCLTC904.txt";
  
  bag->addField("key",     &key_);
  bag->addField("command", &command_);
  bag->addField("loop",    &loop_);
  bag->addField("delay",   &delay_);
  bag->addField("ltc",     &ltc_);
  bag->addField("ttcci",   &ttcci_);
}

emu::supervisor::Application::Application(xdaq::ApplicationStub *stub)
  //throw (xcept::Exception) 
  :
  xdaq::WebApplication(stub),
  emu::base::Supervised(stub),
  logger_(Logger::getInstance("emu::supervisor::Application")),
  isInCalibrationSequence_(false),
  run_type_("Monitor"), run_number_(1), runSequenceNumber_(0),
  daq_mode_("UNKNOWN"), ttc_source_(""),
  rcmsStateNotifier_(getApplicationLogger(), getApplicationDescriptor(), getApplicationContext()),
  TFCellOpState_(""), TFCellOpName_("Configuration"), TFCellClass_("Cell"), TFCellInstance_(8), 
  wl_semaphore_(toolbox::BSem::EMPTY), quit_calibration_(false),
  daq_descr_(NULL), tf_descr_(NULL), ttc_descr_(NULL),
  nevents_(-1),
  step_counter_(0),
  error_message_(""), keep_refresh_(false), hide_tts_control_(true),
  controlTFCellOp_(false), // Default MUST be false, i.e., hands off the TF Cell.
  runInfo_(NULL),
  runDbBookingCommand_( "java -jar runnumberbooker.jar" ),
  runDbWritingCommand_( "java -jar runinfowriter.jar" ),
  runDbAddress_       ( "" ),
  runDbUserFile_      ( "" ),
  isBookedRunNumber_  ( false ),
  state_table_(this)
{  
  appDescriptor_ = getApplicationDescriptor();
  
  xdata::InfoSpace *i = getApplicationInfoSpace();
  i->fireItemAvailable("isInCalibrationSequence", &isInCalibrationSequence_);
  i->fireItemAvailable("RunType", &run_type_);
  i->fireItemAvailable("RunNumber", &run_number_);
  
  i->fireItemAvailable("configKeys", &config_keys_);
  i->fireItemAvailable("calibParams", &calib_params_);
  i->fireItemAvailable("pcKeys",     &pc_keys_);
  i->fireItemAvailable("pcConfigs",  &pc_configs_);
  i->fireItemAvailable("fcKeys",     &fc_keys_);
  i->fireItemAvailable("fcConfigs",  &fc_configs_);
  
  i->fireItemAvailable("DAQMode", &daq_mode_);
  i->fireItemAvailable("TTCSource", &ttc_source_);
  
  i->fireItemAvailable("TFCellOpState",  &TFCellOpState_);
  i->fireItemAvailable("TFCellOpName",   &TFCellOpName_);
  i->fireItemAvailable("TFCellClass",    &TFCellClass_);
  i->fireItemAvailable("TFCellInstance", &TFCellInstance_);

  i->fireItemAvailable("ttsID", &tts_id_);
  i->fireItemAvailable("ttsBits", &tts_bits_);

  i->fireItemAvailable("controlTFCellOp", &controlTFCellOp_);

  // Track Finder Key
  tf_key_ = "310309";   // default key as of 31/03/2009
  i->fireItemAvailable("TrackFinderKey", &tf_key_);  

  i->fireItemAvailable( "runDbBookingCommand", &runDbBookingCommand_ );
  i->fireItemAvailable( "runDbWritingCommand", &runDbWritingCommand_ );
  i->fireItemAvailable( "runDbAddress",        &runDbAddress_        );
  i->fireItemAvailable( "runDbUserFile",       &runDbUserFile_       );
  
  //Interface to Run Control
  i->fireItemAvailable("rcmsStateListener", rcmsStateNotifier_.getRcmsStateListenerParameter());
  i->fireItemAvailable("foundRcmsStateListener", rcmsStateNotifier_.getFoundRcmsStateListenerParameter());
  rcmsStateNotifier_.subscribeToChangesInRcmsStateListener(getApplicationInfoSpace());
	
  xgi::bind(this, &emu::supervisor::Application::webDefault,     "Default");
  xgi::bind(this, &emu::supervisor::Application::webConfigure,   "Configure");
  xgi::bind(this, &emu::supervisor::Application::webStart,       "Start");
  xgi::bind(this, &emu::supervisor::Application::webStop,        "Stop");
  xgi::bind(this, &emu::supervisor::Application::webHalt,        "Halt");
  xgi::bind(this, &emu::supervisor::Application::webReset,       "Reset");
  xgi::bind(this, &emu::supervisor::Application::webSetTTS,      "SetTTS");
  xgi::bind(this, &emu::supervisor::Application::webSwitchTTS,   "SwitchTTS");
  xgi::bind(this, &emu::supervisor::Application::webRunSequence, "RunSequence");
  
  xoap::bind(this, &emu::supervisor::Application::onConfigure,   "Configure",   XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onStart,       "Start",       XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onStop,        "Stop",        XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onHalt,        "Halt",        XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onReset,       "Reset",       XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onSetTTS,      "SetTTS",      XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onRunSequence, "RunSequence", XDAQ_NS_URI);
  
  calib_wl_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("CSC SV Calib", "waiting");
  calib_wl_->activate();
  wl_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("CSC SV", "waiting");
  wl_->activate();
  configure_signature_   = toolbox::task::bind(this, &emu::supervisor::Application::configureAction,      "configureAction");
  start_signature_       = toolbox::task::bind(this, &emu::supervisor::Application::startAction,          "startAction");
  stop_signature_        = toolbox::task::bind(this, &emu::supervisor::Application::stopAction,           "stopAction");
  halt_signature_        = toolbox::task::bind(this, &emu::supervisor::Application::haltAction,           "haltAction");
  calibration_signature_ = toolbox::task::bind(this, &emu::supervisor::Application::calibrationAction,    "calibrationAction");
  sequencer_signature_   = toolbox::task::bind(this, &emu::supervisor::Application::calibrationSequencer, "calibrationSequencer");
  
  fsm_.addState('H', "Halted",     this, &emu::supervisor::Application::stateChanged);
  fsm_.addState('C', "Configured", this, &emu::supervisor::Application::stateChanged);
  fsm_.addState('E', "Running",    this, &emu::supervisor::Application::stateChanged);
  
  fsm_.setStateName('F', "Error");
  fsm_.setFailedStateTransitionAction(this, &emu::supervisor::Application::transitionFailed);
  fsm_.setFailedStateTransitionChanged(this, &emu::supervisor::Application::stateChanged);
  
  //fsm_.setStateName('F',"Error", this, &emu::supervisor::Application::transitionFailed);
  
  //	fsm_.addState('c', "Configuring", this, &emu::supervisor::Application::stateChanged);
  
  fsm_.addStateTransition(
			  'H', 'C', "Configure", this, &emu::supervisor::Application::configureAction);
  fsm_.addStateTransition(
			  'C', 'C', "Configure", this, &emu::supervisor::Application::configureAction);
  fsm_.addStateTransition(
			  'C', 'E', "Start",     this, &emu::supervisor::Application::startAction);
  fsm_.addStateTransition(
			  'E', 'C', "Stop",      this, &emu::supervisor::Application::stopAction);
  fsm_.addStateTransition(
			  'C', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'E', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'H', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'E', 'E', "SetTTS",    this, &emu::supervisor::Application::setTTSAction);
  
  // Define invalid transitions, too, so that they can be ignored, or else FSM will be unhappy when one is fired.
  fsm_.addStateTransition('E', 'E', "Configure", this, &emu::supervisor::Application::noAction);
  fsm_.addStateTransition('H', 'H', "Start"    , this, &emu::supervisor::Application::noAction);
  fsm_.addStateTransition('E', 'E', "Start"    , this, &emu::supervisor::Application::noAction);
  fsm_.addStateTransition('H', 'H', "Stop"     , this, &emu::supervisor::Application::noAction);
  fsm_.addStateTransition('C', 'C', "Stop"     , this, &emu::supervisor::Application::noAction);


  fsm_.setInitialState('H');
  fsm_.reset();
  
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  
  state_table_.addApplication("emu::fed::Manager");
  state_table_.addApplication("emu::pc::EmuPeripheralCrateManager");
  state_table_.addApplication("emu::daq::manager::Application");
  state_table_.addApplication("ttc::TTCciControl");
  state_table_.addApplication("ttc::LTCControl");
  
  // last_log_.size(N_LOG_MESSAGES);
  
  getAppDescriptors();

  LOG4CPLUS_INFO(logger_, "emu::supervisor::Application constructed for " << state_table_ );
}

void emu::supervisor::Application::getAppDescriptors(){

  try {
    daq_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor("emu::daq::manager::Application", 0);
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    LOG4CPLUS_ERROR(logger_, "Failed to get application descriptor for local DAQ Manager. "
		    << xcept::stdformat_exception_history(e));
    stringstream ss;
    ss <<  "Failed to get application descriptor for local DAQ Manager. ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }

  try {
    ttc_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor("ttc::TTCciControl", 0);
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    LOG4CPLUS_ERROR(logger_, "Failed to get application descriptor for ttc::TTCciControl. "
		    << xcept::stdformat_exception_history(e));
    stringstream ss;
    ss <<  "Failed to get application descriptor for ttc::TTCciControl. ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }

  try {
    tf_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor( TFCellClass_.toString(), TFCellInstance_.value_ );
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    stringstream ss;
    ss << "No Track Finder application \"" << TFCellClass_.toString() 
       << "\" of instance " << TFCellInstance_.value_ << " found.";
    LOG4CPLUS_ERROR(logger_, ss.str() << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }  
  
}

xoap::MessageReference emu::supervisor::Application::onConfigure(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  run_number_ = 1;
  nevents_ = -1;
  
  submit(configure_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onStart(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  submit(start_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onStop(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;

  submit(stop_signature_);

  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onHalt(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  quit_calibration_ = true;
  
  submit(halt_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onReset(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  resetAction();
  
  return onHalt(message);
}

xoap::MessageReference emu::supervisor::Application::onSetTTS(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  try{
    fireEvent("SetTTS");
  }
  catch( toolbox::fsm::exception::Exception& e ){
    XCEPT_RETHROW( xoap::exception::Exception, "SetTTS failed", e );
  }

  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onRunSequence(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  isCommandFromWeb_ = false;
  calib_wl_->submit( sequencer_signature_ );
  return createReply(message);
}

void emu::supervisor::Application::webDefault(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  if (keep_refresh_ || bool(isInCalibrationSequence_)) {
    HTTPResponseHeader &header = out->getHTTPResponseHeader();
    header.addHeader("Refresh", "2");
  }
  
  // Header
  *out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
  *out << html() << endl;
  
  *out << head() << endl;
  *out << title(string("Supervisor ")+state_.toString()) << endl;
  *out << cgicc::link().set("rel", "stylesheet")
    .set("href", "/emu/supervisor/html/emusupervisor.css")
    .set("type", "text/css") << endl;
  *out << head() << endl;
  
  // Body
  *out << body() << endl;
  
  *out << table() << tbody() << tr();

  *out << td();
  // Config listbox
  *out << form().set("action",
		     "/" + getApplicationDescriptor()->getURN() + "/Configure") << endl;
  
  int n_keys = config_keys_.size();
  
  *out << "Run Type: " << endl;
  *out << cgicc::select().set("name", "runtype") << endl;
  
  int selected_index = keyToIndex(run_type_.toString());
  
  for (int i = 0; i < n_keys; ++i) {
    if (i == selected_index) {
      *out << option()
	.set("value", (string)config_keys_[i])
	.set("selected", "");
    } else {
      *out << option()
	.set("value", (string)config_keys_[i]);
    }
    *out << (string)config_keys_[i] << option() << endl;
  }
  
  *out << cgicc::select() << endl;
  
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("title", "Configure the selected run.")
    .set("value", "Configure") << endl;
  *out << form() << endl;
  *out << td();

  *out << td() << "OR" << td();
  
  *out << td();
  *out << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/RunSequence") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("title", "Take all calibration runs in an automatic sequence.")
    .set("value", "Run all calibrations") << endl;
  *out << form() << endl;
  *out << td();

  *out << tr() << table();

  /*
   *out << "Run Number: " << endl;
   *out << input().set("type", "text")
   .set("name", "runnumber")
   .set("value", run_number_.toString())
   .set("size", "40") << br() << endl;
   
   *out << "Max # of Events: " << endl;
   *out << input().set("type", "text")
   .set("name", "nevents")
   .set("value", toString(nevents_))
   .set("size", "40") << br() << endl;
  */
  
  // Buttons
  
  *out << table() << tbody() << tr();

  *out << td() << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Start") << endl;
  *out << input().set("type", "submit")
    .set("title", "Start the configured run.")
    .set("name", "command")
    .set("value", "Start") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Stop") << endl;
  *out << input().set("type", "submit")
    .set("title", "Stop the ongoing run.")
    .set("name", "command")
    .set("value", "Stop") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Halt") << endl;
  *out << input().set("type", "submit")
    .set("title", "Halt the configured or ongoing run.")
    .set("name", "command")
    .set("value", "Halt") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Reset") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Reset") << endl;
    *out << form() << td() << endl;
  
  *out << tr() << tbody() << table();
  
  // TTS operation
  if (hide_tts_control_) {
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SwitchTTS") << endl;
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "ShowTTSControl") << endl;
    *out << form() << endl;
  } else {
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SetTTS") << endl;
    
    *out << small() << "(See, "
	 << a().set("href", "https://twiki.cern.ch/twiki/bin/view/CMS/CSCcommissioningFED#FED_Mapping")
	 << "FED commissioning Wiki page"
	 << a()
	 << " to find out sTTS IDs (FMM-ID).)" << small() << br() << endl;
    *out << "sTTS ID: (750-757,760,830-869)" << endl;
    *out << input().set("type", "text")
      .set("name", "tts_id")
				.set("value", tts_id_.toString())
      .set("size", "10") << br() << endl;
    
    *out << "sTTS value: (0-15)" << endl;
    *out << input().set("type", "text")
      .set("name", "tts_bits")
      .set("value", tts_bits_.toString())
      .set("size", "10") << br() << endl;
    
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "SetTTS") << endl;
    *out << form() << endl;
    
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SwitchTTS") << endl;
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "HideTTSControl") << endl;
    *out << form() << endl;
  }
  
  // Error message, if exists.
  if (!error_message_.empty()) {
    *out  << p() << span().set("style", "color: red;")
	  << error_message_ << span() << p() << endl;
    error_message_ = "";
  }
  
  // Configuration parameters
  *out << hr() << endl;

  *out << table(); 
  *out << tr() << td() << "Run type: " << td() << td() << run_type_.toString();
  if ( bool(isInCalibrationSequence_) ){
      *out << " (run " << getCalibParamIndex(run_type_.toString())+1 << " in an automatic sequence of "
	   << calib_params_.size() << " calibration runs)";
  }
  *out << td() << tr() << endl;
  if ( isCalibrationMode() ){
    int index = getCalibParamIndex(run_type_);
    *out << tr() 
	 << td() << "Steps completed: " << td() 
	 << td() << step_counter_ << " of " << calib_params_[index].bag.loop_ << td() 
	 << tr() << endl;
  }
  refreshConfigParameters();
  
  *out << tr() << td() << "TTCci inputs(Clock:Orbit:Trig:BGo): " <<  td() << td() << ttc_source_.toString() << td() << tr() << endl;
  
  *out << tr() << td() << "Mode of DAQManager: " <<  td() << td() << daq_mode_.toString() << td() << tr() << endl;
  string localDAQState = getLocalDAQState();
  *out << tr() << td() << "Local DAQ state: " << td() << td() << span().set("class",localDAQState) << localDAQState << span() << td() << tr() << endl;

  *out << tr() << td() << "State of TF operation " << cite() << TFCellOpName_.toString() << cite() << ": " << td() << td() 
       << span().set("class",TFCellOpState_.toString()) << TFCellOpState_.toString() 
       << span() << td() << tr() << endl;
  *out << table(); 
  
  // Application states
  *out << hr() << endl;
  state_table_.webOutput(out, (string)state_);
  
  // Message logs
  *out << hr() << endl;
  // last_log_.webOutput(out);
  
  *out << body() << html() << endl;
}

void emu::supervisor::Application::webConfigure(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string value;
  isCommandFromWeb_ = true;
  
  value = getCGIParameter(in, "runtype");
  if (value.empty()) { error_message_ += "Please select run type.\n"; }
  run_type_ = value;
  
  run_number_ = 1;
  nevents_ = -1;
  
  if (error_message_.empty()) {
    submit(configure_signature_);
  }
  
  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webStart(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;
  // Book run number here to make sure it's done 
  // only when requested by the user from the web page,
  // and not by the FunctionManager via SOAP.
  bookRunNumber();

  try{
    fireEvent("Start");
  }
  catch( toolbox::fsm::exception::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Start failed", e );
  }
  
  keep_refresh_ = true;
  webRedirect(in, out);
  
  if (!isCalibrationMode()) {
    keep_refresh_ = false;
  }
}

void emu::supervisor::Application::webStop(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;

  try{
    fireEvent("Stop");
  }
  catch( toolbox::fsm::exception::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Stop failed", e );
  }
  
  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webHalt(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;
  quit_calibration_ = true;
  
  try{
    fireEvent("Halt");
  }
  catch( toolbox::fsm::exception::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Halt failed", e );
  }

  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webReset(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;
  resetAction();
  
  webHalt(in, out);
}

void emu::supervisor::Application::webSetTTS(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;
  tts_id_.fromString(getCGIParameter(in, "tts_id"));
  tts_bits_.fromString(getCGIParameter(in, "tts_bits"));
  
  if (error_message_.empty()) {
    try{
      fireEvent("SetTTS");
    }
    catch( toolbox::fsm::exception::Exception& e ){
      XCEPT_RETHROW( xgi::exception::Exception, "SetTTS failed", e );
    }
  }
  
  webRedirect(in, out);
}

void emu::supervisor::Application::webSwitchTTS(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  isCommandFromWeb_ = true;
  hide_tts_control_ = getCGIParameter(in, "command").find("Hide", 0) == 0;
  
  webRedirect(in, out);
}

void emu::supervisor::Application::webRunSequence(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string value;
  isCommandFromWeb_ = true;

  bookRunNumber();
  
  calib_wl_->submit( sequencer_signature_ );
   
  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webRedirect(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string url = in->getenv("PATH_TRANSLATED");
  
  HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location",
		   url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

bool emu::supervisor::Application::configureAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Configure");
  
  return false;
}

bool emu::supervisor::Application::startAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Start");
  
  return false;
}

bool emu::supervisor::Application::stopAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Stop");
  
  return false;
}

bool emu::supervisor::Application::haltAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Halt");
  
  return false;
}

bool emu::supervisor::Application::calibrationAction(toolbox::task::WorkLoop *wl)
{
  LOG4CPLUS_DEBUG(logger_, "calibrationAction " << "(begin)");
  
  unsigned int index = std::max( 0, getCalibParamIndex(run_type_) );

  unsigned int nRuns = ( bool( isInCalibrationSequence_ ) ? calib_params_.size() : 1 );
  unsigned int iRun  = ( bool( isInCalibrationSequence_ ) ? index                : 0 );

  LOG4CPLUS_DEBUG( logger_, "Calibration" << endl
		   << "command: " << calib_params_[index].bag.command_.toString()       << endl
		   << "loop: "    << calib_params_[index].bag.loop_.toString()	        << endl
		   << "delay: "   << calib_params_[index].bag.delay_.toString()	        << endl
		   << "ltc: "     << endl << calib_params_[index].bag.ltc_.toString()   << endl
		   << "ttcci: "   << endl << calib_params_[index].bag.ttcci_.toString() << endl );

  emu::soap::Messenger m( this );

  for (step_counter_ = 0; step_counter_ < calib_params_[index].bag.loop_; ++step_counter_) {
    if (quit_calibration_) { break; }
    LOG4CPLUS_DEBUG(logger_, "calibrationAction: " << step_counter_);
    
    m.sendCommand( "emu::pc::EmuPeripheralCrateManager", calib_params_[index].bag.command_.toString() );

    xdata::String attributeValue( "Start" );
    m.sendCommand( "ttc::LTCControl", "Cyclic", emu::soap::Parameters::none, emu::soap::Attributes().add( "Param", &attributeValue ) );
    sendCalibrationStatus( iRun, nRuns, step_counter_, calib_params_[index].bag.loop_ );

    sleep( calib_params_[index].bag.delay_ );
  }
  
  sendCalibrationStatus( ( iRun+1 == nRuns ? nRuns : iRun ), nRuns, step_counter_, calib_params_[index].bag.loop_ );
  
  if (!quit_calibration_) {
    submit(halt_signature_);
  }

  LOG4CPLUS_DEBUG(logger_, "calibrationAction " << "(end)");

  return false;
}

bool emu::supervisor::Application::calibrationSequencer(toolbox::task::WorkLoop *wl)
{
  // Do all calibrations in one go.
  LOG4CPLUS_DEBUG(logger_, "calibrationSequencer " << "(begin)");
  isInCalibrationSequence_ = true;
  for ( size_t i=0; i<calib_params_.size() && fsm_.getCurrentState() != 'F'; ++i ){
    run_type_ = calib_params_[i].bag.key_;
    if ( !quit_calibration_ ){
      submit(configure_signature_);
      if ( waitForAppsToReach("Configured",60) ){
	if ( fsm_.getCurrentState() != 'F' ) submit(start_signature_); // This is supposed to halt itself when done.
      }
      else{
	if ( fsm_.getCurrentState() != 'F' ) submit(halt_signature_);
      }
      waitForAppsToReach("Halted");
    }
  }
  isInCalibrationSequence_ = false;
  // Keep refreshing the web page so that it can be seen that the sequence has finished.
  keep_refresh_ = true;
  LOG4CPLUS_DEBUG(logger_, "calibrationSequencer " << "(end)");
  return false;
}

void emu::supervisor::Application::sendCalibrationStatus( unsigned int iRun, unsigned int nRuns, unsigned int iStep, unsigned int nSteps ){
  emu::soap::Messenger m(this);
  
  xdata::UnsignedInteger calibNRuns    ( nRuns  );
  xdata::UnsignedInteger calibNSteps   ( nSteps );
  xdata::UnsignedInteger calibRunIndex ( iRun   );
  xdata::UnsignedInteger calibStepIndex( iStep  );

  try{
    if ( daq_descr_ != NULL ) daq_descr_ = m.getAppDescriptor( "emu::daq::manager::Application", 0 );
    m.setParameters( daq_descr_, 
		     emu::soap::Parameters()
		     .add( "calibNRuns"    , &calibNRuns     )
		     .add( "calibNSteps"   , &calibNSteps    )
		     .add( "calibRunIndex" , &calibRunIndex  )
		     .add( "calibStepIndex", &calibStepIndex ) );
  }
  catch( xcept::Exception &e ){
    LOG4CPLUS_WARN( logger_, "Failed to send calibration status to emu::daq::manager::Application : " << xcept::stdformat_exception_history(e) );
  }

}

void emu::supervisor::Application::configureAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  LOG4CPLUS_DEBUG(logger_, "runtype: " << run_type_.toString()
		  << " runnumber: " << run_number_ << " nevents: " << nevents_.toString());
  

  rcmsStateNotifier_.findRcmsStateListener();      	
  step_counter_ = 0;

  emu::soap::Messenger m( this );

  try {

    //
    // Clean up leftover ops and halt apps
    //
    
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      TFCellOpState_ = OpGetStateCell();
      if ( TFCellOpState_.toString() != "UNKNOWN" ){
	// Reset csctf-cell operation
	OpResetCell();
	waitForTFCellOpToReach("halted",60);
      }
    }

    try {
      state_table_.refresh();
      if (state_table_.getState("emu::daq::manager::Application", 0) != "Halted") {
	if ( isDAQManagerControlled("Halt") ){
	  m.sendCommand( "emu::daq::manager::Application", 0, "Halt" );
	  waitForDAQToExecute("Halt", 10);
	}
      }
       
      if (state_table_.getState("ttc::TTCciControl", 0) != "halted") {
	m.sendCommand( "ttc::TTCciControl", "reset" );
      }
      if (state_table_.getState("ttc::LTCControl", 0) != "halted") {
	m.sendCommand( "ttc::LTCControl", "reset" );
	// Allow ttc::LTCControl some time to halt:
	::sleep(2);
      }
    } catch (xcept::Exception ignored) {}
    

    //
    // Configure
    //

    // Tell DAQ Manager the run type, number of events, whether to write bad events only 
    // and whether global DAQ is running the show.
    // Keep the two parameters isGlobalInControl and writeBadEventsOnly separate 
    // as later we may need to write all events even when global DAQ is in control
    // (in centrally started calibration runs).
    xdata::Boolean isGlobalInControl( true );
    if ( isCalibrationMode() || bool( controlTFCellOp_ ) ) isGlobalInControl = false;
    try {
      LOG4CPLUS_INFO( logger_, "Sending to emu::daq::manager::Application : maxNumberOfEvents " << nevents_ .toString() 
		      << ", runType " << run_type_.toString()
		      << ", isGlobalInControl " << isGlobalInControl.toString()
		      << ", writeBadEventsOnly " << isGlobalInControl.toString() );
      m.setParameters( "emu::daq::manager::Application", 
		       emu::soap::Parameters()
		       .add( "maxNumberOfEvents" , &nevents_          )
		       .add( "runType"           , &run_type_         )
		       .add( "isGlobalInControl" , &isGlobalInControl )
		       .add( "writeBadEventsOnly", &isGlobalInControl )
		       );
    } catch (xcept::Exception& e) {
      LOG4CPLUS_WARN( logger_, "Failed to send to emu::daq::manager::Application : maxNumberOfEvents " << nevents_ .toString() 
		      << ", runType " << run_type_.toString()
		      << ", isGlobalInControl " << isGlobalInControl.toString() 
		      << ", writeBadEventsOnly " << isGlobalInControl.toString() 
		      << ": " << xcept::stdformat_exception_history(e) );
    }

    
    // Configure local DAQ first as its FSM is driven asynchronously,
    // and it will probably finish the transition by the time the others do.
    try {
      if ( isDAQManagerControlled("Configure") ) m.sendCommand( "emu::daq::manager::Application", 0, "Configure" );
    } catch (xcept::Exception ignored) {}
    
    if (!isCalibrationMode()) {
      m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "Configure" );
    }
       
    // Configure TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      if ( waitForTFCellOpToReach("halted",60) ){
	sendCommandCell("configure");
	// Allow more time for 'configure' after key change. With a new key, it may take a couple of minutes.
	waitForTFCellOpToReach("configured",180);
      }
      if ( TFCellOpState_.toString() != "configured" ){
	stringstream ss;
	ss << "TF Cell Operation \"" << TFCellOpName_.toString() 
	   << "\" failed to reach configured state. Aborting.";
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss.str() );
	this->notifyQualified( "error", eObj );
	throw eObj;
      } 
    }

    // Configure TTC
    int index = getCalibParamIndex(run_type_);
    if (index >= 0) {
      m.setParameters( "ttc::TTCciControl" , emu::soap::Parameters().add( "Configuration", &calib_params_[index].bag.ttcci_ ) );
    }
    m.sendCommand( "ttc::TTCciControl", "configure" );    
    
    // Configure LTC
    if (index >= 0) {
      m.setParameters( "ttc::LTCControl" , emu::soap::Parameters().add( "Configuration", &calib_params_[index].bag.ltc_ ) );
    }
    m.sendCommand( "ttc::LTCControl", "configure" );

    xdata::String runType( "global" );
    if      ( isCalibrationMode()     ) runType = "calibration";
    else if ( controlTFCellOp_.value_ ) runType = "local";
    m.setParameters( "emu::fed::Manager", emu::soap::Parameters().add( "runType", &runType ) );
    // Configure FED
    m.sendCommand( "emu::fed::Manager", "Configure" );

    if (isCalibrationMode()) {
      m.setResponseTimeout( 600 ); // Allow PCrates ample time to be configured for calibration run.
		if (isAlctCalibrationMode())
		  m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "ConfigCalALCT" );
		else
		  m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "ConfigCalCFEB");
      m.resetResponseTimeout(); // Reset response timeout to default value.
    }   

    // By now the local DAQ must have finished configuring. Checking it is practically only needed
    // in tests when the local DAQ Manager is the only supervised app. We certainly don't need to do it
    // in global runs.
    try{
      if ( runType != "global" ) 
	if ( isDAQManagerControlled("Configure") ) waitForDAQToExecute("Configure", 5, true);
    } catch (xcept::Exception ignored) {}


    state_table_.refresh();
    if (!state_table_.isValidState("Configured")) {
      stringstream ss;
      ss << state_table_;
      XCEPT_RAISE(xcept::Exception,
		  "Applications got to unexpected states: "+ss.str() );
    }
    refreshConfigParameters();
    
  } catch (xoap::exception::Exception e) {
    LOG4CPLUS_ERROR(logger_,
		    "Exception in " << evt->type() << ": " << e.what());
    stringstream ss0;
    ss0 << 
		    "Exception in " << evt->type() << ": " << e.what();
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss0.str() );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    LOG4CPLUS_ERROR(logger_,
		    "Exception in " << evt->type() << ": " << e.what());
    stringstream ss1;
    ss1 << 
		    "Exception in " << evt->type() << ": " << e.what();
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss1.str() );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to configure", e);
	}
  
  state_table_.refresh();
  LOG4CPLUS_DEBUG(logger_,  "Current state is: [" << fsm_.getStateName (fsm_.getCurrentState()) << "]");
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end of this function)");
}

void emu::supervisor::Application::startAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  LOG4CPLUS_DEBUG(logger_, "runtype: " << run_type_.toString()
		  << " runnumber: " << run_number_ << " nevents: " << nevents_.toString());
  
  emu::soap::Messenger m( this );

  try {
    state_table_.refresh();

    m.setParameters( "emu::fed::Manager", emu::soap::Parameters().add( "runNumber", &run_number_ ) );
    m.sendCommand( "emu::fed::Manager", "Enable" );
    
    if (!isCalibrationMode()) {
      m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "Enable" );
    }
    
    try {
      if (state_table_.getState("emu::daq::manager::Application", 0) == "Halted" &&
        isDAQManagerControlled("Configure")                                        ) {
	m.setParameters( "emu::daq::manager::Application", emu::soap::Parameters().add( "maxNumberOfEvents", &nevents_ ) );
	m.sendCommand( "emu::daq::manager::Application", 0, "Configure" );
	if ( isCommandFromWeb_ ) waitForDAQToExecute("Configure", 60, true);
	else                     waitForDAQToExecute("Configure", 2);
      }
      if ( isDAQManagerControlled("Enable") ) {
	m.setParameters( "emu::daq::manager::Application", emu::soap::Parameters().add( "runNumber", &run_number_ ) );
	m.sendCommand( "emu::daq::manager::Application", 0, "Enable" );
	if ( isCommandFromWeb_ ) waitForDAQToExecute("Enable", 60, true);
	else                     waitForDAQToExecute("Enable", 2);
      }
    } catch (xcept::Exception ignored) {}
    
    state_table_.refresh();
    
    if (state_table_.getState("ttc::TTCciControl", 0) != "enabled") {
      m.sendCommand( "ttc::TTCciControl", "enable" );
    }
    if (state_table_.getState("ttc::LTCControl", 0) != "enabled") {
      m.sendCommand( "ttc::LTCControl", "enable" );
    }
    xdata::String attributeValue( "Stop" );
    m.sendCommand( "ttc::LTCControl", "Cyclic", emu::soap::Parameters::none, emu::soap::Attributes().add( "Param", &attributeValue ) );

    // Enable TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("enable");
      waitForTFCellOpToReach("enabled",10);
    }

    refreshConfigParameters();
    
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
    
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
    
  }
  
  if (isCalibrationMode()) {
    quit_calibration_ = false;
    submit(calibration_signature_);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::stopAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  emu::soap::Messenger m( this );

  try {
    emu::base::Stopwatch sw;
    sw.start();
    state_table_.refresh();
    cout << "Timing in stopAction: " << endl
	 << "    state table: " << state_table_
	 << "    state_table_.refresh: " << sw.read() << endl;

    // Stop TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("stop");
      waitForTFCellOpToReach("configured",60);
      cout << "    stop TFCellOp: " << sw.read() << endl;
    }

    if (state_table_.getState("ttc::LTCControl", 0) != "halted") {
      m.sendCommand( "ttc::LTCControl", "reset" );
      cout << "    Halt (reset) ttc::LTCControl: " << sw.read() << endl;
    }
    if (state_table_.getState("ttc::TTCciControl", 0) != "halted") {
      m.sendCommand( "ttc::TTCciControl", "reset" );
      cout << "    Halt (reset) ttc::TTCciControl: " << sw.read() << endl;
    }
        
    try {
      if ( isDAQManagerControlled("Halt") ) m.sendCommand( "emu::daq::manager::Application", 0, "Halt" );
      if ( isCommandFromWeb_ ) waitForDAQToExecute("Halt", 60, true);
      else                     waitForDAQToExecute("Halt", 3);
    } catch (xcept::Exception ignored) {}
    cout << "    Halt emu::daq::manager::Application: " << sw.read() << endl;

    m.sendCommand( "emu::fed::Manager", "Disable" );
    cout << "    Disable emu::fed::Manager: " << sw.read() << endl;
    m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "Disable" );
    cout << "    Disable emu::pc::EmuPeripheralCrateManager: " << sw.read() << endl;
    m.sendCommand( "ttc::TTCciControl", "configure" );
    cout << "    Configure TTCci: " << sw.read() << endl;
    m.sendCommand( "ttc::LTCControl", "configure" );
    cout << "    Configure LTC: " << sw.read() << endl;

    writeRunInfo( isCommandFromWeb_ ); // only write runinfo if Stop was issued from the web interface
    if ( isCommandFromWeb_ ) cout << "    Write run info: " << sw.read() << endl;
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::haltAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  emu::soap::Messenger m( this );

  try {
    emu::base::Stopwatch sw;
    sw.start();
    state_table_.refresh();
    cout << "Timing in haltAction: " << endl
	 << "    state table: " << state_table_
	 << "    state_table_.refresh: " << sw.read() << endl;
    
    // Stop and reset TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("stop");
      waitForTFCellOpToReach("configured",60);
      cout << "    stop TFCellOp: " << sw.read() << endl;
      OpResetCell();
      cout << "    reset TFCellOp: " << sw.read() << endl;
    }

    if (state_table_.getState("ttc::LTCControl", 0) != "halted") {
      m.sendCommand( "ttc::LTCControl", "reset" );
      cout << "    Halt (reset) ttc::LTCControl: " << sw.read() << endl;
    }

    if (state_table_.getState("ttc::TTCciControl", 0) != "halted") {
      m.sendCommand( "ttc::TTCciControl", "reset" );
      cout << "    Halt (reset) ttc::TTCciControl: " << sw.read() << endl;
    }

    m.sendCommand( "emu::fed::Manager", "Halt" );
    cout << "    Halt emu::fed::Manager: " << sw.read() << endl;

    m.sendCommand( "emu::pc::EmuPeripheralCrateManager", "Halt" );
    cout << "    Halt emu::pc::EmuPeripheralCrateManager: " << sw.read() << endl;
    
    try {
      if ( isDAQManagerControlled("Halt") ){
	m.sendCommand( "emu::daq::manager::Application", 0, "Halt" );
	if ( isCommandFromWeb_ ) waitForDAQToExecute("Halt", 60, true);
	else                     waitForDAQToExecute("Halt", 3);
      }
    } catch (xcept::Exception ignored) {}
    cout << "    Halt emu::daq::manager::Application: " << sw.read() << endl;

    // Issue a resync now to make sure L1A is reset to zero in the FEDs in case a global run follows.
    // In a global run, when backpressure is not ignored, this would fail (see http://cmsonline.cern.ch/cms-elog/756961).
    // By resynching through LTC, we make sure it's only done in local runs. 
    // The following command will do nothing if no ttc::LTCControl application is found.
    xdata::String attributeValue( "resync" );
    m.sendCommand( "ttc::LTCControl", "ExecuteSequence", emu::soap::Parameters::none, emu::soap::Attributes().add( "Param", &attributeValue ) );

    writeRunInfo( isCommandFromWeb_ ); // only write runinfo if Halt was issued from the web interface
    if ( isCommandFromWeb_ ) cout << "    Write run info: " << sw.read() << endl;

  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::resetAction() throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, "reset(begin)");
  
  fsm_.reset();
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  
  LOG4CPLUS_DEBUG(logger_, "reset(end)");
}

void emu::supervisor::Application::setTTSAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  emu::soap::Messenger m( this );
  
  try {
    m.setParameters( "emu::fed::Manager", emu::soap::Parameters().add( "ttsID", &tts_id_ ).add( "ttsBits", &tts_bits_ ) );
    m.sendCommand( "emu::fed::Manager", 0, "SetTTSBits" );
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::noAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  stringstream ss;
  ss << evt->type() 
     << " attempted when in " 
     << fsm_.getStateName(fsm_.getCurrentState())
     << " state. Command ignored.";
  LOG4CPLUS_WARN(logger_, ss.str());
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss.str() );
  this->notifyQualified( "warning", eObj );
}

void emu::supervisor::Application::submit(toolbox::task::ActionSignature *signature)
{
  wl_->submit(signature);
}

void emu::supervisor::Application::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception)
{
  keep_refresh_ = false;
  
  LOG4CPLUS_DEBUG(getApplicationLogger(),"Current state is: [" << fsm.getStateName (fsm.getCurrentState()) << "]");
  // Send notification to Run Control
  state_=fsm.getStateName (fsm.getCurrentState());
  try
    {
      LOG4CPLUS_DEBUG(getApplicationLogger(),"Sending state changed notification to Run Control.");
      rcmsStateNotifier_.stateChanged((std::string)state_,"");
    }
  catch(xcept::Exception &e)
    {
      LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to notify state change to Run Control."
		      << xcept::stdformat_exception_history(e));
      stringstream ss2;
      ss2 << "Failed to notify state change to Run Control.";
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss2.str(), e );
      this->notifyQualified( "error", eObj );
      std::cout << "rcmsFailed to notify state change to Run Control:" << std::endl;
    }
  
  emu::base::Supervised::stateChanged(fsm);
}

void emu::supervisor::Application::transitionFailed(toolbox::Event::Reference event)
  throw (toolbox::fsm::exception::Exception)
{
  keep_refresh_ = false;
  toolbox::fsm::FailedEvent &failed = dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
  
  stringstream reason;
  reason << "<![CDATA[" 
	 << endl
	 << "Failure occurred when performing transition"
	 << " from "        << failed.getFromState()
	 << " to "          << failed.getToState()
	 << ". Exception: " << xcept::stdformat_exception_history( failed.getException() )  
	 << endl
	 << "]]>";
  
  reasonForFailure_ = reason.str();
  LOG4CPLUS_ERROR(getApplicationLogger(), reason.str());

  // Send notification to Run Control
  try {
    LOG4CPLUS_DEBUG(getApplicationLogger(),"Sending state changed notification to Run Control.");
    rcmsStateNotifier_.stateChanged("Error",xcept::stdformat_exception_history(failed.getException()));
  } catch(xcept::Exception &e) {
    LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to notify state change to Run Control : "
		    << xcept::stdformat_exception_history(e));
    stringstream ss3;
    ss3 << "Failed to notify state change to Run Control : ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss3.str(), e );
    this->notifyQualified( "error", eObj );
  }
  
}

//////////////////////////////////////////////////////////////////////


void emu::supervisor::Application::sendCommandCell(string command){
  if ( tf_descr_ == NULL ) return;
  emu::soap::Messenger m( this );

  xdata::String async( "false" ); 
  xdata::String cid( "10" );
  xdata::String sid( "73" ); 
  xdata::String paramName( "KEY" );
  xdata::String commandName( command );

  try{
    emu::soap::Attributes paramAttr;
    paramAttr.setUsePrefixOfParent( false ).add( "name", &paramName );
    m.sendCommand( tf_descr_, 
		   emu::soap::QualifiedName( "OpSendCommand", "urn:ts-soap:3.0", "ts-soap" ),
		   emu::soap::Parameters()
		   .add( "operation", &TFCellOpName_ )
		   .add( "command"  , &commandName   )
		   .add( "param"    , &tf_key_       , &paramAttr ),
		   emu::soap::Attributes()
		   .setUsePrefixOfParent( false )
		   .add( "async", &async )
		   .add( "cid"  , &cid   )
		   .add( "sid"  , &sid   )
		   );
  } 
  catch( xcept::Exception& e ){
    LOG4CPLUS_ERROR( getApplicationLogger(), "Failed to send command '" << command << "' to TF Cell " << xcept::stdformat_exception_history(e) );
  }
}
 

std::string emu::supervisor::Application::OpGetStateCell(){
  if ( tf_descr_ == NULL ) return "";

  emu::soap::Messenger m( this );

  xdata::String async( "false" ); 
  xdata::String cid( "10" );
  xdata::String sid( "73" ); 

  try{
    xdata::String state;
    emu::soap::extractParameters( m.sendCommand( tf_descr_, 
						 emu::soap::QualifiedName( "OpGetState", "urn:ts-soap:3.0", "ts-soap" ),
						 emu::soap::Parameters()
						 .add( "operation", &TFCellOpName_ ),
						 emu::soap::Attributes()
						 .setUsePrefixOfParent( false )
						 .add( "async", &async )
						 .add( "cid"  , &cid   )
						 .add( "sid"  , &sid   )
						 ),
				  emu::soap::Parameters().add( emu::soap::QualifiedName( "payload", "urn:ts-soap:3.0" ), &state )
				  );
    return ( state == "" ? "UNKNOWN" : state );
  } 
  catch( xcept::Exception& e ){
    LOG4CPLUS_ERROR( getApplicationLogger(), "Failed to get state TF Cell Operation " << xcept::stdformat_exception_history(e) );
  }
  return "UNKNOWN";
}


void emu::supervisor::Application::OpResetCell(){
  if ( tf_descr_ == NULL ) return;

  emu::soap::Messenger m( this );

  xdata::String async( "false" ); 
  xdata::String cid( "10" );
  xdata::String sid( "73" );

  try{
    m.sendCommand( tf_descr_, 
		   emu::soap::QualifiedName( "OpReset", "urn:ts-soap:3.0", "ts-soap" ),
		   emu::soap::Parameters()
		   .add( "operation", &TFCellOpName_ ),
		   emu::soap::Attributes()
		   .setUsePrefixOfParent( false )
		   .add( "async", &async )
		   .add( "cid"  , &cid   )
		   .add( "sid"  , &sid   )
		   );
  } 
  catch( xcept::Exception& e ){
    LOG4CPLUS_ERROR( getApplicationLogger(), "Failed to reset TF Cell Operation " << xcept::stdformat_exception_history(e) );
  }
}

//////////////////////////////////////////////////////////////////////


bool emu::supervisor::Application::waitForTFCellOpToReach( const string targetState, const unsigned int seconds ){
  if ( tf_descr_ == NULL ) return false;

  // Poll, and return TRUE if and only if TF Cell Operation gets into the expected state before timeout.
  for ( unsigned int i=0; i<=seconds; ++i ){
    TFCellOpState_ = OpGetStateCell();
    if ( TFCellOpState_.toString() == targetState ){ return true; }
    LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for TF Cell Operation " 
		    << TFCellOpName_.toString() << " to get " << targetState 
		    << ". It is still in " << TFCellOpState_.toString() << " state." );
    ::sleep(1);
  }

  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec so far for TF Cell Operation " 
		   << TFCellOpName_.toString() << " to get " << targetState 
		   << ". It is still in " << TFCellOpState_.toString() << " state." );

  stringstream ss10;
  ss10 <<  "Timeout after waiting " << seconds << " sec so far for TF Cell Operation "
       << TFCellOpName_.toString() << " to get "<< targetState 
       << ". It is still in " << TFCellOpState_.toString() << " state.";
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss10.str() );
  this->notifyQualified( "error", eObj );
  return false;
}


void emu::supervisor::Application::refreshConfigParameters()
{
	daq_mode_ = getDAQMode();
	TFCellOpState_ = OpGetStateCell();
	ttc_source_ = getTTCciSource();
}

string emu::supervisor::Application::getCGIParameter(xgi::Input *in, string name)
{
	cgicc::Cgicc cgi(in);
	string value;

	form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}

int emu::supervisor::Application::keyToIndex(const string key)
{
	int index = -1;

	for (unsigned int i = 0; i < config_keys_.size(); ++i) {
		if (config_keys_[i] == key) {
			index = i;
			break;
		}
	}

	return index;
}


bool emu::supervisor::Application::isCalibrationMode()
{
	return (getCalibParamIndex(run_type_) >= 0);
}

bool emu::supervisor::Application::isAlctCalibrationMode()
{
	std::cout << "isAlctCalibMode: runtype: " << run_type_.toString() << "index" << getCalibParamIndex(run_type_);
	bool res = run_type_.toString().find("ALCT") != string::npos;
	std::cout << "isAlctCalibMode result: " << res << std::endl;
	return res;
}

int emu::supervisor::Application::getCalibParamIndex(const string name)
{
	int result = -1;

	for (size_t i = 0; i < calib_params_.size(); ++i) {
		if (calib_params_[i].bag.key_ == name) {
			result = i;
			break;
		}
	}

	return result;
}


string emu::supervisor::Application::getDAQMode(){
  string result("");

  emu::soap::Messenger m( this );

  xdata::Boolean daqMode( false );
  try{
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "supervisedMode", &daqMode ) );
    result = ( bool( daqMode ) ? "supervised" : "unsupervised" );
    REVOKE_ALARM( "noLocalDAQ", NULL );
  } catch (xcept::Exception e) {
    LOG4CPLUS_INFO(logger_, "Failed to get local DAQ state. "
		   << xcept::stdformat_exception_history(e));
    RAISE_ALARM( emu::supervisor::alarm::NoLocalDAQ, "noLocalDAQ", "warn", "Local DAQ is in down or inaccessible.", "", &logger_ );
    result = "UNKNOWN";
  }

  return result;
}

string emu::supervisor::Application::getLocalDAQState(){

  emu::soap::Messenger m( this );

  xdata::String daqState( "UNKNOWN" );
  try{
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "daqState", &daqState ) );
    REVOKE_ALARM( "noLocalDAQ", NULL );
  } catch (xcept::Exception e) {
    LOG4CPLUS_INFO(logger_, "Failed to get local DAQ state. "
		   << xcept::stdformat_exception_history(e));
    RAISE_ALARM( emu::supervisor::alarm::NoLocalDAQ, "noLocalDAQ", "warn", "Local DAQ is in down or inaccessible.", "", &logger_ );
  }

  return daqState.toString();
}

string emu::supervisor::Application::getTTCciSource(){
  string result( "" );
  if ( ttc_descr_ == NULL ) return result;

  emu::soap::Messenger m( this );

  xdata::String ClockSource   = "UNKNOWN";
  xdata::String OrbitSource   = "UNKNOWN";
  xdata::String TriggerSource = "UNKNOWN";
  xdata::String BGOSource     = "UNKNOWN";

  try{
    m.getParameters( ttc_descr_,
		     emu::soap::Parameters()
		     .add( "ClockSource"  , &ClockSource   ) 
		     .add( "OrbitSource"  , &OrbitSource   ) 
		     .add( "TriggerSource", &TriggerSource ) 
		     .add( "BGOSource"    , &BGOSource     ) );
    result  =       ClockSource.toString();
    result += ":" + OrbitSource.toString();
    result += ":" + TriggerSource.toString();
    result += ":" + BGOSource.toString();
  }
  catch(xcept::Exception e){
    result = "UNKNOWN";
  }

  return result;
}


bool emu::supervisor::Application::waitForDAQToExecute( const string command, const unsigned int seconds, const bool poll ){
  string expectedState;
  if      ( command == "Configure" ){ expectedState = "Ready";   }
  else if ( command == "Enable"    ){ expectedState = "Enabled"; }
  else if ( command == "Halt"      ){ expectedState = "Halted";  }
  else                              { return true; }

  // If not polling (default), just wait and return TRUE:
  if ( !poll ){
    ::sleep( seconds );
    return true;
  }

  // Poll, and return TRUE if and only if DAQ gets into the expected state before timeout.
  emu::soap::Messenger m( this );
  xdata::String  daqState;
  for ( unsigned int i=0; i<=seconds; ++i ){
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "daqState", &daqState ) );
    if ( daqState.toString() != "Halted"  && daqState.toString() != "Ready" && 
	 daqState.toString() != "Enabled" && daqState.toString() != "INDEFINITE" ){
      LOG4CPLUS_ERROR( logger_, "Local DAQ is in " << daqState.toString() << " state. Please destroy and recreate local DAQ." );
      stringstream ss9;
      ss9 <<  "Local DAQ is in " << daqState.toString() << " state. Please destroy and recreate local DAQ." ;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss9.str() );
      this->notifyQualified( "error", eObj );
      return false;
    }
    if ( daqState.toString() == expectedState ){ return true; }
    LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for local DAQ to get " 
		    << expectedState << ". It is still in " << daqState.toString() << " state." );
    ::sleep(1);
  }

  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		   << ". It is in " << daqState.toString() << " state." );
  stringstream ss10;
  ss10 <<  "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		   << ". It is in " << daqState.toString() << " state." ;
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss10.str() );
  this->notifyQualified( "error", eObj );
  return false;
}

bool emu::supervisor::Application::isDAQManagerControlled(string command)
{
  emu::soap::Messenger m( this );
  xdata::Boolean supervisedMode;
  // xdata::Boolean configuredInSupervisedMode;
  xdata::String  daqState;
  try {
    m.getParameters( "emu::daq::manager::Application", 0,
		     emu::soap::Parameters()
		     .add( "supervisedMode"            , &supervisedMode             )
		     // .add( "configuredInSupervisedMode", &configuredInSupervisedMode )
		     .add( "daqState"                  , &daqState                   ) );
  }
  catch (xcept::Exception &ignored){
    return false;
  }

  // No point in sending any command when DAQ is in an irregular state (failed, indefinite, ...)
  if ( daqState.toString() != "Halted"  && daqState.toString() != "Ready" && 
       daqState.toString() != "Enabled" && daqState.toString() != "INDEFINITE" ){
    LOG4CPLUS_WARN( logger_, "No command \"" << command << "\" sent to emu::daq::manager::Application because local DAQ is in " 
		    << daqState.toString() << " state. Please destroy and recreate local DAQ." );
    stringstream ss11;
    ss11 <<  "No command \"" << command << "\" sent to emu::daq::manager::Application because local DAQ is in " 
	 << daqState.toString() << " state. Please destroy and recreate local DAQ." ;
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss11.str() );
    this->notifyQualified( "warn", eObj );
    return false;
  }

  // Don't send any other command when DAQ is in unsupervised mode.
  if ( ! bool( supervisedMode ) ) { return false; }
  
  // And don't send any other command when DAQ was configured in unsupervised mode, either.
  // if ( command != "Configure" && !bool( configuredInSupervisedMode ) ) { return false; }

  return true;

}

bool emu::supervisor::Application::waitForAppsToReach( const string targetState, const int seconds ){
  // If seconds is negative, no timeout.
  for ( int i=0; i<=seconds || seconds<0; ++i ){
    state_table_.refresh( false ); // Do not force refresh, we're not in a hurry. We'll refresh soon anyway.
    // Both the supervised apps and the Supervisor app itself should be in the target state:
    if ( state_table_.isValidState( targetState )
	 &&
	 fsm_.getStateName(fsm_.getCurrentState()) == targetState ) return true;
    LOG4CPLUS_DEBUG( logger_, "Waited " << i << " sec so far for applications to get '" << targetState
		    << "'. Their current states are:" << state_table_ );
    if ( fsm_.getCurrentState() == 'F' ) return false; // Abort if in Failed state.
    ::sleep(1);
  }
  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for applications to get " << targetState );
  stringstream ss;
  ss <<  "Timeout after waiting " << seconds << " sec for applications to get " << targetState;
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss.str() );
  this->notifyQualified( "error", eObj );
  return false;
}

emu::supervisor::Application::StateTable::StateTable(emu::supervisor::Application *sv) 
  : app_(sv)
  , bSem_( toolbox::BSem::FULL )
  , lastRefreshTime_(0){}

void emu::supervisor::Application::StateTable::addApplication(string klass)
{

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = app_->getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
                bSem_.give();
		return; // Do nothing if the target doesn't exist
	}

	// add to the table
        bSem_.take();
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		table_.push_back(
				pair<xdaq::ApplicationDescriptor *, string>(*i, "NULL"));
	}
        bSem_.give();
}

void emu::supervisor::Application::StateTable::refresh( bool forceRefresh )
{
        // Limit refresh rate to 1/2 Hz if forceRefresh is false. (forceRefresh is true by default).
        time_t timeNow;
	time( &timeNow );
	if ( timeNow < lastRefreshTime_ + 2 && !forceRefresh ) return;

	string klass = "";

	emu::soap::Messenger m( app_ );

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {

	        klass = i->first->getClassName();

		try {
			xdata::String state;
			m.getParameters( i->first, emu::soap::Parameters().add( "stateName", &state ) );

                        bSem_.take();
			i->second = state.toString();
			lastRefreshTime_ = timeNow;
                        bSem_.give();
		} catch (xcept::Exception &e) {
			i->second = STATE_UNKNOWN;
			LOG4CPLUS_ERROR(app_->logger_, "Exception when trying to get state of "
					<< klass << ": " << xcept::stdformat_exception_history(e));
			stringstream ss12;
			ss12 << "Exception when trying to get state of "
			     << klass << ": " ;
			XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss12.str(), e );
			app_->notifyQualified( "error", eObj );
                        bSem_.give();
		} catch (...) {
			LOG4CPLUS_ERROR(app_->logger_, "Unknown exception when trying to get state of " << klass);
			stringstream ss13;
			ss13 << "Unknown exception when trying to get state of " << klass;
			XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss13.str() );
			app_->notifyQualified( "error", eObj );
			i->second = STATE_UNKNOWN;
                        bSem_.give();
		}

		if (klass == "emu::daq::manager::Application" && i->second == STATE_UNKNOWN) {
			LOG4CPLUS_WARN(app_->logger_, "State of emu::daq::manager::Application will be unknown.");
			stringstream ss14;
			ss14 << "State of emu::daq::manager::Application will be unknown.";
			XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss14.str() );
			app_->notifyQualified( "warn", eObj );
		}
	}

}

string emu::supervisor::Application::StateTable::getState(string klass, unsigned int instance) const
{
	string state = "";

	vector<pair<xdaq::ApplicationDescriptor *, string> >::const_iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass == i->first->getClassName()
				&& instance == i->first->getInstance()) {
			state = i->second;
			break;
		}
	}

	return state;
}

bool emu::supervisor::Application::StateTable::isValidState(string expected) const
{
	bool is_valid = true;

	vector<pair<xdaq::ApplicationDescriptor *, string> >::const_iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		string checked = expected;
		string klass = i->first->getClassName();

		// Ignore emu::daq::manager::Application in global runs.
		// We know we're in a global run if controlTFCellOp_ is false,
		// i.e., we're not in control of the TF Cell, and the run type is "Monitor".
		if ( klass == "emu::daq::manager::Application" 
		     && !app_->controlTFCellOp_.value_ 
		     && app_->run_type_ == "Monitor" ) continue;

		// TTC/LTC have their own peculiar state names. Translate them:
		if (klass == "ttc::TTCciControl" || klass == "ttc::LTCControl") {
			if (expected == "Halted"    ) { checked = "halted";     }
			if (expected == "Configured") { checked = "configured"; }
			if (expected == "Enabled"   ) { checked = "enabled";    }
		}

		if (i->second != checked) {
			is_valid = false;
			break;
		}
	}

	return is_valid;
}

void emu::supervisor::Application::StateTable::webOutput(xgi::Output *out, string sv_state)
		throw (xgi::exception::Exception)
{
	refresh();
	*out << table() << tbody() << endl;

	// My state
	*out << tr();
	*out << td() << "Application" << "(" << "0" << ")" << td();
	*out << td().set("class", sv_state) << sv_state << td();
	*out << tr() << endl;

	// Applications
	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		string klass = i->first->getClassName();
		int instance = i->first->getInstance();
		string state = i->second;

		*out << tr();
		*out << td() << klass << "(" << instance << ")" << td();
		*out << td().set("class", state) << state << td();
		*out << tr() << endl;
	}

	*out << tbody() << table() << endl;
}

ostream& emu::supervisor::operator<<( ostream& os, const emu::supervisor::Application::StateTable& st ){
  os << endl << "emu::supervisor::Application(0) " << st.getApplication()->getFSM()->getCurrentState() << endl;
  for (vector<pair<xdaq::ApplicationDescriptor *, string> >::const_iterator i = st.getTable()->begin(); i != st.getTable()->end(); ++i) {
    os << i->first->getClassName() << "(" << i->first->getInstance() << ") " << i->second << endl;
  }
  return os;
}

void emu::supervisor::Application::LastLog::size(unsigned int size)
{
	size_ = size;
}

unsigned int emu::supervisor::Application::LastLog::size() const
{
	return size_;
}

void emu::supervisor::Application::LastLog::add(string message)
{
	messages_.push_back(getTime() + " " + message);

	if (messages_.size() > size_) { messages_.pop_front(); }
}

string emu::supervisor::Application::LastLog::getTime() const
{
	timeval tv;
	gettimeofday(&tv, 0);

	ostringstream line;
	char s[100];

	strftime(s, 100, "%Y-%m-%dT%H:%M:%S.", localtime(&tv.tv_sec));
	line << s;
	line << tv.tv_usec / 1000;
	strftime(s, 100, "%z", localtime(&tv.tv_sec));
	line << s;

	return line.str();
}

void emu::supervisor::Application::LastLog::webOutput(xgi::Output *out)
		throw (xgi::exception::Exception)
{
	*out << "Last " << messages_.size() << " log messages:" << br() << endl;
	*out << textarea().set("cols", "120").set("rows", "20")
			.set("readonly").set("class", "log") << endl;

	deque<string>::iterator i = messages_.begin();
	for (; i != messages_.end(); ++i) {
		*out << *i << endl;
	}

	*out << textarea() << endl;
}


void emu::supervisor::Application::bookRunNumber(){

  isBookedRunNumber_ = false;

  // Don't book debug runs:
  if ( run_type_.toString() == "Debug" ) return;
  
  // Just in case it's left over from the previuos run:
  if ( runInfo_ ) {
    delete runInfo_; 
    runInfo_ = 0;
  }

  try
    {
      runInfo_ = emu::supervisor::RunInfo::Instance( runDbBookingCommand_.toString(),
					     runDbWritingCommand_.toString(),
					     runDbUserFile_.toString(),
					     runDbAddress_.toString() );
    }
  catch( string e )
    {
      LOG4CPLUS_ERROR(logger_, e);
      stringstream ss20;
      ss20 <<  e;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss20.str() );
      this->notifyQualified( "error", eObj );
    }

  if ( runInfo_ ){

    const string sequence = "CMS.CSC";
    
    LOG4CPLUS_INFO(logger_, "Booking run number with " <<
		   runDbBookingCommand_.toString() << " at " <<
		   runDbAddress_.toString()  << " for " << sequence );
    
    bool success = runInfo_->bookRunNumber( sequence );
    
    if ( success ){
      isBookedRunNumber_ = true;
      run_number_        = runInfo_->runNumber();
      runSequenceNumber_ = runInfo_->runSequenceNumber();
      LOG4CPLUS_INFO(logger_, "Booked run rumber " << run_number_.toString() <<
		     " (" << sequence << " " << runSequenceNumber_.toString() << ")");
    }
    else{
      LOG4CPLUS_ERROR(logger_,
		      "Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << run_number_.toString() 
		      << " specified by user." );
      stringstream ss21;
      ss21 << 
		      "Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << run_number_.toString() 
		      << " specified by user." ;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss21.str() );
      this->notifyQualified( "error", eObj );
    }
  } // if ( runInfo_ ){

}

void emu::supervisor::Application::writeRunInfo( bool toDatabase ){
  // Update run info db

  if ( !toDatabase ) return;

  // Don't write about debug runs:
  if ( run_type_.toString() == "Debug" ) return;

  // If DAQ Manager is absent, we have nothing to do here:
  if ( ! isDAQManagerControlled("Halt") ) return;

  // emu::daq::manager::Application's FSM is asynchronous. Wait for it.
  if ( ! waitForDAQToExecute("Halt", 10, true ) ){
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as local DAQ has not stopped.");
    stringstream ss22;
    ss22 <<  "Nothing written to run database as local DAQ has not stopped.";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss22.str() );
    this->notifyQualified( "warn", eObj );
    return;
  }

  // If it's not a debug run, it should normally have been booked. If not, inform the user that it somehow wasn't.
  if ( toDatabase && !isBookedRunNumber_ ){
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as no run number was booked.");
    stringstream ss23;
    ss23 <<  "Nothing written to run database as no run number was booked.";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss23.str() );
    this->notifyQualified( "warn", eObj );
  }

    bool success = false;
    const string nameSpace = "CMS.CSC";
    string name, value;

    //
    // Deserialize reply to run summary query
    //

    emu::soap::Messenger m( this );

    // Start and end times
    xdata::String start_time( "UNKNOWN" );
    xdata::String stop_time( "UNKNOWN" );
    // FU event count
    xdata::String built_events( "0" );
    // RUI event counts and instances
    xdata::Vector<xdata::String> rui_counts;
    xdata::Vector<xdata::String> rui_instances;

    try{
      emu::soap::extractParameters( m.sendCommand( "emu::daq::manager::Application", 0, "QueryRunSummary" ),
				    emu::soap::Parameters()                  
				    .add( "start_time"   , &start_time     ) 
				    .add( "stop_time"    , &stop_time      ) 
				    .add( "built_events" , &built_events   ) 
				    .add( "rui_counts"   , &rui_counts     ) 
				    .add( "rui_instances", &rui_instances  ) );
    }
    catch( xcept::Exception e ){
      LOG4CPLUS_WARN( logger_, "Run summary unknown: " << xcept::stdformat_exception_history(e) );
      stringstream ss25;
      ss25 <<  "Run summary unknown: "  ;
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss25.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    
    //
    // run type
    //
    name      = "run_type";
    value     = run_type_.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss26;
	ss26 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss26.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // start time and stop time
    //
    name      = "start_time";
    value     = start_time.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss27;
	ss27 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss27.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name      = "stop_time";
    value     = stop_time.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss28;
	ss28 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss28.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // trigger sources
    //
    xdata::String ClockSource   = "UNKNOWN";
    xdata::String OrbitSource   = "UNKNOWN";
    xdata::String TriggerSource = "UNKNOWN";
    xdata::String BGOSource     = "UNKNOWN";
    try{
      m.getParameters( "ttc::TTCciControl", 2,
		       emu::soap::Parameters()
		       .add( "ClockSource"  , &ClockSource   ) 
		       .add( "OrbitSource"  , &OrbitSource   ) 
		       .add( "TriggerSource", &TriggerSource ) 
		       .add( "BGOSource"    , &BGOSource     ) );
    }
    catch(xcept::Exception e){
      LOG4CPLUS_ERROR(logger_,"Failed to get trigger sources from ttc::TTCciControl: " << 
		      xcept::stdformat_exception_history(e) );
      stringstream ss30;
      ss30 << "Failed to get trigger sources from ttc::TTCciControl: ";
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss30.str(), e );
      this->notifyQualified( "error", eObj );
    }
    name  = "clock_source";
    value = ClockSource.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss31;
	ss31 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss31.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "orbit_source";
    value = OrbitSource.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss32;
	ss32 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss32.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "trigger_source";
    value = TriggerSource.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() );
	stringstream ss33;
	ss33 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss33.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "BGO_source";
    value = BGOSource.toString();
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss34;
	ss34 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss34.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // EmuFU event count
    //
      name  = "built_events";
      value = built_events.toString();
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
			 nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss35;
	  ss35 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss35.str() );
	  this->notifyQualified( "error", eObj );
	}
      }

    //
    // EmuRUI event counts
    //
    for ( unsigned int i = 0; i < rui_counts.elements(); ++i ){
      name  = "EmuRUI" + (dynamic_cast<xdata::String*>(rui_instances.elementAt(i)))->toString();
      value = (dynamic_cast<xdata::String*>(rui_counts.elementAt(i)))->toString();
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss36;
	  ss36 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss36.str() );
	  this->notifyQualified( "error", eObj );
	}
      }
    }
}

// End of file
// vim: set sw=4 ts=4:
// End of file
// vim: set sw=4 ts=4:
