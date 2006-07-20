//-----------------------------------------------------------------------
// $Id: CrateUtils.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: CrateUtils.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.1  2006/02/01 18:41:20  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef CrateUtils_h
#define CrateUtils_h

#include <vector>
#include <string>
#include "Crate.h"
#include "TMBTester.h"


class CrateUtils {
public:
  CrateUtils();
  ~CrateUtils();

  void SetCrate(Crate* crate) { MyCrate_ = crate; }

  /// returns the crates in the CrateSetup consistent with the selection list
  std::vector<TMBTester> TMBTests() const;

private:
  //
  Crate * MyCrate_ ;
  //
};

#endif

