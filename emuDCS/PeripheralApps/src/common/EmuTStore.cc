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


  EmuTStore::EmuTStore(xdaq::Application *s)
{
  std::string HomeDir_ =getenv("HOME");
  thisApp = s;
  xmlfile_    = HomeDir_ + "/vme_config.xml";
  dbUserFile_ = HomeDir_ + "/dbuserfile.txt";

}

// ################################
// #   TStore related functions   #
// ################################

xdata::UnsignedInteger64 EmuTStore::getConfigId(const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side) throw (xcept::Exception) {
  
  std::string connectionID=connect();

  std::string queryViewName="getconfigids";
  xdata::Table results;
  xdata::UnsignedInteger64 value(0);
  queryMaxId(connectionID,queryViewName,dbTable,dbColumn,endcap_side,results);

  std::vector<std::string> columns=results.getColumns();
  for (unsigned long rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column==dbColumn){
	value.fromString(results.getValueAt(rowIndex,*column)->toString());
      }
    }
  }  
     
  return value;
  
  disconnect(connectionID);
  
}
//


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

void EmuTStore::synchronize(const std::string &connectionID, const std::string &syncMode, const std::string &syncPattern) throw (xcept::Exception) {
  std::string viewClass=tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");
  TStoreRequest request("sync",viewClass);
  request.addTStoreParameter("connectionID",connectionID);
  request.addTStoreParameter("mode", syncMode);
  request.addTStoreParameter("pattern", syncPattern);

  xoap::MessageReference message=request.toSOAP();
  xoap::MessageReference response=sendSOAPMessage(message);

  xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault()) {
    XCEPT_RAISE (xcept::Exception,"An error has occured during synchronization!");
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

  fstream fs;
  fs.open( dbUserFile_.c_str(), ios::in );
  if ( fs.is_open() ){
    string userAndPasswd;
    fs >> userAndPasswd;
    if ( !userAndPasswd.length() ){
      stringstream oss;
      oss << dbUserFile_ << " contains no username/password. ==> No TStore database access.";
      throw oss.str();
    }
    dbUserAndPassword_ = userAndPasswd;
    fs.close();
  }
  else{
    stringstream oss;
    oss << "Could not open " << dbUserFile_ << " for reading. ==> No TStore database access.";
    throw oss.str();
  }

}
//


// ######################
// #   Uploading data   #
// ######################

void EmuTStore::startUpload() throw (xcept::Exception) {

  // Processing DOM Tree
  if ( TStore_myEndcap_ ) {
    std::cout << "<br>Found EmuEndcap<br>" << std::endl;
  } else {
    std::cout << "<br>EmuEndcap is not loaded into memory. First run ParseXML!<br>" << std::endl;
    return;
  }
    
  // get peripheral crate data from DOM tree
  std::vector<Crate *> myCrates;
  myCrates.clear();
  myCrates = TStore_myEndcap_->AllCrates();

  std::string connectionID=connect();

  // get table definitions
  getDefinition(connectionID,"configuration",tableDefinition_emu_configuration);
  getDefinition(connectionID,"peripheralcrate",tableDefinition_emu_peripheralcrate);
  getDefinition(connectionID,"ccb",tableDefinition_emu_ccb);
  getDefinition(connectionID,"mpc",tableDefinition_emu_mpc);
  getDefinition(connectionID,"vcc",tableDefinition_emu_vcc);
  getDefinition(connectionID,"csc",tableDefinition_emu_csc);
  getDefinition(connectionID,"daqmb",tableDefinition_emu_daqmb);
  getDefinition(connectionID,"cfeb",tableDefinition_emu_cfeb);
  getDefinition(connectionID,"tmb",tableDefinition_emu_tmb);
  getDefinition(connectionID,"alct",tableDefinition_emu_alct);
  getDefinition(connectionID,"anodechannel",tableDefinition_emu_anodechannel);

  uploadConfiguration(connectionID, "plus");
  uploadPeripheralCrate(connectionID, myCrates);

  disconnect(connectionID);
  
}
//

void EmuTStore::uploadConfiguration(const std::string &connectionID, const std::string endcap_side) throw (xcept::Exception) {

  std::string insertViewName="configuration";
  xdata::Table newRows;
  xdata::UnsignedInteger64 emu_config_id;
  toolbox::TimeVal currentTime;

  size_t rowId(0);
  std::string DESCRIPTION("DESCRIPTION");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string EMU_CONFIG_TIME("EMU_CONFIG_TIME");
  std::string EMU_CONFIG_TYPE("EMU_CONFIG_TYPE");
  std::string EMU_ENDCAP_SIDE("EMU_ENDCAP_SIDE");

  emu_config_id_ = getConfigId("EMU_CONFIGURATION", "EMU_CONFIG_ID", endcap_side);
  if(emu_config_id.value_==0) emu_config_id=((endcap_side=="plus")?1000000:2000000);
  emu_config_id++;
  emu_config_id_=emu_config_id;
  xdata::TimeVal _emu_config_time = (xdata::TimeVal)currentTime.gettimeofday();
  // Info to be entered in a form on a HyperDAQ page
  xdata::String _emu_config_type = "GLOBAL";
  xdata::String _description     = "manual entry";
  xdata::String _emu_endcap_side = endcap_side;

#ifdef debugV
  std::cout << "-- CONFIGURATION  emu_config_id --------- " <<  emu_config_id_.toString()       << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_time ------- " <<  _emu_config_time.toString()     << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_type ------- " <<  _emu_config_type.toString()     << std::endl;
  std::cout << "-- CONFIGURATION  description ----------- " <<  _description.toString()         << std::endl;
  std::cout << "-- CONFIGURATION  emu_endcap_side ------- " <<  _emu_endcap_side.toString()     << std::endl;
#endif

  newRows = tableDefinition_emu_configuration;

  newRows.setValueAt(rowId, DESCRIPTION,     _description); 
  newRows.setValueAt(rowId, EMU_CONFIG_ID,   emu_config_id_);
  newRows.setValueAt(rowId, EMU_CONFIG_TIME, _emu_config_time);
  newRows.setValueAt(rowId, EMU_CONFIG_TYPE, _emu_config_type);
  newRows.setValueAt(rowId, EMU_ENDCAP_SIDE, _emu_endcap_side);
  
  insert(connectionID,insertViewName,newRows);
  
  
}
//

void EmuTStore::uploadPeripheralCrate(const std::string &connectionID, const std::vector<Crate *> &TStore_allCrates) throw (xcept::Exception) {

  std::string insertViewName="peripheralcrate";
  xdata::Table newRows;

  cout << "List of crates:<br>" << std::endl;
  for(unsigned i = 0; i < TStore_allCrates.size(); ++i) {
    if(TStore_allCrates[i]) {

      size_t rowId(0);

      std::string CRATEID("CRATEID");
      std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
      std::string LABEL("LABEL");
      std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");

      xdata::UnsignedInteger64 _periph_config_id = emu_config_id_ * 100000000 + (i+1)*1000000;

      xdata::UnsignedShort _crateid = TStore_allCrates[i]->CrateID();
      xdata::String _label = TStore_allCrates[i]->GetLabel();

#ifdef debugV
      std::cout << "-- PERIPHERALCRATE  crateid ------------- " << _crateid.toString()          << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  label --------------- " << _label.toString()            << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  emu_config_id ------- " << emu_config_id_.toString()    << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  periph_config_id ---- " << _periph_config_id.toString() << std::endl; 
#endif
     
      newRows.clear();
      newRows = tableDefinition_emu_peripheralcrate;

      newRows.setValueAt(rowId, EMU_CONFIG_ID,    emu_config_id_);
      newRows.setValueAt(rowId, CRATEID,          _crateid);
      newRows.setValueAt(rowId, LABEL,            _label);
      newRows.setValueAt(rowId, PERIPH_CONFIG_ID, _periph_config_id);

      insert(connectionID,insertViewName,newRows);

      CCB * thisCCB = TStore_allCrates[i]->ccb();
      uploadCCB(connectionID, _periph_config_id, thisCCB);
	
      MPC * thisMPC = TStore_allCrates[i]->mpc();
      uploadMPC(connectionID, _periph_config_id, thisMPC);
      
      uploadVMECC(connectionID, _periph_config_id, TStore_allCrates[i]);

      std::vector<Chamber *> allChambers = TStore_allCrates[i]->chambers();
      uploadCSC(connectionID, _periph_config_id, allChambers);
      

    } // end of if
  } // end of loop over TStore_myCrates[i]

}
//

void EmuTStore::uploadCCB(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, CCB * TStore_thisCCB) throw (xcept::Exception) {
  
  std::string insertViewName="ccb";
  xdata::Table newRows;
  
  size_t rowId(0);
  std::string CCBMODE("CCBMODE");
  std::string CCB_CONFIG_ID("CCB_CONFIG_ID");
  std::string CCB_FIRMWARE_DAY("CCB_FIRMWARE_DAY");
  std::string CCB_FIRMWARE_MONTH("CCB_FIRMWARE_MONTH");
  std::string CCB_FIRMWARE_YEAR("CCB_FIRMWARE_YEAR");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string L1ADELAY("L1ADELAY");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string TTCRXCOARSEDELAY("TTCRXCOARSEDELAY");
  std::string TTCRXFINEDELAY("TTCRXFINEDELAY");
  std::string TTCRXID("TTCRXID");

  xdata::UnsignedInteger64 _ccb_config_id      = periph_config_id + 110000;
  xdata::UnsignedShort     _ccbmode            = TStore_thisCCB->GetCCBmode();
  xdata::UnsignedShort     _ccb_firmware_day   = TStore_thisCCB->GetExpectedFirmwareDay();
  xdata::UnsignedShort     _ccb_firmware_month = TStore_thisCCB->GetExpectedFirmwareMonth();
  xdata::UnsignedShort     _ccb_firmware_year  = TStore_thisCCB->GetExpectedFirmwareYear();
  xdata::UnsignedShort     _l1adelay           = TStore_thisCCB->Getl1adelay();
  xdata::UnsignedShort     _ttcrxcoarsedelay   = TStore_thisCCB->GetTTCrxCoarseDelay();
  xdata::UnsignedShort     _ttcrxfinedelay     = TStore_thisCCB->GetTTCrxFineDelay();
  xdata::UnsignedShort     _ttcrxid            = TStore_thisCCB->GetTTCrxID();

#ifdef debugV
  cout << "-- CCB emu_config_id -------- " << emu_config_id_.toString()      << std::endl;
  cout << "-- CCB ccb_config_id -------- " << _ccb_config_id.toString()      << std::endl;
  cout << "-- CCB ccbmode -------------- " << _ccbmode.toString()            << std::endl;
  cout << "-- CCB ccb_firmware_day ----- " << _ccb_firmware_day.toString()   << std::endl;
  cout << "-- CCB ccb_firmware_month --- " << _ccb_firmware_month.toString() << std::endl;
  cout << "-- CCB ccb_firmware_year ---- " << _ccb_firmware_year.toString()  << std::endl;
  cout << "-- CCB l1adelay ------------- " << _l1adelay.toString()           << std::endl;
  cout << "-- CCB ttcrxcoarsedelay ----- " << _ttcrxcoarsedelay.toString()   << std::endl;
  cout << "-- CCB ttcrxfinedelay ------- " << _ttcrxfinedelay.toString()     << std::endl;
  cout << "-- CCB ttcrxid--------------- " << _ttcrxid.toString()            << std::endl;
#endif

  newRows.clear();
  newRows = tableDefinition_emu_ccb;
 
  newRows.setValueAt(rowId, CCBMODE,            _ccbmode);
  newRows.setValueAt(rowId, CCB_CONFIG_ID,      _ccb_config_id);
  newRows.setValueAt(rowId, CCB_FIRMWARE_DAY,   _ccb_firmware_day);
  newRows.setValueAt(rowId, CCB_FIRMWARE_MONTH, _ccb_firmware_month);
  newRows.setValueAt(rowId, CCB_FIRMWARE_YEAR,  _ccb_firmware_year);
  newRows.setValueAt(rowId, EMU_CONFIG_ID,      emu_config_id_);
  newRows.setValueAt(rowId, L1ADELAY,           _l1adelay);
  newRows.setValueAt(rowId, PERIPH_CONFIG_ID,   periph_config_id);
  newRows.setValueAt(rowId, TTCRXCOARSEDELAY,   _ttcrxcoarsedelay);
  newRows.setValueAt(rowId, TTCRXFINEDELAY,     _ttcrxfinedelay);
  newRows.setValueAt(rowId, TTCRXID,            _ttcrxid);
  
  insert(connectionID,insertViewName,newRows);

}
//

