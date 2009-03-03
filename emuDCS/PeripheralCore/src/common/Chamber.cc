//-----------------------------------------------------------------------
// $Id: Chamber.cc,v 3.8 2009/03/03 14:34:48 liu Exp $
// $Log: Chamber.cc,v $
// Revision 3.8  2009/03/03 14:34:48  liu
// fix problem_mask for TStore
//
// Revision 3.7  2009/02/04 12:53:21  rakness
// update configuration and firmware checking
//
// Revision 3.6  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.5  2008/08/08 15:36:58  liu
// initialize problem_mask
//
// Revision 3.4  2007/12/27 00:33:53  liu
// update
//
// Revision 3.3  2006/11/28 14:17:16  mey
// UPdate
//
// Revision 3.2  2006/11/27 15:06:05  mey
// Made chamber(crate)
//
// Revision 3.1  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.3  2006/03/24 14:35:04  mey
// Update
//
// Revision 1.2  2006/03/23 08:24:58  mey
// Update
//
// Revision 1.1  2006/01/19 10:09:56  mey
// UPdate
//
// Revision 1.5  2006/01/18 12:46:48  mey
// Update
//
// Revision 1.4  2006/01/16 20:29:06  mey
// Update
//
// Revision 1.3  2006/01/12 22:36:27  mey
// UPdate
//
// Revision 1.2  2006/01/12 11:32:43  mey
// Update
//
// Revision 1.1  2006/01/11 08:54:15  mey
// Update
//
// Revision 1.5  2005/12/16 17:49:39  mey
// Update
//
// Revision 1.4  2005/12/14 08:32:36  mey
// Update
//
// Revision 1.3  2005/12/06 13:30:10  mey
// Update
//
// Revision 1.2  2005/11/30 14:58:02  mey
// Update tests
//
// Revision 1.1  2005/10/28 13:09:04  mey
// Timing class
//
//-------------------------------------------
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
//
#include "Chamber.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "CFEB.h"
#include "Crate.h"
//

namespace emu {
  namespace pc {

//
Chamber::Chamber(Crate * csc):
  crate_(csc), label_("new_csc"), problem_mask_(0)
{
  expected_config_problem_alct_  = 0;
  expected_config_problem_tmb_   = 0;
  expected_config_problem_cfeb1_ = 0;
  expected_config_problem_cfeb2_ = 0;
  expected_config_problem_cfeb3_ = 0;
  expected_config_problem_cfeb4_ = 0;
  expected_config_problem_cfeb5_ = 0;
  expected_config_problem_dmb_   = 0;
  //
  csc->AddChamber(this);
}
//
Chamber::~Chamber(){
  //
  //
}
//
  void Chamber::SetTMB(TMB* myTMB)
  {  thisTMB = myTMB; 
     alct = myTMB->alctController();
  }

    void Chamber::SetProblemMask(int problem_mask) {
      //
      problem_mask_ = problem_mask;
      expected_config_problem_alct_  = (problem_mask >> 0) & 0x01;
      expected_config_problem_tmb_   = (problem_mask >> 1) & 0x01;
      expected_config_problem_cfeb1_ = (problem_mask >> 2) & 0x01;
      expected_config_problem_cfeb2_ = (problem_mask >> 3) & 0x01;
      expected_config_problem_cfeb3_ = (problem_mask >> 4) & 0x01;
      expected_config_problem_cfeb4_ = (problem_mask >> 5) & 0x01;
      expected_config_problem_cfeb5_ = (problem_mask >> 6) & 0x01;
      expected_config_problem_dmb_   = (problem_mask >> 7) & 0x01;
      //
      return;
    }

  } // namespace emu::pc
  } // namespace emu
