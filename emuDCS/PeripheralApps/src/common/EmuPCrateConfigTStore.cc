#include "emu/pc/EmuPCrateConfigTStore.h"
#include "emu/pc/TStoreRequest.h"

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
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/SimpleType.h"
#include "XMLParser.h"

#include "xercesc/dom/DOMNode.hpp"

#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/LoadDOM.h"
#include "tstore/client/Client.h"



XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPCrateConfigTStore)

  namespace emu {
    namespace pc {

  EmuPCrateConfigTStore::EmuPCrateConfigTStore(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
{
  xgi::bind(this,&EmuPCrateConfigTStore::Default, "Default");
  xgi::bind(this,&EmuPCrateConfigTStore::parseConfigFromXML, "parse");
  xgi::bind(this,&EmuPCrateConfigTStore::uploadConfigToDB, "upload");
  xgi::bind(this,&EmuPCrateConfigTStore::readConfigFromDB, "read");
  xgi::bind(this,&EmuPCrateConfigTStore::synchronizeToFromDB, "sync");
  xgi::bind(this,&EmuPCrateConfigTStore::SelectConfFile, "SelectConfFile");
  xgi::bind(this,&EmuPCrateConfigTStore::SetTypeDesc, "SetTypeDesc");
  xgi::bind(this,&EmuPCrateConfigTStore::incrementValue, "incrementValue");
  xgi::bind(this,&EmuPCrateConfigTStore::setValue, "setValue");
  xgi::bind(this,&EmuPCrateConfigTStore::changeSingleValue, "changeSingleValue");
  xgi::bind(this,&EmuPCrateConfigTStore::showTable, "Show");
  xgi::bind(this,&EmuPCrateConfigTStore::hideTable, "Hide");
  
  std::string HomeDir_ =getenv("HOME");
  xmlpath_    = HomeDir_ + "/config/pc/"; //xml file chosen must be in this directory. If you choose something in another directory then it will look for it in here and fail.
  xmlfile_    = "";
  dbUserFile_ = HomeDir_ + "/dbuserfile.txt";
  config_type_ = "GLOBAL";
  config_desc_ = "manual entry";
  
/*this allows some structure in the way values are displayed and manipulated; several tables are
'child' tables of a CSC so they should be grouped together and have the possibility of being manipulated on a per-chamber basis.
There is more structure than this in the XML files but it is not important since there is usually only one possible parent node anyway.*/
std::vector<std::string> emptyVector;
tableNames["vmecc"]=emptyVector;
tableNames["csc"]=emptyVector;
tableNames["csc"].push_back("tmb");
tableNames["csc"].push_back("alct");
tableNames["csc"].push_back("anodechannel");
tableNames["csc"].push_back("daqmb");
tableNames["csc"].push_back("cfeb");
	tableNames["ccb"]=emptyVector;
	tableNames["mpc"]=emptyVector;



}

void EmuPCrateConfigTStore::outputHeader(xgi::Output * out) {
  out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Emu Peripheral Crate Config via TStore") << std::endl;
  xgi::Utils::getPageHeader(
			    out,
			    "EmuPCrateConfigTStore",
			    getApplicationDescriptor()->getContextDescriptor()->getURL(),
			    getApplicationDescriptor()->getURN(),
			    "/hyperdaq/images/HyperDAQ.jpg"
			    );
}

std::string EmuPCrateConfigTStore::fullTableID(const std::string &configName,const std::string &identifier) {
	return configName+"_"+identifier;
}

bool EmuPCrateConfigTStore::shouldDisplayConfiguration(const std::string &configName,const std::string &identifier) {
	std::string key=fullTableID(configName,identifier);
	if (tablesToDisplay.count(key)) {
		return tablesToDisplay[key];
	}
	return false;
}

void EmuPCrateConfigTStore::outputShowHideButton(xgi::Output * out,const std::string &configName,const std::string &identifier) {
	std::string key=fullTableID(configName,identifier);
	std::string action;
	if (shouldDisplayConfiguration(configName,identifier)) action="Hide";
	else action="Show";
	*out << cgicc::form().set("method","POST").set("action", toolbox::toString("/%s/%s",getApplicationDescriptor()->getURN().c_str(),action.c_str())) << std::endl;
	*out << cgicc::input().set("type","hidden").set("name","table").set("value",key) << std::endl;

	*out << cgicc::input().set("type","submit").set("value",action) << std::endl;
	*out << cgicc::form() << std::endl;
	
}

//displays the table with name \a configName and identifier \a identifier
void EmuPCrateConfigTStore::displayConfiguration(xgi::Output * out,const std::string &configName,const std::string &identifier) {
	outputShowHideButton(out,configName,identifier);
	if (shouldDisplayConfiguration(configName,identifier)) {
		xdata::Table &currentTable=getCachedTable(configName,identifier);
	    *out << configName << std::endl;
		outputTable(out,currentTable,configName,identifier);
	}
}

//displays all tables with name \a configName which are related to the crate with ID \a crateID
//this works because they all use a table identifier beginning with the crate ID
void EmuPCrateConfigTStore::displayConfiguration(xgi::Output * out,const std::string &configName,int crateID) {			
	displayChildConfiguration(out,configName,crateIdentifierString(crateID));
}

void EmuPCrateConfigTStore::displayChildConfiguration(xgi::Output * out,const std::string &configName,const std::string &parentIdentifier) {
   	if (currentTables.count(configName)) {
		*out << "<p/><table border=\"2\" cellpadding=\"10\">";
    		*out << "<tr><td bgcolor=\"#FFCCFF\">" << configName << " of " << parentIdentifier << "</td></tr><tr><td>"  << std::endl;
 
  		std::map<std::string,xdata::Table> &tables=currentTables[configName];
  		std::map<std::string,xdata::Table>::iterator table;
  		outputTableEditControls(out,configName,parentIdentifier);
  		//loop through all tables whose key begins with the crateID
  		//all keys begin with the appropriate crateID
   		for (table=tables.lower_bound(parentIdentifier);table!=tables.lower_bound(parentIdentifier+"~");++table) {
  			displayConfiguration(out,configName,(*table).first);
			if (tableNames.count(configName)) {
				std::vector<std::string> &subTables=tableNames[configName];
				for (std::vector<std::string>::iterator subTable=subTables.begin();subTable!=subTables.end();++subTable) { 			
					//*out << "crate subtable, showing " << (*subTable) << " of " << (*table).first << std::endl;
	 
					displayChildConfiguration(out,*subTable,(*table).first);
				}
			}
  		}
		*out << "</tr></td></table>";
  	}

}

void EmuPCrateConfigTStore::outputFooter(xgi::Output * out) {
  if (TStore_myEndcap_) {
  	//*out << "got TStore_myEndcap_" << std::endl;
     std::vector<Crate *> myCrates;
  	myCrates.clear();
  	myCrates = TStore_myEndcap_->AllCrates();
  	if (myCrates.size()) {
  		*out << "<table border=\"2\" cellpadding=\"10\"><tr><td>Update all crates</td></tr><tr><td>";
		for (std::map<std::string,xdata::Table>::iterator tableDefinition=tableDefinitions.begin();tableDefinition!=tableDefinitions.end();++tableDefinition) {
			outputTableEditControls(out,(*tableDefinition).first);
  		}
  		*out << "</tr></td></table>";
		for(unsigned i = 0; i < myCrates.size(); ++i) {
			*out << "<p/><table border=\"2\" cellpadding=\"10\">";
			*out << "<tr><td bgcolor=\"#FFFFCC\">Crate " << myCrates[i]->CrateID() << "</td></tr><tr><td>"  << std::endl;
			//todo: change this to just loop through an array of table names and display each one
			for (std::map<std::string,std::vector<std::string> >::iterator tableName=tableNames.begin();tableName!=tableNames.end();++tableName) {
				//*out << "top level, showing " << (*tableName).first << " of crate " << myCrates[i]->CrateID() << std::endl;
				displayConfiguration(out,(*tableName).first,myCrates[i]->CrateID());
			}
			*out << "</tr></td></table>";
  		}
  	}
  }

  *out << cgicc::fieldset() << cgicc::html();
//  xgi::Utils::getPageFooter(*out);
}
//

void EmuPCrateConfigTStore::outputException(xgi::Output * out,xcept::Exception &e) {
  std::cout << e.message() << std::endl;
  *out << "<p>" << e.message() << "</p>" << std::endl;
}

//this needs to be changed to prevent people from changing columns which are
//used as identifiers in currentTables, or else if those should be editable,
//different identifiers should be used.
bool EmuPCrateConfigTStore::canChangeColumn(const std::string &columnName) {
	//can't edit ID columns, which all have _CONFIG_ID in their names
	return columnName.find("CONFIG_ID")==std::string::npos;
}

bool EmuPCrateConfigTStore::isNumericType(const std::string &xdataType) {
	std::string numericTypesC[]={
		(std::string)"float",
		(std::string)"double",
		(std::string)"int",
		(std::string)"int 32",
		(std::string)"int 64",
		(std::string)"unsigned long",
		(std::string)"unsigned int",
		(std::string)"unsigned int 32",
		(std::string)"unsigned int 64",
		(std::string)"unsigned short"
	};
	std::vector<std::string> numericTypes(numericTypesC,numericTypesC+(sizeof(numericTypesC)/sizeof(numericTypesC[0])));
	return std::find(numericTypes.begin(),numericTypes.end(),xdataType)!=numericTypes.end();
}

void EmuPCrateConfigTStore::outputTableEditControls(xgi::Output * out,const std::string &tableName,const std::string &prefix) {
	xdata::Table &definition=tableDefinitions[tableName];
	std::vector<std::string> columns=definition.getColumns();
	std::vector<std::string>::iterator column;
	
	//increment controls
	*out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/incrementValue",getApplicationDescriptor()->getURN().c_str())) << std::endl;
	*out << cgicc::input().set("type","hidden").set("name","table").set("value",tableName);
	*out << cgicc::input().set("type","hidden").set("name","prefix").set("value",prefix);
	//*out << "table " << tableName << cgicc::br();
	*out << "increment all " << cgicc::select().set("name","fieldName");//<< cgicc::input().set("type","text").set("name","view").set("value",view) << std::endl;
	for (column=columns.begin(); column!=columns.end(); column++) {
		if (canChangeColumn(*column)) {
			if (isNumericType(definition.getColumnType(*column))) {
				*out << cgicc::option().set("value",*column) << *column << cgicc::option() << std::endl;
			}
		}
	}
	*out << cgicc::select() << " fields by " << cgicc::input().set("type","text").set("name","amount");
	*out << cgicc::input().set("type","submit").set("value","Increment")  << std::endl;
	*out << cgicc::form() << cgicc::br() << std::endl;	
	
	//set controls
	*out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/setValue",getApplicationDescriptor()->getURN().c_str())) << std::endl;
	*out << cgicc::input().set("type","hidden").set("name","table").set("value",tableName);
	*out << cgicc::input().set("type","hidden").set("name","prefix").set("value",prefix);
	*out << "set all " << cgicc::select().set("name","fieldName");//<< cgicc::input().set("type","text").set("name","view").set("value",view) << std::endl;
	for (column=columns.begin(); column!=columns.end(); column++) {
		if (canChangeColumn(*column)) {
			*out << cgicc::option().set("value",*column) << *column << cgicc::option() << std::endl;
		}
	}
	*out << cgicc::select() << " to " << cgicc::input().set("type","text").set("name","newValue");
	*out << cgicc::input().set("type","submit").set("value","Set")  << std::endl;
	*out << cgicc::form() << std::endl;	
}

//outputs the value of \a value to *out. If the last three parameters are filled in (to indicate where the value comes from, so how to change it) then
//and the column is of a type that can be edited and a column name which does not seem to be a config ID,
//it will be in an editable text field with a change button which called changeSingleValue.
void EmuPCrateConfigTStore::outputSingleValue(xgi::Output * out,xdata::Serializable *value,const std::string &tableName,const std::string &identifier,const std::string &column,int rowIndex) {
	std::string columnType=value->type();
	if (columnType=="table") {
		outputTable(out,*static_cast<xdata::Table *>(value));
	} else if (columnType=="mime") {
		//xdata::Mime *mime=static_cast<xdata::Mime *>(value);
		*out << "mime";
		//output some other data.
	} else {
		if (canChangeColumn(column) && !tableName.empty() && !identifier.empty()) {
			  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("changeSingleValue",getApplicationDescriptor()->getURN().c_str())) << std::endl;
			*out << cgicc::input().set("type","hidden").set("name","table").set("value",tableName);
			*out << cgicc::input().set("type","hidden").set("name","identifier").set("value",identifier);
			*out << cgicc::input().set("type","hidden").set("name","fieldName").set("value",column);
			*out << cgicc::input().set("type","hidden").set("name","row").set("value",to_string(rowIndex));
			*out << cgicc::input().set("type","text").set("name","newValue").set("size","11").set("value",value->toString());

			*out << cgicc::input().set("type","submit").set("value","Change") << std::endl;
			*out << cgicc::form() << std::endl;
		} else {
			*out << value->toString();
		}
	}
}

//basic function to output current values
//will be changed to be arranged more vertically and make individual fields editable
void EmuPCrateConfigTStore::outputTable(xgi::Output * out,xdata::Table &results,const std::string &tableName,const std::string &identifier) {
	//this uses raw HTML tags because cgicc can't handle any nested tags, which makes it pretty much useless
	std::vector<std::string> columns=results.getColumns();
	std::vector<std::string>::iterator columnIterator;
	int rowCount=results.getRowCount();
	if (rowCount==1) {
		*out << "<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
		for(columnIterator=columns.begin(); columnIterator!=columns.end(); ++columnIterator) {
			if (canChangeColumn(*columnIterator)) {
				*out << "<tr><td>" << *columnIterator << /*(" << columnType << ")" << */ ":</td><td>";
				unsigned long rowIndex;
				for (rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
					outputSingleValue(out,results.getValueAt(rowIndex,*columnIterator),tableName,identifier,*columnIterator,rowIndex);
				}
				*out << "</tr>";
			}
		}
		*out << "</table>";
	} else {
		*out << rowCount << " rows";

		*out << "<table border=\"2\" cellpadding=\"2\">";
		*out << "<tr>";
		for(columnIterator=columns.begin(); columnIterator!=columns.end(); ++columnIterator) {
			*out << "<td>" << *columnIterator << " (" << results.getColumnType(*columnIterator) << ")" << "</td>";
		}
		*out << "</tr>";
		unsigned long rowIndex;
		for (rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
			*out << "<tr>";
			for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
				*out << "<td>";
				outputSingleValue(out,results.getValueAt(rowIndex,*columnIterator),tableName,identifier,*columnIterator,rowIndex);
				*out << "</td>";

			}
			*out << "</tr>";
		}
		*out << "</table>";
	}
}

void EmuPCrateConfigTStore::getTableDefinitionsIfNecessary() throw () {
	try {
		if (tableDefinitions.size()==0) {
   			std::string connectionID=connect();
  			// get table definitions (they are used for the web interface as well as for uploading to the db, so do it here.)
			getTableDefinitions(connectionID);
  			disconnect(connectionID);
  		}
  	} catch (xcept::Exception &e) {
  		LOG4CPLUS_WARN(this->getApplicationLogger(),xcept::stdformat_exception_history(e));
  	}
}

void EmuPCrateConfigTStore::outputStandardInterface(xgi::Output * out) {

  *out << cgicc::fieldset().set("style","font-size: 10pt;  font-family: arial;") << std::endl;

  *out << cgicc::table().set("border","0");

  *out << cgicc::tr();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("parse",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Parse XML file").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("upload",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Upload to DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("read",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Read from DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("sync",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Sync to DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
  *out << cgicc::tr();

  *out << cgicc::table();

  *out << "<hr>";
  *out << "XML filename: ";
  *out << cgicc::input().set("type","text").set("value",xmlfile_).set("style", "width:585px;").set("DISABLED") << cgicc::br() << std::endl;
  *out << "config type: ";
  *out << cgicc::input().set("type","text").set("value",config_type_).set("DISABLED") << cgicc::br() << std::endl;
  *out << "Description: ";
  *out << cgicc::input().set("type","text").set("value",config_desc_).set("DISABLED") << cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset() << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Select XML file").set("style","color:blue") << std::endl ;
  //
  std::string methodUpload = toolbox::toString("/%s/SelectConfFile",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
  *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","90") << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Select") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string ReadString = 
    toolbox::toString("/%s/SetTypeDesc",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ReadString) << std::endl ;
  *out << cgicc::input().set("type","text").set("value","").set("name","ConfigType") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set Type") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  ReadString = 
    toolbox::toString("/%s/SetTypeDesc",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ReadString) << std::endl ;
  *out << cgicc::input().set("type","text").set("value","").set("name","ConfigDesc") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set Description") << std::endl ;
  *out << cgicc::form() << std::endl ;

  *out << cgicc::fieldset() << cgicc::br();
  
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //

  *out << "Messages:<br>";
//  *out << cgicc::fieldset() << cgicc::br();
  //
  
}


void EmuPCrateConfigTStore::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  try {
    // As of now this is a soft link pointing to the xml file to be uploaded
    outputHeader(out);
    outputStandardInterface(out);
    outputFooter(out);
  } catch (xcept::Exception &e) {
    outputException(out,e);
  }
}
// 

void EmuPCrateConfigTStore::SelectConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	std::cout << "Button: Select XML file" << std::endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	cgicc::const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");
	//
	if(file != cgi.getFiles().end()) {
	  xmlfile_=(*file).getFilename();
          std::cout << "Select XML file " << xmlfile_ << std::endl;
	}
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
  
  
  
 void EmuPCrateConfigTStore::showTable(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception) {
	cgicc::Cgicc cgi(in);
  	std::string tableName=**cgi["table"];
	tablesToDisplay[tableName]=true;
	Default(in,out);
    }
    
     void EmuPCrateConfigTStore::hideTable(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception) {
	cgicc::Cgicc cgi(in);
  	std::string tableName=**cgi["table"];
	tablesToDisplay[tableName]=false;
	Default(in,out);
    }
    
template <class xdataType>
void set(xdata::Serializable *originalValue,const std::string &newValue) {
	xdataType *original=dynamic_cast<xdataType *>(originalValue);
	if (original) {
		original->fromString(newValue);
	}
}
  
template <class xdataType, typename simpleType>
void add(xdata::Serializable *originalValue,const std::string &addend) {
	xdataType *original=dynamic_cast<xdataType *>(originalValue);
	if (original) {
		xdataType amountToAdd;
		amountToAdd.fromString(addend);
		std::cout << "addend=" << addend << " amountToAdd=" << amountToAdd << " simply " << (simpleType)amountToAdd << std::endl;
		std::cout << "original=" << *original << " simply " << (simpleType)*original << std::endl;
		simpleType sum=(simpleType)*original+(simpleType)amountToAdd;
		std::cout << "sum=" << sum << std::endl;
		//*originalValue=(xdataType)sum;
		*original=sum;
		std::cout << "new value=" << originalValue->toString() << std::endl;
	}
}
  
void EmuPCrateConfigTStore::setValueFromString(xdata::Serializable *value,const std::string &newValue) {
	std::string columnType=value->type();
/*
	Aarrghh, this doesn't work because we need to know the template type for the SimpleType.
	So much for polymorphism.
	xdata::SimpleType *value;
	value=dynamic_cast<xdata::SimpleType *>((*table).second.getValueAt(rowIndex,fieldName));
	if (value) value->fromString(newValue);*/
	if (columnType=="int") {
		set<xdata::Integer>(value,newValue);
	} else if (columnType=="unsigned long") {
		set<xdata::UnsignedLong>(value,newValue);
	} else if (columnType=="float") {
		set<xdata::Float>(value,newValue);
	} else if (columnType=="double") {
		set<xdata::Double>(value,newValue);
	} else if (columnType=="unsigned int") {
		set<xdata::UnsignedInteger>(value,newValue);
	} else if (columnType=="unsigned int 32") {
		set<xdata::UnsignedInteger32>(value,newValue);
	} else if (columnType=="unsigned int 64") {
		set<xdata::UnsignedInteger64>(value,newValue);
	} else if (columnType=="unsigned short") {
		set<xdata::UnsignedShort>(value,newValue);
	} else if (columnType=="string") {
		set<xdata::String>(value,newValue);
	} else if (columnType=="time") {
		set<xdata::TimeVal>(value,newValue);
	} /*else if (columnType=="bool") { //booleans are not used elsewhere in the file, so I assume they don't exist in the configuration
		set<xdata::Boolean>(value,newValue);
	} */
}
  
void EmuPCrateConfigTStore::setValue(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
  	cgicc::Cgicc cgi(in);
  	std::string tableName=**cgi["table"];
  	std::string fieldName=**cgi["fieldName"];
  	std::string newValue=**cgi["newValue"];
  	LOG4CPLUS_DEBUG(this->getApplicationLogger(),"there are "+to_string(currentTables.count(tableName))+ " tables with the name "+tableName);

  	if (currentTables.count(tableName)) {
  		std::map<std::string,xdata::Table> &tables=currentTables[tableName];
  		std::map<std::string,xdata::Table>::iterator table;
  		std::map<std::string,xdata::Table>::iterator firstTable;
  		std::map<std::string,xdata::Table>::iterator lastTable;
		getRangeOfTables(cgi,tables,firstTable,lastTable);

  		for (table=firstTable;table!=lastTable;++table) {
		  	std::string columnType=(*table).second.getColumnType(fieldName);
  			try {
		  		int rowCount=(*table).second.getRowCount();
		  		for (int rowIndex=0;rowIndex<rowCount;rowIndex++) {
		  			xdata::Serializable *value=((*table).second.getValueAt(rowIndex,fieldName));
					setValueFromString(value,newValue);
			  	}
		  	} catch (xdata::exception::Exception &e) {
		  		XCEPT_RETHROW(xgi::exception::Exception,"Value "+newValue+" could not be converted to the data type "+columnType,e);
		  	}
		}
  	}
 	outputHeader(out);
  	outputStandardInterface(out);
  	outputFooter(out);
 }
 
 
 void EmuPCrateConfigTStore::changeSingleValue(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception) {
  	cgicc::Cgicc cgi(in);
  	std::string tableName=**cgi["table"];
  	std::string identifier=**cgi["identifier"];
  	std::string fieldName=**cgi["fieldName"];
  	std::string rowString=**cgi["row"];
  	std::string newValue=**cgi["newValue"];
	if (currentTables.count(tableName)) {
		std::map<std::string,xdata::Table> &tables=currentTables[tableName];
		if (tables.count(identifier)) {
			xdata::Table &table=tables[identifier];
			unsigned int rowIndex;
			std::istringstream myStream(rowString);
			if ((myStream>>rowIndex) && table.getRowCount()>=rowIndex) {
				if (tableHasColumn(table,fieldName) && canChangeColumn(fieldName)) {
					xdata::Serializable *value=table.getValueAt(rowIndex,fieldName);
					std::cout << "changing value from " << value->toString() << " to " << newValue << std::endl;
					setValueFromString(value,newValue);
				} else {
					XCEPT_RAISE(xgi::exception::Exception,"Column "+fieldName+" does not exist or is not editable in the specified table");
				}
			} else {
				XCEPT_RAISE(xgi::exception::Exception,"There is no row number "+rowString+" in the specified table");
			}
		} else {
			XCEPT_RAISE(xgi::exception::Exception,"No "+tableName+" configuration loaded for "+identifier);
		}
	} else {
		XCEPT_RAISE(xgi::exception::Exception,"No such table: "+tableName);
	}
 	outputHeader(out);
  	outputStandardInterface(out);
  	outputFooter(out);
    }
 
 void EmuPCrateConfigTStore::getRangeOfTables(const cgicc::Cgicc &cgi,std::map<std::string,xdata::Table> &tables,std::map<std::string,xdata::Table>::iterator &firstTable,std::map<std::string,xdata::Table>::iterator &lastTable) {
  	std::string prefix=**cgi["prefix"];
	if (prefix.empty()) {
		firstTable=tables.begin();
		lastTable=tables.end();
	} else {
		firstTable=tables.lower_bound(prefix);
		lastTable=tables.upper_bound(prefix+"~"); //~ is sorted last, so this should find the first key with a different prefix
	}
 }
  
 bool EmuPCrateConfigTStore::tableHasColumn(xdata::Table &table,const std::string &column) {
	 std::vector<std::string> columns=table.getColumns();
	 return std::find(columns.begin(),columns.end(),column)!=columns.end();
 }
 
 void EmuPCrateConfigTStore::incrementValue(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
  	cgicc::Cgicc cgi(in);
  	std::string tableName=**cgi["table"];
  	std::string fieldName=**cgi["fieldName"];
  	std::string amountToAdd=**cgi["amount"];
  	
  	LOG4CPLUS_DEBUG(this->getApplicationLogger(),"there are "+to_string(currentTables.count(tableName))+ " tables with the name "+tableName);

  	if (currentTables.count(tableName)) {
  		std::map<std::string,xdata::Table> &tables=currentTables[tableName];
  		std::map<std::string,xdata::Table>::iterator table;
  		std::map<std::string,xdata::Table>::iterator firstTable;
  		std::map<std::string,xdata::Table>::iterator lastTable;
		getRangeOfTables(cgi,tables,firstTable,lastTable);

  		for (table=firstTable;table!=lastTable;++table) {
			if (tableHasColumn((*table).second,fieldName)) {
				std::string columnType=(*table).second.getColumnType(fieldName);
				try {
					if (isNumericType(columnType)) {
						int rowCount=(*table).second.getRowCount();
						for (int rowIndex=0;rowIndex<rowCount;rowIndex++) {
							xdata::Serializable *value=(*table).second.getValueAt(rowIndex,fieldName);
							LOG4CPLUS_DEBUG(this->getApplicationLogger(),"adding "+amountToAdd+ " to "+columnType+" "+value->toString());
							if (columnType=="int") {
								add<xdata::Integer,xdata::IntegerT>(value,amountToAdd);
							} else if (columnType=="unsigned long") {
								add<xdata::UnsignedLong,xdata::UnsignedLongT>(value,amountToAdd);
							} else if (columnType=="float") {
								add<xdata::Float,xdata::FloatT>(value,amountToAdd);
							} else if (columnType=="double") {
								add<xdata::Double,xdata::DoubleT>(value,amountToAdd);
							} else if (columnType=="unsigned int") {
								add<xdata::UnsignedInteger,xdata::UnsignedIntegerT>(value,amountToAdd);
							} else if (columnType=="unsigned int 32") {
								add<xdata::UnsignedInteger32,xdata::UnsignedInteger32T>(value,amountToAdd);
							} else if (columnType=="unsigned int 64") {
								add<xdata::UnsignedInteger64,xdata::UnsignedInteger64T>(value,amountToAdd);
							} else if (columnType=="unsigned short") {
								add<xdata::UnsignedShort,xdata::UnsignedShortT>(value,amountToAdd);
							}
							LOG4CPLUS_DEBUG(this->getApplicationLogger(),"result is "+value->toString());
							(*table).second.setValueAt(rowIndex,fieldName,*value);
						}
					}
				} catch (xdata::exception::Exception &e) {
					XCEPT_RETHROW(xgi::exception::Exception,"Value "+amountToAdd+" could not be converted to the data type "+columnType,e);
				}
			}
		}
  	}
 	outputHeader(out);
  	outputStandardInterface(out);
  	outputFooter(out);
 }

  void EmuPCrateConfigTStore::SetTypeDesc(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("ConfigType");
    if(name2 != cgi.getElements().end()) {
      config_type_ = cgi["ConfigType"]->getValue();
      std::cout << "Type " << config_type_ << std::endl;
      //
    }
    //
    name2 = cgi.getElement("ConfigDesc");
    if(name2 != cgi.getElements().end()) {
      config_desc_ = cgi["ConfigDesc"]->getValue();
      std::cout << "Description " << config_desc_ << std::endl;
      //
    }
    //
    this->Default(in,out);
    //
  }

void EmuPCrateConfigTStore::parseConfigFromXML(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
    try {
 outputHeader(out);
 outputStandardInterface(out);
 if(xmlfile_.length()<=0) 
 {
    *out << "No XML file selected. Please select a valid XML file first." << std::endl;
 }
 else
 {
    std::string xmlname = xmlpath_ + xmlfile_;
    std::cout << "XML full name " << xmlname << std::endl;
    std::cout << "Parsing of the peripheral crate config XML file into DOM tree ..." << std::endl;
    *out << "Parsing of the peripheral crate config XML file into DOM tree ..." << std::endl;
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    std::cout << "Parsing started at: " << asctime (timeinfo) << std::endl;
    *out << "<br>Parsing started at: " << asctime (timeinfo) << std::endl;
    
    XMLParser TStore_emuparser;
    TStore_emuparser.parseFile( xmlname );
    TStore_myEndcap_ = TStore_emuparser.GetEmuEndcap();
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    if(TStore_myEndcap_) {
       std::cout << "Parsing ended at: " << asctime (timeinfo) << std::endl;
      *out << "<br>Parsing ended at: " << asctime (timeinfo) << std::endl;
    } else {
      std::cout << "ERROR.....Parser failed." <<std::endl;
      *out << "<br>ERROR.....Parser failed." <<std::endl;
     }
 	//rather than converting to and from EmuEndcap etc. all the time, we will keep the values in xdata::Tables
 	//for editing, and only convert back to the custom classes when/if necessary (which would be if it needs to be
 	//output as XML.) This way the editing interface can be generic for all tables.
 	//copyEndcapToTables(); 
 	std::vector<Crate *> myCrates = TStore_myEndcap_->AllCrates();
  	xdata::Table dataAsTable;
  	  getTableDefinitionsIfNecessary();
  	if (myCrates.size()) {
		for(unsigned i = 0; i < myCrates.size(); ++i) {
			if (myCrates[i]) {
				copyCCBToTable(dataAsTable,myCrates[i]);
				setCachedTable("ccb",myCrates[i]->CrateID(),dataAsTable);
				copyMPCToTable(dataAsTable,myCrates[i]);
				setCachedTable("mpc",myCrates[i]->CrateID(),dataAsTable);
				copyVMECCToTable(dataAsTable,myCrates[i]);
				setCachedTable("vcc",myCrates[i]->CrateID(),dataAsTable);
				std::vector<Chamber *> chambers=myCrates[i]->chambers();
				for(unsigned chamberIndex = 0; chamberIndex < chambers.size(); ++chamberIndex) {
				if(chambers[chamberIndex]) {
						copyCSCToTable(dataAsTable,chambers[chamberIndex]);
						std::string chamber=chamberID(myCrates[i]->CrateID(),chambers[chamberIndex]->GetLabel());
						setCachedTable("csc",chamber,dataAsTable);
						DAQMB *dmb=chambers[chamberIndex]->GetDMB();
						if (dmb) {
							std::string cacheIdentifier=DAQMBID(chamber,dmb->slot());
							copyDAQMBToTable(dataAsTable,dmb);
							setCachedTable("daqmb",cacheIdentifier,dataAsTable);
							copyCFEBToTable(dataAsTable,dmb);
							setCachedTable("cfeb",cacheIdentifier,dataAsTable);
						}
						TMB *tmb=chambers[chamberIndex]->GetTMB();
						if (tmb) {
							std::string cacheIdentifier=DAQMBID(chamber,tmb->slot());
							copyTMBToTable(dataAsTable,tmb);
							setCachedTable("tmb",cacheIdentifier,dataAsTable);
							 ALCTController * thisALCT = tmb->alctController();
							copyALCTToTable(dataAsTable,thisALCT);
							setCachedTable("alct",cacheIdentifier,dataAsTable);
							copyAnodeChannelToTable(dataAsTable,thisALCT);
							setCachedTable("anodechannel",cacheIdentifier,dataAsTable);
						}
					}
				}
			}
		}
  	}
    outputFooter(out);
 }    
  } catch (xcept::Exception &e) {
    outputException(out,e);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } catch (std::string &e) {
     std::cout << "string thrown: " << e << std::endl;
  } catch (...) {
  	//todo: change this to do something more useful. I only added it to prevent crashing
  	std::cout << "unknown thing thrown" << std::endl;
  }

}
//

void EmuPCrateConfigTStore::uploadConfigToDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
    try {
    outputHeader(out);
    outputStandardInterface(out);
    
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    *out << "<br>Upload started at: " << asctime (timeinfo) << std::endl;
    *out << "<br>Uploading to Database is in progress. This may take over a minute. Please be patient!<br><br>" << std::endl;
    
    startUpload();
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    *out << "<br>Uploading finished at: " << asctime (timeinfo) << std::endl;
    *out << "<br>EMU_Config_ID " << emu_config_id_ << " uploaded to Database." << std::endl;

    outputFooter(out);
  } catch (xcept::Exception &e) {
    outputException(out,e);
  }

}
//

