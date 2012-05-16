
/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini                                    *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include "emuSTEPManager.h"

#include <iostream>
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

#include "test_config_struct.h"

using namespace cgicc;

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";

//
// provides factory method for instantion of SimpleWeb application
//
XDAQ_INSTANTIATOR_IMPL(emuSTEPManager)

emuSTEPManager::emuSTEPManager(xdaq::ApplicationStub * s)
  throw (xdaq::exception::Exception): emuSTEPApplication(s)
{

	string s_endcap_, s_chamtype_, s_chamnum_, s_run_type_; 
	unsigned long s_run_number_;

	// read history
	(new emuSTEPConfigRead)->read_last_used
	(
		s_endcap_, 	 
		s_chamtype_, 
		s_chamnum_,  
		s_run_type_, 
		s_run_number_, 
		ddu_in_
	);

	endcap_     = s_endcap_;
	chamtype_   = s_chamtype_;
	chamnum_    = s_chamnum_; 
	run_type_   = s_run_type_;
	run_number_ = s_run_number_;

    xdata::InfoSpace *i = getApplicationInfoSpace();
    i->fireItemAvailable("RunType", &run_type_);
    i->fireItemAvailable("RunNumber", &run_number_);
    i->fireItemAvailable("configKeys", &config_keys_);

    xgi::bind(this, &emuSTEPManager::webDefault, 	"Default");
    xgi::bind(this, &emuSTEPManager::webConfigure, 	"Configure");
    xgi::bind(this, &emuSTEPManager::webStart,    	"Start");
    xgi::bind(this, &emuSTEPManager::webShow,   	"Show");
    xgi::bind(this, &emuSTEPManager::webHalt,      	"Stop");
    xgi::bind(this, &emuSTEPManager::webReset,     	"Reset");
    xgi::bind(this, &emuSTEPManager::webChamSel, 	"chamber_select");

    xoap::bind(this, &emuSTEPManager::onConfigure, 	"Configure", XDAQ_NS_URI);
    xoap::bind(this, &emuSTEPManager::onStart,    	"Start",     XDAQ_NS_URI);
    xoap::bind(this, &emuSTEPManager::onShow,   	"Show",   XDAQ_NS_URI);
    xoap::bind(this, &emuSTEPManager::onHalt,      	"Halt",      XDAQ_NS_URI);
    xoap::bind(this, &emuSTEPManager::onReset,     	"Reset",     XDAQ_NS_URI);

    wl_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("CSC SV", "waiting");
    wl_->activate();
    configure_signature_ = toolbox::task::bind(
            this, &emuSTEPManager::configureAction,  "configureAction");
    halt_signature_ = toolbox::task::bind(
            this, &emuSTEPManager::haltAction,       "haltAction");


    fsm_.addState('H', "Halted",     this, &emuSTEPManager::stateChanged);
    fsm_.addState('C', "Configured", this, &emuSTEPManager::stateChanged);
    fsm_.addState('E', "Enabled",    this, &emuSTEPManager::stateChanged);

    fsm_.addStateTransition(
            'H', 'C', "Configure", this, &emuSTEPManager::configureAction);
    fsm_.addStateTransition(
            'C', 'C', "Configure", this, &emuSTEPManager::configureAction);
    fsm_.addStateTransition(
            'C', 'E', "Enable",    this, &emuSTEPManager::enableAction);
    fsm_.addStateTransition(
            'E', 'C', "Disable",   this, &emuSTEPManager::disableAction);
    fsm_.addStateTransition(
            'C', 'H', "Halt",      this, &emuSTEPManager::haltAction);
    fsm_.addStateTransition(
            'E', 'H', "Halt",      this, &emuSTEPManager::haltAction);
    fsm_.addStateTransition(
            'H', 'H', "Halt",      this, &emuSTEPManager::haltAction);

    fsm_.setInitialState('H');
    fsm_.reset();

    state_ = fsm_.getStateName(fsm_.getCurrentState());

    // state_table_.addApplication(this, "emuSTEPTests");

}

void emuSTEPManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    // keep_refresh_ = false;

    //EmuApplication::stateChanged(fsm);
      state_ = fsm.getStateName(fsm.getCurrentState());

      LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << (string)state_);
}


xoap::MessageReference emuSTEPManager::onConfigure(xoap::MessageReference message)
        throw (xoap::exception::Exception)
{
    run_number_ = 0;
    nevents_ = 0;

//    submit(configure_signature_);

    return createReply(message);
}

xoap::MessageReference emuSTEPManager::onStart(xoap::MessageReference message)
        throw (xoap::exception::Exception)
{
    fireEvent("Start");

    return createReply(message);
}

xoap::MessageReference emuSTEPManager::onShow(xoap::MessageReference message)
        throw (xoap::exception::Exception)
{
//    fireEvent("Show");

    return createReply(message);
}

xoap::MessageReference emuSTEPManager::onHalt(xoap::MessageReference message)
        throw (xoap::exception::Exception)
{
  //  quit_calibration_ = true;

 //   submit(halt_signature_);

    return createReply(message);
}

xoap::MessageReference emuSTEPManager::onReset(xoap::MessageReference message)
        throw (xoap::exception::Exception)
{
    resetAction();

    return onHalt(message);
}

void emuSTEPManager::configureAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");
	LOG4CPLUS_DEBUG(getApplicationLogger(), "runtype: " << run_type_.toString()
			<< " runnumber: " << run_number_ << " nevents: " << nevents_);

	try {

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

void emuSTEPManager::enableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");
	LOG4CPLUS_DEBUG(getApplicationLogger(), "runtype: " << run_type_.toString()
			<< " runnumber: " << run_number_ << " nevents: " << nevents_);

	try {

	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void emuSTEPManager::disableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void emuSTEPManager::haltAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
	//	sendCommand("Halt", "EmuPeripheralCrateManager");
	//	sendCommand("Halt", "EmuPeripheralCrate");
	//	sendCommand("Halt", "EmuDAQManager");
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}


void emuSTEPManager::webDefault(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	// test number combo text
	static string test_txt_[] = 	
		{
			"Test 11: AFEB Counting Noise",
			"Test 12: AFEB Connectivity",
			"Test 13: AFEB Thresholds and Analog Noise",
			"Test 14: AFEB-ALCT Time Delays",
			"Test 15: CFEB DAQ-Path Noise",
			"Test 16: CFEB Connectivity",
			"Test 17: CFEB Delay calibration",
			"Test17b: CFEB Gain test",
			"Test 18: CFEB Comparator Counting Noise",
			"Test 19: CFEB Thresholds and Analog Noise",
			"Test 20: CFEB Comparator Output Timer",
			"Test 21: CFEB Comparator Logic",
			"Test 25: ALCT Trigger Test",
			"Test 30: TMB  timing"
		};

	// test number combo values
	static string test_val_[] = {"11", "12", "13", "14", "15", "16", "17", "17b", "18", "19", "20", "21", "25","30"};

	// endcap combo text and values
	static string endcap_txt_[] = {"ME+", "ME-"};
	static string endcap_val_[] = {"p",   "m"};

	// chamber type combo text and values
	static string chamtype_txt_[] = {"1/1", "1/2", "1/3", "2/1", "2/2", "3/1", "3/2", "4/1", "4/2"};
	static string chamtype_val_[] = {"1.1", "1.2", "1.3", "2.1", "2.2", "3.1", "3.2", "4.1", "4.2"};

	// chamber number labels and values (identical)
	static string chamnum_txt_[] = 
		{"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18",
		 "19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36"};

	out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << cgicc::head() << std::endl;
	*out << cgicc::title("emuSTEPManager") << cgicc::head() << cgicc::body() << std::endl;
	xgi::Utils::getPageHeader
		(out,
		 "emuSTEPManager",
		 getApplicationDescriptor()->getContextDescriptor()->getURL(),
		 getApplicationDescriptor()->getURN(),
		 "/daq/xgi/images/Application.gif"
			);

	// Config listbox
    *out << "Test Apps: " << cgicc::br() << std::endl;
    std::set<xdaq::ApplicationDescriptor *> apps = getAppsList("emuSTEPTests");
    std::set<xdaq::ApplicationDescriptor *>::iterator itr;

    for (itr=apps.begin(); itr!= apps.end(); ++itr) 
	{
        std::ostringstream st;
        st << (*itr)->getClassName() << "-" << (*itr)->getInstance();
        std::string applink = (*itr)->getContextDescriptor()->getURL()+"/"+(*itr)->getURN();
        *out << "<a href="+applink+">"+st.str()+"</a><br>" << std::endl;
    }

    *out << cgicc::hr() << std::endl;


	// config file selector form
	{
		beginForm(out, "chamber_select");

		beginFieldSet(out, "Chamber Configuration");
		Table t1(2, 2);
		// test type selector
	    Label(t1.cell(0,0), "Test Type&nbsp;&nbsp;&nbsp;&nbsp;");
		Combo(t1.cell(0,1), "runtype", run_type_, test_txt_, test_val_, 14);

		Label(t1.cell(1,0), "Chamber Selector&nbsp;&nbsp;");
		// endcap selector
		Combo(t1.cell(1,1), "endcap", endcap_, endcap_txt_, endcap_val_, 2);
		
		// chamber type selector
		Combo(t1.cell(1,1), "chamtype", chamtype_, chamtype_txt_, chamtype_val_, 9);

		// chamber number selector
		Combo(t1.cell(1,1), "chamnum", chamnum_, chamnum_txt_, chamnum_txt_, 36);

		// Make File name button
		Button(t1.cell(1,1), "Make File Name");
		t1.flush(out);

		endFieldSet(out);

		endForm(out);
	}// end chamber selector

	Table t2(1, 2);
	// configure form
	{
		beginForm(out, "Configure");

		beginFieldSet(out, "Run Parameters");
		Table t(4, 2);

		Label(t.cell(0,0), "XML files directory");
		Label(t.cell(0,1), string(getenv("STEPHOME")) + "/data/xml");

		// xml file name box. Automatically filled by chamber selector
		// the user still can modify that name and then press Init
	    Label(t.cell(1,0), "Chamber XML config file&nbsp;&nbsp;");
		TextField(t.cell(1,1), "xmlFileName", xmlFileName, 40);

		Label(t.cell(2,0), "Test config file");
		Label(t.cell(2,1), "test_config/test_config.xml"); 

		// number of events text field
		Label(t.cell(3,0), "Events to collect&nbsp;&nbsp;");
		Label(t.cell(3,1), emuSTEPApplication::toString(nevents_)); // total event number is calculated automatically from test_config.xml

		t.flush(out);
		endFieldSet(out);

		// DDU fiber mask
		beginFieldSet(out, "DDU fiber mask");
		Table fmask_table(2, 16, 1, true); // table makes it look neater
		for (int i = 0; i < 16; i++)
		{
			Label    (fmask_table.cell(0, i), emuSTEPApplication::toString(i) + " ");
			Checkbox (fmask_table.cell(1, i), "ddu_in_" + emuSTEPApplication::toString(i), ddu_in_[i]);
		}
		fmask_table.flush(out);
		endFieldSet(out);

		beginFieldSet(t2.cell(0,0), "Actions");

		// INIT button
		Button(t2.cell(0,0), "<b>&nbsp;&nbsp;&nbsp;Init&nbsp;&nbsp;&nbsp;</b>");

		endForm(t2.cell(0,0));
	} // end configure form

	FormButton(t2.cell(0,0), "<b>&nbsp;&nbsp;&nbsp;Show&nbsp;&nbsp;&nbsp;</b>",   "Show");
	endFieldSet(out); // Actions

	beginFieldSet(t2.cell(0,1), "Run");
	// run form
	{
		beginForm(t2.cell(0,1), "Start");
		TextField(t2.cell(0,1), "runnumber", run_number_.toString(), 13);
		br(t2.cell(0,1));
		Button(t2.cell(0,1), "<b>&nbsp;&nbsp;&nbsp;Start&nbsp;&nbsp;</b>");
		br(t2.cell(0,1));
		endForm(t2.cell(0,1));
	}

	FormButton(t2.cell(0,1), "<b>&nbsp;&nbsp;&nbsp;Stop&nbsp;&nbsp;&nbsp;</b>", "Stop");
	endFieldSet(t2.cell(0,1)); // run

	t2.flush(out);

 	*out << cgicc::body() << cgicc::html() << std::endl;

}


/*
 *out << cgicc::select().set("name", "crates") << std::endl;
 std::set<xdaq::ApplicationDescriptor *>::iterator i;
 for (i=apps.begin(); i!= apps.end(); ++i) {
 string name = (*i)->getClassName() + ":" + (*i)->getLocalId().toString();
 *out << cgicc::option()
 .set("value", name);
 }
 *out << cgicc::select() << cgicc::br() << std::endl;
 */

void emuSTEPManager::webConfigure(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
    string value;

	// test number was stored into run_type in webChamSel
    setParameter("emuSTEPTests", "RunType", "xsd:string", run_type_.toString());

    xmlFileName = getCGIParameter(in, "xmlFileName");

    LOG4CPLUS_INFO(getApplicationLogger(), "STEP> Chamber config: " << xmlFileName);
    setParameter("emuSTEPTests", "xmlFileName", "xsd:string", xmlFileName);
    
	// build ddu fiber mask from checkboxes
	int ddu_fiber_mask = 0;
	for (int i = 0; i < 16; i++)
	{
		ddu_in_[i] = getCGIParameter(in, "ddu_in_" + emuSTEPApplication::toString(i)).compare("1") == 0;
		if (ddu_in_[i]) ddu_fiber_mask |= (1 << i);
	}
    setParameter("emuSTEPTests", "ddu_fiber_mask", "xsd:integer", emuSTEPApplication::toString(ddu_fiber_mask));

    if (error_message_.empty()) 
	{
//        submit(configure_signature_);

    	LOG4CPLUS_INFO(getApplicationLogger(), "Configuring for STEP test: " << run_type_.toString());
	    sendCommand("step_configure", "emuSTEPTests");
    }
 
    webRedirect(in, out);
}

void emuSTEPManager::webChamSel(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
	run_type_ 	= getCGIParameter(in, "runtype");
	endcap_ 	= getCGIParameter(in, "endcap");
    chamtype_ 	= getCGIParameter(in, "chamtype");
    chamnum_ 	= getCGIParameter(in, "chamnum");

	test_config_struct tcs;

	if (config_reader.read_test_config("test_config.xml", &tcs) == -1)
	{
		LOG4CPLUS_ERROR(getApplicationLogger(), "STEP> cannot find test_config.xml");
		return;
	}
/*
	cout << "test config structure" << endl;
	for (unsigned i = 0; i < sizeof (tcs) / sizeof (int); i++)
		cout << *(((int*)(&tcs)) + i) << endl;
*/

	// calculate number of events
	std::map <string, int> def_num_events; 
	def_num_events["11"] = tcs.t11.events_total;
	def_num_events["12"] = tcs.t12.events_per_strip * 6; 
	def_num_events["13"] = 
		tcs.t13.events_per_threshold * 
		tcs.t13.thresholds_per_tpamp * 
		tcs.t13.tpamps_per_run;
	def_num_events["14"] = 
		tcs.t14.events_per_delay * 
		tcs.t14.delays_per_run; 
	def_num_events["15"] = tcs.t15.events_total; 
	def_num_events["16"] = tcs.t16.events_per_layer * 3; // scans 2 layers at a time 
	def_num_events["17"] = 
		tcs.t17.events_per_delay * 
		tcs.t17.delays_per_strip * 
		tcs.t17.strips_per_run;   
	def_num_events["17b"] = 
	        tcs.t17b.events_per_pulsedac * 
	        tcs.t17b.pulse_dac_settings * 
		tcs.t17b.strips_per_run;   
	def_num_events["18"] = tcs.t18.events_total; 
	def_num_events["19"] =  
		tcs.t19.events_per_thresh * 
		tcs.t19.threshs_per_tpamp * 
		tcs.t19.dmb_tpamps_per_strip * 
		tcs.t19.strips_per_run;
	def_num_events["20"] = 0; 
	def_num_events["21"] = 
		tcs.t21.events_per_hstrip * 
		tcs.t21.hstrips_per_run;
	def_num_events["25"] =
	        tcs.t25.trig_settings *
	        tcs.t25.events_per_trig_set;
	def_num_events["30"] = 
		tcs.t30.events_per_delay * 
		tcs.t30.tmb_l1a_delays_per_run;

	// construct the xml file name according to convention "p2.1.01/t15.xml"
	xmlFileName = 
		endcap_.toString() + 
		chamtype_.toString() + 
		"." + 
		chamnum_.toString() +
		"/t" +
		run_type_.toString() + 
		".xml";

	// find default numbef of events
	nevents_ = def_num_events[run_type_];

    LOG4CPLUS_INFO(getApplicationLogger(), "STEP> Chamber config: " << xmlFileName);
    
    webRedirect(in, out);
}


void emuSTEPManager::webStart(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
	string value;

//    fireEvent("Start");
    value = getCGIParameter(in, "runnumber");
    if (value.empty()) 
	{ 
		LOG4CPLUS_ERROR(getApplicationLogger(), "STEP> Please set run number.\n");
	}
	else
	{
		run_number_ = strtol(value.c_str(), NULL, 0); // this is run number from text box

		LOG4CPLUS_INFO(getApplicationLogger(), "STEP> RunNumber: " << run_number_);
		setParameter("emuSTEPTests", "RunNumber", "xsd:unsignedLong", emuSTEPApplication::toString(run_number_));

		LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: " << run_type_.toString());

		sendCommand("step_start", "emuSTEPTests");

		run_number_++;

		// store history
		(new emuSTEPConfigRead)->write_last_used
		(
			endcap_, 
			chamtype_, 
			chamnum_, 
			run_type_, 
			run_number_, 
			ddu_in_
		);

	}
    webRedirect(in, out);
}

void emuSTEPManager::webShow(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
//    fireEvent("Show");
   	LOG4CPLUS_INFO(getApplicationLogger(), "Show info command");
    sendCommand("step_show", "emuSTEPTests");

    webRedirect(in, out);
}

void emuSTEPManager::webHalt(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
//    submit(halt_signature_);

	sendCommand("step_stop", "emuSTEPTests");

    webRedirect(in, out);
}


void emuSTEPManager::webReset(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
    resetAction();

    webHalt(in, out);
}

void emuSTEPManager::webRedirect(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception)
{
    string url = in->getenv("PATH_TRANSLATED");

    HTTPResponseHeader &header = out->getHTTPResponseHeader();

    header.getStatusCode(303);
    header.getReasonPhrase("See Other");
    header.addHeader("Location",
            url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

bool emuSTEPManager::configureAction(toolbox::task::WorkLoop *wl)
{
    fireEvent("Configure");

    return false;
}

bool emuSTEPManager::haltAction(toolbox::task::WorkLoop *wl)
{
    fireEvent("Halt");

    return false;
}


void emuSTEPManager::resetAction() throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(begin)");

    fsm_.reset();
    state_ = fsm_.getStateName(fsm_.getCurrentState());

    LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(end)");
}

xoap::MessageReference emuSTEPManager::createReply(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	std::string command = "";

	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(
			command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

	return reply;
}


void emuSTEPManager::fireEvent(std::string name)
		throw (toolbox::fsm::exception::Exception)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));

	fsm_.fireEvent(event);
}

