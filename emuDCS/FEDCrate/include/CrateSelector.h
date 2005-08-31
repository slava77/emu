#ifndef CrateSelector_h
#define CrateSelector_h

class DDU;
class DCC;
class Crate;
using namespace std;
#include <vector>
#include <string>

class CrateSelector {
public:
  CrateSelector();
  ~CrateSelector();

  /// lets you give the controller a list of
  /// which card slots you'd like to activate.
  /// default is all active, of course.
  void selectSlots(vector<int> & slots) {theSelectedSlots = slots;}
  void selectCrates(vector<int> & crates) {theSelectedCrates = crates;}
  /// -1 means all slots, or else just a single slot
  void setSlot(int slot);
  void setSlot(string strSlot);

  /// -1 means all crates, or else just a single crate
  void setCrate(int crate);
  void setCrate(string strCrate);

  /// returns the crates in the CrateSetup consistent with the selection list
  vector<Crate *> crates() const;

  /// returns the DDUs consistent with lists of selected slots
  vector<DDU *> ddus() const;
  vector<DDU *> ddus(Crate *) const;

  /// returns the DCCs consistent with lists of selected slots
  vector<DCC *> dccs() const;
  vector<DCC *> dccs(Crate *) const;

private:
  vector<int> theSelectedSlots;
  vector<int> theSelectedCrates;
};

#endif

