// $Id: Exception.h,v 1.2 2011/01/25 17:36:47 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_exception_Exception_h_
#define _pt_http_exception_Exception_h_

#include "pt/exception/Exception.h"

namespace pt {
namespace http {
	namespace exception { 
		class Exception: public pt::exception::Exception 
		{
			public: 
			Exception( std::string name, std::string message, std::string module, int line, std::string function ): 
					pt::exception::Exception(name, message, module, line, function) 
			{} 
			
			Exception( std::string name, std::string message, std::string module, int line, std::string function,
				xcept::Exception& e ): 
					pt::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}
}

#endif
