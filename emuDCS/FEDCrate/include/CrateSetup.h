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
  Crate * crate(unsigned int);
  vector<Crate *> crates() const {return theCrates;}
  void addCrate(unsigned int number, Crate * crate);

private:
  vector<Crate *> theCrates;
};

#endif

