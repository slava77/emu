#include "EmuTStore.h"
#include "TStoreRequest.h"

#include <time.h>
#include "toolbox/TimeInterval.h"
#include "toolbox/TimeVal.h"
#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/Client.h"
#include "xcept/tools.h"
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/Integer.h"
#include "xdata/Float.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "XMLParser.h"

#include "xercesc/dom/DOMNode.hpp"

#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/LoadDOM.h"
#include "tstore/client/Client.h"


namespace emu {
  namespace pc {

  EmuTStore::EmuTStore(xdaq::Application *s)
{
  std::string HomeDir_ =getenv("HOME");
  thisApp = s;
  xmlfile_    = HomeDir_ + "/vme_config.xml";
  dbUserFile_ = HomeDir_ + "/dbuserfile.txt";

}


xoap::MessageReference EmuTStore::sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception) {
	xoap::MessageReference reply;
	
#ifdef debugV
	std::cout << "Message: " << std::endl;
	message->writeTo(std::cout);
	std::cout << std::endl;
#endif

	try {
		xdaq::ApplicationDescriptor * tstoreDescriptor = thisApp->getApplicationContext()->getDefaultZone()->getApplicationDescriptor("tstore::TStore",0);
	    xdaq::ApplicationDescriptor * tstoretestDescriptor=thisApp->getApplicationDescriptor();
		reply = thisApp->getApplicationContext()->postSOAP(message,*tstoretestDescriptor, *tstoreDescriptor);
	} 
	catch (xdaq::exception::Exception& e) {
		LOG4CPLUS_ERROR(thisApp->getApplicationLogger(),xcept::stdformat_exception_history(e));
	     XCEPT_RETHROW(xcept::Exception, "Could not post SOAP message. ", e);
	}
	
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
		
#ifdef debugV
	std::cout << std::endl << "Response: " << std::endl;
	reply->writeTo(std::cout);
	std::cout << std::endl;
#endif
	
	if (body.hasFault()) {
	  //XCEPT_RAISE (xcept::Exception, body.getFault().getFaultString());
	  XCEPT_RAISE (xcept::Exception, body.getFault().getDetail().getTextContent());
	}
	return reply;
}

std::string EmuTStore::connect() throw (xcept::Exception) {
	TStoreRequest request("connect");
	
	//add the view ID
	request.addTStoreParameter("id","urn:tstore-view-SQL:EMUsystem");
	
	//this parameter is mandatory. "basic" is the only value allowed at the moment
	request.addTStoreParameter("authentication","basic");
	
	//login credentials in format username/password
	getDbUserData();
	request.addTStoreParameter("credentials",dbUserAndPassword_);
	

	//connection will time out after 10 minutes
	toolbox::TimeInterval timeout(600,0); 
	request.addTStoreParameter("timeout",timeout.toString("xs:duration"));
	
	xoap::MessageReference message=request.toSOAP();
	
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the response from the reply
	return tstoreclient::connectionID(response);
}

void EmuTStore::disconnect(const std::string &connectionID) throw (xcept::Exception) {
	TStoreRequest request("disconnect");
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	xoap::MessageReference message=request.toSOAP();
	
	sendSOAPMessage(message);
}

void EmuTStore::queryMaxId(const std::string &connectionID, const std::string &queryViewName, const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception) {
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	TStoreRequest request("query",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	request.addViewSpecificParameter("name",queryViewName);
	
	//add parameter name and value (database table)
	request.addViewSpecificParameter("TABLE",dbTable);
	
	//add parameter name and value (databse column)
	request.addViewSpecificParameter("COLUMN",dbColumn);

	//add parameter name and value (endcap_side)
	request.addViewSpecificParameter("SIDE",endcap_side);


	xoap::MessageReference message=request.toSOAP();
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
}
//

void EmuTStore::query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception) {
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	TStoreRequest request("query",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	//We'll use the "hello" one.
	request.addViewSpecificParameter("name",queryViewName);

	//add parameter name and value (emu_config_id)
	request.addViewSpecificParameter("EMU_CONFIG_ID",emu_config_id);
	
	xoap::MessageReference message=request.toSOAP();
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
}
//

void EmuTStore::query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception) {
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	TStoreRequest request("query",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	//We'll use the "hello" one.
	request.addViewSpecificParameter("name",queryViewName);

	//add parameter name and value (emu_config_id)
	request.addViewSpecificParameter("EMU_CONFIG_ID",emu_config_id);

	//add parameter name and value (xxx_config_id; xxx="periph|ccb|mpc|csc|tmb|daqmb|alct|afeb|cfeb" )
	request.addViewSpecificParameter("XXX_CONFIG_ID",xxx_config_id);

	xoap::MessageReference message=request.toSOAP();
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
}
//

void EmuTStore::getDefinition(const std::string &connectionID, const std::string &insertViewName, xdata::Table &results) throw (xcept::Exception) {
	//the definition message is essentially the same as a query message.
	//instead of retrieving a table full of results, we retrieve an empty table
	//with the appropriate column names and types.

	//we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
	
	TStoreRequest request("definition",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a insert section in the configuration
	//since the definition is used for an insert.
	//We'll use the "test" one.
	request.addViewSpecificParameter("name",insertViewName);
	
	xoap::MessageReference message=request.toSOAP();
	
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
}
//

void EmuTStore::insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows) throw (xcept::Exception) {
	
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	TStoreRequest request("insert",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	//We'll use the "test" one.
	request.addViewSpecificParameter("name",insertViewName);
	
	xoap::MessageReference message=request.toSOAP();
	
	//add our new rows as an attachment to the SOAP message
	//the last parameter is the ID of the attachment. The SQLView does not mind what it is, as there should only be one attachment per message.
	tstoreclient::addAttachment(message,newRows,"test");
	
	xoap::MessageReference response=sendSOAPMessage(message);
	xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  XCEPT_RAISE (xcept::Exception,"An error has occured during inserting data to database!");
	}
}

void EmuTStore::getConfiguration(const std::string &xpath) throw (xcept::Exception) {

  // This is to read the configuration from the view file

  std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
  TStoreRequest request("getConfiguration",viewClass);

  //add the view ID
  request.addTStoreParameter("id","urn:tstore-view-SQL:EMUsystem");

  //add view specific parameter
  request.addTStoreParameter("path", xpath);
  
  xoap::MessageReference message=request.toSOAP();
  xoap::MessageReference response=sendSOAPMessage(message);

  xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault()) {
    XCEPT_RAISE (xcept::Exception,"An error has occured during getConfiguration!");
  }

  DOMNode *configNode=tstoreclient::getNodeNamed(response,"getConfigurationResponse");
  //configNode contains the requested configuration.
  std::cout << "configuration corresponding to xpath " << xpath << " is: " << tstoreclient::writeXML(configNode) << std::endl;

}

// ######################
// #  Misc              #
// ######################

/// Gets database user's name and password from \ref dbUserFile_ .
void EmuTStore::getDbUserData(){

  std::fstream fs;
  fs.open( dbUserFile_.c_str(), std::ios::in );
  if ( fs.is_open() ){
    std::string userAndPasswd;
    fs >> userAndPasswd;
    if ( !userAndPasswd.length() ){
      std::stringstream oss;
      oss << dbUserFile_ << " contains no username/password. ==> No TStore database access.";
      throw oss.str();
    }
    dbUserAndPassword_ = userAndPasswd;
    fs.close();
  }
  else{
    std::stringstream oss;
    oss << "Could not open " << dbUserFile_ << " for reading. ==> No TStore database access.";
    throw oss.str();
  }

}
//

// ########################
// #    Retrieving data   #
// ########################

EmuEndcap * EmuTStore::getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception) {

  EmuEndcap * endcap = new EmuEndcap();
  std::cout << "######## Empty EmuEndcap is created." << std::endl;
  
  std::string connectionID=connect();
  // std::cout << "Liu DEBUG: connectionID " << connectionID << std::endl;
  readConfiguration(connectionID, emu_config_id, endcap);
  std::cout << "######## EmuEndcap is complet." << std::endl;

  disconnect(connectionID);
  
  return endcap;

}
//

