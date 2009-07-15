#include "emu/farmer/Application.h"
#include "emu/farmer/version.h"
#include "emu/farmer/utils/Cgi.h"
#include "emu/farmer/utils/Xalan.h"
#include "emu/farmer/utils/FileSystem.h"
#include "emu/farmer/utils/SOAP.h"

#include "xgi/Method.h"
#include "xgi/Utils.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/MessageFactory.h"
#include "cgicc/HTMLClasses.h"

#include "toolbox/Runtime.h"
#include "toolbox/regex.h"
//#include <xercesc/util/XMLURL.hpp>
#include "xcept/tools.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"

#include <time.h>
#include <iomanip>
#include <iterator>

// headers needed for Xalan
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
// #include <xalanc/PlatformSupport/XalanMemoryManagerDefault.hpp> // TODO: remove with old stuff


#include <unistd.h> // for sleep()
#include <stdlib.h> // for getenv()

#include <xercesc/parsers/XercesDOMParser.hpp>
// #include <xercesc/sax/SAXException.hpp>

#include "xalanc/Include/XalanMemoryManagement.hpp"
#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/NodeRefList.hpp"
#include "xalanc/XalanDOM/XalanNodeList.hpp"
#include "xalanc/XalanDOM/XalanElement.hpp"
#include "xalanc/XalanDOM/XalanNamedNodeMap.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"



emu::farmer::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  logger_(Logger::getInstance(generateLoggerName()))
{
  getEnv();
  bindWebInterface();
  exportParams();
}

void emu::farmer::Application::getEnv(){
  if ( getenv("XDAQ_ROOT") != NULL ){
    XDAQ_ROOT_ = getenv("XDAQ_ROOT");
  }
}


string emu::farmer::Application::generateLoggerName()
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

void emu::farmer::Application::bindWebInterface(){
//   xgi::bind(this, &emu::farmer::Application::xslWebPage,        "XSL"        );
  xgi::bind(this, &emu::farmer::Application::defaultWebPage,    "Default"    );
  xgi::bind(this, &emu::farmer::Application::editorWebPage,     "Editor"     );
  xgi::bind(this, &emu::farmer::Application::selectorWebPage,   "Selector"   );
  xgi::bind(this, &emu::farmer::Application::dispatcherWebPage, "Dispatcher" );
  xgi::bind(this, &emu::farmer::Application::displayWebPage,    "Display"    );
}

void emu::farmer::Application::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  outputDir_         = "/tmp/farmer/out";
  rs3UserFile_       = "/nfshome0/cscdaq/config/.rs3User";
  generatingCommand_ = "cd /opt/xdaq/htdocs/emu/farmer/xml && ./generateConfigs.sh";
  uploadingCommand_  = "RCMS_HOME=/nfshome0/cscpro/RunControl /nfshome0/cscpro/bin/duckLoader --usc55 ";
  jobControlClass_   = "jobcontrol::Application";
  executiveClass_    = "executive::Application";

  s->fireItemAvailable( "outputDir"        , &outputDir_         );
  s->fireItemAvailable( "rs3UserFile"      , &rs3UserFile_       );
  s->fireItemAvailable( "generatingCommand", &generatingCommand_ );
  s->fireItemAvailable( "uploadingCommand" , &uploadingCommand_  );
  s->fireItemAvailable( "jobControlClass"  , &jobControlClass_   );
  s->fireItemAvailable( "executiveClass"   , &executiveClass_    );

}


void 
emu::farmer::Application::startExecutives(){

  createExecutives();

  bool allUp = true;
  const int nChecks = 5;
  for ( int iCheck = 0; iCheck<nChecks; iCheck++ ){
    ::sleep( (unsigned int)(1) );
    pollProcesses();
    // Check if all executives are up
    allUp = true;
    for ( map<string,ProcessDescriptor>::iterator ex = processDescriptors_.begin(); ex != processDescriptors_.end(); ++ex ){
      if ( ex->second.getName() == executiveClass_.toString() ){
	allUp &= ( ex->second.getState() == "ON" );
      }
    }
    if ( allUp ) break;
  }

  if ( allUp ){
    configureExecutives();
    ::sleep( (unsigned int)(1) );
    pollProcesses();
  }
  else{
    XCEPT_RAISE( xcept::Exception, "Failed to create all selected XDAQ exectutives: " );
  }
}

void 
emu::farmer::Application::createExecutives(){
  for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){
    if ( pd->second.isSelected() && pd->second.getName() == executiveClass_.toString() ){
      try{
	xoap::MessageReference message = 
	  emu::farmer::utils::createStartXdaqExeSOAPMsg( pd->second.getHost(),
							 pd->second.getPort(),
							 pd->second.getUser(),
							 pd->second.getJid(),
							 pd->second.getEnvironmentVariables() );
	xoap::MessageReference reply = 
	  emu::farmer::utils::postSOAP( message, 
					getApplicationDescriptor()->getContextDescriptor()->getURL(),
					processDescriptors_[pd->second.getJobControlURI()].getURL(),
					processDescriptors_[pd->second.getJobControlURI()].getLid() );
	//reply->writeTo( cout );
	//if ( !reply.isNull() ){
	//  string jid( emu::farmer::utils::extractJidFromSoapMsg( reply ) );
	//  pd->second.setJid( jid ); Create jid automatically on creating the process descriptor instead 
	//                            in order to be able to kill executives after working with some other .duck(s).
	//}
      } catch( xcept::Exception& e ){
	LOG4CPLUS_WARN( logger_, "Failed to create XDAQ executive process at " 
			<< pd->second.getURL() << " : " << xcept::stdformat_exception_history(e) );
      }
    }
  }
}

