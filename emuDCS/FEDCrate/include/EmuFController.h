#ifndef EmuFController_h
#define EmuFController_h
#include "CrateSelector.h"
#include <string>

class DDU;
class DCC;
class Crate;

class EmuFController {
public:
  EmuFController();
  ~EmuFController() {}

  void init();

  void configure();

  void enable();

  void disable();

  inline void SetConfFile(std::string xmlFile) { xmlFile_ = xmlFile; }

  CrateSelector & selector() {return theSelector;}

protected:

private:
  CrateSelector theSelector;
  std::string xmlFile_;
};

#endif
