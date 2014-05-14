#include "emu/odmbdev/VMEWrapper.h" 
#include "emu/odmbdev/Manager.h"
#include "emu/odmbdev/utils.h"
#include <iomanip>
#include <string>
#include <stdlib.h>

namespace emu { namespace odmbdev {
    VMEWrapper::VMEWrapper(Crate * crate) : 
      crate_(crate),
      port_number_(Manager::getPortNumber()),
      logFile_(GetLogFileName(port_number_))
    {
      logger_.open(logFile_.c_str(), std::ofstream::out | std::ofstream::app );
    }
    
    VMEWrapper::VMEWrapper( const VMEWrapper& other ) : 
      crate_(other.crate_),
      port_number_(other.port_number_),
      logFile_(other.logFile_)
    {
      // copy constructor
      logger_.open(logFile_.c_str(), std::ofstream::out | std::ofstream::app );
    }
    
    void VMEWrapper::VMEWrite (unsigned short int address, unsigned short int command, unsigned int slot, string comment) {
      unsigned int shiftedSlot = slot << 19;
      int addr = (address & 0x07ffff) | shiftedSlot;
      char rcv[2];
      crate_->vmeController()->vme_controller(3,addr,&command,rcv);
      // Get timestamp
      time_t rawtime;
      struct tm * timeinfo;
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      char timestamp[30];
      strftime (timestamp,30,"%D %X",timeinfo);
      // Write to log
      logger_ << "W" << std::setw(7) << FixLength(address,4,true)
	      << std::setw(7) << FixLength(command,4,true)
	      << std::setw(15) << dec << slot 
	      << "   " << timestamp
	      << "    " << comment.c_str() << endl;
    }
    unsigned short int VMEWrapper::VMERead (unsigned short int address, unsigned int slot, string comment) {
      unsigned int shiftedSlot = slot << 19;
      int addr = (address & 0x07ffff) | shiftedSlot;
      unsigned short int data;
      char rcv[2];
      crate_->vmeController()->vme_controller(2,addr,&data,rcv);
      unsigned short int result = (rcv[1] & 0xff) * 0x100 + (rcv[0] & 0xff);
        
      // Get timestamp
      time_t rawtime;
      struct tm * timeinfo;
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      char timestamp[30];
      strftime (timestamp,30,"%D %X",timeinfo);
      // Get formatting
      bool readHex = true;
      if((address >= 0x321C && address <= 0x337C) || (address >= 0x33FC && address < 0x35AC)  || 
	 (address > 0x35DC && address <= 0x3FFF) || address == 0x500C || address == 0x510C ||
	 address == 0x520C || address == 0x530C || address == 0x540C   || address == 0x550C || address == 0x560C 
	 || address == 0x8004 ||  (address == 0x5000 && result < 0x1000)) {
	readHex = false;
      }
      // Write to log
      logger_ << "R" << std::setw(7) << FixLength(address,4,true);
      if (readHex) logger_<< std::setw(14) << FixLength(result,4,true);
      else logger_<< std::setw(12) << dec << result << "_d";
      logger_ << std::setw(8) << dec << slot 
	      << "   " << timestamp
	      << "    " << comment.c_str() << endl;
      return result;
    }
    unsigned int VMEWrapper::JTAGShift(unsigned short int IR, unsigned short int DR, unsigned int nBits, unsigned short int hdr_tlr_code, unsigned int slot) {
      this->VMEWrite(0x191C, IR, slot, "Set instruction register");
      unsigned int addr_data_shift(0x1);
      addr_data_shift = (addr_data_shift<<4)|(nBits-1);
      addr_data_shift = (addr_data_shift<<8)|hdr_tlr_code; // hdr_tlr_code is 4, 8, or C,
      // depending on nBits to shift
      this->VMEWrite(addr_data_shift, DR, slot, "Shift data");
      usleep(100);
      unsigned short int VMEresult;
      VMEresult = this->VMERead(0x1014, slot, "Read TDO register");
      usleep(100);
      unsigned int formatted_result(VMEresult>>(16-nBits));
      return formatted_result;
    }
    unsigned int VMEWrapper::JTAGRead (unsigned short int DR, unsigned int nBits, unsigned int slot) {
      unsigned int result;
      result = this->JTAGShift(0x3C2, DR, 8, 0xC, slot);
      result = this->JTAGShift(0x3C3, 0, nBits, 0xC, slot);
      /*unsigned short int VMEresult;
	VMEresult = this->VMERead(0x1014, slot, "Read TDO register");*/
      unsigned int formatted_result(result);
      return formatted_result;
    }
    string VMEWrapper::ODMBVitals(unsigned int slot) {
      string message;
      unsigned int qpll_lock(this->VMERead(0x3124,slot,"Read QPLL lock"));

      unsigned int VMEresult = this->VMERead(0x4100,slot,"Read unique ID");
      if (FixLength(VMEresult,4,true)=="BAAD"||VMEresult<0x2000) message += "\nError: ODMB not connected or FW out-of-date.\n";
      else {
        message += ( "ID = " + FixLength(VMEresult,4,true) );
	string fwv(FixLength(this->VMERead(0x4200,slot,"Read fw version"),3,true));
	VMEresult = this->VMERead(0x4300,slot,"Read fw build");
	message += ( ", FW version.build = " + fwv +  "." + FixLength(VMEresult,1,true) + ", ");
	if(qpll_lock == 1) message += "QPLL locked";                                                                                               
	else message+= "QPLL UNLOCKED";
      }
      return message;
    }
    string VMEWrapper::SYSMONReport(unsigned int slot) {
      string message("");
      unsigned int odmb_id = this->VMERead(0x4100, slot, "Checking ODMB version");
      unsigned int odmb_version = odmb_id >> 12u;
      unsigned int read_addr_vec[9] = {0x7150, 0x7120, 0x7000, 0x7160, 0x7140, 0x7100, 0x7130, 0x7110, 0x7170};
      string description[9] = {"C\t -  Thermistor 1 temperature", odmb_version<=3?"C\t -  Thermistor 2 temperature":"mA\t -  IPPIB: Current for PPIB", "C\t -  FPGA temperature", 
			       "V\t -  P1V0: Voltage for FPGA", "V\t -  P2V5: Voltage for FPGA", "V\t -  LV_P3V3: Voltage for FPGA", 
			       "V\t -  P3V6_PP: Voltage for PPIB", "V\t -  P5V: General voltage", "V\t -  P5V_LVMB: Voltage for LVMB"};
      string error_message[9] ={" C above temperature threshold",odmb_version<=3?" C above temperature threshold":" mA above current threshold"," C above temperature threshold",
				"% below nominal voltage","% below nominal voltage","% below nominal voltage",
				"% below nominal voltage","% below nominal voltage","% below nominal voltage"};
      float thresholds[9] = {55., odmb_version<=3?55:800., 55., 0.93, 2.35, 3.1, 3.35, 4.75, 4.75};
      float voltmax[9] = {1.0, 1.0, 1.0, 1.0, 2.5, 3.3, 3.6, 5.0, 5.0};
      float result2[9];
      for (int i = 0; i < 9; i++){
	unsigned short int VMEresult = this->VMERead(read_addr_vec[i],slot,description[i]);
	VMEresult = this->VMERead(read_addr_vec[i],slot,description[i]);
	if (i == 0 && VMEresult > 0xfff){
	  cout << "ERROR: bad readout from system monitoring.\n";
	  message += "ERROR: bad readout from system monitoring.\n";
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

	message += ( fix_width(result2[i], 4) + " " + description[i]);
	char c_result[6];
	sprintf (c_result, "%s", fix_width(result2[i], 4).c_str());
	float result(atof(c_result));
	bool warning_tc(false), warning_v(false);
	if (i<3 && result>thresholds[i]) warning_tc = true;
	if (i>=3 && result<thresholds[i]) warning_v = true;
	float diff(0.);
	if (warning_tc||warning_v) {
	  if (warning_tc) diff = result-thresholds[i];
	  else if (warning_v) diff = (voltmax[i]-result)/(.01*voltmax[i]);
	  if (i==1||i==2||i==7) message += "\t";
	  if (i!=8) message += "\t";
	  message += ("\t\t\t\t====> " + fix_width(diff, 4) + error_message[i]); 
	}
	message+="\n";
      }
      return message;
    }
    string VMEWrapper::BurnInODMBs() {
      
      // Get timestamp
      time_t rawtime;
      struct tm * timeinfo;
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );
      char timestamp[30];
      strftime (timestamp,30,"%F %X",timeinfo);

      string summary(timestamp);
      summary+="\n====================\n";

      unsigned int odmb_slot[9] = {3, 5, 7, 9, 11, 15, 17, 19, 21};
      char c_slot[4];
      for (unsigned int s=0; s < 9; s++) {
	unsigned int VMEresult = this->VMERead(0x4100,odmb_slot[s],"Read unique ID");
	if (VMEresult>0x5000||VMEresult<0x2000) {
	  usleep(100);
	  continue;
	}	sprintf (c_slot, "%d", odmb_slot[s]);
	summary += ( "***** SLOT " + (string)c_slot + ": " + this->ODMBVitals(odmb_slot[s]) + " *****\n" );
	summary += ( this->SYSMONReport(odmb_slot[s]) + "\n" );
      }

      return summary;
    }
  }
}
