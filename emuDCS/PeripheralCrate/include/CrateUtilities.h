#ifndef CrateUtilities_h
#define CrateUtilities_h

#include <iostream>
#include <fstream>


#include "CrateSelector.h"
#include "Crate.h"

class CrateUtilities {
  //
public:
  //
  CrateUtilities();
  virtual ~CrateUtilities();
  //
  inline void SetCrate(Crate * crate) { myCrate_ = crate ; }
  inline int  GetMpcTMBTestResult() { return MpcTMBTestResult; }
  inline void SetMpcTMBTestResult(int result) { MpcTMBTestResult = result; }
  void MpcTMBTest(int);
  void CreateTstoreTables();
  void DumpTstoreTables();
  //
  inline std::vector<std::string> CrateUtilities::GetPeriphTable(){
    return periph_table;
  }
  //
  inline std::vector<std::string> CrateUtilities::GetCSCTable(int number){
    return csc_table[number];
  }
  //
  inline std::vector<std::string> CrateUtilities::GetTmbTable(int number){
    return tmb_table[number];
  }
  //
  inline std::vector<std::string> CrateUtilities::GetDmbTable(int number){
    return dmb_table[number];
  }
  // 
  inline std::vector<std::string> CrateUtilities::GetAlctTable(int number){
    return alct_table[number];
  }
  //
  inline std::vector<std::vector<std::string> > CrateUtilities::GetAfebTable(){
    return afeb_table;
  }
  //
  inline std::vector<std::vector<std::string> > CrateUtilities::GetCfebTable(){
    return cfeb_table;
  }
  //
 private:
  //
  int MpcTMBTestResult;
  CrateSelector theSelector;
  Crate * myCrate_;
  std::vector<std::string> periph_table;
  std::vector<std::string> csc_table[9];
  std::vector<std::string> tmb_table[9];
  std::vector<std::string> dmb_table[9];
  std::vector<std::string> alct_table[9];
  std::vector<std::vector<std::string> > afeb_table;
  std::vector<std::vector<std::string> > cfeb_table;
  //
};

#endif
