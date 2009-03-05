/*****************************************************************************\
* $Id: CrateParser.cc,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: CrateParser.cc,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.14  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.13  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
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

#include "emu/fed/CrateParser.h"

#include <sstream>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "emu/fed/DDUParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/ChamberParser.h"
#include "emu/fed/DCCParser.h"
#include "emu/fed/VMEControllerParser.h"
#include "emu/fed/Crate.h"

emu::fed::CrateParser::CrateParser(const char *fileName)
throw (emu::fed::exception::ParseException, emu::fed::exception::FileException)
{ 
	// Initialize XML4C system
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch (xercesc::XMLException &e) {
		std::ostringstream error;
		error << "Error during Xerces-c Initialization: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
 
	// Create our parser
	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setCreateEntityReferenceNodes(false);
	//parser->setToCreateXMLDeclTypeNode(true);
	parser->setCreateCommentNodes(false);

	// Parse the XML file, catching any XML exceptions that might propogate
	// out of it.
	try {
		parser->parse(fileName);
	} catch (xercesc::XMLException& e) {
		std::ostringstream error;
		error << "Error during parsing: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	} catch (xercesc::DOMException& e) {
		std::ostringstream error;
		error << "Error during parsing: " << xercesc::XMLString::transcode(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	} catch (...) {
		XCEPT_RAISE(emu::fed::exception::ParseException, "Unknown error during parsing");
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
		error << "Could not find a top-node in the XML document " << fileName;
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}

	if ( strcmp(xercesc::XMLString::transcode(pEmuSystem->getTagName()), "EmuSystem") ) {
		std::ostringstream error;
		error << "The top-node in the XML document " << fileName << " was not named 'EmuSystem'";
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
	
	// Let's get the system name
	std::string tempName(xercesc::XMLString::transcode(pEmuSystem->getAttribute( xercesc::XMLString::transcode("Name"))));
	name_ = tempName; // If unnamed, this will be an empty string.

	// Get Crates and parse
	xercesc::DOMNodeList *pFEDCrates = pEmuSystem->getElementsByTagName(xercesc::XMLString::transcode("FEDCrate"));

	if (pFEDCrates == NULL) {
		std::ostringstream error;
		error << "No FEDCrate elements in the XML document " << fileName;
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}

	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {

		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);

		// Make the FEDCrate.  Get the number first.
		int crateNumber = atoi(xercesc::XMLString::transcode(pFEDCrate->getAttribute( xercesc::XMLString::transcode("Number")))); // If unnumbered, will be zero
		
		Crate *newCrate = new Crate(crateNumber);

		// Get VMEController
		xercesc::DOMNodeList *pVMEControllers = pFEDCrate->getElementsByTagName(xercesc::XMLString::transcode("VMEController"));
		
		if (pVMEControllers->getLength() != 1) {
			std::ostringstream error;
			error << "Exactly one VMEController element must exist as a child element of every FEDCrate element in the XML document " << fileName;
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

		// Done:  push the Crate back.
		crateVector_.push_back(newCrate);
	}

	// Delete the parser itself.  Must be done prior to calling Terminate, below.
	delete parser;

	// And call the termination method
	xercesc::XMLPlatformUtils::Terminate();

}

