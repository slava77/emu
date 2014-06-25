#include "emu/odmbdev/Buttons.h" 
#include "emu/odmbdev/utils.h"
#include "emu/odmbdev/Manager.h"

#include "emu/pc/Crate.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/TMB_constants.h"

#include "emu/utils/String.h"
#include "cgicc/HTMLClasses.h"

#include "eth_lib.cc"

#include <stdio.h>
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>

extern char *wbuf;
extern char rbuf[];
extern int nrbuf;
extern int fd_schar;
extern unsigned char hw_source_addr[];
extern unsigned char hw_dest_addr[];

using namespace std;
using namespace emu::pc;

/******************************************************************************
 * Some classes are declared in the header file because they are short and
 * sweet. Check there first!
 *
 * Also note, at the end of this file is a template for new Action
 * sub-classes.
 * 
 *****************************************************************************/
bool myfunction (pair<float, int> i, pair<float, int> j) { return (i.first < j.first); }
namespace emu {
  namespace odmbdev {
    
    int Manager::slot_number = 5;
    unsigned int Manager::port_ = 9997; // This doesn't affect the xdaq app;
    // I just needed to initialize this
    // static member variable.

    void HardReset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>HardReset"<<endl; 
      if(ccb_->GetCCBmode() != CCB::VMEFPGA) ccb_->setCCBMode(CCB::VMEFPGA); // we want the CCB in this mode for out test stand
      vme_wrapper_->VMEWrite(0x0, 0x0, 13, "Allow hard resets.");
      ccb_->HardReset_crate(); // send a simple hard reset without any sleeps
      usleep(150000); // need at least 150 ms after hard reset 
    }
    
    void L1Reset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>L1Reset"<<endl; ccb_->l1aReset(); }

    void BC0::respond(xgi::Input * in, ostringstream & out) { cout<<"==>BC0"<<endl; ccb_->bc0(); }

    /**************************************************************************
     * Reads back the user codes
     *
     *************************************************************************/
    ReadBackUserCodes::ReadBackUserCodes(Crate * crate)
      : Action(crate) {}


    void ReadBackUserCodes::display(xgi::Output * out)
    {
      addButton(out, "Read back usercodes","width: 230px; ");
    }

    void ReadBackUserCodes::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ReadBackUserCodes"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        vector <CFEB> cfebs = (*dmb)->cfebs();
        
        for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
          int cfeb_index = (*cfeb).number();

