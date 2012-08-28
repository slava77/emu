/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini                                    *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#include <iostream>
#include <iomanip>

#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/Crate.h"
#include "emuSTEPTests.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"
#include "xoap/domutils.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "xgi/Utils.h"

#include "xcept/tools.h"

#include "log4cplus/helpers/sleep.h" // sleepmillis()y

// #include "emu/fed/DDU.h"
// #include "emu/fed/VMELock.h"

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";

//
// provides factory method for instantion of SimpleWeb application
//
XDAQ_INSTANTIATOR_IMPL(emuSTEPTests)

emuSTEPTests::emuSTEPTests(xdaq::ApplicationStub * s)
  throw (xdaq::exception::Exception): emuSTEPApplication(s)
{
    run_type_ = "";
    run_number_ = 0;
    max_events_ = 0;
	alctpulseamp_ = 0;
	ddu_fiber_mask_ = 0xffff;
    xmlFileName_ = "";
    astub =s;
	xdata::InfoSpace *i = getApplicationInfoSpace();
	i->fireItemAvailable("RunType", &run_type_);
	i->fireItemAvailable("RunNumber", &run_number_);
	i->fireItemAvailable("xmlFileName", &xmlFileName_);
	i->fireItemAvailable("maxEvents", &max_events_);
	i->fireItemAvailable("alctpulseamp", &alctpulseamp_);
	i->fireItemAvailable("ddu_fiber_mask", &ddu_fiber_mask_);
	i->addItemChangedListener ("RunType", this);
	i->addItemChangedListener ("RunNumber", this);
	i->addItemChangedListener ("xmlFileName", this);
	i->addItemChangedListener ("maxEvents", this);
	i->addItemChangedListener ("alctpulseamp", this);
	i->addItemChangedListener ("ddu_fiber_mask", this);
	xgi::bind(this,&emuSTEPTests::Default, "Default");

	xoap::bind(this, &emuSTEPTests::on_step_configure, "step_configure", XDAQ_NS_URI);
	xoap::bind(this, &emuSTEPTests::on_step_start,     "step_start", XDAQ_NS_URI);
	xoap::bind(this, &emuSTEPTests::on_step_stop,     "step_stop", XDAQ_NS_URI);
	xoap::bind(this, &emuSTEPTests::on_step_show,      "step_show", XDAQ_NS_URI);

	tt = new test_task(this);

}

xoap::MessageReference emuSTEPTests::on_step_configure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{

	// read test config file

	if (config_reader.read_test_config("test_config.xml", &tcs) == -1)
	{
		LOG4CPLUS_ERROR(getApplicationLogger(), "STEP> cannot find test_config.xml");
	}
	else
	{
/*
  cout << "test config structure" << endl;
  for (unsigned i = 0; i < sizeof (tcs) / sizeof (int); i++)
  cout << *(((int*)(&tcs)) + i) << endl;
*/

//	submit(configure_signature_);
		string xmlname;

		ccb = NULL;	   
		tmb = NULL;	   
		alct = NULL;   
		dmb = NULL;	   
		chamber = NULL;
		crate = NULL;  
		//emuController = NULL;
		emuEndcap=NULL;
		ddu = NULL;
		xmlname = getenv("DATADIR");
		xmlname += "/xml/";
		xmlname += xmlFileName_.toString();
		std::cout << "xmlname=" << xmlname << std::endl << std::flush;
		LOG4CPLUS_INFO(getApplicationLogger(), "using xml file: " << xmlname);
		/*
		emuController = new emu::pc::EmuController();
		emuController->SetConfFile(xmlname.c_str());
		emuController->init();
		LOG4CPLUS_INFO(getApplicationLogger(), "Created EmuController");
		*/
		std::cout << "Before parser" << std::endl << std::flush;
		emu::pc::XMLParser parser;
		std::cout << "After parser"<< std::endl << std::flush;
		parser.parseFile( xmlname );
		std::cout << "After parsing"<< std::endl << std::flush;
		LOG4CPLUS_INFO(getApplicationLogger(), "Configuring EmuEndcap: "); 
		emuEndcap = parser.GetEmuEndcap();
		std::cout << "After GetEmuEndcap"<< std::endl << std::flush;

		// get broadcast components
		//vector<emu::pc::Crate *> tmpcrate=emuController->GetEmuEndcap()->broadcast_crate();
// 		vector<emu::pc::Crate *> tmpcrate=emuEndcap->broadcast_crate();
// 		broadcastCrate = tmpcrate[0];
// 		unsigned int ib=(broadcastCrate->daqmbs()).size()-1;
// 		broadcastDMB = (broadcastCrate->daqmbs())[ib];
// 		broadcastTMB = (broadcastCrate->tmbs())[0];
// 		broadcastALCT = broadcastTMB->alctController();
// //		broadcastRAT  = broadcastTMB->getRAT();
// //		broadcastMPC  = broadcastCrate->mpc();
// 		broadcastCCB  = broadcastCrate->ccb();

		//crates = emuController->crates();
		crates = emuEndcap->crates();
		LOG4CPLUS_INFO(getApplicationLogger(), "number of crates: " << crates.size());


		cout<<"size of crates "<<crates.size()<<endl;

		// check that crates and chambers are found
		for(unsigned i = 0; i < crates.size(); i++) 
		{
		  if(crates[i]) 
		   {
		     if(crates[i]->IsAlive()){
		       //configure(0) powers on chamber (if not done already)
		       //and writes configuration into flash
		       crates[i]->configure(0);
		       //hard reset ensures configuration is uploaded to eproms
		       crates[i]->ccb()->HardReset_crate();
		     }
		    
				LOG4CPLUS_INFO(getApplicationLogger(), "crate found: #" << i );
				chambers = crates[i]->chambers();
				cout<<"size of chambers "<<chambers.size()<<endl;
				for (unsigned j = 0; j < chambers.size(); j++)
				{
					if (chambers[j])
					{
						LOG4CPLUS_INFO(getApplicationLogger(), "\tchamber found: #" << j );
						crate = crates[i];
						chamber = chambers[j];
						tmbs = crate->tmbs();
						tmb = tmbs[0];
						// pick up the only tmb available
						if (tmb == NULL) LOG4CPLUS_ERROR(getApplicationLogger(), "tmb object is NULL" );
						alct = tmb->alctController();
						// pick up ccb 
						ccb = crate->ccb();
						if (ccb == NULL) LOG4CPLUS_ERROR(getApplicationLogger(), "ccb object is NULL" );
						dmbs = crate->daqmbs();
						cout<<"afterDMB size "<<dmbs.size()<<endl;
						// pick up the only dmb available
						dmb = dmbs[0];
						if (dmb == NULL) LOG4CPLUS_ERROR(getApplicationLogger(), "dmb object is NULL" );
						ddus = crate->ddus();
						cout<<"size of DDU vector "<<ddus.size()<<endl;
						ddu = ddus[0];
						if (ddu == NULL) LOG4CPLUS_ERROR(getApplicationLogger(), "ddu object is NULL" );


					}
				}
			}
		}


		//emuController->configure();
		LOG4CPLUS_INFO(getApplicationLogger(), "EmuEndcap configured.");

		// have to set ccb mode once more separately, as it is screwed up inside ccb->configure
		ccb->setCCBMode(emu::pc::CCB::VMEFPGA);
		// disable all triggers 
		ccb_DisableL1a(ccb);

		//configuring DDU (in future from XML file)
		ddu->writeFlashKillFiber(1);
		ddu->writeGbEPrescale(0xF0F0);
		ddu->writeFakeL1(0x8787);
		// try {
		//   emu::fed::DDU fed_ddu( ddu->slot() );
		//   emu::fed::VMELock fed_mutex( "/tmp/STEP_LOCK" );
		//   fed_ddu.setMutex( &fed_mutex );
		//   fed_ddu.resetFPGA( emu::fed::DDUFPGA );
		// } catch (emu::fed::exception::Exception &e) {
		//   std::ostringstream error;
		//   error << "Exception communicating with DDU";
		//   XCEPT_DECLARE_NESTED(xcept::Exception, e2, error.str(), e);
		//   LOG4CPLUS_ERROR( getApplicationLogger(), xcept::stdformat_exception_history( e2 ) );
		//   throw e2;
		// }

	}
	return createReply(message);
}


