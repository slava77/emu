//-----------------------------------------------------------------------
// $Id: DAQMB.cc,v 3.100 2013/02/21 16:09:14 liu Exp $
// $Log: DAQMB.cc,v $
// Revision 3.100  2013/02/21 16:09:14  liu
// add extra delays for LVMB reading
//
// Revision 3.99  2013/02/20 22:20:55  liu
// update DCFEB voltages/currents monitoring code
//
// Revision 3.98  2013/02/20 20:02:39  liu
// new code to read new LVMB's ADCs
//
// Revision 3.97  2013/02/19 04:49:34  liu
// avoid a crash in epromread()
//
// Revision 3.96  2013/02/18 21:52:21  liu
// add debug info
//
// Revision 3.95  2012/12/21 16:48:41  liu
// bug fix in buck_shift()
//
// Revision 3.94  2012/12/03 17:01:04  banicz
// Dan's changes:
// add a function to set and initialize the dcfeb pipeline and fine delays for use after hard resets which wipe these values from the dcfebs.
//
// Revision 3.93  2012/11/28 03:14:04  liu
// add DCFEB fine delay parameter
//
// Revision 3.92  2012/11/26 21:10:02  liu
// add DCFEB pipeline_depth parameter
//
// Revision 3.91  2012/11/15 18:12:10  liu
// *** empty log message ***
//
// Revision 3.90  2012/11/15 18:00:41  liu
// bug fix in preamp_initx() and update Virtex6 monitoring
//
// Revision 3.89  2012/10/16 22:39:07  liu
// read & write FPGA internal registers
//
// Revision 3.88  2012/10/15 20:02:53  liu
// *** empty log message ***
//
// Revision 3.87  2012/10/15 16:02:37  liu
// DCFEB firmware loading update
//
// Revision 3.86  2012/10/11 21:26:44  liu
// add DCFEB firmware download and readback
//
// Revision 3.85  2012/10/08 22:04:44  liu
// DCFEB update
//
// Revision 3.84  2012/10/08 02:35:05  liu
// DCFEB update
//
// Revision 3.83  2012/09/30 21:19:42  liu
// update for ME11 new electronics
//
// Revision 3.82  2012/07/10 15:27:57  liu
// add function used by STEP
//
// Revision 3.81  2012/06/28 18:59:51  liu
// fix CFEB firmware downloading bug by closing the file after use
//
// Revision 3.80  2012/06/20 08:45:01  kkotov
//
// New faster DMB/CFEB EPROM readback routines
//
// Revision 3.79  2012/05/09 20:59:00  liu
// fix missing standard header files
//
// Revision 3.78  2012/04/27 15:48:42  kkotov
//
// Safeguard preventing the library to crash on a missing file in smuload_verify
//
// Revision 3.77  2012/04/09 15:30:33  liu
// fix CVS header
//
// Revision 3.76  2012/04/09 15:25:28  liu
// update for Khristian Kotov
//
// Revision 3.75  2012/03/19 22:52:37  liu
// for Khritian Kotov: CFEB firmware fix
//
// Revision 3.74  2012/02/20 14:41:56  liu
// avoid compiler warning
//
// Revision 3.73  2012/02/14 09:59:22  liu
// comment out some debug info
//
// Revision 3.72  2012/02/14 09:50:32  liu
// *** empty log message ***
//
// Revision 3.71  2012/02/14 09:46:13  liu
// fix FPGA & PROM ID bugs
//
// Revision 3.70  2012/02/13 13:27:25  liu
// fix CFEB firmware downloading bug
//
// Revision 3.69  2011/09/02 14:07:55  cvuosalo
// Added additional diagnostics for CFEB FPGA check
//
// Revision 3.68  2011/09/01 15:54:30  cvuosalo
// Added check to CFEB FPGA test for bad read
//
// Revision 3.67  2011/08/26 16:31:57  cvuosalo
// Adding CFEB FPGA check to Expert Tools and DMB Utils pages
//
// Revision 3.66  2011/02/09 15:10:36  liu
// fix compiler warnings
//
// Revision 3.65  2010/08/23 11:24:25  rakness
// add DMB Control and FPGA firmware check
//
// Revision 3.64  2010/03/03 20:33:19  liu
// adjust CFEB calib timing by Stan
//
// Revision 3.63  2010/01/06 08:32:56  rakness
// make CFEB configuration check more robust for EEPROM bit loss
//
// Revision 3.62  2009/12/18 09:42:48  rakness
// update firmware reloading routine to emulate expert actions
//
// Revision 3.61  2009/12/03 16:39:59  liu
// open eprom.bit file in /tmp instead of current directory
//
// Revision 3.60  2009/11/19 17:04:10  rakness
// remove some extraneous cout statements
//
// Revision 3.59  2009/10/28 09:51:13  liu
// fix a typo
//
// Revision 3.58  2009/10/27 11:43:42  liu
// add back power_mask parameter
//
// Revision 3.57  2009/10/27 11:07:56  rakness
// add firmware check to DAQMB configuration check
//
// Revision 3.55  2009/08/15 07:56:02  durkin
// fixed several bugs and added new routines
//
// Revision 3.54  2009/08/13 01:48:21  liu
// to skip monitoring if vme access failed
//
// Revision 3.53  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 3.52  2009/03/15 13:58:54  liu
// fix compiler warnings
//
// Revision 3.51  2008/10/27 08:52:58  gujh
// Put the CFEB PROM firmware readback capability
//   ---- GU, Oct. 27, 2008
//
// Revision 3.50  2008/09/21 18:26:15  liu
// monitoring update
//
// Revision 3.49  2008/08/26 11:57:06  liu
// change abs() to fabs()
//
// Revision 3.48  2008/08/13 12:49:23  geurts
// explicit std:: reference for abs() function. (Note: this should probably be changed to fabs())
//
// Revision 3.47  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.46  2008/08/08 11:01:24  rakness
// centralize logging
//
// Revision 3.45  2008/08/06 17:24:50  rakness
// add known_problem parameter to xml file; add time stamp + number of reads to config check output file
//
// Revision 3.44  2008/08/05 11:41:48  rakness
// new configuration check page + clean up output
//
// Revision 3.43  2008/08/05 08:40:37  rakness
// add minimum number of times to read when checking configuration
//
// Revision 3.42  2008/07/16 17:28:37  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 3.41  2008/07/15 08:05:57  gujh
// Fixed the bug for CFEB_kill channels
//          ---- GU, July 15, 2008
//
// Revision 3.40  2008/06/22 14:57:52  liu
// new functions for monitoring
//
// Revision 3.39  2008/06/12 21:08:55  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.38  2008/05/28 10:35:31  liu
// DMB counters in jumbo packet
//
// Revision 3.37  2008/03/30 08:25:37  gujh
// Added the corresponding code for DMB/CFEB fine L1A delay adjustment ---GU, Mar. 30, 2008
//
// Revision 3.36  2008/03/27 17:21:54  gujh
//  fix the DAQMB::configure, change the DAV_delay default  ---GU
//
// Revision 3.33  2008/02/24 12:48:30  liu
// DMB online counters
//
// Revision 3.32  2008/01/16 16:47:46  liu
// update print out
//
// Revision 3.31  2008/01/16 14:32:54  liu
// free momery allocated by malloc()
//
// Revision 3.30  2008/01/08 10:22:55  liu
// change %2X to %2hhx in scanf to remove warning; remove epromloadold().
//
// Revision 3.29  2007/08/28 23:13:47  liu
// remove compiler warnings
//
// Revision 3.28  2007/06/06 14:47:41  gujh
// Added methods: buck_shift_comp_bc and set_comp_thresh_bc for broadcast
// loading the comparator pattern and comparator threshold
//      ---- June 6, 2007.   GU
//
// Revision 3.27  2007/05/20 15:48:13  gujh
// Try a maximum of three times if the usercode readback is FFFFFFFF, to protect
// the board number (DMB/CFEB) overwritten by bad readout.
//       --- May 20, 2007.  GU
//
// Revision 3.26  2007/05/03 21:34:23  gujh
// Fix the PROM USERID loading bug
//     ---- May 3, 2007   GU
//
// Revision 3.25  2007/04/24 19:35:46  gujh
// Fixed bug for DMB/CFEB firmware broadcast download
//     --- Apr. 24, 2007   GU
//
// Revision 3.24  2007/04/20 16:06:39  gujh
// Add epromdownload_broadcast for broadcast download
//     --- Apr. 20, 2007  GU
//
// Revision 3.23  2007/04/12 16:34:27  gujh
// Add the buck_shift_ext_bc(strip) function
//     --- Apr. 12, 2007,  GU
//
// Revision 3.22  2007/04/11 18:01:55  gujh
// Move the LctL1aDelay to the CFEB part in DAQMB::Configure
//         --- Apr. 11, 2007, GU
//
// Revision 3.21  2007/03/13 20:56:08  gujh
// Add function DAQMB::set_rndmtrg_rate(int)
//          --- Mar. 13, 2007   GU
//
// Revision 3.20  2007/03/02 20:26:18  gujh
// fix the delay in epromload
//
// Revision 3.19  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.18  2006/10/12 17:52:13  mey
// Update
//
// Revision 3.17  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.16  2006/10/05 08:20:10  mey
// UPdate
//
// Revision 3.15  2006/10/05 07:49:42  mey
// UPdate
//
// Revision 3.14  2006/10/02 18:17:55  mey
// UPdate
//
// Revision 3.13  2006/09/28 12:52:35  mey
// Update
//
// Revision 3.12  2006/09/27 16:28:46  mey
// UPdate
//
// Revision 3.11  2006/09/27 16:18:59  mey
// UPdate
//
// Revision 3.10  2006/09/19 08:16:52  mey
// UPdate
//
// Revision 3.9  2006/09/15 06:50:39  mey
// Update
//
// Revision 3.8  2006/09/15 06:47:36  mey
// Update
//
// Revision 3.7  2006/09/14 09:10:06  mey
// UPdate
//
// Revision 3.6  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.5  2006/09/12 15:50:01  mey
// New software changes to DMB abd CFEB
//
// Revision 3.4  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.3  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.2  2006/08/02 14:44:20  mey
// Fixed downloading
//
// Revision 3.1  2006/08/02 12:24:28  mey
// Added LctL1aDelay
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.67  2006/07/20 11:48:39  mey
// Program SFM
//
// Revision 2.66  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.65  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.64  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.63  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.62  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 2.61  2006/06/22 13:06:14  mey
// Update
//
// Revision 2.60  2006/05/31 15:58:46  mey
// Update
//
// Revision 2.59  2006/05/29 08:09:20  mey
// Update
//
// Revision 2.58  2006/05/22 16:32:40  mey
// UPdate
//
// Revision 2.57  2006/05/22 16:30:59  mey
// UPdate
//
// Revision 2.56  2006/05/22 16:28:10  mey
// UPdate
//
// Revision 2.55  2006/05/22 16:21:43  mey
// UPdate
//
// Revision 2.54  2006/05/22 16:20:38  mey
// UPdate
//
// Revision 2.53  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.52  2006/05/19 13:35:03  mey
// Update
//
// Revision 2.51  2006/05/19 13:24:42  mey
// Update
//
// Revision 2.50  2006/05/19 13:22:36  mey
// Update
//
// Revision 2.49  2006/05/19 13:19:11  mey
// Update
//
// Revision 2.48  2006/05/19 13:17:16  mey
// Update
//
// Revision 2.47  2006/05/12 11:38:39  mey
// Update
//
// Revision 2.46  2006/05/12 11:09:26  mey
// Update
//
// Revision 2.45  2006/05/12 11:03:12  mey
// Update
//
// Revision 2.44  2006/05/12 10:54:13  mey
// Update
//
// Revision 2.43  2006/05/12 10:53:20  mey
// Update
//
// Revision 2.42  2006/05/12 10:39:44  mey
// Update
//
// Revision 2.41  2006/05/12 08:03:06  mey
// Update
//
// Revision 2.40  2006/05/11 13:14:33  mey
// Update
//
// Revision 2.39  2006/05/11 11:00:26  mey
// Update
//
// Revision 2.38  2006/05/10 10:07:56  mey
// Update
//
// Revision 2.37  2006/05/02 21:23:27  mey
// Update
//
// Revision 2.36  2006/05/02 21:12:43  mey
// UPdate
//
// Revision 2.35  2006/05/02 21:04:28  mey
// UPdate
//
// Revision 2.34  2006/04/24 14:57:21  mey
// Update
//
// Revision 2.33  2006/03/31 09:22:05  mey
// Update
//
// Revision 2.32  2006/03/15 13:50:11  mey
// Update
//
// Revision 2.31  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.30  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 2.29  2006/01/30 09:29:53  mey
// Update
//
// Revision 2.28  2006/01/25 19:25:06  mey
// Update
//
// Revision 2.27  2006/01/19 10:03:46  mey
// Update
//
// Revision 2.26  2006/01/12 23:44:26  mey
// Update
//
// Revision 2.25  2006/01/12 12:28:41  mey
// UPdate
//
// Revision 2.24  2006/01/11 13:47:32  mey
// Update
//
// Revision 2.23  2006/01/10 23:32:40  mey
// Update
//
// Revision 2.22  2005/12/16 17:49:47  mey
// Update
//
// Revision 2.21  2005/12/15 14:23:34  mey
// Update
//
// Revision 2.20  2005/12/06 17:58:26  mey
// Update
//
// Revision 2.19  2005/12/05 13:20:58  mey
// Got rid of flush_vme
//
// Revision 2.18  2005/11/30 16:49:26  mey
// Bug DMB firmware load
//
// Revision 2.17  2005/11/30 12:59:52  mey
// DMB firmware loading
//
// Revision 2.16  2005/11/25 23:42:33  mey
// Update
//
// Revision 2.15  2005/11/22 15:14:53  mey
// Update
//
// Revision 2.14  2005/11/21 15:48:06  mey
// Update
//
// Revision 2.13  2005/11/08 06:50:12  mey
// Update
//
// Revision 2.12  2005/11/02 10:01:05  mey
// Update
//
// Revision 2.11  2005/10/04 16:02:07  mey
// Added Output
//
// Revision 2.10  2005/09/28 16:57:22  mey
// Update Tests
//
// Revision 2.9  2005/09/26 07:27:14  mey
// Added BXN toogle routine
//
// Revision 2.8  2005/09/19 07:13:50  mey
// Update
//
// Revision 2.7  2005/09/13 14:46:40  mey
// Get DMB crate id; and DCS
//
// Revision 2.6  2005/09/07 16:18:16  mey
// DMB timing routines
//
// Revision 2.5  2005/09/07 13:54:39  mey
// Included new timing routines from Jianhui
//
// Revision 2.4  2005/09/06 12:11:51  mey
// Update
//
// Revision 2.3  2005/08/31 15:12:58  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 10:05:51  geurts
// calibration-related updates by Alex Tumanov and Jason Gilmore
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
/* New motherboard VTX2 not VTX  so MCTRL,6  not MCTRL,5 */
#include "emu/pc/DAQMB.h"
#include "emu/pc/VMEController.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <unistd.h>
#include <iomanip>
#include "emu/pc/geom.h"
#include <sys/types.h>
#include <sys/stat.h>


#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl; 
#define PRINTSTRING(x) std::cout << #x << std::endl; 
#endif

namespace emu {
  namespace pc {

// declarations
void Parse(char *buf,int *Count,char **Word);
void shuffle(char *a,char *b);


// static init
const int DAQMB::layers[6] = {2,0,4,5,3,1};
const int DAQMB::chip_use[7][6] = {
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1},
  {1,1,1,1,1,1}
};

const int DAQMB::nchips[7] = {6,6,6,6,6,6,6};

DAQMB::DAQMB(Crate * theCrate, Chamber * theChamber, int newslot):
  VMEModule(theCrate, newslot),
  EmuLogger(),
  csc_(theChamber),
  feb_dav_delay_(23),tmb_dav_delay_(22), 
  push_dav_delay_(31), l1acc_dav_delay_(24),
  calibration_LCT_delay_(8), calibration_l1acc_delay_(22),
  pulse_delay_(15), inject_delay_(15),
  pul_dac_set_(1.0), inj_dac_set_(1.0),
  set_comp_thresh_(0.03),
  comp_timing_(1), comp_mode_(2), pre_block_end_(7),
  cable_delay_(0), crate_id_(0xfe), toogle_bxn_(1), ALCT_dav_delay_(2),
  killflatclk_(5183),
  l1a_lct_counter_(-1), cfeb_dav_counter_(-1), 
  tmb_dav_counter_(-1), alct_dav_counter_(-1),
  failed_checkvme_(-1), power_mask_(0)
{
   hardware_version_=0;
   lvdb_mapping_=0;
   //
   //get the initial value first:
   killinput_=GetKillInput();
   cfeb_clk_delay_=GetCfebClkDelay();
   xfinelatency_=GetxFineLatency();
   xlatency_=GetxLatency();

  expected_control_firmware_tag_ = 9999;
  expected_vme_firmware_tag_     = 9999;
  expected_firmware_revision_    = 9999;
  //
  for (int cfeb=0; cfeb<7; cfeb++) {
    expected_cfeb_firmware_tag_[cfeb] = 9999;
    comp_mode_cfeb_[cfeb]   = comp_mode_;
    comp_timing_cfeb_[cfeb] = comp_timing_;
    comp_thresh_cfeb_[cfeb] = set_comp_thresh_;
    pre_block_end_cfeb_[cfeb] = pre_block_end_;
    L1A_extra_cfeb_[cfeb] = xlatency_;
    cfeb_config_status_[cfeb] = false;
    smoking_gun_status_[cfeb] = false;
  }
  dmb_smoking_gun_status_ = false;
  //
  theChamber->SetDMB(this);
  //
  cfebs_.clear();
  std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
  for(int i=0;i<20;i++) TestStatus_[i]=-1;
  //
  //std::cout << "&&&&&&&&&&&&&&&&&& CrateId " << crate_id_ << std::endl ;
  //
}

//DAQMB::DAQMB(Crate * theCrate,int newslot,  int newcfeb):
//VMEModule(theCrate, newslot)
//{
// MyOutput_ = &std::cout ;
//cfebs_.push_back(CFEB(newcfeb));
//std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
//for(int i=0;i<20;i++) TestStatus_[i]=-1;
//}

//DAQMB::DAQMB(Crate * theCrate,int newslot, const std::vector<CFEB> & cfebs):
//VMEModule(theCrate, newslot),
//cfebs_(cfebs)
//{
// MyOutput_ = &std::cout ;
//std::cout << "DMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl; 
//for(int i=0;i<20;i++) TestStatus_[i]=-1;
//}


DAQMB::~DAQMB() {
  std::cout << "Killing DAQMB" << std::endl;
}


void DAQMB::end()
{
  //std::cout << "calling DAQMB::end" << std::endl;
  theController->send_last();
  VMEModule::end();
}

void DAQMB::init(){
}

bool DAQMB::SelfTest(){
  return 0;
}

int DAQMB::ReadRegister(unsigned address)
{
   int temp;
   read_now(address, (char *)&temp);
   return temp&0xFFFF;
}

void DAQMB::WriteRegister(unsigned address, int value)
{
   unsigned short data = value & 0xFFFF;
   write_now(address, data, NULL);
}

//
std::ostream & operator<<(std::ostream & os, DAQMB & daqmb) {
  os << std::dec << "feb_dav_delay " << daqmb.feb_dav_delay_ << std::endl
     << "tmb_dav_delay " << daqmb.tmb_dav_delay_ << std::endl
     << "push_dav_delay " << daqmb.push_dav_delay_ << std::endl
     << "l1acc_dav_delay " << daqmb.l1acc_dav_delay_ << std::endl
     << "ALCT_dav_delay " << daqmb.ALCT_dav_delay_ << std::endl
     << "calibration_LCT_delay_ " << daqmb.calibration_LCT_delay_ << std::endl
     << "calibration_l1acc_delay_ " << daqmb.calibration_l1acc_delay_ << std::endl
     << "pulse_delay_ " << daqmb.pulse_delay_ << std::endl
     << "inject_delay_ " <<daqmb.inject_delay_ << std::endl
     << "pul_dac_set_  " << daqmb.pul_dac_set_ << std::endl
     << "inj_dac_set_ " << daqmb.inj_dac_set_ << std::endl
     << "set_comp_thresh_ " << daqmb.set_comp_thresh_ << std::endl
     << "comp_timing_ " << daqmb.comp_timing_ << std::endl
     << "comp_mode_ " << daqmb.comp_mode_ << std::endl
     << "pre_block_end_ " << daqmb.pre_block_end_ << std::endl
     << "l1a_lct_counter_ " << daqmb.l1a_lct_counter_ << std::endl
     << "cfeb_dav_counter_ " << daqmb.cfeb_dav_counter_ << std::endl
     << "tmb_dav_counter_ " << daqmb.tmb_dav_counter_ << std::endl
     << "alct_dav_counter_ " << daqmb.alct_dav_counter_ << std::endl
     << "cable_delay_ "<<daqmb.cable_delay_ << std::endl
     << "crate_id_ " << daqmb.crate_id_ << std::endl
     << "toogle_bxn_ " << daqmb.toogle_bxn_ << std::endl
     << "xlatency, cfebclk etc " << daqmb.killflatclk_ << std::endl;
  return os;
}
//
void DAQMB::configure() {
   //
  //  std::cout << std::endl;
   //
   // (*MyOutput_) << "DAQMB: switching on LVs on LVMB" << endl; 
  //   lowv_onoff(0x3f); 
  //   ::sleep(2);
  //   calctrl_fifomrst();
  //   ::sleep(1);
   (*MyOutput_) << "Toogle bxn " << crate_id_ << std::endl ;
   if (toogle_bxn_) ToogleBXN();

   //
   //(*MyOutput_) << std::endl;
   (*MyOutput_) << "CFEB size="<<cfebs_.size()<<std::endl;
  //
   std::ostringstream dump, dump2, dump3, dump4;
  dump2 << (int)this->crate();
  dump3 << (int)this->slot();
  dump  << "DAQMB: configure() for crate = " ;
  dump4 << " and slot = " ;
  //
  SendOutput(dump.str()+dump2.str()+dump4.str()+dump3.str(),"INFO");
  //
  (*MyOutput_) << "DAQMB: configure() for crate " << this->crate() << " slot " << this->slot() << std::endl;
  //

   //get the initial value first:
   killinput_=GetKillInput();
   cfeb_clk_delay_=GetCfebClkDelay();
   xfinelatency_=GetxFineLatency();
   xlatency_=GetxLatency();

   //***Do this setting only for calibration ****
  int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
     | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
   (*MyOutput_) << "DAQMB:configure: caldelay " << std::hex << cal_delay_bits << std::dec << std::endl;
   setcaldelay(cal_delay_bits);
   (*MyOutput_) << "doing set_cal_dac " << inj_dac_set_ << " " 
	<<  pul_dac_set_ << std::endl;
   set_cal_dac(inj_dac_set_, pul_dac_set_);
   //

   //*** Do this setting only for older DMB firmware (V18 or older)
   int dav_delay_bits = (feb_dav_delay_    & 0x1F)
      | (tmb_dav_delay_ & 0X1F) << 5
      | (push_dav_delay_   & 0x1F) << 10
      | (l1acc_dav_delay_  & 0x3F) << 15
      | (ALCT_dav_delay_   & 0x1F) << 21;
   (*MyOutput_) << "doing setdavdelay " << dav_delay_bits << std::endl;
   setdavdelay(dav_delay_bits);
   //

   // *** This part is for Buck_Flash (Parallel Memory) *****
   int comp_mode_bits = (comp_mode_ & 3) | ((comp_timing_ & 7) << 2);
   //
   cfebs_readstatus();
   bool cfebmatch=true;
   //check the comp_timing, comp_mode, Pre_block_end and Extr_l1A latency setting
   for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
     //for (int lfeb=0;lfeb<5;lfeb++) {
     //if (iuse[lfeb]==1) {
     //
     for (int y=0; y<4; y++) printf("%2x \n",(febstat_[lfeb][y]&0xff));
     //
     std::cout << "<>" << comp_mode_bits << " " << pre_block_end_ << " " << xlatency_ << std::endl;
     //
     std::cout << "Match &&&&&&&&&&& " << cfebmatch << std::endl;
     //
     std::cout << "1 " << ((febstat_[lfeb][2])&0x1f) << std::endl;
     std::cout << "2 " << (((febstat_[lfeb][2]>>5)&0x07)+((febstat_[lfeb][3]&0x01)<<3)) << std::endl;
     std::cout << "3 " << ((febstat_[lfeb][3]>>1)&0x03) << std::endl;
     //
     if ((((febstat_[lfeb][2])&0x1f)!=comp_mode_bits) ||
	 ((((febstat_[lfeb][2]>>5)&0x07)+((febstat_[lfeb][3]&0x01)<<3))!=pre_block_end_)||
       (((febstat_[lfeb][3]>>1)&0x03)!=xlatency_)) {cfebmatch=false;
       std::cout << "Reprogram DMB flash cfeb" << lfeb << std::endl;
       std::cout << "comp_mode_bits old " << std::hex << (febstat_[lfeb][2]&0x1f) << " new " << comp_mode_bits << std::dec << std::endl;
       std::cout << " pre_block_end old " << (((febstat_[lfeb][2]>>5)&0x07)+((febstat_[lfeb][3]&0x01)<<3)) << " new " << pre_block_end_ << std::endl;
       std::cout << " xlatency old " << ((febstat_[lfeb][3]>>1)&0x03) << " new " << xlatency_ << std::endl;
       }
   }
   //
   enable_cfeb(); //enable..disable CFEBs
   //
   //check the comp_dac setting
   float compthresh[5];
   //
   std::cout << "Match &&&&&&&&&&& " << cfebmatch << std::endl;
   //
   for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++)compthresh[lfeb]=adcplus(2,lfeb);
   //
   (*MyOutput_) << "doing set_comp_thresh " << set_comp_thresh_ << std::endl;

     set_comp_thresh(set_comp_thresh_);
     //(*MyOutput_) << "doing preamp_initx() " << std::endl;
     preamp_initx();

   //  If the comparator threshold setting is more than 5mV off, re-program the BuckFlash
   //for (int lfeb=0;lfeb<5;lfeb++)
   for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
     std::cout << "****************** thresh " << compthresh[lfeb] << " " << adcplus(2,lfeb) << std::endl;
     if((std::fabs(compthresh[lfeb]-adcplus(2,lfeb))>5.)) cfebmatch=false;
    
   }

   if (!cfebmatch) {
     //
     (*MyOutput_) << "doing fxpreblkend " << pre_block_end_ << std::endl;
     fxpreblkend(pre_block_end_);
     //
     (*MyOutput_) << "doing set_comp_mode " << comp_mode_bits << std::endl;
     (*MyOutput_) << comp_mode_ << " " << comp_timing_ << std::endl;
     set_comp_mode(comp_mode_bits);
     LctL1aDelay(xlatency_);
     //
     
     char * flash_content=(char *)malloc(500);
     int n_byts = Fill_BUCK_FLASH_contents(flash_content);
     buckflash_erase();
     buckflash_load2(n_byts,flash_content);
     sleep(2);
     buckflash_pflash();
     sleep(5);
     buckflash_init();
     sleep(1); 
     free(flash_content);     
     //

   }

   set_and_initalize_pipelines_and_fine_delays();

  // ***  This part is related to the SFM (Serial Flash Memory) ****
   //
   // Readout the Current setting on DMB
   char dmbstatus[11];
   dmb_readstatus(dmbstatus);
   //check the DMB setting with the current setup
   if (((CableDelay_)!=cable_delay_)||
       ((CrateID_)!=crate_id_)||
       ((CfebClkDelay_)!=cfeb_clk_delay_)||
       ((XLatency_)!=xlatency_) ||
       ((KillInput_)!=killinput_)||
       ((XFineLatency_)!=xfinelatency_) ) {
     //
	std::cout << "Reprogram DMB SFM flash " << std::endl;
	std::cout << " CableDelay old " << CableDelay_ << " new " << cable_delay_ <<std::endl;
	std::cout << " CrateID old " << CrateID_ << " new " << crate_id_ << std::endl;
	std::cout << " CFEBClkDelay old " << CfebClkDelay_ << " new " << cfeb_clk_delay_ << std::endl;
	std::cout << " xlatency old " << XLatency_ << " new " << xlatency_ << std::endl;
	std::cout << " xfinelatency old " << XFineLatency_ << " new " << xfinelatency_ << std::endl;
	std::cout << " killinput old " << KillInput_ << " new " << killinput_ << std::endl;
     (*MyOutput_) << "Set crate id " << crate_id_ << std::endl ;
     setcrateid(crate_id_);
     (*MyOutput_) << "Set fine_latency, kill_input, xL1A, cfeb clk delay " <<std::hex<< killflatclk_ <<" in hex"<< std::endl ;
     //     comdelay=((xfinelatency_<<10)&0x3c00)+((killinput_<<7)&0x380)+((xlatency_<<5)&0x60)+(cfeb_clk_delay_&0x1f);
     //     cout<<" GUJH program comdelay: "<<hex <<killflatclk_ <<dec<<endl;
     //     cout<<" xfinedelay: "<<xfinelatency_<<" killinput: "<<killinput_<<" xlatency "<<xlatency_<<" cfeb_clk_dly: "<<cfeb_clk_delay_<<endl;
     setfebdelay(killflatclk_);
     //
     (*MyOutput_) << "Set cable delay " << cable_delay_ << std::endl ;
     setcbldly(cable_delay_);
     //
     // Load FLASH memory
     WriteSFM();
   }
   
   if(hardware_version_==2)
   {
      // set delays
      odmb_set_LCT_L1A_delay(l1acc_dav_delay_);
      odmb_set_TMB_delay(tmb_dav_delay_);
      odmb_set_Push_delay(push_dav_delay_);
      odmb_set_ALCT_delay(ALCT_dav_delay_);
      odmb_set_Inj_delay(inject_delay_);
      odmb_set_Ext_delay(pulse_delay_);
      odmb_set_Cal_delay(calibration_LCT_delay_);
      odmb_set_kill_mask(kill_input_mask_);
      // save configuration to EPROM
      odmb_save_config();
   }

}
//
bool DAQMB::checkDAQMBXMLValues() { 
  //
  std::cout << "DAQMB: checkXMLValues() for crate " << this->crate() << " slot " << this->slot() << std::endl;
  //
  const int max_number_of_reads = 2;
  //
  bool cfebmatch = false;
  number_of_configuration_reads_ = 0;
  //
  while (!cfebmatch && number_of_configuration_reads_ < max_number_of_reads) {
    //
    number_of_configuration_reads_++;
    //
    bool print_errors;
    if (number_of_configuration_reads_ == (max_number_of_reads-1) ) {
      print_errors = true;
      //
    } else {
      print_errors = false;
      //
    }
    //
    cfebmatch=true;
    //
    calctrl_fifomrst();
    //
    // Check what the CFEB configuration status is...
    CheckCFEBsConfiguration(print_errors);
    for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
      int cfeb_index = lfeb + 1;
      cfebmatch &= GetCFEBConfigIsOK(cfeb_index);
    }
    //
    //
    // ***  This part is related to the SFM (Serial Flash Memory) ****
    //
    // Readout the Current setting on DMB
    //
    /*temperary: GUJH    
      comdelay=((xfinelatency_<<10)&0x3c00)+((killinput_<<7)&0x380)+((xlatency_<<5)&0x60)+(cfeb_clk_delay_&0x1f);
      cout<<" GUJH program comdelay: "<<comdelay<<endl;
      cout<<" xfinedelay: "<<xfinelatency_<<" killinput: "<<killinput_<<" xlatency "<<xlatency_<<" cfeb_clk_dly: "<<cfeb_clk_delay_<<endl;
      setfebdelay(comdelay);
    */
    //
    //  cout << "****killflatclk: " << hex << killflatclk_ << " cfebclk: " << cfeb_clk_delay_ << endl;
    //
    char dmbstatus[11];
    dmb_readstatus(dmbstatus);
    //
    //check the DMB setting with the current setup
    cfebmatch &= compareValues("DAQMB CableDelays"    ,CableDelay_  ,cable_delay_   ,print_errors);
    cfebmatch &= compareValues("DAQMB CrateID"        ,CrateID_     ,crate_id_      ,print_errors);
    cfebmatch &= compareValues("DAQMB feb_clock_delay",CfebClkDelay_,cfeb_clk_delay_,print_errors);
    cfebmatch &= compareValues("DAQMB xFineLatency"   ,XFineLatency_,xfinelatency_  ,print_errors);
    cfebmatch &= compareValues("DAQMB kill_input"     ,KillInput_   ,killinput_     ,print_errors);
    cfebmatch &= CheckVMEFirmwareVersion();
    cfebmatch &= CheckControlFirmwareVersion();
    //
    // Check to see if all the configuration parameters are set high...  
    // This is a "smoking gun" that the firmware needs to be reloaded on this DMB...
    dmb_smoking_gun_status_ = true;
    //
    if (compareValues("DAQMB CableDelays" , CableDelay_  , 0x0, false) &&
	compareValues("DAQMB CrateID"     , CrateID_     , 0x0, false) &&
	compareValues("DAQMB xFineLatency", XFineLatency_, 0x0, false) &&
	compareValues("DAQMB kill_input"  , KillInput_   , 0x0, false) ) {
      //
      dmb_smoking_gun_status_ = false; 
      //std::cout << "DAQMB ... Gun is smoking...." << std::endl;
      //
      // Nullify this check if the values are, in fact, **intentionally** set low...
      if ( cable_delay_ == 0x0 && crate_id_ == 0x0 && XFineLatency_ == 0x0 && killinput_ == 0x0 ) {
	dmb_smoking_gun_status_ = true;
      }
    }
    //
    if (compareValues("DAQMB CableDelays" , CableDelay_  , 0xff, false) &&
	compareValues("DAQMB CrateID"     , CrateID_     , 0x7f, false) &&
	compareValues("DAQMB xFineLatency", XFineLatency_,  0xf, false) &&
	compareValues("DAQMB kill_input"  , KillInput_   ,  0x7, false) ) {
      //
      dmb_smoking_gun_status_ = false; 
      //std::cout << "DAQMB ... Gun is smoking...." << std::endl;
      //
      // Nullify this check if the values are, in fact, **intentionally** set high...
      if ( cable_delay_ == 0xff && crate_id_ == 0x7f && XFineLatency_ == 0xf && killinput_ == 0x7 ) {
	dmb_smoking_gun_status_ = true;
      }
    }
    //
  }
  //
  return cfebmatch;
  //
}
//
void DAQMB::CheckCFEBsConfiguration(bool print_errors) {
  //
  cfebs_readstatus();
  //
  // get the initial values...
  int comp_mode_bits = (comp_mode_ & 3) | ((comp_timing_ & 7) << 2);
  //
  killinput_=GetKillInput();
  cfeb_clk_delay_=GetCfebClkDelay();
  xfinelatency_=GetxFineLatency();
  xlatency_=GetxLatency();
  //
  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
    cfeb_config_status_[lfeb] = true;
    smoking_gun_status_[lfeb] = true;
  }
  // Buck_Flash check...
  //
  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
    //    for (int y=0; y<4; y++) printf("%2x \n",(febstat_[lfeb][y]&0xff));
    //    std::cout << "<>" << comp_mode_bits << " " << pre_block_end_ << " " << xlatency_ << std::endl;
    //
    int comp_mode_bits_old = febstat_[lfeb][2]&0x1f;
    int pre_block_end_old  = (((febstat_[lfeb][2]>>5)&0x07)+((febstat_[lfeb][3]&0x01)<<3));
    int xlatency_old       = ((febstat_[lfeb][3]>>1)&0x03);
    //    
    std::ostringstream tested_value1;
    tested_value1 << "CFEB " << (lfeb+1) << " comp_mode_bits";
    cfeb_config_status_[lfeb] &= compareValues(tested_value1.str(), comp_mode_bits_old, comp_mode_bits, print_errors);
    //
    std::ostringstream tested_value2;
    tested_value2 << "CFEB " << (lfeb+1) << " pre_block_end";
    cfeb_config_status_[lfeb] &= compareValues(tested_value2.str(), pre_block_end_old, pre_block_end_, print_errors);
    //
    std::ostringstream tested_value3;
    tested_value3 << "CFEB " << (lfeb+1) << " xLatency";
    cfeb_config_status_[lfeb] &= compareValues(tested_value3.str(), xlatency_old, xlatency_, print_errors);
    //
    // Check to see if all the configuration parameters are set high...
    if (compareValues(tested_value1.str(), comp_mode_bits_old, 0x1f, false) &&
	compareValues(tested_value2.str(), pre_block_end_old ,  0xf, false) &&
	compareValues(tested_value3.str(), xlatency_old      ,  0x3, false) ) {
      //
      smoking_gun_status_[lfeb] = false; 
      //std::cout << "CFEB " << lfeb+1 << " ... Gun is smoking...." << std::endl;
      //
      // Nullify this check if the values are, in fact, **intentionally** set high...
      if ( comp_mode_bits == 0x1f && pre_block_end_ == 0xf && xlatency_ == 0x3 ) {
	smoking_gun_status_[lfeb] = true;
      }
    }
  }
  //
  //
  //check the comp_dac setting...
  //
  // The reference threshold (3.590V) is the most uncertain number.  
  // Therefore, we set a threshold on this comparison to check for gross errors
  //
  const float comparison_threshold = 100;
  //
  float compthresh[5];
  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++)  compthresh[lfeb]=adcplus(2,lfeb);
  //
  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
    //
    //as the monitor show that CFEB +5V is only 4.9V, the 3550 is used instead of 3590
    float read_threshold_in_mV = (3550. - compthresh[lfeb]);
    float set_threshold_in_mV  = (set_comp_thresh_ * 1000.);
    //
    std::ostringstream tested_value;
    tested_value << "CFEB " << (lfeb+1) << " set_comp_thresh";
    cfeb_config_status_[lfeb] &= compareValues(tested_value.str(),read_threshold_in_mV,set_threshold_in_mV,comparison_threshold, print_errors);
  }
  //
  //  greg, put in the CFEB firmware version here
  //
  return;
}
//
void DAQMB::enable_cfeb() {
  //
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_STR;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  int cfebs=0;
  //
  for(unsigned i = 0; i < cfebs_.size(); ++i) {
    cfebs += (int) pow(2.0, cfebs_[i].number()); 
  }
  std::cout << "Trigger CFEBs set to = " << cfebs << std::endl;
  sndbuf[0]=cfebs;
  devdo(MCTRL,6,cmd,5,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  }
}
//
void DAQMB::setcrateid(int dword)
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CRATE_ID;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  devdo(MCTRL,6,cmd,7,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  // Update
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=22;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  }
  else if(hardware_version_==2)
  {
     WriteRegister(ODMB_CRATEID, CRATE_ID);
  }
}
//
void DAQMB::setfebdelay(int dword)
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FEB_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF;
  sndbuf[1]=(dword>>8)&0x3f; 
  devdo(MCTRL,6,cmd,14,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FEB_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << " GUJH setfebdelay to " << dword << std::endl;
  std::cout<< " GUJH setfebdelay to " << dword << std::endl;
  //
  // Update
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=23;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  }
  else if(hardware_version_==2)
  {
  } 
}

