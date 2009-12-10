/*****************************************************************************\
* $Id: Configurator.h,v 1.4 2009/12/10 16:30:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CONFIGURATOR_H__
#define __EMU_FED_CONFIGURATOR_H__

#include <vector>
#include <string>
#include <time.h>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
	
		class Crate;

		/** @class Configurator A utility class that simply produces a vector of Crates based on a particular configuration method **/
		class Configurator
		{
		public:
		
			/** Default destructor **/
			virtual ~Configurator() {};

			/** Configure the crates and return them **/
			virtual std::vector<emu::fed::Crate *> setupCrates(const bool &fake = false)
			throw (emu::fed::exception::ConfigurationException) = 0;
			
			/** Access the already-configured crates **/
			inline std::vector<emu::fed::Crate *> getCrates() { return crateVector_; }
			
			/** Access the name of the system **/
			inline std::string getSystemName() { return systemName_; }
			
			/** Access the timestamp **/
			inline time_t getTimeStamp() { return timeStamp_; }

		protected:
			
			/// The vector of crates to configure and return
			std::vector<emu::fed::Crate *> crateVector_;
			
			/// The name of the system (for user convenience)
			std::string systemName_;
			
			/// The time stamp of the configuration
			time_t timeStamp_;

		private:
		
		};

	}
}

#endif

