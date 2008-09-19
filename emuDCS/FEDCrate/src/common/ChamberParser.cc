/*****************************************************************************\
* $Id: ChamberParser.cc,v 1.7 2008/09/19 16:53:52 paste Exp $
*
* $Log: ChamberParser.cc,v $
* Revision 1.7  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.6  2008/08/26 13:09:02  paste
* Documentation update.
*
* Revision 1.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "ChamberParser.h"

//#include <stdlib.h>
#include <iostream>
#include <sstream>
//#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/XMLPScanToken.hpp>
//#include <xercesc/parsers/XercesDOMParser.hpp>

#include "Chamber.h"


emu::fed::ChamberParser::ChamberParser(xercesc::DOMElement *pNode)
{
	//std::cout << "parse" << std::endl;
	parseNode(pNode);

	//std::cout << "new" << std::endl;
	chamber_ = new Chamber();
	
	std::string chamberName;
	std::string pCrateName;
	
	// Chamber Name looks like "+1/2/03"
	fillString("Name", chamberName);
	//std::cout << "chamberName" << chamberName << std::endl;

	int station, type, number;
	sscanf(chamberName.c_str(), "%*c%1d/%1d/%02d", &station, &type, &number);

	std::string endcapString(chamberName.substr(0,1));
	chamber_->endcap = endcapString;
	//std::cout << "endcap" << chamber_->endcap << std::endl;
	chamber_->station = station;
	//std::cout << "station" << chamber_->station << std::endl;
	chamber_->type = type;
	//std::cout << "type" << chamber_->type << std::endl;
	chamber_->number = number;
	//std::cout << "number" << chamber_->number << std::endl;

	// Chamber PeripheralCrate looks like "VMEp1_2"
	fillString("PeripheralCrate", pCrateName);
	//std::cout << "pCrateName" << pCrateName << std::endl;

	int pcrate;
	sscanf(pCrateName.c_str(), "VME%*c%*d_%d", &pcrate);
	
	chamber_->peripheralCrateVMECrate_ = pcrate;
	//std::cout << "peripheralCrateVMECrate" << chamber_->peripheralCrateVMECrate_ << std::endl;

	fillInt("Fiber",fiber_);
	//std::cout << "Fiber" << fiber_ << std::endl;
	fillInt("Killed",killed_);
	//std::cout << "Killed" << killed_ << std::endl;
	//std::cout << "done" << std::endl;
}

/*
emu::fed::ChamberParser::ChamberParser(char *fileName, int crate, int slot)
{
	Chamber *fakeChamber = new Chamber();
	chamberVector_.resize(15,fakeChamber);

	std::stringstream fileNameStream;

//  JRG, now use absolute path for the XML file specification:
	fileNameStream << fileName;

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

			std::cout << "  Found input " << fiber << " chamber " << chamber->name() << std::endl << std::flush;
			chamberVector_[fiber] = chamber;
			
		}
		pNode1 = pNode1->getNextSibling();
	}

	return;
}
*/
