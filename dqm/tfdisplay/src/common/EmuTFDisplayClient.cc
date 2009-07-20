#include <iostream>
#include <string>
#include <sstream>

#include "emu/dqm/tfdisplay/EmuTFDisplayClient.h"


#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"


/*
  #include "TBuffer.h"
  #include "TMessage.h"
  #include "TObject.h"
  #include "TH1F.h"
  #include "TH1.h"
  #include "TCanvas.h"
  #include "THStack.h"
*/  
using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuTFDisplayClient)

  EmuTFDisplayClient::EmuTFDisplayClient(xdaq::ApplicationStub* stub)
  throw (xdaq::exception::Exception)
    : xdaq::WebApplication(stub),
      monitorClass_("EmuTFMonitor"),
      iconsURL_("http://cms-dqm03.phys.ufl.edu/dqm/results/"),
      imageFormat_("png"),
      imagePath_("images"),
      viewOnly_(true),
      debug(false),
      BaseDir("/csc_data/dqm"),
      refImagePath("ref.plots"),
      appBSem_(BSem::FULL)
{


  appBSem_.take();
 
  bsem_tout.tv_sec=10;
  bsem_tout.tv_usec=0;


  errorHandler_ = toolbox::exception::bind (this, &EmuTFDisplayClient::onError, "onError");

  xgi::bind(this, &EmuTFDisplayClient::getRefPlot, "getRefPlot");
  xgi::bind(this, &EmuTFDisplayClient::getNodesStatus, "getNodesStatus");
  xgi::bind(this, &EmuTFDisplayClient::getTFMapping, "getTFMapping");
  xgi::bind(this, &EmuTFDisplayClient::getTFList, "getTFList");
  xgi::bind(this, &EmuTFDisplayClient::getTestsList, "getTestsList");
  xgi::bind(this, &EmuTFDisplayClient::genPlot, "genPlot");
  xgi::bind(this, &EmuTFDisplayClient::controlDQM, "controlDQM");
  xgi::bind(this, &EmuTFDisplayClient::redir, "redir");

  getApplicationInfoSpace()->fireItemAvailable("monitorClass",&monitorClass_);
  getApplicationInfoSpace()->addItemChangedListener ("monitorClass", this);
  getApplicationInfoSpace()->fireItemAvailable("iconsURL",&iconsURL_);
  getApplicationInfoSpace()->addItemChangedListener ("iconsURL", this);
  getApplicationInfoSpace()->fireItemAvailable("imageFormat",&imageFormat_);
  getApplicationInfoSpace()->addItemChangedListener ("imageFormat", this);
  getApplicationInfoSpace()->fireItemAvailable("imagePath",&imagePath_);
  getApplicationInfoSpace()->fireItemAvailable("viewOnly",&viewOnly_);
  getApplicationInfoSpace()->addItemChangedListener ("viewOnly", this);
  getApplicationInfoSpace()->fireItemAvailable("baseDir",&BaseDir);
  getApplicationInfoSpace()->fireItemAvailable("debug",&debug);
  getApplicationInfoSpace()->addItemChangedListener ("debug", this);

  // === Initialize ROOT system
  if (!gApplication)
    TApplication::CreateApplication();
 
  gStyle->SetPalette(1,0);
  foldersMap.setTimeStamp(time(NULL)-20);
  updateFoldersMap();

  appBSem_.give();
}
    
    
EmuTFDisplayClient::~EmuTFDisplayClient() 
{
}



bool EmuTFDisplayClient::onError ( xcept::Exception& ex, void * context )
{
  LOG4CPLUS_INFO (getApplicationLogger(),"onError: " << ex.what());
  return false;
}

// == Run Control requests current parameter values == //
void EmuTFDisplayClient::actionPerformed (xdata::Event& e)
{
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      // std::cout << item << std::endl;

      if ( item == "monitorClass")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "monitor Class : " << monitorClass_.toString());
	  monitors_.clear();
	  monitors_ = getAppsList(monitorClass_);
	  // updateFoldersMap();
        } 
      else if (item == "iconsURL") 
	{
	  if (iconsURL_.toString().find("http") == std::string::npos) {
	    std::string url = getApplicationDescriptor()->getContextDescriptor()->getURL() + iconsURL_.toString();
	    iconsURL_ = url;
	  }
	}
      
    }

}


