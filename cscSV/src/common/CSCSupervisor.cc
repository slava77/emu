// CSCSupervisor.cc

#include "CSCSupervisor.h"

#include <sstream>
#include <set>
#include <cstdlib>  // strtol()
#include <iomanip>

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"
#include "xoap/domutils.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"

#include "xcept/tools.h"

#include "EmuELog.h"

using namespace std;
using namespace cgicc;

XDAQ_INSTANTIATOR_IMPL(CSCSupervisor);

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";
static const unsigned int N_LOG_MESSAGES = 20;
static const string STATE_UNKNOWN = "unknown";

void CSCSupervisor::CalibParam::registerFields(xdata::Bag<CalibParam> *bag)
{
	key_ = "";
	command_ = "";
	loop_ = 1U;
	delay_ = 1U;
	ltc_ = "LTCConfiguration.txt";

	bag->addField("key",     &key_);
	bag->addField("command", &command_);
	bag->addField("loop",    &loop_);
	bag->addField("delay",   &delay_);
	bag->addField("ltc",     &ltc_);
}

CSCSupervisor::CSCSupervisor(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub),
		daq_mode_(""), trigger_config_(""), ttc_source_(""),
		wl_semaphore_(BSem::EMPTY), quit_calibration_(false),
		daq_descr_(NULL), tf_descr_(NULL), ttc_descr_(NULL),
		nevents_(-1),
		step_counter_(0),
		error_message_(""), keep_refresh_(false), hide_tts_control_(true),
		runInfo_(NULL),
		runDbBookingCommand_( "java -jar runnumberbooker.jar" ),
		runDbWritingCommand_( "java -jar runinfowriter.jar" ),
		runDbAddress_       ( "" ),
		runDbUserFile_      ( "" ),
		isBookedRunNumber_  ( false )
{
	run_type_ = "";
	run_number_ = 0;
	runSequenceNumber_ = 0;

	start_attr.insert(std::map<string, string>::value_type("Param", "Start"));
	stop_attr.insert(std::map<string, string>::value_type("Param", "Stop"));

	xdata::InfoSpace *i = getApplicationInfoSpace();
	i->fireItemAvailable("RunType", &run_type_);
	i->fireItemAvailable("RunNumber", &run_number_);

	i->fireItemAvailable("configKeys", &config_keys_);
	i->fireItemAvailable("calibParams", &calib_params_);
	i->fireItemAvailable("pcKeys",     &pc_keys_);
	i->fireItemAvailable("pcConfigs",  &pc_configs_);
	i->fireItemAvailable("fcKeys",     &fc_keys_);
	i->fireItemAvailable("fcConfigs",  &fc_configs_);

	i->fireItemAvailable("DAQMode", &daq_mode_);
	i->fireItemAvailable("TriggerConfig", &trigger_config_);
	i->fireItemAvailable("TTCSource", &ttc_source_);

	i->fireItemAvailable("TTSCrate", &tts_crate_);
	i->fireItemAvailable("TTSSlot", &tts_slot_);
	i->fireItemAvailable("TTSBits", &tts_bits_);

	i->fireItemAvailable( "curlCommand",    &curlCommand_  );
	i->fireItemAvailable( "curlCookies", 	&curlCookies_  );
	i->fireItemAvailable( "CMSUserFile", 	&CMSUserFile_  );
	i->fireItemAvailable( "eLogUserFile",	&eLogUserFile_ );
	i->fireItemAvailable( "eLogURL",     	&eLogURL_      );
	i->fireItemAvailable( "peripheralCrateConfigFiles", &peripheralCrateConfigFiles_ );

	i->fireItemAvailable( "runDbBookingCommand", &runDbBookingCommand_ );
	i->fireItemAvailable( "runDbWritingCommand", &runDbWritingCommand_ );
	i->fireItemAvailable( "runDbAddress",        &runDbAddress_        );
	i->fireItemAvailable( "runDbUserFile",       &runDbUserFile_       );

	xgi::bind(this, &CSCSupervisor::webDefault,   "Default");
	xgi::bind(this, &CSCSupervisor::webConfigure, "Configure");
	xgi::bind(this, &CSCSupervisor::webEnable,    "Enable");
	xgi::bind(this, &CSCSupervisor::webDisable,   "Disable");
	xgi::bind(this, &CSCSupervisor::webHalt,      "Halt");
	xgi::bind(this, &CSCSupervisor::webReset,     "Reset");
	xgi::bind(this, &CSCSupervisor::webSetTTS,    "SetTTS");
	xgi::bind(this, &CSCSupervisor::webSwitchTTS, "SwitchTTS");

	xoap::bind(this, &CSCSupervisor::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onHalt,      "Halt",      XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onReset,     "Reset",     XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onSetTTS,    "SetTTS",    XDAQ_NS_URI);

	wl_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("CSC SV", "waiting");
	wl_->activate();
	configure_signature_ = toolbox::task::bind(
			this, &CSCSupervisor::configureAction,  "configureAction");
	halt_signature_ = toolbox::task::bind(
			this, &CSCSupervisor::haltAction,       "haltAction");
	calibration_signature_ = toolbox::task::bind(
			this, &CSCSupervisor::calibrationAction, "calibrationAction");

	fsm_.addState('H', "Halted",     this, &CSCSupervisor::stateChanged);
	fsm_.addState('C', "Configured", this, &CSCSupervisor::stateChanged);
	fsm_.addState('E', "Enabled",    this, &CSCSupervisor::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &CSCSupervisor::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &CSCSupervisor::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &CSCSupervisor::enableAction);
	fsm_.addStateTransition(
			'E', 'C', "Disable",   this, &CSCSupervisor::disableAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &CSCSupervisor::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &CSCSupervisor::haltAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &CSCSupervisor::haltAction);
	fsm_.addStateTransition(
			'E', 'E', "SetTTS",    this, &CSCSupervisor::setTTSAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	state_table_.addApplication(this, "EmuFCrate");
	state_table_.addApplication(this, "EmuPeripheralCrateManager");
	state_table_.addApplication(this, "EmuPeripheralCrate");
	state_table_.addApplication(this, "EmuDAQManager");
	state_table_.addApplication(this, "TTCciControl");
	state_table_.addApplication(this, "LTCControl");

	last_log_.size(N_LOG_MESSAGES);

	LOG4CPLUS_INFO(getApplicationLogger(), "CSCSupervisor");
}

xoap::MessageReference CSCSupervisor::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	run_number_ = 0;
	nevents_ = -1;

	submit(configure_signature_);

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	quit_calibration_ = true;

	submit(halt_signature_);

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onReset(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	resetAction();

	return onHalt(message);
}

xoap::MessageReference CSCSupervisor::onSetTTS(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("SetTTS");

	return createReply(message);
}

void CSCSupervisor::webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	if (keep_refresh_) {
		HTTPResponseHeader &header = out->getHTTPResponseHeader();
		header.addHeader("Refresh", "2");
	}

	// Header
	*out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	*out << html() << endl;

	*out << head() << endl;
	*out << title("CSCSupervisor") << endl;
	*out << cgicc::link().set("rel", "stylesheet")
			.set("href", "/cscsv.css")
			.set("type", "text/css") << endl;
	*out << head() << endl;

	// Body
	*out << body() << endl;

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
			.set("value", "Configure") << endl;
	*out << form() << endl;

	
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
			"/" + getApplicationDescriptor()->getURN() + "/Enable") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Enable") << endl;
	*out << form() << td() << endl;

	*out << td() << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Disable") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Disable") << endl;
	*out << form() << td() << endl;

	*out << td() << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Halt") << endl;
	*out << input().set("type", "submit")
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

		*out << "Crate #: " << endl;
		*out << cgicc::select().set("name", "tts_crate") << endl;

		const char n[] = "1234";
		string str = "";
		for (int i = 0; i < 4; ++i) {
			if (n[i] == (tts_crate_.toString())[0]) {
				*out << option().set("value", str + n[i]).set("selected", "");
			} else {
				*out << option().set("value", str + n[i]);
			}
			*out << n[i] << option() << endl;
		}

		*out << cgicc::select() << br() << endl;
		
		*out << "Slot # (4-13): " << endl;
		*out << input().set("type", "text")
				.set("name", "tts_slot")
				.set("value", tts_slot_)
				.set("size", "10") << br() << endl;

		*out << "TTS value: (0-15)" << endl;
		*out << input().set("type", "text")
				.set("name", "tts_bits")
				.set("value", tts_bits_)
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
	*out << "Step counter: " << step_counter_ << br() << endl;

	refreshConfigParameters();

	*out << "Mode of DAQManager: " << daq_mode_.toString() << br() << endl;
	*out << "TF configuration: " << trigger_config_.toString() << br() << endl;
	*out << "TTCci inputs(Clock:Orbit:Trig:BGo): " << ttc_source_.toString() << br() << endl;

	*out << "Local DAQ state: " << getLocalDAQState() << br() << endl;

	// Application states
	*out << hr() << endl;
	state_table_.webOutput(out, (string)state_);

	// Message logs
	*out << hr() << endl;
	last_log_.webOutput(out);

	*out << body() << html() << endl;
}

