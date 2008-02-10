#include "ChamberParser.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/** Parses the DDU fiber -> chamber linking using the input XML file.
@param fileName is the absolute path location of Karoly's XML file.
@param crate is the crate number in which the DDU resides.
@param slot is the slot in which the DDU resides in its given crate.
**/
ChamberParser::ChamberParser(char *fileName, int crate, int slot)
{
	Chamber *fakeChamber = new Chamber();
	chamberVector_.resize(15,fakeChamber);

	stringstream fileNameStream;
/*  JRG old: used to Prepend the path XDAQ_ROOT for the XML file
	// Remember that fileName is relative to the path $XDAQ_ROOT

	string XDAQ_ROOT(getenv("XDAQ_ROOT"));
	if (XDAQ_ROOT == "") {
		cerr << "Error before Xerces-c Initialization." << endl;
		cerr << "  SET ENVIRONMENT VARIABLE XDAQ_ROOT FIRST!" << endl;
		return;
	}
	fileNameStream << XDAQ_ROOT << (XDAQ_ROOT[XDAQ_ROOT.size() - 1] == '/' ? "" : "/") << fileName;
*/

//  JRG, now use absolute path for the XML file specification:
	fileNameStream << fileName;

	/// Initialize XML4C system
	try{
	
		XMLPlatformUtils::Initialize();
		
	} catch(const XMLException& toCatch) {
		cerr << "Error during Xerces-c Initialization.\n"
			<< "  Exception message:"
			<< XMLString::transcode(toCatch.getMessage()) << endl;
		return;
	}
 
	//  Create our parser, then attach an error handler to the parser.
	//  The parser will call back to methods of the ErrorHandler if it
	//  discovers errors during the course of parsing the XML document.
	//
	XercesDOMParser *parser = new XercesDOMParser;
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setCreateEntityReferenceNodes(false);
	//parser->setToCreateXMLDeclTypeNode(true);
 
	//  Parse the XML file, catching any XML exceptions that might propogate
	//  out of it.
	//
	bool errorsOccured = false;
	try {
	
		parser->parse(fileNameStream.str().c_str());
		
	} catch (const XMLException& e) {
	
		cerr << "An error occured during parsing\n   Message: "
			<< XMLString::transcode(e.getMessage()) << endl;
		errorsOccured = true;
		
	} catch (const DOMException& e) {
		cerr << "An error occured during parsing\n   Message: "
			<< XMLString::transcode(e.msg) << endl;
		errorsOccured = true;
	}

	catch (...) {
		cerr << "An error occured during parsing\n " << endl;
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured){
		// Get maps
		vector<DOMNode *> pMaps = parseMaps(parser->getDocument());
		
		// Loop over maps
		for (unsigned int iMap = 0; iMap < pMaps.size(); iMap++) {
			// Get RUIs
			vector<DOMNode *> pRUIs = parseRUIs(pMaps[iMap]);

			// Loop over RUIs
			for (unsigned int iRUI = 0; iRUI < pRUIs.size(); iRUI++) {
				// Get DDUs
				vector<DOMNode *> pDDUs = parseDDUs(pRUIs[iRUI], crate, slot);

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
	XMLPlatformUtils::Terminate();
	// DomMemDebug().print();

	//
	//  The DOM document and its contents are reference counted, and need
	//  no explicit deletion.
	//
}


vector<DOMNode *> ChamberParser::parseMaps(DOMNode * pDoc) {
	vector<DOMNode *> nodeVector;
	DOMNode * pNode1 = pDoc->getFirstChild();
	while (pNode1) { // RUI-to-chamber_mapping
	  if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
	    if ( strcmp("RUI-to-chamber_mapping",XMLString::transcode(pNode1->getNodeName())) ){
	      cout << "ChamberParser: WARNING - Wrong Top Element <"
		   << XMLString::transcode(pNode1->getNodeName())
		   << ">, should be <RUI-to-chamber_mapping>" << endl;
	    }

	    nodeVector.push_back(pNode1);
	  }
	  pNode1 = pNode1->getNextSibling();
	}

	return nodeVector;
}


vector<DOMNode *> ChamberParser::parseRUIs(DOMNode *pMap) {
	vector<DOMNode *> nodeVector;
	DOMNode *pNode1 = pMap->getFirstChild();
	while (pNode1) { // RUI
		if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			if ( strcmp("RUI",XMLString::transcode(pNode1->getNodeName())) ){
				cout << "ChamberParser: WARNING - Wrong Child Element <"
					<< XMLString::transcode(pNode1->getNodeName())
					<< ">, should be <RUI>" << endl;
			}

			nodeVector.push_back(pNode1);
		}
		pNode1 = pNode1->getNextSibling();
	}

	return nodeVector;
}

vector<DOMNode *> ChamberParser::parseDDUs(DOMNode *pRUI, int crate, int slot) {
	vector<DOMNode *> nodeVector;
	DOMNode *pNode1 = pRUI->getFirstChild();
	while (pNode1) { // DDU
		if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			if ( strcmp("DDU",XMLString::transcode(pNode1->getNodeName())) == 0 ) {

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

void ChamberParser::parseInput(DOMNode *pDDU) {
	DOMNode *pNode1 = pDDU->getFirstChild();
	while (pNode1) { // DDU
		if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
			if ( strcmp("input",XMLString::transcode(pNode1->getNodeName())) ){
				cout << "ChamberParser: WARNING - Wrong Child Element <"
					<< XMLString::transcode(pNode1->getNodeName())
					<< ">, should be <input>" << endl;
			}

			int fiber;
			parseNode(pNode1);
			fillInt("id",fiber);

			/* Prep the chamber object */
			Chamber *chamber = new Chamber();

			DOMNode *pNode2 = pNode1->getFirstChild();
			while (pNode2) { // FiberCassette, PeripheralCrate, or Chamber
			
				if ( strcmp("FiberCassette",XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillInt("crate", chamber->fiberCassetteCrate_);
					fillInt("pos", chamber->fiberCassettePos_);
					fillString("socket", chamber->fiberCassetteSocket_);
				}
				if ( strcmp("PeripheralCrate",XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillInt("id", chamber->peripheralCrateId_);
					fillInt("VMEcrate", chamber->peripheralCrateVMECrate_);
					fillInt("VMEslot", chamber->peripheralCrateVMESlot_);
				}
				if ( strcmp("Chamber",XMLString::transcode(pNode2->getNodeName())) == 0 ) {
					parseNode(pNode2);
					fillString("endcap", chamber->endcap);
					fillInt("station", chamber->station);
					fillInt("type", chamber->type);
					fillInt("number", chamber->number);
				}

				pNode2 = pNode2->getNextSibling();
			}

			/* Fill the vector */
			cout << "  Found input " << fiber << " chamber " << chamber->name() << endl << flush;
			chamberVector_[fiber] = chamber;
			
		}
		pNode1 = pNode1->getNextSibling();
	}

	return;
}