// XGI Call back

void EmuTFDisplayClient::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  ifstream index;
  index.open( (BaseDir.toString()+"/index.html").c_str());
  if (index)
    {
      *out << index.rdbuf();
    }

}


void EmuTFDisplayClient::redir (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();

  cgicc::Cgicc cgi(in);
  std::string user_host = in->getenv("REMOTE_HOST");

  std::string url="";
  cgicc::const_form_iterator urlInputElement = cgi.getElement("url");
  if (urlInputElement != cgi.getElements().end())
    {
      url = (*urlInputElement).getValue();
    }

  ifstream f;
  f.open( (BaseDir.toString()+"/"+url).c_str());

  if (f)
    {
      *out << f.rdbuf();
    }
  else
    {
      // == Empty map
      *out << "" << std::endl;
    }

  f.close();

  appBSem_.give();
}

void EmuTFDisplayClient::getTFMapping (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  ifstream map;
  map.open( (BaseDir.toString()+"/tf_map.js").c_str());
  if (map) {
    *out << map.rdbuf();
  } else {
    // == Empty map
    *out << "var CSCMAP=[]" << std::endl;
  }
  map.close();


}

// Parse TF Folders 
std::map<int, std::map<int, std::vector<int> > > EmuTFDisplayClient::parseTFMap(FoldersMap& fmap)
{
  std::map<int, std::map<int, std::vector<int> > > tf_map;
  std::map<std::string, std::set<int> >::const_iterator itr;
  for (itr=fmap.begin(); itr != fmap.end(); ++itr)
    {
      std::string folder = itr->first;
      int tf=0, tf_inp=0, tf_csc=0;
      int res = sscanf(folder.c_str(), "c_TF_%d_%d_%d_",&tf,&tf_inp,&tf_csc);
      switch (res)
	{
	case 3:
	  tf_map[tf][tf_inp].push_back(tf_csc);
	  break;
	case 2:
	  break;
	case 1:
	  break;
	}
      
    }

  return tf_map;
}

void EmuTFDisplayClient::getTFList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take(&bsem_tout);
  *out << "var TF_LIST = { " << "'run':'Online'";
  try 
    {
      updateFoldersMap();
      if (!foldersMap.empty()) {
	*out << ", 'tf_map': \n[" << std::endl;
	std::map<int, std::map<int, std::vector<int> > > tf_map = parseTFMap(foldersMap);
	for ( std::map<int, std::map<int, std::vector<int> > >::iterator tf_itr = tf_map.begin();
		  tf_itr != tf_map.end(); ++tf_itr)
	  {
	    *out << "{ 'TF': '" << tf_itr->first << "'";
	    std::map<int, std::vector<int> >& tf_inp_map = tf_itr->second;
	    if (tf_inp_map.size()) 
	      {
		*out << ",'inputs': \n[" << std::endl;
		for ( std::map<int, std::vector<int> >::iterator tf_inp_itr = tf_inp_map.begin();
		      tf_inp_itr != tf_inp_map.end(); ++tf_inp_itr)
		  {
		    *out << "{ 'F': '" << tf_inp_itr->first << "'";
		    std::vector<int> chambers = tf_inp_itr->second;
		    if (chambers.size())
		      {
			*out << ", 'chambers':[";
			for (std::vector<int>::iterator csc_itr = chambers.begin();
			     csc_itr != chambers.end(); ++csc_itr)
			  {
			    *out << "{'CSC': '" << (*csc_itr) << "'}";
			    if (csc_itr+1 != chambers.end()) *out << ",";
			  }
		        *out << "]";
			  
		      }
		    *out << "},";
		    // if (tf_inp_itr+1 != tf_inp_map.end()) *out << ",";
		    *out << std::endl;
		  }
		*out << "]" << std::endl;
	      }
	    *out << "},";
	    // if (tf_itr+1 != tf_map.end()) *out << ",";
	    *out << std::endl;
	  }
	
	*out << "]"<< std::endl;
      }
    }
  catch (xoap::exception::Exception &e) 
    {
      if (debug) LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to getTFList: " 
				 << xcept::stdformat_exception_history(e));
    }
 
  *out << "}" << std::endl;
  appBSem_.give();

}