void 
emu::farmer::Application::configureExecutives(){
  for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){
    if ( pd->second.isSelected() && pd->second.getName() == executiveClass_.toString() ){
      try{
	xoap::MessageReference message = 
	  emu::farmer::utils::createConfigureXdaqExeSOAPMsg( xdaqConfigs_[pd->second.getXdaqConfigPath()] );
	//message->writeTo( cout );
	xoap::MessageReference reply = 
	  emu::farmer::utils::postSOAP( message, 
					getApplicationDescriptor()->getContextDescriptor()->getURL(),
					pd->second.getURL(),
					pd->second.getLid() );
	//reply->writeTo( cout );
      } catch( xcept::Exception& e ){
	LOG4CPLUS_WARN( logger_, "Failed to configure XDAQ executive at " 
			<< pd->second.getURL() << " : " << xcept::stdformat_exception_history(e) );
      }
    }
  }  
}

void 
emu::farmer::Application::destroyExecutives(){
  for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){
    if ( pd->second.isSelected() && pd->second.getName() == executiveClass_.toString() ){
      try{
	xoap::MessageReference message = emu::farmer::utils::createKillByJidSOAPMsg( pd->second.getJid() );
	xoap::MessageReference reply = emu::farmer::utils::postSOAP( message, 
								     getApplicationDescriptor()->getContextDescriptor()->getURL(),
								     processDescriptors_[pd->second.getJobControlURI()].getURL(),
								     processDescriptors_[pd->second.getJobControlURI()].getLid() );
	//reply->writeTo( cout );
      } catch( xcept::Exception& e ){
	LOG4CPLUS_WARN( logger_, "Failed to destroy process at " 
			<< pd->second.getURL() << " : " << xcept::stdformat_exception_history(e) );
      }
    }
  }
}

void 
emu::farmer::Application::pollProcesses(){

  for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){

    pd->second.setState( "UNKNOWN" );

    string URL = pd->second.getURL();

    string parameterName;
    if ( pd->second.getName() == executiveClass_.toString() ) parameterName = "logLevel";
    else                                                    parameterName = "stateName";

    xoap::MessageReference SOAPMessage;
    try{
      SOAPMessage = emu::farmer::utils::createParameterGetSOAPMsg( pd->second.getName(), parameterName, "string" );
    } catch ( xcept::Exception e ) {
      LOG4CPLUS_ERROR( logger_, "Failed to create SOAP message to poll process at " 
		       << URL << " : " << xcept::stdformat_exception_history(e) );
    }
    
    xoap::MessageReference reply;
    try{
      string fromURL( getApplicationDescriptor()->getContextDescriptor()->getURL() );
      reply = emu::farmer::utils::postSOAP( SOAPMessage, fromURL, URL, 0 );
    } catch ( xcept::Exception e ) {
      LOG4CPLUS_WARN( logger_, "No reply to polling from process at " 
		      << URL << " : " << xcept::stdformat_exception_history(e) );
    }

    string parameterValue;
    if ( !reply.isNull() ){
      try{
	parameterValue = emu::farmer::utils::extractScalarParameterValueFromSoapMsg( reply, parameterName );
      } catch ( xcept::Exception e ) {
	LOG4CPLUS_WARN( logger_, "No " << parameterName << " found in reply to polling from process at " 
			<< URL << " : " << xcept::stdformat_exception_history(e) );
      }
    }  

    if ( pd->second.getName() == executiveClass_.toString() ){
      if ( parameterValue.size() ) pd->second.setState( "ON"  );
      else                         pd->second.setState( "OFF" );
    }
    else{
      if ( parameterValue.size() ) pd->second.setState( parameterValue );
      else                         pd->second.setState( "UNKNOWN" );
    }
    //cout << endl; SOAPMessage->writeTo( cout ); cout << endl;
    //cout << endl; reply->writeTo( cout ); cout << endl;

  }
}

string
emu::farmer::Application::generateConfigFilesWithScript()
  throw( xcept::Exception ){
  // Xalan doesn't seem to handle the function "document('')" properly for the XSL to process itself. 
  // Use the old script instead, which invokes the proven "xsltproc" stand-alone program.
  stringstream stdout;
  string command = generatingCommand_.toString() + " " 
    + outputDir_.toString() + "/RUI-to-computer_mapping.xml " 
    + outputDir_.toString() + " 2>&1";
  vector<string> replyLines = emu::farmer::utils::execShellCommand( command );
  copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(stdout, "\n") );
  return stdout.str();
}

