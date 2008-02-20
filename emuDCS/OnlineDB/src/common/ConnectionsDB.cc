/**********************************
       S. Durkin
       Feb. 2, 2008

A quickly written set of routines to 
parse and use xml fiber connection
files. People need to specify chamber
not crate and slot in their GUIs

***********************************/


#include "ConnectionsDB.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

ConnectionsDB::ConnectionsDB(): pAttributes_(0)
{
  HOMEDIR_=getenv("BUILD_HOME");
  HOMEDIR_+="/emu/emuDCS/OnlineDB/xml/";
  std::cout << "Home Directory:" << HOMEDIR_ <<std::endl;

  idb=0;
  ConnectionsDBXMLFile_=HOMEDIR_+"RUI-to-chamber_mapping.xml";
  parseFile(ConnectionsDBXMLFile_);

  isdb=0;
  sConnectionsDBXMLFile_=HOMEDIR_+"SwitchConnections.xml";
  sparseFile(sConnectionsDBXMLFile_);
  
}

ConnectionsDB::~ConnectionsDB()
{
  // std::cout << " kill ConnectionsDB " << std::endl;
}


void ConnectionsDB::parseNode(xercesc::DOMNode * pNode) 
{
  pAttributes_ = pNode->getAttributes();

#ifdef debugV
  std::cout << "   number of attributes = " << pAttributes_->getLength() << std::endl;
#endif
}

bool ConnectionsDB::fillInt(std::string item, int & target) 
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

bool ConnectionsDB::fillIntX(std::string item, int & target) 
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

bool ConnectionsDB::fillLongLongIntX(std::string item, long long int & target) 
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
    std::cout <<  "fillLongLongIntX: " << item << " = 0x" << std::hex << target << std::endl;
#endif
  }
  return found;  
}


bool ConnectionsDB::fillString(std::string item, std::string & target) 
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


bool ConnectionsDB::fillFloat(std::string item, float & target) 
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



