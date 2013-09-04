#include "emu/me11dev/Manager.h"
#include "emu/me11dev/utils.h"

#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/Crate.h"
#include "emu/utils/System.h"
#include "emu/soap/Messenger.h"

//#include "xdaq/NamespaceURI.h"

#include "cgicc/HTMLClasses.h"
#include "xdata/Integer64.h"
#include "xdata/Boolean.h"

#include <iomanip>


#define USE_CRATE_N 0 // ignore anything but the first crate
//#define XML_CONFIGURATION_FILE "/home/cscme11/config/pc/pcrate37-config.xml"
#define UNDEFINEDGROUP "No Group Defined"

using namespace cgicc;
using namespace std;
using namespace emu::pc;
using namespace boost;

namespace emu { namespace me11dev {


    /**************************************************************************
     * The Constructor
     *
     * Here is where you can add actions or common actions. Buttons
     * are plain buttons or buttons with text fields that are
     * displayed on the page. Common actions are actions which scroll
     * in pace with the page and float to the right hand side of the
     * page. They are useful for common actions that should always be
     * visible, such as hard reset.
     *************************************************************************/
    Manager::Manager( xdaq::ApplicationStub *s ) :
      xdaq::WebApplication( s ),
      webOutputLog_(), 
      OutputLogTitle_("Output Log"),
      currentActionVector_(0),
      logger_( Logger::getInstance( generateLoggerName() ) ),
      tmbSlot_(-1)
    {
      bindWebInterface();

      xdata::InfoSpace *is = getApplicationInfoSpace();
      is->fireItemAvailable( "XMLConfigFilename", &xmlConfig_);
      is->fireItemAvailable( "TMBSlot", &tmbSlot_ );
      //is->fireItemAvailable( "", &_ );
    }


    void Manager::bindWebInterface()
    {
      xgi::bind( this, &Manager::defaultWebPage, "Default" );
      xgi::bind( this, &Manager::commonActionsCallback, "commonActions" );
      xgi::bind( this, &Manager::groupActionsCallback, "groupActions" );
      xgi::bind( this, &Manager::logActionsCallback, "logActions" );
    }


    void Manager::putButtonsInGroup(const string& groupname)
    {
      if( find(groups_.begin(),groups_.end(), groupname) == groups_.end() ){
        // If this group doesn't exist, add it to the list (also, map::operator[] will create it automatically)
        groups_.push_back(groupname);
      }
      currentActionVector_ = &groupActions_[groupname];
    }