void EmuTStore::uploadMPC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, MPC * TStore_thisMPC) throw (xcept::Exception) {

  std::string insertViewName="mpc";
  xdata::Table newRows;
  
  size_t rowId(0);
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string MPC_CONFIG_ID("MPC_CONFIG_ID");
  std::string MPC_FIRMWARE_DAY("MPC_FIRMWARE_DAY");
  std::string MPC_FIRMWARE_MONTH("MPC_FIRMWARE_MONTH");
  std::string MPC_FIRMWARE_YEAR("MPC_FIRMWARE_YEAR");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string SERIALIZERMODE("SERIALIZERMODE");
  std::string TRANSPARENTMODE("TRANSPARENTMODE");

  xdata::UnsignedInteger64 _mpc_config_id      = periph_config_id + 120000;
  xdata::UnsignedShort     _mpc_firmware_day   = TStore_thisMPC->GetExpectedFirmwareDay();
  xdata::UnsignedShort     _mpc_firmware_month = TStore_thisMPC->GetExpectedFirmwareMonth();
  xdata::UnsignedShort     _mpc_firmware_year  = TStore_thisMPC->GetExpectedFirmwareYear();
  xdata::UnsignedShort     _transparentmode    = TStore_thisMPC->GetTransparentMode();
  xdata::UnsignedShort     _serializermode     = TStore_thisMPC->GetSerializerMode();

#ifdef debugV
  cout << "-- MPC emu_config_id -------- " << emu_config_id_.toString()      << std::endl;
  cout << "-- MPC mpc_config_id -------- " << _mpc_config_id.toString()      << std::endl;
  cout << "-- MPC mpc_firmware_day ----- " << _mpc_firmware_day.toString()   << std::endl;
  cout << "-- MPC mpc_firmware_month --- " << _mpc_firmware_month.toString() << std::endl;
  cout << "-- MPC mpc_firmware_year ---- " << _mpc_firmware_year.toString()  << std::endl;
  cout << "-- MPC transparentmode ------ " << _transparentmode.toString()    << std::endl;
  cout << "-- MPC serializermode ------- " << _serializermode.toString()     << std::endl;
#endif 

  newRows.clear();
  newRows = tableDefinition_emu_mpc;

  newRows.setValueAt(rowId, EMU_CONFIG_ID,      emu_config_id_);  
  newRows.setValueAt(rowId, MPC_CONFIG_ID,      _mpc_config_id);
  newRows.setValueAt(rowId, MPC_FIRMWARE_DAY,   _mpc_firmware_day);
  newRows.setValueAt(rowId, MPC_FIRMWARE_MONTH, _mpc_firmware_month);
  newRows.setValueAt(rowId, MPC_FIRMWARE_YEAR,  _mpc_firmware_year);
  newRows.setValueAt(rowId, PERIPH_CONFIG_ID,   periph_config_id);
  newRows.setValueAt(rowId, SERIALIZERMODE,     _serializermode);
  newRows.setValueAt(rowId, TRANSPARENTMODE,    _transparentmode);

  insert(connectionID,insertViewName,newRows);

}
//

void EmuTStore::uploadVMECC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, Crate * TStore_thisCrate) throw (xcept::Exception) {

  std::string insertViewName="vcc";
  xdata::Table newRows;
  
  size_t rowId(0);
  std::string BGTO("BGTO");
  std::string BTO("BTO");
  std::string DFLT_SRV_MAC("DFLT_SRV_MAC");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string ETH_PORT("ETH_PORT");
  std::string ETHERNET_CR("ETHERNET_CR");
  std::string EXT_FIFO_CR("EXT_FIFO_CR");
  std::string MAC_ADDR("MAC_ADDR");
  std::string MCAST_1("MCAST_1");
  std::string MCAST_2("MCAST_2");
  std::string MCAST_3("MCAST_3");
  std::string MSG_LVL("MSG_LVL");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string PKT_ON_STARTUP("PKT_ON_STARTUP");
  std::string RST_MISC_CR("RST_MISC_CR");
  std::string VCC_CONFIG_ID("VCC_CONFIG_ID");
  std::string VCC_FRMW_VER("VCC_FRMW_VER");
  std::string VME_CR("VME_CR");
  std::string WARN_ON_SHTDWN("WARN_ON_SHTDWN");

  xdata::String            _bgto            = TStore_thisCrate->vmeController()->GetCR(5);
  xdata::String            _bto             = TStore_thisCrate->vmeController()->GetCR(4);
  xdata::String            _dflt_srv_mac    = TStore_thisCrate->vmeController()->GetMAC(4); 
  xdata::UnsignedShort     _eth_port        = TStore_thisCrate->vmeController()->port(); 
  xdata::String            _ethernet_cr     = TStore_thisCrate->vmeController()->GetCR(0);
  xdata::String            _ext_fifo_cr     = TStore_thisCrate->vmeController()->GetCR(1);
  xdata::String            _mac_addr        = TStore_thisCrate->vmeController()->GetMAC(0);
  xdata::String            _mcast_1         = TStore_thisCrate->vmeController()->GetMAC(1);
  xdata::String            _mcast_2         = TStore_thisCrate->vmeController()->GetMAC(2);
  xdata::String            _mcast_3         = TStore_thisCrate->vmeController()->GetMAC(3);
  xdata::UnsignedShort     _msg_lvl         = TStore_thisCrate->vmeController()->GetMsg_Lvl();
  xdata::String            _pkt_on_startup  = TStore_thisCrate->vmeController()->GetPkt_On_Startup();
  xdata::String            _rst_misc_cr     = TStore_thisCrate->vmeController()->GetCR(2);
  xdata::UnsignedInteger64 _vcc_config_id   = periph_config_id + 130000; 
  xdata::String            _vcc_frmw_ver    = TStore_thisCrate->vmecc()->GetVCC_frmw_ver();
  xdata::String            _vme_cr          = TStore_thisCrate->vmeController()->GetCR(3);
  xdata::String            _warn_on_shtdwn  = TStore_thisCrate->vmeController()->GetWarn_On_Shtdwn();

#ifdef debugV
  cout << "-- VCC emu_config_id -------- " << emu_config_id_.toString()    << std::endl;
  cout << "-- VCC periph_config_id ----- " << periph_config_id.toString()  << std::endl;
  cout << "-- VCC vcc_config_id -------- " << _vcc_config_id.toString()    << std::endl;
  cout << "-- VCC bgto ----------------- " <<_bgto.toString()              << std::endl;
  cout << "-- VCC bto ------------------ " <<_bto.toString()               << std::endl;
  cout << "-- VCC dflt_srv_mac --------- " <<_dflt_srv_mac.toString()      << std::endl;
  cout << "-- VCC eth_port ------------- " <<_eth_port.toString()          << std::endl;
  cout << "-- VCC ethernet_cr ---------- " <<_ethernet_cr.toString()       << std::endl;
  cout << "-- VCC ext_fifo_cr ---------- " <<_ext_fifo_cr.toString()       << std::endl;
  cout << "-- VCC mac_addr ------------- " <<_mac_addr.toString()          << std::endl;
  cout << "-- VCC mcast_1 -------------- " <<_mcast_1.toString()           << std::endl;
  cout << "-- VCC mcast_2 -------------- " <<_mcast_2.toString()           << std::endl;
  cout << "-- VCC mcast_3 -------------- " <<_mcast_3.toString()           << std::endl;
  cout << "-- VCC msg_lvl -------------- " <<_msg_lvl.toString()           << std::endl;
  cout << "-- VCC pkt_on_startup ------- " <<_pkt_on_startup.toString()    << std::endl;
  cout << "-- VCC rst_misc_cr ---------- " <<_rst_misc_cr.toString()       << std::endl;
  cout << "-- VCC vcc_frmw_ver --------- " << _vcc_frmw_ver.toString()     << std::endl;
  cout << "-- VCC vme_cr --------------- " <<_vme_cr.toString()            << std::endl;
  cout << "-- VCC warn_on_shtdwn ------- " <<_warn_on_shtdwn.toString()    << std::endl;
#endif

  newRows.clear();
  newRows = tableDefinition_emu_vcc;

  newRows.setValueAt(rowId, BGTO,             _bgto);
  newRows.setValueAt(rowId, BTO,              _bto);
  newRows.setValueAt(rowId, DFLT_SRV_MAC,     _dflt_srv_mac);
  newRows.setValueAt(rowId, EMU_CONFIG_ID,    emu_config_id_);
  newRows.setValueAt(rowId, ETH_PORT,         _eth_port);
  newRows.setValueAt(rowId, ETHERNET_CR,      _ethernet_cr);
  newRows.setValueAt(rowId, EXT_FIFO_CR,      _ext_fifo_cr);
  newRows.setValueAt(rowId, MAC_ADDR,         _mac_addr);
  newRows.setValueAt(rowId, MCAST_1,          _mcast_1);
  newRows.setValueAt(rowId, MCAST_2,          _mcast_2);
  newRows.setValueAt(rowId, MCAST_3,          _mcast_3);
  newRows.setValueAt(rowId, MSG_LVL,          _msg_lvl);
  newRows.setValueAt(rowId, PERIPH_CONFIG_ID, periph_config_id);
  newRows.setValueAt(rowId, PKT_ON_STARTUP,   _pkt_on_startup);
  newRows.setValueAt(rowId, RST_MISC_CR,      _rst_misc_cr);
  newRows.setValueAt(rowId, VCC_CONFIG_ID,    _vcc_config_id);
  newRows.setValueAt(rowId, VCC_FRMW_VER,     _vcc_frmw_ver);
  newRows.setValueAt(rowId, VME_CR,           _vme_cr);
  newRows.setValueAt(rowId, WARN_ON_SHTDWN,   _warn_on_shtdwn);

  insert(connectionID,insertViewName,newRows);

}
//

