// $Id: ConfigEditor.cc,v 1.1 2011/09/09 16:04:43 khotilov Exp $

/* code is mostly copied and modified from ConfigurationEditor.cc */

#include "emu/db/ConfigEditor.h"
#include "emu/db/TStoreRequest.h"

#include <time.h>
#include "toolbox/TimeInterval.h"
#include "toolbox/TimeVal.h"
#include "toolbox/Runtime.h"
#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/LoadDOM.h"
#include "tstore/client/Client.h"
#include "xcept/tools.h"
#include "xdata/Integer.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/UnsignedShort.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/SimpleType.h"
#include "xdata/Boolean.h"
#include "xoap/domutils.h"
#include "xoap/DOMParserFactory.h"


namespace emu { namespace db {


ConfigEditor::ConfigEditor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s)
{
  xgi::bind(this, &ConfigEditor::Default, "Default");
  xgi::bind(this, &ConfigEditor::parseConfigFromXML, "parse");
  xgi::bind(this, &ConfigEditor::uploadConfigToDB, "upload");
  xgi::bind(this, &ConfigEditor::readConfigFromDB, "read");
  xgi::bind(this, &ConfigEditor::synchronizeToFromDB, "sync");
  xgi::bind(this, &ConfigEditor::SelectLocalConfFile, "selectConfFile");
  xgi::bind(this, &ConfigEditor::uploadConfFile, "UploadConfFile");
  xgi::bind(this, &ConfigEditor::incrementValue, "incrementValue");
  xgi::bind(this, &ConfigEditor::setValue, "setValue");
  xgi::bind(this, &ConfigEditor::viewValues, "viewValues");
  xgi::bind(this, &ConfigEditor::changeSingleValue, "changeSingleValue");
  xgi::bind(this, &ConfigEditor::showTable, "Show");
  xgi::bind(this, &ConfigEditor::hideTable, "Hide");
  xgi::bind(this, &ConfigEditor::selectVersions, "selectVersions"); //select two versions to compare
  xgi::bind(this, &ConfigEditor::compareVersions, "compareVersions");
  xgi::bind(this, &ConfigEditor::selectVersion, "selectVersion"); //select one version to load
  xgi::bind(this, &ConfigEditor::exportAsXML, "exportAsXML");
  std::string HomeDir_ = getenv("HOME");
  xmlpath_ = HomeDir_ + "/config/" + configurationDirectory_ + "/"; //xml file chosen must be in this directory. If you choose something in another directory then it will look for it in here and fail.
  xmlfile_ = "";
  dbUserFile_ = HomeDir_ + "/dbuserfile.txt";
  config_type_ = "GLOBAL";
  config_desc_ = "manual entry";
  crateRealNames.clear();
  for(int i=0;i<65;i++) crateRealNames.push_back("");
}


ConfigEditor::~ConfigEditor() {}


void ConfigEditor::uploadConfFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    cgicc::Cgicc cgi(in);
    cgicc::const_file_iterator file;
    file = cgi.getFile("xmlFileNameUpload");
    //
    if (file != cgi.getFiles().end())
    {
      std::ofstream TextFile;
      xmlfile_ = fullConfigurationDirectory() + (*file).getFilename();
      TextFile.open(xmlfile_.c_str());
      (*file).writeToStream(TextFile);
      TextFile.close();
    }
    //
    //ParsingXML();
    //
    std::cout << "UploadConfFile done" << std::endl;
    //
    this->Default(in, out);
  }
  catch (const std::exception & e)
  {
    XCEPT_RAISE(xgi::exception::Exception, (std::string)"problem uploading file"+e.what());
  }
}


//get the file name from the new select box.
void ConfigEditor::SelectLocalConfFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    std::cout << "Button: Select XML file" << std::endl;
    cgicc::Cgicc cgi(in);
    if (cgi.getElement("xmlFileName") != cgi.getElements().end())
    {
      xmlfile_ = cgi["xmlFileName"]->getValue();
      std::cout << "Select XML file " << xmlfile_ << std::endl;
    }
    this->Default(in, out);
  }
  catch (const std::exception &e)
  {
    XCEPT_RAISE(xgi::exception::Exception, e.what());
  }
}


/*outputs three different ways of selecting a file (presumably someone will decide which of these to keep)
  1) A text box where you can put the full path of a file which is accessible from the server.
  2) A file selector with which you can upload a file which will be saved in the /config/<configDirectory>/ directory on the server
  3) A menu where you can select from files which are already in the /config/<configDirectory>/ directory on the server.
*/
void ConfigEditor::outputFileSelector(xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    *out << "<table border=\"0\" cellpadding=\"5\"><tr><td align=\"right\">";
    std::string method = toolbox::toString("/%s/selectConfFile", getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method", "POST").set("action", method) << std::endl;
    *out << "enter full path of file on server: </td><td>" << cgicc::input().set("type", "text").set("name",
        "xmlFilename").set("size", "90").set("ENCTYPE", "multipart/form-data").set("value", xmlfile_) << std::endl;
    *out << cgicc::input().set("type", "submit").set("value", "Select") << std::endl;
    *out << cgicc::form() << std::endl;
    //
    // Upload file...
    //
    std::string methodUpload = toolbox::toString("/%s/UploadConfFile", getApplicationDescriptor()->getURN().c_str());
    *out << "</tr><tr><td align=\"right\">"
        << cgicc::form().set("method", "POST").set("enctype", "multipart/form-data").set("action", methodUpload)
        << std::endl;
    *out << "or upload file:  </td><td>"
        << cgicc::input().set("type", "file").set("name", "xmlFilenameUpload").set("size", "90") << std::endl;
    *out << cgicc::input().set("type", "submit").set("value", "Upload") << std::endl;
    *out << cgicc::form() << std::endl;

    std::string methodSelect = toolbox::toString("/%s/selectConfFile", getApplicationDescriptor()->getURN().c_str());
    *out << "</tr><tr><td align=\"right\">" << cgicc::form().set("method", "POST").set("action", methodSelect)
        << std::endl;
    //*out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","90") << std::endl;
    // To access the available files, we need a few directories.
    std::string homeDir(getenv("HOME"));
    std::string configPath;
    configPath = fullConfigurationDirectory();

    // Don't use boost to get all the xml files in this directory
    std::vector<std::string> xmlFiles = toolbox::getRuntime()->expandPathName(configPath + "*.xml");

    *out << "or select existing file:  </td><td>"
        << cgicc::select() .set("id", "xml_file_select") .set("class","file_select") .set("name", "xmlFilename")
        << std::endl;
    for (std::vector<std::string>::const_iterator iFile = xmlFiles.begin(); iFile != xmlFiles.end(); iFile++)
    {
      cgicc::option opt(*iFile);
      opt.set("value", *iFile);
      //if (xmlFile_ == *iFile) opt.set("selected", "true");
      *out << opt << std::endl;
    }
    *out << cgicc::select() << std::endl;
    *out << cgicc::input().set("type", "submit").set("value", "Select") << std::endl;
    *out << cgicc::form() << std::endl;
    //
    *out << /*cgicc::fieldset() <<*/cgicc::br() << "</td></tr></table>";
  }
  catch (std::exception &e)
  {
    XCEPT_RAISE(xgi::exception::Exception,(std::string)"Could not get available config files. "+e.what());
  }
}


void ConfigEditor::outputHeader(xgi::Output * out)
{
  out->getHTTPResponseHeader().addHeader("Content-Type", "text/html");
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir", "ltr") << std::endl;
  *out << cgicc::title("Emu " + configName_ + " Config via TStore") << std::endl;
  xgi::Utils::getPageHeader(out,
      getApplicationDescriptor()->getClassName(),
      getApplicationDescriptor()->getContextDescriptor()->getURL(),
      getApplicationDescriptor()->getURN(),
      "/hyperdaq/images/HyperDAQ.jpg");
}


void ConfigEditor::Default(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    // As of now this is a soft link pointing to the xml file to be uploaded
    outputHeader(out);
    outputStandardInterface(out);
    outputCurrentConfiguration(out);
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
}


void ConfigEditor::addTable(const std::string &tableName)
{
  topLevelTables.push_back(tableName);
  std::vector<std::string> emptyVector;
  tableNames[tableName] = emptyVector;
}


void ConfigEditor::addChildTable(const std::string &parentTableName, const std::string &childTableName)
{
  tableNames[parentTableName].push_back(childTableName);
  std::vector<std::string> emptyVector;
  tableNames[childTableName] = emptyVector;
}


std::string ConfigEditor::fullTableID(const std::string &configName, const std::string &identifier)
{
  return configName + "_" + identifier;
}


bool ConfigEditor::shouldDisplayConfiguration(const std::string &configName, const std::string &identifier)
{
  std::string key = fullTableID(configName, identifier);
  if (tablesToDisplay.count(key))
  {
    return tablesToDisplay[key];
  }
  return false;
}


/*void ConfigEditor::setShouldDisplayConfiguration(const std::string &configName,const std::string &identifier,bool show) {
	std::string key=fullTableID(configName,identifier);
	if (tablesToDisplay.count(key)) {
		tablesToDisplay[key]=show;
	}
}*/

void ConfigEditor::outputShowHideButton(
    std::ostream * out,
    const std::string &configName,
    const std::string &identifier,
    const std::string &display)
{
  std::string key = fullTableID(configName, identifier);
  std::string action;
  if (shouldDisplayConfiguration(configName, identifier)) action = "Hide";
  else action = "Show";
  std::string anchor = key;
  //remove spaces from the end, because these are stripped from the URL and then it doesn't match the anchor
  anchor.erase(anchor.find_last_not_of(" ") + 1);
  //add an anchor so we can scroll immediately to the thing we just hid or showed
  *out << cgicc::a().set("name", anchor);
  *out << cgicc::form().set("method", "POST").set("action", toolbox::toString("/%s/%s#%s",
      getApplicationDescriptor()->getURN().c_str(), action.c_str(), anchor.c_str())) << std::endl;
  *out << cgicc::input().set("type", "hidden").set("name", "table").set("value", key) << std::endl;
  *out << cgicc::input().set("type", "hidden").set("name", "display").set("value", display) << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", action) << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::a();
}


