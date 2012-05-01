///
/// @file   ToolBox.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 09:41:28 2010
/// 
/// @brief  Methods for manipulating SOAP messages.
/// 
/// 
///
#ifndef __emu_soap_ToolBox_h__
#define __emu_soap_ToolBox_h__

#include <string>
#include <vector>
#include <deque>

#include "xoap/DOMParser.h"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xoap/MessageReference.h"
#include "xdata/soap/Serializer.h"
#include "xoap/SOAPName.h"
#include "xoap/SOAPElement.h"
#include "xoap/SOAPFault.h"
#include "xdata/xdata.h"
#include "xdaq/NamespaceURI.h"
#include "xdata/soap/NamespaceURI.h"
#include "xoap/domutils.h"
#include "xgi/Input.h"
#include "xoap/Method.h"
#include "xcept/tools.h"

#include "emu/soap/Attachment.h"
#include "emu/soap/Parameters.h"

namespace emu{
  namespace soap{

      ///
      /// Creates a message with optional parameters, attributes and attachments.
      /// 
      /// @param command Qualified XML name of the command. If no namespace is specified, it can be given as a simple character string, and it will be automatically type converted to an \c emu::soap::QualifiedName object. Also, if no namespace is specified, the namespace will defualt to the standard XDAQ SOAP one.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// @return The message reference.
      ///
      /// See the overloaded version of this method for examples, and also emus::soap::Messenger::sendCommand (which takes the same arguments) for a complete illustration.
      ///
      xoap::MessageReference createMessage( const emu::soap::QualifiedName &command, 
					    const emu::soap::Parameters &parameters = emu::soap::Parameters::none,
					    const emu::soap::Attributes &attributes = emu::soap::Attributes::none,
					    const vector<emu::soap::Attachment> &attachments = emu::soap::Attachment::none );


      /// 
      /// Adds attachments to a SOAP message.
      /// 
      /// @param message SOAP message reference.
      /// @param attachments Attachments to add.
      ///
      void addAttachments( xoap::MessageReference message, const vector<emu::soap::Attachment> &attachments );

      /// 
      /// Adds attributes to an element in a SOAP message.
      /// 
      /// @param message SOAP message reference.
      /// @param element Element to add the attributes to.
      /// @param attributes Attributes to add.
      ///
      void addAttributes( xoap::MessageReference message, xoap::SOAPElement* element, const emu::soap::Attributes &attributes );

      /// 
      /// Include parameters as child elements in a parent element.
      /// 
      /// @param message SOAP message reference.
      /// @param parent Parent element to include the parameters in.
      /// @param parameters Parameters to include.
      ///
      void includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, emu::soap::Parameters &parameters );

      /// 
      /// Include parameters as child elements in a parent element.
      /// 
      /// @param message SOAP message reference.
      /// @param parent Parent element to include the parameters in.
      /// @param parameters Parameters to include.
      ///
      /// Example for including parameters in a message:
      /// \code
      ///      xoap::MessageReference reply = xoap::createMessage();
      ///      xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
      ///      xdata::UnsignedLong built_events = count;
      ///      emu::soap::includeParameters( reply, &body, emu::soap::Parameters().add( "built_events", &built_events ) );
      /// \endcode
      ///
      void includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, const emu::soap::Parameters &parameters );

      /// 
      /// Parses a SOAP reply to extract the specified parameters.
      /// 
      /// @param reply SOAP message reference.
      /// @param parameters Parameters to extract. If a parameter has no namespace URI defined, any namespace will match it.
      /// 
      /// Example for sending a command and extracting parameters from the reply:
      /// \code
      ///       xdata::String                start_time;
      ///       xdata::String                stop_time;
      ///       xdata::Vector<xdata::String> rui_counts;
      ///       m.extractParameters( m.sendCommand( "emu::daq::manager::Application", 0, "QueryRunSummary" ),
      ///                            emu::soap::Parameters().add( "start_time", &start_time ) 
      ///                                                   .add( "stop_time" , &stop_time  ) 
      ///                                                   .add( "rui_counts", &rui_counts ) );
      /// \endcode
      ///
      void extractParameters( xoap::MessageReference reply, emu::soap::Parameters &parameters );

      /// 
      /// Converts a SOAP fault reply to plain text.
      /// 
      /// @param fault SOAP fault.
      ///
      /// @return The fault in plain text format.
      ///
      /// Example:
      /// \code
      ///    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
      ///    if ( replyBody.hasFault() ){
      ///      xoap::SOAPFault fault = replyBody.getFault();
      ///      std::cout << emu::soap::faultToPlainText( &fault );
      ///    }
      /// \endcode
      ///
      std::string faultToPlainText( xoap::SOAPFault* fault );

      /// 
      /// Converts a SOAP fault reply to an \c xcept::Exception object that XDAQ applications are supposed to throw.
      /// 
      /// @param fault SOAP fault.
      ///
      /// @return An \c xcept::Exception object.
      ///
      /// Example:
      /// \code
      ///    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
      ///    if ( replyBody.hasFault() ){
      ///      xoap::SOAPFault fault = replyBody.getFault();
      ///      throw emu::soap::faultToException( &fault );
      ///    }
      /// \endcode
      ///
      xcept::Exception faultToException( xoap::SOAPFault* fault );

      /// 
      /// Creates a fault SOAP reply message.
      ///
      /// @param code This should be "Server" if the fault is due to the receiver, "Client" if it is due to the sender.
      /// @param reason Description of the fault.
      /// @param exception An optional \c xcept::Exception object to be serialized into the reply as details.
      ///
      /// @return The fault SOAP reply reference.
      ///
      xoap::MessageReference createFaultReply( const string& code, const string& reason, xcept::Exception* exception = NULL );

      /// 
      /// Sends a SOAP message to a target specified by its URL.
      /// 
      /// @param message SOAP message reference.
      /// @param URL Full URL of target.
      /// @param SOAPAction SOAP action or content location.
      ///
      /// @return SOAP reference to the reply received.
      ///
      xoap::MessageReference postSOAP( xoap::MessageReference message, const string& URL, const string& SOAPAction = std::string() );

      /// 
      /// Recursively converts an element of the SOAP fault and its descendents into plain text.
      /// 
      /// @param elem Element of the SOAP fault.
      /// @param indentDepth Size of indentation.
      ///
      /// @return Element of the SOAP fault in plain text format.
      ///
      std::string faultElementToPlainText( xoap::SOAPElement* elem, const int indentDepth );

      /// Writes the message into a string omitting the attachments.
      ///
      /// @param message Message to be serialized.
      ///
      /// @return The string containing the serialized message.
      ///
      std::string toStringWithoutAttachments( xoap::MessageReference message );

      /// 
      /// Workaround to repair the message serialized from xdata (see ticket https://svnweb.cern.ch/trac/cmsos/ticket/1008)
      /// \todo{ Remove once a fixed XDAQ version (>=v10.1.0) is deployed. } 
      ///
      /// @param message 
      ///
      void repairDOM( xoap::MessageReference message );
  }
}

#endif
