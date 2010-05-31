/*****************************************************************************\
* $Id: Configurable.h,v 1.6 2010/05/31 14:57:19 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CONFIGURABLE_H__
#define __EMU_FED_CONFIGURABLE_H__

#include "emu/fed/Application.h"
#include "xdata/xdata.h"
#include "xdata/UnsignedInteger64.h"
#include "boost/filesystem/path.hpp"
#include "xdata/Integer.h"

namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class Configurable A class from which all FED XDAQ applications that can be configured to talk to the FED hardware should inherit.
		*
		*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
		**/
		class Configurable: public virtual emu::fed::Application
		{

		public:

			/** Default constructor. **/
			Configurable(xdaq::ApplicationStub *stub);
			
			/** Default destructor. **/
			~Configurable();
			
			/** Get the current configuration information of the application and report in JSON **/
			void webGetConfiguration(xgi::Input *in, xgi::Output *out);
			
			/** Change the type of configuration being used **/
			void webChangeConfigMode(xgi::Input *in, xgi::Output *out);
			
			/** Change the name of the XML file used in configuring **/
			void webChangeXMLFile(xgi::Input *in, xgi::Output *out);
			
			/** Change the name of the database key used in configuring **/
			void webChangeDBKey(xgi::Input *in, xgi::Output *out);
			
			/** Reconfigure the software from the web **/
			void webReconfigure(xgi::Input *in, xgi::Output *out);

		protected:
			
			/** Configures the software only. **/
			void softwareConfigure()
			throw (emu::fed::exception::ConfigurationException);
			
			/** Print the software configure options. **/
			std::string printConfigureOptions();
			
			/** Grabs the available DB keys from TStore **/
			std::map<std::string, std::vector<xdata::UnsignedInteger64> > getDBKeys()
			throw (emu::fed::exception::ConfigurationException);
			
			/** Grabs the available XML file names from the configuration directory **/
			std::vector<std::string> getXMLFileNames(const boost::filesystem::path &configDir);
			
			/// The system name for the application.  This is just some name that can be used to distinguish differently-configured applications from each other.
			xdata::String systemName_;
			
			/// A database key
			xdata::UnsignedInteger64 dbKey_;
			
			/// The username for database communication.
			xdata::String dbUsername_;
			
			/// The password for database communication.
			xdata::String dbPassword_;
			
			/// The XML configuration file name.
			xdata::String xmlFile_;
			
			/// The mode by which to configure the application.
			xdata::String configMode_;
			
			/// The TStore instance to use
			xdata::Integer tstoreInstance_;
			
			/// The ever-useful crate vector.
			std::vector<Crate *> crateVector_;

		};
	}
}


#endif
