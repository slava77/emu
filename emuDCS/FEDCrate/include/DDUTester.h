#ifndef DDUTester_h
#define DDUTester_h

using namespace std;
#include <iostream>
#include <vector>
#include <string>
#include "VMEModule.h"
#include "JTAG_constants.h"

class DDU;
class DCC;

class DDUTester {
public:
  // not responsible for deleting pointers
  DDUTester(DDU * ddu,DCC *dcc)
  :ddu_(ddu),dcc_(dcc) {}
  ~DDUTester() {}

  void setDDU(DDU * ddu) {ddu_ = ddu;}
  void setDCC(DCC * dcc) {dcc_ = dcc;}

  void all_chip_info();

  /// sends commands by name
  void executeCommand(string command);

private:
  DDU * ddu_;
  DCC * dcc_;
};

#endif

