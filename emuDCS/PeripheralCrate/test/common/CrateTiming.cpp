//-----------------------------------------------------------------------
// $Id: CrateTiming.cpp,v 1.2 2006/02/01 20:43:10 mey Exp $
// $Log: CrateTiming.cpp,v $
// Revision 1.2  2006/02/01 20:43:10  mey
// Got rid of TestBeamController
//
// Revision 1.1  2006/02/01 19:24:06  mey
// Used to be tmbtiming
//
// Revision 2.53  2006/02/01 18:31:50  mey
// Update
//
// Revision 2.52  2006/01/25 19:49:16  mey
// Update
//
// Revision 2.51  2006/01/25 19:41:46  mey
// UPdate
//
// Revision 2.50  2006/01/25 19:24:55  mey
// Update
//
// Revision 2.49  2006/01/23 10:19:19  mey
// Porting to ChamberUtilities
//
// Revision 2.48  2006/01/18 19:38:17  mey
// Fixed bugs
//
// Revision 2.47  2006/01/18 12:46:04  mey
// Cleaned up old code
//
// Revision 2.46  2006/01/17 18:56:53  mey
// Update
//
// Revision 2.45  2006/01/12 22:36:41  mey
// UPdate
//
// Revision 2.44  2006/01/12 11:32:59  mey
// Update
//
// Revision 2.43  2006/01/11 17:00:29  mey
// Update
//
// Revision 2.42  2006/01/10 23:32:52  mey
// Update
//
// Revision 2.41  2006/01/10 19:47:07  mey
// UPdate
//
// Revision 2.40  2006/01/09 07:17:43  mey
// Update
//
// Revision 2.39  2005/12/20 14:20:15  mey
// Update
//
// Revision 2.38  2005/12/16 12:59:54  mey
// Update
//
// Revision 2.37  2005/12/15 14:34:12  mey
// Update
//
// Revision 2.36  2005/12/10 04:47:44  mey
// Fix bug
//
// Revision 2.35  2005/12/05 18:11:26  mey
// UPdate
//
// Revision 2.34  2005/11/28 14:12:47  mey
// Update
//
// Revision 2.33  2005/11/25 23:43:24  mey
// Update
//
// Revision 2.32  2005/11/22 15:15:36  mey
// Update
//
// Revision 2.31  2005/11/17 13:47:22  mey
// Update
//
// Revision 2.30  2005/11/16 17:02:06  mey
// Update
//
// Revision 2.29  2005/11/15 15:22:57  mey
// Update
//
// Revision 2.28  2005/11/11 13:37:20  mey
// Update
//
// Revision 2.27  2005/11/10 18:25:53  mey
// Update
//
// Revision 2.26  2005/11/09 20:07:34  mey
// Update
//
// Revision 2.25  2005/11/07 16:24:02  mey
// Update
//
// Revision 2.24  2005/11/07 10:12:26  mey
// Byte swap
//
// Revision 2.23  2005/11/04 10:26:33  mey
// Update
//
// Revision 2.22  2005/11/03 18:25:04  mey
// I2C routines
//
// Revision 2.21  2005/09/28 18:44:20  mey
// Update
//
// Revision 2.20  2005/09/28 16:54:27  mey
// Update
//
// Revision 2.19  2005/09/22 13:01:39  mey
// Update
//
// Revision 2.18  2005/09/15 08:13:19  mey
// Update
//
// Revision 2.17  2005/09/13 14:46:40  mey
// Get DMB crate id; and DCS
//
// Revision 2.16  2005/09/07 16:18:17  mey
// DMB timing routines
//
// Revision 2.15  2005/09/07 13:55:16  mey
// Included new timing routines from Jianhui
//
// Revision 2.14  2005/08/31 15:12:59  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.13  2005/08/23 19:27:18  mey
// Update MPC injector
//
// Revision 2.12  2005/08/23 15:49:55  mey
// Update MPC injector for random LCT patterns
//
// Revision 2.11  2005/08/22 16:58:33  mey
// Fixed bug in TMB-MPC injector
//
// Revision 2.10  2005/08/22 16:38:28  mey
// Added TMB-MPC injector
//
// Revision 2.9  2005/08/22 07:55:46  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.8  2005/08/17 12:27:23  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.7  2005/08/15 15:38:15  mey
// Include alct_hotchannel_file
//
// Revision 2.6  2005/08/15 11:00:10  mey
// Added pulsing to MPC Winner timing and several options for DAV DMB settings
//
// Revision 2.5  2005/08/12 19:45:27  mey
// Updated MPC printout in TMB-MPC timing
//
// Revision 2.4  2005/08/12 14:16:04  mey
// Added pulsing vor TMB-MPC delay
//
// Revision 2.2  2005/08/10 12:54:36  geurts
// Martin's updates
//
// Revision 2.1  2005/06/06 15:17:19  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:07  geurts
//  updated by M von der Mey sometimes
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
//! TMB ALCT/CFEB timing utility
/*! \brief TMB-ALCT/CFEB timing
 * \author Frank Geurts
 * \date January 2004
 * 
 * tmbtiming sets up the PeripheralCrate according to the
 * configuration in the XML file. It will pass on the timing
 * parameter to the TMB routines and runs the PHOS4 timing
 * routines on the first TMB defined in the XML-file.
 * For reference the results are written in two files
 * (tmb_alct.dat and tmb_scan.dat).
 *
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include "PeripheralCrateParser.h"
#include "EmuController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "RAT.h"
#include "MPC.h"
#include "DDU.h"
#include "ALCTController.h"
#include "CrateSelector.h"
#include "JTAG_constants.h"
#include "ChamberUtilities.h"
//
//#include "EmuDcs.h"

using namespace std;

int beginning = 0;

TMB *thisTMB ;
DAQMB* thisDMB ;
CCB* thisCCB ;
ALCTController *alct ;
MPC * thisMPC;
//EmuDcs *mEmuDcs;
//
void CFEBTiming(float CFEBMean[5]);
void PulseCFEB(fstream* output_log=0, int HalfStrip = -1, int CLCTInputs = 0x1f );
void CFEBChamberScanning();
void PulseTestStrips();
void ALCTTiming(int &,int &);
void ALCTScanDelays();
void ALCTChamberScanning();
void ALCTSVFLoad();
int  TMBL1aTiming();
int  FindBestL1aAlct();
void PulseRandomALCT();
int  FindWinner(int);
int  FindALCTvpf();
int  FindTMB_L1A_delay(int,int);
int  FindALCT_L1A_delay(int,int);
void Automatic();
void InitStartSystem();
int  AdjustL1aLctDMB();
void InjectMPCData() ;
void ALCT_phase_analysis(int rxtxtiming[13][13]);
//
bool UseCosmic(false);
bool UsePulsing(true);
//
int BestCCBDelaySetting = 98 ;
float CFEBMean[5];
int ALCTRXphase = 0 ;
int ALCTTXphase = 0 ;
int TMB_L1a_timing = 0 ;
int Find_ALCT_L1a_delay = 0;
int ALCT_L1a_delay = 0;
int ALCTvpf = 0;
//
//TestBeamCrateController tbController;
  EmuController emuController;
//
int main(int argc,char **argv){

  //-- default settings
  char *xmlFile ="tmbTiming.xml";
  int scantime(1);
  bool useScint(false);
  bool doPHOS4cfeb(false);
  bool doPHOS4alct(false);
  bool doTMBScope(false);
  bool doALCTRawhits(false);
  bool doTMBRawhits(false);
  bool doPulseTest(false);
  bool doInteractive(false);
  bool doScopeRead(false);
  bool doScopeReadArm(false);
  bool doForceScopeTrigger(false);
  bool doCFEBTiming(false);
  bool doCFEBChamberScanning(false);
  bool doPrintCounters(false);
  bool doResetCounters(false);
  bool doPulseTestStrips(false);
  bool doALCTTiming(false);
  bool doALCTChamberScanning(false);
  bool doALCTSVFLoad(false);
  bool doReadIDCODE(false);
  bool doLv1TMBTiming(false);
  bool doFindBestL1aALCT(false);
  bool doReadALCTThreshold(false);
  bool doDumpFifo(false);
  bool doStartTTC(false);
  bool doCCBFPGA(false);
  bool doCCBDLOG(false);
  bool doReadTMBIDRegister(false);
  bool doReadTMBRegister(false);
  bool doWriteTMBRegister(false);
  bool doTMBRegisterDump(false);
  bool doReadCCBRegister(false);
  bool doWriteCCBRegister(false);
  bool doReadDMBRegister(false);
  bool doWriteDMBRegister(false);
  bool doReadDMBCounters(false);
  bool doReadDMB_DAV(false);
  bool doWriteDMB_DAV(false);
  bool doFindWinner(false);
  bool doFindALCTvpf(false);
  bool doFindTMB_L1A_delay(false);
  bool doFindALCT_L1A_delay(false);
  bool doSetDMBtrgsrc(false);
  bool doInitSystem(false);
  bool doL1aRequest(false);
  bool doCCBstartTrigger(false);
  bool doAutomatic(false);
  bool doAdjustL1aLctDMB(false);
  bool doInjectMPCData(false);
  bool doEnableCLCTInputs(false);
  bool doALCTScanDelays(false);
  bool doSetCableDelay(false);
  bool doWriteSFM(false);
  bool doReadTTCrxID(false);
  bool doReadTTCrxRegs(false);
  bool dodaqmb_promfpga_dump(false);
  bool dodaqmb_adc_dump(false);
  bool dodaqmb_lowv_dump(false);
  bool doccb_firmware_version(false);
  bool doWriteMPCRegister(false);
  bool doReadMPCRegister(false);
  bool doLoadDMBFirmware(false);
  bool doLoadALCTFirmware(false);
  bool doLoadTMBFirmware(false);
  bool doLoadCFEBFirmware(false);
  bool doCrazyTMB(false);
  bool doDecodeALCT(false);
  bool doChambers(false);
  bool doReadRATUser1(false);
  //
  //-- read commandline arguments and xml configuration file
  //
  if (argc>1)
    for (int i=1;i<argc;i++){
      // help: print usage and exit
      if (!strcmp(argv[i],"-h")){
	cout << "Usage: " << argv[0] 
	     << " [-alct] [-cfeb] [-scope] [-ALCTRawhits] [-TMBRawhits] [-PulseTest] [-Interactive] [-CFEBTiming] [-CFEBChamberScanning] [-ALCTTiming][-t (time=" 
	     << scantime << ")] [-f (file="<< xmlFile <<")]" 
	     << "[-scint]"<<endl;
	exit(0);
      }
      if (!strcmp(argv[i],"-t")) scantime=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
      if (!strcmp(argv[i],"-scint")) useScint = true;
      if (!strcmp(argv[i],"-alct"))  doPHOS4alct = true;
      if (!strcmp(argv[i],"-cfeb"))  doPHOS4cfeb = true;
      if (!strcmp(argv[i],"-scope")) doTMBScope = true;
      if (!strcmp(argv[i],"-ALCTRawhits")) doALCTRawhits = true;
      if (!strcmp(argv[i],"-TMBRawhits")) doTMBRawhits = true;
      if (!strcmp(argv[i],"-PulseTest")) doPulseTest = true;
      if (!strcmp(argv[i],"-Interactive")) doInteractive = true;
      if (!strcmp(argv[i],"-CFEBTiming")) doCFEBTiming = true;
      if (!strcmp(argv[i],"-CFEBChamberScanning")) doCFEBChamberScanning = true;
      if (!strcmp(argv[i],"-ALCTTiming")) doALCTTiming = true;
      if (!strcmp(argv[i],"-Automatic"))  doAutomatic = true;
      if (!strcmp(argv[i],"-MPCWinnerScan"))  doFindWinner = true;
    }
  //
  //-- Configure and initialize VME modules
  //
  cout << "Running " << argv[0] << ". time="<< scantime
       <<"s. Config="<<xmlFile;
  if (useScint)
    cout << " scintillator trigger" << endl;
  else
    cout << " Anode trigger" << endl;

  emuController.SetConfFile(xmlFile);
  emuController.init();

  //-- parse XML file
  cout << "---- XML parser ----" << endl;
  //PeripheralCrateParser parser;
  //parser.parseFile(xmlFile);
    
  //-- Set-up and configure Testbeam 
  cout << " ---- Cosmic Particle Controller ----" << endl;


  //-- Make sure that only one TMB in one crate is configured
  CrateSelector selector = emuController.selector();
  vector<Crate*> crateVector = selector.crates();
  if (crateVector.size() > 1){
    cerr << "Error: only one PeripheralCrate allowed" << endl;
    exit(1);
  }
  vector<TMB*> tmbVector = selector.tmbs(crateVector[0]);
  if (tmbVector.size() > 1){
    cerr << "Error: only one TMB in xml file (" << xmlFile 
	 << ") allowed" << endl;
    exit(1);
  }
  vector<DAQMB*> dmbVector = selector.daqmbs(crateVector[0]);
  if (dmbVector.size() > 1){
    cerr << "Error: only one DMB in xml file ("<< xmlFile
	 << ")allowed." << endl;
    exit(1);
  }

  //-- get pointers to CCB, TMB and DMB
  Crate *thisCrate = crateVector[0];
  thisCCB = thisCrate->ccb();
  thisTMB = tmbVector[0];
  thisDMB = dmbVector[0];
  thisMPC = thisCrate->mpc();
  DDU * thisDDU = thisCrate->ddu();
  //
  ChamberUtilities util;
  util.SetCCB(thisCCB);
  util.SetTMB(thisTMB);
  util.SetDMB(thisDMB);
  util.SetMPC(thisMPC);
  //
  //-- Configure the Cosmic Set-up
  //
  cout << "---- Cosmic ParticConfiguration ----"<<endl;
  if (useScint){
    // use default test beam configurator
    //tbController.configure();
  }
  else{ // set-up the anode trigger
    // do the test beam configuration manually
    //  based on TestBeamController::configure()
    
    //cout << "-- Configuring CCB --" << endl;
    //thisCCB->configure();
    //sleep(1);



    //cout << "-- Configuring DMB --" << endl;
    //thisDMB->restoreCFEBIdle(); // keep
    //thisDMB->restoreMotherboardIdle(); //keep
    //thisDMB->configure(); //keep
    
    //cout << "-- Configuring TMB --" << endl;
    //thisTMB->configure(); //keep
    
    //cout << "-- Configuring DMB (cont'd)" << endl;
    //thisDMB->calctrl_fifomrst(); //
    //sleep(1);

    cout << "-- Configuring ALCT --" << endl;
    alct = thisTMB->alctController();
    //util.SetALCT(alct);
    //
    //if (alct) alct->GetWGNumber();

    //if (alct) alct->setup(1);

    //cout << "-- Configuring CCB (cont'd)" << endl;
    //thisCCB->l1CathodeScint();

    //cout << "-- Configuring MPC --" << endl;
    //if(thisMPC) thisMPC->init();
    //sleep(1);

    //cout << "-- Resetting DDU registers (via DCNTRL) " << endl;
    //if(thisDDU) thisDDU->dcntrl_reset();
    //sleep(2);
  }

  
  cout << "---- Test Beam Enable ----"<<endl;


  //tbController.DcsSetup();
  //mEmuDcs = new EmuDcs ;

  if (useScint){
    //tbController.enable();
  } else {
    // do the test beam configuration manually (assuming CCB2001)
    //cout << "-- Enable CCB -- " << endl;
    //thisCCB->enable();
    //cout << "-- Re-init MPC" << endl;
    //if (thisMPC) thisMPC->init();
    //thisCCB->enableCLCT();

    //thisTMB->ResetCounters();

    interactive:

    if (doInteractive) {
       printf("%c7", '\033'); 
       printf("%c8", '\033'); 
       printf("%c[01;35m", '\033');
       //
       cout << "  0:Init System " << endl;
       cout << "  1:TMBscope              2:ALCTrawhits            3:TMBrawhits        " << endl;      
       cout << "  4:PulseTest             5:ScopeReadArm           6:ScopeRead         " << endl;
       cout << "  7:ForceScopeTrigger     8:CFEBtiming             9:CFEBchamberScan   " << endl;
       cout << " 10:PrintCounters        11:ResetCounters         12:PulseRandomALCT   " << endl;
       cout << " 13:ALCTchamberScan      14:ALCT_SVF_Load         15:ReadIDcode        " << endl;
       cout << " 16:L1A_TMBtiming        17:FindBestL1A_ALCT      18:ReadALCTthresh    " << endl;
       cout << " 19:ALCTtiming           20:DumpFifo              21:TMBStartTrigger   " << endl;
       cout << " 22:CCB_FPGA_mode        23:CCB_DLOG_mode         24:ReadTMB_IDregister" << endl;
       cout << " 25:ReadTMBregister      26:WriteTMBregister      27:TMB_CCB_RegDump   " << endl;
       cout << " 28:ReadCCBregister      29:WriteCCBregister      30:ReadDMBregister   " << endl;
       cout << " 31:WriteDMBregister     32:ReadDMB_DelCounters   33:ReadDMB_DAVdelays " << endl;
       cout << " 34:WriteDMB_DAVdelays   35:MPCwinnerScan         36:ALCTvpfScan       " << endl;
       cout << " 37:TMB_L1Adelayscan     38:SetDMBtrgsrc          39:ALCT_L1Adelayscan " << endl;
       cout << " 40:EnableL1aRequest     41:CCBstartTrigger       42:AdjustL1aLctDMB   " << endl;
       cout << " 43:InjectMPCData        44:EnableCLCTInputs      45:ALCTScanDelays    " << endl;
       cout << " 46:SetCableDelay        47:WriteDMBSFM           48:ReadTTCrxID       " << endl ;
       cout << " 49:daqmb_promfpga_dump  50:daqmb_adc_dump        51:damb_lowv_dump    " << endl;
       cout << " 52:ccb_firmware_version 53:WriteMPCRegister      54:ReadMPCRegister   " << std::endl;
       cout << " 55:LoadDMBFirmware      56:LoadALCTfirmware      57:LoadTMBFirmware   " << std::endl;
       cout << " 58:LoadCFEBFirmware     59:doCrazyCrate          60:DumpTTCrxRegs     " << std::endl;
       cout << " 61:DecodeALCT           62:Chambers              63:ReadRATUser1      " <<std::endl;
       //
       printf("%c[01;36m", '\033');
       cout << "What do you want to do today ?"<< endl;
       printf("%c8", '\033'); 
       printf("%c[0m", '\033');
       int Menu;
       cin >> Menu;
       //
       doInitSystem          = false;
       doL1aRequest          = false;
       doTMBScope            = false;
       doALCTRawhits         = false;
       doALCTSVFLoad         = false;
       doTMBRawhits          = false;
       doPulseTest           = false;
       doScopeRead           = false;
       doScopeReadArm        = false;
       doForceScopeTrigger   = false;
       doCFEBTiming          = false;
       doCFEBChamberScanning = false;
       doPrintCounters       = false;
       doResetCounters       = false;
       doPulseTestStrips     = false;
       doALCTChamberScanning = false;
       doReadIDCODE          = false;
       doFindBestL1aALCT     = false;
       doLv1TMBTiming        = false;
       doReadALCTThreshold   = false;
       doALCTTiming          = false;
       doDumpFifo            = false;
       doStartTTC            = false;
       doCCBFPGA             = false;
       doCCBDLOG             = false;
       doReadTMBIDRegister   = false;       
       doReadTMBRegister     = false;
       doWriteTMBRegister    = false;
       doTMBRegisterDump     = false;
       doReadCCBRegister     = false;    
       doWriteCCBRegister    = false;
       doReadDMBRegister     = false;
       doWriteDMBRegister    = false;
       doReadDMBCounters     = false;
       doReadDMB_DAV         = false;
       doWriteDMB_DAV        = false;
       doFindWinner          = false;
       doFindALCTvpf         = false;
       doFindTMB_L1A_delay   = false;
       doSetDMBtrgsrc        = false;
       doFindALCT_L1A_delay  = false;
       doCCBstartTrigger     = false;
       doAdjustL1aLctDMB     = false;
       doInjectMPCData       = false;
       doEnableCLCTInputs    = false;
       doALCTScanDelays      = false;
       doSetCableDelay       = false;
       doWriteSFM            = false;
       doReadTTCrxID         = false;
       doReadTTCrxRegs       = false;
       dodaqmb_promfpga_dump = false;
       dodaqmb_adc_dump      = false;
       dodaqmb_lowv_dump     = false;
       doccb_firmware_version     = false;
       doWriteMPCRegister    = false;
       doReadMPCRegister     = false;
       doLoadDMBFirmware     = false;
       doLoadALCTFirmware    = false;
       doLoadTMBFirmware     = false;
       doLoadCFEBFirmware    = false;
       doDecodeALCT          = false;
       doChambers            = false;
       //
       if ( Menu == -1 ) goto outhere;
       if ( Menu == 0 ) doInitSystem           = true ;
       if ( Menu == 1 ) doTMBScope             = true ;
       if ( Menu == 2 ) doALCTRawhits          = true ;
       if ( Menu == 3 ) doTMBRawhits           = true ;
       if ( Menu == 4 ) doPulseTest            = true ;
       if ( Menu == 5 ) doScopeReadArm         = true ;
       if ( Menu == 6 ) doScopeRead            = true ;
       if ( Menu == 7 ) doForceScopeTrigger    = true ;
       if ( Menu == 8 ) doCFEBTiming           = true ;
       if ( Menu == 9 ) doCFEBChamberScanning  = true ;
       if ( Menu == 10) doPrintCounters        = true ;
       if ( Menu == 11) doResetCounters        = true ;
       if ( Menu == 12) doPulseTestStrips      = true ;
       if ( Menu == 13) doALCTChamberScanning  = true ;
       if ( Menu == 14) doALCTSVFLoad          = true ;
       if ( Menu == 15) doReadIDCODE           = true ;
       if ( Menu == 16) doLv1TMBTiming         = true ;
       if ( Menu == 17) doFindBestL1aALCT      = true ;
       if ( Menu == 18) doReadALCTThreshold    = true ;
       if ( Menu == 19) doALCTTiming           = true ;
       if ( Menu == 20) doDumpFifo             = true ; 
       if ( Menu == 21) doStartTTC             = true ; 
       if ( Menu == 22) doCCBFPGA              = true ; 
       if ( Menu == 23) doCCBDLOG              = true ; 
       if ( Menu == 24) doReadTMBIDRegister    = true ; 
       if ( Menu == 25) doReadTMBRegister      = true ; 
       if ( Menu == 26) doWriteTMBRegister     = true ; 
       if ( Menu == 27) doTMBRegisterDump      = true ; 
       if ( Menu == 28) doReadCCBRegister      = true ; 
       if ( Menu == 29) doWriteCCBRegister     = true ; 
       if ( Menu == 30) doReadDMBRegister      = true ; 
       if ( Menu == 31) doWriteDMBRegister     = true ; 
       if ( Menu == 32) doReadDMBCounters      = true ; 
       if ( Menu == 33) doReadDMB_DAV          = true ; 
       if ( Menu == 34) doWriteDMB_DAV         = true ; 
       if ( Menu == 35) doFindWinner           = true ; 
       if ( Menu == 36) doFindALCTvpf          = true ; 
       if ( Menu == 37) doFindTMB_L1A_delay    = true ;
       if ( Menu == 38) doSetDMBtrgsrc         = true ;
       if ( Menu == 39) doFindALCT_L1A_delay   = true ;
       if ( Menu == 40) doL1aRequest           = true ;
       if ( Menu == 41) doCCBstartTrigger      = true ;
       if ( Menu == 42) doAdjustL1aLctDMB      = true ;
       if ( Menu == 43) doInjectMPCData        = true ;
       if ( Menu == 44) doEnableCLCTInputs     = true ;
       if ( Menu == 45) doALCTScanDelays       = true ;
       if ( Menu == 46) doSetCableDelay        = true ;
       if ( Menu == 47) doWriteSFM             = true ;
       if ( Menu == 48) doReadTTCrxID          = true ;
       if ( Menu == 49) dodaqmb_promfpga_dump  = true ;
       if ( Menu == 50) dodaqmb_adc_dump       = true ;
       if ( Menu == 51) dodaqmb_lowv_dump      = true ;
       if ( Menu == 52) doccb_firmware_version = true ;
       if ( Menu == 53) doWriteMPCRegister     = true ;
       if ( Menu == 54) doReadMPCRegister      = true ;
       if ( Menu == 55) doLoadDMBFirmware      = true ;
       if ( Menu == 56) doLoadALCTFirmware     = true ;
       if ( Menu == 57) doLoadTMBFirmware      = true ;
       if ( Menu == 58) doLoadCFEBFirmware     = true ;
       if ( Menu == 59) doCrazyTMB             = true ;
       if ( Menu == 60) doReadTTCrxRegs        = true ;
       if ( Menu == 61) doDecodeALCT           = true ;
       if ( Menu == 62) doChambers             = true ;
       if ( Menu == 63) doReadRATUser1         = true ;
       if ( Menu  > 63 | Menu < -2) 
	 cout << "Invalid menu choice, try again." << endl << endl;
       //
    }
    //
    if ( doReadRATUser1 ) {
      //
      RAT myRat(thisTMB);
      myRat.ReadRatUser1();
      //
    }
    //
    if ( doChambers) {
      //
      std::cout << crateVector[0]->chamberUtils().size() << std::endl;
      //
    }
    //
    if(doLoadALCTFirmware) {
      //
      if (! alct ) {
	printf("No ALCT\n") ; 
      }
      //
      ALCTIDRegister sc_id, chipID ;
      //
      printf("Reading IDs...") ;
      //
      alct->alct_read_slowcontrol_id(&sc_id) ;
      std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
      alct->alct_fast_read_id(chipID);
      std::cout << " ALCT Fastcontrol ID " << chipID << std::endl;
      //
      thisTMB->disableAllClocks();
      int debugMode(0);
      int jch(3);
      //int status = alct->SVFLoad(&jch,"alct672.svf",debugMode);
      int status = alct->SVFLoad(&jch,"alctcrc384mirror.svf",debugMode);
      thisTMB->enableAllClocks();
      //
      if (status >= 0){
	cout << "=== Programming finished"<< endl;
	cout << "=== " << status << " Verify Errors  occured" << endl;
      }
      else{
	cout << "=== Fatal Error. Exiting with " <<  status << endl;
      }
      //
    }
    //
    if(doCrazyTMB){
      //
      std::cout << "Looping " << std::endl;
      //
      unsigned short int input;
      cin >> input ;
      thisTMB->tmb_set_boot_reg(input);
      thisTMB->tmb_get_boot_reg(&input);
      //
      //for(int i=0; i<100000; i++) {
	//
	//thisTMB->WriteRegister(0x16,0x1234);
	//
	//thisDMB->setcrateid(10);
	//
	//thisCCB->WriteRegister(0x0,0x1234);
	//
      //}
      //
    }
    //
    if(doLoadTMBFirmware) {
      //
      //thisTMB->disableAllClocks();
      int debugMode(0);
      int jch(5);
      string chamberType("ME21");
      ALCTController *alct = new ALCTController(thisTMB,chamberType);
      int status = alct->SVFLoad(&jch,"tmb2005e.svf",debugMode);
      //thisTMB->enableAllClocks();
      //
      if (status >= 0){
	cout << "=== Programming finished"<< endl;
	cout << "=== " << status << " Verify Errors  occured" << endl;
      }
      else{
	cout << "=== Fatal Error. Exiting with " <<  status << endl;
      }
      //
    }
    //
    if(doLoadCFEBFirmware) {
      //
      cout << "Load CFEB firmware (0-4) ?" << endl;
      //
      char out[10];
      thisCCB->firmwareVersion();
      vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //
      int i=0;
      std::cout << "Which CFEB to program (0-4) ?" << std::endl ;
      cin >> i;
      std::cout << "Programming..." <<endl;
      //for (int i=0; i<thisCFEBs.size(); i++) {
	thisDMB->febpromuser(thisCFEBs[i]);
	thisDMB->epromload(thisCFEBs[i].promDevice(),"cfeb_v4_r2.svf",1,out);  // load mprom
	//}
      //thisCCB->firmwareVersion();
    }
    //
    if(doLoadDMBFirmware) {
      char out[10];
      thisCCB->firmwareVersion();
      thisDMB->epromload(MPROM,"dmb6cntl_v17_r2.svf",1,out);  // load mprom
      //
      //thisCCB->firmwareVersion();
      //
    }
    //
    if(doccb_firmware_version){
      thisCCB->firmwareVersion();
    }
    //
    if(dodaqmb_lowv_dump) {
      thisDMB->lowv_dump();
    }		       
    //
    if(dodaqmb_promfpga_dump) {
      thisDMB->daqmb_promfpga_dump();
    }		       
    //
    if(dodaqmb_adc_dump) {
      thisDMB->daqmb_adc_dump();
    }		       
    //
    if(doReadTTCrxID) {
      thisCCB->ReadTTCrxID();
    }
    //
    if(doDecodeALCT) {
      //
      thisTMB->DecodeALCT();
      std::cout << "ALCT WordCount  "  << thisTMB->GetALCTWordCount() <<std::endl;
      //
    }
    //
    if(doReadTTCrxRegs) {
      //
      std::cout << "Register 0 " ;
      std::cout << thisCCB->ReadTTCrxReg(0);
      std::cout << std::endl;
      //
      std::cout << "Register 1 " ;
      std::cout << thisCCB->ReadTTCrxReg(1);
      std::cout << std::endl;
      //
      std::cout << "Register 2 " ;
      std::cout << thisCCB->ReadTTCrxReg(2);
      std::cout << std::endl;
      //
      std::cout << "Register 3 " ;
      std::cout << thisCCB->ReadTTCrxReg(3);
      std::cout << std::endl;
      //
      std::cout << "Register 8 " ;
      std::cout << thisCCB->ReadTTCrxReg(8);
      std::cout << std::endl;
      //
      std::cout << "Register 9 " ;
      std::cout << thisCCB->ReadTTCrxReg(9);
      std::cout << std::endl;
      //
      std::cout << "Register 10 " ;
      std::cout << thisCCB->ReadTTCrxReg(10);
      std::cout << std::endl;
      //
      std::cout << "Register 11 " ;
      std::cout << thisCCB->ReadTTCrxReg(11);
      std::cout << std::endl;
      //
      std::cout << "Register 19 " ;
      std::cout << thisCCB->ReadTTCrxReg(19);
      std::cout << std::endl;
      //
      std::cout << "Register 20 " ;
      std::cout << thisCCB->ReadTTCrxReg(20);
      std::cout << std::endl;
      //
      std::cout << "Register 21 " ;
      std::cout << thisCCB->ReadTTCrxReg(21);
      std::cout << std::endl;
      //
      std::cout << "Register 22 " ;
      std::cout << thisCCB->ReadTTCrxReg(22);
      std::cout << std::endl;
      //
      std::cout << "Register 24 " ;
      std::cout << thisCCB->ReadTTCrxReg(24);
      std::cout << std::endl;
      //
      std::cout << "Register 25 " ;
      std::cout << thisCCB->ReadTTCrxReg(25);
      std::cout << std::endl;
      //
      std::cout << "Register 26 " ;
      std::cout << thisCCB->ReadTTCrxReg(26);
      std::cout << std::endl;
      //
      std::cout << "Register 27 " ;
      std::cout << thisCCB->ReadTTCrxReg(27);
      std::cout << std::endl;
      //
      std::cout << "Register 28 " ;
      std::cout << thisCCB->ReadTTCrxReg(28);
      std::cout << std::endl;
      //
    }

    if(doWriteSFM){
      //tbController.DcsDisable();
      thisDMB->WriteSFM();
      //tbController.DcsEnable();
    }

    if(doSetCableDelay){
      cout << "Cable delay to set : " ;
      int delay;
      cin >> delay ;
      //tbController.DcsDisable();
      thisDMB->setcbldly(delay) ;
      //tbController.DcsEnable();
    }

    if(doALCTScanDelays){
      //tbController.DcsDisable();
      ALCTScanDelays();
      //tbController.DcsEnable();
    }

    if(doEnableCLCTInputs){
      int Inputs;
      cout << " Input which CLCT Inputs to enable " << endl ;
      cin >> Inputs;
      //tbController.DcsDisable();
      thisTMB->DisableCLCTInputs();
      thisTMB->EnableCLCTInputs(Inputs);
      //tbController.DcsEnable();
    }

    if(doInjectMPCData){
      util.InjectMPCData();
    }

    if(doAdjustL1aLctDMB){
      //tbController.DcsDisable();
      AdjustL1aLctDMB();
      //tbController.DcsEnable();
    }

    if (doAutomatic){
      //tbController.DcsDisable();
      Automatic();
      //tbController.DcsEnable();
    }

    if (doCCBstartTrigger) {
      util.CCBStartTrigger();
    }

    if (doL1aRequest) {
      //tbController.DcsDisable();
      thisTMB->EnableL1aRequest();
      thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
      //tbController.DcsEnable();
    }

    if (doFindALCT_L1A_delay) {
      //
      cout << "Enter lowest ALCT L1A delay value to loop over (bunch crossings, decimal)" << endl;
      int minlimit;
      cin >> minlimit;
      //
      cout << "Enter highest ALCT L1A delay value to loop over (bunch crossings, decimal)" << endl;
      int maxlimit;
      cin >> maxlimit;
      //
      int best_alct_l1a = FindALCT_L1A_delay(minlimit,maxlimit);
      printf("Best Alct L1a %d \n",best_alct_l1a);
      //
    }

    if (doInitSystem) {
      util.InitSystem();
    }

    if (doFindWinner) {
      cout << "How may pulses ?" << endl ;
      int npulses;
      cin >> npulses;
      //tbController.DcsDisable();
      FindWinner(npulses); 
      //tbController.DcsEnable();
    }


    if (doFindALCTvpf) {
      FindALCTvpf(); 
    }

    if (doFindTMB_L1A_delay) {
      //
      cout << "Enter lowest TMB L1A delay value to loop over (bunch crossings, decimal)" << endl;
      int idelay_min;
      cin >> idelay_min;
      //
      cout << "Enter highest TMB L1A delay value to loop over (bunch crossings, decimal)" << endl;
      int idelay_max;
      cin >> idelay_max;
      //
      //tbController.DcsDisable();
      int L1adelay = FindTMB_L1A_delay( idelay_min, idelay_max );
      //tbController.DcsEnable();
      //
      printf(" L1a delay = %d \n",L1adelay);
      //
    }
    
    //allows you to set the DMB trigger source value;default is 3 which is
    //internal L1A and LCT; 0 turns these off

    if (doSetDMBtrgsrc) {
      //
      int dword;
      cout << "Input DMB trigger source value in hex (0 turns off internal L1A and LCT)" << endl;
      cin >> hex >> dword >> dec;
      //
      //tbController.DcsDisable();
      thisDMB->settrgsrc(dword);
      //tbController.DcsEnable();
      //
      cout << "Have now set DMB trigger source value to: " << hex << dword << dec << endl;
      //
    } 
    
    
    if (doReadTMBIDRegister) {
      //
      cout << endl;
      printf("TMB ID Reg: addr = 0x2, month day (MMDD) =  %x \n",thisTMB->ReadRegister(0x02));
      cout << endl;
      printf("TMB ID Reg: addr = 0x4, year =  %x \n",thisTMB->ReadRegister(0x04));
      cout << endl;
      printf("TMB ID Reg: addr = 0x2e, CCB status =  %x \n",thisTMB->ReadRegister(0x2e));
      cout << endl;
      printf("TMB ID Reg: addr = 0x38, ALCT seq Control status =  %x \n",thisTMB->ReadRegister(0x38));
      //
      //tbController.DcsDisable();
      thisTMB->WriteRegister(0x9c,0x1);
      //tbController.DcsEnable();
      //
      cout << endl;
      printf("TMB ID Reg: addr = 0x9c, CCB TTC command gen =  %x \n",thisTMB->ReadRegister(0x9c));
      cout << endl;
      printf("TMB ID Reg: addr = 0x10, UserJTAG =  %x \n",thisTMB->ReadRegister(0x10));
      cout << endl;
      //
    }


    // Read arbitrary TMB register: caution, if register doesn't exist, can cause problem(?)
    // JH and DM 26-jun-05
    // use hex for everything (duh)

    if (doReadTMBRegister) {    
      //
      cout << "Enter TMB register address (hex)" << endl;
      int TMBRegAddr;
      cin >> hex >> TMBRegAddr >> dec;            // Make sure to return to decimal mode
      //
      //tbController.DcsDisable();
      int TMBRegValue  = thisTMB->ReadRegister(TMBRegAddr) ;
      //tbController.DcsEnable();
      //
      printf("TMB register Address= %x     value=0x %x \n",TMBRegAddr,TMBRegValue);
      cout << endl  << endl;
      //
    }


    if (doReadMPCRegister) {    
      //
      cout << "Enter MPC register address (hex)" << endl;
      int MPCRegAddr;
      cin >> hex >> MPCRegAddr >> dec;            // Make sure to return to decimal mode
      //
      int MPCRegValue  = thisMPC->ReadRegister(MPCRegAddr) ;
      //
      printf("MPC register Address= %x     value=0x %x \n",MPCRegAddr,MPCRegValue);
      cout << endl  << endl;
      //
    }


    if (doWriteMPCRegister) {   
      //
      cout << "Caution: can cause serious problems, proceed with caution" << endl;
      cout << "Enter MPC register address (hex)" << endl;
      //
      int MPCRegAddr;
      cin >> hex >> MPCRegAddr >> dec;  // Make sure to return to decimal mode after reading
      //
      int MPCRegValue  = thisMPC->ReadRegister(MPCRegAddr) ;
      printf("MPC register Address= %x     read initial value=0x %x \n",MPCRegAddr,MPCRegValue);
      //
      cout << "Enter data value to write (hex, <=4 characters, i.e. 16 bits)" << endl;
      int MPCRegWriteData;
      cin >> hex >> MPCRegWriteData >> dec;  // Make sure to return to decimal mode after reading
      //
      cout << "Echo desired write data=" << hex << MPCRegWriteData << dec << endl;
      //
      thisMPC->WriteRegister(MPCRegAddr,MPCRegWriteData);
      //
      MPCRegValue  = thisMPC->ReadRegister(MPCRegAddr) ;
      printf("MPC register Address= %x     read back written value=0x %x \n",MPCRegAddr,MPCRegValue);
      //
      cout << endl  << endl;
      //
    }


    // Write arbitrary TMB register: caution, can of course cause all kinds of problems
    // JH and DM 26-jun-05
    // use hex for everything (duh)

    if (doWriteTMBRegister) {   
      //
      //tbController.DcsDisable();
      cout << "Caution: can cause serious problems, proceed with caution" << endl;
      cout << "Enter TMB register address (hex)" << endl;
      //
      int TMBRegAddr;
      cin >> hex >> TMBRegAddr >> dec;  // Make sure to return to decimal mode after reading
      //
      int TMBRegValue  = thisTMB->ReadRegister(TMBRegAddr) ;
      printf("TMB register Address= %x     read initial value=0x %x \n",TMBRegAddr,TMBRegValue);
      //
      cout << "Enter data value to write (hex, <=4 characters, i.e. 16 bits)" << endl;
      int TMBRegWriteData;
      cin >> hex >> TMBRegWriteData >> dec;  // Make sure to return to decimal mode after reading
      //
      cout << "Echo desired write data=" << hex << TMBRegWriteData << dec << endl;
      //
      thisTMB->WriteRegister(TMBRegAddr,TMBRegWriteData);
      //
      TMBRegValue  = thisTMB->ReadRegister(TMBRegAddr) ;
      printf("TMB register Address= %x     read back written value=0x %x \n",TMBRegAddr,TMBRegValue);
      //
      cout << endl  << endl;
      //tbController.DcsEnable();
      //
    }

    // Reads all TMB registers and part (the first 12) of the CCB  registers 
    // JH and DM 26-jun-05
    // use hex for everything (duh)

    if (doTMBRegisterDump) { 

      //tbController.DcsDisable();

      //this part does a complte register dump of the TMB registers

      string TMBregname[]={"ADR_IDREG0","ADR_IDREG1","ADR_IDREG2","ADR_IDREG3", "ADR_VME_STATUS",
			"ADR_VME_ADR0","ADR_VME_ADR1",
			"ADR_LOOKBK","ADR_USR_JTAG","ADR_PROM","ADR_DDDSM","ADR_DDD0","ADR_DDD1",
			"ADR_DDD2","ADR_DDD0E",
			"ADR_RATCTRL","ADR_STEP","ADR_LED","ADR_ADC","ADR_DSN","ADR_MOD_CFG",
			"ADR_CCB_CFG","ADR_CCB_TRIG",
			"ADR_CCB_STAT","ADR_ALCT_CFG","ADR_ALCT_INJ","ADR_ALCT0_INJ","ADR_ALCT1_INJ",
			"ADR_ALCT_STAT",
			"ADR_ALCT0_RCD","ADR_ALCT1_RCD","ADR_ALCT_FIFO","ADR_DMB_MON","ADR_CFEB_INJ",
			"ADR_CFEB_INJ_ADR",
			"ADR_CFEB_INJ_WDATA","ADR_CFEB_INJ_RDATA","ADR_HCM001","ADR_HCM023",
			"ADR_HCM045","ADR_HCM101",
			"ADR_HCM123","ADR_HCM145","ADR_HCM201","ADR_HCM223","ADR_HCM245","ADR_HCM301",
			"ADR_HCM323",
			"ADR_HCM345","ADR_HCM401","ADR_HCM423","ADR_HCM445","ADR_SEQ_TRIG_EN",
			"ADR_SEQ_TRIG_DLYO",
			"ADR_SEQ_TRIG_DLY1",
			"ADR_SEQ_ID","ADR_SEQ_CLCT","ADR_SEQ_FIFO","ADR_SEQ_L1A","ADR_SEQ_OFFSET",
			"ADR_SEQ_CLCT0",
			"ADR_SEQ_CLCT1",
			"ADR_SEQ_TRIG_SRC","ADR_DMB_RAM_ADR","ADR_DMB_RAM_WDATA","ADR_DMB_RAM_WDCNT",
			"ADR_DMB_RAM_RDATA","ADR_TMB_TRIG","ADR_MPC0_FRAME0","ADR_MPC0_FRAME1",
			"ADR_MPC1_FRAME0","ADR_MPC1_FRAME1",
			"ADR_MPC_INJ","ADR_MPC_RAM_ADR","ADR_MPC_RAM_WDATA","ADR_MPC_RAM_RDATA",
			"ADR_SCP_CTRL","ADR_SCP_RDATA",
                        "ADR_CCB_CMD","ADR_BUF_STAT","ADR_SRLPGM","ADR_ALCT_F1F01","ADR_ALCT_F1F02",
			"ADR_ADJCFEB0",
			"ADR_ADJCFEB1",
			"ADR_ADJCFEB2","ADR_SEQMOD","ADR_SEQSM","ADR_SEQCLCTM","ADR_TMBTIM",
			"ADR_LHC_CYCLE","ADR_RPC_CFG",
			"ADR_RPC_RDATA", "ADR_RPC_RAW_DELAY","ADR_RPC_INJ","ADR_RPC_INJ_ADR",
			"ADR_RPC_INJ_WDATA","ADR_RPC_INJ_RDATA",
			"ADR_RPC_BXN_DIFF","ADR_RPC0_HCM","ADR_RPC1_HCM","ADR_RPC2_HCM","ADR_RPC3_HCM",
			"ADR_SCP_TRIG","ADR_CNT_CTRL",
			"ADR_CNT_RDATA"};

      string TMBdescription[]={"ID Register 0","ID Register 1","ID Register 2","ID Register 3",
			    "VME Status Register",
			       "VME Address read-back","VME Address read-back","Loop-back Register",
			    "User JTAG","PROM",
			       "3D3444 State Machine Reg + Clock DCMs","3D3444 Delay Chip 0",
			    "3D3444 Delay Chip 1",
			       "3D3444 Delay Chip 2","3D3444 Delay Chip Output Enables",
			    "RAT Module Control","Step Register",
			       "Front Panel + On-board LEDs",
			       "ADCs","Digital Serials","TMB Configurations","CCB Configurations",
			    "CCB Trigger Control",
			       "CCB Status","ALCT Configurations","ALCT Injector Control",
			    "ALCT Injected ALCT0","ALCT Injected ALCT1",
			       "ALCT Sequencer Control/Status","ALCT LCT0 Received by TMB",
			    "ALCT LCT1 Received by TMB",
			       "ALCT FIFO RAM Status","DMB Monitored signals","CFEB Injector Control",
			    "CFEB Injector RAM address",
			       "CFEB Injector Write Data","CFEB Injector Read Data",
			       "CFEB0 Ly0,Ly1 Hot Channel Mask","CFEB0 Ly2,Ly3 Hot Channel Mask",
			    "CFEB0 Ly4,Ly5 Hot Channel Mask",
			       "CFEB1 Ly0,Ly1 Hot Channel Mask","CFEB1 Ly2,Ly3 Hot Channel Mask",
			    "CFEB1 Ly4,Ly5 Hot Channel Mask",
			       "CFEB2 Ly0,Ly1 Hot Channel Mask","CFEB2 Ly2,Ly3 Hot Channel Mask",
			    "CFEB2 Ly4,Ly5 Hot Channel Mask",
			       "CFEB3 Ly0,Ly1 Hot Channel Mask","CFEB3 Ly2,Ly3 Hot Channel Mask",
			    "CFEB3 Ly4,Ly5 Hot Channel Mask",
			       "CFEB4 Ly0,Ly1 Hot Channel Mask","CFEB4 Ly2,Ly3 Hot Channel Mask",
			    "CFEB4 Ly4,Ly5 Hot Channel Mask",
			       "Sequencer Trigger Source Enables","Sequencer Trigger Source Delays",
			    "Sequencer Trigger Source Delays",
			       "Sequencer Board + CSC ID","Sequencer CLCT Configuration",
			    "Sequencer FIFO Configuration",
			       "Sequencer L1A Configuration","Sequencer Counter Offsets",
			    "Sequencer Latched CLCT0",
			       "Sequencer Latched CLCT1","Sequencer Trigger Source Read-back",
			    "Sequencer RAM Address",
			       "Sequencer RAM Write Data","Sequencer RAM Word Count",
			    "Sequencer RAM Read Data",
			       "TMB Trigger Configuration/MPC Accept",
			       "MPC0 Frame 0 Data sent to MPC","MPC0 Frame 1 Data sent to MPC",
			    "MPC1 Frame 0 Data sent to MPC",
			       "MPC1 Frame 1 Data sent to MPC","MPC Injector Control",
			    "MPC Injector RAM address",
			       "MPC Injector RAM Write Data","MPC Injector RAM Read Data",
			    "Scope control","Scope read data",
			       "CCB TTC Command Generator","Buffer Status","SRL LUT Program",
			    "ALCT Raw hits RAM Control",
			       "ALCT Raw hits RAM data","CFEB Adjacent hs Mask lsbs",
			    "CFEB Adjacent hs Mask msbs",
			       "CFEB Adjacent ds Mask",
			       "Sequencer Trigger Modifiers","Sequencer Machine State",
			    "Sequencer CLCT msbs",
			       "TMB Timing for ALCT*CLCT coincidence","LHC Cycle period, Maximum BXN+1",
			       "RPC Configuration","RPC Sync Mode Read Data","RPC Raw Hits Delay",
			    "RPC Injector Control",
			       "Injector RAM Address","RPC Injector Write Data","RPC Injector Read Data",
			       "RPC BXN Differnces",
			       "RPC0 Hot Chamber Mask","RPC1 Hot Chamber Mask","RPC2 Hot Chamber Mask",
			    "RPC3 Hot Chamber Mask",
			       "Scope Trigger Source Channel","Status Counter Control","Status Counter Data"};

      int j;
      int reg_value=0;
      cout << "All TMB Registers" << endl;
      for (j=0;j<106;j++) {
      reg_value = thisTMB->ReadRegister(j*2); //comment this line out to work with code when register can't be read
	printf("addr= %02x   value= %04x  name= %s  \'%s\' \n", j*2,
	     reg_value, TMBregname[j].c_str(),TMBdescription[j].c_str() );
          }
      cout << endl;

      //Now let's do a partial dump of the CCB register...just the first 12 registers

      int CCBaddr[]={0x0,0x2,0x4,0x20,0x22, 0x24,0x26,0x28,0x2a,0x2c,0x2e};
      string CCBname[]={"CSRA1 (control/status register), discrete logic",
			    "CSRA2 (status register), discrete logic",
			    "CSRA3 (status register), discrete logic",
			    "CSRB1 (control register), FPGA",
			    "CSRB2 (command bus), FPGA",
			    "CSRB3 (data bus), FPGA",
			    "CSRB4 (general purpose R/W register, future use), FPGA",
			    "CSRB5 (delay register), FPGA",
			    "CSRB6 (control register), FPGA",
			    "CSRB7 (QPLL control register), FPGA",
			    "CSRB8 (general purpose R/W register, future use), FPGA",
			    "CSRB9 (status register, serial ID chip), FPGA"};

      cout << "First 12 CCB Registers" << endl;
      int i;
      int CCBreg_value=0;

      for (i=0;i<11;i++) {
      CCBreg_value = thisCCB->ReadRegister(CCBaddr[i]); //comment this line out to work with code when register can't be read
	printf("addr= %02x   value= %04x  name= %s \n",
	     CCBaddr[i], CCBreg_value, CCBname[i].c_str() );
          }
      cout << endl;



      //tbController.DcsEnable();

  }

    //
    /* 
           int j;
      for (j=0;j<106;j++) {
	cout << "address= "<< hex << setw(18) << j*2 << dec
	     << "            register value="<< hex << setw(19) << thisTMB->ReadRegister(j*2)<<dec << endl;
	cout <<"register name= " << setw(18) << regname[j]
	     << "      description= " << setw(25) << description[j] << endl;
	cout << endl;
        }
      cout << endl;
  }
    */
    //

    // Read arbitrary CCB register: caution, if register doesn't exist, can cause problem(?)
    // JH and DM 1-july-05
    // use hex for everything (duh)

    if (doReadCCBRegister) {           
      //tbController.DcsDisable();
      cout << "Enter CCB register address (hex)" << endl;
      int CCBRegAddr;
      cin >> hex >> CCBRegAddr >> dec;            // Make sure to return to decimal mode
      int CCBRegValue  = thisCCB->ReadRegister(CCBRegAddr) ;
      printf("CCB register Address= %02x     value=0x %04x \n",CCBRegAddr,CCBRegValue);
      cout << endl  << endl;
      //tbController.DcsEnable();
    }

    // Write arbitrary CCB register: caution, can of course cause all kinds of problems
    // JH and DM 26-jun-05
    // use hex for everything

    if (doWriteCCBRegister) { 
      //
      //tbController.DcsDisable();
      //
      cout << "Caution: can cause serious problems, proceed with caution. CAUTION!" << endl;
      cout << "Enter CCB register address (hex)" << endl;

      int CCBRegAddr;
      cin >> hex >> CCBRegAddr >> dec;  // Make sure to return to decimal mode after reading

      int CCBRegValue  = thisCCB->ReadRegister(CCBRegAddr) ;
      printf("CCB register Address= %02x     read initial value=0x %04x \n",CCBRegAddr,CCBRegValue);

      cout << "Enter data value to write (hex, <=4 characters, i.e. 16 bits)" << endl;
      int CCBRegWriteData;
      cin >> hex >> CCBRegWriteData >> dec;  // Make sure to return to decimal mode after reading

      cout << "Echo desired write data=" << hex << CCBRegWriteData << dec << endl;

      thisCCB->WriteRegister(CCBRegAddr,CCBRegWriteData);

      CCBRegValue  = thisCCB->ReadRegister(CCBRegAddr) ;
      printf("CCB register Address= %02x     read back written value=0x %04x \n",CCBRegAddr,CCBRegValue);

      cout << endl  << endl;
      //
      //tbController.DcsEnable();
      //
    }


    // Read arbitrary DMB register: caution, if register doesn't exist, can cause problem(?)
    // JH and DM 12-july-05
    // use hex for everything

    if (doReadDMBRegister) { 
      cout << "function not implemented" << endl;
    /*          
      cout << "Enter DMB register address (hex)" << endl;
      int DMBRegAddr;
      cin >> hex >> DMBRegAddr >> dec;            // Make sure to return to decimal mode
      int DMBRegValue  = thisDMB->ReadRegister(DMBRegAddr) ;
      printf("DMB register Address= %x     value=0x %x \n",DMBRegAddr,DMBRegValue);
      cout << endl  << endl;
    */
      }
    

    // Write arbitrary DMB register: caution, can of course cause all kinds of problems
    // JH and DM 12-july-05
    // use hex for everything

    if (doWriteDMBRegister) {           
      cout << "function not implemented" << endl;
      
      /*
      cout << "Caution: can cause serious problems, proceed with caution. Caution!" << endl;
      cout << "Enter DMB register address (hex)" << endl;

      int DMBRegAddr;
      cin >> hex >> DMBRegAddr >> dec;  // Make sure to return to decimal mode after reading

      int DMBRegValue  = thisDMB->ReadRegister(DMBRegAddr) ;
      printf("DMB register Address= %x     read initial value=0x %x \n",DMBRegAddr,DMBRegValue);

      cout << "Enter data value to write (hex, <=4 characters, i.e. 16 bits)" << endl;
      int DMBRegWriteData;
      cin >> hex >> DMBRegWriteData >> dec;  // Make sure to return to decimal mode after reading

      cout << "Echo desired write data=" << hex << DMBRegWriteData << dec << endl;

      thisDMB->WriteRegister(DMBRegAddr,MBRegWriteData);

      DMBRegValue  = thisDMB->ReadRegister(DMBRegAddr) ;
      printf("DMB register Address= %x     read back written value=0x %x \n",DMBRegAddr,DMBRegValue);

      cout << endl  << endl;
      */
    }

   
    
    
    if (doReadDMBCounters) {

      //tbController.DcsDisable();

      // JH test code 12 Aug 2005
      // Add some analysis ability to this option


      cout << "Enter 1 for simple print-out" << endl;
      cout << "      2 for print most frequent values" <<endl;
      cout << "      3 for cuts by TMB DAV and/or same DAV, then print most frequent" <<endl;

      int user_option;
      cin >> user_option;

      if(user_option<1 | user_option>3) cout << "Invalid option entered" << endl;

      //Simple read counters option:

      if(user_option==1) {
	//
        thisDMB->readtimingCounter();
	//
        thisDMB->readtimingScope();
	//
	int trials = 0;
	while ( thisDMB->GetL1aLctScope() == 0 && trials < 100 ) {
	  thisDMB->readtimingScope();
	  trials++;
	}
	//
        printf("  L1A to LCT delay: %d", thisDMB->GetL1aLctCounter()  ); printf(" CMS clock cycles \n");
        printf("  CFEB DAV delay:   %d", thisDMB->GetCfebDavCounter() ); printf(" CMS clock cycles \n");
        printf("  TMB DAV delay:    %d", thisDMB->GetTmbDavCounter()  ); printf(" CMS clock cycles \n");
        printf("  ALCT DAV delay:   %d", thisDMB->GetAlctDavCounter() ); printf(" CMS clock cycles \n");
	//
	cout << endl ;
	//
        cout << "  L1A to LCT Scope: " ;
	cout << setw(3) << thisDMB->GetL1aLctScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetL1aLctScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  CFEB DAV Scope:   " ;
	cout << setw(3) << thisDMB->GetCfebDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetCfebDavScope()>>i)&0x1) ;
	cout << endl ;
	//
	cout << "  TMB DAV Scope:    " ;
	cout << setw(3) << thisDMB->GetTmbDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetTmbDavScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  ALCT DAV Scope:   " ;
	cout << setw(3) << thisDMB->GetAlctDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetAlctDavScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  Active DAV Scope: " ;
	cout << setw(3) << thisDMB->GetActiveDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetActiveDavScope()>>i)&0x1) ;
	cout << endl ;
	//
	cout << endl ;
	//
	}
      
      //Loop and choose "best" option:

      else {
	cout << "Enter number of DMB counter reads to perform: " ;
	int nloop;
	cin >> nloop;

	int davsame;
	int tmbdavcut;
	if(user_option==3) {
	  cout << "Enter value of TMB DAV to cut on (suggest 6, use -1 for no cut):" << endl;
	  cin >> tmbdavcut;

	  cout << "Enter -1 for no cut against same DAV delays or 1 to use cut:" << endl;
	  cin >> davsame;
	}
	
	//Before looping, zero all of the counters
	
	int type;
	int delay;
	// counts for [time bin,type] where 
	//       type=0 for LCT-L1A delay, 1 for CFEBDAV, 2 for TMBDAV, 3 for ALCTDAV
	int counts[256][4]; 
	for(type=0;type<4;type++) { 
          for(delay=0;delay<256;delay++) {
	    counts[delay][type] = 0;}
	}
	
	//Next read the counters nloop times and accumulate statistics
	
	int iloop;
	int passcuts=0;
        for(iloop=0;iloop<nloop;iloop++){
	  thisDMB->readtimingCounter();
	  //
	  int l1alct  = thisDMB->GetL1aLctCounter();
	  int cfebdav = thisDMB->GetCfebDavCounter();
	  int tmbdav  = thisDMB->GetTmbDavCounter();
	  int alctdav = thisDMB->GetAlctDavCounter();
	  //
	  //	  cout << "Debug: l1alct= " << l1alct << "cfebdav=" << cfebdav;
	  //      cout << " tmbdav=" <<tmbdav << " alctdav=" << alctdav << endl;
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
	    //	    cout << "Delay=" << delay << " Counts are";
	    //	    cout << "  LCT-L1A : " << counts[delay][0]; 
	    //	    cout << "  CFEB-DAV: " << counts[delay][1];
	    //	    cout << "  TMB-DAV:  " << counts[delay][2];
	    //	    cout << "  ALCT-DAV: " << counts[delay][3] << endl; 
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
	cout << endl << " Best delay settings in " << nloop << " readings and " 
	     << passcuts << " passing cuts are:" << endl;
	cout << endl;
	cout << "  LCT -L1A delay=" << maxdelay[0] << " (" << maxnum[0] << "readings)" << endl;
	cout << "  CFEB-DAV delay=" << maxdelay[1] << " (" << maxnum[1] << "readings)" << endl;
	cout << "  TMB -DAV delay=" << maxdelay[2] << " (" << maxnum[2] << "readings)" << endl;
	cout << "  ALCT-DAV delay=" << maxdelay[3] << " (" << maxnum[3] << "readings)" << endl;
	cout << endl;
	//
      }
      //tbController.DcsEnable();
    }    
    //
    if (doReadDMB_DAV) {
      cout << "Function not implemented" << endl;
      //     DAQMB::readdavdelay()
    }
    
    if (doWriteDMB_DAV) {
      cout << "Function not implemented" << endl;
      //     DAQMB::setdavdelay()
    }
    
    
    
    if (doReadALCTThreshold ) {
      //
      //tbController.DcsDisable();
      //
      int err;
      ALCTIDRegister sc_id, chipID ;
      long readval;
      //for (int i=0; i<0xff; i++) {
      //alct->alct_set_thresh(4, i);
	//alct->alct_read_thresh(3,&readval);
	//printf("ReadBack AFEB : %d = %d \n",3,readval);
	//alct->alct_read_thresh(4,&readval);
	//alct->alct_read_thresh(5,&readval);
	//printf("ReadBack AFEB : %d = %d \n",5,readval);
      //}
      //}
      
      ///
      err = alct->alct_read_slowcontrol_id(&sc_id) ;
      std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;

      alct->alct_fast_read_id(chipID);
      std::cout << "ALCT alct_fast_read_id " << chipID << std::endl;
      //
      unsigned cr[3];
      //
      printf("FastControl SelfTest\n");
      printf ("\nalct_fast_self_test returned %ld\n", alct->alct_fast_self_test ((long int*)NULL,(unsigned long)1));
      //
      printf("SlowControl SelfTest\n");
      long code;
      int slot = thisTMB->slot();
      printf ("\nalct_slow_self_test returned %ld\n", alct->alct_slow_self_test (&slot, &code));
      printf ("code %d \n",code);
      /*
      printf("Number of afebs %d \n",alct->nAfebs());
      int val = 10 ;
      for (int i=0; i<alct->nAfebs(); i++) {
	alct->alct_set_thresh(i, val);
	printf("Setting thresh to %d \n",val);
	alct->alct_read_thresh(i,&readval);
	printf("ReadBack AFEB : %d = %d \n",i,readval);
      }
      */
      //
      //alct->setup(1);
      //
      //tbController.DcsEnable();
    }

    if (doCCBFPGA) {
      //tbController.DcsDisable();
      printf("Setting into FPGA\n");
      thisCCB->setCCBMode(CCB::VMEFPGA);      
      //tbController.DcsEnable();
    }

    if (doCCBDLOG) {
      //tbController.DcsDisable();
      printf("Setting into DLOG\n");
      thisCCB->setCCBMode(CCB::DLOG);      
      //tbController.DcsEnable();
    }

    if (doDumpFifo){
      //tbController.DcsDisable();
      if (alct) alct->DumpFifo();
      //tbController.DcsEnable();
    }

    if ( doStartTTC) {
      //tbController.DcsDisable();
      thisTMB->StartTTC();
      //tbController.DcsEnable();
    }

    if (doFindBestL1aALCT){
      //tbController.DcsDisable();
      util.FindBestL1aAlct();
      //tbController.DcsEnable();
    }

    if (doResetCounters){
      //tbController.DcsDisable();
      thisTMB->ResetCounters();
      //tbController.DcsEnable();
    }

    if (doPrintCounters){
      ////tbController.DcsDisable();
      thisTMB->GetCounters();
      thisTMB->PrintCounters();
      ////tbController.DcsEnable();
    }

    if (doALCTChamberScanning){
      //tbController.DcsDisable();
      ALCTChamberScanning();
      //tbController.DcsEnable();
    }

    if (doALCTSVFLoad) {
      //tbController.DcsDisable();
      ALCTSVFLoad();
      //tbController.DcsEnable();
    }

    // TMB Scope
    if (doTMBScope) {
      //tbController.DcsDisable();
      thisTMB->decode();
      //tbController.DcsEnable();
    }
    
    //ALCT Rawhits
    if (doALCTRawhits) {
      //tbController.DcsDisable();
      thisTMB->ALCTRawhits();
      //tbController.DcsEnable();
    }
    
    if (doTMBRawhits) {
      //tbController.DcsDisable();
      thisTMB->TMBRawhits();
      //tbController.DcsEnable();
    }
    
    if (doPulseTest){
      //tbController.DcsDisable();
      PulseCFEB();
      //tbController.DcsEnable();
    }
    
    if (doCFEBChamberScanning){
      //tbController.DcsDisable();
      CFEBChamberScanning();
      //tbController.DcsEnable();
    }
    
    if (doCFEBTiming){
      //tbController.DcsDisable();
      float CFEBMean[5];
      CFEBTiming(CFEBMean);
      //tbController.DcsEnable();
    }
    
    if (doALCTTiming){
      //tbController.DcsDisable();
      //int RXphase, TXphase;
      util.ALCTTiming();
      //ALCTTiming(RXphase,TXphase);
      //tbController.DcsEnable();
    }

    if (doPulseTestStrips){
      thisTMB->ResetALCTRAMAddress();
      //while (1 <2 ) {
      //PulseRandomALCT();
      //
      util.PulseRandomALCT();
      //printf("\n WordCount = %x \n",thisTMB->GetALCTWordCount());
      //}
      //
    }
  }
  
  if (doReadIDCODE) {
    //tbController.DcsDisable();
    cout << "Read IDCODE" << endl;
    alct->ReadIDCODE();
    //tbController.DcsEnable();
  }
    
  if (doLv1TMBTiming){
    //tbController.DcsDisable();
    int L1aTMB = TMBL1aTiming();
    //tbController.DcsEnable();
  }

  int scp_channel;
  if (doScopeReadArm){
    //tbController.DcsDisable();
     cout << "Arm Scope: select channel (suggest 1d) in hex:" << endl;
     cin >> hex >> scp_channel >> dec;
     // scanf("%x", &scp_channel);
     if(scp_channel > 127 | scp_channel < 0)
       cout << "Invalid channel selected" << endl;
     else
       thisTMB->scope(1,0,scp_channel);
     //
     //tbController.DcsEnable();
  }

  if (doScopeRead){
    //tbController.DcsDisable();
    cout << "Readout Scope"<< endl;
    thisTMB->scope(0,1);
    //tbController.DcsEnable();
  }

  if (doForceScopeTrigger){
    //tbController.DcsDisable();
    cout << "Force Scope Trigger" << endl;
    thisTMB->ForceScopeTrigger();
    //tbController.DcsEnable();
  }

  if (doInteractive) goto interactive;

 outhere:

  // identify TMB
  cout << dec; // restore to decimal printing ...
  cout << "Hello, i am TMB in crate "<< thisTMB->crate()
       << " at slot "<< thisTMB->slot() 
       << " version " << thisTMB->version_ << endl;

  // scan ALCT-TMB timing
  //if (doPHOS4alct){
    //cout << "---- PHOS4 alct timing ----" << endl;
    
    // scan ALCT-TMB rx,tx settings and build histograms
    // UFL cosmic set-up: use 5-10s per scan evt.
    // beam test: 0s should be enough.
    //thisTMB->tmb_PHOS4_alct(scantime);
  //}

  // scan CFEB-TMB timing.
  //if (doPHOS4cfeb){
  //cout << "---- PHOS4 cfeb timing ----" << endl;
  //thisTMB->tmb_PHOS4_cfeb();
  //}

  // done: disable triggering
  //cout << "---- Test Beam Disable ----"<<endl;
  //if (useScint)
    ////tbController.disable();
    //else {
      //thisCCB->disableL1();
      // thisCCB->disable();
      //sleep(1);
  //}
  cout<< "---- done ----" << endl;


  //----------------------------------------------------//
  /*
  // maximum scan range on the PHOS4
  int scanMax(25);
  if (thisTMB->version_ == "2004") scanMax=13;

  int array1[scanMax][scanMax],
      array2[scanMax][scanMax],
      array3[scanMax][scanMax];

  //-- Read back the configuration files --//
  // replce this by making the wordcount1 arrays availabe
  // (protected) as members of TMB

  // open file (created by tmb_PHOS4_alct)
  ifstream alctfile("tmb_alct.dat",ios::binary|ios::in);
  if(!alctfile){
    cerr << "Unable to open file tmb_alct.dat. Exiting"<< endl;
    exit(1);
  }

  // get array1
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array1[j][k];
    
  // skip the intermediate line
  char dummy[80];
  alctfile.getline(dummy,80,'\n');

  // get array2 (not used)
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array2[j][k];

  // skip all the intermediate crap
  for (int i=0;i<12;i++)
    alctfile.getline(dummy,80,'\n');

  // get array3
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array3[j][k];


  //-- ALCT-TMB timing --//
  cout << "-- ALCT-TMB timing (cf. tmb_alct.dat) --" << endl;

  // 1.determine the tx delay
  int minValidRx(scanMax); // determines a valid tx scan (should be scanMax);
  int validTx(0),firstTx(-1);

  // find acceptable tx regio
  for (int tx=0;tx<scanMax;tx++){
    int validRx(0);
    for (int rx=0;rx<scanMax;rx++){
      if (array1[tx][rx]==0) validRx++;      
    }
    if (validRx >= minValidRx){
      if (firstTx ==-1) firstTx=tx;
      validTx++;
    }
    cout << tx << " " << validRx << " " << firstTx << " " << validTx << endl;
  }

  int alctTx;
  if (firstTx!=-1){
     alctTx = firstTx + validTx/2;
    cout << "tx = " << alctTx << endl;
  } else {
    cout << "unable to determine tx" << endl;
    exit(1);
  }


  // 2. use tx and determine rx delay

  int firstRx(-1), validRx(0);
  for (int rx=0;rx<scanMax;rx++){
    if (array3[alctTx][rx]){
      if (firstRx==-1) firstRx = rx;
      validRx++;
    }
  }

  int alctRx(-1);
  if (firstRx!=-1){
    alctRx = firstRx + validRx/2;
    cout << " rx = " << alctRx << endl;
  } else {
    cout << "unable to determine rx" << endl;
    exit(1);
  }


  //-- CFEB-TMB timing --//
  cout << "-- CFEB-TMB timing (cf. tmb_scan.dat) --" << endl;

  // work in progress ...

  */
 
}

