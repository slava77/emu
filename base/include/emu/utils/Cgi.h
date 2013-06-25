#ifndef __emu_utils_Cgi_h__
#define __emu_utils_Cgi_h__

#include "xgi/Method.h"
#include "xgi/Utils.h"

namespace xdaq { class Application; }

namespace emu { namespace utils {

/**
 * select elements from cgi response with names regex-mathcing the namePattern
 */
std::multimap<std::string,std::string> selectFromQueryString( std::vector<cgicc::FormEntry>& fev, const std::string namePattern );

/**
 * Redirect to the default page for the action not to be repeated if the 'reload' button is hit.
 */
void redirect(xgi::Input *in, xgi::Output *out);

/**
 * Safely redirect to the same page for the action not to be repeated if the 'reload' button is hit.
 */
void redirectToSelf(xgi::Input *in, xgi::Output *out);

/**
 * simple redirect to new URL
 */
void redirectTo( const std::string newURL, xgi::Output *out );

/**
 * Customized XDAQ-style web page header.
 * If title is empty, application's class name would be printed.
 * Additional html-header contents (e.g., links to CSS of JavaScript) could be passed via header_html string.
 */
void headerXdaq(xgi::Output * out,
                xdaq::Application * app,
                const std::string &title = "",
                const std::string &subtitle = "",
                const std::string &header_html = "");

/**
 * Custom web page footer.
 * Currently, it just properly closes </body></html>.
 */
void footer(xgi::Output * out);

/**
 * Pops up a "save as" dialog to save the \param contents into a file with initial name \param file_name
 */
void saveAsFileDialog(xgi::Output * out, const std::string &contents, const std::string &file_name);

/**
 * An extended version of xgi::bind that allows binding to a method of application class member.
 * It is useful for splitting large xdaq web applications into separate logical modules.
 *
 * Example:
 *
 * let's say a lestener Application has a member
 *   Foo bar_;
 * And class Foo has the following method:
 *   void Foo::Process(xgi::Input * in, xgi::Output * out)
 *
 * Then the application can create a method that is bound as an http call-back like this:
 *   bindMemberMethod(this, &foo_, &Foo::Process, "Process");
 *
 */
template <typename LISTENER, typename MEMBER>
void bindMemberMethod(LISTENER * obj,
                      MEMBER * member,
                      void (MEMBER::*func)(xgi::Input*,xgi::Output*),
                      const std::string & messageName)
{
  xgi::Method<MEMBER> * f = new xgi::Method<MEMBER>;
  f->obj_ = member;
  f->func_ = func;
  f->name_ = messageName;
  obj->addMethod(f, messageName);
}


}}

#endif
