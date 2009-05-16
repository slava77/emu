/*****************************************************************************\
* $Id: AutoConfigurator.h,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_AUTOCONFIGURATOR_H__
#define __EMU_FED_AUTOCONFIGURATOR_H__

#include "emu/fed/Configurator.h"

namespace emu {
	namespace fed {

		/** @class AutoConfigurator A utility class that produces a vector of Crates based on auto-detection.  Not useful for actually configuring the crates, as it will simply accept whatever it reads from the boards as their proper configuration. **/
		class AutoConfigurator: public Configurator
		{
		public:
			
			/** Constructor. **/
			AutoConfigurator();

			/** Configure the crates and return them **/
			std::vector<emu::fed::Crate *> setupCrates()
			throw (emu::fed::exception::ConfigurationException);

		protected:

		private:
		
		};

	}
}

#endif

