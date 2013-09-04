#ifndef __emu_me11dev_Buttons_h__
#define __emu_me11dev_Buttons_h__

#include "emu/utils/Cgi.h"
#include "emu/utils/System.h"
#include "emu/pc/VMEController.h"
//#include "/home/cscme11/XDAQ/11/TriDAS/emu/emuDCS/PeripheralCore/src/common/VMEController.cc"

#include <vector>
#include <sstream>

#include "emu/me11dev/Action.h"
#include "emu/me11dev/LogAction.h"
#include "emu/me11dev/ActionValue.h"

/******************************************************************************
 * The Buttons
 *
 * This header file and the associated cc file contains all the actions in the
 * application. Short and simple ones are often defined in the header file so
 * as to avoid duplicating code (declaration and definition).
 *
 * If this file starts to take a very long time to compile, you ought to
 * consider breaking it into pieces (perhaps grouped by type of test [e.g. TMB,
 * DMB]). Just remember to include all the pieces in the Manager.h file.
 *
 * NB The Makefile will not check if this file has been modified, if you modify
 * it without modifying the Buttons.cc file, use the command `touch Buttons.cc'
 * to ensure that it is recompiled
 *****************************************************************************/


namespace emu { namespace me11dev {

    /**************************************************************************
     * Simple Buttons
     *
     * For tests/actions that are one or two lines and just need a button and
     * no text boxes 
     *************************************************************************/

    class Manager;

    class HardReset : public Action {
    public:
      HardReset(emu::pc::Crate * crate) : Action(crate) {}
      void display(xgi::Output * out) { addButton(out, "Hard Reset", "width: 100%; "); }
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SoftReset : public Action {
    public:
      SoftReset(emu::pc::Crate * crate) : Action(crate) {}
      void display(xgi::Output * out) { addButton(out, "Soft Reset", "width: 100%; "); }
      void respond(xgi::Input * in, std::ostringstream & out);
    };
    
