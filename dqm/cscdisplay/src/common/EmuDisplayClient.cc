#include <iostream>
#include <string>
#include <sstream>


#include "emu/dqm/cscdisplay/EmuDisplayClient.h"


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

using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuDisplayClient)

EmuDisplayClient::EmuDisplayClient(xdaq::ApplicationStub* stub)
throw (xdaq::exception::Exception)
    : xdaq::WebApplication(stub),
    emu::base::WebReporter(stub),
    Task("EmuDisplayClient"),
    monitorClass_("EmuMonitor"),
    imageFormat_("png"),
    imagePath_("images"),
    viewOnly_(true),
    debug(false),
    BaseDir("/csc_data/dqm"),
    refImagePath("ref.plots"),
    saveResultsDelay(10),
    appBSem_(BSem::FULL)
{


  appBSem_.take();

  bsem_tout.tv_sec=10;
  bsem_tout.tv_usec=0;
  xmlHistosBookingCfgFile_ = "";
  xmlCanvasesCfgFile_ = "";
  cscMapFile_ =  "";

  curRunNumber = "";
  tmap = emu::dqm::utils::getCSCTypeToBinMap();

  errorHandler_ = toolbox::exception::bind (this, &EmuDisplayClient::onError, "onError");


  // Bind CGI callbacks
  xgi::bind(this, &EmuDisplayClient::getRefPlot, "getRefPlot");
  xgi::bind(this, &EmuDisplayClient::getPlot, "getPlot");
  xgi::bind(this, &EmuDisplayClient::getNodesStatus, "getNodesStatus");
  xgi::bind(this, &EmuDisplayClient::getCSCList, "getCSCList");
  xgi::bind(this, &EmuDisplayClient::getTestsList, "getTestsList");
  xgi::bind(this, &EmuDisplayClient::getRunsList, "getRunsList");
  xgi::bind(this, &EmuDisplayClient::getCSCCounters, "getCSCCounters");
  xgi::bind(this, &EmuDisplayClient::getDQMReport, "getDQMReport");
  xgi::bind(this, &EmuDisplayClient::controlDQM, "controlDQM");
  xgi::bind(this, &EmuDisplayClient::redir, "redir");


  getApplicationInfoSpace()->fireItemAvailable("monitorClass",&monitorClass_);
  getApplicationInfoSpace()->addItemChangedListener ("monitorClass", this);

  getApplicationInfoSpace()->fireItemAvailable("imageFormat",&imageFormat_);
  getApplicationInfoSpace()->addItemChangedListener ("imageFormat", this);

  getApplicationInfoSpace()->fireItemAvailable("imagePath",&imagePath_);

  getApplicationInfoSpace()->fireItemAvailable("viewOnly",&viewOnly_);
  getApplicationInfoSpace()->addItemChangedListener ("viewOnly", this);

  getApplicationInfoSpace()->fireItemAvailable("baseDir",&BaseDir);
  getApplicationInfoSpace()->fireItemAvailable("resultsDir",&ResultsDir);

  getApplicationInfoSpace()->fireItemAvailable("debug",&debug);
  getApplicationInfoSpace()->addItemChangedListener ("debug", this);

  getApplicationInfoSpace()->fireItemAvailable("xmlCfgFile",&xmlHistosBookingCfgFile_);
  getApplicationInfoSpace()->addItemChangedListener ("xmlCfgFile", this);

  getApplicationInfoSpace()->fireItemAvailable("xmlCanvasesCfgFile",&xmlCanvasesCfgFile_);
  getApplicationInfoSpace()->addItemChangedListener ("xmlCanvasesCfgFile", this);

  getApplicationInfoSpace()->fireItemAvailable("cscMapFile",&cscMapFile_);
  getApplicationInfoSpace()->addItemChangedListener ("cscMapFile", this);
 
  getApplicationInfoSpace()->fireItemAvailable("saveResultsDelay",&saveResultsDelay);
  getApplicationInfoSpace()->addItemChangedListener ("saveResultsDelay", this);


  book();
  // === Initialize ROOT system
  if (!gApplication)
    TApplication::CreateApplication();

  gStyle->SetPalette(1,0);
  monitors = getAppsList(monitorClass_);
  readRunsList();

  updateNodesStatus();
  updateFoldersMap();
  updateCSCCounters();
  /*
  dqm_report.clearReport();
  dqm_report = updateNodesReports();
  generateSummaryReport(curRunNumber, dqm_report);
  */
  this->activate();

  appBSem_.give();
}

void EmuDisplayClient::cleanup()
{
  LOG4CPLUS_WARN(getApplicationLogger(),"Cleanup Called" );

  clearCanvasesCollection(MECanvases);
  clearMECollection(MEs);

  clearCanvasesCollection(MECanvasFactories);
  clearMECollection(MEFactories);
}


EmuDisplayClient::~EmuDisplayClient()
{
  cleanup();
}



bool EmuDisplayClient::onError ( xcept::Exception& ex, void * context )
{
  LOG4CPLUS_INFO (getApplicationLogger(),"onError: " << ex.what());
  return false;
}

// == Run Control requests current parameter values == //
void EmuDisplayClient::actionPerformed (xdata::Event& e)
{
  if (e.type() == "ItemChangedEvent")
    {
      std::string item = dynamic_cast<xdata::ItemChangedEvent&>(e).itemName();
      // std::cout << item << std::endl;

      if ( item == "monitorClass")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "monitor Class : " << monitorClass_.toString());
          monitors.clear();
          monitors = getAppsList(monitorClass_);
        }
      else if ( item == "xmlCfgFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         "Histograms Booking XML Config File for plotter : " << xmlHistosBookingCfgFile_.toString());
          clearMECollection(MEs);
          loadXMLBookingInfo(xmlHistosBookingCfgFile_.toString());

        }
      else if ( item == "xmlCanvasesCfgFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         "Canvases XML Config File for plotter : " << xmlCanvasesCfgFile_.toString());
          clearCanvasesCollection(MECanvases);
          loadXMLCanvasesInfo(xmlCanvasesCfgFile_.toString());
        }
      else if ( item == "cscMapFile")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         "CSC Mapping File for plotter : " << cscMapFile_.toString());
          setCSCMapFile(cscMapFile_.toString());

        }      
      else if ( item == "saveResultsDelay")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         "Save Nodes Results request delay : " << saveResultsDelay.toString() <<"s");

        }


    }

}


// XGI Call back

void EmuDisplayClient::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  ifstream index;
  index.open( (BaseDir.toString()+"/index.html").c_str());
  if (index)
    {
      *out << index.rdbuf();
    }
}


FoldersMap EmuDisplayClient::readFoldersMap(std::string run)
{
  FoldersMap folders;
  std::string rootfile = ResultsDir.toString()+"/"+run+".root";
  std::string runplots = run+".plots";
  std::string csc_list = "csc_list.js";
  struct stat attrib;                   // create a file attribute structure
  std::vector<std::string>::iterator r_itr = find(runsList.begin(), runsList.end(), run+".root");
  if (r_itr != runsList.end())
    {
      if (stat(rootfile.c_str(), &attrib) == 0)    // Folder exists
        {
          // std::cout << "Found run " << run << std::endl;
          TFile* rootsrc = TFile::Open( rootfile.c_str());
          if (!rootsrc)
            {
              LOG4CPLUS_ERROR (getApplicationLogger(), "Unable to open " << rootfile.c_str());
              return folders;
            }
          if (!rootsrc->cd("DQMData"))
            {
              LOG4CPLUS_ERROR (getApplicationLogger(), "No histos folder in file");
              return folders;
            }


          TDirectory *sourcedir = gDirectory;

          TIter nextkey( sourcedir->GetListOfKeys() );
          TKey *key;

          while ( (key = (TKey*)nextkey()))
            {
              TObject *obj = key->ReadObj();
              if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) // --- Found Directory
                {
                  std::string name=obj->GetName();
                  //      cout << name << endl;
                  if ((name.find("EMU") != std::string::npos)
                      || (name.find("DDU") != std::string::npos)
                      || (name.find("CSC") != std::string::npos))
                    {
                      folders[name].insert(0);
                    }
                }
              delete obj;
            }
          rootsrc->Close();

        }

    }

  return folders;
}

