#ifndef EmuEndcap_h
#define EmuEndcap_h

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

class DAQMB;
class CCB;
class TMB;
class Crate;


class EmuEndcap {
  //
 public:
  //
  EmuEndcap();
  ~EmuEndcap();
  //
  inline Crate * crate(unsigned int i) {
    return theCrates[i];
  }
  //
  std::vector<Crate *> crates();

  inline std::vector<Crate *> AllCrates() { return theCrates; }
  //
  void addCrate(unsigned int i, Crate * crate);
  //
  void addCrate(Crate * crate);
  //
  void selectCrates(std::vector<int> & crates) {theSelectedCrates = crates;}

  /// -1 means all crates, or else just a single crate
  void setCrate(int crate);
  void setCrate(std::string strCrate);

  /// returns the broadcast crate
  std::vector<Crate *> broadcast_crate();

  std::vector<DAQMB *> daqmbs();
  std::vector<DAQMB *> daqmbs(Crate *);

  std::vector<TMB *> tmbs();
  std::vector<TMB *> tmbs(Crate *);

 private:
  //
  std::vector<int> theSelectedCrates;
  std::vector<Crate *> theCrates;
  //
};

#endif
