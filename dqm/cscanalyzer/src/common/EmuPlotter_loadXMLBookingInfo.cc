#include "emu/dqm/cscanalyzer/EmuPlotter.h"

using namespace XERCES_CPP_NAMESPACE;

int EmuPlotter::loadXMLBookingInfo(std::string xmlFile)
{
  LOG4CPLUS_INFO(logger_, "Loading Booking Info from XML file: "  <<  xmlFile)

  if (xmlFile == "")
    {
      LOG4CPLUS_ERROR (logger_, "Histo Booking Invalid configuration file: " << xmlFile);
      return 1;
    }

  XMLPlatformUtils::Initialize();
  XercesDOMParser *parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);
  parser->setDoSchema(true);
  parser->setValidationSchemaFullChecking(false); // this is default
  parser->setCreateEntityReferenceNodes(true);  // this is default
  parser->setIncludeIgnorableWhitespace (false);

  parser->parse(xmlFile.c_str());
  DOMDocument *doc = parser->getDocument();
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Booking") );
  if ( l->getLength() != 1 )
    {
      LOG4CPLUS_ERROR (logger_, "There is not exactly one Booking node in configuration");
      return 1;
    }
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("Histogram") );
  if ( itemList->getLength() == 0 )
    {
      LOG4CPLUS_ERROR (logger_, "There no histograms to book");
      return 1;
    }
  EmuMonitoringObject * obj = NULL;
  appBSem_.take();
  clearMECollection(MEFactories);

  for (uint32_t i=0; i<itemList->getLength(); i++)
    {
      obj = new EmuMonitoringObject(itemList->item(i));

      std::string name = obj->getName();
      if (obj->getPrefix().find("DDU") != std::string::npos)
        {
          MEFactories["DDU"][name] = obj;
        }
      else if (obj->getPrefix().find("CSC") != std::string::npos)
        {
          MEFactories["CSC"][name]=obj;
        }
      else
        {
          MEFactories["EMU"][name]=obj;
        }
    }
  appBSem_.give();
  delete parser;
  return 0;
}


EmuMonitoringObject* EmuPlotter::createME(DOMNode* MEInfo)
{
  DOMNodeList *children = MEInfo->getChildNodes();
  for (uint32_t i=0; i<children->getLength(); i++)
    {
      XMLCh *compXmlCh = XMLString::transcode("Name");
      if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
        {
          if ( children->item(i)->hasChildNodes() )
            {
              const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
              char *textChar = XMLString::transcode(xmlChar);
              LOG4CPLUS_INFO (logger_,"Found histogram: "<<textChar);
              XMLString::release(&textChar);
            }
          XMLString::release(&compXmlCh);
          break;
        }
      XMLString::release(&compXmlCh);
    }
  return NULL;

}




