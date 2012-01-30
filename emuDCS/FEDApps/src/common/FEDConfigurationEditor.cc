#include "emu/fed/FEDConfigurationEditor.h"
#include "emu/db/TStoreRequest.h"

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
#include "xdata/Double.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/SimpleType.h"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xoap/domutils.h"
#include "toolbox/Runtime.h"
#include "xoap/DOMParserFactory.h"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/case_conv.hpp"

#include "xercesc/dom/DOMNode.hpp"

#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/LoadDOM.h"
#include "tstore/client/Client.h"



XDAQ_INSTANTIATOR_IMPL(emu::fed::FEDConfigurationEditor)

  namespace emu {
    namespace fed {

  FEDConfigurationEditor::FEDConfigurationEditor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception)
	: emu::db::ConfigurationEditor(s)
{
	setTableNamePrefix("EMU_FED_");
	setTopLevelTableName("crate");
	setXMLRootElement("FEDSystem");
	setDisplayBooleansAsIntegers(true);
	setViewID("urn:tstore-view-SQL:EMUFEDsystem");
	setConfigurationDirectory("fed");
	
	getApplicationInfoSpace()->fireItemAvailable("dbUsername", &dbUsername_);
	getApplicationInfoSpace()->fireItemAvailable("dbPassword", &dbPassword_);
	setHumanReadableConfigName("FED Crate");
	addTable("controller");
	addTable("ddu");
	addTable("dcc");
	addChildTable("ddu","fiber");
	addChildTable("dcc","fifo");
	
	// xmlPath is now superfluous, because the selector tells the application the full pathname
	xmlpath_ = "";
}

void FEDConfigurationEditor::fillRootElement(DOMElement *rootElement) {
	rootElement->setAttribute(xoap::XStr("NAME"),xoap::XStr(endcapSide_.toString()));
}

void FEDConfigurationEditor::outputStandardInterface(xgi::Output * out) {

  *out << cgicc::fieldset().set("style","font-size: 10pt;  font-family: arial;") << std::endl;

  *out << cgicc::table().set("border","0").set("cellpadding","10");

  *out << cgicc::tr();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/parse",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Parse XML file").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/exportAsXML",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Export as XML").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
	
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/upload",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Upload to DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
	/*
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/read",getApplicationDescriptor()->getURN().c_str())) << std::endl;
	  *out << "<table border=\"1\"><tr><td>";
 	outputEndcapSelector(out);

	*out << "</td><td>" << cgicc::input().set("type","submit").set("value","Read from DB").set("style", "width:120px;") << "</td></tr></table>" << std::endl;
  *out << cgicc::form() << cgicc::td() << std::endl;*/
  
   *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/selectVersion",getApplicationDescriptor()->getURN().c_str())) << std::endl;
	  *out << "<table border=\"1\"><tr><td>";
 	outputEndcapSelector(out);

	*out << "</td><td>" << cgicc::input().set("type","submit").set("value","Read version...").set("style", "width:120px;") << "</td></tr></table>" << std::endl;
  *out << cgicc::form() << cgicc::td() << std::endl;
  
  
  
    *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/selectVersions",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << "<table border=\"1\"><tr><td>";
	outputEndcapSelector(out);
	*out << "</td><td>" << cgicc::input().set("type","submit").set("value","Compare versions...") << "</td></tr></table>" <<  std::endl;
	*out << cgicc::form() << std::endl;
  
  *out << cgicc::td();
  
  //This recreates the tables in the database, it only needs to be done for the initial setup and if the database structure changes,
  //so we don't want to do it by accident.
 
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/sync",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Sync to DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
  
  *out << cgicc::tr();

  *out << cgicc::table();

  *out << "<hr>";
  *out << "XML filename: ";
  *out << cgicc::input().set("type","text").set("value",xmlfile_).set("style", "width:585px;").set("DISABLED") << cgicc::br() << std::endl;
   //
  *out << cgicc::fieldset() << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Select XML file").set("style","color:blue") << std::endl ;
  
  outputFileSelector(out);
  
  //
  *out << cgicc::fieldset() << cgicc::br();
  *out << "Messages:<br>";
//  
  //
}

