#ifndef _FEDConfigurationEditor_h_
#define _FEDConfigurationEditor_h_

#include "emu/db/ConfigurationEditor.h"
#include "xdata/String.h"
#include "emu/fed/Exception.h"
#include "emu/fed/DBAgent.h"

#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {

		class FEDConfigurationEditor: public emu::db::ConfigurationEditor
		{

		public:

			XDAQ_INSTANTIATOR();

			FEDConfigurationEditor(xdaq::ApplicationStub *s)
			throw (xdaq::exception::Exception);
			
		private: 
			void outputStandardInterface(xgi::Output *out);
			
			void startUpload(xgi::Input *in)
			throw (xcept::Exception);
			
			void getDbUserData();
			
			std::string elementNameFromTableName(const std::string &column);
			
			void fillRootElement(DOMElement *rootElement);
			
			void readChildNodesIntoTable(const std::string &tableName,DOMElement *parent,const std::string &parentIdentifier);
			
			void parseConfigFromXML(xgi::Input *in, xgi::Output *out) throw (xgi::exception::Exception);
			
			bool columnIsUniqueIdentifier(const std::string &columnName,const std::string &tableName);
			
			bool columnIsDatabaseOnly(const std::string &columnName,const std::string &tableName);
			
			bool canChangeColumn(const std::string &columnName,const std::string &tableName);
			
			void getForeignKey(const std::string &childTableName,std::string &parentColumn,std::string &childColumn)
			throw (xcept::Exception);
			
			xdata::UnsignedInteger64 getNewConfigId(const std::string &connectionID);
			
			void fillDatabaseOnlyValues(const std::string &tableName, const std::string &prefix, const std::string &globalKeyColumn, xdata::Serializable &keyValue, const std::string &otherKeyColumn="", xdata::Serializable *otherValue = NULL);
			
			void uploadConfiguration(const std::string &connectionID)
			throw (xcept::Exception);
			
			void uploadCrates(const std::string &connectionID,xdata::UnsignedInteger64 &key,xdata::String &endcapSide)
			throw (xcept::Exception);
			
			//void queryMaxId(const std::string &connectionID, const std::string &queryViewName, const std::string &dbColumn, xdata::UnsignedInteger64 &result) throw (xcept::Exception);
			
			void queryMaxId(const std::string &connectionID, const std::string &tableName, const std::string &dbColumn, xdata::UnsignedInteger64 &result)
			throw (xcept::Exception);
			
			xdata::UnsignedInteger64 getNext(const std::string &connectionID,const std::string &columnName,const std::string tableName);
			
			xdata::UnsignedInteger64 addNewConfiguration(const std::string &connectionID,const std::string &description);
			
			void uploadTable(const std::string &connectionID,const std::string &tableName);
			
			xdata::UnsignedInteger64 addNewSystem(const std::string &connectionID,xdata::UnsignedInteger64 &configID,xdata::String &endcapSide,const std::string &hostname)
			throw (xcept::Exception);
			
			void readFibers(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &rui)
			throw (xcept::Exception);
			
			void readDDU(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &rui)
			throw (emu::fed::exception::DBException);
			
			void readDCC(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
			throw (xcept::Exception);
			
			void readFIFOs(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedInteger &fmm_id)
			throw (xcept::Exception);
			
			void readController(const std::string &connectionID,const std::string &parentIdentifier,xdata::UnsignedInteger64 &key,xdata::UnsignedShort &number)
			throw (emu::fed::exception::DBException);
			
			void readConfigFromDB(xgi::Input * in, xgi::Output * out )
			throw (xgi::exception::Exception);
			
			void diff(const std::string &connectionID, const std::string &queryViewName,const std::string &old_key,const std::string &new_key,const std::string &other_parameter_name,const std::string &other_parameter,xdata::Table &results);
			
			void diff(const std::string &connectionID, const std::string &queryViewName,const std::string &old_key,const std::string &new_key,const std::string &other_parameter_name,const std::string &other_parameter,const std::string &resultKey);
			
			void diffCrate(const std::string &connectionID, const std::string &old_key, const std::string &new_key)
			throw (xcept::Exception);
			
			xdata::String dbUsername_;
			xdata::String dbPassword_;
			xdata::String endcapSide_;
		};
	} // namespace emu::fed
} // namespace emu


#endif
