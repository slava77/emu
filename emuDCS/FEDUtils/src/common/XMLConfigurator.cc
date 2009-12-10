/*****************************************************************************\
* $Id: XMLConfigurator.cc,v 1.9 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/

#include "emu/fed/XMLConfigurator.h"

#include <sstream>
#include <sys/stat.h>
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMImplementation.hpp"
#include "xercesc/dom/DOMImplementationLS.hpp"
#include "xercesc/dom/DOMWriter.hpp"
#include "emu/fed/DDUParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/FiberParser.h"
#include "emu/fed/DCCParser.h"
#include "emu/fed/DDU.h"
#include "emu/fed/FIFOParser.h"
#include "emu/fed/VMEControllerParser.h"
#include "emu/fed/CrateParser.h"

#define X(str) xercesc::XMLString::transcode(str)


emu::fed::XMLConfigurator::XMLConfigurator(const std::string &filename):
filename_(filename)
{
	systemName_ = "unnamed";
	timeStamp_ = time(NULL);
}



std::vector<emu::fed::Crate *> emu::fed::XMLConfigurator::setupCrates(const bool &fake)
throw (emu::fed::exception::ConfigurationException)
{
	// Timestamp is easy
	struct stat attrib;
	stat(filename_.c_str(), &attrib);
	timeStamp_ = attrib.st_mtime;
	
	// Initialize XML4C system
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch (xercesc::XMLException &e) {
		std::ostringstream error;
		error << "Error during Xerces-c Initialization: " << X(e.getMessage());
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
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "XML Error during parsing: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	} catch (xercesc::DOMException& e) {
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "DOM Error during parsing: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	} catch (...) {
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, "Unknown error during parsing");
	}
	
	// The file exists, so I can parse out the timestamp
	struct stat fileAttributes;
	stat(filename_.c_str(), &fileAttributes);
	timeStamp_ = fileAttributes.st_mtime;
	
	// If the parse was successful, output the document data from the DOM tree
	xercesc::DOMNode *pDoc = parser->getDocument();
	xercesc::DOMElement *pFEDSystem = (xercesc::DOMElement *) pDoc->getFirstChild();
	
	// Trick to ignore comments
	while (pFEDSystem->getNodeType() == xercesc::DOMNode::COMMENT_NODE) {
		pFEDSystem = (xercesc::DOMElement *) pFEDSystem->getNextSibling();
	}
	
	if (pFEDSystem == NULL) {
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Could not find a top-node in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	if ( strcmp(X(pFEDSystem->getTagName()), "FEDSystem") ) {
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "The top-node in the XML document " << filename_ << " was not named 'FEDSystem'";
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Let's get the system name
	std::string tempName(X(pFEDSystem->getAttribute( X("NAME"))));
	systemName_ = (tempName == "" ? "unnamed" : tempName);
	
	// Parse everything one element at a time.
	
	// Get Crates and parse
	xercesc::DOMNodeList *pFEDCrates = pFEDSystem->getElementsByTagName(X("FEDCrate"));
	
	if (pFEDCrates == NULL) {
		delete parser;
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "No FEDCrate elements in the XML document " << filename_;
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {
		
		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);
		
		Crate *newCrate;
		try {
			newCrate = CrateParser::parse(pFEDCrate);
		} catch (emu::fed::exception::ParseException &e) {
			delete parser;
			xercesc::XMLPlatformUtils::Terminate();
			std::ostringstream error;
			error << "Exception in parsing Crate element: " << e.what();
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
		}
		
		// Get VMEController
		xercesc::DOMNodeList *pVMEControllers = pFEDCrate->getElementsByTagName(X("VMEController"));
		
		if (pVMEControllers->getLength() != 1) {
			delete parser;
			xercesc::XMLPlatformUtils::Terminate();
			std::ostringstream error;
			error << "Exactly one VMEController element must exist as a child element of every FEDCrate element in the XML document " << filename_;
			XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
		}
		
		xercesc::DOMElement *pVMEController = (xercesc::DOMElement *) pVMEControllers->item(0);
		
		// Parse the attributes and make the controller.
		try {
			newCrate->setController(VMEControllerParser::parse(pVMEController, fake));
		} catch (emu::fed::exception::ParseException &e) {
			delete parser;
			xercesc::XMLPlatformUtils::Terminate();
			std::ostringstream error;
			error << "Exception in parsing VMEController element: " << e.what();
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
		}
		
		// Get DDUs.  If there are none, then that is a valid crate anyway (even though it doesn't make sense).
		xercesc::DOMNodeList *pDDUs = pFEDCrate->getElementsByTagName(X("DDU"));
		
		for (unsigned int iDDU = 0; iDDU < pDDUs->getLength(); iDDU++) {
			xercesc::DOMElement *pDDU = (xercesc::DOMElement *) pDDUs->item(iDDU);
			
			// Parse and figure out high 5 bits of killfiber, gbe prescale, etc.
			DDU *newDDU;
			try {
				newDDU = DDUParser::parse(pDDU);
			} catch (emu::fed::exception::ParseException &e) {
				delete parser;
				xercesc::XMLPlatformUtils::Terminate();
				std::ostringstream error;
				error << "Exception in parsing DDU element: " << e.what();
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
			}
			
			// Get Chambers.  OK if there are none.
			xercesc::DOMNodeList *pFibers = pDDU->getElementsByTagName(X("Fiber"));
			
			for (unsigned int iFiber = 0; iFiber < pFibers->getLength(); iFiber++) {
				xercesc::DOMElement *pFiber = (xercesc::DOMElement *) pFibers->item(iFiber);
				// Parse and add to the Fiber.
				try {
					newDDU->addFiber(FiberParser::parse(pFiber));
				} catch (emu::fed::exception::Exception &e) {
					delete parser;
					xercesc::XMLPlatformUtils::Terminate();
					std::ostringstream error;
					error << "Exception in parsing Fiber element: " << e.what();
					XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
				}
			}
			
			// Add the DDU to the crate.
			newCrate->addBoard((VMEModule *) newDDU);
		}
		
		// Get DCCs.  OK if there are none.
		xercesc::DOMNodeList *pDCCs = pFEDCrate->getElementsByTagName(X("DCC"));
		
		for (unsigned int iDCC = 0; iDCC < pDCCs->getLength(); iDCC++) {
			xercesc::DOMElement *pDCC = (xercesc::DOMElement *) pDCCs->item(iDCC);
			
			// Parse
			DCC *newDCC;
			try {
				newDCC = DCCParser::parse(pDCC);
			} catch (emu::fed::exception::ParseException &e) {
				delete parser;
				xercesc::XMLPlatformUtils::Terminate();
				std::ostringstream error;
				error << "Exception in parsing DCC element: " << e.what();
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
			}
			
			// Get FIFOs.  OK if there are none.
			xercesc::DOMNodeList *pFIFOs = pDCC->getElementsByTagName(X("FIFO"));
			
			for (unsigned int iFIFO = 0; iFIFO < pFIFOs->getLength(); iFIFO++) {
				
				xercesc::DOMElement *pFIFO = (xercesc::DOMElement *) pFIFOs->item(iFIFO);
				
				// Parse and add to the FIFO.
				try {
					newDCC->addFIFO(FIFOParser::parse(pFIFO));
				} catch (emu::fed::exception::ParseException &e) {
					delete parser;
					xercesc::XMLPlatformUtils::Terminate();
					std::ostringstream error;
					error << "Exception in parsing FIFO element: " << e.what();
					XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
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



std::string emu::fed::XMLConfigurator::makeXML(const std::vector<emu::fed::Crate *> &crateVector, const std::string &systemName)
throw (emu::fed::exception::ConfigurationException)
{
	// Initialize XML4C system
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch (xercesc::XMLException &e) {
		std::ostringstream error;
		error << "Error during Xerces-c Initialization: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// I need an implementation to build a DOMWriter.  The Core XML implementation is good enough.
	xercesc::DOMImplementation *implementation = xercesc::DOMImplementationRegistry::getDOMImplementation(X("Core"));
	
	if (implementation == NULL) {
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Error getting DOM implementation: feature 'Core' unsupported";
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Create the document
	xercesc::DOMDocument *document;
	try {
		document = implementation->createDocument(NULL, X("FEDSystem"), NULL);
	} catch (xercesc::DOMException& e) {
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Error creating DOMDocument: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Get the newly created root
	xercesc::DOMElement *root;
	try {
		root = (xercesc::DOMElement *) document->getFirstChild();
	} catch (xercesc::DOMException& e) {
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Error getting root element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	try {
		// Set system name
		root->setAttribute(X("NAME"), X(systemName.c_str()));
		
		for (std::vector<Crate *>::const_iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); ++iCrate) {
			
			// Make a crate element
			xercesc::DOMElement *crateElement = CrateParser::makeDOMElement(document, (*iCrate));
			// Append to root
			root->appendChild(crateElement);
			
			// Make a controller element
			xercesc::DOMElement *controllerElement = VMEControllerParser::makeDOMElement(document, (*iCrate)->getController());
			// Append to crate
			crateElement->appendChild(controllerElement);
			
			std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
				
				// Make a DDU element
				xercesc::DOMElement *dduElement = DDUParser::makeDOMElement(document, (*iDDU));
				// Append to crate
				crateElement->appendChild(dduElement);
				
				std::vector<Fiber *> fiberVector = (*iDDU)->getFibers();
				for (std::vector<Fiber *>::iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
					
					// Make a Fiber element
					xercesc::DOMElement *fiberElement = FiberParser::makeDOMElement(document, (*iFiber));
					// Append to DDU
					dduElement->appendChild(fiberElement);
					
				}
				
			}
			
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
				
				// Make a DDU element
				xercesc::DOMElement *dccElement = DCCParser::makeDOMElement(document, (*iDCC));
				// Append to crate
				crateElement->appendChild(dccElement);
				
				std::vector<FIFO *> fifoVector = (*iDCC)->getFIFOs();
				for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
					
					// Make a FIFO element
					xercesc::DOMElement *fifoElement = FIFOParser::makeDOMElement(document, (*iFIFO));
					// Append to DDU
					dccElement->appendChild(fifoElement);
					
				}
				
			}
			
		}
	
	} catch (emu::fed::exception::ParseException &e) {
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Unable to build XML document";
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
	} catch (xercesc::DOMException &e) {
		xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Unable to build XML document due to DOM exception: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	// Create a writer
	xercesc::DOMImplementation *implementationLS = xercesc::DOMImplementationRegistry::getDOMImplementation(X("LS"));
	xercesc::DOMWriter *writer = ((xercesc::DOMImplementationLS *) implementationLS)->createDOMWriter();
	
	// Make things pretty if we can
	if (writer->canSetFeature(X("format-pretty-print"), true)) writer->setFeature(X("format-pretty-print"), true);
	
	// Write
	std::string returnXML = X(writer->writeToString(*document));
	
	delete writer;
	
	// Release all memory used by the document
	document->release();
	
	// Terminate XML services
	xercesc::XMLPlatformUtils::Terminate();
	
	return returnXML;
	
}

