#ifndef CSCMessageLogger_h
#define CSCMessageLogger_h
/*
#ifndef LOG4CPLUS_LOGGER 

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#else
*/
#include <memory>
#include <iostream>
#include <string>
#include <sstream>

#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"

namespace edm  {

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;


static Logger logger = Logger::getInstance("CSCRawUnpacking");

class LogWarning
{
public:
  explicit LogWarning( std::string const & id )
    : ap( new std::ostringstream() )
  { (*ap) << "<" << id <<"> ";}

  ~LogWarning() { LOG4CPLUS_WARN(logger, ap->str());}

  template< class T >
    LogWarning &
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogWarning &
  operator<< ( std::ostream&(*f)(std::ostream&))
                                      { (*ap) << f; return *this; }
  LogWarning &
  operator<< ( std::ios_base&(*f)(std::ios_base&) )
                                      { (*ap) << f; return *this; }

private:
  std::auto_ptr<std::ostringstream> ap;

};  // LogWarning


class LogError
{
public:
  explicit LogError( std::string const & id )
    : ap( new std::ostringstream() )
  { (*ap) << "<" << id <<"> ";}

  ~LogError() { LOG4CPLUS_ERROR(logger, ap->str()); }

  template< class T >
    LogError &
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogError &
  operator<< ( std::ostream&(*f)(std::ostream&))
                                      { (*ap) << f; return *this; }
  LogError &
  operator<< ( std::ios_base&(*f)(std::ios_base&) )
                                      { (*ap) << f; return *this; }


private:
 std::auto_ptr<std::ostringstream> ap;

};  // LogError


class LogInfo
{
public:
  explicit LogInfo( std::string const & id )
    : ap( new std::ostringstream() )
  { (*ap) << "<" << id <<"> ";}
  ~LogInfo() { LOG4CPLUS_INFO(logger, ap->str());}

  template< class T >
    LogInfo &
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogInfo &
  operator<< ( std::ostream&(*f)(std::ostream&))
                                      { (*ap) << f; return *this; }
  LogInfo &
  operator<< ( std::ios_base&(*f)(std::ios_base&) )
                                      { (*ap) << f; return *this; }

private:
  std::auto_ptr<std::ostringstream> ap;

};  // LogInfo

class LogDebug
{
public:
  explicit LogDebug( std::string const & id )
    : ap( new std::ostringstream() )
  { (*ap) << "<" << id <<"> ";}

  ~LogDebug() {LOG4CPLUS_DEBUG(logger, ap->str())}

  template< class T >
    LogDebug &
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogDebug &
  operator<< ( std::ostream&(*f)(std::ostream&))
                                      {(*ap) << f; return *this; }
  LogDebug &
  operator<< ( std::ios_base&(*f)(std::ios_base&) )
                                      { (*ap) << f; return *this; }

private:
  std::auto_ptr<std::ostringstream> ap;

};  // LogDebug


class Suppress_LogDebug_
{
  // With any decent optimization, use of Suppress_LogDebug_ (...)
  // including streaming of items to it via operator<<
  // will produce absolutely no executable code.
public:
  template< class T >
    Suppress_LogDebug_ &
    operator<< (T const & t)  { return *this; }
};  // Suppress_LogDebug_

}  // namespace edm


// #endif

#endif