xoap::MessageReference emuSTEPTests::on_step_start(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	quit = false;
	tt->set_testn(run_type_.toString());
	tt->activate();
	
	return createReply(message);
}

xoap::MessageReference emuSTEPTests::on_step_stop(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	quit = true;
	
	return createReply(message);
}


// missing XML param
/* 
	alct->SetInvertPulse_(ON);
	alct->FillTriggerRegister_();
	alct->WriteTriggerRegister_();
*/

void emuSTEPTests::ccb_EnableL1aFromVme(emu::pc::CCB *ccb)
{
	// enable L1A and clct_pretrig from VME command in CCB
	unsigned csrb1 = 0x1aed; // also disable all other trigger sources
	ccb->WriteRegister(0x20, csrb1);
}

void emuSTEPTests::ccb_EnableL1aFromTmbL1aReq(emu::pc::CCB *ccb)
{
	// enable L1A from TMB L1A request line. 
	// clct_pretrig is not enabled since TMB will generate LCT for CFEBs
	unsigned csrb1 = 0x1edd; // also disable all other trigger sources
	ccb->WriteRegister(0x20, csrb1);
}

void emuSTEPTests::ccb_EnableL1aFromSyncAdb(emu::pc::CCB *ccb)
{
	// enable L1A and clct_pretrig from ALCT sync test pulse
	unsigned csrb1 = 0x12fd; // also disable all other trigger sources
	ccb->WriteRegister(0x20, csrb1);
}

void emuSTEPTests::ccb_EnableL1aFromASyncAdb(emu::pc::CCB *ccb)
{
	// enable L1A and clct_pretrig from ALCT Async test pulse
	unsigned csrb1 = 0x0afd; // also disable all other trigger sources
	ccb->WriteRegister(0x20, csrb1);
	cout<<"enable asynch pulse L1A "<<csrb1<<endl;
}

void emuSTEPTests::ccb_EnableL1aFromDmbCfebCalibX(emu::pc::CCB *ccb)
{
	// enable L1A and clct_pretrig from any of dmb_cfeb_calib signals
	unsigned csrb1 = 0x1af9; // also disable all other trigger sources
	ccb->WriteRegister(0x20, csrb1);
}

void emuSTEPTests::ccb_DisableL1a(emu::pc::CCB *ccb)
{
	// kill all triggers
	unsigned csrb1 = 0xffff;
	ccb->WriteRegister(0x20, csrb1);
}

void emuSTEPTests::ccb_GenerateL1A(emu::pc::CCB *ccb)
{
	ccb->WriteRegister(0x54, 0); // generate L1A and pretriggers
}

void emuSTEPTests::ccb_GenerateDmbCfebCalib0(emu::pc::CCB *ccb)
{
	// CFEB test pulse
	ccb->WriteRegister(0x8a, 0);
}

void emuSTEPTests::ccb_GenerateDmbCfebCalib1(emu::pc::CCB *ccb)
{
	// CFEB inject
	ccb->WriteRegister(0x8c, 0);
}

void emuSTEPTests::ccb_SetExtTrigDelay(emu::pc::CCB *ccb, unsigned delay)
{
	unsigned csrb5 = ccb->ReadRegister(0x28);
	csrb5 &= 0xff;
	//shifts delay by additional 128 (delay in UFL)
	//change from 8 to 7 (shift by 64) 
	csrb5 |= (delay << 7);
	ccb->WriteRegister(0x28, csrb5);
	
}

void emuSTEPTests::tmb_EnableClctExtTrig(emu::pc::TMB * tmb)
{
  tmb->SetClctExtTrigEnable(1);
  int data_to_write = tmb->FillTMBRegister(0x68);
  tmb->WriteRegister(0x68 ,data_to_write);
  cout<<"enable CLCT ext trig"<<endl;
}


void emuSTEPTests::dmb_trighalfx(emu::pc::DAQMB* dmb, int ihalf)
{
    int hs[6];
    int i,j,k,pln,crd;
    int chan[5][6][16];
    for(i=0;i<5;i++)
    {
		for(j=0;j<6;j++)
		{
			for(k=0;k<16;k++)
			{
				chan[i][j][k]=NORM_RUN;
			}
		}
    }
    for(i=0;i<6;i+=2)
    {
		hs[i]=ihalf;
		hs[i+1]=ihalf;
    }
    for(crd=0;crd<5;crd++)
    {
		for(pln=0;pln<6;pln++)
		{
			dmb->halfset(crd,pln,hs[pln],chan);
		}
    }
    for(i=0;i<5;i++)
    {
		for(j=0;j<6;j++)
		{
			for(k=0;k<16;k++)
			{
				printf(" %d",chan[i][j][k]);
			}
			printf("\n");
		}
		printf("\n");
    }
    printf(" chan filled \n");
    dmb->chan2shift(chan);
}