void EmuPCrateConfigTStore::readConfigFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
    try {
    outputHeader(out);
    outputStandardInterface(out);

    // get this from e.g. a HyperDAQ page text field
    //*out << "<br>You clicked Read from DB<br>" << std::endl;
    std::string connectionID=connect();

  	/*std::vector<Crate *> myCrates;
  	myCrates.clear();
  	myCrates = TStore_myEndcap_->AllCrates();
  	std::string endcap_side=getEndcapSide(myCrates);*/

	//Which endcap should we load? This was checking both config IDs but only loading the minus side.
	//Should it load both, into separate EmuEndcap instance variables?
	//Should it check which one has already been loaded (if any) using getEndcapSide?
	//Should it be an option on the web interface?
    std::string endcap_side="minus";
    std::string emu_config_id = getConfigId("EMU_CONFIGURATION", "EMU_CONFIG_ID", endcap_side).toString();
    *out << "<br>max of EMU_CONFIG_ID for " << endcap_side << " side = " << emu_config_id << "<br>" << std::endl;
    endcap_side="plus";
    emu_config_id = getConfigId("EMU_CONFIGURATION", "EMU_CONFIG_ID", endcap_side).toString();
    *out << "<br>max of EMU_CONFIG_ID for " << endcap_side << " side = " << emu_config_id << "<br>" << std::endl;

    disconnect(connectionID);
  
    //EmuEndcap * myEndcap;
    //myEndcap = 
    getTableDefinitionsIfNecessary();
  	//*out << "TStore_myEndcap_=getConfiguredEndcap(" << emu_config_id << ")" << std::endl;
    TStore_myEndcap_=getConfiguredEndcap(emu_config_id);
    //*out << "TStore_myEndcap_==" << (TStore_myEndcap_?"NOT NULL":"NULL") << std::endl;

    outputFooter(out);
  } catch (xcept::Exception &e) {
    outputException(out,e);
  }

}
//

