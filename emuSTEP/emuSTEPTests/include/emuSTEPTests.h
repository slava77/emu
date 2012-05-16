#ifndef _emuSTEPTests_h_
#define _emuSTEPTests_h_

#include "emuSTEPApplication.h"

#include "xgi/Method.h"
#include "xgi/WSM.h"
#include "xgi/Utils.h"
#include "xgi/Input.h"
#include "xgi/Output.h"
#include "xdata/Integer.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/BSem.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"

#include "cgicc/HTMLClasses.h"
// includes from Peripheral Crate
//#include "DAQMBTester.h"
#include "emu/pc/CCB.h"
#include "emu/pc/EmuEndcap.h"
//#include "emu/pc/EmuController.h"
#include "emu/pc/TMB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/MPC.h"
#include "emu/pc/EmuPeripheralCrateBase.h"
#include "emu/pc/DAQMB.h"
#include <unistd.h> // for usleep

#include "test_config_struct.h"
#include "toolbox/Task.h"
#include "toolbox/TaskAttributes.h"
//#include <sys/time.h>
//#include <stdio.h>


class emuSTEPTests;// : public emuSTEPApplication, xdata::ActionListener;

class test_task: public toolbox::Task 
{
 public: 
	test_task(emuSTEPTests *p) : Task("test_task")
		{
			papa = p;
		};
 
	void set_testn(string tn)
		{
			testn = tn;
		};

	int svc();

	string testn;
	emuSTEPTests* papa;
};



class emuSTEPTests : public emuSTEPApplication, xdata::ActionListener
{

public:
    XDAQ_INSTANTIATOR();


    emuSTEPTests(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

    void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	xoap::MessageReference on_step_configure(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	xoap::MessageReference on_step_start(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	xoap::MessageReference on_step_stop(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	xoap::MessageReference on_step_show(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	void prepare_to_test();
	void finish_test();
	void test_11();
	void test_12();
	void test_13();
	void test_14();
	void test_15();
	void test_16();
	void test_17();
	void test_17b();
	void test_18();
	void test_19();
	void test_21();
	void test_25();
	void test_25_old();
	void test_30();

	// routines missing in PeripheralCrate and Core
	// to let FG and GR know
        void ccb_GenerateL1A(emu::pc::CCB *ccb);
	void ccb_GenerateDmbCfebCalib0(emu::pc::CCB *ccb);
	void ccb_GenerateDmbCfebCalib1(emu::pc::CCB *ccb);
	void ccb_EnableL1aFromVme(emu::pc::CCB *ccb); 
	void ccb_EnableL1aFromTmbL1aReq(emu::pc::CCB *ccb);
	void ccb_EnableL1aFromSyncAdb(emu::pc::CCB *ccb);
	void ccb_EnableL1aFromASyncAdb(emu::pc::CCB *ccb);
	void ccb_EnableL1aFromDmbCfebCalibX(emu::pc::CCB *ccb);
	void ccb_DisableL1a(emu::pc::CCB *ccb);
	void ccb_SetExtTrigDelay(emu::pc::CCB *ccb, unsigned delay); 
	void tmb_EnableClctExtTrig(emu::pc::TMB * tmb); // clct_ext_pretrig_enable is in TMBParser already, but commented out
	void dmb_trighalfx(emu::pc::DAQMB* dmb, int ihalf);

protected: 
    void actionPerformed (xdata::Event& e);
    xoap::MessageReference createReply(xoap::MessageReference message)
                        throw (xoap::exception::Exception);

private:
   xdata::String run_type_;
   xdata::UnsignedLong run_number_;
   xdata::Integer max_events_, alctpulseamp_, ddu_fiber_mask_;
   xdata::String xmlFileName_;
   xdata::Vector<xdata::String> config_keys_;

   int keyToIndex(const std::string key);
   std::string toString(const long int i) const;

   xdaq::ApplicationStub *astub;

   //peripheral crate 
      //emu::pc::emuController *emuController;
        emu::pc::EmuPeripheralCrateBase *emuPeripheralCrateBase;
        emu::pc::EmuEndcap *emuEndcap;
	vector <emu::pc::Crate*> crates;
	vector <emu::pc::Chamber*> chambers;
	vector <emu::pc::TMB*> tmbs;
	vector <emu::pc::DAQMB*> dmbs;
	vector <emu::pc::DDU*> ddus;

	emu::pc::Crate* crate;
	emu::pc::Chamber* chamber;
        emu::pc::CCB* ccb;
	emu::pc::DAQMB* dmb;
	emu::pc::DDU* ddu;
	emu::pc::TMB* tmb;
	emu::pc::ALCTController* alct;

	emu::pc::Crate* broadcastCrate;
	emu::pc::DAQMB* broadcastDMB; 
	emu::pc::TMB* broadcastTMB; 
	emu::pc::ALCTController* broadcastALCT;
	emu::pc::CCB* broadcastCCB;


	string show_info;
	int test_pulse_count;
	test_config_struct tcs;

	test_task *tt;
	bool quit;
};
  

#endif
