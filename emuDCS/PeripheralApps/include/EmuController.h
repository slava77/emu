//-----------------------------------------------------------------------
// $Id: EmuController.h,v 1.3 2008/02/21 09:55:37 liu Exp $
// $Log: EmuController.h,v $
// Revision 1.3  2008/02/21 09:55:37  liu
// add fast config button etc.
//
// Revision 1.2  2008/02/05 15:13:18  rakness
// add missing method declaration
//
// Revision 1.1  2007/12/26 11:22:30  liu
// new parser and applications
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.2  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 1.1  2006/01/11 08:55:21  mey
// UPdate
//
// Revision 1.1  2005/12/20 14:38:07  mey
// Update
//
// Revision 2.2  2005/09/13 14:46:39  mey
// Get DMB crate id; and DCS
//
// Revision 2.1  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.7  2004/06/18 23:52:33  tfcvs
// Introduced code for DCS/PCcontrol crate sharing. (FG)
//  -code additions still commented out, awaiting validation-
//
// Revision 1.6  2004/06/01 09:52:07  tfcvs
//  enabled CCB in enable(), added CVS-stuff, cleaned up includes (FG)
//
//-----------------------------------------------------------------------
#ifndef EmuController_h
#define EmuController_h
#include <string>
#include <vector>

class Crate;
class EmuEndcap;

class EmuController {
public:
  EmuController();
  ~EmuController() {}

  void init();

  void configure(int c=0);

  void enable();

  void disable();

  void NotInDCS();

  inline void SetConfFile(std::string xmlFile) { xmlFile_ = xmlFile; }

  inline EmuEndcap * GetEmuEndcap() { return myEndcap; }

  inline std::vector<Crate*> crates() { return myCrates; }

protected:

private:
  std::string xmlFile_;
  EmuEndcap * myEndcap;
  std::vector<Crate*> myCrates;
};

#endif