    void Manager::firstUse()
    {
      // run it only once when the Default page is loaded
      static bool firstTime = true;
      if (firstTime) firstTime = false;
      else return;

      // expand possible environment variables
      xmlConfig_.fromString(emu::utils::performExpansions( xmlConfig_.toString() ));

      cout<<"ME11Dev: will load XML config file: '"<<xmlConfig_.toString()<<"'"<<endl;
      XMLParser xmlparser;
      xmlparser.parseFile(xmlConfig_.toString());

      if (!xmlparser.GetEmuEndcap()) 
      {
        // if something went wrong while parsing ...
        XCEPT_RAISE(xcept::Exception,
            string("Could not parse xml crate configuration file, ") +
            xmlConfig_.toString() + ".");
      }

      Crate * crate = xmlparser.GetEmuEndcap()->crates().at(USE_CRATE_N); // we could make this a member variable and not need to pass it around everywhere

      cout<<"ME11Dev: configured to test TMB in slot "<<int(tmbSlot_)<<endl;


      /************************************************************************
       * The Common Buttons, which are always available on the right hand-side
       * of the page.
       *
       ***********************************************************************/

      addCommonActionByTypename<HardReset>(crate);
      addCommonActionByTypename<SoftReset>(crate);
      addCommonActionByTypename<L1Reset>(crate);
      addCommonActionByTypename<BC0>(crate);
      addCommonActionByTypename<ReadBackUserCodes>(crate);
      addCommonActionByTypename<CommonUtilities_restoreCFEBIdle>(crate);
      addCommonActionByTypename<CommonUtilities_setupDDU>(crate);
      addCommonActionByTypename<CommonUtilities_setupDDU_passThrough>(crate);
      //addCommonActionByTypename<ButtonTests>(crate,this);

      /************************************************************************
       * The Buttons, which are listed in the below order on the web page.
       *
       ***********************************************************************/

      
      putButtonsInGroup( "Routine Tests" );
      //addActionByTypename<STEP16TestBed>(crate,this);
      //addActionByTypename<STEP15TestBed>(crate,this);
      addActionByTypename<ODMBCosmicsTest>(crate, this);	
      addActionByTypename<ODMBCosmicsTestV2>(crate, this);	
      addActionByTypename<ODMBStepTest16>(crate, this);
      addActionByTypename<RoutineTest_StandardCosmics>(crate, this);
      addActionByTypename<RoutineTest_ShortCosmicsRun>(crate, this);
      addActionByTypename<RoutineTest_ShortCosmicsRun_DDUPT>(crate, this);
      addActionByTypename<RoutineTest_PrecisionPulses>(crate, this);
      addActionByTypename<RoutineTest_PatternPulses_TMBCounters>(crate, this);
      

      putButtonsInGroup( "DCFEB" );
      addActionByTypename<SetDMBDACs>(crate);
      addActionByTypename<SetComparatorMode>(crate);
      addActionByTypename<SetComparatorThresholds>(crate);
      //addActionByTypename<SetComparatorThresholdsBroadcast>(crate);
      addActionByTypename<SetPipelineDepthAllDCFEBs>(crate);
      //addActionByTypename<ReadPipelineDepthAllDCFEBs>(crate);
      addActionByTypename<SetFineDelayForADCFEB>(crate);
      addActionByTypename<ShiftBuckeyesNormRun>(crate);
      addActionByTypename<BuckShiftTest>(crate);
      addActionByTypename<BuckShiftTestDebug>(crate, this);
      addActionByTypename<dcfebDebugDump>(crate);

      putButtonsInGroup( "Pulsing" );
      addActionByTypename<SetUpComparatorPulse>(crate);
      addActionByTypename<PulseInternalCapacitorsCCB>(crate);
      //addActionByTypename<PulseInternalCapacitorsDMB>(crate); // don't use DMB pulse/inject because they do not send an L1a to the whole system
      addActionByTypename<SetUpPrecisionCapacitors>(crate);
      addActionByTypename<PulsePrecisionCapacitorsCCB>(crate);
      //addActionByTypename<PulsePrecisionCapacitorsDMB>(crate);

      putButtonsInGroup( "TMB" );
      addActionByTypename<TMBRegisters>(crate);
      addActionByTypename<TMBDisableCopper>(crate);
      addActionByTypename<TMBEnableCLCTInput>(crate);
      addActionByTypename<TMBDisableCLCTInput>(crate);
      addActionByTypename<TMBSetRegisters>(crate, this);
      addActionByTypename<TMBHardResetTest>(crate);
      addActionByTypename<SetTMBdavDelay>(crate);
      
      putButtonsInGroup( "AFEB" );
      addActionByTypename<PulseWires>(crate);

      putButtonsInGroup("Scans" );
      addActionByTypename<PipelineDepthScan_Cosmics>( crate, this );
      addActionByTypename<PipelineDepthScan_Pulses>( crate, this );
      addActionByTypename<L1aDelayScan>( crate, this );
      addActionByTypename<TmbDavDelayScan>( crate, this );

      putButtonsInGroup("DDU" );
      addActionByTypename<DDU_KillFiber>(crate);
      addActionByTypename<DDU_EthPrescale>(crate);
      addActionByTypename<DDU_FakeL1>(crate);

      putButtonsInGroup("Special Functions" );
      addActionByTypename<ButtonTests>(crate,this);
      //addActionByTypename<Stans_SetPipelineDepthAllDCFEBs>(crate);
      addActionByTypename<ExecuteVMEDSL>(crate);
      addActionByTypename<enableVmeDebugPrintout>(crate);
      addActionByTypename<Investigate_MissingEvents>(crate,this);
      addActionByTypename<ODMB_L1A_Testing>(crate,this);
      addActionByTypename<ODMB_OTMB_LCT_Testing>(crate,this);
      addActionByTypename<STEP9bFibers>(crate,this);

      /************************************************************************
       * Log Buttons
       *
       * These are used for maintaining the log displayed on the web page. You
       * probably don't need to modify these. However, it would be neat for
       * someone to add a write to file button.
       ***********************************************************************/

      addLogActionByTypename<ClearLog>(crate);
      addLogActionByTypename<DumpLog>(crate);
      addLogActionByTypename<SaveLogAsFile>(crate);
    }