void emu::farmer::Application::generateConfigFiles()
  throw( xcept::Exception ){

  const char* sideName[] = { "", "_sideP", "_sideM" };
  const char* sides[] = { "B", "P", "M" };
  const char* build[] = { "Y", "N" };
  const char* write[] = { "Y", "N" };
  //
  // DUCK file for DQM Display
  //
  string dqmDisplayConfigName( "DQM_Display" );
  try{
    stringstream mapping( editedMapping_ );
    map<string,string> params;
    params["FARM"] = "'DQM_Display'";
    params["NAME"] = "'DAQ_writeY_buildY'"; // any of them would do
    fstream configGenetrator( (XDAQ_ROOT_+"/htdocs/emu/farmer/xml/EmuDAQDUCKGenerator.xsl").c_str(), fstream::in );
    fstream config( (outputDir_.toString()+"/"+dqmDisplayConfigName+".duck").c_str(), fstream::out );
    emu::farmer::utils::transformWithParams( mapping, configGenetrator, config, params );
  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to generate config file \"" << dqmDisplayConfigName << ".duck" << "\": ";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to generate config file \"" << dqmDisplayConfigName << ".duck" << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to generate config file \"" << dqmDisplayConfigName << ".duck" << "\": unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  for ( unsigned int iSide=0; iSide<sizeof(sides)/sizeof(char*); ++iSide ){
    //
    // DUCK files for DQM
    //
    stringstream dqmConfigName;
    dqmConfigName << "DQM" << sideName[iSide];
    try{
      stringstream mapping( editedMapping_ );
      map<string,string> params;
      params["SIDE"] = string("'") + string(sides[iSide]) + "'";
      params["FARM"] = "'DQM'";
      params["NAME"] = string("'") + dqmConfigName.str() + "'";
      fstream configGenetrator( (XDAQ_ROOT_+"/htdocs/emu/farmer/xml/EmuDAQDUCKGenerator.xsl").c_str(), fstream::in );
      fstream config( (outputDir_.toString()+"/"+dqmConfigName.str()+".duck").c_str(), fstream::out );
      emu::farmer::utils::transformWithParams( mapping, configGenetrator, config, params );
    }catch( xcept::Exception& e ){
      stringstream ess; ess << "Failed to generate config file \"" << dqmConfigName.str() << ".duck" << "\": ";
      XCEPT_RETHROW( xcept::Exception, ess.str(), e );
    }catch( std::exception& e ){
      stringstream ess; ess << "Failed to generate config file \"" << dqmConfigName.str() << ".duck" << "\": " << e.what();
      XCEPT_RAISE( xcept::Exception, ess.str() );
    }catch(...){
      stringstream ess; ess << "Failed to generate config file \"" << dqmConfigName.str() << ".duck" << "\": unexpected exception.";
      XCEPT_RAISE( xcept::Exception, ess.str() );
    }
    for ( unsigned int iBuild=0; iBuild<sizeof(build)/sizeof(char*); ++iBuild ){
      for ( unsigned int iWrite=0; iWrite<sizeof(write)/sizeof(char*); ++iWrite ){
	stringstream daqConfigName;
	daqConfigName << "DAQ"    << sideName[iSide] 
		      << "_write" << write[iWrite] 
		      << "_build" << build[iBuild];
	//
	// DUCK files for DAQ
	//
	try{
	  stringstream mapping( editedMapping_ );
	  map<string,string> params;
	  params["SIDE"] = string("'") + string(sides[iSide]) + "'";
	  params["FARM"] = "'DAQ'";
	  params["NAME"] = string("'") + daqConfigName.str() + "'";
	  fstream configGenetrator( (XDAQ_ROOT_+"/htdocs/emu/farmer/xml/EmuDAQDUCKGenerator.xsl").c_str(), fstream::in );
	  fstream config( (outputDir_.toString()+"/"+daqConfigName.str()+".duck").c_str(), fstream::out );
	  emu::farmer::utils::transformWithParams( mapping, configGenetrator, config, params );
	}catch( xcept::Exception& e ){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".duck" << "\": ";
	  XCEPT_RETHROW( xcept::Exception, ess.str(), e );
	}catch( std::exception& e ){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".duck" << "\": " << e.what();
	  XCEPT_RAISE( xcept::Exception, ess.str() );
	}catch(...){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".duck" << "\": unexpected exception.";
	  XCEPT_RAISE( xcept::Exception, ess.str() );
	}
	//
	// common XDAQ config files for DAQ and DQM and DQM_Display
	//
	try{
	  stringstream mapping( editedMapping_ );
	  map<string,string> params;
	  params["SIDE"]  = string("'") + string(sides[iSide ]) + "'";
	  params["WRITE"] = string("'") + string(write[iWrite]) + "'";
	  params["BUILD"] = string("'") + string(build[iBuild]) + "'";
	  fstream configGenetrator( (XDAQ_ROOT_+"/htdocs/emu/farmer/xml/EmuDAQConfigGenerator.xsl").c_str(), fstream::in );	  
	  fstream config( (outputDir_.toString()+"/"+daqConfigName.str()+".xml").c_str(), fstream::out );
	  //cout << "Transforming with " << XDAQ_ROOT_ << "/htdocs/emu/farmer/xml/EmuDAQConfigGenerator.xsl" << endl;
	  emu::farmer::utils::transformWithParams( mapping, configGenetrator, config, params );
	}catch( xcept::Exception& e ){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".xml" << "\": ";
	  XCEPT_RETHROW( xcept::Exception, ess.str(), e );
	}catch( std::exception& e ){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".xml" << "\": " << e.what();
	  XCEPT_RAISE( xcept::Exception, ess.str() );
	}catch(...){
	  stringstream ess; ess << "Failed to generate config file \"" << daqConfigName.str() << ".xml" << "\": unexpected exception.";
	  XCEPT_RAISE( xcept::Exception, ess.str() );
	}
      }
    }
  }
}

