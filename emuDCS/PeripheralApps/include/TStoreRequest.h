#ifndef _TSTOREREQUEST_H
#define _TSTOREREQUEST_H

#include "xoap/MessageReference.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPElement.h"

namespace emu {
  namespace pc {

class TStoreRequest {
	public:
	TStoreRequest(const std::string &commandName,const std::string &viewClass="");
	void addTStoreParameter(const std::string &parameterName,const std::string &parameterValue) throw ();
	void addViewSpecificParameter(const std::string &parameterName,const std::string &parameterValue) throw (xcept::Exception);
	xoap::MessageReference toSOAP();
	
	private:
	std::string viewClass_;
	std::string commandName_;
	std::map<const std::string,std::string> generalParameters_;
	std::map<const std::string,std::string> viewSpecificParameters_;
	void addParametersWithNamespace(xoap::SOAPElement &element,
												xoap::SOAPEnvelope &envelope,
												std::map<const std::string,std::string> &parameters,
												const std::string &namespaceURI,
												const std::string &namespacePrefix);
	xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message);
};

  } // namespace emu::pc
  } // namespace emu

#endif
