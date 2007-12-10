#ifndef EmuDisplayClient_h
#define EmuDisplayClient_h

#include <algorithm>
#include "xdaq.h"
#include "toolbox.h"
#include "xdata.h"

#include "cgicc.h"
#include "xgi.h"

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TSystem.h>
#include "TBuffer.h"
#include "TMessage.h"
#include "TObject.h"
#include "TH1F.h"
#include "TH1.h"
#include "TCanvas.h"
#include "THStack.h"
#include "ConsumerCanvas.hh"

#include "EmuDQM_AppParameters.h"
#include "EmuDQM_SOAPCommands.h"

class EmuDisplayClient : public xdaq::WebApplication, xdata::ActionListener
{
 public:

  //! define factory method for instantion of EmuLocalRUI application
  XDAQ_INSTANTIATOR();


  EmuDisplayClient(xdaq::ApplicationStub* stub) throw (xdaq::exception::Exception);
  ~EmuDisplayClient();

  // == Callback for requesting current exported parameter values
  void actionPerformed (xdata::Event& e);

  bool onError ( xcept::Exception& ex, void * context );

  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );

  xoap::MessageReference updateList (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference updateObjects (xoap::MessageReference msg) throw (xoap::exception::Exception);

  // Web callback functions
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void createTreeItems(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void createHTMLNavigation(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void createTreePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void createTreeEngine(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void createTreeTemplate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void getImagePage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void getEMUSystemViewPage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void headerPage (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void Configure(xgi::Input * in ) throw (xgi::exception::Exception);
  void Enable(xgi::Input * in ) throw (xgi::exception::Exception);
  void Halt(xgi::Input * in ) throw (xgi::exception::Exception);
  void stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception);
  void failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception);

  /// the thing that receives the message

  std::map<std::string, std::list<std::string> > requestObjectsList(xdaq::ApplicationDescriptor* monitor);
  std::map<std::string, std::list<std::string> > requestCanvasesList(xdaq::ApplicationDescriptor* monitor);
  TMessage* requestObjects(xdata::Integer nodeaddr,  std::string folder, std::string objname);
  TMessage* requestCanvas(xdata::Integer nodeaddr,  std::string folder, std::string objname, int width, int height);

 private:
  
  toolbox::fsm::FiniteStateMachine fsm_; // application state machine
  xgi::WSM wsm_; // Web dialog state machine

  toolbox::exception::HandlerSignature  * errorHandler_;
  
  std::set<xdaq::ApplicationDescriptor*> EmuDisplayClient::getAppsList(xdata::String className);

  // == Vector of all external data servers tids
  std::set<xdaq::ApplicationDescriptor*> monitors_;

  xdata::String monitorClass_;
  xdata::String iconsURL_;
  xdata::String imageFormat_;
  xdata::String imagePath_;
  xdata::Boolean viewOnly_;


};
typedef xdaq::ApplicationDescriptor* pApplicationDescriptor;

class Compare_ApplicationDescriptors {
 public:
  int operator() (const pApplicationDescriptor& ad1, const pApplicationDescriptor& ad2) const 
    { return ad1->getInstance() < ad2->getInstance();}
    };

#endif

