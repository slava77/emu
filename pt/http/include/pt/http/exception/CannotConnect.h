// $Id: CannotConnect.h,v 1.1 2010/01/30 15:53:12 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_http_exception_CannotConnect_h_
#define _pt_http_exception_CannotConnect_h_

#include "pt/http/exception/Exception.h"

namespace pt  {
namespace http {
	namespace exception { 
		class CannotConnect: public pt::http::exception::Exception 
		{
			public: 
			CannotConnect ( std::string name, std::string message, std::string module, int line, std::string function ): 
					http::exception::Exception(name, message, module, line, function) 
			{} 
			
			CannotConnect ( std::string name, std::string message, std::string module, int line, std::string function,
				xcept::Exception& e ): 
					http::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}
}

#endif