void DAQMB::setcaldelay(int dword)
{
  //
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  devdo(MCTRL,6,cmd,19,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2); 
  //(*MyOutput_) << "caldelay was set to " << std::hex << dword <<std::dec << std::endl;
  }
  else if(hardware_version_==2)
  {
     WriteRegister(CAL_DLY, dword);
  }
  //
}

void DAQMB::setdavdelay(int dword)
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=TRG_DAV_DELAY;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  sndbuf[1]=(dword>>8)&0xFF;
  sndbuf[2]=(dword>>16)&0xFF;
  sndbuf[3]=(dword>>24)&0x03;
  devdo(MCTRL,6,cmd,26,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  (*MyOutput_) << "set dav delay to " << dword << std::endl;
  }
  else if(hardware_version_==2)
  {
  } 
}

void DAQMB::fxpreblkend(int dword)
{
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) 
  {
  int hversion=cfebs_[icfeb].GetHardwareVersion();
  if(hversion<=1)
  {
    DEVTYPE dv = cfebs_[icfeb].scamDevice();
    std::cout << "Setting dv= " << dv << " to " << std::dec << dword << std::endl;
    cmd[0]=VTX_USR1;
    sndbuf[0]=PREBLKEND;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    //  default preblkend is state 5
    sndbuf[0]=dword&0x0F; 
    devdo(dv,5,cmd,4,sndbuf,rcvbuf,0);
    cmd[0]=VTX_BYPASS;
    sndbuf[0]=0;
    devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);
  }
  else if (hversion==2)
  {
      dcfeb_hub(cfebs_[icfeb], PREBLKEND, 4, &dword, rcvbuf, NOW);
  }  
  }
}

void DAQMB::LctL1aDelay(int dword) // Set cfeb latency (0=2.9us,1=3.3us,2=3.7us,3=4.1us)
{
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
    LctL1aDelay(dword,icfeb);
  }
}

void DAQMB::LctL1aDelay(int dword,unsigned icfeb) // Set cfeb latency (0=2.9us,1=3.3us,2=3.7us,3=4.1us)
{
  int hversion=cfebs_[icfeb].GetHardwareVersion();
  if(hversion<=1)
  {
  DEVTYPE dv = cfebs_[icfeb].scamDevice();
  cmd[0]=VTX_USR1;
  sndbuf[0]=LCTL1ADELAY;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR2;
  // 
  sndbuf[0]=dword&0x03; 
  devdo(dv,5,cmd,2,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);
  }
  else if (hversion==2)
  {
      dcfeb_hub(cfebs_[icfeb], LCTL1ADELAY, 2, &dword, rcvbuf, NOW);
      udelay(1000);
  }  
}


void DAQMB::calctrl_fifomrst()
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_FIFOMRST;
  std::cout << " CAL_FIFOMRST " << std::hex << (sndbuf[0]&0xff) << std::dec << std::endl;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  for(int i=0;i<1000000;i++); /* wait 15 msec (1 loop = ~15ns) */
  /*  sleep(1); 
  set user mode 1 and put LCA into normal mode
*/
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NORM_MODE;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  std::cout << " FIFOMRST reset done " << std::endl;
  }
}

void DAQMB::calctrl_global()
{

  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=GLOBAL_RST;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  usleep(10000);
  }
}


void DAQMB::restoreCFEBIdle() {
// Liu Oct. 1, 2012
// Set all CFEB's JTAG state machines
   char cfeb_maskX = (hardware_version_<=1)?0x1f:0x7f;
   write_cfeb_selector(cfeb_maskX); 
   WriteRegister(reset_CFEB_JTAG, 0);
}


void DAQMB::restoreMotherboardIdle() {
  if(hardware_version_<=1)
  {
    devdo(MCTRL,-1,cmd,0,sndbuf,rcvbuf,2);
  }
  else if(hardware_version_==2)
  {
    dlog_do(-1, NULL, 0, NULL, NULL, 0);
    daqmb_do(-1, NULL, 0, NULL, NULL, 0, 2);
  }
}

/* DAQMB trigger primitives */

void  DAQMB::set_comp_mode(int dword)
{
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) 
  {
    int hversion=cfebs_[icfeb].GetHardwareVersion();
    if(hversion<=1)
    {
      DEVTYPE dv = cfebs_[icfeb].scamDevice();
      
      cmd[0]=VTX_USR1;
      sndbuf[0]=COMP_MODE;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      // default is 0x17
      sndbuf[0]=dword&0XFF;
      devdo(dv,5,cmd,5,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      sndbuf[0]=0;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
    }
    else if (hversion==2)
    {
      dcfeb_hub(cfebs_[icfeb], COMP_MODE, 5, &dword, rcvbuf, NOW);
      udelay(100);
    }  
  }
  (*MyOutput_) << "calling set_comp_mode " << std::hex << (dword & 0x1F) << std::dec << std::endl;
}

void DAQMB::set_comp_thresh_bc(float thresh)
{
 // Liu, Sept. 2012
 // This is temporary solution to avoid the broadcast for CFEBs going to DCFEBs 
 // if CFEBs and DCFEBs mixed in one DMB, it won't work
 // TODO......
 int hversion=cfebs_[0].GetHardwareVersion();
 if(hversion<=1)
 {            
   char dt[2];
   // 
   /* digitize voltages */
   // 
   int dthresh=int(4095*((3.5-thresh)/3.5)); 
   dt[0]=0;
   dt[1]=0;
   for(int i=0;i<8;i++){
     dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
     dt[1]|=((dthresh>>i)&1)<<(6-i);
   }
   dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
   dt[1]=dt[1]>>1;
   //
   DEVTYPE dv = FASCAM;
   cmd[0]=VTX_USR1;
   sndbuf[0]=COMP_DAC;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR2;
   sndbuf[0]=dt[0];
   sndbuf[1]=dt[1];
   sndbuf[2]=0x00; 
   devdo(dv,5,cmd,15,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR1;
   sndbuf[0]=NOOP;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_BYPASS;
   devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
   usleep(20);
 }
 else if (hversion==2)
 {
   dcfeb_set_comp_thresh_bc(thresh);
   udelay(100);
 }
}

void DAQMB::dcfeb_set_comp_thresh_bc(float thresh)
{
   char dt[2];
   // 
   /* digitize voltages */
   // 
   int dthresh=int(4095*((3.5-thresh)/3.5)); 
   dt[0]=0;
   dt[1]=0;
   for(int i=0;i<8;i++){
     dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
     dt[1]|=((dthresh>>i)&1)<<(6-i);
   }
   dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
   dt[1]=dt[1]>>1;
   //
   // enable all DCFEBs for broadcast
   write_cfeb_selector(0x7F);
   dcfeb_core(COMP_DAC, 15, dt, rcvbuf, NOW|NOOP_YES);
   usleep(200);
}

void DAQMB::set_comp_thresh(float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 // (*MyOutput_) << "CFEB size="<<cfebs_.size() << std::endl;
 //
 // for(unsigned int i=0; i<cfebs_.size();i++) {
 //   (*MyOutput_) << i << " CFEB number" << cfebs_[i].number() << std::endl;
 // }
 //
 for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) 
 {
  int hversion=cfebs_[icfeb].GetHardwareVersion();
  if(hversion<=1)
  {
   DEVTYPE dv = cfebs_[icfeb].scamDevice();
   cmd[0]=VTX_USR1;
   sndbuf[0]=COMP_DAC;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR2;
   sndbuf[0]=dt[0];
   sndbuf[1]=dt[1];
   sndbuf[2]=0x00; 
   devdo(dv,5,cmd,15,sndbuf,rcvbuf,0);
   cmd[0]=VTX_USR1;
   sndbuf[0]=NOOP;
   devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
   cmd[0]=VTX_BYPASS;
   devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
  else if (hversion==2)
  {
   dcfeb_hub(cfebs_[icfeb], COMP_DAC, 15, dt, rcvbuf, NOW|NOOP_YES);
  }          
   usleep(200);
 }
}
//
void DAQMB::set_comp_thresh(int icfeb, float thresh)
{
char dt[2];
// 
/* digitize voltages */
// 
 int dthresh=int(4095*((3.5-thresh)/3.5)); 
 dt[0]=0;
 dt[1]=0;
 for(int i=0;i<8;i++){
   dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
   dt[1]|=((dthresh>>i)&1)<<(6-i);
 }
 dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
 dt[1]=dt[1]>>1;
 //
 (*MyOutput_) << "Set_comp_thresh.icfeb=" << cfebs_[icfeb].number() << " thresh=" << thresh << std::endl;
 //
  int hversion=cfebs_[icfeb].GetHardwareVersion();
  if(hversion<=1)
  {
    DEVTYPE dv = cfebs_[icfeb].scamDevice();
    //
    //(*MyOutput_) << "cfeb= " << icfeb << std::endl;
    //(*MyOutput_) << "dv= " << dv << std::endl;
    //
    cmd[0]=VTX_USR1;
    sndbuf[0]=COMP_DAC;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    sndbuf[0]=dt[0];
    sndbuf[1]=dt[1];
    sndbuf[2]=0x00; 
    devdo(dv,5,cmd,15,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR1;
    sndbuf[0]=NOOP;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_BYPASS;
    devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
  else if (hversion==2)
  {
    dcfeb_hub(cfebs_[icfeb], COMP_DAC, 15, dt, rcvbuf, NOW|NOOP_YES);
  }          
 usleep(200);
}

// TODO: remove set_dac() and use only set_cal_dac()
//
void DAQMB::set_dac(float volt0,float volt1)
{
   set_cal_dac(volt0, volt1);
}


void DAQMB::halfset(int icrd,int ipln,int ihalf,int chan[7][6][16])
{

   int ichan,iside;

   //(*MyOutput_) << "DAQMB.halfset " << std::endl;
   
   if (ihalf<0 && icrd-1>=0) {
      ihalf += 32;
      icrd  -= 1;
   }

   //if(ihalf>=0&&ihalf<32){
      ichan=ihalf/2;
      iside=ihalf-2*ichan;
      if(iside==0){
	 if(ichan-1>=0)chan[icrd][ipln][ichan-1]=MEDIUM_CAP;
	 if(ichan-1<0&&icrd-1>=0)chan[icrd-1][ipln][15]=MEDIUM_CAP;	 
	 if(ichan>=0&&ichan<=15)chan[icrd][ipln][ichan]=LARGE_CAP;	 
	 if(ichan+1<=15)chan[icrd][ipln][ichan+1]=SMALL_CAP;
	 if(ichan+1>15&&icrd+1<7)chan[icrd+1][ipln][0]=SMALL_CAP;
      }
      if(iside==1){
	 if(ichan-1>=0)chan[icrd][ipln][ichan-1]=SMALL_CAP;
	 if(ichan-1<0&&icrd-1>=0)chan[icrd-1][ipln][15]=SMALL_CAP;	 
	 if(ichan>=0&&ichan<=15)chan[icrd][ipln][ichan]=LARGE_CAP;	 
	 if(ichan+1<=15)chan[icrd][ipln][ichan+1]=MEDIUM_CAP;
	 if(ichan+1>15&&icrd+1<7)chan[icrd+1][ipln][0]=MEDIUM_CAP;
      }
      //}
      //else {
      //printf("Half strip out of range: %d (must be between 0<=hs<=31)\n",ihalf);
      //}
}


void  DAQMB::halfset(int ifeb,int ipln,int ihalf)
{
  if(ihalf>=0&&ihalf<32){
    int ichan=ihalf/2;
    int iside=ihalf-2*ichan;
    if(iside==0){
      if(ichan-1>=0) shift_array[ifeb][ipln][ichan-1]=MEDIUM_CAP;
      if(ichan-1<0&&ifeb-1>=0) shift_array[ifeb-1][ipln][15]=MEDIUM_CAP;

      if(ichan>=0&&ichan<=15) shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) shift_array[ifeb][ipln][ichan+1]=SMALL_CAP;
      if(ichan+1>15&&ifeb+1<7) shift_array[ifeb+1][ipln][0]=SMALL_CAP;
    }
    if(iside==1){
      if(ichan-1>=0) shift_array[ifeb][ipln][ichan-1]=SMALL_CAP;
      if(ichan-1<0&&ifeb-1>=0) shift_array[ifeb-1][ipln][15]=SMALL_CAP;

      if(ichan>=0&&ichan<=15) shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) shift_array[ifeb][ipln][ichan+1]=MEDIUM_CAP;
      if(ichan+1>15&&ifeb+1<7) shift_array[ifeb+1][ipln][0]=MEDIUM_CAP;
    }
  }

}

//
void DAQMB::trigsetx(int *hp, int CFEBInput)
{
  //
  //  (*MyOutput_) << "DAQMB.trigsetx" << std::endl;
  //  (*MyOutput_) << "CFEB size="<<cfebs_.size() << std::endl;;
  //
  int hs[6];
  int i,j,k;
  int chan[7][6][16];
  for(i=0;i<7;i++){
    for(j=0;j<6;j++){
      for(k=0;k<16;k++){
        chan[i][j][k]=NORM_RUN;
      }
    }
  }
  //
  //  (*MyOutput_) << "Setting Staggering" << std::endl;
  //
  for(i=0;i<6;i+=2){
     hs[i]=-1;
     hs[i+1]=0;
  } 
  //
  //  (*MyOutput_) << " Setting halfset " << std::endl;
  //
  for(k=0;k<5;k++){
     for(j=0;j<6;j++){
       if ( (CFEBInput>>k)&0x1 ) halfset(k,j,hp[j]+hs[j],chan);
     }
  }
  chan2shift(chan);
}

//broadcast buckeye shift on DMB/CFEB level for external channel
void DAQMB::buck_shift_ext_bc(int nstrip)
{
 // Liu, Sept. 2012
 // This is temporary solution to avoid the broadcast for CFEBs going to DCFEBs 
 // if CFEBs and DCFEBs mixed in one DMB, it won't work
 // TODO......
 int hversion=cfebs_[0].GetHardwareVersion();
 if(hversion<=1)
 {            
  char shft_bits[6]={0,0,0,0,0,0};

  //external cap mode: 100
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3+2;
  if ((nstrip >= 0) && (nstrip < 16)) shft_bits[i/8]=((1<<(i%8))&0xff);

  //enable all CFEBs, and set the CFEB into broadcast mode
  DEVTYPE dv = FASCAM;
  cmd[0]=VTX_USR1;
  sndbuf[0]=CHIP_MASK;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  // (*MyOutput_)<<" first devdo call \n";
  cmd[0]=VTX_USR2;
  sndbuf[0]=0;    //set the chip_in_use to 0 will enable broadcast
  devdo(dv,5,cmd,6,sndbuf,rcvbuf,0);
  //shift in 48 bits for the whole DMB ( 1/30 of non-broadcast )
  cmd[0]=VTX_USR1;
  sndbuf[0]=CHIP_SHFT;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR2;
  devdo(dv,5,cmd,48,shft_bits,rcvbuf,0);
  //set the function into NOOP, and bypass
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);

  ::usleep(200);
 }
 else if (hversion==2)
 {
   dcfeb_buck_shift_ext_bc(nstrip);
 }
}

//broadcast buckeye shift on DMB/DCFEB level for external channel
void DAQMB::dcfeb_buck_shift_ext_bc(int nstrip)
{
  char shft_bits[6]={0,0,0,0,0,0};

  //external cap mode: 100
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3+2;
  if ((nstrip >= 0) && (nstrip < 16)) shft_bits[i/8]=((1<<(i%8))&0xff);

  // enable all DCFEBs for broadcast
  write_cfeb_selector(0x7F);
  sndbuf[0]=0;    //set the mask to 0 will enable broadcast
  dcfeb_core(CHIP_MASK, 6, sndbuf, rcvbuf, NOW|NO_BYPASS);
  dcfeb_core(CHIP_SHFT, 48,shft_bits, rcvbuf, NOW|NOOP_YES);
  ::usleep(200);
}

//broadcast buckeye shift on DMB/CFEB level for comparator
void DAQMB::buck_shift_comp_bc(int nstrip)
{
 // Liu, Sept. 2012
 // This is temporary solution to avoid the broadcast for CFEBs going to DCFEBs 
 // if CFEBs and DCFEBs mixed in one DMB, it won't work
 // TODO......
 int hversion=cfebs_[0].GetHardwareVersion();
 if(hversion<=1)
 {            
  char shft_bitsa[6]={0,0,0,0,0,0};
  char shft_bitsb[6]={0,0,0,0,0,0};

  //small_cap  =1
  //medium_cap =2
  //large_cap  =3
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3;
  long long patterna=0x99; //132 for layer 1,3,5 (channel 15 first)
  long long patternb=0x5a; //231 for layer 2,4,6 (channel 15 first)
  if ((nstrip >= 0) && (nstrip < 16)) {
    long long pattern=0;
    pattern=(patterna<<i)>>3;
    if (i==0) pattern=pattern|(patterna<<45);
    if (i==45) pattern=pattern|(patterna>>6);
    for (int j=0;j<6;j++) {
      shft_bitsa[j]=(pattern>>(j*8))&0xff;
    }
    pattern=0;
    pattern=(patternb<<i)>>3;
    if (i==0) pattern=pattern|(patternb<<45);
    if (i==45) pattern=pattern|(patternb>>6);
    for (int j=0;j<6;j++) {
      shft_bitsb[j]=(pattern>>(j*8))&0xff;
    }
  }

  //enable all CFEBs, and set the CFEB into broadcast mode
  DEVTYPE dv = FASCAM;
  cmd[0]=VTX_USR1;
  sndbuf[0]=CHIP_MASK;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  // (*MyOutput_)<<" first devdo call \n";
  cmd[0]=VTX_USR2;
  sndbuf[0]=0x3f;    //set the chip_in_use to 111111 as the patterns are different for different buckeyes
  devdo(dv,5,cmd,6,sndbuf,rcvbuf,0);
  //shift in 48*6 bits for the whole DMB ( 1/5 of non-broadcast )
  cmd[0]=VTX_USR1;
  sndbuf[0]=CHIP_SHFT;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR2;
  for (int j=0;j<18;j++) {
    sndbuf[j]=(shft_bitsb[j%6])&0xff;
  }
  for (int j=18;j<36;j++) {
    sndbuf[j]=(shft_bitsa[j%6])&0xff;
  }
  devdo(dv,5,cmd,288,sndbuf,rcvbuf,0);
  //set the function into NOOP, and bypass
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);

  ::usleep(200);
 }
 else if (hversion==2)
 {
   dcfeb_buck_shift_comp_bc(nstrip);
 }
}

//broadcast buckeye shift on DMB/DCFEB level for comparator
void DAQMB::dcfeb_buck_shift_comp_bc(int nstrip)
{
  char shft_bitsa[6]={0,0,0,0,0,0};
  char shft_bitsb[6]={0,0,0,0,0,0};

  //small_cap  =1
  //medium_cap =2
  //large_cap  =3
  //When shift, it shift strip 1 first, and low bit first
  int i=(15-nstrip)*3;
  long long patterna=0x99; //132 for layer 1,3,5 (channel 15 first)
  long long patternb=0x5a; //231 for layer 2,4,6 (channel 15 first)
  if ((nstrip >= 0) && (nstrip < 16)) {
    long long pattern=0;
    pattern=(patterna<<i)>>3;
    if (i==0) pattern=pattern|(patterna<<45);
    if (i==45) pattern=pattern|(patterna>>6);
    for (int j=0;j<6;j++) {
      shft_bitsa[j]=(pattern>>(j*8))&0xff;
    }
    pattern=0;
    pattern=(patternb<<i)>>3;
    if (i==0) pattern=pattern|(patternb<<45);
    if (i==45) pattern=pattern|(patternb>>6);
    for (int j=0;j<6;j++) {
      shft_bitsb[j]=(pattern>>(j*8))&0xff;
    }
  }

  // enable all DCFEBs for broadcast
  write_cfeb_selector(0x7F);
  sndbuf[0]=0x3F;    //set the mask to 111111 as the patterns are different for different buckeyes
  dcfeb_core(CHIP_MASK, 6, sndbuf, rcvbuf, NOW|NO_BYPASS);
  //shift in 48*6 bits
  for (int j=0;j<18;j++) {
    sndbuf[j]=(shft_bitsb[j%6])&0xff;
  }
  for (int j=18;j<36;j++) {
    sndbuf[j]=(shft_bitsa[j%6])&0xff;
  }
  dcfeb_core(CHIP_SHFT, 288, sndbuf, rcvbuf, NOW|NOOP_YES);

  ::usleep(200);
}

void DAQMB::chan2shift(int chan[5][6][16])
{
   
   int i,j;
   int chip,lay,nchips;
   char chip_mask;
   char shft_bits[6][6];
   
   //   (*MyOutput_) << "CFEB size=" << cfebs_.size() << std::endl;
   //   (*MyOutput_) << "CFEB numbers" << std::endl;
   //
   //   for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) (*MyOutput_) << " " << cfebs_[icfeb].number() ;
   //
   //   (*MyOutput_) << std::endl;
   
   for(lay=0;lay<6;lay++){
      for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
	 int brdn = cfebs_[icfeb].number();
	 for(int i=0; i<16;i++) {
	   // if ( chan[brdn][lay][i] > 0 ) printf("%c", '\033');
	    (*MyOutput_) << chan[brdn][lay][i] << "" ;
	    //  printf("%c", '\033'); 
	 }
	 (*MyOutput_) << " | " ;		
      }
      (*MyOutput_) << std::endl;
   }
   
   for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
      DEVTYPE dv   = cfebs_[icfeb].scamDevice() ;
      int brdn = cfebs_[icfeb].number();
      //      printf(" brdn=%d F1SCAM=%d dv=%d\n",brdn,F1SCAM,dv);
      for(lay=0;lay<6;lay++){
	 shft_bits[lay][0]=((chan[brdn][lay][13]<<6)|(chan[brdn][lay][14]<<3)|chan[brdn][lay][15])&0XFF;
	 shft_bits[lay][1]=((chan[brdn][lay][10]<<7)|(chan[brdn][lay][11]<<4)|(chan[brdn][lay][12]<<1)|(chan[brdn][lay][13]>>2))&0XFF;
	 shft_bits[lay][2]=((chan[brdn][lay][8]<<5)|(chan[brdn][lay][9]<<2)|(chan[brdn][lay][10]>>1))&0XFF;
	 shft_bits[lay][3]=((chan[brdn][lay][5]<<6)|(chan[brdn][lay][6]<<3)|chan[brdn][lay][7])&0XFF;
	 shft_bits[lay][4]=((chan[brdn][lay][2]<<7)|(chan[brdn][lay][3]<<4)|(chan[brdn][lay][4]<<1)|(chan[brdn][lay][5]>>2))&0XFF;
	 shft_bits[lay][5]=((chan[brdn][lay][0]<<5)|(chan[brdn][lay][1]<<2)|(chan[brdn][lay][2]>>1))&0XFF;
     }
      chip_mask=0;
      for(i=0;i<6;i++){
	 chip_mask=chip_mask|(1<<(5-i));
      }
 // Liu, Sept. 2012
 // This is temporary solution to avoid the broadcast for CFEBs going to DCFEBs 
 // if CFEBs and DCFEBs mixed in one DMB, it won't work
 // TODO......
      int hversion=cfebs_[icfeb].GetHardwareVersion();
      if(hversion<=1)
      {            
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      nchips=6;
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(chip=5;chip>=0;chip--){
	 lay=layers[chip];
	 for(i=0;i<6;i++){
	    sndbuf[j*6+i]=shft_bits[lay][i];
	 }
	 j++;
      }
      devdo(dv,5,cmd,nchips*48,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      }
      else if (hversion==2)
      {
         write_cfeb_selector(cfebs_[icfeb].SelectorBit());
         BuckeyeShift((int)chip_mask, shft_bits);
      }
   }
}


void DAQMB::trigtest()
{
  int i,k;
  int hp[6],ho[]={-1,0,1,2};
  int imid,hits;
  int ip0,ip1,ip2,ip3,ip4,ip5;
  int p0hit,p1hit,p2hit,p3hit,p4hit,p5hit;
  //
  // Set the comparator threshold for all
  //
  set_comp_thresh(.05);
  //
  for(i=0;i<5;i++){
      set_dac(.5,0);
      imid=16;
      ho[3]=-imid-1;
      p0hit=1;
      for(ip0=0;ip0<4;ip0++){
        hp[0]=imid+ho[ip0];
        if(ip0==3)p0hit=0;
        p1hit=1;
        for(ip1=0;ip1<4;ip1++){
          hp[1]=imid+ho[ip1];
          if(ip1==3)p1hit=0;
          p2hit=1;
          for(ip2=1;ip2<4;ip2+=2){
            hp[2]=imid+ho[ip2];
            if(ip2==3)p2hit=0;
            p3hit=1;
            for(ip3=0;ip3<4;ip3++){
              hp[3]=imid+ho[ip3];
              if(ip3==3)p3hit=0;
              p4hit=1;
              for(ip4=0;ip4<4;ip4++){
                hp[4]=imid+ho[ip4];
                if(ip4==3)p4hit=0;
                p5hit=1;
                for(ip5=0;ip5<4;ip5++){
                  hp[5]=imid+ho[ip5];
                  if(ip5==3)p5hit=0;
                  hits=p0hit+p1hit+p2hit+p3hit+p4hit+p5hit;
                  (*MyOutput_) << "Pattern :" << std::endl ;
                  for(k=0;k<6;k++){
		    printf(" %d ",hp[k]);
                  }
                  (*MyOutput_) << std::endl;
                  (*MyOutput_) << "Hits set = " << hits << std::endl;
                  trigsetx(hp);
                  inject(1,0x4f);
		  (*MyOutput_) << " Next ?"<< std::endl;
		  int next;
		  std::cin >> next;
                }
              }
            }
          }
        }
      }
  }
}

/*DAQMB set trigger source */

//extern char cmd[];
//extern char sndbuf[];
//extern char rcvbuf[];

void DAQMB::settrgsrc(int dword)
{
  //
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=37;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  //
  //devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
  //printf("Cal_trg source was Set to %01x (Hex). \n",unpack_ival());
  //
  sndbuf[0]=dword&0x0F; 
  devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  printf("Cal_trg source are Set to %01x (Hex). \n",dword&0xf);
  //
  }
}


/* DAQMB   Voltages  */

float DAQMB::adcplus(int ichp,int ichn){
  if(hardware_version_<=1)
  {
  unsigned int ival= (readADC(ichp, ichn)&0x0fff);
  return (float) ival;
  }
  else return 0.;
}

//
float DAQMB::adcminus(int ichp,int ichn){
  if(hardware_version_<=1)
  {
  short int ival= (readADC(ichp, ichn)&0x0fff);
  if((0x0800&ival)==0x0800)ival=ival|0xf000;
  float cval;
  cval = ival;
  return (float) ival;
  }
  else return 0.;
}

//
float DAQMB::adc16(int ichp,int ichn){
  if(hardware_version_<=1)
  {
  unsigned int ival= readADC(ichp, ichn);
  float cval=ival*4.999924/65535.;
  return cval;
  }
  else return 0.;
}


void DAQMB::dmb_readstatus(char status[11])
{
  if(hardware_version_<=1)
  {
  //
  int i;
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=10;    //F10 read DMB6CNTL status
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_USR2;
  for (i=0;i<11;i++)  {rcvbuf[i]=0;sndbuf[i]=0;}
  //
  devdo(MCTRL,6,cmd,88,sndbuf,rcvbuf,1);
  for(i=0;i<11;i++)status[i]=rcvbuf[i];
  for (i=0;i<11;i++)
    {printf(" i= %d, rcvbuf[i]= %02x, status[i]= %02x \n",i,rcvbuf[i],status[i]);}

  //
  /* DMB6CNTL status: bit[14:7]: L1A buffer length
                      bit[19:15]: CFEB_DAV_ERROR
                      bit[26:20]: FIFO_EMPTY: 1 means empty
                      bit[33:27]: FIFO_Full: 1 means full
                      bit[40:34]: FIFO_HF: 1 means less than half_full
                      bit[47:41]: FIFO_PAE: 1 means more than PAE words
  */
  i=((rcvbuf[1]<<1)&0xfe)+((rcvbuf[0]>>7)&0x01); 
  printf(" L1A buffer length: %02x",i); printf("h \n");
  i=((rcvbuf[2]<<1)&0x1e)+((rcvbuf[1]>>7)&0x01); 
  printf(" DAV Error: %02x",i); printf("h \n");
  i=((rcvbuf[3]<<4)&0x70)+((rcvbuf[2]>>4)&0x0f); 
  printf(" FIFO Empty: %02x",i); printf("h   1 means empty \n");
  i=((rcvbuf[4]<<5)&0x60)+((rcvbuf[3]>>3)&0x1f); 
  printf(" FIFO Full: %02x",i); printf("h   1 means full\n");
  i=((rcvbuf[5]<<6)&0x40)+((rcvbuf[4]>>2)&0x03f); 
  printf(" FIFO half_full: %02x",i); printf("h 1 means less than half_full\n");
  i=(rcvbuf[5]>>1)&0x7f; 
  printf(" FIFO PAE: %03x",i); printf("h  1 means more than PAE words\n");
  printf(" GUs new 32 bits: %02x%02x%02x%02x\n",rcvbuf[9]&0xff,rcvbuf[8]&0xff,rcvbuf[7]&0xff,rcvbuf[6]&0xff);

  CableDelay_ =((rcvbuf[6]>>2)&0x3f)+((rcvbuf[7]<<6)&0xc0); 
  printf(" Cable_Delay: %02xh \n",CableDelay_);
  CrateID_=((rcvbuf[7]>>2)&0x3f)+((rcvbuf[8]<<6)&0x40); 
  printf(" Crate ID: %02xh \n",CrateID_);
  CfebClkDelay_=((rcvbuf[8]>>1)&0x1f); 
  printf(" FEB_Delay: %02xh \n",CfebClkDelay_);
  XLatency_=((rcvbuf[8]>>6)&0x03); 
  printf(" Extra L1A latency: %02xh \n",XLatency_);
  XFineLatency_=((rcvbuf[9])&0x0f);
  printf(" Extra Fine L1A latency: %02xh \n",XFineLatency_);
  KillInput_=((rcvbuf[9]>>4)&0x07); 
  printf(" KillInput: %02xh \n",KillInput_);

  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  }
}

void DAQMB::cfebs_readstatus()
{
 int i,nwrds;
 char febbuf[5][4];
 //int iuse[5]={0,0,0,0,0};

 //fxpreblkend(13);

  std::cout << "DAQMB: cfebs_readstatus" << std::endl;
  for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) {
 // Liu, Sept. 2012
 // disabled for DCFEBs for now 
 // TODO......
    int hversion=cfebs_[icfeb].GetHardwareVersion();
    if(hversion<=1)
    {            
      DEVTYPE dv = cfebs_[icfeb].scamDevice();
      int idv=(int)(dv-F1SCAM); 
      //iuse[idv]=1;
      febbuf[idv][0]='\0';
      febbuf[idv][1]='\0';
      febbuf[idv][2]='\0';
      febbuf[idv][3]='\0';
    //if(iuse[idv]==1){
      std::cout << " dv= " << dv << " STATUS_S " << STATUS_S << std::endl;
      cmd[0]=VTX_USR1;
      sndbuf[0]=STATUS_S;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      //      cmd[0]=VTX2_BYPASS;
      //      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      sndbuf[0]=0xd5;
      sndbuf[1]=0xee;
      sndbuf[2]=0xdf;
      sndbuf[3]=0xba;
      devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
      //
      febbuf[idv][0]=rcvbuf[0];
      febbuf[idv][1]=rcvbuf[1];
      febbuf[idv][2]=rcvbuf[2];
      febbuf[idv][3]=rcvbuf[3];
      //
      febstat_[idv][0]=rcvbuf[0]&0xff;
      febstat_[idv][1]=rcvbuf[1]&0xff;
      febstat_[idv][2]=rcvbuf[2]&0xff;
      febstat_[idv][3]=rcvbuf[3]&0xff;
      //
      printf(" SCA rcvbuf *** %02x %02x %02x %02x \n",rcvbuf[0]&0xFF,rcvbuf[1]&0xFF,rcvbuf[2]&0xFF,rcvbuf[3]&0xFF);

      // The following should return "BADFEED5"
      /*      cmd[0]=VTX2_USR1;
      sndbuf[0]=5-STATUS_CS;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX2_USR2;
      sndbuf[0]=0xd5;
      sndbuf[1]=0xee;
      sndbuf[2]=0xdf;
      sndbuf[3]=0xba;
      devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
      printf(" SCA rcvbuf shift *** %02x %02x %02x %02x \n",rcvbuf[3]&0xFF,rcvbuf[2]&0xFF,rcvbuf[1]&0xFF,rcvbuf[0]&0xFF);    */

      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      //}
    }
    else if(hversion==2)
    {
       unsigned st = dcfeb_read_status(cfebs_[icfeb]);
       memcpy(&febstat_[icfeb][0],&st, 4);
    }
  }
  printf("Boards in use              ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",i+1);
      //}
  }
  printf("\n");
  printf("# Words in LCT FIFO        ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      nwrds=febbuf[i][0]&0xF;
      printf("  : %2.2d",nwrds);
      //}
  }
  printf("\n");
  printf("LCT FIFO Empty             ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(~febbuf[i][1]>>2)&1);
      //}
  }
  printf("\n");
  printf("LCT FIFO full              ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][0]>>4)&1);
      //}
  }
  printf("\n");
  printf("# Words in L1Acc FIFO      ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      nwrds=febbuf[i][0]>>5&0x7;
      nwrds=nwrds|(febbuf[i][1]&1)<<3;
      printf("  : %2.2d",nwrds);
      //}
  }
  printf("\n");
  printf("L1Acc FIFO Empty           ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][1]>>3)&1);
      //}
  }
  printf("\n");
  printf("L1Acc FIFO Full            ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",febbuf[i][1]>>1&1);
      //}
  }
  printf("\n");
  printf("Pop FIFO1                  ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][1]>>4)&1);
      //}
  }
  printf("\n");
  printf("Push to CPLD               ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][1]>>5)&1);
      //}
  }
  printf("\n");
  printf("SCA Overwrite              ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][1]>>6)&1);
      //}
  }
  printf("\n");
  printf("Busy (taking data)         ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][1]>>7)&1);
      //}
  }
  printf("\n");
  printf("Comparator Mode             ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][2])&0x03);
      //}
  }
  printf("\n");
  printf("Comparator Timing           ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][2]>>2)&0x07);
      //}
  }
  printf("\n");
  printf("Pre_block_end               ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",((febbuf[i][2]>>5)&0x07) + ((febbuf[i][3]&0x01)<<3));
      //}
  }
  printf("\n");
  printf("Extra_L1A_delay             ");
  for(i=0;i<5;i++){
    //if(iuse[i]==1){
      printf("  :  %d",(febbuf[i][3]>>1)&0x03);
      //}
  }
  printf("\n");
}

/* Thermometers */

