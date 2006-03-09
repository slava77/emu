#ifndef EmuDisplayMonitor_h
#define EmuDisplayMonitor_h

#include "xdaq.h"
#include "toolbox.h"
#include "xdata.h"

#include "i2o.h"
#include "i2o/Method.h"

#include "cgicc.h"
#include "xgi.h"

#include "TSystem.h"
#include "TApplication.h"
#include "TFile.h"
#include "TMessage.h"
#include "TPad.h"
#include "TH1.h"

#include "Task.h"
#include "TaskAttributes.h"

// Includes from CDF offline
#include "TConsumerInfo.hh"
#include "DisplayServer.hh"

#include "I2O_DQMDataRequest.h"

#define SLIDES_ID 10000
#define SLIDES "Slides"

// TROOT server("ServerROOT","Root of the Server");

class EmuUserDisplayServerTask: public Task
{

public:
   EmuUserDisplayServerTask(): Task("EmuUserDisplayServerTask") 
	{
	// _consinfo = new TConsumerInfo("DQM", 1);
        puserDispServer = NULL;
	}
  ~EmuUserDisplayServerTask() {
	 // _consinfo->clear(); 
	 // delete _consinfo; 
	 // _consinfo = NULL;
	puserDispServer = NULL;
   }
   void setInfo(TConsumerInfo* info) {  // *_consinfo = info;
	if (puserDispServer != NULL && info != NULL) puserDispServer->setCurrentInfo(info);};
   void addStorage(MessageStorage* storage) { if (storage) puserDispServer->updateStorage(storage);
        }
   int svc() {
        gROOT->cd();
        string host = "";
        string hname = "histo";
        string path = "path";
        DisplayServer userDispServer((char*) host.c_str(), 0, gROOT, NULL, true, true, false);
        puserDispServer = &userDispServer;
        int loopRes = 0;
  	loopRes = puserDispServer->loop();
  	return (loopRes);
   }
   // TConsumerInfo* _consinfo;

private:
   DisplayServer * puserDispServer;
};

class EmuRefDisplayServerTask: public Task
{

public:
   EmuRefDisplayServerTask(): Task("EmuRefDisplayServerTask") 
	{
	_consinfo = new TConsumerInfo("DQM", 1);
        puserDispServer = NULL;
	}
  ~EmuRefDisplayServerTask() {delete _consinfo; _consinfo = 0;};
   void setInfo(const TConsumerInfo& info) { *_consinfo = info;
	puserDispServer->setCurrentInfo(_consinfo);};
   void addStorage(MessageStorage* storage) { if (storage) puserDispServer->updateStorage(storage);
        }
   int svc() {
        gROOT->cd();
	string host = "";
        string hname = "histo";
        string path = "path";
        DisplayServer userDispServer((char*) host.c_str(), 0, 9092, gROOT, NULL, true, true, false);
        puserDispServer = &userDispServer;

        int loopRes = 0;
  	loopRes = puserDispServer->loop();
  	return (loopRes);
   }
   TConsumerInfo* _consinfo;

private:
   DisplayServer * puserDispServer;
};

class EmuRef1DisplayServerTask: public Task
{

public:
   EmuRef1DisplayServerTask(): Task("EmuRef1DisplayServerTask") 
	{
	_consinfo = new TConsumerInfo("DQM", 1);
	puserDispServer = NULL;
	}
  ~EmuRef1DisplayServerTask() {delete _consinfo; _consinfo = 0;};
   void setInfo(const TConsumerInfo& info) { *_consinfo = info;
	if (puserDispServer != NULL) puserDispServer->setCurrentInfo(_consinfo);};
   void addStorage(MessageStorage* storage) { if (storage && puserDispServer != NULL) puserDispServer->updateStorage(storage);
        }
   int svc() {
        gROOT->cd();
	string host = "";
        string hname = "histo";
        string path = "path";
	// _consinfo->clear();
        DisplayServer userDispServer((char*) host.c_str(), 0, 9093, gROOT, NULL, true, true, false);
        puserDispServer = &userDispServer;
        int loopRes = 0;
  	loopRes = puserDispServer->loop();	
  	return (loopRes);
   }
   TConsumerInfo* _consinfo;

private:
   DisplayServer * puserDispServer;
};

class EmuDisplayServer : public xdaq::WebApplication
{
public:

  //! define factory method for instantion of EmuLocalRUI application
        XDAQ_INSTANTIATOR();


  EmuDisplayServer(xdaq::ApplicationStub* stub) throw (xdaq::exception::Exception);
  ~EmuDisplayServer();


  bool onError ( xcept::Exception& ex, void * context );

  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );

  xoap::MessageReference updateList (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference updateObjects (xoap::MessageReference msg) throw (xoap::exception::Exception);

  // Web callback functions
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
        void Configure(xgi::Input * in ) throw (xgi::exception::Exception);
        void Enable(xgi::Input * in ) throw (xgi::exception::Exception);
        void Halt(xgi::Input * in ) throw (xgi::exception::Exception);
        void stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception);
        void failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception);

  /// the thing that receives the message
  void I2O_DQMDataRequest(  toolbox::mem::Reference * ref)  throw (i2o::exception::Exception) ;

  void requestList(xdata::Integer nodeaddr);
  void requestObjects(xdata::Integer nodeaddr);
private:
  EmuUserDisplayServerTask * userServer;
  EmuRefDisplayServerTask * RefServer;
  EmuRef1DisplayServerTask * Ref1Server;
  TConsumerInfo *consinfo;
  list<string> nodelist;
  map<string, map<string, list<string> > > objlist; 
  TFile *RFile;
  xdata::String RefFileName;
  xdata::String Ref1FileName;
  void refHistComparator(string, TMessage*, TMessage*, TMessage*); //the thing that compares with ref histos
  void ExtractHistosFromCanvas( TVirtualPad * );
  TH1* ModifyRefHistosFromCanvas( TVirtualPad * );
  int InitialiseRef0;
  
  toolbox::fsm::FiniteStateMachine fsm_; // application state machine
  xgi::WSM wsm_; // Web dialog state machine

  toolbox::exception::HandlerSignature  * errorHandler_;


};

#endif