void EmuTStore::readConfiguration(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception) {

  std::string queryViewName="configuration";
  xdata::Table results;
  query(connectionID, queryViewName, emu_config_id, results);

  std::cout << "EMU Configuration - START" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      std::string StrgValue=results.getValueAt(rowIndex,*column)->toString();
#ifdef debugV
      std::cout << *column + ": " + StrgValue << std::endl;
#endif
    }
  }

  readPeripheralCrate(connectionID, emu_config_id, endcap);

  std::cout << "EMU Configuration - END" << std::endl;
    
}
//

void EmuTStore::readPeripheralCrate(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception) {

  std::string queryViewName="peripheralcrate";
  std::string periph_config_id;
  xdata::Table results;
  int crateid=0;
  std::string  label;

  query(connectionID, queryViewName, emu_config_id, results);
  
  std::cout << "Peripheral Crate" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0; rowIndex<results.getRowCount(); rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      std::string StrgValue=results.getValueAt(rowIndex,*column)->toString();
      if (*column == "PERIPH_CONFIG_ID"){periph_config_id = StrgValue;}
      if (*column == "CRATEID")
      {  xdata::Serializable  * value = results.getValueAt(rowIndex,*column);
         xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
         crateid = (int)*i;
      }
      if (*column == "LABEL"){label = StrgValue;}
      std::cout << *column + ": " + StrgValue << std::endl;
    }

    VMEController * controller = new VMEController();    
    Crate * crate = new Crate(crateid,controller);
    crate->SetLabel(label);

    endcap->addCrate(crate);
    
    readVCC(connectionID, emu_config_id, periph_config_id, crate);
    readCCB(connectionID, emu_config_id, periph_config_id, crate);
    readMPC(connectionID, emu_config_id, periph_config_id, crate);
    readCSC(connectionID, emu_config_id, periph_config_id, crate);
    std::cout << "#### PERIPHERAL_CRATE:  " << periph_config_id << " --- " << to_string(crateid) << " --- " << label << std::endl;
  }
  std::cout << "######## All Peripheral crates are created." << std::endl;
  
}
//