float DAQMB::readthermx(int feb)
{
  float cval=0.0;
  if(hardware_version_<=1)
  {
  float Vout= (float) readADC(1, feb) / 1000.;
  if(feb!=6){
    cval = 1/(0.1049406423E-2+0.2133635468E-3*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-0.27315E3;
  }else{
    cval=Vout;
  }
  }
  return cval;
}


unsigned int DAQMB::readADC(int ireg, int ichn) {
  if(hardware_version_<=1)
  {
  cmd[0]=ireg; /* register 1-4 */
  cmd[1]=ichn; /* channel 0-7 */
  devdo(MADC,16,cmd,0,sndbuf,rcvbuf,2);
  return unpack_ival();
  } 
  else return 0;
}


/* DAQMB LVMB routines */

float DAQMB::lowv_adc(int ichp,int ichn)
{
/*
static int conv_lowv[5][8]={
  {2,2,1,1,2,2,1,2},
  {2,1,2,2,1,2,2,1},
  {2,2,1,2,2,2,2,2},
  {2,2,2,2,2,2,2,2},
  {2,2,2,2,2,2,2,2}
}; 

 cmd[0]=ichp; 
 cmd[1]=ichn; 
 devdo(LOWVOLT,16,cmd,0,sndbuf,rcvbuf,2);
 unsigned int ival=unpack_ival();
  ival=ival*conv_lowv[ichp-1][ichn];
 float cval=ival*5.0/4.096;
  return cval;
*/
     int n, iv;
     WriteRegister(set_ADC_SELECT, ichp-1);
     n=(ichn<<4) + 0xFF89;
     WriteRegister(ADC_CTRL_BYTE, n);
     iv=ReadRegister(ADC_READ);
     float cval=iv*10.0/4.096;
     return cval;

} 

void DAQMB::lowv_onoff(char c)
{
 /* 0x3f (or 0xff for ODMB) means all on, 0x00 means all off  */
 unsigned short all_mask=(hardware_version_<=1)?0x3F:0xFF;
 unsigned short mask= all_mask & c & (~power_mask_);
 WriteRegister(set_POWER_MASK, mask);
}

unsigned int DAQMB::lowv_rdpwrreg()
{
 return ReadRegister(read_POWER_MASK);
}

int DAQMB::lvmb_power_state()
{
  // TODO: try to get the REAL power state of CFEB/ALCT from LVMB
  int all_mask = (hardware_version_<=1)?0x3F:0xFF;
  return (all_mask & ReadRegister(read_POWER_MASK));
}

/* FPGA and PROM codes  */
//
bool DAQMB::CheckVMEFirmwareVersion() {
  //
  // read the value from the DMB:
  vmefpgaid();
  //
  bool check_ok = false;
  //
  //  std::cout << "expected DMB VME =" << std::dec << GetExpectedVMEFirmwareTag() << std::endl;
  //  std::cout << "read     DMB VME =" << std::dec << GetFirmwareVersion() << std::endl;
  //
  if ( GetFirmwareVersion() == GetExpectedVMEFirmwareTag() ) {
    check_ok = true; 
  } else { 
    (*MyOutput_) << "expected DMB VME =" << std::hex << GetExpectedVMEFirmwareTag() << std::endl;
    (*MyOutput_) << "read     DMB VME =" << std::hex << GetFirmwareVersion() << std::endl;
    std::cout    << "expected DMB VME =" << std::hex << GetExpectedVMEFirmwareTag() << std::endl;
    std::cout    << "read     DMB VME =" << std::hex << GetFirmwareVersion() << std::endl;
  }
  //
  return check_ok;
}
//
bool DAQMB::CheckControlFirmwareVersion() {
  //
  bool check_ok = false;
  if ( mbfpgauser() == (unsigned int) GetExpectedControlFirmwareTag() ) {
    check_ok = true;
  } else {
    (*MyOutput_) << "expected DMB Control =" << std::hex << GetExpectedControlFirmwareTag() << std::endl;
    (*MyOutput_) << "read     DMB Control =" << std::hex << mbfpgauser() << std::endl;
    std::cout    << "expected DMB Control =" << std::hex << GetExpectedControlFirmwareTag() << std::endl;
    std::cout    << "read     DMB Control =" << std::hex << mbfpgauser() << std::endl;
  }
  return check_ok;
  //
}
//
bool DAQMB::CheckCFEBFirmwareVersion(CFEB & cfeb) {
  //
  int cfeb_index = cfeb.number();
  //
  //  std::cout << "expected CFEB[" << cfeb_index << "] = 0x" << std::hex << GetExpectedCFEBFirmwareTag(cfeb_index) << std::endl;
  //  std::cout << "read     CFEB[" << cfeb_index << "] = 0x" << std::hex << febfpgauser(cfeb) << std::endl;
  //
  return ( febfpgauser(cfeb) == (unsigned int) GetExpectedCFEBFirmwareTag(cfeb_index) );
}
//
unsigned int DAQMB::febpromuser(CFEB & cfeb)
{ unsigned int ibrd=0;
  int hversion=cfeb.GetHardwareVersion();
if(hversion<=1)
{
  DEVTYPE dv = cfeb.promDevice();
  printf("%d \n",dv);
  cmd[0]=PROM_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  for (int i=0;i<3;i++) {
    devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
    rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
    rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
    rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
    rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
    ibrd=unpack_ibrd();
    cmd[0]=PROM_BYPASS;
    sndbuf[0]=0;
    devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
    usleep(100);
    if (((0xff&rcvbuf[0])!=0xff)||((0xff&rcvbuf[1])!=0xff)||
        ((0xff&rcvbuf[2])!=0xff)||((0xff&rcvbuf[3])!=0xff)) return ibrd;
  }
}
  return ibrd;
}

unsigned int  DAQMB::febpromid(CFEB & cfeb)
{
  unsigned int ibrd=0;
  int hversion=cfeb.GetHardwareVersion();
if(hversion<=1)
{
  DEVTYPE dv = cfeb.promDevice();
  cmd[0]=PROM_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,8,cmd,33,sndbuf,rcvbuf,1);
  rcvbuf[0]=((rcvbuf[0]>>1)&0x7f)+((rcvbuf[1]<<7)&0x80);
  rcvbuf[1]=((rcvbuf[1]>>1)&0x7f)+((rcvbuf[2]<<7)&0x80);
  rcvbuf[2]=((rcvbuf[2]>>1)&0x7f)+((rcvbuf[3]<<7)&0x80);
  rcvbuf[3]=((rcvbuf[3]>>1)&0x7f)+((rcvbuf[4]<<7)&0x80);
  ibrd=unpack_ibrd();
  cmd[0]=PROM_BYPASS;
  sndbuf[0]=0;
  devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
}
  return ibrd;
}

unsigned int  DAQMB::febfpgauser(CFEB & cfeb)
{
  unsigned int ibrd=0;
  int hversion=cfeb.GetHardwareVersion();
if(hversion<=1)
{
  DEVTYPE dv = cfeb.scamDevice();
  cmd[0]=VTX_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
  // RPW not sure about this
  //printf(" The FPGA USERCODE is %02x%02x%02x%02x \n",0xff&rcvbuf[3],0xff&rcvbuf[2],0xff&rcvbuf[1],0xff&rcvbuf[0]);
  ibrd = unpack_ibrd();
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
}
else if(hversion==2)
{
  write_cfeb_selector(cfeb.SelectorBit());
  dcfeb_fpga_call(VTX6_USERCODE, 0, (char *)&ibrd);
}
  return ibrd;
}

unsigned int  DAQMB::febfpgaid(CFEB & cfeb)
{
  unsigned ibrd=0;
  int hversion=cfeb.GetHardwareVersion();
if(hversion<=1)
{
  DEVTYPE dv = cfeb.scamDevice();
  cmd[0]=VTX_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  devdo(dv,5,cmd,32,sndbuf,rcvbuf,1);
  char sbuf[100];
  (*MyOutput_) << " The FEB " << dv-F1SCAM+1 << " FPGA Chip should be 610093 (last 6 digits) "  << std::endl;
  sprintf(sbuf, "%02X%02X%02X%02X", 0xff&rcvbuf[3], 0xff&rcvbuf[2], 0xff&rcvbuf[1], 0xff&rcvbuf[0]);
  (*MyOutput_) << " The FPGA Chip IDCODE is " << sbuf << std::endl;
  ibrd = unpack_ibrd();
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0;
  devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
}
else if(hversion==2)
{
  write_cfeb_selector(cfeb.SelectorBit());
  dcfeb_fpga_call(VTX6_IDCODE, 0, (char *)&ibrd);
}
  return ibrd;
}

unsigned int DAQMB::mbpromuser(int prom)
{
  if(hardware_version_<=1)
  {
  unsigned int ibrd;
  DEVTYPE dv;

  if(prom==0){dv=VPROM;}else{dv=MPROM;}
  for (int i=0;i<3;i++) {
      cmd[0]=PROM_USERCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      ibrd=unpack_ibrd();
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);

      printf("from mbpromuser: %08X %02X %02X %02X %02X\n",ibrd,rcvbuf[0],rcvbuf[1],rcvbuf[2],rcvbuf[3]);
      if (((0xff&rcvbuf[0])!=0xff)||((0xff&rcvbuf[1])!=0xff)||
          ((0xff&rcvbuf[2])!=0xff)||((0xff&rcvbuf[3])!=0xff)) return ibrd;
  }
      return ibrd;
  }
  else return 0;
}

unsigned int  DAQMB::mbpromid(int prom)
{
  if(hardware_version_<=1)
  {
  unsigned int ibrd;
  DEVTYPE dv;

  if(prom==0){dv=VPROM;}else{dv=MPROM;}
      cmd[0]=PROM_IDCODE;
      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0xFF;
      devdo(dv,8,cmd,32,sndbuf,rcvbuf,1);
      ibrd=unpack_ibrd();
      cmd[0]=PROM_BYPASS;
      sndbuf[0]=0;
      devdo(dv,8,cmd,0,sndbuf,rcvbuf,0);
      return ibrd;
  }
  else return 0;
}


unsigned int  DAQMB::mbfpgauser()
{
  unsigned int ibrd=0;

  if(hardware_version_<=1)
  {
  DEVTYPE dv=MCTRL;
  cmd[0]=VTX2_USERCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,6,cmd,32,sndbuf,rcvbuf,1);
  ibrd=unpack_ibrd();
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
  }
  else
  {
     odmb_fpga_call(VTX6_USERCODE, 0, (char *)&ibrd);
  } 
  return ibrd;
}

unsigned int  DAQMB::mbfpgaid()
{
  unsigned int ibrd=0;

  if(hardware_version_<=1)
  {
  DEVTYPE dv=MCTRL;
  cmd[0]=VTX2_IDCODE;
  sndbuf[0]=0xFF;
  sndbuf[1]=0xFF;
  sndbuf[2]=0xFF;
  sndbuf[3]=0xFF;
  sndbuf[4]=0xFF;
  devdo(dv,6,cmd,32,sndbuf,rcvbuf,1);
  ibrd=unpack_ibrd();
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
  }
  else
  {
     odmb_fpga_call(VTX6_IDCODE, 0, (char *)&ibrd);
  } 
  return ibrd;
}

//
void DAQMB::vmefpgaid()
{
  if(hardware_version_<=1)
  {
  //
  cmd[0]=1;
  cmd[1]=0;
  devdo(DEVSTATUS,4,cmd,0,sndbuf,rcvbuf,1);
  //cout <<" Register0 rcv[0,1] "<<hex<<(rcvbuf[0]&0xff)<<' '<<hex<<(rcvbuf[1]&0xff)<<'\n';
  //
  fwvers_=((rcvbuf[1]<<4)&0xff0)+((rcvbuf[0]>>4)&0xf);
  fwrv_=(rcvbuf[0]&0xf);
  //cout <<" DMB VME FPGA firmware Ver. "<< hex <<fwvers<<" Rev.: "<<fwrv<<'\n';
  //
  cmd[0]=1; 
  cmd[1]=1;
  devdo(DEVSTATUS,4,cmd,0,sndbuf,rcvbuf,1);
  //cout <<" Register1 rcv[0,1] "<<hex<<(rcvbuf[0]&0xff)<<' '<<hex<<(rcvbuf[1]&0xff)<<'\n';
  fwyear_=(rcvbuf[0]&0x3f);
  fwmonth_=((rcvbuf[1]>>4)&0xf);
  fwday_=((rcvbuf[1]<<2)&0x1c)+((rcvbuf[0]>>6)&0x3);
  //cout <<" Date code: Month "<<fwmonth<<" Day "<<fwday<<" Year "<<fwyear<<'\n';
  //
  }
}
//
// DAQMB calibrate
//
void DAQMB::set_cal_dac(float volt0,float volt1)
{
  int cfeb_hversion=cfebs_[0].GetHardwareVersion();
  unsigned short int dacout0,dacout1;

    /* digitize voltages */  
  dacout0=int(volt0*4095./5.0);
  dacout1=int(volt1*4095./5.0);

  if(hardware_version_<=1)
  {
    /* load cdac */
    /* cmd[0]=(0xff&dacout0);
      cmd[1]=((dacout0>>8)&0xff);
      cmd[2]=(0xff&dacout1);
      cmd[3]=((dacout1>>8)&0xff);
    */
    //Temperarily swapped here, GU
    cmd[2]=(0xff&dacout0);
    cmd[3]=((dacout0>>8)&0xff);
    cmd[0]=(0xff&dacout1);
    cmd[1]=((dacout1>>8)&0xff);
    devdo(CDAC,32,cmd,0,sndbuf,rcvbuf,2); 
  }
  else if(hardware_version_==2 && cfeb_hversion==2)
  {
    for(unsigned icfeb = 0; icfeb < cfebs_.size(); ++icfeb) 
    {
       unsigned cw, dword;
       cw=0x4000|(dacout0<<1);
       dword=shuffle32(cw)>>16;
       dcfeb_hub(cfebs_[icfeb], Calib_DAC, 16, &dword, rcvbuf, NO_BYPASS|NOW);
       cw=0xc000|(dacout1<<1);
       dword=shuffle32(cw)>>16;
       dcfeb_core(Calib_DAC, 16, &dword, rcvbuf, NOOP_YES|NOW);
    }
  }
}

void DAQMB::buck_shift()
{
int lay,i,j;
int nchips2;
char shft_bits[6][6];

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) 
  {
      int brd=cfebItr->number();
      for(lay=0;lay<6;lay++){
        shft_bits[lay][0]=((shift_array[brd][lay][13]<<6)|(shift_array[brd][lay][14]<<3)|shift_array[brd][lay][15])&0XFF;
        shft_bits[lay][1]=((shift_array[brd][lay][10]<<7)|(shift_array[brd][lay][11]<<4)|(shift_array[brd][lay][12]<<1)|(shift_array[brd][lay][13]>>2))&0XFF;
        shft_bits[lay][2]=((shift_array[brd][lay][8]<<5)|(shift_array[brd][lay][9]<<2)|(shift_array[brd][lay][10]>>1))&0XFF;
        shft_bits[lay][3]=((shift_array[brd][lay][5]<<6)|(shift_array[brd][lay][6]<<3)|shift_array[brd][lay][7])&0XFF;
        shft_bits[lay][4]=((shift_array[brd][lay][2]<<7)|(shift_array[brd][lay][3]<<4)|(shift_array[brd][lay][4]<<1)|(shift_array[brd][lay][5]>>2))&0XFF;
        shft_bits[lay][5]=((shift_array[brd][lay][0]<<5)|(shift_array[brd][lay][1]<<2)|(shift_array[brd][lay][2]>>1))&0XFF;
      }
    int hversion=cfebItr->GetHardwareVersion();
    if(hversion<=1)
    {            
      DEVTYPE dv = cfebItr->scamDevice();
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      // (*MyOutput_)<<" first devdo call \n";
      cmd[0]=VTX_USR2;
      char chip_mask= cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      std::vector<BuckeyeChip> buckeyes = cfebItr->buckeyeChips();
      nchips2=buckeyes.size();
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(int ichip = nchips2-1; ichip >= 0; --ichip) {
        int chip = buckeyes[ichip].number();

        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }

      devdo(dv,5,cmd,nchips2*48,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
    }
    else if (hversion==2)
    {
      int chip_mask= cfebItr->chipMask();
      write_cfeb_selector(cfebItr->SelectorBit());
      BuckeyeShift((int)chip_mask, shft_bits);
    }
  }
  //
  ::usleep(200);
  //
}

void DAQMB::buck_shift_out()
{
int lay,i,j;
int nchips2;
int boffset;
int xtrabits = 2;
int swtchbits = -1;
char shft_bits[6][6];
  boffset=xtrabits+swtchbits;

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) 
  {
    int hversion=cfebItr->GetHardwareVersion();
    if(hversion<=1)
    {            
      DEVTYPE dv = cfebItr->scamDevice();
      int brd=cfebItr->number();
      printf(" BUCKSHIFT scamdev %d brd %d \n",dv,brd);
      for(lay=0;lay<6;lay++){
        shft_bits[lay][0]=((shift_array[brd][lay][13]<<6)|(shift_array[brd][lay][14]<<3)|shift_array[brd][lay][15])&0XFF;
        shft_bits[lay][1]=((shift_array[brd][lay][10]<<7)|(shift_array[brd][lay][11]<<4)|(shift_array[brd][lay][12]<<1)|(shift_array[brd][lay][13]>>2))&0XFF;
        shft_bits[lay][2]=((shift_array[brd][lay][8]<<5)|(shift_array[brd][lay][9]<<2)|(shift_array[brd][lay][10]>>1))&0XFF;
        shft_bits[lay][3]=((shift_array[brd][lay][5]<<6)|(shift_array[brd][lay][6]<<3)|shift_array[brd][lay][7])&0XFF;
        shft_bits[lay][4]=((shift_array[brd][lay][2]<<7)|(shift_array[brd][lay][3]<<4)|(shift_array[brd][lay][4]<<1)|(shift_array[brd][lay][5]>>2))&0XFF;
        shft_bits[lay][5]=((shift_array[brd][lay][0]<<5)|(shift_array[brd][lay][1]<<2)|(shift_array[brd][lay][2]>>1))&0XFF;
      }
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      // cout<<" first devdo call \n";
      cmd[0]=VTX_USR2;
      char chip_mask= cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      std::vector<BuckeyeChip> buckeyes = cfebItr->buckeyeChips();
      nchips2=buckeyes.size();
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      j=0;
      for(int ichip = nchips2-1; ichip >= 0; --ichip) {
        int chip = buckeyes[ichip].number();

        lay=layers[chip];
        for(i=0;i<6;i++){
          sndbuf[j*6+i]=shft_bits[lay][i];
        }
        j++;
      }

      devdo(dv,5,cmd,nchips2*48+xtrabits,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,2); 
      for(int k=0;k<6*nchips2;k++)shift_out[brd][k]=0;
      int jj=0; 
      for(int k=6*(nchips2);k>=0;k--){
        shift_out[brd][jj]=shift_out[brd][jj]|(rcvbuf[k]<<(8-boffset));
        shift_out[brd][jj]=shift_out[brd][jj]|((rcvbuf[k-1]>>boffset)&~(~0<<(8-boffset)));
        jj++;
      }
    }
    else if (hversion==2)
    {
      int chip_mask= cfebItr->chipMask();
      write_cfeb_selector(cfebItr->SelectorBit());
      BuckeyeShift(chip_mask, shft_bits, rcvbuf);
    }
  }
}

int DAQMB::buck_shift_test()
{
#ifdef debugV
  (*MyOutput_) << "inside buck_shift_test()" << std::endl;
#endif

int i,j,nmtch;
int nchips2;
int xtrabits = 2;
int swtchbits = -1;
char pat[42],chk[42] = {0xBA,0xDF,0xEE,0xD5,0xDE,0xAD};

  int pass=0;
  int boffset=xtrabits+swtchbits;

  for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) 
  {
    int hversion=cfebItr->GetHardwareVersion();
    if(hversion<=1)
    {            

      DEVTYPE dv = cfebItr->scamDevice();
      //int brdn=cfebItr->number();

      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_MASK;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_USR2;
      char chip_mask = cfebItr->chipMask();
      devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
      nchips2=cfebItr->buckeyeChips().size();
      printf(" nchips2 %d chip_mask %04x \n",nchips2,chip_mask);
      for(i=0;i<6*nchips2;i++)pat[i]=0;
      cmd[0]=VTX_USR1;
      sndbuf[0]=CHIP_SHFT;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,2);
      cmd[0]=VTX_USR2;
      sndbuf[0]=0xAD;
      sndbuf[1]=0xDE;
      sndbuf[2]=0xD5;
      sndbuf[3]=0xEE;
      sndbuf[4]=0xDF;
      sndbuf[5]=0xBA;
      for(i=6;i<6*(nchips2+1)+1;i++)sndbuf[i]=0x00;
      devdo(dv,5,cmd,6*(nchips2+1)*8+xtrabits,sndbuf,rcvbuf,1);
      j=0;
      //   for(i=6*(nchips2+1);i>=6*nchips2;i--){
      for(i=6*(nchips2+1);i>=0;i--){
        printf("shfttst %d %02X \n",i,rcvbuf[i]&0xFF);
        pat[j]=pat[j]|(rcvbuf[i]<<(8-boffset));
        pat[j]=pat[j]|((rcvbuf[i-1]>>boffset)&~(~0<<(8-boffset)));
        j++;
      }
      nmtch=0;
      for(i=0;i<6;i++){
	// (*MyOutput_)<< hex <<pat[i] << dec;
        printf("%02x",pat[i]&0xff);
        if(pat[i]==chk[i])nmtch++;
      }
      printf("\n");
      // (*MyOutput_)<< endl;
      if(nmtch==6){
        pass=1;
        (*MyOutput_)<<"Pattern returned is OK\n";
      } else {
        pass=0;
        (*MyOutput_)<<"Pattern returned is Wrong\n";
      }
      cmd[0]=VTX_USR1;
      sndbuf[0]=NOOP;
      devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,2);
    }
    else if(hversion==2)
    {
      (*MyOutput_)<<"Buckeye Shift Test for DCFEB #" << cfebItr->number()+1 << std::endl;
                            
      char oldpat[36], newpat[36];
      // initialize some silly data as the test pattern
      for(int chip=0; chip<6; chip++)
      {
          newpat[chip*6+0]=0xAD;
          newpat[chip*6+1]=0xDE;
          newpat[chip*6+2]=0xD0+chip;
          newpat[chip*6+3]=0xEE;
          newpat[chip*6+4]=0xDF;
          newpat[chip*6+5]=0xBA;
      }
      
      // first pass: shift the pattern into the Buckeye chips and read out the old data
      dcfeb_hub(*cfebItr, CHIP_SHFT, 288, newpat, oldpat, NOW|NOOP_YES|READ_YES);
      udelay(2000);
      //  second pass: shift the old data back to the Buckeye chips and read out the pattern
      dcfeb_hub(*cfebItr, CHIP_SHFT, 288, oldpat, rcvbuf, NOW|NOOP_YES|READ_YES);
      // verify: the read-out must be exactly as the pattern
      udelay(2000);
      pass=1;
      for(i=0;i<36; i++)
      {
          printf("byte %02X, data in: %02X; data out: %02X\n", i, newpat[i]&0xFF, rcvbuf[i]&0xFF);
          if(newpat[i]!=rcvbuf[i]) pass=0;
      } 
      if(pass==1) (*MyOutput_)<<"Pattern returned is OK\n";
      else        (*MyOutput_)<<"Pattern returned is Wrong\n";
    }
  }
  return pass;
}


void DAQMB::preamp_initx()
{
int i,j,nchips2;
char shft_bits[6][6];
for(CFEBItr cfebItr = cfebs_.begin(); cfebItr != cfebs_.end(); ++cfebItr) 
{

  int hversion=cfebItr->GetHardwareVersion();
  if(hversion<=1)
  {            
    DEVTYPE dv = cfebItr->scamDevice();

    cmd[0]=VTX_USR1;
    sndbuf[0]=CHIP_MASK;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    char chip_mask= cfebItr->chipMask();
    devdo(dv,5,cmd,6,&chip_mask,rcvbuf,0);
    nchips2 = cfebItr->buckeyeChips().size();
    cmd[0]=VTX_USR1;
    sndbuf[0]=CHIP_SHFT;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR2;
    for(i=0;i<nchips2;i++){
      for(j=0;j<6;j++){
        sndbuf[i*6+j]=0;
      }
    }
    devdo(dv,5,cmd,nchips2*48,sndbuf,rcvbuf,0);
    cmd[0]=VTX_USR1;
    sndbuf[0]=NOOP;
    devdo(dv,5,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=VTX_BYPASS;
    devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
  }
  else if (hversion==2)
  {
      for(i=0; i<6; i++) for(j=0; j<6; j++) shft_bits[i][j]=0;
      int chip_mask= cfebItr->chipMask();
      write_cfeb_selector(cfebItr->SelectorBit());
      BuckeyeShift(chip_mask, shft_bits);
  }
}
  (*MyOutput_) << "done with preamp init " << std::endl;
}


void DAQMB::set_cal_tim_pulse(int itim)
{
  //
  //(*MyOutput_)<< "setting pulse timing to " << itim << std::endl; 
  int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
    | (calibration_l1acc_delay_ & 0x1F) << 4
    | (itim & 0x1F) << 9
    | (inject_delay_ & 0x1F) << 14;
  int dword;
  dword=(cal_delay_bits)&0xfffff;
  //
  setcaldelay(dword);
  //
  usleep(100);
  //

}


void DAQMB::set_cal_tim_inject(int ntim)
{
  (*MyOutput_)<< "setting inject timing to " << ntim << std::endl;
  int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
    | (calibration_l1acc_delay_ & 0x1F) << 4
    | (pulse_delay_ & 0x1F) << 9
    | (inject_delay_ & 0x1F) << 14;
  int dword;
  dword=(cal_delay_bits)&0x3fff;
  dword=dword|((ntim&0x1f)<<14);
  setcaldelay(dword);
}
  

void DAQMB::toggle_pedestal()
{
  if(hardware_version_<=1)
  {
    cmd[0]=VTX2_USR1;
    sndbuf[0]=PED_TRIG;
    devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
/*    printf(" Returned from CYCLYE_TRIG instr: %02X  \n",rcvbuf[0]&0xFF); 
    cmd[0]=VTX2_BYPASS;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
    cmd[0]=VTX2_USR2;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0); */
    cmd[0]=VTX2_USR1;
    sndbuf[0]=NOOP;
    devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
    /*  printf(" Returned from NOOP instr: %02X  \n",rcvbuf[0]&0xFF);*/
    cmd[0]=VTX2_BYPASS;
    devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}


void DAQMB::pulse(int Num_pulse,unsigned int pulse_delay)
{
  if(hardware_version_<=1)
  {
   int j;
   for(j=0;j<Num_pulse;j++){
     sndbuf[1]=(pulse_delay&0xff00)>>8;
     sndbuf[0]=(pulse_delay&0x00ff);
     devdo(MCTRL,-99,sndbuf,0,sndbuf,rcvbuf,0); 
     cmd[0]=VTX2_USR1;
     sndbuf[0]=PULSE;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_USR1;
     sndbuf[0]=NOOP;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     if(j<(Num_pulse-1)){
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);  
     }
     else{
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
     }
   }
  }
}

void DAQMB::inject(int Num_pulse,unsigned int pulse_delay)
{

  if(hardware_version_<=1)
  {
   (*MyOutput_) << "DAQMB.inject " << std::endl;

   for(int j=0;j<Num_pulse;j++){
     sndbuf[1]=(pulse_delay&0xff00)>>8;
     sndbuf[0]=(pulse_delay&0x00ff);
     devdo(MCTRL,-99,sndbuf,0,sndbuf,rcvbuf,0); 
     cmd[0]=VTX2_USR1;
     sndbuf[0]=INJECT;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_USR1;
     sndbuf[0]=NOOP;
     devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
     cmd[0]=VTX2_BYPASS;
     if(j<(Num_pulse-1)){
	devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);  
     }
     else
     {
	devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
     }
   }
  }
}


// DAQMB fifo write and reads

void DAQMB::wrtfifo(int fifo,int nsndfifo,char* sndfifo)
{
  int i=fifo+FIFO1;
  DEVTYPE devnum=(DEVTYPE)i;
  //
  printf(" sndfifo: %d %02x %02x \n",nsndfifo,sndfifo[0]&0xff,sndfifo[1]&0xff);
  /* fifo write */  
  cmd[0]=4;
  //(*MyOutput_) << "wrtfifo devnum FIFO7 " << devnum << " " << FIFO7 << std::endl;
  if(devnum-FIFO7!=0){
    //(*MyOutput_) << "devdo1" << std::endl;
    devdo(devnum,1,cmd,nsndfifo*2,sndfifo,rcvbuf,2);
    //(*MyOutput_) << "devdo1back" << std::endl;
  }
  else{
    //(*MyOutput_) << "devdo2" << std::endl;
    devdo(devnum,1,cmd,nsndfifo,sndfifo,rcvbuf,2);
    //(*MyOutput_) << "devdo2back" << std::endl;
  } 
}
//
void DAQMB::readfifo(int fifo,int nrcvfifo,char* rcvfifo)
{  
  if(hardware_version_<=1)
  {
  //
  //(*MyOutput_) << "readfifo" << std::endl;
  //
  PRINTSTRING(OVAL: before start routine in readfifo);
  PRINTSTRING(OVAL: after start routine in readfifo);
  //
  //(*MyOutput_) << "readfifo2" << std::endl;
  //
  int i=fifo+FIFO1;
  DEVTYPE devnum=(DEVTYPE)i;
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=devnum-FIFO1+1; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  //(*MyOutput_) << "readfifo3" << std::endl;
  //
  cmd[0]=5;
  devdo(devnum,1,cmd,nrcvfifo*2,sndbuf,rcvfifo,1);
  //
  //  for(i=0;i<16380;i++)printf(" %d %04x ",i,((rcvbuf[2*i]<<8)&0xff00)|(rcvbuf[2*i+1]&0xff)); 
  //
  //(*MyOutput_) << "readfifo4" << std::endl;
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;

  sndbuf[0]=0; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  printf("readfifo: %d %02x %02x \n",nrcvfifo,rcvfifo[0]&0xff,rcvfifo[1]&0xff); 
  //
  }
}

// DAQMB load and read flash memory (electronics experts only)

void DAQMB::buckflash_load(char *fshift)
{
  
  if(hardware_version_<=1)
  {
  (*MyOutput_) << "inside load" <<std::endl;
  
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); // initialize programming
 for(int i=0;i<295;i++)sndbuf[i]=fshift[i];
 sndbuf[288]=0x01|sndbuf[288];
 sndbuf[289]=0x01|sndbuf[289];
 cmd[0]=1;
 devdo(BUCSHF,1,cmd,295*8,sndbuf,rcvbuf,0); // load buckeye pattern
 //Just to put some extra TMS low after the process for safety.
 //The exact number needed is 295.
  }
}

void DAQMB::buckflash_load2(int nbytes,char *fshift)
{
  if(hardware_version_<=1)
  {
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); // initialize programming
 for(int i=0;i<nbytes;i++)sndbuf[i]=fshift[i];
 cmd[0]=1;
 devdo(BUCSHF,1,cmd,nbytes*8,sndbuf,rcvbuf,0); // load buckeye pattern
 //The exact number needed is nbits.
  }
}


void DAQMB::buckflash_read(char *rshift)
{
  if(hardware_version_<=1)
  {
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); // initialize programming 
 cmd[0]=3;
 //used to be devdo(BUCSHF,1,cmd,0,sndbuf,rshift,1); 
 devdo(BUCSHF,1,cmd,295*8,sndbuf,rshift,1); 
 /* return 296 bits */
  }
}

void DAQMB::buckflash_read2(int nbytes,char *rshift)
{
  if(hardware_version_<=1)
  {
  printf("entered buckflash_read2 \n");
  cmd[0]=0;
  devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); // initialize programming
  cmd[0]=3;
  devdo(BUCSHF,1,cmd,nbytes*8,sndbuf,rshift,1);
  /* return bits */
  }
}


void DAQMB::buckflash_pflash()
{
  if(hardware_version_<=1)
  {
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); //initialize the counter
 cmd[0]=2;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1);
 // sndbuf[0]=0xff;
 // sndbuf[1]=0xff;
 // devdo(BUCSHF,-99,sndbuf,sndbuf,rcvbuf,0); 
  }
}

void DAQMB::buckflash_init()
{
  if(hardware_version_<=1)
  {
 cmd[0]=0;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,0); // initialize programming
 cmd[0]=4;
 devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,2); 
 // sndbuf[0]=0xff;
 // sndbuf[1]=0x03;
 // devdo(BUCSHF,-99,sndbuf,0,sndbuf,rcvbuf,0);
  }
}

void DAQMB::buckflash_erase()
{
  if(hardware_version_<=1)
  {
  //
  cmd[0]=0;
  devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); // erase Flash memory
  cmd[0]=5;
  devdo(BUCSHF,1,cmd,0,sndbuf,rcvbuf,1); 
  (*MyOutput_) << " Wait for 10 Seconds for Flash Memory to finish " << std::endl;
  ::sleep(10);  // 10 seconds are required after erase
  //
  }
}

