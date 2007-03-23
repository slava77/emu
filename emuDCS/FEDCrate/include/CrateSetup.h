#ifndef CrateSetup_h
#define CrateSetup_h

using namespace std;
#include <vector>
class Crate;

/** Usually used as a Singleton
 */

class CrateSetup {
public:
  CrateSetup();
  ~CrateSetup();
  Crate * crate(int number);
  void addCrate(int number, Crate * crate);
  vector<Crate *> crates() const {return theCrates;}  // exposes internal member

private:
  vector<Crate *> theCrates;
};

#endif

