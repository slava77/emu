#ifndef __emu_soap_Messenger_h__
#define __emu_soap_Messenger_h__

#include <string>
#include <vector>
#include <deque>

#include "xdaq/Application.h"

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
///
/// @file   Messenger.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 09:41:28 2010
/// 
/// @brief  A SOAP messenger class implementing commonly used SOAP messages.
/// 
/// 
///
#include "xgi/Input.h"
#include "xoap/Method.h"
#include "xcept/tools.h"

#include "emu/soap/Attachment.h"
#include "emu/soap/Parameters.h"

namespace emu{
  namespace soap{

    class Messenger{
    public:

      /// 
      /// Ctor.
      /// 
      /// @param parent Pointer to the parent XDAQ application. 
      ///
      Messenger( xdaq::Application *parent );

      /// 
      /// Finds the application descriptor of an application given by its class name and instance.
      /// 
      /// @param className Application's class name.
      /// @param instance Application's instance (0 by default).
      ///
      /// @return Pointer to the application descriptor.
      ///
      xdaq::ApplicationDescriptor* getAppDescriptor( const std::string &className, const unsigned int instance=0 );

      /// 
      /// Gets the specified parameters from the target application given by its descriptor.
      /// 
      /// @param target The application descriptor of the target.
      /// @param parameters The parameters to get.
      ///
      /// Example:
      /// \code
      ///       emu::soap::Messenger m( this );
      ///	xdata::String s;
      ///	xdata::UnsignedLong ul;
      ///	xdata::Vector<xdata::String> Vs;
      ///	m.getParameters( targetDescriptor, 
      ///			 emu::soap::Parameters().add( "stateName", &s  )
      ///			                       .add( "runNumber", &ul )
      ///			                       .add( "runTypes" , &Vs ) );
      ///       cout << ul.toString() << endl << s.toString() << endl << Vs.toString() << endl;
      /// \endcode
      void getParameters( xdaq::ApplicationDescriptor *target, emu::soap::Parameters &parameters );

      ///
      /// Gets the specified parameters from the target application given by its class name and instance.
      /// 
      /// @param className Class name of the target application.
      /// @param instance Instance of the target application.
      /// @param parameters The parameters to get.
      ///
      /// Example:
      /// \code
      ///       emu::soap::Messenger m( this );
      ///	xdata::String s;
      ///	xdata::UnsignedLong ul;
      ///	xdata::Vector<xdata::String> Vs;
      ///	m.getParameters( "emu::daq::manager::Application", 0,
      ///			 emu::soap::Parameters().add( "stateName", &s  )
      ///			                       .add( "runNumber", &ul )
      ///			                       .add( "runTypes" , &Vs ) );
      ///       cout << ul.toString() << endl << s.toString() << endl << Vs.toString() << endl;
      /// \endcode
      void getParameters( const string &className, const unsigned int instance, emu::soap::Parameters &parameters );

      ///
      /// Sets the specified parameters in the target application given by its descriptor.
      /// 
      /// @param target The application descriptor of the target.
      /// @param parameters The parameters to set.
      ///
      /// Example:
      /// \code
      ///       emu::soap::Messenger m( this );
      ///	xdata::UnsignedLong ul = 1234;
      ///	xdata::Vector<xdata::String> Vs;
      ///       Vs.push_back( xdata::String("pace"  ) );
      ///	Vs.push_back( xdata::String("trot"  ) );
      ///	Vs.push_back( xdata::String("gallop") );
      ///	m.setParameters( targetDescriptor, 
      ///			 emu::soap::Parameters().add( "runNumber", &ul )
      ///			                       .add( "runTypes" , &Vs ) );
      /// \endcode
      void setParameters( xdaq::ApplicationDescriptor *target, const emu::soap::Parameters &parameters );

      /// 
      /// Sets the specified parameters in the target application given by its class name and instance.
      /// 
      /// @param className Class name of the target application.
      /// @param instance  Instance of the target application.
      /// @param parameters The parameters to set.
      ///
      /// Example:
      /// \code
      ///       emu::soap::Messenger m( this );
      ///	xdata::UnsignedLong ul = 1234;
      ///	xdata::Vector<xdata::String> Vs;
      ///       Vs.push_back( xdata::String("pace"  ) );
      ///	Vs.push_back( xdata::String("trot"  ) );
      ///	Vs.push_back( xdata::String("gallop") );
      ///	m.setParameters( "emu::daq::manager::Application", 0,
      ///			 emu::soap::Parameters().add( "runNumber", &ul )
      ///			                       .add( "runTypes" , &Vs ) );
      /// \endcode
      void setParameters( const string &className, const unsigned int instance, const emu::soap::Parameters &parameters );