//
void InitStartSystem(){
  //
  cout << "Init System " << endl ;
  int input ;
  cin >> input ;
  //
  //tbController.configureNoDCS();          // Init system
  emuController.init();
  emuController.configure();
  thisTMB->StartTTC();
  thisTMB->EnableL1aRequest();
  thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
  // Set constants
  //
}
//
int AdjustL1aLctDMB(){
  //
  thisDMB->readtimingCounter();
  if ( thisDMB->GetL1aLctCounter() == 0 ) {
    printf(" ****************************** \n");
    printf(" *** You need to enable DMB *** \n");
    printf(" ****************************** \n");
    //return -1;
  }
  //
  int Counter = -1 ;
  //
  for( int l1a=90; l1a<110; l1a++) {
    thisCCB->SetL1aDelay(l1a);
    ::sleep(2);
    thisDMB->readtimingCounter();
    Counter = thisDMB->GetL1aLctCounter() ;
    printf(" ************ L1a lct counter l1a=%d Counter=%d \n ",l1a,Counter);
    //
    if ( Counter >= 115 && Counter <= 117 ) break ;
    //
  }
  //
  return Counter;
  //
}
//
void Automatic(){
  //
  ///////////////////////////////////////////////////////////////// Do AdjustL1aLctDMB()
  //
  InitStartSystem();
  //
  // Enable all...for DMB....be careful....
  //
  thisCCB->startTrigger();
  thisCCB->bc0(); 
  //
  BestCCBDelaySetting = AdjustL1aLctDMB();
  //
  cout << "Best L1a Lct DMB CCB delay "<< BestCCBDelaySetting << endl ;
  //
  if (BestCCBDelaySetting == -1 ) return ;
  //
  ///////////////////////////////////////////////////////////////// Do CFEB phases
  //
  InitStartSystem();
  //
  // Now set L1a delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  CFEBTiming(CFEBMean);
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++ ) printf(" %f ",CFEBMean[CFEBs]);
  printf("\n");
  //
  //////////////////////////////////////////////////////////////// Do ALCT phases
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  int ALCT_L1a_delay_pulse = 110;
  //
  while ( ALCTRXphase == 0 && ALCTTXphase == 0 ) {
    //
    ALCT_L1a_delay_pulse += 5;
    // 
    unsigned cr[3];
    alct->GetConf(cr,1);
    cr[1] = (cr[1] & 0xfffff00f) | ((ALCT_L1a_delay_pulse&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    //
    ALCTTiming(ALCTRXphase,ALCTTXphase);
    //
    printf("ALCT RX=%d TX=%d \n",ALCTRXphase,ALCTTXphase);
    //
    printf("ALCT_L1a_delay = %d \n",ALCT_L1a_delay_pulse);
    // 
    //int input;
    //cin >> input ;
    //
  }
  //
  printf("ALCT_L1a_delay = %d \n",ALCT_L1a_delay_pulse);
  //
  ////////////////////////////////////////////////////////////////// Do TMB L1a timing
  //
  // Init System again
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(ALCTRXphase,5) ;
  thisTMB->tmb_clk_delays(ALCTTXphase,6) ;	 
  //
  TMB_L1a_timing = TMBL1aTiming(); // Use pulsing
  //
  // Init System again
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(ALCTRXphase,5) ;
  thisTMB->tmb_clk_delays(ALCTTXphase,6) ;	 
  //
  int TMB_L1a_delay = FindTMB_L1A_delay(100,150); // Use real data
  //
  printf(" TMB_l1a_timing=%d TMB_L1a_delay=%d \n ",TMB_L1a_timing, TMB_L1a_delay);
  //
  //int input;
  //cin >> input;
  //
  ////////////////////////////////////////////////////////////////////// Do ALCT L1a timing
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(ALCTRXphase,5) ;
  thisTMB->tmb_clk_delays(ALCTTXphase,6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  Find_ALCT_L1a_delay = FindBestL1aAlct(); // Use pulsing
  //
  InitStartSystem();
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(ALCTRXphase,5) ;
  thisTMB->tmb_clk_delays(ALCTTXphase,6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  ALCT_L1a_delay = FindALCT_L1A_delay(50,150); // Use real data
  //
  printf("ALCT delay %d %d \n",Find_ALCT_L1a_delay,ALCT_L1a_delay); 
  //
  //int input2;
  //cin >> input2;
  //
  //////////////////////////////////////////////////////////////////////// Get alct vpf
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(ALCTRXphase,5) ;
  thisTMB->tmb_clk_delays(ALCTTXphase,6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  // Now set L1a ALCT delay
  //
  unsigned cr[3];
  alct->GetConf(cr,1);
  cr[1] = (cr[1] & 0xfffff00f) | ((ALCT_L1a_delay&0xff)<<4) ;
  alct->SetConf(cr,1);
  alct->unpackControlRegister(cr);
  //
  ALCTvpf = FindALCTvpf(); // Use data for this
  //
  ////////////////////////////////////////////////////////////////////////////////////// Result
  //
  // Result
  // 
  printf("******************** End result ********************\n");
  printf("CFEB phases : \n");
  for( int CFEBs=0; CFEBs<5; CFEBs++) printf("CFEBs%d= %f ",CFEBs,CFEBMean[CFEBs]);
  printf("\n");
  printf(" Best CCB delay  ; \n",BestCCBDelaySetting);      
  printf("ALCT phases    : ");
  printf("        ALCT RX=%d TX=%d \n",ALCTRXphase,ALCTTXphase);  
  printf("TMB L1a delay  : ");
  printf("        TMB_L1a_delay=%d \n",TMB_L1a_delay);
  printf("ALCT L1a delay : ");
  printf("        ALCT_L1a_delay=%d ",ALCT_L1a_delay);
  if (ALCT_L1a_delay == 0 ) printf(" *** Failed *** ") ;
  printf("\n");
  printf("ALCTvpf        : ");
  printf("        ALCTvpf=%d \n",ALCTvpf);
  //
}
//
void ALCTSVFLoad(){
  int jch = 3;
  char* filename="/home/fastcosmic/Erase.svf";
  alct->NewSVFLoad(&jch,filename,10);
}
//
void ALCTChamberScanning(){
   //
   unsigned long HCmask[22];
   unsigned long HCmask2[22];
   //
   int NPulses = 1;
   int chamberResult[112];
   int chamberResult2[112];
   int InJected[112];
   //
   thisTMB->SetALCTPatternTrigger();
   //
   for ( int keyWG=0; keyWG<112; keyWG++) chamberResult[keyWG] = 0;
   for ( int keyWG=0; keyWG<112; keyWG++) chamberResult2[keyWG] = 0;
   for ( int keyWG=0; keyWG<112; keyWG++) InJected[keyWG] = 0;
   //
   for (int Ninject=0; Ninject<NPulses; Ninject++){
      //
      for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) {
	 //
	cout << endl;
	printf("%c[01;43m", '\033');
	cout << "Injecting in WG = " << dec << keyWG ;
	printf("%c[0m", '\033'); 
	cout << endl;
	//
	 for (int i=0; i<22; i++) {
	   HCmask[i] = 0;
	   HCmask2[i] = 0;
	 }
	 //
	 bitset<672> bits(*HCmask) ;
	 //
	 for (int i=0;i<672;i++){
	   if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
	   //if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
	 }
	 //
	 bitset<32> Convert;
	 //
	 Convert.reset();
	 //
	 for (int i=0;i<(alct->GetWGNumber());i++){
	   if ( bits.test(i) ) Convert.set(i%32);
	   if ( i%32 == 31 ) {
	     HCmask[i/32] = Convert.to_ulong();
	     //printf(" Convert %d %d \n",i,Convert.to_ulong());
	     Convert.reset();
	   }
	 }
	 /*
	 for (int i=0; i<12; i++) {
	   //if ( i%2 == 0 ) {
	       if (keyWG<32) {
		 //HCmask[i] = 0x1<<keyWG;
	       } else {
		 //HCmask[i] = 0x1<<(keyWG-32);
	       }
	       //} else {
	       //if (keyWG<32) {
	       // HCmask[i] = 0x1<<(keyWG) ;
	       //} else {
	       // HCmask[i] = 0x1<<(keyWG-32);
	       //}
	       //}
	 }
	 */
	 //for (int i=0; i<22; i++ ) {
	 //printf(" %x %x \n",HCmask[i],HCmask2[i]);
	 //}
	 //
	 alct->alct_write_hcmask(HCmask);
	 alct->alct_read_hcmask(HCmask);
	 /*
	 for (int i=0; i<12; i++ ) {
	    if (i == 0 ) cout << "Layer 0 " ;
	    for(int j=0; j<32; j++ ) 
	       if (((HCmask[i]>>j)&0x1) == 0 ) {
		  cout <<"-";
	       } else {
		  printf("%c[01;44m", '\033');	 
		  cout <<"|";
		  printf("%c[0m", '\033'); 
	       }
	    if ((i+1)%2 == 0 ) {
	       cout << endl;
	       if (i+1<12) cout << "Layer " << (i+1)/2 << " ";
	    }
	 }
	 */
	 //
	 printf("\n");
	 for (int i=0; i<(alct->GetWGNumber()); i++) {
	   if ( i%(alct->GetWGNumber()/6) == 0 ) printf("\n");
	   if (bits.test(i)) {
	     printf("|");
	   } else {
	     printf("-");
	   }
	 }
	 //
	 cout << endl;
	 //
	 PulseTestStrips();
	 //
	 thisTMB->DecodeALCT();
	 //
	 printf("Wordcount %x \n",thisTMB->GetALCTWordCount());
	 //
	 if ( thisTMB->GetAlct0FirstKey()  == keyWG && thisTMB->GetAlct0Quality() >= 1 ) 
	    chamberResult[keyWG]++;
	 //
	 if ( thisTMB->GetAlct1SecondKey() == keyWG && thisTMB->GetAlct1Quality() >= 1 ) 
	    chamberResult2[keyWG]++;
	 //
	 InJected[keyWG]++;
	 //
      }
   }
   cout << endl;
   cout << "Wire" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << keyWG/100 ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << ((keyWG/10)%10) ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << keyWG%10 ;
   cout << endl;
   cout << "InJected" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << InJected[keyWG] ;
   cout << endl;
   cout << "CFEBChamberResult" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << chamberResult[keyWG] ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << chamberResult2[keyWG] ;
   cout << endl;
   printf("%c[0m", '\033'); 
}
//
void ALCTScanDelays(){
  //
  unsigned long HCmask[22];
  int CountDelay[20];
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  for (int i=0; i< 20; i++) CountDelay[i] = 0;
  //
  thisTMB->SetALCTPatternTrigger();
  //
  for ( int nloop=0; nloop<20; nloop++){
    for ( int DelaySetting=0; DelaySetting<20; DelaySetting++){
      //
      int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6);
      int ChamberSection = alct->GetWGNumber()/6;
      //
      cout << endl ;
      cout << "Injecting at " << dec << keyWG << endl;
      //
      for (int i=0; i< 22; i++) HCmask[i] = 0;
      //
      bitset<672> bits(*HCmask) ;
      //
      for (int i=0;i<672;i++){
	if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
      }
      //
      bitset<32> Convert;
      //
      Convert.reset();
      //
      for (int i=0;i<(alct->GetWGNumber());i++){
	if ( bits.test(i) ) Convert.set(i%32);
	if ( i%32 == 31 ) {
	  HCmask[i/32] = Convert.to_ulong();
	  Convert.reset();
	}
      }
      //
      printf("\n");
      //
      alct->alct_write_hcmask(HCmask);
      alct->alct_read_hcmask(HCmask);
      //
      cout << alct->alct_set_delay(-1,DelaySetting) << endl ; // Set all delays
      //
      PulseTestStrips();
      printf("Decode ALCT\n");
      thisTMB->DecodeALCT();
      printf("After Decode ALCT\n");
      //
      // Now analize
      //
      if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	   (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	   (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	   (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondKey() != alct1_bxn) ) {
	//
	alct0_quality     = thisTMB->GetAlct0Quality();
	alct1_quality     = thisTMB->GetAlct1Quality();
	alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	alct0_key         = thisTMB->GetAlct0FirstKey();
	alct1_key         = thisTMB->GetAlct1SecondKey();
	//
	if ( alct0_quality == 3 && alct0_key == keyWG &&
	     alct1_quality == 3 && alct1_key == keyWG ) CountDelay[DelaySetting]++ ;
	//
      }
      //
    }
  }
  //
  for ( int DelaySetting=0; DelaySetting < 20; DelaySetting++ ) cout << CountDelay[DelaySetting] << " " ;
  //
  cout << endl ;
  cout << endl ;
  //
}
//
void ALCTTiming( int & RXphase, int & TXphase ){
   //
   int maxTimeBins(13);
   int ProjectionX[13*2], ProjectionY[13*2];
   int selected[13][13];
   int selected2[13][13];
   int selected3[13][13];
   int ALCTWordCount[13][13];
   int rxtx_timing[13][13];
   int ALCTWordCountWrap[13*2][13*2];
   int ALCTConfDone[13][13];
   int j,k;
   int alct0_quality = 0;
   int alct1_quality = 0;
   int alct0_bxn = 0;
   int alct1_bxn = 0;
   int alct0_key = 0;
   int alct1_key = 0;
   //
   alct->set_empty(1);
   //
   thisTMB->SetALCTPatternTrigger();
   //
   for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       selected[j][k]  = 0;
       selected2[j][k] = 0;
       selected3[j][k] = 0;
     }
   }
   //
   unsigned long HCmask[22];
   //
   for (int i=0; i< 22; i++) HCmask[i] = 0;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	//
	int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6/4);
	int keyWG2 = (alct->GetWGNumber())/6-keyWG;
	int ChamberSection = alct->GetWGNumber()/6;
	printf("Injecting at %d \n",keyWG);
	//
	for (int i=0; i< 22; i++) HCmask[i] = 0;
	//
	bitset<672> bits(*HCmask) ;
	//
	for (int i=0;i<672;i++){
	  if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
	  if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
	}
	//
	bitset<32> Convert;
	//
	Convert.reset();
	//
	for (int i=0;i<(alct->GetWGNumber());i++){
	  if ( bits.test(i) ) Convert.set(i%32);
	  if ( i%32 == 31 ) {
	    HCmask[i/32] = Convert.to_ulong();
	    //printf(" Convert %d %d \n",i,Convert.to_ulong());
	    Convert.reset();
	  }
	}
	//
	/*
	  for (int i=0; i<(alct->GetWGNumber()/32); i++) {
	  if ( i%2 == 0 ) {
	  if (keyWG<ChamberSection) {
	  HCmask[i] = 0x1<<keyWG;
	  } else {
	  HCmask[i] = 0x1<<(keyWG-ChamberSection);
	  }
	  } else {
	  if (keyWG<ChamberSection) {
	  HCmask[i] = 0x1<<(keyWG) ;
	  } else {
	  HCmask[i] = 0x1<<(keyWG-ChamberSection);
	  }
	  }
	  }
	*/
	//for (int i=0;i<22;i++) {
	//
	//printf(" %02x ",HCmask[i]);
	//
	//if ( i%((alct->GetWGNumber())/8/6+1) == 0 ) printf("\n");
	//
	//}
	//
	printf("\n");
	//
	alct->alct_write_hcmask(HCmask);
	alct->alct_read_hcmask(HCmask);
	/*
	  for (int i=0; i<(alct->GetWGNumber())/32; i++ ) {
	  if (i == 0 ) cout << "Layer 0 " ;
	  for(int j=0; j<ChamberSection; j++ ) 
	  if (((HCmask[i]>>j)&0x1) == 0 ) {
	  cout <<"-";
	  } else {
	  printf("%c[01;44m", '\033');	 
	  cout <<"|";
	  printf("%c[0m", '\033'); 
	  }
	  if ((i+1)%2 == 0 ) {
	  cout << endl;
	  if (i+1<(alct->GetWGNumber())/32) cout << "Layer " << (i+1)/2 << " ";
	  }
	  }
	  cout << endl;
	*/
	//
	//while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
	//
	cout << "Setting k=" << k << " j="<<j << endl;
	thisTMB->tmb_clk_delays(k,5) ;
	thisTMB->tmb_clk_delays(j,6) ;	 
	thisTMB->ResetALCTRAMAddress();
	PulseTestStrips();
	printf("Decode ALCT\n");
	thisTMB->DecodeALCT();
	printf("After Decode ALCT\n");
	//
	selected[k][j]  = 0;
	selected2[k][j] = 0;
	selected3[k][j] = 0;
	//
	printf("Check data \n");
	if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	     (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	     (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	     (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	     (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	     (thisTMB->GetAlct1SecondKey() != alct1_bxn) )
	  {
	    alct0_quality     = thisTMB->GetAlct0Quality();
	    alct1_quality     = thisTMB->GetAlct1Quality();
	    alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	    alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	    alct0_key         = thisTMB->GetAlct0FirstKey();
	    alct1_key         = thisTMB->GetAlct1SecondKey();
	    ALCTWordCount[k][j] = thisTMB->GetALCTWordCount();
	    ALCTConfDone[k][j]  = thisTMB->ReadRegister(0x38) & 0x1 ;
	    //
	    if ( alct0_quality == 3 && alct0_key == keyWG) selected[k][j]++;
	    if ( alct1_quality == 3 && alct1_key == keyWG) selected[k][j]++;
	    if ( alct0_quality == 3 && alct0_key == keyWG2) selected2[k][j]++;
	    if ( alct1_quality == 3 && alct1_key == keyWG2) selected2[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG) &&
		 (alct1_quality == 3 && alct1_key == keyWG) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
		 (alct1_quality == 3 && alct1_key == keyWG2) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG) &&
		 (alct1_quality == 3 && alct1_key == keyWG2) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
		 (alct1_quality == 3 && alct1_key == keyWG) ) selected3[k][j]++;
	   //
	   /*
	     if ( alct0_quality >= 1 ) selected[k][j]++;
	     if ( alct1_quality >= 1 ) selected[k][j]++;
	     if ( alct0_quality >= 1 ) selected2[k][j]++;
	     if ( alct1_quality >= 1 ) selected2[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	   */
	  }	      
      }
   }
   //
   cout << "WordCount  (tx vs. rx)   tx ---->" << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     cout << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	if ( ALCTWordCount[j][k] >0 ) printf("%c[01;35m", '\033');	 
	printf("%02x ",ALCTWordCount[j][k]&0xffff);
	printf("%c[01;0m", '\033');	 
      }
      cout << endl;
   }
   //
   cout << endl;
   cout << "ConfDone (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     cout << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	if ( ALCTConfDone[j][k] >0 ) printf("%c[01;35m", '\033');	 
	printf("%02x ",ALCTConfDone[j][k]&0xffff);
	printf("%c[01;0m", '\033');	 
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << endl;
   //
   cout << "Selected 1 (tx vs. rx)   tx ----> " << endl;
   for (j=0;j<maxTimeBins;j++){
     cout << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "Selected 2 (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     cout << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected2[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "Selected 3 (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     cout << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected3[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "Result (tx vs. rx)   tx ----> " << endl;
   cout << "           00 01 02 03 04 05 06 07 08 09 10 11 12" << endl;
   cout << "           == == == == == == == == == == == == ==" << endl; 
   //
   // Result
   //
  
   for (j=0;j<maxTimeBins;j++){
     printf(" rx = %02d : ",j);   
     for (k=0;k<maxTimeBins;k++) {
       if ( ALCTConfDone[j][k] > 0 ) {
	 if ( ALCTWordCount[j][k] >0 ) printf("%c[01;35m", '\033');	 
	 printf("%02x ",(ALCTWordCount[j][k]&0xffff));
	 printf("%c[01;0m", '\033');
	 rxtx_timing[j][k]=ALCTWordCount[j][k];
       } else {
	 printf("%02x ",0x00 );
	 rxtx_timing[j][k]=0;
       }
     }
     cout << endl;
   }
   //
   ALCT_phase_analysis(rxtx_timing);

   //
   cout << endl ;
   //
   // Work it
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       ALCTWordCountWrap[j][k] = 0 ;
     }
   }   
   //
   // Copy old data
   // 
   for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       if ( ALCTConfDone[j][k] > 0 ) {
	 ALCTWordCountWrap[j][k] = ALCTWordCount[j][k] ;
       }
     }
   }
   int nloop = 0;
   //
 LOOP:
   //
   // 1dim Projection
   //
   for (j=0;j<maxTimeBins*2;j++){
     ProjectionX[j] = 0;
     ProjectionY[j] = 0;
   }
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] > 0 ) ProjectionY[j]++;
     }
   }
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[k][j] > 0 ) ProjectionX[j]++;
     }
   }
   //
   cout << "ProjectionX " << endl ;
   for (j=0;j<maxTimeBins*2;j++) printf("%02d ",ProjectionX[j]) ;
   printf("\n");
   cout << "ProjectionY " << endl ;
   for (j=0;j<maxTimeBins*2;j++) printf("%02d ",ProjectionY[j]) ;
   printf("\n");
   printf("\n");
   //
   // Kill bad regions
   //
   for (j=0;j<maxTimeBins*2;j++) {
     if (ProjectionX[j] > 11 ) {
       for (k=0;k<maxTimeBins;k++) ALCTWordCountWrap[k][j] = 0;
     }
   }
   //
   // Now do magic
   //
   j = 0;
   while (ProjectionY[j] > 0 ) {
     for (k=0;k<maxTimeBins;k++) {
       ALCTWordCountWrap[j+13][k] = ALCTWordCountWrap[j][k] ;
       ALCTWordCountWrap[j][k]    = 0 ;
     }
     j++;
   }
   //
   j = 0;
   while (ProjectionX[j] > 0 ) {
     for ( k=0; k<maxTimeBins; k++) {
       ALCTWordCountWrap[k][j+13] = ALCTWordCountWrap[k][j] ;
       ALCTWordCountWrap[k][j] = 0 ;
     }
     j++;
   }
   //
   for (int j=0; j<maxTimeBins*2; j++ ){
     printf(" rx = %02d : ",j);   
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] >0 ) printf("%c[01;35m", '\033');	 
       printf("%02x ",(ALCTWordCountWrap[j][k]&0xffff));
       printf("%c[01;0m", '\033');	 
     }
     cout << endl;
   }
   //
   cout << endl;
   //
   nloop++ ;
   //
   //if ( nloop < 2 ) goto LOOP;
   //
   // End Result
   //
   float meanX  = 0;
   float meanXn = 0;
   float meanY  = 0;
   float meanYn = 0;
   //   
   for (int j=0; j<maxTimeBins*2; j++ ){
     printf(" rx = %02d : ",j);   
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] >0 ) printf("%c[01;35m", '\033');	 
       printf("%02x ",(ALCTWordCountWrap[j][k]&0xffff));
       printf("%c[01;0m", '\033');	 
       if ( ALCTWordCountWrap[j][k] >0 ) {
	 meanX += k;
	 meanXn++;
	 meanY += j;
	 meanYn++;
       }       
     }
     cout << endl;
   }
   //
   // Calculate mean
   // 
   meanX /= meanXn+0.0001;
   meanY /= meanYn+0.0001;
   //
   printf(" \n Best Setting TX=%f RX=%f \n",int(meanX)%13 ,int(meanY)%13);
   //
   cout << endl;
   //
   TXphase = int(meanX)%13 ;
   RXphase = int(meanY)%13 ;
   //
}
//
int FindBestL1aAlct(){
  //
  // Now find the best L1a_delay value for the ALCT
  //
  thisTMB->SetALCTPatternTrigger();
  //
  unsigned long HCmask[22];
  /*
  int keyWG = 16;
  for (int i=0; i<12; i++) {
    if ( i%2 == 0 ) {
      if (keyWG<32) {
	HCmask[i] = 0x1<<keyWG;
      } else {
	HCmask[i] = 0x1<<(keyWG-32);
      }
    } else {
      if (keyWG<32) {
	HCmask[i] = 0x1<<(keyWG) ;
      } else {
	HCmask[i] = 0x1<<(keyWG-32);
      }
    }
  }
  */
  /*
  for (int i=0; i<12; i++ ) {
    if (i == 0 ) cout << "Layer 0 " ;
    for(int j=0; j<32; j++ ) 
      if (((HCmask[i]>>j)&0x1) == 0 ) {
	cout <<"-";
      } else {
	printf("%c[01;44m", '\033');	 
	cout <<"|";
	printf("%c[0m", '\033'); 
      }
    if ((i+1)%2 == 0 ) {
      cout << endl;
      if (i+1<12) cout << "Layer " << (i+1)/2 << " ";
    }
  }
  cout << endl;
  */
  //
  int WordCount[200];
  for (int i=0; i<200; i++) WordCount[i] = 0;
  //
  //unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default conf register
  //
  unsigned cr[3];
  //
  alct->GetConf(cr,1);
  //
  int minlimit = 0;
  int maxlimit = 100;
  //
  for (int l1a=minlimit; l1a<maxlimit; l1a++) {
    //
    usleep(100);
    //
    //while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
    //
    int keyWG          = int((rand()/(RAND_MAX+0.01))*(alct->GetWGNumber())/6./2.);
    int ChamberSection = alct->GetWGNumber()/6;
    //
    printf("\n");
    printf("-----> Injecting at %d \n",keyWG);
    //
    for (int i=0; i<22; i++) HCmask[i] = 0;
    //
    bitset<672> bits(*HCmask) ;
    //
    for (int i=0;i<672;i++){
      if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
      //if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
    }
    //
    bitset<32> Convert;
    //
    Convert.reset();
    //
    for (int ii=0;ii<(alct->GetWGNumber());ii++){
      if ( bits.test(ii) ) Convert.set(ii%32);
      if ( ii%32 == 31 ) {
	HCmask[ii/32] = Convert.to_ulong();
	Convert.reset();
      }
    }
    //
    alct->alct_write_hcmask(HCmask);
    alct->alct_read_hcmask(HCmask);
    //
    cr[1] = (cr[1] & 0xfffff00f) | ((l1a&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    thisTMB->ResetALCTRAMAddress();
    PulseTestStrips();
    thisTMB->DecodeALCT();
    thisTMB->GetCounters();
    thisTMB->PrintCounters(3);
    WordCount[l1a] = thisTMB->GetALCTWordCount();
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  }
  //
  for (int i=minlimit; i<maxlimit; i++){
    printf(" Value = %d WordCount = %x \n",i,WordCount[i]);
  }
  //
  float DelayBin  = 0;
  int   DelayBinN = 0;
  int   flag      = 0;
  //
  for (int i=maxlimit; i>minlimit; i--){
    //
    if (flag == 1 && WordCount[i] == 0 ) break;
    //
    if ( WordCount[i]>0 ) {
      //
      //printf("Including %d \n",i);
      //
      flag = 1;
      DelayBin  += i ;
      DelayBinN ++;
    }
    //
  }
  //
  printf(" DelayBin=%f DelaybinN=%d \n",DelayBin,DelayBinN);
  //
  DelayBin /= (DelayBinN+0.0001) ;
  //
  return int(DelayBin);
  //
}
//
int FindALCT_L1A_delay(int minlimit, int maxlimit){
  //
  unsigned cr[3];
  //
  alct->GetConf(cr,1);
  //
  int WordCount[200];
  for (int i=0; i<200; i++) WordCount[i] = 0;
  //
  for (int l1a=minlimit; l1a<maxlimit+1; l1a++) {
    //
    cr[1] = (cr[1] & 0xfffff00f) | ((l1a&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    thisTMB->ResetCounters();
    thisTMB->ResetALCTRAMAddress();
    ::sleep(6);
    thisTMB->GetCounters();
    //
    cout << endl;
    printf("L1a ALCT delay %d : \n",l1a);
    //
    thisTMB->PrintCounters(3);
    thisTMB->DecodeALCT();
    WordCount[l1a] = thisTMB->GetALCTWordCount();
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
    //
  }
  float DelayBin  = 0;
  int   DelayBinN = 0;
  //
  for (int i=maxlimit; i>minlimit; i--){
    //
    if ( WordCount[i]>0 ) {
      DelayBin  += i ;
      DelayBinN ++;
    }
    //
  }
  //
  printf(" DelayBin=%f DelaybinN=%d \n",DelayBin,DelayBinN);
  //
  DelayBin /= (DelayBinN+0.0001) ;
  //
  printf("In.Best L1a ALCT delay %f \n",DelayBin);
  //
  return int(DelayBin);
  //
}


void PulseRandomALCT(){
  //
  unsigned long HCmask[22];
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //
  int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6/4);
  int keyWG2 = (alct->GetWGNumber())/6-keyWG;
  int ChamberSection = alct->GetWGNumber()/6;
  //
  printf("Injecting at %d and %d\n",keyWG,keyWG2);
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //
  bitset<672> bits(*HCmask) ;
  //
  for (int i=0;i<672;i++){
    if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
    if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
  }
  //
  bitset<32> Convert;
  //
  Convert.reset();
  //
  for (int i=0;i<(alct->GetWGNumber());i++){
    if ( bits.test(i) ) Convert.set(i%32);
     if ( i%32 == 31 ) {
       HCmask[i/32] = Convert.to_ulong();
       Convert.reset();
     }
  }
  //
  alct->alct_write_hcmask(HCmask);
  alct->alct_read_hcmask(HCmask);
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips();
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
}	      

//
void PulseTestStrips(){
  //
   int slot = thisTMB->slot();
   int TMBtime(1);
   //
   if ( alct ) {
      //
      //thisTMB->ResetALCTRAMAddress();
     //
     //thisTMB->SetCLCTPatternTrigger();
     //
     //unsigned cr[3]  = {0x80fc5fc0, 0x20a0f786, 0x8}; // Configuration for this test L1a_delay=120 L1a_window=0xf
     //
     //alct->SetConf(cr,1);
     //alct->unpackControlRegister(cr);
     //
      long int StripMask = 0x3f;
      long int PowerUp   = 1 ;
      long int Amplitude = 0x3f;
      //
      thisTMB->DisableCLCTInputs();
      //
      if ( beginning == 0 ) {
	//
	printf("Init \n");
	//
	//
	alct->alct_set_test_pulse_amp(&slot,Amplitude);
	//
	alct->alct_read_test_pulse_stripmask(&slot,&StripMask);
	cout << " StripMask = " << hex << StripMask << endl;
	//
	//old alct->alct_set_test_pulse_stripmask(&slot,0x3f);
	//old alct->alct_set_test_pulse_groupmask(&slot,0xff);
	//
	alct->alct_set_test_pulse_stripmask(&slot,0xff);
	alct->alct_set_test_pulse_groupmask(&slot,0x00);
	//
	alct->alct_read_test_pulse_stripmask(&slot,&StripMask);
	cout << " StripMask = " << hex << StripMask << endl;
	//
	alct->alct_read_test_pulse_powerup(&slot,&PowerUp);
	cout << " PowerUp   = " << hex << PowerUp << dec << endl; //11July05 DM added dec
	//
	alct->alct_fire_test_pulse('A');
	//
	alct->alct_set_test_pulse_powerup(&slot,1);
	//
	beginning = 1;
	//
	PulseTestStrips();
	//
      } else {
	//
	//alct->alct_set_test_pulse_powerup(&slot,PowerUp);
	//alct->alct_set_test_pulse_powerup(&slot,0);
	//
	thisCCB->setCCBMode(CCB::VMEFPGA);
	thisCCB->WriteRegister(0x28,0x7862);  //4Aug05 DM changed 0x789b to 0x7862
	                                      //July05 changed 0x7878 to 0x789b
	//thisCCB->WriteRegister(0x20,0x1e71);
	//
	//                                      
	//
	//cout <<"Enter 78 then l1a delay time (in hex)" <<  endl;
	//cin >> hex >>  TMBtime;
	//thisCCB->WriteRegister(0x28,TMBtime);      //5July05 DM allows you to write in CCB reg value
	                                           //in option 12, but since option 19 calls this also.... 
	//cout <<"TMBtime is " << TMBtime << dec << endl;
	//
	thisCCB->ReadRegister(0x28);
	//
	//thisCCB->WriteRegister(0x20,0x01);
	//
	thisCCB->GenerateAlctAdbASync();	 
	//thisCCB->setCCBMode(CCB::DLOG);  
	//
	}
      //
   } else {
      cout << " No ALCT " << endl;
   }  
   //
   //thisCCB->DumpAddress(0x20);
   //
}

void PulseCFEB(fstream* output_log, int HalfStrip, int CLCTInputs ){
   //
   thisTMB->DisableCLCTInputs();
   thisTMB->SetCLCTPatternTrigger();
   //
   thisDMB->set_comp_thresh(0.3);
   thisDMB->set_dac(.5,0);
   //
   if (HalfStrip == -1 ) HalfStrip = int(rand()*31./(RAND_MAX+1.0));
   //
   cout << endl;
   cout << " -- Injecting in " << HalfStrip << endl;
   cout << endl;
   //
   thisTMB->DiStripHCMask(HalfStrip/4-1); // counting from 0;
   //
   int hp[6] = {HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip};       
   //
   // Set the pattern
   //
   thisDMB->trigsetx(hp);
   //
   thisTMB->EnableCLCTInputs(CLCTInputs);
   //
   // Inject it (pulse the CFEBs)
   //
   thisDMB->inject(1,0x4f);
   //
   // Decode the TMB CLCTs (0 and 1)
   //
   thisTMB->DecodeCLCT();
   //
   cout << endl ;
   //
}

void CFEBChamberScanning(){
   //
   int MaxStrip = 32;
   int Muons[5][MaxStrip];
   int MuonsMaxHits[5][MaxStrip];
   //
   for (int i=0;i<5;i++) 
      for (int j=0; j<MaxStrip; j++) {
	 Muons[i][j] = 0;
	 MuonsMaxHits[i][j] = 0;
      }
   //
   int CLCTInputList[3] = {0x1,0xa,0x14};
   //
   for (int List=0; List<3; List++){
     //
     //
     // Number of pulses = 10
     //
     for (int Nmuons=0; Nmuons<10; Nmuons++){
       //
       for (int Strip=0; Strip<MaxStrip; Strip++) {
	 //
	 printf("Enabling Inputs %x \n",CLCTInputList[List]);
	 //
	 PulseCFEB(0, Strip,CLCTInputList[List]);
	 //
	 int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	 int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	 int clct0nhit = thisTMB->GetCLCT0Nhit();
	 int clct1nhit = thisTMB->GetCLCT1Nhit();
	 int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	 int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	 //
	 cout << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	 cout << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	 cout << " clct0keyHalfStrip " << clct0keyHalfStrip << " clct1keyHalfStrip " << clct1keyHalfStrip << endl;
	 //
	 if ( clct0keyHalfStrip == Strip && MuonsMaxHits[clct0cfeb][Strip] < clct0nhit ) 
	       MuonsMaxHits[clct0cfeb][Strip] = clct0nhit ;
	 if ( clct1keyHalfStrip == Strip && MuonsMaxHits[clct1cfeb][Strip] < clct1nhit ) 
	   MuonsMaxHits[clct1cfeb][Strip] = clct1nhit ;
	 //
	 if ( clct0nhit == 6 && clct0keyHalfStrip == Strip ) Muons[clct0cfeb][Strip]++;
	 if ( clct1nhit == 6 && clct1keyHalfStrip == Strip ) Muons[clct1cfeb][Strip]++;
	 //
	 cout << endl ;
	 //
	 }
     }      
   }
   //
   cout << endl;
   cout << " Number of Muons seen " << endl;
   //
   for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      cout << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
	 cout << setw(3) << Muons[CFEBs][HalfStrip] ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << " Maximum number of hits " << endl;
   //
   for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      cout << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
	 cout << setw(3) << MuonsMaxHits[CFEBs][HalfStrip] ;
      }
      cout << endl;
   }
}
//
int TMBL1aTiming(){
  //
  int wordcounts[200];
  int nmuons = 1;
  //
  for (int delay=0;delay<200;delay++) wordcounts[delay] = 0;
  //
  int minlimit = 100;
  int maxlimit = 160;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;
  //
  for( int delay=minlimit; delay<maxlimit; delay++){
    //
    printf("delay %d\n",delay);
    //
    thisTMB->lvl1_delay(delay);
    //
    for (int Nmuons=0; Nmuons<nmuons; Nmuons++){
      while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
      thisTMB->ResetRAMAddress();
      if ( UseCosmic ) ::sleep(2);
      if ( UsePulsing) PulseCFEB(0,16,0xa);
      wordcounts[delay] += thisTMB->GetWordCount();
      printf(" WordCount %d \n",thisTMB->GetWordCount());
    }
  }
  //
  for (int delay=minlimit;delay<maxlimit;delay++){
    if ( wordcounts[delay] > 0 ) {
      RightTimeBin += delay ;
      DataCounter++ ;
    }
    printf("delay = %d wordcount = %d wordcount/nmuons %d \n",delay,wordcounts[delay],wordcounts[delay]/(nmuons));
  }
  //
  RightTimeBin /= float(DataCounter) ;
  //
  printf("Right L1a delay setting is %f \n",RightTimeBin);
  //
  return int(RightTimeBin) ;
  //
}
//
void CFEBTiming(float CFEBMean[5]){
  //
  int MaxTimeDelay=13;
  //
  int Muons[5][MaxTimeDelay];
  int MuonsWork[5][2*MaxTimeDelay];
  //
  for(int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      Muons[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for(int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      MuonsWork[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){
    //
    std::cout << " Setting TimeDelay to " << TimeDelay << endl;
    //
    thisTMB->tmb_clk_delays(TimeDelay,0) ;
    thisTMB->tmb_clk_delays(TimeDelay,1) ;
    thisTMB->tmb_clk_delays(TimeDelay,2) ;
    thisTMB->tmb_clk_delays(TimeDelay,3) ;
    thisTMB->tmb_clk_delays(TimeDelay,4) ;
    //
    int CLCTInputList[3] = {0x1,0xa,0x14};
    //
    for (int List=0; List<3; List++){
      //
      for (int Nmuons=0; Nmuons<10; Nmuons++){
	//
	PulseCFEB(0, 16,CLCTInputList[List]);
	//
        thisDMB->readtimingCounter();
	//
        thisDMB->readtimingScope();
	//
        printf("  L1A to LCT delay: %d", thisDMB->GetL1aLctCounter()  ); printf(" CMS clock cycles \n");
        printf("  CFEB DAV delay:   %d", thisDMB->GetCfebDavCounter() ); printf(" CMS clock cycles \n");
        printf("  TMB DAV delay:    %d", thisDMB->GetTmbDavCounter()  ); printf(" CMS clock cycles \n");
        printf("  ALCT DAV delay:   %d", thisDMB->GetAlctDavCounter() ); printf(" CMS clock cycles \n");
	//
	cout << endl ;
	//
        cout << "  L1A to LCT Scope: " ;
	cout << setw(3) << thisDMB->GetL1aLctScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetL1aLctScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  CFEB DAV Scope:   " ;
	cout << setw(3) << thisDMB->GetCfebDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetCfebDavScope()>>i)&0x1) ;
	cout << endl ;
	//
	cout << "  TMB DAV Scope:    " ;
	cout << setw(3) << thisDMB->GetTmbDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetTmbDavScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  ALCT DAV Scope:   " ;
	cout << setw(3) << thisDMB->GetAlctDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetAlctDavScope()>>i)&0x1) ;
	cout << endl ;
	//
        cout << "  Active DAV Scope: " ;
	cout << setw(3) << thisDMB->GetActiveDavScope() << " " ;
	for( int i=4; i>-1; i--) cout << ((thisDMB->GetActiveDavScope()>>i)&0x1) ;
	cout << endl ;
//
	thisTMB->DiStripHCMask(16/4-1); // counting from 0;
	//
	std::cout << " TimeDelay " << TimeDelay << " CLCTInput " 
	     << CLCTInputList[List] << " Nmuons " << Nmuons << endl;
	//
	int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	int clct0nhit = thisTMB->GetCLCT0Nhit();
	int clct1nhit = thisTMB->GetCLCT1Nhit();
	int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	//
	std::cout << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	std::cout << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	//
	if ( clct0nhit == 6 && clct0keyHalfStrip == 16 ) Muons[clct0cfeb][TimeDelay]++;
	if ( clct1nhit == 6 && clct1keyHalfStrip == 16 ) Muons[clct1cfeb][TimeDelay]++;
	//
      }
    }
  }
  //
  float CFEBMeanN[5];
  //
  for( int i=0; i<5; i++) {
    CFEBMean[i]  = 0 ;
    CFEBMeanN[i] = 0 ;
  }
  //
  std::cout << endl;
  std::cout << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) std::cout << setw(5) << TimeDelay ;
  std::cout << endl ;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    std::cout << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      std::cout << setw(5) << Muons[CFEBs][TimeDelay] ;
    }     
    std::cout << endl ;
  }   
  //
  std::cout << endl ;
  //
  int TimeDelay;
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      MuonsWork[CFEBs][TimeDelay] = Muons[CFEBs][TimeDelay] ;
    }
  }
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    TimeDelay=0;
    while (Muons[CFEBs][TimeDelay]>0) {
      MuonsWork[CFEBs][TimeDelay+13] = Muons[CFEBs][TimeDelay] ;
      MuonsWork[CFEBs][TimeDelay] = 0 ;
      TimeDelay++;
    }
  }
  //
  std::cout << endl ;
  // 
  std::cout << "TimeDelay Fixed for Delay Wrapping " << endl ;
  std::cout << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) std::cout << setw(5) << TimeDelay ;
  std::cout << endl;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    std::cout << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
      if ( MuonsWork[CFEBs][TimeDelay] > 0  ) {
	CFEBMean[CFEBs]  += TimeDelay  ; 
	CFEBMeanN[CFEBs] += 1 ; 
      }
      std::cout << setw(5) << MuonsWork[CFEBs][TimeDelay] ;
    }     
    std::cout << endl ;
  }   
  //
  std::cout << endl ;
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) {
    CFEBMean[CFEBs] /= CFEBMeanN[CFEBs]+0.0001 ;
    if (CFEBMean[CFEBs] > 12 ) CFEBMean[CFEBs] = (CFEBMean[CFEBs]) - 13 ;
    std::cout << " CFEB = " << CFEBMean[CFEBs] ;
  }
  //
  std::cout << endl ;
  std::cout << endl ;
  //
}
//
int FindWinner(int npulses=10){
  //
  if ( ! thisMPC ) {
    cout << " No MPC defined in XML file " << endl ;
    return -1 ;
  }
  //
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  // thisTMB->alct_match_window_size_ = 3;
  // thisTMB->alct_vpf_delay_ = 3;
  //
  float MpcDelay=0;
  int   MpcDelayN=0;
  //
  float Mpc0Delay=0;
  int   Mpc0DelayN=0;
  //
  float Mpc1Delay=0;
  int   Mpc1DelayN=0;
  //
  int   DelaySize = 15;
  //
  int   MPC0Count[DelaySize];
  int   MPC1Count[DelaySize];
  //
  for (int i=0; i<DelaySize; i++ ) {
    MPC0Count[i] = 0;
    MPC1Count[i] = 0;
  }
  //
  for (int i = 0; i < DelaySize; i++){
    //
    //thisTMB->alct_match_window_size_ = i;
    //
    thisTMB->mpc_delay(i);
    //
    //thisTMB->trgmode(1);
    //
    thisTMB->ResetCounters();
    //
    //thisCCB->startTrigger();
    //thisCCB->bx0();
    //
    int iterations = 0;
    //
  REPEAT:
    //
    thisMPC->SoftReset();
    thisMPC->init();
    //
    if (UsePulsing) {
      iterations++;
      PulseCFEB(0,-1,0xa);
    }

    cout << "Interactions " << iterations << endl ;

    if (UseCosmic)  ::sleep(2);
    //
    //if ( UsePulsing ) PulseCFEB(0,16,0xa);
    //
    //thisCCB->stopTrigger();
    //
    thisTMB->DataSendMPC();
    //
    if (UseCosmic) thisTMB->GetCounters();
    //
    //thisTMB->PrintCounters();
    //
    cout << "mpc_delay_ =  " << dec << i << endl;
    //
    //cout << thisTMB->MPC0Accept() << " " << thisTMB->MPC1Accept() << endl ;
    //
    if (UseCosmic) {
      thisTMB->PrintCounters(8);
      thisTMB->PrintCounters(16);
      thisTMB->PrintCounters(17);
    }
    //
    if (thisTMB->GetCounter(16) || 
	(thisTMB->MPC0Accept()+thisTMB->MPC1Accept()) > 0 ) {
      MpcDelay  += i ;    
      MpcDelayN++;
      if ( thisTMB->MPC0Accept() > 0 ) {
	Mpc0Delay  += i ;    
	Mpc0DelayN++;
	MPC0Count[i]++ ;
      }
      if ( thisTMB->MPC1Accept() > 0 ) {
	Mpc1Delay  += i ;    
	Mpc1DelayN++;
	MPC1Count[i]++ ;
      }
    }
    //
    thisMPC->firmwareVersion();
    //
    //thisMPC->read_fifosA();
    //
    cout << endl;
    //
    thisMPC->read_fifos();
    //
    cout << endl;
    //
    thisMPC->read_csr0();
    //
    if (UsePulsing && iterations < npulses ) goto REPEAT;
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC0 winner delay=" << setw(3) << i << " gives " << MPC0Count[i] << endl;
  }
  //
  cout << endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC1 winner delay=" << setw(3) << i << " gives " << MPC1Count[i] << endl;
  }
  //
  cout << endl ;
  //
  MpcDelay  /= (MpcDelayN  + 0.0001) ;
  Mpc0Delay /= (Mpc0DelayN + 0.0001) ;
  Mpc1Delay /= (Mpc1DelayN + 0.0001) ;
  //
  cout << "Correct MPC  setting  : " << MpcDelay << endl ;
  cout << "Correct MPC0 setting  : " << Mpc0Delay << endl ;
  cout << "Correct MPC1 setting  : " << Mpc1Delay << endl ;
  //
  cout << endl ;
  //
}
//
int FindALCTvpf(){
  //
  // thisCCB->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  // thisTMB->alct_match_window_size_ = 3;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;
  //
  int MaxTimeBin   = 8;
  //
  for (int i = 0; i < MaxTimeBin; i++){
    //
    cout << endl << "ALCT_vpf_delay=" << i << endl;
    //
    thisTMB->alct_vpf_delay(i);    // loop over this
    //thisTMB->trgmode(1);         // 
    thisTMB->ResetCounters();      // reset counters
    //thisCCB->startTrigger();     // 2 commands to get trigger going
    //thisCCB->bx0();
    ::sleep(2);                      // accumulate statistics
    //if (UsePulsing) PulseCFEB(0,16,0xa);
    //thisCCB->stopTrigger();      // stop trigger
    thisTMB->GetCounters();        // read counter values
    //
    //thisTMB->PrintCounters();    // display them to screen
    //
    thisTMB->PrintCounters(8);
    thisTMB->PrintCounters(10);
    //
    if ( thisTMB->GetCounter(10) > 0 ) {
      RightTimeBin += i ;
      DataCounter++;
    }
    //
  }
  
  RightTimeBin /= float(DataCounter) ;
  
  printf("Best Setting is %f \n",RightTimeBin);
  
  printf("\n");
     
  return int(RightTimeBin) ;

}


