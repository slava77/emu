#include "EmuFarmer.h"
#include "EmuFarmerV.h"
#include "xgi/Method.h"
#include "xgi/Utils.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/MessageFactory.h"
#include "cgicc/HTMLClasses.h"

#include "toolbox/Runtime.h"
#include "toolbox/regex.h"
#include <xercesc/util/XMLURL.hpp>
#include "xcept/tools.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"

#include <time.h>
#include <iomanip>

// headers needed for Xalan
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/PlatformSupport/XalanMemoryManagerDefault.hpp>

// For EmuFarmer::postSOAP
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportReceiver.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"

#include <unistd.h> // for sleep()

// Alias used to access the "versioning" namespace EmuFarmer from within
// the class EmuFarmer
namespace EmuFarmerV = EmuFarmer;

EmuFarmer::EmuFarmer(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  logger_(Logger::getInstance(generateLoggerName()))
{
  bindWebInterface();
  exportParams();
  mapUserNames();
  mapApplicationNames();
}

string EmuFarmer::generateLoggerName()
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

void EmuFarmer::bindWebInterface(){
// moved to file:  xgi::bind(this, &EmuFarmer::css           , "EmuFarmer.css");
// moved to file:  xgi::bind(this, &EmuFarmer::javascript    , "EmuFarmer.js");
  xgi::bind(this, &EmuFarmer::defaultWebPage, "Default"   );
}

void EmuFarmer::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  s->fireItemAvailable( "configFilesList", &configFilesList_ );
  s->fireItemAvailable( "mergingXSLT", &mergingXSLT_ );
  s->fireItemAvailable( "mergedConfigDir", &mergedConfigDir_ );
  s->fireItemAvailable( "mergedConfigDirURL", &mergedConfigDirURL_ );

  s->fireItemAvailable( "configFilesListRel", &configFilesListRel_ );
  s->fireItemAvailable( "mergedConfigDirRel", &mergedConfigDirRel_ );

  s->fireItemAvailable( "daqUser", &daqUser_ );
  s->fireItemAvailable( "dqmUser", &dqmUser_ );
  s->fireItemAvailable( "fedUser", &fedUser_ );
  s->fireItemAvailable( "tfUser", &tfUser_ );
  s->fireItemAvailable( "managerUser", &managerUser_ );
  s->fireItemAvailable( "supervisorUser", &supervisorUser_ );
  s->fireItemAvailable( "pcUser", &pcUser_ );
  s->fireItemAvailable( "ltcUser", &ltcUser_ );

  s->fireItemAvailable( "daqApplicationNames", &daqApplicationNames_ );
  s->fireItemAvailable( "dqmApplicationNames", &dqmApplicationNames_ );
  s->fireItemAvailable( "fedApplicationNames", &fedApplicationNames_ );
  s->fireItemAvailable( "tfApplicationNames", &tfApplicationNames_ );
  s->fireItemAvailable( "managerApplicationNames", &managerApplicationNames_ );
  s->fireItemAvailable( "supervisorApplicationNames", &supervisorApplicationNames_ );
  s->fireItemAvailable( "pcApplicationNames", &pcApplicationNames_ );
  s->fireItemAvailable( "ltcApplicationNames", &ltcApplicationNames_ );

  s->fireItemAvailable( "pathToHome", &pathToHome_ );
  s->fireItemAvailable( "commandToStartXDAQ", &commandToStartXDAQ_ );
  s->fireItemAvailable( "commandToReloadDriversForDAQ", &commandToReloadDriversForDAQ_ );
  s->fireItemAvailable( "commandToReloadDriversForPC", &commandToReloadDriversForPC_ );

  s->fireItemAvailable( "ApplicationsWithLogLevel_DEBUG", &ApplicationsWithLogLevel_DEBUG_ );
  s->fireItemAvailable( "ApplicationsWithLogLevel_INFO", &ApplicationsWithLogLevel_INFO_ );
  s->fireItemAvailable( "ApplicationsWithLogLevel_ERROR", &ApplicationsWithLogLevel_ERROR_ );

}

void EmuFarmer::mapUserNames(){
  users_["DAQ"] = &daqUser_;
  users_["DQM"] = &dqmUser_;
  users_["FED"] = &fedUser_;
  users_["TF" ] = &tfUser_;
  users_["DAQManager"] = &managerUser_;
  users_["CSCSupervisor"] = &supervisorUser_;
  users_["PC" ] = &pcUser_;
  users_["LTC"] = &ltcUser_;
}

void EmuFarmer::mapApplicationNames(){
  applicationNames_["DAQ"] = &daqApplicationNames_;
  applicationNames_["DQM"] = &dqmApplicationNames_;
  applicationNames_["FED"] = &fedApplicationNames_;
  applicationNames_["TF" ] = &tfApplicationNames_;
  applicationNames_["DAQManager"] = &managerApplicationNames_;
  applicationNames_["CSCSupervisor"] = &supervisorApplicationNames_;
  applicationNames_["PC" ] = &pcApplicationNames_;
  applicationNames_["LTC"] = &ltcApplicationNames_;
}

void EmuFarmer::mapLogLevels(){
  for ( size_t i=0; i<ApplicationsWithLogLevel_DEBUG_.elements(); ++i ){
    logLevels_[ ApplicationsWithLogLevel_DEBUG_.elementAt(i)->toString() ] = "DEBUG";
  }
  for ( size_t i=0; i<ApplicationsWithLogLevel_INFO_.elements(); ++i ){
    logLevels_[ ApplicationsWithLogLevel_INFO_.elementAt(i)->toString() ] = "INFO";
  }
  // WARN is the default log level...
  for ( size_t i=0; i<ApplicationsWithLogLevel_ERROR_.elements(); ++i ){
    logLevels_[ ApplicationsWithLogLevel_ERROR_.elementAt(i)->toString() ] = "ERROR";
  }

}

const char* EmuFarmer::pageLayout_[][2] = { { "CSCSupervisor", "DAQManager"},
					    { "DAQ","DQM" },
					    { "TF","FED" },
					    { "PC","LTC" } };

string EmuFarmer::createConfigFile(){
  string message("");
  message += mergeConfigFile();
  loadConfigFile();
  collectEmuProcesses();
  mapLogLevels();
  assignLogLevels();
  assignJobControlProcesses();
  return message;
}

string EmuFarmer::mergeConfigFile(){

  string message("");

  // Xalan's namespaces
  XALAN_USING_XERCES(XMLPlatformUtils);
  XALAN_USING_XERCES(XMLUni);
  XALAN_USING_XALAN(XalanTransformer);

  XALAN_USING_XALAN(XalanMemoryManagerDefault);
  XalanMemoryManagerDefault memoryManager;
  // Call the static initializer for Xerces.
  XMLPlatformUtils::Initialize(XMLUni::fgXercescDefaultLocale,
			       0,
			       0,
			       &memoryManager );
  // Initialize Xalan.
  XalanTransformer::initialize( memoryManager );
  
  // Create a XalanTransformer.
  XalanTransformer theXalanTransformer;
  
  // Create Emu's very own problem listener
  XALAN_USING_XALAN(EmuXalanTransformerProblemListener);
  EmuXalanTransformerProblemListener epl( memoryManager, &logger_ );

  // Make XalanTransformer use it
  theXalanTransformer.setProblemListener( &epl );

  // Do the XSL transformation
  string fileName = "EmuConfig_" + getDateTime() + ".xml";
  string mergedConfigFile = mergedConfigDir_.toString() + "/" + fileName;
  int success = -1;
  try
    {
      success = theXalanTransformer.transform(configFilesList_.toString().c_str(),
					      mergingXSLT_.toString().c_str(),
					      mergedConfigFile.c_str());
    }
  catch( xdaq::exception::Exception e )
    {
      XCEPT_RETHROW(xgi::exception::Exception,
                "Failed to create merged configuration file: ", e);
    }

  if ( 0 == success ){
    mergedConfigFileName_ = fileName;
    mergedConfigFileURL_ = mergedConfigDirURL_.toString()+ "/" + fileName;
  }
  else{
    // It will probably never get here, having likely thrown an exception already, but who knows... 
    message += "Failed to create merged configuration file " 
      + mergedConfigFile + " by transforming " 
      + configFilesList_.toString() + " with "
      + mergingXSLT_.toString();
      LOG4CPLUS_ERROR( logger_, message );
  }

  // Terminate Xalan
  XalanTransformer::terminate();
  
  // Terminate Xerces
  XMLPlatformUtils::Terminate();
  
  return message;
}