bool EmuDisplayClient::isCSCListFileAvailable(std::string runname)
{
  struct stat attrib;                   // create a file attribute structure
  std::string runplots = runname+".plots";
  std::string csc_list = "csc_list.js";
  if (stat((ResultsDir.toString()+"/"+runplots+"/"+csc_list).c_str(), &attrib) == 0) return true;
  else return false;
}

void EmuDisplayClient::getCSCList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();
  std::string runname="";
  std::string user_host = in->getenv("REMOTE_HOST");
  FoldersMap folders;
  try
    {
      cgicc::Cgicc cgi(in);
      cgicc::const_form_iterator stateInputElement = cgi.getElement("run");
      if (stateInputElement != cgi.getElements().end())
        {
          runname = (*stateInputElement).getValue();
          //       std::cout << runname << std::endl;
        }

      if (runname.find("Online") != std::string::npos || runname =="")
        {
          *out << "var CSC_LIST=[";
          *out << "['Online Run'";
          folders = foldersMap;// updateFoldersMap();
        }
      else
        {
          if (isCSCListFileAvailable(runname))
            {
              ifstream map;
              map.open( (ResultsDir.toString()+"/"+runname+".plots/csc_list.js").c_str());
              if (map)
                {
                  *out << map.rdbuf();
                }
              else
                {
                  // == Empty map
                  *out << "var CSC_LIST=[['Run"<< runname << "']]" << std::endl;
                }
              map.close();
              // Read CSC list from available file
              appBSem_.give();
              return;
            }
          else
            {
              *out << "var CSC_LIST=[";
              *out << "['"<< runname << "'";
              folders = readFoldersMap(runname);
            }
        }

      if (!folders.empty())
        {
          std::vector<std::string> EMU_folders;
          std::vector<std::string> DDU_folders;
          std::vector<std::string> CSC_folders;
          std::map<std::string, std::set<int> >::const_iterator itr;
          for (itr=folders.begin(); itr != folders.end(); ++itr)
            {

              std::string folder = itr->first;
              if (folder.find("EMU") != std::string::npos) EMU_folders.push_back(folder);
              else if (folder.find("DDU") != std::string::npos) DDU_folders.push_back(folder);
              else if (folder.find("CSC") != std::string::npos) CSC_folders.push_back(folder);
            }

          if (EMU_folders.size())
            {
              *out << ",\n['EMU',[";
              for (uint32_t i=0; i<EMU_folders.size(); i++ )
                {
                  *out << "'"<< EMU_folders[i] << ((i==EMU_folders.size()-1)?"'":"',");
                }
              *out << "]]";
            }
          if (DDU_folders.size())
            {
              *out << ",\n['DDU',[";
              for (uint32_t i=0; i<DDU_folders.size(); i++ )
                {
                  *out << "'"<< DDU_folders[i] << ((i==DDU_folders.size()-1)?"'":"',");
                }
              *out << "]]";
            }


          // Quick fix !!!TODO: Sorted output
          if (CSC_folders.size())
            {
              int crate=0;
              int slot=0;
              int cur_crate=-1;
              for (uint32_t i=0; i<CSC_folders.size(); i++ )
                {
                  std::string csc_ptrn = "CSC_%d_%d";
                  if (sscanf(CSC_folders[i].c_str(),csc_ptrn.c_str(), &crate, &slot) == 2)
                    {
                      if (crate != cur_crate)
                        {
                          if (cur_crate>=0) *out << "]]";
                          *out << ",\n['crate"<< crate << "', [";
                        }
                      *out << "'slot" << slot << "',";
                    }
                  cur_crate = crate;

                }
              *out << "]]";
            }
        }
    }
  catch (xoap::exception::Exception &e)
    {
      if (debug) LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to getCSCList: "
                                   << xcept::stdformat_exception_history(e));
    }

  *out << "]]" << std::endl;
  appBSem_.give();

}

bool EmuDisplayClient::isCSCCountersFileAvailable(std::string runname)
{
  struct stat attrib;                   // create a file attribute structure
  std::string runplots = runname+".plots";
  std::string report_file = "csc_counters.js";
  if (stat((ResultsDir.toString()+"/"+runplots+"/"+report_file).c_str(), &attrib) == 0) return true;
  else return false;
}



void EmuDisplayClient::getCSCCounters (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take();

  std::string runname="";

  cgicc::Cgicc cgi(in);
  cgicc::const_form_iterator stateInputElement = cgi.getElement("run");
  if (stateInputElement != cgi.getElements().end())
    {
      runname = (*stateInputElement).getValue();
      //       std::cout << runname << std::endl;
    }

  if (runname.find("Online") != std::string::npos || runname =="")
    {

      *out << "var CSC_COUNTERS=[" << std::endl;
      *out << "['Online Run'," << std::endl;
      try
        {
          // updateCSCCounters();
          if (!cscCounters.empty())
            {
              CSCCounters::iterator citr;
              for (citr=cscCounters.begin(); citr != cscCounters.end(); ++citr)
                {
                  *out << "['" << citr->first << "',[";
                  std::map<std::string, std::string>::iterator itr;
                  for (itr=citr->second.begin(); itr != citr->second.end(); ++itr)   // == Loop and Output Counters
                    {
                      *out << "['"<< itr->first << "','" << itr->second <<"'],";
                    }
                  *out << "]]," << std::endl;
                }
            }
        }
      catch (xoap::exception::Exception &e)
        {
          if (debug) LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to getCSCCounters: "
                                       << xcept::stdformat_exception_history(e));

        }

      *out << "]]" << std::endl;

    }
  else
    {
      if (isCSCCountersFileAvailable(runname))
        {
          ifstream counters;
          counters.open( (ResultsDir.toString()+"/"+runname+".plots/csc_counters.js").c_str());
          if (counters)
            {
              *out << counters.rdbuf();
            }
          else
            {
              // == Empty map
              *out << "var CSC_COUNTERS=[['Run: "<< runname << "']]" << std::endl;
            }
          counters.close();
          // Read CSC list from available file
          appBSem_.give();
          return;
        }
      else
        {
          *out << "var CSC_COUNTERS=[['Run: "<< runname << "']]" << std::endl;
        }

    }

  appBSem_.give();

}

bool EmuDisplayClient::isDQMReportFileAvailable(std::string runname)
{
  struct stat attrib;                   // create a file attribute structure
  std::string runplots = runname+".plots";
  std::string report_file = "dqm_report.js";
  if (stat((ResultsDir.toString()+"/"+runplots+"/"+report_file).c_str(), &attrib) == 0) return true;
  else return false;
}


void EmuDisplayClient::getDQMReport (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take();

  std::string runname="";

  cgicc::Cgicc cgi(in);
  cgicc::const_form_iterator stateInputElement = cgi.getElement("run");
  if (stateInputElement != cgi.getElements().end())
    {
      runname = (*stateInputElement).getValue();
      //       std::cout << runname << std::endl;
    }

  if (runname.find("Online") != std::string::npos || runname =="")
    {
      /*
      dqm_report.clearReport();
      updateReport();
      generateReport(curRunNumber);
      */
      *out << getReportJSON(curRunNumber);
    }
  else
    {
      if (isDQMReportFileAvailable(runname))
        {
          ifstream report;
          report.open( (ResultsDir.toString()+"/"+runname+".plots/dqm_report.js").c_str());
          if (report)
            {
              *out << report.rdbuf();
            }
          else
            {
              // == Empty map
              *out << "var DQM_REPORT = { \"run\": \"" << runname << "\", \"genDate\": \"" << emu::dqm::utils::now() << "\"};";
            }
          report.close();
          // Read CSC list from available file
          appBSem_.give();
          return;
        }
      else
        {
          *out << "var DQM_REPORT = { \"run\": \"" << runname << "\", \"genDate\": \"" << emu::dqm::utils::now() << "\"};";
        }

    }

  appBSem_.give();

}

