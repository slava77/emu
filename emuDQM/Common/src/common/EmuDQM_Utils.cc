#include "EmuDQM_Utils.h"

namespace emu {
  namespace dqm {
    namespace utils {
     
      std::string now()
      {
	char buf[255];
	time_t now=time(NULL);
	const struct tm * timeptr = localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", timeptr);
	std::string time = std::string(buf);
	if (time.find("\n",0) != std::string::npos)
	  time = time.substr(0,time.find("\n",0));
	else {
	  if (time.length() == 0)
	    time = "---";
	}
	return time;

      };
		
    }
  }
}

