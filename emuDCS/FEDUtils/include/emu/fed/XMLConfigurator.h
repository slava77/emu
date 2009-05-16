/*****************************************************************************\
* $Id: XMLConfigurator.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_XMLCONFIGURATOR_H__
#define __EMU_FED_XMLCONFIGURATOR_H__

#include "emu/fed/Configurator.h"

#include <string>

namespace emu {
	namespace fed {

		/** @class XMLConfigurator A utility class that produces a vector of Crates based on parsing an XML file. **/
		class XMLConfigurator: public Configurator
		{
		public:
			
			/** Constructor.
			*
			*	@param filename The XML file name which to parse.
			**/
			XMLConfigurator(std::string filename);

			/** Configure the crates and return them **/
			std::vector<emu::fed::Crate *> setupCrates()
			throw (emu::fed::exception::ConfigurationException);

		protected:

		private:
			
			/// The XML file name which to parse
			std::string filename_;
		
		};

	}
}

#endif

