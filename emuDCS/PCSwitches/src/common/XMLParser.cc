#include "XMLParser.h"
#include <stdio.h>
#include <iostream>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "Switch.h"

namespace emu {
  namespace pcsw {

XMLParser::XMLParser(): pAttributes_(0) {}

void XMLParser::parseNode(xercesc::DOMNode * pNode) 
{
  pAttributes_ = pNode->getAttributes();

#ifdef debugV
  std::cout << "   number of attributes = " << pAttributes_->getLength() << std::endl;
#endif
}

bool XMLParser::fillInt(std::string item, int & target) 
{
  bool found=false;
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%d", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillIntX(std::string item, int & target) 
{
  bool found=false;
  int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%x", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillLongIntX(std::string item, long int & target) 
{
  bool found=false;
  long int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%lx", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "fillLongIntX: " << item << " = 0x" << std::hex << target << std::endl;
#endif
  }
  return found;  
}

bool XMLParser::fillLongLongIntX(std::string item, long long int & target) 
{
  bool found=false;
  long long int value; 
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%Lx", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
    std::cout << "fillLongLongIntX: " << item << " = 0x" << std::hex << target << std::endl;
#endif
  }
  return found;  
}


bool XMLParser::fillString(std::string item, std::string & target) 
{
  bool found=false;
  std::string value;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    value = xercesc::XMLString::transcode(pAttributeNode->getNodeValue());
    target = value;
    found = true;
#ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;
}


bool XMLParser::fillFloat(std::string item, float & target) 
{
  float value;
  bool found=false;
  XMLCh * name = xercesc::XMLString::transcode(item.c_str());
  xercesc::DOMAttr * pAttributeNode = (xercesc::DOMAttr*) pAttributes_->getNamedItem(name);
  if(pAttributeNode) {
    int err = sscanf(xercesc::XMLString::transcode(pAttributeNode->getNodeValue()), "%f", &value);
    if (err==0) std::cerr << "ERRORS in parsing!!!" << item << " code " << err << std::endl;
    target = value;
    found = true;
#ifdef debugV
      std::cout << "  " << item << " = " << target << std::endl;
#endif
  }
  return found;
}

void XMLParser::ConnectionParser(xercesc::DOMNode *pNode, Switch * S, int Counter) {
  std::string name, label, ipaddr, pmac;
  int nswitch, nport, vlan;
  parseNode(pNode);

  if ( fillString("name", name) ) {
    S->side[Counter].name=name;
  }
  if ( fillString("label", label) ) {
    S->side[Counter].label=label;
  }
  if ( fillInt("nswitch",nswitch) ) {
    S->side[Counter].nswitch=nswitch;
  }
  if ( fillInt("nport",nport) ) {
    S->side[Counter].nport=nport;
  }
  if ( fillInt("vlan",vlan) ) {
    S->side[Counter].vlan=vlan;
  }
  if ( fillString("ipaddr", ipaddr) ) {
    S->side[Counter].ipaddr=ipaddr;
  }
  if ( fillString("pmac", pmac) ) {
    S->side[Counter].pmac.mac=pmac;
  }

}


void XMLParser::FillSwitchParser(xercesc::DOMNode *pNode, Switch * S, int Counter) {
  std::string label, ipaddr;
  parseNode(pNode);

  if ( fillString("label", label) )
    S->ip_addresses[Counter].label = label;
  if ( fillString("ipaddr", ipaddr) )
    S->ip_addresses[Counter].ipaddr=ipaddr;
}

void XMLParser::PCParser(xercesc::DOMNode *pNode, Switch * S, int Counter) {

  std::string machine,eth;
  parseNode(pNode);

  if ( fillString("machine", machine) )
    S->pc[Counter].machine = machine;
  if ( fillString("eth", eth) )
    S->pc[Counter].eth=eth;
}

void XMLParser::EndcapSideParser(xercesc::DOMNode *pNode, Switch * S, xercesc::DOMNode *pNodeGlobal) { 
  parseNode(pNode);
  
  xercesc::DOMNode * pNode3 = pNode->getFirstChild(); 
  if (pNode3==0) std::cout << " Bad element "<< std::endl;
  
  int counter=0, connection_counter=0,switch_counter=0,pc_counter=0;;

  while(pNode3) {
    if (pNode3->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {

#ifdef debugV
       std::cout <<"  "<< xercesc::XMLString::transcode(pNode3->getNodeName()) << std::endl;
#endif
    }
    
    if (strcmp("Connection",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
	ConnectionParser(pNode3, S, connection_counter);
	connection_counter++;
    }

    if (strcmp("Switch",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
        FillSwitchParser(pNode3, S, switch_counter);
	switch_counter++;
    }
    
    if (strcmp("PC",xercesc::XMLString::transcode(pNode3->getNodeName()))==0) {  
        PCParser(pNode3, S, pc_counter);
	pc_counter++;
    }
    
    pNode3 = pNode3->getNextSibling();
   
    counter++; 
  }


  S->num_pcs = pc_counter;
  S->num_connections=connection_counter;
  S->num_switches = switch_counter;

  std::cout <<"EndcapSideParser: finished looping over Connections for Side "<< S->sidelabel << std::endl; 
}


emu::pcsw::Switch * XMLParser::SwitchParser(xercesc::DOMNode *pNode) {
  std::cout << "SwitchParser" << std::endl;
  std::string label;
  S_ = new Switch();
  xercesc::DOMNode * pNodeGlobal = 0x0;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      if (strcmp("Global",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	std::cout << "Found Global" << std::endl;
	//
	pNodeGlobal = pNode1->cloneNode(true);
	//
      }
      //
      if (strcmp("EndcapSide",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	parseNode(pNode1);
	if ( fillString("label", label) )
          S_->sidelabel = label;
	EndcapSideParser(pNode1,S_,pNodeGlobal);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Top Element Loop, <EMU> only (pNode1)
  return S_;
}

void XMLParser::parseFile(const std::string name)
{ 
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
  xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser;
  parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  parser->setCreateEntityReferenceNodes(false);
  //parser->setToCreateXMLDeclTypeNode(true);
  
 
  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;
  try{
    parser->parse(name.c_str());
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
	if (strcmp("EmuSystem",xercesc::XMLString::transcode(pNode1->getNodeName()))==0
         || strcmp("EmuEndcap",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
        {
          S_ = SwitchParser(pNode1);
	} else {
	  std::cerr << "XMLParser.ERROR: First tag must be EmuEndcap or EmuSystem" << std::endl;
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
  //
  //  The DOM document and its contents are reference counted, and need
  //  no explicit deletion.
  //
}
  } // namespace emu::pcsw
  } // namespace emu
