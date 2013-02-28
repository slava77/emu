#include "emu/me11dev/Buttons.h"
#include "emu/me11dev/utils.h"

#include "emu/pc/Crate.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"
#include "emu/pc/TMB_constants.h"

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

namespace emu { namespace me11dev {

    void HardReset::respond(xgi::Input * in, ostringstream & out) { ccb->hardReset(); }

    void L1Reset::respond(xgi::Input * in, ostringstream & out) { ccb->l1aReset(); }

    void BC0::respond(xgi::Input * in, ostringstream & out) { ccb->bc0(); }


    /**************************************************************************
     * Reads back the user codes
     *
     *************************************************************************/

    ReadBackUserCodes::ReadBackUserCodes(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void ReadBackUserCodes::display(xgi::Output * out){
      AddButton(out, "Read back usercodes");
    }

    void ReadBackUserCodes::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin();
	  dmb != dmbs.end();
	  ++dmb) {
	vector <CFEB> cfebs = (*dmb)->cfebs();
	for(CFEBrevItr cfeb = cfebs.rbegin(); cfeb != cfebs.rend(); ++cfeb) {
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
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetComparatorThresholds::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set Comparator Thresholds (volts):",
			   "ComparatorThresholds",
			   "0.03");
    }

    void SetComparatorThresholds::respond(xgi::Input * in, ostringstream & out) {
      float ComparatorThresholds = getFormValueFloat("ComparatorThresholds", in);

      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  (*dmb)->set_comp_thresh(ComparatorThresholds);
	}
    }

    /**************************************************************************
     * SetComparatorThresholdsBroadcast
     *
     *************************************************************************/