void EmuFarmer::loadConfigFile(){
  mergedConfigFileDOM_ = 0;
  try
    {
      mergedConfigFileDOM_ = loadDOM( mergedConfigFileURL_ );
    }
  catch( xdaq::exception::Exception e )
    {
      XCEPT_RETHROW(xgi::exception::Exception, "Failed to load merged configuration file " 
	+ mergedConfigFileURL_ + " ", e);
    }
}

// void EmuFarmer::css( xgi::Input *in, xgi::Output *out ){
//   out->getHTTPResponseHeader().addHeader("Content-Type", "text/css");
//   // moved to an external file
// }


string EmuFarmer::processForm(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string message("");

  bool needRedirecting = false;

  cgicc::Cgicc cgi(in);

  std::vector<cgicc::FormEntry> fev = cgi.getElements();
  std::vector<cgicc::FormEntry>::const_iterator fe;

  const int maxAttemptsToStart = 5;
  int attemptCount = 0;
  bool success = false;

  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
 
    LOG4CPLUS_INFO( logger_, "Received command from user: " 
		    << fe->getName() << ": " << fe->getValue() );

    if ( fe->getName() == "action" ){
      needRedirecting = true;
      if        ( fe->getValue() == "start" ){

	std::vector<cgicc::FormEntry> stillNotRunning = fev;
	while ( !success && attemptCount < maxAttemptsToStart ){
	  success = actOnEmuProcesses( fe->getValue(), stillNotRunning );
	  attemptCount++;
	  if ( success ) {
	    // Child processes seem to have been launched, but check if the executives are really running
	    sleep( (unsigned int)(1) );
	    stillNotRunning = pollExecutives( stillNotRunning );
	    success &= ( stillNotRunning.size() == 0 );
	  }
	  if ( !success && attemptCount == maxAttemptsToStart ){
	    LOG4CPLUS_ERROR( logger_, "Failed to start all selected processes after " << maxAttemptsToStart << " attempts." ); 
	  }
	  if ( success ){
	    LOG4CPLUS_INFO( logger_, "Started all selected processes after " 
			    << attemptCount << " attempt" << ( attemptCount==1 ? "" : "s" ) );
	  }
	}

	pollAllExecutives();

      } else if ( fe->getValue() == "stop" ){

	actOnEmuProcesses( "stop" , fev );

	pollAllExecutives();

      } else if ( fe->getValue() == "stop and restart" ){

	actOnEmuProcesses( "stop" , fev );

	std::vector<cgicc::FormEntry> stillNotRunning = fev;
	while ( !success && attemptCount < maxAttemptsToStart ){
	  success = actOnEmuProcesses( "start", stillNotRunning );
	  attemptCount++;
	  if ( success ) {
	    // Child processes seem to have been launched, but check if the executives are really running
	    sleep( (unsigned int)(1) );
	    stillNotRunning = pollExecutives( stillNotRunning );
	    success &= ( stillNotRunning.size() == 0 );
	  }
	  if ( !success && attemptCount == maxAttemptsToStart ){
	    LOG4CPLUS_ERROR( logger_, "Failed to restart  all selected processes after " << maxAttemptsToStart << "attempts." ); 
	  }
	  if ( success ){
	    LOG4CPLUS_INFO( logger_, "Restarted all selected processes after "
			    << attemptCount << " attempt" << ( attemptCount==1 ? "" : "s" ) );
	  }
	}

	pollAllExecutives();

      } else if ( fe->getValue() == "reload DDU drivers" ){

	reloadDrivers( fev, "DAQ" );

      } else if ( fe->getValue() == "reload VME drivers" ){

	reloadDrivers( fev, "PC" );

      } else if ( fe->getValue() == "create config" ){

	message = createConfigFile();

      } else if ( fe->getValue() == "poll" ){

	pollAllExecutives();

      } else needRedirecting = false;
      break;
    }

  }

  if ( needRedirecting ) webRedirect(in, out);

  return message;
}

void EmuFarmer::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  string message = processForm(in, out);

//   *out << "<html>"                << endl;
  *out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">"                << endl;

  *out << "<head>"                                                       << endl;

  *out << "<title>EmuFarmer</title>"                                     << endl;

//   *out << "<link type=\"text/css\" rel=\"stylesheet\"";
//   *out << " href=\"/" 
//        << getApplicationDescriptor()->getURN() 
//        << "/EmuFarmer.css\"/>"                                            << endl;

  *out << "<link type=\"text/css\" rel=\"stylesheet\"";
  *out << " href=\"/emu/emuFarmer/html/EmuFarmer.css\"/>"       << endl
       << "</link>"                                             << endl;

//   *out << "<script language=\"JavaScript\" type=\"text/javascript\" "
//        << "src=\"/"
//        << getApplicationDescriptor()->getURN()
//        << "/EmuFarmer.js\"/>"                                             << endl;

  *out << "<script language=\"JavaScript\" type=\"text/javascript\" "
       << "src=\"/emu/emuFarmer/html/EmuFarmer.js\">"                   << endl
       << "</script>"                                                   << endl;

  *out << "</head>"                                                       << endl;



  *out << "<body onload=\"changeRowColor()\">"                            << endl;
  *out << "<form method=\"get\" action=\"/" 
       << getApplicationDescriptor()->getURN() 
       << "/\">" << endl;

  *out << "<table border=\"0\" width=\"100%\"><tr><td height=\"100%\">"   << endl;


  *out << "  <table border=\"0\" width=\"100%\" height=\"100%\">"         << endl;
  *out << "   <tr><td id=\"pageTitle\">EmuFarmer</td>"              << endl;
  *out << "   <td align=\"right\" valign=\"top\">"<< endl;
  *out << "     <a href=\"/emu/emuFarmer/html/EmuFarmer_Manual.html\" target=\"_blank\">HELP</a>"<< endl;
  *out << "   </td></tr><tr><td valign=\"center\" colspan=\"2\">"                       << endl;
  configListFileTable(out);
  *out << "   </td></tr><tr><td valign=\"center\" colspan=\"2\">"                       << endl;
  configFileTable(out, message);
  *out << "   </td></tr><tr><td valign=\"bottom\" colspan=\"2\">"                       << endl;
  actionButtons(out);
  *out << "   </td></tr></table>"                                    << endl;

  *out << "   </td><td>"                                             << endl;

  *out << "     <img src=\"/emu/emuFarmer/images/EmuFarmer.jpg\" alt=\"five emus\" />" << endl;

  *out << "   </td></tr><tr><td colspan=\"2\">"                      << endl;


  const int nColsInPageLayout = sizeof(pageLayout_[0])/sizeof(char*);
  const int nRowsInPageLayout = sizeof(pageLayout_)/sizeof(pageLayout_[0]);
  *out << "<table>"                                                  << endl;
  for ( int iRow=0; iRow<nRowsInPageLayout; ++iRow ){
    *out << "  <tr>"                                                 << endl;
    for ( int iCol=0; iCol<nColsInPageLayout; ++iCol ){
      *out << "    <td>"                                             << endl;
      if ( emuProcesses_.count( pageLayout_[iRow][iCol] ) ){
	processGroupTable( pageLayout_[iRow][iCol], out );
      }
      *out << "    </td>"                                            << endl;
    }
    *out << "  </tr>"                                                << endl;
  }
  *out << "</table>"                                                 << endl;


  *out << "</td></tr></table>"                                       << endl;

  *out << "</form>"                                                  << endl;

  *out << "</body>"                                                  << endl;

  *out << "</html>"                                                  << endl;
}