void EmuTFDisplayClient::controlDQM (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take(&bsem_tout);
  cgicc::Cgicc cgi(in);
  std::string user_host = in->getenv("REMOTE_HOST");

  std::string action = "";
  std::string node="ALL";

  cgicc::const_form_iterator actionInputElement = cgi.getElement("action");
  if (actionInputElement != cgi.getElements().end()) {
    action = (*actionInputElement).getValue();
    LOG4CPLUS_INFO(getApplicationLogger(), "Action request: " << action << " from " << user_host);
  }

  cgicc::const_form_iterator nodeInputElement = cgi.getElement("node");
  if (nodeInputElement != cgi.getElements().end()) {
    node = (*nodeInputElement).getValue();
  }


  std::set<xdaq::ApplicationDescriptor*>  monitors = getAppsList(monitorClass_);
  if (!monitors.empty()) {
    std::set<xdaq::ApplicationDescriptor*>::iterator mon;

    for (mon=monitors.begin(); mon!=monitors.end(); ++mon) {
      if (node == "ALL") {
	try
	  {
	    emu::dqm::sendFSMEventToApp(action, getApplicationContext(), getApplicationDescriptor(),*mon);
	  }
	catch(xcept::Exception e)
	  {
	    //      stringstream oss;

	    //      oss << "Failed to " << action << " ";
	    //      oss << (*mon)->getClassName() << (*mon)->getInstance();

	    //      XCEPT_RETHROW(emuDAQManager::exception::Exception, oss.str(), e);

	    // Don't raise exception here. Go on to try to deal with the others.
	    LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to " << action << " "
			    << (*mon)->getClassName() << (*mon)->getInstance() << " "
			    << xcept::stdformat_exception_history(e));
	  }

      }
    }

  }
  appBSem_.give();

}



void EmuTFDisplayClient::getTestsList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  // == Temporary 
  // == TODO: Request or load actual tests list
  ifstream map;
  map.open( (BaseDir.toString()+"/canvases_list.js").c_str());
  if (map) {
    *out << map.rdbuf();
  } else {
    // == Empty map
    *out << "var TREE_ITEMS=[]" << std::endl;
  }
  map.close();
}



void EmuTFDisplayClient::getNodesStatus (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take(&bsem_tout);  
  *out << "var NODES_LIST=[" << std::endl;
  *out << "['Node','State','Run Number','DAQ Events','DQM Events','Rate (Evt/s)','Readout Mode','Data Source','Last event timestamp']," << std::endl;

  std::set<xdaq::ApplicationDescriptor*>  monitors = getAppsList(monitorClass_);
  std::set<xdaq::ApplicationDescriptor*>  ruis = getAppsList("EmuRUI","default");
  if (!monitors.empty()) {
    std::set<xdaq::ApplicationDescriptor*>::iterator pos;
    std::set<xdaq::ApplicationDescriptor*>::iterator rui_itr;
    xdaq::ApplicationDescriptor* rui=NULL;
    
    for (pos=monitors.begin(); pos!=monitors.end(); ++pos) {
      // for (int i=0; i<monitors_.size(); i++) {
      if ((*pos) == NULL) continue;
      if (!ruis.empty()) {
	for (rui_itr=ruis.begin(); rui_itr != ruis.end(); ++rui_itr) {
	  if ((*pos)->getInstance() == (*rui_itr)->getInstance()) {
	    rui=(*rui_itr);
	    break;
	  }
	}
      }


      std::ostringstream st;
      st << (*pos)->getClassName() << "-" << (*pos)->getInstance();
      std::string nodename = st.str();
      
      std::string applink = "NA";
      std::string state =  "NA";
      std::string stateChangeTime = "NA";
      std::string runNumber   = "NA";
      std::string events = "0";
      std::string dataRate   = "0";
      std::string cscUnpacked   = "0";
      std::string cscRate   = "0";
      std::string readoutMode   = "NA";
      std::string lastEventTime = "NA";
     

      std::string nDAQevents = "0";
      std::string dataSource = "NA";

      try
	{
	  applink = (*pos)->getContextDescriptor()->getURL()+"/"+(*pos)->getURN();
	  state =  emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"stateName","string");
	  if (!viewOnly_) {
	    applink += "/showControl";
	  }
	  if (state == "") {
	    state = "Unknown/Dead";
	    continue;
	  }
	  else {
	    stateChangeTime = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"stateChangeTime","string");
	    state += " at " + stateChangeTime;
	  }


	  runNumber   = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(),  (*pos),"runNumber","unsignedInt");
	  events = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"sessionEvents","unsignedInt");
	  dataRate   = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"averageRate","unsignedInt");
	  readoutMode   = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"readoutMode","string");
	  //lastEventTime = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"lastEventTime","string");

	  nDAQevents = "0";
	  if (readoutMode == "internal") {
	    dataSource   = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"inputDeviceName","string");
	    nDAQevents = events;
	  }
	  if (readoutMode == "external") {
	    dataSource   = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"serversClassName","string");
	    if (rui != NULL) {
	      // == Commented it to prevent online DQM freezes
	      // nDAQevents = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), rui,"nEventsRead","unsignedLong");
	      // nDAQevents = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"nDAQEvents","unsignedInt");
	    }
	  }
	}
      catch(xcept::Exception e)
	{
	  if (debug) LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
	}
      *out << "['"<< nodename << "','"<< applink << "','" << state
	   << "','" << runNumber << "','" << nDAQevents << "','" << events 
	   << "','" << dataRate 
	   << "','" << readoutMode << "','" << dataSource << "','" << lastEventTime
	   << "']" << "," << std::endl;

    }
  }  
  *out << "]" << std::endl;
  appBSem_.give();

}



