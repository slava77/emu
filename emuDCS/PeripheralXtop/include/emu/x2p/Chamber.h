#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>
#include <string>
#include "emu/x2p/DcsDimStructures.h"

namespace emu {
  namespace x2p {

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
  inline int GetType() { return type_; }
  //
  void GetDimLV(int hint, LV_1_DimBroker *dim_lv);
  void GetDimTEMP(int hint, TEMP_1_DimBroker *dim_temp);
  void GetDimLV2(int hint, LV_2_DimBroker *dim_lv);
  void GetDimTEMP2(int hint, TEMP_2_DimBroker *dim_temp);
  //
private:
  //
  int type_;
  std::string label_;
  int active_;
  bool dataok_, ready_, corruption;

  int states[8];
  float values[285];

  // DIM
  LV_1_DimBroker dim_lv;
  TEMP_1_DimBroker dim_temp;
  LV_2_DimBroker dim_lv2;
  TEMP_2_DimBroker dim_temp2;
};


  } // namespace emu::x2p
} // namespace emu
#endif
