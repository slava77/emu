#ifndef __emu_me11dev_Manager_h__
#define __emu_me11dev_Manager_h__

#include<map>

#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdaq/NamespaceURI.h"

#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/pc/Crate.h"

#include "xgi/Method.h"

#include "xcept/tools.h"
#include "xcept/Exception.h"

#include <iomanip>
#include <sstream>

#include "emu/me11dev/Action.h"
#include "emu/me11dev/LogAction.h"
#include "emu/me11dev/Buttons.h"
#include "emu/me11dev/utils.h"

#include <boost/shared_ptr.hpp>

#include "emu/pc/Crate.h"

/******************************************************************************
 * The Manager Class
 *
 * This class actually produces a web page and is the xdaq applciation started
 * by the shell script in the script directory.
 *
 * It has a vector of "Action" objects which represent a single action that can
 * be taken on the crate/chamber. Buttons can be parameterized by various form
 * elements such as dropdown boxes or text boxes. There is already written
 * support for text boxes, but the programmer is free to override the `display'
 * method to provide access to other form elements.
 *****************************************************************************/

using namespace std;
using namespace emu::pc;
using namespace boost;

namespace emu { namespace me11dev {

    typedef vector< shared_ptr< Action> > t_actionvector;
    typedef map< string, t_actionvector > t_vectormap;

    class Manager : public virtual xdaq::WebApplication
    {

    public:
      /// define factory method for the instantion of AFEBTeststand applications
      XDAQ_INSTANTIATOR();

      /// constructor
      Manager( xdaq::ApplicationStub *s );

      // We have to use a pointer because otherwise you get "object slicing"
      // where C++ uses the superclass method instead of the subclass method.
      // Additionally, we use shared_ptr because we store them in a vector and
      // vectors are easier to delete when they do not contain raw pointers
      //
      // All that said, you probably want to use the ...ByTypename methods
      // which reduce your typing overhead
      void addAction(shared_ptr<Action> act);
      void addCommonAction(shared_ptr<Action> act);
      void addLogAction(shared_ptr<LogAction> act);

      // These work analgously to the above methods, except they use a bit of
      // STL type hackery to reduce the repitition of the class name
      template <typename T> void addActionByTypename(Crate * crate);
      template <typename T> void addCommonActionByTypename(Crate * crate);
      template <typename T> void addLogActionByTypename(Crate * crate);

    protected:
      ostringstream webOutputLog;
      // see the comment above addAction for why we use a vector of shared_ptr
      vector<shared_ptr<Action> > commonActions;
      t_vectormap groupactions; // map between the group names and they vector of associated actions
      vector<string> groups; // vector of the group names (determines the order of the groups on the GUI)
      vector<shared_ptr<LogAction> > logActions;
      Logger logger_;


      t_actionvector* current_actionvector; // pointer to the actionvector for the "current group"
      void PutButtonsInGroup(string groupname); // put buttons in this group, create the group if it doesn't exist
      string generateLoggerName();
      void bindWebInterface();
      void defaultWebPage(xgi::Input *in, xgi::Output *out);
      template <typename T> string numberToString(T number);
      void commonActionsCallback(xgi::Input *in, xgi::Output *out);
      void actionsCallback(xgi::Input *in, xgi::Output *out);
      void groupActionsCallback(xgi::Input *in, xgi::Output *out);
      void logActionsCallback(xgi::Input *in, xgi::Output *out);
      static int getFormValueInt(const string form_element, xgi::Input *in);
      static void BackToMainPage(xgi::Input * in, xgi::Output * out );
  };

}}

#endif