string 
emu::farmer::Application::getRS3UserInfo()
  throw( xcept::Exception ){
  string pw;
  try{
    fstream fs( rs3UserFile_.toString().c_str(), ios::in );
    fs >> pw;
    fs.close();
    if ( pw.size() == 0 ){
      stringstream ss;
      ss << "RS3 user info file \"" <<  rs3UserFile_.toString() << "\" is empty?!";
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }catch( xcept::Exception& e ){
    stringstream ss;
    ss << "Failed to get RS3 user info from file \"" <<  rs3UserFile_.toString() << "\": ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }catch( std::exception& e ){
    stringstream ss;
    ss << "Failed to get RS3 user info from file \"" <<  rs3UserFile_.toString() << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }catch(...){
    stringstream ss;
    ss << "Failed to get RS3 user info from file \"" <<  rs3UserFile_.toString() << "\": unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return pw;
}

string
emu::farmer::Application::hidePw( const string& in )
  throw( xcept::Exception ){
  string out( in );
  try{
    string pw = getRS3UserInfo();
    size_t pwPos = string::npos;
    while ( ( pwPos=out.find( pw ) ) != string::npos ) {
      out.replace( pwPos, pw.size(), "********");
    }
  }catch( xcept::Exception& e ){
    stringstream ss;
    ss << "Failed to process stdout of uploading command: ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }catch( std::exception& e ){
    stringstream ss;
    ss << "Failed to process stdout of uploading command: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }catch(...){
    stringstream ss;
    ss << "Failed to process stdout of uploading command: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return out;
}

string
emu::farmer::Application::uploadDUCKFile( const string duckFileName )
  throw( xcept::Exception ){
  stringstream stdout;
  string command = uploadingCommand_.toString() + " " + getRS3UserInfo() + " " + duckFileName + " 2>&1";
  vector<string> replyLines = emu::farmer::utils::execShellCommand( command );
  copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(stdout, "\n") );
  return hidePw( stdout.str() );
}

string
emu::farmer::Application::recreateMapping( std::vector<cgicc::FormEntry>& fev )
  throw( xcept::Exception ){
  stringstream ss;
  string pattern;
  try{
    ss << "<?xml version='1.0' encoding='UTF-8'?>" << endl
       << "<?xml-stylesheet type='text/xml' href='RUI-to-computer_mapping_XSLT.xml'?>" << endl
       << "  <!--Generated by emu::farmer-->" << endl
       << "<RUI-to-computer_mapping date='" << getDateTime() << "'>" << endl;
    map<string,string> aliases = emu::farmer::utils::selectFromQueryString( fev, "^c[0-9]*\\.alias$" );
    for ( unsigned int iComp = 0; iComp < aliases.size(); ++iComp ){
      stringstream comp;
      comp << "c" << iComp;
      if ( aliases.find( comp.str()+".alias" ) == aliases.end() ){
	comp << ".alias not found in query string: ";
	XCEPT_RAISE( xcept::Exception, comp.str() );
      }
      else{
	ss << "<computer alias='" << aliases[comp.str()+".alias"] << "'>" << endl;
	map<string,string> instances = emu::farmer::utils::selectFromQueryString( fev, comp.str()+"\\.rui[0-9]*\\.instance$" );
	map<string,string> attributes = emu::farmer::utils::selectFromQueryString( fev, comp.str()+"\\.rui[0-9]*\\." );
	for ( unsigned int iRUI = 0; iRUI < instances.size(); ++iRUI ){
	  stringstream rui;
	  rui << comp.str() << ".rui" << iRUI;
	  if ( attributes.find( rui.str()+".instance" ) == attributes.end() ||
	       attributes.find( rui.str()+".schar"    ) == attributes.end()
	       ){
	    rui << " attributes not all found in query string: ";
	    XCEPT_RAISE( xcept::Exception, rui.str() );
	  }
	  bool isIn = ( attributes.find( rui.str()+".status" ) != attributes.end() );
	  ss << "<RUI instance='" << attributes[rui.str()+".instance"]
	     << "' schar='"       << attributes[rui.str()+".schar"]
	     << "' status='"      << ( isIn ? "in" : "out" )
	     << "'/>" << endl;

	}
	ss << "</computer>" << endl;
      }
    }
    ss << "</RUI-to-computer_mapping>" << endl;
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to create mapping XML: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to create mapping XML: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to create mapping XML: unexpected exception." );
  }
  return ss.str();
}

string emu::farmer::Application::setProcessingInstruction( const string XML, const string xslURI )
  throw( xcept::Exception ){
  stringstream target;
  try{
    if ( XML.size() == 0 ){
      XCEPT_RAISE( xcept::Exception, "XML document is empty." );
    }
    // Load P.I. setting XSLT if it hasn't yet beed loaded.
    if ( processingInstructionSetter_.size() == 0 ){
      processingInstructionSetter_ = emu::farmer::utils::readFile( XDAQ_ROOT_ + "/htdocs/emu/farmer/xml/processingInstructionSetter.xsl" );
    }
    stringstream stylesheet( processingInstructionSetter_ );
    stringstream source; source << XML;
    map<string,string> params;
    params["XSLURI"] = "'" + xslURI + "'";
    emu::farmer::utils::transformWithParams( source, stylesheet, target, params );
  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to set processing instruction.";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to set processing instruction: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to set processing instruction: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }

  return target.str();
}

string emu::farmer::Application::insertStatuses( const string DUCK ){
  string result( DUCK );
  for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){
    result = insertStatus( result, &(pd->second) );
  }
  return result;
}

string emu::farmer::Application::insertStatus( const string DUCK, const ProcessDescriptor* pd )
  throw( xcept::Exception ){
  stringstream target;
  try{
    // Load status inserter XSLT if it hasn't yet beed loaded.
    if ( statusInserter_.size() == 0 ){
      statusInserter_ = emu::farmer::utils::readFile( XDAQ_ROOT_ + "/htdocs/emu/farmer/xml/statusInserter.xsl" );
    }
    stringstream stylesheet( statusInserter_ );
    stringstream source; source << DUCK;
    stringstream port; port << pd->getPort();
    map<string,string> params;
    params["HOST"    ] = string("'") + pd->getHost()                           + "'";
    params["PORT"    ] = string("'") + port.str()                              + "'";
    params["URN"     ] = string("'") + pd->getPath()                           + "'";
    params["SELECTED"] = string("'") + ( pd->isSelected() ? "true" : "false" ) + "'";
    params["STATE"   ] = string("'") + pd->getState()                          + "'";
    emu::farmer::utils::transformWithParams( source, stylesheet, target, params );
  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to insert process status into '" << DUCK << "'.";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to insert process status into '" << DUCK << "': " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to insert process status into '" << DUCK << "': unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }

  return target.str();
}


void emu::farmer::Application::createProcessDescriptors()
  throw( xcept::Exception ){

  XALAN_USING_XALAN(XercesDOMSupport);
  XALAN_USING_XALAN(XercesParserLiaison);
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource);
  
  XALAN_USING_XALAN(XPathEvaluator);
  XALAN_USING_XALAN(XalanDocument);
  XALAN_USING_XALAN(XalanDocumentPrefixResolver);
  XALAN_USING_XALAN(XalanDOMString);
  XALAN_USING_XALAN(XalanNode);
  XALAN_USING_XALAN(XalanNodeList);
  XALAN_USING_XALAN(XalanElement);
  XALAN_USING_XALAN(XObjectPtr);
  
  XALAN_USING_XALAN(XSLException);
//   XALAN_USING_XERCES(SAXException);
  XALAN_USING_XERCES(XMLException);
  XALAN_USING_XERCES(DOMException);
  
  // XalanNode::getChildNodes() for the result nodes of the XPath evaluation is not supported
  // in the default Xalan source tree implementation. We must use a wrapper around the Xerces-C DOM.

  try {
    processDescriptors_.clear();

    XercesDOMParser parser;
    parser.setDoValidation(true);
    parser.setDoNamespaces(true);
    parser.parse( selectedDUCKName_.c_str() );
    DOMDocument* doc = parser.getDocument();
    
    XercesDOMSupport support;
    XercesParserLiaison liaison(support);
    XercesDOMWrapperParsedSource src(doc, liaison, support);
    XalanDocument* xalanDoc = src.getDocument();
    
    XPathEvaluator evaluator;
    XalanDocumentPrefixResolver resolver(xalanDoc);

    // Start from the root node:    
    XalanNode* const theContextNode = evaluator.selectSingleNode( support,
								  xalanDoc,
								  XalanDOMString("/").c_str(),
								  resolver );

    XalanDOMString xpath("/Configuration/FunctionManager/*");
    XObjectPtr result( evaluator.evaluate( support, theContextNode, xpath.c_str( ), resolver) );
    
    //cout << result->str() << endl;
    
    XALAN_USING_XALAN(XObject);
    XALAN_USING_XALAN(XalanNode);
    if ( result->getType() == XObject::eTypeNodeSet ){
      //       cout << "   result->nodeset().getLength()  " << result->nodeset().getLength() << endl;
      for ( unsigned int iNode=0; iNode<result->nodeset().getLength(); ++iNode ){
	XalanNode *node = result->nodeset().item( iNode );
	// 	cout << "---------" << endl;
	// 	cout << "   node->getNodeName()  " << node->getNodeName()   << endl;
	// 	cout << "   node->getNodeValue() " << node->getNodeValue()  << endl;
	// 	cout << "   node->getNodeType()  " << node->getNodeType()   << endl;
	// 	cout << "   node->hasChildNodes()  " << node->hasChildNodes() << endl;
	// 	cout << "   node->getChildNodes()->getLength()  " << node->getChildNodes()->getLength() << endl;
	// 	cout << "   node->getAttributes()  " << node->getAttributes()->getLength() << endl;
	XalanDOMString X_hostname("hostname");
	XalanDOMString X_port("port");
	XalanDOMString X_urn ("urn" );
	XalanDOMString X_name("className");
	XalanDOMString X_environmentString ( "environmentString" );
	XalanDOMString X_unixUser ( "unixUser" );
	XalanDOMString X_configFile ( "configFile" );
	stringstream env;
	stringstream user;
	stringstream uri;
	stringstream configFile;
	uri << "http://" << node->getAttributes()->getNamedItem( X_hostname )->getNodeValue()
	    << ":"       << node->getAttributes()->getNamedItem( X_port     )->getNodeValue()
	    << "/"       << node->getAttributes()->getNamedItem( X_urn      )->getNodeValue();
	stringstream name;
	if ( node->getNodeName() == XalanDOMString("Service") ){ // JobControl
	  name << jobControlClass_.toString();
	}
	else if ( node->getNodeName() == XalanDOMString("XdaqExecutive") ){
	  name << executiveClass_.toString();
	  env  << node->getAttributes()->getNamedItem( X_environmentString )->getNodeValue();
	  user << node->getAttributes()->getNamedItem( X_unixUser )->getNodeValue();
	  // Get config file
	  for ( unsigned int iChild = 0; iChild < node->getChildNodes()->getLength(); ++iChild ){
	    if ( node->getChildNodes()->item( iChild )->getNodeName() == X_configFile ){
	      XalanNode *configFileNode = node->getChildNodes()->item( iChild );
	      for ( unsigned int jChild = 0; jChild < configFileNode->getChildNodes()->getLength(); ++jChild ){
		if ( configFileNode->getChildNodes()->item( jChild )->getNodeType() == XalanNode::TEXT_NODE ){
		  configFile << configFileNode->getChildNodes()->item( jChild )->getNodeValue();
		}
	      }
	    }
	  }
	  // Read in config file and store it
	  if ( configFile.str().size() > 0 ){
	    string cfgXML( emu::farmer::utils::readFile( configFile.str() ) );
	    xdaqConfigs_[configFile.str()] = cfgXML;
	  }
	  else{
	    XCEPT_RAISE( xcept::Exception, "XDAQ executive has no configuration file." );
	  }
	}
	else if ( node->getNodeName() == XalanDOMString("XdaqApplication") ){
	  name << node->getAttributes()->getNamedItem( X_name )->getNodeValue();
	}
	emu::farmer::ProcessDescriptor pd( uri.str() );
	pd.setName( name.str() );
	pd.setEnvironmentString( env.str() );
	pd.setUser( user.str() );
	pd.setXdaqConfigPath( configFile.str() );
	//pd.print( cout );
	processDescriptors_[ uri.str() ] = pd;
      }
    }
    // We're responsible for releasing the memory allocated to DOMDocument
    doc->release();
  }
//   catch( SAXException& e ){
//     stringstream ss; ss << "Failed to collect processes from .duck file: " << e.getMessage();
//     XCEPT_RAISE( xcept::Exception, ss.str() );
//   }
  catch( XMLException& e ){
    stringstream ss; ss << "Failed to collect processes from .duck file: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException& e ){
    stringstream ss; ss << "Failed to collect processes from .duck file: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "Failed to collect processes from .duck file: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to collect processes from .duck file: ", e );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to collect processes from .duck file: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to collect processes from .duck file: Unknown exception." );
  }
}

