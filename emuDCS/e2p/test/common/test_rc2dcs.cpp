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

  int ANY_VALUE;
  int ANY_NEGATIVE_VALUE=-1;

  Rc2Dcs *rc2dcs= new Rc2Dcs();  // corr++ 

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

  rc2dcs->turnLV(1);   // corr++
  
  return 0;
}


