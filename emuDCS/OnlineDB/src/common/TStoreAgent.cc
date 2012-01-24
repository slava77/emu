// $Id: TStoreAgent.cc,v 1.2 2012/01/24 18:33:24 khotilov Exp $

#include "emu/db/TStoreAgent.h"
#include "emu/db/TStoreCommand.h"

#include "tstore/client/Client.h"
#include "tstore/client/AttachmentUtils.h"
#include "toolbox/TimeInterval.h"

#include <sstream>

namespace emu { namespace db {

TStoreAgent::TStoreAgent(xdaq::Application *application, const std::string &tstore_view_id, const int instance) :
    application_(application), instance_(instance), viewID_(tstore_view_id)
{
  //viewID_ = "urn:tstore-view-SQL:EMUFEDsystem";
  //viewID_ = "urn:tstore-view-SQL:EMUsystem";
}


TStoreAgent::~TStoreAgent()
{
  // let's just don't do it...
  //if (!connectionID_.empty())// disconnect();
}


//if you have already opened a connection and want to use the existing connection to query, set the connection ID instead of calling connect()
void TStoreAgent::setConnectionID(const std::string &connectionID)
{
  connectionID_ = connectionID;
  //should maybe renew the connection or otherwise check that it's valid?
}


std::string TStoreAgent::connect(const std::string &username, const std::string &password)
throw (emu::exception::DBException)
{
  return connect(username + "/" + password);
}


std::string TStoreAgent::connect(const std::string &credentials)
throw (emu::exception::DBException)
{
  //maybe if a connectionID has already been set using setConnectionID, this function should just renew it, or do nothing.

  TStoreCommand ts_command(application_, "connect");

  // Add view ID
  ts_command.addCommandParameter("id", viewID_);

  // This parameter is mandatory. "basic" is the only value allowed at the moment
  ts_command.addCommandParameter("authentication", "basic");

  //login credentials in format username/password
  ts_command.addCommandParameter("credentials", credentials);

  //connection will time out after 10 minutes
  toolbox::TimeInterval timeout(600, 0);
  ts_command.addCommandParameter("timeout", timeout.toString("xs:duration"));

  xoap::MessageReference response;
  try
  {
    response = ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    XCEPT_RETHROW(emu::exception::DBException, "Error connecting to TStore " + std::string(e.what()), e);
  }

  //use the TStore client library to extract the response from the reply
  try
  {
    connectionID_ = tstoreclient::connectionID(response);
  }
  catch (xcept::Exception &e)
  {
    std::string error;
    response->writeTo(error);
    XCEPT_RETHROW(emu::exception::DBException, "Unable to parse connection ID: " + error +" : " +  std::string(e.what()), e);
  }

  return connectionID_;
}


void TStoreAgent::disconnect()
throw (emu::exception::DBException)
{
  TStoreCommand ts_command(application_, "disconnect");

  //add the connection ID
  ts_command.addCommandParameter("connectionID", connectionID_);

  try
  {
    ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    std::cout<<e.what()<<std::endl;
    XCEPT_RETHROW(emu::exception::DBException, "Error disconnecting TStore: " + std::string(e.what()), e);
  }
  connectionID_ = "";
}


xdata::Table TStoreAgent::query(const std::string &queryViewName, const std::map<std::string, std::string> &queryParameters)
throw (emu::exception::DBException)
{
  std::cout<<"q: "<<queryViewName<<"  v: "<<viewID_<<"   c: "<<connectionID_<<"  i: "<<instance_<<std::endl;

  TStoreCommand ts_command(application_, "query", viewID_);

  //add the connection ID
  ts_command.addCommandParameter("connectionID", connectionID_);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  ts_command.addViewSpecificParameter("name", queryViewName);

  // add parameter names and values
  for (std::map< std::string, std::string >::const_iterator iPair = queryParameters.begin(); iPair != queryParameters.end(); iPair++)
  {
    ts_command.addViewSpecificParameter(iPair->first, iPair->second);
  }

  xoap::MessageReference response;
  try
  {
    response = ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    XCEPT_RETHROW(emu::exception::DBException, "Error running TStore query: " + std::string(e.what()), e);
  }

  //use the TStore client library to extract the first attachment of type "table"
  //from the SOAP response
  xdata::Table results;
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE(emu::exception::DBException, "Server returned no data");
  }
  //results.writeTo(std::cout);std::cout<<std::endl;
  return results;
}


xdata::Table TStoreAgent::definition(const std::string &tableName) throw (emu::exception::DBException)
{
  //the definition message is essentially the same as a query message.
  //instead of retrieving a table full of results, we retrieve an empty table
  //with the appropriate column names and types.

  TStoreCommand ts_command(application_, "definition", viewID_);

  //add the connection ID
  ts_command.addCommandParameter("connectionID", connectionID_);

  //for an SQLView, the name parameter refers to the table name
  ts_command.addViewSpecificParameter("name", tableName);

  xoap::MessageReference response;
  try
  {
    response = ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    XCEPT_RETHROW(emu::exception::DBException, "Error requesting TStore table definition: " + std::string(e.what()), e);
  }

  //use the TStore client library to extract the first attachment of type "table" from the SOAP response
  xdata::Table result;
  if (!tstoreclient::getFirstAttachmentOfType(response, result))
  {
    XCEPT_RAISE (emu::exception::DBException, "Server returned no data");
  }
  return result;
}


void TStoreAgent::insert(const std::string &insertViewName, const xdata::Table &newRows)
throw (emu::exception::DBException)
{
  TStoreCommand ts_command(application_, "insert", viewID_);

  //add the connection ID
  ts_command.addCommandParameter("connectionID", connectionID_);

  //for an SQLView, the name parameter refers to the name of a insert section in the configuration
  ts_command.addViewSpecificParameter("name", insertViewName);

  //add our new rows as an attachment to the SOAP message
  xdata::Table myNewRows = newRows;
  ts_command.setAttachment(myNewRows);

  xoap::MessageReference response;
  try
  {
    response = ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    XCEPT_RETHROW(emu::exception::DBException, "Error while performing TStore \"insert\": " + std::string(e.what()), e);
  }
}


void TStoreAgent::synchronize(const std::string &syncMode, const std::string &syncPattern)
throw (emu::exception::DBException)
{
  TStoreCommand ts_command(application_, "sync", viewID_);

  ts_command.addCommandParameter("connectionID", connectionID_);
  ts_command.addCommandParameter("mode", syncMode);
  ts_command.addCommandParameter("pattern", syncPattern);

  xoap::MessageReference response;
  try
  {
    response = ts_command.run(instance_);
  }
  catch (emu::exception::SOAPException &e)
  {
    XCEPT_RETHROW(emu::exception::DBException, "Error running TStore sync : " + std::string(e.what()), e);
  }
  //    std::cout << "message: " << std::endl;
  //    message->writeTo(std::cout);
  //    std::cout << "response: " << std::endl;
  //    response->writeTo(std::cout);
}


}}
