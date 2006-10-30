//-----------------------------------------------------------------------
// $Id: CrateSetup.h,v 3.1 2006/10/30 15:53:55 mey Exp $
// $Log: CrateSetup.h,v $
// Revision 3.1  2006/10/30 15:53:55  mey
// Update
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
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
  void addCrate(Crate * crate);

private:
  std::vector<Crate *> theCrates;
};

#endif