void ConfigEditor::displayConfiguration(
    xgi::Output * out,
    const std::string &configName,
    const std::string &identifier)
{
  try
  {
    xdata::Table &currentTable = getCachedTable(configName, identifier);
    if (displayCommonTableElements(out, configName, identifier, currentTable))
    {
      outputTable(out, currentTable, configName, identifier);
    }
  }
  catch (xcept::Exception &e)
  {
    LOG4CPLUS_WARN(this->getApplicationLogger(),"could not display configuration of "+configName+": "+e.what());
  }
}


void ConfigEditor::sumChanges(TableChangeSummary &allChanges, TableChangeSummary &changesToThisTable)
{
  for (TableChangeSummary::iterator column = changesToThisTable.begin(); column != changesToThisTable.end(); ++column)
  {
    allChanges[(*column).first] += (*column).second;
  }
}


void ConfigEditor::displayDiff(
    std::ostream * out,
    const std::string &configName,
    const std::string &identifier,
    ChangeSummary &changes)
{
  try
  {
    xdata::Table &currentTable = getCachedDiff(configName, identifier);
    std::ostringstream diffDisplay;
    TableChangeSummary changesToThisTable;
    outputDiff(&diffDisplay, currentTable, configName, changesToThisTable);
    outputDiffSummary(out, changesToThisTable);
    sumChanges(changes[configName], changesToThisTable);
    if (displayCommonTableElements(out, configName, identifier, currentTable, "diff"))
    {
      *out << diffDisplay.str();
    }
  }
  catch (xcept::Exception &e)
  {
    LOG4CPLUS_WARN(this->getApplicationLogger(),"could not display diff of "+configName+": "+e.what());
  }
}


//subclasses should override
std::string ConfigEditor::elementNameFromTableName(const std::string &tableName)
{
  return toolbox::toupper(tableName);
}


//subclasses should override if attributes in XML differ from the column names
//(usually because XML is case-sensitive and column names are in uppercase)
std::string ConfigEditor::attributeNameFromColumnName(const std::string &column)
{
  return column;
}


std::string ConfigEditor::xdataToHex(xdata::Serializable *xdataValue)
{
  //this is slower but it's easier than having code for each numeric subclass, and speed is not important here
  std::string valueAsString = xdataValue->toString();
  std::istringstream stream(valueAsString);
  long long int intValue;
  stream >> intValue;
  std::string hexValue;
  //see notes in EmuPCrateConfigTStore::shouldDisplayInHex
  //if other hex values need more digits, a new parameter should be added to that function to say how many.
  //But at the moment most hex values are stored as strings.
  convertToHex(hexValue, "%02Lx", intValue);
  //std::cout << "converted " << valueAsString << " to " << intValue << " to " << hexValue << std::endl;
  return hexValue;
}


//some values are read from the XML as hex values, and should be exported to XML as hex
//and displayed in hex on the web interface.
bool ConfigEditor::shouldDisplayInHex(const std::string &columnName)
{
  return false; //should be overridden by subclasses
}


void ConfigEditor::copyTableToAttributes(
    xercesc::DOMElement *node,
    xdata::Table &table,
    const std::string &tableName,
    int rowIndex) throw (xcept::Exception)
{
  std::vector<std::string> columns = table.getColumns();
  for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
  {
    xdata::Serializable *xdataValue = table.getValueAt(rowIndex, *column);
    std::string value;
    if (!columnIsDatabaseOnly(*column, tableName))
    {
      value = valueToString(xdataValue, *column);
      node->setAttribute(xoap::XStr(attributeNameFromColumnName(*column)), xoap::XStr(value));
      std::cout << "adding attribute with name " << *column << std::endl;
    }
  }
}


void ConfigEditor::addChildNodes(
    DOMElement *parentElement,
    const std::string &configName,
    const std::string &parentIdentifier)
{
  DOMDocument* doc = parentElement->getOwnerDocument();
  if (currentTables.count(configName))
  {
    std::map<std::string, xdata::Table> &tables = currentTables[configName];
    std::map<std::string, xdata::Table>::iterator table;
    std::vector<std::string> subTables;
    if (tableNames.count(configName))
    {
      subTables = tableNames[configName];
    }
    //loop through all tables whose key begins with the parent ID
    //all keys begin with the appropriate parent ID
    std::map<std::string, xdata::Table>::iterator firstTable;
    std::map<std::string, xdata::Table>::iterator lastTable;
    getRangeOfTables(parentIdentifier, tables, firstTable, lastTable);
    for (table = firstTable; table != lastTable; ++table)
    {
      //for each row, add an element with the name configName, add it to parentElement
      //for each column, add an attribute.
      unsigned int rowCount = (*table).second.getRowCount();
      for (unsigned rowIndex = 0; rowIndex < rowCount; rowIndex++)
      {
        std::cout << "adding element with name " << configName << std::endl;
        DOMElement *newElement = doc->createElement(xoap::XStr(elementNameFromTableName(configName)));
        copyTableToAttributes(newElement, (*table).second, configName, rowIndex);
        parentElement->appendChild(newElement);
        for (std::vector<std::string>::iterator subTable = subTables.begin(); subTable != subTables.end(); ++subTable)
        {
          //std::cout << "crate subtable, showing " << (*subTable) << " of " << (*table).first << std::endl;
          addChildNodes(newElement, *subTable, (*table).first);
        }
      }
    }
  }
  else std::cout << "no tables of type " << configName << std::endl;
}


DOMNode *ConfigEditor::DOMOfCurrentTables()
{
  //this still needs to be changed to work for FED crates
  try
  {
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(xoap::XStr("LS"));
    DOMDocument* doc = impl->createDocument(xoap::XStr(""), // root element namespace URI.
        xoap::XStr(rootElementName_), // root element name
        0); // document type object (DTD).
    fillRootElement(doc->getDocumentElement());
    std::map<std::string, xdata::Table> &crates = currentTables[topLevelTableName_];
    for (std::map<std::string, xdata::Table>::iterator crate = crates.begin(); crate != crates.end(); ++crate)
    {
      DOMElement *crateElement = doc->createElement(xoap::XStr(elementNameFromTableName(topLevelTableName_)));
      //there is only one row in each crate table
      copyTableToAttributes(crateElement, (*crate).second, topLevelTableName_, 0);
      doc->getDocumentElement()->appendChild(crateElement);
      for (std::vector<std::string>::iterator tableName = topLevelTables.begin(); tableName != topLevelTables.end(); ++tableName)
      {
        addChildNodes(crateElement, *tableName, (*crate).first);
      }
    }
    return doc;
  }
  catch (DOMException &e)
  {
    XCEPT_RAISE(xgi::exception::Exception,xoap::XMLCh2String(e.msg));
  }
}


void ConfigEditor::exportAsXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  DOMNode *pDoc = DOMOfCurrentTables();
  std::string XML = tstoreclient::writeXML(pDoc);

  cgicc::Cgicc cgi(in);
  try
  {
    if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1)
    {
      cgicc::HTTPResponseHeader xmlHeader("HTTP/1.1", 200, "OK");
      xmlHeader.addHeader("Content-Type", "text/xml");
      std::ostringstream attachment;

      std::string emu_config_id = cgi("configID");
      attachment << "attachment; filename=pc_config_" << toolbox::escape(emu_config_id)
        << "_" << toolbox::TimeVal(time(NULL)).toString("%Y-%m-%d_%H-%M-%S_%Z",toolbox::TimeVal::gmt) << ".xml";
      xmlHeader.addHeader("Content-Disposition", attachment.str());
      out->setHTTPResponseHeader(xmlHeader);
    }

    *out << XML;
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
  /*
  outputHeader(out);
  outputStandardInterface(out);
  *out << cgicc::textarea().set("name", "config").set("cols", "60").set("rows", "15") << XML << cgicc::textarea();
  outputFooter(out);
  */
}


//displays either the hide/show button and the table name, or "no rows" if the table is empty. Returns whether the table should be shown.
//\a display is either "diff" or "config" depending on what should be displayed when a show/hide button is pressed. Config is the default.
bool ConfigEditor::displayCommonTableElements(
    std::ostream * out,
    const std::string &configName,
    const std::string &identifier,
    xdata::Table &currentTable,
    const std::string &display)
{
  if (currentTable.getRowCount())
  {
    outputShowHideButton(out, configName, identifier, display);
    if (shouldDisplayConfiguration(configName, identifier))
    {
      *out << configName << std::endl;
      return true;
    }
  }
  else
  {
    *out << "no rows";
    std::cout << configName << " no rows" << std::endl;
  }
  return false;
}


std::string ConfigEditor::displayName(
    const std::string &configName,
    const std::string &identifier,
    xdata::Table &data)
{
  return identifier;
}


void ConfigEditor::displayChildDiff(
    std::ostream * out,
    const std::string &configName,
    const std::string &parentIdentifier,
    ChangeSummary &changes)
{
  if (currentDiff.count(configName))
  {
    std::map<std::string, xdata::Table> &tables = currentDiff[configName];
    std::map<std::string, xdata::Table>::iterator table;
    std::vector<std::string> subTables;
    if (tableNames.count(configName))
    {
      subTables = tableNames[configName];
    }
    //loop through all tables whose key begins with the crateID
    //all keys begin with the appropriate crateID
    std::map<std::string, xdata::Table>::iterator firstTable;
    std::map<std::string, xdata::Table>::iterator lastTable;
    getRangeOfTables(parentIdentifier, tables, firstTable, lastTable);
    for (table = firstTable; table != lastTable; ++table)
    {
      *out << "<p/><table border=\"2\" cellpadding=\"10\">";
      std::string heading;
      if (!subTables.empty())
        heading = displayName(configName, (*table).first, (*table).second);
      else
        heading = parentIdentifier; //there is no need to specify which one since there should be only one
      *out << "<tr><td bgcolor=\"#FFCCFF\">" << configName << " of " << heading << "</td></tr><tr><td>" << std::endl;
      displayDiff(out, configName, (*table).first, changes);

      for (std::vector<std::string>::iterator subTable = subTables.begin(); subTable != subTables.end(); ++subTable)
      {
        displayChildDiff(out, *subTable, (*table).first, changes);
      }
      *out << "</tr></td></table>";
    }
  }
  else
  {
    LOG4CPLUS_DEBUG(this->getApplicationLogger(),"no diffs of of "+configName);
  }
}


