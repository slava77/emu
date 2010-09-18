#include "emu/supervisor/RunInfo.h"
#include <iterator>
#include <sstream>
#include <fstream>
#include <iostream>
#include "pstream.h"

using namespace std;
using namespace redi;

emu::supervisor::RunInfo* emu::supervisor::RunInfo::instance_ = NULL; ///< initial pointer

const string emu::supervisor::RunInfo::runNumKeyword_   ("RUN_NUMBER"         );
const string emu::supervisor::RunInfo::seqNameKeyword_  ("SEQUENCE_NAME"      );
const string emu::supervisor::RunInfo::runSeqNumKeyword_("RUN_SEQUENCE_NUMBER");
const string emu::supervisor::RunInfo::bookingSuccessKeyword_("Created Run Sequence Number");
const string emu::supervisor::RunInfo::updateSuccessKeyword_ ("Updated parameter"          );

emu::supervisor::RunInfo::RunInfo( const string bookingCommand, 
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

emu::supervisor::RunInfo* 
emu::supervisor::RunInfo::Instance( const string bookingCommand, 
				    const string writingCommand, 
				    const string dbUserFile, 
				    const string dbAddress ){
  
  if ( instance_ == 0 )  // is it the first call?
    instance_ = new emu::supervisor::RunInfo( bookingCommand, writingCommand, dbUserFile, dbAddress ); // create sole instance
  
  return instance_; // address of sole instance
  
}

emu::supervisor::RunInfo::~RunInfo(){
  instance_ = 0; // make sure it can be instantiated next time
}

void 
emu::supervisor::RunInfo::findOutWhoIAm(){

    ipstream who("whoami");
    while (who >> user_);

}

void 
emu::supervisor::RunInfo::getDbUserData(){
    
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

bool 
emu::supervisor::RunInfo::bookRunNumber( const string sequence ){

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

    cout << command << endl;

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
    copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

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

bool 
emu::supervisor::RunInfo::writeRunInfo( const string name, const string value, const string nameSpace ){
  
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
