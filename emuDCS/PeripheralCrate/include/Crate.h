//-----------------------------------------------------------------------
// $Id: Crate.h,v 2.1 2005/12/20 23:39:13 mey Exp $
// $Log: Crate.h,v $
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
#include <vector>
#include <iostream>

class VMEModule;
class VMEController;
class DAQMB;
class TMB;
class CCB;
class MPC;
class DDU;

class Crate {
public:
  Crate(int number, VMEController * controller);
  ~Crate();

  int number() const {return theNumber;}

  void enable();
  void disable();
  void configure();
  void addModule(VMEModule * module);

  VMEController * vmeController() const {return theController;}

  /// uses RTTI to find types
  std::vector<DAQMB *> daqmbs() const;
  std::vector<TMB *> tmbs() const;
  CCB * ccb() const;
  MPC * mpc() const;
  DDU * ddu() const;
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
  VMEController * theController;
};

#endif