      /// 
      /// Sets the specified parameters in all applications of \c className in the default zone.
      /// 
      /// @param className Class name of the target application(s).
      /// @param parameters The parameters to set.
      ///
      /// Example:
      /// \code
      ///       emu::soap::Messenger m( this );
      ///	xdata::UnsignedLong ul = 1234;
      ///	xdata::Vector<xdata::String> Vs;
      ///       Vs.push_back( xdata::String("pace"  ) );
      ///	Vs.push_back( xdata::String("trot"  ) );
      ///	Vs.push_back( xdata::String("gallop") );
      ///	m.setParameters( "emu::daq::manager::Application",
      ///			 emu::soap::Parameters().add( "runNumber", &ul )
      ///			                       .add( "runTypes" , &Vs ) );
      /// \endcode
      void setParameters( const string &className, const emu::soap::Parameters &parameters );

      ///
      /// Sends a command to the target application given by its descriptor.
      /// 
      /// @param target The application descriptor of the target.
      /// @param commandNamespaceURI The namespace URI for the command. If it's the standard XDAQ one, the overloaded method without this argument can be used, too.
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// @return The reply.
      ///
      /// Examples:
      /// \code
      ///       // Send a simple command
      ///       emu::soap::Messenger m( this );
      ///	m.sendCommand( targetDescriptor, "Configure" );
      ///
      ///       // Send another command with attributes
      ///       xdata::String s("Start");
      ///       m.sendCommand( targetDescriptor, "Cyclic",
      ///                      emu::soap::Messenger::noParameters, 
      ///                      emu::soap::Attributes().add( "Param", &s ) );
      ///
      ///       // Send yet another command with attachments 
      ///	char* data1 = "Text\0"; unsigned int dataLength1 = 5;
      ///	double d = 1.23456789;
      ///	char* data2 = (char*)( &d ); unsigned int dataLength2 = sizeof( double ) / sizeof( char );
      ///	std::vector<emu::soap::Attachment> attachments;
      ///	attachments.push_back( emu::soap::Attachment( dataLength1, data1 ).setContentType( "text/plain" ).setContentEncoding( "8bit" ) );
      ///	attachments.push_back( emu::soap::Attachment( dataLength2, data2 ).setContentType( "application/octet-stream" ).setContentEncoding( "binary" ) );
      /// 	m.sendCommand( targetDescriptor, "SeeAttachments", 
      ///		       emu::soap::Messenger::noParameters,
      ///		       emu::soap::Messenger::noAttributes,
      ///		       attachments );
      /// \endcode
      xoap::MessageReference sendCommand( xdaq::ApplicationDescriptor *target, 
					  const std::string &commandNamespaceURI,
					  const std::string &command, 
					  const emu::soap::Parameters &parameters = noParameters,
					  const emu::soap::Attributes &attributes = noAttributes,
					  const vector<emu::soap::Attachment> &attachments = noAttachments );

      ///
      /// Sends a command to the target application given by its class name and instance.
      /// 
      /// @param className Class name of the target application.
      /// @param instance Instance of the target application.
      /// @param commandNamespaceURI The namespace URI for the command. If it's the standard XDAQ one, the overloaded method without this argument can be used, too.
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// @return The reply.
      ///
      /// See the overloaded version of this method for examples.
      xoap::MessageReference sendCommand( const string &className, const unsigned int instance, 
					  const std::string &commandNamespaceURI,
					  const std::string &command, 
					  const emu::soap::Parameters &parameters = noParameters,
					  const emu::soap::Attributes &attributes = noAttributes,
					  const vector<emu::soap::Attachment> &attachments = noAttachments );

      ///
      /// Sends a command to all applications of \c className in the default zone.
      /// 
      /// @param className Class name of the target application(s).
      /// @param commandNamespaceURI The namespace URI for the command. If it's the standard XDAQ one, the overloaded method without this argument can be used, too.
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// See the overloaded version of this method for examples.
      void sendCommand( const string &className,
			const std::string &commandNamespaceURI,
			const std::string &command, 
			const emu::soap::Parameters &parameters = noParameters,
			const emu::soap::Attributes &attributes = noAttributes,
			const vector<emu::soap::Attachment> &attachments = noAttachments );





      ///
      /// Sends a command to the target application given by its descriptor.
      /// 
      /// @param target The application descriptor of the target.
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// @return The reply.
      ///
      /// Examples:
      /// \code
      ///       // Send a simple command
      ///       emu::soap::Messenger m( this );
      ///	m.sendCommand( targetDescriptor, "Configure" );
      ///
      ///       // Send another command with attributes
      ///       xdata::String s("Start");
      ///       m.sendCommand( targetDescriptor, "Cyclic",
      ///                      emu::soap::Messenger::noParameters, 
      ///                      emu::soap::Attributes().add( "Param", &s ) );
      ///
      ///       // Send yet another command with attachments 
      ///	char* data1 = "Text\0"; unsigned int dataLength1 = 5;
      ///	double d = 1.23456789;
      ///	char* data2 = (char*)( &d ); unsigned int dataLength2 = sizeof( double ) / sizeof( char );
      ///	std::vector<emu::soap::Attachment> attachments;
      ///	attachments.push_back( emu::soap::Attachment( dataLength1, data1 ).setContentType( "text/plain" ).setContentEncoding( "8bit" ) );
      ///	attachments.push_back( emu::soap::Attachment( dataLength2, data2 ).setContentType( "application/octet-stream" ).setContentEncoding( "binary" ) );
      /// 	m.sendCommand( targetDescriptor, "SeeAttachments", 
      ///		       emu::soap::Messenger::noParameters,
      ///		       emu::soap::Messenger::noAttributes,
      ///		       attachments );
      /// \endcode
      xoap::MessageReference sendCommand( xdaq::ApplicationDescriptor *target, 
					  const std::string &command, 
					  const emu::soap::Parameters &parameters = noParameters,
					  const emu::soap::Attributes &attributes = noAttributes,
					  const vector<emu::soap::Attachment> &attachments = noAttachments );