void EmuTStore::readVCC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

  std::string queryViewName="vcc";
  xdata::Table results;

  xdata::Serializable  * value;
  std::string StrgValue;
  int IntValue;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);
  
  std::cout << "VCC  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    // add VMECC(VMEModule in slot 1) to crate
    VMECC * vcc = new VMECC(theCrate, 1);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "MAC_ADDR"){
	theCrate->vmeController()->SetMAC(0,StrgValue);
	size_t pos=StrgValue.find('-');
	while(pos!= std::string::npos){
	  StrgValue.replace(pos,1,1,':');
	  pos=StrgValue.find('-');
	}
	theCrate->vmeController()->SetVMEAddress(StrgValue);
      }
      if (*column == "ETH_PORT")    {theCrate->vmeController()->SetPort(IntValue);}
      if (*column == "MCAST_1")     {theCrate->vmeController()->SetMAC(1,StrgValue);}
      if (*column == "MCAST_2")     {theCrate->vmeController()->SetMAC(2,StrgValue);}
      if (*column == "MCAST_3")     {theCrate->vmeController()->SetMAC(3,StrgValue);}
      if (*column == "DFLT_SRV_MAC"){theCrate->vmeController()->SetMAC(4,StrgValue);}
      if (*column == "VCC_FRMW_VER"){                      vcc->SetVCC_frmw_ver(StrgValue);}
      if (*column == "ETHERNET_CR") {theCrate->vmeController()->SetCR(0,StrgValue);}
      if (*column == "EXT_FIFO_CR") {theCrate->vmeController()->SetCR(1,StrgValue);}
      if (*column == "RST_MISC_CR") {theCrate->vmeController()->SetCR(2,StrgValue);}
      if (*column == "VME_CR")      {theCrate->vmeController()->SetCR(3,StrgValue);}
      if (*column == "BTO")         {theCrate->vmeController()->SetCR(4,StrgValue);}
      if (*column == "BGTO")        {theCrate->vmeController()->SetCR(5,StrgValue);}
      if (*column == "MSG_LVL")     {theCrate->vmeController()->SetMsg_Lvl(IntValue);}
      if (*column == "WARN_ON_SHTDWN"){
	if(StrgValue == "true"){
	  theCrate->vmeController()->SetWarn_On_Shtdwn(true);
	}else{
	  theCrate->vmeController()->SetWarn_On_Shtdwn(false);
	}
      }
      if (*column == "PKT_ON_STARTUP"){
	if(StrgValue == "true"){
	  theCrate->vmeController()->SetPkt_On_Startup(true);
	}else{
	  theCrate->vmeController()->SetPkt_On_Startup(false);
	}
      }
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    std::cout << "#### VCC:  " << std::endl;
    
  }
  std::cout << "######## VCC is created." << std::endl;
}
//

void EmuTStore::readCSC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

  std::string queryViewName="csc";
  xdata::Table results;

  xdata::Serializable  * value;
  std::string StrgValue;
  int IntValue=0;

  std::string csc_config_id;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);
  
  std::cout << "Chambers  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    Chamber * csc_ = new Chamber(theCrate);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "LABEL")        {csc_->SetLabel(StrgValue);}
      if (*column == "KNOWN_PROBLEM"){csc_->SetProblemDescription(StrgValue);}
      if (*column == "PROBLEM_MASK") {csc_->SetProblemMask(IntValue);}
      if (*column == "CSC_CONFIG_ID"){csc_config_id = StrgValue;}
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    readDAQMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
    readTMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
  }
    
}
//

void EmuTStore::readCCB(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

  std::string queryViewName="ccb";
  xdata::Table results;

  xdata::Serializable  * value;
  std::string StrgValue;
  int IntValue=0;

  int slot=13;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);
  
  std::cout << "CCBs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    CCB * ccb_ = new CCB(theCrate, slot);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "CCBMODE")           {ccb_->SetTTCmode(IntValue);}
      if (*column == "L1ADELAY")          {ccb_->Setl1adelay(IntValue);}
      if (*column == "TTCRXID")           {ccb_->SetTTCrxID(IntValue);}
      if (*column == "TTCRXCOARSEDELAY")  {ccb_->SetTTCrxCoarseDelay(IntValue);}
      if (*column == "TTCRXFINEDELAY")    {ccb_->SetTTCrxFineDelay(IntValue);}
      if (*column == "CCB_FIRMWARE_YEAR") {ccb_->SetExpectedFirmwareYear(IntValue);}
      if (*column == "CCB_FIRMWARE_MONTH"){ccb_->SetExpectedFirmwareMonth(IntValue);}
      if (*column == "CCB_FIRMWARE_DAY")  {ccb_->SetExpectedFirmwareDay(IntValue);}
      std::cout << *column + ": " + StrgValue << std::endl;
    }
  }   
}
//

void EmuTStore::readMPC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

  std::string queryViewName="mpc";
  xdata::Table results;

  xdata::Serializable  * value;
  std::string StrgValue;
  int IntValue=0;

  int slot=12;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);
  
  std::cout << "MPCs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    MPC * mpc_ = new MPC(theCrate, slot);
    mpc_->SetBoardID(theCrate->CrateID());
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "SERIALIZERMODE")    {mpc_->SetTLK2501TxMode(IntValue);}
      if (*column == "TRANSPARENTMODE")   {mpc_->SetTransparentMode(IntValue);}
      if (*column == "MPC_FIRMWARE_YEAR") {mpc_->SetExpectedFirmwareYear(IntValue);}
      if (*column == "MPC_FIRMWARE_MONTH"){mpc_->SetExpectedFirmwareMonth(IntValue);}
      if (*column == "MPC_FIRMWARE_DAY")  {mpc_->SetExpectedFirmwareDay(IntValue);}
      
      std::cout << *column + ": " + StrgValue << std::endl;
    }
  }
    
}
//