void EmuPCrateConfigTStore::synchronizeToFromDB(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  try {
    outputHeader(out);
    outputStandardInterface(out);

    //*out << "<br>You clicked Synchronize from DB<br>" << std::endl;
    std::string connectionID=connect();
    std::string syncMode = "to database"; // possibilities: "to database" OR "from database" OR "both ways"
    std::string syncPattern = "^EMU_.*$";

// LIU 8-22-2008. disable this function. For creating new database tables only 
    //*out << "<br>This function is for creating new database tables only." << std::endl;

     synchronize(connectionID,syncMode,syncPattern);
     *out << "<br>Synchronization to DB has finished.<br>" << std::endl;

    outputFooter(out);
  } catch (xcept::Exception &e) {
    outputException(out,e);
  }

}
//

// ################################
// #   TStore related functions   #
// ################################

xdata::UnsignedInteger64 EmuPCrateConfigTStore::getConfigId(const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side) throw (xcept::Exception) {
  
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
//#define debugV

xoap::MessageReference EmuPCrateConfigTStore::sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception) {
	xoap::MessageReference reply;
	
#ifdef debugV
	//std::cout << "Message: " << std::endl;
	//message->writeTo(std::cout);
	std::cout << std::endl;
#endif
	
	try {
		xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("tstore::TStore",0);
	    xdaq::ApplicationDescriptor * tstoretestDescriptor=this->getApplicationDescriptor();
		reply = getApplicationContext()->postSOAP(message,*tstoretestDescriptor, *tstoreDescriptor);
	} 
	catch (xdaq::exception::Exception& e) {
		LOG4CPLUS_ERROR(this->getApplicationLogger(),xcept::stdformat_exception_history(e));
	     XCEPT_RETHROW(xcept::Exception, "Could not post SOAP message. ", e);
	}
	
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
		
#ifdef debugV
	//std::cout << std::endl << "Response: " << std::endl;
	//reply->writeTo(std::cout);
	std::cout << std::endl;
#endif

	if (body.hasFault()) {
	  //XCEPT_RAISE (xcept::Exception, body.getFault().getFaultString());
	  XCEPT_RAISE (xcept::Exception, body.getFault().getDetail().getTextContent());
	}
	return reply;
}