void CSCSupervisor::webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	string value;

	value = getCGIParameter(in, "runtype");
	if (value.empty()) { error_message_ += "Please select run type.\n"; }
	run_type_ = value;

	run_number_ = 0;
	nevents_ = -1;

	if (error_message_.empty()) {
		submit(configure_signature_);
	}

	webRedirect(in, out);
}

void CSCSupervisor::webEnable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
        // Book run number here to make sure it's done 
        // only when requested by the user from the web page,
        // and not by the FunctionManager via SOAP.
        bookRunNumber();

	fireEvent("Enable");

	webRedirect(in, out);
}

void CSCSupervisor::webDisable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Disable");

	webRedirect(in, out);
}

void CSCSupervisor::webHalt(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	quit_calibration_ = true;

	submit(halt_signature_);

	webRedirect(in, out);
}

void CSCSupervisor::webReset(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	resetAction();

	webHalt(in, out);
}

void CSCSupervisor::webSetTTS(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	tts_crate_ = getCGIParameter(in, "tts_crate");
	tts_slot_  = getCGIParameter(in, "tts_slot");
	tts_bits_  = getCGIParameter(in, "tts_bits");

	if (tts_crate_ == "") { error_message_ += "Please select TTS crate.\n"; }
	if (tts_slot_  == "") { error_message_ += "Please set TTS slot.\n"; }
	if (tts_bits_  == "") { error_message_ += "Please set TTS bits.\n"; }

	if (error_message_.empty()) {
		fireEvent("SetTTS");
	}

	webRedirect(in, out);
}

void CSCSupervisor::webSwitchTTS(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	hide_tts_control_ = getCGIParameter(in, "command").find("Hide", 0) == 0;

	webRedirect(in, out);
}

void CSCSupervisor::webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
			url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

bool CSCSupervisor::configureAction(toolbox::task::WorkLoop *wl)
{
	fireEvent("Configure");

	return false;
}