void EmuTStore::readDAQMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception) {

  std::string queryViewName="daqmb";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue=0;
  long int LongIntValue;
  float FloatValue=0.0;
  std::string StrgValue;

  int slot = 0;
  std::string daqmb_config_id_;

  query(connectionID, queryViewName, emu_config_id, csc_config_id, results);
  
  std::cout << "DAQMB  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column == "SLOT"){
	value = results.getValueAt(rowIndex,*column);
	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
	slot = (int)*i;
      }
    }
    DAQMB * daqmb_ = new DAQMB(theCrate,theChamber,slot);
    daqmb_->SetCrateId(theCrate->CrateID());
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      //std::cout << *column + "[" + results.getColumnType(*column) + "]" + " ==> " + value->toString() << std::endl;     
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      if (results.getColumnType(*column)=="float")          {xdata::Float * f = dynamic_cast<xdata::Float *>(value); FloatValue=(float)*f;}
      StrgValue=value->toString();

      if (*column == "CALIBRATION_LCT_DELAY")  {daqmb_->SetCalibrationLctDelay(IntValue);}
      if (*column == "CALIBRATION_L1ACC_DELAY"){daqmb_->SetCalibrationL1aDelay(IntValue);}
      if (*column == "PULSE_DELAY")            {daqmb_->SetPulseDelay(IntValue);}
      if (*column == "INJECT_DELAY")           {daqmb_->SetInjectDelay(IntValue);}
      if (*column == "KILL_INPUT")             {daqmb_->SetKillInput(IntValue);}
      if (*column == "PUL_DAC_SET")            {daqmb_->SetPulseDac(FloatValue);}
      if (*column == "INJ_DAC_SET")            {daqmb_->SetInjectorDac(FloatValue);}
      if (*column == "SET_COMP_THRESH"){
	daqmb_->SetCompThresh(FloatValue);
	for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompThresholdsCfeb(cfeb,FloatValue);
      }
      if (*column == "COMP_MODE"){
	daqmb_->SetCompMode(IntValue);
	for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompModeCfeb(cfeb,IntValue);
      }
      if (*column == "COMP_TIMING"){
	daqmb_->SetCompTiming(IntValue);
	for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompTimingCfeb(cfeb,IntValue);
      }
      if (*column == "PRE_BLOCK_END"){
	daqmb_->SetPreBlockEnd(IntValue);
	for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetPreBlockEndCfeb(cfeb,IntValue);
      }
      if (*column == "CFEB_CABLE_DELAY"){daqmb_->SetCfebCableDelay(IntValue);}
      if (*column == "TMB_LCT_CABLE_DELAY"){daqmb_->SetTmbLctCableDelay(IntValue);}
      if (*column == "CFEB_DAV_CABLE_DELAY"){daqmb_->SetCfebDavCableDelay(IntValue);}
      if (*column == "ALCT_DAV_CABLE_DELAY"){daqmb_->SetAlctDavCableDelay(IntValue);}
      if (*column == "FEB_CLOCK_DELAY"){daqmb_->SetCfebClkDelay(IntValue);}
      if (*column == "XLATENCY"){
	daqmb_->SetxLatency(IntValue);
	for (int cfeb=0; cfeb<5; cfeb++) daqmb_->SetL1aExtraCfeb(cfeb,IntValue);
      }
      if (*column == "XFINELATENCY"){daqmb_->SetxFineLatency(IntValue);}
      if (*column == "DMB_CNTL_FIRMWARE_TAG"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%lx", &LongIntValue);
	daqmb_->SetExpectedControlFirmwareTag(LongIntValue);
      } 
      if (*column == "DMB_VME_FIRMWARE_TAG"){daqmb_->SetExpectedVMEFirmwareTag(IntValue);}
      if (*column == "DAQMB_CONFIG_ID"){daqmb_config_id_ = StrgValue;}
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    readCFEB(connectionID, emu_config_id, daqmb_config_id_, daqmb_);
  }
}
//

void EmuTStore::readCFEB(const std::string &connectionID, const std::string &emu_config_id, const std::string &daqmb_config_id, DAQMB * theDaqmb) throw (xcept::Exception) {

  std::string queryViewName="cfeb";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue;
  long int LongIntValue;
  std::string StrgValue;

  int number=0;   
  int kill_chip[6]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
  
  query(connectionID, queryViewName, emu_config_id, daqmb_config_id, results);
  
  std::cout << "CFEBs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      if (*column == "CFEB_NUMBER"){number = IntValue;}
    }
    std::cout << "CFEB cfeb( " + to_string(number) << " )" << std::endl;
    CFEB cfeb(number);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();
      if (*column == "CFEB_FIRMWARE_TAG"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%lx", &LongIntValue);
	theDaqmb->SetExpectedCFEBFirmwareTag(number, LongIntValue);
      }
      if (*column == "KILL_CHIP0"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[0] = IntValue;
      }
      if (*column == "KILL_CHIP1"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[1] = IntValue;
      }
      if (*column == "KILL_CHIP2"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[2] = IntValue;
      }
      if (*column == "KILL_CHIP3"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[3] = IntValue;
      }
      if (*column == "KILL_CHIP4"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[4] = IntValue;
      }
      if (*column == "KILL_CHIP5"){                                      // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	kill_chip[5] = IntValue;
      }
      
      for(int chip=0;chip<6;chip++){
	theDaqmb->SetKillChip(number,chip,kill_chip[chip]);
	for(int chan=0;chan<16;chan++){
	  unsigned short int mask=(1<<chan);
	  if((mask&kill_chip[chip])==0x0000){
	    theDaqmb->SetShiftArray(number,5-chip,chan,NORM_RUN);
	  }else{
	    theDaqmb->SetShiftArray(number,5-chip,chan,KILL_CHAN);
	  }
	}
      }      
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    //
    theDaqmb->cfebs_.push_back(cfeb);
  }

}
//


