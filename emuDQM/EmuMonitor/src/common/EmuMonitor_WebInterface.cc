#include "EmuMonitor.h"

// == XGI Call back == //
void EmuMonitor::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  showStatus(in,out);
  // wsm_.displayPage(out);
}

void EmuMonitor::showControl(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  wsm_.displayPage(out);
}
// == WSM Dispatcher function == //
void EmuMonitor::dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
  std::string stateInput = (*stateInputElement).getValue();
  wsm_.fireEvent(stateInput,in,out);
  stateName_ = wsm_.getStateName(wsm_.getCurrentState());
}

void EmuMonitor::showStatus (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  /*
    if ( fsm_.getCurrentState() == 'E' ){
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    }
  */
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuMonitor",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  // display FSM
  std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
  std::set<std::string> allInputs = wsm_.getInputs();


  *out << cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;

  //  printParametersTable(out);

  //  *out << cgicc::hr() << std::endl;

  *out << "<table border cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<th>" << wsm_.getStateName(wsm_.getCurrentState()) << "</th>" << std::endl;
  *out << "</tr>" << std::endl;
  /*  *out << "<tr>" << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++)
    {
      *out << "<td>";
      *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;

      if ( possibleInputs.find(*i) != possibleInputs.end() )
	{
	  *out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
	}
      else
	{
	  *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
	}

      *out << cgicc::form();
      *out << "</td>" << std::endl;
    }

  *out << "</tr>" << std::endl;
  */
  *out << "</table>" << std::endl;
  //

  *out << cgicc::hr() << std::endl;

  printParametersTable(out);
  

  xgi::Utils::getPageFooter(*out);
}

void EmuMonitor::InvalidWebAction(xgi::Input * in ) throw (xgi::exception::Exception)
{
  LOG4CPLUS_WARN(getApplicationLogger(),"Invalid Web FSM transition. Ignored");
}


// == Print application parameters == //
void EmuMonitor::printParametersTable( xgi::Output * out ) throw (xgi::exception::Exception)
{

  // xdata::InfoSpace * params_list = getApplicationInfoSpace(); 
  // xdata::InfoSpace::iterator itr;
  std::map<std::string, xdata::Serializable*, std::less<std::string> > *params_list = getApplicationInfoSpace(); 
  std::map<std::string, xdata::Serializable*, std::less<std::string> >::iterator itr;
  *out 	<< "<table border>"
	<< "<tr>"
        << "<th colspan=3 bgcolor=#7F7FFF>" << "Parameters List" << "</th>"
	<< "</tr>"
	<< "<tr>"
  	<< "<th>" << "Name" << "</th>"
	<< "<th>" << "Type" << "</th>" 
	<< "<th>" << "Value" << "</th>"
 	<< "</tr>" << std::endl;
  for (itr=params_list->begin(); itr != params_list->end(); ++itr) 
    {
      if (itr->second->type() == "properties") continue;
      *out << "<tr>" << std::endl;
      *out << "<td>" << itr->first << "</td>" << std::endl;
      *out << "<td>" << itr->second->type() << "</td>" << std::endl;
      if (itr->second->type() == "vector") {
        *out << "<td>";
	// =VB= !!! possible XDAQ bug: returns wrong pointer to xdata::Vector (+4 bytes offset)
        /* 
	   for (int i=0; i < reinterpret_cast<xdata::Vector<xdata::Serializable>* >((int)(itr->second)-4)->elements(); i++) {
	   *out << reinterpret_cast<xdata::Vector<xdata::Serializable>*>((int)(itr->second)-4)->elementAt(i)->toString() << " ";
	   }
	*/
	*out << "</td>" << std::endl;
      } else {
	*out << "<td>" << itr->second->toString() << "</td>" << std::endl;
      }

      *out << "</tr>" << std::endl;
    }
  *out 	<< "</table>" << std::endl;
}
/*
  std::string EmuMonitor::ageOfPageClock(){
  std::stringstream ss;
  ss << "<script type=\"text/javascript\">"                        << endl;
  ss << "   ageOfPage=0"                                           << endl;
  ss << "   function countSeconds(){"                              << endl;
  ss << "      hours=Math.floor(ageOfPage/3600)"                   << endl;
  ss << "      minutes=Math.floor(ageOfPage/60)%60"                << endl;
  ss << "      age=\"\""                                           << endl;
  ss << "      if (hours) age+=hours+\" h \""                      << endl;
  ss << "      if (minutes) age+=minutes+\" m \""                  << endl;
  ss << "      age+=ageOfPage%60+\" s \""                          << endl;
  ss << "      document.getElementById('ageOfPage').innerHTML=age" << endl;
  ss << "      ageOfPage=ageOfPage+1"                              << endl;
  ss << "      setTimeout('countSeconds()',1000)"                  << endl;
  ss << "   }"                                                     << endl;
  ss << "</script>"                                                << endl;
  return ss.str();
  }

*/
// == Web Navigation Pages == //
void EmuMonitor::stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  /*
    if ( fsm_.getCurrentState() == 'E' ){
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    }
  */
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuMonitor",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  // display FSM
  std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
  std::set<std::string> allInputs = wsm_.getInputs();


  *out << cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;

  //  printParametersTable(out);

  //  *out << cgicc::hr() << std::endl;

  *out << "<table border cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<th>" << wsm_.getStateName(wsm_.getCurrentState()) << "</th>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++)
    {
      *out << "<td>";
      *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;

      if ( possibleInputs.find(*i) != possibleInputs.end() )
	{
	  *out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
	}
      else
	{
	  *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
	}

      *out << cgicc::form();
      *out << "</td>" << std::endl;
    }

  *out << "</tr>" << std::endl;
  *out << "</table>" << std::endl;
  //

  *out << cgicc::hr() << std::endl;

  printParametersTable(out);
  

  xgi::Utils::getPageFooter(*out);
}




// == Failure Pages == //
void EmuMonitor::failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception)
{
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

  xgi::Utils::getPageHeader
    (out,
     "EmuMonitor Failure",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/daq/xgi/images/Application.gif"
     );

  *out << cgicc::br() << e.what() << cgicc::br() << std::endl;
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  *out << cgicc::br() << "<a href=\"" << url << "\">" << "retry" << "</a>" << cgicc::br() << std::endl;

  xgi::Utils::getPageFooter(*out);
}