bool CSCSupervisor::haltAction(toolbox::task::WorkLoop *wl)
{
	fireEvent("Halt");

	return false;
}

bool CSCSupervisor::calibrationAction(toolbox::task::WorkLoop *wl)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "calibrationAction " << "(begin)");

	string command, ltc;
	unsigned int loop, delay;

	int index = getCalibParamIndex(run_type_);
	command = calib_params_[index].bag.command_;
	loop    = calib_params_[index].bag.loop_;
	delay   = calib_params_[index].bag.delay_;
	ltc     = calib_params_[index].bag.ltc_;

	LOG4CPLUS_DEBUG(getApplicationLogger(), "command: " << command
			<< " loop: " << loop << " delay: " << delay << " ltc: " << ltc);

	for (step_counter_ = 0; step_counter_ < loop; ++step_counter_) {
		if (quit_calibration_) { break; }
		LOG4CPLUS_DEBUG(getApplicationLogger(),
				"calibrationAction: " << step_counter_);

		sendCommand(command, "EmuPeripheralCrateManager");
		sendCommandWithAttr("Cyclic", start_attr, "LTCControl");
		sleep(delay);
	}

	keep_refresh_ = false;

	LOG4CPLUS_DEBUG(getApplicationLogger(), "calibrationAction " << "(end)");

	return false;
}

void CSCSupervisor::configureAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");
	LOG4CPLUS_DEBUG(getApplicationLogger(), "runtype: " << run_type_.toString()
			<< " runnumber: " << run_number_ << " nevents: " << nevents_);

	try {
		if (state_table_.getState("EmuDAQManager", 0) == "Configured") {
			sendCommand("Halt", "EmuDAQManager");
		}
		if (state_table_.getState("TTCciControl", 0) != "Halted") {
			sendCommand("Halt", "TTCciControl");
		}
		if (state_table_.getState("LTCControl", 0) != "Halted") {
			sendCommand("Halt", "LTCControl");
		}

		string str = trim(getCrateConfig("PC", run_type_.toString()));
		if (!str.empty()) {
			setParameter(
					"EmuPeripheralCrate", "xmlFileName", "xsd:string", str);
		}

		setParameter("EmuDAQManager", "maxNumberOfEvents", "xsd:integer",
				toString(nevents_));
		setParameter("EmuDAQManager", "runType", "xsd:string",
				run_type_.toString());

		sendCommand("Configure", "EmuFCrate");
		if (!isCalibrationMode()) {
			sendCommand("Configure", "EmuPeripheralCrate");
		} else {
			sendCommand("ConfigCalCFEB", "EmuPeripheralCrateManager");
		}
		sendCommand("Configure", "EmuDAQManager");
		sendCommand("Configure", "TTCciControl");

		int index = getCalibParamIndex(run_type_);
		if (index >= 0) {
			setParameter("LTCControl", "Configuration", "xsd:string",
					"[file=" + calib_params_[index].bag.ltc_.toString() + "]");
		}
		sendCommand("Configure", "LTCControl");

		refreshConfigParameters();

	} catch (xoap::exception::Exception e) {
		LOG4CPLUS_ERROR(getApplicationLogger(),
				"Exception in " << evt->type() << ": " << e.what());
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		LOG4CPLUS_ERROR(getApplicationLogger(),
				"Exception in " << evt->type() << ": " << e.what());
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::enableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");
	LOG4CPLUS_DEBUG(getApplicationLogger(), "runtype: " << run_type_.toString()
			<< " runnumber: " << run_number_ << " nevents: " << nevents_);

	try {
		if (state_table_.getState("EmuDAQManager", 0) == "Halted") {
			setParameter("EmuDAQManager", "maxNumberOfEvents", "xsd:integer",
					toString(nevents_));
			sendCommand("Configure", "EmuDAQManager");
		}
		setParameter("EmuDAQManager", "runNumber", "xsd:unsignedLong",
				run_number_.toString());
		sendCommand("Enable", "EmuFCrate");
		if (!isCalibrationMode()) {
			sendCommand("Enable", "EmuPeripheralCrate");
		}
		sendCommand("Enable", "EmuDAQManager");
		sendCommand("Enable", "TTCciControl");
		sendCommand("Enable", "LTCControl");
		sendCommandWithAttr("Cyclic", stop_attr, "LTCControl");

		refreshConfigParameters();

	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	if (isCalibrationMode()) {
		quit_calibration_ = false;
		submit(calibration_signature_);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::disableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
		sendCommand("Halt", "LTCControl");
		sendCommand("Halt", "TTCciControl");
		sendCommand("Halt", "EmuDAQManager");
		writeRunInfo( true, true );
		sendCommand("Disable", "EmuFCrate");
		if (!isCalibrationMode()) {
			sendCommand("Disable", "EmuPeripheralCrate");
		} else {
			sendCommand("Disable", "EmuPeripheralCrateManager");
		}
		sendCommand("Configure", "TTCciControl");
		sendCommand("Configure", "LTCControl");
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::haltAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
		sendCommand("Halt", "LTCControl");
		sendCommand("Halt", "TTCciControl");
		sendCommand("Halt", "EmuFCrate");
		sendCommand("Halt", "EmuPeripheralCrateManager");
		sendCommand("Halt", "EmuPeripheralCrate");
		sendCommand("Halt", "EmuDAQManager");
		writeRunInfo( true, true );
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::resetAction() throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(begin)");

	fsm_.reset();
	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(end)");
}

void CSCSupervisor::setTTSAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	const string fed_app = "EmuFCrate";

	try {
		setParameter(fed_app, "ttsCrate", "xsd:unsignedInt", tts_crate_);
		setParameter(fed_app, "ttsSlot",  "xsd:unsignedInt", tts_slot_);
		setParameter(fed_app, "ttsBits",  "xsd:unsignedInt", tts_bits_);

		int instance = (tts_crate_ == "1") ? 0 : 1;
		sendCommand("SetTTSBits", fed_app, instance);
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::submit(toolbox::task::ActionSignature *signature)
{
	wl_->submit(signature);
	keep_refresh_ = true;
}

void CSCSupervisor::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	keep_refresh_ = false;

    EmuApplication::stateChanged(fsm);
}

void CSCSupervisor::sendCommand(string command, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	if (klass == "EmuDAQManager" && !isDAQManagerControlled(command)) {
		return;  // Do nothing if EmuDAQManager is not under control.
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);
	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		// postSOAP() may throw an exception when failed.
		reply = getApplicationContext()->postSOAP(message, *i);

		analyzeReply(message, reply, *i);
	}
}

void CSCSupervisor::sendCommand(string command, string klass, int instance)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	xdaq::ApplicationDescriptor *app;
	try {
		app = getApplicationContext()->getDefaultZone()
				->getApplicationDescriptor(klass, instance);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	if (klass == "EmuDAQManager" && !isDAQManagerControlled(command)) {
		return;  // Do nothing if EmuDAQManager is not under control.
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);
	xoap::MessageReference reply;

	// send the message
	// postSOAP() may throw an exception when failed.
	reply = getApplicationContext()->postSOAP(message, app);

	analyzeReply(message, reply, app);
}

void CSCSupervisor::sendCommandWithAttr(
		string command, std::map<string, string> attr, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	if (klass == "EmuDAQManager" && !isDAQManagerControlled(command)) {
		return;  // Do nothing if EmuDAQManager is not under control.
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAPWithAttr(command, attr);
	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		// postSOAP() may throw an exception when failed.
		reply = getApplicationContext()->postSOAP(message, *i);

		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference CSCSupervisor::createCommandSOAP(string command)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(name);

	return message;
}

xoap::MessageReference CSCSupervisor::createCommandSOAPWithAttr(
		string command, std::map<string, string> attr)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	xoap::SOAPElement element = envelope.getBody().addBodyElement(name);

	std::map<string, string>::iterator i;
	for (i = attr.begin(); i != attr.end(); ++i) {
		xoap::SOAPName p = envelope.createName((*i).first, "xdaq", XDAQ_NS_URI);
		element.addAttribute(p, (*i).second);
	}

	return message;
}

void CSCSupervisor::setParameter(
		string klass, string name, string type, string value)
{
	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createParameterSetSOAP(
			klass, name, type, value);
	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		reply = getApplicationContext()->postSOAP(message, *i);
		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference CSCSupervisor::createParameterSetSOAP(
		string klass, string name, string type, string value)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterSet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			name, klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode(value);

	return message;
}

xoap::MessageReference CSCSupervisor::createParameterGetSOAP(
		string klass, string name, string type)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			name, klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode("");

	return message;
}

