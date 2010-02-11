/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2007, CERN.                                        *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber, L. Orsini, R. Moser                             *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef _pt_appweb_AliasBag_h_
#define _pt_appweb_AliasBag_h_

#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/Vector.h"

namespace pt
{

namespace appweb
{

//! this is a Bag for one Alias
//
class Alias
{
	public:
        
	void registerFields(xdata::Bag<Alias>* bag)
	{
		bag->addField ("name", &name);
		bag->addField ("path", &path);
	}

	xdata::String name;
	xdata::String path;
};

}

}

#endif