//now that I have changed it to show the hierarchical structure even when there is always only one child configuration of a particular type,
//it needs to check that there's more than one of them before showing the 'update all...' box.
bool ConfigEditor::moreThanOneChildConfigurationExists(
    const std::string &configName,
    const std::string &parentIdentifier)
{
  std::map<std::string, xdata::Table> &tables = currentDiff[configName];
  std::map<std::string, xdata::Table>::iterator firstTable;//=tables.lower_bound(parentIdentifier);
  std::map<std::string, xdata::Table>::iterator lastTable;//=tables.upper_bound(parentIdentifier+"~"); //~ is sorted last, so this should find the first key with a different prefix
  getRangeOfTables(parentIdentifier, tables, firstTable, lastTable);
  if (firstTable == tables.end() || firstTable == lastTable) return false;
  ++firstTable;
  return firstTable != lastTable;
}


void ConfigEditor::displayChildConfiguration(
    xgi::Output * out,
    const std::string &configName,
    const std::string &parentIdentifier)
{
  if (currentTables.count(configName))
  {
    std::map<std::string, xdata::Table> &tables = currentTables[configName];
    std::map<std::string, xdata::Table>::iterator table;

    std::vector<std::string> subTables;
    if (tableNames.count(configName))
    {
      subTables = tableNames[configName];
    }
    //if this configuration has any child configurations (e.g. it is csc and there are various other configurations per csc)
    //then show controls for editing those values for the entire crate
    if (!subTables.empty() && moreThanOneChildConfigurationExists(configName, parentIdentifier))
    {
      *out << "<table border=\"2\" cellpadding=\"10\"><tr><td>Update all " << configName << "s in " << parentIdentifier
          << "</td></tr><tr><td>";
      std::vector<std::string> &subTables = tableNames[configName];
      for (std::vector<std::string>::iterator subTable = subTables.begin(); subTable != subTables.end(); ++subTable)
      {
        //*out << "table " << *subTable;
        *out << cgicc::br();
        outputTableEditControls(out, *subTable, parentIdentifier);
      }
      *out << "</tr></td></table>";
    }
    //loop through all tables whose key begins with the crateID
    //all keys begin with the appropriate crateID
    std::map<std::string, xdata::Table>::iterator firstTable;
    std::map<std::string, xdata::Table>::iterator lastTable;
    getRangeOfTables(parentIdentifier, tables, firstTable, lastTable);
    for (table = firstTable; table != lastTable; ++table)
    {
      *out << "<p/><table border=\"2\" cellpadding=\"10\">";
      std::string heading;
      if (!subTables.empty())
        heading = (*table).first;
      else
        heading = parentIdentifier; //there is no need to specify which one since there should be only one
      *out << "<tr><td bgcolor=\"#FFCCFF\">" << configName << " of " << heading << "</td></tr><tr><td>" << std::endl;

      try
      {
        xdata::Table &currentTable = getCachedTable(configName, (*table).first);
        if (currentTable.getRowCount() > 1)
        {
          //only display the 'change all' controls if there's more than one row to change
          outputTableEditControls(out, configName, (*table).first);
        }
      }
      catch (xcept::Exception &e)
      {
        //if there is no configuration loaded, this is not really a problem, we don't want to display edit controls in this case
      }
      displayConfiguration(out, configName, (*table).first);

      for (std::vector<std::string>::iterator subTable = subTables.begin(); subTable != subTables.end(); ++subTable)
      {
        //*out << "crate subtable, showing " << (*subTable) << " of " << (*table).first << std::endl;
        displayChildConfiguration(out, *subTable, (*table).first);
      }
      *out << "</tr></td></table>";
    }
  }
}


void ConfigEditor::outputCurrentDiff(xgi::Output * out)
{
  //std::map<std::string,xdata::Table> &crates=currentDiff[topLevelTableName_];
  for (std::vector<std::string>::iterator crateID = crateIDsInDiff.begin(); crateID != crateIDsInDiff.end(); ++crateID)
  {
    //or(std::map<std::string,xdata::Table>::iterator crate=crates.begin(); crate!=crates.end(); ++crate) {
    *out << "<p/><table border=\"2\" cellpadding=\"10\">";
    //should probably select the crate labels as well so that this can show it
    //std::string crateID=(*crate).first;
    *out << "<tr><td bgcolor=\"#FFFFCC\">Crate " << *crateID << "</td></tr><tr><td>" << std::endl;
    ChangeSummary changes;
    std::ostringstream diffDisplay;
    for (std::vector<std::string>::iterator tableName = topLevelTables.begin(); tableName != topLevelTables.end(); ++tableName)
    {
      //displayDiff(&diffDisplay,(*tableName),"CRATE "+*crateID,changes);
      displayChildDiff(&diffDisplay, (*tableName), crateIdentifierString(*crateID), changes);
    }
    outputDiffSummary(out, changes);
    outputShowHideButton(out, "wholecrate", crateIdentifierString(*crateID), "diff");
    if (shouldDisplayConfiguration("wholecrate", crateIdentifierString(*crateID)))
    {
      *out << diffDisplay.str();
    }
    *out << "</tr></td></table>";
  }
  outputFooter(out);
}


void ConfigEditor::outputCurrentConfiguration(xgi::Output * out)
{
  //if (TStore_myEndcap_) {
  //*out << "got TStore_myEndcap_" << std::endl;
  if (currentTables[topLevelTableName_].size())
  {
    *out << "<table border=\"2\" cellpadding=\"10\"><tr><td>Update all crates</td></tr><tr><td>";
    for (std::map<std::string, xdata::Table>::iterator tableDefinition = tableDefinitions.begin();
        tableDefinition != tableDefinitions.end(); ++tableDefinition)
    {
      if (tableDefinition != tableDefinitions.begin() && (*tableDefinition).first != topLevelTableName_)
        *out << cgicc::br() << cgicc::hr() << cgicc::br();
      outputTableEditControls(out, (*tableDefinition).first);
    }
    *out << "</tr></td></table>";
    std::map<std::string, xdata::Table> &crates = currentTables[topLevelTableName_];
    for (std::map<std::string, xdata::Table>::iterator crate = crates.begin(); crate != crates.end(); ++crate)
    {
      *out << "<p/><table border=\"2\" cellpadding=\"10\">";
      *out << "<tr><td bgcolor=\"#FFFFCC\">" << displayName(topLevelTableName_, (*crate).first, (*crate).second)
          << "</td></tr><tr><td>" << std::endl;
      outputShowHideButton(out, "wholecrate", (*crate).first);
      if (shouldDisplayConfiguration("wholecrate", (*crate).first))
      {
        for (std::vector<std::string>::iterator tableName = topLevelTables.begin(); tableName != topLevelTables.end(); ++tableName)
        {
          displayChildConfiguration(out, *tableName, (*crate).first);
          //displayConfiguration(out,*tableName,(*crate).first);
        }
      }
      *out << "</tr></td></table>";
    }
  }
  //}
  outputFooter(out);
}


//to be overridden in subclasses
void ConfigEditor::outputFooter(xgi::Output * out)
{
  *out << cgicc::fieldset() << cgicc::html();
  //  xgi
}


void ConfigEditor::outputException(xgi::Output * out, xcept::Exception &e)
{
  std::cout << e.message() << std::endl;
  *out << "<p>" << e.message() << "</p>" << std::endl;
}


//to be overridden by subclasses
//returns whether a column should be changeable using the 'change all' controls.
bool ConfigEditor::canChangeColumnGlobally(const std::string &columnName, const std::string &tableName)
{
  return canChangeColumn(columnName, tableName);
}


//to be overridden
bool ConfigEditor::columnIsDatabaseOnly(const std::string &columnName, const std::string &tableName)
{
  return columnName.find("CONFIG_ID") != std::string::npos;
}


//to be overridden
bool ConfigEditor::canChangeColumn(const std::string &columnName, const std::string &tableName)
{
  return !columnIsDatabaseOnly(columnName, tableName);
}


bool ConfigEditor::isNumericType(const std::string &xdataType)
{
  std::string numericTypesC[] = {
      (std::string) "float",
      (std::string) "double",
      (std::string) "int",
      (std::string) "int 32",
      (std::string) "int 64",
      (std::string) "unsigned long",
      (std::string) "unsigned int",
      (std::string) "unsigned int 32",
      (std::string) "unsigned int 64",
      (std::string) "unsigned short"
  };
  std::vector<std::string> numericTypes(numericTypesC, numericTypesC + (sizeof(numericTypesC) / sizeof(numericTypesC[0])));
  return std::find(numericTypes.begin(), numericTypes.end(), xdataType) != numericTypes.end();
}


