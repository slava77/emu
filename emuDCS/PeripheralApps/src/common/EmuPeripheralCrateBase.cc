// $Id: EmuPeripheralCrateBase.cc,v 1.1 2009/03/07 11:46:01 liu Exp $

#include "EmuPeripheralCrateBase.h"

namespace emu {
  namespace pc {

EmuPeripheralCrateBase::EmuPeripheralCrateBase(xdaq::ApplicationStub * s): emu::base::Supervised(s)
{	

}  

//
////////////////////////////////////////////////////////////////////
// sending soap commands
////////////////////////////////////////////////////////////////////
int EmuPeripheralCrateBase::PCsendCommand(std::string command, std::string klass, int instance)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  int num = 0;

  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    if(instance < 0)
       apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
    else
       apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
  }
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return num; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    try {
        reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
        num++;
    }
    catch (xcept::Exception &e) {
        std::cout << "PCsendCommand failed, command=" << command << ", klass=" << klass << ", instance=" << instance << std::endl;
    }
    //
  }
  return num;
}
//
xoap::MessageReference EmuPeripheralCrateBase::PCcreateCommandSOAP(std::string command) {
  //
  //This is copied from CSCSupervisor::createCommandSOAP
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
  envelope.getBody().addBodyElement(name);
  //
  return message;
}

 }  // namespace emu::pc
}  // namespace emu

