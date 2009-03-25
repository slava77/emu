#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
#include <math.h>
//
#include "emu/pc/EmuLogger.h"
//

namespace emu {
  namespace pc {



EmuLogger::EmuLogger() {
  //
  std::cout << "Instantiate EmuLogger" << std::endl;
  MyOutput_ = &std::cout ;
};
//
EmuLogger::~EmuLogger() {
}
//
//
/////////////////////////////////////////////////////////////////////
// register comparison methods
/////////////////////////////////////////////////////////////////////
bool EmuLogger::compareValues(std::string TypeOfTest, int testval, int compareval) {
  //
  //Default is that you want a) to print the errors, and b) return true if "testval" equals "compareval"...
  //
  return compareValues(TypeOfTest,testval,compareval,true,true);
  //
}
//
bool EmuLogger::compareValues(std::string TypeOfTest, int testval, int compareval, bool print_errors) {
  //
  //Default is that you want to return true if "testval" equals "compareval"...
  //
  return compareValues(TypeOfTest,testval,compareval,print_errors,true);
  //
}
//
bool EmuLogger::compareValues(std::string TypeOfTest, int testval, int compareval, bool print_errors, bool equal) {
  //
  // test if "testval" is equivalent to the expected value: "compareval"
  // return depends on if you wanted them to be "equal"
  //
  //(*MyOutput_) << "compareValues:  " << TypeOfTest << " -> ";
  //
  if (equal) {
    //
    if (testval == compareval) {
      // if (print_errors) (*MyOutput_) << "PASS = 0x" << std::hex << compareval << std::endl;
      return true;
    } else {
      if (print_errors) {
	(*MyOutput_) << TypeOfTest << "FAIL -> expected value = 0x" << std::hex << compareval << ", returned value = 0x" << std:: hex << testval << std::endl;
	//
	std::ostringstream dump;
	dump << TypeOfTest << " FAIL -> expected value = 0x" << std::hex << compareval << ", returned value = 0x" << std:: hex << testval << std::endl;
	//if (print_errors) SendOutput(dump.str(),"ERROR");
      }
      return false;
    }
    //
  } else {
    //
    if (testval != compareval) {
      // if (print_errors) (*MyOutput_) << "PASS -> 0x" << std::hex << testval << " not equal to 0x" <<std::hex << compareval << std::endl;
      return true;
    } else {
      if (print_errors) {
	(*MyOutput_) << TypeOfTest << " FAIL -> expected = returned = 0x" << std::hex << testval << std::endl;
	//
	std::ostringstream dump;
	dump << TypeOfTest << " FAIL -> expected = returned = 0x" << std::hex << testval << std::endl;
	//SendOutput(dump.str(),"ERROR");
      }
      return false;
    }
    //
  }
}
//
bool EmuLogger::compareValues(std::string TypeOfTest, float testval, float compareval, float tolerance) {
  //
  // default is to print the errors
  //
  return compareValues(TypeOfTest,testval,compareval,tolerance,true);
  //
}
//
bool EmuLogger::compareValues(std::string TypeOfTest, float testval, float compareval, float tolerance, bool print_errors) {
  //
  // test if "testval" is within "tolerance" of "compareval"...
  //
  //  (*MyOutput_) << "compareValues tolerance:  " << TypeOfTest << " -> ";
  //
  float err = (testval - compareval);
  //
  if (fabs(err)>tolerance) {
    if (print_errors) {
      (*MyOutput_) << TypeOfTest << " FAIL -> expected = " << compareval << ", returned = " << testval << " outside of tolerance "<< tolerance << std::endl;
      //
      std::ostringstream dump;
      dump << TypeOfTest << " FAIL -> expected = " << compareval << ", returned = " << testval << " outside of tolerance "<< tolerance << std::endl;
      //SendOutput(dump.str(),"ERROR");
    }
    //
    return false;
  } else {
    // if (print_errors) (*MyOutput_) << TypeOfTest << " PASS -> value = " << testval << " within "<< tolerance << " of " << compareval << std::endl;
    return true;
  }
  //
}
//
void EmuLogger::ReportCheck(std::string check_type, bool status_bit) {
  //
  std::ostringstream dump;
  dump << ": " << check_type << " -> ";
  (*MyOutput_) << ": " << check_type << " -> ";
  //
  if ( status_bit ) {
    dump << "OK" << std::endl;
    (*MyOutput_) << "OK" << std::endl;
    //    this->SendOutput(dump.str(),"INFO");
    //
  } else {
    dump << "FAIL <-" << std::endl;
    (*MyOutput_) << "FAIL <-" << std::endl;
    //    this->SendOutput(dump.str(),"ERROR");
    //
  }
  //
  return;
}
//
} // namespace emu::pc
} // namespace emu
