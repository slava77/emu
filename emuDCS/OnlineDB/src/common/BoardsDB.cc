/**********************************
       S. Durkin
       Feb. 7, 2008

A quickly written set of routines to 
parse and use xml board data bases.

***********************************/


#include "BoardsDB.h"

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

BoardsDB::BoardsDB(): pAttributes_(0)
{
  HOMEDIR_=getenv("HOME");
  HOMEDIR_+="/config/db/";
  std::cout << "Home Directory:" << HOMEDIR_ <<std::endl;

  iPCdb=0;
  PCBoardsDBXMLFile_=HOMEDIR_+"PCBoards.xml";
  parseFile(0,PCBoardsDBXMLFile_);

  iCFEBdb=0;
  CFEBBoardsDBXMLFile_=HOMEDIR_+"CFEBBoards.xml";
  parseFile(1,CFEBBoardsDBXMLFile_);

  iCSCIDdb=0;
  CSC2ChamberDBXMLFile_=HOMEDIR_+"CSCIDtoChamber.xml";
  parseFile(2,CSC2ChamberDBXMLFile_);

  FillCFEBDB();

}

BoardsDB::~BoardsDB()
{
  // std::cout << " kill BoardsDB " << std::endl;
}

void BoardsDB::parseNode(xercesc::DOMNode * pNode) 
{
  pAttributes_ = pNode->getAttributes();

#ifdef debugV
  std::cout << "   number of attributes = " << pAttributes_->getLength() << std::endl;
#endif
}

bool BoardsDB::fillInt(std::string item, int & target) 
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

bool BoardsDB::fillIntX(std::string item, int & target) 
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

bool BoardsDB::fillLongLongIntX(std::string item, long long int & target) 
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


bool BoardsDB::fillString(std::string item, std::string & target) 
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


bool BoardsDB::fillFloat(std::string item, float & target) 
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



void BoardsDB::parseFile(int type,const std::string name)
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
  // std::cout << " enter BoardsDB parseFile " << std::endl;

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
	if (strcmp("emu",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
        {
	  // std::cout << " found emu " << std::endl;
	  MainParser(type,pNode1);
	} else 
        {
	  std::cerr << "BoardsDB.ERROR: First tag must be emu" << std::endl;
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

void BoardsDB::MainParser(int type,xercesc::DOMNode *pNode)
{
  // std::cout << "MainParser" << std::endl;                                                                        
  //                                                                                                               
  //    xercesc::DOMNode * pNodeGlobal = 0x0;                                                                      
  //                                                                                                               
  xercesc::DOMNode * pNode1 = pNode->getFirstChild();
  while (pNode1) {
    if (pNode1->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      //                                                                                                           
      if (strcmp("chamber",xercesc::XMLString::transcode(pNode1->getNodeName()))==0)
	{
	  //std::cout << "Found chamber" << std::endl;           
	  FillParser(type,pNode1);
	}
    }
    pNode1 = pNode1->getNextSibling();
      //                                                                                                             
    } // end of Element Loop                                                                                         

  }


void BoardsDB::FillParser(int type,xercesc::DOMNode *pNode)
{  
  //std::cout << "FillParser" << type << std::endl;
 
  parseNode(pNode);

  if(type==0){
    int slot;
    std::string crate,type,board;
    fillString("crate",crate);
    fillInt("slot",slot);
    fillString("type",type);
    fillString("board",board);
    pcDB[iPCdb].crate=crate;
    pcDB[iPCdb].slot=slot;
    pcDB[iPCdb].type=type;
    pcDB[iPCdb].board=board;
    iPCdb=iPCdb+1;
  }

  if(type==1){
    int cfebID,cscid,cscnumber;
    std::string csctype;
    fillInt("cfebID",cfebID);
    fillString("type",csctype);
    fillInt("id",cscid);
    fillInt("number",cscnumber);
    t_cfebdb[iCFEBdb].cfebID=cfebID;
    t_cfebdb[iCFEBdb].csctype=csctype;
    t_cfebdb[iCFEBdb].cscid=cscid;
    t_cfebdb[iCFEBdb].cscnumber=cscnumber;
    iCFEBdb=iCFEBdb+1;
  }
  if(type==2){
    int cscid;
    std::string csctype,chamber;
    fillString("type",csctype);
    fillInt("id",cscid);
    fillString("chamber",chamber);
    t_csciddb[iCSCIDdb].csctype=csctype;
    t_csciddb[iCSCIDdb].cscid=cscid;
    t_csciddb[iCSCIDdb].chamber=chamber;
    iCSCIDdb=iCSCIDdb+1;
  }
}

void BoardsDB::FillCFEBDB()
{
  for(unsigned int i=0;i<(unsigned int)iCFEBdb;i++){
     for(unsigned int j=0;j<(unsigned int)iCSCIDdb;j++){
       if((t_cfebdb[i].csctype).compare(t_csciddb[j].csctype)==0&&t_cfebdb[i].cscid==t_csciddb[j].cscid){
         cfebDB[i].chamber=t_csciddb[j].chamber;
         cfebDB[i].cfeb=t_cfebdb[i].cscnumber;
         cfebDB[i].cfebID=t_cfebdb[i].cfebID;
       }
     }
  }
}



int BoardsDB::CrateToDMBID(std::string crate,int slot)
{
  std::string type="F";
  std::string name;
  for(unsigned int i=0;i<(unsigned int)iPCdb;i++){
    if(crate.compare(pcDB[i].crate)==0&&slot==pcDB[i].slot&&type==pcDB[i].type){
      name=pcDB[i].board;
    }
  }
  int dmbid=9999;
  if(name.substr(0,3)=="DMB"){
    sscanf(name.substr(3,4).c_str(),"%d",&dmbid);
  }
  return dmbid;
}

int BoardsDB::ChamberToCFEBID(std::string chamber,int cfeb){
  int cfebID=9999;
  for(unsigned int i=0;i<(unsigned int)iCFEBdb;i++){
    if(chamber.compare(cfebDB[i].chamber)==0&&cfeb==cfebDB[i].cfeb){
      cfebID= cfebDB[i].cfebID;
      return cfebID;
    }
  }
  return cfebID;
} 