void EmuTStore::uploadCSC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, const std::vector<Chamber *> &TStore_allChambers) throw (xcept::Exception) {
  
  std::string insertViewName="csc";
  xdata::Table newRows;
  
  size_t rowId(0);
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string LABEL("LABEL");
  std::string KNOWN_PROBLEM("KNOWN_PROBLEM");
  std::string PROBLEM_MASK("PROBLEM_MASK");


  xdata::UnsignedInteger64 _csc_config_id;
  xdata::String _label;
  xdata::String _known_problem;
  xdata::UnsignedInteger64 _problem_mask;
  
  for(unsigned j = 0; j < TStore_allChambers.size(); ++j) {
    if(TStore_allChambers[j]) {      
      _csc_config_id = periph_config_id + (j+1)*10000;
      _label = TStore_allChambers[j]->GetLabel();
      _known_problem = TStore_allChambers[j]->GetProblemDescription();
      _problem_mask = TStore_allChambers[j]->GetProblemMask();
      

#ifdef debugV
      cout << "-- CSC emu_config_id ------ " << emu_config_id_.toString()   << std::endl;
      cout << "-- CSC periph_config_id --- " << periph_config_id.toString() << std::endl;
      cout << "-- CSC csc_config_id ------ " << _csc_config_id.toString()   << std::endl;
      cout << "-- CSC label -------------- " << _label.toString()           << std::endl;
      cout << "-- CSC known_problem ------ " << _known_problem.toString()   << std::endl;
      cout << "-- CSC problem_mask ------- " << _problem_mask.toString()    << std::endl;

      cout << "-- ######################## " << std::endl;
#endif

      newRows.clear();
      newRows = tableDefinition_emu_csc;

      newRows.setValueAt(rowId, CSC_CONFIG_ID,    _csc_config_id);
      newRows.setValueAt(rowId, EMU_CONFIG_ID,    emu_config_id_);
      newRows.setValueAt(rowId, LABEL,            _label);
      newRows.setValueAt(rowId, KNOWN_PROBLEM,    _known_problem);
      newRows.setValueAt(rowId, PROBLEM_MASK,     _problem_mask);
      newRows.setValueAt(rowId, PERIPH_CONFIG_ID, periph_config_id);

      insert(connectionID,insertViewName,newRows);

      DAQMB * thisDAQMB = TStore_allChambers[j]->GetDMB();
      uploadDAQMB(connectionID, _csc_config_id, thisDAQMB);
      
      TMB * thisTMB = TStore_allChambers[j]->GetTMB();
      uploadTMB(connectionID, _csc_config_id, thisTMB);
      
    } // end of if
  } // end of loop over TStore_myChambers[j]

}
//


void EmuTStore::uploadDAQMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, DAQMB * &TStore_thisDAQMB) throw (xcept::Exception) {
  
  std::string insertViewName="daqmb";
  xdata::Table newRows;
  std::string valueInHex;
  
  size_t rowId(0);
  std::string ALCT_DAV_CABLE_DELAY("ALCT_DAV_CABLE_DELAY");
  std::string CALIBRATION_L1ACC_DELAY("CALIBRATION_L1ACC_DELAY");
  std::string CALIBRATION_LCT_DELAY("CALIBRATION_LCT_DELAY");
  std::string CFEB_CABLE_DELAY("CFEB_CABLE_DELAY");
  std::string CFEB_DAV_CABLE_DELAY("CFEB_DAV_CABLE_DELAY");
  std::string myCOMP_MODE("COMP_MODE");
  std::string COMP_TIMING("COMP_TIMING");
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string DMB_CNTL_FIRMWARE_TAG("DMB_CNTL_FIRMWARE_TAG");
  std::string DMB_VME_FIRMWARE_TAG("DMB_VME_FIRMWARE_TAG");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string FEB_CLOCK_DELAY("FEB_CLOCK_DELAY");
  std::string INJ_DAC_SET("INJ_DAC_SET");
  std::string INJECT_DELAY("INJECT_DELAY");
  std::string KILL_INPUT("KILL_INPUT");
  std::string PRE_BLOCK_END("PRE_BLOCK_END");
  std::string PUL_DAC_SET("PUL_DAC_SET");
  std::string PULSE_DELAY("PULSE_DELAY");
  std::string SET_COMP_THRESH("SET_COMP_THRESH");
  std::string SLOT("SLOT");
  std::string TMB_LCT_CABLE_DELAY("TMB_LCT_CABLE_DELAY");
  std::string XFINELATENCY("XFINELATENCY");
  std::string XLATENCY("XLATENCY");

  xdata::UnsignedShort     _alct_dav_cable_delay    = TStore_thisDAQMB->GetAlctDavCableDelay(); 
  xdata::UnsignedShort     _calibration_l1acc_delay = TStore_thisDAQMB->GetCalibrationL1aDelay(); 
  xdata::UnsignedShort     _calibration_lct_delay   = TStore_thisDAQMB->GetCalibrationLctDelay(); 
  xdata::UnsignedShort     _cfeb_cable_delay        = TStore_thisDAQMB->GetCfebCableDelay(); 
  xdata::UnsignedShort     _cfeb_dav_cable_delay    = TStore_thisDAQMB->GetCfebDavCableDelay(); 
  xdata::UnsignedShort     _comp_mode               = TStore_thisDAQMB->GetCompMode(); 
  xdata::UnsignedShort     _comp_timing             = TStore_thisDAQMB->GetCompTiming(); 
  xdata::UnsignedInteger64 _daqmb_config_id         = csc_config_id + 1000;
  xdata::String            _dmb_cntl_firmware_tag;
  convertToHex(valueInHex,"%lx",TStore_thisDAQMB->GetExpectedControlFirmwareTag());
  _dmb_cntl_firmware_tag = valueInHex;
  xdata::UnsignedShort     _dmb_vme_firmware_tag    = TStore_thisDAQMB->GetExpectedVMEFirmwareTag();
  xdata::UnsignedShort     _feb_clock_delay         = TStore_thisDAQMB->GetCfebClkDelay();
  xdata::Float             _inj_dac_set             = TStore_thisDAQMB->GetInjectorDac();
  xdata::UnsignedShort     _inject_delay            = TStore_thisDAQMB->GetInjectDelay();
  xdata::UnsignedShort     _kill_input              = TStore_thisDAQMB->GetKillInput();
  xdata::UnsignedShort     _pre_block_end           = TStore_thisDAQMB->GetPreBlockEnd();
  xdata::Float             _pul_dac_set             = TStore_thisDAQMB->GetPulseDac();
  xdata::UnsignedShort     _pulse_delay             = TStore_thisDAQMB->GetPulseDelay();
  xdata::Float             _set_comp_tresh          = TStore_thisDAQMB->GetCompThresh();
  xdata::UnsignedShort     _slot                    = TStore_thisDAQMB->slot(); 
  xdata::UnsignedShort     _tmb_lct_cable_delay     = TStore_thisDAQMB->GetTmbLctCableDelay(); 
  xdata::UnsignedShort     _xfinelatency            = TStore_thisDAQMB->GetxFineLatency(); 
  xdata::UnsignedShort     _xlatency                = TStore_thisDAQMB->GetxLatency();

#ifdef debugV
  cout << "-- DAQMB emu_config_id -------------- " << emu_config_id_.toString()           << std::endl;
  cout << "-- DAQMB periph_config_id ----------- " << csc_config_id.toString()            << std::endl;
  cout << "-- DAQMB daqmb_config_id ------------ " << _daqmb_config_id.toString()         << std::endl;
  cout << "-- DAQMB alct_dav_cable_delay ------- " << _alct_dav_cable_delay.toString()    << std::endl;
  cout << "-- DAQMB calibration_l1acc_delay ---- " << _calibration_l1acc_delay.toString() << std::endl;
  cout << "-- DAQMB calibration_lct_delay ------ " << _calibration_lct_delay.toString()   << std::endl;
  cout << "-- DAQMB cfeb_cable_delay ----------- " << _cfeb_cable_delay.toString()        << std::endl;
  cout << "-- DAQMB cfeb_dav_cable_delay ------- " << _cfeb_dav_cable_delay.toString()    << std::endl;
  cout << "-- DAQMB comp_mode ------------------ " << _comp_mode.toString()               << std::endl;
  cout << "-- DAQMB comp_timing ---------------- " << _comp_timing.toString()             << std::endl;
  cout << "-- DAQMB dmb_cntl_firmware_tag ------ " << _dmb_cntl_firmware_tag.toString()   << std::endl;
  cout << "-- DAQMB dmb_vme_firmware_tag ------- " << _dmb_vme_firmware_tag.toString()    << std::endl;
  cout << "-- DAQMB feb_clock_delay ------------ " << _feb_clock_delay.toString()         << std::endl;
  cout << "-- DAQMB inj_dac_set ---------------- " << _inj_dac_set.toString()             << std::endl;
  cout << "-- DAQMB inject_delay --------------- " << _inject_delay.toString()            << std::endl;
  cout << "-- DAQMB kill_input ----------------- " << _kill_input.toString()              << std::endl;
  cout << "-- DAQMB pre_block_end -------------- " << _pre_block_end.toString()           << std::endl;
  cout << "-- DAQMB pul_dac_set ---------------- " << _pul_dac_set.toString()             << std::endl;
  cout << "-- DAQMB pulse_delay----------------- " << _pulse_delay.toString()             << std::endl;
  cout << "-- DAQMB set_comp_tresh ------------- " << _set_comp_tresh.toString()          << std::endl;
  cout << "-- DAQMB slot ----------------------- " << _slot.toString()                    << std::endl;
  cout << "-- DAQMB tmb_lct_cable_delay -------- " << _tmb_lct_cable_delay.toString()     << std::endl;
  cout << "-- DAQMB xfinelatency --------------- " << _xfinelatency.toString()            << std::endl;
  cout << "-- DAQMB xlatency ------------------- " << _xlatency.toString()                << std::endl;
#endif

  newRows.clear();
  newRows = tableDefinition_emu_daqmb; 
 
  newRows.setValueAt(rowId, ALCT_DAV_CABLE_DELAY,    _alct_dav_cable_delay);
  newRows.setValueAt(rowId, CALIBRATION_L1ACC_DELAY, _calibration_l1acc_delay);
  newRows.setValueAt(rowId, CALIBRATION_LCT_DELAY,   _calibration_lct_delay);
  newRows.setValueAt(rowId, CFEB_CABLE_DELAY,        _cfeb_cable_delay);
  newRows.setValueAt(rowId, CFEB_DAV_CABLE_DELAY,    _cfeb_dav_cable_delay);
  newRows.setValueAt(rowId, myCOMP_MODE,             _comp_mode);
  newRows.setValueAt(rowId, COMP_TIMING,             _comp_timing);
  newRows.setValueAt(rowId, CSC_CONFIG_ID,           csc_config_id);
  newRows.setValueAt(rowId, DAQMB_CONFIG_ID,         _daqmb_config_id);
  newRows.setValueAt(rowId, DMB_CNTL_FIRMWARE_TAG,   _dmb_cntl_firmware_tag);
  newRows.setValueAt(rowId, DMB_VME_FIRMWARE_TAG,    _dmb_vme_firmware_tag);
  newRows.setValueAt(rowId, EMU_CONFIG_ID,           emu_config_id_);
  newRows.setValueAt(rowId, FEB_CLOCK_DELAY,         _feb_clock_delay);
  newRows.setValueAt(rowId, INJ_DAC_SET,             _inj_dac_set);
  newRows.setValueAt(rowId, INJECT_DELAY,            _inject_delay);
  newRows.setValueAt(rowId, KILL_INPUT,              _kill_input);
  newRows.setValueAt(rowId, PRE_BLOCK_END,           _pre_block_end);
  newRows.setValueAt(rowId, PUL_DAC_SET,             _pul_dac_set);
  newRows.setValueAt(rowId, PULSE_DELAY,             _pulse_delay);
  newRows.setValueAt(rowId, SET_COMP_THRESH,         _set_comp_tresh);
  newRows.setValueAt(rowId, SLOT,                    _slot);
  newRows.setValueAt(rowId, TMB_LCT_CABLE_DELAY,     _tmb_lct_cable_delay); 
  newRows.setValueAt(rowId, XFINELATENCY,            _xfinelatency);
  newRows.setValueAt(rowId, XLATENCY,                _xlatency); 

  insert(connectionID,insertViewName,newRows);

  uploadCFEB(connectionID, _daqmb_config_id, TStore_thisDAQMB);

}
//