//gets the column linking this table to its 'parent'. parentColumn will be set to the column name in the parent table, childColumn will be set to the column name in the child table which should be set to the same value
void FEDConfigurationEditor::getForeignKey(const std::string &childTableName,std::string &parentColumn,std::string &childColumn) throw (xcept::Exception) {
	//there are not many options for this, so I'll just go through them one by one.
	if (childTableName=="fiber") {
		parentColumn=childColumn="RUI";
	} else if (childTableName=="fifo") {
		parentColumn=childColumn="FMM_ID";
	} else {
		parentColumn=childColumn="CRATE_NUMBER";
	}
}

void FEDConfigurationEditor::fillDatabaseOnlyValues(const std::string &tableName,const std::string &prefix,const std::string &globalKeyColumn,xdata::Serializable &keyValue,const std::string &otherKeyColumn,xdata::Serializable *otherValue) {
	std::map<std::string,xdata::Table> &tables=currentTables[tableName];
	std::map<std::string,xdata::Table>::iterator firstTable,lastTable;
	getRangeOfTables(prefix,tables,firstTable,lastTable);

	for(std::map<std::string,xdata::Table>::iterator table=firstTable;table!=lastTable; ++table) {
		unsigned int rowCount=(*table).second.getRowCount();
		for (unsigned rowIndex=0; rowIndex<rowCount; rowIndex++ ) {
			//set the key in each row
			std::cout << "setting " << globalKeyColumn  << " to " << keyValue.toString() << " in " << tableName << std::endl;
			(*table).second.setValueAt(rowIndex, globalKeyColumn, keyValue);
			if (!otherKeyColumn.empty() && otherValue) {
				std::cout << "setting " << otherKeyColumn << " to " << otherValue->toString() << std::endl;
				(*table).second.setValueAt(rowIndex, otherKeyColumn,*otherValue);
			}
			//(*table).second.setValueAt(rowIndex, "ID",uniqueIdentifierForRow(tableName,table,rowIndex,keyValue));
			//and do the same in any child records
			std::vector<std::string> subTables;
			if (tableName==topLevelTableName_ || tableNames.count(tableName)) {
				if (tableName==topLevelTableName_) subTables=topLevelTables;
				else subTables=tableNames[tableName];
				for (std::vector<std::string>::iterator subTable=subTables.begin();subTable!=subTables.end();++subTable) {
					std::string sourceColumn,destinationColumn;
					getForeignKey(*subTable,sourceColumn,destinationColumn);
					fillDatabaseOnlyValues(*subTable,(*table).first,globalKeyColumn, keyValue,destinationColumn,(*table).second.getValueAt(rowIndex, sourceColumn));
				}
			}
		}
		//setCachedTable(tableName,(*table).first,(*table).second);
	}
}

void FEDConfigurationEditor::uploadTable(const std::string &connectionID,const std::string &tableName) {
	std::map<std::string,xdata::Table> &tables=currentTables[tableName];
	std::cout << "inserting " << tableName << std::endl;
	for(std::map<std::string,xdata::Table>::iterator table=tables.begin();table!=tables.end(); ++table) {
	        std::cout << "inserting " << tableName << " " << (*table).first << std::endl;
		insert(connectionID,tableName,(*table).second);
	}
	if (tableNames.count(tableName)) {
		std::vector<std::string> subTables;
		subTables=tableNames[tableName];
		for (std::vector<std::string>::iterator subTable=subTables.begin();subTable!=subTables.end();++subTable) {
			uploadTable(connectionID,*subTable);
		}
	}
}