void ConnectionsDB::parseFile(const std::string name)
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
  // std::cout << " enter ConnectionsDB parseFile " << std::endl;

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
    // std::cout << " enter pnode1 loop " << std::endl;
    while (pNode1) {
      if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("RUI-to-chamber_mapping",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
        {
	  MapParser(pNode1);
	} else 
        {
	  std::cerr << "ConnectionsDB.ERROR: First tag must be RUI-to-chamber_mapping" << std::endl;
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


void ConnectionsDB::MapParser(xercesc::DOMNode *pNode)
{
  // std::cout << "MapParser" << std::endl;
  //
  //    xercesc::DOMNode * pNodeGlobal = 0x0;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("RUI",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found RUI" << std::endl;
	 RUIParser(pNode1);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element Loop
  
}

void ConnectionsDB::RUIParser(xercesc::DOMNode *pNode)
{
  // std::cout << "RUIParser" << std::endl;
 
  parseNode(pNode);

  fillInt("instance",RUI_instance);
  // std::cout <<" RUI_instance " << RUI_instance << std::endl;

  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("DDU",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found DDU" << std::endl;
	 DDUParser(pNode1);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element Loop
  
}

void ConnectionsDB::DDUParser(xercesc::DOMNode *pNode)
{
  // std::cout << "DDUParser" << std::endl;
 
  parseNode(pNode);

  fillInt("crate",ddu_crate);
  fillInt("slot",ddu_slot);

  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("input",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 inputParser(pNode1);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element Loop
  
}

void ConnectionsDB::inputParser(xercesc::DOMNode *pNode)
{
  // std::cout << "inputParser" << std::endl;
 
  parseNode(pNode);

  fillInt("id",ddu_input);

  // std::cout << "ddu-input " << ddu_input << std::endl;

  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("PeripheralCrate",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	//  std::cout << "Found input" << std::endl;
	 PeripheralCrateParser(pNode1);
      }
      if (strcmp("Chamber",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 ChamberParser(pNode1);
      }
       if (strcmp("FiberCassette",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 FiberCassetteParser(pNode1);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element loop

  // push DB data vector here
  db[idb].RUI_instance=RUI_instance;
  db[idb].chamber_endcap=chamber_endcap;
  db[idb].chamber_station=chamber_station;
  db[idb].chamber_type=chamber_type;
  db[idb].chamber_number=chamber_number;
  db[idb].vme_id=vme_id;
  db[idb].vme_crate=vme_crate;
  db[idb].vme_slot=vme_slot;
  db[idb].ddu_crate=ddu_crate;
  db[idb].ddu_slot=ddu_slot;
  db[idb].ddu_input=ddu_input;
  db[idb].cassette_crate=cassette_crate;
  db[idb].cassette_pos=cassette_pos;
  db[idb].cassette_socket=cassette_socket;

  idb=idb+1;
  //  std::cout << " increment idb " << idb << std::endl;
}

void ConnectionsDB::PeripheralCrateParser(xercesc::DOMNode *pNode)
{

  // std::cout <<" PeripheralCrateParser" << std::endl;
  parseNode(pNode);

  fillInt("id",vme_id);
  fillInt("VMEcrate",vme_crate);
  fillInt("VMEslot",vme_slot);
}

void ConnectionsDB::ChamberParser(xercesc::DOMNode *pNode)
{
  // std::cout <<"ChamberParser"<<std::endl;
  parseNode(pNode);

  // std::string chamber_endcap;
  fillString("endcap",chamber_endcap);
  fillInt("station",chamber_station);
  fillInt("type",chamber_type);
  fillInt("number",chamber_number);
}

void ConnectionsDB::FiberCassetteParser(xercesc::DOMNode *pNode)
{
  // std::cout << " FiberCassetteParser "<<std::endl;
  parseNode(pNode);

  fillInt("crate",cassette_crate);
  fillInt("pos",cassette_pos);
  fillIntX("socket",cassette_socket);
}

std::string ConnectionsDB::DDUtoChamber(int crate,int slot,int input)
{
  std::string temp="";
  for(unsigned int i=0;i<540;i++){
    if(db[i].ddu_crate==crate&&db[i].ddu_slot==slot&&db[i].ddu_input==input){
        return chamber_str(i);
    }
  }
  return temp;
}

std::string ConnectionsDB::PCratetoChamber(std::string endcap,int station,int crate,int slot)
{
  std::string temp="";
  for(unsigned int i=0;i<540;i++){
    if((db[i].chamber_endcap).compare(endcap)&&db[i].chamber_station==station&&db[i].vme_crate==crate&&db[i].vme_slot==slot){
        return chamber_str(i);
    }
  }
  return temp;
}

std::string ConnectionsDB::chamber_str(int i)
{
      std::ostringstream dum;
      dum<<"ME"<<db[i].chamber_endcap<<db[i].chamber_station<<"/"<<db[i].chamber_type<<"/"<<db[i].chamber_number;
      return dum.str();    
 }

std::string ConnectionsDB::ddu_str(int i)
{
      std::ostringstream dum;
      dum<<"DDU"<<db[i].ddu_crate<<"/" <<db[i].ddu_slot<<"/"<< db[i].ddu_input;     
      return dum.str();
}

std::string ConnectionsDB::cassette_str(int i)
{
      std::ostringstream dum;
      dum<<"FCTC" << db[i].cassette_crate<<"/" << db[i].cassette_pos << "/" << std::hex << db[i].cassette_socket << std::dec;     
      return dum.str();
}

std::string ConnectionsDB::pcrate_str(int i)
{
      std::ostringstream dum;
      dum<<"PC"<<db[i].vme_id<<"/" <<db[i].vme_slot;
      return dum.str();
}


std::string ConnectionsDB::pcrate_name_str(int i)
{
      std::ostringstream dum;
      dum<<"VME"<<db[i].chamber_endcap << db[i].chamber_station<<"/" << db[i].vme_crate;     
      return dum.str();
}

std::string ConnectionsDB::DDUtoDump(std::string ddu)
{
  std::string temp="";
  std::ostringstream dum;
  int size=ddu.size();
  for(unsigned int i=0;i<540;i++){
    if(ddu_str(i).compare(0,size,ddu,0,size)==0){
      dum<< ddu_str(i) << " " << chamber_str(i) <<" " << pcrate_name_str(i) << " " << pcrate_str(i) << " " << cassette_str(i) <<std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::Pcrate_nametoDump(std::string vme)
{
  std::string temp="";
  std::ostringstream dum;
  int size=vme.size();
  for(unsigned int i=0;i<540;i++){
    if(pcrate_name_str(i).compare(0,size,vme,0,size)==0){
      dum<< ddu_str(i) << " " << chamber_str(i) <<" " << pcrate_name_str(i) << " " << pcrate_str(i) << " " << cassette_str(i) << std::endl;
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::CassettetoDump(std::string cassette)
{
  std::string temp="";
  std::ostringstream dum;
  int size=cassette.size();
  for(unsigned int i=0;i<540;i++){
    if(cassette_str(i).compare(0,size,cassette,0,size)==0){
      dum<< ddu_str(i) << " " << chamber_str(i) <<" " << pcrate_name_str(i) << " " << pcrate_str(i) << " " << cassette_str(i) << std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::ChambertoDump(std::string chamber)
{
  std::string temp="";
  std::ostringstream dum;
  int size=chamber.size();
  for(unsigned int i=0;i<540;i++){
    if(chamber_str(i).compare(0,size,chamber,0,size)==0){
      dum<< ddu_str(i) << " " << chamber_str(i) <<" " << pcrate_name_str(i) << " " << pcrate_str(i) << " " << cassette_str(i)<<std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::NametoDump(std::string name)
{
  std::string s;
  s="";
  if(name.compare(0,2,"DD",0,2)==0){
    //std::cout << " found DDU " << std::endl;
    s=DDUtoDump(name);
    return s;
  }else if(name.compare(0,2,"VM",0,2)==0){
    //std::cout << " found VME " << std::endl;
    s=Pcrate_nametoDump(name);
    return s;
  }else if(name.compare(0,2,"FC",0,2)==0){
    //std::cout << " found FCTC- " << std::endl;
    s=CassettetoDump(name);
    return s;
  }else if(name.compare(0,2,"ME",0,2)==0){
    //std::cout << " found ME " << std::endl;
    s=ChambertoDump(name);
    return s;
  } 
  return s;
}


void ConnectionsDB::sparseFile(const std::string name)
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
   std::cout << " enter sConnectionsDB parseFile " << std::endl;

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
    // std::cout << " enter pnode1 loop " << std::endl;
    while (pNode1) {
      if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("data",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
        {
	  sDataParser(pNode1);
	} else 
        {
	  std::cerr << "ConnectionsDB.ERROR: First tag must be RUI-to-chamber_mapping" << std::endl;
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


void ConnectionsDB::sDataParser(xercesc::DOMNode *pNode)
{
  // std::cout << "sDataParser" << std::endl;
  //
  //    xercesc::DOMNode * pNodeGlobal = 0x0;
  //
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("id",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found id" << std::endl;
	 sIDParser(pNode1);
      }
    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element Loop
  
}

void ConnectionsDB::sIDParser(xercesc::DOMNode *pNode)
{
  // std::cout << "sIDParser" << std::endl;
 
  parseNode(pNode);
     
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
    
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //
      if (strcmp("VME",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	//  std::cout << "Found input" << std::endl;
	 sVMEParser(pNode1);
      }
      if (strcmp("FCTC",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 sFCTCParser(pNode1);
      }
       if (strcmp("MAC",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 sMACParser(pNode1);
      }
       if (strcmp("Switch",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
      {
	// std::cout << "Found input" << std::endl;
	 sSwitchParser(pNode1);
      }

    }
    pNode1 = pNode1->getNextSibling();
    //
  } // end of Element loop

  // push DB data vector here
  sdb[isdb].svme=svme;
  sdb[isdb].sfctc=sfctc;
  sdb[isdb].smac=smac;
  sdb[isdb].sswitch=sswitch;
  isdb=isdb+1;  
}

void ConnectionsDB::sVMEParser(xercesc::DOMNode *pNode)
{
  //std::cout << " sVMEParser "<<std::endl;
  parseNode(pNode);
  fillString("vme",svme);
}

void ConnectionsDB::sFCTCParser(xercesc::DOMNode *pNode)
{
  //std::cout << " sFCTCParser "<<std::endl;
  parseNode(pNode);
  fillString("fctc",sfctc);
}

void ConnectionsDB::sMACParser(xercesc::DOMNode *pNode)
{
  // std::cout << " sMACParser "<<std::endl;
  parseNode(pNode);
  fillString("mac",smac);
}

void ConnectionsDB::sSwitchParser(xercesc::DOMNode *pNode)
{
  // std::cout << " sSwitchParser "<<std::endl;
  parseNode(pNode);
  fillString("switch",sswitch);
}

std::string ConnectionsDB::sVMEtoDump(std::string svme)
{
  std::string temp="";
  std::ostringstream dum;
  int size=svme.size();
  for(unsigned int i=0;i<60;i++){
    if(sdb[i].svme.compare(0,size,svme,0,size)==0){
      dum<< sdb[i].smac << " " << sdb[i].sswitch <<" " << sdb[i].svme << " " << sdb[i].sfctc << std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::sFCTCtoDump(std::string sfctc)
{
  std::string temp="";
  std::ostringstream dum;
  int size=sfctc.size();
  for(unsigned int i=0;i<60;i++){
    if(sdb[i].sfctc.compare(0,size,sfctc,0,size)==0){
      dum<< sdb[i].smac << " " << sdb[i].sswitch <<" " << sdb[i].svme << " " << sdb[i].sfctc << std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}

std::string ConnectionsDB::sMACtoDump(std::string smac)
{
  std::string temp="";
  std::ostringstream dum;
  int size=smac.size();
  for(unsigned int i=0;i<60;i++){
    if(sdb[i].smac.compare(0,size,smac,0,size)==0){
      dum<< sdb[i].smac << " " << sdb[i].sswitch <<" " << sdb[i].svme << " " << sdb[i].sfctc << std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}


std::string ConnectionsDB::sSwitchtoDump(std::string sswitch)
{
  std::string temp="";
  std::ostringstream dum;
  int size=sswitch.size();
  for(unsigned int i=0;i<60;i++){
    if(sdb[i].sswitch.compare(0,size,sswitch,0,size)==0){
      dum<< sdb[i].smac << " " << sdb[i].sswitch <<" " << sdb[i].svme << " " << sdb[i].sfctc <<std::endl;     
    }
  }
  if(dum.str().size()>0)return dum.str();
  return temp;
}



std::string ConnectionsDB::sNametoDump(std::string name)
{
  std::string s;
  s="";
  if(name.compare(0,2,"VM",0,2)==0){
    s=sVMEtoDump(name);
    return s;
  }else if(name.compare(0,2,"FC",0,2)==0){
    s=sFCTCtoDump(name);
    return s;
  }else if(name.compare(0,2,"02",0,2)==0){
    s=sMACtoDump(name);
    return s;
  }else if(name.compare(0,2,"SW",0,2)==0){
    s=sSwitchtoDump(name);
    return s;
  } 
  return s;
}
