#ifndef EmuTFDisplayClient_h
#define EmuTFDisplayClient_h

#include <algorithm>
#include <map>
#include <list>

#include "emu/dqm/common/xdaq.h"
#include "emu/dqm/common/xdata.h"
#include "emu/dqm/common/toolbox.h"
#include "emu/dqm/common/xoap.h"
#include "emu/dqm/common/xgi.h"

#include "emu/dqm/common/cgicc.h"

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TBuffer.h>
#include <TMessage.h>
#include <TObject.h>
#include <TH1F.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TClass.h>
#include <TImage.h>
#include <THStack.h>

#include "emu/dqm/common/ConsumerCanvas.hh"

#include "emu/dqm/common/EmuDQM_AppParameters.h"
#include "emu/dqm/common/EmuDQM_Utils.h"

typedef std::map<std::string, std::set<int> > MapType;
typedef std::map<std::string, std::map<std::string, std::string> >Counters;

class FoldersMap: public MapType {
	public:
		FoldersMap(): MapType(), timestamp(0) {};
		time_t getTimeStamp() const {return timestamp;}	
		void setTimeStamp(time_t t) {timestamp=t;} 
		void clear() {timestamp=0; MapType::clear();}	
	private:
		time_t timestamp;
	
};


using namespace toolbox;

class EmuTFDisplayClient : public xdaq::WebApplication, xdata::ActionListener
{
 public:

  //! define factory method for instantion of EmuLocalRUI application
  XDAQ_INSTANTIATOR();


  EmuTFDisplayClient(xdaq::ApplicationStub* stub) throw (xdaq::exception::Exception);
  ~EmuTFDisplayClient();

  // == Callback for requesting current exported parameter values
  void actionPerformed (xdata::Event& e);

  bool onError ( xcept::Exception& ex, void * context );

  // Web callback functions
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);



  void getNodesStatus (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void getTFMapping (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void getTFList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void getTestsList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void genPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void getRefPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void controlDQM (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void redir (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  

  /// the thing that receives the message

  std::map<std::string, std::list<std::string> > requestObjectsList(xdaq::ApplicationDescriptor* monitor);
  std::map<std::string, std::list<std::string> > requestCanvasesList(xdaq::ApplicationDescriptor* monitor);
  TMessage* requestObjects(xdata::Integer nodeaddr,  std::string folder, std::string objname);
  TMessage* requestCanvas(xdata::Integer nodeaddr,  std::string folder, std::string objname, int width, int height);

  std::set<std::string> requestFoldersList(xdaq::ApplicationDescriptor* dest);
  void updateFoldersMap();
  
 protected:
  TCanvas* getMergedCanvas(std::vector<TObject*>& canvases);
  std::map<int, std::map<int, std::vector<int> > > parseTFMap(FoldersMap& fmap);

 private:
  
  toolbox::exception::HandlerSignature  * errorHandler_;
  
  std::set<xdaq::ApplicationDescriptor*> getAppsList(xdata::String className, xdata::String group="dqm");

  // == Vector of all external data servers tids
  std::set<xdaq::ApplicationDescriptor*> monitors_;

  xdata::String monitorClass_;
  xdata::String iconsURL_;
  xdata::String imageFormat_;
  xdata::String imagePath_;
  xdata::Boolean viewOnly_;
  xdata::Boolean debug;
  xdata::String BaseDir;
  xdata::String refImagePath;
  FoldersMap foldersMap; // === Associate DDUs and CSCs with Monitoring nodes
  BSem appBSem_;
  struct timeval bsem_tout;


};
typedef xdaq::ApplicationDescriptor* pApplicationDescriptor;

class Compare_ApplicationDescriptors {
 public:
  int operator() (const pApplicationDescriptor& ad1, const pApplicationDescriptor& ad2) const 
    { return ad1->getInstance() < ad2->getInstance();}
    };

#endif