TCanvas* EmuTFDisplayClient::getMergedCanvas(std::vector<TObject*>& canvases)
{
  TCanvas* cnv = NULL;
  if (canvases.size()) {
    cnv=reinterpret_cast<TCanvas*>(canvases[0]->Clone());
    int npads = cnv->GetListOfPrimitives()->Capacity();
    for (uint32_t i=1; i< canvases.size(); i++) {      
      TCanvas* cnv2 = reinterpret_cast<TCanvas*>(canvases[i]);
      int npads2 = cnv2->GetListOfPrimitives()->Capacity();
      if (npads2 == npads) {
	TIter citr(cnv->GetListOfPrimitives());
        TIter citr2(cnv2->GetListOfPrimitives());
	TObject *subpad, *subpad2;
	while((subpad=citr()) && (subpad2=citr2())) {
	  if (subpad->InheritsFrom(TPad::Class())  
	      && subpad2->InheritsFrom(TPad::Class()) )   {
	    TIter itr(reinterpret_cast<TPad*>(subpad)->GetListOfPrimitives());
	    TIter itr2(reinterpret_cast<TPad*>(subpad2)->GetListOfPrimitives());
	    TObject* obj, *obj2;
	    while ((obj=itr()) && (obj2=itr2())) {
	      if ( obj && obj2 
		   && obj->InheritsFrom(TH1::Class())
		   && obj2->InheritsFrom(TH1::Class())) {
		// double max1=reinterpret_cast<TH1*>(obj)->GetMaximum();
		double max2=reinterpret_cast<TH1*>(obj2)->GetMaximum();
		//double min1=reinterpret_cast<TH1*>(obj)->GetMinimum();
		double min2=reinterpret_cast<TH1*>(obj2)->GetMinimum();
		(reinterpret_cast<TH1*>(obj))->Add(reinterpret_cast<TH1*>(obj2));
		double max1=reinterpret_cast<TH1*>(obj)->GetMaximum();
		double min1=reinterpret_cast<TH1*>(obj)->GetMinimum();
		// LOG4CPLUS_INFO(getApplicationLogger(), obj->GetName() << " " << max1 << " " << min1 << " " << max2 << " " << min2);	
		if ((max1 == min1) && (max1 == 0)) {
		  reinterpret_cast<TH1*>(obj)->SetMaximum(reinterpret_cast<TH1*>(obj)->GetMinimum()+0.01);
		} 		
		if (max2>max1) reinterpret_cast<TH1*>(obj)->SetMaximum(max2);
		if (min2<min1) reinterpret_cast<TH1*>(obj)->SetMinimum(min2);	
	
	      }
	    }				    	      
	  }
	}
      } 
    }  
  }
  return cnv;
}