xoap::MessageReference CSCSupervisor::createParameterGetSOAP(
		string klass, std::map<string, string> name_type)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	std::map<string, string>::iterator i;
	for (i = name_type.begin(); i != name_type.end(); ++i) {
		xoap::SOAPName n = envelope.createName(
				(*i).first, klass, "urn:xdaq-application:" + klass);
		xoap::SOAPElement e = properties_e.addChildElement(n);
		e.addAttribute(xsitype, (*i).second);
		e.addTextNode("");
	}

	return message;
}

void CSCSupervisor::analyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app)
{
	string message_str, reply_str;

	reply->writeTo(reply_str);
	ostringstream s;
	s << "Reply from "
			<< app->getClassName() << "(" << app->getInstance() << ")" << endl
			<< reply_str;
	last_log_.add(s.str());
	LOG4CPLUS_DEBUG(getApplicationLogger(), reply_str);

	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

	// do nothing when no fault
	if (!body.hasFault()) { return; }

	ostringstream error;

	error << "SOAP message: " << endl;
	message->writeTo(message_str);
	error << message_str << endl;
	error << "Fault string: " << endl;
	error << reply_str << endl;

	LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
	XCEPT_RAISE(xoap::exception::Exception, "SOAP fault: \n" + reply_str);

	return;
}

string CSCSupervisor::extractParameter(
		xoap::MessageReference message, string name)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement parameter = properties.getChildElements(
			*(new xoap::SOAPName(name, "", "")))[0];

	return parameter.getValue();
}

void CSCSupervisor::refreshConfigParameters()
{
	daq_mode_ = getDAQMode();
	trigger_config_ = getTFConfig();
	ttc_source_ = getTTCciSource();
}

string CSCSupervisor::getCGIParameter(xgi::Input *in, string name)
{
	cgicc::Cgicc cgi(in);
	string value;

	form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}

int CSCSupervisor::keyToIndex(const string key)
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

string CSCSupervisor::getCrateConfig(const string type, const string key) const
{
	xdata::Vector<xdata::String> keys;
	xdata::Vector<xdata::String> values;

	if (type == "PC") {
		keys = pc_keys_;
		values = pc_configs_;
	} else if (type == "FC") {
		keys = fc_keys_;
		values = fc_configs_;
	} else {
		return "";
	}

	string result = "";
	for (unsigned int i = 0; i < keys.size(); ++i) {
		if (keys[i] == key) {
			result = values[i];
			break;
		}
	}

	return result;
}

