#include "emu/me11dev/Buttons.h"
#include "emu/me11dev/utils.h"
#include "emu/me11dev/Manager.h"

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

#include <iomanip>
#include <ctime>

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

namespace emu {
  namespace me11dev {
    
    void HardReset::respond(xgi::Input * in, ostringstream & out) { cout<<"==>HardReset"<<endl; 
      if(ccb_->GetCCBmode() != CCB::VMEFPGA) ccb_->setCCBMode(CCB::VMEFPGA); // we want the CCB in this mode for out test stand
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
      addButton(out, "Read back usercodes","width: 100%; ");
    }

    void ReadBackUserCodes::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ReadBackUserCodes"<<endl; 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
	  vector <CFEB> cfebs = (*dmb)->cfebs();
      
	  for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb)      
	    {
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

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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
      
      for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
        {
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
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
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

    PipelineDepthScan_Pulses::PipelineDepthScan_Pulses(Crate * crate, emu::me11dev::Manager* manager)
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

    PipelineDepthScan_Cosmics::PipelineDepthScan_Cosmics(Crate * crate, emu::me11dev::Manager* manager)
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

    TmbDavDelayScan::TmbDavDelayScan(Crate * crate, emu::me11dev::Manager* manager)
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

    L1aDelayScan::L1aDelayScan(Crate * crate, emu::me11dev::Manager* manager)
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
           ++i)
	{
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
    
    TMBSetRegisters::TMBSetRegisters(Crate * crate, emu::me11dev::Manager* manager)
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



	  if(AFEB_STANDBY==1)
	    {

	      for (int lct_chip = 0; lct_chip < alct_->MaximumUserIndex() / 6; lct_chip++)
		{
		  //int astandby = standby_fmask[iLayerPair];
		  for (int afeb = 0; afeb < 6; afeb++)
		    {
		      //	alct_->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
		
		    }
		}
	      //		alct_->WriteStandbyRegister_();
	      ::sleep(10);
	    }
	  //ccb_->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	  ccb_->RedirectOutput( &cout ); // ccb prints a line on each test pulse - waste it

		      
	  for ( uint64_t iPulse = 1; iPulse <= events_per_layer; ++iPulse )
	    {

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

    /**************************************************************************
     * ExecuteVMEDSL
     *
     * A domain-specific-lanaguage for issuing vme commands. 
     *************************************************************************/

    ExecuteVMEDSL::ExecuteVMEDSL(Crate * crate)
      : Action(crate),
	ActionValue<string>("/home/cscme11/vme.commands") {}
    
    void ExecuteVMEDSL::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Execute VME DSL commands in file:",
			   "VMEProgramFile",
			   value(),
			   "min-width: 25em; width: 25%; ",
			   "min-width: 40em; width: 70%; ");
    }

    void ExecuteVMEDSL::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>ExecuteVMEDSL"<<endl; 
      int slot = 15; // hard code a default VME slot number
      string programfile = getFormValueString("VMEProgramFile", in);
      value(programfile); // save the value

      // the arguments for vme_controller //
      char rcv[2];
      unsigned int addr;
      unsigned short int data;
      int irdwr;
      //// From VMEController.cc:
      // irdwr:   
      // 0 bufread
      // 1 bufwrite 
      // 2 bufread snd  
      // 3 bufwrite snd 
      // 4 flush to VME (disabled)
      // 5 loop back (disabled)
      // 6 delay
      
      // Read in commands from specified file
      stringstream alltext;
      ifstream file( programfile.c_str() );
      if ( file ){
	alltext << file.rdbuf();
	file.close();
      }

      //stringstream alltext(this->textBoxContents); // get all the text
      string line;

      while (getline(alltext,line,'\n')) // read in one line at a time
	{
	  if(!line.size()) continue; // Next line, please.

	  istringstream iss(line);

	  int num=0;
	  if(iss >> num && iss.good()) // the first token is a number, which we interpret as:
	    {
	      //
	      // 0 : end of run, stop execution
	      //
	      // 1 : The rest of the line must contain exactly 64
	      // characters, which are interpreted as a 32 bit address and
	      // 32 bits of data (to match simulation files).  Spaces are
	      // ignored and characters that are not 0 are interpreted as
	      // 1.  Only the lowest 19 bits of address and 16 bits of
	      // data are used with the exception that the 26th bit (from
	      // right) of address determines read(1) or write(0).
	      //
	      // >1 : Produces a sleep for that number of microseconds
	      //
	      // <0 : Send an error message to the output and abort further execution.

	      if(num==0){
		out<<"Found EOR, exiting."<<endl;
		return; // EOR instruction
	      }
	      else if(num==1) // The line begins with 1, so now expect 32 bits for address and 32 bits for data
		{
		  string addr_str;
		  string data_str;
		  string tmp_str;

		  while(addr_str.size()<32 && iss.good()) // read in 32 bits for address
		    {
		      iss >> tmp_str;
		      addr_str += tmp_str;
		      //out<<"addr_str:"<<addr_str<<endl;
		    }
		  while(data_str.size()<32 && iss.good()) // read in 32 bits for data
		    {
		      iss >> tmp_str;
		      data_str += tmp_str;
		      //out<<"data_str:"<<data_str<<endl;
		    }

		  if(addr_str.size()!=32 || data_str.size()!=32)
		    {
		      out<<"ERROR: address("<<addr_str<<") or data("<<data_str<<") is not 32 bits on line: "<<line<<endl;
		      return;
		    }

		  irdwr = (addr_str.at(addr_str.size()-26)=='1')? 2 : 3; // 26th and 25th "bits" from right tell read (10) or write (01)
		  addr = binaryStringToUInt(addr_str);
		  data = binaryStringToUInt(data_str);
		}
	      else if(num > 1)
		{
		  out<<"sleep for "<<num<<" microseconds"<<endl;
		  out<<flush;
		  usleep(num);
		  continue; // Next line, please.
		}
	      else
		{ // This shouldn't happen
		  out<<"ERROR: line begins with unexpected number = "<<num<<".  Aborting further execution."<<endl;
		  return;
		}
      
	    }
	  else
	    {
	      // Parse the line as follows:
	      //
	      // readfile <int> :
	      // Readfile <int> :
	      // READFILE <int> :
	      //  Read in VME commands from this file.
	      //	  //
	      // slot <int> :
	      // Slot <int> :
	      // SLOT <int> :
	      //  Change the slot number to <int>.
	      //
	      // w <hex address> <hex data> :
	      // W <hex address> <hex data> :
	      //  A write command using the lowest 19/16 bits of give address/data.
	      //
	      // r <hex address> <hex data> :
	      // R <hex address> <hex data> :
	      //  A read command using the lowest 19/16 bits of give address/data.
	      //
	      // Anything else is treated as a comment.

	      iss.clear(); // reset iss
	      iss.str(line); // not needed, but just to make it clear
	      string key;
	      iss >> key;

	      if( key == "readfile" ||
		  key == "Readfile" ||
		  key == "READFILE" )
		{ //
		  string tmp_filename;
		  if( iss >> tmp_filename )
		    {
		      programfile = tmp_filename;
		      out << "Read commands from file: " << programfile << endl;
		      continue; // Next line, please.
		    }
		  else
		    {
		      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }
		}
	      else if( key == "slot" ||
		       key == "Slot" ||
		       key == "SLOT" ) // Try to change the slot number
		{
		  int tmp_slot;
		  if( iss >> tmp_slot )
		    {
		      slot = tmp_slot;
		      out << "Slot number set to " << slot << endl;
		      continue; // Next line, please.
		    }
		  else
		    {
		      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }
		}
	      else if( key == "w" ||
		       key == "W" ||
		       key == "r" ||
		       key == "R" )
		{
		  if( key == "w" || key == "W" ) irdwr = 2;
		  else irdwr = 3;

		  if( !(iss >> hex >> addr) || !(iss >> hex >> data) )
		    { // put hex string directly into addr/data
		      out<<"ERROR: problem reading hex values for address or data on the line: "<<line<<endl;
		      out<<"  Aborting further execution."<<endl;
		      return;
		    }

		}
	      else
		{
		  out<<"COMMENT: "<<line<<endl;
		  continue; // Next line, please.
		}
	    }

	  //// If we make it here, we have a VME command to run! ////

	  // set the top bits of address to the slot number
	  addr = (addr&0x07ffff) | slot<<19;

	  printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x})  ",irdwr,addr&0xffffff,data&0xffff,rcv[0]&0xff,rcv[1]&0xff);
	  crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); // Send the VME command!
	  VMEController::print_decoded_vme_address(addr,&data);
	  usleep(1);

	  // if it was a read, then show the result
	  if(irdwr==2) printf("  ==> rcv[1,0] = %02x %02x",rcv[1]&0xff,rcv[0]&0xff);
	  printf("\n");
	  fflush(stdout);

	} // while parsing lines
    }


    /**************************************************************************
     * Buck Shift Test
     *
     *************************************************************************/

    BuckShiftTest::BuckShiftTest(Crate * crate)
      : Action(crate) {}

    void BuckShiftTest::display(xgi::Output * out)
    {
      addButton(out, "Buck Shift Test");
    }

    void BuckShiftTest::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>BuckShiftTest"<<endl; 
      out << "=== Buck Shift Test ===" << endl;

      //crate_->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	int val = (*dmb)->buck_shift_test();
	cout<<"Buck Shift Test returns: "<<val<<endl;
      }

      //crate_->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands
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
     * DCFEB debug dump
     *
     *************************************************************************/

    dcfebDebugDump::dcfebDebugDump(Crate * crate)
      : Action(crate) {}

    void dcfebDebugDump::display(xgi::Output * out)
    {
      addButton(out, "DCFEB Debug Dump");
    }

    void dcfebDebugDump::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>dcfebDebugDump"<<endl; 
      out << "DCFEB Debug Dump send to stdout" << endl;

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb)
	{
	  vector <CFEB> cfebs = (*dmb)->cfebs();
	  for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb)	
	    {
	      int cfeb_index = (*cfeb).number();
	      
	      cout << " ********************* " << endl
		   << " FEB" << cfeb_index << " : " << endl;
	      (*dmb)->dcfeb_readreg_statusvirtex6( *cfeb );
	      (*dmb)->dcfeb_readreg_cor0virtex6( *cfeb );
	      (*dmb)->dcfeb_readreg_cor1virtex6( *cfeb ); 
	    }
	}
    }

    /**************************************************************************
     * CommonUtilities_setupDDU
     * -- S.Z. Shalhout April 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_setupDDU::CommonUtilities_setupDDU(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_setupDDU::display(xgi::Output * out)
    {
      addButton(out, "SetUp DDU ","width: 100%; ");
    } 

    void CommonUtilities_setupDDU::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu)
	{
	
	  (*ddu)->writeFlashKillFiber(0x7fff); 
	  usleep(20);
	  ccb_->HardReset_crate();
	  usleep(250000);
	  (*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
	  usleep(10);
	  (*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
	  usleep(10);
	  ccb_->l1aReset();
	  usleep(50000);
	  usleep(50000);
	  ccb_->bc0();


	} 

    }


  
    /**************************************************************************
     * CommonUtilities_restoreCFEBIdle
     * -- S.Z. Shalhout April 26, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    CommonUtilities_restoreCFEBIdle::CommonUtilities_restoreCFEBIdle(Crate * crate)
      : Action(crate) {}
 
    void CommonUtilities_restoreCFEBIdle::display(xgi::Output * out)
    {
      addButton(out, "Restore CFEBS to IDLE","width: 100%; ");
    } 

    void CommonUtilities_restoreCFEBIdle::respond(xgi::Input * in, ostringstream & out)
    {

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }

    }


     /**************************************************************************
     * RoutineTest_ShortCosmicsRun
     * -- S.Z. Shalhout April 4, 2013 (sshalhou@cern.ch)
     *************************************************************************/

    RoutineTest_ShortCosmicsRun::RoutineTest_ShortCosmicsRun(Crate * crate, emu::me11dev::Manager* manager)
      : Action( crate, manager ),
	ActionValue<int>(10) {}
    
    void RoutineTest_ShortCosmicsRun::display(xgi::Output * out)
    {
      addButtonWithTextBox(out,
			   "Routine Test - Short Cosmics Run (HV should be ON)",
			   "time",
                           numberToString(value()));
    }
    
    void RoutineTest_ShortCosmicsRun::respond(xgi::Input * in, ostringstream & out)
    {
      
      // Assuming that you have HV on, and have already written FLASH to crate with
      // the needed xml parameters ...
      
      // equivalent to EmuPeripheralCrateConfig::PrepareForTriggering
      
      int _time = getFormValueInt("time", in);      
      
      ccb_->setCCBMode(CCB::VMEFPGA);
      ccb_->hardReset();
      ccb_->EnableL1aFromTmbL1aReq();
      usleep(1000);

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
	(*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
      	usleep(10);
	(*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
      	usleep(10);
      }

      // set comparator threshold 
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	(*dmb)->set_comp_thresh(0.03);
      }
      usleep(1000);
      
      
      //  set the pipeline depth
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	vector <CFEB> cfebs = (*dmb)->cfebs();
	for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
	  int depth = cfeb->GetPipelineDepth(); // get value that was read in from the crate config xml (unless it was changed later)
	  (*dmb)->dcfeb_set_PipelineDepth(*cfeb, depth);
	  usleep(100);
	  (*dmb)->Pipeline_Restart( *cfeb );
	  usleep(100);
	}
      }
      ccb_->l1aReset();
      usleep(1000);
     
      // shift buckeyes into normal mode
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	(*dmb)->shift_all(NORM_RUN);
	(*dmb)->buck_shift();
	usleep(100);
      }
      //Indara Needs for ALCT-only cosmic run//
     /*   tmb_->WriteRegister(0x4a,0);
        tmb_->WriteRegister(0x4c,0);
        tmb_->WriteRegister(0x4e,0);
        tmb_->WriteRegister(0x50,0);
        tmb_->WriteRegister(0x52,0);
        tmb_->WriteRegister(0x54,0);
        tmb_->WriteRegister(0x56,0);
        tmb_->WriteRegister(0x58,0);
        tmb_->WriteRegister(0x5a,0);
        tmb_->WriteRegister(0x5c,0);
        tmb_->WriteRegister(0x5e,0);
        tmb_->WriteRegister(0x60,0);
        tmb_->WriteRegister(0x62,0);
        tmb_->WriteRegister(0x64,0);
        tmb_->WriteRegister(0x66,0);
        tmb_->WriteRegister(0x16e,0);
        tmb_->WriteRegister(0x170,0);
        tmb_->WriteRegister(0x172,0);
        tmb_->WriteRegister(0x174,0);
        tmb_->WriteRegister(0x176,0);
        tmb_->WriteRegister(0x178,0);      */
  
      ccb_->l1aReset();
      usleep(1000);
      ccb_->bc0();
      usleep(1000);
      
      // start DAQ
      cout<<"starting DAQ..."<<endl;
      manager_->startDAQ( string("ME11Test_ShortCosmicsRun")+emu::utils::stringFrom<int>( _time )+string("s") );
      
      ::sleep(_time); // take data
      
      // stop DAQ 
      cout<<"stopping DAQ..."<<endl;
      manager_->stopDAQ();
      cout<<" Done "<<endl;
    }
    
    
    /**************************************************************************
     * RoutineTest_PrecisionPulses
     *
     *************************************************************************/
  
    RoutineTest_PrecisionPulses::RoutineTest_PrecisionPulses(Crate * crate, emu::me11dev::Manager* manager)
      : Action( crate, manager ) {}
    void RoutineTest_PrecisionPulses::display(xgi::Output * out)
    {
      addButton(out,"Routine Test - Precision Pulses");
    }
    
    void RoutineTest_PrecisionPulses::respond(xgi::Input * in, ostringstream & out)
    {
      cout<<"==>RoutineTest_PrecisionPulses"<<endl; 

      const int N_DCFEBS = 5; // number of DCFEBs per DMB
      const int N_STRIPS = 16; // number of strips per DCFEB
      int n_pulses = 2; // this must be >1 because there is a "feature" that the DAQ doesn't record the first pulse after an l1aReset (except for the very first event)

      // set register 0 appropriately for communication over the VME backplane.
      ccb_->setCCBMode(CCB::VMEFPGA);
      ccb_->hardReset();
      usleep(250000);
      // Enable L1A and clct_pretrig from any of dmb_cfeb_calib signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      ccb_->SetExtTrigDelay( 19 ); // Delay of ALCT and CLCT external triggers before distribution to backplane


      usleep(1000);

      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
	(*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
      	usleep(10);
	(*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
      	usleep(100);
      }
      

      //// Set pulse height ////
      float PulseHeight = 0.5;
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	(*dmb)->set_dac(PulseHeight, PulseHeight);
      }
      usleep(1000);
      
      //// Set comparator thresholds ////
      float ComparatorThresholds = 0.01;
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	(*dmb)->set_comp_thresh(ComparatorThresholds);
      }
      usleep(1000);
  
      //// Set the pipeline depth on all DCFEBs ////
      for(vector<DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	vector<CFEB> cfebs = (*dmb)->cfebs();
        out<<cfebs.size()<<endl; 
	for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
	  
	  int depth = cfeb->GetPipelineDepth(); // get value that was read in from the crate config xml (unless it was changed later)
	  //int depth = 44; // get value that was read in from the crate config xml (unless it was changed later)
	  (*dmb)->dcfeb_set_PipelineDepth( *cfeb, depth ); // set it on the hardware
	  usleep(1000);
	  (*dmb)->Pipeline_Restart( *cfeb ); // must restart pipeline after setting it
	  usleep(1000);
	}
      }
      //szs ccb_->l1aReset(); // needed after setting/restarting pipeline
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }
      usleep(10000);
      
      //// Still need settings to time in TMB data with L1a from pulse
      // I think this means tuning CCB l1aDelay and CCB external trigger delay, but this can come later
      

      // start DAQ
      cout<<"starting DAQ..."<<endl;
      manager_->startDAQ( string("ME11Test_PrecisionPulses") );
      
      //// Pulse individual DCFEBs ////
      int strip_to_pulse = 0;
      int feb_to_pulse = -1;


      for(int dcfeb_to_pulse=0; dcfeb_to_pulse < N_DCFEBS; ++dcfeb_to_pulse){
	strip_to_pulse = 1 + 13*dcfeb_to_pulse/(N_DCFEBS-1); // just a cute way to move the pulse along with the DCFEB
	
	//szs	ccb_->l1aReset(); // stop triggering
	for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }
	//ccb_->stopTrigger();
      	usleep(10000);
	
	for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	  (*dmb)->set_ext_chanx(strip_to_pulse, dcfeb_to_pulse); // this only sets the array in software
	  (*dmb)->buck_shift(); // this shifts the array into the buckeyes
	  usleep(10000);
	}
	//// We are now configured to send pulses
	
	//szs	ccb_->l1aReset();
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }
	//ccb_->stopTrigger();
        usleep(10000);
	ccb_->bc0(); // start triggering
        usleep(10000);
