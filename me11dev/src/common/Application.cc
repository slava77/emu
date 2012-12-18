#include "emu/me11dev/Application.h"

#define USE_CRATE_N 0 // ignore anything but the first crate
#define XML_CONFIGURATION_FILE "/local.home/cscme11/config/pc/dans-crate-config.xml"

using namespace cgicc;

string emu::me11dev::Application::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    unsigned long               appInstance    = appDescriptor->getInstance();
    stringstream                ss;
    string                      loggerName;

    ss << appClass << appInstance;
    loggerName = ss.str();

    return loggerName;
}

void emu::me11dev::Application::bindWebInterface()
{
  xgi::bind( this, &emu::me11dev::Application::defaultWebPage, "Default" );
  xgi::bind( this, &emu::me11dev::Application::Callback, "Callback" );
}

void emu::me11dev::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
{
  *out << html() << head() << head() << body()
	   << h1()
	   << "ME1/1 B904 Test Routines"
	   << h1();
  
  AddButton(out,"Read back usercodes",ReadBackUserCodesEnum);

  AddButtonWithTextBox(out,"Set Comparator Thresholds (in volts):","ComparatorThresholds",".030",SetComparatorThresholdsEnum);

  AddButtonWithTextBox(out,"Set Comparator Thresholds-broadcast (in volts):","ComparatorThresholds",".030",SetComparatorThresholdsBroadcastEnum);

  AddButtonWithTextBox(out,"Set up internal capacitor pulse on halfstrip:","halfstrip","16",SetUpComparatorPulseEnum);

  AddButton(out, "Pulse internal capacitors via DMB",PulseInternalCapacitorsEnum);

  AddButton(out, "Pulse internal capacitors via CCB",PulseInternalCapacitorsCCBEnum);

  AddButtonWithTextBox(out,"Set up precision capacitor pulse on strip:","StripToPulse","8",SetUpPrecisionCapacitorsEnum);

  AddButton(out, "Pulse precision capacitors via DMB",PulsePrecisionCapacitorsEnum);

  AddButton(out, "Pulse precision capacitors via CCB",PulsePrecisionCapacitorsCCBEnum);

  AddButtonWithTextBox(out,"Set DMB DACs 0 and 1 to (in volts):","DACs","1.0",SetDMBDACsEnum);

  AddButton(out, "Shift Buckeyes into normal mode",ShiftBuckeyesNormRunEnum);

  *out << body() << html();
}

int emu::me11dev::Application::GetFormValueInt(const string form_element, xgi::Input *in)
{
  const cgicc::Cgicc cgi(in);
  int form_value;
  cgicc::const_form_iterator name = cgi.getElement(form_element);
  if(name !=cgi.getElements().end())
    {
      form_value = cgi[form_element]->getIntegerValue();
    }
  else
    {
      XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
    }
  return form_value;
}  

float emu::me11dev::Application::GetFormValueFloat(const string form_element, xgi::Input *in)
{
  const cgicc::Cgicc cgi(in);
  float form_value;
  cgicc::const_form_iterator name = cgi.getElement(form_element);
  if(name !=cgi.getElements().end())
    {
      form_value = cgi[form_element]->getDoubleValue();
    }
  else
    {
      XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
    }
  return form_value;
}

string emu::me11dev::Application::GetFormValueString(const string form_element, xgi::Input *in)
{
  const cgicc::Cgicc cgi(in);
  string form_value;
  cgicc::const_form_iterator name = cgi.getElement(form_element);
  if(name !=cgi.getElements().end())
    {
      form_value = cgi[form_element]->getValue();
    }
  else
    {
      XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
    }
  return form_value;
}

string emu::me11dev::Application::CallbackFunctionsToString(CallbackFunctions function)
{
  stringstream conversion_string;
  conversion_string << (int)function;
  return conversion_string.str();
}