bool CSCSupervisor::isCalibrationMode()
{
	return (getCalibParamIndex(run_type_) >= 0);
}

int CSCSupervisor::getCalibParamIndex(const string name)
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

string CSCSupervisor::trim(string orig) const
{
	string s = orig;

	s.erase(0, s.find_first_not_of(" \t\n"));
	s.erase(s.find_last_not_of(" \t\n") + 1);

	return s;
}

string CSCSupervisor::toString(const long int i) const
{
	ostringstream s;
	s << i;

	return s.str();
}

string CSCSupervisor::getDAQMode()
{
	string result = "";

	if (daq_descr_ == NULL) {
		try {
			daq_descr_ = getApplicationContext()->getDefaultZone()
					->getApplicationDescriptor("EmuDAQManager", 0);
		} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
			return result; // Do nothing if the target doesn't exist
		}

		std::map<string, string> m;
		m["globalMode"] = "xsd:boolean";
		m["configuredInGlobalMode"] = "xsd:boolean";
		m["daqState"] = "xsd:string";
		daq_param_ = createParameterGetSOAP("EmuDAQManager", m);
	}

	xoap::MessageReference reply;
	try {
		reply = getApplicationContext()->postSOAP(daq_param_, daq_descr_);
		analyzeReply(daq_param_, reply, daq_descr_);

		result = extractParameter(reply, "globalMode");
		result = (result == "true") ? "global" : "local";
	} catch (xdaq::exception::Exception e) {
		result = "Unknown";
	}

	return result;
}

string CSCSupervisor::getTFConfig()
{
	string result = "";

	if (tf_descr_ == NULL) {
		try {
			tf_descr_ = getApplicationContext()->getDefaultZone()
					->getApplicationDescriptor("TF_hyperDAQ", 0);
		} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
			return result; // Do nothing if the target doesn't exist
		}
		tf_param_ = createParameterGetSOAP(
				"TF_hyperDAQ", "triggerMode", "xsd:string");
	}

	xoap::MessageReference reply;
	try {
		reply = getApplicationContext()->postSOAP(tf_param_, tf_descr_);
		analyzeReply(tf_param_, reply, tf_descr_);

		result = extractParameter(reply, "triggerMode");
	} catch (xdaq::exception::Exception e) {
		result = "Unknown";
	}

	return result;
}

string CSCSupervisor::getTTCciSource()
{
	string result = "";

	if (ttc_descr_ == NULL) {
		try {
			ttc_descr_ = getApplicationContext()->getDefaultZone()
					->getApplicationDescriptor("TTCciControl", 0);
		} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
			return result; // Do nothing if the target doesn't exist
		}

		std::map<string, string> m;
		m["ClockSource"] = "xsd:string";
		m["OrbitSource"] = "xsd:string";
		m["TriggerSource"] = "xsd:string";
		m["BGOSource"] = "xsd:string";
		ttc_param_ = createParameterGetSOAP("TTCciControl", m);
	}

	xoap::MessageReference reply;
	try {
		reply = getApplicationContext()->postSOAP(ttc_param_, ttc_descr_);
		analyzeReply(ttc_param_, reply, ttc_descr_);

		result = extractParameter(reply, "ClockSource");
		result += ":" + extractParameter(reply, "OrbitSource");
		result += ":" + extractParameter(reply, "TriggerSource");
		result += ":" + extractParameter(reply, "BGOSource");
	} catch (xdaq::exception::Exception e) {
		result = "Unknown";
	}

	return result;
}

bool CSCSupervisor::isDAQConfiguredInGlobal()
{
	string result = "";

	if (daq_descr_ != NULL) {
		xoap::MessageReference reply;
		try {
			reply = getApplicationContext()->postSOAP(daq_param_, daq_descr_);
			analyzeReply(daq_param_, reply, daq_descr_);

			result = extractParameter(reply, "configuredInGlobalMode");
		} catch (xdaq::exception::Exception e) {
			result = "Unknown";
		}
	}

	return result == "true";
}

string CSCSupervisor::getLocalDAQState()
{
	string result = "";

	if (daq_descr_ != NULL) {
		xoap::MessageReference reply;
		try {
			reply = getApplicationContext()->postSOAP(daq_param_, daq_descr_);
			analyzeReply(daq_param_, reply, daq_descr_);

			result = extractParameter(reply, "daqState");
		} catch (xdaq::exception::Exception e) {
			result = "Unknown";
		}
	}

	return result;
}

bool CSCSupervisor::isDAQManagerControlled(string command)
{
	// Enforce "Halt" irrespective of DAQ mode.
	if (command == "Halt") { return true; }

	// Don't send any other command when DAQ is in local mode.
	if (getDAQMode() != "global") { return false; }

	// And don't send any other command when DAQ was configured in local mode, either.
	if (command != "Configure" && !isDAQConfiguredInGlobal()) { return false; }

	return true;
}

void CSCSupervisor::StateTable::addApplication(CSCSupervisor *sv, string klass)
{
	sv_ = sv;

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = sv->getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// add to the table
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		table_.push_back(
				pair<xdaq::ApplicationDescriptor *, string>(*i, "NULL"));
	}
}