void EmuFarmer::configListFileTable(xgi::Output *out){

  *out << "<table  class=\"processes\" border=\"0\" width=\"100%\""
       << "style=\"float: left; vertical-align: top:\">"             << endl;

  *out << "<tr class=\"processes\">"                                 << endl;

  *out << "  <th  class=\"processes\" width=\"100%\">"               << endl;
  *out << "    <span style=\"float: left;\">"
       <<         "Recipe to create Emu configuration file</span>"   << endl;
  *out << "  </th>"                                                  << endl;

  *out << "</tr>"                                                    << endl;
  *out << "<tr class=\"processes\">"                                 << endl;

  *out << "  <th class=\"processes\" align=\"center\">"             << endl;
//   *out << "    <a href=\"" << configFilesList_.toString() << "\""
//        << " style=\"font-weight: normal;\""
//        << " id=\"configFilesList\""
//        << " target=\"_blank\">"
//        << configFilesList_.toString() << "</a>"                      << endl;
  *out << "    <a href=\"" 
       << getApplicationContext()->getContextDescriptor()->getURL() 
       << "/" << configFilesListRel_.toString()
       << "\""
       << " style=\"font-weight: normal;\""
       << " id=\"configFilesList\""
       << " target=\"_blank\">"
       << getApplicationContext()->getContextDescriptor()->getURL() 
       << "/" << configFilesListRel_.toString() 
       << "</a>"                      << endl;
  *out << "  </th>"                                                  << endl;

  *out << "</tr>"                                                    << endl;
  *out << "</table>"                                                 << endl;

}

void EmuFarmer::configFileTable(xgi::Output *out, const string& message){

  *out << "<table  class=\"processes\" border=\"0\" width=\"100%\""
       << "style=\"float: left; vertical-align: top:\">"             << endl;

  *out << "<tr class=\"processes\">"                                 << endl;

  *out << "  <th  class=\"processes\" width=\"100%\">"               << endl;
  *out << "    <span style=\"float: left;\">"
       <<         "Emu configuration file</span>"                    << endl;
  *out << "<input"                                                   << endl;
  *out << " class=\"config\""                                        << endl;
  *out << " type=\"button\""                                         << endl;
  *out << " name=\"config\""                                         << endl;
  *out << " title=\"(Re)create the configuration file.\""            << endl;
  *out << " value=\"recreate\""                                      << endl;
  *out << " onclick=\"validateConfigFileChange(event)\""             << endl;
  *out << "/>"                                                       << endl;
  *out << "  </th>"                                                  << endl;

  *out << "</tr>"                                                    << endl;
  *out << "<tr class=\"processes\">"                                 << endl;

  *out << "  <td  class=\"processes\" align=\"center\">"             << endl;
//   *out << "    <a href=\"" << mergedConfigFileURL_ 
//        << "\" id=\"mergedConfigFileURL\""
//        << " target=\"_blank\">";
//   *out << mergedConfigFileURL_                                       << endl;
//   *out << "    </a>"                                                 << endl;
  *out << "    <a href=\"" 
       << getApplicationContext()->getContextDescriptor()->getURL() << "/"
       << mergedConfigDirRel_.toString() << "/" << mergedConfigFileName_
       << "\" id=\"mergedConfigFileURL\""
       << " target=\"_blank\">";
  *out << getApplicationContext()->getContextDescriptor()->getURL() << "/"
       << mergedConfigDirRel_.toString() << "/" << mergedConfigFileName_;
  *out << "    </a>"                                                 << endl;
  *out << "    <span style=\"color:#ff0000;\">";
  *out <<         message << "</span>"                               << endl;;
  *out << "  </td>"                                                  << endl;

  *out << "</tr>"                                                    << endl;
  *out << "</table>"                                                 << endl;

}


void EmuFarmer::actionButtons(xgi::Output *out){
  // Create four visible buttons for start/stop/restart/poll.
  // These are NOT submit buttons, they only invoke a javascript function
  // which in turn will submit the form (provided the user confirms the action).


  // But first create a hidden button whose value will be replaced by 
  // javascript with the name of action (start/stop/restart) to be taken.
  *out << " <input id=\"action\" "                                   << endl;
  *out << " 	 type=\"hidden\""                                    << endl;
  *out << " 	 name=\"action\""                                    << endl;
  *out << " 	 value=\"to_be_written_by_javascript\"/>"            << endl;


  *out << "<table border=\"0\" width=\"100%\""
       << "align=\"center\" valign=\"bottom\">"                      << endl;

  *out << "<tr>"                                                     << endl;

  *out << "  <td align=\"center\">"                                  << endl;
  *out << "<input"                                                   << endl;
  *out << " onclick=\"validateSelection(event)\""                    << endl;
  *out << " class=\"button start\""                                  << endl;
  *out << " type=\"button\""                                         << endl;
  *out << " name=\"action\""                                         << endl;
  *out << " title=\"Hatch the selected Emu processes.\""             << endl;
  *out << " value=\"start\""                                         << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"center\">"                                  << endl;
  *out << "<input"                                                   << endl;
  *out << " onclick=\"validateSelection(event)\""                    << endl;
  *out << " class=\"button stop\""                                   << endl;
  *out << " type=\"button\""                                         << endl;
  *out << " name=\"action\""                                         << endl;
  *out << " title=\"Cull the selected Emu processes.\""              << endl;
  *out << " value=\"stop\""                                          << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"center\">"                                  << endl;
  *out << "<input"                                                   << endl;
  *out << " onclick=\"validateSelection(event)\""                    << endl;
  *out << " class=\"button restart\""                                << endl;
  *out << " type=\"button\""                                         << endl;
  *out << " name=\"action\""                                         << endl;
  *out << " title=\"Cull and then hatch the selected Emu processes.\""<< endl;
  *out << " value=\"stop and restart\""                              << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"center\">"                                  << endl;
  *out << "<input"                                                   << endl;
  *out << " onclick=\"poll(event)\""                                 << endl;
  *out << " class=\"button poll\""                                   << endl;
  *out << " type=\"button\""                                         << endl;
  *out << " name=\"action\""                                         << endl;
  *out << " title=\"Poll all processes if they are up and running. If so, their log level will show.\""<< endl;
  *out << " value=\"poll\""                                          << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;


  *out << "</tr>"                                                    << endl;
  *out << "</table>"                                                 << endl;

}

// void EmuFarmer::javascript(xgi::Input *in, xgi::Output *out){
//   // moved to an external file
// }

DOMDocument* EmuFarmer::loadDOM( const std::string& pathname )
  throw (xdaq::exception::Exception)
{
  // Copied from xdaq::ApplicationContextImpl::loadDOM

  std::vector<std::string> files;
  try
    {
      files = toolbox::getRuntime()->expandPathName(pathname);
    } 
  catch (toolbox::exception::Exception& tbe)
    {
      XCEPT_RETHROW (xdaq::exception::Exception, "Cannot parse pathname " + pathname, tbe);
    }
	
  if (files.size() == 1)
    {
      if ((files[0].find("file:/") == std::string::npos) && (files[0].find("http://")))
	{
	  files[0].insert(0, "file:");
	}
		
      XMLURL* source = 0;
      try
	{
	  source = new XMLURL(files[0].c_str());
	} 
      catch (...)
	{
	  XCEPT_RAISE (xdaq::exception::Exception, "Failed to load from " + files[0]);
	}
      //xoap::DOMParser* p = xoap::DOMParser::get("configure");
      xoap::DOMParser* p = xoap::getDOMParserFactory()->get("configure");
      DOMDocument* doc;
		
      try
	{
	  doc = p->parse(*source);
	  delete source;
	  xoap::getDOMParserFactory()->destroy("configure");
	  return doc;
	} 
      catch (xoap::exception::Exception& xe)
	{
	  delete source;
	  XCEPT_RETHROW (xdaq::exception::Exception, "Cannot parse XML loaded from " + files[0], xe);
	}
    } 
  else
    {
      XCEPT_RAISE (xdaq::exception::Exception, "Pathname " + pathname + " is ambiguous");
    }
}