void EmuTStore::uploadCFEB(const std::string &connectionID, xdata::UnsignedInteger64 &daqmb_config_id, DAQMB * &TStore_thisDAQMB) throw (xcept::Exception) {
  
  std::string insertViewName="cfeb";
  xdata::Table newRows;
  std::string valueInHex;

  newRows = tableDefinition_emu_cfeb;

  size_t rowId(0);
  std::string CFEB_CONFIG_ID("CFEB_CONFIG_ID");
  std::string CFEB_FIRMWARE_TAG("CFEB_FIRMWARE_TAG");
  std::string CFEB_NUMBER("CFEB_NUMBER");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string KILL_CHIP0("KILL_CHIP0");
  std::string KILL_CHIP1("KILL_CHIP1");
  std::string KILL_CHIP2("KILL_CHIP2");
  std::string KILL_CHIP3("KILL_CHIP3");
  std::string KILL_CHIP4("KILL_CHIP4");
  std::string KILL_CHIP5("KILL_CHIP5");

  std::vector<CFEB> TStore_allCFEBs = TStore_thisDAQMB->cfebs();

  for(unsigned j = 0; j < TStore_allCFEBs.size(); ++j) {

    xdata::UnsignedShort      _cfeb_number       = TStore_allCFEBs[j].number();
    convertToHex(valueInHex,"%lx",TStore_thisDAQMB->GetExpectedCFEBFirmwareTag(_cfeb_number));
    xdata::String _cfeb_firmware_tag = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 0));
    xdata::String _kill_chip0 = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 1));
    xdata::String _kill_chip1 = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 2));
    xdata::String _kill_chip2 = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 3));
    xdata::String _kill_chip3 = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 4));
    xdata::String _kill_chip4 = valueInHex;
    convertToHex(valueInHex,"%x",TStore_thisDAQMB->GetKillChip(_cfeb_number, 5));
    xdata::String _kill_chip5 = valueInHex;
    xdata::UnsignedInteger64  _cfeb_config_id    = daqmb_config_id + _cfeb_number;

#ifdef debugV
    cout << "-- CFEB emu_config_id ---------- " << emu_config_id_.toString()       << std::endl;
    cout << "-- CFEB periph_config_id ------- " << daqmb_config_id.toString()      << std::endl;
    cout << "-- CFEB cfeb_config_id --------- " << _cfeb_config_id.toString()      << std::endl;
    cout << "-- CFEB cfeb_number ------------ " << _cfeb_number.toString()         << std::endl;
    cout << "-- CFEB cfeb_firmware_tag ------ " << _cfeb_firmware_tag.toString()   << std::endl;
    cout << "-- CFEB kill_chip0 ------------- " << _kill_chip0.toString()          << std::endl;
    cout << "-- CFEB kill_chip1 ------------- " << _kill_chip1.toString()          << std::endl;
    cout << "-- CFEB kill_chip2 ------------- " << _kill_chip2.toString()          << std::endl;
    cout << "-- CFEB kill_chip3 ------------- " << _kill_chip3.toString()          << std::endl;
    cout << "-- CFEB kill_chip4 ------------- " << _kill_chip4.toString()          << std::endl;
    cout << "-- CFEB kill_chip5 ------------- " << _kill_chip5.toString()          << std::endl;
#endif
    
    newRows.setValueAt(rowId, CFEB_CONFIG_ID,    _cfeb_config_id);
    newRows.setValueAt(rowId, CFEB_FIRMWARE_TAG, _cfeb_firmware_tag);
    newRows.setValueAt(rowId, CFEB_NUMBER,       _cfeb_number);
    newRows.setValueAt(rowId, DAQMB_CONFIG_ID,   daqmb_config_id);
    newRows.setValueAt(rowId, EMU_CONFIG_ID,     emu_config_id_);
    newRows.setValueAt(rowId, KILL_CHIP0,        _kill_chip0);
    newRows.setValueAt(rowId, KILL_CHIP1,        _kill_chip1);
    newRows.setValueAt(rowId, KILL_CHIP2,        _kill_chip2);
    newRows.setValueAt(rowId, KILL_CHIP3,        _kill_chip3);
    newRows.setValueAt(rowId, KILL_CHIP4,        _kill_chip4);
    newRows.setValueAt(rowId, KILL_CHIP5,        _kill_chip5);
    rowId++;
  }
  
  insert(connectionID,insertViewName,newRows);

}
//


