/*****************************************************************************\
* $Id: FIFO.cc,v 1.2 2009/05/29 11:23:18 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFO.h"

#include <sstream>
#include <iomanip>



emu::fed::FIFO::FIFO(unsigned int rui, bool used):
rui_(rui),
used_(used)
{

}
