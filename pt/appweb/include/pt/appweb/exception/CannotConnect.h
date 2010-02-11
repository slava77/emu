/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_exception_CannotConnect_h_
#define _pt_appweb_exception_CannotConnect_h_

#include "appweb/exception/Exception.h"

namespace pt {
namespace appweb {
	namespace exception { 
		class CannotConnect: public appweb::exception::Exception 
		{
			public: 
			CannotConnect ( std::string name, std::string message, std::string module, int line, std::string function ): 
					appweb::exception::Exception(name, message, module, line, function) 
			{} 
			
			CannotConnect ( std::string name, std::string message, std::string module, int line, std::string function,
				xcept::Exception& e ): 
					appweb::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}
}

#endif