int DAQMB::Fill_BUCK_FLASH_contents(char * flash_content)
{
  //
  //int  *flash_bytsiz_p;
  //char *flash_content;
  //
  //flash_bytsiz_p=(int *)malloc(8);
  //flash_content=(char *)malloc(500);
  //
  int tms;
  int tdo[5];
  char tbits[6]={0x01,0x02,0x04,0x08,0x10,0x20};

  int flash_bytsiz=0;

  // fill buckeye shift
 
  printf(" buckeye shift \n");
  for(int chip=0;chip<6;chip++){
    for(int chan=0;chan<16;chan++){
      for(int bit=0;bit<3;bit++){
        tms=0;
        flash_content[flash_bytsiz]=tms;
        for(int cfeb=0;cfeb<5;cfeb++){
          tdo[cfeb]=0x00;
          if(shift_array_[cfeb][chip][16-chan-1]&(1<<bit))tdo[cfeb]=tbits[cfeb+1];; 
          flash_content[flash_bytsiz]=flash_content[flash_bytsiz]|tdo[cfeb];
        }
        buckflash_dump(flash_bytsiz,flash_content);
        flash_bytsiz=flash_bytsiz+1;
      }
    }
  }

  // fill 5 bit flash end word;
  
  printf("buck shift end \n");
  int tmd_end_word=3;
  for(int bit=0;bit<5;bit++){
    tms=0;
    if(tmd_end_word&(1<<bit))tms=1;
    flash_content[flash_bytsiz]=tms; 
    buckflash_dump(flash_bytsiz,flash_content);
    flash_bytsiz=flash_bytsiz+1;
  }

  // now jtag stuff

  int ninstr=4;
  int instr_t=3;
  int instr_d=0;

  int ndata=4;
  int data_t=3;
  int data_d=0;

  int nusr1=5;
  int usr1_t=0;
  int usr1_d=2;

  int nbypass=8;
  int bypass_t=0x0080;
  int bypass_d=0x00ff;

  int nusr2=5;
  int usr2_t=0;
  int usr2_d=3;

  int nf7=8;
  int f7_t=0;
  int f7_d=7;

  int nxtra=1;
  int xtra_t=1;
  int xtra_d=0;   

  int nladc=8;
  int ladc_t=0;
  int ladc_d=4;

  int neoi=1;
  int eoi_t=1;
  int eoi_d=0;

  int nidle=3;
  int idle_t=1;
  int idle_d=0;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" usr1 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nusr1,usr1_t,usr1_d);
  flash_bytsiz=flash_bytsiz+nusr1;

  printf(" prom_bypass \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nbypass,bypass_t,bypass_d);
  flash_bytsiz=flash_bytsiz+nbypass;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" f7 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nf7,f7_t,f7_d);
  flash_bytsiz=flash_bytsiz+nf7;

  printf(" xtra bit \n"); 
  jtag_buckflash_engine(flash_bytsiz,flash_content,nxtra,xtra_t,xtra_d);
  flash_bytsiz=flash_bytsiz+nxtra;

  printf(" end of instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,neoi,eoi_t,eoi_d);
  flash_bytsiz=flash_bytsiz+neoi;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" usr2 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nusr2,usr2_t,usr2_d);
  flash_bytsiz=flash_bytsiz+nusr2;

  printf(" prom_bypass \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nbypass,bypass_t,bypass_d);
  flash_bytsiz=flash_bytsiz+nbypass;

  printf(" data \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ndata,data_t,data_d);
  flash_bytsiz=flash_bytsiz+ndata;

  printf(" comp mode ");
  for(int cfeb=0;cfeb<5;cfeb++)printf(" %d",comp_mode_cfeb_[cfeb]);
  printf("\n");
  jtag_buckflash_engine2(flash_bytsiz,flash_content,2,comp_mode_cfeb_);
  flash_bytsiz=flash_bytsiz+2;

  printf(" comp timing "); 
  for(int cfeb=0;cfeb<5;cfeb++)printf(" %d",comp_timing_cfeb_[cfeb]);
  printf("\n");
  jtag_buckflash_engine2(flash_bytsiz,flash_content,3,comp_timing_cfeb_);
  flash_bytsiz=flash_bytsiz+3;

  printf(" preblockend ");  
  for(int cfeb=0;cfeb<5;cfeb++)printf(" %d",pre_block_end_cfeb_[cfeb]);
  printf("\n");
  jtag_buckflash_engine2(flash_bytsiz,flash_content,4,pre_block_end_cfeb_);
  flash_bytsiz=flash_bytsiz+4;

  printf("L1A extra latency ");
  for(int cfeb=0;cfeb<5;cfeb++)printf(" %d",L1A_extra_cfeb_[cfeb]);
  printf("\n");
  jtag_buckflash_engine2(flash_bytsiz,flash_content,2,L1A_extra_cfeb_);
  flash_bytsiz=flash_bytsiz+2;

  printf(" xtra bit \n"); 
  jtag_buckflash_engine(flash_bytsiz,flash_content,nxtra,xtra_t,xtra_d);
  flash_bytsiz=flash_bytsiz+nxtra;

  printf(" xtra bit \n"); 
  jtag_buckflash_engine(flash_bytsiz,flash_content,nxtra,xtra_t,xtra_d);
  flash_bytsiz=flash_bytsiz+nxtra;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" usr1 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nusr1,usr1_t,usr1_d);
  flash_bytsiz=flash_bytsiz+nusr1;

  printf(" prom_bypass \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nbypass,bypass_t,bypass_d);
  flash_bytsiz=flash_bytsiz+nbypass;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" F4 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nladc,ladc_t,ladc_d);
  flash_bytsiz=flash_bytsiz+nladc;

  printf(" xtra bit \n"); 
  jtag_buckflash_engine(flash_bytsiz,flash_content,nxtra,xtra_t,xtra_d);
  flash_bytsiz=flash_bytsiz+nxtra;

  printf(" end of instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,neoi,eoi_t,eoi_d);
  flash_bytsiz=flash_bytsiz+neoi;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  printf(" usr2 \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nusr2,usr2_t,usr2_d);
  flash_bytsiz=flash_bytsiz+nusr2;

  printf(" prom_bypass \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nbypass,bypass_t,bypass_d);
  flash_bytsiz=flash_bytsiz+nbypass;

  printf(" instr \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,ninstr,instr_t,instr_d);
  flash_bytsiz=flash_bytsiz+ninstr;

  int thresh[5];
  for(int cfeb=0;cfeb<5;cfeb++){
   char dt[2];
   int dthresh=int(4095*((3.5-comp_thresh_cfeb_[cfeb])/3.5)); 
   dt[0]=0;
   dt[1]=0;
   for(int i=0;i<8;i++){
     dt[0]|=((dthresh>>(i+7))&1)<<(7-i);
     dt[1]|=((dthresh>>i)&1)<<(6-i);
   }
   dt[0]=((dt[1]<<7)&0x80) + ((dt[0]>>1)&0x7f);
   dt[1]=dt[1]>>1;
   thresh[cfeb]=((dt[1]<<8)&0xff00)|(dt[0]&0x00ff);
  }

  printf(" comp_thresh:");
  for(int cfeb=0;cfeb<5;cfeb++)printf(" %7.3f",comp_thresh_cfeb_[cfeb]);
  printf("\n");
  jtag_buckflash_engine2(flash_bytsiz,flash_content,15,thresh);
  flash_bytsiz=flash_bytsiz+15;

  printf(" xtra bit \n"); 
  jtag_buckflash_engine(flash_bytsiz,flash_content,nxtra,xtra_t,xtra_d);
  flash_bytsiz=flash_bytsiz+nxtra;

  printf(" idle mode \n");
  jtag_buckflash_engine(flash_bytsiz,flash_content,nidle,idle_t,idle_d);
  flash_bytsiz=flash_bytsiz+nidle;
  //flash_bytsiz_p[0]=flash_bytsiz;
  printf(" total number of bytes %d \n",flash_bytsiz);
  return flash_bytsiz;
}

void DAQMB::jtag_buckflash_engine(int nbuf,char *buf,int n,int t,int d)
{
  int tms;
  int tdo[5];
  char tbits[6]={0x01,0x02,0x04,0x08,0x10,0x20};
  int tbuf=nbuf;
  for(int i=0;i<n;i++){
    tms=0;
    if(t&(1<<i))tms=tbits[0];
    for(int cfeb=0;cfeb<5;cfeb++){
      tdo[cfeb]=0; 
      if(d&(1<<i))tdo[cfeb]=tbits[cfeb+1];
    }
    buf[tbuf]=tms|tdo[0]|tdo[1]|tdo[2]|tdo[3]|tdo[4];
    buckflash_dump(tbuf,buf);
    tbuf=tbuf+1;
  }
}

void DAQMB::jtag_buckflash_engine2(int nbuf,char *buf,int n,int *val)
{
  int tms;
  int tdo[5];
  char tbits[6]={0x01,0x02,0x04,0x08,0x10,0x20};
  int tbuf=nbuf;
  for(int i=0;i<n;i++){
    tms=0;
    for(int cfeb=0;cfeb<5;cfeb++){
      tdo[cfeb]=0; 
      if(val[cfeb]&(1<<i))tdo[cfeb]=tbits[cfeb+1];
    }
    buf[tbuf]=tms|tdo[0]|tdo[1]|tdo[2]|tdo[3]|tdo[4];
    buckflash_dump(tbuf,buf);
    tbuf=tbuf+1;
  }
}

void DAQMB::buckflash_dump(int nbuf,char *buf)
{
  int d[8]={0,0,0,0,0,0,0,0};
  for(int i=0;i<8;i++){
    if(buf[nbuf]&(1<<i))d[i]=1;
  }
  printf(" %03d %1d %1d %1d %1d %1d %1d %1d %1d \n",nbuf,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]);
}

// DAQMB program proms
void DAQMB::epromload_verify(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum)
{
  //
  std::cout << "New epromload GUJH " << std::endl;
  //
  char snd[1024],expect[1024],rmask[1024],smask[1024],cmpbuf[1024];
  DEVTYPE devstp,dv;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[16384],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int tstusr;
  int nowrit=0;
  // 
  (*MyOutput_) << " epromload " << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;
  //
  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
  }
  //
#ifdef OSUcc
  theController->SetUseDelay(true);
#endif
  //
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    if(dwnfp==NULL)
     {   std::cout << "Can't open firmware file " << downfile << std::endl;
         return;
     }
    fpout=fopen("/tmp/eprom.bit","w");
    if(fpout==NULL)
     {   std::cout << "Can't open eprom.bit file"<< std::endl;
         return;
     }
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    //  printf("Programming Design %s (%s) with %s\n",design,devstr,downfile);
    //
    char bogobuf[8192];
    unsigned long int nlines=0;
    unsigned long int line=1;
    FILE *bogodwnfp=fopen(downfile,"r");
    while (fgets(bogobuf,256,bogodwnfp) != NULL)
      if (strrchr(bogobuf,';')!=0) nlines++;
    fclose(bogodwnfp);
    float percent;
    while (fgets(buf,256,dwnfp) != NULL)  {
      percent = (float)line/(float)nlines;
      if ((line%20)==0) printf("<   > Processed line %lu of %lu (%.1f%%)\n",line,nlines,percent*100.0);
      fflush(stdout);
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	//  printf("%s",buf);
      }
      else {
	line++;
	if(strrchr(buf,';')==0){
	  do {
	    lastn=strrchr(buf,'\n');
	    if(lastn!=0)lastn[0]='\0';
	    if (fgets(buf2,256,dwnfp) != NULL){
	      strcat(buf,buf2);
	    }
	    else {
	     //    printf("End of File encountered.  Quiting\n");
	      return;
	   }
	  }
	  while (strrchr(buf,';')==0);
	}
       for(int i=0;i<1024;i++){
	 cmpbuf[i]=0;
	 sndbuf[i]=0;
	 rcvbuf[i]=0;
       }
       Parse(buf, &Count, &(Word[0]));
       // count=count+1;
       if(strcmp(Word[0],"SDR")==0){
	 cmpflag=0;    //disable the comparison for no TDO SDR
	 sscanf(Word[1],"%d",&nbits);
	 std::cout << " SIR nbits " << nbits << std::endl;
	 nbytes=(nbits-1)/8+1;
	 for(int i=2;i<Count;i+=2){
	   if(strcmp(Word[i],"TDI")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
	     }
	     if(nowrit==1&&((cbrdnum[0]!=0)||(cbrdnum[1]!=0))) {
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	       snd[1]=cbrdnum[1];
	       snd[2]=cbrdnum[2];
	       snd[3]=cbrdnum[3];
	     }
	     if(nowrit==1){
	       //  printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	       //FOO[0]=((snd[3]&0x000000ff)<<24)|((snd[2]&0x000000ff)<<16)|((snd[1]&0x000000ff)<<8)|(snd[0]&0x000000ff);
               // printf(" FOO %08x \n",FOO[0]);
	     }
	   }
	   if(strcmp(Word[i],"SMASK")==0){
	     for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
	     }
	   }
	   if(strcmp(Word[i],"TDO")==0){
	     cmpflag=1;
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
	   }
	   if(strcmp(Word[i],"MASK")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
	     }
	   }
	 }
	 for(int i=0;i<nbytes;i++){
	   //sndbuf[i]=snd[i]&smask[i];
	   sndbuf[i]=snd[i]&0xff;
          }
	 //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	 //         printf(" geo[dv].jchan %d \n \n",(geo[dv].jchan));
	 //         sleep(10);
 	 if(nowrit==0){
             if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	       // printf(" Emergency load SDR \n");
             }else{
                   if(nbits==4096){
                        scan(DATA_REG,sndbuf,nbits+xtrbits+1,rcvbuf,1);
                   }else{
                        scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
                   }
             }
	 }else{
	   if(writ==1) {
	     if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }else{ 
	       if(nbits==4096){
		 scan(DATA_REG,sndbuf,nbits+xtrbits+1,rcvbuf,1);
	       }else{
		 scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
	       }
	     }
	   }
	 } 
	 //  Data readback comparison here:
	 std::cout << " Data readback ntypes "<<nbytes<<std::endl;
         if(nbytes==512 || nbytes==1024){
	   rmask[511]=rmask[511]&0x7f; //do not compare bit 7 for the very last byte
	 for (int i=0;i<nbytes;i++) {
	   printf("%02x",rcvbuf[i]&0xff);
            if( devnum==F1PROM ||
                devnum==F2PROM ||
                devnum==F3PROM ||
                devnum==F4PROM ||
                devnum==F5PROM ){
              tmp=(rcvbuf[i]>>1)&0x7F;
	      rcvbuf[i]=tmp | (rcvbuf[i+1]<<7&0x80);
            }
	  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("\n GU *** read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); 
	 }
         printf("\n");
	 if (cmpflag==1) {
	   for (int i=0;i<nbytes;i++) {
             printf("%02x",rcvbuf[i]&0xff);
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3) fprintf(fpout,"\n");
	   }
           printf("\n");
	 }
         }
       }
       //
       else if(strcmp(Word[0],"SIR")==0){
	 nowrit=0;
	 sscanf(Word[1],"%d",&nbits);
	 std::cout << " SIR nbits " << nbits << std::endl;
          nbytes=(nbits-1)/8+1;
          for(int i=2;i<Count;i+=2){
            if(strcmp(Word[i],"TDI")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
              }
              if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1  
            }
            else if(strcmp(Word[i],"SMASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
              }
            }
            if(strcmp(Word[i],"TDO")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
            }
            else if(strcmp(Word[i],"MASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
              }
            }
          }
          for(int i=0;i<nbytes;i++){
            //sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
          }
          if(nowrit==0){
  	      devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
	  }
          else{
            if(writ==1) {devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
            if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
          }
	  /*
          printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
          printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
	  */
        }
        else if(strcmp(Word[0],"RUNTEST")==0){
          sscanf(Word[1],"%d",&pause);
	  // printf("RUNTEST = %d\n",pause);
	  //usleep(pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
          pause=pause;
	  if (pause<100000) pause=2*pause+100;
          if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
	  }
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
	  usleep(pause);
          // printf(" send sleep \n");  
	  /* printf("pause      %d us\n",pause);*/
	  //#ifdef OSUcc
	  //theController->flush_vme();
	  //#endif OSUcc
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	  //  printf("goto reset idle state\n"); 
	   devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
	   //#ifdef OSUcc
	   //theController->flush_vme();
	   //#endif OSUcc
        }
       else if(strcmp(Word[0],"TRST")==0){
       }
       else if(strcmp(Word[0],"ENDIR")==0){
       }
       else if(strcmp(Word[0],"ENDDR")==0){
       }
      }
    }
    fclose(fpout);
    fclose(dwnfp);
  }
}

void DAQMB::epromread(DEVTYPE devnum){
  // read back eprom for VPROM,MPROM,F1PROM-F5PROM
  int data_amount=0;
  int nbits,totbits;
  char tmp;
  int pause,xtrbits;
  char * devstr;
  DEVTYPE devstp,dv;
  char savbuf[513];
  FILE *fpout;
  //
  std::cout << "New epromload LSD (no chk svf file) " << std::endl;
  //
  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
  }
  //
  for(int k=devnum;k<=devstp;k++){
    dv=(DEVTYPE)k;
    xtrbits=geo[dv].sxtrbits;
    devstr=geo[dv].nam;
    if(geo[dv].jchan==4)data_amount=511;
    if(geo[dv].jchan==1)data_amount=255;
    if(geo[dv].jchan==3)data_amount=2047;
    printf(" geo.jchan %d \n",geo[dv].jchan);
    fflush(stdout);  
    fpout=fopen("/tmp/eprom.bit","w");
    if(fpout==NULL) continue;
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    //Loading device with 'idcode' instruction.
    nbits=8;
    sndbuf[0]=0xfe;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=32;
    for(int i=0;i<4;i++)sndbuf[i]=0x00;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    for(int i=0;i<5;i++){
      savbuf[i]=rcvbuf[i];
    }
    printf("idcode:");
    for(int i=0;i<4;i++){
      tmp=(savbuf[i]>>1)&0x7f;
      savbuf[i]=tmp|(savbuf[i+1]<<7&0x80);
    }
    for(int i=0;i<4;i++)printf("%02x",savbuf[3-i]&0xff);
    printf("\n"); 
    //Loading device with 'conld' instruction.
    nbits=8;
    sndbuf[0]=0xf0;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    pause=110000;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);
    
     //Check for Read/Write Protect.
    nbits=8;
    sndbuf[0]=0xff;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    printf("r/w protect %02x \n",rcvbuf[0]&0xff);

    //Loading device with 'idcode' instruction.
    nbits=8;
    sndbuf[0]=0xfe;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=32;
    for(int i=0;i<4;i++)sndbuf[i]=0x00;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    for(int i=0;i<5;i++){
      savbuf[i]=rcvbuf[i];
    }
    printf("idcode:");
    for(int i=0;i<4;i++){
      tmp=(savbuf[i]>>1)&0x7f;
      savbuf[i]=tmp|(savbuf[i+1]<<7&0x80);
    }
    for(int i=0;i<4;i++)printf("%02x",savbuf[3-i]&0xff);
    printf("\n"); 

    //Loading device with 'conld' instruction.
    nbits=8;
    sndbuf[0]=0xf0;
     printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    pause=110000;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);

    
     //Check for Read/Write Protect.
    nbits=8;
    sndbuf[0]=0xff;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    printf("r/w protect %02x \n",rcvbuf[0]&0xff);

    //Loading device with 'bypass' instruction.
    nbits=8;
    sndbuf[0]=0xff;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);


    //Loading device with 'ispen' instruction.
    nbits=8;
    sndbuf[0]=0xe8;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=6;
    for(int i=0;i<1;i++)sndbuf[i]=0x34;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    printf("ispen %02x \n",rcvbuf[0]&0xff);

    //Loading device with 'ispen' instruction.
    nbits=8;
    sndbuf[0]=0xe8;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=6;
    for(int i=0;i<1;i++)sndbuf[i]=0x34;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    printf("ispen %02x \n",rcvbuf[0]&0xff);

    //Loading device with 'conld' instruction.
    nbits=8;
    sndbuf[0]=0xf0;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    pause=110000;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);

    //Loading device with 'ispen' instruction.
    nbits=8;
    sndbuf[0]=0xe8;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=6;
    for(int i=0;i<1;i++)sndbuf[i]=0x34;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    printf("ispen %02x \n",rcvbuf[0]&0xff);

    // Loading device with a 'faddr' instruction.
    nbits=8;
    sndbuf[0]=0xeb;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=16;
    for(int i=0;i<4;i++)sndbuf[i]=0x00;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
     scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    pause=10;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);


    // Loading device with a 'fvfy0' instruction.
    nbits=8;
    sndbuf[0]=0xef;
    printf(" LSD SIR %d %02x %02x 0 \n",nbits,sndbuf[0]&0xff,sndbuf[1]&0xff);
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    pause=10;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);


    //4096x256 for cfeb
    //4096x1024 for dmb mprom
    nbits=4096;
    for(int i=0;i<513;i++)sndbuf[i]=0x00;
    printf(" LSD SDR %d %02x %02x %02x %02x 1 \n",nbits+xtrbits,sndbuf[0]&0xff,sndbuf[1]&0xff,sndbuf[2]&0xff,sndbuf[3]&0xff);
    totbits=4096;
    scan_dmb_headtail(DATA_REG,sndbuf,totbits,rcvbuf,1,3,1);
    printf(" raw:");
    for(int i=0;i<512;i++){
      savbuf[i]=rcvbuf[i];
      printf("%02x ",rcvbuf[i]&0xFF);
    }
    printf("\n"); 
    for(int j=0;j<data_amount;j++){
       totbits=4096;
       scan_dmb_headtail(DATA_REG,sndbuf,totbits,rcvbuf,1,0,1);
       savbuf[512]=rcvbuf[0];
       for(int i=0;i<512;i++){
         if(geo[dv].jchan==1){
           tmp=(savbuf[i]>>1)&0x7f;
           savbuf[i]=tmp|(savbuf[i+1]<<7&0x80);
         }
         fprintf(fpout," %02X",savbuf[i]&0xFF);
         if(i%4==3)fprintf(fpout,"\n");
       }
       for(int i=0;i<512;i++)savbuf[i]=rcvbuf[i];
    }
    scan_dmb_headtail(DATA_REG,sndbuf,totbits,rcvbuf,1,1,1);
    savbuf[512]=0xff;
    for(int i=0;i<512;i++){
      if(geo[dv].jchan==1){
        tmp=(savbuf[i]>>1)&0x7f;
        savbuf[i]=tmp|(savbuf[i+1]<<7&0x80);
      }
      fprintf(fpout," %02X",savbuf[i]&0xFF);
      if(i%4==3)fprintf(fpout,"\n");
      } 

    //Loading device with 'conld' instruction.
    nbits=8;
    sndbuf[0]=0xf0;
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    pause=110000;
    if(pause<100000)pause=2*pause+100;
    if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
    }
    sndbuf[0]=pause-(pause/256)*256;
    sndbuf[1]=pause/256;
    devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
    usleep(pause);

    //Loading device with 'bypass' instruction.
    nbits=8;
    sndbuf[0]=0xff;
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    nbits=8;
    sndbuf[0]=0xff;
    devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
    //SDR 1 TDI (00) SMASK (01) ;
    nbits=1;
    for(int i=0;i<2;i++)sndbuf[i]=0x00;
    scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,1);
    fclose(fpout); 
  }
}
// routine comparing the readback eprom snapshot against the reference firmware file
int DAQMB::check_eprom_readback(const char *rbkfile, const char *expfile){
  char buf[128];
  int i,j;
  char bexp,bgot,btyp;
  int wrds=0;
  FILE *exp;
  FILE *got;
  char texp[4],tgot[4];
  NBB=0;
  NBBTOT=0;
  exp=fopen(expfile,"r");
  if (exp == NULL) {
    std::cout<<"Open "<<expfile<<" failed. End check_prom_readback"<<std::endl;
    return -1;
  }
  got = fopen(rbkfile,"r");
  if (got == NULL) {
    std::cout<<"Open "<<rbkfile<<" failed. End check_prom_readback."<<std::endl;
    return -1;
  }
  while (fgets(buf,128,got) != NULL){
    sscanf(buf,"%hhx %hhx %hhx %hhx",&tgot[0],&tgot[1],&tgot[2],&tgot[3]);
    fgets(buf,128,exp);
    sscanf(buf,"%hhx %hhx %hhx %hhx",&texp[0],&texp[1],&texp[2],&texp[3]);
    // we must ignore 7th-bit mismatch for every 128th word 
    if( ((wrds+1)%128)==0 ){
      tgot[3] = tgot[3] & 0x7F;
      texp[3] = texp[3] & 0x7F;
    }
    for(i=0;i<4;i++){
      // compare the reference and readback words
      if( tgot[i]!=texp[i] ){
          if(i==0)
              std::cout<<std::setw(6)<<std::setfill('0')<<wrds<<"  got: "<<std::hex
                  <<std::setw(2)<<(tgot[0]&0xff)
                  <<std::setw(2)<<(tgot[1]&0xff)
                  <<std::setw(2)<<(tgot[2]&0xff)
                  <<std::setw(2)<<(tgot[3]&0xff)
                  <<" expect: "
                  <<std::setw(2)<<(texp[0]&0xff)
                  <<std::setw(2)<<(texp[1]&0xff)
                  <<std::setw(2)<<(texp[2]&0xff)
                  <<std::setw(2)<<(texp[3]&0xff)
                  <<std::dec<<std::setfill(' ')<<std::endl;
//        printf("%6d  1 %02x%02x%02x%02x 2 %02x%02x%02x%02x\n",wrds,tgot[0]&0xff,tgot[1]&0xff,tgot[2]&0xff,tgot[3]&0xff,texp[0]&0xff,texp[1]&0xff,texp[2]&0xff,texp[3]&0xff);
        for(j=0;j<16;j++){
          bexp=(texp[i]>>j)&0x01;
          bgot=(tgot[i]>>j)&0x01;
          if(bexp!=bgot){
            NBBTOT++;
            btyp=bgot;
            if(NBB<20){
              NBBwrd[NBB]=wrds;
              NBBbit[NBB]=4*i+16-j+1;
              NBBtyp[NBB]=btyp;
              NBB++;
            }
          }
        }
      }
    }
    wrds++;
  }
  std::cout<< NBBTOT << " errors were found in the readback firmware file" << std::endl;
        // << (NBBTOT>NBB?" (reported words above contain first 20)":"") <<std::endl;
  return 0;
}

void DAQMB::epromload(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum)
{
  //
  std::cout << "New epromload" << std::endl;
  //
  char snd[1024],expect[1024],rmask[1024],smask[1024],cmpbuf[1024];
  DEVTYPE devstp,dv;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int tstusr;
  int nowrit=0;
  // 
  (*MyOutput_) << " epromload " << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;
  //
  if(devnum==ALL){
    devnum=F1PROM;
    devstp=F5PROM;
  }
  else {
    devstp=devnum;
  }
  //
#ifdef OSUcc
  theController->SetUseDelay(true);
#endif
  //
  for(int i=devnum;i<=devstp;i++){
    dv=(DEVTYPE)i;
    xtrbits=geo[dv].sxtrbits;
    //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
    devstr=geo[dv].nam;
    dwnfp    = fopen(downfile,"r");
    if(dwnfp==NULL)
     {   std::cout << "Can't open firmware file " << downfile << std::endl;
         return;
     }
    fpout=fopen("/tmp/eprom.bit","w");
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fpout==NULL)
     {   std::cout << "Can't open eprom.bit file"<< std::endl;
         return;
     }
    printf("Programming Design %s with %s\n",devstr,downfile);
    //
    char bogobuf[8192];
    unsigned long int nlines=0;
    unsigned long int line=1;
    FILE *bogodwnfp=fopen(downfile,"r");
    while (fgets(bogobuf,256,bogodwnfp) != NULL)
      if (strrchr(bogobuf,';')!=0) nlines++;
    fclose(bogodwnfp);
    float percent;
    while (fgets(buf,256,dwnfp) != NULL)  {
      percent = (float)line/(float)nlines;
      if ((line%20)==0) printf("<   > Processed line %lu of %lu (%.1f%%)\n",line,nlines,percent*100.0);
      fflush(stdout);
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
	// printf("%s",buf);
      }
      else {
	line++;
	if(strrchr(buf,';')==0){
	  do {
	    lastn=strrchr(buf,'\n');
	    if(lastn!=0)lastn[0]='\0';
	    if (fgets(buf2,256,dwnfp) != NULL){
	      strcat(buf,buf2);
	    }
	    else {
	     //    printf("End of File encountered.  Quiting\n");
	      return;
	   }
	  }
	  while (strrchr(buf,';')==0);
	}
       for(int i=0;i<1024;i++){
	 cmpbuf[i]=0;
	 sndbuf[i]=0;
	 rcvbuf[i]=0;
       }
       Parse(buf, &Count, &(Word[0]));
       // count=count+1;
       if(strcmp(Word[0],"SDR")==0){
	 cmpflag=0;    //disable the comparison for no TDO SDR
	 sscanf(Word[1],"%d",&nbits);
	 nbytes=(nbits-1)/8+1;
	 for(int i=2;i<Count;i+=2){
	   if(strcmp(Word[i],"TDI")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
	     }
	     if(nowrit==1&&((cbrdnum[0]!=0)||(cbrdnum[1]!=0))) {
	       tstusr=0;
	       snd[0]=cbrdnum[0];
	       snd[1]=cbrdnum[1];
	       snd[2]=cbrdnum[2];
	       snd[3]=cbrdnum[3];
	     }
	     if(nowrit==1){
	       //  printf(" snd %02x %02x %02x %02x \n",snd[0],snd[1],snd[2],snd[3]);
	       //FOO[0]=((snd[3]&0x000000ff)<<24)|((snd[2]&0x000000ff)<<16)|((snd[1]&0x000000ff)<<8)|(snd[0]&0x000000ff);
               // printf(" FOO %08x \n",FOO[0]);
	     }
	   }
	   if(strcmp(Word[i],"SMASK")==0){
	     for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
	     }
	   }
	   if(strcmp(Word[i],"TDO")==0){
	     cmpflag=1;
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
	   }
	   if(strcmp(Word[i],"MASK")==0){
	     for(j=0;j<nbytes;j++){
	       sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
	     }
	   }
	 }
	 for(int i=0;i<nbytes;i++){
	   //sndbuf[i]=snd[i]&smask[i];
	   sndbuf[i]=snd[i]&0xff;
          }
	 //   printf("D%04d",nbits+xtrbits);
          // for(i=0;i<(nbits+xtrbits)/8+1;i++)printf("%02x",sndbuf[i]&0xff);printf("\n");
	 //         printf(" geo[dv].jchan %d \n \n",(geo[dv].jchan));
	 //         sleep(10);
 	 if(nowrit==0){
             if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	       // printf(" Emergency load SDR \n");
             }else{
                   scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
             }
	 }else{
	   if(writ==1) {
	     if((geo[dv].jchan==11)){
	       scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }else{ 
	       scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	     }
	   }
	 } 
	 //  Data readback comparison here:
	 for (int i=0;i<nbytes;i++) {
	   tmp=(rcvbuf[i]>>3)&0x1F;
	   rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
	   /*  if (((rcvbuf[i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1) 
		printf("read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcvbuf[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF); */
	 }
	 if (cmpflag==1) {
	   for (int i=0;i<nbytes;i++) {
	     fprintf(fpout," %02X",rcvbuf[i]&0xFF);
	     if (i%4==3) fprintf(fpout,"\n");
	   }
	 }
       }
       //
       else if(strcmp(Word[0],"SIR")==0){
	 nowrit=0;
	 sscanf(Word[1],"%d",&nbits);
          nbytes=(nbits-1)/8+1;
          for(int i=2;i<Count;i+=2){
            if(strcmp(Word[i],"TDI")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
              }
              if(nbytes==1){if(0xfd==(snd[0]&0xff))nowrit=1;} // nowrit=1  
            }
            else if(strcmp(Word[i],"SMASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
              }
            }
            if(strcmp(Word[i],"TDO")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
              }
            }
            else if(strcmp(Word[i],"MASK")==0){
              for(j=0;j<nbytes;j++){
                sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
              }
            }
          }
          for(int i=0;i<nbytes;i++){
            //sndbuf[i]=snd[i]&smask[i];
            sndbuf[i]=snd[i];
          }
          if(nowrit==0){
  	      devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
	  }
          else{
            if(writ==1) {devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
            if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
          }
	  /*
          printf("send %2d instr bits %02X %02X %02X %02X %02X\n",nbits,sndbuf[4]&0xFF,sndbuf[3]&0xFF,sndbuf[2]&0xFF,sndbuf[1]&0xFF,sndbuf[0]&0xFF);
          printf("expect %2d instr bits %02X %02X %02X %02X %02X\n",nbits,expect[4]&0xFF,expect[3]&0xFF,expect[2]&0xFF,expect[1]&0xFF,expect[0]&0xFF);
	  */
        }
        else if(strcmp(Word[0],"RUNTEST")==0){
          sscanf(Word[1],"%d",&pause);
	  // printf("RUNTEST = %d\n",pause);
	  //usleep(pause);
	  /*   ipd=83*pause;
          // sleep(1);
          t1=(double) clock()/(double) CLOCKS_PER_SEC;
          for(i=0;i<ipd;i++);
          t2=(double) clock()/(double) CLOCKS_PER_SEC;
	  //  if(pause>1000)printf("pause = %f s  while erasing\n",t2-t1); */
	  //          for (i=0;i<pause/100;i++)
	  //  devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
          pause=pause;
	  if (pause<100000) pause=2*pause+100;
          if (pause>65535) {
            sndbuf[0]=255;
            sndbuf[1]=255;
            for (int looppause=0;looppause<pause/65536;looppause++) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);}
            pause=65535;
	  }
          sndbuf[0]=pause-(pause/256)*256;
          sndbuf[1]=pause/256;
	  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
	  usleep(pause);
          // printf(" send sleep \n");  
	  /* printf("pause      %d us\n",pause);*/
	  //#ifdef OSUcc
	  //theController->flush_vme();
	  //#endif OSUcc
        }
        else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
	  //  printf("goto reset idle state\n"); 
	   devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
	   //#ifdef OSUcc
	   //theController->flush_vme();
	   //#endif OSUcc
        }
       else if(strcmp(Word[0],"TRST")==0){
       }
       else if(strcmp(Word[0],"ENDIR")==0){
       }
       else if(strcmp(Word[0],"ENDDR")==0){
       }
      }
    }
    fclose(fpout);
    fclose(dwnfp);
  }
  //
  //#ifdef OSUcc
  //theController->flush_vme();
  //#endif OSUcc
  //
  theController->send_last();
  //
#ifdef OSUcc
  theController->SetUseDelay(false);
#endif
  //
  //sndbuf[0]=0x01;
  //sndbuf[1]=0x00;
  // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
  //devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
  //
}

void DAQMB::epromload_broadcast(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum, int ipass)
{
  //
  std::cout << "Broadcast epromload" << std::endl;
  //
  char snd[1024],expect[1024],rmask[1024],smask[1024],cmpbuf[1024];
  DEVTYPE dv;
  char *devstr;
  FILE *dwnfp,*fpout;
  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  int Count,j,nbits,nbytes,pause,xtrbits;
  int tmp,cmpflag;
  int nowrit;
  // 
  int pass;
  pass=1;
  nowrit=0;
  std::cout <<"IPASS: "<<ipass<<" PASS: "<<pass<<std::endl;
  (*MyOutput_) << " epromload " << std::endl;
  (*MyOutput_) << " devnum    " << devnum << std::endl;

  std::cout <<"IPASS: "<<ipass<<" PASS: "<<pass<<std::endl;

/* ipass acts as a hiccup.
ipass == 1 - load up to the part where you have to load the board number
ipass == 2 - load only the board number
ipass == 3 - load only the stuff after the board number
*/
  if(devnum==ALL){
    std::cout <<" Please load individual CFEBs, devnum==ALL is not supported !"<<std::endl;
    return;
  }
  //
#ifdef OSUcc
  theController->SetUseDelay(true);
#endif

  dv=(DEVTYPE)devnum;
  xtrbits=geo[dv].sxtrbits;
  //    printf(" ************************** xtrbits %d geo[dv].sxtrbits %d \n",xtrbits,geo[dv].sxtrbits);
  devstr=geo[dv].nam;
  dwnfp    = fopen(downfile,"r");
    if(dwnfp==NULL)
     {   std::cout << "Can't open firmware file " << downfile << std::endl;
         return;
     }
 
    fpout=fopen("/tmp/eprom.bit","w");
    chmod("/tmp/eprom.bit",S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  printf("Programming Design %s with %s\n",devstr,downfile);

  //switching to the proper device
  //two delays are used to force the device change in CFEB loading
  char dxbuf[2];
  dxbuf[0]=1;
  dxbuf[1]=2;
  devdo(F2PROM,-99,dxbuf,0,dxbuf,rcvbuf,1);
  devdo(dv,-99,dxbuf,0,dxbuf,rcvbuf,1);

  char bogobuf[8192];
  unsigned long int nlines=0;
  unsigned long int line=1;
  FILE *bogodwnfp=fopen(downfile,"r");
  while (fgets(bogobuf,256,bogodwnfp) != NULL) 
    if (strrchr(bogobuf,';')!=0) nlines++;
  fclose(bogodwnfp);
  float percent;
  while (fgets(buf,256,dwnfp) != NULL)  {
    percent = (float)line/(float)nlines;
    if ((line%20)==0) 
      printf("<   > Processed line %lu of %lu (%.1f%%) Ipass %d pass %d NOWRIT %d\n",line,nlines,percent*100.0,ipass,pass,nowrit);
    fflush(stdout);
    if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!'){
      //  printf("%s",buf);
    }
    else {
      line++;
      if(strrchr(buf,';')==0){
        do {
          lastn=strrchr(buf,'\n');
          if(lastn!=0)lastn[0]='\0';
          if (fgets(buf2,256,dwnfp) != NULL){
            strcat(buf,buf2);
          }
          else {
           //    printf("End of File encountered.  Quiting\n");
            return;
          }
        }
        while (strrchr(buf,';')==0);
      }
      for(int i=0;i<1024;i++){
        cmpbuf[i]=0;
        sndbuf[i]=0;
        rcvbuf[i]=0;
      }
      Parse(buf, &Count, &(Word[0]));
      // count=count+1

      if(strcmp(Word[0],"SDR")==0){
	cmpflag=0;    //disable the comparison for no TDO SDR
        sscanf(Word[1],"%d",&nbits);
        nbytes=(nbits-1)/8+1;

        for(int i=2;i<Count;i+=2){
          if(strcmp(Word[i],"TDI")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
            }
            if(nowrit==1)
	      {
              snd[0]=cbrdnum[0];
              snd[1]=cbrdnum[1];
              snd[2]=cbrdnum[2];
              snd[3]=cbrdnum[3];
	      pass++;
            }
          }
          if(strcmp(Word[i],"SMASK")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
            }
          }
          if(strcmp(Word[i],"TDO")==0){
            cmpflag=1;
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
            }
          }
          if(strcmp(Word[i],"MASK")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
            }
          }
        }
        for(int i=0;i<nbytes;i++){
          //sndbuf[i]=snd[i]&smask[i];
          sndbuf[i]=snd[i]&0xff;
        }
        if(nowrit==0){
          if((geo[dv].jchan==11)){
            if (pass==ipass) scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
            if (pass==2) pass++;
             // printf(" Emergency load SDR \n");
          }else{
            if (pass==ipass) scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
            if (pass==2) pass++;
          }
        }else{
          if(writ==1) {
            if((geo[dv].jchan==11)){
              if (pass==ipass) { 
		/*
                if (ipass==2) {
                  char dxbuf[2];
                  dxbuf[0]=1;
                  dxbuf[1]=2;
                  devdo(dv,-99,dxbuf,0,dxbuf,rcvbuf,1);
		}
		*/
                scan_reset(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
              }
              if (pass==2) pass++;
            }else{ 
              if (pass==ipass) {
		/*
                if (ipass==2) {
                  char dxbuf[2];
                  dxbuf[0]=1;
                  dxbuf[1]=2;
                  devdo(dv,-99,dxbuf,0,dxbuf,rcvbuf,1);
		}
                */
                scan(DATA_REG,sndbuf,nbits+xtrbits,rcvbuf,0);
	      }
              if (pass==2) pass++;
            }
          }
        } 
        //  Data readback comparison here:
        for (int i=0;i<nbytes;i++) {
          tmp=(rcvbuf[i]>>3)&0x1F;
          rcvbuf[i]=tmp | (rcvbuf[i+1]<<5&0xE0);
        }
        if (cmpflag==1) {
          for (int i=0;i<nbytes;i++) {
            fprintf(fpout," %02X",rcvbuf[i]&0xFF);
            if (i%4==3) fprintf(fpout,"\n");
          }
        }
      }
      //
      else if(strcmp(Word[0],"SIR")==0){
        nowrit=0;
        sscanf(Word[1],"%d",&nbits);
        nbytes=(nbits-1)/8+1;
        for(int i=2;i<Count;i+=2){
          if(strcmp(Word[i],"TDI")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&snd[j]);
            }
            if(nbytes==1){
              if(0xfd==(snd[0]&0xff)) {
                nowrit=1;
                std::cout<<" nowrit changed"<<std::endl;
              }
            } // nowrit=1  
          }
          else if(strcmp(Word[i],"SMASK")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&smask[j]);
            }
          }
          if(strcmp(Word[i],"TDO")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&expect[j]);
            }
          }
          else if(strcmp(Word[i],"MASK")==0){
            for(j=0;j<nbytes;j++){
              sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2hhx",&rmask[j]);
            }
          }
        }
        for(int i=0;i<nbytes;i++){
          //sndbuf[i]=snd[i]&smask[i];
          sndbuf[i]=snd[i];
        }
        if(nowrit==0){
          if (pass==ipass) devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);
        }
        else{
          if(writ==1 && pass==ipass) {devdo(dv,nbits,sndbuf,0,sndbuf,rcvbuf,0);}
          if(writ==0)printf(" ***************** nowrit %02x \n",sndbuf[0]);
        }
      }
      else if(strcmp(Word[0],"RUNTEST")==0){
        sscanf(Word[1],"%d",&pause);
        pause=pause;
        if (pause<100000) pause=2*pause+100;
        if (pause>65535) {
          sndbuf[0]=255;
          sndbuf[1]=255;
          for (int looppause=0;looppause<pause/65536;looppause++) {
            if (pass==ipass) {
              devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
              usleep(65535);
            }
          }
          pause=65535;
        }
        sndbuf[0]=pause-(pause/256)*256;
        sndbuf[1]=pause/256;
        // printf(" sndbuf %d %d %d \n",sndbuf[1],sndbuf[0],pause);
        if (pass==ipass) {
          devdo(dv,-99,sndbuf,0,sndbuf,rcvbuf,1);
          usleep(pause);
        }
      }
      else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0)&&(strcmp(Word[2],"IDLE;")==0)){
        //  printf("goto reset idle state\n"); 
        devdo(dv,-1,sndbuf,0,sndbuf,rcvbuf,2);
        //#ifdef OSUcc
        //theController->flush_vme();
        //#endif OSUcc
      }
      else if(strcmp(Word[0],"TRST")==0){
      }
      else if(strcmp(Word[0],"ENDIR")==0){
      }
      else if(strcmp(Word[0],"ENDDR")==0){
      }
    }
  }
  fclose(fpout);
  fclose(dwnfp);
  theController->send_last();