void EmuTFDisplayClient::genPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take(&bsem_tout);
  cgicc::Cgicc cgi(in);
  int width=1200;
  int height=900;
  std::string objname = "";
  std::string folder = "";

  // Find node ID
  int nodeID = 0;


  std::string user_host = in->getenv("REMOTE_HOST");
  

  cgicc::const_form_iterator stateInputElement = cgi.getElement("objectName");
  if (stateInputElement != cgi.getElements().end()) {
    objname = (*stateInputElement).getValue();
  }

  stateInputElement = cgi.getElement("folder");
  if (stateInputElement != cgi.getElements().end()) {
    folder = (*stateInputElement).getValue();
  }

  stateInputElement = cgi.getElement("imageWidth");
  if (stateInputElement != cgi.getElements().end()) {
    width = (*stateInputElement).getIntegerValue();
  }

  stateInputElement = cgi.getElement("imageHeight");
  if (stateInputElement != cgi.getElements().end()) {
    height = (*stateInputElement).getIntegerValue();
  }

  
  std::map<std::string, std::set<int> >::iterator itr = foldersMap.find(folder);
  if ((itr == foldersMap.end()) || itr->second.empty()) {
    LOG4CPLUS_WARN (getApplicationLogger(), "Can not locate request node for " << folder);
    return;
  }

  
  // == Send request to nodes from list
  std::set<int>::iterator nitr;
  std::vector<TObject*> canvases;

  TObject* obj = NULL;
  for (nitr = itr->second.begin(); nitr != itr->second.end(); ++nitr) {
    nodeID = *nitr;  
    TMessage* msgbuf =requestCanvas(nodeID, folder, objname,width, height);
 
    if (msgbuf != NULL) {
      msgbuf->Reset();
      msgbuf->SetReadMode();
      if (msgbuf->What() == kMESS_OBJECT) {
	// std::cout << ((msgbuf->GetClass())->ClassName()) << std::endl;
	obj = reinterpret_cast<TObject*>(msgbuf->ReadObjectAny(msgbuf->GetClass()));
	if ((obj != NULL) && obj->InheritsFrom(TCanvas::Class())) {
	  canvases.push_back(obj);
	  // delete obj;
	  obj=NULL;
	}
      }
      
      delete msgbuf;
      msgbuf=NULL;
    }    
  }
  

    
  if (canvases.size()) {
    TCanvas* cnv = getMergedCanvas(canvases);// reinterpret_cast<ConsumerCanvas*>(obj);
    if (cnv != NULL) {
      //      cnv->cd();
      cnv->Draw();

      TImage *img = TImage::Create ();
      char   *data = 0;
      int    size = 0;

      img->FromPad(cnv);
      std::string imgname=BaseDir.toString()+"/"+imagePath_.toString()+"/"+folder+"_"+objname+"."+imageFormat_.toString();
      //cnv->Print(imgname.c_str());
      //img->ReadImage(imgname.c_str());
      img->Gray(false);
      img->GetImageBuffer(&data, &size, TImage::kPng);

      (out->getHTTPResponseHeader()).addHeader("Content-Type ","image/png");
      out->write(data, size);

      LOG4CPLUS_INFO (getApplicationLogger(), "Show plot: \"" << folder << "/" << objname << "\" for " << user_host);
      free (data);
      delete img;
      delete cnv;
    } else {
      std::cout << "Empty canvas" << std::endl;
    }
    for (uint32_t i=0; i<canvases.size(); i++) {
      delete canvases[i];
    }
    canvases.clear();
    
  }
  appBSem_.give();
}