int FindTMB_L1A_delay( int idelay_min, int idelay_max ){
  //
  //bool useCCB = false; // if using TTC for L1A and start trig, bc0, set to false
  //cout << "Value of useCCB is" << useCCB <<endl;
  //
  //if (useCCB) thisCCB->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  //     thisTMB->alct_match_window_size_ = 3;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;

  for (int i = idelay_min; i < idelay_max+1; i++){
    
    //thisTMB->l1adelay_ = i;// loop over this
    //
    thisTMB->lvl1_delay(i);
    //
    // thisTMB->trgmode(1);         // 
    //
    thisTMB->ResetCounters();    // reset counters
    //if (useCCB) thisCCB->startTrigger();     // 2 commands to get trigger going
    //if (useCCB) thisCCB->bx0();
    cout << endl << "TMB_l1adelay=" << i << ":" << endl;
    ::sleep(3);                   // accumulate statistics
    //if (useCCB) thisCCB->stopTrigger();      // stop trigger
    thisTMB->GetCounters();      // read counter values
    
    //thisTMB->PrintCounters(); // display them to screen
    thisTMB->PrintCounters(8);    // display them to screen
    thisTMB->PrintCounters(19);
    thisTMB->PrintCounters(20);

    if ( thisTMB->GetCounter(19) > 0) {
      RightTimeBin += i;
      DataCounter++;
    }

  }

  RightTimeBin /= float(DataCounter) ;
  
  printf("Right L1a delay setting is %f \n",RightTimeBin);

  printf("\n");

  //if (useCCB) thisCCB->setCCBMode(CCB::DLOG);      // return to "regular" mode for CCB

  return int(RightTimeBin) ;

}
//
/*
extern char cmd[];
extern char sndbuf[];
extern char rcvbuf[];

void settrgsrc(int dword)
      {
	cout << "Input DMB trigger source value in hex (0 turns off internal L1A and LCT)" << endl;
	cin >> hex >> dword >> dec;

	cmd[0]=VTX2_USR1;
	sndbuf[0]=37;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR2;
	sndbuf[0]=dword&0x0F; 
	thisDMB->devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR1;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_BYPASS;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
	
	printf("Cal_trg source are Set to %01x (Hex). \n",dword&0x0F);
      }
*/