void 
emu::farmer::Application::assignJobControls(){
  // Assign each XDAQ executive a JobControl
  // Loop over executives
  for ( map<string,ProcessDescriptor>::iterator ex = processDescriptors_.begin(); ex != processDescriptors_.end(); ++ex ){
    if ( ex->second.getName() == executiveClass_.toString() ){
      bool notAssignedJC = true;
      // Loop over JobControls
      for ( map<string,ProcessDescriptor>::iterator jc = processDescriptors_.begin(); jc != processDescriptors_.end() && notAssignedJC; ++jc ){
	if ( jc->second.getName() == jobControlClass_.toString() ){
	  if ( ex->second.getNormalizedHost() == jc->second.getNormalizedHost() ){
	    ex->second.setJobControlURI( jc->first );
	    notAssignedJC = false;
	  }
	}
      }
      if ( notAssignedJC ){
	stringstream ss;
	ss << "Failed to find a JobControl for " << ex->second.getName() << " at " << ex->second.getURL();
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }
    }
    //ex->second.print( cout );
  }
}

void emu::farmer::Application::editorWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  try{

    ProcessDescriptor pd("http://csc-daq08.cms:9999/urn:xdaq-application:lid=10");

    cgicc::Cgicc cgi( in );
    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    
    // Load default mapping if it hasn't yet beed loaded.
    if ( defaultMapping_.size() == 0 ){
      defaultMapping_ = emu::farmer::utils::readFile( XDAQ_ROOT_ + "/htdocs/emu/farmer/xml/RUI-to-computer_mapping.xml" );
      editedMapping_  = defaultMapping_;
    }

    // Action
    map<string,string> actionNameValue = emu::farmer::utils::selectFromQueryString( fev, "^Action$" );
    //for ( map<string,string>::const_iterator nv = actionNameValue.begin(); nv != actionNameValue.end(); ++nv )
    //  cout << nv->first << " " << nv->second << endl;
    if ( actionNameValue.size() ){
      if ( actionNameValue["Action"] == "Verify..." ){
	map<string,string> nameValue = emu::farmer::utils::selectFromQueryString( fev, ".*" );
	//for ( map<string,string>::const_iterator nv = nameValue.begin(); nv != nameValue.end(); ++nv )
	//  cout << nv->first << " " << nv->second << endl;
	editedMapping_ = recreateMapping( fev );
 	*out << setProcessingInstruction( editedMapping_, "/emu/farmer/html/RUI-to-computer_mapping_Verify_XSLT.xml" );
	return;
      }
      else if ( actionNameValue["Action"] == "Reset" ){
	// Reset to the default mapping.
	editedMapping_  = defaultMapping_;
 	*out << setProcessingInstruction( defaultMapping_, "/emu/farmer/html/RUI-to-computer_mapping_Editor_XSLT.xml" );
	return;
      }
      else if ( actionNameValue["Action"] == "Edit" ){
	// Display in the editor the current mapping.
 	*out << setProcessingInstruction( editedMapping_, "/emu/farmer/html/RUI-to-computer_mapping_Editor_XSLT.xml" );
	return;
      }
      else if ( actionNameValue["Action"] == "Generate" ){
	// Generate files and list them.

	// Save edited RUI mapping to file
	fstream ruiMappingFile( (outputDir_.toString()+"/RUI-to-computer_mapping.xml").c_str(), fstream::out );
	ruiMappingFile.write( editedMapping_.c_str(), editedMapping_.size() );
	ruiMappingFile.close();

	//generateConfigFiles();
	// Xalan doesn't seem to handle the function "document('')" properly for the XSL to process itself. 
	// Use the old script instead, which invokes the proven "xsltproc" stand-alone program.
	cout << generateConfigFilesWithScript() << endl;

	stringstream newURL;
	newURL << getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << getApplicationDescriptor()->getURN() << "/Selector";
	emu::farmer::utils::redirectTo( newURL.str(), in, out );
	return;
      }
      else{
	// Unknown action; just display in the current mapping.
 	*out << setProcessingInstruction( editedMapping_, "/emu/farmer/html/RUI-to-computer_mapping_Verify_XSLT.xml" );
	return;
      }
    }
    else{
      // No action; just display in the editor the current mapping.
      *out << setProcessingInstruction( editedMapping_, "/emu/farmer/html/RUI-to-computer_mapping_Editor_XSLT.xml" );
      return;
    }

  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Editor web page failed: ", e );
  }
}


