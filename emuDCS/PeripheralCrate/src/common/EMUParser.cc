//-----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include "EMUParser.h"
#include "EmuSystemParser.h"
#include "Crate.h"

#define debug false

//xercesc::XercesDOMParser *parser;

EMUParser::EMUParser():emuSystem(0)
{
  //
  //
}

EMUParser::~EMUParser(){
  //  delete parser;
}

void EMUParser::parseFile(const char* name){ 
  //
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
      std::cout << std::hex << pNode1 << std::endl;
      if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("EmuSystem",xercesc::XMLString::transcode(pNode1->getNodeName()))==0){
	  std::cout << "Parsing..." << std::endl;
	  EmuSystemParser emuSystemParser = EmuSystemParser(pNode1);
	  emuSystem = emuSystemParser.GetEmuSystem();
	} else {
	  std::cout << "EMUParser.ERROR: First tag should be EmuSystem" << std::endl;
	}
      }
      pNode1 = pNode1->getNextSibling();
    } // end of Top Element Loop, <EMU> only (pNode1)
    //
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

