#ifndef EmuLogger_h
#define EmuLogger_h
//
#include <string>
//
class EmuLogger 
{
  //
public:
  EmuLogger();
  ~EmuLogger();
  //
  /// point the output to be where you want
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  //
  /////////////////////////////////////////////////////////////////////
  // register comparison methods
  /////////////////////////////////////////////////////////////////////
  /// test if "testval" is equal expected value: "compareval", print the errors
  bool compareValues(std::string typeOfTest, int testval, int compareval); 
  //
  /// test if "testval" is equal expected value: "compareval", do or do not the errors
  bool compareValues(std::string typeOfTest, int testval, int compareval, bool print_errors); 
  //
  /// same as compareValues, except return depends if they should be "equalOrNot"
  bool compareValues(std::string typeOfTest, int testval, int compareval, bool print_errors, bool equalOrNot); 
  //
  ///test if "testval" is within a fractional "tolerance" of "compareval", print the errors
  bool compareValues(std::string typeOfTest, float testval, float compareval, float tolerance);   
  //
  ///test if "testval" is within a fractional "tolerance" of "compareval", do or do not print the errors
  bool compareValues(std::string typeOfTest, float testval, float compareval, float tolerance, bool print_errors);   
  //
  ///report status of "check_type" to SendOutput...  whether it passed or not is set by "status_bit"
  void ReportCheck(std::string check_type, bool status_bit);    
  //  
  //
protected:
  std::ostream * MyOutput_ ;
  //
  //
private:
  //
};

#endif