void emu::farmer::Application::selectorWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  vector<string> duckList;
  vector<string> duckTimeList;
  // Get list of .duck files
  try{
    string command = string("ls -1 ") + outputDir_.toString() + "/*.duck";
    duckList = emu::farmer::utils::execShellCommand( command );
    command = string("ls -l --time-style=long-iso ")+ outputDir_.toString() + "/*.duck | awk '{ print $6\" \"$7 }'";
    duckTimeList = emu::farmer::utils::execShellCommand( command );
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to read in list of .duck files: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to read in list of .duck files: " << e.what();
    XCEPT_RAISE( xgi::exception::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xgi::exception::Exception, "Failed to read in list of .duck files: unexpected exception." );
  }

  try{
    string stdoutFromUploading;
    cgicc::Cgicc cgi( in );
    std::vector<cgicc::FormEntry> fev = cgi.getElements();

    // Action
    map<string,string> actionNameValue = emu::farmer::utils::selectFromQueryString( fev, "^Action$" );
    //for ( map<string,string>::const_iterator nv = actionNameValue.begin(); nv != actionNameValue.end(); ++nv )
    //  cout << nv->first << " " << nv->second << endl;
    if ( actionNameValue.size() ){
      if ( actionNameValue["Action"] == "Try..." ){
	map<string,string> nameValue = emu::farmer::utils::selectFromQueryString( fev, "^duck$" );
	for ( map<string,string>::const_iterator nv = nameValue.begin(); nv != nameValue.end(); ++nv ){
	  int duckIndex = -1;
	  stringstream di( nv->second ); di >> duckIndex;
	  //cout << nv->first << " " << nv->second << " " << duckList.at(duckIndex) << endl;
	  selectedDUCKName_ = duckList.at(duckIndex);
	  selectedDUCKOriginal_ = emu::farmer::utils::readFile( selectedDUCKName_ );
	  selectedDUCK_ = selectedDUCKOriginal_;
	  createProcessDescriptors();
	  assignJobControls();
	}
	emu::farmer::utils::redirectTo( "Dispatcher", in, out );
      }
      if ( actionNameValue["Action"] == "Upload" ){
	map<string,string> nameValue = emu::farmer::utils::selectFromQueryString( fev, "^duck$" );
	for ( map<string,string>::const_iterator nv = nameValue.begin(); nv != nameValue.end(); ++nv ){
	  int duckIndex = -1;
	  stringstream di( nv->second ); di >> duckIndex;
	  //cout << nv->first << " " << nv->second << " " << duckList.at(duckIndex) << endl;
	  //stdoutFromUploading = uploadDUCKFile( "nonexistent" );
	  stdoutFromUploading = uploadDUCKFile( duckList.at(duckIndex) );
	}
	//uploadDuckFile( "nonexistent" );
      }
      if ( actionNameValue["Action"] == "Upload All" ){
	for ( vector<string>::const_iterator d = duckList.begin(); d != duckList.end(); ++d ){
	  //cout << *d << endl;
	  //uploadDuckFile( "nonexistent" );
	  stdoutFromUploading += uploadDUCKFile( *d );
	}
      }
    }
    // Write XML
    *out << "<?xml version='1.0' encoding='UTF-8'?>" << endl
	 << "<?xml-stylesheet type='text/xml' href='/emu/farmer/html/Selector_XSLT.xml'?>" << endl
	 << "<selector>" << endl
	 << "<duckList>" << endl;
    for ( unsigned int iDuck=0; iDuck < duckList.size(); ++iDuck ){
      *out << "<duck"
	   << " file='" << duckList.at(iDuck)     << "'"
	   << " time='" << duckTimeList.at(iDuck) << "'"
	   << "/>" << endl;
    }
    *out << "</duckList>" << endl;
    *out << "<stdout><![CDATA[" << stdoutFromUploading << "]]></stdout>" << endl;
    *out << "</selector>" << endl;
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to read and process selected .duck file: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to read and process selected .duck file: " << e.what();
    XCEPT_RAISE( xgi::exception::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xgi::exception::Exception, "Failed to read and process selected .duck file: unexpected exception." );
  }

}

