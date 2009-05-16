/*****************************************************************************\
* $Id: XMLConfigurator.cc,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/XMLConfigurator.h"

#include <sstream>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "emu/fed/DDUParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/Crate.h"
#include "emu/fed/ChamberParser.h"
#include "emu/fed/DCCParser.h"
#include "emu/fed/VMEControllerParser.h"
#include "emu/fed/CrateParser.h"

emu::fed::XMLConfigurator::XMLConfigurator(std::string filename):
filename_(filename)
{
	systemName_ = "unnamed";
	// Does nothing
}



std::vector<emu::fed::Crate *> emu::fed::XMLConfigurator::setupCrates()
throw (emu::fed::exception::ConfigurationException)
{
	
	// Initialize XML4C system
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch (xercesc::XMLException &e) {
		std::ostringstream error;
		error << "Error during Xerces-c Initialization: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
 
	// Create our parser
	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setCreateEntityReferenceNodes(false);
	//parser->setToCreateXMLDeclTypeNode(true);
	parser->setCreateCommentNodes(false);
	
	// Parse the XML file, catching any XML exceptions that might propogate out of it.
	try {
		parser->parse(filename_.c_str());
	} catch (xercesc::XMLException& e) {
		std::ostringstream error;
		error << "Error during parsing: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	} catch (xercesc::DOMException& e) {
		std::ostringstream error;
		error << "Error during parsing: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	} catch (...) {
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, "Unknown error during parsing");
	}
	
	// If the parse was successful, output the document data from the DOM tree
	xercesc::DOMNode *pDoc = parser->getDocument();
	xercesc::DOMElement *pEmuSystem = (xercesc::DOMElement *) pDoc->getFirstChild();
	
	// Trick to ignore comments
	while (pEmuSystem->getNodeType() == xercesc::DOMNode::COMMENT_NODE) {
		pEmuSystem = (xercesc::DOMElement *) pEmuSystem->getNextSibling();
	}
	
	if (pEmuSystem == NULL) {
		std::ostringstream error;
		error << "Could not find a top-node in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	if ( strcmp(xercesc::XMLString::transcode(pEmuSystem->getTagName()), "EmuSystem") ) {
		std::ostringstream error;
		error << "The top-node in the XML document " << filename_ << " was not named 'EmuSystem'";
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Let's get the system name
	std::string tempName(xercesc::XMLString::transcode(pEmuSystem->getAttribute( xercesc::XMLString::transcode("Name"))));
	systemName_ = (tempName == "" ? "unnamed" : tempName);
	
	// Parse everything one element at a time.
	
	// Get Crates and parse
	xercesc::DOMNodeList *pFEDCrates = pEmuSystem->getElementsByTagName(xercesc::XMLString::transcode("FEDCrate"));
	
	if (pFEDCrates == NULL) {
		std::ostringstream error;
		error << "No FEDCrate elements in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
	
	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {
		
		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);
		
		CrateParser *crateParser;
		try {
			crateParser = new CrateParser(pFEDCrate);
		} catch (emu::fed::exception::ParseException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing Crate element", e);
		}
		
		Crate *newCrate = crateParser->getCrate();
		
		// Get VMEController
		xercesc::DOMNodeList *pVMEControllers = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("VMEController"));
		
		if (pVMEControllers->getLength() != 1) {
			std::ostringstream error;
			error << "Exactly one VMEController element must exist as a child element of every FEDCrate element in the XML document " << filename_;
			XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
		}
		
		xercesc::DOMElement *pVMEController = (xercesc::DOMElement *) pVMEControllers->item(0);
		
		// Parse the attributes and make the controller.
		VMEControllerParser *vmeParser;
		try {
			vmeParser = new VMEControllerParser(pVMEController);
		} catch (emu::fed::exception::ParseException &e) {
			XCEPT_RETHROW(emu::fed::exception::ParseException, "Exception in parsing VMEController element", e);
		}
		newCrate->setController(vmeParser->getController());
		
		// Get DDUs.  If there are none, then there are none.
		xercesc::DOMNodeList *pDDUs = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("DDU"));
		
		for (unsigned int iDDU = 0; iDDU < pDDUs->getLength(); iDDU++) {
			
			xercesc::DOMElement *pDDU = (xercesc::DOMElement *) pDDUs->item(iDDU);
			
			// Parse and store killed fiber high 5 bits:
			DDUParser *dduParser;
			try {
				dduParser = new DDUParser(pDDU);
			} catch (emu::fed::exception::ParseException &e) {
				XCEPT_RETHROW(emu::fed::exception::ParseException, "Exception in parsing DDU element", e);
			}
			
			DDU *newDDU = dduParser->getDDU();
			
			// Get Chambers.  OK if there are none.
			xercesc::DOMNodeList *pChambers = pDDU->getElementsByTagName(xercesc::XMLString::transcode("Chamber"));
			
			for (unsigned int iChamber = 0; iChamber < pChambers->getLength(); iChamber++) {
				
				xercesc::DOMElement *pChamber = (xercesc::DOMElement *) pChambers->item(iChamber);
				
				// Parse and add to the DDU.
				ChamberParser *chamberParser;
				try {
					chamberParser = new ChamberParser(pChamber);
				} catch (emu::fed::exception::ParseException &e) {
					XCEPT_RETHROW(emu::fed::exception::ParseException, "Exception in parsing Chamber element", e);
				}
				
				newDDU->addChamber(chamberParser->getChamber(), chamberParser->getFiber());
				
				// Alter the killfiber now.
				if (!chamberParser->isKilled()) {
					newDDU->killfiber_ |= (1 << chamberParser->getFiber());
				}
				
			}
			
			// Add the DDU to the crate.
			newCrate->addBoard((VMEModule *) newDDU);
		}
		
		// Get DCCs.  OK if there are none.
		xercesc::DOMNodeList *pDCCs = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("DCC"));
		
		for (unsigned int iDCC = 0; iDCC < pDCCs->getLength(); iDCC++) {
			
			xercesc::DOMElement *pDCC = (xercesc::DOMElement *) pDCCs->item(iDCC);
			
			// Parse
			DCCParser *dccParser;
			try {
				dccParser = new DCCParser(pDCC);
			} catch (emu::fed::exception::ParseException &e) {
				XCEPT_RETHROW(emu::fed::exception::ParseException, "Exception in parsing DCC element", e);
			}
			
			// Add the DCC to the crate.
			newCrate->addBoard((VMEModule *) dccParser->getDCC());
		}
		
		crateVector_.push_back(newCrate);
		
	}
	
	// Delete the parser itself.  Must be done prior to calling Terminate, below.
	delete parser;
	
	// And call the termination method
	xercesc::XMLPlatformUtils::Terminate();

	return crateVector_;
}