void emu::me11dev::Application::Callback(xgi::Input *in, xgi::Output *out)
{
  *out << "";
  CallbackFunctions function = (CallbackFunctions)GetFormValueInt("function", in);

  if (function==ReadBackUserCodesEnum)
    {
      ReadBackUserCodes();
    }
  if (function==SetComparatorThresholdsEnum)
    {
      float ComparatorThresholds = GetFormValueFloat("ComparatorThresholds", in);
      SetComparatorThresholds(ComparatorThresholds);
    }
  if (function==SetComparatorThresholdsBroadcastEnum)
    {
      float ComparatorThresholds = GetFormValueFloat("ComparatorThresholds", in);
      SetComparatorThresholds(ComparatorThresholds);
    }
  if (function==SetUpComparatorPulseEnum)
    {
      int halfstrip = GetFormValueInt("Halfstrip", in);
      SetUpComparatorPulse(halfstrip);
    }
  if (function==SetUpPrecisionCapacitorsEnum)
    {
      int strip_to_pulse = GetFormValueInt("StripToPulse", in);
      SetUpPrecisionCapacitors(strip_to_pulse);
    }
  if (function==PulseInternalCapacitorsEnum)
    {
      PulseInternalCapacitors();
    }
  if (function==PulsePrecisionCapacitorsEnum)
    {
      PulsePrecisionCapacitors();
    }
  if (function==PulseInternalCapacitorsCCBEnum)
    {
      PulseInternalCapacitorsCCB();
    }
  if (function==PulsePrecisionCapacitorsCCBEnum)
    {
      PulsePrecisionCapacitorsCCB();
    }
  if (function==SetDMBDACsEnum)
    {
      float DACs = GetFormValueFloat("DACs", in);
      SetDMBDACs(DACs);
    }
  if (function==ShiftBuckeyesNormRunEnum)
    {
      ShiftBuckeyesNormRun();
    }
  BackToMainPage(in, out);    
}

void emu::me11dev::Application::AddButton(xgi::Output *out, const string button_name, CallbackFunctions function) throw (xgi::exception::Exception)
{
  *out << cgicc::td();
  *out<<cgicc::form().set("method","GET").set("action",
                                               "/" + getApplicationDescriptor()->getURN() + "/Callback") << std::endl;
  *out << cgicc::input().set("type","submit").set("value",button_name)<<std::endl;
  *out << cgicc::input().set("type","hidden").set("value",CallbackFunctionsToString(function)).set("name","function");
  *out<<cgicc::form()<<std::endl;
  *out << cgicc::td() << std::endl;
}

void emu::me11dev::Application::AddButtonWithTextBox(xgi::Output *out, const string button_name, const string textboxname, const string textbox_default_value, CallbackFunctions function) throw (xgi::exception::Exception)
{
  *out << cgicc::td();
  *out<<cgicc::form().set("method","GET").set("action",
                                               "/" + getApplicationDescriptor()->getURN() + "/Callback") << std::endl;
  *out << cgicc::input().set("type","submit").set("value",button_name)<<std::endl;
  *out << cgicc::input().set("type","text").set("value",textbox_default_value).set("name",textboxname);
  *out << cgicc::input().set("type","hidden").set("value",CallbackFunctionsToString(function)).set("name","function");
  *out<<cgicc::form()<<std::endl;
  *out << cgicc::td() << std::endl;
}

void emu::me11dev::Application::BackToMainPage(xgi::Input * in, xgi::Output * out ) // Redirect back to the main page. -Joe
{
  //// Use this after a "GET" button press to get back to the base url
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::head();
  *out << cgicc::meta().set("http-equiv","Refresh").set("content","0; url=./");
  *out << cgicc::head() << std::endl;
  *out << cgicc::body();
  *out << cgicc::p() << "Operation Complete" << std::endl;
  *out << cgicc::p();
  *out << cgicc::body() << std::endl;
  *out << cgicc::html() << std::endl;
}