std::string EmuPCrateConfigTStore::connect() throw (xcept::Exception) {
try {
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
} catch (std::string &s) {
	//I don't know where it's coming from, but occasionally something is throwing a string during the execution of this function
	//maybe if we log it we can find out what and why
	XCEPT_RAISE (xcept::Exception, "string thrown: "+s);
}
}

void EmuPCrateConfigTStore::disconnect(const std::string &connectionID) throw (xcept::Exception) {
	TStoreRequest request("disconnect");
	
	//add the connection ID
	request.addTStoreParameter("connectionID",connectionID);
	
	xoap::MessageReference message=request.toSOAP();
	
	sendSOAPMessage(message);
}

void EmuPCrateConfigTStore::queryMaxId(const std::string &connectionID, const std::string &queryViewName, const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception) {
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

void EmuPCrateConfigTStore::query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception) {
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

void EmuPCrateConfigTStore::query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception) {
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

void EmuPCrateConfigTStore::getDefinition(const std::string &connectionID, const std::string &insertViewName) throw (xcept::Exception) {
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
	xdata::Table results;
	if (!tstoreclient::getFirstAttachmentOfType(response,results)) {
		XCEPT_RAISE (xcept::Exception, "Server returned no data");
	}
	tableDefinitions[insertViewName]=results;
}
//

void EmuPCrateConfigTStore::insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows) throw (xcept::Exception) {
	
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

void EmuPCrateConfigTStore::synchronize(const std::string &connectionID, const std::string &syncMode, const std::string &syncPattern) throw (xcept::Exception) {
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

// ######################
// #  Misc              #
// ######################

/// Gets database user's name and password from \ref dbUserFile_ .
void EmuPCrateConfigTStore::getDbUserData(){

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

void EmuPCrateConfigTStore::getTableDefinitions(const std::string &connectionID) {
//todo: change this to loop through an array of table names
  getDefinition(connectionID,"configuration");
  getDefinition(connectionID,"peripheralcrate");
  getDefinition(connectionID,"ccb");
  getDefinition(connectionID,"mpc");
  getDefinition(connectionID,"vcc");
  getDefinition(connectionID,"csc");
  getDefinition(connectionID,"daqmb");
  getDefinition(connectionID,"cfeb");
  getDefinition(connectionID,"tmb");
  getDefinition(connectionID,"alct");
  getDefinition(connectionID,"anodechannel");
  std::cout << "got table definitions" << std::endl;
}

std::string EmuPCrateConfigTStore::getEndcapSide(std::vector<Crate *> &myCrates) throw (xcept::Exception) {
    
  // get peripheral crate data from DOM tree
  if(myCrates.size()<=0) {   
     XCEPT_RAISE(xcept::Exception,"No crate found.");
  }
  std::string crate_label = myCrates[0]->GetLabel();
  std::string endcap_side;
  if(strncmp(crate_label.c_str(),"VMEp",4)==0)
  {   endcap_side="plus";   }
  else if(strncmp(crate_label.c_str(),"VMEm",4)==0)
  {   endcap_side="minus";  }
  else
  {   
  	XCEPT_RAISE(xcept::Exception,"Unknown crate label '"+crate_label+"', should be 'VMEp' or 'VMEm'.");
  }
  return endcap_side;
}

// ######################
// #   Uploading data   #
// ######################

void EmuPCrateConfigTStore::startUpload() throw (xcept::Exception) {

  // Processing DOM Tree
  if ( TStore_myEndcap_ ) {
    std::cout << "<br>Found EmuEndcap<br>" << std::endl;
  } else {
    std::cout << "<br>EmuEndcap is not loaded into memory. First run ParseXML!<br>" << std::endl;
    return;
  }

	try {
  std::vector<Crate *> myCrates;
  myCrates.clear();
  myCrates = TStore_myEndcap_->AllCrates();
  std::string endcap_side=getEndcapSide(myCrates);

  std::string connectionID=connect();

  // get table definitions
	getTableDefinitions(connectionID);

  uploadConfiguration(connectionID, endcap_side);
  uploadPeripheralCrate(connectionID, myCrates);

  disconnect(connectionID);
  } catch (xcept::Exception &e) {
  	 LOG4CPLUS_WARN(this->getApplicationLogger(),e.what()+(std::string)" Stop loading to database...");
  	 XCEPT_RETHROW(xcept::Exception,"Problem uploading data to database",e);
  }
  
}
//

//this was to fix a problem with column types which I have since fixed in a better way
//still, leave the function here in case there is ever a need to change the way config IDs are set.
void EmuPCrateConfigTStore::setConfigID(xdata::Table &newRows,size_t rowId,const std::string &columnName,xdata::UnsignedInteger64 &id) {
	newRows.setValueAt(rowId, columnName, id);
}

void EmuPCrateConfigTStore::uploadConfiguration(const std::string &connectionID, const std::string endcap_side) throw (xcept::Exception) {

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

  emu_config_id = getConfigId("EMU_CONFIGURATION", "EMU_CONFIG_ID", endcap_side);
  if(emu_config_id.value_==0) emu_config_id=((endcap_side=="plus")?1000000:2000000);
  emu_config_id++;
  emu_config_id_=emu_config_id;
  xdata::TimeVal _emu_config_time = (xdata::TimeVal)currentTime.gettimeofday();
  // Info to be entered in a form on a HyperDAQ page
  xdata::String _emu_config_type = config_type_;
  xdata::String _description     = config_desc_;
  xdata::String _emu_endcap_side = endcap_side;

#ifdef debugV
  std::cout << "-- CONFIGURATION  emu_config_id --------- " <<  emu_config_id.toString()       << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_time ------- " <<  _emu_config_time.toString()     << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_type ------- " <<  _emu_config_type.toString()     << std::endl;
  std::cout << "-- CONFIGURATION  description ----------- " <<  _description.toString()         << std::endl;
  std::cout << "-- CONFIGURATION  emu_endcap_side ------- " <<  _emu_endcap_side.toString()     << std::endl;
#endif

  newRows = tableDefinitions[insertViewName];

	
  newRows.setValueAt(rowId, DESCRIPTION,     _description); 
  setConfigID(newRows,rowId,EMU_CONFIG_ID,   emu_config_id);
  newRows.setValueAt(rowId, EMU_CONFIG_TIME, _emu_config_time);
  newRows.setValueAt(rowId, EMU_CONFIG_TYPE, _emu_config_type);
  newRows.setValueAt(rowId, EMU_ENDCAP_SIDE, _emu_endcap_side);
  
  insert(connectionID,insertViewName,newRows);
  
  std::cout << "Configuration for " << endcap_side << " has been loaded to database as " << emu_config_id << std::endl;   
}
//

void EmuPCrateConfigTStore::uploadPeripheralCrate(const std::string &connectionID, const std::vector<Crate *> &TStore_allCrates) throw (xcept::Exception) {

  std::string insertViewName="peripheralcrate";
  xdata::Table newRows;

  std::cout << "List of crates:<br>" << std::endl;
  for(unsigned i = 0; i < TStore_allCrates.size(); ++i) {
    if(TStore_allCrates[i]) {

      size_t rowId(0);

      std::string CRATEID("CRATEID");
      std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
      std::string LABEL("LABEL");
      std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");

      xdata::UnsignedInteger64 _periph_config_id = emu_config_id_ * 100000000 + (i+1)*1000000;

	int crateID=TStore_allCrates[i]->CrateID();
      xdata::UnsignedShort _crateid = crateID;
      xdata::String _label = TStore_allCrates[i]->GetLabel();

#ifdef debugV
      std::cout << "-- PERIPHERALCRATE  crateid ------------- " << _crateid.toString()          << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  label --------------- " << _label.toString()            << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  emu_config_id ------- " << emu_config_id_.toString()    << std::endl; 
      std::cout	<< "-- PERIPHERALCRATE  periph_config_id ---- " << _periph_config_id.toString() << std::endl; 
#endif
     
      newRows.clear();
      newRows = tableDefinitions[insertViewName];

      setConfigID(newRows,rowId, EMU_CONFIG_ID,    emu_config_id_);
      newRows.setValueAt(rowId, CRATEID,          _crateid);
      newRows.setValueAt(rowId, LABEL,            _label);
      setConfigID(newRows,rowId, PERIPH_CONFIG_ID, _periph_config_id);

      insert(connectionID,insertViewName,newRows);

      //CCB * thisCCB = TStore_allCrates[i]->ccb();
      uploadCCB(connectionID, _periph_config_id, /*thisCCB*/crateID);
	
      uploadMPC(connectionID, _periph_config_id, /*thisMPC*/crateID);
      
      uploadVMECC(connectionID, _periph_config_id, /*TStore_allCrates[i]*/crateID);

      std::vector<Chamber *> allChambers = TStore_allCrates[i]->chambers();
      uploadCSC(connectionID, _periph_config_id, allChambers,crateID);
      

    } // end of if
  } // end of loop over TStore_myCrates[i]

}
//

void EmuPCrateConfigTStore::copyCCBToTable(xdata::Table &newRows,Crate * TStore_thisCrate) {
	newRows = tableDefinitions["ccb"];
   
  size_t rowId(0);
  std::string CCBMODE("CCBMODE");
  std::string CCB_FIRMWARE_DAY("CCB_FIRMWARE_DAY");
  std::string CCB_FIRMWARE_MONTH("CCB_FIRMWARE_MONTH");
  std::string CCB_FIRMWARE_YEAR("CCB_FIRMWARE_YEAR");
  std::string L1ADELAY("L1ADELAY");
  std::string TTCRXCOARSEDELAY("TTCRXCOARSEDELAY");
  std::string TTCRXFINEDELAY("TTCRXFINEDELAY");
  std::string TTCRXID("TTCRXID");
	
	CCB *TStore_thisCCB=TStore_thisCrate->ccb();
	if (TStore_thisCCB) {
		  xdata::UnsignedShort     _ccbmode            = TStore_thisCCB->GetCCBmode();
		  xdata::UnsignedShort     _ccb_firmware_day   = TStore_thisCCB->GetExpectedFirmwareDay();
		  xdata::UnsignedShort     _ccb_firmware_month = TStore_thisCCB->GetExpectedFirmwareMonth();
		  xdata::UnsignedShort     _ccb_firmware_year  = TStore_thisCCB->GetExpectedFirmwareYear();
		  xdata::UnsignedShort     _l1adelay           = TStore_thisCCB->Getl1adelay();
		  xdata::UnsignedShort     _ttcrxcoarsedelay   = TStore_thisCCB->GetTTCrxCoarseDelay();
		  xdata::UnsignedShort     _ttcrxfinedelay     = TStore_thisCCB->GetTTCrxFineDelay();
		  xdata::UnsignedShort     _ttcrxid            = TStore_thisCCB->GetTTCrxID();
		  
		  newRows.setValueAt(rowId, CCBMODE,            _ccbmode);
		  newRows.setValueAt(rowId, CCB_FIRMWARE_DAY,   _ccb_firmware_day);
		  newRows.setValueAt(rowId, CCB_FIRMWARE_MONTH, _ccb_firmware_month);
		  newRows.setValueAt(rowId, CCB_FIRMWARE_YEAR,  _ccb_firmware_year);
		  newRows.setValueAt(rowId, L1ADELAY,           _l1adelay);
		  newRows.setValueAt(rowId, TTCRXCOARSEDELAY,   _ttcrxcoarsedelay);
		  newRows.setValueAt(rowId, TTCRXFINEDELAY,     _ttcrxfinedelay);
		  newRows.setValueAt(rowId, TTCRXID,            _ttcrxid);
	}
}

void EmuPCrateConfigTStore::uploadCCB(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, int crateID) throw (xcept::Exception) {
  
  std::string insertViewName="ccb";
  std::string CCB_CONFIG_ID("CCB_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  
  size_t rowId(0);

  xdata::UnsignedInteger64 _ccb_config_id      = periph_config_id + 110000;

  xdata::Table &newRows=getCachedTable(/*,TStore_thisCrate*/insertViewName/*,_vcc_config_id*/,crateID); 
 std::cout << "upload type of CCB_CONFIG_ID" << newRows.getColumnType("CCB_CONFIG_ID") << std::endl;

  setConfigID(newRows,rowId, CCB_CONFIG_ID,      _ccb_config_id);
  setConfigID(newRows,rowId, EMU_CONFIG_ID,      emu_config_id_);
  setConfigID(newRows,rowId, PERIPH_CONFIG_ID,   periph_config_id);
  
  std::cout << "set emu_config_id to " << emu_config_id_ << std::endl;
  std::cout << "emu_config_id is " << newRows.getValueAt(rowId, EMU_CONFIG_ID) << std::endl;
  
  insert(connectionID,insertViewName,newRows);

}
void EmuPCrateConfigTStore::copyMPCToTable(xdata::Table &newRows,Crate * TStore_thisCrate) {
  size_t rowId(0);
  
	MPC * TStore_thisMPC = TStore_thisCrate->mpc();
  std::string MPC_FIRMWARE_DAY("MPC_FIRMWARE_DAY");
  std::string MPC_FIRMWARE_MONTH("MPC_FIRMWARE_MONTH");
  std::string MPC_FIRMWARE_YEAR("MPC_FIRMWARE_YEAR");
  std::string SERIALIZERMODE("SERIALIZERMODE");
  std::string TRANSPARENTMODE("TRANSPARENTMODE");
  
  xdata::UnsignedShort     _mpc_firmware_day   = TStore_thisMPC->GetExpectedFirmwareDay();
  xdata::UnsignedShort     _mpc_firmware_month = TStore_thisMPC->GetExpectedFirmwareMonth();
  xdata::UnsignedShort     _mpc_firmware_year  = TStore_thisMPC->GetExpectedFirmwareYear();
  xdata::UnsignedShort     _transparentmode    = TStore_thisMPC->GetTransparentMode();
  xdata::UnsignedShort     _serializermode     = TStore_thisMPC->GetSerializerMode();
  
  newRows = tableDefinitions["mpc"];
  newRows.setValueAt(rowId, MPC_FIRMWARE_DAY,   _mpc_firmware_day);
  newRows.setValueAt(rowId, MPC_FIRMWARE_MONTH, _mpc_firmware_month);
  newRows.setValueAt(rowId, MPC_FIRMWARE_YEAR,  _mpc_firmware_year);
  newRows.setValueAt(rowId, SERIALIZERMODE,     _serializermode);
  newRows.setValueAt(rowId, TRANSPARENTMODE,    _transparentmode);
}

void EmuPCrateConfigTStore::uploadMPC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, /*MPC * TStore_thisMPC*/int crateID) throw (xcept::Exception) {

  size_t rowId(0);
  std::string insertViewName="mpc";
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string MPC_CONFIG_ID("MPC_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");

  xdata::UnsignedInteger64 _mpc_config_id      = periph_config_id + 120000;

	xdata::Table &newRows=getCachedTable(insertViewName,crateID);

  setConfigID(newRows,rowId, EMU_CONFIG_ID,      emu_config_id_);  
  setConfigID(newRows,rowId, MPC_CONFIG_ID,      _mpc_config_id);
  setConfigID(newRows,rowId, PERIPH_CONFIG_ID,   periph_config_id);

  insert(connectionID,insertViewName,newRows);

}
//

void EmuPCrateConfigTStore::copyVMECCToTable(xdata::Table &newRows,Crate * TStore_thisCrate) {
 size_t rowId(0);
  std::string BGTO("BGTO");
  std::string BTO("BTO");
  std::string DFLT_SRV_MAC("DFLT_SRV_MAC");
  std::string ETH_PORT("ETH_PORT");
  std::string ETHERNET_CR("ETHERNET_CR");
  std::string EXT_FIFO_CR("EXT_FIFO_CR");
  std::string MAC_ADDR("MAC_ADDR");
  std::string MCAST_1("MCAST_1");
  std::string MCAST_2("MCAST_2");
  std::string MCAST_3("MCAST_3");
  std::string MSG_LVL("MSG_LVL");
  std::string PKT_ON_STARTUP("PKT_ON_STARTUP");
  std::string RST_MISC_CR("RST_MISC_CR");
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
  xdata::String            _vcc_frmw_ver    = TStore_thisCrate->vmecc()->GetVCC_frmw_ver();
  xdata::String            _vme_cr          = TStore_thisCrate->vmeController()->GetCR(3);
  xdata::String            _warn_on_shtdwn  = TStore_thisCrate->vmeController()->GetWarn_On_Shtdwn();

#ifdef debugV
  //std::cout << "-- VCC emu_config_id -------- " << emu_config_id_.toString()    << std::endl;
  //std::cout << "-- VCC periph_config_id ----- " << periph_config_id.toString()  << std::endl;
  //std::cout << "-- VCC vcc_config_id -------- " << _vcc_config_id.toString()    << std::endl;
  std::cout << "-- VCC bgto ----------------- " <<_bgto.toString()              << std::endl;
  std::cout << "-- VCC bto ------------------ " <<_bto.toString()               << std::endl;
  std::cout << "-- VCC dflt_srv_mac --------- " <<_dflt_srv_mac.toString()      << std::endl;
  std::cout << "-- VCC eth_port ------------- " <<_eth_port.toString()          << std::endl;
  std::cout << "-- VCC ethernet_cr ---------- " <<_ethernet_cr.toString()       << std::endl;
  std::cout << "-- VCC ext_fifo_cr ---------- " <<_ext_fifo_cr.toString()       << std::endl;
  std::cout << "-- VCC mac_addr ------------- " <<_mac_addr.toString()          << std::endl;
  std::cout << "-- VCC mcast_1 -------------- " <<_mcast_1.toString()           << std::endl;
  std::cout << "-- VCC mcast_2 -------------- " <<_mcast_2.toString()           << std::endl;
  std::cout << "-- VCC mcast_3 -------------- " <<_mcast_3.toString()           << std::endl;
  std::cout << "-- VCC msg_lvl -------------- " <<_msg_lvl.toString()           << std::endl;
  std::cout << "-- VCC pkt_on_startup ------- " <<_pkt_on_startup.toString()    << std::endl;
  std::cout << "-- VCC rst_misc_cr ---------- " <<_rst_misc_cr.toString()       << std::endl;
  std::cout << "-- VCC vcc_frmw_ver --------- " << _vcc_frmw_ver.toString()     << std::endl;
  std::cout << "-- VCC vme_cr --------------- " <<_vme_cr.toString()            << std::endl;
  std::cout << "-- VCC warn_on_shtdwn ------- " <<_warn_on_shtdwn.toString()    << std::endl;
#endif
try {
	  newRows.clear(); //get a std::length_exception the second time this is called.
	  newRows = tableDefinitions["vcc"];
	  std::cout << "hek" << std::endl;
	  newRows.setValueAt(rowId, BGTO,             _bgto);
	  newRows.setValueAt(rowId, BTO,              _bto);
	  newRows.setValueAt(rowId, DFLT_SRV_MAC,     _dflt_srv_mac);
	  newRows.setValueAt(rowId, ETH_PORT,         _eth_port);
	  newRows.setValueAt(rowId, ETHERNET_CR,      _ethernet_cr);
	  newRows.setValueAt(rowId, EXT_FIFO_CR,      _ext_fifo_cr);
	  newRows.setValueAt(rowId, MAC_ADDR,         _mac_addr);
	  newRows.setValueAt(rowId, MCAST_1,          _mcast_1);
	  newRows.setValueAt(rowId, MCAST_2,          _mcast_2);
	  newRows.setValueAt(rowId, MCAST_3,          _mcast_3);
	  newRows.setValueAt(rowId, MSG_LVL,          _msg_lvl);
	  newRows.setValueAt(rowId, PKT_ON_STARTUP,   _pkt_on_startup);
	  newRows.setValueAt(rowId, RST_MISC_CR,      _rst_misc_cr);
	  newRows.setValueAt(rowId, VCC_FRMW_VER,     _vcc_frmw_ver);
	  newRows.setValueAt(rowId, VME_CR,           _vme_cr);
	  newRows.setValueAt(rowId, WARN_ON_SHTDWN,   _warn_on_shtdwn);
	  std::cout << " got here " << std::endl;
  } catch (std::exception &e) {
  	XCEPT_RAISE(xcept::Exception,e.what());
  }

}

void EmuPCrateConfigTStore::setCachedTable(const std::string &insertViewName,const std::string &identifier,xdata::Table &table) throw (xcept::Exception) {
	LOG4CPLUS_DEBUG(this->getApplicationLogger(),"setting cached "+insertViewName+" table "+identifier);
	currentTables[insertViewName][identifier]=table;
	LOG4CPLUS_DEBUG(this->getApplicationLogger(),"set cached "+insertViewName+" table "+identifier);

}

void EmuPCrateConfigTStore::setCachedTable(const std::string &insertViewName,int crateID,xdata::Table &table/*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception) {
	std::string identifier=crateIdentifierString(crateID);
	setCachedTable(insertViewName,identifier,table);
}

xdata::Table &EmuPCrateConfigTStore::getCachedTable(const std::string &insertViewName,const std::string &identifier/*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception) {

	if (currentTables.count(insertViewName)) {
		if (currentTables[insertViewName].count(identifier)) {
			LOG4CPLUS_DEBUG(this->getApplicationLogger(),"getting cached "+insertViewName+" table "+identifier);
			return currentTables[insertViewName][identifier];
		}
	}
	XCEPT_RAISE(xcept::Exception,"No "+insertViewName+" configuration loaded for crate "+identifier);
}

std::string EmuPCrateConfigTStore::crateIdentifierString(int crateID) {
	return "crate "+to_string(crateID)+" "; //add space to the end so that when looping over keys beginning with this crate ID, we don't confuse e.g. 1 with 11
}

xdata::Table &EmuPCrateConfigTStore::getCachedTable(const std::string &insertViewName,int crateID/*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception) {
	std::string identifier=crateIdentifierString(crateID);
	return getCachedTable(insertViewName,identifier);
}

void EmuPCrateConfigTStore::uploadVMECC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, /*Crate * TStore_thisCrate*/int crateID) throw (xcept::Exception) {

  std::string insertViewName="vcc";
  size_t rowId(0);
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string VCC_CONFIG_ID("VCC_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  //copyVMECCToTable(newRows,TStore_thisCrate);
  xdata::UnsignedInteger64 _vcc_config_id   = periph_config_id + 130000;
  xdata::Table &newRows=getCachedTable(/*,TStore_thisCrate*/insertViewName/*,_vcc_config_id*/,/*TStore_thisCrate->CrateID()*/crateID); 
  //std::cout << "newRows.getColumnType(PERIPH_CONFIG_ID)=" << newRows.getColumnType(PERIPH_CONFIG_ID) << std::endl;
  setConfigID(newRows,rowId, PERIPH_CONFIG_ID, periph_config_id);
  //std::cout << "newRows.getColumnType(VCC_CONFIG_ID)=" << newRows.getColumnType(VCC_CONFIG_ID) << std::endl;
  setConfigID(newRows,rowId, VCC_CONFIG_ID,    _vcc_config_id);
  setConfigID(newRows,rowId, EMU_CONFIG_ID,    emu_config_id_);
 
  insert(connectionID,insertViewName,newRows);

}

void EmuPCrateConfigTStore::copyCSCToTable(xdata::Table &newRows,Chamber * chamber) {

  size_t rowId(0);
      newRows = tableDefinitions["csc"];
  std::string LABEL("LABEL");
  std::string KNOWN_PROBLEM("KNOWN_PROBLEM");
  std::string PROBLEM_MASK("PROBLEM_MASK");
  
  xdata::String _label;
  xdata::String _known_problem;
  xdata::UnsignedShort _problem_mask;
  
       _label = chamber->GetLabel();
      _known_problem = chamber->GetProblemDescription();
      _problem_mask = chamber->GetProblemMask();
      
      newRows.setValueAt(rowId, LABEL,            _label);
      newRows.setValueAt(rowId, KNOWN_PROBLEM,    _known_problem);
      newRows.setValueAt(rowId, PROBLEM_MASK,     _problem_mask);

}

std::string EmuPCrateConfigTStore::chamberID(int crateID,const std::string &chamberLabel) {
	return crateIdentifierString(crateID)+" chamber "+chamberLabel+" ";
}

std::string EmuPCrateConfigTStore::DAQMBID(const std::string &chamber,int slot) {
	return /*chamberID(crateID,chamberLabel)*/chamber+"_"+to_string(slot);
}


void EmuPCrateConfigTStore::uploadCSC(const std::string &connectionID, xdata::UnsignedInteger64 &periph_config_id, const std::vector<Chamber *> &TStore_allChambers,int crateID) throw (xcept::Exception) {
  
  std::string insertViewName="csc";
  
  size_t rowId(0);
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

  xdata::UnsignedInteger64 _csc_config_id;
  xdata::String _label;
  xdata::String _known_problem;
  xdata::UnsignedShort _problem_mask;
  
  for(unsigned j = 0; j < TStore_allChambers.size(); ++j) {
    if(TStore_allChambers[j]) {      
      _csc_config_id = periph_config_id + (j+1)*10000;
		std::string chamber=chamberID(crateID,TStore_allChambers[j]->GetLabel());
		xdata::Table &newRows=getCachedTable(insertViewName,chamber);

      setConfigID(newRows,rowId, CSC_CONFIG_ID,    _csc_config_id);
      newRows.setValueAt(rowId, EMU_CONFIG_ID,    emu_config_id_);
      newRows.setValueAt(rowId, PERIPH_CONFIG_ID, periph_config_id);

      insert(connectionID,insertViewName,newRows);

      DAQMB * thisDAQMB = TStore_allChambers[j]->GetDMB();
      uploadDAQMB(connectionID, _csc_config_id,thisDAQMB/*this parameter is to be removed*/,thisDAQMB->slot(),chamber);
      
      TMB * thisTMB = TStore_allChambers[j]->GetTMB();
      uploadTMB(connectionID, _csc_config_id, thisTMB,thisTMB->slot(),chamber);
      
    } // end of if
  } // end of loop over TStore_myChambers[j]

}
//

void EmuPCrateConfigTStore::copyDAQMBToTable(xdata::Table &newRows,DAQMB * TStore_thisDAQMB) {
  newRows = tableDefinitions["daqmb"];
  std::string ALCT_DAV_CABLE_DELAY("ALCT_DAV_CABLE_DELAY");
  std::string CALIBRATION_L1ACC_DELAY("CALIBRATION_L1ACC_DELAY");
  std::string CALIBRATION_LCT_DELAY("CALIBRATION_LCT_DELAY");
  std::string CFEB_CABLE_DELAY("CFEB_CABLE_DELAY");
  std::string CFEB_DAV_CABLE_DELAY("CFEB_DAV_CABLE_DELAY");
  std::string myCOMP_MODE("COMP_MODE");
  std::string COMP_TIMING("COMP_TIMING");
  std::string DMB_CNTL_FIRMWARE_TAG("DMB_CNTL_FIRMWARE_TAG");
  std::string DMB_VME_FIRMWARE_TAG("DMB_VME_FIRMWARE_TAG");
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
   xdata::String            _dmb_cntl_firmware_tag;
  std::string valueInHex;
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
  
  size_t rowId(0);
  newRows.setValueAt(rowId, ALCT_DAV_CABLE_DELAY,    _alct_dav_cable_delay);
  newRows.setValueAt(rowId, CALIBRATION_L1ACC_DELAY, _calibration_l1acc_delay);
  newRows.setValueAt(rowId, CALIBRATION_LCT_DELAY,   _calibration_lct_delay);
  newRows.setValueAt(rowId, CFEB_CABLE_DELAY,        _cfeb_cable_delay);
  newRows.setValueAt(rowId, CFEB_DAV_CABLE_DELAY,    _cfeb_dav_cable_delay);
  newRows.setValueAt(rowId, myCOMP_MODE,             _comp_mode);
  newRows.setValueAt(rowId, COMP_TIMING,             _comp_timing);
  newRows.setValueAt(rowId, DMB_CNTL_FIRMWARE_TAG,   _dmb_cntl_firmware_tag);
  newRows.setValueAt(rowId, DMB_VME_FIRMWARE_TAG,    _dmb_vme_firmware_tag);
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
}

void EmuPCrateConfigTStore::uploadDAQMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, DAQMB * &TStore_thisDAQMB,/*int crateID,const std::string &chamberLabel,*/int slot,const std::string &chamber) throw (xcept::Exception) {
  
  std::string insertViewName="daqmb";
  std::string identifier=DAQMBID(chamber,slot);
  xdata::Table &newRows=getCachedTable(insertViewName,identifier);
  
  size_t rowId(0);
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");


  xdata::UnsignedInteger64 _daqmb_config_id         = csc_config_id + 1000;

  //copyDAQMBToTable(newRows,TStore_thisDAQMB);
 
  setConfigID(newRows,rowId, CSC_CONFIG_ID,           csc_config_id);
  setConfigID(newRows,rowId, DAQMB_CONFIG_ID,         _daqmb_config_id);
  setConfigID(newRows,rowId, EMU_CONFIG_ID,           emu_config_id_);
  
  insert(connectionID,insertViewName,newRows);

  uploadCFEB(connectionID, _daqmb_config_id,identifier);

}

void EmuPCrateConfigTStore::copyCFEBToTable(xdata::Table &newRows,DAQMB * TStore_thisDAQMB) {
  std::string valueInHex;
	size_t rowId(0);
  newRows = tableDefinitions["cfeb"];
  
  std::string CFEB_FIRMWARE_TAG("CFEB_FIRMWARE_TAG");
  std::string CFEB_NUMBER("CFEB_NUMBER");
  std::string KILL_CHIP0("KILL_CHIP0");
  std::string KILL_CHIP1("KILL_CHIP1");
  std::string KILL_CHIP2("KILL_CHIP2");
  std::string KILL_CHIP3("KILL_CHIP3");
  std::string KILL_CHIP4("KILL_CHIP4");
  std::string KILL_CHIP5("KILL_CHIP5");
  
  std::vector<CFEB> &TStore_allCFEBs = TStore_thisDAQMB->cfebs_;//was using TStore_thisDAQMB->cfebs();, but this means copying the whole thing, which results in a bad_alloc if there is something uninitialised, making it harder to diagnose problems

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
    
    #ifdef debugV
    std::cout << "-- CFEB cfeb_number ------------ " << _cfeb_number.toString()         << std::endl;
    std::cout << "-- CFEB cfeb_firmware_tag ------ " << _cfeb_firmware_tag.toString()   << std::endl;
    std::cout << "-- CFEB kill_chip0 ------------- " << _kill_chip0.toString()          << std::endl;
    std::cout << "-- CFEB kill_chip1 ------------- " << _kill_chip1.toString()          << std::endl;
    std::cout << "-- CFEB kill_chip2 ------------- " << _kill_chip2.toString()          << std::endl;
    std::cout << "-- CFEB kill_chip3 ------------- " << _kill_chip3.toString()          << std::endl;
    std::cout << "-- CFEB kill_chip4 ------------- " << _kill_chip4.toString()          << std::endl;
    std::cout << "-- CFEB kill_chip5 ------------- " << _kill_chip5.toString()          << std::endl;
	#endif
    
    newRows.setValueAt(rowId, CFEB_FIRMWARE_TAG, _cfeb_firmware_tag);
    newRows.setValueAt(rowId, CFEB_NUMBER,       _cfeb_number);
    
    newRows.setValueAt(rowId, KILL_CHIP0,        _kill_chip0);
    newRows.setValueAt(rowId, KILL_CHIP1,        _kill_chip1);
    newRows.setValueAt(rowId, KILL_CHIP2,        _kill_chip2);
    newRows.setValueAt(rowId, KILL_CHIP3,        _kill_chip3);
    newRows.setValueAt(rowId, KILL_CHIP4,        _kill_chip4);
    newRows.setValueAt(rowId, KILL_CHIP5,        _kill_chip5);
    rowId++;
  }

}

void EmuPCrateConfigTStore::uploadCFEB(const std::string &connectionID, xdata::UnsignedInteger64 &daqmb_config_id,const std::string &identifier) throw (xcept::Exception) {
  
  std::string insertViewName="cfeb";
  xdata::Table &newRows=getCachedTable(insertViewName,identifier);
  std::string valueInHex;

  size_t rowId(0);
  std::string CFEB_CONFIG_ID("CFEB_CONFIG_ID");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

	for (unsigned rowIndex=0;rowIndex<newRows.getRowCount();rowIndex++ ) {

   xdata::UnsignedShort      *_cfeb_number       = dynamic_cast<xdata::UnsignedShort *>(newRows.getValueAt(rowIndex,"CFEB_NUMBER"));//TStore_allCFEBs[j].number();
	if (_cfeb_number) {
		xdata::UnsignedInteger64  _cfeb_config_id    = daqmb_config_id + *_cfeb_number;
	#ifdef debugV
	    std::cout << "-- CFEB emu_config_id ---------- " << emu_config_id_.toString()       << std::endl;
	    std::cout << "-- CFEB periph_config_id ------- " << daqmb_config_id.toString()      << std::endl;
	    std::cout << "-- CFEB cfeb_config_id --------- " << _cfeb_config_id.toString()      << std::endl;
	#endif
	    
	    setConfigID(newRows,rowId, CFEB_CONFIG_ID,    _cfeb_config_id);
	    setConfigID(newRows,rowId, DAQMB_CONFIG_ID,   daqmb_config_id);
	    setConfigID(newRows,rowId, EMU_CONFIG_ID,     emu_config_id_);
	}
    rowId++;
  }
  
  insert(connectionID,insertViewName,newRows);

}
//


void EmuPCrateConfigTStore::copyTMBToTable(xdata::Table &newRows,TMB * TStore_thisTMB) {
  size_t rowId(0);
    newRows = tableDefinitions["tmb"]; 
    std::string ADJACENT_CFEB_DISTANCE("ADJACENT_CFEB_DISTANCE");
  std::string AFF_THRESH("AFF_THRESH");
  std::string ALCT_BX0_DELAY("ALCT_BX0_DELAY");
  std::string ALCT_BX0_ENABLE("ALCT_BX0_ENABLE");
  std::string ALCT_CLEAR("ALCT_CLEAR");
  std::string ALCT_CLOCK_EN_USE_CCB("ALCT_CLOCK_EN_USE_CCB");
  std::string ALCT_PRETRIG_ENABLE("ALCT_PRETRIG_ENABLE");
  std::string ALCT_READOUT_WITHOUT_TRIG("ALCT_READOUT_WITHOUT_TRIG");
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
  std::string CLCT_READOUT_WITHOUT_TRIG("CLCT_READOUT_WITHOUT_TRIG");
  //  std::string CLCT_STAGGER("CLCT_STAGGER");   // obsolete
  std::string CLCT_THROTTLE("CLCT_THROTTLE");
  std::string CLCT_TRIG_ENABLE("CLCT_TRIG_ENABLE");
  std::string DMB_TX_DELAY("DMB_TX_DELAY");
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
  std::string MATCH_READOUT_WITHOUT_TRIG("MATCH_READOUT_WITHOUT_TRIG");
  std::string MATCH_TRIG_ALCT_DELAY("MATCH_TRIG_ALCT_DELAY");
  std::string MATCH_TRIG_ENABLE("MATCH_TRIG_ENABLE");
  std::string MATCH_TRIG_WINDOW_SIZE("MATCH_TRIG_WINDOW_SIZE");
  std::string MPC_BLOCK_ME1A("MPC_BLOCK_ME1A");
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
  std::string TMB_FIFO_NO_RAW_HITS("TMB_FIFO_NO_RAW_HITS");
  std::string TMB_FIFO_MODE("TMB_FIFO_MODE");
  std::string TMB_FIRMWARE_COMPILE_TYPE("TMB_FIRMWARE_COMPILE_TYPE");
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
  xdata::UnsignedShort     _alct_readout_without_trig     = TStore_thisTMB->GetAllowAlctNontrigReadout();
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
  xdata::UnsignedShort     _clct_readout_without_trig     = TStore_thisTMB->GetAllowClctNontrigReadout();
  //  xdata::UnsignedShort     _clct_stagger                  = TStore_thisTMB->GetClctStagger();  // obsolete
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
  std::string valueInHex;
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
  xdata::UnsignedShort     _match_readout_without_trig    = TStore_thisTMB->GetAllowMatchNontrigReadout();
  xdata::UnsignedShort     _match_trig_alct_delay         = TStore_thisTMB->GetAlctVpfDelay();
  xdata::UnsignedShort     _match_trig_enable             = TStore_thisTMB->GetTmbAllowMatch();
  xdata::UnsignedShort     _match_trig_window_size        = TStore_thisTMB->GetAlctMatchWindowSize();
  xdata::UnsignedShort     _mpc_block_me1a                = TStore_thisTMB->GetBlockME1aToMPC();
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
  xdata::UnsignedShort     _tmb_fifo_mode                 = TStore_thisTMB->GetFifoMode();
  xdata::UnsignedShort     _tmb_fifo_no_raw_hits          = TStore_thisTMB->GetFifoNoRawHits();
  convertToHex(valueInHex,"%x",TStore_thisTMB->GetTMBFirmwareCompileType());
  xdata::String            _tmb_firmware_compile_type     = valueInHex;
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
  
    newRows.setValueAt(rowId, ADJACENT_CFEB_DISTANCE,        _adjacent_cfeb_distance); 
  newRows.setValueAt(rowId, AFF_THRESH,                    _aff_thresh);
  newRows.setValueAt(rowId, ALCT_BX0_DELAY,                _alct_bx0_delay);
  newRows.setValueAt(rowId, ALCT_BX0_ENABLE,               _alct_bx0_enable);
  newRows.setValueAt(rowId, ALCT_CLEAR,                    _alct_clear);
  newRows.setValueAt(rowId, ALCT_CLOCK_EN_USE_CCB,         _alct_clock_en_use_ccb);
  newRows.setValueAt(rowId, ALCT_PRETRIG_ENABLE,           _alct_pretrig_enable);
  newRows.setValueAt(rowId, ALCT_READOUT_WITHOUT_TRIG,     _alct_readout_without_trig);
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
  newRows.setValueAt(rowId, CLCT_READOUT_WITHOUT_TRIG,     _clct_readout_without_trig);
  //  newRows.setValueAt(rowId, CLCT_STAGGER,                  _clct_stagger);   // obsolete
  newRows.setValueAt(rowId, CLCT_THROTTLE,                 _clct_throttle);
  newRows.setValueAt(rowId, CLCT_TRIG_ENABLE,              _clct_trig_enable);
  newRows.setValueAt(rowId, DMB_TX_DELAY,                  _dmb_tx_delay);
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
  newRows.setValueAt(rowId, MATCH_READOUT_WITHOUT_TRIG,    _match_readout_without_trig);
  newRows.setValueAt(rowId, MATCH_TRIG_ALCT_DELAY,         _match_trig_alct_delay);
  newRows.setValueAt(rowId, MATCH_TRIG_ENABLE,             _match_trig_enable);
  newRows.setValueAt(rowId, MATCH_TRIG_WINDOW_SIZE,        _match_trig_window_size);
  newRows.setValueAt(rowId, MPC_BLOCK_ME1A,                _mpc_block_me1a);
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
  newRows.setValueAt(rowId, TMB_FIFO_MODE,                 _tmb_fifo_mode);
  newRows.setValueAt(rowId, TMB_FIFO_NO_RAW_HITS,          _tmb_fifo_no_raw_hits);
  newRows.setValueAt(rowId, TMB_FIRMWARE_COMPILE_TYPE,     _tmb_firmware_compile_type);
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
}

void EmuPCrateConfigTStore::uploadTMB(const std::string &connectionID, xdata::UnsignedInteger64 &csc_config_id, TMB * &TStore_thisTMB,int slot,const std::string &chamber) throw (xcept::Exception) {
  
  std::string insertViewName="tmb";
  std::string identifier=DAQMBID(chamber,slot);
  xdata::Table &newRows=getCachedTable(insertViewName,identifier);
  std::string valueInHex;
  
  size_t rowId(0);

  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");
  xdata::UnsignedInteger64 _tmb_config_id                 = csc_config_id + 2000;


  

  setConfigID(newRows,rowId, CSC_CONFIG_ID,                 csc_config_id);
  setConfigID(newRows,rowId, EMU_CONFIG_ID,                 emu_config_id_);
  setConfigID(newRows,rowId, TMB_CONFIG_ID,                 _tmb_config_id);

  insert(connectionID,insertViewName,newRows);

  ALCTController * thisALCT = TStore_thisTMB->alctController();
  uploadALCT(connectionID, _tmb_config_id, thisALCT,identifier);

}
//


void EmuPCrateConfigTStore::copyALCTToTable(xdata::Table &newRows,ALCTController * &TStore_thisALCT) {
	size_t rowId(0);
  std::string ALCT_ACCEL_MODE("ALCT_ACCEL_MODE");
  std::string ALCT_NPLANES_HIT_ACCEL_PATTERN("ALCT_NPLANES_HIT_ACCEL_PATTERN");
  std::string ALCT_NPLANES_HIT_ACCEL_PRETRIG("ALCT_NPLANES_HIT_ACCEL_PRETRIG");
  std::string ALCT_BXN_OFFSET("ALCT_BXN_OFFSET");
  std::string ALCT_CCB_ENABLE("ALCT_CCB_ENABLE");
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
  
  xdata::UnsignedShort     _alct_accel_mode                = TStore_thisALCT->GetWriteAlctAmode();
  xdata::UnsignedShort     _alct_nplanes_hit_accel_pattern = TStore_thisALCT->GetWriteAcceleratorPatternThresh();
  xdata::UnsignedShort     _alct_nplanes_hit_accel_pretrig = TStore_thisALCT->GetWriteAcceleratorPretrigThresh();
  xdata::UnsignedShort     _alct_bxn_offset                = TStore_thisALCT->GetWriteBxcOffset();
  xdata::UnsignedShort     _alct_ccb_enable                = TStore_thisALCT->GetWriteCcbEnable();
	
	
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
  
  std::cout << "-- ALCT alct_accel_mode ------------------- " << _alct_accel_mode.toString()                << std::endl;
  std::cout << "-- ALCT alct_nplanes_hit_accel_pattern ---- " << _alct_nplanes_hit_accel_pattern.toString() << std::endl;
  std::cout << "-- ALCT alct_nplanes_hit_accel_pretrig ---- " << _alct_nplanes_hit_accel_pretrig.toString() << std::endl;
  std::cout << "-- ALCT alct_bxn_offset ------------------- " << _alct_bxn_offset.toString()                << std::endl;
  std::cout << "-- ALCT alct_ccb_enable ------------------- " << _alct_ccb_enable.toString()                << std::endl;
  std::cout << "-- ALCT alct_config_in_readout ------------ " << _alct_config_in_readout.toString()         << std::endl;
  std::cout << "-- ALCT alct_drift_delay ------------------ " << _alct_drift_delay.toString()               << std::endl;
  std::cout << "-- ALCT alct_ext_trig_enable -------------- " << _alct_ext_trig_enable.toString()           << std::endl;
  std::cout << "-- ALCT alct_fifo_mode -------------------- " << _alct_fifo_mode.toString()                 << std::endl;
  std::cout << "-- ALCT alct_fifo_pretrig ----------------- " << _alct_fifo_pretrig.toString()              << std::endl;
  std::cout << "-- ALCT alct_fifo_tbins ------------------- " << _alct_fifo_tbins.toString()                << std::endl;
  std::cout << "-- ALCT alct_firmware_backward_forward ---- " << _alct_firmware_backward_forward.toString() << std::endl;
  std::cout << "-- ALCT alct_firmware_day ----------------- " << _alct_firmware_day.toString()              << std::endl;
  std::cout << "-- ALCT alct_firmware_month --------------- " << _alct_firmware_month.toString()            << std::endl;
  std::cout << "-- ALCT alct_firmware_negat_posit --------- " << _alct_firmware_negat_posit.toString()      << std::endl;
  std::cout << "-- ALCT alct_firmware_year ---------------- " << _alct_firmware_year.toString()             << std::endl;
  std::cout << "-- ALCT alct_inject_mode ------------------ " << _alct_inject_mode.toString()               << std::endl;
  std::cout << "-- ALCT alct_l1a_delay -------------------- " << _alct_l1a_delay.toString()                 << std::endl;
  std::cout << "-- ALCT alct_l1a_internal ----------------- " << _alct_l1a_internal.toString()              << std::endl;
  std::cout << "-- ALCT alct_l1a_offset ------------------- " << _alct_l1a_offset.toString()                << std::endl;
  std::cout << "-- ALCT alct_l1a_window_width ------------- " << _alct_l1a_window_width.toString()          << std::endl;
  std::cout << "-- ALCT alct_nplanes_hit_pattern ---------- " << _alct_nplanes_hit_pattern.toString()       << std::endl;
  std::cout << "-- ALCT alct_nplanes_hit_pretrig ---------- " << _alct_nplanes_hit_pretrig.toString()       << std::endl;
  std::cout << "-- ALCT alct_send_empty ------------------- " << _alct_send_empty.toString()                << std::endl;
  std::cout << "-- ALCT alct_sn_select -------------------- " << _alct_sn_select.toString()                 << std::endl;
  std::cout << "-- ALCT alct_testpulse_amplitude ---------- " << _alct_testpulse_amplitude.toString()       << std::endl;
  std::cout << "-- ALCT alct_testpulse_direction ---------- " << _alct_testpulse_direction.toString()       << std::endl;
  std::cout << "-- ALCT alct_testpulse_invert ------------- " << _alct_testpulse_invert.toString()          << std::endl;
  std::cout << "-- ALCT alct_trig_info_en ----------------- " << _alct_trig_info_en.toString()              << std::endl;
  std::cout << "-- ALCT alct_trig_mode -------------------- " << _alct_trig_mode.toString()                 << std::endl;
  std::cout << "-- ALCT chamber_type ---------------------- " << _chamber_type.toString()                   << std::endl;
#endif

  newRows = tableDefinitions["alct"];
  
  newRows.setValueAt(rowId, ALCT_ACCEL_MODE,                _alct_accel_mode);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_ACCEL_PATTERN, _alct_nplanes_hit_accel_pattern);
  newRows.setValueAt(rowId, ALCT_NPLANES_HIT_ACCEL_PRETRIG, _alct_nplanes_hit_accel_pretrig);
  newRows.setValueAt(rowId, ALCT_BXN_OFFSET,                _alct_bxn_offset);
  newRows.setValueAt(rowId, ALCT_CCB_ENABLE,                _alct_ccb_enable);
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
}

void EmuPCrateConfigTStore::uploadALCT(const std::string &connectionID, xdata::UnsignedInteger64 &tmb_config_id, ALCTController * &TStore_thisALCT,const std::string &identifier) throw (xcept::Exception) {
  
  std::string insertViewName="alct";
  xdata::Table &newRows=getCachedTable(insertViewName,identifier);

  size_t rowId(0);
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");

  xdata::UnsignedInteger64 _alct_config_id                 = tmb_config_id + 100;

#ifdef debugV
  std::cout << "-- ALCT emu_config_id --------------------- " << emu_config_id_.toString()                  << std::endl;
  std::cout << "-- ALCT tmb_config_id --------------------- " << tmb_config_id.toString()                   << std::endl;
  std::cout << "-- ALCT alct_config_id -------------------- " << _alct_config_id.toString()                 << std::endl;
#endif

  setConfigID(newRows,rowId, ALCT_CONFIG_ID,                 _alct_config_id);
  setConfigID(newRows,rowId, EMU_CONFIG_ID,                  emu_config_id_);
  setConfigID(newRows,rowId, TMB_CONFIG_ID,                  tmb_config_id);
  insert(connectionID,insertViewName,newRows);

  uploadAnodeChannel(connectionID, _alct_config_id, TStore_thisALCT,identifier);

}
//

void EmuPCrateConfigTStore::copyAnodeChannelToTable(xdata::Table &newRows,ALCTController * &TStore_thisALCT) {
 newRows = tableDefinitions["anodechannel"];
 int maxUserIndex = TStore_thisALCT->MaximumUserIndex();
  std::string AFEB_FINE_DELAY("AFEB_FINE_DELAY");
  std::string AFEB_NUMBER("AFEB_NUMBER");
  std::string AFEB_THRESHOLD("AFEB_THRESHOLD");
  size_t rowId(0);

  for(int afeb = 0; afeb <= maxUserIndex; afeb++) {
    xdata::UnsignedShort     _afeb_fine_delay = TStore_thisALCT->GetWriteAsicDelay(afeb);
    xdata::UnsignedShort     _afeb_number     = afeb+1;  // in XML it starts with 1 but the setter function uses 'number-1'; this way the XML file and the DB are consistent
    xdata::UnsignedShort     _afeb_threshold  = TStore_thisALCT->GetAfebThresholdDAC(afeb);

#ifdef debugV
    std::cout << "-- AFEB afeb_fine_delay -------- " << _afeb_fine_delay.toString()    << std::endl;
    std::cout << "-- AFEB afeb_number ------------ " << _afeb_number.toString()        << std::endl;
    std::cout << "-- AFEB afeb_threshold --------- " << _afeb_threshold.toString()     << std::endl;
#endif
    newRows.setValueAt(rowId, AFEB_FINE_DELAY, _afeb_fine_delay); 
    newRows.setValueAt(rowId, AFEB_NUMBER,     _afeb_number); 
    newRows.setValueAt(rowId, AFEB_THRESHOLD,  _afeb_threshold); 
    rowId++;
  }
}

void EmuPCrateConfigTStore::uploadAnodeChannel(const std::string &connectionID, xdata::UnsignedInteger64 &alct_config_id, ALCTController * &TStore_thisALCT,const std::string &identifier) throw (xcept::Exception) {
  
  std::string insertViewName="anodechannel";
  xdata::Table &newRows=getCachedTable(insertViewName,identifier);

  size_t rowId(0);
  std::string AFEB_CONFIG_ID("AFEB_CONFIG_ID");
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  int maxUserIndex = TStore_thisALCT->MaximumUserIndex();

  for(int afeb = 0; afeb <= maxUserIndex; afeb++) {
    xdata::UnsignedShort     _afeb_fine_delay = TStore_thisALCT->GetWriteAsicDelay(afeb);
    xdata::UnsignedShort     _afeb_number     = afeb+1;  // in XML it starts with 1 but the setter function uses 'number-1'; this way the XML file and the DB are consistent
    xdata::UnsignedShort     _afeb_threshold  = TStore_thisALCT->GetAfebThresholdDAC(afeb);
    xdata::UnsignedInteger64 _afeb_config_id  = alct_config_id + (afeb+1);

#ifdef debugV
    std::cout << "-- AFEB emu_config_id ---------- " << emu_config_id_.toString()      << std::endl;
    std::cout << "-- AFEB alct_config_id --------- " << alct_config_id.toString()      << std::endl;
    std::cout << "-- AFEB afeb_config_id --------- " << _afeb_config_id.toString()     << std::endl;
    std::cout << "-- AFEB afeb_fine_delay -------- " << _afeb_fine_delay.toString()    << std::endl;
    std::cout << "-- AFEB afeb_number ------------ " << _afeb_number.toString()        << std::endl;
    std::cout << "-- AFEB afeb_threshold --------- " << _afeb_threshold.toString()     << std::endl;
#endif
  
    setConfigID(newRows,rowId, AFEB_CONFIG_ID,  _afeb_config_id);  
    setConfigID(newRows,rowId, ALCT_CONFIG_ID,  alct_config_id);
    setConfigID(newRows,rowId, EMU_CONFIG_ID,   emu_config_id_);
    rowId++;
  }

  insert(connectionID,insertViewName,newRows);

}
//

// ########################
// #    Retrieving data   #
// ########################

EmuEndcap * EmuPCrateConfigTStore::getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception) {
try {
  EmuEndcap * endcap = new EmuEndcap();
  std::cout << "######## Empty EmuEndcap is created." << std::endl;
  
  std::string connectionID=connect();
  //std::cout << "Liu DEBUG: connectionID " << connectionID << std::endl;
  readConfiguration(connectionID, emu_config_id, endcap);
  std::cout << "######## EmuEndcap is complet." << std::endl;

  disconnect(connectionID);
  
  return endcap;
  } catch (std::exception &e) {
  	XCEPT_RAISE(xcept::Exception,e.what());
  }

}
//

