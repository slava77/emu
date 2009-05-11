#include "emu/farmer/utils/FileSystem.h"

#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include "pstream.h"

using namespace std;

string
emu::farmer::utils::readFile( const string fileName )
  throw ( xcept::Exception ){
  stringstream ss;
  try{
    fstream inFile( fileName.c_str(), fstream::in );
    while ( inFile.good() ){
      char c = char( inFile.get() );
      if ( inFile.good() ) ss << c;
    }
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to read file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  //cout << "emu::farmer::utils::readFile" << endl << ss.str() << endl; 
  return ss.str();
}

void
emu::farmer::utils::writeFile( const string fileName, const string content )
  throw ( xcept::Exception ){
  try{
    fstream outFile( fileName.c_str(), fstream::out );
    outFile << content;
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to write file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
}

vector<string> emu::farmer::utils::execShellCommand( const string shellCommand )
  throw ( xcept::Exception ){
  vector<string> replyLines;
  try{
    redi::ipstream command( shellCommand.c_str() );
    string reply;
    while ( std::getline( command, reply ) ) {
      replyLines.push_back(reply);
    }
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to execute shell command \"" << shellCommand << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  return replyLines;
}
