/*****************************************************************************\
* $Id: XMLConfigurator.cc,v 1.4 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/

#include "emu/fed/XMLConfigurator.h"

#include <sstream>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "emu/fed/DDUParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/Crate.h"
#include "emu/fed/FiberParser.h"
#include "emu/fed/DCCParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/FIFOParser.h"
#include "emu/fed/VMEControllerParser.h"
#include "emu/fed/CrateParser.h"

emu::fed::XMLConfigurator::XMLConfigurator(const std::string &filename):
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
	xercesc::DOMElement *pFEDSystem = (xercesc::DOMElement *) pDoc->getFirstChild();
	
	// Trick to ignore comments
	while (pFEDSystem->getNodeType() == xercesc::DOMNode::COMMENT_NODE) {
		pFEDSystem = (xercesc::DOMElement *) pFEDSystem->getNextSibling();
	}
	
	if (pFEDSystem == NULL) {
		std::ostringstream error;
		error << "Could not find a top-node in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	if ( strcmp(xercesc::XMLString::transcode(pFEDSystem->getTagName()), "FEDSystem") ) {
		std::ostringstream error;
		error << "The top-node in the XML document " << filename_ << " was not named 'FEDSystem'";
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Let's get the system name
	std::string tempName(xercesc::XMLString::transcode(pFEDSystem->getAttribute( xercesc::XMLString::transcode("NAME"))));
	systemName_ = (tempName == "" ? "unnamed" : tempName);
	
	// Parse everything one element at a time.
	
	// Get Crates and parse
	xercesc::DOMNodeList *pFEDCrates = pFEDSystem->getElementsByTagName(xercesc::XMLString::transcode("FEDCrate"));
	
	if (pFEDCrates == NULL) {
		std::ostringstream error;
		error << "No FEDCrate elements in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {
		
		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);
		
		Crate *newCrate;
		try {
			newCrate = CrateParser(pFEDCrate).getCrate();
		} catch (emu::fed::exception::ParseException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing Crate element", e);
		}
		
		// Get VMEController
		xercesc::DOMNodeList *pVMEControllers = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("VMEController"));
		
		if (pVMEControllers->getLength() != 1) {
			std::ostringstream error;
			error << "Exactly one VMEController element must exist as a child element of every FEDCrate element in the XML document " << filename_;
			XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
		}
		
		xercesc::DOMElement *pVMEController = (xercesc::DOMElement *) pVMEControllers->item(0);
		
		// Parse the attributes and make the controller.
		try {
			newCrate->setController(VMEControllerParser(pVMEController).getController());
		} catch (emu::fed::exception::ParseException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing VMEController element", e);
		}
		
		// Get DDUs.  If there are none, then that is a valid crate anyway (even though it doesn't make sense).
		xercesc::DOMNodeList *pDDUs = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("DDU"));
		
		for (unsigned int iDDU = 0; iDDU < pDDUs->getLength(); iDDU++) {
			xercesc::DOMElement *pDDU = (xercesc::DOMElement *) pDDUs->item(iDDU);
			
			// Parse and figure out high 5 bits of killfiber, gbe prescale, etc.
			DDU *newDDU;
			try {
				newDDU = DDUParser(pDDU).getDDU();
			} catch (emu::fed::exception::ParseException &e) {
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing DDU element", e);
			}
			
			// Get Chambers.  OK if there are none.
			xercesc::DOMNodeList *pFibers = pDDU->getElementsByTagName(xercesc::XMLString::transcode("Fiber"));
			
			for (unsigned int iFiber = 0; iFiber < pFibers->getLength(); iFiber++) {
				xercesc::DOMElement *pFiber = (xercesc::DOMElement *) pFibers->item(iFiber);
				// Parse and add to the Fiber.
				try {
					FiberParser fiberParser = FiberParser(pFiber);
					newDDU->addFiber(fiberParser.getFiber());
				} catch (emu::fed::exception::Exception &e) {
					XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing Fiber element", e);
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
			DCC *newDCC;
			try {
				newDCC = DCCParser(pDCC).getDCC();
			} catch (emu::fed::exception::ParseException &e) {
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing DCC element", e);
			}
			
			// Get FIFOs.  OK if there are none.
			xercesc::DOMNodeList *pFIFOs = pDCC->getElementsByTagName(xercesc::XMLString::transcode("FIFO"));
			
			for (unsigned int iFIFO = 0; iFIFO < pFIFOs->getLength(); iFIFO++) {
				
				xercesc::DOMElement *pFIFO = (xercesc::DOMElement *) pFIFOs->item(iFIFO);
				
				// Parse and add to the FIFO.
				try {
					FIFOParser fifoParser = FIFOParser(pFIFO);
					// This alters the fifos in use, too.
					newDCC->addFIFO(fifoParser.getFIFO());
				} catch (emu::fed::exception::ParseException &e) {
					XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Exception in parsing FIFO element", e);
				}
			}
			
			// Add the DCC to the crate.

			newCrate->addBoard((VMEModule *) newDCC);
		}
		
		crateVector_.push_back(newCrate);
		
	}
	
	// Delete the parser itself.  Must be done prior to calling Terminate, below.
	delete parser;
	
	// And call the termination method
	xercesc::XMLPlatformUtils::Terminate();

	return crateVector_;
}
