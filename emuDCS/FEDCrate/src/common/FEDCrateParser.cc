/*****************************************************************************\
* $Id: FEDCrateParser.cc,v 3.12 2008/09/19 16:53:52 paste Exp $
*
* $Log: FEDCrateParser.cc,v $
* Revision 3.12  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
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
#include "DDU.h"
#include "ChamberParser.h"
#include "DCCParser.h"
#include "VMEControllerParser.h"
#include "FEDCrate.h"

void emu::fed::FEDCrateParser::parseFile(const char* name){ 
	//PGK: clear the crateVector
	//std::cout << " durkin: entered parser " << std::endl;
	crateVector_.clear();
	
	/// Initialize XML4C system
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch(const xercesc::XMLException& toCatch) {
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
	try {
		parser->parse(name);
	}
  
	catch (const xercesc::XMLException& e) {
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.getMessage()) << std::endl;
		//errorsOccured = true;
		throw;
	}

 
	catch (const xercesc::DOMException& e){
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.msg) << std::endl;
		//errorsOccured = true;
		throw;
	}

	catch (...) {
		std::cerr << "An error occured during parsing\n " << std::endl;
		//errorsOccured = true;
		throw;
	}

	// If the parse was successful, output the document data from the DOM tree

	xercesc::DOMNode *pDoc = parser->getDocument();
	xercesc::DOMElement *pEmuSystem = (xercesc::DOMElement *) pDoc->getFirstChild();

	while (pEmuSystem->getNodeType() == xercesc::DOMNode::COMMENT_NODE) {
		pEmuSystem = (xercesc::DOMElement *) pEmuSystem->getNextSibling();
	}

	if (pEmuSystem == NULL) {

		std::cerr << "No elements in file " << name << std::endl;
		return;
	}

	if ( strcmp(xercesc::XMLString::transcode(pEmuSystem->getTagName()),"EmuSystem") ) {
		std::cerr << "No EmuSystem elements in file " << name << std::endl;
		return;
	}
	
	std::cout << xercesc::XMLString::transcode(pEmuSystem->getTagName()) << std::endl;

	// Let's get the system name
	std::string tempName(xercesc::XMLString::transcode(pEmuSystem->getAttribute( xercesc::XMLString::transcode("Name"))));
	name_ = tempName;

	// Get FEDCrates and parse
	xercesc::DOMNodeList *pFEDCrates = pEmuSystem->getElementsByTagName(xercesc::XMLString::transcode("FEDCrate"));

	if (pFEDCrates == NULL) {
		std::cerr << "No FEDCrate tags defined in file " << name << std::endl;
		return;
	}

	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {

		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);

		// Report!
		std::cout << " " << xercesc::XMLString::transcode(pFEDCrate->getTagName()) << std::endl;

		// Make the FEDCrate.  Get the number first.
		int crateNumber = atoi(xercesc::XMLString::transcode(pFEDCrate->getAttribute( xercesc::XMLString::transcode("Number"))));
		
		FEDCrate *newCrate = new FEDCrate(crateNumber);

		// Get VMEController
		xercesc::DOMNodeList *pVMEControllers = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("VMEController"));
		
		if (pVMEControllers->getLength() != 1) {
			std::cerr << "Exactly one VMEController not defined in a FEDCrate in file " << name << std::endl;
			return;
		}

		// Report!
		xercesc::DOMElement *pVMEController = (xercesc::DOMElement *) pVMEControllers->item(0);
		std::cout << "  " << xercesc::XMLString::transcode(pVMEController->getTagName()) << std::endl;

		// Parse the attributes and make the controller.
		VMEControllerParser vmeParser = VMEControllerParser(pVMEController);
		newCrate->setController(vmeParser.getController());

		// Get DDUs
		xercesc::DOMNodeList *pDDUs = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("DDU"));

		for (unsigned int iDDU = 0; iDDU < pDDUs->getLength(); iDDU++) {
			
			xercesc::DOMElement *pDDU = (xercesc::DOMElement *) pDDUs->item(iDDU);
			
			// Report!
			std::cout << "  " << xercesc::XMLString::transcode(pDDU->getTagName()) << std::endl;

			// Parse and store killed fiber high 5 bits:
			DDUParser dduParser = DDUParser(pDDU);
			
			DDU *newDDU = dduParser.getDDU();
			unsigned long int killfiber = (dduParser.getOptions()) << 15;

			// Get Chambers
			xercesc::DOMNodeList *pChambers = pDDU->getElementsByTagName(xercesc::XMLString::transcode("Chamber"));
			
			for (unsigned int iChamber = 0; iChamber < pChambers->getLength(); iChamber++) {
				
				xercesc::DOMElement *pChamber = (xercesc::DOMElement *) pChambers->item(iChamber);
				
				// Report!
				std::cout << "   " << xercesc::XMLString::transcode(pChamber->getTagName()) << std::endl;

				// Parse and add to the DDU.
				ChamberParser chamberParser = ChamberParser(pChamber);

				//std::cout << "I have a chamber!  Fiber " << chamberParser.getFiber() << " killed " << chamberParser.isKilled() << std::endl;
				newDDU->addChamber(chamberParser.getChamber(), chamberParser.getFiber());
				//std::cout << "Added to DDU!" << std::endl;
				
				// Alter the killfiber now.
				if (!chamberParser.isKilled()) {
					killfiber |= (1 << chamberParser.getFiber());
					std::cout << "   Fiber " << chamberParser.getFiber() << " Alive" << std::endl;
				} else {
					std::cout << "   Fiber " << chamberParser.getFiber() << " Killed" << std::endl;
				}

			}

			std::cout << "   Loading killfiber " << std::hex << killfiber << std::dec << std::endl;
			newDDU->killfiber_ = killfiber;

			// Add the DDU to the crate.
			newCrate->addModule(newDDU);
		}

		// Get DCCs
		xercesc::DOMNodeList *pDCCs = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("DCC"));
		
		for (unsigned int iDCC = 0; iDCC < pDCCs->getLength(); iDCC++) {
			
			xercesc::DOMElement *pDCC = (xercesc::DOMElement *) pDCCs->item(iDCC);
			
			// Report!
			std::cout << "  " << xercesc::XMLString::transcode(pDCC->getTagName()) << std::endl;

			// Parse
			DCCParser dccParser = DCCParser(pDCC);

			// Add the DCC to the crate.
			newCrate->addModule((VMEModule *) dccParser.getDCC());
		}

		// Done:  push the FEDCrate back.
		crateVector_.push_back(newCrate);
	}

	std::cout << "Done parsing file " << name << std::endl;
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