void EmuTStore::uploadTMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, TMB * &TStore_thisTMB) throw (xcept::Exception) {
  
  std::string insertViewName="tmb";
  xdata::Table newRows;
  std::string valueInHex;

  size_t rowId(0);
  std::string ADJACENT_CFEB_DISTANCE("ADJACENT_CFEB_DISTANCE");
  std::string AFF_THRESH("AFF_THRESH");
  std::string ALCT_BX0_DELAY("ALCT_BX0_DELAY");
  std::string ALCT_BX0_ENABLE("ALCT_BX0_ENABLE");
  std::string ALCT_CLEAR("ALCT_CLEAR");
  std::string ALCT_CLOCK_EN_USE_CCB("ALCT_CLOCK_EN_USE_CCB");
  std::string ALCT_PRETRIG_ENABLE("ALCT_PRETRIG_ENABLE");
  std::string ALCT_RX_CLOCK_DELAY("ALCT_RX_CLOCK_DELAY");
  std::string ALCT_TRIG_ENABLE("ALCT_TRIG_ENABLE");
  std::string ALCT_TX_CLOCK_DELAY("ALCT_TX_CLOCK_DELAY");
  std::string ALL_CFEB_ACTIVE("ALL_CFEB_ACTIVE");
  std::string CFEB0DELAY("CFEB0DELAY");
  std::string CFEB1DELAY("CFEB1DELAY");
  std::string CFEB2DELAY("CFEB2DELAY");
  std::string CFEB3DELAY("CFEB3DELAY");
  std::string CFEB4DELAY("CFEB4DELAY");
  std::string CFEB_ENABLE_SOURCE("CFEB_ENABLE_SOURCE");
  std::string CLCT_BLANKING("CLCT_BLANKING");
  std::string CLCT_BX0_DELAY("CLCT_BX0_DELAY");
  std::string CLCT_DRIFT_DELAY("CLCT_DRIFT_DELAY");
  std::string CLCT_EXT_PRETRIG_ENABLE("CLCT_EXT_PRETRIG_ENABLE");
  std::string CLCT_FIFO_PRETRIG("CLCT_FIFO_PRETRIG");
  std::string CLCT_FIFO_TBINS("CLCT_FIFO_TBINS");
  std::string CLCT_HIT_PERSIST("CLCT_HIT_PERSIST");
  std::string CLCT_MIN_SEPARATION("CLCT_MIN_SEPARATION");
  std::string CLCT_NPLANES_HIT_PATTERN("CLCT_NPLANES_HIT_PATTERN");
  std::string CLCT_NPLANES_HIT_PRETRIG("CLCT_NPLANES_HIT_PRETRIG");
  std::string CLCT_PID_THRESH_PRETRIG("CLCT_PID_THRESH_PRETRIG");
  std::string CLCT_PRETRIG_ENABLE("CLCT_PRETRIG_ENABLE");
  std::string CLCT_STAGGER("CLCT_STAGGER");
  std::string CLCT_THROTTLE("CLCT_THROTTLE");
  std::string CLCT_TRIG_ENABLE("CLCT_TRIG_ENABLE");
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string DMB_TX_DELAY("DMB_TX_DELAY");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string ENABLE_ALCT_RX("ENABLE_ALCT_RX");
  std::string ENABLE_ALCT_TX("ENABLE_ALCT_TX");
  std::string ENABLECLCTINPUTS_REG42("ENABLECLCTINPUTS_REG42");
  std::string ENABLECLCTINPUTS_REG68("ENABLECLCTINPUTS_REG68");
  std::string IGNORE_CCB_STARTSTOP("IGNORE_CCB_STARTSTOP");
  std::string L1A_ALLOW_ALCT_ONLY("L1A_ALLOW_ALCT_ONLY");
  std::string L1A_ALLOW_MATCH("L1A_ALLOW_MATCH");
  std::string L1A_ALLOW_NOL1A("L1A_ALLOW_NOL1A");
  std::string L1A_ALLOW_NOTMB("L1A_ALLOW_NOTMB");
  std::string LAYER0_DISTRIP_HOT_CHANN_MASK("LAYER0_DISTRIP_HOT_CHANN_MASK");
  std::string LAYER1_DISTRIP_HOT_CHANN_MASK("LAYER1_DISTRIP_HOT_CHANN_MASK");
  std::string LAYER2_DISTRIP_HOT_CHANN_MASK("LAYER2_DISTRIP_HOT_CHANN_MASK");
  std::string LAYER3_DISTRIP_HOT_CHANN_MASK("LAYER3_DISTRIP_HOT_CHANN_MASK");
  std::string LAYER4_DISTRIP_HOT_CHANN_MASK("LAYER4_DISTRIP_HOT_CHANN_MASK");
  std::string LAYER5_DISTRIP_HOT_CHANN_MASK("LAYER5_DISTRIP_HOT_CHANN_MASK");
  //  std::string LAYER_TRIG_DELAY("LAYER_TRIG_DELAY");  // obsolete
  std::string LAYER_TRIG_ENABLE("LAYER_TRIG_ENABLE");
  std::string LAYER_TRIG_THRESH("LAYER_TRIG_THRESH");
  std::string MATCH_PRETRIG_ALCT_DELAY("MATCH_PRETRIG_ALCT_DELAY");
  std::string MATCH_PRETRIG_ENABLE("MATCH_PRETRIG_ENABLE");
  std::string MATCH_PRETRIG_WINDOW_SIZE("MATCH_PRETRIG_WINDOW_SIZE");
  std::string MATCH_TRIG_ALCT_DELAY("MATCH_TRIG_ALCT_DELAY");
  std::string MATCH_TRIG_ENABLE("MATCH_TRIG_ENABLE");
  std::string MATCH_TRIG_WINDOW_SIZE("MATCH_TRIG_WINDOW_SIZE");
  std::string MPC_IDLE_BLANK("MPC_IDLE_BLANK");
  std::string MPC_OUTPUT_ENABLE("MPC_OUTPUT_ENABLE");
  std::string MPC_RX_DELAY("MPC_RX_DELAY");
  std::string MPC_SYNC_ERR_ENABLE("MPC_SYNC_ERR_ENABLE");
  std::string MPC_TX_DELAY("MPC_TX_DELAY");
  std::string RAT_FIRMWARE_DAY("RAT_FIRMWARE_DAY");
  std::string RAT_FIRMWARE_MONTH("RAT_FIRMWARE_MONTH");
  std::string RAT_FIRMWARE_YEAR("RAT_FIRMWARE_YEAR");
  std::string RAT_TMB_DELAY("RAT_TMB_DELAY");
  std::string REQUEST_L1A("REQUEST_L1A");
  std::string RPC_FIFO_DECOUPLE("RPC_FIFO_DECOUPLE");
  std::string RPC_FIFO_PRETRIG("RPC_FIFO_PRETRIG");
  std::string RPC_FIFO_TBINS("RPC_FIFO_TBINS");
  std::string RPC0_RAT_DELAY("RPC0_RAT_DELAY");
  std::string RPC0_RAW_DELAY("RPC0_RAW_DELAY");
  std::string RPC1_RAT_DELAY("RPC1_RAT_DELAY");
  std::string RPC1_RAW_DELAY("RPC1_RAW_DELAY");
  std::string RPC_BXN_OFFSET("RPC_BXN_OFFSET");
  std::string RPC_EXISTS("RPC_EXISTS");
  std::string RPC_MASK_ALL("RPC_MASK_ALL");
  std::string RPC_READ_ENABLE("RPC_READ_ENABLE");
  std::string SLOT("SLOT");
  std::string TMB_BXN_OFFSET("TMB_BXN_OFFSET");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");
  std::string TMB_FIFO_NO_RAW_HITS("TMB_FIFO_NO_RAW_HITS");
  std::string TMB_FIFO_MODE("TMB_FIFO_MODE");
  std::string TMB_FIRMWARE_DAY("TMB_FIRMWARE_DAY");
  std::string TMB_FIRMWARE_MONTH("TMB_FIRMWARE_MONTH");
  std::string TMB_FIRMWARE_REVCODE("TMB_FIRMWARE_REVCODE");
  std::string TMB_FIRMWARE_TYPE("TMB_FIRMWARE_TYPE");
  std::string TMB_FIRMWARE_VERSION("TMB_FIRMWARE_VERSION");
  std::string TMB_FIRMWARE_YEAR("TMB_FIRMWARE_YEAR");
  std::string TMB_L1A_DELAY("TMB_L1A_DELAY");
  std::string TMB_L1A_OFFSET("TMB_L1A_OFFSET");
  std::string TMB_L1A_WINDOW_SIZE("TMB_L1A_WINDOW_SIZE");
  std::string VALID_CLCT_REQUIRED("VALID_CLCT_REQUIRED");
  std::string WRITE_BUFFER_AUTOCLEAR("WRITE_BUFFER_AUTOCLEAR");
  std::string WRITE_BUFFER_CONTINOUS_ENABLE("WRITE_BUFFER_CONTINOUS_ENABLE");
  std::string WRITE_BUFFER_REQUIRED("WRITE_BUFFER_REQUIRED");

  xdata::UnsignedShort     _adjacent_cfeb_distance        = TStore_thisTMB->GetAdjacentCfebDistance();
  xdata::UnsignedShort     _aff_thresh                    = TStore_thisTMB->GetActiveFebFlagThresh();
  xdata::UnsignedShort     _alct_bx0_delay                = TStore_thisTMB->GetClctBx0Delay();
  xdata::UnsignedShort     _alct_bx0_enable               = TStore_thisTMB->GetAlctBx0Enable();
  xdata::UnsignedShort     _alct_clear                    = TStore_thisTMB->GetAlctClear();
  xdata::UnsignedShort     _alct_clock_en_use_ccb         = TStore_thisTMB->GetEnableAlctUseCcbClock();
  xdata::UnsignedShort     _alct_pretrig_enable           = TStore_thisTMB->GetAlctPatternTrigEnable();
  xdata::UnsignedShort     _alct_rx_clock_delay           = TStore_thisTMB->GetALCTrxPhase();
  xdata::UnsignedShort     _alct_trig_enable              = TStore_thisTMB->GetTmbAllowAlct();
  xdata::UnsignedShort     _alct_tx_clock_delay           = TStore_thisTMB->GetALCTtxPhase();
  xdata::UnsignedShort     _all_cfeb_active               = TStore_thisTMB->GetEnableAllCfebsActive();
  xdata::UnsignedShort     _cfeb0delay                    = TStore_thisTMB->GetCFEB0delay();
  xdata::UnsignedShort     _cfeb1delay                    = TStore_thisTMB->GetCFEB1delay();
  xdata::UnsignedShort     _cfeb2delay                    = TStore_thisTMB->GetCFEB2delay();
  xdata::UnsignedShort     _cfeb3delay                    = TStore_thisTMB->GetCFEB3delay();
  xdata::UnsignedShort     _cfeb4delay                    = TStore_thisTMB->GetCFEB4delay();
  xdata::UnsignedShort     _cfeb_enable_source            = TStore_thisTMB->GetCfebEnableSource_orig();
  xdata::UnsignedShort     _clct_blanking                 = TStore_thisTMB->GetClctBlanking();
  xdata::UnsignedShort     _clct_bx0_delay                = TStore_thisTMB->GetClctBx0Delay();
  xdata::UnsignedShort     _clct_drift_delay              = TStore_thisTMB->GetDriftDelay();
  xdata::UnsignedShort     _clct_ext_pretrig_enable       = TStore_thisTMB->GetClctExtTrigEnable();
  xdata::UnsignedShort     _clct_fifo_pretrig             = TStore_thisTMB->GetFifoPreTrig();
  xdata::UnsignedShort     _clct_fifo_tbins               = TStore_thisTMB->GetFifoTbins();
  xdata::UnsignedShort     _clct_hit_persist              = TStore_thisTMB->GetTriadPersistence();
  xdata::UnsignedShort     _clct_min_separation           = TStore_thisTMB->GetMinClctSeparation();
  xdata::UnsignedShort     _clct_nplanes_hit_pattern      = TStore_thisTMB->GetMinHitsPattern();
  xdata::UnsignedShort     _clct_nplanes_hit_pretrig      = TStore_thisTMB->GetHsPretrigThresh();
  xdata::UnsignedShort     _clct_pid_thresh_pretrig       = TStore_thisTMB->GetClctPatternIdThresh();
  xdata::UnsignedShort     _clct_pretrig_enable           = TStore_thisTMB->GetClctPatternTrigEnable();
  xdata::UnsignedShort     _clct_stagger                  = TStore_thisTMB->GetClctStagger();
  xdata::UnsignedShort     _clct_throttle                 = TStore_thisTMB->GetClctThrottle();
  xdata::UnsignedShort     _clct_trig_enable              = TStore_thisTMB->GetTmbAllowClct();
  xdata::UnsignedShort     _dmb_tx_delay                  = TStore_thisTMB->GetDmbTxDelay();
  xdata::UnsignedShort     _enable_alct_rx                = TStore_thisTMB->GetAlctInput();
  xdata::UnsignedShort     _enable_alct_tx                = TStore_thisTMB->GetEnableAlctTx();
  xdata::UnsignedShort     _enableclctinputs_reg42        = TStore_thisTMB->GetEnableCLCTInputs();
  xdata::UnsignedShort     _enableclctinputs_reg68        = TStore_thisTMB->GetCfebEnable();
  xdata::UnsignedShort     _ignore_ccb_startstop          = TStore_thisTMB->GetIgnoreCcbStartStop();
  xdata::UnsignedShort     _l1a_allow_alct_only           = TStore_thisTMB->GetL1aAllowAlctOnly();
  xdata::UnsignedShort     _l1a_allow_match               = TStore_thisTMB->GetL1aAllowMatch();
  xdata::UnsignedShort     _l1a_allow_nol1a               = TStore_thisTMB->GetL1aAllowNoL1a();
  xdata::UnsignedShort     _l1a_allow_notmb               = TStore_thisTMB->GetL1aAllowNoTmb();
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(0));
  xdata::String            _layer0_distrip_hot_chann_mask = valueInHex;
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(1));
  xdata::String            _layer1_distrip_hot_chann_mask = valueInHex;
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(2));
  xdata::String            _layer2_distrip_hot_chann_mask = valueInHex;
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(3));
  xdata::String            _layer3_distrip_hot_chann_mask = valueInHex;
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(4));
  xdata::String            _layer4_distrip_hot_chann_mask = valueInHex;
  convertToHex(valueInHex,"%Lx",TStore_thisTMB->GetDistripHotChannelMask(5));
  xdata::String            _layer5_distrip_hot_chann_mask = valueInHex;
  //  xdata::UnsignedShort     _layer_trig_delay              = TStore_thisTMB->GetLayerTrigDelay();   // obsolete
  xdata::UnsignedShort     _layer_trig_enable             = TStore_thisTMB->GetEnableLayerTrigger();
  xdata::UnsignedShort     _layer_trig_thresh             = TStore_thisTMB->GetLayerTriggerThreshold();
  xdata::UnsignedShort     _match_pretrig_alct_delay      = TStore_thisTMB->GetAlctPretrigDelay();
  xdata::UnsignedShort     _match_pretrig_enable          = TStore_thisTMB->GetMatchPatternTrigEnable();
  xdata::UnsignedShort     _match_pretrig_window_size     = TStore_thisTMB->GetAlctClctPretrigWidth();
  xdata::UnsignedShort     _match_trig_alct_delay         = TStore_thisTMB->GetAlctVpfDelay();
  xdata::UnsignedShort     _match_trig_enable             = TStore_thisTMB->GetTmbAllowMatch();
  xdata::UnsignedShort     _match_trig_window_size        = TStore_thisTMB->GetAlctMatchWindowSize();
  xdata::UnsignedShort     _mpc_idle_blank                = TStore_thisTMB->GetMpcIdleBlank();
  xdata::UnsignedShort     _mpc_output_enable             = TStore_thisTMB->GetMpcOutputEnable();
  xdata::UnsignedShort     _mpc_rx_delay                  = TStore_thisTMB->GetMpcRxDelay();
  xdata::UnsignedShort     _mpc_sync_err_enable           = TStore_thisTMB->GetTmbSyncErrEnable();
  xdata::UnsignedShort     _mpc_tx_delay                  = TStore_thisTMB->GetMpcTxDelay();
  xdata::UnsignedShort     _rat_firmware_day              = TStore_thisTMB->GetExpectedRatFirmwareDay();
  xdata::UnsignedShort     _rat_firmware_month            = TStore_thisTMB->GetExpectedRatFirmwareMonth();
  xdata::UnsignedShort     _rat_firmware_year             = TStore_thisTMB->GetExpectedRatFirmwareYear();
  xdata::UnsignedShort     _rat_tmb_delay                 = TStore_thisTMB->GetRatTmbDelay();
  xdata::UnsignedShort     _request_l1a                   = TStore_thisTMB->GetRequestL1a();
  xdata::UnsignedShort     _rpc0_rat_delay                = TStore_thisTMB->GetRpc0RatDelay();
  xdata::UnsignedShort     _rpc0_raw_delay                = TStore_thisTMB->GetRpc0RawDelay();
  xdata::UnsignedShort     _rpc1_rat_delay                = TStore_thisTMB->GetRpc1RatDelay();
  xdata::UnsignedShort     _rpc1_raw_delay                = TStore_thisTMB->GetRpc1RawDelay();
  xdata::UnsignedShort     _rpc_bxn_offset                = TStore_thisTMB->GetRpcBxnOffset();
  xdata::UnsignedShort     _rpc_exists                    = TStore_thisTMB->GetRpcExist();
  xdata::UnsignedShort     _rpc_fifo_decouple             = TStore_thisTMB->GetRpcDecoupleTbins();
  xdata::UnsignedShort     _rpc_pretrig                   = TStore_thisTMB->GetFifoPretrigRpc();
  xdata::UnsignedShort     _rpc_tbins                     = TStore_thisTMB->GetFifoTbinsRpc();
  xdata::UnsignedShort     _rpc_mask_all                  = TStore_thisTMB->GetEnableRpcInput();
  xdata::UnsignedShort     _rpc_read_enable               = TStore_thisTMB->GetRpcReadEnable();
  xdata::UnsignedShort     _slot                          = TStore_thisTMB->slot();
  xdata::UnsignedShort     _tmb_bxn_offset                = TStore_thisTMB->GetBxnOffset();
  xdata::UnsignedInteger64 _tmb_config_id                 = csc_config_id + 2000;
  xdata::UnsignedShort     _tmb_fifo_mode                 = TStore_thisTMB->GetFifoMode();
  xdata::UnsignedShort     _tmb_fifo_no_raw_hits          = TStore_thisTMB->GetFifoNoRawHits();
  xdata::UnsignedShort     _tmb_firmware_day              = TStore_thisTMB->GetExpectedTmbFirmwareDay();
  xdata::UnsignedShort     _tmb_firmware_month            = TStore_thisTMB->GetExpectedTmbFirmwareMonth();
  xdata::UnsignedShort     _tmb_firmware_revcode          = TStore_thisTMB->GetExpectedTmbFirmwareRevcode();
  xdata::UnsignedShort     _tmb_firmware_type             = TStore_thisTMB->GetExpectedTmbFirmwareType();
  xdata::UnsignedShort     _tmb_firmware_version          = TStore_thisTMB->GetExpectedTmbFirmwareVersion();
  xdata::UnsignedShort     _tmb_firmware_year             = TStore_thisTMB->GetExpectedTmbFirmwareYear();
  xdata::UnsignedShort     _tmb_l1a_delay                 = TStore_thisTMB->GetL1aDelay();
  xdata::UnsignedShort     _tmb_l1a_offset                = TStore_thisTMB->GetL1aOffset();
  xdata::UnsignedShort     _tmb_l1a_window_size           = TStore_thisTMB->GetL1aWindowSize();
  xdata::UnsignedShort     _valid_clct_required           = TStore_thisTMB->GetRequireValidClct();
  xdata::UnsignedShort     _write_buffer_required         = TStore_thisTMB->GetWriteBufferRequired();
  xdata::UnsignedShort     _write_buffer_autoclear        = TStore_thisTMB->GetWriteBufferAutoclear();
  xdata::UnsignedShort     _write_buffer_continous_enable = TStore_thisTMB->GetClctWriteContinuousEnable();

  newRows.clear();
  newRows = tableDefinition_emu_tmb; 
  
  newRows.setValueAt(rowId, ADJACENT_CFEB_DISTANCE,        _adjacent_cfeb_distance); 
  newRows.setValueAt(rowId, AFF_THRESH,                    _aff_thresh);
  newRows.setValueAt(rowId, ALCT_BX0_DELAY,                _alct_bx0_delay);
  newRows.setValueAt(rowId, ALCT_BX0_ENABLE,               _alct_bx0_enable);
  newRows.setValueAt(rowId, ALCT_CLEAR,                    _alct_clear);
  newRows.setValueAt(rowId, ALCT_CLOCK_EN_USE_CCB,         _alct_clock_en_use_ccb);
  newRows.setValueAt(rowId, ALCT_PRETRIG_ENABLE,           _alct_pretrig_enable);
  newRows.setValueAt(rowId, ALCT_RX_CLOCK_DELAY,           _alct_rx_clock_delay);
  newRows.setValueAt(rowId, ALCT_TRIG_ENABLE,              _alct_trig_enable);
  newRows.setValueAt(rowId, ALCT_TX_CLOCK_DELAY,           _alct_tx_clock_delay);
  newRows.setValueAt(rowId, ALL_CFEB_ACTIVE,               _all_cfeb_active);
  newRows.setValueAt(rowId, CFEB0DELAY,                    _cfeb0delay);
  newRows.setValueAt(rowId, CFEB1DELAY,                    _cfeb1delay);
  newRows.setValueAt(rowId, CFEB2DELAY,                    _cfeb2delay);
  newRows.setValueAt(rowId, CFEB3DELAY,                    _cfeb3delay);
  newRows.setValueAt(rowId, CFEB4DELAY,                    _cfeb4delay);
  newRows.setValueAt(rowId, CFEB_ENABLE_SOURCE,            _cfeb_enable_source);
  newRows.setValueAt(rowId, CLCT_BLANKING,                 _clct_blanking);
  newRows.setValueAt(rowId, CLCT_BX0_DELAY,                _clct_bx0_delay);
  newRows.setValueAt(rowId, CLCT_DRIFT_DELAY,              _clct_drift_delay);
  newRows.setValueAt(rowId, CLCT_EXT_PRETRIG_ENABLE,       _clct_ext_pretrig_enable);
  newRows.setValueAt(rowId, CLCT_FIFO_PRETRIG,             _clct_fifo_pretrig);
  newRows.setValueAt(rowId, CLCT_FIFO_TBINS,               _clct_fifo_tbins);
  newRows.setValueAt(rowId, CLCT_HIT_PERSIST,              _clct_hit_persist);
  newRows.setValueAt(rowId, CLCT_MIN_SEPARATION,           _clct_min_separation);
  newRows.setValueAt(rowId, CLCT_NPLANES_HIT_PATTERN,      _clct_nplanes_hit_pattern);
  newRows.setValueAt(rowId, CLCT_NPLANES_HIT_PRETRIG,      _clct_nplanes_hit_pretrig);
  newRows.setValueAt(rowId, CLCT_PID_THRESH_PRETRIG,       _clct_pid_thresh_pretrig);
  newRows.setValueAt(rowId, CLCT_PRETRIG_ENABLE,           _clct_pretrig_enable);
  newRows.setValueAt(rowId, CLCT_STAGGER,                  _clct_stagger);
  newRows.setValueAt(rowId, CLCT_THROTTLE,                 _clct_throttle);
  newRows.setValueAt(rowId, CLCT_TRIG_ENABLE,              _clct_trig_enable);
  newRows.setValueAt(rowId, CSC_CONFIG_ID,                 csc_config_id);
  newRows.setValueAt(rowId, DMB_TX_DELAY,                  _dmb_tx_delay);
  newRows.setValueAt(rowId, EMU_CONFIG_ID,                 emu_config_id_);
  newRows.setValueAt(rowId, ENABLE_ALCT_RX,                _enable_alct_rx);
  newRows.setValueAt(rowId, ENABLE_ALCT_TX,                _enable_alct_tx);
  newRows.setValueAt(rowId, ENABLECLCTINPUTS_REG42,        _enableclctinputs_reg42);
  newRows.setValueAt(rowId, ENABLECLCTINPUTS_REG68,        _enableclctinputs_reg68);
  newRows.setValueAt(rowId, IGNORE_CCB_STARTSTOP,          _ignore_ccb_startstop);
  newRows.setValueAt(rowId, L1A_ALLOW_ALCT_ONLY,           _l1a_allow_alct_only);
  newRows.setValueAt(rowId, L1A_ALLOW_MATCH,               _l1a_allow_match);
  newRows.setValueAt(rowId, L1A_ALLOW_NOL1A,               _l1a_allow_nol1a);
  newRows.setValueAt(rowId, L1A_ALLOW_NOTMB,               _l1a_allow_notmb);
  newRows.setValueAt(rowId, LAYER0_DISTRIP_HOT_CHANN_MASK, _layer0_distrip_hot_chann_mask);
  newRows.setValueAt(rowId, LAYER1_DISTRIP_HOT_CHANN_MASK, _layer1_distrip_hot_chann_mask);
  newRows.setValueAt(rowId, LAYER2_DISTRIP_HOT_CHANN_MASK, _layer2_distrip_hot_chann_mask);
  newRows.setValueAt(rowId, LAYER3_DISTRIP_HOT_CHANN_MASK, _layer3_distrip_hot_chann_mask);
  newRows.setValueAt(rowId, LAYER4_DISTRIP_HOT_CHANN_MASK, _layer4_distrip_hot_chann_mask);
  newRows.setValueAt(rowId, LAYER5_DISTRIP_HOT_CHANN_MASK, _layer5_distrip_hot_chann_mask);
  //  newRows.setValueAt(rowId, LAYER_TRIG_DELAY,              _layer_trig_delay);   // obsolete
  newRows.setValueAt(rowId, LAYER_TRIG_ENABLE,             _layer_trig_enable);
  newRows.setValueAt(rowId, LAYER_TRIG_THRESH,             _layer_trig_thresh);
  newRows.setValueAt(rowId, MATCH_PRETRIG_ALCT_DELAY,      _match_pretrig_alct_delay);
  newRows.setValueAt(rowId, MATCH_PRETRIG_ENABLE,          _match_pretrig_enable);
  newRows.setValueAt(rowId, MATCH_PRETRIG_WINDOW_SIZE,     _match_pretrig_window_size);
  newRows.setValueAt(rowId, MATCH_TRIG_ALCT_DELAY,         _match_trig_alct_delay);
  newRows.setValueAt(rowId, MATCH_TRIG_ENABLE,             _match_trig_enable);
  newRows.setValueAt(rowId, MATCH_TRIG_WINDOW_SIZE,        _match_trig_window_size);
  newRows.setValueAt(rowId, MPC_IDLE_BLANK,                _mpc_idle_blank);
  newRows.setValueAt(rowId, MPC_OUTPUT_ENABLE,             _mpc_output_enable);
  newRows.setValueAt(rowId, MPC_RX_DELAY,                  _mpc_rx_delay);
  newRows.setValueAt(rowId, MPC_SYNC_ERR_ENABLE,           _mpc_sync_err_enable);
  newRows.setValueAt(rowId, MPC_TX_DELAY,                  _mpc_tx_delay);
  newRows.setValueAt(rowId, RAT_FIRMWARE_DAY,              _rat_firmware_day);
  newRows.setValueAt(rowId, RAT_FIRMWARE_MONTH,            _rat_firmware_month);
  newRows.setValueAt(rowId, RAT_FIRMWARE_YEAR,             _rat_firmware_year);
  newRows.setValueAt(rowId, RAT_TMB_DELAY,                 _rat_tmb_delay);
  newRows.setValueAt(rowId, REQUEST_L1A,                   _request_l1a);
  newRows.setValueAt(rowId, RPC0_RAT_DELAY,                _rpc0_rat_delay);
  newRows.setValueAt(rowId, RPC0_RAW_DELAY,                _rpc0_raw_delay);
  newRows.setValueAt(rowId, RPC1_RAT_DELAY,                _rpc1_rat_delay);
  newRows.setValueAt(rowId, RPC1_RAW_DELAY,                _rpc1_raw_delay);
  newRows.setValueAt(rowId, RPC_BXN_OFFSET,                _rpc_bxn_offset);
  newRows.setValueAt(rowId, RPC_EXISTS,                    _rpc_exists);
  newRows.setValueAt(rowId, RPC_FIFO_DECOUPLE,             _rpc_fifo_decouple);
  newRows.setValueAt(rowId, RPC_FIFO_PRETRIG,              _rpc_pretrig);
  newRows.setValueAt(rowId, RPC_FIFO_TBINS,                _rpc_tbins);
  newRows.setValueAt(rowId, RPC_MASK_ALL,                  _rpc_mask_all);
  newRows.setValueAt(rowId, RPC_READ_ENABLE,               _rpc_read_enable);
  newRows.setValueAt(rowId, SLOT,                          _slot);
  newRows.setValueAt(rowId, TMB_BXN_OFFSET,                _tmb_bxn_offset);
  newRows.setValueAt(rowId, TMB_CONFIG_ID,                 _tmb_config_id);
  newRows.setValueAt(rowId, TMB_FIFO_MODE,                 _tmb_fifo_mode);
  newRows.setValueAt(rowId, TMB_FIFO_NO_RAW_HITS,          _tmb_fifo_no_raw_hits);
  newRows.setValueAt(rowId, TMB_FIRMWARE_DAY,              _tmb_firmware_day);
  newRows.setValueAt(rowId, TMB_FIRMWARE_MONTH,            _tmb_firmware_month);
  newRows.setValueAt(rowId, TMB_FIRMWARE_REVCODE,          _tmb_firmware_revcode);
  newRows.setValueAt(rowId, TMB_FIRMWARE_TYPE,             _tmb_firmware_type);
  newRows.setValueAt(rowId, TMB_FIRMWARE_VERSION,          _tmb_firmware_version);
  newRows.setValueAt(rowId, TMB_FIRMWARE_YEAR,             _tmb_firmware_year);
  newRows.setValueAt(rowId, TMB_L1A_DELAY,                 _tmb_l1a_delay);
  newRows.setValueAt(rowId, TMB_L1A_OFFSET,                _tmb_l1a_offset);
  newRows.setValueAt(rowId, TMB_L1A_WINDOW_SIZE,           _tmb_l1a_window_size);
  newRows.setValueAt(rowId, VALID_CLCT_REQUIRED,           _valid_clct_required);
  newRows.setValueAt(rowId, WRITE_BUFFER_REQUIRED,         _write_buffer_required);
  newRows.setValueAt(rowId, WRITE_BUFFER_AUTOCLEAR,        _write_buffer_autoclear);
  newRows.setValueAt(rowId, WRITE_BUFFER_CONTINOUS_ENABLE, _write_buffer_continous_enable);

  insert(connectionID,insertViewName,newRows);

  ALCTController * thisALCT = TStore_thisTMB->alctController();
  uploadALCT(connectionID, _tmb_config_id, thisALCT);

}
//

