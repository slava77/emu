#ifndef __emu_step_ChamberMap_h__
#define __emu_step_ChamberMap_h__

#include "xdata/String.h"
#include "xdata/Bag.h"

#include <string>

namespace emu{
  namespace step{

    struct ChamberMap{
      xdata::String chamberLabel_;
      xdata::String dmbSlot_;
      xdata::String crateId_;
      void registerFields( xdata::Bag<ChamberMap>* bag );
      std::string toString();
    };

  }
}

#endif
