#ifndef DDU_h
#define DDU_h

#include <iostream>
#include <vector>
#include <string>
#include "emu/x2p/DcsDimStructures.h"

namespace emu {
  namespace x2p {

class DDU {
  //
public:
  //
  DDU();
  ~DDU();
  //
  void Fill(char *buf, int source=0);
  //
  inline bool Ready() { return ready_; } 
  inline void SetLabel(std::string label) { label_ = label; }
  inline std::string GetLabel() { return label_; }
  inline void SetActive(int act) { active_ = act; }
  inline int GetActive() { return active_; }
  inline int GetStatus() { return states[1]; }
  inline int GetTime() { return states[0]; }
//  inline int GetCrate() { return states[2] & 0xFF; }
//  inline int GetSlot() { return (states[2]>>8) & 0xFF; }
  //
  void GetDimDDU(int hint, DDU_1_DimBroker *dim_ddu);
  //
private:
  //
  std::string label_;
  int active_;
  bool ready_;
  bool corruption;

  int old_time;
  int states[4];
  float values[10];

  // book values
  int states_bk[4];
  float values_bk[10];

  // DIM
  DDU_1_DimBroker dim_ddu;
};


  } // namespace emu::x2p
} // namespace emu
#endif
