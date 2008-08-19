/*****************************************************************************\
* $Id: FEDCrateParser.cc,v 3.11 2008/08/19 14:51:02 paste Exp $
*
* $Log: FEDCrateParser.cc,v $
* Revision 3.11  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.10  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.9  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/

#include "FEDCrateParser.h"

#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "DDUParser.h"
#include "DCCParser.h"
#include "VMEControllerParser.h"
#include "FEDCrate.h"

void emu::fed::FEDCrateParser::parseFile(const char* name){ 
	//PGK: clear the crateVector
	//std::cout << " durkin: entered parser " << std::endl;
	crateVector_.clear();
	/// Initialize XML4C system
	try{
		xercesc::XMLPlatformUtils::Initialize();
	}
  
	catch(const xercesc::XMLException& toCatch){
		std::cerr << "Error during Xerces-c Initialization.\n"
			<< "  Exception message:"
			<< xercesc::XMLString::transcode(toCatch.getMessage()) << std::endl;
		return ;
	}
 
	//  Create our parser, then attach an error handler to the parser.
	//  The parser will call back to methods of the ErrorHandler if it
	//  discovers errors during the course of parsing the XML document.
	//
	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setCreateEntityReferenceNodes(false);
	//parser->setToCreateXMLDeclTypeNode(true);
  
 
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	bool errorsOccured = false;
	try{
		parser->parse(name);
	}
  
	catch (const xercesc::XMLException& e){
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.getMessage()) << std::endl;
		errorsOccured = true;
	}

 
	catch (const xercesc::DOMException& e){
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.msg) << std::endl;
		errorsOccured = true;
	}

	catch (...){
		std::cerr << "An error occured during parsing\n " << std::endl;
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured) {
		xercesc::DOMNode * pDoc = parser->getDocument();
		xercesc::DOMNode * pNode1 = pDoc->getFirstChild();
		while (pNode1) { // EmuSystem
			if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
				std::cout << "FEDCrateParser: pNode1=" 
					<< xercesc::XMLString::transcode(pNode1->getNodeName()) << std::endl;
				if (strcmp("EmuSystem",xercesc::XMLString::transcode(pNode1->getNodeName()))){
					std::cout << "FEDCrateParser: WARNING - Wrong Top Element <"
						<< xercesc::XMLString::transcode(pNode1->getNodeName())
						<< ">, should be <EmuSystem>" << std::endl;
				}
				/* PGK I added a new attribute that will link you to the
				XML file you want to parse for DDU fiber -> chamber mapping. */

				xercesc::DOMNamedNodeMap *ruiAttributes = pNode1->getAttributes();
				xercesc::DOMNode *ruiNode = ruiAttributes->getNamedItem(xercesc::XMLString::transcode("RUI-to-chamber_map"));
				char *RUIXMLFile = xercesc::XMLString::transcode(ruiNode->getNodeValue());

				//std::cout << "JRG  RUIXMLFile = " << RUIXMLFile << std::endl;
				//char cmdstring[222];
				//sprintf(cmdstring,"/bin/ls -al %s",RUIXMLFile);
				//printf("JRG  try system command %s \n",cmdstring);
				//system(cmdstring);

				xercesc::DOMNode * pNode2 = pNode1->getFirstChild();
				if (pNode2==0) std::cout << " Bad element "<< std::endl;
				while(pNode2) { // Crate
					if (pNode2->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
						std::cout <<"FEDCrateParser: pNode2=" 
							<< xercesc::XMLString::transcode(pNode2->getNodeName()) << std::endl;

						if (strcmp("FEDCrate",xercesc::XMLString::transcode(pNode2->getNodeName()))){
							std::cout << "FEDCrateParser: WARNING - Wrong EmuSystem Child Element <"
							<< xercesc::XMLString::transcode(pNode2->getNodeName()) 
							<< ">, should be <FEDCrate>" << std::endl; 
						}
				
						xercesc::DOMNamedNodeMap * pAttributes = pNode2->getAttributes();
				
						xercesc::DOMNode * e = pAttributes->getNamedItem(xercesc::XMLString::transcode("Crate"));
						int crateNumber = atoi(xercesc::XMLString::transcode(e->getNodeValue()));

						/* At this point, the crate has been fully specified.
							Let us initialize it and later add the appropriate
							objects.
						*/
						FEDCrate* crate = new FEDCrate(crateNumber);

						xercesc::DOMNode * pNode3 = pNode2->getFirstChild(); 
						if (pNode3==0) std::cout << " Bad element "<< std::endl;

						while(pNode3) { // VMEModules (DDU, DCC, controller?)
							if (pNode3->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
								std::cout <<"  "<< xercesc::XMLString::transcode(pNode3->getNodeName()) << std::endl;
							}
					
							if (strcmp("VME",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {
								VMEControllerParser vmeParser = VMEControllerParser(pNode3);
								crate->setController(vmeParser.getController());
							}

							if (strcmp("DDU",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {
								/* PGK the crateNumber is only because the ChamberParser needs it.
								The DDU should be ignorant of its crateNumber. */
								DDUParser dduParser = DDUParser(pNode3, crateNumber, RUIXMLFile);
								crate->addModule((VMEModule *) dduParser.getDDU());
							}
						
							if (strcmp("DCC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {
								DCCParser dccParser = DCCParser(pNode3);
								crate->addModule((VMEModule *) dccParser.getDCC());
							}
						
							pNode3 = pNode3->getNextSibling();
					
						} // end of looping over boards (pNode3)
						std::cout <<"FEDCrateParser: finished looping over boards for crate #"<< crateNumber << std::endl; 
						/* At this time, the crate object is complete. */
						crateVector_.push_back(crate);
				
					} // pNode2 
					pNode2 = pNode2->getNextSibling();
				
				} //end of looping over crates (pNode2) 
			}
		
			pNode1 = pNode1->getNextSibling();
	
		} // end of Top Element Loop, <EMU> only (pNode1)
	
	} //end of parsing config file

	//
	//  Clean up the error handler. The parser does not adopt handlers
	//  since they could be many objects or one object installed for multiple
	//  handlers.
	//

	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	delete parser;
	

	// And call the termination method
	xercesc::XMLPlatformUtils::Terminate();
	// DomMemDebug().print();
		
	//
	//  The DOM document and its contents are reference counted, and need
	//  no explicit deletion.
	//
  
}