void EmuTFDisplayClient::getRefPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take(&bsem_tout);
  cgicc::Cgicc cgi(in);
  std::string plot = "";
  std::string folder = "";

  std::string user_host = in->getenv("REMOTE_HOST");

  cgicc::const_form_iterator stateInputElement = cgi.getElement("plot");
  if (stateInputElement != cgi.getElements().end()) {
    plot = (*stateInputElement).getValue();
  }

  stateInputElement = cgi.getElement("folder");
  if (stateInputElement != cgi.getElements().end()) {
    folder = (*stateInputElement).getValue();  
  }

  TImage *img = TImage::Create ();
  char   *data = 0;
  int    size = 0;

  if (folder.find("CSC_") ==0) { // Convert CSC_XXX_YY to crateX/slotX string
    int crate=0, slot=0;
    int n = sscanf(folder.c_str(), "CSC_%03d_%02d", &crate, &slot);
    if (n==2) folder = Form("crate%d/slot%d",crate,slot);
  }

  std::string plotpath=BaseDir.toString()+"/"+refImagePath.toString()+"/"+folder+"/"+plot;
  struct stat stats;
  if (stat(plotpath.c_str(), &stats)<0) {
    LOG4CPLUS_WARN(getApplicationLogger(), plotpath << ": " <<
		    strerror(errno));
  } else {
    img->ReadImage(plotpath.c_str());
  }

  img->Gray(false);
  img->GetImageBuffer(&data, &size, TImage::kPng);

  (out->getHTTPResponseHeader()).addHeader("Content-Type ","image/png");
  out->write(data, size);

  LOG4CPLUS_DEBUG (getApplicationLogger(), "Show Reference plot: \"" << folder << "/" << plot << "\" for " << user_host);
  free (data);
  delete img;

  appBSem_.give();

}


std::map<std::string, std::list<std::string> > EmuTFDisplayClient::requestObjectsList(xdaq::ApplicationDescriptor* monitor)
{

  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjectsList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPName originator ("originator", "", "");
  //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );
    
  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {
      if (monitor == NULL) return bmap;
      // appBSem_.take();
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestObjectsList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *monitor);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      // appBSem_.give();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	  return bmap;
	} else {       
	  LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestObjectsList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
	  //	  std::map<std::string, std::list<std::string> > bmap;
	  std::list<std::string> olist;
	  vector<xoap::SOAPElement> content = rb.getChildElements ();
	  xoap::SOAPName nodeTag ("DQMNode", "", "");
	  for (vector<xoap::SOAPElement>::iterator itr = content.begin();
	       itr != content.end(); ++itr) {
            
	    std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
	    for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
		 n_itr != nodeElement.end(); ++n_itr) {
	      std::stringstream stdir;
	      bmap.clear();
	      xoap::SOAPName branchTag ("Branch", "", "");

	      std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

	      for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
		   b_itr != branchElement.end(); ++b_itr) {

		olist.clear();
		stringstream stdir;
		string dir="";
		stdir.clear();
		dir = b_itr->getValue();
		// dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
		xoap::SOAPName objTag("Obj", "", "");

		vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

		for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
		     o_itr != objElement.end(); ++o_itr ) {
		  olist.push_back(o_itr->getValue());
		  // Problem with duplicates search
		
		  std::string value = o_itr->getValue();
		  /*
		    int pos = value.rfind("/",value.size());
		    if (pos != std::string::npos) {
		    std::string name = value.substr(pos+1,value.size());
		    std::string path = dir+"/"+value.substr(0, pos);
		    cout << "path:" << path << " name:" << name << endl;
		    }
		  */
		  // std::cout << dir << "/" << value << std:: endl;
		  olist.sort();

		}
		bmap[b_itr->getValue()] = olist;
	      }
	      //	      objlist[n_itr->getValue()] = bmap;
	    }
	  }
	}
    }
  catch (xdaq::exception::Exception& e)
    {
      return bmap;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      return bmap;
    }


  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Objects List is updated");
  return bmap;
}

std::map<std::string, std::list<std::string> > EmuTFDisplayClient::requestCanvasesList(xdaq::ApplicationDescriptor* monitor)
{

  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvasesList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPName originator ("originator", "", "");
  //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
  xoap::SOAPElement command = body.addBodyElement(commandName );
    
  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {
      if (monitor == NULL) return bmap;
      // appBSem_.take();
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestCanvasesList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *monitor);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      // appBSem_.give();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	  return bmap;
	} else {       
	  LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestCanvasesList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
	  //	  std::map<std::string, std::list<std::string> > bmap;
	  std::list<std::string> olist;
	  vector<xoap::SOAPElement> content = rb.getChildElements ();
	  xoap::SOAPName nodeTag ("DQMNode", "", "");
	  for (vector<xoap::SOAPElement>::iterator itr = content.begin();
	       itr != content.end(); ++itr) {
            
	    std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
	    for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
		 n_itr != nodeElement.end(); ++n_itr) {
	      std::stringstream stdir;
	      bmap.clear();
	      xoap::SOAPName branchTag ("Branch", "", "");

	      std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

	      for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
		   b_itr != branchElement.end(); ++b_itr) {

		olist.clear();
		stringstream stdir;
		string dir="";
		stdir.clear();
		dir = b_itr->getValue();
		// dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
		xoap::SOAPName objTag("Obj", "", "");

		vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

		for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
		     o_itr != objElement.end(); ++o_itr ) {
		  olist.push_back(o_itr->getValue());
		  std::string value = o_itr->getValue();
		  olist.sort();

		}
		bmap[b_itr->getValue()] = olist;
	      }
	
	    }
	  }
	}
    }
  catch (xdaq::exception::Exception& e)
    {
      return bmap;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      return bmap;
    }


  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Canvases List is updated");
  return bmap;
}


