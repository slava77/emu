#ifndef _csc_display_SimpleWeb_h
#define _csc_display_SimpleWeb_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xdaq/Application.h"
#include "xgi/Method.h"

#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TImage.h"
#include "TASImage.h"
#include "TSystem.h"

#include "csc_display/utils.h"
#include "emu/daq/reader/RawDataFile.h"

#include "csc_display/JSON.h"
#include "csc_display/unpacker.h"
#include "csc_display/graph.h"

namespace csc_display {

class SimpleWeb: public xdaq::Application {
    public:
    
    XDAQ_INSTANTIATOR();
    
    SimpleWeb(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
    
    // web page handlers
    void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    void SQLForm(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    void LoadJSFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    void LoadCSSFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    void ShowImage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    
    // functions to support web page handlers
    void showStaticHTML(const char * filename, xgi::Output * out);
    
    // called when the event display starts running
    void GetInitParams(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    void ShowGraphics(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
    
    // (for testing purpose only ???)
    void ShowDummyGraphics(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
};

}

#endif