emu::me11dev::Application::Application( xdaq::ApplicationStub *s ) :
  xdaq::WebApplication( s ),
  logger_( Logger::getInstance( generateLoggerName() ) )
{
  // xdata::InfoSpace *is = getApplicationInfoSpace();
  // is->fireItemAvailable( "", &_ );
    
  XMLParser xmlparser;
  xmlparser.parseFile(XML_CONFIGURATION_FILE);

  if (!xmlparser.GetEmuEndcap()) {
    // if something went wrong while parsing ...
    XCEPT_RAISE(xcept::Exception,
                string("Could not parse xml crate configuration file, ") +
                XML_CONFIGURATION_FILE + ".");
  }

  ME11TestStandPeripheralCrate = xmlparser.GetEmuEndcap()->crates().at(USE_CRATE_N);

  thisCCB = ME11TestStandPeripheralCrate->ccb();
  thisTMB = ME11TestStandPeripheralCrate->tmbs().at(0);
  thisDMBs = ME11TestStandPeripheralCrate->daqmbs();

  bindWebInterface();

  thisCCB->setCCBMode(emu::pc::CCB::VMEFPGA);  
}

void emu::me11dev::Application::ReadBackUserCodes()
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      vector <CFEB> cfebs = (*dmb)->cfebs();
      for(CFEBrevItr cfeb = cfebs.rbegin(); cfeb != cfebs.rend(); ++cfeb) {
	int cfeb_index = (*cfeb).number();
	printf(" ********************* \n");  
	printf(" FEB%d : ",cfeb_index);
	int usercode =(*dmb)->febfpgauser(*cfeb);
	printf(" Usercode: %x\n", usercode);
	int status = (*dmb)->virtex6_readreg(7);
	printf(" Virtex 6 Status: %x\n", status);
      }
      (*dmb)->daqmb_adc_dump();
    }
}

void emu::me11dev::Application::SetComparatorThresholds(float ComparatorThresholds)
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->set_comp_thresh(ComparatorThresholds);
    }
}

void emu::me11dev::Application::SetComparatorThresholdsBroadcast(float ComparatorThresholds)
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->dcfeb_set_comp_thresh_bc(ComparatorThresholds);
    }
}

void emu::me11dev::Application::SetUpComparatorPulse(int halfstrip)
{
  thisCCB->hardReset();
  sleep(5);
  
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(emu::pc::seq_trig_en_adr);

  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      int hp[6] = {halfstrip+1, halfstrip, halfstrip+1, halfstrip, halfstrip+1, halfstrip};
      (*dmb)->trigsetx(hp,0x7f); 
      (*dmb)->buck_shift();
    }   
 
  thisCCB->syncReset();//check
  sleep(1);
  thisCCB->bx0();   //check
}

void emu::me11dev::Application::SetUpPrecisionCapacitors(int strip_to_pulse)
{
  thisCCB->hardReset();
  sleep(5);
  
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(emu::pc::seq_trig_en_adr);

  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->set_ext_chanx(strip_to_pulse);//check
      (*dmb)->buck_shift();//check
    }   
 
  thisCCB->syncReset();//check
  sleep(1);
  thisCCB->bx0();  
}

void emu::me11dev::Application::PulseInternalCapacitors()
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->inject(1,0);
    }   
}

void emu::me11dev::Application::PulseInternalCapacitorsCCB()
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      thisCCB->inject(1,0);
    }   
}

void emu::me11dev::Application::PulsePrecisionCapacitors()
{
    for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->pulse(1,0);
    }  
}

void emu::me11dev::Application::PulsePrecisionCapacitorsCCB()
{
    for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      thisCCB->pulse(1,0);
    }  
}

void emu::me11dev::Application::SetDMBDACs(float DAC)
{
  for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {
      (*dmb)->set_dac(DAC,DAC);
    }  

}

void emu::me11dev::Application::ShiftBuckeyesNormRun()
{
   
 for(vector <DAQMB*>::iterator dmb = thisDMBs.begin(); dmb != thisDMBs.end(); ++dmb)
    {

        (*dmb)->shift_all(NORM_RUN); 

    }


}


/**
 * Provides the factory method for the instantiation of applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::me11dev::Application)