void emuSTEPTests::prepare_to_test()
{

	static bool first = true;

   	LOG4CPLUS_INFO(getApplicationLogger(), "Configuring emuDAQManager for STEP test: " << run_type_.toString());

	// this will make it understand my run type and max events.
	setParameter("EmuDAQManager", "globalMode",        "xsd:boolean",      "true");
	setParameter("EmuDAQManager", "maxNumberOfEvents", "xsd:integer",       max_events_.toString());
	// run type must contain "STEP" for DAQManager to recognize it
	setParameter("EmuDAQManager", "runType",           "xsd:string",       "STEP_" + run_type_.toString());
	setParameter("EmuDAQManager", "runNumber",         "xsd:unsignedLong", run_number_.toString());

	if (first) // FCrate crashes if configured more than once
	{
		// configure DDU
		sendCommand("Configure", "EmuFCrate");
		sendCommand("Enable", "EmuFCrate");
		first = false;
	}
	// set fiber mask 
	setParameter("EmuFCrate",  "step_killfiber", "xsd:unsignedInt", ddu_fiber_mask_.toString());

	//configure DAQManager
	sendCommand("Configure", "EmuDAQManager");
	sendCommand("Enable", "EmuDAQManager"); // open data file

	LOG4CPLUS_INFO(getApplicationLogger(), "Resync");
	ccb->l1aReset();
	// resync DDU via VME
	sendCommand("Passthru", "EmuFCrate");

	::sleep(8); // looks like DDU needs this delay...

	LOG4CPLUS_INFO(getApplicationLogger(), "Start trigger");
	ccb->startTrigger(); // necessary for tmb to start triggering
	// alct should work with just L1A reset and bc0

	LOG4CPLUS_INFO(getApplicationLogger(), "BC0");
	ccb->bc0(); 

	//::sleep(20);
}

void emuSTEPTests::finish_test()
{
	int i;
	LOG4CPLUS_INFO(getApplicationLogger(), "Waiting for data to end up in the file");

	// read STEPFinished param from DAQManager until it's true
	for (i = 0; i < 600; i++) // 10 min max
	{
		::sleep(1);
		string finished = getParameter ("EmuDAQManager", "STEPFinished", "xsd:boolean");
		cout << setw(3) << setfill(' ') << "\r" << i << " sec" << flush;
//		cout << "finished: " << finished << endl;
		if (finished == "true" || quit) break;
	}
	cout << setw(3) << setfill(' ') << "\r" << i << " sec" << endl;

	// disable all triggers 
	ccb_DisableL1a(ccb);

	sendCommand("Halt", "EmuDAQManager"); // close data file
//	sendCommand("Disable", "EmuFCrate");
}

void emuSTEPTests::test_11()
{
	max_events_ = tcs.t11.events_total;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 11, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromTmbL1aReq(ccb);
	// end "should be XML" section

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 11 finished");
}

