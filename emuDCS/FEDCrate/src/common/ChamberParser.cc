/*****************************************************************************\
* $Id: ChamberParser.cc,v 1.5 2008/08/15 08:35:51 paste Exp $
*
* $Log: ChamberParser.cc,v $
* Revision 1.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "ChamberParser.h"

//#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "Chamber.h"

/** Parses the DDU fiber -> chamber linking using the input XML file.
@param fileName is the absolute path location of Karoly's XML file.
@param crate is the crate number in which the DDU resides.
@param slot is the slot in which the DDU resides in its given crate.
**/
emu::fed::ChamberParser::ChamberParser(char *fileName, int crate, int slot)
{
	Chamber *fakeChamber = new Chamber();
	chamberVector_.resize(15,fakeChamber);

	std::stringstream fileNameStream;
/*  JRG old: used to Prepend the path XDAQ_ROOT for the XML file
	// Remember that fileName is relative to the path $XDAQ_ROOT

	std::string XDAQ_ROOT(getenv("XDAQ_ROOT"));
	if (XDAQ_ROOT == "") {
		std::cerr << "Error before Xerces-c Initialization." << std::endl;
		std::cerr << "  SET ENVIRONMENT VARIABLE XDAQ_ROOT FIRST!" << std::endl;
		return;
	}
	fileNameStream << XDAQ_ROOT << (XDAQ_ROOT[XDAQ_ROOT.size() - 1] == '/' ? "" : "/") << fileName;
*/

//  JRG, now use absolute path for the XML file specification:
	fileNameStream << fileName;

	/// Initialize XML4C system
	try{
	
		xercesc::XMLPlatformUtils::Initialize();
		
	} catch(const xercesc::XMLException& toCatch) {
		std::cerr << "Error during Xerces-c Initialization.\n"
			<< "  Exception message:"
			<< xercesc::XMLString::transcode(toCatch.getMessage()) << std::endl;
		return;
	}
 
	//  Create our parser, then attach an error handler to the parser.
	//  The parser will call back to methods of the ErrorHandler if it
	//  discovers errors during the course of parsing the XML document.
	//
	xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser;
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setCreateEntityReferenceNodes(false);
	//parser->setToCreateXMLDeclTypeNode(true);
 
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	bool errorsOccured = false;
	try {
	
		parser->parse(fileNameStream.str().c_str());
		
	} catch (const xercesc::XMLException& e) {
	
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.getMessage()) << std::endl;
		errorsOccured = true;
		
	} catch (const xercesc::DOMException& e) {
		std::cerr << "An error occured during parsing\n   Message: "
			<< xercesc::XMLString::transcode(e.msg) << std::endl;
		errorsOccured = true;
	}

	catch (...) {
		std::cerr << "An error occured during parsing\n " << std::endl;
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured){
		// Get maps
		std::vector<xercesc::DOMNode *> pMaps = parseMaps(parser->getDocument());
		
		// Loop over maps
		for (unsigned int iMap = 0; iMap < pMaps.size(); iMap++) {
			// Get RUIs
			std::vector<xercesc::DOMNode *> pRUIs = parseRUIs(pMaps[iMap]);

			// Loop over RUIs
			for (unsigned int iRUI = 0; iRUI < pRUIs.size(); iRUI++) {
				// Get DDUs
				std::vector<xercesc::DOMNode *> pDDUs = parseDDUs(pRUIs[iRUI], crate, slot);

				// Loop over DDUs
				for (unsigned int iDDU = 0; iDDU < pDDUs.size(); iDDU++) {
					// Parse the input into the chamberVector.
					parseInput(pDDUs[iDDU]);
				}

			}

		}

	}
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


