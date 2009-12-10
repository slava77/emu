/*****************************************************************************\
* $Id: AutoConfigurator.h,v 1.2 2009/12/10 16:30:03 paste Exp $
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
			
			/** Destructor **/
			virtual ~AutoConfigurator() {};

			/** Configure the crates and return them **/
			virtual std::vector<emu::fed::Crate *> setupCrates(const bool &fake = false)
			throw (emu::fed::exception::ConfigurationException);

		protected:

		private:
		
		};

	}
}

#endif