#ifdef OSUcc
  theController->SetUseDelay(false);
#endif
}


void Parse(char *buf,int *Count,char **Word)
{

  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;
    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  *buf = '\0';
}



void DAQMB::rdbkvirtexII()
{
  if(hardware_version_<=1)
  {
  char a[4],b[4]; 
  #define clbword 2494 //(x32)
  int i,k,totbits,totbytes;
  char sndbuf2[36];
  int ival;
  FILE *fp=fopen("rbk.dat","w");
 
  DEVTYPE dv=MCTRL;

  for(k=0;k<1;k++){
      cmd[0]=VTX2_CFG_IN;

      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66; 
     // write to FLR register
      sndbuf[8]=0x0C;
      sndbuf[9]=0x80;
      sndbuf[10]=0x06;
      sndbuf[11]=0x80;
      // fill frame length
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x6a;  //AA 
      // write to CMD register
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      // RCRC
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0xe0;  
      //write to CMD register, READBACK command
      sndbuf[24]=0x0C;
      sndbuf[25]=0x00;
      sndbuf[26]=0x01;
      sndbuf[27]=0x80;
      //register value for RCFG
      sndbuf[28]=0x00;
      sndbuf[29]=0x00;
      sndbuf[30]=0x00;
      sndbuf[31]=0x20;
      //send to FAR
      sndbuf[32]=0x0C;
      sndbuf[33]=0x00;
      sndbuf[34]=0x04;
      sndbuf[35]=0x80;
      //starting CLB
      ival=0;
      a[3]=(ival&0x000000ff);
      a[2]=(ival&0x0000ff00)>>8;
      a[1]=(ival&0x00ff0000)>>16;
      a[0]=0x00;
      shuffle(a,b);
      for(i=0;i<4;i++)sndbuf[36+i]=b[3-i];
      //read from RDRO register
      sndbuf[40]=0x14;
      sndbuf[41]=0x00;
      sndbuf[42]=0x06;
      sndbuf[43]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      // xc2v500 480009be
      a[0]=0x48;
      a[1]=0x01;
      a[2]=0x37;
      a[3]=0xc0;
      shuffle(a,b); 
      for(i=0;i<4;i++)sndbuf[44+i]=b[3-i];
      /*  sndbuf[44]=0x12;
      sndbuf[45]=0x80; //00;
      sndbuf[46]=0x93; //00;
      sndbuf[47]=0x51; //96; */
      //flush pipe
      sndbuf[48]=0x00;
      sndbuf[49]=0x00;
      sndbuf[50]=0x00;
      sndbuf[51]=0x00;
      //durkin added more flush pipe
      sndbuf[52]=0x00;
      sndbuf[53]=0x00;
      sndbuf[54]=0x00;
      sndbuf[55]=0x00;
 
      for(i=0;i<52;i++)sndbuf2[i]=sndbuf[i+4]; 
      devdo(dv,6,cmd,416,sndbuf2,rcvbuf,0);
      cmd[0]=VTX2_CFG_OUT;
        totbytes=4*clbword;
        totbits=8*totbytes;
        devdo(dv,6,cmd,2752,sndbuf,rcvbuf,1);
	/*  bits=0;
    for(i=0;i<totbytes;i++){
      for(j=0;j<8;j++) { 
        if (bits>0) fprintf(fp,"%d",(rtn_ptr[i]>>j)&1);
        bits++;
        if (bits>2 && (bits-3)%32==0) fprintf(fp,"\n"); 
        if (bits==totbits) fprintf(fp,"0\n");
      }
      } */
    }
    for(k=0;k<928;k++){
      cmd[0]=VTX2_CFG_IN;

      sndbuf[0]=0xFF;
      sndbuf[1]=0xFF;
      sndbuf[2]=0xFF;
      sndbuf[3]=0xFF;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66; 
     // write to FLR register
      sndbuf[8]=0x0C;
      sndbuf[9]=0x80;
      sndbuf[10]=0x06;
      sndbuf[11]=0x80;
      // fill frame length
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x6a; 
      // write to CMD register
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      // RCRC
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0xe0;  
      //write to CMD register, READBACK command
      sndbuf[24]=0x0C;
      sndbuf[25]=0x00;
      sndbuf[26]=0x01;
      sndbuf[27]=0x80;
      //register value for RCFG
      sndbuf[28]=0x00;
      sndbuf[29]=0x00;
      sndbuf[30]=0x00;
      sndbuf[31]=0x20;
      //read from RDRO register
      sndbuf[32]=0x14;
      sndbuf[33]=0x00;
      sndbuf[34]=0x06;
      sndbuf[35]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      // xc2v500 480009be
      a[0]=0x48;
      a[1]=0x01;
      a[2]=0x37;
      a[3]=0xc0;
      shuffle(a,b); 
      for(i=0;i<4;i++)sndbuf[36+i]=b[3-i];

      //flush pipe
      sndbuf[40]=0x00;
      sndbuf[41]=0x00;
      sndbuf[42]=0x00;
      sndbuf[43]=0x00;
      //durkin added more flush pipe
      sndbuf[44]=0x00;
      sndbuf[45]=0x00;
      sndbuf[46]=0x00;
      sndbuf[47]=0x00;
 
      for(i=0;i<40;i++)sndbuf2[i]=sndbuf[i+8]; 
      devdo(dv,6,cmd,320,sndbuf2,rcvbuf,0);
      cmd[0]=VTX2_CFG_OUT;
        totbytes=4*clbword;
        totbits=8*totbytes;
        devdo(dv,6,cmd,2752,sndbuf,rcvbuf,1);
        for(i=0;i<344;i++)fprintf(fp,"%02x",rcvbuf[i]&0xff);fprintf(fp,"\n");
    cmd[0]=VTX2_BYPASS;
    devdo(dv,6,cmd,0,sndbuf,rcvbuf,0);
    
  }
  fclose(fp);
  return;
  }
}

void shuffle(char *a,char *b)
{
int i,j,k;
char msk;
for(k=0;k<4;k++)b[k]=0x00;
for(i=0;i<4;i++){
  j=3-i;
  for(k=0;k<8;k++){
    msk=(0x01<<k);
    msk=msk&a[i];
    if(msk!=0x00)b[j]=b[j]|(0x80>>k);
  }
}
}


void DAQMB::rdbkvirtex(DEVTYPE devnum)  //FEB FPGA 1-5 or  F1SCAM->F5SCAM
{
  if(hardware_version_<=1)
  {
  const int clbword2 = 15876;
  //const int bramword = 780;   // That is just for XCV50
  int i,j,totbits,totbytes,bits;
  char sndbuf2[36];
  int ibstr,ibstr2,ival,imod,imod2,ival2;
  char cval;
  FILE * fp=fopen("rbk.dat","w");
 
  DEVTYPE dv=devnum;

      cmd[0]=VTX_CFG_IN;

      sndbuf[0]=0x00;
      sndbuf[1]=0x00;
      sndbuf[2]=0x00;
      sndbuf[3]=0x00;
      sndbuf[4]=0x55;
      sndbuf[5]=0x99;
      sndbuf[6]=0xAA;
      sndbuf[7]=0x66;
      //send to FAR
      sndbuf[8]=0x0C;
      sndbuf[9]=0x00;
      sndbuf[10]=0x04;
      sndbuf[11]=0x80;
      //starting CLB
      sndbuf[12]=0x00;
      sndbuf[13]=0x00;
      sndbuf[14]=0x00;
      sndbuf[15]=0x00;
      //write to CMD register, READBACK command
      sndbuf[16]=0x0C;
      sndbuf[17]=0x00;
      sndbuf[18]=0x01;
      sndbuf[19]=0x80;
      //register value for RCFG
      sndbuf[20]=0x00;
      sndbuf[21]=0x00;
      sndbuf[22]=0x00;
      sndbuf[23]=0x20;
      //read from RDRO register
      sndbuf[24]=0x14;
      sndbuf[25]=0x00;
      sndbuf[26]=0x06;
      sndbuf[27]=0x00;
      //VTX CLB word counter xcv50 15876 words
      // xcv50 48003e04
      // xcv151 480076b0
      sndbuf[28]=0x12;
      sndbuf[29]=0x00;
      sndbuf[30]=0x7C;
      sndbuf[31]=0x20;
      //flush pipe
      sndbuf[32]=0x00;
      sndbuf[33]=0x00;
      sndbuf[34]=0x00;
      sndbuf[35]=0x00;
      sndbuf[36]=0x00;
      // bits have to be shifed for 3 bit delay
      for(i=0;i<36;i++)sndbuf2[i]=0x00;
      ibstr=1;
      ibstr2=0;
LOOP:     
      imod=ibstr-8*(ibstr/8);
      ival=ibstr/8;
      imod2=ibstr2-8*(ibstr2/8);
      ival2=ibstr2/8;
      cval=((sndbuf[ival]>>imod)&0x01);
      if(cval==0x01){
          sndbuf2[ival2]=sndbuf2[ival2]+int(pow(2.0,imod2));
      }
      ibstr=ibstr+1;
      ibstr2=ibstr2+1;
      if(ibstr<288)goto LOOP;
      //   for(i=0;i<10;i++)printf(" %d %02x \n",i,sndbuf2[i]);
      devdo(dv,5,cmd,288-1,sndbuf2,rcvbuf,0);
      cmd[0]=VTX_CFG_OUT;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      totbytes=4*clbword2;
      totbits=8*totbytes;
      // char * data_ptr = new char;
      // char * rtn_ptr = new char;
      // data_ptr=(char *)malloc((clbword2*4+1),sizeof(char));
      // rtn_ptr=(char *)malloc((clbword2*4+1),sizeof(char));
      //@@ looks wrong!
      char data_ptr[63505];
      char rtn_ptr[63505];
      scan(DATA_REG,data_ptr,totbits+2,rtn_ptr,1);
      
      /*     bits=0;
      for(i=0;i<totbytes;i++){
	for(j=0;j<8;j++) { 
	  if (bits>0) fprintf(fp,"%d",(rtn_ptr[i]>>j)&1);
	  bits++;
	  if (bits>2 && (bits-3)%32==0) fprintf(fp,"\n"); 
	  if (bits==totbits) fprintf(fp,"0\n");
	}
	} */
      bits=0;
      for(i=0;i<totbytes;i++){
        for(j=0;j<8;j++){
           fprintf(fp,"%d",(rtn_ptr[i]>>j)&1);
           bits++;
           if(bits%32==0)fprintf(fp,"\n");
           if(bits==totbits)fprintf(fp,"\n");
        }
      }
      //    delete data_ptr;
      //  delete rtn_ptr;
      //  printf("\n Total bits read %d \n",totbits);
      cmd[0]=VTX_BYPASS;
      devdo(dv,5,cmd,0,sndbuf,rcvbuf,0);
      
      // free(rtn_ptr);
      // free(data_ptr);
      
      //  printf(" SCA Master Configure file is read back. \n\n");
      fclose(fp);
  }
}


void DAQMB::shift_all(int mode) {
  for(int brd=0;brd<7;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=mode;
      }
    }
  }
}


unsigned int DAQMB::unpack_ibrd() const {
  unsigned int ibrd=0x00000000;
  return ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)
      |((rcvbuf[3]&0xff)<<24)|ibrd;
}


unsigned int DAQMB::unpack_ival() const {
  return ((rcvbuf[1]<<8)&0xff00)|(rcvbuf[0]&0xff);
}


void DAQMB::executeCommand(std::string command) {
  #ifdef debugV
    (*MyOutput_) << "inside DAQMB executeCommand"<< std::endl;
  #endif
  if(command == "BuckeyeShiftTest")  buck_shift_test();
/*
  if(command == "fifo")                fifo();
  if(command == "dump")                dump();
  if(command == "bucktest")            bucktest();
  if(command == "daqmb_init")          daqmb_init();
  if(command == "cfeb_pulse")          cfeb_pulse();
  if(command == "cfeb_inject")         cfeb_inject();
  if(command == "cfeb_pedestals")      cfeb_pedestals();
  if(command == "lowv_dump")           lowv_dump();
  if(command == "daqmb_adc_dump")      daqmb_adc_dump();
  if(command == "daqmb_promfpga_dump") daqmb_promfpga_dump();
*/
}

void DAQMB::toggle_caltrg()
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=11;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  printf("Toggled Cal_trigger_generation_disable in MCTRL FPGA\n");
  }
}

void DAQMB::set_ext_chanx(int schan, int feb)
{
  for(int brd=0;brd<7;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=NORM_RUN;
      }
      if(feb<0 || feb==brd) shift_array[brd][chip][schan]=EXT_CAP;
    }
  }
}