void EmuPCrateConfigTStore::readConfiguration(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception) {
  try {
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
    
  } catch (std::exception &e) {
  	XCEPT_RAISE(xcept::Exception,e.what());
  }
}
//

void EmuPCrateConfigTStore::readPeripheralCrate(const std::string &connectionID, const std::string &emu_config_id, EmuEndcap * endcap) throw (xcept::Exception) {

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
      	std::cout << "crateid is of type " << value->type() << std::endl;
      	if (value->type()=="int") {
         	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
         	if (i) crateid = (int)*i;
         }
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

void EmuPCrateConfigTStore::readVCC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

  std::string queryViewName="vcc";
  xdata::Table results;

  xdata::Serializable  * value;
  std::string StrgValue;
  int IntValue=0;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);
  
  
  std::cout << "VCC  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    // add VMECC(VMEModule in slot 1) to crate
    VMECC * vcc = new VMECC(theCrate, 1);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "MAC_ADDR"){
	theCrate->vmeController()->SetMAC(0,StrgValue);
	size_t pos=StrgValue.find('-');
	while(pos!=std::string::npos){
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
  
  copyVMECCToTable(results,theCrate); //this is because the results from TStore have the wrong column types
  setCachedTable(queryViewName,theCrate->CrateID(),results);
}
//

void EmuPCrateConfigTStore::readCSC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

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
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "LABEL")        {csc_->SetLabel(StrgValue);}
      if (*column == "KNOWN_PROBLEM"){csc_->SetProblemDescription(StrgValue);}
      if (*column == "PROBLEM_MASK") {csc_->SetProblemMask(IntValue);}
      if (*column == "CSC_CONFIG_ID"){csc_config_id = StrgValue;}
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    copyCSCToTable(results,csc_);
    setCachedTable(queryViewName,chamberID(theCrate->CrateID(),csc_->GetLabel()),results);
    readDAQMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
    readTMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
  }
}
//

