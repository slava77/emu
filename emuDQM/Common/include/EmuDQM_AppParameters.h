#ifndef EmuDQM_AppParameters_h
#define EmuDQM_AppParameters_h

#include "xdaq.h"
#include "toolbox.h"
#include "xdata.h"

#include "xdaq.h"
#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"

#include "EmuDQM/exception/Exception.h"


namespace emu {
  namespace dqm {
    DOMNode *findNode
      (
       DOMNodeList *nodeList,
       const std::string nodeLocalName
       )
       throw (emu::dqm::exception::Exception);

    /**
     * Gets and returns the value of the specified parameter from the specified
     * application.
     */

    std::string getScalarParam
      (
       xdaq::ApplicationContext *appContext_,
       xdaq::ApplicationDescriptor* appSrcDescriptor,
       xdaq::ApplicationDescriptor* appDescriptor,
       const std::string                 paramName,
       const std::string                 paramType
       )
        throw (emu::dqm::exception::Exception);

    /**
     * Sets the specified parameter of the specified application to the
     * specified value.
     */
    void setScalarParam
      (
       xdaq::ApplicationContext *appContext_,      
       xdaq::ApplicationDescriptor* appSrcDescriptor, 
       xdaq::ApplicationDescriptor* appDescriptor,
       const std::string                 paramName,
       const std::string                 paramType,
       const std::string                 paramValue
       )
      throw (emu::dqm::exception::Exception);

    /**
     * Creates a ParameterGet SOAP message.
     */
    xoap::MessageReference createParameterGetSOAPMsg
      (
       const std::string appClass,
       const std::string paramName,
       const std::string paramType
       )
      throw (emu::dqm::exception::Exception);

    /**
     * Creates a ParameterSet SOAP message.
     */
    xoap::MessageReference createParameterSetSOAPMsg
      (
       const std::string appClass,
       const std::string paramName,
       const std::string paramType,
       const std::string paramValue
       )
      throw (emu::dqm::exception::Exception);

    /**
     * Returns the value of the specified parameter from the specified SOAP
     * message.
     */
    std::string extractScalarParameterValueFromSoapMsg
      (
       xoap::MessageReference msg,
       const std::string           paramName
       )
      throw (emu::dqm::exception::Exception);

   /**
     * Sends the specified FSM event as a SOAP message to the specified
     * application.  An exception is raised if the application does not reply
     * successfully with a SOAP response.
     */

   void sendFSMEventToApp
    (
        const std::string                 eventName,
	xdaq::ApplicationContext *appContext_,
        xdaq::ApplicationDescriptor* appSrcDescriptor,
        xdaq::ApplicationDescriptor* appDescriptor
    )
    throw (emu::dqm::exception::Exception);

      /**
     * Creates a simple SOAP message representing a command with no
     * parameters.
     */
    xoap::MessageReference createSimpleSOAPCmdMsg(const std::string cmdName)
    throw (emu::dqm::exception::Exception);


  }
}

#endif