          out << " ********************* " << endl
              << " FEB" << cfeb_index << " : "
              << " Usercode: " << hex << (*dmb)->febfpgauser(*cfeb) << endl
              << " Virtex 6 Status: " << (*dmb)->virtex6_readreg(7);
        }

        (*dmb)->RedirectOutput(&out);
        (*dmb)->daqmb_adc_dump();
        (*dmb)->RedirectOutput(&cout);
      }
    }

    /**************************************************************************
     * SetComparatorMode
     *
     *************************************************************************/

    SetComparatorMode::SetComparatorMode(Crate * crate)
      : Action(crate),
        ActionValue<string>("0xa") {}

    void SetComparatorMode::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set Comparator Mode Bits (hex):",
                           "ComparatorMode",
                           numberToString(value()));
    }

    void SetComparatorMode::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetComparatorMode"<<endl; 
      int ComparatorMode = getFormValueIntHex("ComparatorMode", in);
      //value(ComparatorMode); // save the value


      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_comp_mode(ComparatorMode);
        cout << "calling set comparator mode bits with " << ComparatorMode << endl;
      }
    }

    /**************************************************************************
     * SetComparatorThresholds
     *
     *************************************************************************/

    SetComparatorThresholds::SetComparatorThresholds(Crate * crate)
      : Action(crate),
        ActionValue<float>(0.03f) {}

    void SetComparatorThresholds::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set Comparator Thresholds (volts):",
                           "ComparatorThresholds",
                           numberToString(value()));
    }

    void SetComparatorThresholds::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetComparatorThresholds"<<endl; 
      float ComparatorThresholds = getFormValueFloat("ComparatorThresholds", in);
      value(ComparatorThresholds); // save the value

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_comp_thresh(ComparatorThresholds);
      }
    }

    /**************************************************************************
     * SetComparatorThresholdsBroadcast
     *
     *************************************************************************/

    SetComparatorThresholdsBroadcast::SetComparatorThresholdsBroadcast(Crate * crate)
      : Action(crate),
        ActionValue<float>(0.03f) {}

    void SetComparatorThresholdsBroadcast::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set Comparator Thresholds-broadcast (volts):",
                           "ComparatorThresholds",
                           numberToString(value()));
    }

    void SetComparatorThresholdsBroadcast::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetComparatorThresholdsBroadcast"<<endl; 
      float ComparatorThresholds = getFormValueFloat("ComparatorThresholds", in);
      value(ComparatorThresholds); // save the value

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->dcfeb_set_comp_thresh_bc(ComparatorThresholds);
      }
    }

    /**************************************************************************
     * SetUpComparatorPulse
     *
     *************************************************************************/

    SetUpComparatorPulse::SetUpComparatorPulse(Crate * crate)
      : Action(crate),
        Action2Values<int,int>(16,-1) {}

    void SetUpComparatorPulse::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set up comparator pulse: halfstrip(0-31), DCFEB(0-4):",
                                "halfstrip",
                                numberToString(value1()),
                                "dcfeb_number",
                                numberToString(value2()));
    }

    void SetUpComparatorPulse::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetUpComparatorPulse"<<endl; 
      int halfstrip = getFormValueInt("halfstrip", in);
      int dcfeb_number = getFormValueInt("dcfeb_number", in);
      value1(halfstrip); // save the value
      value2(dcfeb_number); // save the value
     
      tmb_->SetClctPatternTrigEnable(1); // enable CLCT pretriggers
      tmb_->WriteRegister(emu::pc::seq_trig_en_adr);

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);
      
      int hp[6] = {halfstrip+1, halfstrip, halfstrip+1, halfstrip, halfstrip+1, halfstrip};
      // Note: +1 for layers 0,2,4 is because ME1/1 doesn't have
      // staggered strips, but DAQMB codes assumes staggering.
      int CFEB_mask = 0xff;

      switch(dcfeb_number){
      case 0: CFEB_mask = 0x01; break;
      case 1: CFEB_mask = 0x02; break;
      case 2: CFEB_mask = 0x04; break;
      case 3: CFEB_mask = 0x08; break;
      case 4: CFEB_mask = 0x10; break;
      default: CFEB_mask = 0x1f; break;
      }

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->trigsetx(hp, CFEB_mask); // this calls chan2shift, which does the shift
        usleep(100);
      }

      ccb_->syncReset();//check
      usleep(100);
      ccb_->bx0();   //check
    }

    /**************************************************************************
     * SetUpPrecisionCapacitors
     *
     *************************************************************************/

    SetUpPrecisionCapacitors::SetUpPrecisionCapacitors(Crate * crate)
      : Action(crate),
        Action2Values<int,int>(8,-1) {}

    void SetUpPrecisionCapacitors::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set up precision pulse: strip(0-15), DCFEB(0-4)",
                                "StripToPulse",
                                numberToString(value1()),
                                "dcfeb_number",
                                numberToString(value2()));
    }

    void SetUpPrecisionCapacitors::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetUpPrecisionCapacitors"<<endl; 
      int strip_to_pulse = getFormValueInt("StripToPulse", in);
      int dcfeb_number = getFormValueInt("dcfeb_number", in);
      value1(strip_to_pulse); // save the value
      value2(dcfeb_number); // save the value
      
      //tmb_->SetClctPatternTrigEnable(1); // set flag to enable CLCT pretriggers
      //usleep(1000);
      //tmb_->WriteRegister(emu::pc::seq_trig_en_adr); // make it so
      //usleep(1000);
      //tmb_->SetTmbAllowClct(1); // set flag to enable CLCT only triggers
      //usleep(1000);
      //tmb_->SetTmbAllowMatch(1); // set flag to enable CLCT+ALCT match triggers
      //usleep(1000);
      //tmb_->WriteRegister(emu::pc::tmb_trig_adr); /// make it so
      //usleep(1000);

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);
      
      //crate_->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_ext_chanx(strip_to_pulse, dcfeb_number); // this only sets the array in software
        (*dmb)->buck_shift(); // this shifts the array into the buckeyes
        usleep(100);
      }
      //crate_->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands

      ccb_->l1aReset();
      usleep(1000);
      ccb_->bx0();
      usleep(1000);
    }


    SetTMBdavDelay::SetTMBdavDelay(Crate * crate)
      : Action(crate),
        ActionValue<int>(22) {}

    void SetTMBdavDelay::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set tmb_dav_delay",
                           "idelay",
                           numberToString(value()));
    }

    void SetTMBdavDelay::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetTMBdavDelay"<<endl; 
      int idelay = getFormValueInt("idelay", in);
      value(idelay); // save the value
      
      for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->varytmbdavdelay(idelay);
      }   

    }

    /**************************************************************************
     * PulseInternalCapacitorsDMB
     *
     *************************************************************************/

    PulseInternalCapacitorsDMB::PulseInternalCapacitorsDMB(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulseInternalCapacitorsDMB::display(xgi::Output * out) {
      addButton(out, "Pulse internal capacitors via DMB");
    }

    void PulseInternalCapacitorsDMB::respond(xgi::Input * in, ostringstream & out) {
      cout<<"==>PulseInternalCapacitorsDMB"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->inject(1,0);
      }
    }

    /**************************************************************************
     * PulseInternalCapacitorsCCB
     *
     *************************************************************************/

    PulseInternalCapacitorsCCB::PulseInternalCapacitorsCCB(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulseInternalCapacitorsCCB::display(xgi::Output * out) {
      addButton(out, "Pulse internal capacitors via CCB");
    }

    void PulseInternalCapacitorsCCB::respond(xgi::Input * in, ostringstream & out) {
      cout<<"==>PulseInternalCapacitorsCCB"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        ccb_->inject(1,0);
      }
    }

    /**************************************************************************
     * PulsePrecisionCapacitorsDMB
     *
     *************************************************************************/

    PulsePrecisionCapacitorsDMB::PulsePrecisionCapacitorsDMB(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulsePrecisionCapacitorsDMB::display(xgi::Output * out) {
      addButton(out, "Pulse precision capacitors via DMB");
    }

    void PulsePrecisionCapacitorsDMB::respond(xgi::Input * in, ostringstream & out) {
      cout<<"==>PulsePrecisionCapacitorsDMB"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->pulse(1,0);
      }
    }

    /**************************************************************************
     * PulsePrecisionCapacitorsCCB
     *
     *************************************************************************/

    PulsePrecisionCapacitorsCCB::PulsePrecisionCapacitorsCCB(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulsePrecisionCapacitorsCCB::display(xgi::Output * out) {
      addButton(out, "Pulse precision capacitors via CCB");
    }

    void PulsePrecisionCapacitorsCCB::respond(xgi::Input * in, ostringstream & out) {
      cout<<"==>PulsePrecisionCapacitorsCCB"<<endl; 
      //ccb_->pulse(1,0); // send the pulses 
      ccb_->GenerateDmbCfebCalib0(); // send a pulse
    }

    /**************************************************************************
     * SetDMBDACs
     *
     *************************************************************************/

    SetDMBDACs::SetDMBDACs(Crate * crate)
      : Action(crate),
        ActionValue<float>(1.0f) {}

    void SetDMBDACs::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set DMB DACs 0 and 1 to (volts):",
                           "DAC",
                           numberToString(value()));
    }

    void SetDMBDACs::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetDMBDACs"<<endl; 
      float DAC = getFormValueFloat("DAC", in);
      value(DAC); // save the value

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_dac(DAC,DAC); // this was tested and appeared to work correctly
      }
    }

    /**************************************************************************
     * ShiftBuckeyesNormRun
     *
     *************************************************************************/

    ShiftBuckeyesNormRun::ShiftBuckeyesNormRun(Crate * crate)
      : Action(crate) {}

    void ShiftBuckeyesNormRun::display(xgi::Output * out)
    {
      addButton(out, "Shift Buckeyes into normal mode");
    }

    void ShiftBuckeyesNormRun::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ShiftBuckeyesNormRun"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->shift_all(NORM_RUN);
        (*dmb)->buck_shift();
      }
    }
    
    /**************************************************************************
     * SetPipelineDepthAllDCFEBs
     *
     *************************************************************************/

    SetPipelineDepthAllDCFEBs::SetPipelineDepthAllDCFEBs(Crate * crate)
      : Action(crate),
        ActionValue<int>(66) {}

    void SetPipelineDepthAllDCFEBs::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Set pipeline depth on all DCFEBs:",
                           "depth",
                           numberToString(value()));
    }

    void SetPipelineDepthAllDCFEBs::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetPipelineDepthAllDCFEBs"<<endl; 
      int depth = getFormValueInt("depth", in);
      value(depth); // save the value

      //crate_->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        vector <CFEB> cfebs = (*dmb)->cfebs();
        
        for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
          (*dmb)->dcfeb_set_PipelineDepth(*cfeb, depth);
          usleep(100);
          (*dmb)->Pipeline_Restart( *cfeb );
        }
      }
      //crate_->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands
    }

    Stans_SetPipelineDepthAllDCFEBs::Stans_SetPipelineDepthAllDCFEBs(Crate * crate)
      : Action(crate),
        ActionValue<int>(66) {}

    void Stans_SetPipelineDepthAllDCFEBs::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Stan's Set pipeline depth on all DCFEBs:",
                           "depth",
                           numberToString(value()));
    }

    void Stans_SetPipelineDepthAllDCFEBs::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>Stans_SetPipelineDepthAllDCFEBs"<<endl; 
      int depth = getFormValueInt("depth", in);
      value(depth); // save the value

      //crate_->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands
      //// HACK to see if Stan's functions work better -Joe
      //      DAQMB* dmb = dmbs_[0];
      //// This is the same orders and the oringinal

      //// For SVN, this is commented out to do nothing because the methods called are just experimental and not in SVN.
      out << "THIS DOES NOTHING UNLESS YOU UNCOMMENT THE EXPERIMENTAL CODE." << endl;

      //// Experimental code not in SVN:
      //       dmb->Set_PipelineDepth_Stan(F1DCFEBM, depth);
      //       dmb->Pipeline_Restart_Stan( F1DCFEBM );
      //       usleep(100);
      //       dmb->Set_PipelineDepth_Stan(F2DCFEBM, depth);
      //       dmb->Pipeline_Restart_Stan( F2DCFEBM );
      //       usleep(100);
      //       dmb->Set_PipelineDepth_Stan(F3DCFEBM, depth);
      //       dmb->Pipeline_Restart_Stan( F3DCFEBM );
      //       usleep(100);
      //       dmb->Set_PipelineDepth_Stan(F4DCFEBM, depth);
      //       dmb->Pipeline_Restart_Stan( F4DCFEBM );
      //       usleep(100);
      //       dmb->Set_PipelineDepth_Stan(F5DCFEBM, depth);
      //       dmb->Pipeline_Restart_Stan( F5DCFEBM );
      //       usleep(100);

      //crate_->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands
    }

    /**************************************************************************
     * ReadPipelineDepthAllDCFEBs
     *
     *Added by J. Pilot 5 Mar 2013
     *************************************************************************/

    ReadPipelineDepthAllDCFEBs::ReadPipelineDepthAllDCFEBs(Crate * crate)
      : Action(crate){}

    void ReadPipelineDepthAllDCFEBs::display(xgi::Output * out)
    {
      addButton(out, "Read Pipeline Depth");
    }

    void ReadPipelineDepthAllDCFEBs::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ReadPipelineDepthAllDCFEBs"<<endl; 
      //int depth = getFormValueInt("depth", in);
      //value(depth); // save the value
      
      out << "Reading pipeline depth....." << endl;
      
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        vector <CFEB> cfebs = (*dmb)->cfebs();
        //int currentPD = -1;
        
        for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
          out << "ReadPipelineDepthAllDCFEBs is still under constuction." << endl;
          // currentPD = (*cfeb).GetPipelineDepth();
          // out << "Pipeline Depth is: " << currentPD << endl;
          // (*dmb)->dcfeb_set_PipelineDepth(*cfeb, currentPD);
          // usleep(100);
          // (*dmb)->Pipeline_Restart( *cfeb );
        }
      }
    }

    /**************************************************************************
     * SetFineDelayForADCFEB
     *
     *************************************************************************/

    SetFineDelayForADCFEB::SetFineDelayForADCFEB(Crate * crate)
      : Action(crate),
        Action2Values<int, int>(1, 0) {}

    void SetFineDelayForADCFEB::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set Fine Delay: delay(0-15), DCFEB(0-4):",
                                "FineDelay",
                                numberToString(value1()),
                                "DcfebNumber",
                                numberToString(value2()));
    }
    
    void SetFineDelayForADCFEB::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>SetFineDelayForADCFEB"<<endl; 
      int delay = getFormValueInt("FineDelay", in);
      int cfeb_number = getFormValueInt("DcfebNumber", in);
      value1(delay); // save the value
      value2(cfeb_number); // save the value
      
      for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        vector<CFEB> cfebs = (*dmb)->cfebs();
        (*dmb)->dcfeb_fine_delay(cfebs.at(cfeb_number), delay); // careful, I this may depend on the order in the xml
        usleep(100);
        (*dmb)->Pipeline_Restart(cfebs[cfeb_number]); // careful, I this may depend on the order in the xml
      }
    }

    /**************************************************************************
     * PipelineDepthScan with Pulses
     *
     *************************************************************************/

    PipelineDepthScan_Pulses::PipelineDepthScan_Pulses(Crate * crate, emu::odmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<int, int>(50, 70) {}

    void PipelineDepthScan_Pulses::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan pipeline depth with pulses",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void PipelineDepthScan_Pulses::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>PipelineDepthScan_Pulses"<<endl; 
      int fromDepth = getFormValueInt("From", in);
      value1( fromDepth ); // save the value

      int toDepth = getFormValueInt("To", in);
      value2( toDepth ); // save the value

      const int strip_to_pulse = 8; // TODO: make configurable
      int n_pulses = 5; // TODO: make configurable

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
        (*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
      }
      
      // set register 0 appropriately for communication over the VME backplane,
      // this is necessary for the CCB to issue hard resets and to respond to L1
      // requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);
      usleep(100);

      //// Hard Reset ////
      // ccb_->HardReset_crate(); // send a simple hard reset without
      // any sleeps usleep(150000); // need at least 150 ms after hard reset
      ccb_->hardReset();
      
      //// Set DAC (pulse height) ////
      float DAC = 1.0;
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_dac(DAC,DAC); // I'm not sure this is working. -Joe
      }
      usleep(100);
      
      //// Set comparator thresholds ////
      float ComparatorThresholds = 0.05;
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_comp_thresh(ComparatorThresholds);
      }
      usleep(100);
      
      //// Resync ////
      //ccb_->l1aReset();
      //usleep(10000);
      
      //// SetUpPrecisionCapacitors
      
      //// This should enable CLCT pretriggers and trigger with a CLCT
      //tmb_->SetClctPatternTrigEnable(1); // set flag to enable CLCT pretriggers
      //usleep(1000);
      //tmb_->WriteRegister(emu::pc::seq_trig_en_adr); // make it so
      //usleep(1000);
      //tmb_->SetTmbAllowClct(1); // set flag to enable CLCT only triggers
      //usleep(1000);
      //tmb_->SetTmbAllowMatch(1); // set flag to enable CLCT+ALCT match triggers
      //usleep(1000);
      //tmb_->WriteRegister(emu::pc::tmb_trig_adr); /// make it so
      //usleep(1000);

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      if(n_pulses>0){
        ccb_->EnableL1aFromDmbCfebCalibX();
        usleep(100);
        
        for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
          (*dmb)->set_ext_chanx(strip_to_pulse);
          (*dmb)->buck_shift();
          usleep(10);
        }
      }else{
        for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
          (*dmb)->shift_all(NORM_RUN);
          (*dmb)->buck_shift();
          usleep(10);
        }
      }
      usleep(100);
      ccb_->l1aReset(); // needed after shifting buckeyes
      usleep(100);
      
      string subdir = "PDScan_Pulses_" + emu::utils::getDateTime( true );
      manager_->setDAQOutSubdir( subdir );
      //
      // Loop over the requested range of pipeline depth
      //
      for ( int iDepth = fromDepth; iDepth <= toDepth; ++iDepth ){
        //// Set the pipeline depth on all DCFEBs ////
        for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
          vector<CFEB> cfebs = (*dmb)->cfebs();
          for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
            
            (*dmb)->dcfeb_set_PipelineDepth( *cfeb, iDepth );
            usleep(100);
            (*dmb)->Pipeline_Restart( *cfeb );
            usleep(100);
          }
        }
        ccb_->l1aReset(); // needed after setting/restarting pipeline
        usleep(100);

        //// Start triggering
        ccb_->bc0();
        usleep(100);

        // start DAQ
        cout<<"starting DAQ..."<<endl;
        manager_->startDAQ( string("Pipeline")+emu::utils::stringFrom<int>( iDepth ) );

        // send pulses
        for(int i=0; i<n_pulses; ++i){
          ccb_->pulse(1,0);
          //ccb_->GenerateDmbCfebCalib0();
          usleep(10000);
        }
        
        // stop DAQ
        cout<<"stopping DAQ..."<<endl;
        manager_->stopDAQ();
        
      } // loop over next pipeline depth
      manager_->setDAQOutSubdir( "" );
    }

    /**************************************************************************
     * PipelineDepthScan with Cosmics
     *
     *************************************************************************/

    PipelineDepthScan_Cosmics::PipelineDepthScan_Cosmics(Crate * crate, emu::odmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<int, int>(50, 70) {}

    void PipelineDepthScan_Cosmics::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan pipeline depth with cosmics",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void PipelineDepthScan_Cosmics::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>PipelineDepthScan_Cosmics"<<endl; 
      int fromDepth = getFormValueInt("From", in);
      value1( fromDepth ); // save the value

      int toDepth = getFormValueInt("To", in);
      value2( toDepth ); // save the value

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
        (*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
      }
      
      // set register 0 appropriately for communication over the VME backplane,
      // this is necessary for the CCB to issue hard resets and to respond to L1
      // requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);
      usleep(10000);

      //// Hard Reset ////
      // ccb_->HardReset_crate(); // send a simple hard reset without
      // any sleeps usleep(150000); // need at least 150 ms after hard reset
      ccb_->hardReset();

      //// Set comparator thresholds ////
      float ComparatorThresholds = 0.05;
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_comp_thresh(ComparatorThresholds);
      }
      usleep(10000);
      
      //// Shift into normal mode
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->shift_all(NORM_RUN);
        (*dmb)->buck_shift();
        usleep(10);
      }
      usleep(10000);
      ccb_->l1aReset(); // needed after buckeye shift
      usleep(10000);

      string subdir = "PDScan_Cosmics_" + emu::utils::getDateTime( true );
      manager_->setDAQOutSubdir( subdir );

      //
      // Loop over the requested range of pipeline depth
      //
      for ( int iDepth = fromDepth; iDepth <= toDepth; ++iDepth ){
        
        //// Set the pipeline depth on all DCFEBs ////
        for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
          vector<CFEB> cfebs = (*dmb)->cfebs();
          for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
            
            (*dmb)->dcfeb_set_PipelineDepth( *cfeb, iDepth );
            usleep(100);
            (*dmb)->Pipeline_Restart( *cfeb );
            usleep(100);
          }
        }
        ccb_->l1aReset(); // needed after setting/restarting pipeline
        usleep(100);

        //// Start triggering
        ccb_->bc0();
        usleep(100);

        // start DAQ
        cout<<"starting DAQ..."<<endl;
        manager_->startDAQ( string("Pipeline")+emu::utils::stringFrom<int>( iDepth ) );

        //// Take cosmics
        ::sleep(1);
        
        // stop DAQ
        cout<<"stopping DAQ..."<<endl;
        manager_->stopDAQ();
        
      } // loop over next pipeline depth
      manager_->setDAQOutSubdir( "" );
    }
    
    /**************************************************************************
     * TmbDavDelayScan
     *
     *************************************************************************/

    TmbDavDelayScan::TmbDavDelayScan(Crate * crate, emu::odmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<int, int>(0, 31) {}

    void TmbDavDelayScan::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan TMB DAV delay with pulses",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void TmbDavDelayScan::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TmbDavDelayScan"<<endl; 
      int fromdelay = getFormValueInt("From", in);
      value1( fromdelay ); // save the value

      int todelay = getFormValueInt("To", in);
      value2( todelay ); // save the value

      const int strip_to_pulse = 8; // TODO: make configurable

      // set register 0 appropriately for communication over the VME
      // backplane, this is necessary for the CCB to issue hard resets
      // and to respond to L1 requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);

      //
      // SetUpPrecisionCapacitors
      //
      ccb_->hardReset();

      // tmb_->SetClctPatternTrigEnable(1); // enable CLCT pretriggers
      // tmb_->WriteRegister(emu::pc::seq_trig_en_adr);

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
        {
          (*dmb)->set_ext_chanx(strip_to_pulse);
          (*dmb)->buck_shift();
          usleep(100);
        }

      //
      // Loop over the requested range of dav delays
      //
      for ( int idelay = fromdelay; idelay <= todelay; ++idelay ){
        ccb_->l1aReset();
        usleep(100);
        
        for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
          (*dmb)->varytmbdavdelay(idelay);
        }
        
        ccb_->l1aReset();
        usleep(100);
        ccb_->bc0();
        
        manager_->startDAQ( string("TmbDavDelay")+emu::utils::stringFrom<int>( idelay ) );
        
        ccb_->pulse(1,0);
        //ccb_->GenerateDmbCfebCalib0();
        
        manager_->stopDAQ();
      }
    }

    /**************************************************************************
     * L1aDelayScan
     *
     *************************************************************************/

    L1aDelayScan::L1aDelayScan(Crate * crate, emu::odmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<int, int>(100, 140) {}

    void L1aDelayScan::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Scan CCB L1A delay with pulses",
                                "From",
                                numberToString(value1()),
                                "To",
                                numberToString(value2()));
    }

    void L1aDelayScan::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>L1aDelayScan"<<endl; 
      int fromdelay = getFormValueInt("From", in);
      value1( fromdelay ); // save the value

      int todelay = getFormValueInt("To", in);
      value2( todelay ); // save the value

      const int strip_to_pulse = 8; // TODO: make configurable

      // set register 0 appropriately for communication over the VME
      // backplane, this is necessary for the CCB to issue hard resets
      // and to respond to L1 requests from the TMB.
      ccb_->setCCBMode(CCB::VMEFPGA);

      //
      // SetUpPrecisionCapacitors
      //
      ccb_->hardReset();

      // enable L1A and clct_pretrig from any of dmb_cfeb_calib
      // signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      usleep(100);
      //ccb_->SetExtTrigDelay(19);
      //usleep(100);

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_ext_chanx(strip_to_pulse);
        (*dmb)->buck_shift();
        usleep(100);
      }


      //
      // Loop over the requested range of l1a delays
      //
      for ( int idelay = fromdelay; idelay <= todelay; ++idelay ){

        ccb_->l1aReset();
        usleep(100);
        
        ccb_->SetL1aDelay(idelay);

        ccb_->l1aReset();
        usleep(100);
        ccb_->bc0();
        
        manager_->startDAQ( string("L1aDelay")+emu::utils::stringFrom<int>( idelay ) );
    
        ccb_->pulse(1,0);
        //ccb_->GenerateDmbCfebCalib0();
        usleep(10);

        manager_->stopDAQ();
      }
    }

    /**************************************************************************
     * TMBHardResetTest
     *
     *************************************************************************/

    TMBHardResetTest::TMBHardResetTest(Crate * crate)
      : Action(crate),
        ActionValue<int>(100) {}

    void TMBHardResetTest::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "TMB Hard Reset Test, number of resets:",
                           "NumberOfHardResets",
                           numberToString(value()));
    }

    void TMBHardResetTest::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBHardResetTest"<<endl; 
      int NumberOfHardResets = getFormValueInt("NumberOfHardResets", in);
      value(NumberOfHardResets); // save the value

      out << "=== TMB Hard Reset Test ===\n";

      int expected_day = tmb_->GetExpectedTmbFirmwareDay();
      int expected_month = tmb_->GetExpectedTmbFirmwareMonth();
      int expected_year = tmb_->GetExpectedTmbFirmwareYear();
      int expected_type = tmb_->GetExpectedTmbFirmwareType();
      int expected_version = tmb_->GetExpectedTmbFirmwareVersion();
      int hiccup_number = 0;

      time_t now = time(0);
      struct tm* tm = localtime(&now);
      out << "Beginning time: " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << endl;

      int i; // we'll want to get the value of this after the loop is complete
      // in order to print how many succesful hard resets we ran
      bool firmware_lost = false;

      // the CCB writes to stdout every time it issues a hard rest, but we
      // don't care we turn this back on after the loop
      ostringstream waste;
      ccb_->RedirectOutput(&waste);

      for (i = 0;
           i < NumberOfHardResets && !firmware_lost;
           ++i){
        if (i % 500 == 0) {
          out << "Hard Reset Number " << i << endl;
        }

        //ccb_->hardReset(); // slow
        ccb_->HardReset_crate(); // no sleeps
        usleep(800000); // need at least 150 ms for hard resets

        const int maximum_firmware_readback_attempts = 2;
        int firmware_readback_attempts = 0;
        do {
          firmware_lost = false;
          tmb_->FirmwareDate(); // reads the month and day off of the tmb
          int actual_day = tmb_->GetReadTmbFirmwareDay();
          int actual_month = tmb_->GetReadTmbFirmwareMonth();
          tmb_->FirmwareYear(); // reads the year off of the tmb
          int actual_year = tmb_->GetReadTmbFirmwareYear();
          tmb_->FirmwareVersion(); // reads the version off of the tmb
          int actual_type = tmb_->GetReadTmbFirmwareType();
          int actual_version = tmb_->GetReadTmbFirmwareVersion();

          if ((actual_day != expected_day) ||
              (actual_month != expected_month) ||
              (actual_year != expected_year) ||
              (actual_type != expected_type) ||
              (actual_version != expected_version))
            {
              firmware_lost = true;
              hiccup_number++;
              usleep(1000); // sometimes the readback fails, so wait and try again
            }
          // if we haven't gone over our maximum number of readback attempts and
          // the firmware was "lost" (i.e. the readback didn't match the expected
          // values), then try again.

        } while (++firmware_readback_attempts < maximum_firmware_readback_attempts &&
                 firmware_lost);
      }

      ccb_->RedirectOutput(&cout);

      now = time(0);
      tm =  localtime(&now);
      out << "End time: " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << endl;
      out << "Number of hiccups: " << hiccup_number << endl;

      if(firmware_lost) {
        out << "The frimware was lost after " << i << " hard resets." << endl;
      } else {
        out << "The firmware was *never* lost after " << i << " hard resets." << endl;
      }
    }

    TMBRegisters::TMBRegisters(Crate * crate)
      : Action(crate) { }

    void TMBRegisters::display(xgi::Output * out)
    {
      addButton(out, "Read TMB Registers");
    }

    void TMBRegisters::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBRegisters"<<endl;      
      //Print out current registers
      out<< "GTX Registers for fibers 0-6 "<<endl;
      out<<" 0x14a \t"<<std::hex<<tmb_->ReadRegister(0x14a)<<endl; 
      out<<" 0x14c \t"<<std::hex<<tmb_->ReadRegister(0x14c)<<endl;
      out<<" 0x14e \t"<<std::hex<<tmb_->ReadRegister(0x14e)<<endl;
      out<<" 0x150 \t"<<std::hex<<tmb_->ReadRegister(0x150)<<endl;
      out<<" 0x152 \t"<<std::hex<<tmb_->ReadRegister(0x152)<<endl;
      out<<" 0x154 \t"<<std::hex<<tmb_->ReadRegister(0x154)<<endl;
      out<<" 0x156 \t"<<std::hex<<tmb_->ReadRegister(0x156)<<endl;
      out<<" 0x158 \t"<<std::hex<<tmb_->ReadRegister(0x158)<<endl;
      out<<"Hot Channel Masks"<<endl;
      out<<" 0x42 \t"<<std::hex<<tmb_->ReadRegister(0x42)<<endl;
      out<<" 0x4a \t"<<std::hex<<tmb_->ReadRegister(0x4a)<<endl;
      out<<" 0x4c \t"<<std::hex<<tmb_->ReadRegister(0x4c)<<endl;
      out<<" 0x4e \t"<<std::hex<<tmb_->ReadRegister(0x4e)<<endl;
      out<<" 0x50 \t"<<std::hex<<tmb_->ReadRegister(0x50)<<endl;
      out<<" 0x52 \t"<<std::hex<<tmb_->ReadRegister(0x52)<<endl;
      out<<" 0x54 \t"<<std::hex<<tmb_->ReadRegister(0x54)<<endl;
      out<<" 0x56 \t"<<std::hex<<tmb_->ReadRegister(0x56)<<endl;
      out<<" 0x58 \t"<<std::hex<<tmb_->ReadRegister(0x58)<<endl;
      out<<" 0x5a \t"<<std::hex<<tmb_->ReadRegister(0x5a)<<endl;
      out<<" 0x5c \t"<<std::hex<<tmb_->ReadRegister(0x5c)<<endl;
      out<<" 0x5e \t"<<std::hex<<tmb_->ReadRegister(0x5e)<<endl;
      out<<" 0x60 \t"<<std::hex<<tmb_->ReadRegister(0x60)<<endl;
      out<<" 0x62 \t"<<std::hex<<tmb_->ReadRegister(0x62)<<endl;
      out<<" 0x64 \t"<<std::hex<<tmb_->ReadRegister(0x64)<<endl;
      out<<" 0x66 \t"<<std::hex<<tmb_->ReadRegister(0x66)<<endl;
      out<<" 0x16e \t"<<std::hex<<tmb_->ReadRegister(0x16e)<<endl;
      out<<" 0x170 \t"<<std::hex<<tmb_->ReadRegister(0x170)<<endl;
      out<<" 0x172 \t"<<std::hex<<tmb_->ReadRegister(0x172)<<endl;
      out<<" 0x174 \t"<<std::hex<<tmb_->ReadRegister(0x174)<<endl;
      out<<" 0x176 \t"<<std::hex<<tmb_->ReadRegister(0x176)<<endl;
      out<<" 0x178 \t"<<std::hex<<tmb_->ReadRegister(0x178)<<endl;
      out<<"bad bit registers"<<endl;
      out<<" 0x122 \t"<<std::hex<<tmb_->ReadRegister(0x122)<<endl;
      out<<" 0x15c \t"<<std::hex<<tmb_->ReadRegister(0x15c)<<endl; 
      
    }
    
    TMBSetRegisters::TMBSetRegisters(Crate * crate, emu::odmbdev::Manager* manager)
      : Action( crate, manager ),
        Action2Values<string, string>("0x14a", "0x0001") {}

    void TMBSetRegisters::display(xgi::Output * out)
    {
      addButtonWithTwoTextBoxes(out,
                                "Set TMB Registers (hex)",
                                "Register",
                                value1(),
                                "Value",
                                value2());
    }

    void TMBSetRegisters::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBSetRegisters"<<endl; 
      int RegisterValue = getFormValueIntHex("Register", in);
      int setting = getFormValueIntHex("Value", in);

      std::stringstream hexstr_RegisterValue;
      hexstr_RegisterValue << std::hex << RegisterValue;
      value1( hexstr_RegisterValue.str() ); // save value in hex
      
      std::stringstream hexstr_setting;
      hexstr_setting << std::hex << setting;
      value2( hexstr_setting.str() ); // save value in hex
      
      tmb_->WriteRegister(RegisterValue,setting);                                                                                     
      usleep(100000);
      
      out<<"Set TMB Register: "<<std::hex<<RegisterValue<<" to "<<std::hex<<tmb_->ReadRegister(RegisterValue)<<endl;
    }
    TMBEnableCLCTInput::TMBEnableCLCTInput(Crate * crate)
      : Action(crate),
        ActionValue<int>(1) {}

    void TMBEnableCLCTInput::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Enable DCFEB:",
                           "(1-7)",
                           numberToString(value()));
    }

    void TMBEnableCLCTInput::respond(xgi::Input * in, ostringstream & out)
    {
      int DCFEBtoEnable = getFormValueInt("(1-7)", in);
      value(DCFEBtoEnable); // save the value
      if(DCFEBtoEnable ==1){
        tmb_->WriteRegister(0x4a,0xffff);
        tmb_->WriteRegister(0x4c,0xffff);
        tmb_->WriteRegister(0x4e,0xffff);
        if(tmb_->ReadRegister(0x4a)==0xffff && tmb_->ReadRegister(0x4c)==0xffff &&tmb_->ReadRegister(0x4e)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB1"<<endl;
      }
      if(DCFEBtoEnable ==2){
        tmb_->WriteRegister(0x50,0xffff);
        tmb_->WriteRegister(0x52,0xffff);
        tmb_->WriteRegister(0x54,0xffff);
        if(tmb_->ReadRegister(0x50)==0xffff && tmb_->ReadRegister(0x52)==0xffff &&tmb_->ReadRegister(0x54)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB2"<<endl;
      }
      if(DCFEBtoEnable ==3){
        tmb_->WriteRegister(0x56,0xffff);
        tmb_->WriteRegister(0x58,0xffff);
        tmb_->WriteRegister(0x5a,0xffff);
        if(tmb_->ReadRegister(0x56)==0xffff && tmb_->ReadRegister(0x58)==0xffff &&tmb_->ReadRegister(0x5a)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB3"<<endl;
      }
      if(DCFEBtoEnable ==4){
        tmb_->WriteRegister(0x5c,0xffff);
        tmb_->WriteRegister(0x5e,0xffff);
        tmb_->WriteRegister(0x60,0xffff);
        if(tmb_->ReadRegister(0x5c)==0xffff && tmb_->ReadRegister(0x5e)==0xffff &&tmb_->ReadRegister(0x60)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB4"<<endl;
      }
      if(DCFEBtoEnable ==5){
        tmb_->WriteRegister(0x62,0xffff);
        tmb_->WriteRegister(0x64,0xffff);
        tmb_->WriteRegister(0x66,0xffff);
        if(tmb_->ReadRegister(0x62)==0xffff && tmb_->ReadRegister(0x64)==0xffff &&tmb_->ReadRegister(0x66)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB5"<<endl;
      }
      if(DCFEBtoEnable ==6){
        tmb_->WriteRegister(0x16e,0xffff);
        tmb_->WriteRegister(0x170,0xffff);
        tmb_->WriteRegister(0x172,0xffff);
        if(tmb_->ReadRegister(0x16e)==0xffff && tmb_->ReadRegister(0x170)==0xffff &&tmb_->ReadRegister(0x172)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB6"<<endl;
      }
      if(DCFEBtoEnable ==7){
        tmb_->WriteRegister(0x174,0xffff);
        tmb_->WriteRegister(0x176,0xffff);
        tmb_->WriteRegister(0x178,0xffff);
        if(tmb_->ReadRegister(0x174)==0xffff && tmb_->ReadRegister(0x176)==0xffff &&tmb_->ReadRegister(0x178)==0xffff) out<<"FELICIDADES! Has prendido el DCFEB7"<<endl;
      }
    }

    TMBDisableCLCTInput::TMBDisableCLCTInput(Crate * crate)
      : Action(crate),
        ActionValue<int>(1) {}

    void TMBDisableCLCTInput::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Disable DCFEB:",
                           "(1-7)", 
                           numberToString(value()));
    }

    void TMBDisableCLCTInput::respond(xgi::Input * in, ostringstream & out)
    {
      int DCFEBtoDisable = getFormValueInt("(1-7)", in);
      value(DCFEBtoDisable); // save the value
      if(DCFEBtoDisable ==1){ 
        tmb_->WriteRegister(0x4a,0);
        tmb_->WriteRegister(0x4c,0);
        tmb_->WriteRegister(0x4e,0);
        if(tmb_->ReadRegister(0x4a)==0 && tmb_->ReadRegister(0x4c)==0 &&tmb_->ReadRegister(0x4e)==0) out<<"FELICIDADES! Has apagado el DCFEB1"<<endl;
      }
      if(DCFEBtoDisable ==2){  
        tmb_->WriteRegister(0x50,0);
        tmb_->WriteRegister(0x52,0);
        tmb_->WriteRegister(0x54,0);
        if(tmb_->ReadRegister(0x50)==0 && tmb_->ReadRegister(0x52)==0 &&tmb_->ReadRegister(0x54)==0) out<<"FELICIDADES! Has apagado el DCFEB2"<<endl;
      }
      if(DCFEBtoDisable ==3){
        tmb_->WriteRegister(0x56,0);
        tmb_->WriteRegister(0x58,0);
        tmb_->WriteRegister(0x5a,0);
        if(tmb_->ReadRegister(0x56)==0 && tmb_->ReadRegister(0x58)==0 &&tmb_->ReadRegister(0x5a)==0) out<<"FELICIDADES! Has apagado el DCFEB3"<<endl;
      }
      if(DCFEBtoDisable ==4){
        tmb_->WriteRegister(0x5c,0);
        tmb_->WriteRegister(0x5e,0);
        tmb_->WriteRegister(0x60,0);
        if(tmb_->ReadRegister(0x5c)==0 && tmb_->ReadRegister(0x5e)==0 &&tmb_->ReadRegister(0x60)==0) out<<"FELICIDADES! Has apagado el DCFEB4"<<endl;
      }
      if(DCFEBtoDisable ==5){
        tmb_->WriteRegister(0x62,0);
        tmb_->WriteRegister(0x64,0);
        tmb_->WriteRegister(0x66,0);
        if(tmb_->ReadRegister(0x62)==0 && tmb_->ReadRegister(0x64)==0 &&tmb_->ReadRegister(0x66)==0) out<<"FELICIDADES! Has apagado el DCFEB5"<<endl;
      }
      if(DCFEBtoDisable ==6){
        tmb_->WriteRegister(0x16e,0);
        tmb_->WriteRegister(0x170,0);
        tmb_->WriteRegister(0x172,0);
        if(tmb_->ReadRegister(0x16e)==0 && tmb_->ReadRegister(0x170)==0 &&tmb_->ReadRegister(0x172)==0) out<<"FELICIDADES! Has apagado el DCFEB6"<<endl;
      }
      if(DCFEBtoDisable ==7){
        tmb_->WriteRegister(0x174,0);
        tmb_->WriteRegister(0x176,0);
        tmb_->WriteRegister(0x178,0);
        if(tmb_->ReadRegister(0x174)==0 && tmb_->ReadRegister(0x176)==0 &&tmb_->ReadRegister(0x178)==0) out<<"FELICIDADES! Has apagado el DCFEB7)"<<endl;
      }
    }

    TMBDisableCopper::TMBDisableCopper(Crate * crate)
      : Action(crate) { }

    void TMBDisableCopper::display(xgi::Output * out)
    {
      addButton(out, "TMB Disable Copper");
    }

    void TMBDisableCopper::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>TMBDisableCopper"<<endl; 
      
      out<<" Initial TMB Register 42 = "<<std::hex<<tmb_->ReadRegister(42)<<endl;
      
      tmb_->SetDistripHotChannelMask(0,0x00000000ff);                       
      tmb_->SetDistripHotChannelMask(1,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(2,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(3,0x00000000ff);                        
      tmb_->SetDistripHotChannelMask(4,0x00000000ff);  
      tmb_->SetDistripHotChannelMask(5,0x00000000ff);                      
      tmb_->WriteDistripHotChannelMasks(); 
      
      out<<" TMB Register 42 = "<<std::hex<<tmb_->ReadRegister(42)<<endl;
      
    }
 

    /**************************************************************************
     * PulseWires (based on STEP test 16)
     *
     *************************************************************************/

    PulseWires::PulseWires(Crate * crate)
      : Action(crate), 
        ActionValue<int>(20) {}

    void PulseWires::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Pulse Wires with External Trigger Delay =",
                           "ExTrigDelay",
                           numberToString(value()));
    }

    void PulseWires::respond(xgi::Input * in, ostringstream & out)
    {
      ////////////////// -- set xml parameters specific to test 1

      int x_CCBmode = 1;
      int x_l1aDelay = 155;
      int x_alct_send_empty = 1;
      int x_alct_trig_mode = 0;
      int x_all_cfeb_active = 1;
      int x_clct_pretrig_enable = 0;
      int x_clct_readout_without_trig = 0;
      int x_clct_trig_enable = 1;
      int x_match_trig_enable = 0;
      int x_request_l1a = 1;
      int x_alct_l1a_delay = 146;

      ccb_->SetCCBmode(x_CCBmode);
      ccb_->Setl1adelay(x_l1aDelay);
      alct_->SetSendEmpty(x_alct_send_empty);
      alct_->SetTriggerMode(x_alct_trig_mode);
      tmb_->SetEnableAllCfebsActive(x_all_cfeb_active);
      tmb_->SetClctPatternTrigEnable(x_clct_pretrig_enable); // actually named pretrig in xml
      tmb_->SetAllowClctNontrigReadout(x_clct_readout_without_trig);
      tmb_->SetTmbAllowClct(x_clct_trig_enable);
      tmb_->SetTmbAllowMatch(x_match_trig_enable);
      tmb_->SetRequestL1a(x_request_l1a);
      alct_->SetL1aDelay(x_alct_l1a_delay);
      // tmb_->SetL1aDelay(x_alct_l1a_delay);
      alct_->configure();

      for(unsigned long int numReg = 0; numReg<  tmb_->TMBConfigurationRegister.size(); numReg++){
        unsigned long int x_address = tmb_->TMBConfigurationRegister.at(numReg);
        tmb_->WriteRegister(x_address);
      }
      
      //////////////////

      unsigned int ExTrigDelay = getFormValueInt("ExTrigDelay", in);
      value(ExTrigDelay); // save the value
      
      for(int AFEB_STANDBY=0; AFEB_STANDBY<=1; AFEB_STANDBY++){
        ostream noBuffer( NULL );
        const uint64_t nLayerPairs = 3; // Pairs of layers to scan, never changes. (Scans 2 layers at a time.)
        uint64_t events_per_layer    = 1; //parameters_["events_per_layer"]; normally 1000
        uint64_t alct_test_pulse_amp = 255; //parameters_["alct_test_pulse_amp"];

        // /home/cscme11/TriDAS/emu/emuDCS/PeripheralCore/include/emu/pc
        // CCB.h 
        ccb_->EnableL1aFromSyncAdb();
      
        // fixed to unsigned int
        ccb_->SetExtTrigDelay(ExTrigDelay); 

        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;

        std::cout<<" ExTrigDelay = "<<ExTrigDelay<<" AFEB_STANDBY = "<<AFEB_STANDBY<<std::endl;

        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;
        std::cout<<" ************************** "<<std::endl;

        uint64_t afebGroupMask = 0x7f; // AFEB mask - pulse all of them from test 16
        // uint64_t afebGroupMask = 0x3fff; // all afebs from test 14
        
        alct_->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_SYNC );

        tmb_->EnableClctExtTrig();
        alct_->SetInvertPulse_(ON);  
        std::cout<<" invert pulse has been set to "<<alct_->Get_InvertPulse()<<std::endl;

        alct_->FillTriggerRegister_();
        alct_->WriteTriggerRegister_();

        // added a call to the print out 
        alct_->PrintTriggerRegister_();

        // added a call to the config printout 
        alct_->PrintALCTConfiguration();

        for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair ){

          // reprogram standby register to enable 2 layers at a time
          //const int standby_fmask[nLayerPairs] = {066, 055, 033};

          if(AFEB_STANDBY==1){

            for (int lct_chip = 0;
                 lct_chip < alct_->MaximumUserIndex() / 6;
                 lct_chip++){
              //int astandby = standby_fmask[iLayerPair];
              for (int afeb = 0; afeb < 6; afeb++)
                {
                  //    alct_->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
                
                }
            }
            //          alct_->WriteStandbyRegister_();
            ::sleep(10);
          }
          //ccb_->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
          ccb_->RedirectOutput( &cout ); // ccb prints a line on each test pulse - waste it
                      
          for ( uint64_t iPulse = 1; iPulse <= events_per_layer; ++iPulse ){

            // from test 14 also throw in this call
            //    ccb_->GenerateAlctAdbASync();
            //    usleep(10000);         

            // from test 16
            ccb_->GenerateAlctAdbSync();
            usleep(10);   
          } 
      
          ccb_->RedirectOutput (&cout); // get back ccb output

          /////////
             
        }
      }
    }

    /**************************************************************************
     * DDU_KillFiber
     *
     *************************************************************************/

    DDU_KillFiber::DDU_KillFiber(Crate * crate)
      : Action(crate),
        ActionValue<string>("read") {}

    void DDU_KillFiber::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Read(read)/Write(15bit hex#) DDU Kill Fiber",
                           "KillFiber",
                           value());
    }

    void DDU_KillFiber::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_KillFiber"<<endl; 
      int KillFiber = getFormValueIntHex("KillFiber", in);
      string KillFiberString = getFormValueString("KillFiber", in);
      //value( KillFiberString ); // save value in hex
      value("read"); // always default to "read"

      if( KillFiberString == "read" ){ // READ
        out << "DDU Read Kill Fiber:" << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu){
          out << "  DDU in slot " << (*ddu)->slot() << ": " << endl;
          out << "  DDU with ctrl fpga user code: " << (*ddu)->CtrlFpgaUserCode()
              << hex << setfill('0') // set up for next two hex values
              << " and vme prom user code: "
              << setw(8) << (*ddu)->VmePromUserCode()
              << " has Kill Fiber is set to: "
              << setw(4) << (*ddu)->readFlashKillFiber() << endl;
        }
      }else{  // WRITE
        out << "DDU Write Kill Fiber:" << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu){
          out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
          (*ddu)->writeFlashKillFiber(KillFiber);
        }
      }
    }

    /**************************************************************************
     * DDU_EthPrescale
     *
     *************************************************************************/

    DDU_EthPrescale::DDU_EthPrescale(Crate * crate)
      : Action(crate),
        ActionValue<string>("read") {}
    
    void DDU_EthPrescale::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Read(read)/Write(hex#) DDU Gb Eth Prescale",
                           "prescale",
                           value());
    }
    
    void DDU_EthPrescale::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_EthPrescale"<<endl; 
      int prescale = getFormValueIntHex("prescale", in);
      string prescaleString = getFormValueString("prescale", in);
      //value( prescaleString );
      value("read"); // always default to "read"      

      if(prescaleString == "read" ){ // READ
        out << "DDU Read Gb Eth Prescale: " << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
          out << "  DDU in slot " << (*ddu)->slot() << hex << setfill('0') << ": " << setw(4) << (*ddu)->readGbEPrescale() << dec << endl;
        }
        
      }else{ // WRITE
        out << "DDU Write Gb Eth Prescale: " << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
          out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
          (*ddu)->writeGbEPrescale(prescale);
        }
      }
    }

    /**************************************************************************
     * DDU_FakeL1
     *
     *************************************************************************/

    DDU_FakeL1::DDU_FakeL1(Crate * crate)
      : Action(crate),
        ActionValue<string>("read") {}
    
    void DDU_FakeL1::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
                           "Read(read)/Write(hex#) DDU Fake L1 (passthrough)",
                           "mode",
                           value());
    }
    
    void DDU_FakeL1::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>DDU_FakeL1"<<endl; 
      int mode = getFormValueIntHex("mode", in);
      string modeString = getFormValueString("mode", in);
      //value( modeString );
      value("read"); // always default to "read"
      
      if(modeString == "read" ){ // READ
        out << "DDU Read Fake L1 (passthrough): " << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
          out << "  DDU in slot " << (*ddu)->slot() << hex << setfill('0') << ": " << setw(4) << (*ddu)->readFakeL1() << dec << endl;
        }
        
      }else{ // WRITE
        out << "DDU Write Fake L1 (passthrough): " << endl;
        for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end(); ++ddu) {
          out << "  DDU in slot " << (*ddu)->slot() << "..." << endl;
          (*ddu)->writeFakeL1(mode);
        }
      }
    }

    std::string ToUpper(std::string s){
      for(unsigned int i(0); i<s.length(); ++i){
        s.at(i)=toupper(s.at(i));
      }
      return s;
    }

    unsigned short CountSetBits(unsigned short x){
      unsigned short count(0);
      for(count =0; x; count++){
        x &= x-1;
      }
      return count;
    }
        
    string hexToBin(char inHex) {
      switch(inHex) {
      case ' ':
      case '0':
        return "0000";
      case '1':
        return "0001";
      case '2':
        return "0010";
      case '3':
        return "0011";
      case '4':
        return "0100";
      case '5':
        return "0101";
      case '6':
        return "0110";
      case '7':
        return "0111";
      case '8':
        return "1000";
      case '9':
        return "1001";
      case 'A':
      case 'a':
        return "1010";
      case 'B':
      case 'b':
        return "1011";
      case 'C':
      case 'c':
        return "1100";
      case 'D':
      case 'd':
        return "1101";
      case 'E':
      case 'e':
        return "1110";
      case 'F':
      case 'f':
        return "1111";
      default:
        return "0";
      }
    } 
    
    string printBinary(unsigned int Number) {
      string hexRepn = FixLength(Number,4,true);
      string result("");
      for (unsigned int i=0;i<hexRepn.size();i++) {
        result+=hexToBin(hexRepn.at(i));
      }
      return result;
    }
        
    unsigned int GetBitFlips (unsigned int ui1, unsigned int ui2) {
      string s_Binary1(printBinary(ui1)), s_Binary2(printBinary(ui2));
      if (s_Binary1.size()!=s_Binary2.size())
        cerr << "Error: strings of binary numbers not equal in length." << endl;
      unsigned int numFlips(0);
      for (unsigned int i=0;i<s_Binary1.size();i++) {
        if (s_Binary1.at(i)!=s_Binary2.at(i)) numFlips++;
      }
      return numFlips;
    }
        
    int write_eth_raw(std::string tag){
      time_t rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo = localtime(&rawtime);
      char year[10], month[10], day[10], hour[10], minute[10], second[10];
      strftime(year, 10, "%g", timeinfo ); strftime(month, 10, "%m", timeinfo ); strftime(day, 10, "%d", timeinfo ); 
      strftime(hour, 10, "%H", timeinfo ); strftime(minute, 10, "%M", timeinfo ); strftime(second, 10, "%S", timeinfo ); 
      std::ostringstream outnamestream;

      outnamestream << "/local/data/odmb_ucsb/raw/odmb_" << tag <<"_" <<year<<month<<day<<"_"
                    <<hour<<minute<<second<<".raw";
      std::string outname =outnamestream.str();
      FILE *outfile;
      outfile = fopen(outname.c_str(),"wb");

      unsigned short dduheader[] = {0xF860, 0x0602, 0x0200, 0x5000, 0x0000, 0x8000, 
                                    0x0001, 0x8000, 0x2FC1, 0x0001, 0x3030, 0x0001};
      unsigned short ddutrailer[] = {0x8000, 0x8000, 0xFFFF, 0x8000, 0x0001, 0x0005, 
                                     0xC2DB, 0x8040, 0xC2C0, 0x4918, 0x000E, 0xA000};
      unsigned short odmbewords[] = {0xE001, 0xE002, 0xE003, 0xE004};
      EthBuf myeth;
      eth_open("schar3",myeth);
      usleep(1);
      eth_register_mac();

      EthStr ethstr=eth_readmm(myeth);
      int n = ethstr.n_evt; int nevents = 0;
      vector<char> rbuf = ethstr.rbuf;

      while(n!=0){// && nevents < 10){
        fwrite(dduheader, sizeof(dduheader[0]), 12, outfile);
        //if((rbuf[n-7]|0x0F) == -1 && (rbuf[n-9]|0x0F) == -1 && (rbuf[n-11]|0x0F) == -1 && (rbuf[n-13]|0x0F) == -1)
        //  cout <<" matches"<<endl;
        //      printf("%02x%02x %02x%02x %02x%02x", rbuf[n-7], rbuf[n-8], rbuf[n-9], rbuf[n-10], rbuf[n-11], rbuf[n-12]);

        printf(" Event %d has %d words\n",nevents+1,n/2);
        for(int j=0; j<n; j = j+2){
          printf(" %02x%02x",rbuf[j+1]&0xff,rbuf[j]&0xff);
          if(j%16 == 14){   //print words in 24 columns, in three sets of 8 
            printf("   ");} 
          if(j%48 == 46){   //if header for odmb go to next event  
            printf("\n");}
          //if((rbuf[j]&0xF000)==0xF001) printf("X");
        }
        printf("\n\n");

        for(int j=0; j<n-6; j++){
          fwrite(&rbuf[j], sizeof(rbuf[j]), 1, outfile);
        }
        fwrite(odmbewords, sizeof(odmbewords[0]), 4, outfile);
        fwrite(ddutrailer, sizeof(ddutrailer[0]), 12, outfile);
        nevents++;

        EthStr j_ethstr=eth_readmm(myeth);
        n = j_ethstr.n_evt;
      }

      if(outfile) fclose(outfile);
      eth_close(myeth);

      cout<<"Written "<<outname<<endl;
      return nevents;
    }

    /**************************************************************************
     * ChangeSlotNumber 
     *
     * A domain-specific-lanaguage for issuing vme commands. 
     *************************************************************************/

    ChangeSlotNumber::ChangeSlotNumber(Crate * crate, emu::odmbdev::Manager* manager)
      : OneTextBoxAction(crate, manager, "update")
    {
      //cout << "Updating the slot number box" << endl; 
    }

    int parse(string str_temp){
      int str = 0;
      //If it ends in h:
      if (str_temp.size() > 2 && str_temp.substr(str_temp.size()-2,2) == "_h"){ 
        stringstream hex_string;
        hex_string << hex << str_temp.substr(0,str_temp.size()-2).c_str();
        hex_string >> str; 
      } else if (str_temp.size() > 2 && str_temp.substr(str_temp.size()-2,2) == "_d"){ //If it ends in d: 
        stringstream dec_string; 
        dec_string << dec << str_temp.substr(0,str_temp.size()-2).c_str();
        dec_string >> str;
      } else { //If it ends in nothing at all
        stringstream hex_string;
        hex_string << hex <<  str_temp.c_str();
        hex_string >> str;
      }
      return str;
    }

    int parse_default_d(string str_temp){
      int str = 0;
      //If it ends in h:
      if (str_temp.size() > 2 && str_temp.substr(str_temp.size()-2,2) == "_h"){ 
        stringstream hex_string;
        hex_string << hex << str_temp.substr(0,str_temp.size()-2).c_str();
        hex_string >> str; 
      }

      //If it ends in d: 
      else if (str_temp.size() > 2 && str_temp.substr(str_temp.size()-2,2) == "_d"){ 
        stringstream dec_string; 
        dec_string << dec << str_temp.substr(0,str_temp.size()-2).c_str();
        dec_string >> str;
      }
 
      //If it ends in nothing at all
      else { 
        stringstream dec_string;
        dec_string << dec <<  str_temp.c_str();
        dec_string >> str;
      }
      return str;
    }

    void ChangeSlotNumber::display(xgi::Output * out)
    {
      string str = static_cast<ostringstream*>( &(ostringstream() << Manager::getSlotNumber() ))->str();
      addButtonWithSlotTextBox(out, buttonLabel, "textbox", str.c_str());
    }

    void ChangeSlotNumber::respond(xgi::Input * in, ostringstream & out) {
      OneTextBoxAction::respond(in, out);
      
      unsigned int p_slot(Manager::getSlotNumber());
      Manager::setSlotNumber(atoi(textBoxContent.c_str()));
      out <<"Changed default slot to " 
          << Manager::getSlotNumber()<< "        (previous slot " << p_slot << ")" <<endl<<endl;
    } // End ChangeSlotNumber::respond


    /**************************************************************************
     * ExecuteVMEDSL
     *
     * A domain-specific-lanaguage for issuing vme commands. 
     *************************************************************************/

    ExecuteVMEDSL::ExecuteVMEDSL(Crate * crate, emu::odmbdev::Manager* manager)
      : FourTextBoxAction(crate, manager, "Run VME commands")
    {
      cout << " Initializing the DAQMBs, which are VMEModules" << endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb) 
        (*dmb)->start();
      nCommand = 0;
      //cout<<"Creating DAQ folder"<<endl;
      //manager_->setDAQOutSubdir( "" );
      //cout<<"Created DAQ folder"<<endl;
    }
    
    void ExecuteVMEDSL::respond(xgi::Input * in, ostringstream & out) {
      FourTextBoxAction::respond(in, out);
      
      nCommand++;
      out<<"****************   VME command "<<nCommand<<"   ***************"<<endl;
      int slot = Manager::getSlotNumber(); //the slot number to use
      unsigned int sleepTimer;
      unsigned int testReps(0);
      unsigned int addr;
      unsigned short int data;
      unsigned int pipeDepth;
      int irdwr, TypeCommand=0; 
      // irdwr:   
      // 0 bufread
      // 1 bufwrite 
      // 2 bufread snd  
      // 3 bufwrite snd 
      // 4 flush to VME (disabled)
      // 5 loop back (disabled)
      // 6 delay

      //------------------------------------------SETUP NAME/LOCATION OF OUTPUT LOG FILE------------------------------------------------
      //Variables needed for output log file
      char outputfilename[80]; //the name of the output file
      string log_subdir = "-1"; //the box number read from text box

      time_t rawtime;
      struct tm *timeinfo;
      time(&rawtime);
      timeinfo = localtime(&rawtime);

      //Read box number from test box
      if (textBoxContent3 != "") log_subdir = (textBoxContent3).c_str();
      
      //If log subdir is empty, just put the log file directly in the logfiles directory
      if (log_subdir == "-1"){ strftime( outputfilename, 80, "logfiles/%y_%m_%d_ODMB_testing.log", timeinfo );
        // cout << "output file name: " << outputfilename << endl; 
      }
      
      //Otherwise, put the log files into a new subdirectory specified by the box number
      else {
        //Put format with time information into temporary variable
        char temp_outputfilename[80]; //temporary, used to hold time information, but without box number information
        strftime( temp_outputfilename, 80, "%y_%m_%d_ODMB_testing.log", timeinfo );
        //Add the box number to this, and change type to string stream
        stringstream outputfilename3;
        outputfilename3 << "logfiles/" << log_subdir << "/" << temp_outputfilename;
        //Read this filename into "outputfilename", which has the proper type
        outputfilename3 >> outputfilename;
        //cout << "output file name: " << outputfilename << endl; 

        //Now we need to create the directory.  Start with a stringstream with the directory name
        stringstream temp_outputdirectory;
        temp_outputdirectory << "logfiles/" << log_subdir;
        //Convert this to a string
        string temp2_outputdirectory;
        temp2_outputdirectory = temp_outputdirectory.str();
        //Convert this to const char*.  
        const char* outputdirectory;
        outputdirectory = temp2_outputdirectory.c_str(); 
        //Use this to make the directory
        mkdir(outputdirectory, S_IRWXU);
      }

      //Needed to put time stamps in logs
      char timestamp[80];
      strftime( timestamp, 80, "%T", timeinfo );

      //Needed to write header in logs
      struct stat statbuffer;
      int writeheader = stat( outputfilename, &statbuffer );
           
      //Open logs and write header
      ofstream logfile;
      logfile.open( outputfilename, ios::app );
      if( writeheader!=0 ) logfile << "# R/W\tCommand\tData\tSlot\tTime\t\tComments" << endl;

      //------------------------------------------READ COMMANDS FROM BOX OR TEXT FILE------------------------------------------------
      istringstream countertext(textBoxContent1); //read the number of times to repeat commands 
      istringstream filetext(textBoxContent2); //input a file path to the wimp page, read commands from the file
      bool textFileMode(false); // Option to input a text file from the second box on WIMP
      string filePath, fileContents, file_line;
 
      //Read file containing commands
      if (filetext.str().size()) { 
        textFileMode = true;
        filePath = filetext.str();
        ifstream fileFromWIMP(filePath.c_str());
        while ( fileFromWIMP.good() ) {
          getline (fileFromWIMP,file_line);
          fileContents+=file_line;
          fileContents+="\n";
        }
        fileFromWIMP.close();
      }

      istringstream alltext; 
      if (textFileMode) alltext.str(fileContents); 
      else alltext.str(textBoxContent4); // get the text from the box                                                                                                      
      
      //Now alltext has the commands no matter whether you used the box or the file mode.
      string line, buffer;
      getline(countertext,line,'\n');
      const unsigned long repeatNumber=strtoul(line.c_str(),NULL,0);
      std::vector<std::string> allLines(0);
      while (getline(alltext,line,'\n')){
        allLines.push_back(line);
      }
      bool writeHex = true;

      //------------------------------------------EXECUTE COMMANDS (REST OF FUNCTION)------------------------------------------------
      for(unsigned int repNum=0; repNum<repeatNumber; ++repNum){
        unsigned int loopLevel(0);
        std::vector<unsigned int> loopCounter(0), loopMax(0), loopStart(0);
        for(unsigned int lineNum=0; lineNum<allLines.size(); ++lineNum){
          slot = Manager::getSlotNumber(); //Load the default slot number for each iteration
          unsigned int shiftedSlot = slot << 19;
          line=allLines.at(lineNum);
          while(line.length()>0 && (line.at(0)==' ' || line.at(0)=='\t')) line.erase(0,1);
          istringstream iss(line);

          // 0 :       End of run, stop execution
          // 1 :       Binary (simulation file style)
          // 2,r,R :   Reads in hex
          // 3,w,W :   Writes in hex
          // 4,rs,RS : Reads in hex
          // 5,ws,WS : Writes in hex
          // 6,bl,BL : Begin loop
          // 7,el,EL : End loop
          // 
          // >3 : Produces a sleep for that number of microseconds
          // 
          // <0 : Send an error message to the output and abort further execution.
          if(line.size()>1) iss >> buffer;
          else buffer = "#";  // This avoids problems when the line is too short
          buffer=ToUpper(buffer);
          if(buffer=="0")  {
            out<<"Found EOR, exiting."<<endl;
            break;// EOR instruction
          } else if(buffer=="1") { // Expect 32 bits for address and 32 bits for data
            string addr_str, data_str, tmp_str;

            while(addr_str.size()<32 && iss.good()){ // read in 32 bits for address
              iss >> tmp_str;
              addr_str += tmp_str;
            }
            while(data_str.size()<32 && iss.good()){ // read in 32 bits for data
              iss >> tmp_str;
              data_str += tmp_str;
            }
            if(addr_str.size()!=32 || data_str.size()!=32){
              out<<"ERROR: address("<<addr_str<<") or data("<<data_str
                 <<") is not 32 bits on line: "<<line<<endl;
              return;
            }
            // 26th and 25th "bits" from right tell read (10) or write (01)
            irdwr = (addr_str.at(addr_str.size()-26)=='1')? 2 : 3; 
            addr = binaryStringToUInt(addr_str);
            data = binaryStringToUInt(data_str);
            TypeCommand = irdwr;
          } else if(buffer=="2" || buffer=="R") { // Read in hex
            string addr_temp;
            iss >> addr_temp;   
            addr = parse(addr_temp); 
            string data_temp;
            iss >> data_temp;
            data = parse(data_temp);
            irdwr = 2; TypeCommand = 2;
            //cout << "Read command.  Addr in hex: " << hex << addr << " data in hex: " << hex << data << endl;
          } else if(buffer=="3" || buffer=="W") {
            string addr_temp;
            iss >> addr_temp;   
            addr = parse(addr_temp); 
            string data_temp;
            iss >> data_temp;
            data = parse(data_temp);
            irdwr = 3; TypeCommand = 3;
            //cout << "Write command.  Addr in hex: " << hex << addr << " data in hex: " << hex << data << endl;
          } else if(buffer=="4" || buffer=="RS") { // Read in hex with slot
            string addr_temp, data_temp, slot_temp;
            iss >> addr_temp >> data_temp >> slot;      
            addr = parse(addr_temp);
            data = parse(data_temp);
            irdwr = 2; TypeCommand = 4;
            //cout << "Read command.  Addr in hex: " << hex << addr << " data in hex: " << hex << data << " slot: " << dec << slot << endl;
          } else if(buffer=="5" || buffer=="WS") { // Write in hex with slot
            string addr_temp;
            iss >> addr_temp;   
            addr = parse(addr_temp);
            string data_temp;
            iss >> data_temp;
            data = parse(data_temp);
            if(addr >= 0x4000 && addr <= 0x4018) {iss >> dec >> data >> dec >> slot; writeHex = false;} 
            string slot_temp;
            iss >> slot;
            irdwr = 3; TypeCommand = 5;
            //cout << "Write command.  Addr in hex: " << hex << addr << " data in hex: " << hex << data << " slot: " << dec << slot << endl;
          } else if(buffer=="6" || buffer=="BL") {
            ++loopLevel;
            loopCounter.push_back(1);
            int thisLoopMax(0);
            iss >> dec >> thisLoopMax;
            loopMax.push_back(thisLoopMax);
            loopStart.push_back(lineNum);
            irdwr = -1; TypeCommand= -1; //Negative; do not send to VME - AD
          } else if(buffer=="7" || buffer=="EL") {
            if(loopCounter.at(loopLevel-1)<loopMax.at(loopLevel-1)){
              lineNum=loopStart.at(loopLevel-1);
              ++loopCounter.at(loopLevel-1);
            }else{
              loopCounter.pop_back();
              loopMax.pop_back();
              loopStart.pop_back();
              --loopLevel;
            }
            irdwr = -1; TypeCommand = -1; //Negative; do not send to VME - AD
          } else if(buffer=="8" || buffer=="SLEEP" || buffer=="WAIT") {
            iss >> sleepTimer;
            usleep(sleepTimer);
            continue; // Nothing else to do from this line.
          } else if(buffer=="RL1A") {
            TypeCommand = 9;
            iss >> sleepTimer;
          } else if(buffer=="RL1AM") {
            TypeCommand = 10;
            iss >> sleepTimer;
          } else if(buffer=="COMP_DDU") {
            TypeCommand = 11;
            iss >> testReps;
          } else if(buffer=="COMP_PC"){
            TypeCommand = 12;
            iss >> testReps;
          } else if(buffer=="BEGIN_DAQ"){
            manager_->startDAQ( string("DDU_ODMB_UCSB") );
            out << "BEGIN_DAQ"<<endl;
            logfile << "BEGIN_DAQ"<< "                "  << "    "<< timestamp << "\t" <<" Starting DAQ"<<endl;
            continue; // Next line, please.
          } else if(buffer=="END_DAQ"){
            manager_->stopDAQ();
            out << "END_DAQ"<<endl;
            logfile << "END_DAQ"<< "                "  << "    "<< timestamp << "\t" <<" Stopping DAQ"<<endl;
            continue; // Next line, please.
          } else if(buffer=="RESET_ETH"){
            EthBuf myeth;
            eth_open("schar3",myeth);
            eth_reset(myeth);
            eth_reset_close(myeth);
            out<<"RESET_ETH"<<endl;
            logfile<<"RESET_ETH"<<endl;
            continue; // Next line, please.
          } else if(buffer=="WRITE_ETH"){
            std::string tag;
            iss >> tag;
            int nevents = write_eth_raw(tag);
            out << "WRITE_ETH               "<<tag << " - Written "<<nevents<<" events"<<endl;
            logfile << "WRITE_ETH               "<<timestamp<<"\t"<<tag << " - Written "<<nevents<<" events"<<endl;
            continue; // Next line, please.

          } else if(buffer=="SET_SLOT"){
            int old_slot = slot; 
            string slot_temp;
            iss >> dec >> slot;
            Manager::setSlotNumber(slot);
            out << "SET_SLOT     "<<slot<<"           Changed default slot from "<<dec<<old_slot << " to " << slot << endl;
            continue;
          } else if(buffer=="SET_PIPE"){
            TypeCommand = 13;
            iss >> pipeDepth;
            out << "SET_PIPE   " << pipeDepth << endl;
          } else if (buffer=="BURN_IN_TEST") { 
	    out << vme_wrapper_->BurnInODMBs().c_str() << endl;
	  } else { // Anything else is treated as a comment.
            out  << line << endl;
            logfile << "# " << line << endl;
            continue; // Next line, please.
          }

          //// We are at the end of official commands, add the rest to comments - KF
          string comments;
          getline( iss, comments );
          while(comments.size()>0 && (comments.at(0)==' ' || comments.at(0)=='\t')) comments.erase(0,1);
          //// Output type of command is always W or R - KF
          //// Added command types L for loops, T for tests - AD
          string commandtype;
          switch(irdwr){
          case 2:
            commandtype="R";
            break;
          case 3:
            commandtype="W";
            break;
          case -1:
            commandtype="L";
            break;
          case 11:
          case 12:
            commandtype="T";
            break;
          default:
            commandtype="Error with command type!";
            break;
          }

          //// If we make it here, we have a VME command to run! ////
          //Only if TypeCommand>0 due to addition of loop! - AD
          //printf("TypeCommand: %d\n", TypeCommand);
          //out << TypeCommand << endl;

          // Set the top bits of address to the slot number
          int nDigits = 5;
          unsigned short int VMEresult(0);
          if(TypeCommand>=2 && TypeCommand<=5){
            if(irdwr==2) {
              VMEresult = vme_wrapper_->VMERead(addr,slot,comments);
              usleep(1);
            }
            else if(irdwr==3) {
              vme_wrapper_->VMEWrite(addr,data,slot,comments);
              usleep(1);
            }
            else cerr << "Error: the wrapper couldn't handle your command!";
            printf("\n");
            fflush(stdout);
            
            
            // Output to website
            stringstream slot_stream; 
            slot_stream << slot;
            string slot_s = slot_stream.str(), label = "  ", command;
            bool readHex = true;
            if((addr >= 0x321C && addr <= 0x337C) || (addr >= 0x33FC && addr < 0x35AC)  || 
               (addr > 0x35DC && addr <= 0x3FFF) || addr == 0x500C || addr == 0x510C ||
               addr == 0x520C || addr == 0x530C || addr == 0x540C   || addr == 0x550C || addr == 0x560C 
               || addr == 0x8004 ||  (addr == 0x5000 && VMEresult < 0x1000) || (addr >= 0x9400 && addr <= 0x940C)) {
              readHex = false;
              label = "_d";
            }
            switch (irdwr) {
            case 2:
              command = "R  ";
              if(slot != Manager::getSlotNumber()) {
                command = "RS ";
                comments = slot_s + "\t" + comments;
              }
              out << command << FixLength(addr) <<  "        "  << FixLength(VMEresult, nDigits, readHex) << label  << "   "<<comments<<endl;
              logfile << command << FixLength(addr) <<  "        "  << FixLength(VMEresult, nDigits, readHex) << label << "   "
                      << timestamp << "\t" << comments<<endl;
              break;
            case 3:
              command = "W  ";
              if(slot != Manager::getSlotNumber()) {
                command = "WS ";
                comments = slot_s + "\t" + comments;
              }
              out << command << FixLength(addr) << " " << FixLength(data, nDigits, true) << label <<  "          "<<comments<<endl;
              logfile << command << FixLength(addr) << "  " << FixLength(data, nDigits, true) <<  label << "          "
                      << timestamp << "\t" << comments<<endl;
              break;
            }
          } else if (TypeCommand==9 || TypeCommand==10){
            unsigned int read_fifo = 0x005000;
            unsigned int reset_fifo = 0x005020;
            VMEresult = vme_wrapper_->VMERead(read_fifo,slot,"Read the 1st word for L1A_MATCH");
            if(TypeCommand == 9) VMEresult = vme_wrapper_->VMERead(read_fifo,slot,"Read the 2nd word for L1A");
            usleep(1);
            
            data = 0xff;
            vme_wrapper_->VMEWrite(reset_fifo,data,slot,"Reset fifo");
            usleep(1);
            long l1a_cnt = VMEresult;
            string command = "RL1A ", l1a_comment = "L1A";
            if(TypeCommand == 10) {command = "RL1AM";l1a_comment = "L1A_MATCH";}
            if(l1a_cnt < 0x1000){
              out << command<<"           "  << FixLength(l1a_cnt, nDigits, false)  << "    "<<comments<<endl;
              logfile << command<< "           "  << FixLength(l1a_cnt, nDigits, false)  << "    "
                      << timestamp << "\t" << comments<<endl;
            } else {
              out << command<< "            XXXX"  << "    "<<"No good DCFEB "<<l1a_comment<<"_CNT read"<<endl;
              logfile << command<< "            XXXX"  << "    "<< timestamp << "\t" <<"No good DCFEB "<<l1a_comment<<"_CNT read"<<endl;
            }
          }else if (TypeCommand==11 || TypeCommand==12){
            std::string testType = TypeCommand==11 ? "DDU" : "PC ";
            char rcv_tx[2], rcv_rx[2];
            unsigned short rcv_tx_i(0), rcv_rx_i(0), rcv_diff_i(0);
            unsigned long bitChanges(0), bitMatches(0);
            unsigned short arg(0);
            const unsigned int addr_tx = TypeCommand==11 ? (0x005300 & 0x07ffff) | shiftedSlot : (0x005100 & 0x07ffff) | shiftedSlot;
            const unsigned int addr_rx = TypeCommand==11 ? (0x005400 & 0x07ffff) | shiftedSlot : (0x005200 & 0x07ffff) | shiftedSlot;
            for(unsigned int j(0); j<testReps; ++j){
              crate_->vmeController()->vme_controller(2, addr_tx, &arg, rcv_tx); //Read TX FIFO
              usleep(1);
              crate_->vmeController()->vme_controller(2, addr_rx, &arg, rcv_rx); //Read RX FIFO
              usleep(1);
              
              //Combine into int
              rcv_tx_i=(rcv_tx[1]*0x100) + rcv_tx[0];
              rcv_rx_i=(rcv_rx[1]*0x100) + rcv_rx[0];
              
              //Compare FIFOs
              rcv_diff_i=rcv_tx_i^rcv_rx_i;
              
              //Count bits set to 1 (where differences occur)
              unsigned int setBits(CountSetBits(rcv_diff_i));
              bitChanges+=setBits;
              bitMatches+=16-setBits;
            }
            if(bitChanges){
              out << "COMP_" << testType << FixLength(testReps, nDigits+2, false) << FixLength(bitChanges, nDigits+1, false) << "    Bit changes = " << bitChanges << ", bit matches = " << bitMatches << std::endl;
              logfile << "COMP_" << testType << FixLength(testReps, nDigits+2, false) << FixLength(bitChanges, nDigits+1, false) << "    " << timestamp << "\tBit changes = " << bitChanges << ", bit matches = " << bitMatches << std::endl;
            }else{
              out << "COMP_" << testType << FixLength(testReps, nDigits+2, false) << FixLength(bitChanges, nDigits+1, false) << "    All " << bitMatches << " bits match" << std::endl;
              logfile << "COMP_" << testType << FixLength(testReps, nDigits+2, false) << FixLength(bitChanges, nDigits+1, false) << "    " << timestamp << "\tAll " << bitMatches << " bits match" << std::endl;
            }
          } else if (TypeCommand == 13){
            cout << "Setting PD" << endl;

            for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
              vector <CFEB> cfebs = (*dmb)->cfebs();
        
              for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
                (*dmb)->dcfeb_set_PipelineDepth(*cfeb, pipeDepth);
                usleep(100);
                cout << "Set PD CFEB" << endl;
                (*dmb)->Pipeline_Restart( *cfeb );
              }
            }
          }     

        } // while parsing lines
        out<<endl;
      }
      
      //// close the logfile -KF
      logfile.close();
      
    } // End ExecuteVMEDSL::respond
    
    /**************************************************************************
     * SYSMON
     *
     * A small class to print the SYSMON
     **************************************************************************/
    SYSMON::SYSMON(Crate * crate) 
      : ButtonAction(crate,"ODMB Voltages/Temperatures") 
    { /* The choices here are really a blank constructor vs duplicating the ExecuteVMEDSL constructor.
         I've tried the former -- TD
      */
    }
    
    void SYSMON::respond(xgi::Input * in, ostringstream & out) { // TD
      out << "********** System Monitoring **********" << endl;
            
int slot = Manager::getSlotNumber();
      /*unsigned int odmb_id = vme_wrapper_->VMERead(0x4100, slot, "Checking ODMB version");
      unsigned int odmb_version = odmb_id >> 12u;
      unsigned int read_addr_vec[9] = {0x7150, 0x7120, 0x7000, 0x7160, 0x7140, 0x7100, 0x7130, 0x7110, 0x7170};
      string description[9] = {"C\t -  Thermistor 1 temperature", odmb_version<=3?"C\t -  Thermistor 2 temperature":"mA\t -  IPPIB: Current for PPIB", "C\t -  FPGA temperature", 
                               "V\t -  P1V0: Voltage for FPGA", "V\t -  P2V5: Voltage for FPGA", "V\t -  LV_P3V3: Voltage for FPGA", 
                               "V\t -  P3V6_PP: Voltage for PPIB", "V\t -  P5V: General voltage", "V\t -  P5V_LVMB: Voltage for LVMB"};
      //int precision[9] = {1, 1, 1, 2, 2, 2, 2, 2, 2};
      float voltmax[9] = {1.0, 1.0, 1.0, 1.0, 2.5, 3.3, 3.6, 5.0, 5.0};
      float result2[9];
      for (int i = 0; i < 9; i++){
        // old version...
        // crate_->vmeController()->vme_controller(2,read_addr,&data,rcv);
        //unsigned short int VMEresult = (rcv[1] & 0xff) * 0x100 + (rcv[0] & 0xff);
        //crate_->vmeController()->vme_controller(2,read_addr,&data,rcv);
        //VMEresult = (rcv[1] & 0xff) * 0x100 + (rcv[0] & 0xff);
        // new version ...
        unsigned short int VMEresult = vme_wrapper_->VMERead(read_addr_vec[i],slot,description[i]);
        VMEresult = vme_wrapper_->VMERead(read_addr_vec[i],slot,description[i]);
        if (i == 0 && VMEresult > 0xfff){
          cout << "ERROR: bad readout from system monitoring." << endl; out << "ERROR: bad readout from system monitoring." << endl;
          break;
        }
        if (i == 2){
          result2[i] = 503.975*VMEresult/4096.0 - 273.15;
        }else if (i > 1){
          result2[i] = VMEresult*2.0*voltmax[i]/4096.0;
        }else if (i == 0 || i == 1){
          if(i==1 && odmb_version>=4){
            result2[i] = VMEresult *5000.0/4096.0-10.0;
          }else{
            result2[i] = ((7.865766417e-10 * VMEresult - 7.327237418e-6) * VMEresult + 3.38189673e-2) * VMEresult - 9.678340882;
          }
        }

        //out << "R  " << FixLength(read_addr & 0xffff) << "        " << description[i] << ": " << setprecision(precision[i]) << result2[i] << endl;      
        out << fix_width(result2[i], 4) << " " << description[i]<< endl;
      }
      */
      string s_sysmon=vme_wrapper_->SYSMONReport(slot);
      out << s_sysmon;
      out<<endl;
    }

    CreateTestLog::CreateTestLog(Crate * crate, Manager* manager) 
      : SignatureTextBoxAction(crate, manager, "Save Test Log") 
    { 
      // blank constructor
    }
    
    void CreateTestLog::respond(xgi::Input * in, xgi::Output * out, std::ostringstream & ssout, std::ostringstream & log) { // JB-F
      SignatureTextBoxAction::respond(in, out, ssout);
      istringstream text(textBoxContent);
      string initials;
      getline(text,initials,'\n');
      
      // First obtain ODMB id and firmware version
      int slot = Manager::getSlotNumber();
      //addresses
      unsigned short int VMEresult = 0;
      int addr_read_fwv(0x004200);
      int addr_read_unique_id(0x004100);

      // Read firmware version
      VMEresult = vme_wrapper_->VMERead(addr_read_fwv,slot,"Read firmware version");
      string fwv = FixLength(VMEresult, 3, true); // Format result
      // Read unique id
      VMEresult = vme_wrapper_->VMERead(addr_read_unique_id,slot,"Read unique ID");
      string unique_id = FixLength(VMEresult, 4, true); // Format result

      cout << "Saving production test log for ODMB " << unique_id << endl;
      cout << "Tester: " << initials << endl;
      // create log file
      //string file_name("logfiles/odmb_#_fw_v");
      //string file_name("/data/odmb/logfiles/production_tests/odmb");
      string file_name("logfiles/button_tests/odmb");
      file_name += unique_id + string("_fwv") + fwv;
      file_name += string("_") + emu::utils::getDateTime(true) + string("_") + initials + string(".log");
      ofstream ofs(file_name.c_str(),ios::app);
      if(ofs.good()){   // print log header
        ofs << "Production test log for ODMB " << endl;
        ofs << "Tester: " << initials << endl;
        ofs << "Firmware version: " << fwv << endl;
        // Now copy the test outputs displayed on the web page into the log
        ofs << log.str();
      }
      
      ssout << "Created log file " << file_name << endl;
      
    }

    /*unsigned short JTAGWrapper(unsigned short IR, unsigned short DR, unsigned int nbits) {
      
      }*/

    LVMBtest::LVMBtest(Crate * crate, emu::odmbdev::Manager* manager) 
      : ThreeTextBoxAction(crate, manager, "LVMB test") 
    { 
      //This constructor intentionally left blank.
    }
    
    void LVMBtest::respond(xgi::Input * in, ostringstream & out,
                           const string& textBoxContent_in) { // TD
      ThreeTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;
      string hdr("********** LVMB **********");
      JustifyHdr(hdr);
      out_local << hdr;
      unsigned int slot(Manager::getSlotNumber());
      unsigned short int VMEresult;
      //unsigned short int data;

      istringstream textBoxContent1(textBoxContent);
      string volt1_string, volt2_string;
      textBoxContent1 >> volt2_string;
      textBoxContent1 >> volt1_string;
      float volt1 = atof(volt1_string.c_str());
      vector <int> hexes;
      vector <float> the_voltages;
      float volt2 = atof(volt2_string.c_str());
      if (!(volt1 > 0 && volt1 < 10)) volt1 = 0;
      if (!(volt2 > 0 && volt2 < 10)) volt2 = 0;
      float expectedV[7] = {volt1, volt1, volt2, volt1, volt2, volt2, volt2};
      float tol = atof(textBoxContent2.c_str());

      //addresses
      unsigned short int addr_sel_adc = 0x008020;
      unsigned short int addr_cntl_byte = 0x008000;
      unsigned short int addr_read_adc = 0x008004;
      unsigned short int addr_turn_on = 0x008010;
      unsigned short int addr_verify_on = 0x008014;
      int notConnected = 0;
      unsigned short int ctrl_byte_vec[7] =       {0x89, 0xB9, 0xA9, 0xD9, 0x99, 0xE9, 0xD9};
      unsigned short int ctrl_byte_vec2[7] =      {0x81, 0xB1, 0xA1, 0xD1, 0x91, 0xE1, 0xD1};
      unsigned short int ctrl_byte_vec_onoff[7] = {0xC9, 0xE9, 0xE9, 0x89, 0xD9, 0xA9, 0x99};
      vector <pair<float, int> > voltages[7], v1_vec[7], v2_vec[7];
      unsigned short int ADC_number_vec[7] =      {0x00, 0x04, 0x01, 0x05, 0x03, 0x06, 0x02};
      unsigned int n_p_on_off_fails(0);
      vector<bool> ADC_fail_pon_test(7,false), ADC_fail_poff_test(7,false), ADC_not_connected(7,false);
      vector<bool> ADC_10V_not_matched(7,false), ADC_5V_not_matched(7,false);
      unsigned short int on_off_ctrl_byte[2] =    {0x00, 0xFF};
      int pass = 0;
      int fail = 0;

      int nReps = atoi(textBoxContent3.c_str());

      //1) Test on-off 
      //unsigned short int ctrl_byte = 0xFF;
      for (int mode = 0; mode < 2; mode++){
        vme_wrapper_->VMEWrite(addr_turn_on, on_off_ctrl_byte[mode], slot, "Select ADC to power on" );
        usleep(10);
        //Read from ADC
        VMEresult = vme_wrapper_->VMERead(addr_verify_on, slot, "Read ADCs powered on" );
        usleep(10);
        for (int i = 0; i < 7; i++){
          //Write ADC to be read 
          vme_wrapper_->VMEWrite(addr_sel_adc, ADC_number_vec[i], slot, "Select ADC to be read");
          usleep(30);
          //Send control byte to ADC
          vme_wrapper_->VMEWrite(addr_cntl_byte, ctrl_byte_vec_onoff[i], slot, "Send control byte to ADC");
          usleep(30);
          //Read from ADC
          VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read ADC" );
          //crate_->vmeController()->vme_controller(2, addr_read_adc, &data, rcv); //often returns -100, maybe fixed by introducing sleeps?
          usleep(10);
          //Format result
          float voltage_result_1 = float(VMEresult)*10.0/float(0xfff);
          if (VMEresult == 65535 && i == 0){ 
            VMEresult = 0;
            ADC_not_connected[i] = true;
            notConnected++;
            if (notConnected == 3) {
              out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
              out_local << "LVMB not connected." << endl; 
              out << out_local.str();
              UpdateLog(vme_wrapper_, slot, out_local);
              return;
            }
          } else{
            //Error checking
            //bool already_failed = false;
            float voltage = voltage_result_1;
            if (mode == 0 && fabs(voltage) > .1) {
              n_p_on_off_fails++;
              ADC_fail_poff_test[i] = true;
            }
            if(mode == 1 && fabs(voltage) < .5) {
              n_p_on_off_fails++;
              ADC_fail_pon_test[i] = true;
            }
          }
        }//i-loop
      }
      // ouput of test
      if (notConnected>=1) {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "ADCs not connected: ";
        for (int ADC = 0; ADC < 7; ADC++){
          if (ADC_not_connected[ADC]) out_local << ADC_number_vec[ADC] << " ";
        }
        out_local << endl << endl;
        out << out_local.str();
        UpdateLog(vme_wrapper_, slot, out_local);
        return;
      } else if (n_p_on_off_fails>=1) {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "ADCs failing power-on/off test: ";
        for (int ADC = 0; ADC < 7; ADC++){
          if (ADC_fail_pon_test[ADC]==true||ADC_fail_poff_test[ADC]==true) out_local << ADC_number_vec[ADC] << " ";
        }
        out_local << endl << endl;
        out << out_local.str();
        UpdateLog(vme_wrapper_, slot, out_local);
        return;
      }

      //2) Test ADC
      for (int j = 0; j < nReps; j++){
        for (int i = 0; i < 7; i++){
          //Write ADC to be read 
          vme_wrapper_->VMEWrite(addr_sel_adc, ADC_number_vec[i], slot, "Select ADC to be read");
          usleep(10);
          //Send control byte to ADC
          vme_wrapper_->VMEWrite(addr_cntl_byte, ctrl_byte_vec[i], slot, "Send control byte to ADC -- 5V scale");
          usleep(100);
          //Read from ADC
          VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read ADC" );
          usleep(10);
          unsigned int hex_1 = VMEresult;
          float voltage_result_1 = float(VMEresult*10.0)/float(0xfff);
          //if (voltage_result_1 > 9) cout << "BAD.  received:  " << std::hex << (rcv[1] & 0xff) << " " << std::hex << (rcv[0] & 0xff) << " VME result: " << VMEresult << " voltage " << voltage_result_1 << endl;

          //Send control byte to ADC -- method 2
          vme_wrapper_->VMEWrite(addr_cntl_byte, ctrl_byte_vec2[i], slot, "Send control byte to ADC -- 10V scale");
          usleep(100);
          //Read from ADC
          VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read ADC" );
          int hex_2 = VMEresult;
          if (VMEresult == 65535 && i == 0){ 
            VMEresult = 0;
            //notConnected++;
            //if (notConnected == 1) out << "Failed test: LVMB not connected" << endl;
            //if (notConnected == 3) return; 
            break;
          }
          else{
            float voltage_result_2 = float(VMEresult)*5.0/float(0xfff);
            //Error checking
            bool already_failed = false;
            if (fabs(voltage_result_2 - voltage_result_1) > .05 && voltage_result_1 < 4.9){ already_failed = true; }
            float voltage = voltage_result_1; //(voltage_result_1 > 4.0 ? voltage_result_1 : 0.5*(voltage_result_1 + voltage_result_2));
            bool done = false;

            //Filling 10V range histogram
            bool bin_exists = false;
            for (unsigned int bin = 0; bin < v1_vec[ADC_number_vec[i]].size(); bin++){
              if (fabs(v1_vec[ADC_number_vec[i]][bin].first - voltage_result_1) < .00001){
                v1_vec[ADC_number_vec[i]][bin].second++;
                bin_exists = true;
                break;
              }
            }
            if(bin_exists == false) v1_vec[ADC_number_vec[i]].push_back(make_pair(voltage_result_1, 1));

            //Filling 5V range histogram
            bin_exists = false;
            for (unsigned int bin = 0; bin < v2_vec[ADC_number_vec[i]].size(); bin++){
              if (fabs(v2_vec[ADC_number_vec[i]][bin].first - voltage_result_2) < .00001){
                v2_vec[ADC_number_vec[i]][bin].second++;
                bin_exists = true;
                break;
              }
            }
            if(bin_exists == false) v2_vec[ADC_number_vec[i]].push_back(make_pair(voltage_result_2, 1));

            for (unsigned int l = 0; l < voltages[ADC_number_vec[i]].size(); l++){
              if (fabs(voltages[ADC_number_vec[i]][l].first - voltage) < .00001){
                voltages[ADC_number_vec[i]][l].second++;
                done = true;
                break;
              } 
            }
            
            if (done == false){
              voltages[ADC_number_vec[i]].push_back(make_pair(voltage, 1));
            }

            if (fabs(voltage - expectedV[ADC_number_vec[i]]) > tol) cout << "inst: " << dec << j << " Voltage 1: " << voltage_result_1 << "  from hex   " << hex << hex_1 << " Voltage 2: " << voltage_result_2 << " from hex: " << hex <<  hex_2  << " expected: " << expectedV[ADC_number_vec[i]] << hex << int(expectedV[ADC_number_vec[i]]*4095/10) << endl;
            hexes.push_back(hex_1);
            the_voltages.push_back(voltage_result_1);

          }
        }//i-loop
      }//j-loop
      cout << "Printing hex numbers" << endl;
      sort(hexes.begin(), hexes.end()); 
      sort(the_voltages.begin(), the_voltages.end()); 
      int old = 0;
      for (unsigned int i1 = 0; i1 < hexes.size(); i1++){
        if (hexes[i1] != old) cout << hex << hexes[i1] << " voltages " << the_voltages[i1] <<  endl;
        old = hexes[i1]; 
      }

      for (int i = 0; i < 7; i++){
        std::sort( voltages[ADC_number_vec[i]].begin(), voltages[ADC_number_vec[i]].end(), myfunction );
        std::sort( v1_vec[ADC_number_vec[i]].begin(), v1_vec[ADC_number_vec[i]].end(), myfunction );
        std::sort( v2_vec[ADC_number_vec[i]].begin(), v2_vec[ADC_number_vec[i]].end(), myfunction );

        cout <<endl<< "Printing everything for ADC " << ADC_number_vec[i] << ": Expected voltage " << expectedV[ADC_number_vec[i]] << endl;
        cout << "Histogram for 10V range"<<endl;
        for (unsigned int l = 0; l < v1_vec[ADC_number_vec[i]].size(); l++){
          printf("%6.4f   %5d \n", v1_vec[ADC_number_vec[i]][l].first, v1_vec[ADC_number_vec[i]][l].second);
          if ( fabs(v1_vec[ADC_number_vec[i]][l].first - expectedV[ADC_number_vec[i]]) > tol ) {
            fail += v1_vec[ADC_number_vec[i]][l].second;
            ADC_10V_not_matched[i] = true;
          }
          else pass += v1_vec[ADC_number_vec[i]][l].second;
        }
       
        cout << "Histogram for 5V range"<<endl;
        for (unsigned int l = 0; l < v2_vec[ADC_number_vec[i]].size(); l++){
          printf("%6.4f   %5d \n", v2_vec[ADC_number_vec[i]][l].first, v2_vec[ADC_number_vec[i]][l].second);
          if ( fabs(v2_vec[ADC_number_vec[i]][l].first - expectedV[ADC_number_vec[i]]) > tol ) {
            fail += v2_vec[ADC_number_vec[i]][l].second;
          }
          else pass += v2_vec[ADC_number_vec[i]][l].second;
          ADC_5V_not_matched[i] = true;
        }

      }
      //out << "Voltage reading failure rate: " << fail << " out of " << nReps*7  << ". " << endl;
      if (fail < nReps*7*.02) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      out_local << "Power on/off test on 7/7.";  
      out_local << "Read 200 voltages per device. ";
      out_local << "Voltage reading: " << pass << "/" << 2*nReps*7 << "." << endl; 
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }

    LVMB904::LVMB904(Crate * crate, emu::odmbdev::Manager* manager) 
      : RepeatTextBoxAction(crate, manager, "LVMB 904",300) 
    { 
      //This constructor intentionally left blank.
    }
    
    void LVMB904::respond(xgi::Input * in, ostringstream & out,
                          const string& textBoxContent_in) { // TD
      
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;
      string hdr("********** LVMB--Building 904 **********");
      JustifyHdr(hdr);
      out_local << hdr;
      const unsigned int slot(Manager::getSlotNumber());
      unsigned int nReps(atoi(textBoxContent.c_str()));

     
      unsigned short int VMEresult;
      unsigned short int data;


      //1) Test on-off 
      unsigned short int addr_turn_on = 0x008010;
      unsigned short int on_off_byte[2] = {0x00, 0xFF};
      unsigned int addr_sel_dcfeb = (0x001020);
      unsigned short int DCFEB_number[7] = {0x1, 0x2, 0x4, 0x08, 0x10, 0x20,0x40};
      unsigned int addr_set_int_reg = (0x00191C);
      unsigned int addr_read_hdr = (0x001F04);
      unsigned int addr_read_tlr = (0x001F08);
      unsigned int addr_read_tdo = (0x001F14);
      unsigned short int reg_user_code = 0x3C8;

      unsigned int nConn_DCFEBs(0);
      // Need a new power-on/off test for 904--merging with DCFEB JTAG
      for (int on_off=0; on_off<1; on_off++) { // once for off, once for on
        vme_wrapper_->VMEWrite(addr_turn_on,on_off_byte[on_off],slot,"Power-off/on all DCFEBs");
        usleep(500000);
        for (int d = 0; d < 7; d++){ // Loop over all DCFEBs
          // Select DCFEB (one bit per DCFEB)
          vme_wrapper_->VMEWrite(addr_sel_dcfeb,DCFEB_number[d],slot,"Select DCFEB (one bit per DCFEB)");
          // Read selected DCFEB
          //VMEresult = vme_wrapper_->VMERead(addr_read_dcfeb,slot,"Read selected DCFEB");
          // Set instruction register to *Read UserCode*
          vme_wrapper_->VMEWrite(addr_set_int_reg,reg_user_code,slot,"Set instruction register to *Read UserCode*");
          // Shift 16 lower bits
          vme_wrapper_->VMEWrite(addr_read_hdr,data,slot,"Shift 16 lower bits");
          // Read first half of UserCode
          VMEresult = vme_wrapper_->VMERead(addr_read_tdo,slot,"Read first half of UserCode");
          // check firmware version
          string s_result = FixLength(VMEresult, 4, true);
          string firmwareVersion = s_result.substr(1,1)+"."+s_result.substr(2,2);
          // Shift 16 upper bits
          vme_wrapper_->VMEWrite(addr_read_tlr,data,slot,"Shift 16 upper bits");
          // Read second half of UserCode
          VMEresult = vme_wrapper_->VMERead(addr_read_tdo,slot,"Read second half of UserCode");
          // check to see if DCFEB is connected
          if (FixLength(VMEresult, 4, true)=="DCFE") nConn_DCFEBs++;
        } // end loop over DCFEBs
        if ( (on_off==0 && nConn_DCFEBs>0) || (on_off==1 && nConn_DCFEBs<7) ) {
          out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
          out_local << "Failed to power-off/on DCFEBs." << endl;
          out << out_local.str();
          UpdateLog(vme_wrapper_, slot, out_local);
          return;
        }
      } // end power-off/on test
      vme_wrapper_->VMEWrite(addr_turn_on,0xFF,slot,"Power-on all DCFEBs"); // just to be safe
      usleep(900000);

      //2) Test ADCs
      vector <int> hexes;
      vector <float> the_voltages;

      int best_adc_chan[7] = {5, 6, 4, 6, 1, 6, 0};
      vector <pair<float, float> > cv_tol;
      cv_tol.push_back(make_pair (2.107,0.05));
      cv_tol.push_back(make_pair (2.081,0.05));
      cv_tol.push_back(make_pair (2.,0.05));
      cv_tol.push_back(make_pair (5.387,0.05));
      cv_tol.push_back(make_pair (5.4237,0.05));
      cv_tol.push_back(make_pair (3.2845,0.05));
      cv_tol.push_back(make_pair (5.5751,0.05));

      //addresses
      unsigned short int addr_sel_adc = 0x008020;
      unsigned short int addr_cntl_byte = 0x008000;
      unsigned short int addr_read_adc = 0x008004;
      vector <pair<float, int> > voltages[7], v1_vec[7];
      unsigned short int ADC_number_vec[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
      vector<bool> ADC_10V_not_matched(7,false);
      int pass = 0;
      int fail = 0;
      for (int j = 0; j < (int)nReps; j++){
        for (int ADC = 0; ADC < 7; ADC++){
          //Write ADC to be read 
          vme_wrapper_->VMEWrite(addr_sel_adc, ADC_number_vec[ADC], slot, "Select ADC to be read");
          usleep(10);
          // select best channel for test

          //Send control byte to ADC -- 10V range
          unsigned short int byte = 0x89 | (best_adc_chan[ADC]<<4);
          //Send control byte to ADC -- 10V range
          vme_wrapper_->VMEWrite(addr_cntl_byte, byte, slot, "Send control byte to ADC -- 10V range");
          usleep(100);
          //Read from ADC
          VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read from ADC");
          usleep(10);
          int hex_1 = VMEresult;
          if (VMEresult == 65535 && ADC == 0){ 
            VMEresult = 0;
            //notConnected++;
            //if (notConnected == 1) out << "Failed test: LVMB not connected" << endl;
            //if (notConnected == 3) return; 
            break;
          }
          else{
            float voltage_result_1 = float(VMEresult)*10.0/float(0xfff);
            //Error checking
            float voltage = voltage_result_1; //(voltage_result_1 > 4.0 ? voltage_result_1 : 0.5*(voltage_result_1 + voltage_result_2));
            bool done = false;

            //Filling 10V range histogram
            bool bin_exists = false;
            for (unsigned int bin = 0; bin < v1_vec[ADC_number_vec[ADC]].size(); bin++){
              if (fabs(v1_vec[ADC_number_vec[ADC]][bin].first - voltage_result_1) < .00001){
                v1_vec[ADC_number_vec[ADC]][bin].second++;
                bin_exists = true;
                break;
              }
            }
            if(bin_exists == false) v1_vec[ADC_number_vec[ADC]].push_back(make_pair(voltage_result_1, 1));

            for (unsigned int l = 0; l < voltages[ADC_number_vec[ADC]].size(); l++){
              if (fabs(voltages[ADC_number_vec[ADC]][l].first - voltage) < .00001){
                voltages[ADC_number_vec[ADC]][l].second++;
                done = true;
                break;
              } 
            }
            
            if (done == false){
              voltages[ADC_number_vec[ADC]].push_back(make_pair(voltage, 1));
            }

            if (fabs(voltage - cv_tol[ADC].first) > cv_tol[ADC].second) cout << "inst: " << dec << j << " Voltage 1: " << voltage_result_1 << "  from hex   " << hex << hex_1 << " expected: " << cv_tol[ADC].first << hex << int(cv_tol[ADC].first*4095/10) << endl;
            hexes.push_back(hex_1);
            the_voltages.push_back(voltage_result_1);

          }
        }//ADC-loop
      }//j-loop
      cout << "Printing hex numbers" << endl;
      sort(hexes.begin(), hexes.end()); 
      sort(the_voltages.begin(), the_voltages.end()); 
      int old = 0;
      for (unsigned int i1 = 0; i1 < hexes.size(); i1++){
        if (hexes[i1] != old) cout << hex << hexes[i1] << " voltages " << the_voltages[i1] <<  endl;
        old = hexes[i1]; 
      }

      for (int i = 0; i < 7; i++){
        std::sort( voltages[ADC_number_vec[i]].begin(), voltages[ADC_number_vec[i]].end(), myfunction );
        std::sort( v1_vec[ADC_number_vec[i]].begin(), v1_vec[ADC_number_vec[i]].end(), myfunction );

        cout <<endl<< "Printing everything for ADC " << ADC_number_vec[i] << ": Expected voltage " << cv_tol[i].first << endl;
        cout << "Histogram for 10V range"<<endl;
        for (unsigned int l = 0; l < v1_vec[ADC_number_vec[i]].size(); l++){
          printf("%6.4f   %5d \n", v1_vec[ADC_number_vec[i]][l].first, v1_vec[ADC_number_vec[i]][l].second);
          if ( fabs(v1_vec[ADC_number_vec[i]][l].first - cv_tol[i].first) > cv_tol[i].second ) {
            fail += v1_vec[ADC_number_vec[i]][l].second;
            ADC_10V_not_matched[i] = true;
          }
          else pass += v1_vec[ADC_number_vec[i]][l].second;
        }
       

      }
      //out << "Voltage reading failure rate: " << fail << " out of " << nReps*7  << ". " << endl;
      if (fail==0) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      out_local << "Successfully powered-on/off all DCFEBs. ";
      out_local << "ADC voltage readings: " << pass << "/" << nReps*7 << "." << endl; 
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }

    BurnInTest::BurnInTest(Crate* crate) :
      ButtonAction(crate, "Check all ODMBs in crate"){
    }

    void BurnInTest::respond(xgi::Input* in, ostringstream& out){
      string output = vme_wrapper_->BurnInODMBs();
      out << output << endl;
    }

    ReadODMBVitals::ReadODMBVitals(Crate* crate) :
      ButtonAction(crate, "ODMB Vitals"){
    }

    void ReadODMBVitals::respond(xgi::Input* in, ostringstream& out){
      unsigned int addr_unique_id(0x4100), addr_fwv(0x4200), addr_fwb(0x4300),
        addr_fw_mmod(0x4400), addr_fw_ymod(0x4500), addr_qpll_lock(0x3124);
      unsigned short int VMEresult;
      int slot(Manager::getSlotNumber());

      int qpll_lock(vme_wrapper_->VMERead(addr_qpll_lock,slot,"Read QPLL lock"));
      if(qpll_lock == 1) out << "****** ODMB Vitals: QPLL locked ******" << endl;
      else if(qpll_lock == 0) out << "****** ODMB Vitals:QPLL UNLOCKED ******" << endl;
      else out << "******* Read ODMB Vitals *******" << endl;

      VMEresult = vme_wrapper_->VMERead(addr_unique_id,slot,"Read unique ID");
      if (FixLength(VMEresult,4,true)=="BAAD") out << "Error: ODMB not connected or FW out-of-date." << endl;
      else {
        out << "R   4100         " << setw(4) << FixLength(VMEresult,4,true) << "     Unique ID" << endl;
        string fwv(FixLength(vme_wrapper_->VMERead(addr_fwv,slot,"Read fw version"),3,true));
        string mmdd(FixLength(vme_wrapper_->VMERead(addr_fw_mmod,slot,"Read fw mm/dd modified"),4,true));
        string yr(FixLength(vme_wrapper_->VMERead(addr_fw_ymod,slot,"Read fw year modified"),4,true));
        string date(mmdd.substr(0,2)+"/"+mmdd.substr(2,2)+"/"+yr);
        out << "R   4200         " << setw(4) << fwv << "     FW version. Date " << date << endl;
        VMEresult = vme_wrapper_->VMERead(addr_fwb,slot,"Read fw build");
        out << "R   4300         " << setw(4) << FixLength(VMEresult,4,true) << "     FW build" <<  endl;
      }
      out << endl;
    }

    MasterTest::MasterTest(Crate* crate, emu::odmbdev::Manager* manager):
      TextBoxAction(crate, manager, "One button to rule them all"){
    }

    void MasterTest::respond(xgi::Input* in, ostringstream& out){
      TextBoxAction::respond(in, out);

      t_actionvector* buttons(NULL);
      if(manager_==NULL) return;
      buttons=(manager_->currentActionVector_);
      if(buttons==NULL) return;
      size_t found_cfg = textBoxContent.find("cfg");
      bool do_cfg = found_cfg!=::string::npos;
      for(t_actionvector::iterator button(buttons->begin());
          button!=buttons->end();
          ++button){
        std::string short_arg(""), long_arg("");
        bool good_button(false);
        Action& button_ref(*(button->get()));
        if(typeid(button_ref)==typeid(LVMB904)){
          short_arg="100";
          long_arg="5000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(DCFEBJTAGcontrol)){
          short_arg="1";
          long_arg="10";
          good_button=true;
        }else if(typeid(button_ref)==typeid(DCFEBFiber)){
          short_arg="100";
          long_arg="10000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(DCFEBPulses)){
          short_arg="100";
          long_arg="4000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(CCBReg)){
          short_arg="100";
          long_arg="5000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(DDUPRBSTest)){
          short_arg="6";
          long_arg="60000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(PCPRBSTest)){
          short_arg="6";
          long_arg="60000";
          good_button=true;
        }else if(typeid(button_ref)==typeid(OTMBPRBSTest)){
          short_arg="6";
          long_arg="60001";
          good_button=true;
          /*}else if(typeid(button_ref)==typeid(MCSBackAndForthBPI)){
            short_arg="0";
            long_arg="1";
            good_button=true;*/
        }else if(typeid(button_ref)==typeid(DiscreteLogicTest)){
          short_arg="1000";
          long_arg="10000";
          good_button=true;
        }else{
          short_arg="0";
          long_arg="0";
        }

        std::cout << typeid(button_ref).name() << std::endl;
        RepeatTextBoxAction* const rtba_ptr(dynamic_cast<RepeatTextBoxAction*>(button->get()));
        ThreeTextBoxAction* const ttba_ptr(dynamic_cast<ThreeTextBoxAction*>(button->get()));
        // High-stats mode?
        size_t found = textBoxContent.find("h");
        bool highStat(found!=::string::npos);
        found = textBoxContent.find("d");
        bool dcfeb_only(found!=::string::npos);
        if(good_button && !do_cfg){
          if(rtba_ptr!=NULL){
            if (dcfeb_only) { // swapping the cables mode -- just do dcfeb tests
              if (typeid(button_ref)==typeid(DCFEBJTAGcontrol) || typeid(button_ref)==typeid(DCFEBFiber)){
                rtba_ptr->respond(in, out, long_arg);
              }
            }else { // usual mode -- do all tests
              if (highStat){
                rtba_ptr->respond(in, out, long_arg);
              }else{
                rtba_ptr->respond(in, out, short_arg);
              }
            }
          }else if(ttba_ptr!=NULL&&!dcfeb_only){ // usual mode -- do LVMB test
            if (highStat){
              ttba_ptr->respond(in, out, long_arg);
            }else{
              ttba_ptr->respond(in, out, short_arg);
            }
          }
        }
      } // Loop over all buttons

      // Setting good default values for configuration registers
      size_t found = textBoxContent.find("h");
      bool highStat(found!=::string::npos);
      if (highStat || do_cfg){
        const unsigned int slot(Manager::getSlotNumber());
        const unsigned int addr_lctl1a_dly(0x4000), addr_otmb_dly(0x4004), addr_alct_dly(0x400C);
        const unsigned int addr_inj_dly(0x4010), addr_ext_dly(0x4014), addr_callct_dly(0x4018);
        const unsigned int addr_kill(0x401C), addr_crateid(0x4020), addr_nwords(0x4028);
        const unsigned int addr_bpi_write(0x6000);
        vme_wrapper_->VMEWrite(addr_lctl1a_dly, 26,    slot, "Set LCT-L1A delay");
        vme_wrapper_->VMEWrite(addr_otmb_dly,   2,     slot, "Set OTMBDAV delay");
        vme_wrapper_->VMEWrite(addr_alct_dly,   31,    slot, "Set ALCTDAV delay");
        vme_wrapper_->VMEWrite(addr_inj_dly,    0,     slot, "Set INJPLS delay");
        vme_wrapper_->VMEWrite(addr_ext_dly,    0,     slot, "Set EXTPLS delay");
        vme_wrapper_->VMEWrite(addr_callct_dly, 0,     slot, "Set CALLCT delay");
        vme_wrapper_->VMEWrite(addr_kill,       0x1FF, slot, "Set KILL");
        vme_wrapper_->VMEWrite(addr_crateid,    0,     slot, "Set crate ID");
        vme_wrapper_->VMEWrite(addr_nwords,     4,     slot, "Set number of dummy words");
        vme_wrapper_->VMEWrite(addr_bpi_write,  0,     slot, "Write to PROM");
        usleep(1000000);
        out << "*** Set good default values for cfg registers ****"<<endl<<endl;
      }

    }

    PCPRBSTest::PCPRBSTest(Crate* crate, emu::odmbdev::Manager* manager):
      RepeatTextBoxAction(crate, manager, "PC PRBS Test", 6){
    }

    void PCPRBSTest::respond(xgi::Input* in, ostringstream& out,
                             const string& textBoxContent_in){

      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;
      string hdr("********** PC PRBS Test **********");
      JustifyHdr(hdr);
      out_local << hdr;
      const unsigned int slot(Manager::getSlotNumber());
      const unsigned int v2_slot(slot); // easy to switch back to v3-v2 version
      const unsigned int prbs_type_addr = 0x9300;
      const unsigned int prbs_txen_addr = 0x9100;
      const unsigned int prbs_rxen_addr = 0x9104;
      unsigned int num_sequences(atoi(textBoxContent.c_str()));

      vme_wrapper_->VMEWrite(prbs_type_addr, 1, slot, "Set PRBS-7 mode");
      vme_wrapper_->VMEWrite(prbs_type_addr, 1, v2_slot, "Set PRBS-7 mode");
      vme_wrapper_->VMEWrite(prbs_txen_addr, 1, slot, "Activate PC TX PRBS");
      usleep(1000);
      vme_wrapper_->VMEWrite(prbs_rxen_addr, num_sequences, v2_slot, "Activate PC RX PRBS");
      usleep(500*num_sequences);
      unsigned short num_errors(vme_wrapper_->VMERead(0x910C, v2_slot, "Read number of errors"));
      if (num_errors==0) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;     
      out_local << "Number of PRBS sequences: " << num_sequences << " (10,000 bits each)" << endl;
      out_local << "PRBS sequences matched: " << num_sequences-num_errors << " (expected " << num_sequences << ")" << endl;
      out_local << "PRBS bit errors: " << num_errors << " (expected 0)" << endl;
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }

    DDUPRBSTest::DDUPRBSTest(Crate* crate, emu::odmbdev::Manager* manager):
      RepeatTextBoxAction(crate, manager, "DDU PRBS Test", 6){
    }

    void DDUPRBSTest::respond(xgi::Input* in, ostringstream& out,
                              const string& textBoxContent_in){

      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;
      string hdr("********** DDU PRBS Test **********");
      JustifyHdr(hdr);
      out_local << hdr;
      const unsigned int slot(Manager::getSlotNumber());
      const unsigned int v2_slot(slot); // easy to switch back to v3-v2 version
      const unsigned int prbs_type_addr = 0x9300;
      const unsigned int prbs_txen_addr = 0x9000;
      const unsigned int prbs_rxen_addr = 0x9004;
      unsigned int num_sequences(atoi(textBoxContent.c_str()));

      vme_wrapper_->VMEWrite(prbs_type_addr, 1, slot, "Set PRBS-7 mode");
      vme_wrapper_->VMEWrite(prbs_type_addr, 1, v2_slot, "Set PRBS-7 mode");
      vme_wrapper_->VMEWrite(prbs_txen_addr, 1, slot, "Activate PC TX PRBS");
      usleep(1000);
      vme_wrapper_->VMEWrite(prbs_rxen_addr, num_sequences, v2_slot, "Activate PC RX PRBS");
      usleep(500*num_sequences);
      unsigned short num_errors(vme_wrapper_->VMERead(0x900C, v2_slot, "Read number of errors"));
      if (num_errors==0) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      out_local << "Number of PRBS sequences: " << num_sequences << " (10,000 bits each)" << endl;
      out_local << "PRBS sequences matched: " << num_sequences-num_errors << " (expected " << num_sequences << ")" << endl;
      out_local << "PRBS bit errors: " << num_errors << " (expected 0)" << endl;
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }

    LVMBtest_dos::LVMBtest_dos(Crate* crate, emu::odmbdev::Manager* manager):
      RepeatTextBoxAction(crate, manager, "LVMB histogram",100){ 
      //This constructor intentionally left blank.
    }
    
    void LVMBtest_dos::respond(xgi::Input* in, ostringstream& out,
                               const string& textBoxContent_in) { // JB-F
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      out << "********** Low Voltage Monitoring **********" << endl;
      unsigned int slot(Manager::getSlotNumber());
      unsigned short int VMEresult;
      //addresses
      unsigned int addr_sel_adc(0x008020), addr_cntl_byte(0x008000), addr_read_adc(0x008004);
      vector <pair<float, int> > v1_vec[7][8], v2_vec[7][8];

      unsigned int nReps = atoi(textBoxContent.c_str());
      unsigned int update(nReps/10);
      for (unsigned int rep = 0; rep < nReps; rep++){
        if (rep%update==0) printf("Sequence %d/%d\n",rep,nReps);
        for (unsigned int ADC = 0; ADC < 7; ADC++){
          // Select ADC
          vme_wrapper_->VMEWrite(addr_sel_adc, ADC, slot, "Select ADC");
          usleep(10);
          for (unsigned int channel = 0; channel < 8; channel++){
            //Send control byte to ADC
            unsigned short int byte = 0x81 | (channel<<4);
            vme_wrapper_->VMEWrite(addr_cntl_byte, byte, slot, "Send control byte to ADC -- 5V range");
            usleep(100);
            //Read from ADC
            VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read from ADC");
            usleep(10);
            float voltage_result_1 = float(VMEresult*5.0)/float(0xfff);
            // Filling 5V range histogram
            bool bin_exists = false;
            for (unsigned int bin = 0; bin < v1_vec[ADC][channel].size(); bin++){
              if (fabs(v1_vec[ADC][channel][bin].first - voltage_result_1) < .00001){
                v1_vec[ADC][channel][bin].second++;
                bin_exists = true;
                break;
              }
            }
            if(bin_exists == false) v1_vec[ADC][channel].push_back(make_pair(voltage_result_1, 1));

            //Send control byte to ADC -- 10V range
            byte = 0x89 | (channel<<4);
            vme_wrapper_->VMEWrite(addr_cntl_byte, byte, slot, "Send control byte to ADC -- 10V range");
            usleep(100);
            //Read from ADC
            VMEresult = vme_wrapper_->VMERead(addr_read_adc, slot, "Read from ADC");
            usleep(10);
            float voltage_result_2 = float(VMEresult)*10.0/float(0xfff);

            // Filling 10V range histogram
            bin_exists = false;
            for (unsigned int bin = 0; bin < v2_vec[ADC][channel].size(); bin++){
              if (fabs(v2_vec[ADC][channel][bin].first - voltage_result_2) < .00001){
                v2_vec[ADC][channel][bin].second++;
                bin_exists = true;
                break;
              }
            }
            if(bin_exists == false) v2_vec[ADC][channel].push_back(make_pair(voltage_result_2, 1));
          } // end loop over channels
        }//ADC-loop
      }//nReps-loop
      printf("Sequence %d/%d\n",nReps,nReps);

      for (int ADC = 0; ADC < 7; ADC++){
        for (int channel = 0; channel <8; channel++) {
          std::sort( v1_vec[ADC][channel].begin(), v1_vec[ADC][channel].end(), myfunction );
          std::sort( v2_vec[ADC][channel].begin(), v2_vec[ADC][channel].end(), myfunction );

          out <<endl<< "Printing everything for ADC " << ADC << ", channel " << channel << endl;
          out << "Histogram for 5V range"<<endl;
          for (unsigned int l = 0; l < v1_vec[ADC][channel].size(); l++){
            char line[100];
            sprintf(line,"%6.4f   %5d \n", v1_vec[ADC][channel][l].first, v1_vec[ADC][channel][l].second);
            out << line;
          }
       
          out << "Histogram for 10V range"<<endl;
          for (unsigned int l = 0; l < v2_vec[ADC][channel].size(); l++){
            char line[100];
            sprintf(line,"%6.4f   %5d \n", v2_vec[ADC][channel][l].first, v2_vec[ADC][channel][l].second);
            out << line;
          }
        } // channel loop     
      }// ADC loop
 
    }
    
    DDUFIFOTest::DDUFIFOTest(Crate* crate, emu::odmbdev::Manager* manager) :
      RepeatTextBoxAction(crate, manager, "DDU FIFO Test",100) { 
    }
    
    void DDUFIFOTest::respond(xgi::Input* in, ostringstream& out,
                              const string& textBoxContent_in) {
      out << "********** Test FIFO Consistency *********" << endl;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      istringstream countertext(textBoxContent);
      string line;
      getline(countertext,line,'\n');
      const unsigned long testReps=strtoul(line.c_str(),NULL,0);
      char rcv_tx[2], rcv_rx[2];
      char junk[2];
      unsigned short rcv_tx_i(0), rcv_rx_i(0), rcv_diff_i(0);
      unsigned long bitChanges(0), bitMatches(0);
      const int slot(Manager::getSlotNumber()); //the slot number to use
      const unsigned int shiftedSlot(slot << 19);
      const unsigned int addr_tx = (0x005300 & 0x07ffff) | shiftedSlot;
      const unsigned int addr_rx = (0x005400 & 0x07ffff) | shiftedSlot;
      unsigned short arg(0);
      vme_wrapper_->VMEWrite(0x3004, 0x1, slot, "Reset");
      usleep(1000000);
      vme_wrapper_->VMEWrite(0x5320, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x5420, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3300, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3304, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3308, 0x1, slot, "Reset");
      usleep(1000000);
      vme_wrapper_->VMEWrite(0x401C, 0x0, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3100, 0x0, slot, "Reset");
      vme_wrapper_->VMEWrite(0x400C, 0x5, slot, "Reset");
      vme_wrapper_->VMEWrite(0x4004, 0x2, slot, "Reset");
      for(unsigned int j(0); j<testReps; ++j){
        vme_wrapper_->VMEWrite(0x5320, 0x1, slot, "Reset");
        vme_wrapper_->VMEWrite(0x5420, 0x1, slot, "Reset");
        vme_wrapper_->VMEWrite(0x3200, 0x4, slot, "Reset");
        vme_wrapper_->VMERead(0x530C, slot, "Reset");
        unsigned int word_count((junk[1]*0x100) + junk[0]);
        vme_wrapper_->VMERead(0x4024, slot, "Reset");
        for(unsigned int word(0); word<word_count; ++word){
          vme_wrapper_->VMERead(addr_tx, slot, "Reset");
          usleep(1);
          crate_->vmeController()->vme_controller(2, addr_tx, &arg, rcv_tx); //Read TX
          usleep(1);
          crate_->vmeController()->vme_controller(2, addr_rx, &arg, rcv_rx); //Read RX

          //Combine into int
          rcv_tx_i=(rcv_tx[1]*0x100) + rcv_tx[0];
          rcv_rx_i=(rcv_rx[1]*0x100) + rcv_rx[0];

          //cout << hex << rcv_tx_i << " " << rcv_rx_i << endl;
          
          //Compare FIFOs
          rcv_diff_i=rcv_tx_i^rcv_rx_i;
          
          //Count bits set to 1 (where differences occur)
          unsigned int setBits(CountSetBits(rcv_diff_i));
          bitChanges+=setBits;
          bitMatches+=16-setBits;
        }
      }
      out << "Bit success/errors: " << bitMatches << " / " << bitChanges << std::endl;
      usleep(1);
    }
    
    PCFIFOTest::PCFIFOTest(Crate* crate, emu::odmbdev::Manager* manager) :
      RepeatTextBoxAction(crate, manager, "PC FIFO Test"/*,"1"*/) { 
    }
    
    void PCFIFOTest::respond(xgi::Input* in, ostringstream& out,
                             const string& textBoxContent_in) {
      out << "********** Test FIFO Consistency *********" << endl;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      istringstream countertext(textBoxContent);
      string line;
      getline(countertext,line,'\n');
      const unsigned long testReps=strtoul(line.c_str(),NULL,0);
      char rcv_tx[2], rcv_rx[2];
      char junk[2];
      unsigned short rcv_tx_i(0), rcv_rx_i(0), rcv_diff_i(0);
      unsigned long bitChanges(0), bitMatches(0);
      const int slot(Manager::getSlotNumber()); //the slot number to use
      const unsigned int shiftedSlot(slot << 19);
      const unsigned int addr_tx = (0x005100 & 0x07ffff) | shiftedSlot;
      const unsigned int addr_rx = (0x005200 & 0x07ffff) | shiftedSlot;
      unsigned short arg(0);
      vme_wrapper_->VMEWrite(0x3004, 0x1, slot, "Reset");
      usleep(1000000);
      vme_wrapper_->VMEWrite(0x5120, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x5220, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3300, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3304, 0x1, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3308, 0x1, slot, "Reset");
      usleep(1000000);
      vme_wrapper_->VMEWrite(0x410C, 0x0, slot, "Reset");
      vme_wrapper_->VMEWrite(0x3100, 0x0, slot, "Reset");
      vme_wrapper_->VMEWrite(0x400C, 0x5, slot, "Reset");
      vme_wrapper_->VMEWrite(0x4004, 0x2, slot, "Reset");
      for(unsigned int j(0); j<testReps; ++j){
        vme_wrapper_->VMEWrite(0x5120, 0x1, slot, "Reset");
        vme_wrapper_->VMEWrite(0x5220, 0x1, slot, "Reset");
        vme_wrapper_->VMEWrite(0x3200, 0x4, slot, "Reset");
        vme_wrapper_->VMEWrite(0x510C, 0x0, slot, "Reset");
        unsigned int word_count((junk[1]*0x100) + junk[0]);
        vme_wrapper_->VMERead(0x4024, slot, "Reset");

        for(unsigned int word(0); word<word_count; ++word){
          usleep(1);
          crate_->vmeController()->vme_controller(2, addr_tx, &arg, rcv_tx); //Read TX
          usleep(1);
          crate_->vmeController()->vme_controller(2, addr_rx, &arg, rcv_rx); //Read RX

          //Combine into int
          rcv_tx_i=(rcv_tx[1]*0x100) + rcv_tx[0];
          rcv_rx_i=(rcv_rx[1]*0x100) + rcv_rx[0];

          //cout << hex << rcv_tx_i << " " << rcv_rx_i << endl;
          
          //Compare FIFOs
          rcv_diff_i=rcv_tx_i^rcv_rx_i;
          
          //Count bits set to 1 (where differences occur)
          unsigned int setBits(CountSetBits(rcv_diff_i));
          bitChanges+=setBits;
          bitMatches+=16-setBits;
        }
      }
      out << "Bit success/errors: " << bitMatches << " / " << bitChanges << std::endl;
      usleep(1);
    }
    
    DCFEBPulses::DCFEBPulses(Crate* crate, Manager* manager):
      RepeatTextBoxAction(crate, manager, "DCFEB Pulses", 1){
    }

    void DCFEBPulses::respond(xgi::Input* in, ostringstream& out, const string& textBoxContent_in) {
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;      
      string hdr("********** DCFEB Pulses **********");
      JustifyHdr(hdr);
      out_local << hdr;

      const unsigned long repeatNumber=atoi(textBoxContent_in.c_str());
      
      int slot = Manager::getSlotNumber();
      
      unsigned int addr_odmb_reg(0x3000), addr_dcfeb_reg(0x3200), addr_dcfeb_resync(0x3014);
      unsigned int addr_sel_dcfeb(0x1020), addr_done_dcfeb(0x3120);
      unsigned int addr_set_kill(0x401C);
      unsigned short dr_injpls(59), dr_extpls(60), dr_l1a_match(58), dr_bc0(61);
      // unsigned short dr_bc0(61);
      unsigned short injpls(0x1), extpls(0x2), l1a_match(0x4), bc0(0x20);
      unsigned short int VMEresult;
      vector<unsigned int> n_injpls_reads(7,0), n_etxpls_reads(7,0), n_l1a_match_reads(7,0), n_bc0_reads(7,0);
      // vector<unsigned int> n_bc0_reads(7,0);
      vector<bool> dcfeb_connected(7,false);
      unsigned int nConnected(0);
      
      
      //       vme_wrapper_->VMEWrite(addr_odmb_reg, 0x3F, slot, "Set calibration mode");      
      //       vme_wrapper_->VMEWrite(addr_dcfeb_reg, injpls, slot, "Send INJPLS signal");
      //       usleep(100);
      //       vme_wrapper_->VMEWrite(addr_dcfeb_reg, extpls, slot, "Send EXTPLS signal");        
      //       usleep(100);
      //       vme_wrapper_->VMEWrite(addr_dcfeb_reg, bc0, slot, "Send BC0");     
      
      //       return;

      //  Set-up DCFEBs
      vme_wrapper_->VMEWrite(addr_set_kill,0,slot,"Set KILL");  
      vme_wrapper_->VMEWrite(addr_dcfeb_resync, 0x1, slot, "DCFEB resync--reset all counters");
      // Read which DCFEBs are connected
      VMEresult = vme_wrapper_->VMERead(addr_done_dcfeb,slot,"Read DCFEB done bits");
      if (VMEresult==0x0) {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "Error: could not find DCFEBs. Please check connections." << endl << endl;
        out << out_local.str();
        UpdateLog(vme_wrapper_, slot, out_local);
        return;
      }   
      unsigned short int done_bits(VMEresult);
      // loop over dcfebs
      // Send pulses repeatNumber times, read the result via JTAG
      for (unsigned int n(0); n<repeatNumber; n++) {
        vme_wrapper_->VMEWrite(addr_odmb_reg, 0x3F, slot, "Set calibration mode");      
        vme_wrapper_->VMEWrite(addr_dcfeb_reg, injpls, slot, "Send INJPLS signal");
        usleep(100);
        vme_wrapper_->VMEWrite(addr_dcfeb_reg, extpls, slot, "Send EXTPLS signal");       
        usleep(100);
        vme_wrapper_->VMEWrite(addr_dcfeb_reg, bc0, slot, "Send BC0");    
        usleep(100);
        vme_wrapper_->VMEWrite(addr_odmb_reg, 0x0, slot, "Set calibration mode");      
        vme_wrapper_->VMEWrite(addr_dcfeb_reg, l1a_match, slot, "Send L1A_MATCH signal");
      }
      for (unsigned short dcfeb(0); dcfeb<7; dcfeb++) {
        unsigned short dcfeb_bit(1<<dcfeb);
        // Is this DCFEB connected?
        if (!(dcfeb_bit&done_bits)) continue;
        dcfeb_connected[dcfeb]=true;
        nConnected++;
        vme_wrapper_->VMEWrite(addr_sel_dcfeb, dcfeb_bit, slot, "Select DCFEB");
        n_injpls_reads[dcfeb]=vme_wrapper_->JTAGRead(dr_injpls,12,slot);
        n_etxpls_reads[dcfeb]=vme_wrapper_->JTAGRead(dr_extpls,12,slot);
        n_l1a_match_reads[dcfeb]=vme_wrapper_->JTAGRead(dr_l1a_match,12,slot);
        n_bc0_reads[dcfeb]=vme_wrapper_->JTAGRead(dr_bc0,12,slot);
      } // end first loop over dcfebs
      // Analyze the results
      unsigned int nPassed[4] = {0,0,0,0};
      for (unsigned short dcfeb(0); dcfeb<7; dcfeb++) {
        if (!dcfeb_connected[dcfeb]) continue;
        /*if (n_injpls_reads[dcfeb]!=repeatNumber) nFails++; 
          if (n_etxpls_reads[dcfeb]!=repeatNumber) nFails++;
          if(n_bc0_reads[dcfeb]!=repeatNumber) nFails++;
          if(n_l1a_match_reads[dcfeb]!=repeatNumber) nFails++;
        */
        if (n_injpls_reads[dcfeb]>repeatNumber/5*4) nPassed[0]++;
        if (n_etxpls_reads[dcfeb]>repeatNumber/5*4) nPassed[1]++;
        if (n_bc0_reads[dcfeb]>repeatNumber/5*4) nPassed[2]++;
        if (n_l1a_match_reads[dcfeb]>repeatNumber/5*4) nPassed[3]++;
      }
      unsigned int nFailedSignals(0);
      for (unsigned int signal(0); signal<4; signal++) {
        if (nPassed[signal]<1) nFailedSignals++;
      }
      if (nFailedSignals==0) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      // Print the results
      for (unsigned short dcfeb(0); dcfeb<7; dcfeb++) {
        if (!dcfeb_connected[dcfeb]) continue;
        out_local << "DCFEB " << dcfeb+1 << ": "; 
        out_local << "INJPLS " << n_injpls_reads[dcfeb] << "/" << repeatNumber << ", "
                  << "EXTPLS: " << n_etxpls_reads[dcfeb] << "/" << repeatNumber << ", "
                  << "BC0: " << n_bc0_reads[dcfeb] << "/" << repeatNumber << ", "
                  << "L1A_MATCH: " << n_l1a_match_reads[dcfeb] << "/" << repeatNumber << "." << endl;      
        // out_local << "BC0 Result: " << n_bc0_reads << endl;      
      }
      out_local << endl;
      //vme_wrapper_->VMEWrite(addr_dcfeb_reg, 0x2, slot, "Reset counters");
      //out_local << "\t\t\t\t\t\tPASSED" << endl << endl;
      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }

    DCFEBJTAGcontrol::DCFEBJTAGcontrol(Crate* crate, emu::odmbdev::Manager* manager):
      RepeatTextBoxAction(crate, manager, "DCFEB JTAG Control",0){ 
      // Has a "repeat n times" textbox.
    }
    
    void DCFEBJTAGcontrol::respond(xgi::Input* in, ostringstream& out,
                                   const string& textBoxContent_in) { // JB-F
      const unsigned increment(32);
      ostringstream out_local;
      string hdr("********** DCFEB JTAG Control **********");
      JustifyHdr(hdr);
      out_local << hdr;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      const unsigned long repeatNumber=atoi(textBoxContent.c_str());
      vector<bool> v_UCRead(7,false); // tell us if we read UserCode of each DCFEB
      vector<unsigned int> v_nShiftReads(7,0); // number of times we successfully read shifted bits on ADC mask
      vector<string> v_firmwareVersion(7,""); // store the firmware version
      vector<unsigned int> v_nJTAGshifts(7,0); // number of JTAG shifts commands issued
      unsigned short int data(0x0);
      unsigned short int reg_user_code = 0x3C8;
      unsigned short int reg_dev_sel = 0x3C2;
      unsigned short int reg_dev_val = 0xC;
      unsigned short int reg_val_sel = 0x3C3;
      unsigned short int VMEresult;
      //addresses
      unsigned int addr_sel_dcfeb = (0x001020);
      unsigned int addr_set_int_reg = (0x00191C);
      unsigned int addr_read_hdr = (0x001F04);
      unsigned int addr_read_tlr = (0x001F08);
      unsigned int addr_shift_ht = (0x00170C);
      unsigned int addr_shift_dr_12(0x001B0C);
      unsigned int addr_read_tdo = (0x001F14);
      unsigned short int DCFEB_number[7] = {0x1, 0x2, 0x4, 0x08, 0x10, 0x20,0x40};
      unsigned int nConnected(0);
      unsigned short int start(0x0), end(0xFFF);
      int slot = Manager::getSlotNumber();
      unsigned attempts[7];

      for (int d = 0; d < 7; d++){ // Loop over all DCFEBs
        attempts[d]=0;
        // Select DCFEB (one bit per DCFEB)
        vme_wrapper_->VMEWrite(addr_sel_dcfeb,DCFEB_number[d],slot,"Select DCFEB (one bit per DCFEB)");
        // Read selected DCFEB
        //VMEresult = vme_wrapper_->VMERead(addr_read_dcfeb,slot,"Read selected DCFEB");
        // Set instruction register to *Read UserCode*
        vme_wrapper_->VMEWrite(addr_set_int_reg,reg_user_code,slot,"Set instruction register to *Read UserCode*");
        // Shift 16 lower bits
        vme_wrapper_->VMEWrite(addr_read_hdr,data,slot,"Shift 16 lower bits");
        // Read first half of UserCode
        VMEresult = vme_wrapper_->VMERead(addr_read_tdo,slot,"Read first half of UserCode");
        // check firmware version
        string s_result = FixLength(VMEresult, 4, true);
        string firmwareVersion = s_result.substr(1,1)+"."+s_result.substr(2,2);
        // Shift 16 upper bits
        vme_wrapper_->VMEWrite(addr_read_tlr,data,slot,"Shift 16 upper bits");
        // Read second half of UserCode
        VMEresult = vme_wrapper_->VMERead(addr_read_tdo,slot,"Read second half of UserCode");
        // check to see if DCFEB is connected
        if (FixLength(VMEresult, 4, true)!="DCFE") continue;
        else {
          if (v_firmwareVersion[d].empty()) v_firmwareVersion[d] = firmwareVersion;
          v_UCRead[d]=true;
          nConnected++;
        }
        if (repeatNumber==0) continue; // default: just read UserCodes
        unsigned index(0);
        for(unsigned int repNum=0; repNum<repeatNumber; ++repNum){ // repeat the test repNum times
          // Set instruction register to *Device select*
          vme_wrapper_->VMEWrite(addr_set_int_reg,reg_dev_sel,slot,"Set instruction register to *Device select*");
          // Set device register to *ADC mask*
          vme_wrapper_->VMEWrite(addr_shift_ht,reg_dev_val,slot,"Set device register to *ADC mask*");
          // Set IR to *Value select*
          vme_wrapper_->VMEWrite(addr_set_int_reg,reg_val_sel,slot,"Set IR to *Value select*");
          vector<string> tdi;
          vector<string> tdo;        
          vme_wrapper_->VMEWrite(addr_shift_dr_12,0x0,slot,"Set DR, shift 12 bits");
          usleep(100);
          for (unsigned short int reg_val_shft = start; reg_val_shft<=end; reg_val_shft+=increment) {
            ++v_nJTAGshifts[d];
            // Set DR, shift 12 bits
            vme_wrapper_->VMEWrite(addr_shift_dr_12,reg_val_shft,slot,"Set DR, shift 12 bits");
            usleep(100);
            tdi.push_back(FixLength(reg_val_shft, 3, true));
            // Read TDO
            VMEresult = vme_wrapper_->VMERead(addr_read_tdo,slot,"Read TDO");
            usleep(100);
            tdo.push_back(FixLength(VMEresult, 4, true));
            if (reg_val_shft == start){
              ++index;
              continue;
            }
            if (tdo.at(index).substr(0,3) == tdi.at(index-1)) ++v_nShiftReads[d];
            ++attempts[d];
            ++index;
          } // loop over words to shift
        } // repeat the test repNum times per DCFEB
      } // Loop over all DCFEBs
      if (nConnected==0) {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "Error: could not find DCFEBs. Please check connections." << endl;
      }else {
        unsigned int nFailedDCFEBs(0);
        // Now loop over DCFEBs again to count failed reads
        for (int d = 0; d < 7; d++){ 
          if (v_UCRead[d]==false) continue;
          if ((2*attempts[d]-v_nJTAGshifts[d]-v_nShiftReads[d])!=0) nFailedDCFEBs++;
        }
        // Now loop one more time to display results
        if (nFailedDCFEBs==0)  out_local << "\t\t\t\t\t\tPASSED" << endl;
        else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        for (int d = 0; d < 7; d++){ 
          if (v_UCRead[d]==false) continue;
          out_local << "DCFEB " << d+1 << ": ";
          out_local << "read UserCode.";
          out_local << " Firmware version " << v_firmwareVersion[d];
          if (repeatNumber==0) out_local << endl;
          else {
            out_local << " Sent " << 2*attempts[d] << " shift+read commands.";
            out_local << " Errors: " << 2*attempts[d]-v_nJTAGshifts[d]-v_nShiftReads[d] << "." << endl;
          }
        }
      }
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);

    }
    
    DCFEBFiber::DCFEBFiber(Crate* crate, Manager* manager):
      RepeatTextBoxAction(crate, manager, "DCFEB Fiber Test",100){ 
      // blank constructor
    }
    
    void DCFEBFiber::respond(xgi::Input* in, ostringstream& out,
                             const string& textBoxContent_in) { // JB-F
      ostringstream out_local;
      string hdr("********** DCFEB fiber tests **********");
      JustifyHdr(hdr);
      out_local << hdr;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      const unsigned long repeatNumber=atoi(textBoxContent.c_str());
      // repeatNumber is currently the number of packets to send
      int slot = Manager::getSlotNumber();
      unsigned int addr_odmb_rst(0x003004), addr_dcfeb_ctrl_reg(0x3200);
      unsigned int addr_dcfeb_resync(0x3014);
      unsigned int addr_set_kill(0x00401C), addr_read_done_bits(0x003120);
      unsigned int addr_read_nrx_pckt(0x00340C), addr_read_ncrcs(0x00360C);
      unsigned int addr_sel_dcfeb_fifo(0x005010), addr_reset_fifo(0x005020);
      unsigned int addr_read_word(0x005000), addr_sel_dcfeb(0x1020);
      // Commands
      //unsigned short int data;
      unsigned short dr_l1a(57);
      // unsigned short int dcfeb_done_bits[7] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40};
      unsigned short int cms_l1a_match(0x4);
      vector<bool> dcfeb_isConnected(7,false);
      unsigned int notConnected(0), n_l1a_match_before, n_l1a_match_after;
      vector<unsigned int> dcfeb_nRxPckt(7,0), dcfeb_nGoodCRCs(7,0);
      vector<unsigned int> dcfeb_L1A_cnt(7,0);
      // Results
      unsigned short int VMEresult;
      // == ================ Configuration ================ ==
      // Reset!
      vme_wrapper_->VMEWrite(addr_odmb_rst,0x1,slot,"Reset");
      usleep(1000000);
      // Set real data and internal triggers
      vme_wrapper_->VMEWrite(0x3300, 0x0, slot, "Set real data.");
      vme_wrapper_->VMEWrite(0x3304, 0x1, slot, "Set internal trigger.");
      // check which DCFEBs are connected
      VMEresult = vme_wrapper_->VMERead(addr_read_done_bits,slot,"Read DCFEB done bits");
      if (VMEresult==0x0) {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "Error: could not find DCFEBs. Please check connections." << endl << endl;
        out << out_local.str();
        UpdateLog(vme_wrapper_, slot, out_local);
        return;
      }
      unsigned short int done_bits(VMEresult);
      for (unsigned short int dcfeb(0x0); dcfeb<7; dcfeb++) {
        // Set KILL
        unsigned int cmd_kill_d = ~(unsigned int)pow(2,dcfeb);
        vme_wrapper_->VMEWrite(addr_set_kill,cmd_kill_d,slot,"Set KILL");
        if ((1<<dcfeb)&done_bits) { // is this DCFEB connected?
          //if (VMEresult==dcfeb_done_bits[dcfeb]){
          dcfeb_isConnected[dcfeb]=true;
          vme_wrapper_->VMEWrite(addr_sel_dcfeb_fifo,dcfeb+1,slot,"Select DCFEB FIFO");
          // Number of received packets before
          unsigned int addr_read_nrx_pckt_d = addr_read_nrx_pckt | (0x00F0&((dcfeb+1)<<4));
          unsigned int addr_read_ncrcs_d = addr_read_ncrcs | (0x00F0&((dcfeb+1)<<4));
          VMEresult = vme_wrapper_->VMERead(addr_read_nrx_pckt_d,slot,"Read number of received packets before");
          // Number of good CRCs before
          VMEresult = vme_wrapper_->VMERead(addr_read_ncrcs_d,slot,"Read number of good CRCs before");
          unsigned int nCntRst(0); // how many times we hit FFFF and restart the counter
          // == ============ Send N real packets ============ ==
          for (unsigned int p = 0; p < repeatNumber; p++) {
            // Send test L1A(_MATCH) to all DCFEBs
            vme_wrapper_->VMEWrite(addr_dcfeb_ctrl_reg,cms_l1a_match,slot,"Send test L1A(_MATCH) to all DCFEBs");
            usleep(100);
            // Read number of received packets
            VMEresult = vme_wrapper_->VMERead(addr_read_nrx_pckt_d,slot,"Read number of received packets");
            if (p>0&&VMEresult==0) nCntRst++; // keep track of how many times the counter resets
            // Reset for L1A_MATCH counter, and L1A counter
            if(p==repeatNumber-2) vme_wrapper_->VMEWrite(addr_reset_fifo,done_bits,slot,"Reset FIFOs");

          }
          // == ============ Status summary ============ ==
          // Read L1A counter
          VMEresult = vme_wrapper_->VMERead(addr_read_word,slot,"Read first word (L1A counter MSB)"); 
          //unsigned int l1ac_MSB(VMEresult&0x0FFF);
          VMEresult = vme_wrapper_->VMERead(addr_read_word,slot,"Read second word (L1A counter LSB)");
          unsigned int l1ac_LSB(VMEresult&0x0FFF);
          //dcfeb_L1A_cnt[dcfeb] = (l1ac_MSB<<12)|l1ac_LSB;
          dcfeb_L1A_cnt[dcfeb]=l1ac_LSB;
          vme_wrapper_->VMEWrite(addr_sel_dcfeb, 1<<dcfeb, slot, "Select DCFEB");
          n_l1a_match_before=vme_wrapper_->JTAGRead(dr_l1a,16,slot);
          vme_wrapper_->VMEWrite(addr_dcfeb_resync, 0x1, slot, "DCFEB resync--reset all counters");
          n_l1a_match_after=vme_wrapper_->JTAGRead(dr_l1a,16,slot);
          // Number of received packets
          VMEresult = vme_wrapper_->VMERead(addr_read_nrx_pckt_d,slot,"Read number of received packets");
          unsigned int nRxPckt(VMEresult+nCntRst*65536);
          dcfeb_nRxPckt[dcfeb]=nRxPckt;
          // Number of good CRCs
          VMEresult = vme_wrapper_->VMERead(addr_read_ncrcs_d,slot,"Read number of good CRCs");
          unsigned int nGoodCRCs(VMEresult+nCntRst*65536);
          dcfeb_nGoodCRCs[dcfeb]=nGoodCRCs;
        } else {// if DCFEB is not connected
          notConnected++;
          if (notConnected==7) {
            out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
            out_local << "Error: could not find DCFEBs. Please check connections." << endl << endl;
            out << out_local.str();
            UpdateLog(vme_wrapper_, slot, out_local);
            return;
          }
        }
      } // loop over DCFEBs
      // output results
      unsigned int nFailedDCFEBs(0);
      for (unsigned short int dcfeb(0x0); dcfeb<7; dcfeb++) { // count how many DCFEBs failed
        if (!dcfeb_isConnected[dcfeb]) continue;
        if (dcfeb_nGoodCRCs[dcfeb]!=dcfeb_nRxPckt[dcfeb]||dcfeb_nRxPckt[dcfeb]!=repeatNumber) nFailedDCFEBs++;
        if (dcfeb_L1A_cnt[dcfeb]!=repeatNumber) nFailedDCFEBs++;
      }
      // Pass or fail?
      if (nFailedDCFEBs>0) out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      else out_local << "\t\t\t\t\t\tPASSED" << endl;
      for (unsigned short int dcfeb(0x0); dcfeb<7; dcfeb++) {
        if (!dcfeb_isConnected[dcfeb]) continue;
        out_local << "DCFEB " << dcfeb+1 << ": ";
        out_local << "Received " << dcfeb_nRxPckt[dcfeb] << "/" << repeatNumber << " packets, "
                  << dcfeb_nGoodCRCs[dcfeb] << "/" << repeatNumber << " CRCs, "<< dcfeb_L1A_cnt[dcfeb] << " L1A. ";
        if (dcfeb_L1A_cnt[dcfeb]==n_l1a_match_before && n_l1a_match_after==0) out_local << "RESYNC worked.";
        else out_local << "RESYNC failed.";
        out_local << endl;
      }
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }
    
    CCBReg::CCBReg(Crate* crate, Manager* manager) :
      RepeatTextBoxAction(crate, manager, "CCB Registers Test",100){ 
    }
    
    void CCBReg::respond(xgi::Input* in, ostringstream& out,
                         const string& textBoxContent_in) { // JB-F
      srand(time(NULL));
      ostringstream out_local;
      string hdr("********** CCB registers tests **********");
      JustifyHdr(hdr);
      out_local << hdr;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      istringstream countertext(textBoxContent);
      string line;
      getline(countertext,line,'\n');
      const unsigned long repeatNumber=strtoul(line.c_str(),NULL,0);

      unsigned int ccb_slot(13), odmb_slot(Manager::getSlotNumber());
      unsigned int addr_odmb_ctrl_reg(0x003000);
      unsigned int addr_ccb_other(0x0035CC);
      unsigned int addr_ccb_cs_dl(0x000000);
      unsigned int addr_ccb_ctr_reg(0x000020);
      vector<unsigned int> addr_pulses(0);
      addr_pulses.push_back(0x000054);
      addr_pulses.push_back(0x000050);
      addr_pulses.push_back(0x000052);
      addr_pulses.push_back(0x000022);
      addr_pulses.push_back(0x000024);
      addr_pulses.push_back(0x00008e);
      addr_pulses.push_back(0x00008c);
      addr_pulses.push_back(0x00008a);
      vector<unsigned short> args(0);
      args.push_back(0x0);
      args.push_back(0x0);
      args.push_back(0x0);
      args.push_back(0x7);
      args.push_back(0x1);
      args.push_back(0x0);
      args.push_back(0x0);
      args.push_back(0x0);
      vector<unsigned int> other_bits(0);
      other_bits.push_back(0x010);
      other_bits.push_back(0x020);
      other_bits.push_back(0x080);
      other_bits.push_back(0x0C6);
      other_bits.push_back(0x001);
      other_bits.push_back(0x410);
      other_bits.push_back(0x210);
      other_bits.push_back(0x110);
      //vector<int> success (other_bits.size(),0);
      vector<string> signalNames(0); // for output
      signalNames.push_back("l1acc");
      signalNames.push_back("bx0");
      signalNames.push_back("l1arst");
      signalNames.push_back("bxrst,evcntres,cmd_s");
      signalNames.push_back("data_s");
      signalNames.push_back("cal(2)");
      signalNames.push_back("cal(1)");
      signalNames.push_back("cal(0)");

      unsigned short int data(0x300);
      unsigned short int VMEresult, VMEresult_prev;
      // Reset counters
      vme_wrapper_->VMEWrite(addr_odmb_ctrl_reg,data,odmb_slot,"Reset counters, select internal L1A/LCT");
      usleep(10000);
      data=0;
      vme_wrapper_->VMEWrite(addr_ccb_cs_dl,data,ccb_slot);
      data=1;
      usleep(1);
      vme_wrapper_->VMEWrite(addr_ccb_ctr_reg,data,ccb_slot,"Write to CCB command bus");
      usleep(1);
      VMEresult = vme_wrapper_->VMERead(addr_ccb_other,odmb_slot,"Read CCB *other* signals from ODMB");
      usleep(1);
      vector<unsigned int> nBAADs_other(other_bits.size(),0);
      unsigned int nBAADs_other_reg(0), nBitFlips_other_reg(0);
      unsigned int nBAADs_cmd(0), nBAADs_data(0);
      vector<unsigned int> nBitFlips_other(other_bits.size(),0);
      unsigned int nBitFlips_cmd(0), nBitFlips_data(0);
      for (unsigned int n(0);n<repeatNumber;n++) {
        bool BAAD_read(false);
        for (unsigned int p(0);p<addr_pulses.size();++p) {
          BAAD_read = false;
          vme_wrapper_->VMEWrite(addr_pulses.at(p),args.at(p),ccb_slot,"Send address pulse to CCB");
          usleep(1);
          unsigned short int temp = vme_wrapper_->VMERead(addr_ccb_other,odmb_slot,"Read CCB *other* signals from ODMB");
          VMEresult_prev = VMEresult;
          VMEresult = temp;
          if (VMEresult>0x7FF) BAAD_read = true;
          if (BAAD_read) {
            nBAADs_other[p]++;
            nBAADs_other_reg++;
          }
          else {
            unsigned int bit_flips = GetBitFlips((VMEresult^VMEresult_prev)&(0xFF7),(other_bits.at(p) & 0xffff));
            nBitFlips_other[p]+=bit_flips;
            nBitFlips_other_reg+=bit_flips;
          }
        }
        
        BAAD_read = false; // reset for data
        unsigned short to_data(0);
        vme_wrapper_->VMEWrite(0x000024,to_data,ccb_slot,"Write to CCB data bus");
        usleep(1);
        unsigned int data_result_before = vme_wrapper_->VMERead(0x35BC,odmb_slot,"Read CCB data register from ODMB");
        if (data_result_before>0xFF) BAAD_read=true;
        to_data=0xFF;
        vme_wrapper_->VMEWrite(0x000024,to_data,ccb_slot,"Write to CCB data bus");
        usleep(1);
        unsigned int data_result_after = vme_wrapper_->VMERead(0x35BC,odmb_slot,"Read CCB data register from ODMB");
        if (data_result_after>0xFF) BAAD_read=true;
        if (BAAD_read) nBAADs_data++;
        else { // only count flipped bits if we don't have a BAAD
          nBitFlips_data+=GetBitFlips(data_result_before,0x00FF);
          nBitFlips_data+=GetBitFlips(data_result_after,0x0000);
        }
        BAAD_read = false; // reset to check cmd
        //if(data_result_before==0x00FF && data_result_after==0x0000) ++data_success;
        to_data=0;
        vme_wrapper_->VMEWrite(0x000022,to_data,ccb_slot,"Write to CCB command bus");
        usleep(1);
        unsigned int cmd_result_before = vme_wrapper_->VMERead(0x35AC,odmb_slot,"Read CCB command register from ODMB");
        if (cmd_result_before>0xFF) BAAD_read=true;
        to_data=0xFC;
        vme_wrapper_->VMEWrite(0x000022,to_data,ccb_slot,"Write to CCB command bus");
        usleep(1);
        unsigned int cmd_result_after = vme_wrapper_->VMERead(0x35AC,odmb_slot,"Read CCB command register from ODMB");
        if (cmd_result_after>0xFF) BAAD_read=true;
        if (BAAD_read) nBAADs_cmd++;
        else { // only count flipped bits if we don't have a BAAD
          nBitFlips_cmd+=GetBitFlips(cmd_result_before,0x00FF);
          nBitFlips_cmd+=GetBitFlips(cmd_result_after,0x0003);
        }
      }
      if ((nBAADs_other_reg+nBitFlips_other_reg+nBAADs_data+nBitFlips_data+nBAADs_cmd+nBitFlips_cmd)==0) {
        out_local << "\t\t\t\t\t\tPASSED" << endl;
        out_local << "Repeated test " << repeatNumber << " times. ";
        out_local << "No BAAD reads or bit flips in 10 signals and registers." << endl;
      }
      else {
        out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
        out_local << "Repeated test " << repeatNumber << " times." <<endl;
        for (unsigned int p(0);p<addr_pulses.size();p++) {
          out_local << setfill(' ');
          out_local << "CCB Signal / BAAD Reads / Bit Flips: " << setw(32) << signalNames[p] << " / "
                    << nBAADs_other[p] << " / " << nBitFlips_other[p] << endl;  
        }
        out_local << "CCB Data Register: BAAD Reads / Bit Flips: " 
                  << dec << setw(16) << nBAADs_data << " / " << nBitFlips_data << endl;
        out_local << "CCB Command Register: BAAD Reads / Bit Flips: " 
                  << dec << setw(16) << nBAADs_cmd << " / " << nBitFlips_cmd << endl;
      }
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, odmb_slot, out_local);
    }

        
    OTMBPRBSTest::OTMBPRBSTest(Crate* crate, Manager* manager) :
      RepeatTextBoxAction(crate, manager, "OTMB PRBS Test",100){ 
    }
    
    void OTMBPRBSTest::respond(xgi::Input* in, ostringstream& out,
                               const string& textBoxContent_in) { // JB-F
   
      ostringstream out_local;
      string hdr("********** OTMB PRBS Test **********");
      JustifyHdr(hdr);
      out_local << hdr;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);

      unsigned int odmb_slot(Manager::getSlotNumber()), otmb_slot(4);
      unsigned int addr_otmb_cnt_rst(0x9410);
      unsigned int addr_otmb_mode(0x1EE), addr_otmb_prbs_start(0x31EE);
      unsigned int /*addr_otmb_prbs_en(0x9400),*/ addr_read_prbs_matches(0x9408), addr_read_prbs_errors(0x940C);
      unsigned short int reset_command(0x0), odmb_mode(0x1);

      vme_wrapper_->VMEWrite(addr_otmb_cnt_rst,reset_command,odmb_slot,"Reset OTMB PRBS count");
      vme_wrapper_->VMEWrite(addr_otmb_mode,odmb_mode,otmb_slot,"Set OTMB in ODMB mode");
      vme_wrapper_->VMEWrite(addr_otmb_mode,odmb_mode,otmb_slot,"Set OTMB in ODMB mode");
      
      unsigned int n_prbs_sequences = atoi(textBoxContent.c_str());
      vme_wrapper_->VMEWrite(addr_otmb_prbs_start,(unsigned short int)n_prbs_sequences,otmb_slot,"Start OTMB PRBS sequence");
      usleep(n_prbs_sequences*500); // 50 ns sleep per bit
      // Read matches and errors
      //      vme_wrapper_->VMEWrite(addr_otmb_prbs_en,(unsigned short int)n_prbs_sequences,odmb_slot,"Enable PRBS readout on ODMB");
      unsigned int num_matches = vme_wrapper_->VMERead(addr_read_prbs_matches,odmb_slot,"Read number of PRBS matches");
      unsigned int num_errors = vme_wrapper_->VMERead(addr_read_prbs_errors,odmb_slot,"Read number of PRBS errors");
      if (num_errors==100&&num_matches==(n_prbs_sequences-1)) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      out_local << "Number of PRBS sequences: " << n_prbs_sequences << " (10,000 bits each)" << endl;
      out_local << "PRBS sequences matched: " << num_matches << " (expected " << n_prbs_sequences-1 << ")" << endl;
      out_local << "PRBS bit errors: " << num_errors << " (expected 100)" << endl;
      out_local << endl;

      out << out_local.str();

      UpdateLog(vme_wrapper_, odmb_slot, out_local);
    }

    DiscreteLogicTest::DiscreteLogicTest(Crate* crate, Manager* manager) :
      RepeatTextBoxAction(crate, manager, "Discrete Logic / VME JTAG Test",1000){ 
    }
    
    void DiscreteLogicTest::respond(xgi::Input* in, ostringstream& out,
                                    const string& textBoxContent_in) { // JB-F
      ostringstream out_local;
      string hdr("********** Discrete Logic Test **********");
      JustifyHdr(hdr);
      out_local << hdr;
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      unsigned int slot(Manager::getSlotNumber());
      unsigned int addr_vme(0x00FFFC);
      unsigned short int VMEresult;
      vector<string> v_UserCode;
      unsigned int UserCode(0);

      int nReps = atoi(textBoxContent.c_str());
      int nFails(0);
      for (int rep(0); rep<nReps; rep++) { // nReps
        string s_UserCode("");
        for (int command(0); command<5; command++) { // reset JTAG   
          vme_wrapper_->VMEWrite(addr_vme,0x1,slot,"Reset JTAG to Test-Logic-Reset state");
        }
        vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"To Run_Test/Idle");
          
        vme_wrapper_->VMEWrite(addr_vme,0x1,slot,"To Select-DR-Scan");
        vme_wrapper_->VMEWrite(addr_vme,0x1,slot,"To Select-IR-Scan");
          
        for (int command(0); command<5; command++) { // capture/shift IR   
          vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Capture/shift IR (read UserCode 3C8)");
        }
        vme_wrapper_->VMEWrite(addr_vme,0x2,slot,"Shift IR (read UserCode 3C8)");
        vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Shift IR (read UserCode 3C8)");
        vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Shift IR (read UserCode 3C8)");
      
        for (int command(0); command<3; command++) { // capture/shift IR   
          vme_wrapper_->VMEWrite(addr_vme,0x2,slot,"Shift IR (read UserCode 3C8)");
        }
        vme_wrapper_->VMEWrite(addr_vme,0x3,slot,"Shift IR -- and to Exit1-IR");
          
        vme_wrapper_->VMEWrite(addr_vme,0x1,slot,"To Update-IR");
        vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"To Run_Test/Idle");
        vme_wrapper_->VMEWrite(addr_vme,0x1,slot,"To Select-DR-Scan");
        vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"To Capture-DR");
      
        for (int hex_digit(0); hex_digit<8; hex_digit++) { // loop over hex digits
          for (int command(0); command<8; command++) { // loop over JTAG commands
            if (hex_digit==0) { // 1st hex digit
              if (command%2>0) {
                VMEresult = vme_wrapper_->VMERead(addr_vme,slot,"Read UserCode 0 (hex digit 1)");
                if (VMEresult>1) {
                  out_local << "Error: read something greater than 1" << endl << endl ;
                  out << out_local.str();
                  UpdateLog(vme_wrapper_, slot, out_local);
                  return;
                }
                char s_result[4];
                sprintf (s_result,"%d",VMEresult);
                s_UserCode.insert(0,s_result);
                //cout << s_UserCode << endl;
                //v_UserCode.push_back(s_UserCode);
              } else if (command==0) {
                vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Shifting DR");
              } else {
                vme_wrapper_->VMEWrite(addr_vme,0x2,slot,"Shifting DR");
              }
            } else if (hex_digit==1) { // 2nd hex digit
              if (command==0) vme_wrapper_->VMEWrite(addr_vme,0x2,slot,"Shifting DR");
              else if (command%2>0) {
                VMEresult = vme_wrapper_->VMERead(addr_vme,slot,"Read UserCode 0 (hex digit 2)");
                if (VMEresult>1) {
                  out_local << "Error: read something greater than 1" << endl << endl;
                  out << out_local.str();
                  UpdateLog(vme_wrapper_, slot, out_local);
                  return;
                }
                char s_result[4];
                sprintf (s_result,"%d",VMEresult);
                s_UserCode.insert(0,s_result);
                //cout << s_UserCode << endl; 
                //v_UserCode.push_back(s_UserCode);
              }
              else vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Shifting DR");
            }
            else { // all remaining digits
              char log_out[30];
              sprintf(log_out,"Read UserCode 0 (hex digit %d)",hex_digit+1);
              if (command%2>0) {
                VMEresult = vme_wrapper_->VMERead(addr_vme,slot,log_out);
                if (VMEresult>1) {
                  out_local << "Error: read something greater than 1" << endl << endl;
                  out << out_local.str();
                  UpdateLog(vme_wrapper_, slot, out_local);
                  return;
                }
                char s_result[4];
                sprintf (s_result,"%d",VMEresult);
                s_UserCode.insert(0,s_result);
                //cout << s_UserCode << endl;   
              }
              else vme_wrapper_->VMEWrite(addr_vme,0x0,slot,"Shifting DR");
            }
          } // loop over JTAG commands  
        } // loop over hex digits
        UserCode = binaryStringToUInt(s_UserCode);
        v_UserCode.push_back(s_UserCode);
        if (v_UserCode[rep].substr(16,16)!="1101101111011011") nFails++;
      } // nReps
      if (nFails==0) out_local << "\t\t\t\t\t\tPASSED" << endl;
      else out_local << "\t\t\t\t\t\tNOT PASSED" << endl;
      out_local << "Successfully read UserCode " << nReps-nFails << "/" << nReps 
                << " times (" << FixLength(UserCode,8,true) << ")." << endl;
      out_local << endl;

      out << out_local.str();
      UpdateLog(vme_wrapper_, slot, out_local);
    }
        
    /**************************************************************************
     * ResetRegisters
     *
     * A small class to implement a reset from the ODMB_CTRL bits --TD
     **************************************************************************/
    ResetRegisters::ResetRegisters(Crate * crate) 
      : ButtonAction(crate,"Soft reset (ODMB)") 
    { /* The choices here are really a blank constructor vs duplicating the ExecuteVMEDSL constructor.
         I've tried the former -- TD
      */
    }
    
    void ResetRegisters::respond(xgi::Input * in, ostringstream & out) { // TD
      // ButtonAction::respond(in, out);
      out << "********** VME REGISTER RESET **********" << endl;
      bool debug = false;
      int slot = Manager::getSlotNumber();
      // These are the appropriate R/W addresses for register reset
      unsigned int write_addr = 0x003004;
      unsigned short int reset_command = 0x1;
      unsigned short int VMEresult;
   
      vme_wrapper_->VMEWrite(write_addr,reset_command,slot,"Send reset to ODMB_CTRL register");
      usleep(100);

      out << "W  " << FixLength(write_addr & 0xffff) << "        " << FixLength(1)<<"\tODMB soft reset"  << endl<< endl;      
      
    }

    /**************************************************************************
     * ReprogramDCFEB
     *
     * A small class to reprogram the DCFEBs --TD
     **************************************************************************/
    ReprogramDCFEB::ReprogramDCFEB(Crate * crate) 
      : ButtonAction(crate,"Reprogram DCFEBs") 
    { /* The choices here are really a blank constructor vs duplicating the ExecuteVMEDSL constructor.
         I've tried the former -- TD
      */
    }
    
    void ReprogramDCFEB::respond(xgi::Input * in, ostringstream & out) { // TD
      out << "********** VME REGISTER RESET **********" << endl;
      int slot = Manager::getSlotNumber();
      // These are the appropriate R/W addresses for register reset
      unsigned int write_addr = 0x003010;
      unsigned short int reset_command = 0x1;
      
      vme_wrapper_->VMEWrite(write_addr,reset_command,slot,"Send reset to DCFEB_CTRL register");
      usleep(100);      

      out << "W   3010      0          Reprogram all DCFEBs" << endl;      
      
    }
    

    /**************************************************************************
     * Enable VME debug printouts
     *
     *************************************************************************/

    enableVmeDebugPrintout::enableVmeDebugPrintout(Crate * crate)
      : Action(crate), ActionValue<int>(0) {}

    void enableVmeDebugPrintout::display(xgi::Output * out)
    {
      addButtonWithTextBox(out, 
                           "Enable VME Debug Printouts",
                           "enable",
                           numberToString(value()));
    }
    
    void enableVmeDebugPrintout::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>enableVmeDebugPrintout"<<endl; 
      bool enable = getFormValueInt("enable", in);
      value(int(enable)); // save the value
      crate_->vmeController()->SetPrintVMECommands(enable); // turn on/off debug printouts of VME commands
    }

    /**************************************************************************
     * CommonUtilities_setupDDU_passThrough
     * -- S.Z. Shalhout June 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_setupDDU_passThrough::CommonUtilities_setupDDU_passThrough(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_setupDDU_passThrough::display(xgi::Output * out)
    {
      addButton(out, "SetUp DDU PassThrough","width: 230px; ");
    } 

    void CommonUtilities_setupDDU_passThrough::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
        (*ddu)->writeFlashKillFiber(0x7fff); 
        usleep(20);
        //ccb_->HardReset_crate();
        //usleep(250000);
        (*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
        usleep(10);
        (*ddu)->writeFakeL1( 0x8787 ); // 0x8787: passthrough // 0x0000: normal
        usleep(10);
        //ccb_->l1aReset();
        //usleep(50000);
        //usleep(50000);
        //ccb_->bc0();
      } 
    }


    /**************************************************************************
     * Load MCS via BPI
     *
     * Load MCS to PROM using BPI engine
     *************************************************************************/

    LoadMCSviaBPI::LoadMCSviaBPI(Crate * crate, emu::odmbdev::Manager* manager)
      : MCSTextBoxAction(crate, manager, "load MCS")
    {
      //cout << "Updating the slot number box" << endl; 
    }

    void LoadMCSviaBPI::respond(xgi::Input * in, ostringstream & out)
    {

      cout << "Request to program PROM..." << endl;

      MCSTextBoxAction::respond(in, out);
      std::string filename(textBoxContent);
      if (filename.empty()){
        printf("No MCS file specified.  Exiting.\n");
      }else{
        if (dmbs_.size() == 0){
          printf("No ODMBs found.  Exiting.\n");
        }else{
          int manager_slot = Manager::getSlotNumber();

          for (vector <DAQMB*>::iterator dmb = dmbs_.begin();
               dmb != dmbs_.end();
               ++dmb){
            int hw_version = (*dmb)->GetHardwareVersion();
            if (hw_version != 2) {
              printf("DO NOT PROGAM OLD DMB!!! Skipping.\n");
              continue; // only program ODMB
            }

            int slot = (*dmb)->slot();
            if (slot != manager_slot) {
              cout << "skipping ODMB in slot " << manager_slot << " which is not in requested slot " << slot << endl;
              continue; // only program the ODMB requested
            }
                    
            printf("loading MCS file %s...\n", filename.c_str());
            (*dmb)->odmb_program_eprom_poll(filename.c_str());
          }
        }

      }
    } // end LoadMCSviaBPI::respond    

    MCSBackAndForthBPI::MCSBackAndForthBPI(Crate* crate,
                                           emu::odmbdev::Manager* manager):
      RepeatTextBoxAction(crate, manager, "BPI MCS Test"/*,"1"*/) {
    }
    
    void MCSBackAndForthBPI::respond(xgi::Input* in, ostringstream& out,
                                     const string& textBoxContent_in){
      RepeatTextBoxAction::respond(in, out, textBoxContent_in);
      ostringstream out_local;
      const unsigned long testReps=strtoul(textBoxContent.c_str(),NULL,0);
      if(testReps<=0) return;
      const unsigned int manager_slot(Manager::getSlotNumber());

      string filename1("/data/Dropbox/odmb/VME/V03-03/write_unique_id/VF3-03_write_unique_id.mcs");
      string filename2("/data/Dropbox/odmb/VME/V03-03//V03-03_odmb_ucsb.mcs");
      const unsigned short int fw_ver1 = 0xF303, fw1_lead = fw_ver1/0x100;
      const unsigned short int fw_ver2 = 0x0303, fw2_lead = fw_ver2/0x100;
      int singleLoadFailCount = 0, errorType = 0;
      ostringstream fw1_stream, fw2_stream;
      if(fw_ver1>0xFFF) fw1_stream <<"V"<<hex<<fw1_lead<<"-0"<<hex<<fw_ver1-fw1_lead*0x100;
      else fw1_stream <<"V0"<<hex<<fw1_lead<<"-0"<<hex<<fw_ver1-fw1_lead*0x100;
      fw2_stream <<"V0"<<hex<<fw2_lead<<"-0"<<hex<<fw_ver2-fw2_lead*0x100;

      if (filename1.empty() || filename2.empty()) {
        printf("No MCS file specified.  Exiting.\n");
      } else {
        if (dmbs_.size() == 0){
          printf("No ODMBs found.  Exiting.\n");
        } else {
          for (vector <DAQMB*>::iterator dmb = dmbs_.begin();
               dmb != dmbs_.end();
               ++dmb){
            //      (*dmb)->set_odmb_eprom_debug(true);
            unsigned int slot = (*dmb)->slot();
            if (slot != manager_slot){
              //cout << "skipping ODMB in slot " << slot << " which is not in requested slot " << manager_slot << endl;
              continue; // only program the ODMB requested
            }
          
            for (unsigned int iTest = 0; iTest < testReps; iTest++) {

              // check number of words in read back FIFO, expect to be 0
              unsigned short int addr = 0x6034;
              unsigned short int VMEresult = vme_wrapper_->VMERead(addr,slot,"number of words in BPI read-back FIFO");        

              // get current firmware version         
              std::ostringstream convert;
              int current_fw = (*dmb)->odmb_firmware_version();
              int new_fw = current_fw;
              convert << hex << new_fw;
              std::string current_fw_name = convert.str();

              if (current_fw == 0xBAAD) {
                cout << "STOP! Check your system and retry." << endl;
                errorType = 1; 
                break;
              }

              cout<<"Programming ODMB on slot "<<slot<<" with MCS file "<<filename1.c_str()<<"...\n";
              while (!(*dmb)->odmb_program_eprom_poll(filename1.c_str())) {
                singleLoadFailCount++;
                if (singleLoadFailCount > 2) {
                  cout << "3 consecutive fails!  bailing!" << endl; 
                  errorType = 2;
                  break;}
              }
              singleLoadFailCount = 0;

              // store number of words in BPI read-back FIFO before hard reset
              VMEresult = vme_wrapper_->VMERead(addr,slot,"number of words in BPI read-back FIFO");           

              // Issue the hard reset (and remember to add the sleep!)        
              if(ccb_->GetCCBmode() != CCB::VMEFPGA) ccb_->setCCBMode(CCB::VMEFPGA); // we want the CCB in this mode for out test stand
              ccb_->HardReset_crate();
              usleep(3000000);

              // Check the fw version against ver1
              new_fw = (*dmb)->odmb_firmware_version();
              if (new_fw == fw_ver1) { // YAY!
                cout<<"Success! FW version "<<current_fw_name<<" programmed PROM with "<< fw_ver1<<endl;
                counter[current_fw_name]++;
                convert << hex << new_fw;
                current_fw_name = convert.str();

                //              printf("\n");
                //              for (map<string, unsigned int>::iterator iter = counter.begin(); iter != counter.end(); iter++){
                //                cout << "counter first = " << iter->first << endl;
                //                cout << "counter second = " << iter->second << endl;
                //                printf("\tFW version %s successfully programmed PROM %d times\n", iter->first.c_str(), iter->second);
                //              }
                //              printf("Found %d words in BPI read-back FIFO.\n", VMEresult);
                //              printf("\n");
              } else { // BOO!
                cout << "FW version " << current_fw_name << " failed to program PROM with "<< fw_ver1<< endl;
                errorType = 3;
                //              for (std::map<std::string, unsigned int>::iterator iter = counter.begin(); iter != counter.end(); iter++){
                //                out_local << "FW version " <<  iter->first.c_str() << " successfully programmed PROM " << iter->second << " times" << endl;
                //              }
                cout << "Found " << VMEresult << " words in BPI read-back FIFO" << endl;
                break;
              }
                    
              // Now we load the other FW .mcs file
              cout<<"Programming ODMB on slot "<<slot<<" with MCS file "<<filename2.c_str()<<"...\n";
              while (!(*dmb)->odmb_program_eprom_poll(filename2.c_str())) {
                singleLoadFailCount++;
                if (singleLoadFailCount > 2) {
                  cout << "3 consecutive fails!  bailing!" << endl; 
                  errorType = 4;
                  break;}
              }
              singleLoadFailCount = 0;

              // store number of words in BPI read-back FIFO before hard reset
              VMEresult = vme_wrapper_->VMERead(addr,slot,"number of words in BPI read-back FIFO");           

              // Issue the hard reset (and remember to add the sleep!)
              if(ccb_->GetCCBmode() != CCB::VMEFPGA) ccb_->setCCBMode(CCB::VMEFPGA); // we want the CCB in this mode for out test stand
              ccb_->HardReset_crate();
              usleep(3000000);

              // Check the fw version against ver2
              new_fw = (*dmb)->odmb_firmware_version();
              if (new_fw == fw_ver2) { // YAY!
                cout<<"Success! FW version "<<current_fw_name<<" programmed PROM with "<< fw_ver2<<endl;
                counter[current_fw_name]++;
                convert << hex << new_fw;
                current_fw_name = convert.str();

                //              printf("\n");
                //              for (std::map<std::string, unsigned int>::iterator iter = counter.begin(); iter != counter.end(); iter++)
                //                {
                //                  printf("\tFW version %s successfully programmed PROM %d times\n", iter->first.c_str(), iter->second);
                //                }
                //              printf("Found %d words in BPI read-back FIFO.\n", VMEresult);
                //              printf("\n");
              } else { // BOO!
                cout << "FW version " << current_fw_name << " failed to program PROM with "<< fw_ver2<< endl;
                errorType = 5;
                //              for (std::map<std::string, unsigned int>::iterator iter = counter.begin(); iter != counter.end(); iter++){
                //                out_local << "FW version " <<  iter->first.c_str() << " successfully programmed PROM " << iter->second << " times" << endl;
                //              }
                cout << "Found " << VMEresult << " words in BPI read-back FIFO" << endl;
                break;
              }

              //              for (std::map<std::string, unsigned int>::iterator iter = counter.begin(); iter != counter.end(); iter++){
              //                out_local << "FW version " <<  iter->first.c_str() << " successfully programmed PROM " << iter->second << " times" << endl;
              //              }
            } // end test loop
          } // end loop over DMBs
        } 


        out_local << "***** Test PROM Programming via BPI ****\t\t\t\t\t\t";
        if(errorType==0){
          out_local <<"PASSED"<<endl;
          out_local <<"Successfully programmed PROM with "<<fw1_stream.str()<<" and "<<fw2_stream.str()<<endl;
        } else {
          out_local <<"NOT PASSED"<<endl;
          switch(errorType){
          case 1:
            out_local <<"Read BAAD FW. Check your system"<<endl;
            break;
          case 2:
            out_local <<"Gave up trying to load "<<fw1_stream.str()<<endl;
            break;
          case 3:
            out_local <<"Not able to load "<<fw1_stream.str()<<endl;
            break;
          case 4:
            out_local <<"Gave up trying to load "<<fw2_stream.str()<<endl;
            break;
          case 5:
            out_local <<"Not able to load "<<fw2_stream.str()<<endl;
            break;
          }
        }
        out_local <<endl;
        out << out_local.str();
      
      } 
    } // end MCSBackAndForthBPI::respond
  } // namespace odmbdev
} // namespace emu

