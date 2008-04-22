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

  int irqtest();
  int irqlast[4];
  int irqcrate,irqslot,irqstatus;

  inline void SetConfFile(std::string xmlFile) { xmlFile_ = xmlFile; }

  void writeTTSBits(unsigned int crate, unsigned int slot, unsigned int bits);
  unsigned int readTTSBits(unsigned int crate, unsigned int slot);

  CrateSelector & selector() {return theSelector;}
  
  vector<Crate *> getCrates() { return theCrates; }

private:
  DCC *getDCC(int crate, int slot);
  DDU *getDDU(int crate, int slot);

protected:

private:
  CrateSelector theSelector;
  std::string xmlFile_;
  vector<Crate *> theCrates;
};

#endif