void emuSTEPTests::test_12()
{
	int strips = 6; // strips to scan, never changes
	int ev_per_strip = tcs.t12.events_per_strip; // from config file
	max_events_ = ev_per_strip * strips;
	int j;

	string chamtype = alct->GetChamberType(); // returns "MEXX"

	// map of test pulse amplitudes for all chamber types, taken from config file
	std::map <string, int*> tpamp_map;
	tpamp_map["ME11"] = &tcs.t12.alct_test_pulse_amp_11;
	tpamp_map["ME12"] = &tcs.t12.alct_test_pulse_amp_12;
	tpamp_map["ME13"] = &tcs.t12.alct_test_pulse_amp_13;
	tpamp_map["ME21"] = &tcs.t12.alct_test_pulse_amp_21;
	tpamp_map["ME22"] = &tcs.t12.alct_test_pulse_amp_22;
	tpamp_map["ME31"] = &tcs.t12.alct_test_pulse_amp_31;
	tpamp_map["ME32"] = &tcs.t12.alct_test_pulse_amp_32;
	tpamp_map["ME41"] = &tcs.t12.alct_test_pulse_amp_41;
	tpamp_map["ME42"] = &tcs.t12.alct_test_pulse_amp_42;

	ostringstream waste;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 12, max events: " << strips * ev_per_strip);
	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromSyncAdb(ccb);
	// the two lines below make CFEB data to appear. Workaround for missing L1As at ALCT
	tmb_EnableClctExtTrig(tmb);
	//ccb_SetExtTrigDelay(ccb, 12); // 12 is arbitrary, L1ADelay must be this value + 128 in UF
	// end "should be XML" section

	for (int i = 0; i < strips; i++) // strip loop
	{
		// every ev_per_strip events switch test strip
		LOG4CPLUS_INFO(getApplicationLogger(), "Setting mask for strip:  " << i + 1);

		alct->SetUpPulsing
		(
			alctpulseamp_ = *(tpamp_map[chamtype]), 
			PULSE_LAYERS, 
			(1 << i), // strip mask
			ADB_SYNC
		);
		std::cout<<"after setup pulse alctpulseamp_ "<<alctpulseamp_<<" PULSE_Layers "<<PULSE_LAYERS<<" ADB_SYNC? "<<ADB_SYNC<<std::endl;

		ccb->RedirectOutput (&waste); // ccb prints a line on each test pulse - waste it
			
		for (j = 1; j <= ev_per_strip; j++) // pulse loop
		{
			ccb->GenerateAlctAdbSync();
			usleep(1000);
			if (j % 100 == 0) 
				cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
			if (quit)
			{
				finish_test();
				LOG4CPLUS_INFO(getApplicationLogger(), "Test 12 stopped");
				return;
			}
		}
		cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		cout << endl;
		ccb->RedirectOutput (&cout); // get back ccb output
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 12 finished");
}

void emuSTEPTests::test_13()
{
	int tpamps_per_run = tcs.t13.tpamps_per_run; // how many test pulse amplitudes to scan
	int num_thresh     = tcs.t13.thresholds_per_tpamp; // number of thresholds to scan with each test pulse amp
	int first_thresh   = tcs.t13.threshold_first; // first thresh
	int thresh_step    = tcs.t13.threshold_step; // threshold step

	// events per test pulse amp and per threshold is automatically calculated to accomodate user's max events
	int ev_per_threshold = tcs.t13.events_per_threshold; // events per each threshold
	// total events
	max_events_ = tcs.t13.events_per_threshold * tcs.t13.thresholds_per_tpamp * tcs.t13.tpamps_per_run;
	int j;

	ostringstream waste;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 13, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromSyncAdb(ccb);
	// the two lines below make CFEB data to appear. Workaround for missing L1As at ALCT
	tmb_EnableClctExtTrig(tmb);
	//ccb_SetExtTrigDelay(ccb, 34); // 12 is arbitrary, L1ADelay must be this value + 128 in UF
	// end "should be XML" section

	for (int i = 0; i < tpamps_per_run; i++) // tpamp loop
	{
		for (int k = 0; k < num_thresh; k++)
		{
			int cur_thresh = first_thresh + thresh_step*k;
			// every ev_per_strip events switch test strip
			LOG4CPLUS_INFO(getApplicationLogger(), "Setting tpamp:  " << tcs.t13.tpamp_first + i * tcs.t13.tpamp_step << " thresh: " << cur_thresh);

			for (int c = 0; c <= alct->MaximumUserIndex(); c++)
				alct->SetAfebThreshold(c, cur_thresh);

			alct->WriteAfebThresholds();

			alct->SetUpPulsing
			(
				alctpulseamp_ = tcs.t13.tpamp_first + i * tcs.t13.tpamp_step, 
				PULSE_AFEBS, 
				0x7f, // afeb group mask
				ADB_SYNC
			);

			ccb->RedirectOutput (&waste); // ccb prints a line on each test pulse - waste it
			
			for (j = 1; j <= ev_per_threshold; j++) // pulse loop
			{
				ccb->GenerateAlctAdbSync();
				usleep(1000);
				if (j % 100 == 0) 
					cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
				if (quit)
				{
					finish_test();
					LOG4CPLUS_INFO(getApplicationLogger(), "Test 13 stopped");
					return;
				}
			}
			cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
			cout << endl;
			ccb->RedirectOutput (&cout); // get back ccb output
		}
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 13 finished");
}

void emuSTEPTests::test_14()
{
	max_events_ = tcs.t14.events_per_delay *  tcs.t14.delays_per_run;

	string chamtype = alct->GetChamberType(); // returns "MEXX"
	int j;

	ostringstream waste;

	LOG4CPLUS_INFO(getApplicationLogger(), 
				   "Starting STEP test: 14, max events: " << tcs.t14.events_per_delay *  tcs.t14.delays_per_run);

	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromASyncAdb(ccb);
	// the two lines below make CFEB data to appear. Workaround for missing L1As at ALCT
	tmb_EnableClctExtTrig(tmb);
	//ccb_SetExtTrigDelay(ccb, 34); // 12 is arbitrary, L1ADelay must be this value + 128 in UF
	// end "should be XML" section

	alct->SetUpPulsing
	(
		tcs.t14.alct_test_pulse_amp, 
		PULSE_AFEBS, 
		0x3fff, // all afebs
		ADB_ASYNC
	);

	for (int i = 0; i < tcs.t14.delays_per_run; i++) // delay loop
	{
		int cur_delay = tcs.t14.delay_first + tcs.t14.delay_step * i;

		// every ev_per_strip events switch test strip
		LOG4CPLUS_INFO(getApplicationLogger(), "Setting delay:  " << cur_delay);

		for (int k = 0; k <= alct->MaximumUserIndex(); k++) 
			alct->SetAsicDelay(k, cur_delay);

		alct->WriteAsicDelaysAndPatterns();

		ccb->RedirectOutput (&waste); // ccb prints a line on each test pulse - waste it
			
		for (j = 1; j <= tcs.t14.events_per_delay; j++) // pulse loop
		{
			ccb->GenerateAlctAdbASync();
			usleep(1000);
			if (j % 100 == 0) 
				cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
			if (quit)
			{
				finish_test();
				LOG4CPLUS_INFO(getApplicationLogger(), "Test 14 stopped");
				return;
			}
		}
		cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		cout << endl;
		ccb->RedirectOutput (&cout); // get back ccb output
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 14 finished");
}


void emuSTEPTests::test_15()
{
	int j;
	max_events_ = tcs.t15.events_total;

	ostringstream waste;

	LOG4CPLUS_INFO(getApplicationLogger(), 
				   "Starting STEP test: 15, max events: " <<  tcs.t15.events_total);

	prepare_to_test();

	// the below should be done via XML params
	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromVme(ccb);
	ccb_SetExtTrigDelay(ccb, 1); // 1 is arbitrary, L1ADelay must be this value + 128 in UF
	// end "should be XML" section

	ccb->RedirectOutput (&waste); // ccb prints a line on each test pulse - waste it
			
	for (j = 1; j <=  tcs.t15.events_total; j++) // pulse loop
	{
		ccb_GenerateL1A(ccb);
		usleep(10);
		if (j % 100 == 0) 
			cout << "\rTriggers: " << setw(3) << setfill(' ') << j << flush;
		if (quit)
		{
			finish_test();
			LOG4CPLUS_INFO(getApplicationLogger(), "Test 15 stopped");
			return;
		}
	}
	cout << "\rTriggers: " << setw(3) << setfill(' ') << j-1 << flush;
	cout << endl;
	ccb->RedirectOutput (&cout); // get back ccb output

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 15 finished");
}

void emuSTEPTests::test_16()
{
	int layers = 3; // layers to scan, never changes. Scans 2 layers at a time
	max_events_ = tcs.t16.events_per_layer * layers;
	int j;

	ostringstream waste;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 16, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromSyncAdb(ccb);
	// end "should be XML" section

	alct->SetUpPulsing
	(
		tcs.t16.alct_test_pulse_amp,
		PULSE_AFEBS, 
		0x7f, // AFEB mask - pulse all of them
		ADB_SYNC
	);

	// the below should be done via XML params
	tmb_EnableClctExtTrig(tmb);
	ccb_SetExtTrigDelay(ccb, 40); // L1ADelay must be this value + 128 in UF
	alct->SetInvertPulse_(ON);    
	alct->FillTriggerRegister_();
	alct->WriteTriggerRegister_();
	// end "should be XML" section

	for (int i = 0; i < layers; i++) // layer loop
	{
		// every events_per_layer events switch layer
		LOG4CPLUS_INFO(getApplicationLogger(), "Setting standby for layers: " << i*2 + 1 << " and " << i*2 + 2);

		// reprogram standby register to enable 2 layers at a time

		int standby_fmask[] = {066, 055, 033};
		int astandby;
		for (int lct_chip = 0; lct_chip < alct->MaximumUserIndex() / 6; lct_chip++)
		{
			astandby = standby_fmask[i];
			LOG4CPLUS_INFO(getApplicationLogger(), "Setting standby " << lct_chip << " to "  << hex << astandby << dec);
			for (int afeb = 0; afeb < 6; afeb++)
			{
				alct->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
			}
		}
		alct->WriteStandbyRegister_();

		ccb->RedirectOutput (&waste); // ccb prints a line on each test pulse - waste it
			
		for (j = 1; j <= tcs.t16.events_per_layer; j++) // pulse loop
		{
			ccb->GenerateAlctAdbSync();
			usleep(10);
			if (j % 100 == 0) 
				cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
			if (quit)
			{
				finish_test();
				LOG4CPLUS_INFO(getApplicationLogger(), "Test 16 stopped");
				return;
			}
		}
		cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		cout << endl;
		ccb->RedirectOutput (&cout); // get back ccb output
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 16 finished");
}

void emuSTEPTests::test_17()
{

	int j;
	max_events_ = tcs.t17.events_per_delay * tcs.t17.delays_per_strip * tcs.t17.strips_per_run;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 17, max events: " << max_events_);

	prepare_to_test();
	//test values take them maybe out
	char dmbstatus[11];
	int cfeb_clk_delay=31;
	int pre_block_end=7;
	int feb_cable_delay=0;
	int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
	float dac=1.00;
	std::cout<<"start dword: "<<dword<<std::endl;
	// the below should be done via XML params
	tmb->DisableALCTInputs();
	tmb->DisableCLCTInputs();
	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromDmbCfebCalibX(ccb);
	ccb_SetExtTrigDelay(ccb, 34);
	std::cout<<"extTrigDelay "<<34<<std::endl;
	//dmb->set_dac(0, (float)tcs.t17.dmb_test_pulse_amp * 5. / 256.);
	//loop over strips - 16
	
	dmb->calctrl_fifomrst();
          usleep(5000);
	   
          dmb->restoreCFEBIdle();
          dmb->restoreMotherboardIdle();
	  //float dac=1.00;
          dmb->set_cal_dac(dac,dac);
          dmb->setcaldelay(dword);

	  //dmb->settrgsrc(1);
	  dmb->fxpreblkend(pre_block_end);
	  dmb->SetCfebClkDelay(cfeb_clk_delay);
	  dmb->setfebdelay(dmb->GetKillFlatClk());
	  dmb->load_feb_clk_delay();
	  if(dmb->GetCfebCableDelay() == 1){
	    // In the calibration, we set all cfeb_cable_delay=0 for all DMB's for
	    // timing analysis.  If the collision setting is normally cfeb_cable_delay=1,
	    // in order to ensure the proper timing of the CFEB DAV, we will need to 
	    // adjust cfeb_dav_cable_delay to follow the change of cfeb_cable_delay...
	    dmb->SetCfebDavCableDelay(dmb->GetCfebDavCableDelay()+1);
	    std::cout<<"in cable delay==1 : "<<dmb->GetCfebDavCableDelay()+1<<"/"<<dmb->GetCableDelay()<<std::endl;
	  }
	  dmb->setcbldly(dmb->GetCableDelay());
	  dmb->SetCfebCableDelay(feb_cable_delay);
	  dmb->calctrl_global();
	  //
	  // Now check the DAQMB status.  Did the configuration "take"?
	  std::cout << "After config: check status " << std::endl;
	  usleep(50);
	  dmb->dmb_readstatus(dmbstatus);
	  if( ((dmbstatus[9]&0xff)==0x00 || (dmbstatus[9]&0xff)==0xff) || 
 	      ((dmbstatus[8]&0xff)==0x00 || (dmbstatus[8]&0xff)==0xff) ||
 	      ((dmbstatus[7]&0xff)==0x00 || (dmbstatus[7]&0xff)==0xff) ||
 	      ((dmbstatus[6]&0xff)==0x00 || (dmbstatus[6]&0xff)==0xff) ||
	      ((dmbstatus[0]&0xff)!=0x21)                              ) {
	    std::cout << "... config check not OK for DMB "  << std::endl;
	  }
	  


	for (int i = 0; i < tcs.t17.strips_per_run; i++) // strip loop
	{
		LOG4CPLUS_INFO(getApplicationLogger(), "Strip: " << i * tcs.t17.strip_step + tcs.t17.strip_first - 1);

		dmb->set_ext_chanx(i * tcs.t17.strip_step + tcs.t17.strip_first - 1); // strips start from 1 in config file (is that important for analysis?)
		dmb->buck_shift();
		dmb->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML

		for (int k = 0; k < tcs.t17.delays_per_strip; k++) // delay loop
		{
		  int nsleep = 100;  


		  //
		  //implement the cal0 setup process:
		  std::cout << "DMB setup for CFEB Time, k= " <<k<< std::endl;
		  //
		  //Start the setup process:
		  int timesetting =k%10;
		  dmb->set_cal_tim_pulse(timesetting+5);
		  //dmb->setcaldelay (k * tcs.t17.delay_step + tcs.t17.delay_first);
		  ::usleep(nsleep);
		  cout<<"after calibration delay"<<endl;
		  log4cplus::helpers::sleepmillis(400);
		  std::cout<<"ccb l1a delay "<<ccb->Getl1adelay()<<std::endl;
			
		  LOG4CPLUS_INFO(getApplicationLogger(), "time was set to: " << timesetting<<" delay_first/step "<<tcs.t17.delay_first<<"/"<<tcs.t17.delay_step<<" strip "<<i);
  //original test
  //dmb->setcaldelay (k * tcs.t17.delay_step + tcs.t17.delay_first);

			for (j = 1; j <= tcs.t17.events_per_delay; j++) // pulse loop
			{
				ccb_GenerateDmbCfebCalib0(ccb); // pulse
				::usleep(100);
				log4cplus::helpers::sleepmillis(1);
				if (j % 100 == 0) 
					cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
				if (quit)
				{
					finish_test();
					LOG4CPLUS_INFO(getApplicationLogger(), "Test 17 stopped");
					return;
				}
			}
			cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
			cout << endl;
		}
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 17 finished");
}


void emuSTEPTests::test_17b()
{
	int j;
	max_events_ = tcs.t17b.events_per_pulsedac * tcs.t17b.pulse_dac_settings * tcs.t17b.strips_per_run;
	//dmb->set_dac(0, (float)tcs.t17b.dmb_test_pulse_amp * 5. / 256.);

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 17b, max events: " << max_events_);
	prepare_to_test();
	//test values take them maybe out
	char dmbstatus[11];
	int cfeb_clk_delay=31;
	int pre_block_end=7;
	int feb_cable_delay=0;
	int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
	float dac=1.00;
	// the below should be done via XML params
	tmb->DisableALCTInputs();
	tmb->DisableCLCTInputs();
	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromDmbCfebCalibX(ccb);
	ccb_SetExtTrigDelay(ccb, 34);
	//loop over strips - 16
	
	dmb->calctrl_fifomrst();
          usleep(5000);
	   
          dmb->restoreCFEBIdle();
          dmb->restoreMotherboardIdle();
	  //float dac=1.00;
          dmb->set_cal_dac(dac,dac);
          dmb->setcaldelay(dword);

	  //dmb->settrgsrc(1);
	  dmb->fxpreblkend(pre_block_end);
	  dmb->SetCfebClkDelay(cfeb_clk_delay);
	  dmb->setfebdelay(dmb->GetKillFlatClk());
	  dmb->load_feb_clk_delay();
	  if(dmb->GetCfebCableDelay() == 1){
	    // In the calibration, we set all cfeb_cable_delay=0 for all DMB's for
	    // timing analysis.  If the collision setting is normally cfeb_cable_delay=1,
	    // in order to ensure the proper timing of the CFEB DAV, we will need to 
	    // adjust cfeb_dav_cable_delay to follow the change of cfeb_cable_delay...
	    dmb->SetCfebDavCableDelay(dmb->GetCfebDavCableDelay()+1);
	    std::cout<<"in cable delay==1 : "<<dmb->GetCfebDavCableDelay()+1<<"/"<<dmb->GetCableDelay()<<std::endl;
	  }
	  dmb->setcbldly(dmb->GetCableDelay());
	  dmb->SetCfebCableDelay(feb_cable_delay);
	  dmb->calctrl_global();
	  //
	  // Now check the DAQMB status.  Did the configuration "take"?
	  std::cout << "After config: check status " << std::endl;
	  usleep(50);
	  dmb->dmb_readstatus(dmbstatus);
	  if( ((dmbstatus[9]&0xff)==0x00 || (dmbstatus[9]&0xff)==0xff) || 
 	      ((dmbstatus[8]&0xff)==0x00 || (dmbstatus[8]&0xff)==0xff) ||
 	      ((dmbstatus[7]&0xff)==0x00 || (dmbstatus[7]&0xff)==0xff) ||
 	      ((dmbstatus[6]&0xff)==0x00 || (dmbstatus[6]&0xff)==0xff) ||
	      ((dmbstatus[0]&0xff)!=0x21)                              ) {
	    std::cout << "... config check not OK for DMB "  << std::endl;
	  }
	  
	  int nsleep = 100;
	
	  for (int i = 0; i < tcs.t17b.strips_per_run; i++) // strip loop
	    {
	      LOG4CPLUS_INFO(getApplicationLogger(), "Strip: " << i * tcs.t17b.strip_step + tcs.t17b.strip_first - 1);
	      dmb->set_ext_chanx(i * tcs.t17b.strip_step + tcs.t17b.strip_first - 1); // strips start from 1 in config file (is that important for analysis?)
	      dmb->buck_shift();     
	      dmb->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML
	      
	      for (int k = 0; k < tcs.t17b.pulse_dac_settings; k++) // amplitude dac loop
		{
		  std::cout << "DMB setup for CFEB Gain, step = " <<k<< std::endl;
		  //steps given in integers - convert to float
		  //has same effect as lines below, nstrip loop placed above, no
		  //need to call nstrip function
		  //int nstrip=(k)/20;
		  //int gainsetting=((k)%20);
		  //if (!gainsetting) dmb->buck_shift_ext_bc(nstrip);
		  //dac=0.1+0.25*gainsetting;
		  dac=0.1+0.25*k;//actual test at P5
		  dmb->set_cal_dac (dac,dac);
		  ::usleep(nsleep);
		  LOG4CPLUS_INFO(getApplicationLogger(), "loop No "<<k<< " DAC: " << dac);
		  
		  for (j = 1; j <= tcs.t17b.events_per_pulsedac; j++) // pulse loop
		    {
		      ccb_GenerateDmbCfebCalib0(ccb); // pulse
		      usleep(10);
		      if (j % 100 == 0) 
			cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
		      if (quit)
			{
			  finish_test();
			  LOG4CPLUS_INFO(getApplicationLogger(), "Test 17b stopped");
			  return;
			}
		    }
		  cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		  cout << endl;
		}
	    }
	  finish_test();
	  LOG4CPLUS_INFO(getApplicationLogger(), "Test 17b finished");
}

void emuSTEPTests::test_18()
{
	max_events_ = tcs.t18.events_total;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 18, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	ccb_EnableL1aFromTmbL1aReq(ccb);
	// end "should be XML" section

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 18 finished");
}

void emuSTEPTests::test_19()
{
	int j;
	max_events_ = 
		tcs.t19.events_per_thresh * 
		tcs.t19.threshs_per_tpamp * 
		tcs.t19.dmb_tpamps_per_strip * 
		tcs.t19.strips_per_run;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 19, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromDmbCfebCalibX(ccb);
	ccb_SetExtTrigDelay(ccb, 38); // L1ADelay must be this value + 128 in UF
	// end "should be XML" section

	for (int i = 0; i < tcs.t19.strips_per_run; i++) // strip loop
	{
		LOG4CPLUS_INFO(getApplicationLogger(), "Strip: " << i * tcs.t19.strip_step + tcs.t19.strip_first - 1);

		dmb->set_ext_chanx(i * tcs.t19.strip_step + tcs.t19.strip_first - 1); // strips start from 1 in config file (is that important for analysis?)
		dmb->buck_shift();
		dmb->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML

		for (int k = 0; k < tcs.t19.dmb_tpamps_per_strip; k++) // test pulse amp loop
		{

			int cur_dac = k * tcs.t19.dmb_tpamp_step + tcs.t19.dmb_tpamp_first;
			LOG4CPLUS_INFO(getApplicationLogger(), "DMB test pulse amp: " << cur_dac);
			//if(k<1){
			  //dmb->set_dac(0, 155 * 5. / 4096.); 
			//}else{ 
			// set the test pulse amplitude - second parameter
			dmb->set_dac(0, (float)(cur_dac) * 5. / 4096.); // dac values in t19 assume 12-bit DAC
			//}
			// calculate thresh_first based on current dac value
			tcs.t19.thresh_first = cur_dac * tcs.t19.scale_turnoff / 16 - tcs.t19.range_turnoff;
			if (tcs.t19.thresh_first < 0) tcs.t19.thresh_first = 0;  

			for (int l = 0; l < tcs.t19.threshs_per_tpamp; l++) // threshold loop
			{

				LOG4CPLUS_INFO(getApplicationLogger(), "CFEB thresholds: " << l * tcs.t19.thresh_step + tcs.t19.thresh_first);
				// set cfeb thresholds (for the entire test)
				dmb->set_comp_thresh((float)(l * tcs.t19.thresh_step + tcs.t19.thresh_first)/1000.);
				//usleep(500000);
				for (j = 1; j <= tcs.t19.events_per_thresh; j++) // pulse loop
				{
					ccb_GenerateDmbCfebCalib0(ccb); // pulse
					usleep(10);
					if (j % 100 == 0) 
						cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
					if (quit)
					{
						finish_test();
						LOG4CPLUS_INFO(getApplicationLogger(), "Test 19 stopped");
						return;
					}
				}
				cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
				cout << endl;
			}
		}
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 19 finished");
}

void emuSTEPTests::test_21()
{
	int j;
	max_events_ = tcs.t21.events_per_hstrip * tcs.t21.hstrips_per_run;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 21, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	// set inject amplitude - first parameter (same for the entire test)
	dmb->set_dac((float)tcs.t21.dmb_test_pulse_amp * 5. / 256., 0);

	// set cfeb thresholds (for the entire test)
	dmb->set_comp_thresh((float)tcs.t21.cfeb_threshold/1000.);

	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromDmbCfebCalibX(ccb);
	ccb_SetExtTrigDelay(ccb, 34); // L1ADelay must be this value + 128 in UF
	dmb->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML
	// end "should be XML" section

	for (int i = 0; i < tcs.t21.hstrips_per_run; i++) // hstrip loop
	{
		LOG4CPLUS_INFO(getApplicationLogger(), "1/2-strip: " << i * tcs.t21.hstrip_step + tcs.t21.hstrip_first - 1);

		dmb_trighalfx(dmb, i * tcs.t21.hstrip_step + tcs.t21.hstrip_first - 1);

		for (j = 1; j <= tcs.t21.events_per_hstrip; j++) // pulse loop
		{
			ccb_GenerateDmbCfebCalib1(ccb); // inject
			usleep(10);
			if (j % 100 == 0) 
				cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
			if (quit)
			{
				finish_test();
				LOG4CPLUS_INFO(getApplicationLogger(), "Test 21 stopped");
				return;
			}
		}
		cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		cout << endl;
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 21 finished");
}

void emuSTEPTests::test_25_old()
{
	
	max_events_ = tcs.t25.trig_settings*tcs.t25.events_per_trig_set; 
	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 25, max events: " << max_events_);
	prepare_to_test();

	usleep(100);
	alct->configure();

	usleep(100);
	// the below should be done via XML params
	// end "should be XML" section
	//ccb is put in this mode to disable L1A after a single trigger
	//allows us to control how many events we collect for each setting
	//ccb_EnableL1aFromTmbL1aReq(ccb);
        //unsigned csrb1 = ccb->ReadRegister(0x20);
	string time;
	ofstream timestampfile;
	timestampfile.open("/home/cscme42/test_25.txt");
	
	unsigned csrb1=0x1edd;
        csrb1 |= 0x2000;
	cout<<"csrb1"<<csrb1<<endl;
	
        ccb->WriteRegister(0x20, csrb1);
        // enable L1A counter:
        ccb->WriteRegister(0x96, 0);
	
	int event_counter=0;
	cout<< "at start evt "<<event_counter +1<<" pretrig-layer "<<alct->GetPretrigNumberOfLayers()<<" pretrig-pattern "<<alct->GetPretrigNumberOfPattern()<<endl;
	
	//usleep (100);
	
	for (int i=0;i<tcs.t25.trig_settings;i++){
	  //do for each trigger setting
	  if(i==0){
	    alct->SetPretrigNumberOfLayers (1);
	  }else{
	    alct->SetPretrigNumberOfLayers (2);
	  }
	  alct->SetPretrigNumberOfPattern (i+1);
	  alct->WriteConfigurationReg();
	  struct timeval start,end;
	  gettimeofday(&start,NULL);
	  for(int j=0;j<tcs.t25.events_per_trig_set;j++){
	    ccb->WriteRegister(0x94, 0); // reset L1A counter in CCB
	    ccb->WriteRegister(0x58, 0); // enable one L1A
	    //event_counter+=1;
	    //if(j%1000==0){
	    //cout<< "step "<<i<<" evt "<<event_counter +1<<" threshold "<<threshold<<" pretrig-layer "<<alct->GetPretrigNumberOfLayers()<<" pretrig-pattern "<<alct->GetPretrigNumberOfPattern()<<endl;
	    //}
	    while (ccb->ReadRegister(0x90) == 0); 
	  }
	  gettimeofday(&end,NULL);
	  //cout<<"msecs "<<end.tv_sec*1000-start.tv_sec*1000+end.tv_usec/1000.-start.tv_usec/1000.<<endl;
	  timestampfile<<"msecs "<<end.tv_sec*1000-start.tv_sec*1000+end.tv_usec/1000.-start.tv_usec/1000.<<"\n";
	}
	timestampfile.close();
	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 25 old code finished");
}
void emuSTEPTests::test_25()
{
	
	max_events_ = tcs.t25.trig_settings*tcs.t25.events_per_trig_set; 
	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 25, max events: " << max_events_);
	prepare_to_test();


	time_t rawtime;
	
	struct tm* timestamp;
	time (& rawtime);
	timestamp=localtime(&rawtime);
	char filename_appendix[80];
	cout<<"time now "<<endl;
        // enable L1A counter:
        ccb->WriteRegister(0x96, 0);
	ofstream timestampfile;
        strftime (filename_appendix,80,"%y%m%d_%H%M%S",timestamp);
	char* char_start="/home/cscme42/STEP/data/test_results/test_25_";
	char* char_end=".txt";
	char* filename= (char*)malloc(strlen(char_start) + strlen(filename_appendix)  + strlen(char_end));
	sprintf(filename,"%s%s%s",char_start,filename_appendix,char_end);
	timestampfile.open(filename);	
	int event_counter=0;
	 cout<< "at start evt "<<event_counter +1<<" pretrig-layer "<<alct->GetPretrigNumberOfLayers()<<" pretrig-pattern "<<alct->GetPretrigNumberOfPattern()<<endl;

	 timestampfile<<"#time in musec      event counts\n";

	 for (int i=0;i<tcs.t25.trig_settings;i++){
	  if(i==0){
	    alct->SetPretrigNumberOfLayers (1);
	  }else{
	    alct->SetPretrigNumberOfLayers (2);
	  }
	  alct->SetPretrigNumberOfPattern (i+1);
	  alct->WriteConfigurationReg();
	  if(i==1){
	    log4cplus::helpers::sleepmillis(50000);
	  }else{
	    log4cplus::helpers::sleepmillis(20000);
	  }

	  ccb->WriteRegister(0x94,0);
	  ccb->WriteRegister(0x90,0);
	  ccb->WriteRegister(0x92,0);

	  unsigned csrb1=0x1edd;
	  ccb->WriteRegister(0x96, 0);

	  struct timeval start,end;
	  cout<<"reg 90/reg92 b "<<(ccb->ReadRegister(0x90) & 0xfffff)<<"/"<<ccb->ReadRegister(0x90)<<"/"<<(ccb->ReadRegister(0x92) & 0xfffff)<<"/"<<ccb->ReadRegister(0x92)<<" pretrig-layer "<<alct->GetPretrigNumberOfLayers()<<" pretrig-pattern "<<alct->GetPretrigNumberOfPattern()<<endl;
	  ccb->WriteRegister(0x20, csrb1);
	  gettimeofday(&start,NULL);
	  if(i==0){
	    log4cplus::helpers::sleepmillis(5000);
	  }else{
	    log4cplus::helpers::sleepmillis((i+1)*10000);
	  }
	  csrb1=0x1af9;
	  ccb->WriteRegister(0x20, csrb1);
	  //ccb->WriteRegister(0x98, 1);
	  gettimeofday(&end,NULL);
	  unsigned l1a_counter_low_bits = ccb->ReadRegister(0x90) & 0xfffff; // read lower 16 bits
	  unsigned l1a_counter_high_bits = ccb->ReadRegister(0x92) & 0xfffff; // read higher 16 bits
	  unsigned l1a_counter = l1a_counter_low_bits | (l1a_counter_high_bits << 16); // merge into counter
	  cout<<"reg 90/reg92 a "<<(ccb->ReadRegister(0x90) & 0xffff)<<"/"<<ccb->ReadRegister(0x90)<<"/"<<(ccb->ReadRegister(0x92) & 0xffff)<<"/"<<ccb->ReadRegister(0x92)<<"/"<<(l1a_counter_low_bits | (l1a_counter_high_bits << 16))<<endl;
	  cout<<"msecs "<<end.tv_sec*1000-start.tv_sec*1000+end.tv_usec/1000.-start.tv_usec/1000.<<endl;
	  timestampfile<<end.tv_sec*1000-start.tv_sec*1000+end.tv_usec/1000.-start.tv_usec/1000.<<" "<<l1a_counter<<"\n";
	  cout<<"l1a_counter_low/high/counter "<<l1a_counter_low_bits<<"/"<<l1a_counter_high_bits<<"/"<<l1a_counter<<endl;
	}

	timestampfile.close();
	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 25 finished");
}


void emuSTEPTests::test_30()
{
	int j;
	max_events_ = tcs.t30.events_per_delay * tcs.t30.tmb_l1a_delays_per_run;

	LOG4CPLUS_INFO(getApplicationLogger(), "Starting STEP test: 30, max events: " << max_events_);
	prepare_to_test();

	// the below should be done via XML params
	// set inject amplitude - first parameter (same for the entire test)
	dmb->set_dac(50 * 5. / 256., 0);

	// set cfeb thresholds (for the entire test)
	dmb->set_comp_thresh(50./1000.);

	tmb_EnableClctExtTrig(tmb);
	ccb_EnableL1aFromDmbCfebCalibX(ccb);
	//ccb_SetExtTrigDelay(ccb, 15); // L1ADelay must be this value + 128 in UF
	dmb->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML
	// end "should be XML" section

	// pulsing just one strip in every CFEB
	dmb_trighalfx(dmb, 7);

	for (int i = 0; i < tcs.t30.tmb_l1a_delays_per_run; i++) // delay loop
	{
		LOG4CPLUS_INFO(getApplicationLogger(), "tmb l1a delay: " << i * tcs.t30.tmb_l1a_delay_step + tcs.t30.tmb_l1a_delay_first);

		// set up TMB l1a delay
		tmb->SetL1aDelay(i * tcs.t30.tmb_l1a_delay_step + tcs.t30.tmb_l1a_delay_first);
		int data_to_write = tmb->FillTMBRegister(0x74);
		tmb->WriteRegister(0x74, data_to_write);

		for (j = 1; j <= tcs.t30.events_per_delay; j++) // pulse loop
		{
			ccb_GenerateDmbCfebCalib1(ccb); // inject
			usleep(10);
			if (j % 100 == 0) 
				cout << "\rPulses: " << setw(3) << setfill(' ') << j << flush;
			if (quit)
			{
				finish_test();
				LOG4CPLUS_INFO(getApplicationLogger(), "Test 30 stopped");
				return;
			}
		}
		cout << "\rPulses: " << setw(3) << setfill(' ') << j-1 << flush;
		cout << endl;
	}

	finish_test();
	LOG4CPLUS_INFO(getApplicationLogger(), "Test 30 finished");
}


xoap::MessageReference emuSTEPTests::on_step_show(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{

	ostringstream st;

	st << "<font face=\"courier\">" << endl;
	st << "Debugging information: " << cgicc::br() << endl;
	st <<  std::hex << "CCB.csra1 [00] 0x" << std::setw(4) << std::setfill('0') << ccb->ReadRegister(   0) << cgicc::br() << endl;
	st <<  std::hex << "CCB.csrb1 [20] 0x" << std::setw(4) << std::setfill('0') << ccb->ReadRegister(0x20) << cgicc::br() << endl;
	st <<  std::hex << "TMB.cfinj [42] 0x" << std::setw(4) << std::setfill('0') << tmb->ReadRegister(0x42) << cgicc::br() << endl;
	st <<  std::hex << "TMB.trgen [68] 0x" << std::setw(4) << std::setfill('0') << tmb->ReadRegister(0x68) << cgicc::br() << endl;
	st << "</font>" << endl;

	show_info = st.str();

	return createReply(message);
}


// == Run Control requests current parameter values == //
void emuSTEPTests::actionPerformed (xdata::Event& e)
{
	// appBSem_.take();
	// update measurements monitors
	if (e.type() == "ItemRetrieveEvent")
    {
		std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
		if ( item == "xmlFileName")
		{
			LOG4CPLUS_INFO(getApplicationLogger(), "xmlFileName : " << xmlFileName_.toString());
		} 
		else if ( item == "RunType")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "RunType : " << run_type_.toString());
        }

    }
	if (e.type() == "ItemChangedEvent")
    {
		std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
		if ( item == "xmlFileName")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "xmlFileName : " << xmlFileName_.toString());
        } 
		else if ( item == "RunType")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "RunType : " << run_type_.toString());
        }
		else if ( item == "RunNumber")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "RunNumber : " << run_number_.toString());
        }
		else if ( item == "maxEvents")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "maxEvents : " << max_events_.toString());
        }
		else if ( item == "alctpulseamp")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "alctpulseamp : " << alctpulseamp_.toString());
        }
		else if ( item == "ddu_fiber_mask")
        {
			LOG4CPLUS_INFO(getApplicationLogger(), "ddu_fiber_mask : " << ddu_fiber_mask_.toString());
        }


    }
}