void EmuPCrateConfigTStore::readCCB(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

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
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
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
	copyCCBToTable(results,theCrate); //this is because the results from TStore have the wrong column types
  setCachedTable(queryViewName,theCrate->CrateID(),results);   
}
//

void EmuPCrateConfigTStore::readMPC(const std::string &connectionID, const std::string &emu_config_id, const std::string &periph_config_id, Crate * theCrate) throw (xcept::Exception) {

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
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      StrgValue=value->toString();

      if (*column == "SERIALIZERMODE")    {mpc_->SetTLK2501TxMode(IntValue);}
      if (*column == "TRANSPARENTMODE")   {mpc_->SetTransparentMode(IntValue);}
      if (*column == "MPC_FIRMWARE_YEAR") {mpc_->SetExpectedFirmwareYear(IntValue);}
      if (*column == "MPC_FIRMWARE_MONTH"){mpc_->SetExpectedFirmwareMonth(IntValue);}
      if (*column == "MPC_FIRMWARE_DAY")  {mpc_->SetExpectedFirmwareDay(IntValue);}
      
      std::cout << *column + ": " + StrgValue << std::endl;
    }
  }
  copyMPCToTable(results,theCrate); //this is because the results from TStore have the wrong column types
  setCachedTable(queryViewName,theCrate->CrateID(),results); 
}

