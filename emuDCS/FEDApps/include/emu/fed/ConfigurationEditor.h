/*****************************************************************************\
* $Id: ConfigurationEditor.h,v 1.6 2009/11/22 22:52:22 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CONFIGURATIONEDITOR_H__
#define __EMU_FED_CONFIGURATIONEDITOR_H__

#include <string>
#include <vector>
#include <time.h>

#include "emu/fed/Application.h"
#include "emu/fed/JSONSpiritValue.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger64.h"

namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class ConfigurationEditor An XDAq application class that reads and exits the FED database (and XML files)
		**/
		class ConfigurationEditor : public emu::fed::Application
		{
			
		public:
			XDAQ_INSTANTIATOR();
			
			/** Default constructor **/
			ConfigurationEditor(xdaq::ApplicationStub *stub);
			
			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);
			
			/** Upload an XML file and save it to disk **/
			void webUploadFile(xgi::Input *in, xgi::Output *out);
			
			/** Get database keys **/
			void webGetDBKeys(xgi::Input *in, xgi::Output *out);
			
		private:
			
			/// Database user name
			xdata::String dbUsername_;
			
			/// Database password
			xdata::String dbPassword_;
			
			/// Current system name
			xdata::String systemName_;
			
			/// Current configuration time
			time_t timeStamp_;
			
			/// Current crate vector (with properly built crates)
			std::vector<Crate *> crateVector_;
			
			/// Current configuration key
			xdata::UnsignedInteger64 dbKey_;
			
		};
		
	}
}

#endif