void EmuDisplayClient::redir (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
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

void EmuDisplayClient::controlDQM (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();
  cgicc::Cgicc cgi(in);
  std::string user_host = in->getenv("REMOTE_HOST");

  std::string action = "";
  std::string node="ALL";

  cgicc::const_form_iterator actionInputElement = cgi.getElement("action");
  if (actionInputElement != cgi.getElements().end())
    {
      action = (*actionInputElement).getValue();
      LOG4CPLUS_INFO(getApplicationLogger(), "Action request: " << action << " from " << user_host);
    }

  cgicc::const_form_iterator nodeInputElement = cgi.getElement("node");
  if (nodeInputElement != cgi.getElements().end())
    {
      node = (*nodeInputElement).getValue();
    }


  if (!monitors.empty())
    {
      std::set<xdaq::ApplicationDescriptor*>::iterator mon;

      for (mon=monitors.begin(); mon!=monitors.end(); ++mon)
        {
          if (node == "ALL")
            {
              try
                {
                  emu::dqm::sendFSMEventToApp(action, getApplicationContext(), getApplicationDescriptor(),*mon);
                }
              catch (xcept::Exception e)
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

void EmuDisplayClient::saveNodesResults()
{
  std::string action = "saveResults";
  LOG4CPLUS_INFO (getApplicationLogger(), "Save Nodes Results");
  if (!monitors.empty())
    {
      std::set<xdaq::ApplicationDescriptor*>::iterator mon;

      for (mon=monitors.begin(); mon!=monitors.end(); ++mon)
        {
          try
            {
	      xoap::MessageReference msg = xoap::createMessage();
	      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
	      xoap::SOAPBody body = envelope.getBody();
	      xoap::SOAPName commandName = envelope.createName(action,"xdaq", "urn:xdaq-soap:3.0");


	      xoap::SOAPElement command = body.addBodyElement(commandName );
	      /*
	      xoap::SOAPName timeStamp = envelope.createName("TimeStamp", "", "");
	      xoap::SOAPElement folderElement = command.addChildElement(folderName);
	      folderElement.addTextNode(tstamp);
	      */

	      // LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending saveResults command to " << (*mon)->getClassName() << " ID" << (*mon)->getLocalId());
	      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *(*mon));

	      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
	      if (rb.hasFault() )
		{
		  xoap::SOAPFault fault = rb.getFault();
		  std::string errmsg = "DQMNode: ";
		  errmsg += fault.getFaultString();
		  XCEPT_RAISE(xoap::exception::Exception, errmsg);
		  
		  continue;
		}
	      else
		{
		  //		  LOG4CPLUS_INFO (getApplicationLogger(), "Sent saveResults command to " << (*mon)->getClassName() << " ID" << (*mon)->getLocalId());
		}


              // emu::dqm::sendFSMEventToApp(action, getApplicationContext(), getApplicationDescriptor(),*mon);
            }
          catch (xcept::Exception e)
            {
              //      stringstream oss;

              //      oss << "Failed to " << action << " ";
              //      oss << (*mon)->getClassName() << (*mon)->getInstance();

              //      XCEPT_RETHROW(emuDAQManager::exception::Exception, oss.str(), e);

              // Don't raise exception here. Go on to try to deal with the others.
              LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to send " << action << " "
                              << (*mon)->getClassName() << (*mon)->getInstance() << " "
                              << xcept::stdformat_exception_history(e));
            }

        }

    }
}


void EmuDisplayClient::getTestsList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take();
  // == Temporary
  // == TODO: Request or load actual tests list
  ifstream map;
  map.open( (BaseDir.toString()+"/canvases_list.js").c_str());
  if (map)
    {
      *out << map.rdbuf();
    }
  else
    {
      // == Empty map
      *out << "var TREE_ITEMS=[]" << std::endl;
    }
  map.close();
  appBSem_.give();
}


vector<std::string> EmuDisplayClient::readRunsList()
{
  runsList.clear();
  DIR *dp;
  struct dirent *dirp;
  // struct stat attrib;                   // create a file attribute structure

  if ((dp  = opendir(ResultsDir.toString().c_str())) == NULL)
    {
      LOG4CPLUS_ERROR (getApplicationLogger(), "Error(" << errno << ") opening " << ResultsDir.toString());
    }
  else
    {

      while ((dirp = readdir(dp)) != NULL)
        {
          std::string fn = string(dirp->d_name);
          if (fn.find(".root") != std::string::npos)
            {
              //        stat((ResultsDir.toString()+"/"+fn).c_str(), &attrib);            // get the attributes of afile.txt
              //        fn += " ("+emu::dqm::utils::now(attrib.st_mtime)+")";        // Get the last modified time and put it into the time structure
              runsList.push_back(fn);
            }
        }
      closedir(dp);
    }

  sort( runsList.rbegin(), runsList.rend() );
  return runsList;
}

void EmuDisplayClient::getRunsList (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take();
  // == TODO: Read list of available DQM results root files from resultsDir
  std::vector<std::string> runs_list = readRunsList();
  struct stat attrib;                   // create a file attribute structure

  *out << "var RUNS=[" << std::endl;
  for (unsigned i=0; i <runs_list.size(); i++)
    {
      stat((ResultsDir.toString()+"/"+runs_list[i]).c_str(), &attrib);            // get the attributes of afile.txt
      *out << "['" << runs_list[i] << " (" << emu::dqm::utils::now(attrib.st_mtime) << ")" << "']," << std::endl;
    }
  *out << "]" << std::endl;
  appBSem_.give();
}



void EmuDisplayClient::getNodesStatus (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();
  *out << "var NODES_LIST=[" << std::endl;
  *out << "['Node','State','Run Number','DAQ Events','DQM Events','Rate (Evt/s)','Detected CSCs', 'Unpacked CSCs','Rate (CSCs/s)','Readout Mode','Data Source','Last event timestamp']," << std::endl;

  if (!nodesStatus.empty())
    {
      DQMNodesStatus::iterator nitr;
      for (nitr=nodesStatus.begin(); nitr != nodesStatus.end(); ++nitr)
        {
          std::string nodename = nitr->first;
          std::map<std::string, std::string>::iterator itr;

          std::string applink = "NA";
          std::string state =  "NA";
          std::string stateChangeTime = "NA";
          std::string runNumber   = "NA";
          std::string events = "0";
          std::string dataRate   = "0";
          std::string cscUnpacked   = "0";
          std::string cscDetected   = "0";
          std::string cscRate   = "0";
          std::string readoutMode   = "NA";
          std::string lastEventTime = "NA";
          std::string nDAQevents = "0";
          std::string dataSource = "NA";

          if ((itr = nitr->second.find("appLink")) != nitr->second.end())
            {
              applink = nitr->second["appLink"];

            }
          if ((itr = nitr->second.find("stateName")) != nitr->second.end())
            {
              state = nitr->second["stateName"];
            }

          if (!viewOnly_)
            {
              applink += "/showControl";
            }
          if (state == "")
            {
              state = "Unknown/Dead";
              continue;
            }
          else
            {
              if ((itr = nitr->second.find("stateChangeTime")) != nitr->second.end())
                {
                  stateChangeTime = nitr->second["stateChangeTime"];
                  state += " at " + stateChangeTime;
                }
            }


          if ((itr = nitr->second.find("runNumber")) != nitr->second.end())
            {
              runNumber = nitr->second["runNumber"];
            }

          if ((itr = nitr->second.find("sessionEvents")) != nitr->second.end())
            {
              events = nitr->second["sessionEvents"];
            }


          if ((itr = nitr->second.find("averageRate")) != nitr->second.end())
            {
              dataRate = nitr->second["averageRate"];
            }

          if ((itr = nitr->second.find("cscUnpacked")) != nitr->second.end())
            {
              cscUnpacked = nitr->second["cscUnpacked"];
            }

          if ((itr = nitr->second.find("cscDetected")) != nitr->second.end())
            {
              cscDetected = nitr->second["cscDetected"];
            }

          if ((itr = nitr->second.find("cscRate")) != nitr->second.end())
            {
              cscRate = nitr->second["cscRate"];
            }

          if ((itr = nitr->second.find("readoutMode")) != nitr->second.end())
            {
              readoutMode = nitr->second["readoutMode"];
            }

          if ((itr = nitr->second.find("dataSource")) != nitr->second.end())
            {
              dataSource = nitr->second["dataSource"];
            }

          if ((itr = nitr->second.find("nDAQEvents")) != nitr->second.end())
            {
              nDAQevents = nitr->second["nDAQevents"];
            }

          if ((itr = nitr->second.find("lastEventTime")) != nitr->second.end())
            {
              lastEventTime = nitr->second["lastEventTime"];
            }

          *out << "['"<< nodename << "','"<< applink << "','" << state
          << "','" << runNumber << "','" << nDAQevents << "','" << events
          << "','" << dataRate << "','"  << cscDetected << "','"<< cscUnpacked << "','" << cscRate
          << "','" << readoutMode << "','" << dataSource << "','" << lastEventTime
          << "']" << "," << std::endl;

        }
    }

  *out << "]" << std::endl;

  appBSem_.give();

}



TCanvas* EmuDisplayClient::getMergedCanvas(std::vector<TObject*>& canvases)
{
  TCanvas* cnv = NULL;
  if (canvases.size())
    {
      cnv=reinterpret_cast<TCanvas*>(canvases[0]->Clone());
      int npads = cnv->GetListOfPrimitives()->Capacity();
      for (uint32_t i=1; i< canvases.size(); i++)
        {
          TCanvas* cnv2 = reinterpret_cast<TCanvas*>(canvases[i]);
          int npads2 = cnv2->GetListOfPrimitives()->Capacity();
          if (npads2 == npads)
            {
              TIter citr(cnv->GetListOfPrimitives());
              TIter citr2(cnv2->GetListOfPrimitives());
              TObject *subpad, *subpad2;
              while ((subpad=citr()) && (subpad2=citr2()))
                {
                  if (subpad->InheritsFrom(TPad::Class())
                      && subpad2->InheritsFrom(TPad::Class()) )
                    {
                      TIter itr(reinterpret_cast<TPad*>(subpad)->GetListOfPrimitives());
                      TIter itr2(reinterpret_cast<TPad*>(subpad2)->GetListOfPrimitives());
                      TObject* obj, *obj2;
                      while ((obj=itr()) && (obj2=itr2()))
                        {
                          if ( obj && obj2
                               && obj->InheritsFrom(TH1::Class())
                               && obj2->InheritsFrom(TH1::Class()))
                            {
                              // double max1=reinterpret_cast<TH1*>(obj)->GetMaximum();
                              double max2=reinterpret_cast<TH1*>(obj2)->GetMaximum();
                              //double min1=reinterpret_cast<TH1*>(obj)->GetMinimum();
                              double min2=reinterpret_cast<TH1*>(obj2)->GetMinimum();
                              (reinterpret_cast<TH1*>(obj))->Add(reinterpret_cast<TH1*>(obj2));
                              double max1=reinterpret_cast<TH1*>(obj)->GetMaximum();
                              double min1=reinterpret_cast<TH1*>(obj)->GetMinimum();
                              // LOG4CPLUS_INFO(getApplicationLogger(), obj->GetName() << " " << max1 << " " << min1 << " " << max2 << " " << min2);
                              if ((max1 == min1) && (max1 == 0))
                                {
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

void EmuDisplayClient::getPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();
  cgicc::Cgicc cgi(in);
  int width=1200;
  int height=900;
  std::string objname = "";
  std::string folder = "";
  std::string runname = "";

  std::string user_host = in->getenv("REMOTE_HOST");


  cgicc::const_form_iterator stateInputElement = cgi.getElement("objectName");
  if (stateInputElement != cgi.getElements().end())
    {
      objname = (*stateInputElement).getValue();
    }

  stateInputElement = cgi.getElement("folder");
  if (stateInputElement != cgi.getElements().end())
    {
      folder = (*stateInputElement).getValue();
    }

  stateInputElement = cgi.getElement("imageWidth");
  if (stateInputElement != cgi.getElements().end())
    {
      width = (*stateInputElement).getIntegerValue();
    }

  stateInputElement = cgi.getElement("imageHeight");
  if (stateInputElement != cgi.getElements().end())
    {
      height = (*stateInputElement).getIntegerValue();
    }

  stateInputElement = cgi.getElement("run");
  if (stateInputElement != cgi.getElements().end())
    {
      runname = (*stateInputElement).getValue();
//      std::cout << "Run " << runname << std::endl;
    }


  if (runname == "Online" || runname == "")
    {
      std::map<std::string, std::set<int> >::iterator itr = foldersMap.find(folder);
      if ((itr == foldersMap.end()) || itr->second.empty())
        {
          if (debug) LOG4CPLUS_WARN (getApplicationLogger(), "Can not locate request node for " << folder);
          appBSem_.give();
          return;
        }

    }

  LOG4CPLUS_INFO (getApplicationLogger(), "Request for : " << folder << "/" << objname);

  EmuMonitoringCanvas* cnv = NULL;


  if (bookCanvas(folder, objname, emu::dqm::utils::genCSCTitle(folder), cnv))
    {
      std::map<std::string, std::string> pads = cnv->getListOfPads();
      std::map<std::string, std::string>::iterator p_itr;

      uint32_t booked_pads=0;
      for (p_itr = pads.begin(); p_itr != pads.end(); ++p_itr)
        {

          EmuMonitoringObject* mo = 0;
          if (bookME(folder, p_itr->second, emu::dqm::utils::genCSCTitle(folder), mo))
            {
              if (updateME(folder, p_itr->second, mo, runname) )  booked_pads++;
            }
          else
            {
              LOG4CPLUS_ERROR (getApplicationLogger(), "Can not book " << p_itr->first << " object " << p_itr->second);
            };
          //usleep(200000);
        }


      bool fBooked=((pads.size()!=0) && (booked_pads==pads.size()));

      if (fBooked)
        {
          // Update Efficiency plots
          if (objname.find("EMU_Status") != std::string::npos) updateEfficiencyHistos(runname);

          cnv->reset();
          //      const time_t t = cnv->getTimestamp();
          //std::cout << asctime(localtime(&t)) << std::endl;
          // LOG4CPLUS_INFO (getApplicationLogger(), "All objects for Canvas " << objname << " are booked");
          cnv->setCanvasWidth(width);
          cnv->setCanvasHeight(height);
          cnv->setRunNumber(curRunNumber);

          cnv->Draw(MEs[folder],true);


          TCanvas* cnv_obj =cnv->getCanvasObject();
          if (cnv_obj != 0)
            {
              TImage *img = TImage::Create ();
              char   *data = NULL;
              int    size = 0;
              img->FromPad(cnv->getCanvasObject());
              std::string imgname=BaseDir.toString()+"/"+imagePath_.toString()+"/"+folder+"_"+objname+"."+imageFormat_.toString();
              //cnv->Print(imgname.c_str());
              //img->ReadImage(imgname.c_str());
              img->Gray(false);
              img->GetImageBuffer(&data, &size, TImage::kPng);

              (out->getHTTPResponseHeader()).addHeader("Content-Type ","image/png");
              out->write(data, size);

              // LOG4CPLUS_INFO (getApplicationLogger(), "Show plot: \"" << folder << "/" << objname << "\" for " << user_host);
              free (data);
              if (img!=NULL) delete img;
            }
          else
            {
              LOG4CPLUS_ERROR (getApplicationLogger(), "Can not draw empty canvas object");
            }

        }

    }
  else
    {
      LOG4CPLUS_ERROR (getApplicationLogger(), "Can not book canvas");
    }
  appBSem_.give();

}

/*
void EmuDisplayClient::getPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  appBSem_.take();
  cgicc::Cgicc cgi(in);
  int width=1200;
  int height=900;
  std::string objname = "";
  std::string folder = "";

  std::string user_host = in->getenv("REMOTE_HOST");


  cgicc::const_form_iterator stateInputElement = cgi.getElement("objectName");
  if (stateInputElement != cgi.getElements().end())
    {
      objname = (*stateInputElement).getValue();
    }

  stateInputElement = cgi.getElement("folder");
  if (stateInputElement != cgi.getElements().end())
    {
      folder = (*stateInputElement).getValue();
    }

  stateInputElement = cgi.getElement("imageWidth");
  if (stateInputElement != cgi.getElements().end())
    {
      width = (*stateInputElement).getIntegerValue();
    }

  stateInputElement = cgi.getElement("imageHeight");
  if (stateInputElement != cgi.getElements().end())
    {
      height = (*stateInputElement).getIntegerValue();
    }


  std::map<std::string, std::set<int> >::iterator itr = foldersMap.find(folder);
  if ((itr == foldersMap.end()) || itr->second.empty())
    {
      LOG4CPLUS_WARN (getApplicationLogger(), "Can not locate request node for " << folder);
      appBSem_.give();
      return;
    }


  LOG4CPLUS_INFO (getApplicationLogger(), "Request for : " << folder << "/" << objname);

  EmuMonitoringCanvas* cnv = NULL;


  if (bookCanvas(folder, objname, emu::dqm::utils::genCSCTitle(folder), cnv))
    {

      bool fUpdated = updateCanvas(folder, objname, emu::dqm::utils::genCSCTitle(folder), cnv);

      if (fUpdated)
        {
          // Update Efficiency plots
          if (objname.find("EMU_Status") != std::string::npos) updateEfficiencyHistos();

          cnv->reset();
          //      const time_t t = cnv->getTimestamp();
          //std::cout << asctime(localtime(&t)) << std::endl;
          // LOG4CPLUS_INFO (getApplicationLogger(), "All objects for Canvas " << objname << " are booked");
          cnv->setCanvasWidth(width);
          cnv->setCanvasHeight(height);
          cnv->setRunNumber(curRunNumber);

          cnv->Draw(MEs[folder],true);


          TCanvas* cnv_obj =cnv->getCanvasObject();
          if (cnv_obj != 0)
            {
              TImage *img = TImage::Create ();
              char   *data = NULL;
              int    size = 0;
              img->FromPad(cnv->getCanvasObject());
              std::string imgname=BaseDir.toString()+"/"+imagePath_.toString()+"/"+folder+"_"+objname+"."+imageFormat_.toString();
              //cnv->Print(imgname.c_str());
              //img->ReadImage(imgname.c_str());
              img->Gray(false);
              img->GetImageBuffer(&data, &size, TImage::kPng);

              (out->getHTTPResponseHeader()).addHeader("Content-Type ","image/png");
              out->write(data, size);

              // LOG4CPLUS_INFO (getApplicationLogger(), "Show plot: \"" << folder << "/" << objname << "\" for " << user_host);
              free (data);
              if (img!=NULL) delete img;
            }
          else
            {
              LOG4CPLUS_ERROR (getApplicationLogger(), "Can not draw empty canvas object");
            }

        }

    }
  else
    {
      LOG4CPLUS_ERROR (getApplicationLogger(), "Can not book canvas");
    }
  appBSem_.give();

}
*/

void EmuDisplayClient::getRefPlot (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  appBSem_.take();
  cgicc::Cgicc cgi(in);
  std::string plot = "";
  std::string folder = "";

  std::string user_host = in->getenv("REMOTE_HOST");

  cgicc::const_form_iterator stateInputElement = cgi.getElement("plot");
  if (stateInputElement != cgi.getElements().end())
    {
      plot = (*stateInputElement).getValue();
    }

  stateInputElement = cgi.getElement("folder");
  if (stateInputElement != cgi.getElements().end())
    {
      folder = (*stateInputElement).getValue();
    }

  TImage *img = TImage::Create ();
  char   *data = 0;
  int    size = 0;

  if (folder.find("CSC_") ==0)   // Convert CSC_XXX_YY to crateX/slotX string
    {
      int crate=0, slot=0;
      int n = sscanf(folder.c_str(), "CSC_%03d_%02d", &crate, &slot);
      if (n==2) folder = Form("crate%d/slot%d",crate,slot);
    }

  std::string plotpath=BaseDir.toString()+"/"+refImagePath.toString()+"/"+folder+"/"+plot;
  struct stat stats;
  if (stat(plotpath.c_str(), &stats)<0)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), plotpath << ": " <<
                     strerror(errno));
    }
  else
    {
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


std::map<std::string, std::list<std::string> > EmuDisplayClient::requestObjectsList(xdaq::ApplicationDescriptor* monitor)
{

  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {

      // Prepare SOAP Message for DQM Mode
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName commandName = envelope.createName("requestObjectsList","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName originator ("originator", "", "");
      //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
      xoap::SOAPElement command = body.addBodyElement(commandName );

      if (monitor == NULL) return bmap;
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestObjectsList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *monitor);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return bmap;
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestObjectsList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
          //	  std::map<std::string, std::list<std::string> > bmap;
          std::list<std::string> olist;
          vector<xoap::SOAPElement> content = rb.getChildElements ();
          xoap::SOAPName nodeTag ("DQMNode", "", "");
          for (vector<xoap::SOAPElement>::iterator itr = content.begin();
               itr != content.end(); ++itr)
            {

              std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);

              for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                   n_itr != nodeElement.end(); ++n_itr)
                {
                  std::stringstream stdir;
                  bmap.clear();
                  xoap::SOAPName branchTag ("Branch", "", "");

                  std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

                  for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
                       b_itr != branchElement.end(); ++b_itr)
                    {

                      olist.clear();
                      stringstream stdir;
                      string dir="";
                      stdir.clear();
                      dir = b_itr->getValue();
                      // dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
                      xoap::SOAPName objTag("Obj", "", "");

                      vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

                      for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
                           o_itr != objElement.end(); ++o_itr )
                        {
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
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      // handle exception
      return bmap;
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

std::map<std::string, std::list<std::string> > EmuDisplayClient::requestCanvasesList(xdaq::ApplicationDescriptor* monitor)
{

  std::map<std::string, std::list<std::string> > bmap;
  bmap.clear();
  // Get reply from DQM node and populate TConsumerInfo list
  try
    {

      // Prepare SOAP Message for DQM Mode
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName commandName = envelope.createName("requestCanvasesList","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName originator ("originator", "", "");
      //  xoap::SOAPName targetAddr = envelope.createName("targetAddr");
      xoap::SOAPElement command = body.addBodyElement(commandName );

      if (monitor == NULL) return bmap;
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestCanvasesList to " << monitor->getClassName() << " ID" << monitor->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *monitor);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return bmap;
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestCanvasesList reply from " << monitor->getClassName() << " ID" << monitor->getLocalId());
          //	  std::map<std::string, std::list<std::string> > bmap;
          std::list<std::string> olist;
          vector<xoap::SOAPElement> content = rb.getChildElements ();
          xoap::SOAPName nodeTag ("DQMNode", "", "");
          for (vector<xoap::SOAPElement>::iterator itr = content.begin();
               itr != content.end(); ++itr)
            {

              std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);

              for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                   n_itr != nodeElement.end(); ++n_itr)
                {
                  std::stringstream stdir;
                  bmap.clear();
                  xoap::SOAPName branchTag ("Branch", "", "");

                  std::vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );

                  for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin();
                       b_itr != branchElement.end(); ++b_itr)
                    {

                      olist.clear();
                      stringstream stdir;
                      string dir="";
                      stdir.clear();
                      dir = b_itr->getValue();
                      // dir = "MonitorNode"+n_itr->getValue()+ "/" + dir;
                      xoap::SOAPName objTag("Obj", "", "");

                      vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );

                      for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
                           o_itr != objElement.end(); ++o_itr )
                        {
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
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      // handle exception
      return bmap;
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return bmap;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return bmap;
    }


  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Canvases List is updated");
  return bmap;
}


TMessage* EmuDisplayClient::requestObjects(xdata::Integer nodeaddr, std::string folder, std::string objname)
{
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
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

      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      if (d==NULL) return buf;
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestObjects to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *d);
      /*
            std::cout << std::endl;
            msg->writeTo(std::cout);
            std::cout << std::endl;
      */
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return buf;
        }
      else
        {
          /*
            std::ofstream fout("/csc_data/soap_dump.txt", ios::app);
                      std::string dump="";
                        fout << "------- START -------" << std::endl;
                        reply->writeTo(dump);
                        fout << dump<< std::endl;
                        fout << "------- STOP -------" << std::endl;
                      fout.close();
                */

          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestObjects reply from " << d->getClassName() << " ID" << d->getLocalId());
          std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
          std::list<xoap::AttachmentPart*>::iterator iter;
          // if (reply->countAttachments()>1) std::cout << reply->countAttachments() << std::endl;
          if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);
          if (attachments.size() > 1)
            {
              LOG4CPLUS_WARN (getApplicationLogger(), "Received corrupted objects " << folder << "/" << objname);
              /*
                        std::ofstream fout("/csc_data/soap_dump.txt", ios::app);
                        std::string dump="";
                 		fout << "------- START -------" << std::endl;
                		reply->writeTo(dump);
                		fout << dump<< std::endl;
                		fout << "------- STOP -------" << std::endl;
                        fout.close();
              */

              return buf;
            }

          for (iter = attachments.begin(); iter != attachments.end(); iter++)
            {
              int size = (*iter)->getSize();
              char * content = (*iter)->getContent();
              map<string, std::string, less<string> > mimeHdrs = (*iter)->getAllMimeHeaders();
              // std:: cout << "size:" << size << ", name: " << (*iter)->getContentLocation() << std::endl;
              if (buf != NULL)
                {
                  delete buf;
                }
              buf = new TMessage(kMESS_OBJECT);
              buf->Reset();
              buf->WriteBuf(content, size);

            }
        }
    }
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      if (buf) delete buf;
      return NULL;
      // handle exception
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      if (buf) delete buf;
      return NULL;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      if (buf) delete buf;
      return NULL;
    }
  catch (xcept::Exception e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      if (buf) delete buf;
      return NULL;
    }

  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Object is updated");
  return buf;
}

TMessage* EmuDisplayClient::requestCanvas(xdata::Integer nodeaddr, std::string folder, std::string objname, int width, int height)
{
  // Get reply from DQM node and populate TConsumerInfo list
  TMessage* buf = NULL;
  try
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


      xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
      if (d==NULL) return buf;
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestCanvas: \"" << folder << "/" << objname << "\" to " << d->getClassName() << " ID" << d->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *d);
      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
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
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestCanvas: \"" << folder << "/" << objname << "\" reply from " << d->getClassName() << " ID" << d->getLocalId());
          std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
          std::list<xoap::AttachmentPart*>::iterator iter;
          if (attachments.size() == 0) LOG4CPLUS_WARN (getApplicationLogger(), "Received empty object " << folder << "/" << objname);

          for (iter = attachments.begin(); iter != attachments.end(); iter++)
            {
              int size = (*iter)->getSize();
              char * content = (*iter)->getContent();
              if (buf != NULL)
                {
                  delete buf;
                }
              buf = new TMessage(kMESS_OBJECT);
              buf->Reset();
              buf->WriteBuf(content, size);

            }
        }
    }
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return buf;
      // handle exception
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return buf;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return buf;
    }


  // LOG4CPLUS_INFO (getApplicationLogger(), "Monitoring Canvas is updated");
  return buf;
}