void FEDConfigurationEditor::readFibers(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &rui)
throw (xcept::Exception) {
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["RUI"] = rui.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		query(connectionID,"get_fibers_by_key_rui", parameters,result);
		setCachedTable("fiber",parentIdentifier,result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
}

void FEDConfigurationEditor::readDDU(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
throw (emu::fed::exception::DBException) {
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"]=crateNumber.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		query(connectionID,"get_ddus_by_key_crate", parameters,result);
		//setCachedTable("ddu",parentIdentifier,result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	if (!result.getRowCount()) {
		XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
	}
	int rowIndex=0;
	for (xdata::Table::iterator iRow = result.begin(); iRow != result.end(); ++iRow,++rowIndex) {			
		xdata::UnsignedShort *rui=dynamic_cast<xdata::UnsignedShort *>(iRow->getField("RUI"));
		if (rui) {
			xdata::Table justThisRow(result.getTableDefinition());
			justThisRow.insert(*iRow);
			//in theory could use parentIdentifier+" "+uniqueIdentifierForRow(result,rowIndex), instead oc directly RUI, but this picks the crate number as unique ID... this should be fixed somehow
			//however, this is not a problem in the more generic code (which uniqueIdentifierForRow was made for) to read from the XML since the crate number does not exist in the DDU in the XML
			setCachedTable("ddu",parentIdentifier+" RUI "+rui->toString(),justThisRow);
			readFibers(connectionID,parentIdentifier+" RUI "+rui->toString(),key,*rui);
		} else {
			XCEPT_RAISE(emu::fed::exception::DBException,"rui is of type "+rui->type()+", expected unsigned short");
		}
	}
}

void FEDConfigurationEditor::readFIFOs(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedInteger &fmm_id)
throw (xcept::Exception) {
		// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["FMM_ID"] = fmm_id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		query(connectionID,"get_fifos_by_key_fmmid", parameters,result);
		setCachedTable("fifo",parentIdentifier,result);
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(xcept::Exception, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
		case 0: 
			XCEPT_RAISE(xcept::Exception, "No matching rows found");
			break;
		default:
			break;
	}

}

void FEDConfigurationEditor::readDCC(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
throw (xcept::Exception) {
		// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"] = crateNumber.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		query(connectionID,"get_dccs_by_key_crate", parameters,result);
		setCachedTable("dcc",parentIdentifier,result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
		case 0: 
			XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
			break;
		case 1:
			break;
		default:
			XCEPT_RAISE(emu::fed::exception::DBException, "More than one matching row found");
			break;
	}
	
	try {
		for (xdata::Table::iterator iRow = result.begin(); iRow != result.end(); ++iRow) {
			xdata::UnsignedInteger *fmm_id=dynamic_cast<xdata::UnsignedInteger *>(iRow->getField("FMM_ID"));
			if (fmm_id) {
				readFIFOs(connectionID,parentIdentifier,key,*fmm_id);
			} else {
				XCEPT_RAISE(emu::fed::exception::DBException,"fmm_id is of type "+fmm_id->type()+", expected unsigned int");
			}
		}
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(xcept::Exception, "Error finding columns", e);
	}
}


void FEDConfigurationEditor::readController(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException) {
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["CRATE_NUMBER"] = number.toString();
	parameters["KEY"]=key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		query(connectionID,"get_controller_by_key_crate", parameters,result);
		setCachedTable("controller",parentIdentifier,result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
	case 0: 
		XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
		break;
	case 1:
		break;
	default:
		XCEPT_RAISE(emu::fed::exception::DBException, "More than one matching row found");
		break;
	}
}

void FEDConfigurationEditor::readConfigFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
	//read the key? No, we will already have that
	//read the crates
	//for each crate, read other stuff. Simple!
	//in fact, let's look at what is done already in the other application.
	std::map<std::string, std::string> parameters;
  	cgicc::Cgicc cgi(in);
	std::string key=cgi("configID");
	parameters["KEY"] = key;
	
	
	try {
		
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(xgi::exception::Exception, "Error reading crates", e);
	}
	
	// Execute the query
	xdata::Table crates;
	try {
		std::string connectionID=connect();
		clearCachedTables();
		
		// The system names are stored in the EMU_FED_CONFIGURATIONS table under "DESCRIPTION"
		xdata::Table configurations;
		std::map<std::string, std::string> configParams = parameters;
		configParams["TABLE"] = "EMU_FED_CONFIGURATIONS";
		query(connectionID, "get_all_by_key", configParams, configurations);
		//std::cout << "found " << configurations.getRowCount() << " rows" << std::endl;	
		xdata::String *endcapSide = dynamic_cast<xdata::String *>(configurations.getValueAt(0, "DESCRIPTION"));
		if (endcapSide) {
			endcapSide_ = *endcapSide;
		} else {
			XCEPT_RAISE(xgi::exception::Exception, "Could not determine endcap side");
		}
		
		query(connectionID,"crates", parameters,crates);
		std::cout << "found " << crates.getRowCount() << " rows" << std::endl;	
		
		for (xdata::Table::iterator iRow = crates.begin(); iRow != crates.end(); ++iRow) {
			std::cout << "a row" << std::endl;
			// Parse out the ID and crate number
			xdata::UnsignedInteger64 id;
			std::cout << "crate number type is " << iRow->getField("CRATE_NUMBER")->type() << std::endl;
			xdata::UnsignedShort *number=dynamic_cast<xdata::UnsignedShort *>(iRow->getField("CRATE_NUMBER"));
			if (number) {
				id.fromString(key); 
				std::string crateNumber=crateIdentifierString(number->toString());
				xdata::Table justThisRow(crates.getTableDefinition());
				justThisRow.insert(*iRow);
				setCachedTable("crate",crateNumber,justThisRow);
				
				//now load VMEs, DDUs, DCCs and FIFOs.
				readController(connectionID,crateNumber,id,*number);
				
				readDDU(connectionID,crateNumber,id,*number);
				readDCC(connectionID,crateNumber,id,*number);
			}
		}
	    disconnect(connectionID);
	    outputHeader(out);
	    outputStandardInterface(out);
	    outputCurrentConfiguration(out);
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(xgi::exception::Exception, "Error reading crates", e);
	}
}

void FEDConfigurationEditor::uploadCrates(const std::string &connectionID,xdata::UnsignedInteger64 &key,xdata::String &endcapSide) throw (xcept::Exception) {
	fillDatabaseOnlyValues(topLevelTableName_,"","KEY",key,"SYSTEM_NAME",&endcapSide);
	uploadTable(connectionID,topLevelTableName_);
	for (std::vector<std::string>::iterator tableName=topLevelTables.begin();tableName!=topLevelTables.end();++tableName) {
		//fillDatabaseOnlyValues(*tableName,"","KEY",key);
		uploadTable(connectionID,*tableName);
	}
}

void FEDConfigurationEditor::queryMaxId(const std::string &connectionID, const std::string &tableName, const std::string &dbColumn, xdata::UnsignedInteger64 &result) throw (xcept::Exception) {
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass=tstoreclient::classNameForView(viewID_);
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	emu::db::TStoreRequest request("query",viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	request.addViewSpecificParameter("name","getMax");
	request.addViewSpecificParameter("column",dbColumn);
	request.addViewSpecificParameter("table",tableName);

	xoap::MessageReference message=request.toSOAP();
	xoap::MessageReference response=sendSOAPMessage(message);
	
	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	xdata::Table results;
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
	xdata::Serializable *resultPointer=NULL;
	if (!tableHasColumn(results,dbColumn)) {
		XCEPT_RAISE (xcept::Exception, "results have no "+dbColumn+" column");
	}
	std::cout << "1" << std::endl;
	if (results.getRowCount()!=1) {
		XCEPT_RAISE (xcept::Exception, "Not expected number of rows");
	}
	std::cout << "2" << std::endl;
	if ((resultPointer=/*dynamic_cast<xdata::String *>(*/results.getValueAt(0,dbColumn)/*)*/)) {
		//result will be returned as a string because TStore does not know which table/column it comes from so can't be sure it'll fit into an unsigned int 64
	std::cout << "3" << std::endl;
		result.fromString(resultPointer->toString());//=*resultPointer;
	} else {
		XCEPT_RAISE (xcept::Exception, "Data is of wrong type "+results.getValueAt(0,dbColumn)->type());
	}
}


 xdata::UnsignedInteger64 FEDConfigurationEditor::getNext(const std::string &connectionID,const std::string &columnName,const std::string tableName) {
	 xdata::Table results;
	 xdata::UnsignedInteger64 maxID;
	queryMaxId(connectionID,tableName,columnName,maxID);
	 return xdata::UnsignedInteger64((xdata::UnsignedInteger64T)maxID+1);
 }

//adds a row to the EMU_FED_SYSTEMS table and returns the new key
  xdata::UnsignedInteger64 FEDConfigurationEditor::addNewSystem(const std::string &connectionID,xdata::UnsignedInteger64 &configID,xdata::String &endcapSide,const std::string &hostname)  throw (xcept::Exception) {
	xdata::UnsignedInteger64 nextID=getNext(connectionID,"ID","EMU_FED_SYSTEMS"); 
	std::string tableName="system";
	getDefinition(connectionID,tableName);
	xdata::Table newRow=tableDefinitions[tableName];
	newRow.setValueAt(0,"KEY",configID);
	newRow.setValueAt(0,"ID",nextID);
	//need to make variables because setValueAt does not accept const references
	//xdata::String endcapSideXData=endcapSide;
	xdata::String hostnameXData=hostname;
	newRow.setValueAt(0,"NAME",endcapSide);
	newRow.setValueAt(0,"HOSTNAME",hostnameXData);
	insert(connectionID,tableName,newRow);
	return configID;
  }

  
//adds a row to the EMU_FED_CONFIGURATIONS table and returns the new ID
  xdata::UnsignedInteger64 FEDConfigurationEditor::addNewConfiguration(const std::string &connectionID,const std::string &description) {
	xdata::UnsignedInteger64 ID=getNext(connectionID,"ID","EMU_FED_CONFIGURATIONS"); 
	std::string tableName="configuration";
	getDefinition(connectionID,tableName);
	xdata::Table newRow=tableDefinitions[tableName];
	newRow.setValueAt(0,"ID",ID);
	xdata::String descriptionXData=description;
	newRow.setValueAt(0,"DESCRIPTION",descriptionXData);
	xdata::TimeVal now=toolbox::TimeVal::gettimeofday();
	newRow.setValueAt(0,"TIMESTAMP",now);
	insert(connectionID,tableName,newRow);
	return ID;
  }

void FEDConfigurationEditor::startUpload(xgi::Input *in) throw (xcept::Exception) {
try {
	std::string connectionID=connect();
	getTableDefinitions(connectionID);
  	cgicc::Cgicc cgi(in);
	
	//std::string endcapSide=endcapSide_;//cgi("side");
	std::string description=cgi("description");
	std::string hostname=cgi("hostname");
	
	xdata::UnsignedInteger64 configID=addNewConfiguration(connectionID,description);
	xdata::UnsignedInteger64 key=addNewSystem(connectionID,configID,endcapSide_,hostname);
	emu_config_id_=key;
	uploadCrates(connectionID,key,endcapSide_);
	disconnect(connectionID);
  } catch (xcept::Exception &e) {
  	 LOG4CPLUS_WARN(this->getApplicationLogger(),e.what()+(std::string)" Stop loading to database...");
  	 XCEPT_RETHROW(xcept::Exception,"Problem uploading data to database",e);
  }
  
}

void FEDConfigurationEditor::getDbUserData(){

  setUsername(dbUsername_.toString());
	setPassword(dbPassword_.toString());
}

std::string FEDConfigurationEditor::elementNameFromTableName(const std::string &tableName) {
	if (tableName=="crate") return "FEDCrate";
	if (tableName=="controller") return "VMEController";
	if (tableName=="fiber") return "Fiber";
	return toolbox::toupper(tableName);
}

void FEDConfigurationEditor::readChildNodesIntoTable(const std::string &tableName,DOMElement *parent,const std::string &parentIdentifier) {
	DOMNodeList *children=parent->getElementsByTagName(xoap::XStr(elementNameFromTableName(tableName)));
	int childrenCount=children->getLength();
	/*if (childrenCount!=1) {
		std::ostringstream error;
		error << "Exactly one VMEController element must exist as a child element of every FEDCrate element in the XML document " << xmlname;
		XCEPT_RAISE(xgi::exception::Exception, error.str());
	}*/
	
	xdata::Table table=tableDefinitions[tableName];
	bool tableCanHaveChildren=false;
	if (tableNames.count(tableName)) tableCanHaveChildren=tableNames[tableName].size();
	for (int childIndex=0; childIndex<childrenCount;childIndex++) {
		xercesc::DOMElement *child=dynamic_cast<xercesc::DOMElement *>(children->item(childIndex));
		if (child) {
			//xdata::Table table=tableDefinitions[tableName];
			std::string childIdentifier=copyAttributesToTable(table,tableName,child,tableCanHaveChildren?0:childIndex);
			
			//set crateNumber to some property of parent
			std::string fullIdentifier=parentIdentifier;
			if (!parentIdentifier.empty()) fullIdentifier+=" ";
			fullIdentifier+=childIdentifier;
			std::vector<std::string> subTables;
			if (tableNames.count(tableName)) {
				if (tableCanHaveChildren) {
					//if this has child tables, we need to make sure that each row is cached separately
					setCachedTable(tableName,fullIdentifier,table);
					table=tableDefinitions[tableName];
				}
				subTables=tableNames[tableName];
				for (std::vector<std::string>::iterator subTable=subTables.begin();subTable!=subTables.end();++subTable) {
					readChildNodesIntoTable(*subTable,child,fullIdentifier);
				}
			}
		}
	}
	if (!tableCanHaveChildren) setCachedTable(tableName,parentIdentifier,table);
}

void FEDConfigurationEditor::parseConfigFromXML(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
	getTableDefinitionsIfNecessary();
	clearCachedTables();
	DOMDocument* pDoc=NULL;
	std::string xmlname = xmlpath_ + xmlfile_;
	try {
		std::cout << "configuring from " << xmlname;

		pDoc = xoap::getDOMParserFactory()->get("configure")->loadXML(tstoreclient::parsePath(xmlname)); //The parser owns the returned DOMDocument. It will be deleted when the parser is released.
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(xgi::exception::Exception,"Could not parse document at "+xmlname,e);
	}
	xercesc::DOMElement *pFEDSystem = (xercesc::DOMElement *) pDoc->getFirstChild();
	
	// Trick to ignore comments
	while (pFEDSystem->getNodeType() == xercesc::DOMNode::COMMENT_NODE) {
		pFEDSystem = (xercesc::DOMElement *) pFEDSystem->getNextSibling();
	}
	
	if (pFEDSystem == NULL) {
		//delete parser;
		//xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "Could not find a top-node in the XML document " << xmlname;
		XCEPT_RAISE(xgi::exception::Exception, error.str());
	}
	
	if ( xoap::XMLCh2String(pFEDSystem->getTagName())!=std::string( "FEDSystem") ) {
		//delete parser;
		//xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "The top-node in the XML document " << xmlname << " was not named 'FEDSystem'";
		XCEPT_RAISE(xgi::exception::Exception, error.str());
	}
	
	// get the system name (this should be the endcap side, if I understand correctly)
	std::string tempName(xoap::XMLCh2String(pFEDSystem->getAttribute(xoap::XStr("NAME"))));
	endcapSide_ = xdata::String(tempName == "" ? "unnamed" : tempName);
	
	// Parse everything one element at a time.
	
	// Get Crates and parse
	xercesc::DOMNodeList *pFEDCrates = pFEDSystem->getElementsByTagName(xoap::XStr("FEDCrate"));
	
	if (pFEDCrates == NULL) {
		//delete parser;
		//xercesc::XMLPlatformUtils::Terminate();
		std::ostringstream error;
		error << "No FEDCrate elements in the XML document " << xmlname;
		XCEPT_RAISE(xgi::exception::Exception, error.str());
	}
	
	for (unsigned int iFEDCrate = 0; iFEDCrate < pFEDCrates->getLength(); iFEDCrate++) {
		
		xercesc::DOMElement *pFEDCrate = (xercesc::DOMElement *) pFEDCrates->item(iFEDCrate);
		try {
			xdata::Table crate=tableDefinitions["crate"];
			std::string crateNumber=copyAttributesToTable(crate,"crate",pFEDCrate,0);//crate.getValueAt(0,"CRATE_NUMBER")->toString();
			setCachedTable("crate",crateNumber,crate);
			readChildNodesIntoTable("controller",pFEDCrate,crateNumber);
			readChildNodesIntoTable("ddu",pFEDCrate,crateNumber);
			readChildNodesIntoTable("dcc",pFEDCrate,crateNumber);
		} catch (xcept::Exception &e) {
		//	delete parser;
		//	xercesc::XMLPlatformUtils::Terminate();
			XCEPT_RETHROW(xgi::exception::Exception, "Exception in parsing Crate element", e);
		}
		
		// Get VMEController		
	}
	
	// Delete the parser itself.  Must be done prior to calling Terminate, below.
	//delete parser;
	
	// And call the termination method
	//xercesc::XMLPlatformUtils::Terminate();

	//return crateVector_;
	outputHeader(out);
	outputStandardInterface(out);
	outputCurrentConfiguration(out);
	outputFooter(out);
}

bool FEDConfigurationEditor::columnIsUniqueIdentifier(const std::string &columnName,const std::string &tableName) {
	return (columnName=="FMM_ID"  && tableName=="dcc") || (columnName=="RUI" && tableName=="ddu");
}

void FEDConfigurationEditor::diffCrate(const std::string &connectionID, const std::string &old_key, const std::string &new_key) throw (xcept::Exception) {

	std::string queryViewName=topLevelTableName_;
	std::string periph_config_id;
	xdata::Table results;
	std::string crateid;
	std::string  label;

	//there is actually nothing in the FED crate table that can be changed.
	//for now the diff just selects the crate IDs necessary to select child records,
	//even for crates which may not have changed.
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = old_key;
	//std::string connectionID=connect();
	query(connectionID,"crates", parameters,results);
	  //diff(connectionID, queryViewName, old_emu_config_id, new_emu_config_id,results);
	  std::vector<std::string> columns=results.getColumns();
	  //std::string old_key;
	  //std::string new_key;
	crateIDsInDiff.clear();
	std::string configIDName="PERIPH_CONFIG_ID";
	std::string crateIdentifierColumn="CRATE_NUMBER";
	  for (unsigned rowIndex=0; rowIndex<results.getRowCount(); rowIndex++ ) {
	    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
		      std::string StrgValue=results.getValueAt(rowIndex,*column)->toString();
		      if (*column == crateIdentifierColumn) {
			      crateid=results.getValueAt(rowIndex,crateIdentifierColumn)->toString();//readCrateID(results,rowIndex);
			      crateIDsInDiff.push_back(crateid);
		      }
	      }
	      std::string crateIdentifier=crateIdentifierString(crateid);
	      diff(connectionID,"controller",old_key,new_key,"CRATE_NUMBER",crateid,crateIdentifier);
	      xdata::Table dduDiff;
	      diff(connectionID,"ddu",old_key,new_key,"CRATE_NUMBER",crateid,dduDiff);
	       setCachedDiff("ddu",crateIdentifier,dduDiff);
	       for (unsigned rowIndex=0;rowIndex<dduDiff.getRowCount();rowIndex++ ) {
		       xdata::Serializable *rui=dduDiff.getValueAt(rowIndex,"RUI");
		       if (rui) diff(connectionID,"fiber",old_key,new_key,"RUI",rui->toString(),crateIdentifier+" "+uniqueIdentifierForRow(dduDiff,"ddu",rowIndex));
	       }
	      xdata::Table dccDiff;
		diff(connectionID,"dcc",old_key,new_key,"CRATE_NUMBER",crateid,dccDiff);
	       setCachedDiff("dcc",crateIdentifier,dccDiff);
	       for (unsigned rowIndex=0;rowIndex<dccDiff.getRowCount();rowIndex++ ) {
		       xdata::Serializable *rui=dccDiff.getValueAt(rowIndex,"FMM_ID");
		       if (rui) diff(connectionID,"fifo",old_key,new_key,"FMM_ID",rui->toString(),crateIdentifier);
	       }
	 }
 }

void FEDConfigurationEditor::diff(const std::string &connectionID, const std::string &queryViewName,const std::string &old_key,const std::string &new_key,const std::string &other_parameter_name,const std::string &other_parameter,xdata::Table &results) {
	std::map<std::string, std::string> parameters;
	parameters.clear();
      parameters["OLD_KEY"]=old_key;
      parameters["NEW_KEY"]=new_key;
      parameters[other_parameter_name]=other_parameter;
      xdata::Table controllerDiff;
      query(connectionID,queryViewName+"_diff",parameters,results);
}

void FEDConfigurationEditor::diff(const std::string &connectionID, const std::string &queryViewName,const std::string &old_key,const std::string &new_key,const std::string &other_parameter_name,const std::string &other_parameter,const std::string &resultKey) {
	xdata::Table results;
	diff(connectionID,queryViewName,old_key,new_key,other_parameter_name,other_parameter,results);
      setCachedDiff(queryViewName,resultKey,results);
 
  }

bool FEDConfigurationEditor::columnIsDatabaseOnly(const std::string &columnName,const std::string &tableName) {
	//std::cout << "FEDConfigurationEditor::columnIsDatabaseOnly(" << columnName << "," << tableName << ")" << std::endl;
	if (columnName=="KEY" || columnName=="SYSTEM_NAME" ) return true;
	if (columnName=="CRATE_NUMBER" && tableName!="crate") return true;
	if (columnName=="RUI" && tableName=="fiber") return true;
	if (columnName=="FMM_ID" && tableName=="fifo") return true;
	return false;
}

bool FEDConfigurationEditor::canChangeColumn(const std::string &columnName,const std::string &tableName) {
	if (columnName.find("_NUMBER") != std::string::npos)
		return false; //this check should also be put in columnIsUniqueIdentifier
	if (columnIsUniqueIdentifier(columnName,tableName)) return false;
	return !columnIsDatabaseOnly(columnName,tableName);
}

}
}
