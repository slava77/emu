/*****************************************************************************\
* $Id: ConfigurationEditor.h,v 1.7 2009/12/10 16:55:02 paste Exp $
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
			
			/** Load from the DB given a configuration key **/
			void webLoadFromDB(xgi::Input *in, xgi::Output *out);
			
			/** Create a new configuration from scratch **/
			void webCreateNew(xgi::Input *in, xgi::Output *out);
			
			/** Write configuration to XML **/
			void webWriteXML(xgi::Input *in, xgi::Output *out);
			
			/** Build a system and delete the old system **/
			void webBuildSystem(xgi::Input *in, xgi::Output *out);
			
			/** Build a crate and add it to the crate vector **/
			void webBuildCrate(xgi::Input *in, xgi::Output *out);
			
			/** Build a controller and add it to the specified crate **/
			void webBuildController(xgi::Input *in, xgi::Output *out);
			
			/** Build a DDU and add it to the specified crate **/
			void webBuildDDU(xgi::Input *in, xgi::Output *out);
			
			/** Build a fiber and add it to the specified DDU **/
			void webBuildFiber(xgi::Input *in, xgi::Output *out);
			
			/** Build a DCC and add it to the specified crate **/
			void webBuildDCC(xgi::Input *in, xgi::Output *out);
			
			/** Build a FIFO and add it to the specified DCC **/
			void webBuildFIFO(xgi::Input *in, xgi::Output *out);
			
			/** Upload configuration to DB **/
			void webUploadToDB(xgi::Input *in, xgi::Output *out);
			
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