FoldersMap EmuDisplayClient::updateFoldersMap()
{
  if (time(NULL)- foldersMap.getTimeStamp()>10)
    {
      foldersMap.clear();
      if (!monitors.empty())
        {


          // LOG4CPLUS_INFO (getApplicationLogger(), "Start Monitoring Folders List updating");
          std::set<xdaq::ApplicationDescriptor*>::iterator pos;

          for (pos=monitors.begin(); pos!=monitors.end(); ++pos)
            {
              if ((*pos) == NULL) continue;
              int nodeID= (*pos)->getLocalId();
              std::set<std::string> flist = requestFoldersList(*pos);
              std::set<std::string>::iterator litr;
              for (litr=flist.begin(); litr != flist.end(); ++litr)
                {
                  if (*litr != "") foldersMap[*litr].insert(nodeID);
                }
            }
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Folders List is updated");
          foldersMap.setTimeStamp(time(NULL));
        }
    }
  return foldersMap;
}

CSCCounters EmuDisplayClient::updateCSCCounters()
{
  if (time(NULL)- cscCounters.getTimeStamp()>10)
    {
      cscCounters.clear();
      if (!monitors.empty())
        {

          std::set<xdaq::ApplicationDescriptor*>::iterator pos;

          for (pos=monitors.begin(); pos!=monitors.end(); ++pos)
            {
              if ((*pos) == NULL) continue;
              Counters clist = requestCSCCounters(*pos);
              Counters::iterator citr;
              for (citr=clist.begin(); citr != clist.end(); ++citr)
                {
                  cscCounters[citr->first] = citr->second;
                }
            }
          LOG4CPLUS_DEBUG (getApplicationLogger(), "CSC Counters are updated");
          cscCounters.setTimeStamp(time(NULL));
        }
    }
  return cscCounters;
}


