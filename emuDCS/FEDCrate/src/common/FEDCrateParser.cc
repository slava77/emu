
//-----------------------------------------------------------------------
// $Id: FEDCrateParser.cc,v 1.1 2005/08/31 18:20:32 gilmore Exp $
// $Log: FEDCrateParser.cc,v $
// Revision 1.1  2005/08/31 18:20:32  gilmore
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
#include "FEDCrateParser.h"
#include "Crate.h"


XERCES_CPP_NAMESPACE_USE
using namespace std;

void FEDCrateParser::parseFile(const char* name){ 
  /// Initialize XML4C system
  try{
    XMLPlatformUtils::Initialize();
  }
  
  catch(const XMLException& toCatch){
    cerr << "Error during Xerces-c Initialization.\n"
	 << "  Exception message:"
	 << XMLString::transcode(toCatch.getMessage()) << endl;
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
    cerr << "An error occured during parsing\n   Message: "
	 << XMLString::transcode(e.getMessage()) << endl;
    errorsOccured = true;
  }

 
  catch (const DOMException& e){
    cerr << "An error occured during parsing\n   Message: "
	 << XMLString::transcode(e.msg) << endl;
    errorsOccured = true;
  }

  catch (...){
    cerr << "An error occured during parsing\n " << endl;
    errorsOccured = true;
  }

  // If the parse was successful, output the document data from the DOM tree
  if (!errorsOccured){
    DOMNode * pDoc = parser->getDocument();
    DOMNode * pNode1 = pDoc->getFirstChild();
    while (pNode1) {
      if (pNode1->getNodeType() == DOMNode::ELEMENT_NODE) {
	cout << "FEDCrateParser: pNode1=" << XMLString::transcode(pNode1->getNodeName()) << endl;
	if (strcmp("EmuSystem",XMLString::transcode(pNode1->getNodeName()))){
	  cout << "FEDCrateParser: WARNING - Wrong Top Element <"
	       << XMLString::transcode(pNode1->getNodeName()) << ">, should be <EmuSystem>" << endl;
	}

	DOMNode * pNode2 = pNode1->getFirstChild();
	if (pNode2==0) cout << " Bad element "<< endl;
  	while(pNode2) {
	  if (pNode2->getNodeType() == DOMNode::ELEMENT_NODE) {
	    cout <<"FEDCrateParser: pNode2=" << XMLString::transcode(pNode2->getNodeName()) << endl;
#ifdef debugV
	    cout << "  Getting FED Crate attributes" << endl;
#endif
	    if (strcmp("FEDCrate",XMLString::transcode(pNode2->getNodeName()))){
	      cout << "FEDCrateParser: WARNING - Wrong EmuSystem Child Element <"
		   << XMLString::transcode(pNode2->getNodeName()) << ">, should be <FEDCrate>" << endl; 
	    }      

	    DOMNamedNodeMap * pAttributes = pNode2->getAttributes();
#ifdef debugV
	    int attrCount = pAttributes->getLength();
	    cout << "  number of attributes = " << attrCount << endl;
#endif

	    DOMNode * e = pAttributes->getNamedItem(XMLString::transcode("Crate"));
	    crateNumber = atoi(XMLString::transcode(e->getNodeValue()));
#ifdef debugV
	    cout << "  crateNumber = " << crateNumber << endl;
#endif
	    
	    DOMNode * pNode3 = pNode2->getFirstChild(); 
	    if (pNode3==0) cout << " Bad element "<< endl;
      
	    while(pNode3) {
	      // the node was really a board of the FED crate like DMB, TMB, etc.
	      if (pNode3->getNodeType() == DOMNode::ELEMENT_NODE) {
		cout <<"  "<< XMLString::transcode(pNode3->getNodeName()) << endl;
	      }

	      if (strcmp("VME",XMLString::transcode(pNode3->getNodeName()))==0) {  
		vmeParser_ = VMEParser(pNode3, crateNumber);                       
		Crate * crate = vmeParser_.crate();
		crateNumber = crate->number();  
	      }

	      if (strcmp("DDU",XMLString::transcode(pNode3->getNodeName()))==0) {  
		dduParser_ = DDUParser(pNode3, crateNumber);
	      }
	
	      if (strcmp("DCC",XMLString::transcode(pNode3->getNodeName()))==0) {  
		dccParser_ = DCCParser(pNode3, crateNumber);
	      }
	
	      pNode3 = pNode3->getNextSibling();
      
	    } // end of looping over boards (pNode3)
	    cout <<"FEDCrateParser: finished looping over boards for crate #"<< crateNumber << endl; 

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