void ALCT_phase_analysis (int rxtx_timing[13][13]) {
  int i, j, k, p;  
  //
  /* 
  cout << "The array you are working with is:" << endl;
  cout << endl;
	for (j = 0; j < 13; j++) {
		for (k = 0; k < 13; k++) {
		cout << rxtx_timing[j][k] << " ";
				}
			cout << endl;
		cout << endl;
		}
  */
  //
//now find best position by the non-zero number that is 
//farthest from zero in both directions

	int nmin_best, ntot_best;
	int nup, ndown, nleft, nright;
	int ntot, nmin;
	int best_element_row, best_element_col;
	nmin_best=0;
	ntot_best=0;
	for (j = 0; j < 13; j++) {           //loops over rows
		for (k = 0; k < 13; k++) {       //loops over columns
			
			if ( rxtx_timing[j][k] != 0) {        //find all non-zero matrix elements
//			  cout << "element (row column) " << j << k << " is  good "<< endl;
			  //
			  for (i=j+1;rxtx_timing[i%13][k]!=0 && i<j+13;i++) {   //scans all non-zero element below
				}
			  ndown = i-1-j;  //number of non-zero elements below
//			  cout << "ndown = " << ndown << "  ";
			  for (p=k+1;rxtx_timing[j][p%13]!=0 && p<k+13 ;p++) {  //scans all non-zero elements to the right
				}
			  nright = p-1-k;  //number of non-zero elements to the right
//			  cout << "nright = " << nright << "  ";		       
			  for (i=j-1;rxtx_timing[(13+i)%13][k]!=0 && i>j-13;i--) {   //scans all non-zero elements above
				}
			  nup = j-1-i;  //number of non-zero elements above
//			  cout << "nup = " << nup << "  ";
			  for (p=k-1;rxtx_timing[j][(13+p)%13]!=0 && p>k-13;p--) {   //scans all non-zero elements to the left
				}
			  nleft = k-1-p;  //number of non-zero elements to the left
//			  cout << "nleft = " << nleft << endl;
			  nmin = 100;
				int numbers[] = {nup, ndown, nleft, nright};

				for (int d = 0; d < 4; d++) {    // finds the minimum number of non-zero elements in any direction
					if (nmin > numbers[d]) {
						nmin = numbers [d];
					}
				}
				//
// 				cout << "nmin =  " << nmin << endl;
				ntot=ndown+nup+nright+nleft;         //finds the total number of non-zero elements in all directions
//				cout << "ntot =  " << ntot << endl;
//				cout << endl;
				
				if (nmin_best < nmin) {         //finds the array element with the best nmin and ntot value
				  nmin_best = nmin;
				  if (ntot_best < ntot) {
					ntot_best = ntot;
					}
				  best_element_row = j;
				  best_element_col = k;
//				  cout << "best element so far is " << best_element_row << best_element_col << endl;
//				  cout << endl;
				}
			}
		}
	}
	cout << endl;
	cout << "the best values of nmin and ntot are:  " << endl;
	cout << "nmin =  " << nmin_best << "  and ntot =  " << ntot_best << endl;
	cout << endl;
     	cout << "best element is: " << endl;
	cout << "rx =  " << best_element_row << "    tx =  " << best_element_col << endl;
	cout << endl;
}	