std::vector<xercesc::DOMNode *> emu::fed::ChamberParser::parseMaps(xercesc::DOMNode * pDoc) {
	std::vector<xercesc::DOMNode *> nodeVector;
	xercesc::DOMNode * pNode1 = pDoc->getFirstChild();
	while (pNode1) { // RUI-to-chamber_mapping
	  if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	    if ( strcmp("RUI-to-chamber_mapping",xercesc::XMLString::transcode(pNode1->getNodeName())) ){
	      std::cout << "ChamberParser: WARNING - Wrong Top Element <"
		   << xercesc::XMLString::transcode(pNode1->getNodeName())
		   << ">, should be <RUI-to-chamber_mapping>" << std::endl;
	    }

	    nodeVector.push_back(pNode1);
	  }
	  pNode1 = pNode1->getNextSibling();
	}

	return nodeVector;
}


std::vector<xercesc::DOMNode *> emu::fed::ChamberParser::parseRUIs(xercesc::DOMNode *pMap) {
	std::vector<xercesc::DOMNode *> nodeVector;
	xercesc::DOMNode *pNode1 = pMap->getFirstChild();
	while (pNode1) { // RUI
		if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			if ( strcmp("RUI",xercesc::XMLString::transcode(pNode1->getNodeName())) ){
				std::cout << "ChamberParser: WARNING - Wrong Child Element <"
					<< xercesc::XMLString::transcode(pNode1->getNodeName())
					<< ">, should be <RUI>" << std::endl;
			}

			nodeVector.push_back(pNode1);
		}
		pNode1 = pNode1->getNextSibling();
	}

	return nodeVector;
}

std::vector<xercesc::DOMNode *> emu::fed::ChamberParser::parseDDUs(xercesc::DOMNode *pRUI, int crate, int slot) {
	std::vector<xercesc::DOMNode *> nodeVector;
	xercesc::DOMNode *pNode1 = pRUI->getFirstChild();
	while (pNode1) { // DDU
		if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			if ( strcmp("DDU",xercesc::XMLString::transcode(pNode1->getNodeName())) == 0 ) {

				// Skip everything that's not the correct DDU
				// Ugly ugly ugly!
				int DDUcrate, DDUslot;
				parseNode(pNode1);
				fillInt("crate",DDUcrate);
				fillInt("slot",DDUslot);
				if (DDUcrate == crate && DDUslot == slot) {
					nodeVector.push_back(pNode1);
				}
			}
		}
		pNode1 = pNode1->getNextSibling();
	}

	return nodeVector;
}

void emu::fed::ChamberParser::parseInput(xercesc::DOMNode *pDDU) {
	xercesc::DOMNode *pNode1 = pDDU->getFirstChild();
	while (pNode1) { // DDU
		if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			if ( strcmp("input",xercesc::XMLString::transcode(pNode1->getNodeName())) ){
				std::cout << "ChamberParser: WARNING - Wrong Child Element <"
					<< xercesc::XMLString::transcode(pNode1->getNodeName())
					<< ">, should be <input>" << std::endl;
			}

			int fiber;
			parseNode(pNode1);
			fillInt("id",fiber);

			/* Prep the chamber object */
			Chamber *chamber = new Chamber();

			xercesc::DOMNode *pNode2 = pNode1->getFirstChild();
			while (pNode2) { // FiberCassette, PeripheralCrate, or Chamber
			
				if ( strcmp("FiberCassette",xercesc::XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillInt("crate", chamber->fiberCassetteCrate_);
					fillInt("pos", chamber->fiberCassettePos_);
					fillString("socket", chamber->fiberCassetteSocket_);
				}
				if ( strcmp("PeripheralCrate",xercesc::XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillInt("id", chamber->peripheralCrateId_);
					fillInt("VMEcrate", chamber->peripheralCrateVMECrate_);
					fillInt("VMEslot", chamber->peripheralCrateVMESlot_);
				}
				if ( strcmp("Chamber",xercesc::XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillString("endcap", chamber->endcap);
					fillInt("station", chamber->station);
					fillInt("type", chamber->type);
					fillInt("number", chamber->number);
				}

				pNode2 = pNode2->getNextSibling();
			}

			/* Fill the std::vector */
			std::cout << "  Found input " << fiber << " chamber " << chamber->name() << std::endl << std::flush;
			chamberVector_[fiber] = chamber;
			
		}
		pNode1 = pNode1->getNextSibling();
	}

	return;
}
