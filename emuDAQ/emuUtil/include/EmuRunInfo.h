#ifndef __EmuRunInfo_h__
#define __EmuRunInfo_h__

#include "pstream.h"
#include <string>
#include <iterator>
// #include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace redi;

/// Books run number with, and writes run summary to, run info database.

/// EmuRunInfo is a destructible singleton.<br/>
/// Instantiate at configuration:<br/>
/// <tt>EmuRunInfo *ri = EmuRunInfo::\ref Instance ("dooobee","doobee","dooo");</tt><br/>
/// Delete it when the run is stopped, and, just in case, when it's configured:<br/>
/// <tt>delete ri;</tt>
class EmuRunInfo{

private:

  static EmuRunInfo* instance_;	///< the (one and only) instance of EmuRunInfo

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
  EmuRunInfo( const string bookingCommand, 
	      const string writingCommand, 
	      const string dbUserFile, 
	      const string dbAddress ):
    bookingCommand_  (bookingCommand),
    writingCommand_  (writingCommand),
    dbUserFile_      (dbUserFile    ),
    dbAddress_       (dbAddress     ),
    dbUser_          (""            ),
    dbPassword_      (""            ),
    runNumber_       (0             ),
    runNumberString_ (""            ),
    errorMessage_    (""            )
  {
    findOutWhoIAm();
    getDbUserData();
  }

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
  static EmuRunInfo* Instance( const string bookingCommand, 
			       const string writingCommand, 
			       const string dbUserFile, 
			       const string dbAddress ){
    
    if ( instance_ == 0 )  // is it the first call?
      instance_ = new EmuRunInfo( bookingCommand, writingCommand, dbUserFile, dbAddress ); // create sole instance

    return instance_; // address of sole instance

  }

  /// destructor
  ~EmuRunInfo(){
    instance_ = 0; // make sure it can be instantiated next time
  }

  /// Gets name of user running this application.
  void findOutWhoIAm(){

    ipstream who("whoami");
    while (who >> user_);

  }

  /// Gets database user's name and password from \ref dbUserFile_ .
  void getDbUserData(){
    
    fstream fs;
    fs.open( dbUserFile_.c_str(), ios::in );
    if ( fs.is_open() ){
      string userAndPasswd;
      fs >> userAndPasswd;
      string::size_type loc = userAndPasswd.find(":");
      if ( loc == string::npos ){
	stringstream oss;
	oss << dbUserFile_ << " contains no username:password. ==> No run number database access.";
	throw oss.str();
      }
      dbUser_     = userAndPasswd.substr( 0, loc );
      dbPassword_ = userAndPasswd.substr( loc+1 );
      fs.close();
    }
    else{
      stringstream oss;
      oss << "Could not open " << dbUserFile_ << " for reading. ==> No run number database access.";
      throw oss.str();
    }

  }

  /// Books run number with run info database.

  /// Invokes the external command specified by \ref bookingCommand_
  /// @param sequence run sequence name
  ///
  /// @return \c TRUE if successful
  ///
  bool bookRunNumber( const string sequence ){

    errorMessage_ = "";
    bool success  = false;

    if ( runNumberString_ != "" ) {
      stringstream ss;
      ss << "Already booked run number " << runNumberString_ 
	 << ". Cannot book another until this run is stopped." << endl;
      errorMessage_ = ss.str();
      return false;
    }

    string command = bookingCommand_;
    command += " ";
    command += dbAddress_;
    command += " ";
    command += dbUser_;
    command += " ";
    command += dbPassword_;
    command += " ";
    command += user_;
    command += " ";
    command += sequence;

//     cout << command << endl;

    string reply;
    ipstream book( command.c_str() );
//     while (book >> reply){
//       cout << reply;
//     }
//     cout << endl;

    vector<string> replyLines;
    while ( std::getline(book, reply) ) {
      replyLines.push_back(reply);
    }
//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    vector<string>::const_iterator line;
    for ( line = replyLines.begin(); line != replyLines.end(); ++line ){

      string::size_type keywordPos;

      keywordPos = (*line).find(runNumKeyword_,0);
      if ( keywordPos == 0 ){
	runNumberString_ = (*line).substr( keywordPos + runNumKeyword_.length() + 1 );
	stringstream ss( runNumberString_ );
	ss >> runNumber_;
      }

      keywordPos = (*line).find(seqNameKeyword_,0);
      if ( keywordPos == 0 )
	sequenceName_ = (*line).substr( keywordPos + seqNameKeyword_.length() + 1 );

      keywordPos = (*line).find(runSeqNumKeyword_,0);
      if ( keywordPos  == 0 ){
	runSequenceNumberString_ = (*line).substr( keywordPos + runSeqNumKeyword_.length() + 1 );
	stringstream ss( runSequenceNumberString_ );
	ss >> runSequenceNumber_;
      }

      keywordPos = (*line).find(bookingSuccessKeyword_,0);
      if ( keywordPos  == 0 ) success = true;

    }

    if ( !success )
      for ( line = replyLines.begin(); line != replyLines.end(); ++line ) errorMessage_ += *line;

    return success;

  }

  /// Writes a parameter to the run info database.

  /// Invokes the external command specified by \ref writingCommand_
  /// @param name parameter name
  /// @param value parameter value
  /// @param nameSpace parameter's name space
  ///
  /// @return \c TRUE if successful
  ///  
  bool writeRunInfo( const string name, const string value, const string nameSpace ){

    errorMessage_ = "";
    bool success = false;

    string command = writingCommand_;
    command += " ";
    command += dbAddress_;
    command += " ";
    command += dbUser_;
    command += " ";
    command += dbPassword_;
    command += " ";
    command += runNumberString_;
    command += " \"";
    command += name;
    command += "\" \"";
    command += value;
    command += "\" ";
    command += nameSpace;

//     cout << command << endl;

    string reply;
    ipstream write( command.c_str() );

    vector<string> replyLines;
    while ( std::getline(write, reply) ) {
      replyLines.push_back(reply);
    }

//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    vector<string>::const_iterator line;
    for ( line = replyLines.begin(); line != replyLines.end(); ++line )
      success |= ( (*line).find(updateSuccessKeyword_,0) == 0 );

    if ( !success )
      for ( line = replyLines.begin(); line != replyLines.end(); ++line ) 
	errorMessage_ += *line;

    return success;

  }

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

EmuRunInfo* EmuRunInfo::instance_ = 0; ///< initial pointer

const string EmuRunInfo::runNumKeyword_   ("RUN_NUMBER"         );
const string EmuRunInfo::seqNameKeyword_  ("SEQUENCE_NAME"      );
const string EmuRunInfo::runSeqNumKeyword_("RUN_SEQUENCE_NUMBER");
const string EmuRunInfo::bookingSuccessKeyword_("Created Run Sequence Number");
const string EmuRunInfo::updateSuccessKeyword_ ("Updated parameter"          );

#endif
