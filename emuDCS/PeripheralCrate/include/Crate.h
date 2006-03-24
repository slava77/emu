//-----------------------------------------------------------------------
// $Id: Crate.h,v 2.5 2006/03/24 14:35:03 mey Exp $
// $Log: Crate.h,v $
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
class CCB;
class MPC;
class DDU;
class ChamberUtilities;
class Chamber;

class Crate {
public:
  Crate(int number, VMEController * controller);
  ~Crate();

  int number() const {return theNumber;}

  void enable();
  void disable();
  void configure();
  void init();
  void addModule(VMEModule * module);
  void AddChamber(Chamber * chamber);

  VMEController * vmeController() const {return theController;}

  /// uses RTTI to find types
  std::vector<DAQMB *> daqmbs() const;
  std::vector<TMB *> tmbs() const;
  std::vector<ChamberUtilities> chamberUtilsMatch() const;
  std::vector<Chamber*> chambers() const;
  std::vector<Chamber> chambersMatch() const;
  //
  CCB * ccb() const;
  MPC * mpc() const;
  DDU * ddu() const;
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

  int theNumber;
  /// indexed by slot 
  std::vector<VMEModule *> theModules;
  std::vector<Chamber *> theChambers;
  VMEController * theController;
};

#endif

