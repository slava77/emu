#include "emu/dqm/tfanalyzer/EmuTFxmlParsing.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace XERCES_CPP_NAMESPACE;

namespace EmuTFxmlParsing
{

std::map<std::string,CanvasAttributes> parseCanvasXML(const char *filename) throw (std::exception)
{
  std::map<std::string,CanvasAttributes> canvasList;

  try
    {
      XMLPlatformUtils::Initialize();
      XercesDOMParser *parser = new XercesDOMParser();
      parser->setValidationScheme(XercesDOMParser::Val_Always);
      parser->setDoNamespaces(true);
      parser->setDoSchema(true);
      parser->setValidationSchemaFullChecking(false); // this is default
      parser->setCreateEntityReferenceNodes(true);  // this is default
      parser->setIncludeIgnorableWhitespace (false);

      parser->parse(filename);
      DOMDocument *doc = parser->getDocument();
      DOMNodeList *l = doc->getElementsByTagName(XMLString::transcode("Binding"));
      if ( l->getLength() != 1 )
        {
          std::cerr<<"There is not exactly one Booking node in configuration"<<std::endl;
          throw std::exception();
        }
      DOMNodeList *itemList = doc->getElementsByTagName(XMLString::transcode("Canvas"));
      if ( itemList->getLength() == 0 )
        {
          std::cerr<<"There no Canvases to book"<<std::endl;
          throw std::exception();
        }

      for (unsigned int i=0; i<itemList->getLength(); i++)
        {
          CanvasAttributes canvas;

          DOMNode *item = itemList->item(i);
          DOMNamedNodeMap *attributes = item->getAttributes();

          DOMNode *subitem = attributes->getNamedItem(XMLString::transcode("Options"));
          if ( subitem )
            {
              const XMLCh* valPtr = subitem->getNodeValue();
              char* xmlChar = XMLString::transcode(valPtr);
              char *begin = xmlChar, *end = xmlChar;
              while ( (end=strchr(begin,':'))!=NULL )
                {
                  *end = '\0';
                  canvas.options.push_back(begin);
                  begin = end + 1;
                }
              if (strlen(begin)) canvas.options.push_back(begin);
              XMLString::release( &xmlChar );
            }

          DOMNodeList *children = item->getChildNodes();
          for (unsigned int i=0; i<children->getLength(); i++)
            {
              XMLCh *compXmlCh = 0;
              // Type tag
              compXmlCh = XMLString::transcode("Type");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      canvas.type = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Prefix tag
              compXmlCh = XMLString::transcode("Prefix");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      canvas.prefix = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Name tag
              compXmlCh = XMLString::transcode("Name");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      canvas.name = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Title tag
              compXmlCh = XMLString::transcode("Title");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      canvas.title = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Item tag
              compXmlCh = XMLString::transcode("Item");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      canvas.items[textChar];
                      // Options for this Item
                      DOMNode *item = children->item(i);
                      DOMNamedNodeMap *attributes = item->getAttributes();
                      DOMNode *subitem = attributes->getNamedItem(XMLString::transcode("Options"));
                      if ( subitem )
                        {
                          const XMLCh* valPtr = subitem->getNodeValue();
                          char *xmlChar = XMLString::transcode(valPtr);
                          char *begin = xmlChar, *end = xmlChar;
                          while ( (end=strchr(begin,':'))!=NULL )
                            {
                              *end = '\0';
                              canvas.items[textChar].push_back(begin);
                              begin = end + 1;
                            }
                          if (strlen(begin)) canvas.items[textChar].push_back(begin);
                          XMLString::release( &xmlChar );
                        }
                      // end of Options
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
            }
          if ( canvas.name.length() ) canvasList[canvas.name] = canvas;
        }
      delete parser;
    }
  catch (...)
    {
      std::cerr<<"Exception occured"<<std::endl;
      throw std::exception();
    }

  return canvasList;

}

std::map<std::string,HistAttributes> parseHistXML(const char *filename) throw (std::exception)
{
  std::map<std::string,HistAttributes> histList;

  try
    {
      XMLPlatformUtils::Initialize();
      XercesDOMParser *parser = new XercesDOMParser();
      parser->setValidationScheme(XercesDOMParser::Val_Always);
      parser->setDoNamespaces(true);
      parser->setDoSchema(true);
      parser->setValidationSchemaFullChecking(false); // this is default
      parser->setCreateEntityReferenceNodes(true);  // this is default
      parser->setIncludeIgnorableWhitespace (false);

      parser->parse(filename);
      DOMDocument *doc = parser->getDocument();
      DOMNodeList *l = doc->getElementsByTagName(XMLString::transcode("Booking"));
      if ( l->getLength() != 1 )
        {
          std::cerr<<"There is not exactly one Booking node in configuration"<<std::endl;
          throw std::exception();
        }
      DOMNodeList *itemList = doc->getElementsByTagName(XMLString::transcode("Histogram"));
      if ( itemList->getLength() == 0 )
        {
          std::cerr<<"There no histograms to book"<<std::endl;
          throw std::exception();
        }

      for (unsigned int i=0; i<itemList->getLength(); i++)
        {
          HistAttributes histogram;

          DOMNode *item = itemList->item(i);
          DOMNamedNodeMap *attributes = item->getAttributes();

          DOMNode *subitem = attributes->getNamedItem(XMLString::transcode("Options"));
          if ( subitem )
            {
              const XMLCh* valPtr = subitem->getNodeValue();
              char *xmlChar = XMLString::transcode(valPtr);
              char *begin = xmlChar, *end = xmlChar;
              while ( (end=strchr(begin,':'))!=NULL )
                {
                  *end = '\0';
                  histogram.options.push_back(begin);
                  begin = end + 1;
                }
              if (strlen(begin)) histogram.options.push_back(begin);
              XMLString::release( &xmlChar );
            }

          DOMNodeList *children = item->getChildNodes();
          for (unsigned int i=0; i<children->getLength(); i++)
            {
              XMLCh *compXmlCh = 0;
              // Type tag
              compXmlCh = XMLString::transcode("Type");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.type = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Prefix tag
              compXmlCh = XMLString::transcode("Prefix");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.prefix = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Name tag
              compXmlCh = XMLString::transcode("Name");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.name = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Title tag
              compXmlCh = XMLString::transcode("Title");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.title = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // XRange tag
              compXmlCh = XMLString::transcode("XRange");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      char *first  = strchr(textChar,'(') + 1;
                      char *second = strchr(textChar,':') + 1;
                      if ( first!=NULL && second!=NULL )
                        {
                          second[-1] = '\0';
                          histogram.xMin = atoi(first);
                          histogram.xMax = atoi(second);
                        }
                      else
                        std::cerr<<"Unknown format of XRange tag: "<<xmlChar<<std::endl;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // XBins tag
              compXmlCh = XMLString::transcode("XBins");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.xNbins = atoi(textChar);
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // XLabel tag
              compXmlCh = XMLString::transcode("XLabel");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.xLabel = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // YRange tag
              compXmlCh = XMLString::transcode("YRange");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      char *first  = strchr(textChar,'(') + 1;
                      char *second = strchr(textChar,':') + 1;
                      if ( first!=NULL && second!=NULL )
                        {
                          second[-1] = '\0';
                          histogram.yMin = atoi(first);
                          histogram.yMax = atoi(second);
                        }
                      else
                        std::cerr<<"Unknown format of YRange tag: "<<xmlChar<<std::endl;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // YBins tag
              compXmlCh = XMLString::transcode("YBins");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.yNbins = atoi(textChar);
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // YLabel tag
              compXmlCh = XMLString::transcode("YLabel");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      histogram.yLabel = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // XBinLabels tag
              compXmlCh = XMLString::transcode("XBinLabels");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      char *begin = textChar, *end = textChar;
                      while ( (end=strchr(begin,','))!=NULL )
                        {
                          *end = '\0';
                          histogram.xBinLabels.push_back(begin);
                          begin = end + 1;
                        }
                      if (strlen(begin)) histogram.xBinLabels.push_back(begin);
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // YBinLabels tag
              compXmlCh = XMLString::transcode("YBinLabels");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      char *begin = textChar, *end = textChar;
                      while ( (end=strchr(begin,','))!=NULL )
                        {
                          *end = '\0';
                          histogram.yBinLabels.push_back(begin);
                          begin = end + 1;
                        }
                      if (strlen(begin)) histogram.yBinLabels.push_back(begin);
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
            }
          if ( histogram.name.length() ) histList[histogram.name] = histogram;
        }
      delete parser;
    }
  catch (...)
    {
      std::cerr<<"Exception occured"<<std::endl;
      throw std::exception();
    }
  return histList;
}

std::map<std::string,CheckAttributes> parseCheckXML(const char *filename) throw (std::exception)
{
  std::map<std::string,CheckAttributes> checkList;

  try
    {
      XMLPlatformUtils::Initialize();
      XercesDOMParser *parser = new XercesDOMParser();
      parser->setValidationScheme(XercesDOMParser::Val_Always);
      parser->setDoNamespaces(true);
      parser->setDoSchema(true);
      parser->setValidationSchemaFullChecking(false); // this is default
      parser->setCreateEntityReferenceNodes(true);  // this is default
      parser->setIncludeIgnorableWhitespace (false);

      parser->parse(filename);
      DOMDocument *doc = parser->getDocument();
      DOMNodeList *l = doc->getElementsByTagName(XMLString::transcode("Checking"));
      if ( l->getLength() != 1 )
        {
          std::cerr<<"There is not exactly one Checking node in configuration"<<std::endl;
          throw std::exception();
        }
      DOMNodeList *itemList = doc->getElementsByTagName(XMLString::transcode("Check"));
      if ( itemList->getLength() == 0 )
        {
          std::cerr<<"There no checks to apply"<<std::endl;
          throw std::exception();
        }

      for (unsigned int i=0; i<itemList->getLength(); i++)
        {
          CheckAttributes check;

          DOMNode *item = itemList->item(i);
          DOMNamedNodeMap *attributes = item->getAttributes();

          DOMNode *subitem = attributes->getNamedItem(XMLString::transcode("Options"));
          if ( subitem )
            {
              const XMLCh* valPtr = subitem->getNodeValue();
              char* xmlChar = XMLString::transcode(valPtr);
              char *begin = xmlChar, *end = xmlChar;
              while ( (end=strchr(begin,':'))!=NULL )
                {
                  *end = '\0';
                  check.options.push_back(begin);
                  begin = end + 1;
                }
              if (strlen(begin)) check.options.push_back(begin);
              XMLString::release( &xmlChar );
            }

          DOMNodeList *children = item->getChildNodes();
          for (unsigned int i=0; i<children->getLength(); i++)
            {
              XMLCh *compXmlCh = 0;
              // Name tag
              compXmlCh = XMLString::transcode("Name");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      check.name = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Title tag
              compXmlCh = XMLString::transcode("Title");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      check.title = textChar;
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
              // Item tag
              compXmlCh = XMLString::transcode("Item");
              if (XMLString::equals(children->item(i)->getNodeName(),compXmlCh))
                {
                  if ( children->item(i)->hasChildNodes() )
                    {
                      const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
                      char *textChar = XMLString::transcode(xmlChar);
                      check.items[textChar];
                      // Options for this Item
                      DOMNode *item = children->item(i);
                      DOMNamedNodeMap *attributes = item->getAttributes();
                      DOMNode *subitem = attributes->getNamedItem(XMLString::transcode("Properties"));
                      if ( subitem )
                        {
                          const XMLCh* valPtr = subitem->getNodeValue();
                          char *xmlChar = XMLString::transcode(valPtr);
                          char *begin = xmlChar, *end = xmlChar;
                          while ( (end=strchr(begin,':'))!=NULL )
                            {
                              *end = '\0';
                              check.items[textChar].push_back(begin);
                              begin = end + 1;
                            }
                          if (strlen(begin)) check.items[textChar].push_back(begin);
                          XMLString::release( &xmlChar );
                        }
                      // end of Options
                      XMLString::release(&textChar);
                    }
                }
              XMLString::release(&compXmlCh);
            }
          if ( check.name.length() ) checkList[check.name] = check;
        }
      delete parser;
    }
  catch (...)
    {
      std::cerr<<"Exception occured"<<std::endl;
      throw std::exception();
    }

  return checkList;

}

} //end of namespace