      ///
      /// Sends a command to the target application given by its class name and instance.
      /// 
      /// @param className Class name of the target application.
      /// @param instance Instance of the target application.
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// @return The reply.
      ///
      /// See the overloaded version of this method for examples.
      xoap::MessageReference sendCommand( const string &className, const unsigned int instance, 
					  const std::string &command, 
					  const emu::soap::Parameters &parameters = noParameters,
					  const emu::soap::Attributes &attributes = noAttributes,
					  const vector<emu::soap::Attachment> &attachments = noAttachments );

      ///
      /// Sends a command to all applications of \c className in the default zone.
      /// 
      /// @param className Class name of the target application(s).
      /// @param command Command.
      /// @param parameters Optional parameters.
      /// @param attributes Optional attributes of the command.
      /// @param attachments Optional attachments.
      ///
      /// See the overloaded version of this method for examples.
      void sendCommand( const string &className,
			const std::string &command, 
			const emu::soap::Parameters &parameters = noParameters,
			const emu::soap::Attributes &attributes = noAttributes,
			const vector<emu::soap::Attachment> &attachments = noAttachments );


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
      void includeParameters( xoap::MessageReference message, xoap::SOAPElement* parent, const emu::soap::Parameters &parameters );

      /// 
      /// Parses a SOAP reply to extract the specified parameters.
      /// 
      /// @param reply SOAP message reference.
      /// @param parameters Parameters to extract.
      /// @param parametersNamespaceURI Parameters' namespace URI (empty by default, in which case any namespace will match).
      /// 
      /// Example for sending a command and extracting parameters from the reply:
      /// \code
      ///       xdata::String                start_time;
      ///       xdata::String                stop_time;
      ///       xdata::Vector<xdata::String> rui_counts;
      ///       m.extractParameters( m.sendCommand( "emu::daq::manager::Application", 0, "QueryRunSummary" ),
      ///       		     emu::soap::Parameters().add( "start_time", &start_time ) 
      ///        		                            .add( "stop_time" , &stop_time  ) 
      ///       		                            .add( "rui_counts", &rui_counts ) );
      /// \endcode
      ///
      void extractParameters( xoap::MessageReference reply, emu::soap::Parameters &parameters, const string &parametersNamespaceURI="" );

      /// 
      /// Converts a SOAP fault reply to plain text.
      /// 
      /// @param fault SOAP fault.
      ///
      /// @return The fault in plain text format.
      ///
      std::string faultToPlainText( xoap::SOAPFault* fault );

      /// 
      /// Converts a SOAP fault reply to an \c xcept::Exception object that XDAQ applications are supposed to throw.
      /// 
      /// @param fault SOAP fault.
      ///
      /// @return An \c xcept::Exception object.
      ///
      xcept::Exception faultToException( xoap::SOAPFault* fault );      

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

    private:

      /// 
      /// Default ctor. Do not use.
      ///
      Messenger();

      /// 
      /// Recursively converts an element of the SOAP fault and its descendents into plain text.
      /// 
      /// @param elem Element of the SOAP fault.
      /// @param indentDepth Size of indentation.
      ///
      /// @return Element of the SOAP fault in plain text format.
      ///
      std::string faultElementToPlainText( xoap::SOAPElement* elem, const int indentDepth );

      /// 
      /// Recursively converts an element of the SOAP fault and its descendents into a stack of \c xcept::ExceptionInformation . 
      /// 
      /// @param elem Element of the SOAP fault.
      /// @param eStack A stack of \c xcept::ExceptionInformation .
      /// @param level Level of nestedness of \c elem .
      ///
      void faultElementToException( xoap::SOAPElement* elem, std::vector<xcept::ExceptionInformation> &eStack, int level );

    public:
      static const emu::soap::Attributes               noAttributes; ///< An empty container of attributes.
      static const emu::soap::Parameters               noParameters; ///< An empty container of parameters.
      static const std::vector<emu::soap::Attachment> noAttachments; ///< An empty container of attachments.
    private:
      xdaq::Application *application_; ///< Pointer to the parent XDAQ application.
    };

  }
}

#endif
