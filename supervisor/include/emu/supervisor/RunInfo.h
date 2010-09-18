#ifndef __emu_supervisor_RunInfo_h__
#define __emu_supervisor_RunInfo_h__

#include <string>

using namespace std;

namespace emu {
  namespace supervisor {

/// Books run number with, and writes run summary to, run info database.

/// RunInfo is a destructible singleton.<br/>
/// Instantiate at configuration:<br/>
/// <tt>emu::supervisor::RunInfo *ri = emu::supervisor::RunInfo::\ref Instance ("dooobee","doobee","dooo");</tt><br/>
/// Delete it when the run is stopped, and, just in case, when it's configured:<br/>
/// <tt>delete ri;</tt>
class RunInfo{

private:

  static  emu::supervisor::RunInfo* instance_;	///< the (one and only) instance of RunInfo

  string  bookingCommand_;	///< name of (external) command to be invoked for booking
  string  writingCommand_;	///< name of (external) command to be invoked writing to database
  string  dbUserFile_;		///< file containing the name and password of the database user
  string  dbAddress_;		///< address of the database

  string  dbUser_;		///< name of the database user
  string  dbPassword_;		///< password of the database user
  string  user_;		///< user running this application

  unsigned int runNumber_;	///< booked run number
  unsigned int runSequenceNumber_; ///< booked run sequence number
  string runNumberString_;	///< run number in string format
  string sequenceName_;		///< run sequence name
  string runSequenceNumberString_; ///< run sequence number in string format

  string errorMessage_;		///< error message

  static const string runNumKeyword_; ///< keyword to look for in order to find the run number in the output of the booking command
  static const string seqNameKeyword_; ///< keyword to look for in order to find the sequence name in the output of the booking command
  static const string runSeqNumKeyword_; ///< keyword to look for in order to find sequence number in the output of the booking command
  static const string bookingSuccessKeyword_; ///< keyword in the output of the booking command indicating success
  static const string updateSuccessKeyword_; ///< keyword in the output of the writing command indicating success


protected:

  /// constructor

  /// The constructor is protected in order to be callable by \ref Instance only.
  /// @param bookingCommand name of (external) command to be invoked for booking
  /// @param writingCommand name of (external) command to be invoked writing to database
  /// @param dbUserFile file containing the name and password of the database user
  /// @param dbAddress address of the database
  ///
  RunInfo( const string bookingCommand, 
	   const string writingCommand, 
	   const string dbUserFile, 
	   const string dbAddress );

public:

  /// Instantiates this class.

  /// With the constructor being protected, this is the only way to instantiate this class.
  /// It makes sure there exists only one single instance of it.
  /// @param bookingCommand name of (external) command to be invoked for booking
  /// @param writingCommand name of (external) command to be invoked writing to database
  /// @param dbUserFile file containing the name and password of the database user
  /// @param dbAddress address of the database
  ///
  /// @return pointer to the sole instance
  ///
  static RunInfo* Instance( const string bookingCommand, 
			    const string writingCommand, 
			    const string dbUserFile, 
			    const string dbAddress );
  /// destructor
  ~RunInfo();

  /// Gets name of user running this application.
  void findOutWhoIAm();

  /// Gets database user's name and password from \ref dbUserFile_ .
  void getDbUserData();

  /// Books run number with run info database.

  /// Invokes the external command specified by \ref bookingCommand_
  /// @param sequence run sequence name
  ///
  /// @return \c TRUE if successful
  ///
  bool bookRunNumber( const string sequence );

  /// Writes a parameter to the run info database.

  /// Invokes the external command specified by \ref writingCommand_
  /// @param name parameter name
  /// @param value parameter value
  /// @param nameSpace parameter's name space
  ///
  /// @return \c TRUE if successful
  ///  
  bool writeRunInfo( const string name, const string value, const string nameSpace );

  /// accessor of booked run number
  unsigned int runNumber()        { return runNumber_; }
  /// accessor of booked run sequence number
  unsigned int runSequenceNumber(){ return runSequenceNumber_; }
  /// accessor of booked run number in string format
  string runNumberString()        { return runNumberString_; }
  /// accessor of booked run sequence number in string format
  string runSequenceNumberString(){ return runSequenceNumberString_; }
  /// accessor of run sequence name
  string sequenceName()           { return sequenceName_; }
  /// accessor of error message
  string errorMessage()           { return errorMessage_; }
};

}} // namespace emu::supervisor

#endif
