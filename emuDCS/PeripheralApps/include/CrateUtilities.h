#ifndef CrateUtilities_h
#define CrateUtilities_h

#include <iostream>
#include <fstream>


#include "Crate.h"

namespace emu {
  namespace pc {


class CrateUtilities {
  //
public:
  //
  CrateUtilities();
  virtual ~CrateUtilities();
  //
  inline void SetCrate(Crate * crate) { myCrate_ = crate ; }
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void MpcTMBTest(int number_of_events, int minimum_delay_value, int maximum_delay_value); //"safe window" scan
  void MpcTMBTest(int number_of_events);                                                   //not the "safe window" scan
  inline int  GetMpcTMBTestResult() { return MpcTMBTestResult; }             //result of the non-"safe window" scan
  inline void SetMpcTMBTestResult(int result) { MpcTMBTestResult = result; }
  //
  void CreateTstoreTables();
  void DumpTstoreTables();
  //
  inline std::vector< std::string> CrateUtilities::GetPeriphTable(){
    return periph_table;
  }
  //
  inline std::vector< std::string> CrateUtilities::GetCSCTable(int number){
    return csc_table[number];
  }
  //
  inline std::vector< std::string> CrateUtilities::GetTmbTable(int number){
    return tmb_table[number];
  }
  //
  inline std::vector< std::string> CrateUtilities::GetDmbTable(int number){
    return dmb_table[number];
  }
  // 
  inline std::vector< std::string> CrateUtilities::GetAlctTable(int number){
    return alct_table[number];
  }
  //
  inline std::vector< std::vector< std::string> > CrateUtilities::GetAfebTable(){
    return afeb_table;
  }
  //
  inline std::vector< std::vector< std::string> > CrateUtilities::GetCfebTable(){
    return cfeb_table;
  }
  //
 private:
  //
  bool debug_;
  //
  int MpcTMBTestResult;
  std::ostream * MyOutput_ ;
  //
  Crate * myCrate_;
  std::vector< std::string> periph_table;
  std::vector< std::string> csc_table[9];
  std::vector< std::string> tmb_table[9];
  std::vector< std::string> dmb_table[9];
  std::vector< std::string> alct_table[9];
  std::vector< std::vector< std::string> > afeb_table;
  std::vector< std::vector< std::string> > cfeb_table;
  //
};

  } // namespace emu::pc
  } // namespace emu
#endif