TMessage* EmuTFDisplayClient::requestObjects(xdata::Integer nodeaddr, std::string folder, std::string objname)
{
  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjects","xdaq", "urn:xdaq-soap:3.0");


  xoap::SOAPElement command = body.addBodyElement(commandName );

  xoap::SOAPName folderName = envelope.createName("Folder", "", "");
  xoap::SOAPElement folderElement = command.addChildElement(folderName);
  folderElement.addTextNode(folder);

  xoap::SOAPName objectName = envelope.createName("Object", "", "");
  xoap::SOAPElement objectElement = folderElement.addChildElement(objectName);
  objectElement.addTextNode(objname);
    
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
    {
      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      if (d==NULL) return buf;
      // appBSem_.take();
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestObjects to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *d);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      // appBSem_.give();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	  return buf;
	} else { 
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestObjects reply from " << d->getClassName() << " ID" << d->getLocalId());	  
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  std::list<xoap::AttachmentPart*>::iterator iter;
	  if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);
	  
	  for (iter = attachments.begin(); iter != attachments.end(); iter++)
	    {
	      int size = (*iter)->getSize();
	      char * content = (*iter)->getContent();

	      buf = new TMessage(kMESS_OBJECT);
	      buf->Reset();
	      buf->WriteBuf(content, size);
	      buf->Reset();
	      
	      map<string, std::string, less<string> > mimeHdrs = (*iter)->getAllMimeHeaders();
              std:: cout << "size:" << size << " ,name: " << (*iter)->getContentLocation() << std::endl;
	      // std:: cout << (*iter)->getContentLocation() << std::endl;
	      
	      // buf->Reset();
	      
	    }
	}
    } 
  catch (xdaq::exception::Exception& e)
    {
      return buf;
      // handle exception
    }	
  
  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Object is updated");
  return buf;
}

TMessage* EmuTFDisplayClient::requestCanvas(xdata::Integer nodeaddr, std::string folder, std::string objname, int width, int height)
{
  // Prepare SOAP Message for DQM Mode 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvas","xdaq", "urn:xdaq-soap:3.0");


  xoap::SOAPElement command = body.addBodyElement(commandName );

  xoap::SOAPName folderName = envelope.createName("Folder", "", "");
  xoap::SOAPElement folderElement = command.addChildElement(folderName);
  folderElement.addTextNode(folder);

  xoap::SOAPName objectName = envelope.createName("Canvas", "", "");
  xoap::SOAPElement objectElement = folderElement.addChildElement(objectName);
  objectElement.addTextNode(objname);

  xoap::SOAPName widthTag = envelope.createName("Width", "", "");
  objectElement.addAttribute(widthTag, xdata::Integer(width).toString());
  xoap::SOAPName heightTag = envelope.createName("Height", "", "");
  objectElement.addAttribute(heightTag, xdata::Integer(height).toString());

    
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
    {
      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      if (d==NULL) return buf;
      // appBSem_.take();
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestCanvas: \"" << folder << "/" << objname << "\" to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *d);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      // appBSem_.give();
      /*
	std::cout << std::endl;
	reply->writeTo(cout);
	std::cout << endl;
      */

      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	  return buf;
	} else { 
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestCanvas: \"" << folder << "/" << objname << "\" reply from " << d->getClassName() << " ID" << d->getLocalId());
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  std::list<xoap::AttachmentPart*>::iterator iter;
	  if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);

	  for (iter = attachments.begin(); iter != attachments.end(); iter++)
	    {
	      int size = (*iter)->getSize();
	      char * content = (*iter)->getContent();
	      if (buf != NULL) {
		delete buf;
	      }
	      buf = new TMessage(kMESS_OBJECT);
	      buf->Reset();
	      buf->WriteBuf(content, size);

	    }
	}
    } 
  catch (xdaq::exception::Exception& e)
    {
      return buf;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      return buf;
    }
	
  
  // LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Canvas is updated");
  return buf;
}

