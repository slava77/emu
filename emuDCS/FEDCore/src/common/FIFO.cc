/*****************************************************************************\
* $Id: FIFO.cc,v 1.4 2009/07/01 14:17:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFO.h"

#include <sstream>
#include <iomanip>



emu::fed::FIFO::FIFO(const unsigned int &fifoNumber, const unsigned int &rui, const bool &used):
fifoNumber_(fifoNumber),
rui_(rui),
used_(used)
{
}
