// $Id: System.cc,v 1.1 2011/09/09 16:26:25 khotilov Exp $

#include "emu/utils/System.h"
#include "emu/utils/IO.h"
#include "emu/utils/String.h"

#include "toolbox/regex.h"

#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include "pstream.h"

//using namespace std;

std::string emu::utils::readFile( const std::string fileName )
throw ( xcept::Exception )
{
  std::stringstream ss;
  try
  {
    std::fstream inFile(fileName.c_str(), std::fstream::in);
    while (inFile.good())
    {
      char c = char(inFile.get());
      if (inFile.good()) ss << c;
    }
  }
  catch (std::exception& e)
  {
    std::stringstream ess;
    ess << "Failed to read file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str());
  }
  //cout << "emu::utils::readFile" << endl << ss.str() << endl;
  return ss.str();
}


void emu::utils::writeFile( const std::string fileName, const std::string content )
throw ( xcept::Exception )
{
  try
  {
    std::fstream outFile(fileName.c_str(), std::fstream::out);
    outFile << content;
  }
  catch (std::exception& e)
  {
    std::stringstream ess;
    ess << "Failed to write file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str());
  }
}


std::vector< std::string >
emu::utils::execShellCommand(const std::string shellCommand) throw (xcept::Exception)
{
  std::vector< std::string > replyLines;
  try
  {
    redi::ipstream command(shellCommand.c_str());
    std::string reply;
    while (std::getline(command, reply))
    {
      replyLines.push_back(reply);
    }
  }
  catch (std::exception& e)
  {
    std::stringstream ess;
    ess << "Failed to execute shell command \"" << shellCommand << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str());
  }
  return replyLines;
}


std::string emu::utils::getDateTime()
{
  using namespace std;

  time_t t;
  struct tm *tm;
  time(&t);
  tm = localtime(&t);

  std::stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << "T"
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec;

  return ss.str();
}


emu::utils::SCSI_t
emu::utils::getSCSI( const std::string vendor, const std::string model )
{
  using namespace std;

  // /proc/scsi/scsi has entries like this:
  // Host: scsi3 Channel: 00 Id: 00 Lun: 00
  //   Vendor: JORWAY   Model: 73A              Rev: 208 
  //   Type:   Unknown                          ANSI SCSI revision: 02
  emu::utils::SCSI_t scsi;
  string scsiInfo = readFile("/proc/scsi/scsi");
  //const string regex1( "^[[:blank:]]*Host:[[:blank:]]\+scsi\([[:digit:]]\{1,3\}\)[[:blank:]]\+Channel:[[:blank:]]\+\([[:digit:]][[:digit:]]\)[[:blank:]]\+Id:[[:blank:]]\+\([[:digit:]][[:digit:]]\)[[:blank:]]\+Lun:[[:blank:]]\+\([[:digit:]][[:digit:]]\)[[:blank:]]*$" );
  //const string regex2( "^[[:blank:]]*Vendor: \([[:print:]]\+\)Model: \([[:print:]]\+\)Rev: \([[:print:]]\+\)$" );
  // compiler was complaining about the above
  const string regex1( "^[[:blank:]]*Host:[[:blank:]]+scsi([[:digit:]]{1,3})[[:blank:]]+Channel:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]+Id:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]+Lun:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]*$" );
  const string regex2( "^[[:blank:]]*Vendor: ([[:print:]]+)Model: ([[:print:]]+)Rev: ([[:print:]]+)$" );

  vector<string> matches;
  vector<string> lines = utils::splitString(scsiInfo, "\n");

  try
  {
    for (vector<string>::iterator l = lines.begin(); l != lines.end(); ++l)
    {
      if (toolbox::regx_match(*l, regex1, matches))
      {
        scsi.host = utils::stringTo< int >(matches[1]);
        scsi.channel = utils::stringTo< int >(matches[2]);
        scsi.id = utils::stringTo< int >(matches[3]);
        scsi.lun = utils::stringTo< int >(matches[4]);
        //cout << matches.size() << " regex1 matches: " << endl << matches << endl;
      }
      if (toolbox::regx_match(*l, regex2, matches))
      {
        scsi.vendor = utils::shaveOffBlanks(matches[1]);
        scsi.model = utils::shaveOffBlanks(matches[2]);
        scsi.revision = utils::shaveOffBlanks(matches[3]);
        //cout << matches.size() << " regex2 matches: " << endl << matches << endl;
        // Require strict match for vendor. The model name should start with the given std::string.
        if (scsi.vendor.compare(vendor) == 0 && scsi.model.find(model) == 0) return scsi;
      }
    }
    XCEPT_RAISE( xcept::Exception, string( "No SCSI device of vendor \"" ) + vendor + "\" and model \"" + model + "\" found.");
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW( xcept::Exception, "Failed to get SCSI parameters.", e);
  }
  catch (std::exception &e)
  {
    stringstream ss;
    ss << "Failed to get SCSI parameters. " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch (...)
  {
    XCEPT_RAISE( xcept::Exception, "Unknown exception." );
  }

  return scsi;
}