void ConfigEditor::outputTableEditControls(
    xgi::Output * out,
    const std::string &tableName,
    const std::string &prefix)
{
  xdata::Table &definition = tableDefinitions[tableName];
  std::vector<std::string> columns = definition.getColumns();
  std::vector<std::string>::iterator column;

  bool fieldsToIncrement = false;
  std::ostringstream increment;

  bool fieldsToSet = false;
  std::ostringstream set;

  bool fieldsToView = false;
  std::ostringstream view;

  std::string anchor = tableName + prefix + "edit";
  increment << cgicc::form().set("method", "POST").set("action",
      toolbox::toString("/%s/incrementValue#%s", getApplicationDescriptor()->getURN().c_str(), anchor.c_str()))
      << std::endl;
  increment << cgicc::input().set("type", "hidden").set("name", "table").set("value", tableName);
  increment << cgicc::input().set("type", "hidden").set("name", "prefix").set("value", prefix);

  increment << "increment all " << cgicc::select().set("name", "fieldName");
  //<< cgicc::input().set("type","text").set("name","view").set("value",view) << std::endl;
  for (column = columns.begin(); column != columns.end(); column++)
  {
    if (canChangeColumnGlobally(*column, tableName))
    {
      if (isNumericType(definition.getColumnType(*column)))
      {
        increment << cgicc::option().set("value", *column) << *column << cgicc::option() << std::endl;
        fieldsToIncrement = true;
      }
    }
  }
  increment << cgicc::select() << " fields by " << cgicc::input().set("type", "text").set("name", "amount");
  increment << cgicc::input().set("type", "submit").set("value", "Increment") << std::endl;
  increment << cgicc::form() << cgicc::br() << std::endl;

  //set controls
  set << cgicc::form().set("method", "POST").set("action",
      toolbox::toString("/%s/setValue#%s", getApplicationDescriptor()->getURN().c_str(), anchor.c_str()))
      << std::endl;
  set << cgicc::input().set("type", "hidden").set("name", "table").set("value", tableName);
  set << cgicc::input().set("type", "hidden").set("name", "prefix").set("value", prefix);
  set << "set all " << cgicc::select().set("name", "fieldName");
  for (column = columns.begin(); column != columns.end(); column++)
  {
    if (canChangeColumnGlobally(*column, tableName))
    {
      set << cgicc::option().set("value", *column) << *column << cgicc::option() << std::endl;
      fieldsToSet = true;
    }
  }
  set << cgicc::select() << " to " << cgicc::input().set("type", "text").set("name", "newValue");
  set << cgicc::input().set("type", "submit").set("value", "Set") << std::endl;
  set << cgicc::form() << std::endl;

  //view all
  view << cgicc::form().set("method", "POST").set("action",
      toolbox::toString("/%s/viewValues", getApplicationDescriptor()->getURN().c_str()))
      << std::endl;
  view << cgicc::input().set("type", "hidden").set("name", "table").set("value", tableName);
  view << cgicc::input().set("type", "hidden").set("name", "prefix").set("value", prefix);
  view << "view all " << cgicc::select().set("name", "fieldName");
  for (column = columns.begin(); column != columns.end(); column++)
  {
    if (!columnIsDatabaseOnly(*column, tableName))
    { //we want to be able to show all even when we can't change all
      view << cgicc::option().set("value", *column) << *column << cgicc::option() << std::endl;
      fieldsToView = true;
    }
  }
  view << cgicc::select();
  view << cgicc::input().set("type", "submit").set("value", "View") << std::endl;
  view << cgicc::form() << cgicc::br() << std::endl;

  if (fieldsToIncrement || fieldsToSet /*|| fieldsToView*/)
  {
    //the only one with fieldsToView but no fieldsToSet is peripheralcrate,
    //and it doesn't work for that anyway since I don't cache the values
    //I probably should, it would further decouple a few things from the device classes,
    //although it's not really necessary since they can't be changed in memory.
    //in any case, I doubt anybody wants to view all crate labels, so it should be okay to have it like this for now.
    //*out << "table " << tableName << cgicc::br();
    *out << cgicc::a().set("name", anchor) << "table " << tableName;
    /*if (!prefix.empty()) {
     std::string key=fullTableID(tableName,prefix);
     //do not show the 'change all' controls of a given table without also showing the data which would be changing.
     tablesToDisplay[key]=true;
     }*/
    if (fieldsToView) *out << view.str();
    if (fieldsToIncrement) *out << increment.str();
    if (fieldsToSet) *out << set.str();
    *out << cgicc::a();
  }
}


std::string ConfigEditor::valueToString(xdata::Serializable *value, const std::string &columnName)
{
  if (shouldDisplayInHex(columnName))
  {
    return xdataToHex(value);
  }
  if (displayBooleansAsIntegers_)
  {
    xdata::Boolean *boolValue = dynamic_cast<xdata::Boolean *> (value);
    if (boolValue) return (bool) *boolValue ? "1" : "0";
  }
  return value->toString();
}


//outputs the value of \a value to *out. If the last three parameters are filled in
//(to indicate where the value comes from, so how to change it) then
//and the column is of a type that can be edited and a column name which does not seem to be a config ID,
//it will be in an editable text field with a change button which called changeSingleValue.
void ConfigEditor::outputSingleValue(
    std::ostream * out,
    xdata::Serializable *value,
    const std::string &column,
    const std::string &tableName,
    const std::string &identifier,
    int rowIndex)
{
  std::string columnType = value->type();
  if (columnType == "table")
  {
    outputTable(out, *static_cast<xdata::Table *> (value));
  }
  else if (columnType == "mime")
  {
    //xdata::Mime *mime=static_cast<xdata::Mime *>(value);
    *out << "mime";
    //output some other data.
  }
  else
  {
    std::string displayValue = valueToString(value, column);
    //display with 0x so that it's clear to the user and also so it can be correctly parsed after they make changes
    if (shouldDisplayInHex(column)) displayValue = "0x" + displayValue;
    if (canChangeColumn(column, tableName) && !tableName.empty() && !identifier.empty())
    {
      std::string anchor = tableName + identifier + column + to_string(rowIndex);
      //add an anchor so we can scroll immediately to the thing we just hid or showed
      *out << cgicc::a().set("name", anchor);
      /*This must be a POST form, since with a GET, even though a + is correctly escaped to %2B in the URL,
       when we read it back from the Cgicc FormElement it is double-unencoded to space,
       and there doesn't seem to be a way of telling the difference between a space that was encoded as a + and a + that was encoded as %2B.
       So we will just ensure we never use identifiers with + in.*/
      *out << cgicc::form().set("method", "POST").set("action", toolbox::toString("/%s/changeSingleValue#%s",
          getApplicationDescriptor()->getURN().c_str(), anchor.c_str())) << std::endl;
      *out << cgicc::input().set("type", "hidden").set("name", "table").set("value", tableName);
      *out << cgicc::input().set("type", "hidden").set("name", "identifier").set("value", identifier);
      *out << cgicc::input().set("type", "hidden").set("name", "fieldName").set("value", column);
      *out << cgicc::input().set("type", "hidden").set("name", "row").set("value", to_string(rowIndex));
      *out << cgicc::input().set("type", "text").set("name", "newValue").set("size", "11").set("value", displayValue);

      *out << cgicc::input().set("type", "submit").set("value", "Change") << std::endl;
      *out << cgicc::form() << std::endl;
      *out << cgicc::a() << std::endl;
    }
    else
    {
      *out << displayValue;
    }
  }
}


std::string ConfigEditor::withoutVersionNumber(const std::string &columnName)
{
  size_t space = columnName.find(' ');
  if (space != std::string::npos) return columnName.substr(0, space);
  return columnName;
}


bool ConfigEditor::getVersionNumber(const std::string &columnName, std::string &versionNumber)
{
  size_t space = columnName.find(' ');
  if (space != std::string::npos)
  {
    versionNumber = columnName.substr(space);
    return true;
  }
  versionNumber = "";
  return false;
}


//returns the HTML to open a new table cell, with the appearance depending on whether the two values are the same.
//I guess this would ne nicer using CSS styles, but this is easiest for now
std::string ConfigEditor::newCell(xdata::Serializable *newValue, xdata::Serializable *oldValue)
{
  if (!oldValue->equals(*newValue)) return "<td bgcolor=\"#CCCCFF\">";
  return "<td>";
}


bool ConfigEditor::getNextColumn(
    std::vector<std::string>::iterator &nextColumn,
    std::string &columnWithoutVersionNumber,
    const std::vector<std::string>::iterator &currentColumn,
    const std::string &tableName,
    const std::vector<std::string>::iterator &end)
{
  columnWithoutVersionNumber = withoutVersionNumber(*currentColumn);
  if (canChangeColumn(columnWithoutVersionNumber, tableName))
  {
    if (currentColumn != end)
    {
      nextColumn = currentColumn;
      ++nextColumn;
      std::string newColumnWithoutVersionNumber = withoutVersionNumber(*nextColumn);
      if (columnWithoutVersionNumber == newColumnWithoutVersionNumber)
      {
        return true;
      }
    }
  }
  return false;
}


void ConfigEditor::outputDiffRow(
    std::ostream * out,
    xdata::Table &results,
    int rowIndex,
    bool vertical,
    const std::string &tableName,
    TableChangeSummary &changes)
{
  std::vector<std::string> columns = results.getColumns();
  std::sort(columns.begin(), columns.end());
  std::vector<std::string>::iterator column;
  std::vector<std::string>::iterator nextColumn;
  for (column = columns.begin(); column != columns.end(); ++column)
  {
    std::string columnWithoutVersionNumber;
    if (getNextColumn(nextColumn, columnWithoutVersionNumber, column, tableName, columns.end()))
    {
      if (vertical) *out << "<tr><td>" << columnWithoutVersionNumber << "</td>";
      xdata::Serializable *oldValue = results.getValueAt(rowIndex, *column);
      xdata::Serializable *newValue = results.getValueAt(rowIndex, *nextColumn);
      std::string tableCellTag = newCell(oldValue, newValue);
      if (!oldValue->equals(*newValue)) changes[columnWithoutVersionNumber]++;
      *out << tableCellTag;
      outputSingleValue(out, oldValue, tableName, columnWithoutVersionNumber);
      *out << "</td>";
      *out << tableCellTag;
      outputSingleValue(out, newValue, tableName, columnWithoutVersionNumber);
      column = nextColumn;
      *out << "</td>";
      if (vertical) *out << "</tr>";
    }
    else if (!columnIsDatabaseOnly(*column, tableName))
    {
      //if there is no matching column, then it is probably something that can't be changed, which was only selected
      //so we could tell which row is which.
      //std::cout << "column without new version: " << *column << std::endl;
      xdata::Serializable *value = results.getValueAt(rowIndex, *column);
      if (vertical)
      {
        *out << "<tr><td>" << columnWithoutVersionNumber << "</td><td colspan=\"2\">";
        outputSingleValue(out, value, tableName, columnWithoutVersionNumber);
        *out << "</td></tr>";
      }
      else
      {
        *out << "<td>";
        outputSingleValue(out, value, tableName, columnWithoutVersionNumber);
        *out << "</td>";
      }
    }
  }
}


