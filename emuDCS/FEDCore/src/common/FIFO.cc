/*****************************************************************************\
* $Id: FIFO.cc,v 1.3 2009/06/13 17:59:28 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFO.h"

#include <sstream>
#include <iomanip>



emu::fed::FIFO::FIFO(const unsigned int fifoNumber, const unsigned int rui, const bool used):
fifoNumber_(fifoNumber),
rui_(rui),
used_(used)
{

}