void emu::farmer::Application::dispatcherWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  try{
    cgicc::Cgicc cgi( in );
    std::vector<cgicc::FormEntry> fev = cgi.getElements();

    // Get action
    map<string,string> actionNameValue = emu::farmer::utils::selectFromQueryString( fev, "^Action$" );
    //for ( map<string,string>::const_iterator nv = actionNameValue.begin(); nv != actionNameValue.end(); ++nv )
    //  cout << nv->first << " " << nv->second << endl;
    if ( actionNameValue.size() ){

      selectedDUCK_ = selectedDUCKOriginal_;

      // Remember user's selection
      map<string,string> nameValue = emu::farmer::utils::selectFromQueryString( fev, ":[0-9]*/" );
      for ( map<string,ProcessDescriptor>::iterator pd = processDescriptors_.begin(); pd != processDescriptors_.end(); ++pd ){
	pd->second.setSelected( nameValue.find( pd->first.substr(7) ) != nameValue.end() );
      }

      // Perform action
      if      ( actionNameValue["Action"] == "Create" ){
	startExecutives();
	selectedDUCK_ = insertStatuses( selectedDUCK_ );
	*out << setProcessingInstruction( selectedDUCK_, "/emu/farmer/html/Dispatcher_XSLT.xml" );
	return;
      }
      else if ( actionNameValue["Action"] == "Destroy" ){
	destroyExecutives();
	sleep( (unsigned int)(1) );
	pollProcesses();
	selectedDUCK_ = insertStatuses( selectedDUCK_ );
	*out << setProcessingInstruction( selectedDUCK_, "/emu/farmer/html/Dispatcher_XSLT.xml" );
	return;
      }
      else if ( actionNameValue["Action"] == "Poll" ){
	pollProcesses();
	selectedDUCK_ = insertStatuses( selectedDUCK_ );
	*out << setProcessingInstruction( selectedDUCK_, "/emu/farmer/html/Dispatcher_XSLT.xml" );
	return;
      }

    }
    else{
      *out << setProcessingInstruction( selectedDUCK_, "/emu/farmer/html/Dispatcher_XSLT.xml" );
    }
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to read in list of .duck files: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to read in list of .duck files: " << e.what();
    XCEPT_RAISE( xgi::exception::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xgi::exception::Exception, "Failed to read in list of .duck files: unexpected exception." );
  }
}

