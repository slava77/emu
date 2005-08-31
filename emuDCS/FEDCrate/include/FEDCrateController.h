#ifndef FEDCrateController_h
#define FEDCrateController_h

class DDU;
class DCC;
class Crate;
using namespace std;
#include <vector>

class FEDCrateController {
public:
  FEDCrateController();
  ~FEDCrateController();

  /// lets you give the controller a list of
  /// which card slots you'd like to activate.
  /// default is all active, of course.
  void selectSlots(vector<int> & slots) {theSelectedSlots = slots;}
  void selectCrates(vector<int> & crates) {theSelectedCrates = crates;}

  /// returns the crates in the CrateSetup consistent with the selection list
  vector<Crate *> crates() const;

  /// returns the DMBs consistent with lists of selected slots
  vector<DDU *> ddus(Crate *) const;

  /// returns the DCCs consistent with lists of selected slots
  vector<DCC *> dccs(Crate *) const;

private:
  vector<int> theSelectedSlots;
  vector<int> theSelectedCrates;
};

#endif

