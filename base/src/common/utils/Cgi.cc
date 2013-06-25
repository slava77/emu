// $Id: Cgi.cc,v 1.3 2012/06/21 12:22:03 banicz Exp $

#include "emu/utils/Cgi.h"

#include "cgicc/HTMLClasses.h"
#include "toolbox/regex.h"
#include "xdaq/Application.h"

#include <exception>
#include <sstream>
#include <iostream>


std::multimap<std::string, std::string>
emu::utils::selectFromQueryString( std::vector<cgicc::FormEntry>& fev, const std::string namePattern )
{
  std::multimap< std::string, std::string > nameValue;
  try
  {
    std::vector< cgicc::FormEntry >::const_iterator fe;
    for (fe = fev.begin(); fe != fev.end(); ++fe)
    {
      bool isMatched = false;
      try
      {
        isMatched = toolbox::regx_match(fe->getName(), namePattern);
      }
      catch (xcept::Exception& e)
      {
        std::stringstream ss;
        ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": ";
        XCEPT_RETHROW( xcept::Exception, ss.str(), e);
      }
      catch (std::exception& e)
      {
        std::stringstream ss;
        ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": " << e.what();
        XCEPT_RAISE( xcept::Exception, ss.str());
      }
      catch (...)
      {
        std::stringstream ss;
        ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": Unexpected exception";
        XCEPT_RAISE( xcept::Exception, ss.str());
      }

      if (isMatched) nameValue.insert( std::pair< std::string, std::string >( fe->getName(), fe->getValue() ) );
    }
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( xcept::Exception, "Failed to select from HTTP query string: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to select from HTTP query string: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( xcept::Exception, "Failed to select from HTTP query string: Unexpected exception");
  }

  return nameValue;
}


void emu::utils::redirect(xgi::Input *in, xgi::Output *out)
{
  // Redirect for the action not to be repeated if the 'reload' button is hit.
  // cout << "PATH_TRANSLATED: " <<  in->getenv( "PATH_TRANSLATED" ) << endl
  //      << "PATH_INFO: " << in->getenv( "PATH_INFO" ) << endl;
  std::string url = in->getenv("PATH_TRANSLATED");
  std::size_t p = url.find("/" + in->getenv("PATH_INFO") + "?");
  if (p != std::string::npos)
  {
    cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
    header.getStatusCode(303);
    header.getReasonPhrase("See Other");
    // Use the following line to redirect to the default web page in the same directory.
    header.addHeader("Location", url.substr(0, p) + "/");
  }
}

void emu::utils::redirectToSelf(xgi::Input *in, xgi::Output *out)
{
  // Redirect for the action not to be repeated if the 'reload' button is hit.
  std::string url = in->getenv("PATH_TRANSLATED");
  std::size_t p = url.find("/" + in->getenv("PATH_INFO") + "?");
  if (p != std::string::npos)
  {
    cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
    header.getStatusCode(303);
    header.getReasonPhrase("See Other");
    // Use the following line to redirect to the same page stripped of the query string.
    header.addHeader( "Location", url.substr(0, p) + "/" + in->getenv("PATH_INFO") );
  }
}

void emu::utils::redirectTo(const std::string newURL, xgi::Output *out)
{
  cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location", newURL);
}


void emu::utils::headerXdaq(xgi::Output * out,
                            xdaq::Application * app,
                            const std::string &title,
                            const std::string &subtitle,
                            const std::string &header_html)
{
  using std::endl;

  xdaq::ApplicationDescriptor* d = app->getApplicationDescriptor();
  std::string url = d->getContextDescriptor()->getURL();
  std::string urn = d->getURN();

  std::string the_title = title;
  if (title.empty()) the_title = d->getClassName();

  std::string the_subtitle = subtitle;
  if (subtitle.empty() && !title.empty()) the_subtitle = d->getClassName();

  // use more advanced XHTML doctype
  *out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
  *out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;

  //out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
  //*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
  //cgicc::html().reset();
  //*out << cgicc::html().set("lang", "en").set("dir","ltr") << endl;

  cgicc::head().reset();
  *out << cgicc::head() << endl;
  *out << cgicc::title(the_title) << endl;
  *out << header_html << endl;
  *out << cgicc::head() << endl;

  *out << cgicc::body() << endl;

  *out << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"0\" style=\"width: 100%; font-family: arial;\">" << endl;
  *out << "<tbody>" << endl;

  // First row
  *out << "<tr>" << endl;
  *out << "<td valign=middle rowspan=3 style=\"height: 20px; width: 20px;\">";
  *out << "<a href=\"/\"><img border=\"0\" src=\"/xgi/images/XDAQLogo.png\" title=\"XDAQ\" alt=\"XDAQ\"></a></td>";

  // Print title
  *out << "<td style=\"text-align: center; vertical-align: bottom; color:#003366; font-size: 18pt; font-weight: bold;\">";
  *out << the_title;
  *out << "</td>" << endl;

  *out << "<td rowspan=3 style=\"text-align: right; vertical-align: top; white-space: nowrap;\" >" << endl;

  // application icon with link on the right side
  std::stringstream applicationlink;
  applicationlink << "<a href=\"" << url << "/" << urn << "/\">";
  applicationlink << "<img style=\"padding: 3px;\" src=\"/xgi/images/Application.gif\" alt=\"" << title << "\" width=\"32\" height=\"32\" border=\"\" ></a>";
  *out << endl;
  *out << applicationlink.str() << endl;
  *out << "<br>" << endl;
  *out << "<font size=2>" << url << "</font>" <<  cgicc::br() << endl;
  *out << "<font size=1>" << urn << "</font>" << endl;
  *out << "</td>";

  // Second row for subtitle
  *out << "<tr>" << endl;
  *out << "<td style=\"text-align: center; vertical-align: top; color:grey; font-size: 14pt;\">";
  *out << the_subtitle << "</td>" << endl;
  *out << "</tr>" << endl;

  // Third row empty as spacer
  *out << "<tr>" << endl;
  *out << "<td style=\"text-align: center; vertical-align: top; color:grey; font-size: 10pt;\">" << " " << "</td>" << endl;
  *out << "</tr>" << endl;

  *out << "</tr></tbody></table>" << endl;
  *out << "<hr style=\"width: 100%; height: 1px;\"><br>" << endl;
}


void emu::utils::footer(xgi::Output * out)
{
  *out << "</body></html>" << std::endl;
}


void emu::utils::saveAsFileDialog(xgi::Output * out, const std::string &contents, const std::string &file_name)
{
  cgicc::HTTPResponseHeader header("HTTP/1.1", 200, "OK");
  header.addHeader("Content-Type", "application/x-unknown");

  std::ostringstream attachment;
  attachment << "attachment; filename=" << toolbox::escape(file_name);
  header.addHeader("Content-Disposition", attachment.str());

  *out << contents;

  out->setHTTPResponseHeader(header);
}
