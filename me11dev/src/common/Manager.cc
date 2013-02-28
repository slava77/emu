#include "emu/me11dev/Manager.h"
#include "emu/me11dev/utils.h"

#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/Crate.h"

//#include "xdaq/NamespaceURI.h"

#include "cgicc/HTMLClasses.h"

#include <iomanip>


#define USE_CRATE_N 0 // ignore anything but the first crate
#define XML_CONFIGURATION_FILE "/local.home/cscme11/config/pc/dans-crate-config.xml"
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
      currentActionVector_(0),
      logger_( Logger::getInstance( generateLoggerName() ) )
    {
      XMLParser xmlparser;
      xmlparser.parseFile(XML_CONFIGURATION_FILE);

      if (!xmlparser.GetEmuEndcap()) {
        // if something went wrong while parsing ...
        XCEPT_RAISE(xcept::Exception,
                    string("Could not parse xml crate configuration file, ") +
                    XML_CONFIGURATION_FILE + ".");
      }

      Crate * crate = xmlparser.GetEmuEndcap()->crates().at(USE_CRATE_N); // we could make this a member variable and not need to pass it around everywhere



      /************************************************************************
       * The Common Buttons, which are always available on the right hand-side
       * of the page.
       *
       ***********************************************************************/

      addCommonActionByTypename<HardReset>(crate);
      addCommonActionByTypename<L1Reset>(crate);
      addCommonActionByTypename<BC0>(crate);
      addCommonActionByTypename<ReadBackUserCodes>(crate);



      /************************************************************************
       * The Buttons, which are listed in the below order on the web page.
       *
       ***********************************************************************/
      
      putButtonsInGroup( "Routine Tests" );
      addActionByTypename<ReadBackUserCodes>(crate);

      putButtonsInGroup( "DCFEB Settings" );
      addActionByTypename<SetDMBDACs>(crate);
      addActionByTypename<SetComparatorThresholds>(crate);
      addActionByTypename<SetComparatorThresholdsBroadcast>(crate);
      addActionByTypename<SetUpComparatorPulse>(crate);
      addActionByTypename<SetUpPrecisionCapacitors>(crate);
      addActionByTypename<SetPipelineDepthAllDCFEBs>(crate);
      addActionByTypename<SetFineDelayForADCFEB>(crate);
      addActionByTypename<ShiftBuckeyesNormRun>(crate);

      putButtonsInGroup( "DCFEB Tests" );
      addActionByTypename<BuckShiftTest>(crate);

      putButtonsInGroup( "TMB/Trigger Tests" );
      addActionByTypename<PulseInternalCapacitors>(crate);
      addActionByTypename<PulseInternalCapacitorsCCB>(crate);
      addActionByTypename<PulsePrecisionCapacitors>(crate);
      addActionByTypename<PulsePrecisionCapacitorsCCB>(crate);
      addActionByTypename<TMBHardResetTest>(crate);
      
      putButtonsInGroup("Other Functions" );
      addActionByTypename<DDUReadKillFiber>(crate);
      addActionByTypename<DDUWriteKillFiber>(crate);
      addActionByTypename<ExecuteVMEDSL>(crate);
      addActionByTypename<IndaraButton>(crate);


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

      bindWebInterface();
    }


    void Manager::putButtonsInGroup(const string& groupname){
      if( find(groups_.begin(),groups_.end(), groupname) == groups_.end() ){
	// If this group doesn't exist, add it to the list (also, map::operator[] will create it automatically)
	groups_.push_back(groupname);
      }
      currentActionVector_ = &groupActions_[groupname];
    }

    void Manager::bindWebInterface()
    {
      xgi::bind( this, &Manager::defaultWebPage, "Default" );
      xgi::bind( this, &Manager::commonActionsCallback, "commonActions" );
      xgi::bind( this, &Manager::groupActionsCallback, "groupActions" );
      xgi::bind( this, &Manager::logActionsCallback, "logActions" );
    }

    void Manager::defaultWebPage(xgi::Input *in, xgi::Output *out)
    {
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
           << h1()
           << "ME1/1 B904 Test Routines"
           << h1()
	   << endl
	   << endl;
	

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
	*out << a().set("href","#"+withoutSpecialChars(groups_[g]))
	     << groups_[g]
	     << a()
	     << br() << endl;
      }
      *out << a().set("href","#OutputLog")
	   << "Output Log"
	   << a()
	   << br() << endl;


      // most actions will appear here
      for(uint g=0; g<groups_.size(); ++g) {

	// Group anchor and header
	*out << a().set("name",withoutSpecialChars(groups_[g]))
	     << a()
	     << endl;
	*out << hr()
	     << h3()
	     << groups_[g]
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
	<< a().set("name","OutputLog") << a() << endl
	<< hr()
	<< h3() << "Output Log"
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

      backToMainPage(in, out);
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

      backToMainPage(in, out);
    }

    void Manager::addAction(shared_ptr<Action> act) {
      if(!currentActionVector_) putButtonsInGroup(UNDEFINEDGROUP);
      currentActionVector_->push_back(act);
    }

    template <typename T>
    void Manager::addActionByTypename(Crate * crate) {
      if(!currentActionVector_) putButtonsInGroup(UNDEFINEDGROUP);
      currentActionVector_->push_back(shared_ptr<T>(new T(crate)));
    }

    void Manager::addCommonAction(shared_ptr<Action> act) {
      commonActions_.push_back(act);
    }

    template <typename T>
    void Manager::addCommonActionByTypename(Crate * crate) {
      commonActions_.push_back(shared_ptr<T>(new T(crate)));
    }

    void Manager::addLogAction(shared_ptr<LogAction> act) {
      logActions_.push_back(act);
    }

    template <typename T>
    void Manager::addLogActionByTypename(Crate * crate) {
      logActions_.push_back(shared_ptr<T>(new T(crate)));
    }


    void Manager::backToMainPage(xgi::Input * in, xgi::Output * out ) // Redirect back to the main page. -Joe
    {
      //// Use this after a "GET" button press to get back to the base url
      *out << HTMLDoctype(HTMLDoctype::eStrict)
           << endl
           << html().set("lang", "en").set("dir","ltr")
           << endl
           << head()
           << meta().set("http-equiv","Refresh").set("content","0; url=./")
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

    /**
     * Provides the factory method for the instantiation of applications.
     */
    XDAQ_INSTANTIATOR_IMPL(Manager)
  }
}

