#include "emu/dqm/cscanalyzer/EmuPlotter.h"

using namespace XERCES_CPP_NAMESPACE;

int EmuPlotter::loadXMLCanvasesInfo(std::string xmlFile)
{
  LOG4CPLUS_INFO(logger_, "Loading Booking Info from XML file: "  <<  xmlFile)

  if (xmlFile == "")
    {
      LOG4CPLUS_ERROR (logger_, "Canvases Booking Invalid configuration file: " << xmlFile);
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
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Canvases") );
  if ( l->getLength() != 1 )
    {
      LOG4CPLUS_ERROR (logger_, "There is not exactly one Canvases node in configuration");
      return 1;
    }
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("Canvas") );
  if ( itemList->getLength() == 0 )
    {
      LOG4CPLUS_ERROR (logger_, "There no canvases to book");
      return 1;
    }
  EmuMonitoringCanvas * obj = NULL;
  appBSem_.take();
  clearCanvasesCollection(MECanvasFactories);

  for (uint32_t i=0; i<itemList->getLength(); i++)
    {
      obj = new EmuMonitoringCanvas(itemList->item(i));

      std::string name = obj->getName();
      if (obj->getPrefix().find("DDU") != std::string::npos)
        {
          MECanvasFactories["DDU"][name] = obj;

        }
      else if (obj->getPrefix().find("CSC") != std::string::npos)
        {
          MECanvasFactories["CSC"][name] = obj;
        }
      else
        {
          obj->setSummaryMap(&summaryMap);
          obj->setChamberMap(&chamberMap);
          MECanvasFactories["EMU"][name] = obj;

        }
    }
  appBSem_.give();
  delete parser;
  return 0;
}


