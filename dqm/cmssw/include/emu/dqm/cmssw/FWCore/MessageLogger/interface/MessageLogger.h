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
#include "coloredlayout.h"

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

static Logger logroot = Logger::getRoot();
static Logger logger  = Logger::getInstance("logger");
static SharedAppenderPtr appender(new ConsoleAppender());

namespace edm  {

  class LocalLogger {

    public:

      explicit LocalLogger() : ap( new std::ostringstream() ) { }
      explicit LocalLogger(std::string const &id) : ap( new std::ostringstream() ) {
        (*ap) << "<" << id << ">: ";
      }
      virtual ~LocalLogger() { }

      template< class T >
      LocalLogger& operator<< (T const & t) { (*ap) << t; return *this; }
      LocalLogger& operator<< ( std::ostream&(*f)(std::ostream&)) { (*ap) << f; return *this; }
      LocalLogger& operator<< ( std::ios_base&(*f)(std::ios_base&)) { (*ap) << f; return *this; }

      const std::string getStr() const { return ap->str(); }

    private:

      std::auto_ptr<std::ostringstream> ap;

  };

  class LogWarning : public LocalLogger {

    public:

      explicit LogWarning() : LocalLogger() { }
      explicit LogWarning(std::string const &id) : LocalLogger(id) { }

      ~LogWarning() { 
        LOG4CPLUS_WARN(logger, getStr());
      }

  };

  class LogError : public LocalLogger {

    public:

      explicit LogError() : LocalLogger() { }
      explicit LogError(std::string const &id) : LocalLogger(id) { }

      ~LogError() { 
        LOG4CPLUS_ERROR(logger, getStr()); 
      }

  };

  class LogInfo : public LocalLogger {

    public:

      explicit LogInfo() : LocalLogger() { }
      explicit LogInfo(std::string const &id) : LocalLogger(id) { }

      ~LogInfo() { 
        LOG4CPLUS_INFO(logger, getStr());
      }

  };

/*
class LogTrace
{
public:
  explicit LogTrace( std::string const & id )
    : ap( new std::ostringstream() )
  { (*ap) << "<" << id <<"> ";}
  ~LogTrace() { LOG4CPLUS_DEBUG(logger, ap->str());}

  template< class T >
    LogTrace &
    operator<< (T const & t)  { (*ap) << t; return *this; }
  LogTrace &
  operator<< ( std::ostream&(*f)(std::ostream&))
                                      { (*ap) << f; return *this; }
  LogTrace &
  operator<< ( std::ios_base&(*f)(std::ios_base&) )
                                      { (*ap) << f; return *this; }

private:
  std::auto_ptr<std::ostringstream> ap;

};  // LogTrace
*/


  class LogDebug : public LocalLogger {

    public:

      explicit LogDebug() : LocalLogger() { }
      explicit LogDebug(std::string const &id) : LocalLogger(id) { }

      ~LogDebug() {
        LOG4CPLUS_DEBUG(logger, getStr())
      }

  };


class Suppress_LogDebug_ {

  // With any decent optimization, use of Suppress_LogDebug_ (...)
  // including streaming of items to it via operator<<
  // will produce absolutely no executable code.

  public:

    template< class T >
    Suppress_LogDebug_ & operator<< (T const & t)  { return *this; }

};  // Suppress_LogDebug_

}  // namespace edm

class LogTrace : public edm::LocalLogger {

  public:
    
    explicit LogTrace() : LocalLogger() { }
    explicit LogTrace(std::string const &id) : LocalLogger(id) { }

    ~LogTrace() { 
      LOG4CPLUS_TRACE(logger, getStr());
    }

};

#endif

