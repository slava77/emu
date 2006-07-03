#ifndef EmuGlobalPlotter_h
#define EmuGlobalPlotter_h

#include "EmuPlotterInterface.h"
#include <iostream>
class MuEndEventData;

class EmuGlobalPlotter : public EmuPlotterInterface {
public:

  void SetHistoFile(string) {};
  void SaveHistos() {};
  void SetDDUCheckMask(unsigned int) {};
  void SetBinCheckMask(unsigned int) {};
  void SetDDU2004(int) {};
  virtual bool isListModified() {return false;};
  virtual void setListModified(bool flag) {};
  virtual TConsumerInfo* getConsumerInfo() {return NULL;};
  /// books the histograms
  void book() {};

  /// since I have no idea what format the final events will come out in,
  /// I'll leave this as a dummy
  void fill(unsigned char * data, int dataSize, unsigned short errorStat) {
    cout << "Dummy EmuGlobalPlotter::fill() called " << endl;
  }

};

#endif