    void Manager::defaultWebPage(xgi::Input *in, xgi::Output *out)
    {
      firstUse();

      *out << HTMLDoctype(HTMLDoctype::eStrict)
           << endl
           << endl
           << html().set("lang", "en").set("dir","ltr")
           << head()
           << style().set("rel", "stylesheet").set("type", "text/css")
           << "" // you could add page-wide styles here
           << style()
           << script().set("type", "text/javascript")
           // I appologize to the programming gods for writing JavaScript as
           // a string inside a C++ program ... (NB gcc will concatenate
           // adjacent string literals)
           << "function toggleSidebox() {"
              "  var elements = document.getElementsByClassName('sidebox');"
              "  Array.prototype.slice.call(elements, 0).map("
              "    function (e) { "
              "      e.style.display = e.style.display == 'none' ? 'block' "
              "                                                  : 'none'"
              "    })"
              "}"
           << script()
           << head()
           << endl
           << body().set("style","padding-bottom: 10em; color: #333; ")
           << endl
           << cgicc::div().set("style","font-size: xx-small")
           << a().set("href","/") << "<< back to XDAQ" << a()
           << cgicc::div()
           << endl
           << h1()
           << "ME1/1 B904 Test Routines"
           << h1()
           << endl << endl;


      // begin: floating right hand side box
      *out << cgicc::div()
	.set("style",
	     string("position:fixed;") +
	     "float:right;" +
	     "border: #000 solid 1px;" +
	     "top: 1em;" +
	     "right: 1em;" +
	     "padding: 1em;" +
	     "background-color: #eee")
	   << endl
	// the minimize button
           << cgicc::a()
	.set("onclick", "toggleSidebox();")
	.set("accesskey", "m")
	.set("style",
	     string("position:absolute;") +
	     "float:right;" +
	     "border: #000 solid 1px;" +
	     "top: 0.5em;" +
	     "right: 0.5em;" +
	     "background-color: #222;" +
	     "color: #eee;" +
	     "font-weight: bold;" +
	     "text-decoration: none;")
           << "&mdash;"
           << cgicc::a()
	   << endl
           << h3().set("class", "sidebox") 
	   << "Common Utilities" 
	   << h3() << endl;

      // this is only for common actions which we always want visible
      for(unsigned int i = 0; i < commonActions_.size(); ++i) {
        // this multi-line statement sets up a form for the action,
        // which will create buttons, etc. The __action_to_call hidden
        // form element tells the Manager which action to use when
        // this form is submitted.
        *out << p()
             << cgicc::form()
	  .set("class", "sidebox")
	  .set("method","GET")
	  .set("action", "commonActions")
             << cgicc::input()
	  .set("type","hidden")
	  .set("value",numberToString(i))
	  .set("name","__action_to_call")
             << endl;
	
        commonActions_[i]->display(out);
	
        // and here we close the form
        *out << cgicc::form()
             << p()
             << endl;
      }
      
      *out << cgicc::div() << endl << endl; // end: floating right hand side box


      
      //// Make links to each group header
      for(uint g=0; g<groups_.size(); ++g) {
	*out << a().set("href","#"+withoutSpecialChars(groups_[g])) << groups_[g] << a()
	     << br() << endl;
      }
      *out << a().set("href",string("#")+withoutSpecialChars(OutputLogTitle_)) << OutputLogTitle_ << a()
	   << br() << endl;


      // most actions will appear here
      for(uint g=0; g<groups_.size(); ++g) {

	// Group anchor and header
	*out << a().set("name",withoutSpecialChars(groups_[g]))
	     << a()
	     << endl;
	*out << hr()
	     << h3()
	     << a().set("href",string("#")+withoutSpecialChars(groups_[g])) << groups_[g] << a()
	     << "&nbsp;&nbsp;&nbsp;" << a().set("href","") << "(top)" << a() 
	     << h3()
	     << endl;

	t_actionvector av=groupActions_[groups_[g]];
	for(unsigned int i = 0; i <av.size(); ++i) {

	  // this multi-line statement sets up a form for the action,
	  // which will create buttons, etc. The __action_to_call hidden
	  // form element tells the Manager which action to use when
	  // this form is submitted.
	  *out << p()
	       << cgicc::form()
	    .set("method","GET")
	    .set("action", "groupActions")
	       << cgicc::input()
	    .set("type","hidden")
	    .set("value",numberToString(i*groups_.size()+g))
	    .set("name","__action_to_call")
	       << endl;
	  
	  av[i]->display(out);
	  
	  // and here we close the form
	  *out << cgicc::form()
	       << p()
	       << endl;
	}
      }


      *out
	<< endl
	<< a().set("name",withoutSpecialChars(OutputLogTitle_)) << a() << endl
	<< hr()
	<< h3() << a().set("href",string("#")+withoutSpecialChars(OutputLogTitle_)) << OutputLogTitle_ << a()
	<< "&nbsp;&nbsp;&nbsp;" << a().set("href","") << "(top)" << a() 
	<< h3();
		   
      for(unsigned int i = 0; i < logActions_.size(); ++i) { // display log buttons at the top
        *out << p()
	     << cgicc::form()
	  .set("method","GET")
	  .set("action", "logActions")
             << cgicc::input()
	  .set("type","hidden")
	  .set("value",numberToString(i))
	  .set("name","__action_to_call");
	
	logActions_[i]->display(out);
	
	*out << cgicc::form()
	     << p();
      }

      *out << textarea().set("style","width: 100%; height: 100em; ")
           // NB, I purposely called .str(), I don't want to remove all the
           // contents of the log into the web page, I want them to persist
           << webOutputLog_.str()
           << textarea();

      for(unsigned int i = 0; i < logActions_.size(); ++i) { // display log buttons at the bottom
        *out << p()
	     << cgicc::form()
	  .set("method","GET")
	  .set("action", "logActions")
             << cgicc::input()
	  .set("type","hidden")
	  .set("value",numberToString(i))
	  .set("name","__action_to_call");
	
	logActions_[i]->display(out);

	*out << cgicc::form()
	     << p();
      }
      *out 
	<< endl
	<< body() 
	<< html()
	<< endl;
    }

