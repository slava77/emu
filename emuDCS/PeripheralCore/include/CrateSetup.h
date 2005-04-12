//-----------------------------------------------------------------------
// $Id: CrateSetup.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: CrateSetup.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CrateSetup_h
#define CrateSetup_h
#include <vector>

class Crate;

/** Usually used as a Singleton
 */

class CrateSetup {
public:
  CrateSetup();
  ~CrateSetup();
  Crate * crate(unsigned int);
  std::vector<Crate *> crates() const {return theCrates;}
  void addCrate(unsigned int number, Crate * crate);

private:
  std::vector<Crate *> theCrates;
};

#endif