void emuSTEPTests::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
	out->getHTTPResponseHeader().addHeader("Refresh", "5");
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << cgicc::head() << std::endl;
	*out << cgicc::title("emuSTEPTests") << cgicc::body() << std::endl;

	xgi::Utils::getPageHeader
	(
		out,
		"emuSTEPTests",
		getApplicationDescriptor()->getContextDescriptor()->getURL(),
		getApplicationDescriptor()->getURN(),
		"/daq/xgi/images/Application.gif"
	);

	Table t(5, 2, 1);

	string fn = "<font face=\"courier\">";
	string nfn = "</font>";

    *t.cell(0,0) << "<b>Run Number</b>";          *t.cell(0,1) << fn << run_number_.toString() << nfn;
    *t.cell(1,0) << "<b>Number of Events</b>";    *t.cell(1,1) << fn << max_events_.toString() << nfn;
    *t.cell(2,0) << "<b>Test Type</b>";           *t.cell(2,1) << fn << run_type_.toString() << nfn;
    *t.cell(3,0) << "<b>Config File</b>";         *t.cell(3,1) << fn << xmlFileName_.toString() << nfn;
    *t.cell(4,0) << "<b>DDU Fiber Mask</b>";      *t.cell(4,1) << fn << "0x" << emuSTEPApplication::toHexString(ddu_fiber_mask_, 4) << nfn;

	t.flush(out);

	*out << cgicc::br() << std::endl;
	// print show info
	*out << show_info << endl;


	*out << cgicc::body() << cgicc::html() << std::endl;

}