void CSCSupervisor::StateTable::refresh()
{
	string klass = "";
	xoap::MessageReference message, reply;

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass != i->first->getClassName()) {
			klass = i->first->getClassName();
			message = createStateSOAP(klass);
		}

		try {
			reply = sv_->getApplicationContext()->postSOAP(message, i->first);
			sv_->analyzeReply(message, reply, i->first);

			i->second = extractState(reply, klass);
		} catch (xdaq::exception::Exception e) {
			i->second = STATE_UNKNOWN;
		} catch (...) {
			LOG4CPLUS_DEBUG(sv_->getApplicationLogger(), "Exception with " << klass);
			i->second = STATE_UNKNOWN;
		}
	}
}

string CSCSupervisor::StateTable::getState(string klass, unsigned int instance)
{
	string state = "";

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
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

void CSCSupervisor::StateTable::webOutput(xgi::Output *out, string sv_state)
		throw (xgi::exception::Exception)
{
	refresh();
	*out << table() << tbody() << endl;

	// My state
	*out << tr();
	*out << td() << "CSCSupervisor" << "(" << "0" << ")" << td();
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

xoap::MessageReference CSCSupervisor::StateTable::createStateSOAP(
		string klass)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			"stateName", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, "xsd:string");

	return message;
}

string CSCSupervisor::StateTable::extractState(xoap::MessageReference message, string klass)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement state = properties.getChildElements(
			*(new xoap::SOAPName("stateName", "", "")))[0];

	return state.getValue();
}

void CSCSupervisor::LastLog::size(unsigned int size)
{
	size_ = size;
}

unsigned int CSCSupervisor::LastLog::size() const
{
	return size_;
}

void CSCSupervisor::LastLog::add(string message)
{
	messages_.push_back(message);
	if (messages_.size() > size_) { messages_.pop_front(); }
}

void CSCSupervisor::LastLog::webOutput(xgi::Output *out)
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

string CSCSupervisor::reformatTime( string time ){
  // reformat from YYMMDD_hhmmss_UTC to YYYY-MM-DD hh:mm:ss UTC
  string reformatted("");
  reformatted += "20";
  reformatted += time.substr(0,2);
  reformatted += "-";
  reformatted += time.substr(2,2);
  reformatted += "-";
  reformatted += time.substr(4,2);
  reformatted += " ";
  reformatted += time.substr(7,2);
  reformatted += ":";
  reformatted += time.substr(9,2);
  reformatted += ":";
  reformatted += time.substr(11,2);
  reformatted += " UTC";
  return reformatted;
}

vector< vector<string> > CSCSupervisor::getFUEventCounts()
{
  vector< vector<string> > ec;

  unsigned int totalProcessed = 0;

  std::set<xdaq::ApplicationDescriptor *> EmuFUs;
  try {
    EmuFUs = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptors("EmuFU");
  } 
  catch (...){}
  // Zone::getApplicationDescriptors doesn't throw!
  if ( EmuFUs.size() == 0 ){  
    LOG4CPLUS_WARN(getApplicationLogger(), 
		   "Failed to get application descriptors for EmuFUs");
    vector<string> svt;
    svt.push_back( "Total" );
    svt.push_back( "UNKNOWN" );
    ec.push_back( svt );
    return ec;
  }

  xoap::MessageReference message = createParameterGetSOAP(
		"EmuFU", "nbEventsProcessed", "xsd:unsignedLong");

  std::set<xdaq::ApplicationDescriptor *>::iterator fu;
  for ( fu = EmuFUs.begin(); fu!=EmuFUs.end(); ++fu ){
	string       count;
	stringstream name;
    unsigned int nProcessed = 0;
    stringstream ss;
    try
    {
	  name << "EmuFU" << setfill('0') << setw(2) << (*fu)->getInstance();
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, *fu);
	  analyzeReply(message, reply, *fu);
	  count = extractParameter(reply, "nbEventsProcessed");
      ss << count;
      ss >> nProcessed;
      totalProcessed += nProcessed;
    }
    catch(xcept::Exception e)
    {
      count = "UNKNOWN";
      LOG4CPLUS_WARN(getApplicationLogger(),
			"Failed to get event count of " << name.str()
			<< " : " << xcept::stdformat_exception_history(e));
    }
    vector<string> sv;
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }
  stringstream sst;
  sst << totalProcessed;
  vector<string> svt;
  svt.push_back( "Total" );
  svt.push_back( sst.str()  );
  ec.push_back( svt );

  return ec;
}

vector< vector<string> > CSCSupervisor::getRUIEventCounts()
{
  vector< vector<string> > ec;

  std::set<xdaq::ApplicationDescriptor *> EmuRUIs;
  try {
    EmuRUIs = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptors("EmuRUI");
  } 
  catch (...){}
  // Zone::getApplicationDescriptors doesn't throw!
  if ( EmuRUIs.size() == 0 ) {
    LOG4CPLUS_WARN(getApplicationLogger(), 
		    "Failed to get application descriptors for EmuRUIs");
    return ec;
  }

  std::map<string, string> m;
  m["nEventsRead"     ] = "xsd:unsignedLong";
  m["hardwareMnemonic"] = "xsd:string";
  xoap::MessageReference message = createParameterGetSOAP("EmuRUI", m);

  std::set< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = EmuRUIs.begin(); rui!=EmuRUIs.end(); ++rui ){
    string count;
	string mnemonic;
    stringstream name;
	try {
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, *rui);
	  analyzeReply(message, reply, *rui);
	  count = extractParameter(reply, "nEventsRead");
	  mnemonic = extractParameter(reply, "hardwareMnemonic");
	}
    catch(xcept::Exception e)
    {
      count    = "UNKNOWN";
      LOG4CPLUS_WARN(getApplicationLogger(),
			"Failed to get event count of "
			<< "EmuRUI" << setfill('0') << setw(2) << (*rui)->getInstance()
			<< " [" << mnemonic << "]"
		    << " : " << xcept::stdformat_exception_history(e));
    }
    vector<string> sv;
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }

  return ec;
}