void EmuTStore::uploadALCT(const std::string &connectionID, xdata::UnsignedInteger64 &tmb_config_id, ALCTController * &TStore_thisALCT) throw (xcept::Exception) {
  
  std::string insertViewName="alct";
  xdata::Table newRows;

  size_t rowId(0);
  std::string ALCT_ACCEL_MODE("ALCT_ACCEL_MODE");
  std::string ALCT_NPLANES_HIT_ACCEL_PATTERN("ALCT_NPLANES_HIT_ACCEL_PATTERN");
  std::string ALCT_NPLANES_HIT_ACCEL_PRETRIG("ALCT_NPLANES_HIT_ACCEL_PRETRIG");
  std::string ALCT_BXN_OFFSET("ALCT_BXN_OFFSET");
  std::string ALCT_CCB_ENABLE("ALCT_CCB_ENABLE");
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string ALCT_CONFIG_IN_READOUT("ALCT_CONFIG_IN_READOUT");
  std::string ALCT_DRIFT_DELAY("ALCT_DRIFT_DELAY");
  std::string ALCT_EXT_TRIG_ENABLE("ALCT_EXT_TRIG_ENABLE");
  std::string ALCT_FIFO_MODE("ALCT_FIFO_MODE");
  std::string ALCT_FIFO_PRETRIG("ALCT_FIFO_PRETRIG");
  std::string ALCT_FIFO_TBINS("ALCT_FIFO_TBINS");
  std::string ALCT_FIRMWARE_BACKWARD_FORWARD("ALCT_FIRMWARE_BACKWARD_FORWARD");
  std::string ALCT_FIRMWARE_DAY("ALCT_FIRMWARE_DAY");
  std::string ALCT_FIRMWARE_MONTH("ALCT_FIRMWARE_MONTH");
  std::string ALCT_FIRMWARE_NEGAT_POSIT("ALCT_FIRMWARE_NEGAT_POSIT");
  std::string ALCT_FIRMWARE_YEAR("ALCT_FIRMWARE_YEAR");
  std::string ALCT_INJECT_MODE("ALCT_INJECT_MODE");
  std::string ALCT_L1A_DELAY("ALCT_L1A_DELAY");
  std::string ALCT_L1A_INTERNAL("ALCT_L1A_INTERNAL");
  std::string ALCT_L1A_OFFSET("ALCT_L1A_OFFSET");
  std::string ALCT_L1A_WINDOW_WIDTH("ALCT_L1A_WINDOW_WIDTH");
  std::string ALCT_NPLANES_HIT_PATTERN("ALCT_NPLANES_HIT_PATTERN");
  std::string ALCT_NPLANES_HIT_PRETRIG("ALCT_NPLANES_HIT_PRETRIG");
  std::string ALCT_SEND_EMPTY("ALCT_SEND_EMPTY");
  std::string ALCT_SN_SELECT("ALCT_SN_SELECT");
  std::string ALCT_TESTPULSE_AMPLITUDE("ALCT_TESTPULSE_AMPLITUDE");
  std::string ALCT_TESTPULSE_DIRECTION("ALCT_TESTPULSE_DIRECTION");
  std::string ALCT_TESTPULSE_INVERT("ALCT_TESTPULSE_INVERT");
  std::string ALCT_TRIG_INFO_EN("ALCT_TRIG_INFO_EN");
  std::string ALCT_TRIG_MODE("ALCT_TRIG_MODE");
  std::string CHAMBER_TYPE("CHAMBER_TYPE");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");

  xdata::UnsignedShort     _alct_accel_mode                = TStore_thisALCT->GetWriteAlctAmode();
  xdata::UnsignedShort     _alct_nplanes_hit_accel_pattern = TStore_thisALCT->GetWriteAcceleratorPatternThresh();
  xdata::UnsignedShort     _alct_nplanes_hit_accel_pretrig = TStore_thisALCT->GetWriteAcceleratorPretrigThresh();
  xdata::UnsignedShort     _alct_bxn_offset                = TStore_thisALCT->GetWriteBxcOffset();
  xdata::UnsignedShort     _alct_ccb_enable                = TStore_thisALCT->GetWriteCcbEnable();
  xdata::UnsignedInteger64 _alct_config_id                 = tmb_config_id + 100;
  xdata::UnsignedShort     _alct_config_in_readout         = TStore_thisALCT->GetWriteConfigInReadout();
  xdata::UnsignedShort     _alct_drift_delay               = TStore_thisALCT->GetWriteDriftDelay();
  xdata::UnsignedShort     _alct_ext_trig_enable           = TStore_thisALCT->GetWriteExtTrigEnable();
  xdata::UnsignedShort     _alct_fifo_mode                 = TStore_thisALCT->GetWriteFifoMode();
  xdata::UnsignedShort     _alct_fifo_pretrig              = TStore_thisALCT->GetWriteFifoPretrig();
  xdata::UnsignedShort     _alct_fifo_tbins                = TStore_thisALCT->GetWriteFifoTbins();
  xdata::String            _alct_firmware_backward_forward = TStore_thisALCT->Get_fastcontrol_backward_forward_type();
  xdata::UnsignedShort     _alct_firmware_day              = TStore_thisALCT->GetExpectedFastControlDay();
  xdata::UnsignedShort     _alct_firmware_month            = TStore_thisALCT->GetExpectedFastControlMonth();
  xdata::String            _alct_firmware_negat_posit      = TStore_thisALCT->Get_fastcontrol_negative_positive_type();
  xdata::UnsignedShort     _alct_firmware_year             = TStore_thisALCT->GetExpectedFastControlYear();
  xdata::UnsignedShort     _alct_inject_mode               = TStore_thisALCT->GetWriteInjectMode();
  xdata::UnsignedShort     _alct_l1a_delay                 = TStore_thisALCT->GetWriteL1aDelay();
  xdata::UnsignedShort     _alct_l1a_internal              = TStore_thisALCT->GetWriteL1aInternal();
  xdata::UnsignedShort     _alct_l1a_offset                = TStore_thisALCT->GetWriteL1aOffset();
  xdata::UnsignedShort     _alct_l1a_window_width          = TStore_thisALCT->GetWriteL1aWindowSize();
  xdata::UnsignedShort     _alct_nplanes_hit_pattern       = TStore_thisALCT->GetWritePretrigNumberOfPattern();
  xdata::UnsignedShort     _alct_nplanes_hit_pretrig       = TStore_thisALCT->GetWritePretrigNumberOfLayers();
  xdata::UnsignedShort     _alct_send_empty                = TStore_thisALCT->GetWriteSendEmpty();
  xdata::UnsignedShort     _alct_sn_select                 = TStore_thisALCT->GetWriteSnSelect();
  xdata::UnsignedShort     _alct_testpulse_amplitude       = TStore_thisALCT->GetTestpulseAmplitude();
  xdata::String            _alct_testpulse_direction       = TStore_thisALCT->Get_PulseDirection();
  xdata::String            _alct_testpulse_invert          = TStore_thisALCT->Get_InvertPulse();
  xdata::UnsignedShort     _alct_trig_info_en              = TStore_thisALCT->GetWriteTriggerInfoEnable();
  xdata::UnsignedShort     _alct_trig_mode                 = TStore_thisALCT->GetWriteTriggerMode();
  xdata::String            _chamber_type                   = TStore_thisALCT->GetChamberType();

#ifdef debugV
  cout << "-- ALCT emu_config_id --------------------- " << emu_config_id_.toString()                  << std::endl;
  cout << "-- ALCT tmb_config_id --------------------- " << tmb_config_id.toString()                   << std::endl;
  cout << "-- ALCT alct_config_id -------------------- " << _alct_config_id.toString()                 << std::endl;
  cout << "-- ALCT alct_accel_mode ------------------- " << _alct_accel_mode.toString()                << std::endl;
  cout << "-- ALCT alct_nplanes_hit_accel_pattern ---- " << _alct_nplanes_hit_accel_pattern.toString() << std::endl;
  cout << "-- ALCT alct_nplanes_hit_accel_pretrig ---- " << _alct_nplanes_hit_accel_pretrig.toString() << std::endl;
  cout << "-- ALCT alct_bxn_offset ------------------- " << _alct_bxn_offset.toString()                << std::endl;
  cout << "-- ALCT alct_ccb_enable ------------------- " << _alct_ccb_enable.toString()                << std::endl;
  cout << "-- ALCT alct_config_in_readout ------------ " << _alct_config_in_readout.toString()         << std::endl;
  cout << "-- ALCT alct_drift_delay ------------------ " << _alct_drift_delay.toString()               << std::endl;
  cout << "-- ALCT alct_ext_trig_enable -------------- " << _alct_ext_trig_enable.toString()           << std::endl;
  cout << "-- ALCT alct_fifo_mode -------------------- " << _alct_fifo_mode.toString()                 << std::endl;
  cout << "-- ALCT alct_fifo_pretrig ----------------- " << _alct_fifo_pretrig.toString()              << std::endl;
  cout << "-- ALCT alct_fifo_tbins ------------------- " << _alct_fifo_tbins.toString()                << std::endl;
  cout << "-- ALCT alct_firmware_backward_forward ---- " << _alct_firmware_backward_forward.toString() << std::endl;
  cout << "-- ALCT alct_firmware_day ----------------- " << _alct_firmware_day.toString()              << std::endl;
  cout << "-- ALCT alct_firmware_month --------------- " << _alct_firmware_month.toString()            << std::endl;
  cout << "-- ALCT alct_firmware_negat_posit --------- " << _alct_firmware_negat_posit.toString()      << std::endl;
  cout << "-- ALCT alct_firmware_year ---------------- " << _alct_firmware_year.toString()             << std::endl;
  cout << "-- ALCT alct_inject_mode ------------------ " << _alct_inject_mode.toString()               << std::endl;
  cout << "-- ALCT alct_l1a_delay -------------------- " << _alct_l1a_delay.toString()                 << std::endl;
  cout << "-- ALCT alct_l1a_internal ----------------- " << _alct_l1a_internal.toString()              << std::endl;
  cout << "-- ALCT alct_l1a_offset ------------------- " << _alct_l1a_offset.toString()                << std::endl;
  cout << "-- ALCT alct_l1a_window_width ------------- " << _alct_l1a_window_width.toString()          << std::endl;
  cout << "-- ALCT alct_nplanes_hit_pattern ---------- " << _alct_nplanes_hit_pattern.toString()       << std::endl;
  cout << "-- ALCT alct_nplanes_hit_pretrig ---------- " << _alct_nplanes_hit_pretrig.toString()       << std::endl;
  cout << "-- ALCT alct_send_empty ------------------- " << _alct_send_empty.toString()                << std::endl;
  cout << "-- ALCT alct_sn_select -------------------- " << _alct_sn_select.toString()                 << std::endl;
  cout << "-- ALCT alct_testpulse_amplitude ---------- " << _alct_testpulse_amplitude.toString()       << std::endl;
  cout << "-- ALCT alct_testpulse_direction ---------- " << _alct_testpulse_direction.toString()       << std::endl;
  cout << "-- ALCT alct_testpulse_invert ------------- " << _alct_testpulse_invert.toString()          << std::endl;
  cout << "-- ALCT alct_trig_info_en ----------------- " << _alct_trig_info_en.toString()              << std::endl;
  cout << "-- ALCT alct_trig_mode -------------------- " << _alct_trig_mode.toString()                 << std::endl;
  cout << "-- ALCT chamber_type ---------------------- " << _chamber_type.toString()                   << std::endl;
#endif

  newRows.clear();
  newRows = tableDefinition_emu_alct;
  
  newRows.setValueAt(rowId, ALCT_ACCEL_MODE,                _alct_accel_mode);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_ACCEL_PATTERN, _alct_nplanes_hit_accel_pattern);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_ACCEL_PRETRIG, _alct_nplanes_hit_accel_pretrig);
  newRows.setValueAt(rowId, ALCT_BXN_OFFSET,                _alct_bxn_offset);
  newRows.setValueAt(rowId, ALCT_CCB_ENABLE,                _alct_ccb_enable);
  newRows.setValueAt(rowId, ALCT_CONFIG_ID,                 _alct_config_id);
  newRows.setValueAt(rowId, ALCT_CONFIG_IN_READOUT,         _alct_config_in_readout);
  newRows.setValueAt(rowId, ALCT_DRIFT_DELAY,               _alct_drift_delay);
  newRows.setValueAt(rowId, ALCT_EXT_TRIG_ENABLE,           _alct_ext_trig_enable);
  newRows.setValueAt(rowId, ALCT_FIFO_MODE,                 _alct_fifo_mode);
  newRows.setValueAt(rowId, ALCT_FIFO_PRETRIG,              _alct_fifo_pretrig);
  newRows.setValueAt(rowId, ALCT_FIFO_TBINS,                _alct_fifo_tbins);
  newRows.setValueAt(rowId, ALCT_FIRMWARE_BACKWARD_FORWARD, _alct_firmware_backward_forward);
  newRows.setValueAt(rowId, ALCT_FIRMWARE_DAY,              _alct_firmware_day);
  newRows.setValueAt(rowId, ALCT_FIRMWARE_MONTH,            _alct_firmware_month);
  newRows.setValueAt(rowId, ALCT_FIRMWARE_NEGAT_POSIT,      _alct_firmware_negat_posit);
  newRows.setValueAt(rowId, ALCT_FIRMWARE_YEAR,             _alct_firmware_year);
  newRows.setValueAt(rowId, ALCT_INJECT_MODE,               _alct_inject_mode);
  newRows.setValueAt(rowId, ALCT_L1A_DELAY,                 _alct_l1a_delay);
  newRows.setValueAt(rowId, ALCT_L1A_INTERNAL,              _alct_l1a_internal);
  newRows.setValueAt(rowId, ALCT_L1A_OFFSET,                _alct_l1a_offset);
  newRows.setValueAt(rowId, ALCT_L1A_WINDOW_WIDTH,          _alct_l1a_window_width);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_PATTERN,       _alct_nplanes_hit_pattern);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_PRETRIG,       _alct_nplanes_hit_pretrig);
  newRows.setValueAt(rowId, ALCT_SEND_EMPTY,                _alct_send_empty);
  newRows.setValueAt(rowId, ALCT_SN_SELECT,                 _alct_sn_select);
  newRows.setValueAt(rowId, ALCT_TESTPULSE_AMPLITUDE,       _alct_testpulse_amplitude);
  newRows.setValueAt(rowId, ALCT_TESTPULSE_DIRECTION,       _alct_testpulse_direction);
  newRows.setValueAt(rowId, ALCT_TESTPULSE_INVERT,          _alct_testpulse_invert);
  newRows.setValueAt(rowId, ALCT_TRIG_INFO_EN,              _alct_trig_info_en);
  newRows.setValueAt(rowId, ALCT_TRIG_MODE,                 _alct_trig_mode);
  newRows.setValueAt(rowId, CHAMBER_TYPE,                   _chamber_type);
  newRows.setValueAt(rowId, EMU_CONFIG_ID,                  emu_config_id_);
  newRows.setValueAt(rowId, TMB_CONFIG_ID,                  tmb_config_id);

  insert(connectionID,insertViewName,newRows);

  uploadAnodeChannel(connectionID, _alct_config_id, TStore_thisALCT);

}
//