void DAQMB::setpulsedelay(int tinj){
  int cal_delay_bits;
  if(tinj==-1){
     cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
  }else{
     cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (tinj & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
  }
  std::cout << "DAQMB:configure: caldelay " << std::hex << cal_delay_bits << std::dec << std::endl;
   setcaldelay(cal_delay_bits);
}

void DAQMB::set_rndmtrg_rate(int rate)
{
  if(hardware_version_<=1)
  {
  if (rate<0) rate=0x2db6d;
  cmd[0]=VTX2_USR1;
  sndbuf[0]=TRG_RATE;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR2;
  sndbuf[0]=rate&0xff;
  sndbuf[1]=(rate>>8)&0xff;
  sndbuf[2]=(rate>>16)&0xff;
  devdo(MCTRL,6,cmd,18,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;   // NO_OP mode
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  std::cout<< "Random Trigger Rate set to: "<<std::hex<<rate<<std::endl;
  }
}

void DAQMB::toggle_rndmtrg_start()
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=RTRG_TGL;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);

  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;   // NO_OP mode
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
  printf("Toggled RandomTrigger Start in MCTRL FPGA, RTRG_TGL %02x\n",RTRG_TGL);
  }
}
//
void DAQMB::burst_rndmtrg()
{
  toggle_rndmtrg_start();
  usleep(5000);
  toggle_rndmtrg_start();
}
//
void DAQMB::sfm_test_load(char *sndpat)
{    
  if(hardware_version_<=1)
  {
  //Program SFM with 0,1,2,3,...,263
  cmd[0]=VTX2_USR1;
  sndbuf[0]=35;   //Serial Flash Memory TEST
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0x41;  //bit reversed for 82H
  sndbuf[1]=0xc0;
  sndbuf[2]=0x3c;
  sndbuf[3]=0x00;
  for (int i=0;i<264;i++) sndbuf[i+4]=sndpat[i];
  devdo(MCTRL,6,cmd,2144,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
  }
}
//
void DAQMB::sfm_test_read(char *rcvpat)
{
  if(hardware_version_<=1)
  {
  int boffset;
  //Read SFM 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=35;   //Serial Flash Memory TEST
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0x4B;  //bit reversed for D2H
  sndbuf[0]=0x4A;  //bit reversed for 52H
  sndbuf[1]=0xc0;
  sndbuf[2]=0x3c;
  sndbuf[3]=0x00;
  for (int i=0;i<268;i++) sndbuf[i+4]=0;  //264 data bytes plus 4 dummy bytes
  devdo(MCTRL,6,cmd,2176,sndbuf,rcvbuf,1);
  boffset=2;
  for(int i=0;i<256;i++){
    // printf("shfttst %d %02X \n",i,rcvbuf[i]&0xFF);
    rcvpat[i]=0x00;
    rcvpat[i]=rcvpat[i]|(rcvbuf[i+9]<<(8-boffset));
    rcvpat[i]=rcvpat[i]|((rcvbuf[i+8]>>boffset)&~(~0<<(8-boffset)));
  }
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
  }
}
//

void DAQMB::cbldly_init(){
  if(hardware_version_<=1)
  {
	 printf(" Initialize \n");
         cmd[0]=VTX_USR1; 
         sndbuf[0]=0x1A;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX_USR1;
         sndbuf[0]=NOOP;
         devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
         cmd[0]=VTX2_BYPASS;
         devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::trigset2(int nset, int iuse[5])
{
  if(hardware_version_<=1)
  {
  int i,j,k;
  char tsndbuf[512];
  //
  //  printf(" Loading Motherboard trigger register...");
  /* load trigger pattern */
  //
  for(i=0;i<512;i++){
    tsndbuf[i]=0;
  }
  //
  /*
  if(nset<2)tsndbuf[4]=0x03;
  if(nset<3)tsndbuf[10]=0x03;
  if(nset<4)tsndbuf[44]=0x03;
  */
  if (nset>0) tsndbuf[4]=0x03;
  if (nset>1) tsndbuf[21]=0x03;
  //
  //  if (nset>1) tsndbuf[40]=0x03;
  //  if (nset>1) tsndbuf[60]=0x03;
  //
  if (nset>2) tsndbuf[44]=0x03;
  if (nset>3) tsndbuf[64]=0x03;
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_TRIG;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0); 
  for(i=0;i<512;i++)sndbuf[i]=tsndbuf[i]&0xFF;
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,4096,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=LOAD_STR;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  j=0;
  k=1;
  for(i=0;i<5;i++){if(iuse[i]==1)j=j+k;k=k*2;}
  //  printf(" here is what we set %d \n",j);
  sndbuf[0]=j;
  devdo(MCTRL,6,cmd,5,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}


void DAQMB::trgfire()
{
  if(hardware_version_<=1)
  {
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CYCLE_TRIG;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
  /*    printf(" Returned from CYCLYE_TRIG instr: %02X  \n",rcvbuf[0]&0xFF); */
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,1);
  /*  printf(" Returned from NOOP instr: %02X  \n",rcvbuf[0]&0xFF);*/
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  }
}

void DAQMB::cbldly_trig(){
  if(hardware_version_<=1)
  {
  printf(" Trigger Once \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x03;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::cbldly_loadfinedelay(){
  if(hardware_version_<=1)
  {
  //
  printf(" Load Fine Delay \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x15;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}

void DAQMB::cbldly_programSFM(){
  if(hardware_version_<=1)
  {
  (*MyOutput_) <<" Program Serial Flash Memory" << std::endl ;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x18;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::cbldly_wrtprotectSFM(){
  if(hardware_version_<=1)
  {
  (*MyOutput_) << " SFM Write Protect" <<std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1e; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::cbldly_loadmbidSFM(){
  if(hardware_version_<=1)
  {
  printf(" Load DAQMB ID to SFM  \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x16;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::cbldly_loadcfebdlySFM(){
  if(hardware_version_<=1)
  {
  printf(" Load CFEB clock delay to SFM \n"); 
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x17;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

void DAQMB::cbldly_refreshcfebdly(){
  if(hardware_version_<=1)
  {
  printf(" Refresh Onboard CFEB delay \n");
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1d;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  }
}

//
void DAQMB::devdoReset(){
/// used for emergency loading
  devdo(RESET,-1,cmd,0,sndbuf,rcvbuf,2);
}
//
std::string DAQMB::CounterName(int counter){
  //
  std::string name = "NO counter found" ;
  //
  if ( counter == 0 ) name = "L1A to LCT delay";
  if ( counter == 1 ) name = "CFEB DAV delay  ";
  if ( counter == 2 ) name = "TMB DAV delay   ";
  if ( counter == 3 ) name = "ALCT DAV delay  ";
  if ( counter == 4 ) name = "L1A to LCT Scope";
  if ( counter == 5 ) name = "CFEB DAV Scope  ";
  if ( counter == 6 ) name = "TMB DAV Scope   ";
  if ( counter == 7 ) name = "ALCT DAV Scope  ";
  //
  return name;
  //
}
//
void DAQMB::PrintCounters(){
  //
  readtimingCounter();
  //
  readtimingScope();
  //
  printf("  L1A to LCT delay: %d", GetL1aLctCounter()  ); printf(" CMS clock cycles \n");
  printf("  CFEB DAV delay:   %d", GetCfebDavCounter() ); printf(" CMS clock cycles \n");
  printf("  TMB DAV delay:    %d", GetTmbDavCounter()  ); printf(" CMS clock cycles \n");
  printf("  ALCT DAV delay:   %d", GetAlctDavCounter() ); printf(" CMS clock cycles \n");
  //
  std::cout << std::endl ;
  //
  std::cout << "  L1A to LCT Scope: " ;
  std::cout << std::setw(3) << GetL1aLctScope() << " " ;
  for( int i=4; i>-1; i--) std::cout << ((GetL1aLctScope()>>i)&0x1) ;
  std::cout << std::endl ;
  //
  std::cout << "  CFEB DAV Scope:   " ;
  std::cout << std::setw(3) << GetCfebDavScope() << " " ;
  for( int i=4; i>-1; i--) std::cout << ((GetCfebDavScope()>>i)&0x1) ;
  std::cout << std::endl ;
  //
  std::cout << "  TMB DAV Scope:    " ;
  std::cout << std::setw(3) << GetTmbDavScope() << " " ;
  for( int i=4; i>-1; i--) std::cout << ((GetTmbDavScope()>>i)&0x1) ;
  std::cout << std::endl ;
  //
  std::cout << "  ALCT DAV Scope:   " ;
  std::cout << std::setw(3) << GetAlctDavScope() << " " ;
  for( int i=4; i>-1; i--) std::cout << ((GetAlctDavScope()>>i)&0x1) ;
  std::cout << std::endl ;
  //
  std::cout << "  Active DAV Scope: " ;
  std::cout << std::setw(3) << GetActiveDavScope() << " " ;
  for( int i=4; i>-1; i--) std::cout << ((GetActiveDavScope()>>i)&0x1) ;
  std::cout << std::endl ;
  //
}

void DAQMB::readtimingCounter()
{
  if(hardware_version_<=1)
  {
  //
  //printf(" Entered READ_TIMING \n");
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=36;      //F36 in DMB6cntl, July 5, 2005
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  usleep(200);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,1);
  //
  l1a_lct_counter_  = rcvbuf[0]&0xff ;
  cfeb_dav_counter_ = rcvbuf[1]&0xff ;
  tmb_dav_counter_  = rcvbuf[2]&0xff ;
  alct_dav_counter_ = rcvbuf[3]&0xff ;
  memcpy(FinalCounter, rcvbuf, 4);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  usleep(200);
  //
  }
}

void DAQMB::readtimingScope()
{
  if(hardware_version_<=1)
  {
  //printf(" Entered READ_TIMING \n");
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=38;      //F38
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  usleep(200);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,1);
  //
  cfeb_dav_scope_   = rcvbuf[0]&0x1f ;
  tmb_dav_scope_    = ((rcvbuf[1]<<3)&0x18) + ((rcvbuf[0]>>5)&0x07);
  alct_dav_scope_   = (rcvbuf[1]>>2)&0x1f;
  active_dav_scope_ = ((rcvbuf[2]<<1)&0x1e)+((rcvbuf[1]>>7)&0x01);  
  l1a_lct_scope_    = ((rcvbuf[3]<<4)&0x10) + ((rcvbuf[2]>>4)&0x0f);

  FinalCounter[4]=cfeb_dav_scope_ & 0xff;
  FinalCounter[5]=tmb_dav_scope_ & 0xff;
  FinalCounter[6]=alct_dav_scope_ & 0xff;
  FinalCounter[7]=active_dav_scope_ & 0xff;
  FinalCounter[8]=l1a_lct_scope_ & 0xff;

  //
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
  usleep(200);
  //
  }
}

char * DAQMB::GetCounters()
{ 
  if(hardware_version_<=1)
  {
  //
  //printf(" Entered READ_TIMING \n");
  //
  if(failed_checkvme_<0) checkvme_fail();
  if(failed_checkvme_>0) return NULL;

  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=36;      //F36 in DMB6cntl, July 5, 2005
  new_devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  new_devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
//  vme_delay(200);
//  usleep(200);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  new_devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,2);
  //
  cmd[0]=VTX2_BYPASS;
  new_devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
//  vme_delay(200);
//  usleep(200);
  //
  //printf(" Entered READ_TIMING \n");
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=CAL_STATUS;
  sndbuf[0]=38;      //F38
  new_devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  cmd[0]=VTX2_BYPASS;
  new_devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  //
//  vme_delay(200);
//  usleep(200);
  //
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0;
  sndbuf[1]=0;
  sndbuf[2]=0;
  sndbuf[3]=0;
  new_devdo(MCTRL,6,cmd,32,sndbuf,rcvbuf,2);

  cmd[0]=VTX2_BYPASS;
  new_devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,1);
  //
  l1a_lct_counter_  = rcvbuf[0]&0xff ;
  cfeb_dav_counter_ = rcvbuf[1]&0xff ;
  tmb_dav_counter_  = rcvbuf[2]&0xff ;
  alct_dav_counter_ = rcvbuf[3]&0xff ;
  memcpy(FinalCounter, rcvbuf, 4);
  //
  cfeb_dav_scope_   = rcvbuf[4]&0x1f ;
  tmb_dav_scope_    = ((rcvbuf[5]<<3)&0x18) + ((rcvbuf[4]>>5)&0x07);
  alct_dav_scope_   = (rcvbuf[5]>>2)&0x1f;
  active_dav_scope_ = ((rcvbuf[6]<<1)&0x1e)+((rcvbuf[5]>>7)&0x01);  
  l1a_lct_scope_    = ((rcvbuf[7]<<4)&0x10) + ((rcvbuf[6]>>4)&0x0f);

  FinalCounter[4]=cfeb_dav_scope_ & 0xff;
  FinalCounter[5]=tmb_dav_scope_ & 0xff;
  FinalCounter[6]=alct_dav_scope_ & 0xff;
  FinalCounter[7]=active_dav_scope_ & 0xff;
  FinalCounter[8]=l1a_lct_scope_ & 0xff;

  return (char *)FinalCounter;
  }
  else return NULL;
}

unsigned DAQMB::GetCounter(int counter)
{
  if(counter>=0 && counter<=8) return FinalCounter[counter];
  else return 0;
}

void DAQMB::lowv_dump()
{
  (*MyOutput_) << " CFEB1 OCM 3 = "  << lowv_adc(1,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 OCM 5 = "  << lowv_adc(1,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 OCM 6 = "  << lowv_adc(1,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 3 = "  << lowv_adc(1,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 5 = "  << lowv_adc(1,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 OCM 6 = "  << lowv_adc(1,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 3 = "  << lowv_adc(1,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 5 = "  << lowv_adc(1,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 OCM 6 = "  << lowv_adc(2,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 3 = "  << lowv_adc(2,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 5 = "  << lowv_adc(2,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 OCM 6 = "  << lowv_adc(2,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 3 = "  << lowv_adc(2,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 5 = "  << lowv_adc(2,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 OCM 6 = "  << lowv_adc(2,6)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 3.3 = " << lowv_adc(2,7)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 1.8 = " << lowv_adc(3,0)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 5B = "  << lowv_adc(3,1)/1000 << std::endl;
  (*MyOutput_) << " ALCT OCM 5A = "  << lowv_adc(3,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 3.3V = "   << lowv_adc(3,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 5.0V = "   << lowv_adc(3,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB1 6.0V = "   << lowv_adc(3,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 3.3V = "   << lowv_adc(3,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 5.0V = "   << lowv_adc(3,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB2 6.0V = "   << lowv_adc(4,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 3.3V = "   << lowv_adc(4,1)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 5.0V = "   << lowv_adc(4,2)/1000 << std::endl;
  (*MyOutput_) << " CFEB3 6.0V = "   << lowv_adc(4,3)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 3.3V = "   << lowv_adc(4,4)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 5.0V = "   << lowv_adc(4,5)/1000 << std::endl;
  (*MyOutput_) << " CFEB4 6.0V = "   << lowv_adc(4,6)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 3.3V = "   << lowv_adc(4,7)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 5.0V = "   << lowv_adc(5,0)/1000 << std::endl;
  (*MyOutput_) << " CFEB5 6.0V = "   << lowv_adc(5,1)/1000 << std::endl;
  (*MyOutput_) << " ALCT 3.3V = "    << lowv_adc(5,2)/1000 << std::endl;
  (*MyOutput_) << " ALCT 1.8V = "    << lowv_adc(5,3)/1000 << std::endl;
  (*MyOutput_) << " ALCT 5.5V B = "  << lowv_adc(5,4)/1000 << std::endl;
  (*MyOutput_) << " ALCT 5.5V A = "  << lowv_adc(5,5)/1000 << std::endl;
}




/*********** DAQMB ADC dump ***************/

void DAQMB::daqmb_adc_dump()
{ 
  (*MyOutput_) << " DAC1 Volts = "  << adcplus(2,0) << std::endl;
  (*MyOutput_) << " DAC2 Volts = "  << adcplus(2,1) << std::endl;
  (*MyOutput_) << " DAC3 Volts = "  << adcplus(2,2) << std::endl;
  (*MyOutput_) << " DAC4 Volts = "  << adcplus(2,3) << std::endl;
  (*MyOutput_) << " DAC5 Volts = "  << adcplus(2,4) << std::endl;
  (*MyOutput_) << " 1.8V Chip 1 = " << adcplus(1,6) << std::endl;
  (*MyOutput_) << " 1.8V Chip 2 = " << adcplus(2,6) << std::endl;
  (*MyOutput_) << " 1.8V Chip 3 = " << adcminus(3,6) << std::endl;
  (*MyOutput_) << " DMB GND = "     << adcminus(3,0) << std::endl;
  (*MyOutput_) << " CFEB1 GND = "   << adcminus(3,1) << std::endl;
  (*MyOutput_) << " CFEB2 GND = "   << adcminus(3,2) << std::endl;
  (*MyOutput_) << " CFEB3 GND = "   << adcminus(3,3) << std::endl;
  (*MyOutput_) << " CFEB4 GND = "   << adcminus(3,4) << std::endl;
  (*MyOutput_) << " CFEB5 GND = "   << adcminus(3,5) << std::endl;
  (*MyOutput_) << " Internal Calib. DAC = "  << adcplus(2,7) << std::endl;
  (*MyOutput_) << " External Calib. DAC = "  << adcminus(3,7) << std::endl; 
  (*MyOutput_) << " Precision Calib. DAC = " << adc16(4,0) << std::endl;
  (*MyOutput_) << " DAQMB temperature = "    << readthermx(0) << std::endl;
  (*MyOutput_) << " FEB1 temperature = "     << readthermx(1) << std::endl;
  (*MyOutput_) << " FEB2 temperature = "     << readthermx(2) << std::endl;
  (*MyOutput_) << " FEB3 temperature = "     << readthermx(3) << std::endl;
  (*MyOutput_) << " FEB4 temperature = "     << readthermx(4) << std::endl;
  (*MyOutput_) << " FEB5 temperature = "     << readthermx(5) << std::endl; 
}
 

/*********** DAQMB PROM/FPGA dumps ***************/

void DAQMB::daqmb_promfpga_dump()
{
  (*MyOutput_) << "DMB VPROM usercodes = " << std::hex << mbpromuser(0) << std::endl;
  (*MyOutput_) << "DMB VPROM ID codes = "  << std::hex << mbpromid(0) << std::endl;
  (*MyOutput_) << "DMB MPROM usercodes = " << std::hex << mbpromuser(1) << std::endl;
  (*MyOutput_) << "DMB MPROM ID codes = "       << std::hex << mbpromid(1) << std::endl;
  (*MyOutput_) << "DMB Virtex usercodes = "     << std::hex << mbfpgauser() << std::endl;
  (*MyOutput_) << "DMB Virtex Chip ID codes = " << std::hex << mbfpgaid() << std::endl;
  /*
  std::vector<CFEB> cfebs = cfebs();
  for(unsigned i = 0; i < cfebs.size(); ++i) {
    (*MyOutput_) << " CFEB FPGA usercodes = " << std::hex << febpromuser(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB ISPROM usercodes = " << std::hex << febfpgauser(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB FPGA Chip ID code = " << std::hex << febpromid(cfebs[i]) << std::endl;
    (*MyOutput_) << " CFEB ISPROM Chip ID code = " << std::hex << febfpgaid(cfebs[i]) << std::endl;
  }
  */
  (*MyOutput_) << std::dec;
}
//
void DAQMB::ProgramSFM(){
  if(hardware_version_<=1)
  {
  //
  (*MyOutput_) << " Program Serial Flash Memory" << std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x18;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}
//
void DAQMB::LoadCFEBDelaySFM(){
  if(hardware_version_<=1)
  {
  //
  printf(" Load CFEB clock delay to SFM \n"); 
  cmd[0]=VTX2_USR1; 
  sndbuf[0]=0x17;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}
//
void DAQMB::LoadDMBIdSFM(){
  if(hardware_version_<=1)
  {
  //
  printf(" Load DAQMB ID to SFM  \n");
  cmd[0]=VTX2_USR1; 
  sndbuf[0]=0x16;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}
//
void DAQMB::SFMWriteProtect(){
  if(hardware_version_<=1)
  {
  //
  (*MyOutput_) << " SFM Write Protect" << std::endl;
  cmd[0]=VTX_USR1; 
  sndbuf[0]=0x1e; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}
//
void DAQMB::ToogleBXN(){
  if(hardware_version_<=1)
  {
  //
  cmd[0]=VTX_USR1; 
  sndbuf[0]=34; 
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  }
}
//
void DAQMB::LoadCableDelaySFM()
{
  if(hardware_version_<=1)
  {
  printf(" Load Cable delay \n");
  cmd[0]=VTX2_USR1; 
  sndbuf[0]=0x15;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);  
  }
}
//
void DAQMB::setcbldly(int dword)
{
  if(hardware_version_<=1)
  {
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=28;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF; 
  //
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
  //
  printf(" Cable delay Set to %02X. \n",dword&0xFF);
  //
  // Update
  //
  cmd[0]=VTX2_USR1;
  sndbuf[0]=21;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  //
  }
}

void DAQMB::WriteSFM(){
  //
  //setcrateid(0);
  //
  LoadCableDelaySFM();
  LoadDMBIdSFM();
  LoadCFEBDelaySFM();
  // Enable
  SFMWriteProtect();
  // Write
  ProgramSFM();
  ::sleep(1);
  // Disable
  SFMWriteProtect();
  //
}
//

//
void DAQMB::PrintCounters(int user_option){
  //
  (*MyOutput_) << "Enter 1 for simple print-out" << std::endl;
  (*MyOutput_) << "      2 for print most frequent values" <<std::endl;
  (*MyOutput_) << "      3 for cuts by TMB DAV and/or same DAV, then print most frequent" <<std::endl;
  //
  if(user_option<1 | user_option>3) (*MyOutput_) << "Invalid option entered" << std::endl;
  //
  //Simple read counters option:
  //
  if(user_option==1) {
    //
    readtimingCounter();
    //
    readtimingScope();
    //
    (*MyOutput_) << "  Counters " << std::endl ;
    //
    (*MyOutput_) << "  L1A to LCT delay: " << GetL1aLctCounter()  << std::endl ;
    (*MyOutput_) << "  CFEB DAV delay:   " << GetCfebDavCounter() << std::endl ;
    (*MyOutput_) << "  TMB DAV delay:    " << GetTmbDavCounter()  << std::endl ;
    (*MyOutput_) << "  ALCT DAV delay:   " << GetAlctDavCounter() << std::endl ;
    //
    (*MyOutput_) << std::endl ;
    //
    int trials = 0;
    while ( GetL1aLctScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  L1A to LCT Scope: " ;
    (*MyOutput_) << std::setw(5) << GetL1aLctScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetL1aLctScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetCfebDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  CFEB DAV Scope:   " ;
    (*MyOutput_) << std::setw(5) << GetCfebDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetCfebDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetTmbDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  TMB DAV Scope:    " ;
    (*MyOutput_) << std::setw(5) << GetTmbDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetTmbDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    trials = 0;
    while ( GetAlctDavScope() == 0 && trials < 10 ) {
      readtimingScope();
      trials++;
    }
    //
    (*MyOutput_) << "  ALCT DAV Scope:   " ;
    (*MyOutput_) << std::setw(5) << GetAlctDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetAlctDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    (*MyOutput_) << "  Active DAV Scope: " ;
    (*MyOutput_) << std::setw(5) << GetActiveDavScope() << " " ;
    for( int i=4; i>-1; i--) (*MyOutput_) << ((GetActiveDavScope()>>i)&0x1) ;
    (*MyOutput_) << std::endl ;
    //
    (*MyOutput_) << std::endl ;
    //
  }
  //Loop and choose "best" option:
  else {
    //
    int nloop = 100;
    //
    int davsame;
    int tmbdavcut;
    if(user_option==3) {
      (*MyOutput_) << "Enter value of TMB DAV to cut on (suggest 6, use -1 for no cut):" << std::endl;
      std::cin >> tmbdavcut;
      //
      (*MyOutput_) << "Enter -1 for no cut against same DAV delays or 1 to use cut:" << std::endl;
      std::cin >> davsame;
    }
    //
    //Before looping, zero all of the counters
    //
    int type;
    int delay;
    // counts for [time bin,type] where 
    //       type=0 for LCT-L1A delay, 1 for CFEBDAV, 2 for TMBDAV, 3 for ALCTDAV
    int counts[256][4]; 
    for(type=0;type<4;type++) { 
      for(delay=0;delay<256;delay++) {
	counts[delay][type] = 0;}
    }
    //
    //Next read the counters nloop times and accumulate statistics
    //
    int iloop;
    int passcuts=0;
    for(iloop=0;iloop<nloop;iloop++){
      readtimingCounter();
      //
      int l1alct  = GetL1aLctCounter();
      int cfebdav = GetCfebDavCounter();
      int tmbdav  = GetTmbDavCounter();
      int alctdav = GetAlctDavCounter();
      //
      //	  (*MyOutput_) << "Debug: l1alct= " << l1alct << "cfebdav=" << cfebdav;
      //      (*MyOutput_) << " tmbdav=" <<tmbdav << " alctdav=" << alctdav << std::endl;
      //
      //Pass cuts?
      //Maybe no cuts, or else have to 
      //pass tmbdav value cuts and pass not same cuts
      //
      bool nocuts=(user_option==2);
      bool tmbdavok=(tmbdavcut==-1 || tmbdav==tmbdavcut);
      bool davdiff=(davsame==-1 || (cfebdav!=tmbdav || cfebdav!=alctdav || tmbdav!=alctdav));
      //
      if (nocuts || (tmbdavok && davdiff))
	{
	  passcuts+=1;
	  counts[ l1alct  ][0] += 1;
	  counts[ cfebdav ][1] += 1;
	  counts[ tmbdav  ][2] += 1;
	  counts[ alctdav ][3] += 1;
	}
    }
    //
    //Next analyze the counters to find the most frequent setting
    //
    int maxnum[4],maxdelay[4];
    //
    maxnum[0]=-1;
    maxnum[1]=-1;
    maxnum[2]=-1;
    maxnum[3]=-1;
    //
    // Exclude delay=0 (no meaning!)
    //
    for(delay=1;delay<256;delay++)
      {
	//
	//Debug
	//	    (*MyOutput_) << "Delay=" << delay << " Counts are";
	//	    (*MyOutput_) << "  LCT-L1A : " << counts[delay][0]; 
	//	    (*MyOutput_) << "  CFEB-DAV: " << counts[delay][1];
	//	    (*MyOutput_) << "  TMB-DAV:  " << counts[delay][2];
	//	    (*MyOutput_) << "  ALCT-DAV: " << counts[delay][3] << std::endl; 
	//
	for(type=0;type<4;type++)
	  {
	    if( counts[delay][type] > maxnum[type] )
	      {
		maxnum[type]=counts[delay][type];
		maxdelay[type]=delay;
	      }
	  }	    
      }
    //
    (*MyOutput_) << std::endl << " Best delay settings in " << nloop << " readings and " 
		 << passcuts << " passing cuts are:" << std::endl;
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "  LCT -L1A delay=" << maxdelay[0] << " (" << maxnum[0] << "readings)" << std::endl;
    (*MyOutput_) << "  CFEB-DAV delay=" << maxdelay[1] << " (" << maxnum[1] << "readings)" << std::endl;
    (*MyOutput_) << "  TMB -DAV delay=" << maxdelay[2] << " (" << maxnum[2] << "readings)" << std::endl;
    (*MyOutput_) << "  ALCT-DAV delay=" << maxdelay[3] << " (" << maxnum[3] << "readings)" << std::endl;
    (*MyOutput_) << std::endl;
    //
  }    
  //
}
//
void DAQMB::test3()
{
  int errs,err[8];
  int pass;
  //
  calctrl_fifomrst(); 
  usleep(5000);
  /*
  errs=0;
  printf("Running Memchk\n");
  (*MyOutput_) << "FIFO1 " << std::endl ;
  err[1]=memchk(FIFO1);
  errs+=err[1];
  (*MyOutput_) << "FIFO2 " << std::endl ;
  err[2]=memchk(FIFO2);
  errs+=err[2];
  (*MyOutput_) << "FIFO3 " << std::endl ;
  err[3]=memchk(FIFO3);
  errs+=err[3];
  (*MyOutput_) << "FIFO4 " << std::endl ;
  err[4]=memchk(FIFO4);
  errs+=err[4];
  (*MyOutput_) << "FIFO5 " << std::endl ;
  err[5]=memchk(FIFO5);
  errs+=err[5];
  (*MyOutput_) << "FIFO7 " << std::endl ;
  err[7]=memchk(FIFO7);
  errs+=err[7];
  //
  calctrl_fifomrst(); 
  usleep(5000);
  */
  //
  pass=1;
  errs=0; 
  //
  calctrl_fifomrst(); 
  usleep(5000);
  //
  (*MyOutput_) << "Running Memchk" << std::endl;
  err[1]=memchk(FIFO1);
  errs+=err[1];
  if(err[1]==0){
    (*MyOutput_) << " FIFO1 is OK  " << std::endl;
  }else{
    (*MyOutput_) << " FIFO1 is Bad " << err[1] << std::endl;
  }
  //
  err[2]= memchk(FIFO2);
  errs+=err[2];
  if(err[2]==0){
    (*MyOutput_) << " FIFO2 is OK " << std::endl ;
  }else{
    (*MyOutput_) << " FIFO2 is Bad " << err[2] << std::endl;
  }
  err[3]= memchk(FIFO3);
  errs+=err[3];
  if(err[3]==0){
    (*MyOutput_) << " FIFO3 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO3 is Bad " << err[3] << std::endl;
  }
  err[4]= memchk(FIFO4);
  errs+=err[4];
  if(err[4]==0){
    (*MyOutput_) << " FIFO4 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO4 is Bad " << err[4] << std::endl;
  }
  err[5]= memchk(FIFO5);
  errs+=err[5];
  if(err[5]==0){
    (*MyOutput_) << " FIFO5 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO5 is Bad " << err[5] << std::endl;
  }
  err[6]= memchk(FIFO6);
  if(err[6]==0){
    (*MyOutput_) << " FIFO6 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO6 is Bad " << err[6] << std::endl;
  }
  errs+=err[6];
  //
  err[7]= memchk(FIFO7);
  errs+=err[7];
  if(err[7]==0){
    (*MyOutput_) << " FIFO7 is OK " << std::endl;
  }else{
    (*MyOutput_) << " FIFO7 is Bad " << err[7] << std::endl;
  }
  errs+=err[7];
  calctrl_fifomrst();
  if(errs!=0)pass=0;
  //
  TestStatus_[3] = errs;
  //
}
//
void DAQMB::wrtfifox(enum DEVTYPE devnum,unsigned short int pass)
{ 
  if(hardware_version_<=1)
  {

 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return;
 }
 //
 /* fifo write */  
 cmd[0]=0;
 sndbuf[0]=pass&0xff;
 sndbuf[1]=((pass>>8)&0xff);
 for(int i=0;i<16;i++){sndbuf[0+2*i]=sndbuf[0];sndbuf[1+2*i]=sndbuf[1];};
 //  printf(" wrtfifox 16384 %02x %02x \n",sndbuf[1]&0xff,sndbuf[0]&0xff);
 if(devnum-FIFO7!=0){
    devdo(devnum,1,cmd,16*2,sndbuf,rcvbuf,2);} 
   // devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
 else{
   devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
  }
}

int DAQMB::readfifox_chk(enum DEVTYPE devnum,unsigned int short memchk)
{
  if(hardware_version_<=1)
  {
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }
 //
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=devnum-FIFO1+1; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 //
 /* fifo read check */  
 cmd[0]=1;
 sndbuf[0]=memchk&0xff;
 sndbuf[1]=((memchk>>8)&0xff);
 // devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,1);
 devdo(devnum,1,cmd,16*2,sndbuf,rcvbuf,1);
 //
 (*MyOutput_) << " Number Bad: rcvbuf " << std::hex << rcvbuf[1] << " " << rcvbuf[0] << std::endl; 
 //
 bad=256*rcvbuf[1]+rcvbuf[0];
 //
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=0; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 //
 return bad;
 //
  }
  else return 0;
}
//
int DAQMB::memchk(enum DEVTYPE devnum)
{
  int ierr,ierr2;

  //
  if(devnum<FIFO1||devnum>FIFO7){
    (*MyOutput_) << " Device is not a FIFO " << std::endl;
    return -1;
  }
  //
  for (unsigned i=0; i<sizeof(rcvbuf);i++) {
    rcvbuf[i] = 0;
  }
  //
  ierr=0;
  ierr2=0;
  char snd[40];
  for(int j=0;j<32;j++)snd[j]=0xff;
  wrtfifo(1,16,snd);
  char rcv[40];
  readfifo(1,16,rcv);
  ierr2+=ierr;   /* this doesn't make any sense! ierr never got value before */
  (*MyOutput_) << " ierr " << ierr << ierr2 << std::endl;
  wrtfifox(devnum,0x0000);
  ierr=readfifox_chk(devnum,0x0000);
  (*MyOutput_) << " ierr "  << ierr << std::endl;
  ierr2+=ierr;
  wrtfifox(devnum,0xaaaa);
  ierr=readfifox_chk(devnum,0xaaaa);
  (*MyOutput_) << " ierr " << ierr << std::endl;
  ierr2+=ierr; 
  wrtfifox(devnum,0x5555);
  ierr=readfifox_chk(devnum,0x5555);
  (*MyOutput_) << " ierr " << ierr << std::endl ;
  ierr2+=ierr;  
  wrtfifo_123(devnum);
  ierr=readfifox_123chk(devnum);
  (*MyOutput_) << " ierr " << ierr << std::endl;
  ierr2+=ierr; 
  wrtfifo_toggle(devnum);
  ierr=readfifox_togglechk(devnum);
  (*MyOutput_) << " ierr " << ierr << std::endl ;
  ierr2+=ierr;   
  (*MyOutput_) << " ierr2 " << ierr2 << std::endl;
  return ierr2;
}
//
int DAQMB::readfifox_togglechk(enum DEVTYPE devnum)
{
  if(hardware_version_<=1)
  {
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }

 /* fifo read  */  
 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=devnum-FIFO1+1; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_USR1;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 
 cmd[0]=3;
 sndbuf[0]=0;
 devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);
 //  printf(" Number Bad: rcvbuf %02x %02x \n",rcvbuf[1],rcvbuf[0]); 
 bad=256*rcvbuf[1]+rcvbuf[0];

 cmd[0]=VTX2_USR1;
 sndbuf[0]=FIFO_RD;
 devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
 cmd[0]=VTX2_USR2;
 sndbuf[0]=0; 
 devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
 cmd[0]=VTX2_BYPASS;
 sndbuf[0]=0;
 devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
 
 return bad;
  }
  else return 0;
}
//
void DAQMB::wrtfifo_toggle(enum DEVTYPE devnum)
{
  if(hardware_version_<=1)
  {
    if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return;
    }

   /* fifo write */  
    cmd[0]=2;
    sndbuf[0]=0;
    if(devnum-FIFO7!=0){
       devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
    else{ 
       devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
  }
}
//
void DAQMB::wrtfifo_123(enum DEVTYPE devnum)
{
  if(hardware_version_<=1)
  {
     if(devnum<FIFO1||devnum>FIFO7){
       printf(" Device is not a FIFO \n");
       return;
     }
     //
     /* fifo write */  
     cmd[0]=2;
     sndbuf[0]=2;
     if(devnum-FIFO7!=0){
       devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);}
     else{
       devdo(devnum,1,cmd,8190*2,sndbuf,rcvbuf,2);}
  }
}

//
int DAQMB::readfifox_123chk(enum DEVTYPE devnum)
{
  if(hardware_version_<=1)
  {
 int bad;
 if(devnum<FIFO1||devnum>FIFO7){
    printf(" Device is not a FIFO \n");
    return -1;
 }

   /* fifo read */
    cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=devnum-FIFO1+1; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,2);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);

 cmd[0]=3;
 sndbuf[0]=2;
 devdo(devnum,1,cmd,16380*2,sndbuf,rcvbuf,2);
 // printf(" Number Bad: rcvbuf %02x %02x \n",rcvbuf[1],rcvbuf[0]); 
 bad=256*rcvbuf[1]+rcvbuf[0];

  cmd[0]=VTX2_USR1;
  sndbuf[0]=FIFO_RD;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=0; 
  devdo(MCTRL,6,cmd,3,sndbuf,rcvbuf,0); 
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);

 return bad;
  }
  else return 0;
}

//

int DAQMB::memchk(int fifo)
{
  static int fifosize=16380;
  int err,err1=0;
  char *sndfifo;
  char *rcvfifo;
  sndfifo=(char *)malloc(33000);
  rcvfifo=(char *)malloc(33000);
  //
  (*MyOutput_) << " MEMCHK for FIFO"<<fifo<<std::endl;
  calctrl_fifomrst(); usleep(5000);
  // 0xffff
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0xff;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0xffff "<< err << std::endl;
  /* 
  // 0x0000
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0x00;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x0000 " << err << std::endl; 
  // 0x5555
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0x55;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x5555 "<< err << std::endl; 
// 0xaaaa
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=0xaa;
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0xaaaa "<< err << std::endl; 
  // 0,1,2,3,... 
  for(int i=0;i<fifosize*2;i++)sndfifo[i]=(i&0xff);
  wrtfifo(fifo,fifosize,sndfifo);
  readfifo(fifo,fifosize,rcvfifo);
  err=0;for(int i=0;i<fifosize*2;i++)if(sndfifo[i]!=rcvfifo[i])err=err+1;err1=err1+err;
  (*MyOutput_) << " Error 0x1234 " << err << std::endl; 
  free(sndfifo);
  free(rcvfifo);
  //
  */
  return err1;
  //
}
//
int DAQMB::test4()
{
  //
  int pass;
  float voltage, temp;
  //
  int ierr=0; 
  //
  for(int j=1;j<=3;j++){
    (*MyOutput_) << "Reading 1.8V Chip "<<j<<std::endl;
    voltage = adcminus(j,6);
    (*MyOutput_) << "The voltage is " << voltage <<" mv" << std::endl;
    //
    if(voltage>=1740.0 && voltage<=1860.0){
      (*MyOutput_) << "The voltage on 1.8V Chip "<<j<<" is good " << std::endl;
    }else{
      (*MyOutput_) << "The voltage on 1.8V Chip " << j << " is out of range" << std::endl;
      ierr=1;
    }
  }
  //
  (*MyOutput_) << "Reading DMB GND" << std::endl;
  voltage=adcminus(3,0); 
  (*MyOutput_) << "The voltage is " << voltage << " mv " << std::endl;
  //
  if(voltage>=-10.0 &&voltage<=10.0){
    (*MyOutput_) << "The voltage on DMB GND is good" << std::endl;
  }
  else {
    (*MyOutput_) << "The voltage on DMB GND is out of range" << std::endl;
    ierr=1;
  }
  //
  for(int k=1;k<6;k++){
    (*MyOutput_) << "Reading CFEB"<<k<< " GND "<<std::endl;
    //
    voltage = adcminus(3,k);
    (*MyOutput_) << "The voltage is " << voltage << " mv " << std::endl;
    //
    if(voltage>=-35.0 &&voltage<=35.0){
      (*MyOutput_) << "The voltage on CFEB"<<k<< "is good"<<std::endl;
    } else {
      (*MyOutput_) << "The voltage on CFEB"<<k<< "is out of range" << std::endl;
      ierr=1;
    }
  }
  //
  (*MyOutput_) << "Temperature Reading for DMB" << std::endl;
  temp=readthermx(0); 
  (*MyOutput_) << "The temperature is " << temp <<" F " << std::endl;
  //
  if(temp>=10.0 &&temp<=45.0){
    (*MyOutput_) << "The temperature on DMB is good " << std::endl;
  } else {
    (*MyOutput_) << "The temperature on DMB is out of range" << std::endl;
    ierr=1;
  }
  //
  for(int k=1;k<6;k++){
    (*MyOutput_) << "Temperature Reading CFEB "<<k << std::endl;
    temp = readthermx(k);
    (*MyOutput_) << "The temperature is " << temp << " F " << std::endl;
    //
    if(temp>=10.0 && temp<=45.0){
      (*MyOutput_) << "The temperature on CFEB"<<k<<" is good " << std::endl;
      
    }else{
      (*MyOutput_) << "The temperature on CFEB"<<k<<" is out of range " << std::endl;
      ierr=1;
    }
  }
  //
  TestStatus_[4] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test5()
{
  int i,k;
  int pass,ierr;
  int itog;
  int f1[5]={3,3,4,4,5};
  int f2[5]={4,7,2,5,0};
  float value;
  unsigned int ival=0;
  char c=0;
  //
  ierr=0;  
  itog=0;
  for(k=0;k<2;k++){
    if(itog==0)c=0x00; 
    if(itog==1)c=0x3f;
    lowv_onoff(c);
    lowv_rdpwrreg(); 
    (*MyOutput_) << " power register " << ival << std::endl;
    ::sleep(1);
    itog=itog+1;
    if(itog==2)itog=0;
    /* these are the adcs connected to the power register */
    (*MyOutput_) << " Selected Reg. Channel" << std::endl;
    //
    /*j is 0-7 i is 1-5*/
    for(i=0;i<5;i++){
      value = lowv_adc(f1[i],f2[i]);
      if(itog==0){
	if(value<5000 && value>4100.0){
	  (*MyOutput_) <<"The voltage on "<<f1[i]<<" "<<f2[i]
		       <<" is good - "<<value<<" mv "<<std::endl;
	}else{   
	  (*MyOutput_) <<"The voltage on "<<f1[i]<<" "<<f2[i]
		       <<" is out of range - "<<value<<" mv "<<std::endl;
	  ierr=1;
	} 
      }
      //
      if(itog==1){
	if(value>-200 && value<200){
	  (*MyOutput_) << "The voltage off "<<f1[i]<<" "<<f2[i]
		       <<" is good - "<<value<<" mv " << std::endl;
	}else{   
	  (*MyOutput_) << "The voltage off "<<f1[i]<<" "<<f2[i]
		       <<" is out of range - "<<value<<" mv " << std::endl;
	  ierr=1;
	} 
      }
    }
  }
  //
  TestStatus_[5] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test6()
{
  //
  int err2,pass;
  int ierr=0;
  unsigned long int ival;
  //
  err2=0;
  if(cfebs_.size()!=5){
    err2=err2+100;
    (*MyOutput_) << "ERROR: only " <<cfebs_.size()<< " FEBs found " << std::endl;
  }
  err2=0;
  for(unsigned i = 0; i < cfebs_.size(); ++i) {
    ival=febfpgaid(cfebs_[i]);
    usleep(100);
    if(ival!=0x20610093){
      err2=err2+1;
      ierr=1;
      (*MyOutput_) << " ERROR: "<<std::hex<<i<<" febfpgaid is: "<<ival
		   << " and should be 0x20610093" << std::endl;
    }
  }
  //
  TestStatus_[6] = ierr;
  //
  pass=1; 
  if(err2!=0)pass=0;
  //
  return pass;
  //
}
//
int  DAQMB::test8()
{
  //
  int i,ierr,err2,pass;
  float v0,vout,diff,diff2;
  //
  double sn,sx,sy,sxy,sx2;
  float x[9],y[9];
  float a,b;
  ierr=0;
  //
  // check comparator DAC/ADC
  //
  if(cfebs_.size()!=5){
     ierr=ierr+100;
     (*MyOutput_) << "ERROR: only "<<cfebs_.size()<<" FEBs found "<<std::endl;
  }
  err2=0;
  for(unsigned cfeb = 0; cfeb < cfebs_.size(); ++cfeb) {
    //
    for(i=0;i<10;i++){
      //
      v0=0.25*i;
      set_comp_thresh(cfeb,v0);
      //set_comp_thresh(v0);
      usleep(500000);
      //
      for(unsigned cfeby=0; cfeby<cfebs_.size(); ++cfeby) {
	vout=adcplus(2,cfebs_[cfeby].number());
	(*MyOutput_) << "cfeb="<<cfeby<<" "<<" v0=" << v0 << " vout="<<vout<<std::endl;
      }
      //
      vout=adcplus(2,cfebs_[cfeb].number());
      if(i>0){
	x[i-1]=v0;
	y[i-1]=vout;
	(*MyOutput_) << "CFEB=" << cfeb<<" "<<v0<<" "<<vout<<std::endl;
      }
      //
    }
    //
    sn=0.;
    sx=0.;
    sy=0.;
    sxy=0.;
    sx2=0.;
    //
    for(i=0;i<9;i++){
      sn=sn+1;
      sx=sx+x[i];
      sy=sy+y[i];
      sxy=sxy+x[i]*y[i];
      sx2=sx2+x[i]*x[i];
    }
    //
    a=(sxy*sn-sx*sy)/(sn*sx2-sx*sx);
    b=(sy*sx2-sxy*sx)/(sn*sx2-sx*sx);
    (*MyOutput_)<<"a " <<a<< " b " <<b<<std::endl;
    //
    if(a<-1025.||a>-800.){
      ierr=1;
      (*MyOutput_) << " slope     out of range- got "<<a<<" should be -1005. " <<std::endl;
    }
    //
    if(b<3400.||b>3800.){ //changed a>3800 to b>3800
      ierr=1;
      (*MyOutput_) << " intercept out of range- got "<<a<<" should be 3600. " << std::endl;
    }
    //
    for(i=0;i<9;i++){
      diff=y[i]-a*x[i]-b;
      diff2=diff;
      if(diff2<0)diff2=-diff2;
      if(diff2<2.0){
	(*MyOutput_) << "Good - i "<<i<<" " <<y[i]<<" "<<a*x[i]+b<<" "<<diff<<std::endl;
      }else{
	ierr=1;
	(*MyOutput_) << "Bad  - i "<<i<<" " <<y[i]<<" "<<a*x[i]+b<<" "<<diff<<std::endl;
      }
    }
  }
  //
  // end comparator DAQ/ADC
  //
  TestStatus_[8] = ierr;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}
//
int DAQMB::test9()
{
  int i,ierr,pass;
  float v0,v1,vout,diff;
  int ius;
  float voff;
  float capn[5] ={0.0,0.25,0.50,0.748,0.992};
  float presn[5]={0.0,0.25,0.50,0.745,0.988};
  //
  ierr=0;  
  // 
  ius=0;
  voff=0.0;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adcplus(2,7);
    vout=vout/1000.;
    if(ius==0&&vout>0.0){ius=1;voff=fabs(vout-v0);}
    diff=fabs(capn[i]-vout)-voff;
    if(diff<0.0)diff=0;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Int CDAC - got " << vout << " expect " << v0 << std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Int CDAC - got " << vout << " expect " << v0 << std::endl;
    }
  }
  // end int cal
  (*MyOutput_) << " end int cal " << std::endl;
  // ext cal
  ius=0;
  voff=0.;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adcminus(3,7);
    vout=vout/1000.; 
    if(ius==0&&vout>0.0){ius=1;voff=fabs(v0-vout);}
    diff=fabs(presn[i]-vout)-voff;
    if(diff<0.0)diff=0.;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }
  }
  // end ext cal
  (*MyOutput_) << "end ext cal " << std::endl;
  // ext cal ptrd
  ius=0;
  voff=0.0;
  for(i=0;i<5;i++){
    v0=.25*i;
    v1=v0;
    set_dac(v0,v1);
    usleep(100);
    vout=adc16(4,0);
    if(ius==0&&vout>0.0){ius=1;voff=fabs(v0-vout);}
    diff=fabs(presn[i]-vout)-voff;
    if(diff<0.0)diff=0.;
    if(diff>0.020){
      ierr=1;
      (*MyOutput_) << "Problem -Pres Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }else{ 
      (*MyOutput_) << "Good    -Pres Ext CDAC - got "<<vout<<" expect "<<v0<<std::endl;
    }
  }
  // end ext pres
  (*MyOutput_) << " end ext pres " << std::endl;
  //
  pass=1; 
  if(ierr!=0)pass=0;
  //
  TestStatus_[9] = ierr;
  //
  return pass;
  //
}
//
int DAQMB::test10()
{
  //
  int i,ierr,pass;
  char cmp;
  char rcvpat[256];
  char sndpat[256];
  int match;
  //
  ierr=0;
  //Program SFM with 0,1,2,3,...,263
  for(i=0;i<264;i++)sndpat[i]=i&0xff;
  //
  sfm_test_load(sndpat);  
  //
  SFMWriteProtect();
  // Write
  ProgramSFM();
  ::sleep(2);
  // Disable
  SFMWriteProtect();
  //
  //Read SFM 
  //
  sfm_test_read(rcvpat);
  //
  match=0;
  for(i=0;i<256;i++){cmp=i&0xff;if(cmp==rcvpat[i])match=match+1;}
  if(match!=256){
    for (i=0;i<256;i++) printf("%02x ",rcvpat[i]&0xff);printf("\n");
    ierr=match;
  }
  //
  pass=1; 
  if(ierr!=0)pass=0;
  //
  TestStatus_[10] = ierr;
  //
  return pass;
}
//
int  DAQMB::test11()
{  
  // enum DEVTYPE dv;
  int i,j,itog;
  int nmtch;
  char pat[36],chk[32];
  char vshift[300];
  char v[2]={0x00,0xff};
  int ierr,pass;
  //
  ierr=0;
  itog=0;
  j=0;
 LOOP: j=j+1; 
   itog=itog+1;
   if(itog==2)itog=0;
   buckflash_erase();
   printf(" enter buckflash_load2 \n");
   //
   for(int k=0;k<295;k++)vshift[k]=v[itog];
   //
   buckflash_load(vshift);
   printf(" Sleep after buckflash Load \n");
   ::sleep(1);
   //
   buckflash_pflash();
   printf(" Sleep after buckflash program \n");
   ::sleep(5);
   //
   buckflash_init();
   ::sleep(1);
   //
   buckflash_init();         //add an extra init
   printf(" Sleep after buckeye init \n");
   ::sleep(2);
   //
   buck_shift_out();
   for(int brd=0;brd<5;brd++){
     for(i=0;i<36;i++)pat[i]=shift_out[brd][i];
     for(i=0;i<36;i++)chk[i]=v[itog];
     if(itog==1)chk[0]=0x7f;
     nmtch=0;
     for(i=0;i<36;i++){
       if(pat[i]==chk[i])nmtch++;
     }
     if(nmtch!=36){
       ierr=ierr+1;}
     printf(" nmtch %d \n",nmtch);
     for(i=0;i<36;i++)printf("%02x",pat[i]&0xff);printf("\n");
     printf("*Bad CFEB %d",brd); for(i=0;i<36;i++)printf("%02x",pat[i]&0xff);printf("\n");
   }
   if(j==2)goto ENDR;
   goto LOOP;
 ENDR: 
   //
   TestStatus_[11] = ierr;
   //
   pass=1; 
   if(ierr!=0)pass=0;
   return pass;
   //
}

int DAQMB::DCSreadAll(char *data)
{
  unsigned short n, m;
  int retn=0;
 if (hardware_version_<=1)
 {
  if(checkvme_fail()) return 0;

  for(int i=1; i<6; i++)
  {
     // devdo() dev=25, ncmd=16, Cmd[0]=i, Cmd[1]=j

     m = i-1;
     for(int j=0;j<8; j++)
     {
        write_later(0x8020, m);
        n=(j<<4) + 0xFF89;
        write_later(0x8000, n);
        read_later(0x8004);
     }
  }
  for(int j=0; j<6; j++)
  {
     // devdo() dev=17, ncmd=16, Cmd[0]=1, Cmd[1]=j

     write_later(0x7020, 0x0E);
     n=(j<<4) + 0xFF89;
     write_later(0x7000, n);
     if(j==5) retn=read_now(0x7004, data);
     else     read_later(0x7004);
  }
 }
 else if (hardware_version_==2)
 {
  if(checkvme_fail()) return 0;

  for(int i=1; i<8; i++)
  {
     // devdo() dev=25, ncmd=16, Cmd[0]=i, Cmd[1]=j

     m = i-1;
     write_later(0x8020, m);
     for(int j=0;j<8; j++)
     {
        vme_delay(10);
        n=(j<<4) + 0xFF89;
        write_later(0x8000, n);
        vme_delay(10);
        if( i==7 && j==7) retn=read_now(0x8004, data);
        else read_later(0x8004);
     }
  }

// no ADC on ODMB
// TODO:  add DCFEB monitoring info here
//     loop through all DCFEBs
//     {    1. DCFEB ADC
//          2. DCFEB SYSMON
//     }

#if 0
//comment out this, for Mike Matveev's test_firmware only
   // this is for the ODMB test_firmware version with direct MAX1271 access
   // VME commands not buffered
     unsigned select_addr=0x7c, write_addr=0x7a, read_addr=4;
     unsigned short adc_select_word, control_byte, CLK=1, SDI=2, dout, rback;
     unsigned short *data2=(unsigned short *)data;
     char tmp[1000];
     
     // init
     write_now(write_addr, 0, tmp);
     // loop through all 7 ADCs
     for(int i=0; i<7; i++)
     {
         udelay(100);
         adc_select_word = 0xFFFF^(1<<i);
         write_now(select_addr, adc_select_word, tmp);
         udelay(100);
         // loop through 8 channels
         for(int ch=0; ch<8; ch++)
         {
            control_byte=0x89+(ch<<4);  // binary=1xxx1001, xxx=channel
            // send 8 bits of control_byte, MSB first
            for(int l=7; l>=0; l--)
            {  
               dout = ((control_byte>>l)&1) * SDI;
               for(int j=0;j<3;j++) write_now(write_addr, (j==1)?(dout+CLK):dout, tmp);
               write_now(write_addr, 0, tmp);
            }
            udelay(1000);
            // send 4 clocks
            for(int k=0; k<4; k++)
            {  
               for(int j=0;j<2;j++) write_now(write_addr, (j==0)?CLK:0, tmp);
            }
            // read 12 bits of data, plus 1 extra clock
            rback=0;
            for(int k=0; k<13; k++)
            {  
               for(int j=0;j<2;j++) write_now(write_addr, (j==0)?CLK:0, tmp);
               if(k<12)
               {
                  read_now(read_addr, tmp);
                  rback <<= 1;
                  rback += (tmp[0]&1);
               }
            }
            data2[i*8+ch]=rback;
            retn++;
         }
     }
     write_now(select_addr,0xFFFF, tmp);
#endif
 }
  return retn;
}

bool DAQMB::checkvme_fail()
{  // return true:  DMB vme access failed
   //       false:  DMB vme access OK

   unsigned short data;

   failed_checkvme_ = 0;
   int i=read_now(read_ADC_SELECT, (char *) &data);
   if(i<=0) failed_checkvme_ = 1;  // if VCC problem
   else if(data==0xBAAD)  failed_checkvme_ = 1; // DMB time-out
   return (bool)failed_checkvme_;
}

int DAQMB::cfeb_testjtag_shift(int icfeb,char *out){
  int hversion=cfebs_[icfeb].GetHardwareVersion();
  if(hversion<=1)
  {
    int ierr = 0;
    DEVTYPE dv = cfebs_[icfeb].scamDevice();
    cmd[0]=VTX_BYPASS;
    sndbuf[0]=0x00;
    sndbuf[1]=0xaa;
    sndbuf[2]=0x55;
    sndbuf[3]=0x00;
    devdo(dv,5,cmd,40,sndbuf,rcvbuf,1);
    if((rcvbuf[1]&0xFF)!=0xa8)ierr=1;
    if((rcvbuf[2]&0xFF)!=0x56)ierr=1;
    if((rcvbuf[3]&0xFF)!=0x01)ierr=1;
    if((rcvbuf[4]&0x3F)!=0x00)ierr=1;
    out[0]=rcvbuf[0]; 
    out[1]=rcvbuf[1];
    out[2]=rcvbuf[2];
    out[3]=rcvbuf[3];
    out[4]=rcvbuf[4];
    return ierr;
  }
  else return 0;
}


void DAQMB::vtx_cmpfiles(const std::string fileDir, int cbits[]) {
	const unsigned int arrSiz = 36;
	const unsigned int wordframe = 12;
	const unsigned int clbframe = 1323;
  char line[arrSiz], linecmp[arrSiz], linemask[arrSiz], last[arrSiz];
  FILE *cmpfile, *rbkfile, *maskfile;
  int gbits=0;
	int bbits=0;
  int mskbits=0;

  cbits[0] = -1; // -1 indicates failure. Reset below.

  std::string filename = "rbk.dat"; // Located in current directory
  rbkfile = fopen(filename.c_str(),"r");
  if (rbkfile == 0) {
		(*MyOutput_) << "Open rbk.dat failed. End vtx_cmpfiles.\n";
    return;
  }
	bool all1 = true, all0 = true;
  while ((all0 || all1) && fgets(line, arrSiz, rbkfile) != NULL) {
		unsigned int ind = 0;
		do {
			// Skip whitespace
			if (line[ind] != '\n' && line[ind] != ' ') {
				if (line[ind] == '0') {
					if (all1)
						all1 = false;
				} else if (all0)
					all0 = false;
			}
			++ind;
		} while (ind < arrSiz - 1 && (all0 || all1));
	}
  if (all0 || all1) {
		fclose(rbkfile);
		return;
	}
	rewind(rbkfile);
  filename = fileDir;
  filename += "/cmp.dat";
  cmpfile=fopen(filename.c_str(),"r");
  if (cmpfile == 0) {
		(*MyOutput_) << "Open cmp.dat failed. End vtx_cmpfiles.\n";
		fclose(rbkfile);
    return;
  }
  filename = fileDir;
  filename += "/msk.dat";
  maskfile=fopen(filename.c_str(),"r");
  if (maskfile == 0) {
		(*MyOutput_) << "Open msk.dat failed. End vtx_cmpfiles.\n";
		fclose(rbkfile);
		fclose(cmpfile);
    return;
  }
  //jump to the beginning of COMPARE file, rbt file, skip the header,
  // dummy/sync words, and command word, and one pad word

  for (int i=0;i<19;i++)
		fgets(linecmp, arrSiz, cmpfile);

  //Jump to the begining of readback data file, one pad frame and
  // one additional pad word
  // it is also offset by one bit ?????
  fgets(line, arrSiz, rbkfile);
  for (unsigned int i = 0; i < wordframe + 2; i++) {
    fgets(last, arrSiz, rbkfile);
    for (int j = 0; j < 31; j++)
			line[j] = line[j+1];
    line[31]=last[0];
  }
  // Jump to the beginning of MASK file, skip the header, Dummy/Sync words
  // and CLB frame readback command set, 32 bytes
  for (int i = 0; i < 22; i++)
		fgets(linemask,arrSiz,maskfile);

  int totwords = wordframe * (clbframe - 1) - 2;
	(*MyOutput_) << " Bad bits in word:bit format" << std::endl;
  for (int i = 0; i < totwords; i++) {
    fgets(linecmp, arrSiz, cmpfile);
    for(int j = 0; j < 32; j++)
			line[j] = last[j];
    fgets(last, arrSiz, rbkfile);
    for(int j = 0; j < 31; j++)
			line[j] = line[j+1];
    if (i == totwords - 1)
			last[0] = '0';
    line[31] = last[0];
    fgets(linemask, arrSiz, maskfile);
    for (int j = 0; j < 32; j++) {
      if (line[j] != linecmp[j] && linemask[j] == '0') {
        linemask[j] = '1';
        bbits++;
				(*MyOutput_) << i << ":" << j << " ";
      } else {
        gbits++;
      }
      if (linemask[j] == '1')
				mskbits++;
    }
  }
	if (bbits > 0)
		(*MyOutput_) << std::endl;
  (*MyOutput_) << " bbits " << bbits << " mskbits " << mskbits << std::endl;
  cbits[0] = gbits;
  cbits[1] = bbits;
  cbits[2] = mskbits;
  fclose(cmpfile);
  fclose(rbkfile);
  fclose(maskfile);
}


void DAQMB::set_chans_mode(int schan,int mode)
{
  for(int brd=0;brd<5;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=NORM_RUN;
      }
      shift_array[brd][chip][schan]=mode;
    }
  }
}

void DAQMB::set_chans_by4(int schan,int mode)
{
  for(int brd=0;brd<5;brd++){
    for(int chip=0;chip<6;chip++){
      for(int ch=0;ch<16;ch++){
        shift_array[brd][chip][ch]=NORM_RUN;
      }
      shift_array[brd][chip][schan]=mode;
      shift_array[brd][chip][schan+4]=mode;
      shift_array[brd][chip][schan+8]=mode;
      shift_array[brd][chip][schan+12]=mode;
    }
  }
}

void DAQMB::small_configure() {

   //***Do this setting only for calibration ****
  int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
     | (calibration_l1acc_delay_ & 0x1F) << 4
      | (pulse_delay_ & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
   (*MyOutput_) << "DAQMB:configure: caldelay " << std::hex << cal_delay_bits << std::dec << std::endl;
   setcaldelay(cal_delay_bits);
   (*MyOutput_) << "doing set_cal_dac " << inj_dac_set_ << " " 
	<<  pul_dac_set_ << std::endl;
   set_cal_dac(inj_dac_set_, pul_dac_set_);
   //

   //*** Do this setting only for older DMB firmware (V18 or older)
   int dav_delay_bits = (feb_dav_delay_    & 0x1F)
      | (tmb_dav_delay_ & 0X1F) << 5
      | (push_dav_delay_   & 0x1F) << 10
      | (l1acc_dav_delay_  & 0x3F) << 15
      | (ALCT_dav_delay_   & 0x1F) << 21;
   (*MyOutput_) << "doing setdavdelay " << dav_delay_bits << std::endl;
   setdavdelay(dav_delay_bits);
   int comp_mode_bits = (comp_mode_ & 3) | ((comp_timing_ & 7) << 2);
     set_comp_mode(comp_mode_bits);
   //

   }

void DAQMB::testlink(DEVTYPE devnum){
//
// this function  never been used anywhere
/*
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x05;
  sndbuf[1]=0x00;
  devdo(devnum,5,cmd,8,sndbuf,rcvbuf,0);
  usleep(50000);
  // due to the routing in Xilinx, we need extra time here
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x03;
  sndbuf[1]=0x00;
  devdo(devnum,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_USR1;
  sndbuf[0]=0x04;
  sndbuf[1]=0x00;
  devdo(devnum,5,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX_BYPASS;
  sndbuf[0]=0x00;
  devdo(devnum,5,cmd,0,sndbuf,rcvbuf,0);
*/
}

void DAQMB::varytmbdavdelay(int delay)
{

   int dav_delay_bits = (feb_dav_delay_    & 0x1F)
      | (delay & 0X1F) << 5
      | (push_dav_delay_   & 0x1F) << 10
      | (l1acc_dav_delay_  & 0x3F) << 15
      | (ALCT_dav_delay_   & 0x1F) << 21;
   (*MyOutput_) << "doing setdavdelay " << dav_delay_bits << std::endl;
   printf(" delay %02x standard %02x \n",delay,tmb_dav_delay_);
   setdavdelay(dav_delay_bits);
}

void DAQMB::load_feb_clk_delay()
{
  if(hardware_version_<=1)
  {
  printf(" load_feb_clk_delay called \n");
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0x1D;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=NOOP;
  devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
  cmd[0]=VTX2_BYPASS;
  devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,0);
  }
}

// code used by STEP
//
void DAQMB::trighalfx(int ihalf)
{
    int hs[6];
    int i,j,k,pln,crd;
    int chan[7][6][16];
    for(i=0;i<7;i++)
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
    for(crd=0;crd<7;crd++)
    {
		for(pln=0;pln<6;pln++)
		{
			halfset(crd,pln,hs[pln],chan);
		}
    }
    for(i=0;i<7;i++)
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
    chan2shift(chan);
}

void DAQMB::write_cfeb_selector(int cfeb_mask)
{
  char temp[4];
  unsigned short mask = cfeb_mask&0xFF;
  vme_delay(20);
  write_now(WRITE_CFEB_SELECTOR, mask, temp);
//  std::cout << "Write CFEB selector: " << std::hex << cfeb_mask << std::dec << std::endl;
  udelay(100);
//  int mask_read=read_cfeb_selector();
//  if(mask_read != (cfeb_mask&0xFF))
//    std::cout << "ERROR: read back CFEB selector: " << std::hex << mask_read << std::dec << std::endl;
}

int DAQMB::read_cfeb_selector()
{
  int mask;
  read_now(READ_CFEB_SELECTOR, (char *)&mask);
  mask &= ((hardware_version_==2)?0x7F:0x1F);
  return mask;
}

void DAQMB::cfeb_do(int ncmd, void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd)
{
  /* irdsnd for jtag
          irdsnd = 0 no read, later
          irdsnd = 1 no read, now
          irdsnd = 2    read, later
          irdsnd = 3    read, now
  */
   // code moved to daqmb_do()
   daqmb_do(ncmd, cmd, nbuf, inbuf, outbuf, irdsnd, 1);
}

void DAQMB::dcfeb_fpga_call(int inst, unsigned data, char *outbuf)
{
  char temp[4];
  cfeb_do(10, &inst, 32, &data, outbuf, NOW|READ_YES);
  int comd=VTX6_BYPASS;
  cfeb_do(10, &comd, 0, &data, temp, NOW);
  udelay(10);
}

std::vector<float> DAQMB::dcfeb_fpga_monitor(CFEB & cfeb)
{
  // only read out first 3 channels
  
  std::vector<float> readout;
  char buf[4]={0,0,0,0};
  int comd=VTX6_SYSMON;
  unsigned data, ibrd, adc;
  float readf;

  readout.clear();
  int hversion=cfeb.GetHardwareVersion();
  if(hversion==2)
  {
     write_cfeb_selector(cfeb.SelectorBit());
     comd=VTX6_SYSMON;
//     this can be used to change register 0x48 to enable more channels
//     data=0x8483F00;
//     cfeb_do(10, &comd, 32, &data, rcvbuf, 3);
     data=0x4000000;
     cfeb_do(10, &comd, 32, &data, rcvbuf, NOW|READ_YES);
     udelay(100);
     for(unsigned i=0; i<3; i++)
     {
        data += 0x10000;
        cfeb_do(0, buf, 32, &data, (char *)&ibrd, NOW|READ_YES);
        udelay(100);
        adc = (ibrd>>6)&0x3FF;
        if(i==0)
          readf=adc*503.975/1024.0-273.15;
        else
          readf=adc*3.0/1024.0;
        readout.push_back(readf);
     }
     data=0x4100000;
     cfeb_do(10, &comd, 32, &data, rcvbuf, NOW|READ_YES);
     udelay(100);
     for(unsigned i=0; i<16; i++)
     {
        data += 0x10000;
        cfeb_do(0, buf, 32, &data, (char *)&ibrd, NOW|READ_YES);
        udelay(100);
        adc = (ibrd>>6)&0x3FF;
        if(i<6 && i!=3)
          // currents in (A) not (mA) !
          readf=adc*2./1024.0;
        else
        {
          // voltages in (V)
          if((i>8 && i<11) || i>13) readf=adc*10.5/1024.0;  // TODO: the conversion factor 10.5 is not accurate
          else readf=adc*5.008/1024.0;
        }
        readout.push_back(readf);
     }
     comd=VTX6_BYPASS;
     cfeb_do(10, &comd, 0, &data, rcvbuf, NOW);
     udelay(10);
  }
  return readout;
}

void DAQMB::dcfeb_core(int jfunc, int nbit,void *inbuf, char *outbuf, int option)
{
// option
//    bit 0   = 0  later
//            = 1  now
//    bit 1   = 0  no read
//            = 2  read
//    bit 2   = 0  no NOOP
//            = 4  NOOP
//    bit 3   = 0  with BYPASS (normal)
//            = 8  No BYPASS at the end

  char buf[4]={0,0,0,0};
  int comd;

     comd=VTX6_USR1;
     buf[0]=jfunc&0xFF;
     cfeb_do(10, &comd, 8, buf, rcvbuf, (nbit>0)?LATER:NOW);
     if(nbit>0)
     {
        vme_delay(10);
        comd=VTX6_USR2;
        cfeb_do(10, &comd, nbit, inbuf, outbuf, option&0x3);
        if(option & NOOP_YES) 
        {
           comd=VTX6_USR1;
           buf[0]=NOOP;
           cfeb_do(10, &comd, 8, buf, rcvbuf, (option & NO_BYPASS)?LATER:NOW);
           vme_delay(10);
           udelay(10);
        }
     }
     if((option & NO_BYPASS)==0)
     {
        comd=VTX6_BYPASS;
        cfeb_do(10, &comd, 0, buf, rcvbuf, NOW);
        udelay(10);
     }
  return;
}
//
// the only diffrence between dcfeb_hub() and dcfeb_core() is that dcfeb_hub() selects which DCFEB to use
//
void DAQMB::dcfeb_hub(CFEB & cfeb, int jfunc, int nbit, void *inbuf, char *outbuf, int option)
{
// option
//    bit 0   = 0  later
//            = 1  now
//    bit 1   = 0  no read
//            = 2  read
//    bit 2   = 0  no NOOP
//            = 4  NOOP

  write_cfeb_selector(cfeb.SelectorBit());
  dcfeb_core(jfunc, nbit, inbuf, outbuf, option);
  return;
}

void DAQMB::dcfeb_sys_reset(CFEB & cfeb)
{
  char buf[4];
  dcfeb_hub(cfeb, RESET_MODE, 0, buf, buf, NOW);
  return;
}

unsigned DAQMB::dcfeb_read_status(CFEB & cfeb)
{
  unsigned temp;
  char buf[4]={0,0,0,0};
  dcfeb_hub(cfeb, STATUS_S, 32, buf, (char *)&temp, NOW|READ_YES);
  return temp;
}

void DAQMB::BuckeyeShift(int chip_mask,char shft_bits[6][6], char *shft_out)
{
  int lay;
  int readback=0;
  if(shft_out != NULL) readback=READ_YES;

  sndbuf[0]=chip_mask&0xFF; 
  dcfeb_core(CHIP_MASK, 6, sndbuf, rcvbuf, NOW|NO_BYPASS);
  // shuffle the data to the correct layer (each layer has 6 bytes) 
    int  j=0;
      for(int chip=5;chip>=0;chip--){
	 lay=layers[chip];
	 for(int i=0;i<6;i++){
	    sndbuf[j*6+i]=shft_bits[lay][i];
	 }
	 j++;
      }
  udelay(20);
  dcfeb_core(CHIP_SHFT, 288, sndbuf, rcvbuf, NOW|NOOP_YES|readback);
  udelay(2000);
  if(readback)  memcpy(shft_out, rcvbuf, 36);
}
    
void DAQMB::dcfeb_Set_ReadAnyL1a(CFEB & cfeb)
{
  char tmp[2];
  dcfeb_hub(cfeb, SET_ANYL1AFLAG, 0, tmp, tmp, NOW);
  return;
}
    
void DAQMB::dcfeb_Clear_ReadAnyL1a(CFEB & cfeb)
{
  char tmp[2];
  dcfeb_hub(cfeb, CLEAR_ANYL1AFLAG, 0, tmp, tmp, NOW);
  return;
}

void DAQMB::FADC_SetMask(CFEB & cfeb, short int mask)
{
  unsigned temp;
  dcfeb_hub(cfeb, FADC_MASK, 12, &mask, (char *)&temp, NOW);
  return;
}

void DAQMB::FADC_Initialize(CFEB & cfeb)
{
  char tmp[2];
  dcfeb_hub(cfeb, FADC_INIT, 0, tmp, tmp, NOW);
  return;
}

void DAQMB::FADC_ShiftData(CFEB & cfeb, unsigned bits)
{
  unsigned temp;
  dcfeb_hub(cfeb, FADC_SHIFT, 26, &bits, (char *)&temp, NOW);
  return;
}

void DAQMB::Pipeline_Restart(CFEB & cfeb)
{
  char tmp[2];
  dcfeb_hub(cfeb, Pipeline_Restrt, 0, tmp, tmp, NOW);
  return;
}

void DAQMB::dcfeb_set_PipelineDepth(CFEB & cfeb, short int depth)
{
  unsigned temp;
  dcfeb_hub(cfeb, Pipeline_Depth, 9, &depth, (char *)&temp, NOW);
  return;
}

// set pipeline depths and fine delays for all DCFEBs; this is necessary to
// properly read out data
void DAQMB::set_and_initalize_pipelines_and_fine_delays() {
  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++){
    int hversion=cfebs_[lfeb].GetHardwareVersion();
    if(hversion==2)
      {
	dcfeb_set_PipelineDepth(cfebs_[lfeb], cfebs_[lfeb].GetPipelineDepth());
	usleep(100);
	Pipeline_Restart(cfebs_[lfeb]);
	usleep(100);
	dcfeb_fine_delay(cfebs_[lfeb], cfebs_[lfeb].GetFineDelay());
	usleep(100);
      }
  }
}

void DAQMB::Set_NSAMPLE(CFEB & cfeb, int nsample)
{
  unsigned temp;
  dcfeb_hub(cfeb, Pipeline_NSAMPLE, 7, &nsample, (char *)&temp, NOW);
  return;
}

unsigned short DAQMB::dcfeb_fine_delay(CFEB & cfeb, unsigned short delay)
{
  unsigned short temp;
  dcfeb_hub(cfeb, FINE_DELAY, 4, &delay, (char *)&temp, NOW|READ_YES);
  return (temp&0xF);
}

void DAQMB::dcfeb_XPROM_do(unsigned short command)
{
  unsigned short temp;
  dcfeb_core(EPROM_bpi_write, 16, &command, (char *)&temp, NOW);
  return;
}

unsigned DAQMB::dcfeb_bpi_readtimer()
{
  unsigned temp;
  char buf[4]={0xFF,0xFF,0xFF,0xFF};
  dcfeb_core(EPROM_bpi_timer, 32, buf, (char *)&temp, NOW|READ_YES);
  return temp;
}

void DAQMB::dcfeb_bpi_reset()
{
  unsigned temp;
  char buf[4];
  dcfeb_core(EPROM_bpi_reset, 0, buf, (char *)&temp, NOW);
  return;
}

void DAQMB::dcfeb_bpi_disable()
{
  unsigned temp;
  char buf[4];
  dcfeb_core(EPROM_bpi_disable, 0, buf, (char *)&temp, NOW);
  return;
}

void DAQMB::dcfeb_bpi_enable()
{
  unsigned temp;
  char buf[4];
  dcfeb_core(EPROM_bpi_enable, 0, buf, (char *)&temp, NOW);
  return;
}

unsigned short DAQMB::dcfeb_bpi_status()
{
  /* status register
     low 8 bits XLINK
      0-blank write status/multiple work program status
      1-block protection status
      2-program suspend status
      3-vpp status
      4-program status
      5-erase/blank check status
      6-erase/suspend status
      7-P.E.C. Status
     high 8 bits Ben
      0-cmd fifo write error
      1-cmd fifo read error
      2-cmd fifo full 
      3-cmd fifo empty
      4-rbk fifo write error
      5-rbk fifo read error
      6-rbk fifo full 
      7-rbk fifo empty
  */  
  unsigned short temp;
  char buf[4]={0xFF,0xFF,0xFF,0xFF};
  dcfeb_core(EPROM_bpi_status, 16, buf, (char *)&temp, NOW|READ_YES);
  return temp;
}

void DAQMB::dcfebprom_multi(int cnt, unsigned short *manbuf)
{
    unsigned short comd, tmp;
    comd=VTX6_USR1;
    tmp=EPROM_bpi_write; 
    cfeb_do(10, &comd, 8, &tmp, rcvbuf, LATER);
    for(int i=0; i<cnt; i++)
    {
      comd=VTX6_USR2;
      tmp=manbuf[i];
      cfeb_do(10, &comd, 16, &tmp, rcvbuf, LATER);
    }
    comd=VTX6_BYPASS;
    tmp=0;
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    udelay(30);                             
    return;
}

void DAQMB::dcfebprom_unlockerase() 
{ 
   unsigned short tmp[2];
   tmp[0]=XPROM_Block_UnLock;
   tmp[1]=XPROM_Block_Erase;
   dcfebprom_multi(2, tmp);
}

void DAQMB::dcfebprom_loadaddress(unsigned short uaddr, unsigned short laddr) 
{ 
   unsigned short tmp[2];
   tmp[0]=((uaddr<<5)&0xffe0)|XPROM_Load_Address;
   tmp[1]=laddr;
   dcfebprom_multi(2, tmp);
}

void DAQMB::dcfebprom_bufferprogram(unsigned nwords,unsigned short *prm_dat)
{
// nwords max. 11 bits (2048 words)
    unsigned short comd, tmp;
    comd=VTX6_USR1;
    tmp=EPROM_bpi_write; 
    cfeb_do(10, &comd, 8, &tmp, rcvbuf, LATER);
    comd=VTX6_USR2;
    tmp= (((nwords-1)<<5)&0xffe0)|XPROM_Buffer_Program;
    cfeb_do(10, &comd, 16, &tmp, rcvbuf, LATER);
    // send data
    for(unsigned i=0; i<nwords; i++)
    {
      comd=VTX6_USR2;
      tmp=prm_dat[i];
      cfeb_do(10, &comd, 16, &tmp, rcvbuf, LATER);
    }
    comd=VTX6_BYPASS;
    tmp=0;
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    udelay(40);
    return;
}


void DAQMB::dcfebprom_read(unsigned nwords, unsigned short *pdata)
{
// nwords max. 11 bits (2048 words)
    int toread, leftover;
    unsigned short comd, tmp;
    leftover=nwords;
    while(leftover>0)
    {
      toread = (leftover>16)?16:leftover;
      leftover -= toread;
      
      comd=VTX6_USR1;
      tmp=EPROM_bpi_write; 
      cfeb_do(10, &comd, 8, &tmp, rcvbuf, LATER);
      // how many words (max 16) to read
      comd=VTX6_USR2;
      tmp= (((toread-1)<<5)&0xffe0)|XPROM_Read_n;
      cfeb_do(10, &comd, 16, &tmp, rcvbuf, LATER);
      comd=VTX6_BYPASS;
      tmp=0;
      cfeb_do(10, &comd, 0, &tmp, rcvbuf, LATER);
      //sleep_vme()
      vme_delay(20);
      // enter READ mode
      comd=VTX6_USR1;
      tmp=EPROM_bpi_read; 
      cfeb_do(10, &comd, 8, &tmp, rcvbuf, LATER);

      // read in words
      comd=VTX6_USR2;
      tmp= 0;  // NOOP
      for(int i=0; i<toread; i++)
      {
        cfeb_do(10, &comd, 16, &tmp, rcvbuf, READ_YES|LATER);
      }
      int sendnow=LATER;
      if(leftover==0) sendnow=NOW;
      comd=VTX6_BYPASS;
      tmp=0;
      cfeb_do(10, &comd, 0, &tmp, rcvbuf, sendnow);
      udelay(10);
    }
    memcpy(pdata, rcvbuf, nwords*2);
    return;
}

// copied from Stan's DCFEB code
// must select a DCFEB first before calling it
void DAQMB::dcfeb_loadparam(int paramblock, int nwords, unsigned short int  *val)
{
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. the starting
      addresses are:

           block 0  007f 0000
           block 1  007f 4000
           block 2  007f 8000
           block 3  007f c000

     the config program takes up the range
                    0000 0000
                    0005 4000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf

                   2-byte words       bytes
      eprom size     0x00800000        0x01000000
      mcs size       0x002A0000        0x00540000
      params addr    0x007f0000        0x00fe0000
      
      mcs file addressing is in bytes
*/

  unsigned int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int nxt_blk_addr;

  if(nwords>2048){
    printf(" Catastrophy:parameter space large rewrite program %d \n",nwords);
    return;
  }
  dcfeb_bpi_reset();
  dcfeb_bpi_enable();
  dcfebprom_timerstop();
  dcfebprom_timerreset();
  uaddr=0x007f;  // segment address for parameter blocks
  laddr=paramblock*0x4000;
  fulladdr = (uaddr<<16) + laddr;
  printf(" parameter_write fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  dcfebprom_timerstart();

  dcfebprom_loadaddress(uaddr,laddr);
  // unlock and erase the block
  dcfebprom_unlockerase();

  udelay(400000);

  // program with new data from the beginning of the block
  dcfebprom_bufferprogram(nwords,val);
  unsigned int sleeep=1984*64+164;
  udelay(sleeep);
  
  nxt_blk_addr=fulladdr+0x4000;
  uaddr = (nxt_blk_addr >> 16);
  laddr = nxt_blk_addr &0xffff;
  // printf(" lock address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  dcfebprom_loadaddress(uaddr,laddr);
  // lock last block
  dcfebprom_lock();
  udelay(500000);
  dcfeb_bpi_disable();
  udelay(10);
}

// must select a DCFEB first before calling it
void DAQMB::dcfeb_readparam(int paramblock,int nwords,unsigned short int  *val)
{
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. The starting
      addresses are:

           block 0  007f 0000
           block 1  007f 4000
           block 2  007f 8000
           block 3  007f c000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf 
   */
  
  unsigned int uaddr,laddr;
  if(paramblock<0 || paramblock>3) return;
  
  if(nwords>2048){
    printf(" Catastrophy: parameter space too large: %d\n",nwords);
    return;
  }
  dcfeb_bpi_reset();
  dcfeb_bpi_enable();
  uaddr=0x007f;  // segment address for parameter blocks
  laddr=paramblock*0x4000;
  printf(" parameter_read fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  dcfebprom_loadaddress(uaddr,laddr);
  dcfebprom_read(nwords,val);
  dcfeb_bpi_disable();
}

void DAQMB::dcfeb_readfirmware_mcs(CFEB & cfeb, const char *filename)
{

   unsigned fulladdr=0, uaddr, laddr;
   unsigned read_size=0x800;
   unsigned short *buf;
   FILE *mcsfile;
   int total_blocks=1335;
// int readback_size=read_size*total_blocks*2=5468160; 
// XC6VLX130T's configuration bitstream (firmware) is exactly 5464972 bytes:
   const int FIRMWARE_SIZE=5464972;

   mcsfile=fopen(filename, "w");
   if(mcsfile==NULL)
   {
      std::cout << "Unable to open file to write :" << filename << std::endl;
      return;
   }

   write_cfeb_selector(cfeb.SelectorBit());
   buf=(unsigned short *)malloc(8*1024*1024);
   if(buf==NULL) return;
   dcfeb_bpi_reset();
   dcfeb_bpi_enable();
   
   for(int i=0; i< total_blocks; i++)
   {
       uaddr = (fulladdr >> 16);
       laddr = fulladdr &0xffff;
       dcfebprom_loadaddress(uaddr, laddr);

       dcfebprom_read(read_size, buf+i*read_size);

       fulladdr += read_size;
   }
   dcfeb_bpi_disable();

   write_mcs((char *)buf, FIRMWARE_SIZE, mcsfile);
   fclose(mcsfile);
   free(buf);
   std::cout << " Total " << FIRMWARE_SIZE << " bytes are read back from EPROM and saved in mcs-format file: " << filename << std::endl;
   return;
}

void DAQMB::dcfeb_configure(CFEB & cfeb) 
{

  int number_ = cfeb.number();

// these values should be set in the config xml file:
  int pipeline_length_[7]={60,60,60,60,60,60,60};
  int trigger_clk_phase_[7]={0,0,0,0,0,0,0};
  int daq_clk_phase_[7]={0,0,0,0,0,0,0};
  
  write_cfeb_selector(cfeb.SelectorBit());
  
  bool changed = false;
  char bytesToLoad[44], dt[2];

  dt[0] = dt[1] = 0;
  int dthresh = int (4095 * ((3.5 - comp_thresh_cfeb_[number_]) / 3.5));
  for (int i = 0; i < 8; i++)
    {
      dt[0] |= ((dthresh >> (i + 7)) & 1) << (7 - i);
      dt[1] |= ((dthresh >> i) & 1) << (6 - i);
    }
  dt[0] = ((dt[1] << 7) & 0x80) + ((dt[0] >> 1) & 0x7f);
  dt[1] = dt[1] >> 1;

  for (int i = 0; i < 44; i++)
    bytesToLoad[i] = 0;
  dcfeb_readparam(3, 22, (unsigned short int *) bytesToLoad);
  changed = bytesToLoad[36] != comp_mode_cfeb_[number_]
         || bytesToLoad[37] != comp_timing_cfeb_[number_]
         || bytesToLoad[38] != dt[0]
         || bytesToLoad[39] != dt[1]
         || bytesToLoad[40] != pipeline_length_[number_]
         || bytesToLoad[41] != trigger_clk_phase_[number_]
         || bytesToLoad[42] != daq_clk_phase_[number_];

  printf ("current values of parameters in parameter block 3:\n");
  printf ("  comp_mode: %d\n", bytesToLoad[36]);
  printf ("  comp_timing: %d\n", bytesToLoad[37]);
  printf ("  comp_thresh: %d\n", (bytesToLoad[38] & 0x00ff) | (bytesToLoad[39] & 0xff00));
  printf ("  pipeline_length: %d\n", bytesToLoad[40]);
  printf ("  trigger_clk_phase: %d\n", bytesToLoad[41]);
  printf ("  daq_clk_phase: %d\n", bytesToLoad[42]);

  if (changed)
    {
      bytesToLoad[36] = comp_mode_cfeb_[number_];
      bytesToLoad[37] = comp_timing_cfeb_[number_];
      bytesToLoad[38] = dt[0];
      bytesToLoad[39] = dt[1];
      bytesToLoad[40] = pipeline_length_[number_];
      bytesToLoad[41] = trigger_clk_phase_[number_];
      bytesToLoad[42] = daq_clk_phase_[number_];
      bytesToLoad[43] = 0;

      printf ("parameters from configuration file do not match currently stored values\n");
      dcfeb_loadparam(3, 22, (unsigned short int *) bytesToLoad);

      printf ("  new values of parameters in parameter block 3:\n");
      printf ("    comp_mode: %d\n", bytesToLoad[36]);
      printf ("    comp_timing: %d\n", bytesToLoad[37]);
      printf ("    comp_thresh: %d\n", (bytesToLoad[38] & 0x00ff) | (bytesToLoad[39] & 0xff00));
      printf ("    pipeline_length: %d\n", bytesToLoad[40]);
      printf ("    trigger_clk_phase: %d\n", bytesToLoad[41]);
      printf ("    daq_clk_phase: %d\n", bytesToLoad[42]);
    }
  else
    printf ("parameters in configuration file match currently stored values\n");

}

void DAQMB::dcfeb_test_dummy(CFEB & cfeb, int test)
{
// This dummy function can be used in various tests instead of creating a new function which would
// require to recompile everything in PeripheralCore & PeripheralApps
     write_cfeb_selector(cfeb.SelectorBit());
     virtex6_readreg(test);
     
}

unsigned  DAQMB::dcfeb_readreg_virtex6(CFEB & cfeb,int test){
  write_cfeb_selector(cfeb.SelectorBit());                                   
  unsigned out=virtex6_readreg(test);
  return out;         
}

void DAQMB::dcfeb_readreg_statusvirtex6(CFEB& cfeb)
{
  printf("Virtex 6 status: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_STAT));
  return;
}

void DAQMB::dcfeb_readreg_cor0virtex6(CFEB& cfeb)
{
  printf("Virtex 6 cor0: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_COR0));
  return;
}

void DAQMB::dcfeb_readreg_cor1virtex6(CFEB& cfeb)
{
  printf("Virtex 6 cor1: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_COR1));
  return;
}

void DAQMB::dcfeb_readreg_idcodevirtex6(CFEB& cfeb)
{

  printf("Virtex 6 IDCODE: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_IDCODE));
  return;
}

void DAQMB::dcfeb_readreg_farvirtex6(CFEB& cfeb)
{
  unsigned int far=dcfeb_readreg_virtex6(cfeb,VTX6_REG_FAR);
  printf("Virtex 6 IDCODE: %08x \n",far);
}
void DAQMB::dcfeb_readreg_ctl0virtex6(CFEB& cfeb)
{
  printf("Virtex 6 ctl0: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_CTL0));
  return;
}

void DAQMB::dcfeb_readreg_crcvirtex6(CFEB& cfeb)
{
  printf("Virtex 6 CRC: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_CRC));
  return;
}

void DAQMB::dcfeb_readreg_wbstarvirtex6(CFEB& cfeb)
{
  printf("Virtex 6 WBSTAR: %08x \n",dcfeb_readreg_virtex6(cfeb,VTX6_REG_WBSTAR));
  return;
}

void DAQMB::dcfeb_program_eprom(CFEB & cfeb, const char *mcsfile, int broadcast)
{
   unsigned int fulladdr;
   unsigned int uaddr,laddr;
   unsigned int i, blocks, lastblock;

   const int FIRMWARE_SIZE=5464972/2; // in words

   // each eprom block has 0x10000 words
   const int BLOCK_SIZE=0x10000; // in words

   // each write call takes 0x400 words
   const int WRITE_SIZE=0x400;  // in words

// 1. read mcs file
   char *bufin;
   bufin=(char *)malloc(16*1024*1024);
   if(bufin==NULL)  return;
   unsigned short *bufw= (unsigned short *)bufin;
   FILE *fin=fopen(mcsfile,"r");
   if(fin==NULL ) 
   { 
      free(bufin);  
      std::cout << "ERROR: Unable to open MCS file :" << mcsfile << std::endl;
      return; 
   }
   int mcssize=read_mcs(bufin, fin);
   fclose(fin);
   std::cout << "Read MCS size: " << mcssize << " bytes" << std::endl;
   if(mcssize<FIRMWARE_SIZE)
   {
       std::cout << "ERROR: Wrong MCS file. Quit..." << std::endl;
       free(bufin);
       return;
   }

   if(broadcast)
      write_cfeb_selector(0x7F);   // broadcast to all DCFEBs
   else
      write_cfeb_selector(cfeb.SelectorBit());

      dcfeb_bpi_reset();
      dcfeb_bpi_enable();
      dcfebprom_timerstop();
      dcfebprom_timerreset();
      dcfebprom_timerstart();
// 2. erase eprom
   blocks=FIRMWARE_SIZE/BLOCK_SIZE;
   if((FIRMWARE_SIZE%BLOCK_SIZE)>0) blocks++;
   std::cout << "Erasing EPROM..." << std::endl;
   for(i=0; i<blocks; i++)
   {
      uaddr=i;
      laddr=0;

      // printf(" eprom_load fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
      dcfebprom_loadaddress(uaddr,laddr);
      // unlock and erase the block
      dcfebprom_unlockerase();

      udelay(2000000);
   }

// 3. write eprom
   blocks=FIRMWARE_SIZE/WRITE_SIZE;
   lastblock=FIRMWARE_SIZE%WRITE_SIZE;
   int p1pct=blocks/100;
   int j=0, pcnts=0;
   if(lastblock>0) blocks++;
   else lastblock=WRITE_SIZE;
   std::cout << "Start programming EPROM..." << std::endl;
   fulladdr=0;
   for(i=0; i<blocks; i++)  
   {
      int nwords=WRITE_SIZE;
      if(i==blocks-1) nwords=lastblock;
      uaddr = (fulladdr >> 16);
      laddr = fulladdr &0xffff;
      // printf(" load address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
      dcfebprom_loadaddress(uaddr,laddr);
      // program with new data from the beginning of the block
      dcfebprom_bufferprogram(nwords,bufw+i*WRITE_SIZE);
      udelay(120000);
      fulladdr += WRITE_SIZE;
       j++;
       if(j==p1pct)
       {  pcnts++;
          if(pcnts<100) std::cout << "Sending " << pcnts <<"%..." << std::endl;
          j=0;
       }   
   }
    std::cout << "Sending 100%..." << std::endl;
   uaddr = (fulladdr >> 16);
   laddr = fulladdr &0xffff;
   // printf(" lock address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
   dcfebprom_loadaddress(uaddr,laddr);
   dcfebprom_lock();
   udelay(100000);
   dcfeb_bpi_disable();
   free(bufin);
}

void DAQMB::dcfeb_program_virtex6(CFEB & cfeb, const char *mcsfile)
{
   const int FIRMWARE_SIZE=5464972; // in bytes
   char *bufin, c;
   bufin=(char *)malloc(16*1024*1024);
   if(bufin==NULL)  return;
   FILE *fin=fopen(mcsfile,"r");
   if(fin==NULL ) 
   { 
      free(bufin);  
      std::cout << "ERROR: Unable to open MCS file :" << mcsfile << std::endl;
      return; 
   }
   int mcssize=read_mcs(bufin, fin);
   fclose(fin);
   std::cout << "Read MCS size: " << mcssize << " bytes" << std::endl;
   if(mcssize<FIRMWARE_SIZE)
   {
       std::cout << "ERROR: Wrong MCS file. Quit..." << std::endl;
       free(bufin);
       return;
   }
// byte swap
   for(int i=0; i<FIRMWARE_SIZE/2; i++)
   {  c=bufin[i*2];
      bufin[i*2]=bufin[i*2+1];
      bufin[i*2+1]=c;
   }
     write_cfeb_selector(cfeb.SelectorBit());
     int blocks=FIRMWARE_SIZE/4;  // firmware size must be in units of 32-bit words
     int p1pct=blocks/100;
     int j=0, pcnts=0;
     unsigned short comd, tmp;
//
// The IEEE 1532 ISC (In-System-Configuration) procedure is used.       
// The bitstream doesn't need to be sent in one JTAG package.
// It is different from Xilinx's Jtag procedure which uses CFG_IN.
//
     comd=VTX6_JPROG;
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);

     comd=VTX6_ISC_NOOP; 
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
     udelay(200000);
     comd=VTX6_ISC_ENABLE; 
     tmp=0;
     cfeb_do(10, &comd, 5, &tmp, rcvbuf, NOW);
//    cfeb_do(0, &comd, -200, &tmp, rcvbuf, NOW);
     udelay(100);
     comd=VTX6_ISC_PROGRAM; 
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    for(int i=0; i<blocks; i++)
    {
       cfeb_do(0, &comd, 32, bufin+4*i, rcvbuf, NOW);
       udelay(32);
       j++;
       if(j==p1pct)
       {  pcnts++;
          if(pcnts<100) std::cout << "Sending " << pcnts <<"%..." << std::endl;
          j=0;
       }   
    }
    std::cout << "Sending 100%..." << std::endl;

    comd=VTX6_ISC_DISABLE; 
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
//    cfeb_do(0, &comd, -100, &tmp, rcvbuf, NOW);
    udelay(100);
    comd=VTX6_BYPASS;
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);

    comd=VTX6_JSTART;
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    std::cout <<" Start sending clocks... " << std::endl;
    cfeb_do(0, &comd, -4000, &tmp, rcvbuf, NOW);
    //restore idle;
    cfeb_do(-1, &comd, 0, &tmp, rcvbuf, NOW);    
    comd=VTX6_BYPASS;
    cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    udelay(10);
    std::cout << "FPGA configuration done!" << std::endl;             
    free(bufin);
}


unsigned DAQMB::virtex6_readreg(int reg)
{
   unsigned short comd;
   unsigned data[7]={0x66AA9955, 4, 0, 4, 4, 4};
   unsigned *rt, rtv;
   comd=VTX6_CFG_IN;
   unsigned ins=((reg&0x1F)<<13)+(1<<27)+(1<<29)+1;
   data[2]=shuffle32(ins);
   cfeb_do(10, &comd, 6*32, data, rcvbuf, LATER);
   comd=VTX6_CFG_OUT;
   data[0]=0;
   cfeb_do(10, &comd, 32, data, rcvbuf, NOW|READ_YES);     
   rt = (unsigned *)rcvbuf;
   rtv=shuffle32(*rt);
   // printf("return: %08X\n", rtv);
   comd=VTX6_BYPASS;
   cfeb_do(10, &comd, 0, data, rcvbuf, NOW);
   udelay(10);
   return rtv;
}

void DAQMB::virtex6_writereg(int reg, unsigned value)
{
   unsigned short comd;
   unsigned data[6]={0x66AA9955, 4, 0, 0, 4, 4};
   comd=VTX6_CFG_IN;
   unsigned ins=((reg&0x1F)<<13)+(2<<27)+(1<<29)+1;
   data[2]=shuffle32(ins);
   data[3]=shuffle32(value);
   cfeb_do(10, &comd, 6*32, data, rcvbuf, NOW);
   comd=VTX6_BYPASS;
   cfeb_do(10, &comd, 0, data, rcvbuf, NOW);
   udelay(10);
}

void DAQMB::dcfeb_fpga_shutdown()
{
   unsigned short comd;
   unsigned ndata[14]={0x66AA9955, 4, 0x8001000C, 0xD0000000, 4, 0x8001000C, 0xE0000000, 4, 4, 4, 4, 4, 4, 4};
   comd=VTX6_CFG_IN;
   cfeb_do(10, &comd, 14*32, ndata, rcvbuf, NOW);
   udelay(50000);
}

int DAQMB::dcfeb_dna(CFEB & cfeb, void *dna)
{
     unsigned short comd, tmp;
     unsigned char *dout;
     unsigned short data[4]={0,0,0,0};
     int rtv;
     
     write_cfeb_selector(cfeb.SelectorBit());

     // shutdown FPGA
     dcfeb_fpga_shutdown();

     // random bits as signature
     data[0]=((int)time(NULL) & 0xFF);

     dout=(unsigned char *)dna;
     comd=VTX6_ISC_ENABLE;
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
     udelay(1000);
     comd=VTX6_ISC_DNA;
     cfeb_do(10, &comd, 64, data, (char *)dna, NOW|READ_YES);

//      printf("DNA: %02x %02X %02x %02X %02x %02X %02x %02X\n",
//            dout[7], dout[6], dout[5], dout[4], dout[3], dout[2], dout[1], dout[0]);
                                       
     // the last 7 bits must be the same as the signature's lowest 7 bits
     if((dout[7]>>1)==(data[0]&0x7F))
     {
        shuffle57(dout);
        rtv=0;
     }
     else
     {
        rtv=-1;
        std::cout << "Error: DNA readback verification failed!" << std::endl;
     }
     comd=VTX6_BYPASS;
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
     udelay(100);

     // restart FPGA
     comd=VTX6_JSTART;
     cfeb_do(10, &comd, 0, &tmp, rcvbuf, NOW);
     cfeb_do(0, &comd, -4000, &tmp, rcvbuf, NOW);
     udelay(1000000);
     // restore Idle state
     cfeb_do(-1, &comd, 0, &tmp, rcvbuf, NOW);

     return rtv;
}

std::vector<float> DAQMB::dcfeb_adc(CFEB & cfeb)
{
     std::vector<float> readout;
     int temp;
     char data[4]={0,0,0,0}, tchan[8]={0,4,2,6,1,5,3,7};
     float Vout, therm;
     
     readout.clear();
     write_cfeb_selector(cfeb.SelectorBit());
     for(int chan=0; chan<8; chan++)
     {
       data[0]=0x91|(tchan[chan&7]<<1);
       dcfeb_core(ADC_ctrl, 25, data, (char *)&temp, NOW|NO_BYPASS);
       udelay(1000);
       data[0]=0;
       temp=0;
       dcfeb_core(ADC_rdbk, 16, data, (char *)&temp, NOW|READ_YES|NOOP_YES);     
       udelay(100);
       Vout=(temp&0xFFF)/1000.;  // convert into Volts 
       if(chan==3 || chan==4)
       {
           // channel 3 & 4 are thermistors; convert into degree C
           therm=1./(1.049406423E-3+2.133635468E-4*log(65000.0/Vout-13000.0)+7.522287E-8*pow(log(65000.0/Vout-13000.0),3.0))-273.15;
           readout.push_back(therm);
       }
       else 
       { 
          readout.push_back(Vout);
       }
     }
     return readout;
}

void DAQMB::dcfeb_adc_finedelay(CFEB & cfeb, unsigned short finedelay)
{
  unsigned short temp;
  dcfeb_hub(cfeb, ADC_Fine_Delay, 3, &finedelay, (char *)&temp, NOW|READ_YES|NOOP_YES);
  return;
}

unsigned DAQMB::dcfeb_startup_status(CFEB & cfeb)
{
  /*
         variable        bits
         --------        ----
         por_state        0-2
         eos              3
         al_status        4-6
         run              7
         adc_rdy          8
         daq_mmcm_lock    9
         trg_mmcm_lock   10
         0               11
         0               12
         qpll_cnt_ovrflw 13
         qpll_error      14
         qpll_lock       15
   */
  unsigned temp=0;
  char buf[4]={0,0,0,0};
  dcfeb_hub(cfeb, STARTUP_STATUS, 16, buf, (char *)&temp, NOW|READ_YES);
  return temp&0xFFFF;
}

unsigned DAQMB::dcfeb_qpll_lost_count(CFEB & cfeb)
{
  unsigned temp=0;
  char buf[4]={0,0,0,0};
  dcfeb_hub(cfeb, QPLL_LK_LOST_CNT, 8, buf, (char *)&temp, NOW|READ_YES);
  return temp&0xFF;
}

int DAQMB::LVDB_map(int chn)
{
     // return the LVDB position (0-7) of channel <chn> (for CFEB<chn+1>)

     int lvdb7_f[8]={3,6,5,4,2,0,1,7};  // on LVDB: 6,7,5,1,4,3,2,8
     int lvdb7_b[8] ={3,2,1,0,4,5,6,7};  // on LVDB: 4,3,2,1,5,6,7,8
     if(chn<0 || chn>7) return 0;
     if(hardware_version_<=1 || lvdb_mapping_==0) return chn;
     if(lvdb_mapping_==1) return lvdb7_f[chn];
     else if(lvdb_mapping_==2) return lvdb7_b[chn];
     else return chn;
}

// ODMB discrete logic JTAG port.This method has exactly the same interface as cfeb_do(). 
void DAQMB::dlog_do(int ncmd, void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd)
{
     daqmb_do(ncmd, cmd, nbuf, inbuf, outbuf, irdsnd, 0);
}

void DAQMB::odmb_fpga_call(int inst, unsigned data, char *outbuf)
{
  char temp[4];
  dlog_do(10, &inst, 32, &data, outbuf, NOW|READ_YES);
  udelay(100);
  int comd=VTX6_BYPASS;
  dlog_do(10, &comd, 0, &data, temp, NOW);
  udelay(20);
}

void DAQMB::daqmb_do(int ncmd, void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd, int dev)
{
     // dev=0  discrete logic (O/DMB emergency PROM access)
     //    =1  D/CFEB      (device 1 direct)
     //    =2  O/DMB FPGA  (device 2)
     //    =3  DMB PROM    (device 3)
     //    =4  VME PROM    (device 4)
     //    =5  CFEB PROM   (device 1 with head-tail)
     //    =6  CFEB FPGA   (device 1 with head-tail)

     // irdsnd for jtag
     //     irdsnd = 0 no read, later
     //     irdsnd = 1 no read, now
     //     irdsnd = 2    read, later
     //     irdsnd = 3    read, now
     //

  if(dev<0 || dev>6) return;
  int DAQMB_DEV;
  char tmp[2];
  int ncmd_u, nbuf_u;
  
  if(dev==0)
  {
     int DAQMB_DEV=0xFFFC;
     if(ncmd<0)
     { // Reset Jtag State Machine
         Jtag_Lite(DAQMB_DEV, 0, tmp,-1, tmp, 0, (irdsnd&1));
         return;
     }
     if(ncmd>0) Jtag_Lite(DAQMB_DEV, 0, (char *)cmd,ncmd, outbuf,0,(nbuf>0)?LATER:(irdsnd&NOW));
     //   if(ncmd>0 && nbuf>0) vme_delay(100); 
     if(nbuf>0) Jtag_Lite(DAQMB_DEV, 1,(char *)inbuf,nbuf,outbuf,(irdsnd>>1)&1,irdsnd&NOW);

     // send empty clocks |nbuf|, inbuf & outbuf not used
     if(nbuf<0) Jtag_Lite(DAQMB_DEV, 2, (char *)inbuf, -nbuf, outbuf, 0, irdsnd&NOW);
  }
  else
  {  
     ncmd_u=ncmd;
     nbuf_u=nbuf;
     if(dev==5) 
     { 
         DAQMB_DEV=1;
         if(ncmd>0) { add_headtail((char *)cmd, ncmd, 0, 5);   ncmd_u += 5; }
         if(nbuf>0) { add_headtail((char *)inbuf, nbuf, 0, 1); nbuf_u += 1; }
     } 
     else if(dev==6)
     { 
         DAQMB_DEV=1;
         if(ncmd>0) { add_headtail((char *)cmd, ncmd, 8, 0);   ncmd_u += 8; }
         if(nbuf>0) { add_headtail((char *)inbuf, nbuf, 1, 0); nbuf_u += 1; }
     } 
     else
     {
         DAQMB_DEV=dev;
     } 
     if(ncmd<0)
     { // Reset Jtag State Machine
         Jtag_Ohio(DAQMB_DEV, 0, tmp,-1, tmp, 0, (irdsnd&1));
         return;
     }
     if(ncmd>0) Jtag_Ohio(DAQMB_DEV, 0, (char *)cmd, ncmd_u, outbuf,0,(nbuf>0)?LATER:(irdsnd&NOW));
     //   if(ncmd>0 && nbuf>0) sleep_vme(200); 
     if(nbuf>0) Jtag_Ohio(DAQMB_DEV, 1,(char *)inbuf,nbuf_u, outbuf,(irdsnd>>1)&1,irdsnd&NOW);

     // send empty clocks |nbuf|, inbuf & outbuf not used
     if(nbuf<0) Jtag_Ohio(DAQMB_DEV, 2, (char *)inbuf, -nbuf, outbuf, 0, irdsnd&NOW);

     // !!!!!>>>> Never buffer read with odd bits (nbuf%16!=0)
     if((irdsnd&3)==3 && nbuf%16!=0)
     {
        // The last short-word (16 bit) contains the real data bits
        // at the MSB. Must shift them to the LSB. 
        int ishft=16-nbuf%16;
        unsigned temp=((outbuf[2*(nbuf/16)+1]<<8)&0xff00)|(outbuf[2*(nbuf/16)]&0xff);
        temp=(temp>>ishft);
        outbuf[2*(nbuf/16)+1]=(temp&0xff00)>>8;
        outbuf[2*(nbuf/16)]=temp&0x00ff;
     }

     if(dev==5) 
     { 
         if(nbuf>0 && (irdsnd&3)==3) cut_headtail((char *)outbuf, nbuf_u, 0, 1);
     } 
     else if(dev==6)
     { 
         if(nbuf>0 && (irdsnd&3)==3) cut_headtail((char *)outbuf, nbuf_u, 1, 0);
     } 
     
  }
}

std::vector<float> DAQMB::odmb_fpga_sysmon()
{
  // only read out first 3 channels
  
  std::vector<float> readout;
  char buf[4]={0,0,0,0};
  int comd=VTX6_SYSMON;
  unsigned data, ibrd, adc;
  float readf;

  readout.clear();
  if(hardware_version_==2)
  {
     comd=VTX6_SYSMON;
//     this can be used to change register 0x48 to enable more channels
//     data=0x8483F00;
//     cfeb_do(10, &comd, 32, &data, rcvbuf, 3);
     data=0x4000000;
     dlog_do(10, &comd, 32, &data, rcvbuf, NOW|READ_YES);
     udelay(100);
     for(unsigned i=0; i<3; i++)
     {
        data += 0x10000;
        dlog_do(0, buf, 32, &data, (char *)&ibrd, NOW|READ_YES);
        udelay(100);
        adc = (ibrd>>6)&0x3FF;
        if(i==0)
          readf=adc*503.975/1024.0-273.15;
        else
          readf=adc*3.0/1024.0;
        readout.push_back(readf);
     }
     comd=VTX6_BYPASS;
     dlog_do(10, &comd, 0, &data, rcvbuf, NOW);
     udelay(100);
  }
  return readout;
}

std::vector<float> DAQMB::odmb_fpga_adc()
{

  unsigned short addoff[9]={0, 0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170};   
  float vnorm[9]={0., 3.3, 5.0, 0., 3.3, 2.5, 0., 1.0, 5.0};
  std::vector<float> readout;

  int adc;
  float readf;

  readout.clear();
  if(hardware_version_==2)
  {
     for(int i=0; i<9; i++)
     {
        adc = ReadRegister(FPGA_ADC_BASE+addoff[i])&0xFFF;
        if(i==0)
        {
          readf=adc*503.975/4096.0-273.15;
        }
        else if(i==3 || i==6)
        {
            float Vout=adc;
            readf=7.865766417e-10*pow(Vout,3) - 7.327237418e-6*pow(Vout,2) + 3.38189673e-2*Vout - 9.678340882;
        }
        else
        {
          readf=adc*vnorm[i]/2048.0;
        }
        readout.push_back(readf);
     }
  }
  return readout;
}

int DAQMB::DCSread2(char *data)
{

// add DCFEB monitoring info here
//     loop through all DCFEBs
//     {    1. DCFEB SYSMON
//          2. DCFEB ADC
//     }

  int retn=0;
  short *data2= (short *)data;
  int TOTAL_SYSMON=19;
  int TOTAL_ADC=8;
  int TOTAL_DCFEB=TOTAL_SYSMON+TOTAL_ADC;
  int TOTAL_ODMB=9;

  if (hardware_version_!=2) return 0;

  if(checkvme_fail()) return 0;

  for(unsigned lfeb=0; lfeb<cfebs_.size();lfeb++)
  {
      std::vector<float> fsysmon=dcfeb_fpga_monitor(cfebs_[lfeb]);
      int febnum=cfebs_[lfeb].number();
      for(unsigned i=0; i<fsysmon.size(); i++)
      {
         data2[febnum*TOTAL_DCFEB+i]=int(fsysmon[i]*100);
      }
      fsysmon.clear();
      std::vector<float> dadc=dcfeb_adc(cfebs_[lfeb]);
      for(unsigned i=0; i<dadc.size(); i++)
      {
         data2[febnum*TOTAL_DCFEB+TOTAL_SYSMON+i]=int(dadc[i]*100);
      }
      retn += TOTAL_DCFEB;
  }
  std::vector<float> dsysmon=odmb_fpga_adc();
  for(unsigned int i=0; i<dsysmon.size(); i++)
  {
      data2[retn+i]=int(dsysmon[i]*100);
  }        
  retn += TOTAL_ODMB;
  return retn;
}

void DAQMB::odmb_XPROM_do(unsigned short command)
{
  WriteRegister(BPI_Write, command);
}

void DAQMB::odmb_bpi_reset()
{  WriteRegister(BPI_Reset, 0); }

void DAQMB::odmb_bpi_disable()
{  WriteRegister(BPI_Disable, 0); }

void DAQMB::odmb_bpi_enable()
{  WriteRegister(BPI_Enable, 0); }

unsigned DAQMB::odmb_bpi_readtimer()
{
  return (ReadRegister(BPI_Timer_h)<<16)+ReadRegister(BPI_Timer_l);
}

unsigned short DAQMB::odmb_bpi_status()
{
  /* status register
     low 8 bits XLINK
      0-blank write status/multiple work program status
      1-block protection status
      2-program suspend status
      3-vpp status
      4-program status
      5-erase/blank check status
      6-erase/suspend status
      7-P.E.C. Status
     high 8 bits Ben
      0-cmd fifo write error
      1-cmd fifo read error
      2-cmd fifo full 
      3-cmd fifo empty
      4-rbk fifo write error
      5-rbk fifo read error
      6-rbk fifo full 
      7-rbk fifo empty
  */  
  return ReadRegister(BPI_Status);
}

void DAQMB::odmbeprom_multi(int cnt, unsigned short *manbuf)
{
    for(int i=0; i<cnt; i++)
    {
      WriteRegister(BPI_Write, manbuf[i]);
      udelay(30);                             
    }
    return;
}

    bool DAQMB::odmbeprom_cmd_fifo_empty(unsigned int poll_interval /*us*/)
    {
      const unsigned int max_cnt = 40000000/poll_interval;
      unsigned int cnt = 0;
      while ((odmb_bpi_status() & 0x0F00) != 0x0800) {	
	udelay(poll_interval);
	cnt++;
	// if (odmb_eprom_debug) printf("line %d: BPI status (cnt = %d) = %x\n",__LINE__, cnt, status);
	if (cnt >= max_cnt) return false;
      }

      return true;
    }

    bool DAQMB::odmbeprom_pec_ready(unsigned int poll_interval /*us*/)
    {
      const unsigned int max_cnt = 40000000/poll_interval;
      udelay (poll_interval);
      unsigned int cnt = 0;
      while ((odmb_bpi_status() & 0xFFF0) != 0x8880) {	
	udelay(poll_interval);
	cnt++;
	// if (odmb_eprom_debug) printf("line %d: BPI status (cnt = %d) = %x\n",__LINE__, cnt, status);
	if (cnt >= max_cnt) return false;
      }

      return true;
    }

void DAQMB::odmbeprom_unlockerase() 
{ 
   unsigned short tmp[2];
   tmp[0]=XPROM_Block_UnLock;
   tmp[1]=XPROM_Block_Erase;
   odmbeprom_multi(2, tmp);
}

void DAQMB::odmbeprom_loadaddress(unsigned short uaddr, unsigned short laddr) 
{ 
   unsigned short tmp[2];
   tmp[0]=((uaddr<<5)&0xffe0)|XPROM_Load_Address;
   tmp[1]=laddr;
   odmbeprom_multi(2, tmp);
}

void DAQMB::odmbeprom_bufferprogram(unsigned nwords,unsigned short *prm_dat)
{
    // nwords max. 11 bits (2048 words)
    unsigned short tmp;
    tmp= (((nwords-1)<<5)&0xffe0)|XPROM_Buffer_Program;
    odmb_XPROM_do(tmp);
    udelay(40);

    // send data
    odmbeprom_multi(nwords, prm_dat);
    return;
}

void DAQMB::odmbeprom_read(unsigned nwords, unsigned short *pdata)
{
    // nwords max. 11 bits (2048 words)
    int toread, leftover, pindex=0;
    unsigned short tmp;
    leftover=nwords;
    while(leftover>0)
    {
      toread = (leftover>16)?16:leftover;
      leftover -= toread;
      
      tmp= (((toread-1)<<5)&0xffe0)|XPROM_Read_n;
      odmb_XPROM_do(tmp);
      udelay(20);
      // read in words
      for(int i=0; i<toread; i++)
      {
        pdata[pindex]=ReadRegister(BPI_Read);
        pindex++;  
      }
    }
    return;
}

void DAQMB::odmb_loadparam(int paramblock, int nwords, unsigned short int  *val)
{
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. the starting
      addresses are:

           block 0  007f 0000
           block 1  007f 4000
           block 2  007f 8000
           block 3  007f c000

     the config program takes up the range
                    0000 0000
                    0005 4000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf

                   2-byte words       bytes
      eprom size     0x00800000        0x01000000
      mcs size       0x002A0000        0x00540000
      params addr    0x007f0000        0x00fe0000
      
      mcs file addressing is in bytes
*/

  unsigned int fulladdr;
  unsigned int uaddr,laddr;
  unsigned int nxt_blk_addr;

  if(nwords>2048){
    printf(" Catastrophy:parameter space large rewrite program %d \n",nwords);
    return;
  }
  odmb_bpi_reset();
  odmb_bpi_enable();
  odmbeprom_timerstop();
  odmbeprom_timerreset();
  uaddr=0x007f;  // segment address for parameter blocks
  laddr=paramblock*0x4000;
  fulladdr = (uaddr<<16) + laddr;
  printf(" parameter_write fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  odmbeprom_timerstart();

  odmbeprom_loadaddress(uaddr,laddr);
  // unlock and erase the block
  odmbeprom_unlockerase();

  udelay(400000);

  // program with new data from the beginning of the block
  odmbeprom_bufferprogram(nwords,val);
  unsigned int sleeep=1984*64+164;
  udelay(sleeep);
  
  nxt_blk_addr=fulladdr+0x4000;
  uaddr = (nxt_blk_addr >> 16);
  laddr = nxt_blk_addr &0xffff;
  // printf(" lock address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  odmbeprom_loadaddress(uaddr,laddr);
  // lock last block
  odmbeprom_lock();
  odmb_bpi_disable();
  udelay(10);
}

void DAQMB::odmb_readparam(int paramblock,int nwords,unsigned short int  *val)
{
  /*  The highest four blocks in the eprom are parameter banks of
      length 16k 16 bit words. The starting
      addresses are:

           block 0  007f 0000
           block 1  007f 4000
           block 2  007f 8000
           block 3  007f c000

      ref: http://www.xilinx.com/support/documentation/data_sheets/ds617.pdf 
   */
  
  unsigned int uaddr,laddr;
  if(paramblock<0 || paramblock>3) return;
  
  if(nwords>2048){
    printf(" Catastrophy: parameter space too large: %d\n",nwords);
    return;
  }
  odmb_bpi_reset();
  odmb_bpi_enable();
  uaddr=0x007f;  // segment address for parameter blocks
  laddr=paramblock*0x4000;
  printf(" parameter_read fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
  odmbeprom_loadaddress(uaddr,laddr);
  odmbeprom_read(nwords,val);
  odmb_bpi_disable();
}

void DAQMB::odmb_readfirmware_mcs(const char *filename)
{

   unsigned fulladdr=0, uaddr, laddr;
   unsigned read_size=0x800;
   unsigned short *buf;
   FILE *mcsfile;
   int total_blocks=1335;
// int readback_size=read_size*total_blocks*2=5468160; 
// XC6VLX130T's configuration bitstream (firmware) is exactly 5464972 bytes:

   const int FIRMWARE_SIZE=5464972;

   mcsfile=fopen(filename, "w");
   if(mcsfile==NULL)
   {
      std::cout << "Unable to open file to write :" << filename << std::endl;
      return;
   }

   buf=(unsigned short *)malloc(8*1024*1024);
   if(buf==NULL) return;
   odmb_bpi_reset();
   odmb_bpi_enable();
   
   for(int i=0; i< total_blocks; i++)
   {
       uaddr = (fulladdr >> 16);
       laddr = fulladdr &0xffff;
       odmbeprom_loadaddress(uaddr, laddr);

       odmbeprom_read(read_size, buf+i*read_size);

       fulladdr += read_size;
   }
   odmb_bpi_disable();
 
   write_mcs((char *)buf, FIRMWARE_SIZE, mcsfile);
   fclose(mcsfile);
   free(buf);
   std::cout << " Total " << FIRMWARE_SIZE << " bytes are read back from ODMB's EPROM and saved in mcs-format file: " << filename << std::endl;
   return;
}

void DAQMB::odmb_program_eprom(const char *mcsfile)
{
   unsigned int fulladdr;
   unsigned int uaddr,laddr;
   unsigned int i, blocks, lastblock;

   const int FIRMWARE_SIZE=5464972/2; // in words

   // each eprom block has 0x10000 words
   const int BLOCK_SIZE=0x10000; // in words

   // each write call takes 0x400 words
   const int WRITE_SIZE=0x400;  // in words

// 1. read mcs file
   char *bufin;
   bufin=(char *)malloc(16*1024*1024);
   if(bufin==NULL)  return;
   unsigned short *bufw= (unsigned short *)bufin;
   FILE *fin=fopen(mcsfile,"r");
   if(fin==NULL ) 
   { 
      free(bufin);  
      std::cout << "ERROR: Unable to open MCS file :" << mcsfile << std::endl;
      return; 
   }
   int mcssize=read_mcs(bufin, fin);
   fclose(fin);
   std::cout << "Read MCS size: " << mcssize << " bytes" << std::endl;
   if(mcssize<FIRMWARE_SIZE)
   {
       std::cout << "ERROR: Wrong MCS file. Quit..." << std::endl;
       free(bufin);
       return;
   }

      odmb_bpi_reset();
      odmb_bpi_enable();
      odmbeprom_timerstop();
      odmbeprom_timerreset();
      odmbeprom_timerstart();
// 2. erase eprom
   blocks=FIRMWARE_SIZE/BLOCK_SIZE;
   if((FIRMWARE_SIZE%BLOCK_SIZE)>0) blocks++;
   std::cout << "Erasing EPROM..." << std::endl;
   for(i=0; i<blocks; i++)
   {
      uaddr=i;
      laddr=0;

      // printf(" eprom_load fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
      odmbeprom_loadaddress(uaddr,laddr);
      // unlock and erase the block
      odmbeprom_unlockerase();

      udelay(2000000);
   }

// 3. write eprom
   blocks=FIRMWARE_SIZE/WRITE_SIZE;
   lastblock=FIRMWARE_SIZE%WRITE_SIZE;
   int p1pct=blocks/100;
   int j=0, pcnts=0;
   if(lastblock>0) blocks++;
   else lastblock=WRITE_SIZE;
   std::cout << "Start programming EPROM..." << std::endl;
   fulladdr=0;
   for(i=0; i<blocks; i++)  
   {
      int nwords=WRITE_SIZE;
      if(i==blocks-1) nwords=lastblock;
      uaddr = (fulladdr >> 16);
      laddr = fulladdr &0xffff;
      // printf(" load address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
      odmbeprom_loadaddress(uaddr,laddr);
      // program with new data from the beginning of the block
      odmbeprom_bufferprogram(nwords,bufw+i*WRITE_SIZE);
      udelay(120000);
      fulladdr += WRITE_SIZE;
       j++;
       if(j==p1pct)
       {  pcnts++;
          if(pcnts<100) std::cout << "Sending " << pcnts <<"%..." << std::endl;
          j=0;
       }   
   }
    std::cout << "Sending 100%..." << std::endl;
   uaddr = (fulladdr >> 16);
   laddr = fulladdr &0xffff;
   // printf(" lock address %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
   odmbeprom_loadaddress(uaddr,laddr);
   odmbeprom_lock();
   udelay(100000);
   odmb_bpi_disable();
   free(bufin);
}

    bool DAQMB::odmb_program_eprom_poll(const char *mcsfile)
    {
      bool odmb_eprom_debug=false;

      unsigned int fulladdr;
      unsigned int uaddr,laddr;
      unsigned int i, blocks, lastblock;

      const int FIRMWARE_SIZE=5464972/2; // in words

      // each eprom block has 0x10000 words
      const int BLOCK_SIZE=0x10000; // in words

      // each write call takes 0x800 words
      const int WRITE_SIZE=0x400;  // in words, was 0x800

      // 1. read mcs file
      char *bufin;
      bufin=(char *)malloc(16*1024*1024);
      if(bufin==NULL) {
	std::cout << "Nothing in bufin." << std::endl;
	free(bufin);
	return false;
      }
      unsigned short *bufw= (unsigned short *)bufin;
      FILE *fin=fopen(mcsfile,"r");
      if(fin==NULL ) 
	{ 
	  free(bufin);  
	  std::cout << "ERROR: Unable to open MCS file :" << mcsfile << std::endl;
	  return false;
	}
      int mcssize=read_mcs(bufin, fin);
      fclose(fin);
      std::cout << "Read MCS size: " << mcssize << " bytes" << std::endl;
      if(mcssize<FIRMWARE_SIZE)
	{
	  free(bufin);
	  std::cout << "ERROR: Wrong MCS file. Quit..." << std::endl;
	  return false;
	}

      odmb_bpi_reset();
      odmbeprom_timerstop();
      odmbeprom_timerreset();
      odmbeprom_timerstart();
      odmb_bpi_enable();
      udelay(1000);
      odmb_bpi_disable();

      // 2. erase eprom
      blocks=FIRMWARE_SIZE/BLOCK_SIZE;
      if((FIRMWARE_SIZE%BLOCK_SIZE)>0) blocks++;
      std::cout << "Erasing EPROM..." << std::endl;
      for(i=0; i<blocks; i++)
	{
	  uaddr=i;
	  laddr=0;

	  odmbeprom_clearstatus();
	  // printf(" eprom_load fulladdr %04x%04x \n",(uaddr&0xFFFF),(laddr&0xFFFF));
	  odmbeprom_loadaddress(uaddr,laddr);
	  // unlock and erase the block
	  odmbeprom_unlockerase();
	  udelay(40);

	  odmb_bpi_enable();
	  udelay(4000);

	  // This is the erase polling that we're trying to implement.  When BPI_STATUS = 8880, 
	  // we are good to go.

	  unsigned int interval = 500000;
	  unsigned int max_count = 4000000/interval;
	  unsigned int cnt = 0;
	  // unsigned int status = odmb_bpi_status();
	  while ((odmb_bpi_status() & 0xFFFF) != 0x8880) {
	    udelay(interval);
	    ++cnt;
	    // status = odmb_bpi_status();
	    if (cnt >= max_count)
	      {
		printf("erase time out for block %d, address %04x%04x with status %04x\n", i, uaddr, laddr, odmb_bpi_status());
		//		throw "took more than 4 seconds to erase a block!";
		free(bufin);
		odmb_bpi_disable();
		return false;
	      }
	  }

	  udelay(100);
	  odmb_bpi_disable();

	}       

      printf("Erase complete.\n");

      // 3. write eprom      
      int global_write_delay = 2000;
      blocks=FIRMWARE_SIZE/WRITE_SIZE;
      lastblock=FIRMWARE_SIZE%WRITE_SIZE;
      int p1pct=blocks/10;
      int j=0, pcnts=0;
      if(lastblock>0) blocks++;
      else lastblock=WRITE_SIZE;
      std::cout << "Start programming EPROM..." << std::endl;
      fulladdr=0;
      if (odmb_eprom_debug) {
	udelay(global_write_delay);
	printf("status before disable %04x\n", odmb_bpi_status());
      }
      odmb_bpi_disable();      
      if (odmb_eprom_debug) {
	udelay(global_write_delay);
	printf("status after disable %04x\n", odmb_bpi_status());
      }
      int nwords=WRITE_SIZE;
      for(i=0; i<blocks; i++)  
	{
	  if(i==blocks-1) nwords=lastblock;
	  uaddr = (fulladdr >> 16);
	  laddr = fulladdr &0xffff;
	  
	  if (odmb_eprom_debug) {
	    printf("beginning write block %i at address %08x\n", i, fulladdr);
	    printf("status before loadaddress %04x\n", odmb_bpi_status());
	    udelay(global_write_delay);
	  }
	  odmbeprom_loadaddress(uaddr,laddr);

	  if (odmb_eprom_debug) {
	    udelay(global_write_delay);
	    printf("status before buffer program %04x\n", odmb_bpi_status());
	    udelay(global_write_delay);
	  }
	  
	  odmbeprom_bufferprogram(nwords, bufw+i*WRITE_SIZE);
	  odmb_bpi_enable();
	  udelay(global_write_delay);
	  // make sure the status goes back to 8880 => FIFOs empty, ready for next chunk.
	  if (!odmbeprom_pec_ready(global_write_delay)) {
	    printf("P/E.C. controller did not return to normal status during program. Exiting with status %04x\n", odmb_bpi_status());
	    //	    throw "BPI parser failed to empty CMD FIFO";
	    free(bufin);
	    odmb_bpi_disable();
	    return false;
	  }
	  udelay(global_write_delay);
	  odmb_bpi_disable();
	  udelay(global_write_delay);
	  unsigned int nwords_rbk = ReadRegister(BPI_Read_n);
	  
	  if (odmb_eprom_debug) {
	    udelay(global_write_delay);
	    printf("\t%d words remaining in RBK FIFO\n", nwords_rbk);	  
	    while(nwords_rbk-- > 0) {
	      udelay(global_write_delay);
	      printf("\t\trbk %d: %04x\n", nwords_rbk, ReadRegister(BPI_Read));
	    }
	  }
	  
	  udelay(global_write_delay);
	  if (odmb_eprom_debug) printf("controller ready status: %04x\n\n", odmb_bpi_status());
	  
	  fulladdr += WRITE_SIZE;
	  
	  j++;
	  if(j==p1pct)
	    {  pcnts++;
	      if(pcnts<100) std::cout << "Sending " << std::dec << pcnts*10 <<"%..." << std::endl;
	      j=0;
	    }   
	}
      std::cout << "Sending 100%..." << std::endl;
      uaddr = (fulladdr >> 16);
      laddr = fulladdr &0xffff;
      if (odmb_eprom_debug) printf("line %d: lock address %04x%04x \n", __LINE__, (uaddr&0xFFFF),(laddr&0xFFFF));
      odmbeprom_loadaddress(uaddr,laddr);
      odmbeprom_lock();      
      udelay(global_write_delay);
      odmb_bpi_enable();
      if (!odmbeprom_pec_ready(global_write_delay))
	{
	  printf("P/E.C. controller did not return to normal status on final lock. Exiting with status %04x\n", odmb_bpi_status());
	  //	  throw "crashing now";
	  odmb_bpi_disable();
	  free(bufin);
	  return false;
	}
      odmb_bpi_disable();
      free(bufin);
      return true;
    }

void DAQMB::odmb_program_virtex6(const char *mcsfile)
{
   const int FIRMWARE_SIZE=5464972; // in bytes
   char *bufin, c;
   bufin=(char *)malloc(16*1024*1024);
   if(bufin==NULL)  return;
   FILE *fin=fopen(mcsfile,"r");
   if(fin==NULL ) 
   { 
      free(bufin);  
      std::cout << "ERROR: Unable to open MCS file :" << mcsfile << std::endl;
      return; 
   }
   int mcssize=read_mcs(bufin, fin);
   fclose(fin);
   std::cout << "Read MCS size: " << mcssize << " bytes" << std::endl;
   if(mcssize<FIRMWARE_SIZE)
   {
       std::cout << "ERROR: Wrong MCS file. Quit..." << std::endl;
       free(bufin);
       return;
   }
// byte swap
   for(int i=0; i<FIRMWARE_SIZE/2; i++)
   {  c=bufin[i*2];
      bufin[i*2]=bufin[i*2+1];
      bufin[i*2+1]=c;
   }
     int blocks=FIRMWARE_SIZE/4;  // firmware size must be in units of 32-bit words
     int p1pct=blocks/100;
     int j=0, pcnts=0;
     unsigned short comd, tmp;
//
// The IEEE 1532 ISC (In-System-Configuration) procedure is used.       
// The bitstream doesn't need to be sent in one JTAG package.
// It is different from Xilinx's Jtag procedure which uses CFG_IN.
//
     comd=VTX6_JPROG;
     dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);

     comd=VTX6_ISC_NOOP; 
     dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);
     udelay(200000);
     comd=VTX6_ISC_ENABLE; 
     tmp=0;
     dlog_do(10, &comd, 5, &tmp, rcvbuf, NOW);
//    dlog_do(0, &comd, -200, &tmp, rcvbuf, NOW);
     udelay(100);
     comd=VTX6_ISC_PROGRAM; 
     dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    for(int i=0; i<blocks; i++)
    {
       dlog_do(0, &comd, 32, bufin+4*i, rcvbuf, NOW);
       udelay(32);
       j++;
       if(j==p1pct)
       {  pcnts++;
          if(pcnts<100) std::cout << "Sending " << pcnts <<"%..." << std::endl;
          j=0;
       }   
    }
    std::cout << "Sending 100%..." << std::endl;

    comd=VTX6_ISC_DISABLE; 
    dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);
//    dlog_do(0, &comd, -100, &tmp, rcvbuf, NOW);
    udelay(100);
    comd=VTX6_BYPASS;
    dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);

    comd=VTX6_JSTART;
    dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    std::cout <<" Start sending clocks... " << std::endl;
    dlog_do(0, &comd, -4000, &tmp, rcvbuf, NOW);
    //restore idle;
    dlog_do(-1, &comd, 0, &tmp, rcvbuf, NOW);    
    comd=VTX6_BYPASS;
    dlog_do(10, &comd, 0, &tmp, rcvbuf, NOW);
    udelay(10);
    std::cout << "FPGA configuration done!" << std::endl;             
    free(bufin);
}

int DAQMB::read_cfeb_done()
{
    int sig=0;
    if(hardware_version_==2)
    {
       sig=ReadRegister(DCFEB_DONE) & 0x7F;
    }
    return sig;
}

int DAQMB::read_qpll_state()
{
    int sig=0;
    if(hardware_version_==2)
    {
       sig=ReadRegister(ODMB_QPLL) & 0xFFFF;
    }
    return sig;
}

int DAQMB::read_odmb_id()
{
    int sig=0;
    if(hardware_version_==2)
    {
       sig=ReadRegister(read_ODMB_ID) & 0xFFFF;
    }
    return sig;
}

void DAQMB::odmb_save_config()
{
    if(hardware_version_==2)
    {
       WriteRegister(ODMB_Save_Config, 0);
       ::sleep(2);
    }
    return;
}

void DAQMB::odmb_retrieve_config()
{
    if(hardware_version_==2)
    {
       WriteRegister(ODMB_Retrieve_Config, 0);
       ::sleep(1);
    }
    return;
}

void DAQMB::chan2shift(int chan[16],unsigned int shft_bits[3]){
    shft_bits[0]=((chan[10]<<15)|(chan[11]<<12)|(chan[12]<<9)|(chan[13]<<6)|(chan[14]<<3)|chan[15])&0XFFFF;
    shft_bits[1]=((chan[5]<<14)|(chan[6]<<11)|(chan[7]<<8)|(chan[8]<<5)|(chan[9]<<2)|(chan[10]>>1))&0XFFFF;
    shft_bits[2]=((chan[0]<<13)|(chan[1]<<10)|(chan[2]<<7)|(chan[3]<<4)|(chan[4]<<1)|(chan[5]>>2))&0XFFFF;
}

void DAQMB::set_dcfeb_parambuffer(CFEB &cfeb, unsigned short int bufload[34]){
    unsigned number = cfeb.number();
    bufload[0]=0x4321;                         //ready (not 0xffff)
    float dthresh=0.05;
    int comp_thresh=int(4095*((3.5-dthresh)/3.5));
    int pipeline_depth = cfeb.GetPipelineDepth();

    bufload[1]=comp_thresh&0x0fff;               // comp. thresh. 12 bits
    bufload[2]=comp_mode_cfeb_[number]&0x0003;             // comp. mode 2 bits
    bufload[3]=comp_timing_cfeb_[number]&0x0007;           // comp. timing 3 bits
    bufload[4]=comp_clk_phase_cfeb_[number]&0x000f;        // comp. CLK phase 4 bits
    bufload[5]=adcsamp_clk_phase_cfeb_[number]&0x0007;     // adc samp. CLK phase 3 bits
    bufload[6]=nsample_cfeb_[number]&0x007f;               // # of samples 7 bits
    bufload[7]=pipeline_depth&0x01ff;         // pipeline depth 9 bits
    for(int i=8;i<16;i++)bufload[i]=0xffff;  // reserved

    unsigned int shft_bits[3];
    int chan[16];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][0])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[16]=shft_bits[0];//bgb order flipped                       // buckshift pln 1
    bufload[17]=shft_bits[1];
    bufload[18]=shft_bits[2];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][1])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[19]=shft_bits[0];                       // buckshift pln 3
    bufload[20]=shft_bits[1];
    bufload[21]=shft_bits[2];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][2])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[22]=shft_bits[0];                       // buckshift pln 5
    bufload[23]=shft_bits[1];
    bufload[24]=shft_bits[2];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][3])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[25]=shft_bits[0];                       // buckshift pln 4
    bufload[26]=shft_bits[1];
    bufload[27]=shft_bits[2];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][4])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[28]=shft_bits[0];                       // buckshift pln 0
    bufload[29]=shft_bits[1];
    bufload[30]=shft_bits[2];
    set_all_chan_norm(chan);
    for(int ichan=0;ichan<16;ichan++){
      unsigned short int mask=(1<<ichan);
      if((mask&kill_chip_[number][5])!=0x0000)set_chan_kill(ichan,chan);
    }
    chan2shift(chan,shft_bits);
    bufload[31]=shft_bits[0];                       // buckshift pln 2
    bufload[32]=shft_bits[1];
    bufload[33]=shft_bits[2];
}

void DAQMB::autoload_select_readback_wrd(CFEB &cfeb, int ival){
  /*
      0 - xtra l1a w 2 bits
      1 - pre block end 4 bits
      2 - comp time[2:0] comp mode [1:0] 5 bits
      3 - buckeye mask  6 bits
      4 - adc mask 12 bits
      5 - adc cnfg mem wrd 26 bits
      6 - pipeline depth 9 bits
      7 - ttc source 2 bits
      8 # samples 7 bits
      9 bpi write fifo 16 bits
     10 comp clock phase 4 bits
     11 samp clock phase 3 bits
     12 tmb transmit mode 3 bits
     13 hs settings 30 bits
     14 tmb layer mask 6 bits
     15 prbs test mode 3 bits
     16 sem cmd 3 bits
     17 reg sel wrd 8 bits
  */
    DEVTYPE dv = cfeb.dscamDevice();
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=REG_SEL_WRD;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=ival;
    sndbuf[1]=0x00;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

void DAQMB::autoload_readback_wrd(CFEB &cfeb, char wrd[2]){
    DEVTYPE dv = cfeb.dscamDevice();
    cmd[0]=(VTX6_USR1&0xff);
    cmd[1]=((VTX6_USR1&0x300)>>8);
    sndbuf[0]=REG_RD_WRD;
    devdo(dv,10,cmd,8,sndbuf,rcvbuf,0);
    cmd[0]=(VTX6_USR2&0xff);
    cmd[1]=((VTX6_USR2&0x300)>>8);
    sndbuf[0]=0xff;
    sndbuf[1]=0xff;
    devdo(dv,10,cmd,16,sndbuf,rcvbuf,1);
    wrd[0]=rcvbuf[0];
    wrd[1]=rcvbuf[1];
    cmd[0]=(VTX6_BYPASS&0xff);
    cmd[1]=((VTX6_BYPASS&0x300)>>8);
    devdo(dv,10,cmd,0,sndbuf,rcvbuf,2);
}

} // namespace emu::pc
} // namespace emu
