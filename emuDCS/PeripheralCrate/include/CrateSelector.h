//-----------------------------------------------------------------------
// $Id: CrateSelector.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: CrateSelector.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CrateSelector_h
#define CrateSelector_h

#include <vector>
#include <string>


class DAQMB;
class CCB;
class TMB;
class Crate;

class CrateSelector {
public:
  CrateSelector();
  ~CrateSelector();

  /// lets you give the controller a list of
  /// which card slots you'd like to activate.
  /// default is all active, of course.
  void selectSlots(std::vector<int> & slots) {theSelectedSlots = slots;}
  void selectCrates(std::vector<int> & crates) {theSelectedCrates = crates;}
  /// -1 means all slots, or else just a single slot
  void setSlot(int slot);
  void setSlot(std::string strSlot);

  /// -1 means all crates, or else just a single crate
  void setCrate(int crate);
  void setCrate(std::string strCrate);

  /// returns the crates in the CrateSetup consistent with the selection list
  std::vector<Crate *> crates() const;

  /// returns the DAQMBs consistent with lists of selected slots
  std::vector<DAQMB *> daqmbs() const;
  std::vector<DAQMB *> daqmbs(Crate *) const;

  /// returns the TMBs consistent with lists of selected slots
  std::vector<TMB *> tmbs() const;
  std::vector<TMB *> tmbs(Crate *) const;

private:
  std::vector<int> theSelectedSlots;
  std::vector<int> theSelectedCrates;
};

#endif