void ConfigEditor::outputDiffSummary(std::ostream *out, TableChangeSummary &changes)
{
  for (TableChangeSummary::iterator column = changes.begin(); column != changes.end(); ++column)
  {
    *out << (*column).second << " change" << (((*column).second > 1) ? "s" : "")
        << " to " << (*column).first << "<br/>";
  }
}


void ConfigEditor::outputDiffSummary(std::ostream *out, ChangeSummary &changes)
{
  bool changesFound = false;
  for (ChangeSummary::iterator table = changes.begin(); table != changes.end(); ++table)
  {
    if (!(*table).second.empty())
    {
      *out << cgicc::dt() << "table " << (*table).first << cgicc::dt() << cgicc::dd();
      outputDiffSummary(out, (*table).second);
      *out << cgicc::dd();
      changesFound = true;
    }
  }
  if (!changesFound) *out << "no changes";
}


void ConfigEditor::outputDiff(
    std::ostream * out,
    xdata::Table &results,
    const std::string &tableName,
    TableChangeSummary &changes)
{
  //this uses raw HTML tags because cgicc can't handle any nested tags, which makes it pretty much useless
  std::vector<std::string> columns = results.getColumns();
  int rowCount = results.getRowCount();
  std::vector<std::string>::iterator column;
  std::sort(columns.begin(), columns.end());

  std::string oldConfigID;
  std::string newConfigID;
  //find the first two adjacent column names which have config IDs
  for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
  {
    if (getVersionNumber(*column, oldConfigID) && getVersionNumber(*++column, newConfigID)) break;
  }
  if (columns.size() < 2 || oldConfigID.empty() || newConfigID.empty())
  {
    XCEPT_RAISE(xcept::Exception,"Columns returned do not seem to be a valid diff.");
  }

  std::vector<std::string>::iterator nextColumn;
  std::string columnWithoutVersionNumber;
  if (rowCount == 1)
  {
    *out << "<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
    *out << "<tr><td></td><td> configuration " << oldConfigID << "</td><td> configuration " << newConfigID << "</td>";
    outputDiffRow(out, results, 0, true, tableName, changes);
    *out << "</table>";
  }
  else
  {
    *out << rowCount << " rows";

    *out << "<table border=\"2\" cellpadding=\"2\">";
    *out << "<tr>";
    for (column = columns.begin(); column != columns.end(); ++column)
    {
      if (!columnIsDatabaseOnly(withoutVersionNumber(*column), tableName))
      {
        *out << "<td>" << *column << "</td>";
      }
      /*this version only shows the matched columns, but in fact we also want to see any columns
       which have no version number because they are identifiers that are the same for both versions
       if (getNextColumn(nextColumn,columnWithoutVersionNumber,column,columns.end())) {
       *out << "<td>" << *column << "</td><td>" << *nextColumn << "</td>";
       column=nextColumn;
       }*/
    }
    *out << "</tr>";
    unsigned long rowIndex;
    for (rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
    {
      *out << "<tr>";
      outputDiffRow(out, results, rowIndex, false, tableName, changes);
      *out << "</tr>";
    }
    *out << "</table>";
  }
}


void ConfigEditor::outputTable(
    std::ostream * out,
    xdata::Table &results,
    const std::string &tableName,
    const std::string &identifier)
{
  //this uses raw HTML tags because cgicc can't handle any nested tags, which makes it pretty much useless
  std::vector<std::string> columns = results.getColumns();
  std::vector<std::string>::iterator columnIterator;
  int rowCount = results.getRowCount();
  if (rowCount == 1)
  {
    *out << "<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\">";
    for (columnIterator = columns.begin(); columnIterator != columns.end(); ++columnIterator)
    {
      std::string stringValue = results.getValueAt(0, *columnIterator)->toString();
      if (canChangeColumn(*columnIterator, tableName) && stringValue != "NaN")
      {
        *out << "<tr><td>" << *columnIterator << /*(" << columnType << ")" << */":</td><td>";
        outputSingleValue(out, results.getValueAt(0, *columnIterator), *columnIterator, tableName, identifier, 0);
        *out << "</tr>";
      }
    }
    *out << "</table>";
  }
  else
  {
    *out << rowCount << " rows";

    *out << "<table border=\"2\" cellpadding=\"2\">";
    *out << "<tr>";
    for (columnIterator = columns.begin(); columnIterator != columns.end(); ++columnIterator)
    {
      *out << "<td>" << *columnIterator << " (" << results.getColumnType(*columnIterator) << ")" << "</td>";
    }
    *out << "</tr>";
    unsigned long rowIndex;
    for (rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
    {
      *out << "<tr>";
      for (columnIterator = columns.begin(); columnIterator != columns.end(); columnIterator++)
      {
        *out << "<td>";
        outputSingleValue(out, results.getValueAt(rowIndex, *columnIterator),
            *columnIterator, tableName, identifier, rowIndex);
        *out << "</td>";
      }
      *out << "</tr>";
    }
    *out << "</table>";
  }
}


void ConfigEditor::getTableDefinitionsIfNecessary() throw ()
{
  try
  {
    if (tableDefinitions.size() == 0)
    {
      std::string connectionID = connect();
      // get table definitions (they are used for the web interface as well as for uploading to the db, so do it here.)
      getTableDefinitions(connectionID);
      disconnect(connectionID);
    }
  }
  catch (xcept::Exception &e)
  {
    LOG4CPLUS_WARN(this->getApplicationLogger(),xcept::stdformat_exception_history(e));
  }
}


void ConfigEditor::outputEndcapSelector(xgi::Output * out)
{
  *out << cgicc::select().set("name", "side");
  *out << cgicc::option().set("value", "minus") << "minus" << cgicc::option() << std::endl;
  *out << cgicc::option().set("value", "plus") << "plus" << cgicc::option() << std::endl;
  *out << cgicc::select();
}


void ConfigEditor::showHideTable(xgi::Input * in, xgi::Output * out, bool show) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string tableName = **cgi["table"];
  tablesToDisplay[tableName] = show;
  std::string display = **cgi["display"]; //"diff" if we want to display the current diff, "config" otherwise
  outputHeader(out);
  outputStandardInterface(out);
  if (display == "diff")
    outputCurrentDiff(out);
  else
    outputCurrentConfiguration(out);
}


void ConfigEditor::showTable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  showHideTable(in, out, true);
}


void ConfigEditor::hideTable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  showHideTable(in, out, false);
}


void ConfigEditor::compareVersions(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  clearCachedDiff();
  try
  {
    cgicc::Cgicc cgi(in);
    std::string old_emu_config_id = **cgi["oldConfigID"];
    std::string new_emu_config_id = **cgi["newConfigID"];
    std::string connectionID = connect();
    //here should call a virtual method implemented by subclasses
    diffCrate(connectionID, old_emu_config_id, new_emu_config_id);
    disconnect(connectionID);
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not diff",e);
  }
  outputHeader(out);
  outputStandardInterface(out);
  outputCurrentDiff(out);
}


void ConfigEditor::viewValues(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string tableName = **cgi["table"];
  std::string fieldName = **cgi["fieldName"];
  outputHeader(out);
  outputStandardInterface(out);
  if (currentTables.count(tableName))
  {
    std::map<std::string, xdata::Table> &tables = currentTables[tableName];
    std::map<std::string, xdata::Table>::iterator table;
    std::map<std::string, xdata::Table>::iterator firstTable;
    std::map<std::string, xdata::Table>::iterator lastTable;
    getRangeOfTables(cgi, tables, firstTable, lastTable);
    xdata::Table allTables;
    std::map<std::string, unsigned int> differentValues;
    for (table = firstTable; table != lastTable; ++table)
    {
      try
      {
        xdata::Table relevantColumns = (*table).second;
        std::vector<std::string> columns = (*table).second.getColumns();
        for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
        {
          if (*column != fieldName)
          {
            if (*column != "LABEL" && *column != "CFEB_NUMBER" && *column != "AFEB_NUMBER")
            {
              relevantColumns.removeColumn(*column);
            }
          }
        }
        std::string contextColumnName = "Crate/chamber";
        relevantColumns.addColumn(contextColumnName, "string");
        int rowCount = (*table).second.getRowCount();
        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
          xdata::String context = (*table).first;
          relevantColumns.setValueAt(rowIndex, contextColumnName, context);
          xdata::Serializable *value = relevantColumns.getValueAt(rowIndex, fieldName);
          differentValues[value->toString()]++;
        }
        if (allTables.getRowCount() == 0) allTables = relevantColumns;
        else
          allTables.merge(&allTables, &relevantColumns);
      }
      catch (xdata::exception::Exception &e)
      {
        XCEPT_RETHROW(xgi::exception::Exception,"Could not get all values",e);
      }
    }
    std::string qualifier = "";
    if (!cgi("prefix").empty())
    {
      qualifier = " of " + cgi("prefix");
    }
    if (differentValues.size() == 1)
    {
      std::map<std::string, unsigned int>::iterator onlyValue = differentValues.begin();
      *out << "All " << fieldName << " values" << qualifier << " are " << (*onlyValue).first;
    }
    else
    {
      if (differentValues.size() < allTables.getRowCount() / 2)
      { //if there isn't a different value for almost every row, show the counts of different values
        std::multimap<unsigned int, std::string> valuesByCount;
        //insert the values into this with the key and value swapped,
        // so that we can have them sorted by the number of times each value is used
        for (std::map<std::string, unsigned int>::iterator value = differentValues.begin();
            value != differentValues.end(); ++value)
        {
          valuesByCount.insert(std::pair<unsigned int, std::string>((*value).second, (*value).first));
        }
        for (std::multimap<unsigned int, std::string>::reverse_iterator count = valuesByCount.rbegin();
            count != valuesByCount.rend(); ++count)
        {
          *out << (*count).first << " " << fieldName << " values" << qualifier << " are "
              << (*count).second << cgicc::br();
        }
      }
      outputTable(out, allTables);
    }
  }
  outputFooter(out);
}