void emu::farmer::Application::displayWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  try{
    cgicc::Cgicc cgi( in );
    std::vector<cgicc::FormEntry> fev = cgi.getElements();

    // Get file path
    map<string,string> queryParams = emu::farmer::utils::selectFromQueryString( fev, "^file$" );
    //for ( map<string,string>::const_iterator qp = queryParams.begin(); qp != queryParams.end(); ++qp )
    //  cout << qp->first << " " << qp->second << endl;
    if ( queryParams.size() ){
      // It should normally have been read in already, but in case it somehow hasn't:
      if ( xdaqConfigs_.count( queryParams["file"] ) == 0 ){
	string cfgXML( emu::farmer::utils::readFile( queryParams["file"] ) );
	xdaqConfigs_[queryParams["file"]] = cfgXML;
      }
      *out << setProcessingInstruction( xdaqConfigs_[queryParams["file"]], "/emu/farmer/html/XDAQConfig_XSLT.xml" );
    }
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to display file: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to display file: " << e.what();
    XCEPT_RAISE( xgi::exception::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xgi::exception::Exception, "Failed to display file: unexpected exception." );
  }

}


void emu::farmer::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location", "/emu/farmer/html/Default.html");
  
  return;
}


string emu::farmer::Application::getDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << " "
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec       << " UTC";

  return ss.str();
}

/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::farmer::Application)