DQMNodesStatus EmuDisplayClient::updateNodesStatus()
{
  if (time(NULL)- nodesStatus.getTimeStamp()>10)
    {
      nodesStatus.clear();

      //      std::set<xdaq::ApplicationDescriptor*>  ruis = getAppsList("EmuRUI","default");
      if (!monitors.empty())
        {
          std::set<xdaq::ApplicationDescriptor*>::iterator pos;

          // std::set<xdaq::ApplicationDescriptor*>::iterator rui_itr;
          // xdaq::ApplicationDescriptor* rui=NULL;

          for (pos=monitors.begin(); pos!=monitors.end(); ++pos)
            {
              if ((*pos) == NULL) continue;

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
              std::string cscDetected   = "0";
              std::string cscRate   = "0";
              std::string readoutMode   = "NA";
              std::string lastEventTime = "NA";
              std::string nDAQevents = "0";
              std::string dataSource = "NA";

              try
                {
                  applink = (*pos)->getContextDescriptor()->getURL()+"/"+(*pos)->getURN();
                  state =  emu::dqm::getScalarParam(getApplicationContext(),
                                                    getApplicationDescriptor(), (*pos),"stateName","string");
                  if (!viewOnly_)
                    {
                      applink += "/showControl";
                    }
                  if (state == "")
                    {
                      state = "Unknown/Dead";
                      continue;
                    }
                  else
                    {
                      stateChangeTime = emu::dqm::getScalarParam(getApplicationContext(),
                                        getApplicationDescriptor(), (*pos),"stateChangeTime","string");
                    }


                  runNumber   = emu::dqm::getScalarParam(getApplicationContext(),
                                                         getApplicationDescriptor(),  (*pos),"runNumber","unsignedInt");
                  curRunNumber = runNumber;

                  events = emu::dqm::getScalarParam(getApplicationContext(),
                                                    getApplicationDescriptor(), (*pos),"sessionEvents","unsignedInt");
                  dataRate   = emu::dqm::getScalarParam(getApplicationContext(),
                                                        getApplicationDescriptor(), (*pos),"averageRate","unsignedInt");
                  cscUnpacked   = emu::dqm::getScalarParam(getApplicationContext(),
                                  getApplicationDescriptor(), (*pos),"cscUnpacked","unsignedInt");
                  cscDetected   = emu::dqm::getScalarParam(getApplicationContext(),
                                  getApplicationDescriptor(), (*pos),"cscDetected","unsignedInt");
                  cscRate   = emu::dqm::getScalarParam(getApplicationContext(),
                                                       getApplicationDescriptor(), (*pos),"cscRate","unsignedInt");
                  readoutMode   = emu::dqm::getScalarParam(getApplicationContext(),
                                  getApplicationDescriptor(), (*pos),"readoutMode","string");

                  //lastEventTime = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"lastEventTime","string");

                  nDAQevents = "0";
                  if (readoutMode == "internal")
                    {
                      dataSource   = emu::dqm::getScalarParam(getApplicationContext(),
                                                              getApplicationDescriptor(), (*pos),"inputDeviceName","string");
                      nDAQevents = events;
                    }
                  if (readoutMode == "external")
                    {
                      dataSource   = emu::dqm::getScalarParam(getApplicationContext(),
                                                              getApplicationDescriptor(), (*pos),"serversClassName","string");
                      /*
                      if (rui != NULL)
                      {
                      // == Commented it to prevent online DQM freezes
                      // nDAQevents = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), rui,"nEventsRead","unsignedLong");
                      // nDAQevents = emu::dqm::getScalarParam(getApplicationContext(), getApplicationDescriptor(), (*pos),"nDAQEvents","unsignedInt");
                      }
                      */
                    }
                }
              catch (xcept::Exception e)
                {
                  if (debug) LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
                }

              nodesStatus[nodename]["appLink"] = applink;
              nodesStatus[nodename]["stateName"] = state;
              nodesStatus[nodename]["stateChangeTime"] = stateChangeTime;
              nodesStatus[nodename]["runNumber"] = runNumber;
              nodesStatus[nodename]["sessionEvents"] = events;
              nodesStatus[nodename]["averageRate"] = dataRate;
              nodesStatus[nodename]["cscUnpacked"] = cscUnpacked;
              nodesStatus[nodename]["cscDetected"] = cscDetected;
              nodesStatus[nodename]["cscRate"] = cscRate;
              nodesStatus[nodename]["readoutMode"] = readoutMode;
              nodesStatus[nodename]["nDAQevents"] = nDAQevents;
              nodesStatus[nodename]["dataSource"] = dataSource;


            }

          LOG4CPLUS_DEBUG (getApplicationLogger(), "DQM Nodes Statuses are updated");
          nodesStatus.setTimeStamp(time(NULL));
        }
    }
  return nodesStatus;
}


