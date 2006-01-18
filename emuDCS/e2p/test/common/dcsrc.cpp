#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "TestBeamCrateController.h"
#include "TMB.h"
#include "ALCTController.h"
#include "MPC.h"
#include <unistd.h>
#include "PeripheralCrateParser.h"

#include "EmuDcs.h"  // corr++
#include "Rc2Dcs.h" 

int main(int argc, char **argv)
{

  string slow_control;
  string detector;
  int isOn;
  float value_to_set;
  int option_to_set;

  int ANY_VALUE;
  int ANY_NEGATIVE_VALUE=-1;
  int ret;

  Rc2Dcs *rc2dcs= new Rc2Dcs();  // corr++ 

  //==============================================================
  extern int	optind;
  extern char	*optarg;
  int c;
       string option;

  rc2dcs->slow_control=(char *)malloc(40);
  rc2dcs->detector=(char *)malloc(40);

   strcpy(rc2dcs->slow_control,"");
   strcpy(rc2dcs->detector,"all");
  // rc2dcs->detector=(char *)NULL;
  rc2dcs->isOn_detector=-1;
  rc2dcs->value_to_set=-1;
  rc2dcs->option_to_set=-1;

  while(1)
  {

    c = getopt(argc, argv, "?hs:d:t:v:");
      //          long_options, &option_index);
    if (c == -1)
    break;

    ////      printf("%d\n",c);

    switch (c)
    {
    case 'h':
    case '?':
      rc2dcs->help();
      /*
      cout << endl;
      cout << "==============================================" << endl;
      cout << "usage:" << endl;
      cout << "==============================================" << endl;
      cout << "dcsrc -s slow_control [-d detector] [-t isOn] [-v value_to_set] " << endl;
      cout << "==============================================" << endl;
      cout << "available 'slow_control' parameters:" << endl;
      cout << "'hv'    (High Voltage)"  << endl;
      cout << "'lv'    (Low Voltage of on-chamber boards)"  << endl;
      cout << "'temp'  (Temperature of on-chamber boards (read-only)) "  << endl;
      cout << "'mrtn'  (Low Voltage of Wiener Maraton Power Supplies)"  << endl;
      cout << "'crb'   (Low Voltage of Peripheral Crate boards)"  << endl;
      cout << "'wth'   (Weather station parameters at SX5 (read-only))"  << endl;
      cout << "'gas'   (Monitoring of Gas components at SX5 (read-only))"  << endl;
      cout << "'pt100' (Monitoring of Gas flows via Chambers(read-only))"  << endl;
      cout << "==============================================" << endl;
      cout << "examples of 'detector' parameters:"  << endl;
      cout << "no parameter  (all relevant detectors for the slow control:"  << endl;
      cout << "               it is applicable for any slow_control "  << endl;
      cout << "'sp3c02'      (a chamber example: StationPlus3Chamber02:"  << endl;
      cout << "               it is applicable for hv, lv, temp "  << endl;
      cout << "'sp3p1'       (a Peripheral Crate example: StationPlus3Peripheralcrate1:"  << endl;
      cout << "               it is applicable for crb "  << endl;
      cout << "'maraton02'   (a maraton example: Wiener Maraton PS # 02:"  << endl;
      cout << "               it is applicable for mrtn "  << endl;
      cout << "==============================================" << endl;
      cout << "examples:" << endl;
      cout << "dcsrc -s gas                    -> read GAS at SX5" << endl;
      cout << "dcsrc -s wth                    -> read Weather station parameters at SX5" << endl;
      cout << "dcsrc -s crb -d sp2p2           -> read LV at StationPlus2PeripheralCrate2" << endl;
      cout << "dcsrc -s crb -d sp2p2 -t 1      -> turn on LV at StationPlus2PeripheralCrate2" << endl;
      cout << "dcsrc -s hv -d all -t 1 -v 3600 -> turn on HV at all chambers of slice test and set V=3600" << endl;
      cout << "dcsrc -s hv -d all -t 0         -> turn off HV at all chambers of slice test" << endl;
      cout << "dcsrc -s hv -d sp3c01 -t 1      -> turn on HV at StationPlus3Chamber01" << endl;
      cout << "dcsrc -s hv -d sp3c01           -> read HV data at StationPlus3Chamber01" << endl;
      cout << "" << endl;
      exit(0);
      */
         break;
       case 's':
         strcpy(rc2dcs->slow_control,optarg);
         //printf("ind=%d opt=%s\n",option_index,optarg);
	 //         if(!strcmp(optarg,"wth"))ret=rc2dcs->readWTH();
         break;
       case 'd':
         strcpy(rc2dcs->detector,optarg);
         break;
       case 't':
         rc2dcs->isOn_detector=atoi(optarg);
         break;
       case 'v':
         rc2dcs->value_to_set=atof(optarg);
         option=string(optarg);
         if(option=="hr_all")rc2dcs->option_to_set=0;          //all
	 else if(option=="hr_alct")rc2dcs->option_to_set=1;     //alct
	 else if(option=="hr_dmb")rc2dcs->option_to_set=2;     //dmb
	 else if(option=="hr_tmb")rc2dcs->option_to_set=3;     //cfebs
	 else if(option=="hr_cfebs")rc2dcs->option_to_set=4;     //tmb
	 else if(option=="hr_mpc")rc2dcs->option_to_set=5;     //mpc
	 //  rc2dcs->option_to_set=atoi(optarg);       
         break;
      default:
        printf("default\n");
      break;

    } // switch (c)

  } // while(1)

  int found_wrong_params=0; 
        for (; optind < argc; optind++) {   // it is for free parameters (i.e. parameters without option (like -s))
	  printf("wrong parameter(parameter without option)=%s\n",argv[optind]);
          found_wrong_params=1;
	} // for (; optind < argc; optind++) {
	if(found_wrong_params)exit(0);

//==============================================================

  slow_control=string(rc2dcs->slow_control);
  detector=string(rc2dcs->detector);
  isOn=rc2dcs->isOn_detector;
  value_to_set=rc2dcs->value_to_set;
  option_to_set=rc2dcs->option_to_set;

  if(slow_control=="")rc2dcs->help();

//==============================================================
 
 if(slow_control=="hv"){
   if(isOn != -1){
    if(value_to_set < 0)ret=rc2dcs->turnHV(isOn,-1,(char *)detector.c_str());
    else ret=rc2dcs->turnHV(isOn,(int)value_to_set,(char *)detector.c_str());
   }
   else ret=rc2dcs->readHV((char *)detector.c_str());
 } //-------------------------------------------
 else  if(slow_control=="lv"){
   if(isOn != -1)ret=rc2dcs->turnLV(isOn,(char *)detector.c_str());
   else ret=rc2dcs->readLV((char *)detector.c_str());
 } //-------------------------------------------
 else  if(slow_control=="temp"){
   ret=rc2dcs->readTEMP((char *)detector.c_str());
 } //-------------------------------------------
 else  if(slow_control=="mrtn"){
   if(isOn != -1)ret=rc2dcs->turnMRTN(isOn,(char *)detector.c_str());
   else ret=rc2dcs->readMRTN((char *)detector.c_str());
 } //-------------------------------------------
 else  if(slow_control=="crb"){
   if(isOn != -1)ret=rc2dcs->turnCRB(isOn,(char *)detector.c_str());
   else ret=rc2dcs->readCRB((char *)detector.c_str());
 } //-------------------------------------------
 else  if(slow_control=="wth"){
   ret=rc2dcs->readWTH();
 } //-------------------------------------------
 else  if(slow_control=="gas"){
   ret=rc2dcs->readGAS();
 } //-------------------------------------------
 else  if(slow_control=="pt100"){
   ret=rc2dcs->readPT100();
 } //-------------------------------------------
 else  if(slow_control=="chip"){
   if(option_to_set==-1){
      printf("option must be set for 'chip' slow control, for example:\n");
      printf("dcsrc -s chip -v hr_all  \n");
      exit(0);
   }
     ret=rc2dcs->controlCHIP(option_to_set, (char *)detector.c_str());
 } //-------------------------------------------


//==============================================================

	//  Rc2Dcs *rc2dcs= new Rc2Dcs();  // corr++ 

//------------------------------------------------------------------------------------------------
//  rc2dcs->turnHV(1,1000);  //  1) to set HV = 1000 V on on slice test chambers
                             //  2) to turn HV on
//------------------------------------------------------------------------------------------------    
//  rc2dcs->turnHV(1,ANY_NEGATIVE_VALUE);   //  to turn HV on (without setting voltage value) on slice test chambers
//------------------------------------------------------------------------------------------------    
//  rc2dcs->turnHV(0,ANY_VALUE);   //  to turn HV off on slice test chambers
//------------------------------------------------------------------------------------------------     
//  rc2dcs->turnLV(1);  // to turn LV on  on slice test chambers
//------------------------------------------------------------------------------------------------     
//  rc2dcs->turnLV(0);  // to turn LV off  on slice test chambers
//------------------------------------------------------------------------------------------------    

//  rc2dcs->turnLV(0);   // corr++
  
//////rc2dcs->turnHV(1,3600);
////rc2dcs->turnMRTN(0,"maraton01");
///rc2dcs->turnCRB(1,"dp2p1");

////  ret=rc2dcs->turnHV(1,3600);

/////  ret=rc2dcs->readMRTN("maraton01");
/////  ret=rc2dcs->readCRB("dp2p1");

// ret=rc2dcs->readLV("dp2r1c14");
/////  ret=rc2dcs->turnLV(1,"sp2r1c14");

//// ret=rc2dcs->turnHV(1,-1,"sp2r1c01");

//// ret=rc2dcs->readHV("sp2r1c01");

//ret=rc2dcs->readTEMP("sp2r1c14");

//ret=rc2dcs->readGAS();

//ret=rc2dcs->readWTH();

//ret=rc2dcs->readPT100();

//=====================================================================================
// rc2dcs->errorProcessing(ret);


 return 0;

}

