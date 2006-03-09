#ifndef EmuPlotterInterface_h
#define EmuPlotterInterface_h

#include <iostream>
#include <string>
#include "TH1.h"
#include "TConsumerInfo.hh"
#include "ConsumerCanvas.hh"


#define DEFAULT_IMAGE_FORMAT "png"
#define DEFAULT_CANVAS_WIDTH  800
#define DEFAULT_CANVAS_HEIGHT 600

/** The interface for all Emu plotters to receive data
 \author Rick Wilkinson
 */

using namespace std;

class EmuPlotterInterface {
public:
  virtual ~EmuPlotterInterface() {};
  virtual void SetHistoFile(string) = 0;
  virtual void SaveImagesFromROOTFile(std::string filename,
	std::string path,
        std::string format=DEFAULT_IMAGE_FORMAT,
        int width=DEFAULT_CANVAS_WIDTH,
        int height=DEFAULT_CANVAS_HEIGHT) = 0;
  virtual void SaveImages(std::string path,
        std::string format=DEFAULT_IMAGE_FORMAT,
        int width=DEFAULT_CANVAS_WIDTH,
        int height=DEFAULT_CANVAS_HEIGHT) = 0;
  virtual void SaveHistos() = 0;
  virtual void SetDDUCheckMask(unsigned int) = 0;
  virtual void SetBinCheckMask(unsigned int) = 0;
  virtual void SetDDU2004(int) = 0;
  virtual void book() = 0;
  virtual void fill(unsigned char * data, int dataSize, unsigned short errorStat) = 0;
  virtual bool isListModified() = 0;
  virtual void setListModified(bool flag) = 0;
  virtual TConsumerInfo* getConsumerInfo() = 0;
  map<int, map<string, TH1*> > histos;
  map<int, map<string, ConsumerCanvas*> > canvases;
 
};

#endif