void EmuTStore::uploadAnodeChannel(const std::string &connectionID, xdata::UnsignedInteger64 &alct_config_id, ALCTController * &TStore_thisALCT) throw (xcept::Exception) {
  
  std::string insertViewName="anodechannel";
  xdata::Table newRows;
  
  newRows = tableDefinition_emu_anodechannel;

  size_t rowId(0);
  std::string AFEB_CONFIG_ID("AFEB_CONFIG_ID");
  std::string AFEB_FINE_DELAY("AFEB_FINE_DELAY");
  std::string AFEB_NUMBER("AFEB_NUMBER");
  std::string AFEB_THRESHOLD("AFEB_THRESHOLD");
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  int maxUserIndex = TStore_thisALCT->MaximumUserIndex();

  for(int afeb = 0; afeb <= maxUserIndex; afeb++) {
    xdata::UnsignedShort     _afeb_fine_delay = TStore_thisALCT->GetWriteAsicDelay(afeb);
    xdata::UnsignedShort     _afeb_number     = afeb+1;  // in XML it starts with 1 but the setter function uses 'number-1'; this way the XML file and the DB are consistent
    xdata::UnsignedShort     _afeb_threshold  = TStore_thisALCT->GetAfebThresholdDAC(afeb);
    xdata::UnsignedInteger64 _afeb_config_id  = alct_config_id + (afeb+1);

#ifdef debugV
    cout << "-- AFEB emu_config_id ---------- " << emu_config_id_.toString()      << std::endl;
    cout << "-- AFEB alct_config_id --------- " << alct_config_id.toString()      << std::endl;
    cout << "-- AFEB afeb_config_id --------- " << _afeb_config_id.toString()     << std::endl;
    cout << "-- AFEB afeb_fine_delay -------- " << _afeb_fine_delay.toString()    << std::endl;
    cout << "-- AFEB afeb_number ------------ " << _afeb_number.toString()        << std::endl;
    cout << "-- AFEB afeb_threshold --------- " << _afeb_threshold.toString()     << std::endl;
#endif
  
    newRows.setValueAt(rowId, AFEB_CONFIG_ID,  _afeb_config_id);  
    newRows.setValueAt(rowId, AFEB_FINE_DELAY, _afeb_fine_delay); 
    newRows.setValueAt(rowId, AFEB_NUMBER,     _afeb_number); 
    newRows.setValueAt(rowId, AFEB_THRESHOLD,  _afeb_threshold); 
    newRows.setValueAt(rowId, ALCT_CONFIG_ID,  alct_config_id);
    newRows.setValueAt(rowId, EMU_CONFIG_ID,   emu_config_id_);
    rowId++;
  }

  insert(connectionID,insertViewName,newRows);

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
      std::cout << *column + ": " + StrgValue << std::endl;
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
	while(pos!=string::npos){
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
  int IntValue;

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
  int IntValue;

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
  int IntValue;

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
  int IntValue;
  long int LongIntValue;
  float FloatValue;
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
      if (*column == "TMB_FIFO_TBINS"               ) {tmb_->SetFifoTbins(IntValue);                 }
      if (*column == "CLCT_FIFO_PRETRIG"            ) {tmb_->SetFifoPreTrig(IntValue);               }
      if (*column == "MPC_SYNC_ERR_ENABLE"          ) {tmb_->SetTmbSyncErrEnable(IntValue);          }
      if (*column == "ALCT_TRIG_ENABLE"             ) {tmb_->SetTmbAllowAlct(IntValue);              }
      if (*column == "CLCT_TRIG_ENABLE"             ) {tmb_->SetTmbAllowClct(IntValue);              }
      if (*column == "MATCH_TRIG_ENABLE"            ) {tmb_->SetTmbAllowMatch(IntValue);             }
      if (*column == "MPC_RX_DELAY"                 ) {tmb_->SetMpcRxDelay(IntValue);                }
      if (*column == "MPC_IDLE_BLANK"               ) {tmb_->SetMpcIdleBlank(IntValue);              }
      if (*column == "MPC_OUTPUT_ENABLE"            ) {tmb_->SetMpcOutputEnable(IntValue);           }
      if (*column == "WRITE_BUFFER_REQUIRED"        ) {tmb_->SetWriteBufferRequired(IntValue);       }
      if (*column == "VALID_CLCT_REQUIRED"          ) {tmb_->SetRequireValidClct(IntValue);          }
      if (*column == "L1A_ALLOW_MATCH"              ) {tmb_->SetL1aAllowMatch(IntValue);             }
      if (*column == "L1A_ALLOW_NOTMB"              ) {tmb_->SetL1aAllowNoTmb(IntValue);             }
      if (*column == "L1A_ALLOW_NOL1A"              ) {tmb_->SetL1aAllowNoL1a(IntValue);             }
      if (*column == "L1A_ALLOW_ALCT_ONLY"          ) {tmb_->SetL1aAllowAlctOnly(IntValue);          }
      if (*column == "LAYER_TRIG_ENABLE"            ) {tmb_->SetEnableLayerTrigger(IntValue);        }
      if (*column == "LAYER_TRIG_THRESH"            ) {tmb_->SetLayerTriggerThreshold(IntValue);     }
      if (*column == "CLCT_BLANKING"                ) {tmb_->SetClctBlanking(IntValue);              }
      if (*column == "CLCT_STAGGER"                 ) {tmb_->SetClctStagger(IntValue);               }
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
  int IntValue;
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
  int IntValue;
  std::string StrgValue;
  int afeb_number;

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