void CSCSupervisor::postToELog( string subject, string body, vector<string> *attachments ){
  // Post to eLog:
  EmuELog *eel;
  try
    {
      eel = new EmuELog(curlCommand_.toString(),
			curlCookies_.toString(),
			CMSUserFile_.toString(),
			eLogUserFile_.toString(),
			eLogURL_.toString());
    }
  catch( string e ){
    LOG4CPLUS_ERROR(getApplicationLogger(), e);
    eel = 0;
  }
  if ( eel ) {
    string attachmentList;
    if ( attachments )
      for ( vector<string>::iterator attm = attachments->begin(); attm != attachments->end(); ++attm )
	attachmentList += *attm + "\n";
    LOG4CPLUS_INFO(getApplicationLogger(), 
		   "Posting to eLog address " << eLogURL_.toString() << 
		   " as user " << eel->eLogUser() << " (" << eel->CMSUser() << ") " <<
		   ":\nSubject: " << subject << 
		   "\nBody:\n" << body <<
		   "\nAttachments:\n" << attachmentList );
    eel->postMessage( subject, body, attachments );
  }
  delete eel;
}

void CSCSupervisor::bookRunNumber(){

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
      runInfo_ = EmuRunInfo::Instance( runDbBookingCommand_.toString(),
				       runDbWritingCommand_.toString(),
				       runDbUserFile_.toString(),
				       runDbAddress_.toString() );
    }
  catch( string e )
    {
      LOG4CPLUS_ERROR(getApplicationLogger(), e);
    }

  if ( runInfo_ ){

    const string sequence = "CMS.CSC";
    
    LOG4CPLUS_INFO(getApplicationLogger(), "Booking run number with " <<
		   runDbBookingCommand_.toString() << " at " <<
		   runDbAddress_.toString()  << " for " << sequence );
    
    bool success = runInfo_->bookRunNumber( sequence );
    
    if ( success ){
      isBookedRunNumber_ = true;
      run_number_        = runInfo_->runNumber();
      runSequenceNumber_ = runInfo_->runSequenceNumber();
      LOG4CPLUS_INFO(getApplicationLogger(), "Booked run rumber " << run_number_.toString() <<
		     " (" << sequence << " " << runSequenceNumber_.toString() << ")");
    }
    else LOG4CPLUS_ERROR(getApplicationLogger(),
			 "Failed to book run number: " 
			 <<  runInfo_->errorMessage()
			 << " ==> Falling back to run number " << run_number_.toString() 
			 << " specified by user." );
  } // if ( runInfo_ ){

}

void CSCSupervisor::writeRunInfo( bool toDatabase, bool toELog ){
  // Update run info db and post to eLog as well

  // Don't write about debug runs:
  if ( run_type_.toString() == "Debug" ) return;

  // If it's not a debug run, it should normally have been booked. If not, inform the user that it somehow wasn't.
  if ( toDatabase && !isBookedRunNumber_ ) LOG4CPLUS_WARN(getApplicationLogger(), "Nothing written to run database as no run number was booked.");

    stringstream subjectToELog;
    subjectToELog << "Emu local run " << run_number_.toString()
		  << " (" << run_type_.toString() << ")"// << ( badRun_? " is bad" : "" );
		  << " ---CSCSupervisor";
    //
    // run number; bad run; global run number
    //
    stringstream htmlMessageToELog;
    htmlMessageToELog << " <b>Emu local run</b><br/><br/>"; // Attention: Body must not start with html tag (elog feature...)
    htmlMessageToELog << "<table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run number</td><td>" << run_number_.toString() << "</td></tr>";
//     htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">bad run</td><td>" << ( badRun_? "true" : "false" ) << "</td></tr>";

    bool success = false;
    const string nameSpace = "CMS.CSC";
    string name, value;

    //
    // run type
    //
    name      = "run_type";
    value     = run_type_.toString();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run type</td><td>" << run_type_.toString() << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
    }

    std::map<string,string> namesAndTypes;
    xdaq::ApplicationDescriptor* app;
    std::set<xdaq::ApplicationDescriptor *> apps;
    
    //
    // start time and stop time
    //
    namesAndTypes["runStartTime"] = "xsd:string";
    namesAndTypes["runStopTime" ] = "xsd:string";
    string runStartTime("UNKNOWN");
    string runStopTime("UNKNOWN");
    try{
      app = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("EmuTA",0);
	  xoap::MessageReference message =
			createParameterGetSOAP("EmuTA", namesAndTypes);
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, app);
	  analyzeReply(message, reply, app);
      runStartTime = reformatTime(extractParameter(reply, "runStartTime"));
      runStopTime  = reformatTime(extractParameter(reply, "runStopTime"));
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e) {
      LOG4CPLUS_ERROR(getApplicationLogger(),"Failed to get time of run start and stop from EmuTA 0: " << 
		      xcept::stdformat_exception_history(e) );
    }
    catch(xcept::Exception e){
      LOG4CPLUS_WARN(getApplicationLogger(),"Failed to get time of run start and stop from EmuTA 0: " << 
		     xcept::stdformat_exception_history(e) );
    }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">start time</td><td>" << runStartTime << "</td></tr>";
    name      = "start_time";
    value     = runStartTime;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString()  <<
				      " : " << runInfo_->errorMessage() ); }
    }
    name      = "stop_time";
    value     = runStopTime;
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">stop time</td><td>" << value << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString()  <<
				      " : " << runInfo_->errorMessage() ); }
    }


