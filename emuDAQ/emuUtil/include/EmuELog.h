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

class EmuELog{

  // Logs in to elog, posts entry, and logs out. Uses curl.
  // Gets username:password of elog user and CMS user from files.

private:

  string  curlCommand_;
  string  curlCookies_;

  string  CMSUserFile_;
  string  eLogUserFile_;

  string  eLogURL_;

  string  eLogUser_;
  string  eLogPassword_;
  string  CMSUser_;
  string  CMSPassword_;

  string errorMessage_;

public:

  EmuELog( string curlCommand, 
	   string curlCookies, 
	   string CMSUserFile, 
	   string eLogUserFile, 
	   string eLogURL       ):
    curlCommand_ (curlCommand ),
    curlCookies_ (curlCookies ),
    CMSUserFile_ (CMSUserFile ),
    eLogUserFile_(eLogUserFile),
    eLogURL_     (eLogURL     )
  {
    getCMSUserData();
    getELogUserData();
  }

  ~EmuELog(){}

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

  void getELogUserData(){
    
    fstream fs;
    fs.open( eLogUserFile_.c_str(), ios::in );
    if ( fs.is_open() ){
      string userAndPasswd;
      fs >> userAndPasswd;
      string::size_type loc = userAndPasswd.find(":");
      if ( loc == string::npos ){
	stringstream oss;
	oss << eLogUserFile_ << " contains no username:password. No elog entry will be posted.";
	throw oss.str();
      }
      eLogUser_     = userAndPasswd.substr( 0, loc );
      eLogPassword_ = userAndPasswd.substr( loc+1 );
      fs.close();
    }
    else{
      stringstream oss;
      oss << "Could not open " << eLogUserFile_ << " for reading. No elog entry will be posted.";
      throw oss.str();
    }

  }

  bool postMessage( string subject, string body ){

    // TODO: try to find out whether or not posting was successful

    errorMessage_ = "";
    bool success  = false;

    //
    // Login:
    //
    string command = curlCommand_;
    command += " ";
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

//     cout << command << endl;

    vector<string> replyLines;
    string reply;
    ipstream login( command.c_str() );
//     while ( std::getline(login, reply) ) {
//       replyLines.push_back(reply);
//     }
//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );


    //
    // Post:
    //
    command = curlCommand_;
    command += " ";
    command += CMSUser_;
    command += ":";
    command += CMSPassword_;
    command += " -F cmd=Submit -F Author=\"emu\" -F Type=\"EMU Local DAQ runs\" -F Subject=\"";
    command += subject;
    command += "\" -F Text=\"";
    command += body;
    command += "\" -F suppress=1 -b ";
    command += curlCookies_;
    command += " ";
    command += eLogURL_;

//     cout << command << endl;

    replyLines.clear();
    ipstream post( command.c_str() );
//     while ( std::getline(post, reply) ) {
//       replyLines.push_back(reply);
//     }
//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );


    //
    // Logout:
    //
    command = curlCommand_;
    command += " ";
    command += CMSUser_;
    command += ":";
    command += CMSPassword_;
    command += " -F cmd=Logout -F uname=";
    command += eLogUser_;
    command += " -b ";
    command += curlCookies_;
    command += " ";
    command += eLogURL_;

//     cout << command << endl;

    replyLines.clear();
    ipstream logout( command.c_str() );
//     while ( std::getline(logout, reply) ) {
//       replyLines.push_back(reply);
//     }
//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    return success;

  }
  
  string errorMessage(){ return errorMessage_; }
};

#endif