void EmuTFDisplayClient::updateFoldersMap()
{
  if (time(NULL)- foldersMap.getTimeStamp()>10) {
    // appBSem_.take();
    foldersMap.clear();
    std::set<xdaq::ApplicationDescriptor*> monitors = getAppsList(monitorClass_);
    if (!monitors.empty()) {
  

      LOG4CPLUS_DEBUG (getApplicationLogger(), "Start Monitoring Folders List updating");  
      std::set<xdaq::ApplicationDescriptor*>::iterator pos;

      for (pos=monitors.begin(); pos!=monitors.end(); ++pos) {
        if ((*pos) == NULL) continue;
	int nodeID= (*pos)->getLocalId();
	std::set<std::string> flist = requestFoldersList(*pos);
	std::set<std::string>::iterator litr;
	for(litr=flist.begin(); litr != flist.end(); ++litr) {
	  if (*litr != "")  {
		foldersMap[*litr].insert(nodeID);	
		// std::cout << *litr << std::endl;
	  }
	}      
      }
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Folders List is updated");
      foldersMap.setTimeStamp(time(NULL));
    }  
    // appBSem_.give();
  }
}


std::set<std::string>  EmuTFDisplayClient::requestFoldersList(xdaq::ApplicationDescriptor* dest)
{

  // Prepare SOAP Message for DQM Node 
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestFoldersList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPName originator ("originator", "", "");
  xoap::SOAPElement command = body.addBodyElement(commandName );
    
  std::set<std::string> flist;
  flist.clear();


  try
    {
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestFoldersList to " << dest->getClassName() << " ID" << dest->getLocalId());
      // appBSem_.take();
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *dest);

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      //appBSem_.give();
      if (rb.hasFault() )
	{
	  xoap::SOAPFault fault = rb.getFault();
	  std::string errmsg = "DQMNode: ";
	  errmsg += fault.getFaultString();
	  XCEPT_RAISE(xoap::exception::Exception, errmsg);
	  return flist;
	} else {       
	  LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestFoldersList reply from " << dest->getClassName() << " ID" << dest->getLocalId());

	  vector<xoap::SOAPElement> content = rb.getChildElements ();
	  xoap::SOAPName nodeTag ("DQMNode", "", "");
	  for (vector<xoap::SOAPElement>::iterator itr = content.begin();
	       itr != content.end(); ++itr) {
            
	    std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
	    for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
		 n_itr != nodeElement.end(); ++n_itr) {
	      std::stringstream stdir;
	      xoap::SOAPName folderTag ("Folder", "", "");

	      std::vector<xoap::SOAPElement> folderElement = n_itr->getChildElements (folderTag );

	      for (vector<xoap::SOAPElement>::iterator f_itr = folderElement.begin();
		   f_itr != folderElement.end(); ++f_itr) {


		flist.insert(f_itr->getValue());

	      }
	    }
	  }
	}
    }
  catch (xdaq::exception::Exception& e)
    {
      return flist;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      flist.clear();
      return flist;
    }


  //  LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Folders List is updated");
  return flist;
}

// == Get Application Descriptors for specified Data Server class name == //
std::set<xdaq::ApplicationDescriptor*> EmuTFDisplayClient::getAppsList(xdata::String className, xdata::String group)
{

  std::set<xdaq::ApplicationDescriptor*> applist;
  try
    {
      applist.clear();

      xdaq::ApplicationGroup *g = getApplicationContext()->getDefaultZone()->getApplicationGroup(group);
      if (g) applist =	g->getApplicationDescriptors(className.toString());
      // sort(applist.begin(), applist.end(), Compare_ApplicationDescriptors());
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_ERROR (getApplicationLogger(),
		       "No Applications with class name " << className.toString() <<
		       "found." << xcept::stdformat_exception_history(e));
      return applist;
    }
  catch (pt::exception::Exception& e)
    {
      applist.clear();
      return applist;
    }

  return applist;
}