void EmuFarmer::collectEmuProcesses(){
  // Walk through the merged config file and group the contexts.

  // But first clear containers to start with tabula rasa
  emuGroups_.clear();
  emuProcessDescriptors_.clear();
  emuProcesses_.clear();

  // Loop over Contexts
  DOMNodeList* con = mergedConfigFileDOM_->getElementsByTagName( xoap::XStr("xc:Context") );
  for ( unsigned int i=0; i<con->getLength(); ++i ){

    EmuProcessDescriptor epd;
    const string protocol("http://");

    // Loop over Context's attributes to get URL
    DOMNamedNodeMap* conAtt = con->item(i)->getAttributes();
    for ( unsigned int j=0; j<conAtt->getLength(); ++j ){
      if ( xoap::XMLCh2String(conAtt->item(j)->getNodeName()) == "url" ){
	string url = xoap::XMLCh2String( conAtt->item(j)->getNodeValue() );
	// Try to create a process descriptor for this URL, and put it into emuProcessDescriptors_
	try{
	  epd.setURL( url );
	  emuProcessDescriptors_[ epd.getNormalizedURL() ] = epd;
	} 
	catch( toolbox::net::exception::MalformedURL e ){
	  LOG4CPLUS_ERROR( logger_, "Malformed URL " << url << " : " <<
			   xcept::stdformat_exception_history(e) );
	} 
	catch( toolbox::net::exception::BadURL e ){
	  LOG4CPLUS_ERROR( logger_, "Failed to resolve URL " << url << " : " <<
			   xcept::stdformat_exception_history(e) );
	}
      }
    }// Loop over Context's attributes

    // Go on to the next context if the URL of this one couldn't be resolved
    if ( epd.getNormalizedURL().size() == 0 ) continue;

    // Loop over Applications
    DOMNodeList* app = con->item(i)->getChildNodes();
    for ( unsigned int j=0; j<app->getLength(); ++j ){
      if ( xoap::XMLCh2String(app->item(j)->getNodeName()) != "xc:Application" ) continue;

      string appClass = "";
      string groupName;
      int appInstance = -1;
      // Loop over Application's attributes
      DOMNamedNodeMap* appAtt = app->item(j)->getAttributes();
      for ( unsigned int k=0; k<appAtt->getLength(); ++k ){
	if ( xoap::XMLCh2String(appAtt->item(k)->getNodeName()) == "class" ){
	  appClass = xoap::XMLCh2String(appAtt->item(k)->getNodeValue());
	  groupName = applicationGroupName( xoap::XMLCh2String(appAtt->item(k)->getNodeValue()) );
          // Only assign this process a group if it has been assigned "other" already or none yet
          if ( emuGroups_.find( epd.getNormalizedURL() ) == emuGroups_.end() ) // not assigned anything yet
            emuGroups_[ epd.getNormalizedURL() ] = groupName;
          else if ( emuGroups_[ epd.getNormalizedURL() ] == "other" ) // assigned "other"
	    emuGroups_[ epd.getNormalizedURL() ] = groupName;
	}
	if ( xoap::XMLCh2String(appAtt->item(k)->getNodeName()) == "instance" ){
	  stringstream ai; 
	  ai << xoap::XMLCh2String(appAtt->item(k)->getNodeValue());
	  ai >> appInstance;
	}
      }// Loop over Application's attributes

      // Loop over Application's 'properties' list looking for 'hardwareMnemonic' and 'inputDeviceName'
      DOMNodeList* appsChildren = app->item(j)->getChildNodes();
      for ( unsigned int k=0; k<appsChildren->getLength(); ++k ){
	if ( xoap::XMLCh2String( appsChildren->item(k)->getNodeName() ) == "properties" ){
	  DOMNodeList* properties = appsChildren->item(k)->getChildNodes();
	  for ( unsigned int l=0; l<properties->getLength(); ++l ){
	    if ( xoap::XMLCh2String( properties->item(l)->getNodeName() ) == "hardwareMnemonic" ){
	      appClass += "[" + xoap::XMLCh2String( properties->item(l)->getTextContent() ) + "]";
	    }
	    if ( xoap::XMLCh2String( properties->item(l)->getNodeName() ) == "inputDeviceName" ){
	      emuProcessDescriptors_[ epd.getNormalizedURL() ]
		.setDeviceName( xoap::XMLCh2String( properties->item(l)->getTextContent() ) );
	    }
	  }
	  break;
	}
      }// Loop over Application's 'properties' list looking for 'hardwareMnemonic'

      // Add this app to URL's application list
      emuProcessDescriptors_[ epd.getNormalizedURL() ].addApplication( appClass, appInstance );

    }// Loop over Applications

  }// Loop over Contexts


  // Create group_name --> URL multimap (the inverse of emuGroups_)
  for ( map< string, string >::iterator egi = emuGroups_.begin();
	egi != emuGroups_.end(); ++egi ){
	emuProcesses_.insert( pair<string,string>(egi->second, egi->first) );
  }
  
}

void EmuFarmer::assignLogLevels(){
  // Assign log levels to processes. By default it's WARN.
  // If some of its applications are listed otherwise in EmuFarmer's config file,
  // assign the lowest severity one.

  map< string, int > rank;
  rank["DEBUG"] = 1;
  rank["INFO" ] = 2;
  rank["WARN" ] = 3;
  rank["ERROR"] = 4;

  for ( map< string, EmuProcessDescriptor >::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd ) {
    // Loop over this process's applications and pick the lowest severity level requested in the config file.
    set< pair<string, int> > apps = epd->second.getApplications();
    string lowestLevel("ERROR");
    for ( set< pair<string, int> >::const_iterator app = apps.begin(); app != apps.end(); ++app ){
      // First strip app name of anything that may have been added to it. (hardware mnemonic name to EmuRUI)
      string appName = app->first;
      string::size_type delimiter = appName.find_first_of( " [" );
      if ( delimiter != string::npos ) appName = appName.substr( 0, delimiter );
      // Get level explicitly requested in the config file (if any)
      if ( logLevels_.find( appName ) != logLevels_.end() ){
	string requestedLevel = logLevels_[ appName ];
	// Apparently there was an explicit request. If this is lower than the other requests so far, set it.
	if ( rank[requestedLevel] < rank[lowestLevel] ){
	  epd->second.setStartingLogLevel( requestedLevel );
	  lowestLevel = requestedLevel;
	}
      }
    }
  }

  debugPrint( "assignLogLevels()" );

}

void EmuFarmer::debugPrint( const string& message ){

  stringstream ss;
  ss  << "*** " << message << " ***" << endl << "Emu groups" << endl;
  for ( map< string, string >::iterator egi = emuGroups_.begin();
	egi != emuGroups_.end(); ++egi )
    ss << "    " << egi->first << "   " << egi->second << endl;

  ss  << endl << "Emu processes" << endl;
  for ( multimap< string, string >::iterator ep = emuProcesses_.begin();
	ep != emuProcesses_.end(); ++ep )
    ss << "    " << ep->first << "   " << ep->second << endl;

  ss  << endl << "Emu process attributes" << endl;
  for ( map< string, EmuProcessDescriptor >::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
    {
      ss << "    " << epd->first << endl;
      ss << "         ";
      epd->second.print( ss );
    }

  ss  << endl << "Explicitly requested log levels" << endl;
  for ( map< string, string >::iterator ll = logLevels_.begin();
	ll != logLevels_.end(); ++ll )
    ss << "    " << ll->first << "   " << ll->second << endl;

  LOG4CPLUS_DEBUG( logger_, "Process groups:\n" + ss.str() );
  cerr << ss.str() << endl;

}

void EmuFarmer::assignJobControlProcesses(){
  // Loop over all JobControl apps, try to resolve their URLs, and assign
  // their resolved URLs and applicationDescriptors to all Emu processes on the same host

  // Find all JobControl apps
  std::set<xdaq::ApplicationDescriptor *> jcs;
  try {
    jcs = getApplicationContext()
      ->getDefaultZone()
      ->getApplicationDescriptors("JobControl");
  } catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    LOG4CPLUS_ERROR( logger_, "Failed to find JobControl applications: "
		     + xcept::stdformat_exception_history(e) );
    return;
  }
  
  // Loop over all JobControl apps
  EmuProcessDescriptor jcpd;
  for ( std::set<xdaq::ApplicationDescriptor *>::iterator jci = jcs.begin();
	jci != jcs.end(); ++jci ){
    string jcURL = (*jci)->getContextDescriptor()->getURL();
    try{
      jcpd.setURL( jcURL );
    } 
    catch( toolbox::net::exception::MalformedURL e ){
      LOG4CPLUS_ERROR( logger_, "Malformed URL " << jcURL << " : " <<
		       xcept::stdformat_exception_history(e) );
    } 
    catch( toolbox::net::exception::BadURL e ){
      LOG4CPLUS_ERROR( logger_, "Failed to resolve URL " << jcURL << " : " <<
		       xcept::stdformat_exception_history(e) );
    }

    // Go on to the next JobControl process if this one cannot be resolved
    if ( jcpd.getNormalizedURL().size() == 0 ) continue;

    // Assign it to all Emu processes on the same host
    assignJobControlProcess( jcpd, *jci );
    
  }

}

