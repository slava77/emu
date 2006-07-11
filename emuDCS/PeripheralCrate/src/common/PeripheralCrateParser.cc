//-----------------------------------------------------------------------
// $Id: PeripheralCrateParser.cc,v 2.8 2006/07/11 14:49:29 mey Exp $
// $Log: PeripheralCrateParser.cc,v $
// Revision 2.8  2006/07/11 14:49:29  mey
// New Parser sturcture ready to go
//
// Revision 2.7  2006/05/17 14:16:45  mey
// Update
//
// Revision 2.6  2006/03/31 09:22:05  mey
// Update
//
// Revision 2.5  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.4  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.3  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.2  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.1  2006/01/19 10:03:46  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:05  geurts
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
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "PeripheralCrateParser.h"
#include "Crate.h"

#define debug false

xercesc::XercesDOMParser *parser;

PeripheralCrateParser::~PeripheralCrateParser(){
  //  delete parser;
}

void PeripheralCrateParser::parseFile(const char* name){ 
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
  parser = new xercesc::XercesDOMParser;
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
    std::cerr << "An error occured during parsing" << std::endl;
    errorsOccured = true;
  }

  // If the parse was successful, output the document data from the DOM tree
  if (!errorsOccured){
    xercesc::DOMNode * pDoc = parser->getDocument();
    xercesc::DOMNode * pNode1 = pDoc->getFirstChild();
    while (pNode1) {
      if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (debug) std::cout << "PeripheralCrateParser: pNode1=" << xercesc::XMLString::transcode(pNode1->getNodeName()) << std::endl;
	if (strcmp("EmuSystem",xercesc::XMLString::transcode(pNode1->getNodeName()))){
	  std::cout << "PeripheralCrateParser: WARNING - Wrong Top Element <"
	       << xercesc::XMLString::transcode(pNode1->getNodeName()) << ">, should be <EmuSystem>" << std::endl;
	}

	xercesc::DOMNode * pNode2 = pNode1->getFirstChild();
	if (pNode2==0) std::cout << " Bad element "<< std::endl;
  	while(pNode2) {
	  if (pNode2->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	    if (debug) std::cout <<"PeripheralCrateParser: pNode2=" << xercesc::XMLString::transcode(pNode2->getNodeName()) << std::endl;
	    if (strcmp("PeripheralCrate",xercesc::XMLString::transcode(pNode2->getNodeName()))){
	      std::cout << "PeripheralCrateParser: WARNING - Wrong EmuSystem Child Element <"
			<< xercesc::XMLString::transcode(pNode2->getNodeName()) << ">, should be <PeripheralCrate>" << std::endl; 
	    }      
	    
	    xercesc::DOMNamedNodeMap * pAttributes = pNode2->getAttributes();
#ifdef debugV
	    int attrCount = pAttributes->getLength();
	    std::cout << "  number of attributes = " << attrCount << std::endl;
#endif
	    
	    xercesc::DOMNode * e = pAttributes->getNamedItem(xercesc::XMLString::transcode("Crate"));
	    crateNumber = atoi(xercesc::XMLString::transcode(e->getNodeValue()));
#ifdef debugV
	    std::cout << "  crateNumber = " << crateNumber << std::endl;
#endif
	    
  
	    //MvdM new Parser ready vmeParser_ = VMEParser(pNode2, crateNumber);                       

	    xercesc::DOMNode * pNode3 = pNode2->getFirstChild(); 
	    if (pNode3==0) std::cout << " Bad element "<< std::endl;
	    
	    while(pNode3) {
	      // the node was really a board of the peripheral crate like DMB, TMB, etc.
	      if (pNode3->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
		if (debug) std::cout <<"  "<< xercesc::XMLString::transcode(pNode3->getNodeName()) << std::endl;
	      }
	      
	      if (strcmp("VME",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		 vmeParser_ = VMEParser(pNode3, crateNumber);                       
		//Crate * crate = vmeParser_.crate();
		//crateNumber = crate->number();  
		//
	      }
	      
	      if (strcmp("CSC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		cscParser_ = CSCParser(pNode3, crateNumber);
	      }

	      if (strcmp("DAQMB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		daqmbParser_ = DAQMBParser(pNode3, crateNumber);
	      }
	
	      if (strcmp("TMB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		tmbParser_ = TMBParser(pNode3, crateNumber);
	      }
	
	      if (strcmp("CCB",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		ccbParser_ = CCBParser(pNode3, crateNumber);
	      }
	
	      if (strcmp("MPC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
		mpcParser_ = MPCParser(pNode3, crateNumber);
	      }

	      //if (strcmp("DDU",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
	      //dduParser_ = DDUParser(pNode3, crateNumber);       
	      //}
       
	      pNode3 = pNode3->getNextSibling();
      
	    } // end of looping over boards (pNode3)
	    std::cout <<"PeripheralCrateParser: finished looping over boards for crate #"<< crateNumber << std::endl; 

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

