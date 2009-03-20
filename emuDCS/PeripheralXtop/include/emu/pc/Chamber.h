#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>
#include <string>
#include "emu/pc/DcsDimStructures.h"

namespace emu {
  namespace e2p {

class Chamber {
  //
public:
  //
  Chamber();
  ~Chamber();
  //
  void Fill(char *buf, int source=0);
  //
  inline bool Ready() { return ready_; } 
  inline void SetLabel(std::string label) { label_ = label; }
  inline std::string GetLabel() { return label_; }
  inline void SetActive(int act) { active_ = act; }
  inline int GetActive() { return active_; }
  inline int GetStatus() { return states[0]; }
  inline int GetTime() { return states[1]; }
  inline int GetCrate() { return states[2] & 0xFF; }
  inline int GetSlot() { return (states[2]>>8) & 0xFF; }
  //
  void GetDimLV(int hint, LV_1_DimBroker *dim_lv);
  void GetDimTEMP(int hint, TEMP_1_DimBroker *dim_temp);
  //
private:
  //
  std::string label_;
  int active_;
  bool ready_;
  bool corruption;

  int old_time_lv, old_time_temp;
  int states[4];
  float values[60];

  // book values
  int states_bk[4];
  float values_bk[60];

  // DIM
  LV_1_DimBroker dim_lv;
  TEMP_1_DimBroker dim_temp;
};


  } // namespace emu::e2p
} // namespace emu
#endif
