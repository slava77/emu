/*****************************************************************************\
* $Id: XMLConfigurator.h,v 1.3 2009/12/10 16:30:04 paste Exp $
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
			XMLConfigurator(const std::string &filename);
			
			virtual ~XMLConfigurator() {};

			/** Configure the crates and return them **/
			virtual std::vector<emu::fed::Crate *> setupCrates(const bool &fake = false)
			throw (emu::fed::exception::ConfigurationException);
			
			static std::string makeXML(const std::vector<emu::fed::Crate *> &crateVector, const std::string &systemName)
			throw (emu::fed::exception::ConfigurationException);

		protected:

		private:
			
			/// The XML file name which to parse
			std::string filename_;
		
		};

	}
}

#endif