void EmuPCrateConfigTStore::readDAQMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception) {

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
	if (i) slot = (int)*i;
      }
    }
    DAQMB * daqmb_ = new DAQMB(theCrate,theChamber,slot);
    daqmb_->SetCrateId(theCrate->CrateID());
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      //std::cout << *column + "[" + results.getColumnType(*column) + "]" + " ==> " + value->toString() << std::endl;     
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      if (results.getColumnType(*column)=="float")          {xdata::Float * f = dynamic_cast<xdata::Float *>(value); if (f) FloatValue=(float)*f;}
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
    std::string identifier=DAQMBID(chamberID(theCrate->CrateID(),theChamber->GetLabel()),slot);
    readCFEB(connectionID, emu_config_id, daqmb_config_id_, daqmb_,identifier);
    copyDAQMBToTable(results,daqmb_); //this is because the results from TStore have the wrong column types
  	setCachedTable(queryViewName,identifier,results);
  }
}
//

void EmuPCrateConfigTStore::readCFEB(const std::string &connectionID, const std::string &emu_config_id, const std::string &daqmb_config_id, DAQMB * theDaqmb,const std::string &cacheIdentifier) throw (xcept::Exception) {

  std::string queryViewName="cfeb";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue=0;
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
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      if (*column == "CFEB_NUMBER"){number = IntValue;}
    }
    std::cout << "CFEB cfeb( " + to_string(number) << " )" << std::endl;
    CFEB cfeb(number);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int") {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
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
  copyCFEBToTable(results,theDaqmb);
  setCachedTable(queryViewName,cacheIdentifier,results);
}
//