// \return  a string containing all the <option> tags for a menu containing \a configIDs
std::string ConfigEditor::configIDOptions(std::vector<std::string> &configIDs)
{
  // sort in reverse order, so the newest are on the top
  std::sort(configIDs.begin(), configIDs.end());
  std::reverse(configIDs.begin(), configIDs.end());

  std::ostringstream menu;
  //for (std::vector<std::string>::reverse_iterator configID = configIDs.rbegin(); configID != configIDs.rend(); ++configID)
  for (std::vector<std::string>::const_iterator configID = configIDs.begin(); configID != configIDs.end(); ++configID)
  {
    menu << cgicc::option().set("value", *configID) << *configID << cgicc::option() << std::endl;
  }
  return menu.str();
}


void ConfigEditor::outputCompareVersionsForm(xgi::Output * out, const std::string &endcap_side)
{
  try
  {
    std::vector<std::string> configIDs;
    getConfigIds(configIDs, endcap_side);
    *out << cgicc::form().set("method", "GET").set("action",
        toolbox::toString("/%s/compareVersions", getApplicationDescriptor()->getURN().c_str())) << std::endl;
    std::string menu = configIDOptions(configIDs);
    *out << "compare version " << cgicc::select().set("name", "oldConfigID") << menu << cgicc::select();
    *out << "with version " << cgicc::select().set("name", "newConfigID") << menu << cgicc::select();
    *out << cgicc::input().set("type", "submit").set("value", "Compare") << std::endl;
    *out << cgicc::form();
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not fetch available config IDs to compare. ",e);
  }
}


void ConfigEditor::selectVersions(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string endcap_side = **cgi["side"];
  outputHeader(out);
  outputStandardInterface(out);
  outputCompareVersionsForm(out, endcap_side);
  outputFooter(out);
}


void ConfigEditor::selectVersion(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    cgicc::Cgicc cgi(in);
    std::string endcap_side = **cgi["side"];
    std::vector<std::string> configIDs;
    getConfigIds(configIDs, endcap_side);
    outputHeader(out);
    outputStandardInterface(out);

    *out << cgicc::form().set("method", "GET").set("action",
        toolbox::toString("/%s/read", getApplicationDescriptor()->getURN().c_str())) << std::endl;
    *out << "read version " << cgicc::select().set("name", "configID")
        << configIDOptions(configIDs) << cgicc::select();
    *out << cgicc::input().set("type", "submit").set("value", "Read") << std::endl;
    *out << cgicc::form();
    outputFooter(out);
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not fetch available config IDs to load. ",e);
  }
}


template <class xdataType>
void ConfigEditor::set(xdata::Serializable *originalValue, const std::string &newValue)
{
  xdataType *original = dynamic_cast<xdataType *> (originalValue);
  if (original)
  {
    original->fromString(newValue);
  }
}


template <class xdataType, typename simpleType>
void ConfigEditor::add(xdata::Serializable *originalValue, const std::string &addend)
{
  xdataType *original = dynamic_cast<xdataType *> (originalValue);
  if (original)
  {
    xdataType amountToAdd;
    amountToAdd.fromString(addend);
    //std::cout << "addend=" << addend << " amountToAdd=" << amountToAdd << " simply " << (simpleType)amountToAdd << std::endl;
    //std::cout << "original=" << *original << " simply " << (simpleType)*original << std::endl;
    simpleType sum = (simpleType) *original + (simpleType) amountToAdd;
    //std::cout << "sum=" << sum << std::endl;
    //*originalValue=(xdataType)sum;
    *original = sum;
    //std::cout << "new value=" << originalValue->toString() << std::endl;
  }
}


void ConfigEditor::setValueFromString(xdata::Serializable *value, const std::string &newValue)
    throw (xdata::exception::Exception)
{
  std::string columnType = value->type();
  /*
   Aarrghh, this doesn't work because we need to know the template type for the SimpleType.
   So much for polymorphism.
   xdata::SimpleType *value;
   value=dynamic_cast<xdata::SimpleType *>((*table).second.getValueAt(rowIndex,fieldName));
   if (value) value->fromString(newValue);*/
  if (columnType == "int")
  {
    set<xdata::Integer> (value, newValue);
  }
  else if (columnType == "unsigned long")
  {
    set<xdata::UnsignedLong> (value, newValue);
  }
  else if (columnType == "float")
  {
    set<xdata::Float> (value, newValue);
  }
  else if (columnType == "double")
  {
    set<xdata::Double> (value, newValue);
  }
  else if (columnType == "unsigned int")
  {
    set<xdata::UnsignedInteger> (value, newValue);
  }
  else if (columnType == "unsigned int 32")
  {
    set<xdata::UnsignedInteger32> (value, newValue);
  }
  else if (columnType == "unsigned int 64")
  {
    set<xdata::UnsignedInteger64> (value, newValue);
  }
  else if (columnType == "unsigned short")
  {
    set<xdata::UnsignedShort> (value, newValue);
  }
  else if (columnType == "string")
  {
    set<xdata::String> (value, newValue);
  }
  else if (columnType == "time")
  {
    set<xdata::TimeVal> (value, newValue);
  }
  else if (columnType == "bool")
  {
    //set only works if the value is "true" or "false", but some XML files use 1 and 0 instead, so check for them first
    if (newValue == "1") value->setValue(xdata::Boolean(true));
    else if (newValue == "0") value->setValue(xdata::Boolean(false));
    else
      set<xdata::Boolean> (value, newValue);
  }
}


std::string ConfigEditor::copyAttributesToTable(
    xdata::Table &table,
    const std::string &tableName,
    xercesc::DOMElement *node,
    int rowIndex) throw (xcept::Exception)
{
  std::string identifier;
  if (node)
  {
    xercesc::DOMNamedNodeMap *pAttributes = node->getAttributes();
    unsigned int attributeCount = pAttributes->getLength();
    std::cout << attributeCount << " attributes" << std::endl;
    table.append(); //I'm sure I looked for this method before but I couldn't find it
    for (unsigned int attributeIndex = 0; attributeIndex <= attributeCount; attributeIndex++)
    {
      DOMAttr *attribute = dynamic_cast<DOMAttr *> (pAttributes->item(attributeIndex));
      if (attribute)
      {
        std::string columnName = toolbox::toupper(xoap::XMLCh2String(attribute->getName()));
        std::string stringValue = xoap::XMLCh2String(attribute->getValue());
        if (tableHasColumn(table, columnName))
        {
          std::cout << "setting " << columnName << " to " << stringValue << std::endl;
          xdata::Serializable *value = table.getValueAt(rowIndex, columnName);
          setValueFromString(value, stringValue);
          size_t numberPosition = columnName.find("_NUMBER");
          if (numberPosition != std::string::npos)
          {
            identifier = columnName.substr(0, numberPosition) + " " + stringValue;
            std::cout << "identifier for column " + columnName + " is " + identifier << std::endl;
          }
          else if (columnIsUniqueIdentifier(columnName, tableName) && identifier.empty())
          {
            identifier = columnName + " " + stringValue;
          }
        }
        else
        {
          XCEPT_RAISE(xcept::Exception,"Attribute "+columnName+" has no corresponding column in the database");
        }
      }
    }
  }
  return identifier;
}


std::string ConfigEditor::uniqueIdentifierForRow(
    xdata::Table &table,
    const std::string &tableName,
    unsigned int rowIndex)
{
  std::vector<std::string> columns = table.getColumns();
  std::string identifier;
  for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
  {
    xdata::Serializable *value = table.getValueAt(rowIndex, *column);
    size_t numberPosition = (*column).find("_NUMBER");
    if (numberPosition != std::string::npos)
    {
      identifier = (*column).substr(0, numberPosition) + " " + value->toString();
      std::cout << "identifier for column " + *column + " is " + identifier << std::endl;
    }
    else if (columnIsUniqueIdentifier(*column, tableName) && identifier.empty())
    {
      identifier = *column + " " + value->toString();
    }
  }
  return identifier;
}


void ConfigEditor::setValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string tableName = **cgi["table"];
  std::string fieldName = **cgi["fieldName"];
  std::string newValue = **cgi["newValue"];
  LOG4CPLUS_DEBUG(this->getApplicationLogger(),"there are "+to_string(currentTables.count(tableName))+ " tables with the name "+tableName);

  if (currentTables.count(tableName))
  {
    std::map<std::string, xdata::Table> &tables = currentTables[tableName];
    std::map<std::string, xdata::Table>::iterator table;
    std::map<std::string, xdata::Table>::iterator firstTable;
    std::map<std::string, xdata::Table>::iterator lastTable;
    getRangeOfTables(cgi, tables, firstTable, lastTable);

    for (table = firstTable; table != lastTable; ++table)
    {
      std::string columnType = (*table).second.getColumnType(fieldName);
      try
      {
        int rowCount = (*table).second.getRowCount();
        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
          xdata::Serializable *value = ((*table).second.getValueAt(rowIndex, fieldName));
          setValueFromString(value, newValue);
        }
      }
      catch (xdata::exception::Exception &e)
      {
        XCEPT_RETHROW(xgi::exception::Exception,"Value "+newValue+" could not be converted to the data type "+columnType,e);
      }
    }
  }
  outputHeader(out);
  outputStandardInterface(out);
  outputCurrentConfiguration(out);
}


void ConfigEditor::changeSingleValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string tableName = **cgi["table"];
  std::string identifier = **cgi["identifier"];
  std::string fieldName = **cgi["fieldName"];
  std::string rowString = **cgi["row"];
  std::string newValue = **cgi["newValue"];
  if (currentTables.count(tableName))
  {
    std::map<std::string, xdata::Table> &tables = currentTables[tableName];
    if (tables.count(identifier))
    {
      xdata::Table &table = tables[identifier];
      unsigned int rowIndex;
      std::istringstream myStream(rowString);
      if ((myStream >> rowIndex) && table.getRowCount() >= rowIndex)
      {
        if (tableHasColumn(table, fieldName) && canChangeColumn(fieldName, tableName))
        {
          xdata::Serializable *value = table.getValueAt(rowIndex, fieldName);
          std::cout << "changing value from " << value->toString() << " to " << newValue << std::endl;
          setValueFromString(value, newValue);
        }
        else
        {
          XCEPT_RAISE(xgi::exception::Exception,"Column "+fieldName+" does not exist or is not editable in the specified table");
        }
      }
      else
      {
        XCEPT_RAISE(xgi::exception::Exception,"There is no row number "+rowString+" in the specified table");
      }
    }
    else
    {
      XCEPT_RAISE(xgi::exception::Exception,"No "+tableName+" configuration loaded for "+identifier);
    }
  }
  else
  {
    XCEPT_RAISE(xgi::exception::Exception,"No such table: "+tableName);
  }
  outputHeader(out);
  outputStandardInterface(out);
  outputCurrentConfiguration(out);
}