void EmuTStore::readTMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception) {

  std::string queryViewName="tmb";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue;
  long long int LongLongIntValue;
  float FloatValue;
  std::string StrgValue;
  int slot = 0;
  std::string tmb_config_id_;
  
  query(connectionID, queryViewName, emu_config_id, csc_config_id, results);
  
  std::cout << "TMB  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column == "SLOT"){
	value = results.getValueAt(rowIndex,*column);
	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
	slot = (int)*i;
      }
    }
    TMB * tmb_ = new TMB(theCrate, theChamber, slot);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int"  ) {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      if (results.getColumnType(*column)=="float"           ) {xdata::Float * f = dynamic_cast<xdata::Float *>(value); FloatValue=(float)*f;}
      StrgValue=value->toString();
      
      if (*column == "TMB_FIRMWARE_MONTH"           ) {tmb_->SetExpectedTmbFirmwareMonth(IntValue);  }
      if (*column == "TMB_FIRMWARE_DAY"             ) {tmb_->SetExpectedTmbFirmwareDay(IntValue);    }
      if (*column == "TMB_FIRMWARE_YEAR"            ) {tmb_->SetExpectedTmbFirmwareYear(IntValue);   }
      if (*column == "TMB_FIRMWARE_VERSION"         ) {tmb_->SetExpectedTmbFirmwareVersion(IntValue);}
      if (*column == "TMB_FIRMWARE_REVCODE"         ) {tmb_->SetExpectedTmbFirmwareRevcode(IntValue);}
      if (*column == "TMB_FIRMWARE_TYPE"            ) {tmb_->SetExpectedTmbFirmwareType(IntValue);   }
      if (*column == "TMB_FIRMWARE_COMPILE_TYPE") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%x", &IntValue);
	tmb_->SetTMBFirmwareCompileType(IntValue);
      }
      if (*column == "RAT_FIRMWARE_MONTH"           ) {tmb_->SetExpectedRatFirmwareMonth(IntValue);  }
      if (*column == "RAT_FIRMWARE_DAY"             ) {tmb_->SetExpectedRatFirmwareDay(IntValue);    }
      if (*column == "RAT_FIRMWARE_YEAR"            ) {tmb_->SetExpectedRatFirmwareYear(IntValue);   }
      if (*column == "ENABLE_ALCT_TX"               ) {tmb_->SetEnableAlctTx(IntValue);              }
      if (*column == "ENABLE_ALCT_RX"               ) {tmb_->SetAlctInput(IntValue);                 }
      if (*column == "ALCT_CLEAR"                   ) {tmb_->SetAlctClear(IntValue);                 }
      if (*column == "ENABLECLCTINPUTS_REG42"       ) {tmb_->SetEnableCLCTInputs(IntValue);          }
      if (*column == "RPC_EXISTS"                   ) {tmb_->SetRpcExist(IntValue);                  }
      if (*column == "RPC_READ_ENABLE"              ) {tmb_->SetRpcReadEnable(IntValue);             }
      if (*column == "RPC_BXN_OFFSET"               ) {tmb_->SetRpcBxnOffset(IntValue);              }
      if (*column == "RPC_MASK_ALL"                 ) {tmb_->SetEnableRpcInput(IntValue);            }
      if (*column == "MATCH_PRETRIG_WINDOW_SIZE"    ) {tmb_->SetAlctClctPretrigWidth(IntValue);      }
      if (*column == "MATCH_PRETRIG_ALCT_DELAY"     ) {tmb_->SetAlctPretrigDelay(IntValue);          }
      //      if (*column == "LAYER_TRIG_DELAY"             ) {tmb_->SetLayerTrigDelay(IntValue);            } // obsolete
      if (*column == "TMB_L1A_DELAY"                ) {tmb_->SetL1aDelay(IntValue);                  }
      if (*column == "TMB_L1A_WINDOW_SIZE"          ) {tmb_->SetL1aWindowSize(IntValue);             }
      if (*column == "TMB_L1A_OFFSET"               ) {tmb_->SetL1aOffset(IntValue);                 }
      if (*column == "TMB_BXN_OFFSET"               ) {tmb_->SetBxnOffset(IntValue);                 }
      if (*column == "MATCH_TRIG_ALCT_DELAY"        ) {tmb_->SetAlctVpfDelay(IntValue);              }
      if (*column == "MATCH_TRIG_WINDOW_SIZE"       ) {tmb_->SetAlctMatchWindowSize(IntValue);       }
      if (*column == "MPC_TX_DELAY"                 ) {tmb_->SetMpcTxDelay(IntValue);                }
      if (*column == "RPC0_RAW_DELAY"               ) {tmb_->SetRpc0RawDelay(IntValue);              }
      if (*column == "RPC1_RAW_DELAY"               ) {tmb_->SetRpc1RawDelay(IntValue);              }
      if (*column == "CLCT_PRETRIG_ENABLE"          ) {tmb_->SetClctPatternTrigEnable(IntValue);     }
      if (*column == "ALCT_PRETRIG_ENABLE"          ) {tmb_->SetAlctPatternTrigEnable(IntValue);     }
      if (*column == "MATCH_PRETRIG_ENABLE"         ) {tmb_->SetMatchPatternTrigEnable(IntValue);    }
      if (*column == "CLCT_EXT_PRETRIG_ENABLE"      ) {tmb_->SetClctExtTrigEnable(IntValue);         }
      if (*column == "ALL_CFEB_ACTIVE"              ) {tmb_->SetEnableAllCfebsActive(IntValue);      }
      if (*column == "ENABLECLCTINPUTS_REG68"       ) {tmb_->SetCfebEnable(IntValue);                }
      if (*column == "CFEB_ENABLE_SOURCE"           ) {tmb_->Set_cfeb_enable_source(IntValue);       }
      if (*column == "CLCT_HIT_PERSIST"             ) {tmb_->SetTriadPersistence(IntValue);          }
      if (*column == "CLCT_NPLANES_HIT_PRETRIG"     ) {tmb_->SetHsPretrigThresh(IntValue);           }
      if (*column == "CLCT_NPLANES_HIT_PATTERN"     ) {tmb_->SetMinHitsPattern(IntValue);            }
      if (*column == "CLCT_DRIFT_DELAY"             ) {tmb_->SetDriftDelay(IntValue);                }
      if (*column == "TMB_FIFO_MODE"                ) {tmb_->SetFifoMode(IntValue);                  }
      if (*column == "CLCT_FIFO_TBINS"              ) {tmb_->SetFifoTbins(IntValue);                 }
      if (*column == "CLCT_FIFO_PRETRIG"            ) {tmb_->SetFifoPreTrig(IntValue);               }
      if (*column == "MPC_SYNC_ERR_ENABLE"          ) {tmb_->SetTmbSyncErrEnable(IntValue);          }
      if (*column == "ALCT_TRIG_ENABLE"             ) {tmb_->SetTmbAllowAlct(IntValue);              }
      if (*column == "CLCT_TRIG_ENABLE"             ) {tmb_->SetTmbAllowClct(IntValue);              }
      if (*column == "MATCH_TRIG_ENABLE"            ) {tmb_->SetTmbAllowMatch(IntValue);             }
      if (*column == "ALCT_READOUT_WITHOUT_TRIG"    ) {tmb_->SetAllowAlctNontrigReadout(IntValue);   }
      if (*column == "CLCT_READOUT_WITHOUT_TRIG"    ) {tmb_->SetAllowClctNontrigReadout(IntValue);   }
      if (*column == "MATCH_READOUT_WITHOUT_TRIG"   ) {tmb_->SetAllowMatchNontrigReadout(IntValue);  }
      if (*column == "MPC_RX_DELAY"                 ) {tmb_->SetMpcRxDelay(IntValue);                }
      if (*column == "MPC_IDLE_BLANK"               ) {tmb_->SetMpcIdleBlank(IntValue);              }
      if (*column == "MPC_OUTPUT_ENABLE"            ) {tmb_->SetMpcOutputEnable(IntValue);           }
      if (*column == "MPC_BLOCK_ME1A"               ) {tmb_->SetBlockME1aToMPC(IntValue);            }
      if (*column == "WRITE_BUFFER_REQUIRED"        ) {tmb_->SetWriteBufferRequired(IntValue);       }
      if (*column == "VALID_CLCT_REQUIRED"          ) {tmb_->SetRequireValidClct(IntValue);          }
      if (*column == "L1A_ALLOW_MATCH"              ) {tmb_->SetL1aAllowMatch(IntValue);             }
      if (*column == "L1A_ALLOW_NOTMB"              ) {tmb_->SetL1aAllowNoTmb(IntValue);             }
      if (*column == "L1A_ALLOW_NOL1A"              ) {tmb_->SetL1aAllowNoL1a(IntValue);             }
      if (*column == "L1A_ALLOW_ALCT_ONLY"          ) {tmb_->SetL1aAllowAlctOnly(IntValue);          }
      if (*column == "LAYER_TRIG_ENABLE"            ) {tmb_->SetEnableLayerTrigger(IntValue);        }
      if (*column == "LAYER_TRIG_THRESH"            ) {tmb_->SetLayerTriggerThreshold(IntValue);     }
      if (*column == "CLCT_BLANKING"                ) {tmb_->SetClctBlanking(IntValue);              }
      //      if (*column == "CLCT_STAGGER"                 ) {tmb_->SetClctStagger(IntValue);               }  // obsolete
      if (*column == "CLCT_PID_THRESH_PRETRIG"      ) {tmb_->SetClctPatternIdThresh(IntValue);       }
      if (*column == "AFF_THRESH"                   ) {tmb_->SetActiveFebFlagThresh(IntValue);       }
      if (*column == "CLCT_MIN_SEPARATION"          ) {tmb_->SetMinClctSeparation(IntValue);         }
      if (*column == "LAYER0_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(0, LongLongIntValue);
      }
      if (*column == "LAYER1_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(1, LongLongIntValue);
      }
      if (*column == "LAYER2_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(2, LongLongIntValue);
      }
      if (*column == "LAYER3_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(3, LongLongIntValue);
      }
      if (*column == "LAYER4_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(4, LongLongIntValue);
      }
      if (*column == "LAYER5_DISTRIP_HOT_CHANN_MASK") {                   // saved as string in Oracle since there is no Hex support
	sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
	tmb_->SetDistripHotChannelMask(5, LongLongIntValue);
      }
      if (*column == "REQUEST_L1A"                  ) {tmb_->SetRequestL1a(IntValue);                }
      if (*column == "IGNORE_CCB_STARTSTOP"         ) {tmb_->SetIgnoreCcbStartStop(IntValue);        }
      if (*column == "ALCT_CLOCK_EN_USE_CCB"        ) {tmb_->SetEnableAlctUseCcbClock(IntValue);     }
      if (*column == "ALCT_TX_CLOCK_DELAY"          ) {tmb_->SetAlctTXclockDelay(IntValue);          }
      if (*column == "ALCT_RX_CLOCK_DELAY"          ) {tmb_->SetAlctRXclockDelay(IntValue);          }
      if (*column == "DMB_TX_DELAY"                 ) {tmb_->SetDmbTxDelay(IntValue);                }
      if (*column == "RAT_TMB_DELAY"                ) {tmb_->SetRatTmbDelay(IntValue);               }
      if (*column == "CFEB0DELAY"                   ) {tmb_->SetCFEB0delay(IntValue);                }
      if (*column == "CFEB1DELAY"                   ) {tmb_->SetCFEB1delay(IntValue);                }
      if (*column == "CFEB2DELAY"                   ) {tmb_->SetCFEB2delay(IntValue);                }
      if (*column == "CFEB3DELAY"                   ) {tmb_->SetCFEB3delay(IntValue);                }
      if (*column == "CFEB4DELAY"                   ) {tmb_->SetCFEB4delay(IntValue);                }
      if (*column == "RPC0_RAT_DELAY"               ) {tmb_->SetRpc0RatDelay(IntValue);              }
      if (*column == "RPC1_RAT_DELAY"               ) {tmb_->SetRpc1RatDelay(IntValue);              }
      if (*column == "ADJACENT_CFEB_DISTANCE"       ) {tmb_->SetAdjacentCfebDistance(IntValue);      }
      if (*column == "ALCT_BX0_DELAY"               ) {tmb_->SetAlctBx0Delay(IntValue);              }
      if (*column == "ALCT_BX0_ENABLE"              ) {tmb_->SetAlctBx0Enable(IntValue);             }
      if (*column == "CLCT_BX0_DELAY"               ) {tmb_->SetClctBx0Delay(IntValue);              }
      if (*column == "CLCT_THROTTLE"                ) {tmb_->SetClctThrottle(IntValue);              }
      if (*column == "RPC_FIFO_DECOUPLE"            ) {tmb_->SetRpcDecoupleTbins(IntValue);          }
      if (*column == "RPC_FIFO_PRETRIG"             ) {tmb_->SetFifoPretrigRpc(IntValue);            }
      if (*column == "RPC_FIFO_TBINS"               ) {tmb_->SetFifoTbinsRpc(IntValue);              }
      if (*column == "WRITE_BUFFER_AUTOCLEAR"       ) {tmb_->SetWriteBufferAutoclear(IntValue);      }
      if (*column == "WRITE_BUFFER_CONTINOUS_ENABLE") {tmb_->SetClctWriteContinuousEnable(IntValue); }
      if (*column == "TMB_FIFO_NO_RAW_HITS"         ) {tmb_->SetFifoNoRawHits(IntValue);             }
      if (*column == "TMB_CONFIG_ID"                ) {tmb_config_id_ = StrgValue;                   }

      std::cout << *column + ": " + StrgValue << std::endl;
    }
    readALCT(connectionID, emu_config_id, tmb_config_id_, tmb_);
  }
}
//

