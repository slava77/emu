//-----------------------------------------------------------------------
// $Id: PeripheralCrateController.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: PeripheralCrateController.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef PeripheralCrateController_h
#define PeripheralCrateController_h
#include <vector>

class DAQMB;
class CCB;
class TMB;
class Crate;

class PeripheralCrateController {
public:
  PeripheralCrateController();
  ~PeripheralCrateController();

  /// lets you give the controller a list of
  /// which card slots you'd like to activate.
  /// default is all active, of course.
  void selectSlots(std::vector<int> & slots) {theSelectedSlots = slots;}
  void selectCrates(std::vector<int> & crates) {theSelectedCrates = crates;}

  /// returns the crates in the CrateSetup consistent with the selection list
  std::vector<Crate *> crates() const;

  /// returns the DAQMBs consistent with lists of selected slots
  std::vector<DAQMB *> daqmbs(Crate *) const;

  /// returns the TMBs consistent with lists of selected slots
  std::vector<TMB *> tmbs(Crate *) const;

private:
  std::vector<int> theSelectedSlots;
  std::vector<int> theSelectedCrates;
};

#endif

