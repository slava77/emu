// $Id: EmuPeripheralCrateBase.h,v 1.1 2009/03/07 11:46:01 liu Exp $

#ifndef _EmuPeripheralCrateBase_h_
#define _EmuPeripheralCrateBase_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "emu/base/Supervised.h"

namespace emu {
  namespace pc {
  
class EmuPeripheralCrateBase: public emu::base::Supervised
{
  
public:
  
  EmuPeripheralCrateBase(xdaq::ApplicationStub * s);
  //
  //
  xoap::MessageReference PCcreateCommandSOAP(std::string command);
  int PCsendCommand(std::string command, std::string klass, int instance = -1) throw (xoap::exception::Exception, xdaq::exception::Exception);
  //

private:

};

  } // namespace emu::pc
} // namespace emu
  
#endif