void ConfigEditor::getRangeOfTables(
    const cgicc::Cgicc &cgi,
    std::map<std::string, xdata::Table> &tables,
    std::map<std::string, xdata::Table>::iterator &firstTable,
    std::map<std::string, xdata::Table>::iterator &lastTable)
{
  std::string prefix = cgi("prefix");
  getRangeOfTables(prefix, tables, firstTable, lastTable);
}


void ConfigEditor::getRangeOfTables(
    const std::string &prefix,
    std::map<std::string, xdata::Table> &tables,
    std::map<std::string, xdata::Table>::iterator &firstTable,
    std::map<std::string, xdata::Table>::iterator &lastTable)
{
  if (prefix.empty())
  {
    firstTable = tables.begin();
    lastTable = tables.end();
  }
  else
  {
    firstTable = tables.lower_bound(prefix);
    lastTable = tables.upper_bound(prefix + "~"); //~ is sorted last, so this should find the first key with a different prefix
  }
}


bool ConfigEditor::tableHasColumn(xdata::Table &table, const std::string &column)
{
  std::vector<std::string> columns = table.getColumns();
  return std::find(columns.begin(), columns.end(), column) != columns.end();
}


void ConfigEditor::incrementValue(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  std::string tableName = **cgi["table"];
  std::string fieldName = **cgi["fieldName"];
  std::string amountToAdd = **cgi["amount"];
  try
  {
    LOG4CPLUS_DEBUG(this->getApplicationLogger(),"there are "+to_string(currentTables.count(tableName))+
        " tables with the name "+tableName);

    if (currentTables.count(tableName))
    {
      std::map<std::string, xdata::Table> &tables = currentTables[tableName];
      std::map<std::string, xdata::Table>::iterator table;
      std::map<std::string, xdata::Table>::iterator firstTable;
      std::map<std::string, xdata::Table>::iterator lastTable;
      getRangeOfTables(cgi, tables, firstTable, lastTable);

      for (table = firstTable; table != lastTable; ++table)
      {
        if (tableHasColumn((*table).second, fieldName))
        {
          std::string columnType = (*table).second.getColumnType(fieldName);
          try
          {
            if (isNumericType(columnType))
            {
              int rowCount = (*table).second.getRowCount();
              for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
              {
                xdata::Serializable *value = (*table).second.getValueAt(rowIndex, fieldName);
                LOG4CPLUS_DEBUG(this->getApplicationLogger(),"adding "+amountToAdd+ " to "+columnType+" "+value->toString());
                if (columnType == "int")
                {
                  add<xdata::Integer, xdata::IntegerT> (value, amountToAdd);
                }
                else if (columnType == "unsigned long")
                {
                  add<xdata::UnsignedLong, xdata::UnsignedLongT> (value, amountToAdd);
                }
                else if (columnType == "float")
                {
                  add<xdata::Float, xdata::FloatT> (value, amountToAdd);
                }
                else if (columnType == "double")
                {
                  add<xdata::Double, xdata::DoubleT> (value, amountToAdd);
                }
                else if (columnType == "unsigned int")
                {
                  add<xdata::UnsignedInteger, xdata::UnsignedIntegerT> (value, amountToAdd);
                }
                else if (columnType == "unsigned int 32")
                {
                  add<xdata::UnsignedInteger32, xdata::UnsignedInteger32T> (value, amountToAdd);
                }
                else if (columnType == "unsigned int 64")
                {
                  add<xdata::UnsignedInteger64, xdata::UnsignedInteger64T> (value, amountToAdd);
                }
                else if (columnType == "unsigned short")
                {
                  add<xdata::UnsignedShort, xdata::UnsignedShortT> (value, amountToAdd);
                }
                LOG4CPLUS_DEBUG(this->getApplicationLogger(),"result is "+value->toString());
                (*table).second.setValueAt(rowIndex, fieldName, *value);
              }
            }
          }
          catch (xdata::exception::Exception &e)
          {
            XCEPT_RETHROW(xgi::exception::Exception,"Value "+amountToAdd+" could not be converted to the data type "+columnType,e);
          }
        }
      }
    }
    outputHeader(out);
    outputStandardInterface(out);
    outputCurrentConfiguration(out);
  }
  catch (xdata::exception::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not increment "+fieldName+" by "+amountToAdd,e);
  }
}


//to be implemented by subclasses
void ConfigEditor::parseConfigFromXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  //actually, I should write a generic version of this
  //indeed! In progress. Stand by.
  //ConfigEditor::getTableDefinitionsIfNecessary();
  //for each node of type topLevelTableName_
    //add a row to that table
    //for each table in topLevelTables
      //find all nodes with that name. For each one
      //get child nodes
}


void ConfigEditor::uploadConfigToDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    outputHeader(out);
    outputStandardInterface(out);

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    *out << "<br>Upload started at: " << asctime(timeinfo) << std::endl;
    *out << "<br>Uploading to Database is in progress. This may take over a minute. Please be patient!<br><br>"
        << std::endl;

    startUpload(in);

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    *out << "<br>Uploading finished at: " << asctime(timeinfo) << std::endl;
    *out << "<br>EMU_Config_ID " << emu_config_id_ << " uploaded to Database." << std::endl;

    outputCurrentConfiguration(out);
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
}


//to be implemented by subclasses
void ConfigEditor::readConfigFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    cgicc::Cgicc cgi(in);
    outputHeader(out);
    outputStandardInterface(out);
    std::string endcap_side = cgi("side");
    std::string emu_config_id = cgi("configID");
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
}


void ConfigEditor::setViewID(const std::string &viewID)
{
  viewID_ = viewID;
}


void ConfigEditor::setHumanReadableConfigName(const std::string &configName)
{
  configName_ = configName;
}


void ConfigEditor::setTopLevelTableName(const std::string &tableName)
{
  topLevelTableName_ = tableName;
}


void ConfigEditor::setDisplayBooleansAsIntegers(bool displayBooleansAsIntegers)
{
  displayBooleansAsIntegers_ = displayBooleansAsIntegers;
}


void ConfigEditor::setXMLRootElement(const std::string &rootElementName)
{
  rootElementName_ = rootElementName;
}


void ConfigEditor::setConfigurationDirectory(const std::string &configurationDirectory)
{
  configurationDirectory_ = configurationDirectory;
}


std::string ConfigEditor::fullConfigurationDirectory()
{
  std::string HomeDir_ = getenv("HOME");
  size_t pos = HomeDir_.find_last_of("/");
  if (pos != HomeDir_.length() - 1) HomeDir_ += "/";
  return HomeDir_ + "config/" + configurationDirectory_ += "/";
}


//subclasses should override this to add any attributes to the root element of the XML.
void ConfigEditor::fillRootElement(DOMElement *rootElement) {}


void ConfigEditor::setTableNamePrefix(const std::string &prefix)
{
  syncPattern_ = "^" + prefix + ".*$";
}


//syncPattern should be set by subclasses
void ConfigEditor::synchronizeToFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    outputHeader(out);
    outputStandardInterface(out);

    //*out << "<br>You clicked Synchronize from DB<br>" << std::endl;
    std::string connectionID = connect();
    std::string syncMode = "to database"; // possibilities: "to database" OR "from database" OR "both ways"
    //std::string syncPattern = "^EMU_.*$";

    // LIU 8-22-2008. disable this function. For creating new database tables only
    //*out << "<br>This function is for creating new database tables only." << std::endl;

    synchronize(connectionID, syncMode, syncPattern_);
    *out << "<br>Synchronization to DB has finished.<br>" << std::endl;

    outputCurrentConfiguration(out);
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
}


//gets all config IDs for the given endcap side
void ConfigEditor::getConfigIds(std::vector<std::string> &configIDs, const std::string endcap_side)
    throw (xcept::Exception)
{
  xdata::Table results;
  std::string connectionID = connect();

  std::string queryViewName = "configuration_ids_side";

  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView(viewID_);

  //If we give the name of the view class when constructing the TStoreRequest,
  //it will automatically use that namespace for
  //any view specific parameters we add.
  emu::db::TStoreRequest request("query", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  request.addViewSpecificParameter("name", queryViewName);

  //add parameter name and value (endcap_side)
  request.addViewSpecificParameter("SIDE", endcap_side);

  xoap::MessageReference message = request.toSOAP();
  xoap::MessageReference response = sendSOAPMessage(message);

  //use the TStore client library to extract the first attachment of type "table"
  //from the SOAP response
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE (xcept::Exception, "Server returned no data");
  }

  disconnect(connectionID);
  std::vector<std::string> columns = results.getColumns();
  std::string column = *columns.begin();
  for (unsigned long rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    configIDs.push_back(results.getValueAt(rowIndex, column)->toString());
  }
}


xoap::MessageReference ConfigEditor::sendSOAPMessage(xoap::MessageReference &message) throw (xcept::Exception)
{
  xoap::MessageReference reply;

#ifdef debugV
  std::cout << "Message: " << std::endl;
  message->writeTo(std::cout);
  std::cout << std::endl;
#endif

  try
  {
    xdaq::ApplicationDescriptor * tstoreDescriptor =
        getApplicationContext()->getFirstApplication("tstore::TStore")->getApplicationDescriptor();
        //getApplicationContext()->getDefaultZone()->getApplicationDescriptor("tstore::TStore", 0);
    xdaq::ApplicationDescriptor * tstoretestDescriptor = this->getApplicationDescriptor();
    reply = getApplicationContext()->postSOAP(message, *tstoretestDescriptor, *tstoreDescriptor);
  }
  catch (xdaq::exception::Exception& e)
  {
    LOG4CPLUS_ERROR(this->getApplicationLogger(),xcept::stdformat_exception_history(e));
    XCEPT_RETHROW(xcept::Exception, "Could not post SOAP message. ", e);
  }

  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

#ifdef debugV
  std::cout << std::endl << "Response: " << std::endl;
  reply->writeTo(std::cout);
  std::cout << std::endl;
#endif

  if (body.hasFault())
  {
    //XCEPT_RAISE (xcept::Exception, body.getFault().getFaultString());
    XCEPT_RAISE (xcept::Exception, body.getFault().getDetail().getTextContent());
  }
  return reply;
}


