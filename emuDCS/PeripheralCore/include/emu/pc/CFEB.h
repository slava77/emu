//-----------------------------------------------------------------------
// $Id: CFEB.h,v 1.6 2012/11/28 03:14:04 liu Exp $
// $Log: CFEB.h,v $
// Revision 1.6  2012/11/28 03:14:04  liu
// add DCFEB fine delay parameter
//
// Revision 1.5  2012/11/26 21:10:02  liu
// add DCFEB pipeline_depth parameter
//
// Revision 1.4  2012/10/08 18:04:00  liu
// DCFEB update
//
// Revision 1.3  2012/09/05 22:34:46  liu
// introduce HardwareVersion attribute
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.1  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CFEB_h
#define CFEB_h
#include "emu/pc/JTAG_constants.h"
#include "emu/pc/BuckeyeChip.h"
#include <vector>

namespace emu {
  namespace pc {


class CFEB {
public:
  CFEB(int number, std::vector<BuckeyeChip> buckeyes);
  /// assumes all default Buckeyes are present
  explicit CFEB(int number);

  int number() const {return number_;}
  DEVTYPE scamDevice() const {return (DEVTYPE) (F1SCAM+number_);}
  DEVTYPE dscamDevice() const {return (DEVTYPE) (F1DCFEBM+number_);}
  DEVTYPE promDevice() const {return (DEVTYPE) (F1PROM+number_);}
  
  std::vector<BuckeyeChip> buckeyeChips() const {return buckeyeChips_;}
  char chipMask() const;
  //
  inline void SetHardwareVersion(int version) {hardware_version_ = version;}
  inline int GetHardwareVersion() {return hardware_version_;}
  inline void SetPipelineDepth(int value) {pipeline_depth_ = value;}
  inline int GetPipelineDepth() {return pipeline_depth_;}
  inline void SetFineDelay(int value) {fine_delay_ = value;}
  inline int GetFineDelay() {return fine_delay_;}

  int SelectorBit();
        
private:
  int number_;
  std::vector<BuckeyeChip> buckeyeChips_;
  int hardware_version_;     
  int pipeline_depth_;
  int fine_delay_;
};


  } // namespace emu::pc
  } // namespace emu
#endif

