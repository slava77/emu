#include <iostream>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

using namespace XERCES_CPP_NAMESPACE;

// To compile: g++ -o test xmlExample.cpp -I$XDAQ_DIR/daq/extern/xerces/linuxx86/include/ -L$XDAQ_DIR/daq/extern/xerces/linuxx86/lib/ -lxerces-c
// To run: ./test ExportedBookingData.xml
int main(int argc, char *argv[]){
	XMLPlatformUtils::Initialize();
	XercesDOMParser *parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(false); // this is default
	parser->setCreateEntityReferenceNodes(true);  // this is default
	parser->setIncludeIgnorableWhitespace (false);

	parser->parse(argv[1]);
	DOMDocument *doc = parser->getDocument();
	DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Booking") );
	if( l->getLength() != 1 ){
		std::cout<<"There is not exactly one Booking node in configuration"<<std::endl;
		return 1;
	}
	DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("Histogram") );
	if( itemList->getLength() == 0 ){
		std::cout<<"There no histograms to book"<<std::endl;
		return 1;
	}
	for(unsigned int i=0; i<itemList->getLength(); i++){
		DOMNode *item = itemList->item(i);
		//DOMNamedNodeMap *attributes = item->getAttributes();
		//const XMLCh* valPtr = attributes->getNamedItem(XMLString::transcode("HistAttribute"))->getNodeValue();
		//char* xmlChar = XMLString::transcode(valPtr);
		//key = string( xmlChar );
		//XMLString::release( &xmlChar );
		DOMNodeList *children = item->getChildNodes();
		for(unsigned int i=0; i<children->getLength(); i++){
			XMLCh *compXmlCh = XMLString::transcode("Name");
			if(XMLString::equals(children->item(i)->getNodeName(),compXmlCh)){
				if ( children->item(i)->hasChildNodes() ) {
					const XMLCh *xmlChar =  children->item(i)->getFirstChild()->getNodeValue();
					char *textChar = XMLString::transcode(xmlChar);
					std::cout<<"Found histogram: "<<textChar<<std::endl;
					XMLString::release(&textChar);
				}
				XMLString::release(&compXmlCh);
				break;
			}
			XMLString::release(&compXmlCh);
		}
	}
	delete parser;
}