    void Manager::commonActionsCallback(xgi::Input *in, xgi::Output *out)
    {
      int action_to_call = getFormValueInt("__action_to_call", in);

      ostringstream action_output;

      commonActions_.at(action_to_call)->respond(in, action_output);

      webOutputLog_ << action_output.str();

      backToMainPage(in, out);
    }

    void Manager::groupActionsCallback(xgi::Input *in, xgi::Output *out)
    {
      int action_to_call = getFormValueInt("__action_to_call", in);

      ostringstream action_output;

      int g = action_to_call%groups_.size();
      int i = action_to_call/groups_.size();
      groupActions_[groups_[g]].at(i)->respond(in, action_output);

      webOutputLog_ << action_output.str();

      string anchor = "#"+withoutSpecialChars(groups_[g]);
      backToMainPage(in, out, anchor);
    }

    void Manager::logActionsCallback(xgi::Input *in, xgi::Output *out)
    {
      int action_to_call = getFormValueInt("__action_to_call", in);

      ostringstream action_output;

      logActions_.at(action_to_call)->respond(in, out, action_output, webOutputLog_);

      webOutputLog_ << action_output.str();

      // if the content was saved as a log file,
      // don't append the header to out, and no need to do BackToMainPage
      if (action_output.str() == "*** Contents above was saved to a log file ***") return;

      backToMainPage(in, out, string("#")+withoutSpecialChars(OutputLogTitle_));
    }

    void Manager::addAction(shared_ptr<Action> act) {
      if(!currentActionVector_) putButtonsInGroup(UNDEFINEDGROUP);
      currentActionVector_->push_back(act);
      currentActionVector_->back()->useTMBInSlot(tmbSlot_);
    }

    template <typename T>
    void Manager::addActionByTypename(Crate * crate) {
      if(!currentActionVector_) putButtonsInGroup(UNDEFINEDGROUP);
      currentActionVector_->push_back(shared_ptr<T>(new T(crate)));
      currentActionVector_->back()->useTMBInSlot(tmbSlot_);
    }

    template <typename T>
    void Manager::addActionByTypename(Crate * crate, emu::me11dev::Manager* manager ) {
      if(!currentActionVector_) putButtonsInGroup(UNDEFINEDGROUP);
      currentActionVector_->push_back(shared_ptr<T>(new T(crate, manager)));
      currentActionVector_->back()->useTMBInSlot(tmbSlot_);
    }

    void Manager::addCommonAction(shared_ptr<Action> act) {
      act->useTMBInSlot(tmbSlot_);
      commonActions_.push_back(act);
    }

    template <typename T>
    void Manager::addCommonActionByTypename(Crate * crate, emu::me11dev::Manager* manager ) {
      commonActions_.push_back(shared_ptr<T>(new T(crate, manager)));
      commonActions_.back()->useTMBInSlot(tmbSlot_);
    }

    template <typename T>
    void Manager::addCommonActionByTypename(Crate * crate) {
      commonActions_.push_back(shared_ptr<T>(new T(crate)));
      commonActions_.back()->useTMBInSlot(tmbSlot_);
    }

    void Manager::addLogAction(shared_ptr<LogAction> act) {
      logActions_.push_back(act);
      logActions_.back()->useTMBInSlot(tmbSlot_);
    }

