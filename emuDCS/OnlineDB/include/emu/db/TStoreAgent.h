#ifndef __EMU_PC_TSTOREAGENT_H__
#define __EMU_PC_TSTOREAGENT_H__

#include "xdaq/Application.h"
#include "xoap/MessageReference.h"
#include "xdata/Table.h"
#include "xdata/UnsignedInteger64.h"

#include "emu/exception/Exception.h"

namespace emu { namespace db {

/** @class TStoreAgent
 * A utility class that allows for easy basic communication with a database through TStore.
 */
class TStoreAgent
{
public:

  /** Default constructor. **/
  TStoreAgent(xdaq::Application *application, const std::string &tstore_view_id = "urn:tstore-view-SQL:EMUsystem", const int instance = 0);

  /** Default destructor. **/
  virtual ~TStoreAgent();

  /** Connect to the database and store the connectionID for later use.
   * @param username the username to use for connecting to the database.
   * @param password the password to use for connecting to the database.
   **/
  virtual std::string connect(const std::string &username, const std::string &password) throw (emu::exception::DBException);

  /** Connect to the database and store the connectionID for later use.
   * @param credentials   is a string of the "username/password" format for connecting to the database.
   **/
  virtual std::string connect(const std::string &credentials) throw (emu::exception::DBException);

  /** Disconnect from the database **/
  virtual void  disconnect() throw (emu::exception::DBException);

  /** Query the database and return a response.
   *	@param queryViewName The name of the query operation from the TStore configuration
   *	@param queryParameters A collection of string:string pairs of parameter names and values for the query
   *
   *	@returns a Table representing the data requested from the database.
   **/
  virtual xdata::Table query(const std::string &queryViewName, const std::map< std::string, std::string > &queryParameters)  throw (emu::exception::DBException);

  /** Query the database for a table definition
   *    @param tableName The name of the query operation from the TStore configuration
   *
   *    @returns an empty Table representing the requested data structure.
   **/
  virtual xdata::Table definition(const std::string &tableName) throw (emu::exception::DBException);

  /** Insert into the database new rows.
   *	@param insertViewName The name of the insert operation from the TStore configuration
   *	@param newRows A collection of new rows to insert in Table format (can insert multiple rows simultaneously)
   **/
  void insert(const std::string &insertViewName, const xdata::Table &newRows) throw (emu::exception::DBException);

  /** Synchronize with DB using TStore functionality
   *    @param syncMode  possibilities: "to database" OR "from database" OR "both ways"
   *    @param syncPattern e.g., "^EMU_.*$" to match all PC tables
   **/
  void synchronize(const std::string &syncMode, const std::string &syncPattern) throw (emu::exception::DBException);


  void setConnectionID(const std::string &connectionID);

  /** Required to make maps with xdata::UnsignedInteger64 keys.
   *
   *  @note Had to define special comparison operator for the const case, as it was not defined
   **/
  struct comp
  {
    bool operator()(const xdata::UnsignedInteger64 &lhs, const xdata::UnsignedInteger64 &rhs) const
    {
      return *(const_cast< xdata::UnsignedInteger64 * >(&lhs)) < *(const_cast< xdata::UnsignedInteger64 * >(&rhs));
    }
  };

protected:

  /// The application context from which to send the SOAP messages
  xdaq::Application *application_;

  /// The TStore instance number to use for this DBAgent. An instance < 0 means send to the first TStore instance found on the executive, whatever that might be.
  int instance_;

  /// The ID recieved from the database after connecting for future requests
  std::string connectionID_;

  /// The name of the table this agent accesses
  std::string table_;

  /// The ID recieved from the database after connecting for future requests
  std::string viewID_;

  /// flag for whether post-processing of tables retrieved by TStore should be performed
  bool fixTStore_;

private:

};

}} // namespaces

#endif
