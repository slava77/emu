#ifndef __EmuELog_h__
#define __EmuELog_h__

#include "pstream.h"
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace redi;

/// Class for posting to electronic log book by invoking the @c curl command.

/// 
/// Logs in to elog, posts entry, and logs out. Uses @c curl.
/// Gets username:password of elog user and CMS user from files.
class EmuELog{

private:

  string curlHost_;		///< host on which to execute curl command
  string curlCommand_;		///< curl command to be executed by the shell
  string curlCookies_;		///< name of cookies file created by curl

  string CMSUserFile_;		///< name of file containing user's id and password
  string eLogUserFile_;		///< name of file containing e-log user's id and password

  string eLogURL_;		///< URL of electronic log book

  string eLogUser_;		///< e-log user's id
  string eLogPassword_;		///< e-log user's password
  string eLogAuthor_;		///< e-log entry's author
  string CMSUser_;		///< user id
  string CMSPassword_;		///< user password

  string errorMessage_;		///< error message

public:

  /// constructor

  /// @param curlCommand curl command to be executed by the shell
  /// @param curlCookies name of cookies file created by curl
  /// @param CMSUserFile name of file containing user's id and password
  /// @param eLogUserFile name of file containing e-log user's id and password
  /// @param eLogURL URL of electronic log book
  ///
  EmuELog( string curlHost,
	   string curlCommand, 
	   string curlCookies, 
	   string CMSUserFile, 
	   string eLogUserFile, 
	   string eLogURL       ):
    curlHost_    (curlHost    ),
    curlCommand_ (curlCommand ),
    curlCookies_ (curlCookies ),
    CMSUserFile_ (CMSUserFile ),
    eLogUserFile_(eLogUserFile),
    eLogURL_     (eLogURL     )
  {
    getCMSUserData();
    getELogUserData();
  }

  /// destructor
  ~EmuELog(){}

  /// Gets user id and password from \ref CMSUserFile_ .
  void getCMSUserData(){
    
    fstream fs;
    fs.open( CMSUserFile_.c_str(), ios::in );
    if ( fs.is_open() ){
      string userAndPasswd;
      fs >> userAndPasswd;
      string::size_type loc = userAndPasswd.find(":");
      if ( loc == string::npos ){
	stringstream oss;
	oss << CMSUserFile_ << " contains no username:password. No elog entry will be posted.";
	throw oss.str();
      }
      CMSUser_     = userAndPasswd.substr( 0, loc );
      CMSPassword_ = userAndPasswd.substr( loc+1 );
      fs.close();
    }
    else{
      stringstream oss;
      oss << "Could not open " << CMSUserFile_ << " for reading. No elog entry will be posted.";
      throw oss.str();
    }

  }

  /// Gets e-log user id, password and author of entry from \ref eLogUserFile_ .
  void getELogUserData(){
    
    fstream fs;
    fs.open( eLogUserFile_.c_str(), ios::in );
    if ( fs.is_open() ){
      const int bufSize = 100;
      char buf[bufSize];
      fs.getline( buf, bufSize );
      string userData( buf );
      string::size_type first  = userData.find_first_of(":");
      string::size_type second = userData.find_first_of(":",first+1);
      if ( first == string::npos || first == 0 || second == 0 || first == second || second == userData.size()-1 ){
	stringstream oss;
	oss << eLogUserFile_ << " contains no username:password:author. No elog entry will be posted.";
	throw oss.str();
      }
      eLogUser_     = userData.substr( 0       , first  );
      eLogPassword_ = userData.substr( first+1 , second-first-1 );
      eLogAuthor_   = userData.substr( second+1         );
      fs.close();
    }
    else{
      stringstream oss;
      oss << "Could not open " << eLogUserFile_ << " for reading. No elog entry will be posted.";
      throw oss.str();
    }

  }

  /// Invokes curl to post e-log entry.

  /// @param subject subject of entry
  /// @param body message body
  /// @param attachments names of files to be attached
  ///
  /// @return TRUE if successful
  ///
  bool postMessage( string subject, string body, vector<string> *attachments=0 ){

    // TODO: try to find out whether or not posting was successful

    errorMessage_ = "";
    bool success  = false;

    string command = "ssh -2 ";
    command += curlHost_;
    command += " '";

    //
    // Login:
    //
    command += curlCommand_;
    command += " -u ";
    command += CMSUser_;
    command += ":";
    command += CMSPassword_;
    command += " -F cmd=Login -F uname=";
    command += eLogUser_;
    command += " -F upassword=";
    command += eLogPassword_;
    command += " -c ";
    command += curlCookies_;
    command += " ";
    command += eLogURL_;
    command += " && ";
    //
    // Post:
    //
    command += curlCommand_;
    command += " -u ";
    command += CMSUser_;
    command += ":";
    command += CMSPassword_;
    command += " -F cmd=Submit -F Author=\"";
    command += eLogAuthor_;
    command += "\" -F Type=\"EMU Local DAQ runs\" -F encoding=\"HTML\" -F Subject=\"";
    command += subject;
    command += "\" -F Text=\"";
    command += body;
    command += "\"";
    if ( attachments )
      for ( vector<string>::iterator attm = attachments->begin(); attm != attachments->end(); ++attm )
	command += " -F attfile=@" + *attm;
    command += " -F suppress=1 -b ";
    command += curlCookies_;
    command += " ";
    command += eLogURL_;
    command += " ; ";
    //
    // Logout:
    //
    command += curlCommand_;
    command += " -u ";
    command += CMSUser_;
    command += ":";
    command += CMSPassword_;
    command += " -F cmd=Logout -F uname=";
    command += eLogUser_;
    command += " -b ";
    command += curlCookies_;
    command += " ";
    command += eLogURL_;

    command += "'";

//     cout << command << endl;

    vector<string> replyLines;
    string reply;
    replyLines.clear();
    ipstream logout( command.c_str() );
    while ( std::getline(logout, reply) ) {
      replyLines.push_back(reply);
    }
    copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    return success;

  }

  /// accessor of e-log user's id

  ///
  /// @return e-log user's id
  string eLogUser(){ return eLogUser_; }

  /// accessor of e-log user's password

  ///
  /// @return e-log user's password
  string eLogPassword(){ return eLogPassword_; }

  /// accessor of user's id

  ///
  /// @return user's id
  string CMSUser(){ return CMSUser_; }

  /// accessor of error message

  ///
  /// @return error message
  string errorMessage(){ return errorMessage_; }
};

#endif