//     //
//     // comments
//     //
//     htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">comments</td><td>" << textToHtml(comments_) << "</td></tr>";


    //
    // trigger mode
    //
    value = "UNKNOWN";
    try{
      app = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("TF_hyperDAQ",0);
	  xoap::MessageReference message =
			createParameterGetSOAP("TF_hyperDAQ", "triggerMode", "xsd:string");
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, app);
	  analyzeReply(message, reply, app);
      value = extractParameter(reply, "triggerMode");
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e) {
      LOG4CPLUS_ERROR(getApplicationLogger(),"Failed to get trigger mode from TF_hyperDAQ 0: " << 
		      xcept::stdformat_exception_history(e) );
    }
    catch(xcept::Exception e){
      LOG4CPLUS_ERROR(getApplicationLogger(),"Failed to get trigger mode from TF_hyperDAQ 0: " << 
		      xcept::stdformat_exception_history(e) );
    }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">Track Finder</td>";
    htmlMessageToELog << "<td><table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger mode" << "</td><td align=\"right\">" 
		      << value << "</td></tr>";
    htmlMessageToELog << "</table></td></tr>";
    name  = "trigger_mode";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
    }

    //
    // trigger sources
    //
    namesAndTypes.clear();
    namesAndTypes["ClockSource"  ] = "xsd:string";
    namesAndTypes["OrbitSource"  ] = "xsd:string";
    namesAndTypes["TriggerSource"] = "xsd:string";
    namesAndTypes["BGOSource"    ] = "xsd:string";
    string ClockSource("UNKNOWN");
    string OrbitSource("UNKNOWN");
    string TriggerSource("UNKNOWN");
    string BGOSource("UNKNOWN");
    try{
      app = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("TTCciControl",0);
	  xoap::MessageReference message =
			createParameterGetSOAP("TTCciControl", namesAndTypes);
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, app);
	  analyzeReply(message, reply, app);
      ClockSource   = extractParameter(reply, "ClockSource");
      OrbitSource   = extractParameter(reply, "OrbitSource");
      TriggerSource = extractParameter(reply, "TriggerSource");
      BGOSource     = extractParameter(reply, "BGOSource");
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e) {
      LOG4CPLUS_ERROR(getApplicationLogger(),"Failed to get trigger sources from TTCciControl 0: " << 
		      xcept::stdformat_exception_history(e) );
    }
    catch(xcept::Exception e){
      LOG4CPLUS_ERROR(getApplicationLogger(),"Failed to get trigger sources from TTCciControl 0: " << 
		      xcept::stdformat_exception_history(e) );
    }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">TTCci</td>";
    htmlMessageToELog << "<td><table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "clock source"   << "</td><td align=\"right\">" 
		      << ClockSource   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "orbit source"   << "</td><td align=\"right\">" 
		      << OrbitSource   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger source" << "</td><td align=\"right\">" 
		      << TriggerSource << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "BGO source"     << "</td><td align=\"right\">" 
		      << BGOSource     << "</td></tr>";
    htmlMessageToELog << "</table></td></tr>";
    name  = "clock_source";
    value = ClockSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
    }
    name  = "orbit_source";
    value = OrbitSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
    }
    name  = "trigger_source";
    value = TriggerSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() ); }
    }
    name  = "BGO_source";
    value = BGOSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(getApplicationLogger(),
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
    }

    //
    // EmuFU event count
    //
    vector< vector<string> > counts = getFUEventCounts();
    if ( counts.size() > 0 ){
      int nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
      name      = "EmuFU";
      value     = counts.at(nFUs).at(1); // the last element is the sum of all FUs' event counts
      htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events built</td><td>" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value ); }
	else          { LOG4CPLUS_ERROR(getApplicationLogger(),
					"Failed to write " << nameSpace << ":" << name << 
					" to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
      }
    }

    //
    // EmuRUI event counts
    //
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events read</td><td><table>";
    counts.clear();
    counts = getRUIEventCounts();
    int nRUIs = counts.size();
    for ( int rui=0; rui<nRUIs; ++rui ){
      name  = counts.at(rui).at(0);
      value = counts.at(rui).at(1);
      htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << name << "</td><td align=\"right\">" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){ LOG4CPLUS_INFO(getApplicationLogger(), "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value ); }
	else          { LOG4CPLUS_ERROR(getApplicationLogger(),
					"Failed to write " << nameSpace << ":" << name << 
					" to run database " << runDbAddress_.toString() <<
				      " : " << runInfo_->errorMessage() ); }
      }
    }
    htmlMessageToELog << "</table>";

    htmlMessageToELog << "</td></tr></table>";


    if ( toELog ){
      vector<string> attachments;
      for ( unsigned int i=0; i<peripheralCrateConfigFiles_.elements(); ++i ){
	xdata::String* f = dynamic_cast<xdata::String*>(peripheralCrateConfigFiles_.elementAt(i));
	attachments.push_back( f->toString() );
      }
      postToELog( subjectToELog.str(), htmlMessageToELog.str(), &attachments );

      // Just in case submission to e-log failed...
      cout << 
	"\n========================================================================\n" <<
	"If automatic posting to eLog address " << eLogURL_.toString() << 
	" failed, post this manually:\nSubject: " << subjectToELog.str() << 
	"\nBody:\n" << htmlMessageToELog.str() <<
	"\n========================================================================\n";
    }
}

// End of file
// vim: set sw=4 ts=4:
