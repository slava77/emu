#ifndef __emu_step_Test_h__
#define __emu_step_Test_h__

#include "emu/step/TestParameters.h"

#include "emu/pc/XMLParser.h"
#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/DAQMB.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace emu{
  namespace step{
    class Test : public TestParameters {

    public:
      // enum CCBRegisterAddress_t {
      // 	CSRB1             = 0x20, ///< control register, FPGA
      // 	CSRB5             = 0x28, ///< delay register, FPGA
      // 	Gen_L1A_ExtTrg    = 0x54, ///< Generate L1ACC 25 ns pulse (write only) and external triggers (if enabled), FPGA
      // 	Gen_DMB_CFEB_Cal0 = 0x8a, ///< Generate “DMB_cfeb_calibrate[0]” 25 ns pulse (write only), FPGA 
      // 	Gen_DMB_CFEB_Cal1 = 0x8c, ///< Generate “DMB_cfeb_calibrate[1]” 25 ns pulse (write only), FPGA 
      // 	Gen_DMB_CFEB_Cal2 = 0x8e  ///< Generate “DMB_cfeb_calibrate[2]” 25 ns pulse (write only), FPGA 
      // };

      // Test();
      Test( const string& id, 
	    const string& testParametersXML, 
	    const string& generalSettingsXML,
	    const string& specialSettingsXML,
	    const bool    isFake,
	    Logger*       pLogger              );
      // ~Test();
      void execute();
      double getProgress();
      void stop(){ isToStop_ = true; }

    private:
      bool                isFake_;
      bool                isToStop_;
      emu::pc::XMLParser  parser_;
      uint64_t            iEvent_;
      void ( emu::step::Test::* procedure_ )();

      void ( emu::step::Test::* getProcedure( const string& testId ) )(); 
      void createEndcap( const string& generalSettingsXML,
			 const string& specialSettingsXML  );
      void configureCrates();
      void enableTrigger();
      void disableTrigger();
      void setUpDMB( emu::pc::DAQMB *dmb );
      void _11();
      void _12();
      void _13();
      void _14();
      void _15();
      void _16();
      void _17();
      void _17b();
      void _18();
      void _19();
      void _21();
      void _25();
      void _30();
      void _fake();

      // void ccb_GenerateL1A(emu::pc::CCB *ccb);
      // void ccb_GenerateDmbCfebCalib0(emu::pc::CCB *ccb);
      // void ccb_GenerateDmbCfebCalib1(emu::pc::CCB *ccb);
      // void ccb_EnableL1aFromVme(emu::pc::CCB *ccb); 
      // void ccb_EnableL1aFromTmbL1aReq(emu::pc::CCB *ccb);
      // void ccb_EnableL1aFromSyncAdb(emu::pc::CCB *ccb);
      // void ccb_EnableL1aFromASyncAdb(emu::pc::CCB *ccb);
      // void ccb_EnableL1aFromDmbCfebCalibX(emu::pc::CCB *ccb);
      // void ccb_DisableL1a(emu::pc::CCB *ccb);
      // void ccb_SetExtTrigDelay(emu::pc::CCB *ccb, unsigned delay); 
      // void tmb_EnableClctExtTrig(emu::pc::TMB * tmb); // clct_ext_pretrig_enable is in TMBParser already, but commented out
      // void dmb_trighalfx(emu::pc::DAQMB* dmb, int ihalf);
    };
  }
}

#endif