void EmuFarmer::assignJobControlProcess( EmuProcessDescriptor& jobControl, xdaq::ApplicationDescriptor* jcad ){
  // Assign jobControl's URL and ApplicationDescriptor to all Emu processes running on the same host

  // Loop over all Emu processes 
  for ( map< string, EmuProcessDescriptor > ::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
    if ( epd->second.getNormalizedHost() == jobControl.getNormalizedHost() )
      epd->second.setJobControl( jobControl.getNormalizedURL(), jcad );
      
}

string EmuFarmer::applicationGroupName( const string& applicationName ){
  // Find out which group this application belongs to.

  map< string, xdata::Vector<xdata::String>* >::iterator a;
  for ( a  = applicationNames_.begin();
	a != applicationNames_.end();
	++a )
    if ( isContained( applicationName, a->second ) ) return a->first;

  return string("other");
}

bool EmuFarmer::isContained( const string& name, xdata::Vector<xdata::String>* array ){
  // Is name an element of this application name array?

  for ( size_t i=0; i<array->elements(); ++i )
    if ( array->elementAt(i)->toString() == name )
      return true;

  return false;
}

void EmuFarmer::processGroupTable(const string& groupName, xgi::Output *out){
  
  *out << "   <table class=\"processes\" name=\"processTable\" id=\""
       << groupName << "\">" << endl;
  *out << "     <tr>                                                                                       " << endl;
  *out << "       <th class=\"processes title\" colspan=\"7\">" << groupName << " processes</th>          " << endl;
  if ( groupName == "DAQ" || groupName == "PC" ){
    string device;
    if      ( groupName == "DAQ" ) device = "DDU";
    else if ( groupName == "PC"  ) device = "VME";
    *out << "   </tr><tr>                                                                                  " << endl;
    *out << "       <td  class=\"processes\"  colspan=\"7\" width=\"100%\">"                                 << endl;
    *out << "          <input"                                                                               << endl;
    *out << "           id=\"" << groupName << "\""                                                          << endl;
    *out << "           class=\"config\""                                                                    << endl;
    *out << "           type=\"button\""                                                                     << endl;
    *out << "           name=\"drivers\""                                                                    << endl;
    *out << "           title=\"(Re)load the " << device << " drivers for the selected DAQ processes.\""     << endl;
    *out << "           value=\"reload " << device << " drivers\""                                           << endl;
    *out << "           onclick=\"validateDriverReload(event,'" << groupName << "')\""                       << endl;
    *out << "          />"                                                                                   << endl;
    *out << "       </td>"                                                                                   << endl;
  }
  *out << "     </tr>                                                                                      " << endl;
  *out << "     <tr>                                                                                       " << endl;
  *out << "       <th class=\"processes\">host</th>                                                        " << endl;
  *out << "       <th class=\"processes\">port</th>                                                        " << endl;
  *out << "       <th class=\"processes\">apps                                                             " << endl;
  *out << "       <input id=\"" << groupName << "_apps\"                                                   " << endl;
  *out << "              type=\"checkbox\"                                                                 " << endl;
  *out << "              title=\"toggle applications' visibility\"                                         " << endl;
  *out << "              onclick=\"appsVisible(event)\"/></th>                                             " << endl;
  *out << "       <th class=\"processes\">job id</th>                                                        " << endl;
  *out << "       <th class=\"processes\">log level</th>                                                      " << endl;
  *out << "       <th class=\"processes\"><input id=\"" << groupName << "\"                                   " << endl;
  *out << "                   name=\"dummy\"                                                                  " << endl;
  *out << "                   value=\"0\"                                                                     " << endl;
  *out << "                   type=\"checkbox\"                                                               " << endl;
  *out << "                   checked=\"checked\"                                                             " << endl;
  *out << "                   title=\"select all\"                                                            " << endl;
  *out << "                   onclick=\"multiCheck(event)\"/></th>                                            " << endl;
  *out << "       <th class=\"processes\"><input id=\"" << groupName << "\"                                   " << endl;
  *out << "                   name=\"dummy\"                                                                  " << endl;
  *out << "                   value=\"0\"                                                                     " << endl;
  *out << "                   type=\"checkbox\"                                                               " << endl;
  *out << "                   title=\"select none\"                                                           " << endl;
  *out << "                   onclick=\"multiCheck(event)\"/></th>                                            " << endl;
  *out << "     </tr>                                                                                         " << endl;
  // Loop over processes:
  for ( map< string, EmuProcessDescriptor > ::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
    { 
      if ( emuGroups_[epd->first] != groupName ) continue;
      *out << "     <tr>                                                                                      " << endl;
      *out << "       <td class=\"processes\"><a href=\"" 
	   << epd->second.getJobControlNormalizedURL() << "\" target=\"_blank\">"
	   << epd->second.getHost() << "</a></td>"                                                              << endl;
      *out << "       <td class=\"processes\"><a href=\"" 
	   << epd->second.getNormalizedURL() << "\" target=\"_blank\">"
	   << epd->second.getPort() << "</a></td>"                                                              << endl;
      *out << "       <td class=\"processes\"><span style=\"display:none;\" id=\"" << groupName << "_apps\">";
      set< pair<string, int> > apps = epd->second.getApplications();
      for ( set< pair<string, int> >::iterator appi = apps.begin(); appi != apps.end(); ++appi ){
	*out << appi->first;
	if ( appi->second>=0 ) *out << "." << appi->second; 
	*out << " ";
      }
      *out << "</span></td>      " << endl;
      *out << "       <td class=\"processes\" id=\"jobId\" align=\"center\">";
      if ( epd->second.getJobId() >= 0 )
	*out <<          epd->second.getJobId();
      else
	*out <<          "-";
      *out << "       </td>"                                                                                     << endl;
      *out << "       <td class=\"processes\" id=\"logLevel\" align=\"center\">";
      *out << ( epd->second.getLogLevel().size() ? epd->second.getLogLevel() : string("") );
      *out << "       </td>"                                                                                     << endl;
      *out << "       <td class=\"processes\" colspan=\"2\" align=\"center\">                                  " << endl;
      *out << "         <input id=\"" << groupName << "\"                                                      " << endl;
      *out << "                type=\"checkbox\"                                                               " << endl;
      *out << "                title=\"check to select for killing/(re)starting\"                              " << endl;
      if ( epd->second.isSelected() ) *out << "checked=\"checked\"                                             " << endl;
      *out << "                onclick=\"changeRowColor()\"                                                    " << endl;
      *out << "                value=\"on\"                                                                    " << endl;
      *out << "                name=\"" 
	   << epd->second.getNormalizedHost() << ":" 
	   << epd->second.getPort() << "\"/></td>"                                                               << endl;
      *out << "     </tr>                                                                                      " << endl;
    }
  *out << "   </table>                                                                                         " << endl;

}

bool EmuFarmer::actOnEmuProcesses( const string& action, const vector<cgicc::FormEntry>& fev )
  throw (xdaq::exception::Exception){

  bool success = true;

  std::vector<cgicc::FormEntry>::const_iterator fe;

  // Let's first forget any previous selection
  for ( map< string, EmuProcessDescriptor >::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
    epd->second.setSelected( false );

  // Get from the query string the processes selected by the user and act on them
  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
    // Identify processes by the colon between host and port
    string::size_type colonPosition = fe->getName().find( ':', 0 );
    if ( colonPosition != string::npos && fe->getValue() == "on" ){
      if ( fe->getName().size() < 7 ){
	LOG4CPLUS_WARN( logger_, "Suspicious element in HTTP query string: " 
			+ fe->getName() + " cannot possibly be a valid host:port." );
      }
      // Remember it's been selected
      const string url = "http://"+fe->getName();
      emuProcessDescriptors_[url].setSelected();
      // Action!
      try
	{
	  success &= actOnEmuProcess( action, url );
	}
      catch(xdaq::exception::Exception e)
	{
	  // Let's not rethrow here, but rather let it go on to do the rest.
	  LOG4CPLUS_ERROR( logger_, 
			   "Failed to " + action + " " + fe->getName() + ": "
			   + xcept::stdformat_exception_history(e));
	  success = false;
	}
    }
  }
  
  return success;

}

bool EmuFarmer::actOnEmuProcess( const string& action, const string& url )
  throw (xdaq::exception::Exception){

  bool success = false;

  // Find the JobControl process in charge of this process.
  xdaq::ApplicationDescriptor* jcDescriptor = emuProcessDescriptors_[url].getJobControlAppDescriptor();
  if ( jcDescriptor == 0 ){
    LOG4CPLUS_ERROR( logger_, "Failed to " + action + " " + url
		     + ": no JobControl found on that host.");
    return false;
  }

  // create SOAP message to be sent to JobControl
  xoap::MessageReference m;
  if      ( action == "stop"   ) m = createSOAPCommandToCull( url );
  else if ( action == "start"  ){
    // If the process is already up and running, we have nothing to do.
    if ( pollExecutive( url ) ) return true;
    m = createSOAPCommandToHatch( url );
  }
  else if ( action == "reload" ) m = createSOAPCommandToReload( url );
  else{
    LOG4CPLUS_ERROR( logger_, "Unknown action: " + action );
    return false;
  }
  // bail out if message is empty
  if ( ! m->getSOAPPart().getEnvelope().getBody().getDOM()->hasChildNodes() ) return false;

// chainsaw doesn't display XML-formatted text
//   stringstream log;
//   if ( !m.isNull() ) {
//     m->writeTo( log );
//     LOG4CPLUS_INFO( logger_, "Sending message: " + log.str() );
//   }
  if ( !m.isNull() ){ 
    cout << endl << "Sending command to " << jcDescriptor->getContextDescriptor()->getURL() << endl; 
    m->writeTo( cout );
    cout.flush();
    cout << endl; 
  }

  // send SOAP message
  xoap::MessageReference reply;
  try {
    //reply = getApplicationContext()->postSOAP( m, jcDescriptor );
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    reply = getApplicationContext()->postSOAP( m, *appDescriptor, *jcDescriptor );
  } catch (xdaq::exception::Exception e) {
    LOG4CPLUS_ERROR( logger_, "Failed to " + action + " " + url
		     + ": " + xcept::stdformat_exception_history(e) );      
    return false;
  }

// chainsaw doesn't display XML-formatted text
//   log.clear();
//   reply->writeTo( log );
//   LOG4CPLUS_INFO( logger_, "Received message:\n" + log.str() );

  cout << endl << "Received reply from " << jcDescriptor->getContextDescriptor()->getURL() << endl; 
  reply->writeTo( cout );
  cout.flush();
  cout << endl;

  // check for errors in reply
  xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
  if ( replyBody.hasFault() ){
    LOG4CPLUS_ERROR( logger_, "Failed to " + action + " " + url
		     + ": Fault (code" 
		     + replyBody.getFault().getFaultCode() + ") in SOAP reply: " 
		     + replyBody.getFault().getFaultString() );
    success = false;
  }
  else if ( action == "start" ){
    // dig up job id from reply
    string jobId("-1");
    DOMNodeList *nodeList;
    try {
      nodeList = replyBody.getDOM()->getChildNodes();
//       cerr << "Body's child nodes" << endl << printNodeList( nodeList );
//       nodeList = findNode( nodeList, "jidResponse")->getChildNodes();
      // JobControl returns jid as pid... Therefore:
      nodeList = findNode( nodeList, "pidResponse")->getChildNodes();
//       cerr << "jidResponse's child nodes" << endl << printNodeList( nodeList );
//       nodeList = findNode( nodeList, "jid"        )->getChildNodes();
      // JobControl returns jid as pid... Therefore:
      nodeList = findNode( nodeList, "pid"        )->getChildNodes();
//       cerr << "jid's child nodes" << endl << printNodeList( nodeList );
      success = true;
    } catch ( xdaq::exception::Exception e ){
      LOG4CPLUS_ERROR( logger_, "Job id of newly hatched process " + url
		       + " not found in SOAP reply ==> it is probably dead. : "
		       + xcept::stdformat_exception_history(e) );
      success = false;
    } catch (...){
      LOG4CPLUS_ERROR( logger_, "Job id of newly hatched process " + url
		       + " not found in SOAP reply: Unknown exception." );      
      success = false;
    }
    if ( nodeList->getLength() ){
      jobId = xoap::XMLCh2String( nodeList->item(0)->getNodeValue() );
      success = true;
    } else {
      LOG4CPLUS_ERROR( logger_, "Job id of newly hatched process " + url
		       + " not found in SOAP reply." );      
      success = false;
    }

    // Even if it failed to start, JobControl may have spawned a child process. Kill it, just in case.
    if ( !success ) actOnEmuProcess( "stop", url );

    // remember job id of newly hatched process
    emuProcessDescriptors_[url].setJobId( jobId );
//     emuProcessDescriptors_[url].print( cerr );
  }
  else if ( action == "stop" ){
    // Let's assume it's been a kill, or that that process didn't exist in the first place.
    // TODO: check what's actually taken place in JobControl
    emuProcessDescriptors_[url].setJobId( -1 );
    success = true;
  }

  return success;

}

vector<cgicc::FormEntry> EmuFarmer::pollExecutives( const vector<cgicc::FormEntry> fev ){
  // Poll all URL's found selected in the form entry and return those not yet running

  std::vector<cgicc::FormEntry> stillNotRunning;

  std::vector<cgicc::FormEntry>::const_iterator fe;
  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
    if ( fe->getValue() != "on" ) continue;
    const string url = "http://"+fe->getName();
    if ( emuProcessDescriptors_.find( url ) == emuProcessDescriptors_.end() ) continue;
    if ( ! pollExecutive( url ) ) { 
      stillNotRunning.push_back( *fe );
      // Even if it's not running, JobControl may have spawned a child process. Kill it, just in case.
      actOnEmuProcess( "stop", url );
    }
  }

  return stillNotRunning;
}

void EmuFarmer::pollAllExecutives(){
  for ( map< string, EmuProcessDescriptor > ::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
      pollExecutive( epd->second.getNormalizedURL() );
}


bool EmuFarmer::pollExecutive( const string& URL ){
  // Get the log level from the executive at URL. Return false on failure.

  emuProcessDescriptors_[URL].setLogLevel( "" );

  xoap::MessageReference SOAPMessage;
  try{
    SOAPMessage = createParameterGetSOAPMsg( "executive::Application", "logLevel", "string" );
  } catch ( xdaq::exception::Exception e ) {
    LOG4CPLUS_ERROR( logger_, "Failed to create SOAP message to poll executive at " 
		     << URL << " : " << xcept::stdformat_exception_history(e) );
    return false;
  }

  xoap::MessageReference reply;
  try{
    reply = postSOAP( SOAPMessage, URL, 0 );
  } catch ( xdaq::exception::Exception e ) {
    LOG4CPLUS_WARN( logger_, "No reply to polling from executive at " 
		     << URL << " : " << xcept::stdformat_exception_history(e) );
    return false;
  }

  string logLevel;
  try{
    logLevel = extractScalarParameterValueFromSoapMsg( reply, "logLevel" );
  } catch ( xdaq::exception::Exception e ) {
    LOG4CPLUS_WARN( logger_, "No log level found in reply to polling from executive at " 
		     << URL << " : " << xcept::stdformat_exception_history(e) );
    return false;
  }

  emuProcessDescriptors_[URL].setLogLevel( logLevel );

  cout << endl; reply->writeTo( cout ); cout << endl;

  return true;

}

void EmuFarmer::reloadDrivers( const vector<cgicc::FormEntry>& fev, const string group )
  throw (xdaq::exception::Exception){
  std::vector<cgicc::FormEntry>::const_iterator fe;

  // Let's first forget any previous selection
  for ( map< string, EmuProcessDescriptor >::iterator epd = emuProcessDescriptors_.begin();
	epd != emuProcessDescriptors_.end(); ++epd )
    epd->second.setSelected( false );

  // Get from the query string the processes selected by the user and collect the different
  // hosts from them into a map. (One command will reload all drivers on a given host.)
  string host;
  map<string,string> urls; // hostMachine->URL
  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
    // Identify processes by the colon between host and port
    string::size_type colonPosition = fe->getName().find( ':', 0 );
    if ( colonPosition != string::npos && fe->getValue() == "on" ){
      if ( fe->getName().size() < 7 ){
	LOG4CPLUS_WARN( logger_, "Suspicious element in HTTP query string: " 
			+ fe->getName() + " cannot possibly be a valid host:port." );
      }
      // Remember it's been selected
      const string url = "http://"+fe->getName();
      emuProcessDescriptors_[url].setSelected();

      if ( group == "DAQ" ){
	// It must be a DAQ process with EmuRUI
	if ( ! emuProcessDescriptors_[url].hasApplication( "EmuRUI" ) ) continue;
      }
      else if ( group == "PC" ){
	// It must be a PC process with EmuPeripheralCrate
	if ( ! emuProcessDescriptors_[url].hasApplication( "EmuPeripheralCrate" ) ) continue;
      }

      // Chop off port number
      host = fe->getName().substr( 0, colonPosition );

      // Get host machine name, stripped of any domain name.
      bool isIPNumber = false;
      try{
	isIPNumber = toolbox::regx_match( host, "^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$" );
      }catch( std::bad_alloc e ){
	LOG4CPLUS_WARN( logger_, "Failed to check whether host name " << 
			host << " is a resolved IP number: " << e.what() );
      }catch( std::invalid_argument e ){
	LOG4CPLUS_WARN( logger_, "Failed to check whether host name " << 
			host << " is a resolved IP number: " << e.what() );
      }
      if ( !isIPNumber ){
	// strip host of any domain name
	string::size_type dotPosition = host.find( '.', 0 );
	if ( dotPosition != string::npos ) host = host.substr( 0, dotPosition );
      }
      urls[host] = url;
    }
  }

  // Loop over urls to reload the drivers on them
  for ( map<string,string>::iterator u = urls.begin(); u != urls.end(); ++u ){
    // Reload the driver
    try
      {
	actOnEmuProcess( "reload", u->second );
      }
    catch(xdaq::exception::Exception e)
      {
	// Let's not rethrow here, but rather let it go on to do the rest.
	LOG4CPLUS_ERROR( logger_, 
			 "Failed to reload " << group << " driver for " + fe->getName() + ": "
			 + xcept::stdformat_exception_history(e));
      }
  }
}

string EmuFarmer::printNodeList( DOMNodeList *List ){
  string printout("");
  if ( !List ) return printout;
  for ( unsigned int i=0; i<List->getLength(); ++i )
    printout += string("   ") 
      + xoap::XMLCh2String( List->item(0)->getNodeName() )
      + string(" = ")
      + xoap::XMLCh2String( List->item(0)->getNodeValue() )
      + string("\n");
  return printout;
}

xoap::MessageReference EmuFarmer::createSOAPCommandToHatch( const string& url ){
  // Creates a SOAP command to be sent to JobControl to hatch this Emu process.

  xoap::MessageReference message = xoap::createMessage();

  // Find which user is to run this process
  string group = emuGroups_[url];
  if ( group.size() == 0 ){
    LOG4CPLUS_ERROR( logger_, url + " belongs to no group." );
    return message;
  }
  xdata::String* User = users_[group];
  if ( User == 0 ){
    LOG4CPLUS_ERROR( logger_, url + "'s group (" + group + ") has no user associated to it.");
    return message;
  }
  
  // Contruct SOAP
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName("executeCommand", "xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
  string user     = User->toString();
  string execPath = pathToHome_.toString() + string("/") + user + string("/") + commandToStartXDAQ_.toString();
  stringstream argv;
  argv << " -h " << emuProcessDescriptors_[url].getNormalizedHost()
       << " -p " << emuProcessDescriptors_[url].getPort()
       << " -l " << emuProcessDescriptors_[url].getStartingLogLevel()
       << " -c " << mergedConfigFileURL_;
  name = envelope.createName("execPath","","");
  bodyelement.addAttribute( name, execPath );
  name = envelope.createName("argv","","");
  bodyelement.addAttribute( name, argv.str() );
  name = envelope.createName("user","","");
  bodyelement.addAttribute( name, user );

  return message;
}

xoap::MessageReference EmuFarmer::createSOAPCommandToCull( const string& url ){
  // Creates a SOAP command to be sent to JobControl to cull this Emu process.

  xoap::MessageReference message = xoap::createMessage();

  // Find the job id of this process
  EmuProcessDescriptor process = emuProcessDescriptors_[url];
  // process.print( cerr );
  if ( process.getJobId() < 0 ){
    LOG4CPLUS_ERROR( logger_, "Failed to kill " + url + " as it has no job id associated to it.");
    return message;
  }
  stringstream jobId;
  jobId << process.getJobId();

  // Contruct SOAP
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName("killExec", "xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
  name = envelope.createName("jid", "", "");
  bodyelement.addAttribute( name, jobId.str() );

  return message;
}

xoap::MessageReference EmuFarmer::createSOAPCommandToReload( const string& url ){
  // Creates a SOAP command to be sent to JobControl to reload the DDU driver for this Emu process.

  xoap::MessageReference message = xoap::createMessage();

  // Find which user is to run this process
  string group = emuGroups_[url];
  if ( group.size() == 0 ){
    LOG4CPLUS_ERROR( logger_, url + " belongs to no group." );
    return message;
  }
  xdata::String* User = users_[group];
  if ( User == 0 ){
    LOG4CPLUS_ERROR( logger_, url + "'s group (" + group + ") has no user associated to it.");
    return message;
  }
  
  // Contruct SOAP
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName("executeCommand", "xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPBodyElement bodyelement = envelope.getBody().addBodyElement(name);
  string user     = User->toString();
  string execPath = pathToHome_.toString() + string("/") + user + string("/");
  if      ( group == "DAQ" ) execPath += commandToReloadDriversForDAQ_.toString();
  else if ( group == "PC"  ) execPath += commandToReloadDriversForPC_.toString();
  stringstream argv;
//   argv << " " << emuProcessDescriptors_[url].getDeviceName();
  name = envelope.createName("execPath","","");
  bodyelement.addAttribute( name, execPath );
//   name = envelope.createName("argv","","");
//   bodyelement.addAttribute( name, argv.str() );
  name = envelope.createName("user","","");
  bodyelement.addAttribute( name, user );

  name = envelope.createName("EnvironmentVariable", "", "");
  xoap::SOAPElement childelement = bodyelement.addChildElement( name );
  string envVarName  = "HOME";
  string envVarValue = pathToHome_.toString() + string("/") + user;
  name = envelope.createName( envVarName, "", "" );
  childelement.addAttribute( name, envVarValue );

//   message->writeTo(cerr);
  return message;
}

DOMNode* EmuFarmer::findNode( DOMNodeList *nodeList, const string& nodeLocalName )
  throw (xdaq::exception::Exception)
{
    DOMNode            *node = 0;
    string             name  = "";
    unsigned int       i     = 0;

    for(i=0; i<nodeList->getLength(); i++)
    {
        node = nodeList->item(i);

        if(node->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            name = xoap::XMLCh2String(node->getLocalName());

            if(name == nodeLocalName)
            {
                return node;
            }
        }
    }

    XCEPT_RAISE(xdaq::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}

void EmuFarmer::webRedirect(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
			url.substr(0, url.find("/" + in->getenv("PATH_INFO") + "?" )) 
			 + "/" + in->getenv("PATH_INFO"));

// 	cout << endl << "URL " << url << endl;
// 	cout << "Redirect to " << url.substr(0, url.find("/" + in->getenv("PATH_INFO") + "?" )) 
// 	     << "/" << in->getenv("PATH_INFO") << endl;
}

string EmuFarmer::getDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << ""
     << setfill('0') << setw(2) << tm->tm_mon+1     << ""
     << setfill('0') << setw(2) << tm->tm_mday      << "_"
     << setfill('0') << setw(2) << tm->tm_hour      << ""
     << setfill('0') << setw(2) << tm->tm_min       << ""
     << setfill('0') << setw(2) << tm->tm_sec       << "_UTC";

  return ss.str();
}

xoap::MessageReference EmuFarmer::createParameterGetSOAPMsg
( const string appClass,
  const string paramName,
  const string paramType )
  throw (xdaq::exception::Exception)
{
  string appNamespace = "urn:xdaq-application:" + appClass;
  string paramXsdType = "xsd:" + paramType;

  try
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPPart soapPart = message->getSOAPPart();
      xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
      envelope.addNamespaceDeclaration("xsi",
				       "http://www.w3.org/2001/XMLSchema-instance");
      envelope.addNamespaceDeclaration("xsd",
				       "http://www.w3.org/2001/XMLSchema");
      envelope.addNamespaceDeclaration("soapenc",
				       "http://schemas.xmlsoap.org/soap/encoding/");
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName cmdName =
	envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPBodyElement cmdElement =
	body.addBodyElement(cmdName);
      xoap::SOAPName propertiesName =
	envelope.createName("properties", "xapp", appNamespace);
      xoap::SOAPElement propertiesElement =
	cmdElement.addChildElement(propertiesName);
      xoap::SOAPName propertiesTypeName =
	envelope.createName("type", "xsi",
			    "http://www.w3.org/2001/XMLSchema-instance");
      propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
      xoap::SOAPName propertyName =
	envelope.createName(paramName, "xapp", appNamespace);
      xoap::SOAPElement propertyElement =
	propertiesElement.addChildElement(propertyName);
      xoap::SOAPName propertyTypeName =
	envelope.createName("type", "xsi",
			    "http://www.w3.org/2001/XMLSchema-instance");

      propertyElement.addAttribute(propertyTypeName, paramXsdType);

      return message;
    }
  catch(xcept::Exception e)
    {
      XCEPT_RETHROW(xdaq::exception::Exception,
		    "Failed to create ParameterGet SOAP message for parameter " +
		    paramName + " of type " + paramType, e);
    }
}

string EmuFarmer::extractScalarParameterValueFromSoapMsg
( xoap::MessageReference msg,
  const string           paramName )
  throw (xdaq::exception::Exception)
{
  try
    {
      xoap::SOAPPart part = msg->getSOAPPart();
      xoap::SOAPEnvelope env = part.getEnvelope();
      xoap::SOAPBody body = env.getBody();
      DOMNode *bodyNode = body.getDOMNode();
      DOMNodeList *bodyList = bodyNode->getChildNodes();
      DOMNode *responseNode = findNode(bodyList, "ParameterGetResponse");
      DOMNodeList *responseList = responseNode->getChildNodes();
      DOMNode *propertiesNode = findNode(responseList, "properties");
      DOMNodeList *propertiesList = propertiesNode->getChildNodes();
      DOMNode *paramNode = findNode(propertiesList, paramName);
      DOMNodeList *paramList = paramNode->getChildNodes();
      DOMNode *valueNode = paramList->item(0);
      string paramValue = xoap::XMLCh2String(valueNode->getNodeValue());

      return paramValue;
    }
  catch(xcept::Exception e)
    {
      XCEPT_RETHROW(xdaq::exception::Exception,
		    "Parameter " + paramName + " not found", e);
    }
  catch(...)
    {
      XCEPT_RAISE(xdaq::exception::Exception,
		  "Parameter " + paramName + " not found");
    }
}

xoap::MessageReference EmuFarmer::postSOAP
(
 xoap::MessageReference message, 
 const string& URL,
 const int localId
 ) 
  throw (xdaq::exception::Exception)
// Adapted from xdaq::ApplicationContextImpl::postSOAP.
// This is necessary for sending SOAP to contexts not defined in this process's config file.
{
	
  bool setSOAPAction = false;
  if ( message->getMimeHeaders()->getHeader("SOAPAction").size() == 0 )
    {
      stringstream URN;
      URN << "urn:xdaq-application:lid=" << localId;
      message->getMimeHeaders()->setHeader("SOAPAction", URN.str());		
      setSOAPAction = true;
    }
	
  xoap::SOAPBody b = message->getSOAPPart().getEnvelope().getBody();
  DOMNode* node = b.getDOMNode();
	
  DOMNodeList* bodyList = node->getChildNodes();
  DOMNode* command = bodyList->item(0);
	
  if (command->getNodeType() == DOMNode::ELEMENT_NODE) 
    {                
      try
	{	
	  // Local dispatch: if remote and local address are on same host, get local messenger
			
	  // Get the address on the fly from the URL
	  pt::Address::Reference remoteAddress = pt::getPeerTransportAgent()
	    ->createAddress(URL,"soap");
				
	  pt::Address::Reference localAddress = 
	    pt::getPeerTransportAgent()->createAddress(getApplicationDescriptor()->getContextDescriptor()->getURL(),"soap");
			
	  // force here protocol http, service soap, because at this point we know over withc protocol/service to send.
	  // this allows specifying a host URL without the SOAP service qualifier
	  //		
	  std::string protocol = remoteAddress->getProtocol();
			
	  pt::PeerTransportSender* s = dynamic_cast<pt::PeerTransportSender*>(pt::getPeerTransportAgent()->getPeerTransport (protocol, "soap", pt::Sender));

	  // These two lines cannot be merges, since a reference that is a temporary object
	  // would delete the contained object pointer immediately after use.
	  //
	  pt::Messenger::Reference mr = s->getMessenger(remoteAddress, localAddress);
	  pt::SOAPMessenger& m = dynamic_cast<pt::SOAPMessenger&>(*mr);
	  xoap::MessageReference rep = m.send(message);	 
			
	  if (setSOAPAction)
	    {
	      message->getMimeHeaders()->removeHeader("SOAPAction");
	    }
	  return rep;
	}
      catch (xdaq::exception::HostNotFound& hnf)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", hnf);
	} 
      catch (xdaq::exception::ApplicationDescriptorNotFound& acnf)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", acnf);
	}
      catch (pt::exception::Exception& pte)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", pte);
	}
      catch(std::exception& se)
	{
	  XCEPT_RAISE (xdaq::exception::Exception, se.what());
	}
      catch(...)
	{
	  XCEPT_RAISE (xdaq::exception::Exception, "Failed to post SOAP message, unknown exception");
	}
    } 
  else
    {
      /*applicationDescriptorFactory_.unlock();
       */
      XCEPT_RAISE (xdaq::exception::Exception, "Bad SOAP message. Cannot find command tag");
    }

}


/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuFarmer)