Counters EmuDisplayClient::requestCSCCounters(xdaq::ApplicationDescriptor* dest)
{

  Counters clist;
  clist.clear();
  try
    {
      // Prepare SOAP Message for DQM Node
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName commandName = envelope.createName("requestCSCCounters","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName originator ("originator", "", "");
      xoap::SOAPElement command = body.addBodyElement(commandName );
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestCSCCounters to " << dest->getClassName() << " ID" << dest->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *dest);

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return clist;
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestCSCCounters reply from " << dest->getClassName() << " ID" << dest->getLocalId());

          vector<xoap::SOAPElement> content = rb.getChildElements ();
          xoap::SOAPName nodeTag ("DQMNode", "", "");
          for (vector<xoap::SOAPElement>::iterator itr = content.begin();
               itr != content.end(); ++itr)
            {

              std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);

              for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                   n_itr != nodeElement.end(); ++n_itr)
                {
                  std::stringstream stdir;
                  xoap::SOAPName cscTag ("CSC", "", "");

                  std::vector<xoap::SOAPElement> cscElement = n_itr->getChildElements (cscTag );

                  for (vector<xoap::SOAPElement>::iterator f_itr = cscElement.begin();
                       f_itr != cscElement.end(); ++f_itr)
                    {
                      std::string csc = f_itr->getValue();
                      // LOG4CPLUS_INFO (getApplicationLogger(), f_itr->getValue());
                      vector<xoap::SOAPElement> objElement = f_itr->getChildElements();

                      for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
                           o_itr != objElement.end(); ++o_itr )
                        {
                          std::string tag=o_itr->getElementName().getLocalName();
                          std::string value = o_itr->getValue();

                          if (value != "")
                            {
                              clist[csc][tag]=value;
                              //	LOG4CPDEBUGINFO (getApplicationLogger(),o_itr->getElementName().getLocalName() << "="<< value);
                            }

                        }



                      //                flist.insert(f_itr->getValue());

                    }
                }
            }
        }
    }
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return clist;
      // handle exception
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return clist;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      clist.clear();
      return clist;
    }

  LOG4CPLUS_DEBUG (getApplicationLogger(), "CSC Counters are updated");


  return clist;
}