void EmuTStore::readALCT(const std::string &connectionID, const std::string &emu_config_id, const std::string &tmb_config_id, TMB * theTmb) throw (xcept::Exception) {

  std::string queryViewName="alct";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue=0;
  std::string StrgValue;
  std::string chamberType;
  std::string alct_config_id_;

  query(connectionID, queryViewName, emu_config_id, tmb_config_id, results);
  
  std::cout << "ALCT  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column == "CHAMBER_TYPE") {chamberType=results.getValueAt(rowIndex,*column)->toString();}
    }
    ALCTController * alct_ = new ALCTController(theTmb, chamberType);
    RAT * rat_ = new RAT(theTmb);
    theTmb->SetAlct(alct_); //store alct_ pointer in theTmb
    theTmb->SetRat(rat_); //store rat_  in theTmb
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int"  ) {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      if (results.getColumnType(*column)=="string"          ) {StrgValue=value->toString();}
      StrgValue=value->toString();
      
      if (*column == "ALCT_FIRMWARE_DAY"             ) {alct_->SetExpectedFastControlDay(IntValue);               }
      if (*column == "ALCT_FIRMWARE_MONTH"           ) {alct_->SetExpectedFastControlMonth(IntValue);             }
      if (*column == "ALCT_FIRMWARE_YEAR"            ) {alct_->SetExpectedFastControlYear(IntValue);              }
      if (*column == "ALCT_FIRMWARE_BACKWARD_FORWARD") {alct_->Set_fastcontrol_backward_forward_type(StrgValue);  }
      if (*column == "ALCT_FIRMWARE_NEGAT_POSIT"     ) {alct_->Set_fastcontrol_negative_positive_type(StrgValue); }
      if (*column == "ALCT_TRIG_MODE"                ) {alct_->SetTriggerMode(IntValue);                          }
      if (*column == "ALCT_EXT_TRIG_ENABLE"          ) {alct_->SetExtTrigEnable(IntValue);                        }
      if (*column == "ALCT_SEND_EMPTY"               ) {alct_->SetSendEmpty(IntValue);                            }
      if (*column == "ALCT_INJECT_MODE"              ) {alct_->SetInjectMode(IntValue);                           }
      if (*column == "ALCT_BXN_OFFSET"               ) {alct_->SetBxcOffset(IntValue);                            }
      if (*column == "ALCT_NPLANES_HIT_ACCEL_PRETRIG") {alct_->SetAcceleratorPretrigThresh(IntValue);             }
      if (*column == "ALCT_NPLANES_HIT_ACCEL_PATTERN") {alct_->SetAcceleratorPatternThresh(IntValue);             }
      if (*column == "ALCT_NPLANES_HIT_PATTERN"      ) {alct_->SetPretrigNumberOfPattern(IntValue);               }
      if (*column == "ALCT_NPLANES_HIT_PRETRIG"      ) {alct_->SetPretrigNumberOfLayers(IntValue);                }
      if (*column == "ALCT_DRIFT_DELAY"              ) {alct_->SetDriftDelay(IntValue);                           }
      if (*column == "ALCT_FIFO_TBINS"               ) {alct_->SetFifoTbins(IntValue);                            }
      if (*column == "ALCT_FIFO_PRETRIG"             ) {alct_->SetFifoPretrig(IntValue);                          }
      if (*column == "ALCT_FIFO_MODE"                ) {alct_->SetFifoMode(IntValue);                             }
      if (*column == "ALCT_L1A_DELAY"                ) {alct_->SetL1aDelay(IntValue);                             }
      if (*column == "ALCT_L1A_WINDOW_WIDTH"         ) {alct_->SetL1aWindowSize(IntValue);                        }
      if (*column == "ALCT_L1A_OFFSET"               ) {alct_->SetL1aOffset(IntValue);                            }
      if (*column == "ALCT_L1A_INTERNAL"             ) {alct_->SetL1aInternal(IntValue);                          }
      if (*column == "ALCT_CCB_ENABLE"               ) {alct_->SetCcbEnable(IntValue);                            }
      if (*column == "ALCT_CONFIG_IN_READOUT"        ) {alct_->SetConfigInReadout(IntValue);                      }
      if (*column == "ALCT_ACCEL_MODE"               ) {alct_->SetAlctAmode(IntValue);                            }
      if (*column == "ALCT_TRIG_INFO_EN"             ) {alct_->SetTriggerInfoEnable(IntValue);                    }
      if (*column == "ALCT_SN_SELECT"                ) {alct_->SetSnSelect(IntValue);                             }
      if (*column == "ALCT_TESTPULSE_AMPLITUDE"      ) {alct_->SetTestpulseAmplitude(IntValue);                   }
      if (*column == "ALCT_TESTPULSE_INVERT"         ) {alct_->Set_InvertPulse(StrgValue);                        }
      if (*column == "ALCT_TESTPULSE_DIRECTION"      ) {alct_->Set_PulseDirection(StrgValue);                     }
      if (*column == "ALCT_CONFIG_ID"                ) {alct_config_id_ = StrgValue;                              }
      
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    readAnodeChannel(connectionID, emu_config_id, alct_config_id_, alct_);
  }
}
//

void EmuTStore::readAnodeChannel(const std::string &connectionID, const std::string &emu_config_id, const std::string &alct_config_id, ALCTController * theAlct) throw (xcept::Exception) {

  std::string queryViewName="anodechannel";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue=0;
  std::string StrgValue;
  int afeb_number=0;

  query(connectionID, queryViewName, emu_config_id, alct_config_id, results);

  std::cout << "ANODECHANNEL  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column == "AFEB_NUMBER") {
	value = results.getValueAt(rowIndex,*column);
	std::cout << *column + ": " + value->toString() << std::endl;
	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
	afeb_number = (int)*i;
      }
    }
    theAlct->SetStandbyRegister_(afeb_number-1,ON);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int"  ) {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); IntValue=(int)*i;}
      StrgValue=value->toString();
      
      if (*column == "AFEB_FINE_DELAY") {theAlct->SetAsicDelay(afeb_number-1,IntValue);    }
      if (*column == "AFEB_THRESHOLD")  {theAlct->SetAfebThreshold(afeb_number-1,IntValue);}
      
      std::cout << *column + ": " + StrgValue << std::endl;
    }   
  }
}
//
  } // namespace emu::pc
  } // namespace emu