    SetComparatorThresholdsBroadcast::SetComparatorThresholdsBroadcast(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetComparatorThresholdsBroadcast::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set Comparator Thresholds-broadcast (volts):",
			   "ComparatorThresholds",
			   "0.03");
    }

    void SetComparatorThresholdsBroadcast::respond(xgi::Input * in, ostringstream & out) {
      float ComparatorThresholds = getFormValueFloat("ComparatorThresholds", in);

      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  (*dmb)->dcfeb_set_comp_thresh_bc(ComparatorThresholds);
	}
    }

    /**************************************************************************
     * SetUpComparatorPulse
     *
     *************************************************************************/

    SetUpComparatorPulse::SetUpComparatorPulse(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetUpComparatorPulse::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set up internal capacitor pulse on halfstrip:",
			   "halfstrip",
			   "16");
    }

    void SetUpComparatorPulse::respond(xgi::Input * in, ostringstream & out) {
      int halfstrip = getFormValueInt("halfstrip", in);

      ccb->hardReset();

      tmb->SetClctPatternTrigEnable(1);
      tmb->WriteRegister(emu::pc::seq_trig_en_adr);

      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  int hp[6] = {halfstrip+1, halfstrip, halfstrip+1, halfstrip, halfstrip+1, halfstrip}; 
	  // Note: +1 for layers 0,2,4 is because ME1/1 doesn't have
	  // staggered strips, but DAQMB codes assumes staggering.
	  int CFEB_mask = 0x7f;

	  (*dmb)->trigsetx(hp,CFEB_mask); // this calls chan2shift, which I think is buggy -Joe
	  //(*dmb)->buck_shift(); // I think trigsetx is supposed to do the shift, too. -Joe
	  usleep(100);
	}

      ccb->syncReset();//check
      usleep(100);
      ccb->bx0();   //check
    }

    /**************************************************************************
     * SetUpPrecisionCapacitors
     *
     *************************************************************************/

    SetUpPrecisionCapacitors::SetUpPrecisionCapacitors(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetUpPrecisionCapacitors::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set up precision capacitor pulse on strip:",
			   "StripToPulse",
			   "8");
    }

    void SetUpPrecisionCapacitors::respond(xgi::Input * in, ostringstream & out) {


      int strip_to_pulse = getFormValueInt("StripToPulse", in);

      ccb->hardReset();

      tmb->SetClctPatternTrigEnable(1);
      tmb->WriteRegister(emu::pc::seq_trig_en_adr);


      crate->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  (*dmb)->set_ext_chanx(strip_to_pulse);//check
	  (*dmb)->buck_shift();
	  usleep(100);
	}
      crate->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands

      ccb->syncReset();//check
      usleep(100);
      ccb->bx0();
    }

    /**************************************************************************
     * PulseInternalCapacitors
     *
     *************************************************************************/

    PulseInternalCapacitors::PulseInternalCapacitors(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulseInternalCapacitors::display(xgi::Output * out) {
      AddButton(out, "Pulse internal capacitors via DMB");
    }

    void PulseInternalCapacitors::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
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
      AddButton(out, "Pulse internal capacitors via CCB");
    }

    void PulseInternalCapacitorsCCB::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  ccb->inject(1,0);
	}
    }

    /**************************************************************************
     * PulsePrecisionCapacitors
     *
     *************************************************************************/

    PulsePrecisionCapacitors::PulsePrecisionCapacitors(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void PulsePrecisionCapacitors::display(xgi::Output * out) {
      AddButton(out, "Pulse precision capacitors via DMB");
    }

    void PulsePrecisionCapacitors::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
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
      AddButton(out, "Pulse precision capacitors via CCB");
    }

    void PulsePrecisionCapacitorsCCB::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  ccb->pulse(1,0);
	}
    }

    /**************************************************************************
     * SetDMBDACs
     *
     *************************************************************************/

    SetDMBDACs::SetDMBDACs(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetDMBDACs::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set DMB DACs 0 and 1 to (volts):",
			   "DAC",
			   "1.0");
    }

    void SetDMBDACs::respond(xgi::Input * in, ostringstream & out) {
      float DAC = getFormValueFloat("DAC", in);
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  (*dmb)->set_dac(DAC,DAC);
	}
    }

    /**************************************************************************
     * ShiftBuckeyesNormRun
     *
     *************************************************************************/

    ShiftBuckeyesNormRun::ShiftBuckeyesNormRun(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void ShiftBuckeyesNormRun::display(xgi::Output * out) {
      AddButton(out, "Shift Buckeyes into normal mode");
    }

    void ShiftBuckeyesNormRun::respond(xgi::Input * in, ostringstream & out) {
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  (*dmb)->shift_all(NORM_RUN);
	}
    }

    /**************************************************************************
     * SetPipelineDepthAllDCFEBs
     *
     *************************************************************************/

    SetPipelineDepthAllDCFEBs::SetPipelineDepthAllDCFEBs(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetPipelineDepthAllDCFEBs::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Set pipeline depth on all DCFEBs:",
			   "depth",
			   "61");
    }

    void SetPipelineDepthAllDCFEBs::respond(xgi::Input * in, ostringstream & out) {
      int depth = getFormValueInt("depth", in);
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  vector <CFEB> cfebs = (*dmb)->cfebs();
	  for(CFEBrevItr cfeb = cfebs.rbegin(); cfeb != cfebs.rend(); ++cfeb)
	    {
	      (*dmb)->dcfeb_set_PipelineDepth(*cfeb, depth);
	    }
	}
    }

    /**************************************************************************
     * SetFineDelayForADCFEB
     *
     *************************************************************************/

    SetFineDelayForADCFEB::SetFineDelayForADCFEB(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void SetFineDelayForADCFEB::display(xgi::Output * out) {
      AddButtonWithTwoTextBoxes(out,
				"Set Fine Delay on FEB(0-4) to (0-15):",
				"DcfebNumber",
				"1",
				"FineDelay",
				"0");
    }

    void SetFineDelayForADCFEB::respond(xgi::Input * in, ostringstream & out) {
      int delay = getFormValueInt("FineDelay", in);
      int cfeb_number = getFormValueInt("DcfebNumber", in);
      for(vector <DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb)
	{
	  vector <CFEB> cfebs = (*dmb)->cfebs();
	  (*dmb)->dcfeb_fine_delay(cfebs.at(cfeb_number), delay);
	  usleep(100);
	  (*dmb)->Pipeline_Restart(cfebs[cfeb_number]);
	}
    }

    /**************************************************************************
     * TMBHardResetTest
     *
     *************************************************************************/

    TMBHardResetTest::TMBHardResetTest(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void TMBHardResetTest::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "TMB Hard Reset Test, number of resets:",
			   "NumberOfHardResets",
			   "100");
    }

    void TMBHardResetTest::respond(xgi::Input * in, ostringstream & out) {
      int NumberOfHardResets = getFormValueInt("NumberOfHardResets", in);
      out << "=== TMB Hard Reset Test ===\n";

      int expected_day = tmb->GetExpectedTmbFirmwareDay();
      int expected_month = tmb->GetExpectedTmbFirmwareMonth();
      int expected_year = tmb->GetExpectedTmbFirmwareYear();
      int expected_type = tmb->GetExpectedTmbFirmwareType();
      int expected_version = tmb->GetExpectedTmbFirmwareVersion();
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
      ccb->RedirectOutput(&waste);

      for (i = 0;
	   i < NumberOfHardResets && !firmware_lost;
	   ++i)
	{
	  if (i % 500 == 0) {
	    out << "Hard Reset Number " << i << endl;
	  }

	  ccb->hardReset();

	  const int maximum_firmware_readback_attempts = 2;
	  int firmware_readback_attempts = 0;
	  do {
	    firmware_lost = false;
            tmb->FirmwareDate(); // reads the month and day off of the tmb
	    int actual_day = tmb->GetReadTmbFirmwareDay();
	    int actual_month = tmb->GetReadTmbFirmwareMonth();
	    tmb->FirmwareYear(); // reads the year off of the tmb
	    int actual_year = tmb->GetReadTmbFirmwareYear();
	    tmb->FirmwareVersion(); // reads the version off of the tmb
	    int actual_type = tmb->GetReadTmbFirmwareType();
	    int actual_version = tmb->GetReadTmbFirmwareVersion();

	    if ((actual_day != expected_day) ||
		(actual_month != expected_month) ||
		(actual_year != expected_year) ||
		(actual_type != expected_type) ||
		(actual_version != expected_version)) {
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

      ccb->RedirectOutput(&cout);

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

    /**************************************************************************
     * DDUReadKillFiber
     *
     *************************************************************************/

    DDUReadKillFiber::DDUReadKillFiber(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void DDUReadKillFiber::display(xgi::Output * out) {
      AddButton(out, "Read DDU Kill Fiber");
    }

    void DDUReadKillFiber::respond(xgi::Input * in, ostringstream & out) {
      out << "=== DDU Read Kill Fiber ===" << endl;

      for(vector <DDU*>::iterator ddu = ddus.begin();
	  ddu != ddus.end();
	  ++ddu) {
	out << "DDU with ctrl fpga user code: " << (*ddu)->CtrlFpgaUserCode()
	    << hex << setfill('0') // set up for next two hex values
	    << " and vme prom user code: "
	    << setw(8) << (*ddu)->VmePromUserCode()
	    << " has Kill Fiber is set to: "
	    << setw(4) << (*ddu)->readFlashKillFiber() << endl;
      }
    }

    /**************************************************************************
     * DDUWriteKillFiber
     *
     *************************************************************************/

    DDUWriteKillFiber::DDUWriteKillFiber(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void DDUWriteKillFiber::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Write DDU Kill Fiber (15 bits in hex)",
			   "KillFiber",
			   "7fff");
    }

    void DDUWriteKillFiber::respond(xgi::Input * in, ostringstream & out) {
      int KillFiber = getFormValueIntHex("KillFiber", in);

      out << "=== DDU Write Kill Fiber ===" << endl;

      for(vector <DDU*>::iterator ddu = ddus.begin();
	  ddu != ddus.end();
	  ++ddu) {
	(*ddu)->writeFlashKillFiber(KillFiber);
      }
    }

   /**************************************************************************
    * ExecuteVMEDSL
    *
    * A domain-specific-lanaguage for issuing vme commands. 
    *************************************************************************/

    ExecuteVMEDSL::ExecuteVMEDSL(Crate * crate)
      : Action(crate)
    { /* ... nothing to see here ... */ }
    
    void ExecuteVMEDSL::display(xgi::Output * out) {
      AddButtonWithTextBox(out,
			   "Execute VME DSL commands in file:",
			   "VMEProgramFile",
			   "/local.home/cscme11/vme.commands",
			   "min-width: 25em; width: 25%; ",
			   "min-width: 40em; width: 70%; ");
    }

    void ExecuteVMEDSL::respond(xgi::Input * in, ostringstream & out) {
      
      int slot = 15; // hard code a default VME slot number
      string programfile = getFormValueString("VMEProgramFile", in);

      //// the arguments for vme_controller ////
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

      while (getline(alltext,line,'\n')) { // read in one line at a time
	
      	if(!line.size()) continue; // Next line, please.

      	istringstream iss(line);

      	int num=0;
      	if(iss >> num && iss.good()){ // the first token is a number, which we interpret as:
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

      	  }else if(num==1){ //// The line begins with 1, so now expect 32 bits for address and 32 bits for data
	    
	    string addr_str;
	    string data_str;
	    string tmp_str;
	    
      	    while(addr_str.size()<32 && iss.good()){ // read in 32 bits for address
      	      iss >> tmp_str;
      	      addr_str += tmp_str;
	      //out<<"addr_str:"<<addr_str<<endl;
      	    }
      	    while(data_str.size()<32 && iss.good()){ // read in 32 bits for data
      	      iss >> tmp_str;
      	      data_str += tmp_str;
	      //out<<"data_str:"<<data_str<<endl;
      	    }
	    
      	    if(addr_str.size()!=32 || data_str.size()!=32){
      	      out<<"ERROR: address("<<addr_str<<") or data("<<data_str<<") is not 32 bits on line: "<<line<<endl;
      	      return;
      	    }
	    
      	    irdwr = (addr_str.at(addr_str.size()-26)=='1')? 2 : 3; // 26th and 25th "bits" from right tell read (10) or write (01)
      	    addr = BinaryString_to_UInt(addr_str);
      	    data = BinaryString_to_UInt(data_str);
	    
      	  }else if(num>1){
      	    out<<"sleep for "<<num<<" microseconds"<<endl;
	    out<<flush;
      	    usleep(num);
      	    continue; // Next line, please.
      	  }else{ // This shouldn't happen
      	    out<<"ERROR: line begins with unexpected number = "<<num<<".  Aborting further execution."<<endl;
      	    return;
      	  }
      
      	}else{//// Parse the line as follows:
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
	      key == "READFILE" ){ // 
	    string tmp_filename;
	    if( iss >> tmp_filename ){
	      programfile = tmp_filename;
	      out << "Read commands from file: " << programfile << endl;
	      continue; // Next line, please.
	    }else{
	      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
	      out<<"  Aborting further execution."<<endl;
	      return;
	    }
	  }else if( key == "slot" ||
	      key == "Slot" ||
	      key == "SLOT" ){ // Try to change the slot number
	    int tmp_slot;
	    if( iss >> tmp_slot ){
	      slot = tmp_slot;
	      out << "Slot number set to " << slot << endl;
	      continue; // Next line, please.
	    }else{
	      out<<"ERROR: did not find an integer number after \""<<key<<"\" on the line: "<<line<<endl;
	      out<<"  Aborting further execution."<<endl;
	      return;
	    }
	  }else if( key == "w" ||
		    key == "W" ||
		    key == "r" ||
		    key == "R" ){
	    if( key == "w" || key == "W" ) irdwr = 2;
	    else irdwr = 3;

	    if( !(iss >> hex >> addr) || !(iss >> hex >> data) ){ // put hex string directly into addr/data
	      out<<"ERROR: problem reading hex values for address or data on the line: "<<line<<endl;
	      out<<"  Aborting further execution."<<endl;
	      return;
	    }

	  }else{
	    out<<"COMMENT: "<<line<<endl;
	    continue; // Next line, please.
	  }
	}

	//// If we make it here, we have a VME command to run! ////

	// set the top bits of address to the slot number
	addr = (addr&0x07ffff) | slot<<19;
	
	printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x})  ",irdwr,addr&0xffffff,data&0xffff,rcv[0]&0xff,rcv[1]&0xff);
	crate->vmeController()->vme_controller(irdwr,addr,&data,rcv); // Send the VME command!
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
      : Action(crate)
    { /* ... nothing to see here ... */ }

    void BuckShiftTest::display(xgi::Output * out) {
      AddButton(out, "Buck Shift Test");
    }

    void BuckShiftTest::respond(xgi::Input * in, ostringstream & out) {
      out << "=== Buck Shift Test ===" << endl;
      crate->vmeController()->SetPrintVMECommands(1); // turn on debug printouts of VME commands
      for(vector <DAQMB*>::iterator dmb = dmbs.begin();
	  dmb != dmbs.end();
	  ++dmb) {
	int val = (*dmb)->buck_shift_test();
	cout<<"Buck Shift Test returns: "<<val<<endl;
      }
      crate->vmeController()->SetPrintVMECommands(0); // turn off debug printouts of VME commands
    }


    IndaraButton::IndaraButton(Crate * crate)
     : Action(crate)
    { }

    void IndaraButton::display(xgi::Output * out) {
      AddButton(out, "IndaraButton");
    }
    void IndaraButton::respond(xgi::Input * in, ostringstream & out) {
      out << "You Pushed Indara's Buttons!! " <<endl;

   }
//    /**************************************************************************
//     * ActionTemplate
//     *
//     *************************************************************************/
/*
    ActionTemplate::ActionTemplate(Crate * crate)
     : Action(crate)
    { }

    void ActionTemplate::display(xgi::Output * out) {

    }
    void ActionTemplate::respond(xgi::Input * in, ostringstream & out) {

   }*/
  }
}