void EmuPCrateConfigTStore::readTMB(const std::string &connectionID, const std::string &emu_config_id, const std::string &csc_config_id, Crate * theCrate, Chamber * theChamber) throw (xcept::Exception) {

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
	if (i) slot = (int)*i;
      }
    }
    TMB * tmb_ = new TMB(theCrate, theChamber, slot);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
      if (results.getColumnType(*column)=="int"  ) {xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); if (i) IntValue=(int)*i;}
      if (results.getColumnType(*column)=="float"           ) {xdata::Float * f = dynamic_cast<xdata::Float *>(value); if (f) FloatValue=(float)*f;}
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
      std::string identifier=DAQMBID(chamberID(theCrate->CrateID(),theChamber->GetLabel()),slot);
    readALCT(connectionID, emu_config_id, tmb_config_id_, tmb_,identifier);
	copyTMBToTable(results,tmb_); //this is because the results from TStore have the wrong column types
  	setCachedTable(queryViewName,identifier,results);
  }
}
//

void EmuPCrateConfigTStore::readALCT(const std::string &connectionID, const std::string &emu_config_id, const std::string &tmb_config_id, TMB * theTmb,const std::string &identifier) throw (xcept::Exception) {

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
      if (results.getColumnType(*column)=="int"  ) {
      	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); 
      	if (i) IntValue=(int)*i;
      }
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
    readAnodeChannel(connectionID, emu_config_id, alct_config_id_, alct_,identifier);
    if (rowIndex==0) copyALCTToTable(results,alct_); //this is because the results from TStore have the wrong column types
	else { //as far as I can tell, there can only be one ALCT per TMB 
		XCEPT_RAISE(xcept::Exception,"More than one ALCT for TMB");
	}
	
	setCachedTable(queryViewName,identifier,results);
  }
  }
//

void EmuPCrateConfigTStore::readAnodeChannel(const std::string &connectionID, const std::string &emu_config_id, const std::string &alct_config_id, ALCTController * theAlct,const std::string &identifier) throw (xcept::Exception) {

  std::string queryViewName="anodechannel";
  xdata::Table results;

  xdata::Serializable  * value;
  int IntValue=0;
  std::string StrgValue;
  int afeb_number=0;

  query(connectionID, queryViewName/*"EMU_ANODECHANNEL"*/, emu_config_id, alct_config_id, results);

  std::cout << "ANODECHANNEL  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns=results.getColumns();
  for (unsigned rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      if (*column == "AFEB_NUMBER") {
		value = results.getValueAt(rowIndex,*column);
		std::cout << *column + ": " + value->toString() << std::endl;
		std::cout << "type: " << results.getColumnType(*column) << std::endl;
      	if (results.getColumnType(*column)=="int"  ) {
			xdata::Integer * i = dynamic_cast<xdata::Integer *>(value);
			if (i) afeb_number = (int)*i;
		}
      }
    }
    theAlct->SetStandbyRegister_(afeb_number-1,ON);
    for (std::vector<std::string>::iterator column=columns.begin(); column!=columns.end(); ++column) {
      value = results.getValueAt(rowIndex,*column);
		std::cout << "type: " << results.getColumnType(*column) << std::endl;
      if (results.getColumnType(*column)=="int"  ) {
      	xdata::Integer * i = dynamic_cast<xdata::Integer *>(value); 
      	if (i) IntValue=(int)*i;
      }
      StrgValue=value->toString();
      
      if (*column == "AFEB_FINE_DELAY") {theAlct->SetAsicDelay(afeb_number-1,IntValue);    }
      if (*column == "AFEB_THRESHOLD")  {theAlct->SetAfebThreshold(afeb_number-1,IntValue);}
      
      std::cout << *column + ": " + StrgValue << std::endl;
    }   
  }
  copyAnodeChannelToTable(results,theAlct); //this is because the results from TStore have the wrong column types
	setCachedTable(queryViewName,identifier,results);
}
  } 
  }
//  }
