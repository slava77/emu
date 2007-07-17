//-----------------------------------------------------------------------
// $Id: Crate.h,v 3.6 2007/07/17 16:16:09 liu Exp $
// $Log: Crate.h,v $
// Revision 3.6  2007/07/17 16:16:09  liu
// remove ChamberUtilities dependence
//
// Revision 3.5  2006/11/28 14:17:15  mey
// UPdate
//
// Revision 3.4  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.3  2006/11/13 16:25:31  mey
// Update
//
// Revision 3.2  2006/10/30 15:53:39  mey
// Update
//
// Revision 3.1  2006/10/03 07:36:01  mey
// UPdate
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.6  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.5  2006/03/24 14:35:03  mey
// Update
//
// Revision 2.4  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.3  2006/01/18 12:46:18  mey
// Update
//
// Revision 2.2  2006/01/10 23:31:58  mey
// Update
//
// Revision 2.1  2005/12/20 23:39:13  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CRATE_h
#define CRATE_h
//
#include <vector>
#include <iostream>
//
class VMEModule;
class VMEController;
class DAQMB;
class TMB;
class ALCTController;
class CCB;
class MPC;
//class ChamberUtilities;
class Chamber;
class EmuSystem;

class Crate {
public:
  Crate(int, VMEController *, EmuSystem *);
  ~Crate();
  
  int CrateID() const {return theCrateID;}
  
  void enable();
  void disable();
  void configure();
  void init();
  void addModule(VMEModule * module);
  void AddChamber(Chamber * chamber);
  void DumpConfiguration();
  //
  std::string GetLabel() {return label_;}
  inline void SetLabel(std::string label ) {label_ = label;}
  //
  VMEController * vmeController() const {return theController;}

  /// uses RTTI to find types
  std::vector<DAQMB *> daqmbs() const;
  std::vector<TMB *> tmbs() const;
  std::vector<ALCTController *> alcts() const;
  //std::vector<ChamberUtilities> chamberUtilsMatch() const;
  std::vector<Chamber*> chambers() const;
  std::vector<Chamber> chambersMatch() const;
  //
  CCB * ccb() const;
  MPC * mpc() const;
  //
private:

  template<class T> T * findBoard() const
  {
    for(unsigned i = 0; i < theModules.size(); ++i) {
      T * result = dynamic_cast<T *>(theModules[i]);
      if(result != 0) return result;
    }
    return 0;
  }
  
  int theCrateID;
  std::string label_;
  /// indexed by slot 
  std::vector<VMEModule *> theModules;
  std::vector<Chamber *> theChambers;
  VMEController * theController;
};

#endif

