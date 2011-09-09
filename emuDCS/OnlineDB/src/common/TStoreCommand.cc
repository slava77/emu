// $Id: TStoreCommand.cc,v 1.1 2011/09/09 16:04:44 khotilov Exp $

#include "emu/db/TStoreCommand.h"
#include "emu/soap/Messenger.h"
#include "emu/soap/ToolBox.h"

#include "tstore/client/Client.h"
#include "xdata/exdr/Serializer.h"


namespace emu { namespace db {


TStoreCommand::TStoreCommand(xdaq::Application *application, const std::string &command_name, const std::string &view_id):
qualifiedCommandName_(command_name, "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd", "tstoresoap"),
application_(application),
attachment_(),
exdrBuffer_()
{
  // all what it does, e.g., for the view ID "urn:tstore-view-SQL:EMUsystem" it would return "urn:tstore-view-SQL"
  viewClass_ = tstoreclient::classNameForView(view_id);
}


void TStoreCommand::addCommandParameter(const std::string &parameter_name, const std::string &parameter_value)
{
  commandParameters_[parameter_name] = xdata::String(parameter_value);
}


void TStoreCommand::addViewSpecificParameter(const std::string &parameter_name, const std::string &parameter_value)
throw (emu::exception::SOAPException)
{
  if (viewClass_.empty())
  {
    XCEPT_RAISE(xcept::Exception,"Could not add view specific parameter to message. No view ID was specified.");
  }
  viewSpecificParameters_[parameter_name] = parameter_value;
}


void TStoreCommand::setAttachment(xdata::Table &data)
throw (emu::exception::SOAPException)
{
  // maximum one attachment is allowed
  if (attachment_.size() >= 1)
  {
    XCEPT_RAISE(emu::exception::SOAPException, "Cannot add more then one attachment!");
  }

  // encode data into an exdr buffer, similar as in tstoreclient::addAttachment()
  try
  {
    xdata::exdr::Serializer serializer;
    serializer.exportAll(&data, &exdrBuffer_);
  }
  catch (xdata::exception::Exception &e)
  {
    XCEPT_RETHROW(emu::exception::SOAPException, "Could not serialize attachment data " + std::string(e.what()),e);
  }

  emu::soap::Attachment a( exdrBuffer_.tellp(), exdrBuffer_.getBuffer());
  a.setContentType("application/xdata+table");
  a.setContentEncoding("exdr");
  // it does not matter what the content id for tstore insert, as here we allow only one attachment per message
  a.setContentId("whatever");
  attachment_.push_back(a);
}


xoap::MessageReference TStoreCommand::run(const int instance)
throw (emu::exception::SOAPException)
{
  // fill the Attributes container with parameters first
  emu::soap::Attributes attributes;

  typedef std::map<const std::string, xdata::String> ParametersMap;

  for (ParametersMap::iterator p = viewSpecificParameters_.begin(); p != viewSpecificParameters_.end(); p++)
  {
    attributes.add( emu::soap::QualifiedName( (*p).first, viewClass_, "viewspecific"), &((*p).second) );
  }
  for (ParametersMap::iterator p = commandParameters_.begin(); p != commandParameters_.end(); p++)
  {
    attributes.add( (*p).first, &((*p).second) );
  }

  // send the command
  try
  {
    //xoap::MessageReference message = emu::soap::createMessage( qualifiedCommandName_,  emu::soap::Parameters::none,  attributes, attachments_ );
    //std::cout<<"soap:   "<<std::endl;message->writeTo(std::cout); std::cout<<std::endl;

    xoap::MessageReference response = emu::soap::Messenger(application_).sendCommand(
        "tstore::TStore", instance, qualifiedCommandName_,  emu::soap::Parameters::none,  attributes, attachment_);
    return response;
  }
  catch (xcept::Exception &e) // if response has fault, it would be caught as well
  {
    XCEPT_RETHROW(emu::exception::SOAPException, "TStoreCommand: error sending command : " + std::string(e.what()), e);
  }
}

}}
