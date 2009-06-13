/*****************************************************************************\
* $Id: TStoreRequest.h,v 1.2 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_TSTOREREQUEST_H__
#define __EMU_FED_TSTOREREQUEST_H__

#include "xoap/MessageReference.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPElement.h"

namespace emu {

	namespace fed {

		/** @class TStoreRequest is a utility class designed to make TStore SOAP requests.
		**/
		class TStoreRequest {

		public:
			/** Constructor.
			*	@param commandName the name of the command to send TStore.
			*	@param viewClass the SQL view to use to send the command.
			**/
			TStoreRequest(const std::string &commandName, const std::string &viewClass = "");

			/** Add a parameter to the request with a name and a value.
			*	@param parameterName the parameter name to add.
			*	@param parameterValue the value of the added parameter in string form.
			**/
			void addTStoreParameter(const std::string &parameterName, const std::string &parameterValue);
			
			/** Add a view-specific parameter to the request with a name and a value.
			*	@param parameterName the parameter name to add.
			*	@param parameterValue the value of the added parameter in string form.
			**/
			void addViewSpecificParameter(const std::string &parameterName, const std::string &parameterValue)
			throw (xcept::Exception);
			
			/** Return the request as a SOAP message
			**/
			xoap::MessageReference toSOAP();
			
		private:
			
			/// The SQL view to use
			std::string viewClass_;
			
			/// The name of the command
			std::string commandName_;
			
			/// The parameters, stored in a name:value map
			std::map<const std::string, std::string> generalParameters_;
			
			/// The view-specific parameters, stored in a name:value map
			std::map<const std::string, std::string> viewSpecificParameters_;
			
			/// Add the parameters to a SOAP message with a corresponding namespace
			void addParametersWithNamespace(xoap::SOAPElement &element, xoap::SOAPEnvelope &envelope, std::map<const std::string,std::string> &parameters, const std::string &namespaceURI, const std::string &namespacePrefix);
			
			/// Send the SOAP message
			//xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message);
		};

	} // namespace emu::pc

} // namespace emu

#endif
