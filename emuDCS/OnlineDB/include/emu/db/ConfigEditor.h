#ifndef _ConfigEditor_h_
#define _ConfigEditor_h_

#include "xdaq/WebApplication.h"
#include "xdaq/ApplicationGroup.h"
//#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "xdata/TableIterator.h"
#include "xdata/Table.h"
#include "xdata/UnsignedInteger64.h"


#include <sstream>

namespace emu {
namespace db {

class ConfigEditor: public xdaq::WebApplication
{
public:

  typedef std::map<std::string,unsigned int> TableChangeSummary;
  typedef std::map<std::string,TableChangeSummary > ChangeSummary;

  //XDAQ_INSTANTIATOR();

  ConfigEditor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
  ~ConfigEditor();
  //
  void Default(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);

  // HyperDAQ interface
  void outputHeader(xgi::Output * out);
  void outputCurrentConfiguration(xgi::Output * out);
  void outputFooter(xgi::Output * out);
  void outputException(xgi::Output * out, xcept::Exception &e);
  virtual void outputStandardInterface(xgi::Output * out)=0;
  virtual std::string displayName(const std::string &configName, const std::string &identifier, xdata::Table &data);
  void displayChildDiff(std::ostream * out, const std::string &configName, const std::string &parentIdentifier,
      ChangeSummary &changes);
  bool displayCommonTableElements(std::ostream * out, const std::string &configName, const std::string &identifier,
      xdata::Table &currentTable, const std::string &display = "config");
  void displayDiff(std::ostream* out, const std::string &configName, const std::string &identifier,
      ChangeSummary &changes);
  void displayChildConfiguration(xgi::Output * out, const std::string &configName, const std::string &parentIdentifier);
  void displayConfiguration(xgi::Output * out, const std::string &configName, const std::string &identifier);
  void outputTableEditControls(xgi::Output * out, const std::string &tableName, const std::string &prefix = "");
  void outputSingleValue(std::ostream * out, xdata::Serializable *value, const std::string &column,
      const std::string &tableName = "", const std::string &identifier = "", int rowIndex = 0);
  void outputCurrentDiff(xgi::Output * out);
  bool getVersionNumber(const std::string &columnName, std::string &versionNumber);
  std::string withoutVersionNumber(const std::string &columnName);
  std::string copyAttributesToTable(xdata::Table &table, const std::string &tableName, xercesc::DOMElement *node,
      int rowIndex) throw (xcept::Exception);
  void
      copyTableToAttributes(xercesc::DOMElement *node, xdata::Table &table, const std::string &tableName, int rowIndex)
          throw (xcept::Exception);
  void sumChanges(TableChangeSummary &allChanges, TableChangeSummary &changesToThisTable);
  void outputDiffSummary(std::ostream *out, TableChangeSummary &changes);
  void outputDiffSummary(std::ostream *out, ChangeSummary &changes);
  void outputDiff(std::ostream *out, xdata::Table &results, const std::string &tableName, TableChangeSummary &changes);
  void outputTable(std::ostream * out, xdata::Table &results, const std::string &tableName = "",
      const std::string &identifier = "");
  void outputShowHideButton(std::ostream * out, const std::string &configName, const std::string &identifier,
      const std::string &display = "config");
  // Actions