std::set<std::string>  EmuDisplayClient::requestFoldersList(xdaq::ApplicationDescriptor* dest)
{

  std::set<std::string> flist;
  flist.clear();
  // Prepare SOAP Message for DQM Node
  try
    {
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName commandName = envelope.createName("requestFoldersList","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName originator ("originator", "", "");
      xoap::SOAPElement command = body.addBodyElement(commandName );


      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestFoldersList to " << dest->getClassName() << " ID" << dest->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *dest);

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return flist;
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestFoldersList reply from " << dest->getClassName() << " ID" << dest->getLocalId());

          vector<xoap::SOAPElement> content = rb.getChildElements ();
          xoap::SOAPName nodeTag ("DQMNode", "", "");
          for (vector<xoap::SOAPElement>::iterator itr = content.begin();
               itr != content.end(); ++itr)
            {

              std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);

              for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                   n_itr != nodeElement.end(); ++n_itr)
                {
                  std::stringstream stdir;
                  xoap::SOAPName folderTag ("Folder", "", "");

                  std::vector<xoap::SOAPElement> folderElement = n_itr->getChildElements (folderTag );

                  for (vector<xoap::SOAPElement>::iterator f_itr = folderElement.begin();
                       f_itr != folderElement.end(); ++f_itr)
                    {


                      flist.insert(f_itr->getValue());

                    }
                }
            }
        }
    }
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return flist;
      // handle exception
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return flist;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      flist.clear();
      return flist;
    }


  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Folders List is updated");
  return flist;
}

DQMReport  EmuDisplayClient::requestReport(xdaq::ApplicationDescriptor* dest)
{

  DQMReport report;
  try
    {
      // Prepare SOAP Message for DQM Node
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName commandName = envelope.createName("requestReport","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName originator ("originator", "", "");
      xoap::SOAPElement command = body.addBodyElement(commandName );

      LOG4CPLUS_DEBUG (getApplicationLogger(), "Sending requestReport to " << dest->getClassName() << " ID" << dest->getLocalId());
      xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, *(this->getApplicationDescriptor()), *dest);

      /*
      std::cout << std::endl;
      reply->writeTo(cout);
      std::cout << endl;
      */

      xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
      if (rb.hasFault() )
        {
          xoap::SOAPFault fault = rb.getFault();
          std::string errmsg = "DQMNode: ";
          errmsg += fault.getFaultString();
          XCEPT_RAISE(xoap::exception::Exception, errmsg);
          return report;
        }
      else
        {
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Received requestReport reply from " << dest->getClassName() << " ID" << dest->getLocalId());

          vector<xoap::SOAPElement> content = rb.getChildElements ();
          xoap::SOAPName nodeTag ("DQMNode", "", "");
          for (vector<xoap::SOAPElement>::iterator itr = content.begin();
               itr != content.end(); ++itr)
            {

              std::vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
              xoap::SOAPName nodeID("id","","");
              for (std::vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                   n_itr != nodeElement.end(); ++n_itr)
                {
                  std::stringstream stdir;
                  std::string s_nodeID = n_itr->getAttributeValue(nodeID);


                  // Report Info entry
                  xoap::SOAPName reportInfo("ReportInfo", "", "");
                  std::vector<xoap::SOAPElement> reportInfoElement = n_itr->getChildElements (reportInfo);
                  for (vector<xoap::SOAPElement>::iterator f_itr = reportInfoElement.begin();
                       f_itr != reportInfoElement.end(); ++f_itr)
                    {
                      xoap::SOAPName runName = envelope.createName("run","","");
                      xoap::SOAPName genDate = envelope.createName("genDate","","");

                      LOG4CPLUS_DEBUG(getApplicationLogger(),"node:" << s_nodeID
                                      << " run: " <<  f_itr->getAttributeValue(runName)
                                      << " genDate: " << f_itr->getAttributeValue(genDate));

                    }

                  // Report List
                  xoap::SOAPName reportList("ReportList", "", "");
                  std::vector<xoap::SOAPElement> reportListElements = n_itr->getChildElements (reportList );
                  for (vector<xoap::SOAPElement>::iterator f_itr = reportListElements.begin();
                       f_itr != reportListElements.end(); ++f_itr)
                    {

                      // Loop over report objects (EMU, DDU, CSC)
                      xoap::SOAPName objectTag = envelope.createName("object","","");
                      std::vector<xoap::SOAPElement> objectElements = f_itr->getChildElements (objectTag );
                      for (vector<xoap::SOAPElement>::iterator o_itr = objectElements.begin();
                           o_itr != objectElements.end(); ++o_itr)
                        {
                          xoap::SOAPName objID = envelope.createName("id","","");
                          xoap::SOAPName objName = envelope.createName("name","","");
                          std::string s_objID = o_itr->getAttributeValue(objID);
                          std::string s_objName =  o_itr->getAttributeValue(objName);
                          LOG4CPLUS_DEBUG(getApplicationLogger(),"objID: " << s_objID
                                          << " name: " << s_objName);

                          // Loop over report entries for objects
                          xoap::SOAPName entryTag = envelope.createName("entry","","");
                          std::vector<xoap::SOAPElement> entryElements = o_itr->getChildElements (entryTag );
                          for (vector<xoap::SOAPElement>::iterator e_itr = entryElements.begin();
                               e_itr != entryElements.end(); ++e_itr)
                            {
                              xoap::SOAPName testID = envelope.createName("testID","","");
                              xoap::SOAPName descr = envelope.createName("descr","","");
                              xoap::SOAPName severity = envelope.createName("severity","","");
                              std::string s_testID = e_itr->getAttributeValue(testID);
                              std::string s_descr =  e_itr->getAttributeValue(descr);
                              std::string s_severity =  e_itr->getAttributeValue(severity);
                              LOG4CPLUS_DEBUG(getApplicationLogger(), "testID: " << s_testID
                                              << " descr: " << s_descr
                                              << " severity: " << s_severity);
                              int sev = 0;
                              if (sscanf(s_severity.c_str(),"%d", &sev)==1)
                                {
                                  ReportEntry entry;
                                  report.addEntry(s_objID,entry.fillEntry(s_descr,DQM_SEVERITY(sev),s_testID));
                                }
                              else
                                {
                                  // Unable to extract severity value
                                }

                            }

                        }

                    }


                }
            }
        }
    }
  catch (xoap::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return report;
      // handle exception
    }
  catch (xdaq::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return report;
      // handle exception
    }
  catch (pt::exception::Exception& e)
    {
      LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e));
      return report;
    }


  LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Report is updated");
  return report;
}

