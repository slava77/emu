
//-----------------------------------------------------------------------
// $Id: FEDCrateParser.cc,v 3.7 2008/08/13 14:20:42 paste Exp $
// $Log: FEDCrateParser.cc,v $
// Revision 3.7  2008/08/13 14:20:42  paste
// Massive update removing "using namespace" code and cleaning out stale header files as preparation for RPMs.
//
// Revision 3.6  2008/06/10 13:52:12  gilmore
// improved FED Crate HyperDAQ operability
//
// Revision 3.4  2008/02/10 00:06:06  gilmore
// compatibility fixes for current cscpro/cscdev configuration
//
// Revision 3.3  2008/02/01 16:18:15  gilmore
// added fiber-to-CSCid map decode using local XML file as reference
//
// Revision 3.2  2007/07/23 05:03:28  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.2  2006/01/21 19:55:02  gilmore
// *** empty log message ***
//
// Revision 1.21  2004/07/19 19:37:57  tfcvs
// Removed unused variables and unsigned some variables in order to prevent compiler warnings (-Wall flag) (FG)
//
// Revision 1.20  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------

#include "FEDCrateParser.h"

XERCES_CPP_NAMESPACE_USE
//using namespace std;

void FEDCrateParser::parseFile(const char* name){ 
	//PGK: clear the crateVector
  std::cout << " durkin: entered parser " << std::endl;
	crateVector_.clear();
	/// Initialize XML4C system
	try{
		XMLPlatformUtils::Initialize();
	}
  
	catch(const XMLException& toCatch){
		std::cerr << "Error during Xerces-c Initialization.\n"
			<< "  Exception message:"
			<< XMLString::transcode(toCatch.getMessage()) << std::endl;
		return ;
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
	try{
		parser->parse(name);
	}
  
	catch (const XMLException& e){
		std::cerr << "An error occured during parsing\n   Message: "
			<< XMLString::transcode(e.getMessage()) << std::endl;
		errorsOccured = true;
	}

 
	catch (const DOMException& e){
		std::cerr << "An error occured during parsing\n   Message: "
			<< XMLString::transcode(e.msg) << std::endl;
		errorsOccured = true;
	}

	catch (...){
		std::cerr << "An error occured during parsing\n " << std::endl;
		errorsOccured = true;
	}

	// If the parse was successful, output the document data from the DOM tree
	if (!errorsOccured){
		DOMNode * pDoc = parser->getDocument();
		DOMNode * pNode1 = pDoc->getFirstChild();
		while (pNode1) { // EmuSystem
			if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
				std::cout << "FEDCrateParser: pNode1=" 
					<< XMLString::transcode(pNode1->getNodeName()) << std::endl;
				if (strcmp("EmuSystem",XMLString::transcode(pNode1->getNodeName()))){
					std::cout << "FEDCrateParser: WARNING - Wrong Top Element <"
						<< XMLString::transcode(pNode1->getNodeName())
						<< ">, should be <EmuSystem>" << std::endl;
				}
				/* PGK I added a new attribute that will link you to the
				XML file you want to parse for DDU fiber -> chamber mapping. */

				DOMNamedNodeMap *ruiAttributes = pNode1->getAttributes();
				DOMNode *ruiNode = ruiAttributes->getNamedItem(XMLString::transcode("RUI-to-chamber_map"));
				RUIXMLFile_ = XMLString::transcode(ruiNode->getNodeValue());

				std::cout << "JRG  RUIXMLFile_ = " << RUIXMLFile_ << std::endl;
				char cmdstring[222];
				sprintf(cmdstring,"/bin/ls -al %s",RUIXMLFile_);
				printf("JRG  try system command %s \n",cmdstring);
				system(cmdstring);

				#ifdef debugV
				std::cout << "  RUIXMLFile_ = " << RUIXMLFile_ << std::endl;
				#endif

				DOMNode * pNode2 = pNode1->getFirstChild();
				if (pNode2==0) std::cout << " Bad element "<< std::endl;
				while(pNode2) { // Crate
					if (pNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
						std::cout <<"FEDCrateParser: pNode2=" 
							<< XMLString::transcode(pNode2->getNodeName()) << std::endl;
						#ifdef debugV
						std::cout << "  Getting FED Crate attributes" << std::endl;
						#endif
						if (strcmp("FEDCrate",XMLString::transcode(pNode2->getNodeName()))){
							std::cout << "FEDCrateParser: WARNING - Wrong EmuSystem Child Element <"
							<< XMLString::transcode(pNode2->getNodeName()) 
							<< ">, should be <FEDCrate>" << std::endl; 
						}
				
						DOMNamedNodeMap * pAttributes = pNode2->getAttributes();
						#ifdef debugV
						int attrCount = pAttributes->getLength();
						std::cout << "  number of attributes = " << attrCount << std::endl;
						#endif
				
						DOMNode * e = pAttributes->getNamedItem(XMLString::transcode("Crate"));
						crateNumber = atoi(XMLString::transcode(e->getNodeValue()));
						#ifdef debugV
						std::cout << "  crateNumber = " << crateNumber << std::endl;
						#endif

						/* At this point, the crate has been fully specified.
							Let us initialize it and later add the appropriate
							objects.
						*/
						FEDCrate* crate = new FEDCrate(crateNumber);

						DOMNode * pNode3 = pNode2->getFirstChild(); 
						if (pNode3==0) std::cout << " Bad element "<< std::endl;

						while(pNode3) { // VMEModules (DDU, DCC, controller?)
							if (pNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
								std::cout <<"  "<< XMLString::transcode(pNode3->getNodeName()) << std::endl;
							}
					
							if (strcmp("VME",XMLString::transcode(pNode3->getNodeName()))==0) {  
								vmeParser_ = VMEParser(pNode3);
								crate->setController(vmeParser_.controller());
							}

							if (strcmp("DDU",XMLString::transcode(pNode3->getNodeName()))==0) {
								/* PGK the crateNumber is only because the ChamberParser needs it.
								The DDU should be ignorant of its crateNumber. */
								dduParser_ = DDUParser(pNode3, crateNumber, RUIXMLFile_);
								crate->addModule(dduParser_.ddu());
							}
						
							if (strcmp("DCC",XMLString::transcode(pNode3->getNodeName()))==0) {
								dccParser_ = DCCParser(pNode3);
								crate->addModule(dccParser_.dcc());
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
	XMLPlatformUtils::Terminate();
	// DomMemDebug().print();
		
	//
	//  The DOM document and its contents are reference counted, and need
	//  no explicit deletion.
	//
  
}