cout<<"npulses = "<<n_pulses<<endl;
	for(int p=0; p<n_pulses; ++p){
	  cout<<"pulsing dcfeb "<<dcfeb_to_pulse<<", strip "<<strip_to_pulse<<endl;
	  //ccb_->pulse(1,0); // send the pulses
	  ccb_->GenerateDmbCfebCalib0(); // send the pulses 
	  usleep(10000);
	}
      }	
      

      //// Loop over strips, pulsing all DCFEBs ("walking one")////
      feb_to_pulse = -1; // -1 means all DCFEBs
      for(strip_to_pulse=0; strip_to_pulse < N_STRIPS; ++strip_to_pulse){

      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }
	//	ccb_->l1aReset(); // stop triggering
      	usleep(10000);
	
	for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
	  (*dmb)->set_ext_chanx(strip_to_pulse, feb_to_pulse); // this only sets the array in software
	  (*dmb)->buck_shift(); // this shifts the array into the buckeyes
	  usleep(1000);
	}
	//// We are now configured to send pulses

	//	ccb_->l1aReset();
      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){ (*dmb)->restoreCFEBIdle(); }
        usleep(10000);
	ccb_->bc0(); // start triggering
        usleep(10000);

	for(int p=0; p<n_pulses; ++p){
	  cout<<"pulsing dcfeb "<<feb_to_pulse<<", strip "<<strip_to_pulse<<endl;
	  //ccb_->pulse(1,0); // send the pulses
	  ccb_->GenerateDmbCfebCalib0(); // send the pulses 
	  usleep(10000);
	}
      }	
      
      // stop DAQ
      cout<<"stopping DAQ..."<<endl;
      manager_->stopDAQ();
    }
    
    /**************************************************************************
     * RoutineTest_PatternPulses:Counters
     * I. Suarez
     *************************************************************************/
    RoutineTest_PatternPulses_TMBCounters::RoutineTest_PatternPulses_TMBCounters(Crate * crate, emu::me11dev::Manager* manager)
      : Action( crate, manager ) {}
    void RoutineTest_PatternPulses_TMBCounters::display(xgi::Output * out)
    {
      addButton(out,"Routine Test - Pattern Pulses");
    }

    void RoutineTest_PatternPulses_TMBCounters::respond(xgi::Input * in, ostringstream & out)
    {

      // set register 0 appropriately for communication over the VME backplane.
      ccb_->setCCBMode(CCB::VMEFPGA);
      ccb_->hardReset();

      //tmb_->SetRequestL1a(0);  //do not request l1a cause the pulse comes with one            
      
      for(vector <DDU*>::iterator ddu = ddus_.begin(); ddu != ddus_.end();++ddu){
        (*ddu)->writeFakeL1( 0x0000 ); // 0x8787: passthrough // 0x0000: normal
        usleep(10);
        (*ddu)->writeGbEPrescale( 0x7878 ); // 0x7878: test-stand without TCC
        usleep(10);
      }

      // Enable L1A and clct_pretrig from any of dmb_cfeb_calib signals and disable all other trigger sources
      ccb_->EnableL1aFromDmbCfebCalibX();
      ccb_->SetExtTrigDelay( 19 ); // Delay of ALCT and CLCT external triggers before distribution to backplane
      usleep(100);

      //// Set pulse height ////
      float PulseHeight = 25.0;
      float ComparatorThresholds = 2.5;
      int NumberofPulses = 1;
      int timebetweenpulse = 400;
      ccb_->l1aReset(); // needed after setting/restarting pipeline
      usleep(100);
      int DMBposition=0;

     // int halfstrip2 = 10;
     // int hp[6] = {halfstrip2+1, halfstrip2, halfstrip2+1, halfstrip2, halfstrip2+1, halfstrip2};
      int dcfeb_pulsed = 0;
      char dcfebtrig[5] = {0x01,0x02,0x04,0x08,0x10};
      //int position;
      int counter_DCFEB[7]; 
      //int sum_DCFEBCounters=0;
     
      int last_keyhalfstrip_pulsed = 0;
     for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        vector<CFEB> cfebs = (*dmb)->cfebs();
        for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); ++cfeb){
          int depth = 64; // get value that was read in from the crate config xml (unless it was changed later)
          (*dmb)->dcfeb_set_PipelineDepth( *cfeb, depth ); // set it on the hardware
          usleep(100);
          (*dmb)->Pipeline_Restart( *cfeb ); // must restart pipeline after setting it
          usleep(100);
          ccb_->l1aReset(); // needed after setting/restarting pipeline
          usleep(100);
        }
      }
     
      // start DAQ
      cout<<"starting DAQ..."<<endl;
      manager_->startDAQ( string("ME11Test_PatternPulses") );


      for(vector <DAQMB*>::iterator dmb = dmbs_.begin(); dmb != dmbs_.end(); ++dmb){
        (*dmb)->set_dac(PulseHeight, PulseHeight);
        (*dmb)->set_comp_thresh(ComparatorThresholds);
        DMBposition = int(dmb - dmbs_.begin());
        if(DMBposition==0) dcfeb_pulsed=5;
        if(DMBposition==1) dcfeb_pulsed=0;
        vector<CFEB> cfebs = (*dmb)->cfebs();

        for(CFEBItr cfeb = cfebs.begin(); cfeb != cfebs.end(); cfeb++){
          for(int halfstrip = 0; halfstrip<32; ++halfstrip){
          int  hp[6] = {halfstrip+1, halfstrip, halfstrip+1, halfstrip, halfstrip+1, halfstrip};
          (*dmb)->trigsetx(hp, dcfebtrig[cfeb->number()]);
          ccb_->l1aReset(); // needed after setting/restarting pipeline
          usleep(1000);
          ccb_->bc0(); // start triggering
          usleep(1000);

          tmb_->ResetCounters();
          usleep(1000);
          //if(halfstrip==0) 
          out<<dcfeb_pulsed+1<<"\t"<<NumberofPulses<<"\t"<<halfstrip<<"\t";
          ccb_->inject(NumberofPulses,timebetweenpulse); // send the pulses
          usleep(100000);
	  
          last_keyhalfstrip_pulsed = int(((tmb_->ReadRegister(0x8a78) & 0xff00)>>8 ));
          out<<last_keyhalfstrip_pulsed<<"\t";

          tmb_->WriteRegister(0xd0,0x1ce2);
          counter_DCFEB[0] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[0]>0) 
          out<<counter_DCFEB[0]<<"\t";

          tmb_->WriteRegister(0xd0,0x1ee2);
          counter_DCFEB[1] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[1]>0) 
          out<<counter_DCFEB[1]<<"\t";

          tmb_->WriteRegister(0xd0,0x20e2);
          counter_DCFEB[2] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[2]>0) 
          out<<counter_DCFEB[2]<<"\t"; 

          tmb_->WriteRegister(0xd0,0x22e2);
          counter_DCFEB[3] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[3]>0) 
          out<<counter_DCFEB[3]<<"\t";

          tmb_->WriteRegister(0xd0,0x24e2);
          counter_DCFEB[4] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[4]>0) 
          out<<counter_DCFEB[4]<<"\t";

          tmb_->WriteRegister(0xd0,0x26e2);
          counter_DCFEB[5] = int(tmb_->ReadRegister(0xd2));
          //if(counter_DCFEB[5]>0) 
          out<<counter_DCFEB[5]<<"\t";

          tmb_->WriteRegister(0xd0,0x28e2);
          counter_DCFEB[6] = int(tmb_->ReadRegister(0xd2));
	  //if(counter_DCFEB[6]>0)  
          out<<counter_DCFEB[6]<<endl;  

          //if(NumberofPulses != counter_DCFEB[dcfeb_pulsed]) out<<"Houston, we got problems.  DCFEB"<<dcfeb_pulsed+1<<" got "<<counter_DCFEB[dcfeb_pulsed]<<" and expected "<<NumberofPulses<<endl;
          //sum_DCFEBCounters = counter_DCFEB[0]+counter_DCFEB[1]+counter_DCFEB[2]+counter_DCFEB[3]+counter_DCFEB[4]+counter_DCFEB[5]+counter_DCFEB[6];     
          //if((sum_DCFEBCounters - counter_DCFEB[dcfeb_pulsed]) > 0) out<<"More than 1 DCFEB was pulsed"<<endl;

         //shifting everything back to normal
         (*dmb)->shift_all(NORM_RUN);
         (*dmb)->buck_shift();
          usleep(100);
          ccb_->l1aReset();
          }
          dcfeb_pulsed++;
        }
      }

      // stop DAQ
      cout<<"stopping DAQ..."<<endl;
      manager_->stopDAQ();

    }

    /***********************************************
     *  A place to run other buttons in some sequence
     ***********************************************/

    ButtonTests::ButtonTests(Crate * crate, emu::me11dev::Manager* manager)
      : Action( crate, manager ){}
    
    void ButtonTests::display(xgi::Output * out)
    {
      addButton(out, "Button Tests");

      *out << cgicc::input().set("type","hidden").set("name","depth").set("value","67")<< endl;
      *out << cgicc::input().set("type","hidden").set("name","ComparatorThresholds").set("value","0.03")<< endl;
      
    }
    
    void ButtonTests::respond(xgi::Input * in, ostringstream & out)
    {

      int sleeptime=300000;

      // Hard Reset
      HardReset hr(crate_);
      hr.respond(in,out);
      
      usleep(sleeptime);

      // Setup DDU
      CommonUtilities_setupDDU setupDDU(crate_);
      setupDDU.respond(in,out);
      
      usleep(sleeptime);

      //Set pipeine depth: 68
      SetPipelineDepthAllDCFEBs pd(crate_);
      pd.respond(in,out);

      usleep(sleeptime);

      //Restore CFEBs
      CommonUtilities_restoreCFEBIdle restore(crate_);
      //restore.respond(in,out);
      ccb_->l1aReset();
      
      usleep(sleeptime);

      //Set comp thresh: 0.03
      SetComparatorThresholds comp(crate_);
      comp.respond(in,out);
      
      usleep(sleeptime);

      //Shift to normal mode
      ShiftBuckeyesNormRun norm(crate_);
      norm.respond(in,out);

      usleep(sleeptime);

      //Restore CFEBs
      restore.respond(in,out);
      //ccb_->l1aReset();

      usleep(sleeptime);

      //BC0
      ccb_->bc0();

      usleep(sleeptime);

      // start DAQ
      cout<<"starting DAQ..."<<endl;
      manager_->startDAQ( string("ME11Test_ButtonTests") );

      ::sleep(3);
      
      // stop DAQ 
      cout<<"stopping DAQ..."<<endl;
      manager_->stopDAQ();
      cout<<" Done "<<endl;
    }

  } // namespace me11dev
} // namespace emu