  virtual void parseConfigFromXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void exportAsXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void uploadConfigToDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  virtual void readConfigFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void synchronizeToFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SelectLocalConfFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void uploadConfFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SetTypeDesc(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void incrementValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void setValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void changeSingleValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void outputEndcapSelector(xgi::Output * out);
  void showHideTable(xgi::Input * in, xgi::Output * out, bool show) throw (xgi::exception::Exception);
  void showTable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void hideTable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void viewValues(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  //select two versions for diff
  void selectVersions(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  //select one version for read
  void selectVersion(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void compareVersions(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void getConfigIds(std::vector<std::string> &configIDs, const std::string endcap_side) throw (xcept::Exception);
  // Communication with TStore
  xoap::MessageReference sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception);
  std::string connect() throw (xcept::Exception);
  void disconnect(const std::string &connectionID) throw (xcept::Exception);
  //void diff(const std::string &connectionID, const std::string &queryViewName, const std::string &old_emu_config_id, const std::string &new_emu_config_id,xdata::Table &results) throw (xcept::Exception);
  //void diff(const std::string &connectionID, const std::string &queryViewName, const std::string &old_emu_config_id, const std::string &old_xxx_config_id, const std::string &new_emu_config_id, const std::string &new_xxx_config_id,xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id,
      xdata::Table &results) throw (xcept::Exception);
  //void query(const std::string &connectionID, const std::string &queryViewName, const std::string &emu_config_id, const std::string &xxx_config_id, xdata::Table &results) throw (xcept::Exception);
  void query(const std::string &connectionID, const std::string &queryViewName,
      const std::map<std::string, std::string> &queryParameters, xdata::Table &results) throw (xcept::Exception);
  void queryMaxId(const std::string &connectionID, const std::string &queryViewName, const std::string &dbTable,
      const std::string &dbColumn, const std::string endcap_side, xdata::Table &results) throw (xcept::Exception);
  void getDefinition(const std::string &connectionID, const std::string &insertViewName) throw (xcept::Exception);
  void insert(const std::string &connectionID, const std::string &insertViewName, xdata::Table &newRows)
      throw (xcept::Exception);
  void synchronize(const std::string &connectionID, const std::string &syncMode, const std::string &syncPattern)
      throw (xcept::Exception);
  //void getConfiguration(const std::string &xpath) throw (xcept::Exception);

  //
  virtual void getDbUserData()=0;

  // XML Data Uploads
  virtual void startUpload(xgi::Input * in) throw (xcept::Exception)=0;
  //xdata::UnsignedInteger64 getConfigId(const std::string &dbTable, const std::string &dbColumn, const std::string endcap_side) throw (xcept::Exception);
  void uploadConfiguration(const std::string &connectionID, const std::string endcap_side) throw (xcept::Exception);

  // DB Data Reads
  //void simpleDiff(const std::string &queryViewName,const std::string &connectionID, const std::string &old_emu_config_id, const std::string &old_xxx_config_id, const std::string &new_emu_config_id, const std::string &new_xxx_config_id,const std::string &identifier) throw (xcept::Exception);
  virtual void diffCrate(const std::string &connectionID, const std::string &old_key, const std::string &new_key)
      throw (xcept::Exception)=0;

  //protected:
  template<class xdataType>
  static void set(xdata::Serializable *originalValue, const std::string &newValue);
  template<class xdataType, typename simpleType>
  static void add(xdata::Serializable *originalValue, const std::string &addend);
  std::vector<std::string> crateIDsInDiff;
  std::string xmlfile_;
  std::string viewID_;
  void addTable(const std::string &tableName);
  void addChildTable(const std::string &parentTableName, const std::string &childTableName);
  void setUsername(const std::string &username);
  void setPassword(const std::string &password);
  //private:
  virtual bool shouldDisplayInHex(const std::string &columnName);
  std::string valueToString(xdata::Serializable *value, const std::string &columnName);
  std::string xdataToHex(xdata::Serializable *xdataValue);
  DOMNode *DOMOfCurrentTables();
  virtual std::string attributeNameFromColumnName(const std::string &column);
  virtual std::string elementNameFromTableName(const std::string &column);

  void addChildNodes(DOMElement *parentElement, const std::string &configName, const std::string &parentIdentifier);
  std::string configIDOptions(std::vector<std::string> &configIDs);
  void outputFileSelector(xgi::Output * out) throw (xgi::exception::Exception);
  void outputCompareVersionsForm(xgi::Output * out, const std::string &endcap_side);
  std::string newCell(xdata::Serializable *newValue, xdata::Serializable *oldValue);
  bool getNextColumn(std::vector<std::string>::iterator &nextColumn, std::string &columnWithoutVersionNumber,
      const std::vector<std::string>::iterator &currentColumn, const std::string &tableName, const std::vector<
          std::string>::iterator &end);
  void outputDiffRow(std::ostream * out, xdata::Table &results, int rowIndex, bool vertical,
      const std::string &tableName, TableChangeSummary &changes);
  bool shouldDisplayConfiguration(const std::string &configName, const std::string &identifier);
  std::string fullTableID(const std::string &configName, const std::string &identifier);
  bool tableHasColumn(xdata::Table &table, const std::string &column);
  void setValueFromString(xdata::Serializable *value, const std::string &newValue) throw (xdata::exception::Exception);
  void getRangeOfTables(const cgicc::Cgicc &cgi, std::map<std::string, xdata::Table> &tables, std::map<std::string,
      xdata::Table>::iterator &firstTable, std::map<std::string, xdata::Table>::iterator &lastTable);
  void getRangeOfTables(const std::string &prefix, std::map<std::string, xdata::Table> &tables, std::map<std::string,
      xdata::Table>::iterator &firstTable, std::map<std::string, xdata::Table>::iterator &lastTable);
  std::string crateIdentifierString(int crateID);
  std::string crateIdentifierString(const std::string &crateID);
  void setConfigID(xdata::Table &newRows, size_t rowId, const std::string &columnName, xdata::UnsignedInteger64 &id);

  xdata::Table &getCachedTableFrom(std::map<std::string, std::map<std::string, xdata::Table> > &cache,
      const std::string &insertViewName, const std::string &identifier
      /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception);

  void clearCachedDiff();
  void setCachedDiff(const std::string &insertViewName, const std::string &identifier, xdata::Table &table)
      throw (xcept::Exception);
  xdata::Table &getCachedDiff(const std::string &insertViewName, const std::string &identifier)
      throw (xcept::Exception);
  //xdata::Table &getCachedDiff(const std::string &insertViewName,int crateIndex) throw (xcept::Exception);
  //void setCachedDiff(const std::string &insertViewName,int crateIndex,xdata::Table &table) throw (xcept::Exception);

  void clearCachedTables();
  void setCachedTable(const std::string &insertViewName, const std::string &identifier, xdata::Table &table)
      throw (xcept::Exception);
  xdata::Table &getCachedTable(const std::string &insertViewName, const std::string &identifier)
      throw (xcept::Exception);
  xdata::Table &getCachedTable(const std::string &insertViewName, int crateIndex) throw (xcept::Exception);
  //void setCachedTable(const std::string &insertViewName,int crateIndex,xdata::Table &table) throw (xcept::Exception);

  virtual bool columnIsUniqueIdentifier(const std::string &columnName, const std::string &tableName)=0;
  std::string uniqueIdentifierForRow(xdata::Table &table, const std::string &tableName, unsigned int rowIndex);
  virtual bool columnIsDatabaseOnly(const std::string &columnName, const std::string &tableName);
  virtual bool canChangeColumn(const std::string &columnName, const std::string &tableName);
  virtual bool canChangeColumnGlobally(const std::string &columnName, const std::string &tableName);
  static bool isNumericType(const std::string &xdataType);
  void getTableDefinitionsIfNecessary() throw ();
  virtual void getTableDefinitions(const std::string &connectionID);
  bool moreThanOneChildConfigurationExists(const std::string &configName, const std::string &parentIdentifier);
  std::string keyContaining(std::map<std::string, xdata::Table> &haystack, const std::string &needle)
      throw (xdaq::exception::Exception);
  xdata::Table &valueForKeyContaining(std::map<std::string, xdata::Table> &haystack, const std::string &needle)
      throw (xdaq::exception::Exception);
  void setTableNamePrefix(const std::string &prefix);
  void setViewID(const std::string &viewID);
  void setTopLevelTableName(const std::string &tableName);
  //set the name of what you are configuring (used in the page title)
  void setHumanReadableConfigName(const std::string &configName);
  void setDisplayBooleansAsIntegers(bool displayBooleansAsIntegers);
  virtual void fillRootElement(DOMElement *rootElement);
  void setXMLRootElement(const std::string &rootElementName);
  void setConfigurationDirectory(const std::string &configurationDirectory);
  std::string fullConfigurationDirectory();
  void createCredentialString();
  void fillCrateRealName(int id, std::string name);

  std::string config_type_;
  std::string config_desc_;
  std::string xmlpath_;
  std::string dbUserFile_;
  std::string dbUserAndPassword_;
  std::string password_;
  std::string username_;
  xdata::UnsignedInteger64 emu_config_id_;
  std::string syncPattern_;
  xdata::Table configIDs;
  std::string configName_; //to be set by subclasses
  std::string configurationDirectory_;
  //std::vector<std::string> topLevelIdentifiersInDiff;
  std::string topLevelTableName_;
  std::string rootElementName_;
  bool displayBooleansAsIntegers_;

  //the data is kept as tables in memory, to allow a common interface for changing values
  //the most recent data is always in these tables.
  //the key in the first map is the table name as passed to TStore (e.g. "vcc", "tmb")
  //the value is a map of all tables which come from a query of that table (e.g. vcc from a particular crate)
  //the key in this inside map can be anything which uniquely identifies that particular table 
  //so it can be edited.
  //it is named hierarchically beginning with the crate identifier so that it's easy to loop through
  //just the tables relating to a particular crate or chamber to change the values.
  std::map<std::string, std::map<std::string, xdata::Table> > currentTables;

  //these are not changed in memory but should be kept in memory so that they don't have to be reloaded every time you show or hide something.
  std::map<std::string, std::map<std::string, xdata::Table> > currentDiff;

  std::map<std::string, xdata::Table> currentTestSummary;
  std::map<std::string, signed int> TTCrxIDs;
  //maps the keys used in currentDiff and currentTables to the titles of those tables on the display
  //std::map<std::string,std::string> titles;

  std::map<std::string, bool> tablesToDisplay;

  std::map<std::string, xdata::Table> tableDefinitions;
  //this stores the hierarchical structure the tables should be displayed in
  std::map<std::string, std::vector<std::string> > tableNames;
  //and this is just a list of the 'top level' tables which are immediately in the crate.
  std::vector<std::string> topLevelTables;
  
  std::vector<std::string> crateRealNames;
};

// Convert to string
template <class T>
inline std::string to_string(const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

// Convert to HEX
template <class T>
inline bool convertToHex(std::string &dataHex, const std::string format, const T& data)
{
  char buffer[100];
  int err;
  bool converted = false;
  err = sprintf(buffer, format.c_str(), data);
  if (err == 0)
  {
    std::cout << "Could not convert " << to_string(data) << " to hexadecimal format" << std::endl;
    dataHex = "";
  }
  else
  {
    dataHex = to_string(buffer);
    converted = true;
  }
  return converted;
}

} // namespace emu::db
} // namespace emu


#endif
