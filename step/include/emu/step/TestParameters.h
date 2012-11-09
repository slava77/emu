#ifndef __emu_step_TestParameters_h__
#define __emu_step_TestParameters_h__

#include "toolbox/BSem.h"
#include "log4cplus/logger.h"

#include <stdint.h>

#include <vector>
#include <map>
#include <string>

using namespace std;
using namespace log4cplus;

namespace emu{
  namespace step{
    class TestParameters{
    public:
      TestParameters( const string& id, 
		      const string& testParametersXML,
		      Logger*       pLogger );
      uint64_t getNEvents() const { return nEvents_; }
      string getId() const { return id_; }

    protected:
      toolbox::BSem       bsem_;	///< Binary semaphore.
      string              id_;
      string              name_;
      map<string,int>     parameters_;
      uint64_t            nEvents_;
      Logger             *pLogger_;

    private:
      void extractParameters( const string& testParametersXML );
      void calculateNEvents();
    };
  }
}

#endif