    template <typename T>
    void Manager::addLogActionByTypename(Crate * crate) {
      logActions_.push_back(shared_ptr<T>(new T(crate)));
      logActions_.back()->useTMBInSlot(tmbSlot_);
    }


    // Redirect back to the main page. -Joe
    void Manager::backToMainPage(xgi::Input * in, xgi::Output * out, const std::string& anchor )
    {
      //// Use this after a "GET" button press to get back to the base url
      *out << HTMLDoctype(HTMLDoctype::eStrict)
           << endl
           << html().set("lang", "en").set("dir","ltr")
           << endl
           << head()
           << meta().set("http-equiv","Refresh").set("content","0; url=./" + anchor)
           << head()
           << endl
           << body()
           << p() << "Operation Complete" << cgicc::p()
           << body()
           << endl
           << html()
           << endl;
    }


    string Manager::generateLoggerName()
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

    void Manager::setDAQOutSubdir( const string& subdir ){
      xdata::String pathToRUIDataOutFile = string( "/local/data/" ) + subdir;
      emu::utils::execShellCommand( string( "mkdir -p " ) + (string) pathToRUIDataOutFile );
      emu::soap::Messenger( this ).setParameters( "emu::daq::rui::Application", emu::soap::Parameters().add( "pathToRUIDataOutFile", &pathToRUIDataOutFile ) );
    }

    void Manager::startDAQ( const string& runtype ){
      emu::soap::Messenger m( this );
      //
      // Configure DAQ
      //
      xdata::String    runType            = ( runtype.size() == 0 ? "Monitor" : runtype );
      xdata::Integer64 maxNumberOfEvents  = -1; // unlimited if negative
      xdata::Boolean   writeBadEventsOnly = false;
      m.setParameters( "emu::daq::manager::Application", 
		       emu::soap::Parameters()
		       .add( "runType"           , &runType            )
		       .add( "maxNumberOfEvents" , &maxNumberOfEvents  )
		       .add( "writeBadEventsOnly", &writeBadEventsOnly ) );
      m.sendCommand( "emu::daq::manager::Application", "Configure" );      
      waitForDAQToExecute( "Configure", 10 );
      //
      // Enable DAQ
      //
      m.sendCommand( "emu::daq::manager::Application", "Enable" );
      waitForDAQToExecute( "Enable", 10 );

//       xdata::Integer64 tmp;
//       m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "maxNumberOfEvents", &tmp ) );
//       cout<<" emu::daq::manager::Application ==> maxNumberOfEvents = "<<tmp.toString()<<endl;
//       cout<<" emu::daq::manager::Application ==> maxNumberOfEvents = "<<tmp.toString()<<endl;
//       cout<<" emu::daq::manager::Application ==> maxNumberOfEvents = "<<tmp.toString()<<endl;
//       cout<<" emu::daq::manager::Application ==> maxNumberOfEvents = "<<tmp.toString()<<endl;
//       cout<<" emu::daq::manager::Application ==> maxNumberOfEvents = "<<tmp.toString()<<endl;
    }

    void Manager::stopDAQ(){
      emu::soap::Messenger m( this );
      emu::soap::Messenger( this ).sendCommand( "emu::daq::manager::Application", "Halt" );
      waitForDAQToExecute( "Halt", 10 );
    }

    bool Manager::waitForDAQToExecute( const string command, const uint64_t seconds ){
      string expectedState;
      if      ( command == "Configure" ){ expectedState = "Ready";   }
      else if ( command == "Enable"    ){ expectedState = "Enabled"; }
      else if ( command == "Halt"      ){ expectedState = "Halted";  }
      else                              { return true; }
      
      // Poll, and return TRUE if and only if DAQ gets into the expected state before timeout.
      emu::soap::Messenger m( this );
      xdata::String  daqState;
      for ( uint64_t i=0; i<=seconds; ++i ){
	m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "daqState", &daqState ) );
	if ( daqState.toString() != "Halted"  && daqState.toString() != "Ready" && 
	     daqState.toString() != "Enabled" && daqState.toString() != "INDEFINITE" ){
	  LOG4CPLUS_ERROR( logger_, "Local DAQ is in " << daqState.toString() << " state. Please destroy and recreate local DAQ." );
	  return false;
	}
	if ( daqState.toString() == expectedState ){ return true; }
	LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for local DAQ to get " 
			<< expectedState << ". It is still in " << daqState.toString() << " state." );
	::sleep( 1 );
      }
      
      LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		       << ". It is in " << daqState.toString() << " state." );
      return false;
    }

    /**
     * Provides the factory method for the instantiation of applications.
     */
    XDAQ_INSTANTIATOR_IMPL(Manager)
  }
}