    class L1Reset : public Action {
    public:
      L1Reset(emu::pc::Crate * crate) : Action(crate) {}
      void display(xgi::Output * out) { addButton(out, "L1 Reset (Resync)", "width: 100%; "); }
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class BC0 : public Action {
    public:
      BC0(emu::pc::Crate * crate) : Action(crate) {}
      void display(xgi::Output * out) { addButton(out, "BC0", "width: 100%; "); }
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    /**************************************************************************
     * Other Buttons
     *
     *************************************************************************/

    // class Header_GeneralFunctions : public Header {
    // public:
    //   Header_GeneralFunctions(emu::pc::Crate * crate);
    //   void display(xgi::Output * out);
    //  };


    class ReadBackUserCodes : public Action {
    public:
      ReadBackUserCodes(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetTMBdavDelay : public Action, public ActionValue<int> {
    public:
      SetTMBdavDelay(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetComparatorMode : public Action, public ActionValue<std::string> {
    public:
      SetComparatorMode(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
    
    class SetComparatorThresholds : public Action, public ActionValue<float> {
    public:
      SetComparatorThresholds(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetComparatorThresholdsBroadcast : public Action, public ActionValue<float> {
    public:
      SetComparatorThresholdsBroadcast(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetUpComparatorPulse : public Action, public Action2Values<int,int> {
    public:
      SetUpComparatorPulse(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetUpPrecisionCapacitors : public Action, public Action2Values<int,int> {
    public:
      SetUpPrecisionCapacitors(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class PulseInternalCapacitorsDMB : public Action {
    public:
      PulseInternalCapacitorsDMB(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class PulseInternalCapacitorsCCB : public Action {
    public:
      PulseInternalCapacitorsCCB(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class PulsePrecisionCapacitorsDMB : public Action {
    public:
      PulsePrecisionCapacitorsDMB(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class PulsePrecisionCapacitorsCCB : public Action {
    public:
      PulsePrecisionCapacitorsCCB(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class SetDMBDACs : public Action, public ActionValue<float> {
    public:
      SetDMBDACs(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class ShiftBuckeyesNormRun : public Action {
    public:
      ShiftBuckeyesNormRun(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    class PulseWires : public Action, public ActionValue<int> {
    public:
      PulseWires(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    class SetPipelineDepthAllDCFEBs : public Action, public ActionValue<int> {
    public:
      SetPipelineDepthAllDCFEBs(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class Stans_SetPipelineDepthAllDCFEBs : public Action, public ActionValue<int> {
    public:
      Stans_SetPipelineDepthAllDCFEBs(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class ReadPipelineDepthAllDCFEBs : public Action {
    public:
      ReadPipelineDepthAllDCFEBs(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
  

    class SetFineDelayForADCFEB : public Action, public Action2Values<int, int> {
    public:
      SetFineDelayForADCFEB(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class TMBHardResetTest : public Action, public ActionValue<int> {
    public:
      TMBHardResetTest(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class DDU_KillFiber : public Action, public ActionValue<std::string> {
    public:
      DDU_KillFiber(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class DDU_EthPrescale : public Action, public ActionValue<std::string> {
    public:
      DDU_EthPrescale(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class DDU_FakeL1 : public Action, public ActionValue<std::string> {
    public:
      DDU_FakeL1(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class BuckShiftTest : public Action {
    public:
      BuckShiftTest(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


  class BuckShiftTestDebug : public Action, public ActionValue<int> {
    public:
     BuckShiftTestDebug(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    
    class TMBRegisters : public Action {
    public:
      TMBRegisters(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

    class TMBSetRegisters : public Action, public Action2Values<std::string, std::string> {
    public:
      TMBSetRegisters(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
   
    class TMBDisableCopper : public Action {
    public:
      TMBDisableCopper(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

   class TMBEnableCLCTInput : public Action, public ActionValue<int> {
    public:
      TMBEnableCLCTInput(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
   };
 
   class TMBDisableCLCTInput : public Action, public ActionValue<int> {
    public:
      TMBDisableCLCTInput(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
   };  

    class PipelineDepthScan_Pulses : public Action, public Action2Values<int, int> {
    public:
      PipelineDepthScan_Pulses(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     

    class PipelineDepthScan_Cosmics : public Action, public Action2Values<int, int> {
    public:
      PipelineDepthScan_Cosmics(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     

    class TmbDavDelayScan : public Action, public Action2Values<int, int> {
    public:
      TmbDavDelayScan(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     

    class L1aDelayScan : public Action, public Action2Values<int, int> {
    public:
      L1aDelayScan(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     

    /**************************************************************************
     * ExecuteVMEDSL
     *
     * A domain-specific-lanaguage for issuing vme commands.
     *************************************************************************/
    class ExecuteVMEDSL : public Action, public ActionValue<std::string> {
    public:
      ExecuteVMEDSL(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    /**************************************************************************
     * Log Buttons
     *
     * These are special actions that modify the log
     *************************************************************************/

    class ClearLog : public LogAction {
    public:
      ClearLog(emu::pc::Crate * crate) : LogAction(crate) { }
      void display(xgi::Output * out) { addButton(out, "Clear Log"); }
      void respond(xgi::Input * in, xgi::Output * out, std::ostringstream & ssout, std::ostringstream & log) {
	log.clear(); // remove any error flags
	log.str(""); // empty the log
      }
    };

    class SaveLogAsFile : public LogAction {
    public:
      SaveLogAsFile(emu::pc::Crate * crate) : LogAction(crate) { }
      void display(xgi::Output * out) { addButton(out, "Save Log as..."); }
      void respond(xgi::Input * in, xgi::Output * out, std::ostringstream & ssout, std::ostringstream & log) {
        std::string file_name = "me11dev_" + emu::utils::getDateTime(true) + ".log";
        emu::utils::saveAsFileDialog(out, log.str(), file_name);
        ssout.str("*** Contents above was saved to a log file ***");
      }
    };

    class DumpLog : public LogAction {
    public:
      DumpLog(emu::pc::Crate * crate) : LogAction(crate) { }
      void display(xgi::Output * out) { addButton(out, "Dump Log to stdout"); }
      void respond(xgi::Input * in, xgi::Output * out, std::ostringstream & ssout, std::ostringstream & log) {
        std::cout<<"----- "<<emu::utils::getDateTime(false)<<": Log Dump -----"<<std::endl<<std::endl;
        std::cout<<log.str()<<std::endl;
        std::cout<<"----- End of Log Dump -----"<<std::endl;
        ssout.str("*** Contents above was dumped to stdout ***");
      }
    };

    class dcfebDebugDump : public Action {
    public:
      dcfebDebugDump(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    class enableVmeDebugPrintout : public Action, ActionValue<int> {
    public:
      enableVmeDebugPrintout(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


   class CommonUtilities_setupDDU_passThrough : public Action {
   public:
      CommonUtilities_setupDDU_passThrough(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
   };


   class CommonUtilities_setupDDU : public Action {
   public:
      CommonUtilities_setupDDU(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
   };


   class CommonUtilities_restoreCFEBIdle : public Action {
   public:
      CommonUtilities_restoreCFEBIdle(emu::pc::Crate * crate);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
   };




 class ODMB_OTMB_LCT_Testing : public Action, public ActionValue<int> {
    public:
     ODMB_OTMB_LCT_Testing(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
  


  class Investigate_MissingEvents : public Action, public ActionValue<int> {
    public:
     Investigate_MissingEvents(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


  class ODMB_L1A_Testing : public Action, public ActionValue<int> {
    public:
     ODMB_L1A_Testing(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };




  class RoutineTest_StandardCosmics : public Action, public ActionValue<int> {
    public:
      RoutineTest_StandardCosmics(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };



   class STEP15TestBed : public Action, public Action2Values<int, int> {
   public:
     STEP15TestBed(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
     void display(xgi::Output * out);
     void respond(xgi::Input * in, std::ostringstream & out);
   };


   class STEP16TestBed : public Action, public Action2Values<int, int> {
   public:
     STEP16TestBed(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
     void display(xgi::Output * out);
     void respond(xgi::Input * in, std::ostringstream & out);
   };
  


   class ODMBCosmicsTest : public Action, public Action2Values<int, int> {  
   public:
     ODMBCosmicsTest(emu::pc::Crate * crate, emu::me11dev::Manager* manager);  
     void display(xgi::Output * out);
     void respond(xgi::Input * in, std::ostringstream & out);
   };


   class ODMBCosmicsTestV2 : public Action, public Action2Values<int, int> {  
   public:
     ODMBCosmicsTestV2(emu::pc::Crate * crate, emu::me11dev::Manager* manager);  
     void display(xgi::Output * out);
     void respond(xgi::Input * in, std::ostringstream & out);
   };


  class ODMBStepTest16 : public Action {
    public:
	ODMBStepTest16(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
	 void display(xgi::Output * out);
	void respond(xgi::Input * in, std::ostringstream & out);
    };

  class RoutineTest_ShortCosmicsRun : public Action, public ActionValue<int> {
    public:
      RoutineTest_ShortCosmicsRun(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };

  class RoutineTest_ShortCosmicsRun_DDUPT : public Action, public ActionValue<int> {
    public:
      RoutineTest_ShortCosmicsRun_DDUPT(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };


    class RoutineTest_PrecisionPulses : public Action {
    public:
      RoutineTest_PrecisionPulses(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
    class RoutineTest_PatternPulses_TMBCounters : public Action {
    public:
      RoutineTest_PatternPulses_TMBCounters(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };
    class STEP9bFibers : public Action {
    public:
      STEP9bFibers(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     

    class ButtonTests : public Action {
    public:
      ButtonTests(emu::pc::Crate * crate, emu::me11dev::Manager* manager);
      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);
    };     
    
  }
}

#endif //__emu_me11dev_Buttons_h__


