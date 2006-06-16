#ifndef _LOG4CPLUS_COLORLAYOUT_HEADER_
#define _LOG4CPLUS_COLORLAYOUT_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/streams.h>
#include <log4cplus/tstring.h>

#include <log4cplus/helpers/logloguser.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/stringhelper.h>

#include <vector>

#define GRAY    "\e[30;40m"
#define RED     "\e[31;40m"
#define GREEN   "\e[32;40m"
#define YELLOW  "\e[33;40m"
#define BLUE    "\e[34;40m"
#define MAGENTA "\e[35;40m"
#define CYAN    "\e[36;40m"
#define WHITE   "\e[37;40m"
#define NORM    "\e[0m"

#define INFO    GREEN
#define WARN    YELLOW
#define DEBUG   WHITE
#define ERROR   RED
#define FATAL   RED

using namespace std;
// using namespace log4cplus;
// using namespace log4cplus::helpers;
// using namespace log4cplus::spi;

namespace log4cplus {

  /**
     * SimpleLayout consists of the LogLevel of the log statement,
     * followed by " - " and then the log message itself. For example,
     *
     * <pre>
     *         DEBUG - Hello world
     * </pre>
     *
     * <p>
     *
     * <p>{@link PatternLayout} offers a much more powerful alternative.
     */
    class LOG4CPLUS_EXPORT SimpleColoredLayout : public Layout {
    public:
	SimpleColoredLayout(bool use_gmtime = false)
		: dateFormat( LOG4CPLUS_TEXT("%m-%d-%y %H:%M:%S") ),
  		use_gmtime(use_gmtime) {SetDefaultColorMap();}
//        SimpleColoredLayout() {SetDefaultColorMap();}
        SimpleColoredLayout(const log4cplus::helpers::Properties& properties) 
		: Layout(properties), 
		dateFormat( LOG4CPLUS_TEXT("%m-%d-%y %H:%M:%S") ),
  		use_gmtime(false)
		{
			if(properties.exists( LOG4CPLUS_TEXT("DateFormat") )) {
        		dateFormat  = properties.getProperty( LOG4CPLUS_TEXT("DateFormat") );
    			}

    			tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("Use_gmtime") );
    			use_gmtime = (log4cplus::helpers::toLower(tmp) == LOG4CPLUS_TEXT("true"));
			SetDefaultColorMap();
		}

        virtual void formatAndAppend(log4cplus::tostream& output,
                                     const log4cplus::spi::InternalLoggingEvent& event)
	{
		output << event.getTimestamp().getFormattedTime(dateFormat, use_gmtime) << "| "
           	<< colormap[event.getLogLevel()] << llmCache.toString(event.getLogLevel())
           	<< LOG4CPLUS_TEXT(" - ")
           	<< event.getMessage() << NORM
           	<< LOG4CPLUS_TEXT("\n");
	}

    protected:
       log4cplus::tstring dateFormat;
       bool use_gmtime;

    private:
	void SetDefaultColorMap() 
	{
		colormap.clear();
		colormap[OFF_LOG_LEVEL]="";
		colormap[FATAL_LOG_LEVEL]=FATAL;
		colormap[ERROR_LOG_LEVEL]=ERROR;
		colormap[WARN_LOG_LEVEL]=WARN;
		colormap[INFO_LOG_LEVEL]=INFO;
		colormap[DEBUG_LOG_LEVEL]=DEBUG;
		colormap[TRACE_LOG_LEVEL]="";
		colormap[ALL_LOG_LEVEL]="";
		colormap[NOT_SET_LOG_LEVEL]="";
	};
      // Disallow copying of instances of this class
        SimpleColoredLayout(const SimpleColoredLayout&);
        SimpleColoredLayout& operator=(const SimpleColoredLayout&);
	map<LogLevel, string> colormap;
    };

}
#endif // _LOG4CPLUS_COLORLAYOUT_HEADER_
