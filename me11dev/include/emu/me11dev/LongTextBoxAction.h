#ifndef __emu_me11dev_LongTextBoxAction_h__
#define __emu_me11dev_LongTextBoxAction_h__

#include "emu/me11dev/Action.h"

/******************************************************************************
 * The LongTextBoxAction Class
 *
 * This is a class for accepting long-form (like gmail's compose
 * box). Currently, it is only used for Joe Haley's VME command
 * domain-specific-langauge.
 *
 * Be certain to call the respond method from LongTextBoxAction (the base
 * class) in your derived classes so that the `textBoxContents' field gets
 * loaded with the user's input.
 * E.g.:
 * 
 *   class DoesSomethingWithLongText : public LongTextBoxAction {
 *   
 *     // ...
 *
 *     void respond(xgi::Input * in) {
 *       LongTextBoxAction::respond(in);
 *
 *       // your code here
 *     }
 *   }
 *
 *****************************************************************************/


namespace emu { namespace me11dev {

    class LongTextBoxAction : public Action
    {
    public:

      LongTextBoxAction(emu::pc::Crate * crate, const std::string& buttonLabel);

      void display(xgi::Output * out);
      void respond(xgi::Input * in, std::ostringstream & out);

    protected:

      std::string buttonLabel_;
      std::string textBoxContents_;
    };
  }
}

#endif

