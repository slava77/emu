#ifndef __emu_step_Configuration_h__
#define __emu_step_Configuration_h__

#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"
#include "toolbox/BSem.h"

#include <map>
#include <vector>

using namespace std;

namespace emu { namespace step {

    class Configuration
    {
    public:

      /// constructor
      Configuration( const string& XMLnamespace, 
		     xdata::String& configurationXSLFileName,
		     xdata::String& testParametersFileName,
		     const map<string,string>& pCrateSettingsFileNames );
      xdata::Vector<xdata::String> getCrateIds( const string& group ) const; ///< Returns the selected crate ids in the given group.
      xdata::Vector<xdata::String> getChamberLabels( const string& group ) const; ///< Returns the selected chamber labels in the given group.
      xdata::Vector<xdata::String> getChamberLabels() const; ///< Returns the selected chamber labels.
      xdata::Vector<xdata::String> getTestIds() const; ///< Returns the selected crate ids.
      string getXML() const { return xml_; }
      string getTestParametersXML() const { return testParametersXML_; }
      string getModificationTime() const { return modificationTime_; }
      void setSelection( const multimap<string,string>& values );
      void setTestStatus( const string& testId, const string& status, const string& details );
      void setTestStatus( const string& status, const string& details );
      void setTestProgress( const map<string,pair<double,string> >& groupsProgress );

    private:

      void createXML();

      mutable toolbox::BSem bsem_;	///< Binary semaphore.
      string namespace_;
      string xml_; ///< The configuration XML.
      string xslt_; ///< The configuration XSLT.
      string modificationTime_; ///< Date and time of last modification.
      string testParametersXML_;
      map<string,string> pCrateSettingsXMLs_; ///< peripheral crate group --> peripheral crate XML map
    };

}}

#endif