void ConfigEditor::setPassword(const std::string &password)
{
  password_ = password;
  createCredentialString();
}


void ConfigEditor::setUsername(const std::string &username)
{
  username_ = username;
  createCredentialString();
}


void ConfigEditor::createCredentialString()
{
  dbUserAndPassword_ = username_ + "/" + password_;
  std::cout << "credentials are " << dbUserAndPassword_;
}


std::string ConfigEditor::connect() throw (xcept::Exception)
{
  try
  {
    emu::db::TStoreRequest request("connect");

    //add the view ID
    request.addTStoreParameter("id", viewID_);

    //this parameter is mandatory. "basic" is the only value allowed at the moment
    request.addTStoreParameter("authentication", "basic");

    //login credentials in format username/password
    getDbUserData();
    request.addTStoreParameter("credentials", dbUserAndPassword_);

    //connection will time out after 10 minutes
    toolbox::TimeInterval timeout(600, 0);
    request.addTStoreParameter("timeout", timeout.toString("xs:duration"));

    xoap::MessageReference message = request.toSOAP();

    xoap::MessageReference response = sendSOAPMessage(message);

    //use the TStore client library to extract the response from the reply
    return tstoreclient::connectionID(response);
  }
  catch (std::string &s)
  {
    //I don't know where it's coming from, but occasionally something is throwing a string during the execution of this function
    //maybe if we log it we can find out what and why
    XCEPT_RAISE (xcept::Exception, "string thrown: "+s);
  }
}


void ConfigEditor::disconnect(const std::string &connectionID) throw (xcept::Exception)
{
  emu::db::TStoreRequest request("disconnect");
  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);
  xoap::MessageReference message = request.toSOAP();
  sendSOAPMessage(message);
}


/*
void ConfigEditor::query(const std::string &connectionID, const std::string &queryViewName,
    const std::string &emu_config_id, xdata::Table &results) throw (xcept::Exception)
{
  query(connectionID, queryViewName, emu_config_id, "", results);
}
*/


void ConfigEditor::query(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::map<std::string, std::string> &queryParameters,
    xdata::Table &results) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView(viewID_);

  //If we give the name of the view class when constructing the TStoreRequest,
  //it will automatically use that namespace for
  //any view specific parameters we add.
  emu::db::TStoreRequest request("query", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  request.addViewSpecificParameter("name", queryViewName);

  // add parameter names and values
  for (std::map<std::string, std::string>::const_iterator iPair = queryParameters.begin();
      iPair != queryParameters.end(); iPair++)
  {
    request.addViewSpecificParameter(iPair->first, iPair->second);
  }

  xoap::MessageReference message = request.toSOAP();
  xoap::MessageReference response;
  try
  {
    response = sendSOAPMessage(message);
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW(xcept::Exception, "Error sending SOAP message", e);
  }

  //use the TStore client library to extract the first attachment of type "table"
  //from the SOAP response
  //xdata::Table results;
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE (xcept::Exception, "Server returned no data");
  }
  //return results;
}


void ConfigEditor::getDefinition(
    const std::string &connectionID,
    const std::string &insertViewName) throw (xcept::Exception)
{
  //the definition message is essentially the same as a query message.
  //instead of retrieving a table full of results, we retrieve an empty table
  //with the appropriate column names and types.

  //we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView(viewID_);

  emu::db::TStoreRequest request("definition", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a insert section in the configuration
  //since the definition is used for an insert.
  //We'll use the "test" one.
  request.addViewSpecificParameter("name", insertViewName);

  xoap::MessageReference message = request.toSOAP();

  xoap::MessageReference response = sendSOAPMessage(message);

  //use the TStore client library to extract the first attachment of type "table"
  //from the SOAP response
  xdata::Table results;
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE (xcept::Exception, "Server returned no data");
  }
  tableDefinitions[insertViewName] = results;
}


void ConfigEditor::insert(
    const std::string &connectionID,
    const std::string &insertViewName,
    xdata::Table &newRows) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView(viewID_);

  //If we give the name of the view class when constructing the TStoreRequest,
  //it will automatically use that namespace for
  //any view specific parameters we add.
  emu::db::TStoreRequest request("insert", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  //We'll use the "test" one.
  request.addViewSpecificParameter("name", insertViewName);

  xoap::MessageReference message = request.toSOAP();

  //add our new rows as an attachment to the SOAP message
  //the last parameter is the ID of the attachment. The SQLView does not mind what it is, as there should only be one attachment per message.
  tstoreclient::addAttachment(message, newRows, "test");

  xoap::MessageReference response = sendSOAPMessage(message);
  xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault())
  {
    XCEPT_RAISE (xcept::Exception,"An error has occured during inserting data to database!");
  }
}


void ConfigEditor::synchronize(
    const std::string &connectionID,
    const std::string &syncMode,
    const std::string &syncPattern) throw (xcept::Exception)
{
  std::string viewClass = tstoreclient::classNameForView(viewID_);
  emu::db::TStoreRequest request("sync", viewClass);
  request.addTStoreParameter("connectionID", connectionID);
  request.addTStoreParameter("mode", syncMode);
  request.addTStoreParameter("pattern", syncPattern);

  xoap::MessageReference message = request.toSOAP();
  xoap::MessageReference response = sendSOAPMessage(message);
  //	std::cout << "message: " << std::endl;
  //	message->writeTo(std::cout);
  //	std::cout << "response: " << std::endl;
  //	response->writeTo(std::cout);

  xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault())
  {
    XCEPT_RAISE (xcept::Exception,"An error has occured during synchronization!");
  }
}


void ConfigEditor::getTableDefinitions(const std::string &connectionID)
{
  //todo: change this to loop through an array of table names
  for (std::map<std::string, std::vector<std::string> >::iterator tableName = tableNames.begin();
      tableName != tableNames.end(); ++tableName)
  {
    getDefinition(connectionID, (*tableName).first);
  }
  getDefinition(connectionID, topLevelTableName_);
  //done!
  std::cout << "got table definitions" << std::endl;
}

void ConfigEditor::setCachedTable(
    const std::string &insertViewName,
    const std::string &identifier,
    xdata::Table &table) throw (xcept::Exception)
{
  std::ostringstream logMessage;
  logMessage << "setting cached " + insertViewName + " table " + identifier + " with " << table.getRowCount() << " rows";
  LOG4CPLUS_DEBUG(this->getApplicationLogger(),logMessage.str());
  //add space to identifier so that something beginning with e.g. 'crate 11' is not included when looking for 'crate 1 '
  currentTables[insertViewName][identifier + " "] = table;
  LOG4CPLUS_DEBUG(this->getApplicationLogger(),"set cached "+insertViewName+" table "+identifier);
}

void ConfigEditor::setCachedDiff(
    const std::string &insertViewName,
    const std::string &identifier,
    xdata::Table &table) throw (xcept::Exception)
{
  std::ostringstream logMessage;
  logMessage << "setting cached " + insertViewName + " diff " + identifier + " with " << table.getRowCount() << " rows";
  LOG4CPLUS_DEBUG(this->getApplicationLogger(),logMessage.str());
  currentDiff[insertViewName][identifier + " "] = table;
  LOG4CPLUS_DEBUG(this->getApplicationLogger(),"set cached "+insertViewName+" diff "+identifier);
}

xdata::Table &ConfigEditor::getCachedTableFrom(
    std::map<std::string, std::map<std::string, xdata::Table> > &cache,
    const std::string &insertViewName,
    const std::string &identifier
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  if (cache.count(insertViewName))
  {
    std::string realIdentifier = identifier;
    if (cache[insertViewName].count(realIdentifier) == 0) realIdentifier += " "; //setCached... adds a space to the end
    if (cache[insertViewName].count(realIdentifier) == 1)
    {
      LOG4CPLUS_DEBUG(this->getApplicationLogger(),"getting cached "+insertViewName+" table "+identifier);
      xdata::Table &returnValue = cache[insertViewName][realIdentifier];
      LOG4CPLUS_DEBUG(this->getApplicationLogger(),"got cached "+insertViewName+" table "+identifier);
      return returnValue;
    }
  }
  XCEPT_RAISE(xcept::Exception,"No "+insertViewName+" configuration loaded for "+identifier);
}


xdata::Table &ConfigEditor::getCachedTable(
    const std::string &insertViewName,
    const std::string &identifier
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  return getCachedTableFrom(currentTables, insertViewName, identifier);
}


xdata::Table &ConfigEditor::getCachedDiff(
    const std::string &insertViewName,
    const std::string &identifier
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  return getCachedTableFrom(currentDiff, insertViewName, identifier);
}


void ConfigEditor::clearCachedTables()
{
  currentTables.clear();
}


void ConfigEditor::clearCachedDiff()
{
  currentDiff.clear();
}


std::string ConfigEditor::crateIdentifierString(int crateID)
{
  if(crateID>=0 && crateID<=60 && crateRealNames[crateID]!="") return "crate "+crateRealNames[crateID]+" ";
  //add space to the end so that when looping over keys beginning with this crate ID, we don't confuse e.g. 1 with 11
  else return crateIdentifierString(to_string(crateID));
}


std::string ConfigEditor::crateIdentifierString(const std::string &crateID)
{
  //add space to the end so that when looping over keys beginning with this crate ID, we don't confuse e.g. 1 with 11
  return "crate " + crateID + " ";
}


void ConfigEditor::fillCrateRealName(int id, std::string name)
{
  if(id<65) crateRealNames[id]=name;
}

}} // namespaces
