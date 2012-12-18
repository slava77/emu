#ifndef __emu_me11dev_Application_h__
#define __emu_me11dev_Application_h__


#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"
#include "xcept/tools.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "math.h"
#include "emu/pc/Crate.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/TMB_constants.h"
#include "xdaq/NamespaceURI.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "xcept/tools.h"
#include "xcept/Exception.h"


#include <set>
#include <map>
#include <vector>

using namespace std;
using namespace emu::pc;

namespace emu { namespace me11dev {

    enum CallbackFunctions {
      ReadBackUserCodesEnum,
      SetComparatorThresholdsEnum, 
      SetComparatorThresholdsBroadcastEnum,
      SetUpComparatorPulseEnum,
      SetUpPrecisionCapacitorsEnum,
      PulseInternalCapacitorsEnum,
      PulsePrecisionCapacitorsEnum,
      PulseInternalCapacitorsCCBEnum,
      PulsePrecisionCapacitorsCCBEnum,      
      SetDMBDACsEnum,
      ShiftBuckeyesNormRunEnum
    };

    class Application : public virtual xdaq::WebApplication
    {

    public:
      /// define factory method for the instantion of AFEBTeststand applications
      XDAQ_INSTANTIATOR();

      /// constructor
      Application( xdaq::ApplicationStub *s );
      
      
    protected:
      typedef vector <CFEB>::iterator CFEBItr;
      typedef vector <CFEB>::reverse_iterator CFEBrevItr;
      Logger logger_;
      Crate * ME11TestStandPeripheralCrate;
      vector <DAQMB*> thisDMBs;
      TMB* thisTMB;
      CCB* thisCCB;

      void bindWebInterface();
      void defaultWebPage( xgi::Input *in, xgi::Output *out );
      void Callback(xgi::Input *in, xgi::Output *out);
      void AddButton(xgi::Output *out, const string button_name, CallbackFunctions function) throw (xgi::exception::Exception);
      void AddButtonWithTextBox(xgi::Output *out, const string button_name, const string textboxname, const string textbox_default_value, CallbackFunctions function) throw (xgi::exception::Exception);
      void BackToMainPage(xgi::Input * in, xgi::Output * out );
      int GetFormValueInt(const string form_element, xgi::Input *in);
      float GetFormValueFloat(const string form_element, xgi::Input *in);
      string GetFormValueString(const string form_element, xgi::Input *in);

      string CallbackFunctionsToString(CallbackFunctions function);
      string generateLoggerName();

      //Test Routines
      void ReadBackUserCodes();
      void SetComparatorThresholds(float ComparatorThresholds = .030);
      void SetComparatorThresholdsBroadcast(float ComparatorThresholds = .030);
      void SetUpComparatorPulse(int halfstrip = 16);
      void SetUpPrecisionCapacitors(int strip_to_pulse = 8);
      void PulseInternalCapacitors();
      void PulsePrecisionCapacitors();
      void SetDMBDACs(float DAC = 1.0);
      void PulsePrecisionCapacitorsCCB();
      void PulseInternalCapacitorsCCB();
      void ShiftBuckeyesNormRun();
  };

}}

#endif
