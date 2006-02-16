//***************************************************************************************
// ConsumerCanvas.hh 
//
// Description: canvas for the consumers
//
//  
// Author List: 
//   Hartmut Stadie  IEKP Karlsruhe     
//======================================================================================= 

#ifndef CONSUMERCANVAS_HH
#define CONSUMERCANVAS_HH 1


#include "TCanvas.h"

class TConsumerInfo;
class TPad;
class TPaveLabel;

class ConsumerCanvas : public TCanvas
{
public:
  ConsumerCanvas(const char* name, const char* title, const char* title2, int nx, int ny);
  virtual ~ConsumerCanvas();
    
//  virtual void cd(Int_t subpadnumber = 0);
  virtual void cd(Int_t subpadnumber = 0, Int_t gridX = 0, Int_t gridY = 0);
  virtual void Streamer(TBuffer& b);
  virtual void Divide(Int_t nx = 1, Int_t ny = 1, Float_t xmargin = 0.005, Float_t ymargin = 0.005, 
		      Int_t color = 0);
  virtual TPad* GetPad(Int_t subpadnumber = 0);
private:
  TPad** _pads;
  TPaveLabel *_title,*_counter;
  TConsumerInfo *_info;
  int _n;
};

#endif