int test_task::svc()
{
	     if (testn == "11") papa->test_11(); // test 11 - AFEB noise
	else if (testn == "12") papa->test_12(); // test 12 - AFEB connectivity, test strip scan
	else if (testn == "13") papa->test_13(); // test 13 - AFEB thresholds
	else if (testn == "14") papa->test_14(); // test 14 - AFEB delays
	else if (testn == "15") papa->test_15(); // test 15 - CFEB pedestals & noise
	else if (testn == "16") papa->test_16(); // test 16 - CFEB connectivity
	else if (testn == "17") papa->test_17(); // test 17 - CFEB timing calibration
	else if (testn == "17b") papa->test_17b(); // test 17b - CFEB gain test
	else if (testn == "18") papa->test_18(); // test 18 - CFEB comparator noise
	else if (testn == "19") papa->test_19(); // test 19 - CFEB thresholds calibration
	else if (testn == "21") papa->test_21(); // test 21 - CFEB comparator logic
	else if (testn == "25") papa->test_25(); // test 25 - ALCT trigger test
	else if (testn == "30") papa->test_30(); // test 30 - TMB  timing
	else
	{
		cout << "Unsupported STEP test: " << testn << endl;
	}
	return 0;
}

xoap::MessageReference emuSTEPTests::createReply(xoap::MessageReference message)
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