// == Get Application Descriptors for specified Data Server class name == //
std::set<xdaq::ApplicationDescriptor*> EmuDisplayClient::getAppsList(xdata::String className, xdata::String group)
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


DQMReport EmuDisplayClient::updateNodesReports()
{
  DQMReport report;
  if (!monitors.empty())
    {
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Start Reports updating");
      std::set<xdaq::ApplicationDescriptor*>::iterator pos;

      std::map<uint32_t,DQMReport> rep_lists;
      for (pos=monitors.begin(); pos!=monitors.end(); ++pos)
        {
          if ((*pos) == NULL) continue;
          uint32_t nodeID= (*pos)->getLocalId();
          rep_lists.insert(make_pair(nodeID,requestReport(*pos)));
        }

//      dqm_report.clearReport();
      report = mergeNodesReports(rep_lists);
      LOG4CPLUS_DEBUG (getApplicationLogger(), "Monitoring Reports are updated");
    }

  return report;
}

std::vector<emu::base::WebReportItem> EmuDisplayClient::materialToReportOnPage1()
{
  appBSem_.take();
  std::vector<emu::base::WebReportItem> items;
  std::string state = getDQMState();
  std::string valueTip;
  std::string title = "Local DQM";

  std::string controlURL = getHref( getApplicationDescriptor() );
  if ( ! ( state == "Enabled" || state == "Ready" || state == "Halted" ) )
    valueTip = "Local DQM needs attention. Click to control it manually.";
  items.push_back( emu::base::WebReportItem( "state",
                   state,
                   "The overall state of local DQM.",
                   valueTip,
                   "",
                   controlURL ) );

  std::string quality = getDQMQuality();
  valueTip = "Calculated as number of reported CSCs / max number of CSCs (468)";
  items.push_back( emu::base::WebReportItem( "quality",
                   quality,
                   "The overall DQM quaility",
                   valueTip,
                   "",
                   "" ) );

  items.push_back( emu::base::WebReportItem( "title",
                   title,
                   "Summary Local DQM",
                   "",
                   "",
                   "" ) );


  std::string cscrate = getDQMUnpackingRate();
  valueTip = "Local DQM Events Unpacking Rate";
  items.push_back( emu::base::WebReportItem( "cscrate",
                   cscrate,
                   "The Overall DQM Events Unpacking Rate",
                   valueTip,
                   "",
                   "" ) );

  std::string evtrate = getDQMEventsRate();
  valueTip = "Local DQM Events Readout Rate";
  items.push_back( emu::base::WebReportItem( "evtrate",
                   cscrate,
                   "The Overall DQM Events Readout Rate",
                   valueTip,
                   "",
                   "" ) );

  appBSem_.give();
  return items;
}



std::string EmuDisplayClient::getDQMState()
{

  // Combine states:
  // If one is failed, the combined state will also be failed.
  // Else, if one is unknown, the combined state will also be unknown.
  // Else, if all are known but not the same, the combined state will be indefinite.
  std::string combinedState("UNKNOWN");
  std::string state = "";

  if (!nodesStatus.empty())
    {
      DQMNodesStatus::iterator nitr;
      for (nitr=nodesStatus.begin(); nitr != nodesStatus.end(); ++nitr)
        {
          std::string nodename = nitr->first;
          std::map<std::string, std::string>::iterator itr;
          if ((itr = nitr->second.find("stateName")) != nitr->second.end())
            {
              state = nitr->second["stateName"];
            }
          if (state == "") state = "UNKNOWN";
          if (state == "Failed" )
            {
              combinedState = state;
              break;
            }
          else if ( state == "UNKNOWN" )
            {
              combinedState = state;
              break;
            }
          else if ( state != combinedState && combinedState != "UNKNOWN" )
            {
              combinedState = "INDEFINITE";
              break;
            }
          else
            {
              combinedState = state;
            }

        }
    }
  return combinedState;
}

std::string EmuDisplayClient::getDQMUnpackingRate()
{

  std::string cscRate = "";
  uint32_t sum_cscRate = 0;

  if (!nodesStatus.empty())
    {
      DQMNodesStatus::iterator nitr;
      for (nitr=nodesStatus.begin(); nitr != nodesStatus.end(); ++nitr)
        {
          std::string nodename = nitr->first;
          std::map<std::string, std::string>::iterator itr;

          if ((itr = nitr->second.find("cscRate")) != nitr->second.end())
            {
              cscRate = nitr->second["cscRate"];
            }
          int value = 0;
          if ( (cscRate != "") && (sscanf(cscRate.c_str(), "%d",&value) == 1) )
            {
              sum_cscRate += value;
            }
        }
    }

  cscRate = Form("%d", sum_cscRate);
  return cscRate;
}

std::string EmuDisplayClient::getDQMEventsRate()
{

  std::string cscRate = "";
  uint32_t sum_cscRate = 0;

  if (!nodesStatus.empty())
    {
      DQMNodesStatus::iterator nitr;
      for (nitr=nodesStatus.begin(); nitr != nodesStatus.end(); ++nitr)
        {
          std::string nodename = nitr->first;
          std::map<std::string, std::string>::iterator itr;

          if ((itr = nitr->second.find("averageRate")) != nitr->second.end())
            {
              cscRate = nitr->second["averageRate"];
            }
          int value = 0;
          if ( (cscRate != "") && (sscanf(cscRate.c_str(), "%d",&value) == 1) )
            {
              sum_cscRate += value;
            }
        }
    }

  cscRate = Form("%d", sum_cscRate);
  return cscRate;
}



std::string EmuDisplayClient::getDQMQuality()
{

  int maxCSCs = 468; // !!! Move it from here
  std::string dqmQuality = "1.000";
  float all_cscUnpacked = 0.0;
  std::string cscDetected = "0";

  if (!nodesStatus.empty())
    {
      DQMNodesStatus::iterator nitr;
      for (nitr=nodesStatus.begin(); nitr != nodesStatus.end(); ++nitr)
        {
          std::string nodename = nitr->first;
          std::map<std::string, std::string>::iterator itr;
          if ((itr = nitr->second.find("cscDetected")) != nitr->second.end())
            {
              cscDetected = nitr->second["cscDetected"];
            }
          int value = 0;
          if ( (cscDetected != "") && (sscanf(cscDetected.c_str(), "%d",&value) == 1) )
            {
              all_cscUnpacked += value;
            }
        }
    }

  dqmQuality = Form("%.3f", all_cscUnpacked/maxCSCs);
  return dqmQuality;
}

std::string EmuDisplayClient::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
  std::string href;
  href  = appDescriptor->getContextDescriptor()->getURL();
  href += "/";
  href += appDescriptor->getURN();
  return href;
}

int EmuDisplayClient::svc()
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Starting Nodes Requestor task...");
  int counter=0;
  while (1)
    {
      usleep(10*1000000);
      counter++;

      appBSem_.take();
      updateNodesStatus();
      appBSem_.give();

      if (counter % 2 == 0)
        {
          appBSem_.take();
          updateFoldersMap();
          appBSem_.give();

          appBSem_.take();
          updateCSCCounters();
          appBSem_.give();
        }

      if (counter % 3 == 0)
        {
          appBSem_.take();
          DQMReport report;
          LOG4CPLUS_DEBUG (getApplicationLogger(), "Generating DQM Report");
          report = updateNodesReports();
          generateSummaryReport(curRunNumber, report);
          dqm_report.clearReport();
          dqm_report = report;
          appBSem_.give();
        }
      if (counter % (6*saveResultsDelay) == 0)
        {
          // Send command to Monitors to save results
	  appBSem_.take();
	  saveNodesResults();
	  appBSem_.give();
        }
      // if (counter % 3*2*saveResultsDelay == 0) counter = 0;

    }
  return 0;
}
