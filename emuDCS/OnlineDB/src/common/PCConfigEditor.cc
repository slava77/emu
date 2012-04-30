// $Id: PCConfigEditor.cc,v 1.5 2012/04/30 23:54:55 khotilov Exp $

#include "emu/db/PCConfigEditor.h"
#include "emu/db/TStoreRequest.h"
#include "emu/pc/XMLParser.h"
#include "emu/db/ConfigIDInfo.h"

#include <time.h>
#include "toolbox/TimeInterval.h"
#include "toolbox/TimeVal.h"
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
#include "xoap/domutils.h"

#include "xercesc/dom/DOMNode.hpp"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/System.h"
#include "emu/utils/SimpleTimer.h"

//#include "emu/db/PeripheralDBID.h"
//#include "emu/db/TStoreFixer.h"
#include "emu/db/TableDefinitions.h"
#include "emu/db/PCConfigHierarchy.h"
#include "emu/db/ConfigTable.h"
#include "emu/db/ConfigRow.h"
#include "emu/db/ConfigTree.h"
#include "emu/db/TStoreReadWriter.h"
#include "emu/pc/EmuEndcapConfigWrapper.h"
#include "emu/db/XMLReadWriter.h"

XDAQ_INSTANTIATOR_IMPL(emu::db::PCConfigEditor)


using namespace emu::utils;
using namespace emu::pc;

namespace emu { namespace db {


PCConfigEditor::PCConfigEditor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception) :
  emu::db::ConfigEditor(s)
{

  xdata::InfoSpace *infoSpace = this->getApplicationInfoSpace();
  infoSpace->fireItemAvailable("tstoreViewXML", &tstoreConf_);

  xgi::bind(this, &PCConfigEditor::SetTypeDesc, "SetTypeDesc");
  xgi::bind(this, &PCConfigEditor::SelectTestSummaryFile, "SelectTestSummaryFile");
  xgi::bind(this, &PCConfigEditor::parseTestSummary, "parseTestSummary");
  xgi::bind(this, &PCConfigEditor::applyTestSummary, "applyTestSummary");

  setTableNamePrefix("EMU_");
  setTopLevelTableName("peripheralcrate");
  setXMLRootElement("EmuSystem");
  //false is the default anyway, but just to make it obvious to anyone looking at this code later
  setDisplayBooleansAsIntegers(false);
  setViewID("urn:tstore-view-SQL:EMUsystem");
  setHumanReadableConfigName("Peripheral Crate");
  setConfigurationDirectory("pc");

  TStore_myEndcap_ = NULL;

  addTable("vcc");
  addTable("csc");
  addChildTable("csc", "tmb");
  addChildTable("csc", "daqmb");
  addTable("ccb");
  addTable("mpc");
  addChildTable("daqmb", "cfeb");
  addChildTable("tmb", "alct");
  addChildTable("alct", "anodechannel");

  getDbUserData();

  my_test();
}


void PCConfigEditor::my_test()
{
  using std::cout;
  using std::endl;
  using std::string;

  /*
  emu::pc::PCConfigurationsDBAgent test(this, "plus");
  test.connect(dbUserAndPassword_);
  std::cout<<"tezzzt "<<test.getConfigId().toString()<<std::endl;
  //test.disconnect();
  std::cout<<"got it!"<<std::endl;
*/

  //std::cout<<"iSpace: "<<iSpace->name()<<std::endl;
  std::cout<<"fname: "<<tstoreConf_.toString()<<std::endl;
  std::string configurationXML = readFile(tstoreConf_.toString());
  //std::string configurationXML = readFile("/home/cscdev/TriDAS/emu/emuDCS/PeripheralApps/xml/EMUsystem.view");
  std::cout<<"content: "<<configurationXML <<std::endl;

  //emu::db::TableDefinitions def("/home/cscdev/TriDAS/emu/emuDCS/PeripheralApps/xml/EMUsystem.view", "EMU_");


//  timestamp_t t0 = get_timestamp();
//  timestamp_t t1;

  emu::db::PCConfigHierarchy h("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view");
  //emu::db::PCConfigHierarchy h("/home/cscdev/TriDAS/emu/emuDCS/OnlineDB/xml/EMUsystem.view", emu::db::ConfigHierarchy::TRUNCATE_2);

  //std::map<std::string, emu::db::ConfigTable> tab = h.definitionsTree();

  //emu::db::TStoreReadWriter tstore_rw(&h, this, dbUserAndPassword_, 0);
  emu::db::TStoreReadWriter tstore_rw(&h, this, "", 0);
  //test.readIDs("minus");

  xdata::UnsignedInteger64 maxid = tstore_rw.readMaxID("minus");
  std::cout<<"The latest minus endcap config id: "<<maxid.toString()<<std::endl;

  xdata::UnsignedInteger64 flashid = tstore_rw.readLastConfigIdFlashed("minus");
  std::cout<<"The latest flashed id: "<<flashid.toString()<<std::endl;
  //tstore_rw.writeFlashTime(maxid);

  std::vector<std::string> all_ids = tstore_rw.readIDs(2, 20);
  std::cout<<"all ids:"<<std::endl;
  for (size_t i=0; i<all_ids.size(); ++i) std::cout<<" "<<all_ids[i];
  std::cout<<std::endl;

  std::vector<std::pair< std::string, std::string> > flash_list = tstore_rw.readFlashList("minus");
  for (size_t i=0; i<flash_list.size(); ++i) std::cout<<" f: "<<flash_list[i].first<<" \t "<<flash_list[i].second<<std::endl;


  std::vector<ConfigIDInfo> id_infos = tstore_rw.readFlashIDInfos("minus");
  cout<<"------- flash id infos minus:"<<endl;
  for (size_t i=0; i<id_infos.size(); ++i) cout<<id_infos[i] <<endl;

  id_infos = tstore_rw.readFlashIDInfos("plus");
  cout<<"------- flash id infos plus:"<<endl;
  for (size_t i=0; i<id_infos.size(); ++i) cout<<id_infos[i] <<endl;

  id_infos = tstore_rw.readIDInfos("minus");
  cout<<"------- id infos minus:"<<endl;
  for (size_t i=0; i<id_infos.size(); ++i) cout<<id_infos[i] <<endl;
  id_infos = tstore_rw.readIDInfos("plus");
  cout<<"------- id infos plus:"<<endl;
  for (size_t i=0; i<id_infos.size(); ++i) cout<<id_infos[i] <<endl;

  cout<<"encoded description:"<<endl;
  cout<<ConfigIDInfo::encodeDescription("short short","long long long long long long long long")<<endl;

  cout<<ConfigIDInfo("id","tw","tf","short short","long long long long long long long long")<<endl;
  cout<<ConfigIDInfo("id","tw","tf","short short\nlong long long long long long long long")<<endl;
  cout<<ConfigIDInfo("id","tw","tf","short")<<endl;
  cout<<ConfigIDInfo("id","tw","tf","")<<endl;


  tstore_rw.read(maxid);
  //tstore_rw.write();return;

  emu::db::ConfigTree tree_from_db(tstore_rw.configTables());
  
  /*
  std::cout<<std::endl<<"find CSCs:"<<std::endl;
  std::vector<emu::db::ConfigRow*> cscs = tree.find("CSC");
  for (std::vector<emu::db::ConfigRow*>::iterator it = cscs.begin(); it != cscs.end(); it++) { std::cout<<**it<<std::endl; }

  */


//  emu::db::XMLReadWriter xml1(&h, "/home/cscdev/config/pc/2000114.xml");
//  xml1.read(maxid);
//  emu::db::ConfigTree tree_from_xml(xml1.configTables());

//  emu::db::XMLReadWriter xml2(&h, "/home/cscdev/config/pc/test2.xml");
//  xml2.write(&(xml1.configTables()));

  emu::db::XMLReadWriter xml(&h, "config_save.xml");
  xml.write(&tree_from_db);
  //xml.write(&(tstore_rw.configTables()));


  emu::pc::EmuEndcapConfigWrapper wrapper_from_db(&tree_from_db);
  EmuEndcap * emue = wrapper_from_db.getConfiguredEndcap();
  //delete emue;

//  emu::db::EmuEndcapConfigWrapper wrapper_from_xml(&tree_from_xml);
//  emue = wrapper_from_xml.getConfiguredEndcap();

  //exportAsXML_test(emue, "/home/cscdev/config/pc/test_EmuEndcapConfigWrapper.xml");
  //exportAsXML_test(emue, "test_EmuEndcapConfigWrapper.xml");
  delete emue;

  //readConfigFromDB_test(maxid.toString());
}


/*std::string PCConfigEditor::fullTableID(const std::string &configName, const std::string &identifier)
{
  return configName + "_" + identifier;
}
*/

void PCConfigEditor::exportAsXML_test(EmuEndcap * endcap, const std::string &filename)
throw (xgi::exception::Exception)
{
  parseConfigFromEndcap_test(endcap);
  if (endcap == 0) XCEPT_RAISE(xgi::exception::Exception, "couldn't parse endcap");
  DOMNode *pDoc = 0;
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
    pDoc = (DOMNode*) doc;
  }
  catch (DOMException &e)
  {
    XCEPT_RAISE(xgi::exception::Exception,xoap::XMLCh2String(e.msg));
  }

  std::string XML = tstoreclient::writeXML(pDoc);
  emu::utils::writeFile(filename, XML);
}


void PCConfigEditor::parseConfigFromEndcap_test(EmuEndcap * endcap)
throw (xgi::exception::Exception)
{
  try
  {
    std::vector< Crate * > myCrates = endcap->AllCrates();
    xdata::Table dataAsTable;
    getTableDefinitionsIfNecessary();

    clearCachedTables();
    if (myCrates.size())
    {
      for (unsigned i = 0; i < myCrates.size(); ++i)
      {
        if (myCrates[i])
        {
          copyPeripheralCrateToTable(dataAsTable, myCrates[i]);
          setCachedTable("peripheralcrate", myCrates[i]->CrateID(), dataAsTable);
          //show the chambers by default so it is easy to search for them on the page
          //setShouldDisplayConfiguration("wholecrate",crateIdentifierString(myCrates[i]->CrateID()),true);
          copyCCBToTable(dataAsTable, myCrates[i]);
          setCachedTable("ccb", myCrates[i]->CrateID(), dataAsTable);
          copyMPCToTable(dataAsTable, myCrates[i]);
          setCachedTable("mpc", myCrates[i]->CrateID(), dataAsTable);
          copyVMECCToTable(dataAsTable, myCrates[i]);
          setCachedTable("vcc", myCrates[i]->CrateID(), dataAsTable);
          std::vector< Chamber * > chambers = myCrates[i]->chambers();
          for (unsigned chamberIndex = 0; chamberIndex < chambers.size(); ++chamberIndex)
          {
            if (chambers[chamberIndex])
            {
              copyCSCToTable(dataAsTable, chambers[chamberIndex]);
              std::string chamber = chamberID(myCrates[i]->CrateID(), chambers[chamberIndex]->GetLabel());
              emu::db::ConfigEditor::setCachedTable("csc", chamber, dataAsTable);
              DAQMB *dmb = chambers[chamberIndex]->GetDMB();
              if (dmb)
              {
                std::string cacheIdentifier = DAQMBID(chamber, dmb->slot());
                copyDAQMBToTable(dataAsTable, dmb);
                emu::db::ConfigEditor::setCachedTable("daqmb", cacheIdentifier, dataAsTable);
                copyCFEBToTable(dataAsTable, dmb);
                emu::db::ConfigEditor::setCachedTable("cfeb", cacheIdentifier, dataAsTable);
              }
              TMB *tmb = chambers[chamberIndex]->GetTMB();
              if (tmb)
              {
                std::string cacheIdentifier = DAQMBID(chamber, tmb->slot());
                copyTMBToTable(dataAsTable, tmb);
                emu::db::ConfigEditor::setCachedTable("tmb", cacheIdentifier, dataAsTable);
                ALCTController * thisALCT = tmb->alctController();
                if (thisALCT)
                {
                  copyALCTToTable(dataAsTable, thisALCT);
                  emu::db::ConfigEditor::setCachedTable("alct", cacheIdentifier, dataAsTable);
                  copyAnodeChannelToTable(dataAsTable, thisALCT);
                  emu::db::ConfigEditor::setCachedTable("anodechannel", cacheIdentifier, dataAsTable);
                }
              }
            }
          }
        }
      }
    }
  }
  catch (xcept::Exception &e)
  {
    std::cout <<"parseConfigFromEndcap_test: "<< e.what() << std::endl;
    //outputException(out, e);
  }
  catch (std::exception &e)
  {
    std::cout <<"parseConfigFromEndcap_test: "<< e.what() << std::endl;
  }
  catch (std::string &e)
  {
    std::cout <<"parseConfigFromEndcap_test: string thrown: " << e << std::endl;
  }
  catch (...)
  {
    std::cout <<"parseConfigFromEndcap_test: unknown thing thrown" << std::endl;
  }
}




std::string PCConfigEditor::elementNameFromTableName(const std::string &tableName)
{
  if (tableName == "anodechannel") return "AnodeChannel";
  if (tableName == "peripheralcrate") return "PeripheralCrate";
  return toolbox::toupper(tableName);
}


std::string PCConfigEditor::attributeNameFromColumnName(const std::string &column)
{
  static std::map<std::string, std::string> columnNames;
  if (columnNames.empty())
  {
    std::string columnNamesArray[] = {"MAC_addr", "MCAST_1", "MCAST_2", "MCAST_3", "Dflt_Srv_MAC", "VCC_frmw_ver",
        "Ethernet_CR", "Ext_FIFO_CR", "Rst_Misc_CR", "VME_CR", "BTO", "BGTO", "Msg_Lvl", "Warn_On_Shtdwn",
        "Pkt_On_Startup", "xLatency", "xFineLatency", "calibration_LCT_delay", "enableCLCTInputs_reg42",
        "enableCLCTInputs_reg68", "l1aDelay", "CCBmode", "TTCrxID", "TTCrxCoarseDelay", "TTCrxFineDelay", "crateID",
        "TransparentMode"};
    for (size_t columnIndex = 0; columnIndex < sizeof(columnNamesArray) / sizeof(columnNamesArray[0]); columnIndex++)
    {
      columnNames[toolbox::tolower(columnNamesArray[columnIndex])] = columnNamesArray[columnIndex];
    }
  }
  std::string lowerCaseName = toolbox::tolower(column);
  if (columnNames.count(lowerCaseName))
  {
    return columnNames[lowerCaseName];
  }
  if (lowerCaseName == "problem_mask") return "problem_location_mask";
  return lowerCaseName;
}


//some values are read from the XML as hex values, and should be exported to XML as hex
///and displayed in hex on the web interface.
bool PCConfigEditor::shouldDisplayInHex(const std::string &columnName)
{
  /*std::string hexColumns[]={"TMB_FIRMWARE_COMPILE_TYPE",
   "PROBLEM_MASK",
   "DMB_CNTL_FIRMWARE_TAG",
   "CFEB_FIRMWARE_TAG",
   "LAYER0_DISTRIP_HOT_CHANN_MASK",
   "LAYER1_DISTRIP_HOT_CHANN_MASK",
   "LAYER2_DISTRIP_HOT_CHANN_MASK",
   "LAYER3_DISTRIP_HOT_CHANN_MASK",
   "LAYER4_DISTRIP_HOT_CHANN_MASK",
   "LAYER5_DISTRIP_HOT_CHANN_MASK"};
   if (columnName.find("KILL_CHIP")!=std::string::npos) return true;
   std::string *lastElement=&hexColumns[sizeof hexColumns/sizeof hexColumns[0]];
   return (std::find(&hexColumns[0],lastElement,columnName)!=lastElement);*/

  //the other ones are stored as strings anyway, so there is no point converting them
  //from hex strings to numbers and then back into hex strings, except to validate input.
  std::string uppercaseName = toolbox::toupper(columnName);
  if (uppercaseName == "PROBLEM_MASK")
  {
    //if ever there are other columns which should be displayed as hex and are stored as numbers,
    //this method should take an extra output parameter minimumDigits, which would be used by ConfigEditor::xdataToHex
    //but since so far all hex parameters except this one are stored as strings,
    //for now ConfigEditor::xdataToHex always sets the minimum digits to 2.
    //minimumDigits=2;
    return true;
  }
  return false;
}


/*
void PCConfigEditor::outputException(xgi::Output * out, xcept::Exception &e)
{
  std::cout << e.message() << std::endl;
  *out << "<p>" << e.message() << "</p>" << std::endl;
}
*/


bool PCConfigEditor::canChangeColumnGlobally(const std::string &columnName, const std::string &tableName)
{
  if (columnName == "DESCRIPTION") return false;
  if (columnName == "PROBLEM_MASK") return false;
  //can't edit ID columns, which all have _CONFIG_ID in their names
  return canChangeColumn(columnName, tableName);
}


bool PCConfigEditor::columnIsUniqueIdentifier(const std::string &columnName, const std::string &tableName)
{
  return columnIsDatabaseOnly(columnName, tableName);
}


bool PCConfigEditor::columnIsDatabaseOnly(const std::string &columnName, const std::string &tableName)
{
  return columnName.find("CONFIG_ID") != std::string::npos;
}


bool PCConfigEditor::canChangeColumn(const std::string &columnName, const std::string &tableName)
{
  if (columnName == "CRATEID") return false;
  if (columnName == "CFEB_NUMBER") return false;
  if (columnName == "AFEB_NUMBER") return false;
  if (columnName == "LABEL") return false;
  return !columnIsDatabaseOnly(columnName, tableName);
}


void PCConfigEditor::outputStandardInterface(xgi::Output * out)
{
  *out << cgicc::fieldset().set("style", "font-size: 10pt;  font-family: arial;") << std::endl;

  *out << cgicc::table().set("border", "0").set("cellpadding", "10");

  *out << cgicc::tr();
  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/parse",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Parse XML file").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/parseTestSummary",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Parse Timing Scan") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/exportAsXML",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Export as XML").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/upload",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Upload to DB").set("style", "width:120px;") << std::endl;
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
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/selectVersion",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << "<table border=\"1\"><tr><td>";
  outputEndcapSelector(out);

  *out << "</td><td>" << cgicc::input().set("type", "submit").set("value", "Read version...").set("style",
      "width:120px;") << "</td></tr></table>" << std::endl;
  *out << cgicc::form() << cgicc::td() << std::endl;

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method", "GET").set("action", toolbox::toString("/%s/selectVersions",
      getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << "<table border=\"1\"><tr><td>";
  outputEndcapSelector(out);
  *out << "</td><td>" << cgicc::input().set("type", "submit").set("value", "Compare versions...")
      << "</td></tr></table>" << std::endl;
  *out << cgicc::form() << std::endl;

  *out << cgicc::td();

#if 1
  //This recreates the tables in the database, it only needs to be done for the initial setup or if the database structure changes,
  //so we don't want to do it by accident.

  *out << cgicc::td().set("style", "width:130px;");
  *out << cgicc::form().set("method","GET").set("action", toolbox::toString("/%s/sync",getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Sync to DB").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td();
#endif

  *out << cgicc::tr();
  *out << cgicc::table();

  *out << "<hr>";
  *out << "XML filename: ";
  *out << cgicc::input().set("type", "text").set("value", xmlfile_).set("style", "width:585px;").set("DISABLED")
      << cgicc::br() << std::endl;
  *out << "config type: ";
  *out << cgicc::input().set("type", "text").set("value", config_type_).set("DISABLED") << cgicc::br() << std::endl;
  *out << "Description: ";
  *out << cgicc::input().set("type", "text").set("value", config_desc_).set("DISABLED") << cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset() << cgicc::br();
  //
  *out << cgicc::fieldset().set("style", "font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Select XML file").set("style", "color:blue") << std::endl;
  //
  outputFileSelector(out);
  std::string ReadString = toolbox::toString("/%s/SetTypeDesc", getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method", "GET").set("action", ReadString) << std::endl;
  *out << cgicc::input().set("type", "text").set("value", "").set("name", "ConfigType") << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Set Type") << std::endl;
  *out << cgicc::form() << std::endl;
  //
  ReadString = toolbox::toString("/%s/SetTypeDesc", getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method", "GET").set("action", ReadString) << std::endl;
  *out << cgicc::input().set("type", "text").set("value", "").set("name", "ConfigDesc") << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Set Description") << std::endl;
  *out << cgicc::form() << std::endl;

  *out << cgicc::fieldset() << cgicc::br();
  //
  *out << cgicc::fieldset().set("style", "font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Timing Scan").set("style", "color:blue") << std::endl;

  *out << cgicc::form().set("method", "POST").set("enctype", "multipart/form-data").set("action", toolbox::toString(
      "/%s/SelectTestSummaryFile", getApplicationDescriptor()->getURN().c_str())) << std::endl;
  *out << cgicc::input().set("type", "file").set("name", "testSummaryFile").set("size", "90") << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Read") << std::endl;
  *out << cgicc::form() << std::endl;

  *out << cgicc::fieldset() << cgicc::br();

  *out << cgicc::fieldset().set("style", "font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //

  *out << "Messages:<br>";
  //  *out << cgicc::fieldset() << cgicc::br();
  //
}


void PCConfigEditor::SelectTestSummaryFile(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    std::cout << "Button: Select crate test file" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::const_file_iterator file;
    file = cgi.getFile("testSummaryFile");
    //
    if (file != cgi.getFiles().end())
    {
      testSummary_ = (*file).getData();
      std::cout << "Select Crate test file " << testSummary_ << std::endl;
    }
    this->Default(in, out);
    //
  }
  catch (const std::exception & e)
  {
    XCEPT_RAISE(xgi::exception::Exception, e.what());
  }
}


//runs a diff query and caches the results
//this is used in any situation where you don't need values from the diff to read in child records or do anything else complicated.
void PCConfigEditor::simpleDiff(
    const std::string &queryViewName,
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_xxx_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_xxx_config_id,
    const std::string &identifier) throw (xcept::Exception)
{
  xdata::Table results;
  diff(connectionID, queryViewName, old_emu_config_id, old_xxx_config_id, new_emu_config_id, new_xxx_config_id, results);
  setCachedDiff(queryViewName, identifier, results);
}


void PCConfigEditor::simpleCrateDiff(
    const std::string &queryViewName,
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_periph_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_periph_config_id,
    int crateID) throw (xcept::Exception)
{
  std::string identifier = crateIdentifierString(crateID);
  simpleDiff(queryViewName, connectionID, old_emu_config_id, old_periph_config_id, new_emu_config_id,
      new_periph_config_id, identifier);
}


void PCConfigEditor::diffCrate(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &new_emu_config_id) throw (xcept::Exception)
{
  std::string queryViewName = "peripheralcrate";
  std::string periph_config_id;
  xdata::Table results;
  int crateid = 0;
  std::string label;

  //there is actually nothing in the peripheral crate table that can be changed.
  //for now the diff just selects the crate IDs necessary to select child records,
  //even for crates which may not have changed.
  diff(connectionID, queryViewName, old_emu_config_id, new_emu_config_id, results);
  std::cout << "Peripheral Crate" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  std::string old_periph_config_id;
  std::string new_periph_config_id;
  crateIDsInDiff.clear();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      std::string StrgValue = results.getValueAt(rowIndex, *column)->toString();
      if (*column == "PERIPH_CONFIG_ID " + old_emu_config_id)
      {
        old_periph_config_id = StrgValue;
      }
      else if (*column == "PERIPH_CONFIG_ID " + new_emu_config_id)
      {
        new_periph_config_id = StrgValue;
      }
      else if (*column == "CRATEID")
      {
        crateid = readCrateID(results, rowIndex);
        crateIDsInDiff.push_back(results.getValueAt(rowIndex, "CRATEID")->toString());
      }
    }

    for (std::map<std::string, std::vector<std::string> >::iterator tableName = tableNames.begin();
        tableName != tableNames.end(); ++tableName)
    {
      if ((*tableName).second.empty())
      { //if we don't have to load any child configurations based on this one
        simpleCrateDiff((*tableName).first, connectionID, old_emu_config_id, old_periph_config_id,
            new_emu_config_id, new_periph_config_id, crateid);
      }
    }
    diffCSC(connectionID, old_emu_config_id, old_periph_config_id, new_emu_config_id, new_periph_config_id, crateid);
    std::cout << "#### PERIPHERAL_CRATE:  " << periph_config_id << " --- "
        << emu::db::to_string(crateid) << " --- " << label << std::endl;
  }
}


void PCConfigEditor::SetTypeDesc(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  cgicc::form_iterator name2 = cgi.getElement("ConfigType");

  if (name2 != cgi.getElements().end())
  {
    config_type_ = cgi["ConfigType"]->getValue();
    std::cout << "Type " << config_type_ << std::endl;
    //
  }
  name2 = cgi.getElement("ConfigDesc");
  if (name2 != cgi.getElements().end())
  {
    config_desc_ = cgi["ConfigDesc"]->getValue();
    std::cout << "Description " << config_desc_ << std::endl;
    //
  }
  this->Default(in, out);
}


//reads up to the first occurrence of \a delimiter in \a input,
//and checks that the value read is equal to \a expectedValue.
//throws an exception if the value is not as expected.
//\a delimiter defaults to \n
void PCConfigEditor::checkLine(
    std::istream &input,
    const std::string &expectedValue,
    char delimiter) throw (xdaq::exception::Exception)
{
  std::string line;
  std::getline(input, line, delimiter);
  if (line != expectedValue && !expectedValue.empty())
  {
    XCEPT_RAISE(xdaq::exception::Exception,"File format does not seem to be valid. Expected '"+expectedValue+"', read '"+line+"'");
  }
}


//parses all the values for a single chamber
//returns whether there is another chamber
bool PCConfigEditor::parseTestSummaryChamber(std::istringstream &testSummary, xdata::Table &tmbChanges,int &TTCrxID,std::string &chamberName)
throw (xdaq::exception::Exception)
{
  getTableDefinitionsIfNecessary();
  tmbChanges=tableDefinitions["tmb"];
  std::vector<std::string> columns=tmbChanges.getColumns();
  for (std::vector<std::string>::iterator column=columns.begin();column!=columns.end();++column)
  {
    if (!isNumericType(tmbChanges.getColumnType(*column)))
    {
      tmbChanges.removeColumn(*column);
    }
  }
  tmbChanges.append();
  chamberName="";
  while (true)
  {
    int beginningOfLine=testSummary.tellg();
    std::string column;
    std::getline(testSummary,column,' ');
    column=toolbox::toupper(toolbox::trim(column));
    if (column=="++++++++++++++++++++")
    {
      checkLine(testSummary,"Timing scans : "); //we already read the space in front of it
      checkLine(testSummary,"--------------------");
      return true;
    }
    checkLine(testSummary,"",'M');
    std::string chamber;
    std::getline(testSummary,chamber,' ');
    chamber=std::string("M")+chamber;
    if (chamberName.empty()) chamberName=chamber;
    else if (chamberName!=chamber)
    {
      //std::cout << "old chamber was " << chamberName << ", new chamber is " << chamber << std::endl;
      testSummary.seekg(beginningOfLine);
      return (chamber!="M");
    }
    signed int value;
    testSummary >> value;
    //std::cout << "value of " << column << " in " << chamber << " is " << value << std::endl;
    //set the value of this column if value is positive (usually it is -1 if the value does not need to be changed, but it can be -998)
    if (value>0)
    {
      if (column=="TTCRXID")
      {
        TTCrxID=value;
      } else if (tableHasColumn(tmbChanges,column))
      {
        tmbChanges.getValueAt(0,column)->fromString(toolbox::toString("%d",value));
      }
    }
  }
}


//looks for a key in \a haystack which contains the string \a needle, and returns the corresponding value.
//throws an exception if none is found.
//The functionality is pretty generic, but it is only used from applyTestSummary
//to find configuration relating a specific chamber when the crate is not known
//and the wording of the exception it throws if the value can not be found specifically refers to that
//if it is used in any other contexts then the exception would have to be changed.
xdata::Table &PCConfigEditor::valueForKeyContaining(
    std::map<std::string, xdata::Table> &haystack,
    const std::string &needle) throw (xdaq::exception::Exception)
{
  return haystack[keyContaining(haystack, needle)];
}


std::string PCConfigEditor::keyContaining(
    std::map<std::string, xdata::Table> &haystack,
    const std::string &needle) throw (xdaq::exception::Exception)
{
  for (std::map<std::string, xdata::Table>::iterator table = haystack.begin(); table != haystack.end(); ++table)
  {
    std::string key = table->first;
    std::string::size_type foundAt = key.rfind(needle);
    if (foundAt != std::string::npos && foundAt + needle.size() == key.size() - 1)
    { //make sure it is at the end of the key, in case we are looking for 1//1/1 and we find 1/1/10
      //std::cout << key << " contains " << needle << " rfind+needlesize=" << key.rfind(needle)+needle.size()
      //  << " keysize=" << key.size() << std::endl;
      return key;
    }
  }
  XCEPT_RAISE(xdaq::exception::Exception,"No data loaded for chamber "+needle+".");
}


void PCConfigEditor::applyTestSummary(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  outputHeader(out);
  outputStandardInterface(out);
  try
  {
    for (std::map<std::string, xdata::Table>::iterator chamber = currentTestSummary.begin();
        chamber != currentTestSummary.end(); ++chamber)
    {
      std::map<std::string, xdata::Table> &tmbTables = currentTables["tmb"];
      xdata::Table &tmbTableToChange = valueForKeyContaining(tmbTables, chamber->first);
      xdata::Table &changes = chamber->second;
      std::vector<std::string> columns = changes.getColumns();
      for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
      {
        xdata::Serializable *newValue = changes.getValueAt(0, *column);
        if (newValue->toString() != "NaN")
        {
          tmbTableToChange.setValueAt(0, *column, *newValue);
        }
      }
    }

    for (std::map<std::string, signed int>::iterator rxID = TTCrxIDs.begin(); rxID != TTCrxIDs.end(); ++rxID)
    {
      xdata::Table &ccbTableToChange = emu::db::ConfigEditor::getCachedTable(std::string("ccb"), rxID->first);
      // = valueForKeyContaining(currentTables["ccb"],rxID->first);
      ccbTableToChange.getValueAt(0, "TTCRXID")->fromString(toolbox::toString("%d", rxID->second));
    }
    outputCurrentConfiguration(out);
  }
  catch (xdaq::exception::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not apply changes.",e);
  }
}


std::string PCConfigEditor::crateForChamber(const std::string &chamberName) throw (xdaq::exception::Exception)
{
  std::string chamberKey = keyContaining(currentTables["csc"], chamberName);
  //std::cout << "crateForChamber " << chamberName << " is "
  //  << chamberKey.substr(0,chamberKey.length()-chamberName.length()-10)
  //  << "(from " << chamberKey << ")" << std::endl;
  return chamberKey.substr(0, chamberKey.length() - chamberName.length() - 10);
}


void PCConfigEditor::parseTestSummary(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  outputHeader(out);
  outputStandardInterface(out);
  try
  {
    TTCrxIDs.clear();
    std::istringstream testSummary(testSummary_);
    checkLine(testSummary, " *** Output : Test Summary *** ");
    checkLine(testSummary, "");
    checkLine(testSummary, "Operator ", ':');
    std::string operatorName;
    std::getline(testSummary, operatorName);
    checkLine(testSummary, "Time     ", ':');
    std::string time;
    std::getline(testSummary, time);
    checkLine(testSummary, "");
    checkLine(testSummary, "XML File ", ':');
    std::string expectedXMLFile;
    std::getline(testSummary, expectedXMLFile);
    checkLine(testSummary, "++++++++++++++++++++");
    checkLine(testSummary, " Timing scans : ");
    checkLine(testSummary, "--------------------");
    *out << "Operator: " << operatorName << cgicc::br() << "Time: " << time << cgicc::br()
        << "XML file:" << expectedXMLFile << cgicc::br();
    bool readChamber = false;
    do
    {
      xdata::Table changes;
      std::string chamberName;
      int TTCrxID = -1;
      readChamber = parseTestSummaryChamber(testSummary, changes, TTCrxID, chamberName);
      if (readChamber)
      {
        *out << "changes to " << chamberName << cgicc::br();
        if (TTCrxID > -1)
        {
          *out << "TTCrxID=" << TTCrxID << cgicc::br();
          std::string crateName = crateForChamber(chamberName);
          *out << "crate " << crateName << cgicc::br();
          if (TTCrxIDs.count(crateName))
          {
            if (TTCrxIDs[crateName] != TTCrxID)
            {
              std::ostringstream errorMessage;
              errorMessage << "Conflicting values for TTCrxID of " << crateName
                  << ". Already set to " << TTCrxIDs[crateName] << ", new value: " << TTCrxID;
              XCEPT_RAISE(xdaq::exception::Exception,errorMessage.str());
            }
          }
          TTCrxIDs[crateName] = TTCrxID;
        }
        outputTable(out, changes);
        currentTestSummary[chamberName] = changes;
      }
    }
    while (readChamber);
  }
  catch (xdaq::exception::Exception &e)
  {
    XCEPT_RETHROW(xgi::exception::Exception,"Could not parse test summary file. " ,e);
  }

  *out << cgicc::form().set("method", "GET").set("action",
      toolbox::toString("/%s/applyTestSummary", getApplicationDescriptor()->getURN().c_str()))
      << std::endl;
  *out << cgicc::input().set("type", "submit").set("value", "Apply").set("style", "width:120px;") << std::endl;
  *out << cgicc::form() << std::endl;
  outputFooter(out);
}


void PCConfigEditor::parseConfigFromXML(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    outputHeader(out);
    outputStandardInterface(out);
    if (xmlfile_.length() <= 0)
    {
      *out << "No XML file selected. Please select a valid XML file first." << std::endl;
    }
    else
    {
      std::string xmlname = /* xmlpath_ + */xmlfile_;
      std::cout << "XML full name " << xmlname << std::endl;
      std::cout << "Parsing of the peripheral crate config XML file into DOM tree ..." << std::endl;
      *out << "Parsing of the peripheral crate config XML file into DOM tree ..." << std::endl;
      time_t rawtime;
      struct tm * timeinfo;
      time(&rawtime);
      timeinfo = localtime(&rawtime);
      std::cout << "Parsing started at: " << asctime(timeinfo) << std::endl;
      *out << "<br>Parsing started at: " << asctime(timeinfo) << std::endl;

      XMLParser TStore_emuparser;
      TStore_emuparser.parseFile(xmlname);
      TStore_myEndcap_ = TStore_emuparser.GetEmuEndcap();

      time(&rawtime);
      timeinfo = localtime(&rawtime);
      if (TStore_myEndcap_)
      {
        std::cout << "Parsing ended at: " << asctime(timeinfo) << std::endl;
        *out << "<br>Parsing ended at: " << asctime(timeinfo) << std::endl;
      }
      else
      {
        std::cout << "ERROR.....Parser failed." << std::endl;
        *out << "<br>ERROR.....Parser failed." << std::endl;
      }
      //rather than converting to and from EmuEndcap etc. all the time, we will keep the values in xdata::Tables
      //for editing, and only convert back to the custom classes when/if necessary (which would be if it needs to be
      //output as XML.) This way the editing interface can be generic for all tables.
      //copyEndcapToTables();
      if (TStore_myEndcap_)
      {
        std::vector<Crate *> myCrates = TStore_myEndcap_->AllCrates();
        xdata::Table dataAsTable;
        getTableDefinitionsIfNecessary();

        clearCachedTables();
        if (myCrates.size())
        {
          for (unsigned i = 0; i < myCrates.size(); ++i)
          {
            if (myCrates[i])
            {
              copyPeripheralCrateToTable(dataAsTable, myCrates[i]);
              setCachedTable("peripheralcrate", myCrates[i]->CrateID(), dataAsTable);
              //show the chambers by default so it is easy to search for them on the page
              //setShouldDisplayConfiguration("wholecrate",crateIdentifierString(myCrates[i]->CrateID()),true);
              copyCCBToTable(dataAsTable, myCrates[i]);
              setCachedTable("ccb", myCrates[i]->CrateID(), dataAsTable);
              copyMPCToTable(dataAsTable, myCrates[i]);
              setCachedTable("mpc", myCrates[i]->CrateID(), dataAsTable);
              copyVMECCToTable(dataAsTable, myCrates[i]);
              setCachedTable("vcc", myCrates[i]->CrateID(), dataAsTable);
              std::vector<Chamber *> chambers = myCrates[i]->chambers();
              for (unsigned chamberIndex = 0; chamberIndex < chambers.size(); ++chamberIndex)
              {
                if (chambers[chamberIndex])
                {
                  copyCSCToTable(dataAsTable, chambers[chamberIndex]);
                  std::string chamber = chamberID(myCrates[i]->CrateID(), chambers[chamberIndex]->GetLabel());
                  emu::db::ConfigEditor::setCachedTable("csc", chamber, dataAsTable);
                  DAQMB *dmb = chambers[chamberIndex]->GetDMB();
                  if (dmb)
                  {
                    std::string cacheIdentifier = DAQMBID(chamber, dmb->slot());
                    copyDAQMBToTable(dataAsTable, dmb);
                    emu::db::ConfigEditor::setCachedTable("daqmb", cacheIdentifier, dataAsTable);
                    copyCFEBToTable(dataAsTable, dmb);
                    emu::db::ConfigEditor::setCachedTable("cfeb", cacheIdentifier, dataAsTable);
                  }
                  TMB *tmb = chambers[chamberIndex]->GetTMB();
                  if (tmb)
                  {
                    std::string cacheIdentifier = DAQMBID(chamber, tmb->slot());
                    copyTMBToTable(dataAsTable, tmb);
                    emu::db::ConfigEditor::setCachedTable("tmb", cacheIdentifier, dataAsTable);
                    ALCTController * thisALCT = tmb->alctController();
                    if (thisALCT)
                    {
                      copyALCTToTable(dataAsTable, thisALCT);
                      emu::db::ConfigEditor::setCachedTable("alct", cacheIdentifier, dataAsTable);
                      copyAnodeChannelToTable(dataAsTable, thisALCT);
                      emu::db::ConfigEditor::setCachedTable("anodechannel", cacheIdentifier, dataAsTable);
                    }
                  }
                }
              }
            }
          }
        }
      }
      outputCurrentConfiguration(out);
    }
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  catch (std::string &e)
  {
    std::cout << "string thrown: " << e << std::endl;
  }
  catch (...)
  {
    //todo: change this to do something more useful. I only added it to prevent crashing
    std::cout << "unknown thing thrown" << std::endl;
  }
}


void PCConfigEditor::setCachedTable(
    const std::string &insertViewName,
    int crateID,
    xdata::Table &table
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  std::string identifier = crateIdentifierString(crateID);
  emu::db::ConfigEditor::setCachedTable(insertViewName, identifier, table);
}


void PCConfigEditor::setCachedDiff(
    const std::string &insertViewName,
    int crateID,
    xdata::Table &table
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  std::string identifier = crateIdentifierString(crateID);
  emu::db::ConfigEditor::setCachedDiff(insertViewName, identifier, table);
}


void PCConfigEditor::readConfigFromDB(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{
  try
  {
    emu::utils::SimpleTimer timer;

    cgicc::Cgicc cgi(in);
    outputHeader(out);
    outputStandardInterface(out);
    std::string endcap_side;
    std::string emu_config_id = cgi("configID");
    if (emu_config_id.empty())
    { //if no config ID is specified, we load the latest one for the given side
      endcap_side = **cgi["side"];
      if (endcap_side != "plus" && endcap_side != "minus") endcap_side = "minus";
      std::string connectionID = connect();
      emu_config_id = getConfigId("EMU_CONFIGURATION", "EMU_CONFIG_ID", endcap_side).toString();
      *out << "<br>max of EMU_CONFIG_ID for " << endcap_side << " side = " << emu_config_id << "<br>" << std::endl;
      disconnect(connectionID);
    }
    else
    {
      *out << "<br>fetching config for config id " << emu_config_id << "<br>" << std::endl;
    }
    getTableDefinitionsIfNecessary();
    clearCachedTables();
    if (TStore_myEndcap_)
    {
      delete TStore_myEndcap_;
    }
    //*out << "TStore_myEndcap_=getConfiguredEndcap(" << emu_config_id << ")" << std::endl;
    TStore_myEndcap_ = getConfiguredEndcap(emu_config_id);
    //*out << "TStore_myEndcap_==" << (TStore_myEndcap_?"NOT NULL":"NULL") << std::endl;

    std::cout<<"Endc read time: "<<timer.sec()<<std::endl;

    outputCurrentConfiguration(out);
  }
  catch (xcept::Exception &e)
  {
    outputException(out, e);
  }
}

void PCConfigEditor::readConfigFromDB_test(const std::string &emu_config_id) throw (xgi::exception::Exception)
{
  try
  {
    emu::utils::SimpleTimer timer;

    std::cout << "\n\n\n fetching config for config id " << emu_config_id << "<br>" << std::endl;

    getTableDefinitionsIfNecessary();
    clearCachedTables();
    if (TStore_myEndcap_) delete TStore_myEndcap_;
    //*out << "TStore_myEndcap_=getConfiguredEndcap(" << emu_config_id << ")" << std::endl;
    TStore_myEndcap_ = getConfiguredEndcap(emu_config_id);
    //*out << "TStore_myEndcap_==" << (TStore_myEndcap_?"NOT NULL":"NULL") << std::endl;

    std::cout<<"Endc read time: "<<timer.sec()<<" sec"<<std::endl;
  }
  catch (xcept::Exception &e)
  {
    std::cout<<"problemos!"<<std::endl;
  }
}


// ################################
// #   TStore related functions   #
// ################################


xdata::UnsignedInteger64 PCConfigEditor::getConfigId(
    const std::string &dbTable,
    const std::string &dbColumn,
    const std::string endcap_side) throw (xcept::Exception)
{
  std::string connectionID = connect();
  std::string queryViewName = "getconfigids";
  xdata::Table results;
  xdata::UnsignedInteger64 value(0);
  queryMaxId(connectionID, queryViewName, dbTable, dbColumn, endcap_side, results);

  std::vector<std::string> columns = results.getColumns();
  for (unsigned long rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      if (*column == dbColumn)
      {
        value.fromString(results.getValueAt(rowIndex, *column)->toString());
      }
    }
  }
  disconnect(connectionID);
  return value;
}


void PCConfigEditor::queryMaxId(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::string &dbTable,
    const std::string &dbColumn,
    const std::string endcap_side,
    xdata::Table &results) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");

  //If we give the name of the view class when constructing the emu::db::TStoreRequest,
  //it will automatically use that namespace for
  //any view specific parameters we add.
  emu::db::TStoreRequest request("query", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  request.addViewSpecificParameter("name", queryViewName);

  //add parameter name and value (database table)
  request.addViewSpecificParameter("TABLE", dbTable);

  //add parameter name and value (databse column)
  request.addViewSpecificParameter("COLUMN", dbColumn);

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
}


void PCConfigEditor::diff(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::string &old_emu_config_id,
    const std::string &new_emu_config_id,
    xdata::Table &results) throw (xcept::Exception)
{
  diff(connectionID,queryViewName,old_emu_config_id,"",new_emu_config_id,"",results);
}


void PCConfigEditor::diff(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::string &old_emu_config_id,
    const std::string &old_xxx_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_xxx_config_id,
    xdata::Table &results) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");

  //If we give the name of the view class when constructing the emu::db::TStoreRequest,
  //it will automatically use that namespace for
  //any view specific parameters we add.
  emu::db::TStoreRequest request("query", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  //We'll use the "hello" one.
  request.addViewSpecificParameter("name", queryViewName + "_diff");

  //add parameter name and value (emu_config_id)
  request.addViewSpecificParameter("OLD_EMU_CONFIG_ID", old_emu_config_id);

  if (!old_xxx_config_id.empty() && !new_xxx_config_id.empty())
  {
    //add parameter name and value (xxx_config_id; xxx="periph|ccb|mpc|csc|tmb|daqmb|alct|afeb|cfeb" )
    request.addViewSpecificParameter("OLD_XXX_CONFIG_ID", old_xxx_config_id);

    //add parameter name and value (xxx_config_id; xxx="periph|ccb|mpc|csc|tmb|daqmb|alct|afeb|cfeb" )
    request.addViewSpecificParameter("NEW_XXX_CONFIG_ID", new_xxx_config_id);
  }

  //add parameter name and value (emu_config_id)
  request.addViewSpecificParameter("NEW_EMU_CONFIG_ID", new_emu_config_id);

  xoap::MessageReference message = request.toSOAP();
  xoap::MessageReference response = sendSOAPMessage(message);

  //use the TStore client library to extract the first attachment of type "table"
  //from the SOAP response
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE (xcept::Exception, "Server returned no data");
  }
}


void PCConfigEditor::query(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::string &emu_config_id,
    xdata::Table &results) throw (xcept::Exception)
{
  query(connectionID, queryViewName, emu_config_id, "", results);
}


void PCConfigEditor::query(
    const std::string &connectionID,
    const std::string &queryViewName,
    const std::string &emu_config_id,
    const std::string &xxx_config_id,
    xdata::Table &results) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");

  //If we give the name of the view class when constructing the emu::db::TStoreRequest,
  //it will automatically use that namespace for any view specific parameters we add.
  emu::db::TStoreRequest request("query", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  request.addViewSpecificParameter("name", queryViewName);

  //add parameter name and value (emu_config_id)
  request.addViewSpecificParameter("EMU_CONFIG_ID", emu_config_id);

  if (!xxx_config_id.empty())
  {
    //add parameter name and value (xxx_config_id; xxx="periph|ccb|mpc|csc|tmb|daqmb|alct|afeb|cfeb" )
    request.addViewSpecificParameter("XXX_CONFIG_ID", xxx_config_id);
  }

  xoap::MessageReference message = request.toSOAP();
  xoap::MessageReference response = sendSOAPMessage(message);

  //use the TStore client library to extract the first attachment of type "table" from the SOAP response
  if (!tstoreclient::getFirstAttachmentOfType(response, results))
  {
    XCEPT_RAISE (xcept::Exception, "Server returned no data");
  }

  //xdata::Table rr = emu::utils::fixTStoreTable(results);
}


void PCConfigEditor::insert(
    const std::string &connectionID,
    const std::string &insertViewName,
    xdata::Table &newRows) throw (xcept::Exception)
{
  //for a query, we need to send some parameters which are specific to SQLView.
  //these use the namespace tstore-view-SQL.

  //In general, you might have the view name in a variable, so you won't know the view class. In this
  //case you can find out the view class using the TStore client library:
  std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUsystem");

  //If we give the name of the view class when constructing the emu::db::TStoreRequest,
  //it will automatically use that namespace for any view specific parameters we add.
  emu::db::TStoreRequest request("insert", viewClass);

  //add the connection ID
  request.addTStoreParameter("connectionID", connectionID);

  //for an SQLView, the name parameter refers to the name of a query section in the configuration
  request.addViewSpecificParameter("name", insertViewName);

  xoap::MessageReference message = request.toSOAP();

  //add our new rows as an attachment to the SOAP message
  //the last parameter is the ID of the attachment. The SQLView does not mind what it is, as there
  //should only be one attachment per message.
  tstoreclient::addAttachment(message, newRows, "test");

  xoap::MessageReference response = sendSOAPMessage(message);
  xoap::SOAPBody body = response->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault())
  {
    XCEPT_RAISE (xcept::Exception,"An error has occured during inserting data to database!");
  }
}


// ######################
// #  Misc              #
// ######################


/// Gets database user's name and password from \ref dbUserFile_ .
void PCConfigEditor::getDbUserData()
{
  std::fstream fs;
  fs.open(dbUserFile_.c_str(), std::ios::in);
  if (fs.is_open())
  {
    std::string userAndPasswd;
    fs >> userAndPasswd;
    if (!userAndPasswd.length())
    {
      std::stringstream oss;
      oss << dbUserFile_ << " contains no username/password. ==> No TStore database access.";
      throw oss.str();
    }
    dbUserAndPassword_ = userAndPasswd;
    fs.close();
  }
  else
  {
    std::stringstream oss;
    oss << "Could not open " << dbUserFile_ << " for reading. ==> No TStore database access.";
    throw oss.str();
  }
}


void PCConfigEditor::getTableDefinitions(const std::string &connectionID)
{
  ConfigEditor::getTableDefinitions(connectionID);
  getDefinition(connectionID, "configuration");
}


std::string PCConfigEditor::getEndcapSide(std::vector<Crate *> &myCrates) throw (xcept::Exception)
{
  // get peripheral crate data from DOM tree
  if (myCrates.size() <= 0)
  {
    XCEPT_RAISE(xcept::Exception,"No crate found.");
  }
  std::string crate_label = myCrates[0]->GetLabel();
  std::string endcap_side;
  if (strncmp(crate_label.c_str(), "VMEp", 4) == 0)
  {
    endcap_side = "plus";
  }
  else if (strncmp(crate_label.c_str(), "VMEm", 4) == 0)
  {
    endcap_side = "minus";
  }
  else
  {
    XCEPT_RAISE(xcept::Exception,"Unknown crate label '"+crate_label+"', should be 'VMEp' or 'VMEm'.");
  }
  return endcap_side;
}


//this was to fix a problem with column types which I have since fixed in a better way
//still, leave the function here in case there is ever a need to change the way config IDs are set.
void PCConfigEditor::setConfigID(
    xdata::Table &newRows,
    size_t rowId,
    const std::string &columnName,
    xdata::UnsignedInteger64 &id)
{
  newRows.setValueAt(rowId, columnName, id);
}


// ######################
// #   Uploading data   #
// ######################

void PCConfigEditor::startUpload(xgi::Input * in) throw (xcept::Exception)
{
  // Processing DOM Tree
  if (TStore_myEndcap_)
  {
    std::cout << "<br>Found EmuEndcap<br>" << std::endl;
  }
  else
  {
    std::cout << "<br>EmuEndcap is not loaded into memory. First run ParseXML!<br>" << std::endl;
    return;
  }

  try
  {
    std::vector<Crate *> myCrates;
    myCrates.clear();
    myCrates = TStore_myEndcap_->AllCrates();
    std::string endcap_side = getEndcapSide(myCrates);

    std::string connectionID = connect();

    getTableDefinitions(connectionID);

    uploadConfiguration(connectionID, endcap_side);
    uploadPeripheralCrate(connectionID, myCrates);

    disconnect(connectionID);
  }
  catch (xcept::Exception &e)
  {
    LOG4CPLUS_WARN(this->getApplicationLogger(),e.what()+(std::string)" Stop loading to database...");
    XCEPT_RETHROW(xcept::Exception,"Problem uploading data to database",e);
  }
}


void PCConfigEditor::uploadConfiguration(
    const std::string &connectionID,
    const std::string endcap_side) throw (xcept::Exception)
{
  std::string insertViewName = "configuration";
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
  if (emu_config_id.value_ == 0) emu_config_id = ((endcap_side == "plus") ? 1000000 : 2000000);
  emu_config_id++;
  emu_config_id_ = emu_config_id;
  xdata::TimeVal _emu_config_time = (xdata::TimeVal) currentTime.gettimeofday();
  // Info to be entered in a form on a HyperDAQ page
  xdata::String _emu_config_type = config_type_;
  xdata::String _description = config_desc_;
  xdata::String _emu_endcap_side = endcap_side;

#ifdef debugV
  std::cout << "-- CONFIGURATION  emu_config_id --------- " << emu_config_id.toString() << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_time ------- " << _emu_config_time.toString() << std::endl;
  std::cout << "-- CONFIGURATION  emu_config_type ------- " << _emu_config_type.toString() << std::endl;
  std::cout << "-- CONFIGURATION  description ----------- " << _description.toString() << std::endl;
  std::cout << "-- CONFIGURATION  emu_endcap_side ------- " << _emu_endcap_side.toString() << std::endl;
#endif

  newRows = tableDefinitions[insertViewName];

  newRows.setValueAt(rowId, DESCRIPTION, _description);
  setConfigID(newRows, rowId, EMU_CONFIG_ID, emu_config_id);
  newRows.setValueAt(rowId, EMU_CONFIG_TIME, _emu_config_time);
  newRows.setValueAt(rowId, EMU_CONFIG_TYPE, _emu_config_type);
  newRows.setValueAt(rowId, EMU_ENDCAP_SIDE, _emu_endcap_side);

  insert(connectionID, insertViewName, newRows);

  std::cout << "Configuration for " << endcap_side << " has been loaded to database as " << emu_config_id << std::endl;
}


void PCConfigEditor::copyPeripheralCrateToTable(xdata::Table &newRows, Crate * TStore_thisCrate)
{
  newRows = tableDefinitions["peripheralcrate"];

  std::string LABEL("LABEL");
  std::string CRATEID("CRATEID");
  xdata::UnsignedShort _crateid = TStore_thisCrate->CrateID();
  xdata::String _label = TStore_thisCrate->GetLabel();

  newRows.setValueAt(0, CRATEID, _crateid);
  newRows.setValueAt(0, LABEL, _label);
}


void PCConfigEditor::uploadPeripheralCrate(
    const std::string &connectionID,
    const std::vector<Crate *> &TStore_allCrates) throw (xcept::Exception)
{
  std::string insertViewName = "peripheralcrate";
  xdata::Table newRows;
  std::cout << "List of crates:<br>" << std::endl;
  for (unsigned i = 0; i < TStore_allCrates.size(); ++i)
  {
    if (TStore_allCrates[i])
    {
      size_t rowId(0);
      std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
      std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");

      xdata::UnsignedInteger64 _periph_config_id = emu_config_id_ * 100000000 + (i + 1) * 1000000;
      int crateID = TStore_allCrates[i]->CrateID();
      xdata::UnsignedShort _crateid = crateID;
      xdata::String _label = TStore_allCrates[i]->GetLabel();

#ifdef debugV
      std::cout << "-- PERIPHERALCRATE  crateid ------------- " << _crateid.toString() << std::endl;
      std::cout << "-- PERIPHERALCRATE  label --------------- " << _label.toString() << std::endl;
      std::cout << "-- PERIPHERALCRATE  emu_config_id ------- " << emu_config_id_.toString() << std::endl;
      std::cout << "-- PERIPHERALCRATE  periph_config_id ---- " << _periph_config_id.toString() << std::endl;
#endif

      newRows.clear();
      newRows = tableDefinitions[insertViewName];
      copyPeripheralCrateToTable(newRows, TStore_allCrates[i]);
      setConfigID(newRows, rowId, EMU_CONFIG_ID, emu_config_id_);
      setConfigID(newRows, rowId, PERIPH_CONFIG_ID, _periph_config_id);

      insert(connectionID, insertViewName, newRows);

      uploadCCB(connectionID, _periph_config_id, /*thisCCB*/crateID);
      uploadMPC(connectionID, _periph_config_id, /*thisMPC*/crateID);
      uploadVMECC(connectionID, _periph_config_id, /*TStore_allCrates[i]*/crateID);
      std::vector<Chamber *> allChambers = TStore_allCrates[i]->chambers();
      uploadCSC(connectionID, _periph_config_id, allChambers, crateID);
    } // end of if
  } // end of loop over TStore_myCrates[i]
}


void PCConfigEditor::copyCCBToTable(xdata::Table &newRows, Crate * TStore_thisCrate)
{
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

  CCB *TStore_thisCCB = TStore_thisCrate->ccb();
  if (TStore_thisCCB)
  {
    xdata::UnsignedShort _ccbmode = TStore_thisCCB->GetCCBmode();
    xdata::UnsignedShort _ccb_firmware_day = TStore_thisCCB->GetExpectedFirmwareDay();
    xdata::UnsignedShort _ccb_firmware_month = TStore_thisCCB->GetExpectedFirmwareMonth();
    xdata::UnsignedShort _ccb_firmware_year = TStore_thisCCB->GetExpectedFirmwareYear();
    xdata::UnsignedShort _l1adelay = TStore_thisCCB->Getl1adelay();
    xdata::UnsignedShort _ttcrxcoarsedelay = TStore_thisCCB->GetTTCrxCoarseDelay();
    xdata::UnsignedShort _ttcrxfinedelay = TStore_thisCCB->GetTTCrxFineDelay();
    xdata::UnsignedShort _ttcrxid = TStore_thisCCB->GetTTCrxID();

    newRows.setValueAt(rowId, CCBMODE, _ccbmode);
    newRows.setValueAt(rowId, CCB_FIRMWARE_DAY, _ccb_firmware_day);
    newRows.setValueAt(rowId, CCB_FIRMWARE_MONTH, _ccb_firmware_month);
    newRows.setValueAt(rowId, CCB_FIRMWARE_YEAR, _ccb_firmware_year);
    newRows.setValueAt(rowId, L1ADELAY, _l1adelay);
    newRows.setValueAt(rowId, TTCRXCOARSEDELAY, _ttcrxcoarsedelay);
    newRows.setValueAt(rowId, TTCRXFINEDELAY, _ttcrxfinedelay);
    newRows.setValueAt(rowId, TTCRXID, _ttcrxid);
  }
}


void PCConfigEditor::uploadCCB(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &periph_config_id,
    int crateID) throw (xcept::Exception)
{
  std::string insertViewName = "ccb";
  std::string CCB_CONFIG_ID("CCB_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

  size_t rowId(0);

  xdata::UnsignedInteger64 _ccb_config_id = periph_config_id + 110000;

  xdata::Table &newRows = getCachedTable(/*,TStore_thisCrate*/insertViewName/*,_vcc_config_id*/, crateID);
  std::cout << "upload type of CCB_CONFIG_ID" << newRows.getColumnType("CCB_CONFIG_ID") << std::endl;

  setConfigID(newRows, rowId, CCB_CONFIG_ID, _ccb_config_id);
  setConfigID(newRows, rowId, EMU_CONFIG_ID, emu_config_id_);
  setConfigID(newRows, rowId, PERIPH_CONFIG_ID, periph_config_id);

  std::cout << "set emu_config_id to " << emu_config_id_ << std::endl;
  std::cout << "emu_config_id is " << newRows.getValueAt(rowId, EMU_CONFIG_ID) << std::endl;

  insert(connectionID, insertViewName, newRows);
}


void PCConfigEditor::copyMPCToTable(xdata::Table &newRows, Crate * TStore_thisCrate)
{
  size_t rowId(0);

  MPC * TStore_thisMPC = TStore_thisCrate->mpc();
  std::string MPC_FIRMWARE_DAY("MPC_FIRMWARE_DAY");
  std::string MPC_FIRMWARE_MONTH("MPC_FIRMWARE_MONTH");
  std::string MPC_FIRMWARE_YEAR("MPC_FIRMWARE_YEAR");
  std::string SERIALIZERMODE("SERIALIZERMODE");
  std::string TRANSPARENTMODE("TRANSPARENTMODE");

  xdata::UnsignedShort _mpc_firmware_day = TStore_thisMPC->GetExpectedFirmwareDay();
  xdata::UnsignedShort _mpc_firmware_month = TStore_thisMPC->GetExpectedFirmwareMonth();
  xdata::UnsignedShort _mpc_firmware_year = TStore_thisMPC->GetExpectedFirmwareYear();
  xdata::UnsignedShort _serializermode = TStore_thisMPC->GetSerializerMode();
  xdata::UnsignedShort _transparentmode = TStore_thisMPC->GetTransparentMode();

  newRows = tableDefinitions["mpc"];
  newRows.setValueAt(rowId, MPC_FIRMWARE_DAY, _mpc_firmware_day);
  newRows.setValueAt(rowId, MPC_FIRMWARE_MONTH, _mpc_firmware_month);
  newRows.setValueAt(rowId, MPC_FIRMWARE_YEAR, _mpc_firmware_year);
  newRows.setValueAt(rowId, SERIALIZERMODE, _serializermode);
  newRows.setValueAt(rowId, TRANSPARENTMODE, _transparentmode);
}


void PCConfigEditor::uploadMPC(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &periph_config_id,
    /*MPC * TStore_thisMPC*/
    int crateID) throw (xcept::Exception)
{
  size_t rowId(0);
  std::string insertViewName = "mpc";
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string MPC_CONFIG_ID("MPC_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");

  xdata::UnsignedInteger64 _mpc_config_id = periph_config_id + 120000;

  xdata::Table &newRows = getCachedTable(insertViewName, crateID);

  setConfigID(newRows, rowId, EMU_CONFIG_ID, emu_config_id_);
  setConfigID(newRows, rowId, MPC_CONFIG_ID, _mpc_config_id);
  setConfigID(newRows, rowId, PERIPH_CONFIG_ID, periph_config_id);

  insert(connectionID, insertViewName, newRows);
}


void PCConfigEditor::copyVMECCToTable(xdata::Table &newRows, Crate * TStore_thisCrate)
{
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
//	   std::cout << "hek" << std::endl;
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
    //std::cout << " got here " << std::endl;
  }
  catch (std::exception &e)
  {
  	XCEPT_RAISE(xcept::Exception,e.what());
  }
}


xdata::Table &PCConfigEditor::getCachedDiff(
    const std::string &insertViewName,
    int crateID
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  std::string identifier = crateIdentifierString(crateID);
  return emu::db::ConfigEditor::getCachedDiff(insertViewName, identifier);
}


xdata::Table &PCConfigEditor::getCachedTable(
    const std::string &insertViewName,
    int crateID
    /*,xdata::UnsignedInteger64 &_vcc_config_id*//*,Crate *thisCrate*/) throw (xcept::Exception)
{
  std::string identifier = crateIdentifierString(crateID) + " ";
  return emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);
}


void PCConfigEditor::uploadVMECC(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &periph_config_id,
    /*Crate * TStore_thisCrate*/
    int crateID) throw (xcept::Exception)
{
  std::string insertViewName = "vcc";
  size_t rowId(0);
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string VCC_CONFIG_ID("VCC_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  //copyVMECCToTable(newRows,TStore_thisCrate);
  xdata::UnsignedInteger64 _vcc_config_id = periph_config_id + 130000;
  xdata::Table &newRows = getCachedTable(insertViewName, crateID);

  //std::cout << "newRows.getColumnType(PERIPH_CONFIG_ID)=" << newRows.getColumnType(PERIPH_CONFIG_ID) << std::endl;
  setConfigID(newRows, rowId, PERIPH_CONFIG_ID,  periph_config_id);
  //std::cout << "newRows.getColumnType(VCC_CONFIG_ID)=" << newRows.getColumnType(VCC_CONFIG_ID) << std::endl;
  setConfigID(newRows, rowId, VCC_CONFIG_ID,     _vcc_config_id);
  setConfigID(newRows, rowId, EMU_CONFIG_ID,     emu_config_id_);

  insert(connectionID, insertViewName, newRows);
}

void PCConfigEditor::copyCSCToTable(xdata::Table &newRows, Chamber * chamber)
{
  size_t rowId(0);
  newRows = tableDefinitions["csc"];
  std::string LABEL("LABEL");
  std::string KNOWN_PROBLEM("KNOWN_PROBLEM");
  std::string PROBLEM_MASK("PROBLEM_MASK");

  xdata::String _label;
  xdata::String _known_problem;
  xdata::UnsignedShort _problem_mask;

  _label         = chamber->GetLabel();
  _known_problem = chamber->GetProblemDescription();
  _problem_mask  = chamber->GetProblemMask();
  newRows.setValueAt(rowId, LABEL,          _label);
  newRows.setValueAt(rowId, KNOWN_PROBLEM,  _known_problem);
  newRows.setValueAt(rowId, PROBLEM_MASK,   _problem_mask);
}


std::string PCConfigEditor::chamberID(int crateID, const std::string &chamberLabel)
{
  return crateIdentifierString(crateID) + " chamber " + chamberLabel + " ";
}


std::string PCConfigEditor::DAQMBID(const std::string &chamber, int slot)
{
  return /*chamberID(crateID,chamberLabel)*/chamber;//+"_"+to_string(slot);
}


void PCConfigEditor::uploadCSC(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &periph_config_id,
    const std::vector<Chamber *> &TStore_allChambers,
    int crateID) throw (xcept::Exception)
{
  std::string insertViewName = "csc";

  size_t rowId(0);
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string PERIPH_CONFIG_ID("PERIPH_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

  xdata::UnsignedInteger64 _csc_config_id;

  for (unsigned j = 0; j < TStore_allChambers.size(); ++j)
  {
    if (TStore_allChambers[j])
    {
      _csc_config_id = periph_config_id + (j + 1) * 10000;
      std::string chamber = chamberID(crateID, TStore_allChambers[j]->GetLabel());
      xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, chamber);

      setConfigID(newRows, rowId, CSC_CONFIG_ID,  _csc_config_id);
      newRows.setValueAt(rowId, EMU_CONFIG_ID,    emu_config_id_);
      newRows.setValueAt(rowId, PERIPH_CONFIG_ID, periph_config_id);

      insert(connectionID, insertViewName, newRows);

      DAQMB * thisDAQMB = TStore_allChambers[j]->GetDMB();
      uploadDAQMB(connectionID, _csc_config_id, thisDAQMB/*this parameter is to be removed*/, thisDAQMB->slot(), chamber);

      TMB * thisTMB = TStore_allChambers[j]->GetTMB();
      uploadTMB(connectionID, _csc_config_id, thisTMB, thisTMB->slot(), chamber);
    } // end of if
  } // end of loop over TStore_myChambers[j]
}


void PCConfigEditor::copyDAQMBToTable(xdata::Table &newRows, DAQMB * TStore_thisDAQMB)
{
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
  std::string POWER_MASK("POWER_MASK");
  
  xdata::UnsignedShort     _alct_dav_cable_delay    = TStore_thisDAQMB->GetAlctDavCableDelay(); 
  xdata::UnsignedShort     _calibration_l1acc_delay = TStore_thisDAQMB->GetCalibrationL1aDelay(); 
  xdata::UnsignedShort     _calibration_lct_delay   = TStore_thisDAQMB->GetCalibrationLctDelay(); 
  xdata::UnsignedShort     _cfeb_cable_delay        = TStore_thisDAQMB->GetCfebCableDelay(); 
  xdata::UnsignedShort     _cfeb_dav_cable_delay    = TStore_thisDAQMB->GetCfebDavCableDelay(); 
  xdata::UnsignedShort     _comp_mode               = TStore_thisDAQMB->GetCompMode(); 
  xdata::UnsignedShort     _comp_timing             = TStore_thisDAQMB->GetCompTiming(); 
  xdata::String            _dmb_cntl_firmware_tag;
  std::string valueInHex;
  emu::db::convertToHex(valueInHex,"%lx",TStore_thisDAQMB->GetExpectedControlFirmwareTag());
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
  xdata::UnsignedShort     _power_mask              = TStore_thisDAQMB->GetPowerMask();
  
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
  newRows.setValueAt(rowId, POWER_MASK,              _power_mask);
}


void PCConfigEditor::uploadDAQMB(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &csc_config_id,
    DAQMB * &TStore_thisDAQMB,
    /*int crateID,const std::string &chamberLabel,*/
    int slot,
    const std::string &chamber) throw (xcept::Exception)
{
  std::string insertViewName = "daqmb";
  std::string identifier = DAQMBID(chamber, slot);
  xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);

  size_t rowId(0);
  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

  xdata::UnsignedInteger64 _daqmb_config_id = csc_config_id + 1000;

  //copyDAQMBToTable(newRows,TStore_thisDAQMB);

  setConfigID(newRows, rowId, CSC_CONFIG_ID,    csc_config_id);
  setConfigID(newRows, rowId, DAQMB_CONFIG_ID,  _daqmb_config_id);
  setConfigID(newRows, rowId, EMU_CONFIG_ID,    emu_config_id_);

  insert(connectionID, insertViewName, newRows);

  uploadCFEB(connectionID, _daqmb_config_id, identifier);
}


void PCConfigEditor::copyCFEBToTable(xdata::Table &newRows, DAQMB * TStore_thisDAQMB)
{
  try
  {
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
    
    //was using TStore_thisDAQMB->cfebs();, but this means copying the whole thing,
    //which results in a bad_alloc if there is something uninitialised, making it harder to diagnose problems
    std::vector<CFEB> &TStore_allCFEBs = TStore_thisDAQMB->cfebs_;
    for (unsigned j = 0; j < TStore_allCFEBs.size(); ++j)
    {
      xdata::UnsignedShort _cfeb_number = TStore_allCFEBs[j].number();
      emu::db::convertToHex(valueInHex, "%lx", TStore_thisDAQMB->GetExpectedCFEBFirmwareTag(_cfeb_number));
      xdata::String _cfeb_firmware_tag = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 0));
      xdata::String _kill_chip0 = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 1));
      xdata::String _kill_chip1 = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 2));
      xdata::String _kill_chip2 = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 3));
      xdata::String _kill_chip3 = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 4));
      xdata::String _kill_chip4 = valueInHex;
      emu::db::convertToHex(valueInHex, "%05x", TStore_thisDAQMB->GetKillChip(_cfeb_number, 5));
      xdata::String _kill_chip5 = valueInHex;

#ifdef debugV
      std::cout << "-- CFEB cfeb_number ------------ " << _cfeb_number.toString() << std::endl;
      std::cout << "-- CFEB cfeb_firmware_tag ------ " << _cfeb_firmware_tag.toString() << std::endl;
      std::cout << "-- CFEB kill_chip0 ------------- " << _kill_chip0.toString() << std::endl;
      std::cout << "-- CFEB kill_chip1 ------------- " << _kill_chip1.toString() << std::endl;
      std::cout << "-- CFEB kill_chip2 ------------- " << _kill_chip2.toString() << std::endl;
      std::cout << "-- CFEB kill_chip3 ------------- " << _kill_chip3.toString() << std::endl;
      std::cout << "-- CFEB kill_chip4 ------------- " << _kill_chip4.toString() << std::endl;
      std::cout << "-- CFEB kill_chip5 ------------- " << _kill_chip5.toString() << std::endl;
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
  catch (xdata::exception::Exception &e)
  {
    //std::cout << e.what() << std::endl;
    XCEPT_RETHROW(xgi::exception::Exception,"could not copy to TMB",e);
  }
  catch (xcept::Exception &e)
  {
    //std::cout << e.what() << std::endl;
    XCEPT_RETHROW(xgi::exception::Exception,"could not copy to TMB",e);
  }
  catch (std::exception &e)
  {
    std::cout << "std::ex" << e.what() << std::endl;
    //XCEPT_RETHROW(xgi::exception::Exception,"could not copy to TMB",e);
  }
}


void PCConfigEditor::uploadCFEB(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &daqmb_config_id,
    const std::string &identifier) throw (xcept::Exception)
{
  std::string insertViewName = "cfeb";
  xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);
  std::string valueInHex;

  size_t rowId(0);
  std::string CFEB_CONFIG_ID("CFEB_CONFIG_ID");
  std::string DAQMB_CONFIG_ID("DAQMB_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");

  for (unsigned rowIndex = 0; rowIndex < newRows.getRowCount(); rowIndex++)
  {
    xdata::UnsignedShort *_cfeb_number =
        dynamic_cast<xdata::UnsignedShort *> (newRows.getValueAt(rowIndex, "CFEB_NUMBER"));
        //= TStore_allCFEBs[j].number();
    if (_cfeb_number)
    {
      xdata::UnsignedInteger64 _cfeb_config_id = daqmb_config_id + *_cfeb_number;
#ifdef debugV
      std::cout << "-- CFEB emu_config_id ---------- " << emu_config_id_.toString() << std::endl;
      std::cout << "-- CFEB periph_config_id ------- " << daqmb_config_id.toString() << std::endl;
      std::cout << "-- CFEB cfeb_config_id --------- " << _cfeb_config_id.toString() << std::endl;
#endif

      setConfigID(newRows, rowId, CFEB_CONFIG_ID,   _cfeb_config_id);
      setConfigID(newRows, rowId, DAQMB_CONFIG_ID,  daqmb_config_id);
      setConfigID(newRows, rowId, EMU_CONFIG_ID,    emu_config_id_);
    }
    rowId++;
  }
  insert(connectionID, insertViewName, newRows);
}


void PCConfigEditor::copyTMBToTable(xdata::Table &newRows, TMB * TStore_thisTMB)
{
  try
  {
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
    std::string ALCT_POSNEG("ALCT_POSNEG");
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
    std::string ALCT_TX_POSNEG("ALCT_TX_POSNEG");
    std::string CFEB0POSNEG("CFEB0POSNEG");
    std::string CFEB1POSNEG("CFEB1POSNEG");
    std::string CFEB2POSNEG("CFEB2POSNEG");
    std::string CFEB3POSNEG("CFEB3POSNEG");
    std::string CFEB4POSNEG("CFEB4POSNEG");
    std::string MPC_SEL_TTC_BX0("MPC_SEL_TTC_BX0");
    std::string ALCT_TOF_DELAY("ALCT_TOF_DELAY");
    std::string TMB_TO_ALCT_DATA_DELAY("TMB_TO_ALCT_DATA_DELAY");
    std::string CFEB_TOF_DELAY("CFEB_TOF_DELAY");
    std::string CFEB0_TOF_DELAY("CFEB0_TOF_DELAY");
    std::string CFEB1_TOF_DELAY("CFEB1_TOF_DELAY");
    std::string CFEB2_TOF_DELAY("CFEB2_TOF_DELAY");
    std::string CFEB3_TOF_DELAY("CFEB3_TOF_DELAY");
    std::string CFEB4_TOF_DELAY("CFEB4_TOF_DELAY");
    std::string CFEB_BADBITS_BLOCK("CFEB_BADBITS_BLOCK");
    std::string CFEB0_RXD_INT_DELAY("CFEB0_RXD_INT_DELAY");
    std::string CFEB1_RXD_INT_DELAY("CFEB1_RXD_INT_DELAY");
    std::string CFEB2_RXD_INT_DELAY("CFEB2_RXD_INT_DELAY");
    std::string CFEB3_RXD_INT_DELAY("CFEB3_RXD_INT_DELAY");
    std::string CFEB4_RXD_INT_DELAY("CFEB4_RXD_INT_DELAY");
    std::string CFEB_BADBITS_READOUT("CFEB_BADBITS_READOUT");
    std::string L1A_PRIORITY_ENABLE("L1A_PRIORITY_ENABLE");
    std::string MINISCOPE_ENABLE("MINISCOPE_ENABLE");
    xdata::UnsignedShort  _adjacent_cfeb_distance      = TStore_thisTMB->GetAdjacentCfebDistance();
    xdata::UnsignedShort  _aff_thresh                  = TStore_thisTMB->GetActiveFebFlagThresh();
    xdata::UnsignedShort  _alct_bx0_delay              = TStore_thisTMB->GetAlctBx0Delay();
    xdata::UnsignedShort  _alct_bx0_enable             = TStore_thisTMB->GetAlctBx0Enable();
    xdata::UnsignedShort  _alct_clear                  = TStore_thisTMB->GetAlctClear();
    xdata::UnsignedShort  _alct_clock_en_use_ccb       = TStore_thisTMB->GetEnableAlctUseCcbClock();
    xdata::UnsignedShort  _alct_pretrig_enable         = TStore_thisTMB->GetAlctPatternTrigEnable();
    xdata::UnsignedShort  _alct_readout_without_trig   = TStore_thisTMB->GetAllowAlctNontrigReadout();
    xdata::UnsignedShort  _alct_rx_clock_delay         = TStore_thisTMB->GetALCTrxPhase();
    xdata::UnsignedShort  _alct_trig_enable            = TStore_thisTMB->GetTmbAllowAlct();
    xdata::UnsignedShort  _alct_tx_clock_delay         = TStore_thisTMB->GetALCTtxPhase();
    xdata::UnsignedShort  _alct_posneg		            = TStore_thisTMB->GetAlctPosNeg();
    xdata::UnsignedShort  _all_cfeb_active             = TStore_thisTMB->GetEnableAllCfebsActive();
    xdata::UnsignedShort  _cfeb0delay                  = TStore_thisTMB->GetCFEB0delay();
    xdata::UnsignedShort  _cfeb1delay                  = TStore_thisTMB->GetCFEB1delay();
    xdata::UnsignedShort  _cfeb2delay                  = TStore_thisTMB->GetCFEB2delay();
    xdata::UnsignedShort  _cfeb3delay                  = TStore_thisTMB->GetCFEB3delay();
    xdata::UnsignedShort  _cfeb4delay                  = TStore_thisTMB->GetCFEB4delay();
    xdata::UnsignedShort  _cfeb_enable_source          = TStore_thisTMB->GetCfebEnableSource_orig();
    xdata::UnsignedShort  _clct_blanking               = TStore_thisTMB->GetClctBlanking();
    xdata::UnsignedShort  _clct_bx0_delay              = TStore_thisTMB->GetClctBx0Delay();
    xdata::UnsignedShort  _clct_drift_delay            = TStore_thisTMB->GetDriftDelay();
    xdata::UnsignedShort  _clct_ext_pretrig_enable     = TStore_thisTMB->GetClctExtTrigEnable();
    xdata::UnsignedShort  _clct_fifo_pretrig           = TStore_thisTMB->GetFifoPreTrig();
    xdata::UnsignedShort  _clct_fifo_tbins             = TStore_thisTMB->GetFifoTbins();
    xdata::UnsignedShort  _clct_hit_persist            = TStore_thisTMB->GetTriadPersistence();
    xdata::UnsignedShort  _clct_min_separation         = TStore_thisTMB->GetMinClctSeparation();
    xdata::UnsignedShort  _clct_nplanes_hit_pattern    = TStore_thisTMB->GetMinHitsPattern();
    xdata::UnsignedShort  _clct_nplanes_hit_pretrig    = TStore_thisTMB->GetHsPretrigThresh();
    xdata::UnsignedShort  _clct_pid_thresh_pretrig     = TStore_thisTMB->GetClctPatternIdThresh();
    xdata::UnsignedShort  _clct_pretrig_enable         = TStore_thisTMB->GetClctPatternTrigEnable();
    xdata::UnsignedShort  _clct_readout_without_trig   = TStore_thisTMB->GetAllowClctNontrigReadout();
    //xdata::UnsignedShort  _clct_stagger                = TStore_thisTMB->GetClctStagger();  // obsolete
    xdata::UnsignedShort  _clct_throttle               = TStore_thisTMB->GetClctThrottle();
    xdata::UnsignedShort  _clct_trig_enable            = TStore_thisTMB->GetTmbAllowClct();
    xdata::UnsignedShort  _dmb_tx_delay                = TStore_thisTMB->GetDmbTxDelay();
    xdata::UnsignedShort  _enable_alct_rx              = TStore_thisTMB->GetAlctInput();
    xdata::UnsignedShort  _enable_alct_tx              = TStore_thisTMB->GetEnableAlctTx();
    xdata::UnsignedShort  _enableclctinputs_reg42      = TStore_thisTMB->GetEnableCLCTInputs();
    xdata::UnsignedShort  _enableclctinputs_reg68      = TStore_thisTMB->GetCfebEnable();
    xdata::UnsignedShort  _ignore_ccb_startstop        = TStore_thisTMB->GetIgnoreCcbStartStop();
    xdata::UnsignedShort  _l1a_allow_alct_only         = TStore_thisTMB->GetL1aAllowAlctOnly();
    xdata::UnsignedShort  _l1a_allow_match             = TStore_thisTMB->GetL1aAllowMatch();
    xdata::UnsignedShort  _l1a_allow_nol1a             = TStore_thisTMB->GetL1aAllowNoL1a();
    xdata::UnsignedShort  _l1a_allow_notmb             = TStore_thisTMB->GetL1aAllowNoTmb();

    std::string valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(0));
    xdata::String _layer0_distrip_hot_chann_mask = valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(1));
    xdata::String _layer1_distrip_hot_chann_mask = valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(2));
    xdata::String _layer2_distrip_hot_chann_mask = valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(3));
    xdata::String _layer3_distrip_hot_chann_mask = valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(4));
    xdata::String _layer4_distrip_hot_chann_mask = valueInHex;
    emu::db::convertToHex(valueInHex, "%Lx", TStore_thisTMB->GetDistripHotChannelMask(5));
    xdata::String _layer5_distrip_hot_chann_mask = valueInHex;

    //xdata::UnsignedShort  _layer_trig_delay            = TStore_thisTMB->GetLayerTrigDelay();   // obsolete
    xdata::UnsignedShort  _layer_trig_enable           = TStore_thisTMB->GetEnableLayerTrigger();
    xdata::UnsignedShort  _layer_trig_thresh           = TStore_thisTMB->GetLayerTriggerThreshold();
    xdata::UnsignedShort  _match_pretrig_alct_delay    = TStore_thisTMB->GetAlctPretrigDelay();
    xdata::UnsignedShort  _match_pretrig_enable        = TStore_thisTMB->GetMatchPatternTrigEnable();
    xdata::UnsignedShort  _match_pretrig_window_size   = TStore_thisTMB->GetAlctClctPretrigWidth();
    xdata::UnsignedShort  _match_readout_without_trig  = TStore_thisTMB->GetAllowMatchNontrigReadout();
    xdata::UnsignedShort  _match_trig_alct_delay       = TStore_thisTMB->GetAlctVpfDelay();
    xdata::UnsignedShort  _match_trig_enable           = TStore_thisTMB->GetTmbAllowMatch();
    xdata::UnsignedShort  _match_trig_window_size      = TStore_thisTMB->GetAlctMatchWindowSize();
    xdata::UnsignedShort  _mpc_block_me1a              = TStore_thisTMB->GetBlockME1aToMPC();
    xdata::UnsignedShort  _mpc_idle_blank              = TStore_thisTMB->GetMpcIdleBlank();
    xdata::UnsignedShort  _mpc_output_enable           = TStore_thisTMB->GetMpcOutputEnable();
    xdata::UnsignedShort  _mpc_rx_delay                = TStore_thisTMB->GetMpcRxDelay();
    xdata::UnsignedShort  _mpc_sync_err_enable         = TStore_thisTMB->GetTmbSyncErrEnable();
    xdata::UnsignedShort  _mpc_tx_delay                = TStore_thisTMB->GetMpcTxDelay();
    xdata::UnsignedShort  _rat_firmware_day            = TStore_thisTMB->GetExpectedRatFirmwareDay();
    xdata::UnsignedShort  _rat_firmware_month          = TStore_thisTMB->GetExpectedRatFirmwareMonth();
    xdata::UnsignedShort  _rat_firmware_year           = TStore_thisTMB->GetExpectedRatFirmwareYear();
    xdata::UnsignedShort  _rat_tmb_delay               = TStore_thisTMB->GetRatTmbDelay();
    xdata::UnsignedShort  _request_l1a                 = TStore_thisTMB->GetRequestL1a();
    xdata::UnsignedShort  _rpc0_rat_delay              = TStore_thisTMB->GetRpc0RatDelay();
    xdata::UnsignedShort  _rpc0_raw_delay              = TStore_thisTMB->GetRpc0RawDelay();
    xdata::UnsignedShort  _rpc1_rat_delay              = TStore_thisTMB->GetRpc1RatDelay();
    xdata::UnsignedShort  _rpc1_raw_delay              = TStore_thisTMB->GetRpc1RawDelay();
    xdata::UnsignedShort  _rpc_bxn_offset              = TStore_thisTMB->GetRpcBxnOffset();
    xdata::UnsignedShort  _rpc_exists                  = TStore_thisTMB->GetRpcExist();
    xdata::UnsignedShort  _rpc_fifo_decouple           = TStore_thisTMB->GetRpcDecoupleTbins();
    xdata::UnsignedShort  _rpc_pretrig                 = TStore_thisTMB->GetFifoPretrigRpc();
    xdata::UnsignedShort  _rpc_tbins                   = TStore_thisTMB->GetFifoTbinsRpc();
    xdata::UnsignedShort  _rpc_mask_all                = TStore_thisTMB->GetEnableRpcInput();
    xdata::UnsignedShort  _rpc_read_enable             = TStore_thisTMB->GetRpcReadEnable();
    xdata::UnsignedShort  _slot                        = TStore_thisTMB->slot();
    xdata::UnsignedShort  _tmb_bxn_offset              = TStore_thisTMB->GetBxnOffset();
    xdata::UnsignedShort  _tmb_fifo_mode               = TStore_thisTMB->GetFifoMode();
    xdata::UnsignedShort  _tmb_fifo_no_raw_hits        = TStore_thisTMB->GetFifoNoRawHits();
    emu::db::convertToHex(valueInHex,"%x",TStore_thisTMB->GetTMBFirmwareCompileType());
    xdata::String         _tmb_firmware_compile_type     = valueInHex;
    xdata::UnsignedShort  _tmb_firmware_day              = TStore_thisTMB->GetExpectedTmbFirmwareDay();
    xdata::UnsignedShort  _tmb_firmware_month            = TStore_thisTMB->GetExpectedTmbFirmwareMonth();
    xdata::UnsignedShort  _tmb_firmware_revcode          = TStore_thisTMB->GetExpectedTmbFirmwareRevcode();
    xdata::UnsignedShort  _tmb_firmware_type             = TStore_thisTMB->GetExpectedTmbFirmwareType();
    xdata::UnsignedShort  _tmb_firmware_version          = TStore_thisTMB->GetExpectedTmbFirmwareVersion();
    xdata::UnsignedShort  _tmb_firmware_year             = TStore_thisTMB->GetExpectedTmbFirmwareYear();
    xdata::UnsignedShort  _tmb_l1a_delay                 = TStore_thisTMB->GetL1aDelay();
    xdata::UnsignedShort  _tmb_l1a_offset                = TStore_thisTMB->GetL1aOffset();
    xdata::UnsignedShort  _tmb_l1a_window_size           = TStore_thisTMB->GetL1aWindowSize();
    xdata::UnsignedShort  _valid_clct_required           = TStore_thisTMB->GetRequireValidClct();
    xdata::UnsignedShort  _write_buffer_required         = TStore_thisTMB->GetWriteBufferRequired();
    xdata::UnsignedShort  _write_buffer_autoclear        = TStore_thisTMB->GetWriteBufferAutoclear();
    xdata::UnsignedShort  _write_buffer_continous_enable = TStore_thisTMB->GetClctWriteContinuousEnable();
    xdata::UnsignedShort  _alct_tx_posneg                =  TStore_thisTMB->GetAlctTxPosNeg();
    xdata::UnsignedShort  _cfeb0posneg                   = TStore_thisTMB->GetCfeb0RxPosNeg();
    xdata::UnsignedShort  _cfeb1posneg                   = TStore_thisTMB->GetCfeb1RxPosNeg();
    xdata::UnsignedShort  _cfeb2posneg                   = TStore_thisTMB->GetCfeb2RxPosNeg();
    xdata::UnsignedShort  _cfeb3posneg                   = TStore_thisTMB->GetCfeb3RxPosNeg();
    xdata::UnsignedShort  _cfeb4posneg                   = TStore_thisTMB->GetCfeb4RxPosNeg();
    xdata::UnsignedShort  _mpc_sel_ttc_bx0               = TStore_thisTMB->GetSelectMpcTtcBx0();
    xdata::UnsignedShort  _alct_tof_delay                = TStore_thisTMB->GetAlctTOFDelay();
    xdata::UnsignedShort  _tmb_to_alct_data_delay        = TStore_thisTMB->GetALCTTxDataDelay();
    xdata::UnsignedShort  _cfeb_tof_delay                = TStore_thisTMB->GetCfebTOFDelay();
    xdata::UnsignedShort  _cfeb0_tof_delay               = TStore_thisTMB->GetCfeb0TOFDelay();
    xdata::UnsignedShort  _cfeb1_tof_delay               = TStore_thisTMB->GetCfeb1TOFDelay();
    xdata::UnsignedShort  _cfeb2_tof_delay               = TStore_thisTMB->GetCfeb2TOFDelay();
    xdata::UnsignedShort  _cfeb3_tof_delay               = TStore_thisTMB->GetCfeb3TOFDelay();
    xdata::UnsignedShort  _cfeb4_tof_delay               = TStore_thisTMB->GetCfeb4TOFDelay();
    xdata::UnsignedShort  _cfeb_badbits_block            = TStore_thisTMB->GetCFEBBadBitsBlock();
    xdata::UnsignedShort  _cfeb0_rxd_int_delay           = TStore_thisTMB->GetCFEB0RxdIntDelay();
    xdata::UnsignedShort  _cfeb1_rxd_int_delay           = TStore_thisTMB->GetCFEB1RxdIntDelay();
    xdata::UnsignedShort  _cfeb2_rxd_int_delay           = TStore_thisTMB->GetCFEB2RxdIntDelay();
    xdata::UnsignedShort  _cfeb3_rxd_int_delay           = TStore_thisTMB->GetCFEB3RxdIntDelay();
    xdata::UnsignedShort  _cfeb4_rxd_int_delay           = TStore_thisTMB->GetCFEB4RxdIntDelay();
    xdata::UnsignedShort  _cfeb_badbits_readout          = TStore_thisTMB->GetCFEBBadBitsReadout();
    xdata::UnsignedShort  _l1a_priority_enable           = TStore_thisTMB->GetL1APriorityEnable();
    xdata::UnsignedShort  _miniscope_enable              = TStore_thisTMB->GetMiniscopeEnable();

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
    newRows.setValueAt(rowId, ALCT_POSNEG,                   _alct_posneg);
    newRows.setValueAt(rowId, ALCT_TX_POSNEG,                _alct_tx_posneg);
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
  
    newRows.setValueAt(rowId, CFEB0POSNEG,                   _cfeb0posneg);
    newRows.setValueAt(rowId, CFEB1POSNEG,                   _cfeb1posneg);
    newRows.setValueAt(rowId, CFEB2POSNEG,                   _cfeb2posneg);
    newRows.setValueAt(rowId, CFEB3POSNEG,                   _cfeb3posneg);
    newRows.setValueAt(rowId, CFEB4POSNEG,                   _cfeb4posneg);
    newRows.setValueAt(rowId, MPC_SEL_TTC_BX0,               _mpc_sel_ttc_bx0);
    newRows.setValueAt(rowId, ALCT_TOF_DELAY,                _alct_tof_delay);
    newRows.setValueAt(rowId, TMB_TO_ALCT_DATA_DELAY,        _tmb_to_alct_data_delay);
    newRows.setValueAt(rowId, CFEB_TOF_DELAY,                _cfeb_tof_delay);
    newRows.setValueAt(rowId, CFEB0_TOF_DELAY,               _cfeb0_tof_delay);
    newRows.setValueAt(rowId, CFEB1_TOF_DELAY,               _cfeb1_tof_delay);
    newRows.setValueAt(rowId, CFEB2_TOF_DELAY,               _cfeb2_tof_delay);
    newRows.setValueAt(rowId, CFEB3_TOF_DELAY,               _cfeb3_tof_delay);
    newRows.setValueAt(rowId, CFEB4_TOF_DELAY,               _cfeb4_tof_delay);
    newRows.setValueAt(rowId, CFEB_BADBITS_BLOCK,            _cfeb_badbits_block);
    newRows.setValueAt(rowId, CFEB0_RXD_INT_DELAY,           _cfeb0_rxd_int_delay);
    newRows.setValueAt(rowId, CFEB1_RXD_INT_DELAY,           _cfeb1_rxd_int_delay);
    newRows.setValueAt(rowId, CFEB2_RXD_INT_DELAY,           _cfeb2_rxd_int_delay);
    newRows.setValueAt(rowId, CFEB3_RXD_INT_DELAY,           _cfeb3_rxd_int_delay);
    newRows.setValueAt(rowId, CFEB4_RXD_INT_DELAY,           _cfeb4_rxd_int_delay);
    newRows.setValueAt(rowId, CFEB_BADBITS_READOUT,          _cfeb_badbits_readout);
    newRows.setValueAt(rowId, L1A_PRIORITY_ENABLE,           _l1a_priority_enable);
    newRows.setValueAt(rowId, MINISCOPE_ENABLE,              _miniscope_enable);
  }
  catch (xcept::Exception &e)
  {
    std::cout << e.what() << std::endl;
    XCEPT_RETHROW(xgi::exception::Exception,"could not copy to TMB",e);
  }
  catch (std::exception &e)
  {
    std::cout << "std::ex" << e.what() << std::endl;
    //XCEPT_RETHROW(xgi::exception::Exception,"could not copy to TMB",e);
  }
}


void PCConfigEditor::uploadTMB(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &csc_config_id,
    TMB * &TStore_thisTMB,
    int slot,
    const std::string &chamber) throw (xcept::Exception)
{
  std::string insertViewName = "tmb";
  std::string identifier = DAQMBID(chamber, slot);
  xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);
  std::string valueInHex;

  size_t rowId(0);

  std::string CSC_CONFIG_ID("CSC_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");
  xdata::UnsignedInteger64 _tmb_config_id   = csc_config_id + 2000;

  setConfigID(newRows, rowId, CSC_CONFIG_ID,  csc_config_id);
  setConfigID(newRows, rowId, EMU_CONFIG_ID,  emu_config_id_);
  setConfigID(newRows, rowId, TMB_CONFIG_ID,  _tmb_config_id);

  insert(connectionID,insertViewName,newRows);

  ALCTController * thisALCT = TStore_thisTMB->alctController();
  uploadALCT(connectionID, _tmb_config_id, thisALCT,identifier);
}


void PCConfigEditor::copyALCTToTable(xdata::Table &newRows, ALCTController * &TStore_thisALCT)
{
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
  std::string ALCT_ZERO_SUPPRESS("ALCT_ZERO_SUPPRESS");
  std::string CHAMBER_TYPE("CHAMBER_TYPE");
  
  xdata::UnsignedShort _alct_accel_mode                = TStore_thisALCT->GetWriteAlctAmode();
  xdata::UnsignedShort _alct_nplanes_hit_accel_pattern = TStore_thisALCT->GetWriteAcceleratorPatternThresh();
  xdata::UnsignedShort _alct_nplanes_hit_accel_pretrig = TStore_thisALCT->GetWriteAcceleratorPretrigThresh();
  xdata::UnsignedShort _alct_bxn_offset                = TStore_thisALCT->GetWriteBxcOffset();
  xdata::UnsignedShort _alct_ccb_enable                = TStore_thisALCT->GetWriteCcbEnable();
  xdata::UnsignedShort _alct_config_in_readout         = TStore_thisALCT->GetWriteConfigInReadout();
  xdata::UnsignedShort _alct_drift_delay               = TStore_thisALCT->GetWriteDriftDelay();
  xdata::UnsignedShort _alct_ext_trig_enable           = TStore_thisALCT->GetWriteExtTrigEnable();
  xdata::UnsignedShort _alct_fifo_mode                 = TStore_thisALCT->GetWriteFifoMode();
  xdata::UnsignedShort _alct_fifo_pretrig              = TStore_thisALCT->GetWriteFifoPretrig();
  xdata::UnsignedShort _alct_fifo_tbins                = TStore_thisALCT->GetWriteFifoTbins();
  xdata::String        _alct_firmware_backward_forward = TStore_thisALCT->Get_fastcontrol_backward_forward_type();
  xdata::UnsignedShort _alct_firmware_day              = TStore_thisALCT->GetExpectedFastControlDay();
  xdata::UnsignedShort _alct_firmware_month            = TStore_thisALCT->GetExpectedFastControlMonth();
  xdata::String        _alct_firmware_negat_posit      = TStore_thisALCT->Get_fastcontrol_negative_positive_type();
  xdata::UnsignedShort _alct_firmware_year             = TStore_thisALCT->GetExpectedFastControlYear();
  xdata::UnsignedShort _alct_inject_mode               = TStore_thisALCT->GetWriteInjectMode();
  xdata::UnsignedShort _alct_l1a_delay                 = TStore_thisALCT->GetWriteL1aDelay();
  xdata::UnsignedShort _alct_l1a_internal              = TStore_thisALCT->GetWriteL1aInternal();
  xdata::UnsignedShort _alct_l1a_offset                = TStore_thisALCT->GetWriteL1aOffset();
  xdata::UnsignedShort _alct_l1a_window_width          = TStore_thisALCT->GetWriteL1aWindowSize();
  xdata::UnsignedShort _alct_nplanes_hit_pattern       = TStore_thisALCT->GetWritePretrigNumberOfPattern();
  xdata::UnsignedShort _alct_nplanes_hit_pretrig       = TStore_thisALCT->GetWritePretrigNumberOfLayers();
  xdata::UnsignedShort _alct_send_empty                = TStore_thisALCT->GetWriteSendEmpty();
  xdata::UnsignedShort _alct_sn_select                 = TStore_thisALCT->GetWriteSnSelect();
  xdata::UnsignedShort _alct_testpulse_amplitude       = TStore_thisALCT->GetTestpulseAmplitude();
  xdata::String        _alct_testpulse_direction       = TStore_thisALCT->Get_PulseDirection();
  xdata::String        _alct_testpulse_invert          = TStore_thisALCT->Get_InvertPulse();
  //std::cout << "ALCT_TESTPULSE_INVERT read: " << _alct_testpulse_invert << std::endl;
  xdata::UnsignedShort _alct_trig_info_en              = TStore_thisALCT->GetWriteTriggerInfoEnable();
  xdata::UnsignedShort _alct_trig_mode                 = TStore_thisALCT->GetWriteTriggerMode();
  xdata::UnsignedShort _alct_zero_suppress             = TStore_thisALCT->GetWriteAlctZeroSuppress();
  xdata::String        _chamber_type                   = TStore_thisALCT->GetChamberType();
  
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
  newRows.setValueAt(rowId, ALCT_ZERO_SUPPRESS,             _alct_zero_suppress);
  newRows.setValueAt(rowId, CHAMBER_TYPE,                   _chamber_type);
}


void PCConfigEditor::uploadALCT(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &tmb_config_id,
    ALCTController * &TStore_thisALCT,
    const std::string &identifier) throw (xcept::Exception)
{
  std::string insertViewName = "alct";
  xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);

  size_t rowId(0);
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  std::string TMB_CONFIG_ID("TMB_CONFIG_ID");

  xdata::UnsignedInteger64 _alct_config_id = tmb_config_id + 100;

#ifdef debugV
  std::cout << "-- ALCT emu_config_id --------------------- " << emu_config_id_.toString()  << std::endl;
  std::cout << "-- ALCT tmb_config_id --------------------- " << tmb_config_id.toString()   << std::endl;
  std::cout << "-- ALCT alct_config_id -------------------- " << _alct_config_id.toString() << std::endl;
#endif

  setConfigID(newRows, rowId, ALCT_CONFIG_ID,  _alct_config_id);
  setConfigID(newRows, rowId, EMU_CONFIG_ID,   emu_config_id_);
  setConfigID(newRows, rowId, TMB_CONFIG_ID,   tmb_config_id);

  insert(connectionID, insertViewName, newRows);

  uploadAnodeChannel(connectionID, _alct_config_id, TStore_thisALCT, identifier);
}


void PCConfigEditor::copyAnodeChannelToTable(xdata::Table &newRows, ALCTController * &TStore_thisALCT)
{
  size_t rowId(0);
  std::string AFEB_FINE_DELAY("AFEB_FINE_DELAY");
  std::string AFEB_NUMBER("AFEB_NUMBER");
  std::string AFEB_THRESHOLD("AFEB_THRESHOLD");

  newRows = tableDefinitions["anodechannel"];
  int maxUserIndex = TStore_thisALCT->MaximumUserIndex();

  for (int afeb = 0; afeb <= maxUserIndex; afeb++)
  {
    xdata::UnsignedShort  _afeb_fine_delay = TStore_thisALCT->GetWriteAsicDelay(afeb);
    // in XML it starts with 1 but the setter function uses 'number-1'; this way the XML file and the DB are consistent
    xdata::UnsignedShort  _afeb_number     = afeb+1;
    xdata::UnsignedShort  _afeb_threshold  = TStore_thisALCT->GetAfebThresholdDAC(afeb);

#ifdef debugV
    std::cout << "-- AFEB afeb_fine_delay -------- " << _afeb_fine_delay.toString() << std::endl;
    std::cout << "-- AFEB afeb_number ------------ " << _afeb_number.toString()     << std::endl;
    std::cout << "-- AFEB afeb_threshold --------- " << _afeb_threshold.toString()  << std::endl;
#endif
    newRows.setValueAt(rowId, AFEB_FINE_DELAY, _afeb_fine_delay); 
    newRows.setValueAt(rowId, AFEB_NUMBER,     _afeb_number); 
    newRows.setValueAt(rowId, AFEB_THRESHOLD,  _afeb_threshold); 
    rowId++;
  }
}


void PCConfigEditor::uploadAnodeChannel(
    const std::string &connectionID,
    xdata::UnsignedInteger64 &alct_config_id,
    ALCTController * &TStore_thisALCT,
    const std::string &identifier) throw (xcept::Exception)
{
  std::string insertViewName = "anodechannel";
  xdata::Table &newRows = emu::db::ConfigEditor::getCachedTable(insertViewName, identifier);

  size_t rowId(0);
  std::string AFEB_CONFIG_ID("AFEB_CONFIG_ID");
  std::string ALCT_CONFIG_ID("ALCT_CONFIG_ID");
  std::string EMU_CONFIG_ID("EMU_CONFIG_ID");
  int maxUserIndex = TStore_thisALCT->MaximumUserIndex();

  for (int afeb = 0; afeb <= maxUserIndex; afeb++)
  {
    xdata::UnsignedShort     _afeb_fine_delay = TStore_thisALCT->GetWriteAsicDelay(afeb);
    // in XML it starts with 1 but the setter function uses 'number-1'; this way the XML file and the DB are consistent
    xdata::UnsignedShort     _afeb_number     = afeb+1;
    xdata::UnsignedShort     _afeb_threshold  = TStore_thisALCT->GetAfebThresholdDAC(afeb);
    xdata::UnsignedInteger64 _afeb_config_id  = alct_config_id + (afeb+1);

#ifdef debugV
    std::cout << "-- AFEB emu_config_id ---------- " << emu_config_id_.toString()   << std::endl;
    std::cout << "-- AFEB alct_config_id --------- " << alct_config_id.toString()   << std::endl;
    std::cout << "-- AFEB afeb_config_id --------- " << _afeb_config_id.toString()  << std::endl;
    std::cout << "-- AFEB afeb_fine_delay -------- " << _afeb_fine_delay.toString() << std::endl;
    std::cout << "-- AFEB afeb_number ------------ " << _afeb_number.toString()     << std::endl;
    std::cout << "-- AFEB afeb_threshold --------- " << _afeb_threshold.toString()  << std::endl;
#endif

    setConfigID(newRows,rowId, AFEB_CONFIG_ID,  _afeb_config_id);  
    setConfigID(newRows,rowId, ALCT_CONFIG_ID,  alct_config_id);
    setConfigID(newRows,rowId, EMU_CONFIG_ID,   emu_config_id_);
    rowId++;
  }
  insert(connectionID,insertViewName,newRows);
}


// ########################
// #    Retrieving data   #
// ########################


EmuEndcap * PCConfigEditor::getConfiguredEndcap(const std::string &emu_config_id) throw (xcept::Exception)
{
  try
  {
    EmuEndcap * endcap = new EmuEndcap();
    std::cout << "######## Empty EmuEndcap is created." << std::endl;
    std::string connectionID = connect();
    //std::cout << "Liu DEBUG: connectionID " << connectionID << std::endl;
    readConfiguration(connectionID, emu_config_id, endcap);
    std::cout << "######## EmuEndcap is complet." << std::endl;
    disconnect(connectionID);
    return endcap;
  }
  catch (std::exception &e)
  {
    XCEPT_RAISE(xcept::Exception,e.what());
  }
}


void PCConfigEditor::readConfiguration(
    const std::string &connectionID,
    const std::string &emu_config_id,
    EmuEndcap * endcap) throw (xcept::Exception)
{
  try
  {
    std::string queryViewName = "configuration";
    xdata::Table results;
    query(connectionID, queryViewName, emu_config_id, results);

    std::cout << "EMU Configuration - START" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::vector<std::string> columns = results.getColumns();
    for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
    {
      for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
      {
        std::string StrgValue = results.getValueAt(rowIndex, *column)->toString();
        std::cout << *column + ": " + StrgValue << std::endl;
      }
    }
    readPeripheralCrate(connectionID, emu_config_id, endcap);
    std::cout << "EMU Configuration - END" << std::endl;
  }
  catch (std::exception &e)
  {
    XCEPT_RAISE(xcept::Exception,e.what());
  }
}


int PCConfigEditor::readCrateID(xdata::Table &results, unsigned rowIndex) throw (xcept::Exception)
{
  xdata::Serializable * value = results.getValueAt(rowIndex, "CRATEID");
  if (value->type() == "int")
  {
    xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
    if (i) return (int) *i;
  }
  XCEPT_RAISE(xcept::Exception,"Coult not read crate ID "+value->toString());
}


void PCConfigEditor::readPeripheralCrate(
    const std::string &connectionID,
    const std::string &emu_config_id,
    EmuEndcap * endcap) throw (xcept::Exception)
{
  std::string queryViewName = "peripheralcrate";
  std::string periph_config_id;
  xdata::Table results;
  int crateid = 0;
  std::string label;

  query(connectionID, queryViewName, emu_config_id, results);
  std::cout << "Peripheral Crate" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      std::string StrgValue = results.getValueAt(rowIndex, *column)->toString();
      if (*column == "PERIPH_CONFIG_ID")
      {
        periph_config_id = StrgValue;
      }
      if (*column == "CRATEID")
        crateid = readCrateID(results, rowIndex);
      if (*column == "LABEL")
      {
        label = StrgValue;
      }
      std::cout << *column + ": " + StrgValue << std::endl;
    }

    VMEController * controller = new VMEController();
    Crate * crate = new Crate(crateid, controller);
    crate->SetLabel(label);
    fillCrateRealName(crateid, label);
    endcap->addCrate(crate);

    xdata::Table thisCrate;
    //this is because the results from TStore have the wrong column types
    copyPeripheralCrateToTable(thisCrate, crate);
    setCachedTable(queryViewName, crate->CrateID(), thisCrate);
    //show the chambers by default so it is easy to search for them on the page
    //not actually very practical because you see the outlines of all the other tables
    //and it takes a lot of space; this will have to be done a different way
    //setShouldDisplayConfiguration("wholecrate",crateIdentifierString(crate->CrateID()),true);

    readVCC(connectionID, emu_config_id, periph_config_id, crate);
    readCCB(connectionID, emu_config_id, periph_config_id, crate);
    readMPC(connectionID, emu_config_id, periph_config_id, crate);
    readCSC(connectionID, emu_config_id, periph_config_id, crate);

    std::cout << "#### PERIPHERAL_CRATE:  " << periph_config_id << " --- "
        << emu::db::to_string(crateid) << " --- " << label << std::endl;
  }
  std::cout << "######## All Peripheral crates are created." << std::endl;
}


void PCConfigEditor::readVCC(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &periph_config_id,
    Crate * theCrate) throw (xcept::Exception)
{
  std::string queryViewName = "vcc";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;
  int IntValue = 0;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);

  std::cout << "VCC  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    // add VMECC(VMEModule in slot 1) to crate
    VMECC * vcc = new VMECC(theCrate, 1);
    std::cout << "added vmecc" << (vcc ? "Not NULL" : "NULL") << std::endl;
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "MAC_ADDR")
      {
        theCrate->vmeController()->SetMAC(0, StrgValue);
        size_t pos = StrgValue.find('-');
        while (pos != std::string::npos)
        {
          StrgValue.replace(pos, 1, 1, ':');
          pos = StrgValue.find('-');
        }
        theCrate->vmeController()->SetVMEAddress(StrgValue);
      }
      if (*column == "ETH_PORT")     theCrate->vmeController()->SetPort(IntValue);
      if (*column == "MCAST_1")      theCrate->vmeController()->SetMAC(1,StrgValue);
      if (*column == "MCAST_2")      theCrate->vmeController()->SetMAC(2,StrgValue);
      if (*column == "MCAST_3")      theCrate->vmeController()->SetMAC(3,StrgValue);
      if (*column == "DFLT_SRV_MAC") theCrate->vmeController()->SetMAC(4,StrgValue);
      if (*column == "VCC_FRMW_VER") vcc->SetVCC_frmw_ver(StrgValue);
      if (*column == "ETHERNET_CR")  theCrate->vmeController()->SetCR(0,StrgValue);
      if (*column == "EXT_FIFO_CR")  theCrate->vmeController()->SetCR(1,StrgValue);
      if (*column == "RST_MISC_CR")  theCrate->vmeController()->SetCR(2,StrgValue);
      if (*column == "VME_CR")       theCrate->vmeController()->SetCR(3,StrgValue);
      if (*column == "BTO")          theCrate->vmeController()->SetCR(4,StrgValue);
      if (*column == "BGTO")         theCrate->vmeController()->SetCR(5,StrgValue);
      if (*column == "MSG_LVL")      theCrate->vmeController()->SetMsg_Lvl(IntValue);
      if (*column == "WARN_ON_SHTDWN")
      {
        if(StrgValue == "true")      theCrate->vmeController()->SetWarn_On_Shtdwn(true);
        else                         theCrate->vmeController()->SetWarn_On_Shtdwn(false);
      }
      if (*column == "PKT_ON_STARTUP")
      {
        if (StrgValue == "true")     theCrate->vmeController()->SetPkt_On_Startup(true);
        else                         theCrate->vmeController()->SetPkt_On_Startup(false);
      }
      std::cout << *column + ": " + StrgValue << std::endl;
    }
    std::cout << "#### VCC:  " << std::endl;
  }
  std::cout << "######## VCC is created." << std::endl;
  
  copyVMECCToTable(results,theCrate); //this is because the results from TStore have the wrong column types
  setCachedTable(queryViewName,theCrate->CrateID(),results);
}


void PCConfigEditor::diffCSC(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_periph_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_periph_config_id,
    int crateID) throw (xcept::Exception)
{
  std::string queryViewName = "csc";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;

  std::string old_csc_config_id;
  std::string new_csc_config_id;
  std::string label;

  diff(connectionID, queryViewName, old_emu_config_id, old_periph_config_id,
      new_emu_config_id, new_periph_config_id, results);

  std::vector<std::string> columns = results.getColumns();
  std::map<std::string, std::string, xdata::Table::ci_less> &definition = results.getTableDefinition();
  xdata::Table thisRowOnly(definition);
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      StrgValue = value->toString();

      if (*column == "LABEL")
      {
        label = StrgValue;
      }
      if (*column == "CSC_CONFIG_ID " + old_emu_config_id)
      {
        old_csc_config_id = StrgValue;
      }
      else if (*column == "CSC_CONFIG_ID " + new_emu_config_id)
      {
        new_csc_config_id = StrgValue;
      }
      thisRowOnly.setValueAt(0, *column, *value);
    }
    std::string chamber = chamberID(crateID, label);

    emu::db::ConfigEditor::setCachedDiff(queryViewName, chamber, thisRowOnly);

    diffDAQMB(connectionID, old_emu_config_id, old_csc_config_id, new_emu_config_id, new_csc_config_id, chamber);
    diffTMB(connectionID, old_emu_config_id, old_csc_config_id, new_emu_config_id, new_csc_config_id, chamber);
  }
}


void PCConfigEditor::readCSC(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &periph_config_id,
    Crate * theCrate) throw (xcept::Exception)
{
  std::string queryViewName = "csc";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;
  int IntValue = 0;

  std::string csc_config_id;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);

  std::cout << results.getRowCount() << " Chambers  in crate " << theCrate->CrateID() << std::endl;
  std::cout << "==================================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    Chamber * csc_ = new Chamber(theCrate);
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "LABEL")         csc_->SetLabel(StrgValue);
      if (*column == "KNOWN_PROBLEM") csc_->SetProblemDescription(StrgValue);
      if (*column == "PROBLEM_MASK")  csc_->SetProblemMask(IntValue);
      if (*column == "CSC_CONFIG_ID") csc_config_id = StrgValue;
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
    //we can't overwrite results because we are still looping through it
    xdata::Table correctedResults;
    copyCSCToTable(correctedResults, csc_);

    emu::db::ConfigEditor::setCachedTable(queryViewName, chamberID(theCrate->CrateID(), csc_->GetLabel()), correctedResults);

    //readDAQMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
    //readTMB(connectionID, emu_config_id, csc_config_id, theCrate, csc_);
  }
}


void PCConfigEditor::readCCB(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &periph_config_id,
    Crate * theCrate) throw (xcept::Exception)
{
  std::string queryViewName = "ccb";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;

  int IntValue = 0;
  int slot = 13;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);

  std::cout << "CCBs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    CCB * ccb_ = new CCB(theCrate, slot);
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "CCBMODE")            ccb_->SetCCBmode(IntValue);
      if (*column == "L1ADELAY")           ccb_->Setl1adelay(IntValue);
      if (*column == "TTCRXID")            ccb_->SetTTCrxID(IntValue);
      if (*column == "TTCRXCOARSEDELAY")   ccb_->SetTTCrxCoarseDelay(IntValue);
      if (*column == "TTCRXFINEDELAY")     ccb_->SetTTCrxFineDelay(IntValue);
      if (*column == "CCB_FIRMWARE_YEAR")  ccb_->SetExpectedFirmwareYear(IntValue);
      if (*column == "CCB_FIRMWARE_MONTH") ccb_->SetExpectedFirmwareMonth(IntValue);
      if (*column == "CCB_FIRMWARE_DAY")   ccb_->SetExpectedFirmwareDay(IntValue);
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
  }
  //this is because the results from TStore have the wrong column types
  copyCCBToTable(results, theCrate);
  setCachedTable(queryViewName, theCrate->CrateID(), results);
}


void PCConfigEditor::readMPC(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &periph_config_id,
    Crate * theCrate) throw (xcept::Exception)
{
  std::string queryViewName = "mpc";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;

  int IntValue = 0;
  int slot = 12;

  query(connectionID, queryViewName, emu_config_id, periph_config_id, results);

  std::cout << "MPCs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    MPC * mpc_ = new MPC(theCrate, slot);
    mpc_->SetBoardID(theCrate->CrateID());
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "SERIALIZERMODE")     mpc_->SetTLK2501TxMode(IntValue);
      if (*column == "TRANSPARENTMODE")    mpc_->SetTransparentMode(IntValue);
      if (*column == "MPC_FIRMWARE_YEAR")  mpc_->SetExpectedFirmwareYear(IntValue);
      if (*column == "MPC_FIRMWARE_MONTH") mpc_->SetExpectedFirmwareMonth(IntValue);
      if (*column == "MPC_FIRMWARE_DAY")   mpc_->SetExpectedFirmwareDay(IntValue);
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
  }
  //this is because the results from TStore have the wrong column types
  copyMPCToTable(results,theCrate);
  setCachedTable(queryViewName,theCrate->CrateID(),results); 
}


void PCConfigEditor::diffDAQMB(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_csc_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_csc_config_id,
    const std::string &chamberID) throw (xcept::Exception)
{
  std::string queryViewName = "daqmb";
  xdata::Table results;

  std::string old_daqmb_config_id, new_daqmb_config_id;

  diff(connectionID, queryViewName, old_emu_config_id, old_csc_config_id, new_emu_config_id, new_csc_config_id, results);

  std::vector<std::string> columns = results.getColumns();
  xdata::Serializable *value;
  std::cout << "DAQMB got " << results.getRowCount() << " rows" << std::endl;
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (*column == "DAQMB_CONFIG_ID " + old_emu_config_id)
      {
        old_daqmb_config_id = value->toString();
      }
      else if (*column == "DAQMB_CONFIG_ID " + new_emu_config_id)
      {
        new_daqmb_config_id = value->toString();
      }
    }
    emu::db::ConfigEditor::setCachedDiff(queryViewName, chamberID, results);
    diffCFEB(connectionID, old_emu_config_id, old_daqmb_config_id, new_emu_config_id, new_daqmb_config_id, chamberID);
  }
}


void PCConfigEditor::readDAQMB(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &csc_config_id,
    Crate * theCrate,
    Chamber * theChamber) throw (xcept::Exception)
{
  std::string queryViewName = "daqmb";
  xdata::Table results;

  xdata::Serializable * value;
  int IntValue = 0;
  long int LongIntValue;
  float FloatValue = 0.0;
  std::string StrgValue;
  int slot = 0;
  std::string daqmb_config_id_;

  query(connectionID, queryViewName, emu_config_id, csc_config_id, results);

  std::cout << "DAQMB  " << std::endl;
  results.writeTo(std::cout);std::cout << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      if (*column == "SLOT")
      {
        value = results.getValueAt(rowIndex, *column);
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if (i) slot = (int) *i;
      }
    }
    DAQMB * daqmb_ = new DAQMB(theCrate, theChamber, slot);
    daqmb_->SetCrateId(theCrate->CrateID());
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      //std::cout << *column + "[" + results.getColumnType(*column) + "]" + " ==> " + value->toString() << std::endl;     
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      if (results.getColumnType(*column) == "float")
      {
        xdata::Float * f = dynamic_cast<xdata::Float *> (value);
        if (f) FloatValue = (float) *f;
      }
      StrgValue = value->toString();

      if (*column == "CALIBRATION_LCT_DELAY")   daqmb_->SetCalibrationLctDelay(IntValue);
      if (*column == "CALIBRATION_L1ACC_DELAY") daqmb_->SetCalibrationL1aDelay(IntValue);
      if (*column == "PULSE_DELAY")             daqmb_->SetPulseDelay(IntValue);
      if (*column == "INJECT_DELAY")            daqmb_->SetInjectDelay(IntValue);
      if (*column == "KILL_INPUT")              daqmb_->SetKillInput(IntValue);
      if (*column == "PUL_DAC_SET")             daqmb_->SetPulseDac(FloatValue);
      if (*column == "INJ_DAC_SET")             daqmb_->SetInjectorDac(FloatValue);
      if (*column == "SET_COMP_THRESH")
      {
        daqmb_->SetCompThresh(FloatValue);
        for(int cfeb=0; cfeb<5; cfeb++)         daqmb_->SetCompThresholdsCfeb(cfeb,FloatValue);
      }
      if (*column == "COMP_MODE")
      {
        daqmb_->SetCompMode(IntValue);
        for(int cfeb=0; cfeb<5; cfeb++)         daqmb_->SetCompModeCfeb(cfeb,IntValue);
      }
      if (*column == "COMP_TIMING")
      {
        daqmb_->SetCompTiming(IntValue);
        for(int cfeb=0; cfeb<5; cfeb++)         daqmb_->SetCompTimingCfeb(cfeb,IntValue);
      }
      if (*column == "PRE_BLOCK_END"){
        daqmb_->SetPreBlockEnd(IntValue);
        for(int cfeb=0; cfeb<5; cfeb++)         daqmb_->SetPreBlockEndCfeb(cfeb,IntValue);
      }
      if (*column == "CFEB_CABLE_DELAY")        daqmb_->SetCfebCableDelay(IntValue);
      if (*column == "TMB_LCT_CABLE_DELAY")     daqmb_->SetTmbLctCableDelay(IntValue);
      if (*column == "CFEB_DAV_CABLE_DELAY")    daqmb_->SetCfebDavCableDelay(IntValue);
      if (*column == "ALCT_DAV_CABLE_DELAY")    daqmb_->SetAlctDavCableDelay(IntValue);
      if (*column == "FEB_CLOCK_DELAY")         daqmb_->SetCfebClkDelay(IntValue);
      if (*column == "XLATENCY")
      {
        daqmb_->SetxLatency(IntValue);
        for (int cfeb=0; cfeb<5; cfeb++)        daqmb_->SetL1aExtraCfeb(cfeb,IntValue);
      }
      if (*column == "XFINELATENCY")            daqmb_->SetxFineLatency(IntValue);
      if (*column == "DMB_CNTL_FIRMWARE_TAG")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%lx", &LongIntValue);
        daqmb_->SetExpectedControlFirmwareTag(LongIntValue);
      }
      if (*column == "DMB_VME_FIRMWARE_TAG")    daqmb_->SetExpectedVMEFirmwareTag(IntValue);
      if (*column == "DAQMB_CONFIG_ID")         daqmb_config_id_ = StrgValue;
      if (*column == "POWER_MASK")              daqmb_->SetPowerMask(IntValue);
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
    std::string identifier = DAQMBID(chamberID(theCrate->CrateID(), theChamber->GetLabel()), slot);
    readCFEB(connectionID, emu_config_id, daqmb_config_id_, daqmb_, identifier);

    //this is because the results from TStore have the wrong column types
    copyDAQMBToTable(results, daqmb_);
    emu::db::ConfigEditor::setCachedTable(queryViewName, identifier, results);
  }
}


void PCConfigEditor::diffCFEB(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_daqmb_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_daqmb_config_id,
    const std::string &cacheIdentifier) throw (xcept::Exception)
{
  simpleDiff("cfeb", connectionID, old_emu_config_id, old_daqmb_config_id,
      new_emu_config_id, new_daqmb_config_id, cacheIdentifier);
}


void PCConfigEditor::diffAnodeChannel(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_alct_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_alct_config_id,
    const std::string &cacheIdentifier) throw (xcept::Exception)
{
  simpleDiff("anodechannel", connectionID, old_emu_config_id, old_alct_config_id,
      new_emu_config_id, new_alct_config_id, cacheIdentifier);
}


void PCConfigEditor::readCFEB(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &daqmb_config_id,
    DAQMB * theDaqmb,
    const std::string &cacheIdentifier) throw (xcept::Exception)
{
  std::string queryViewName = "cfeb";
  xdata::Table results;

  xdata::Serializable * value;
  int IntValue = 0;
  long int LongIntValue;
  std::string StrgValue;

  int number = 0;
  int kill_chip[6] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

  query(connectionID, queryViewName, emu_config_id, daqmb_config_id, results);

  std::cout << "CFEBs  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      if (*column == "CFEB_NUMBER") number = IntValue;
    }
    std::cout << "CFEB cfeb( " + emu::db::to_string(number) << " )" << std::endl;
    CFEB cfeb(number);
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "CFEB_FIRMWARE_TAG")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%lx", &LongIntValue);
        theDaqmb->SetExpectedCFEBFirmwareTag(number, LongIntValue);
      }
      if (*column == "KILL_CHIP0")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[0] = IntValue;
      }
      if (*column == "KILL_CHIP1")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[1] = IntValue;
      }
      if (*column == "KILL_CHIP2")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[2] = IntValue;
      }
      if (*column == "KILL_CHIP3")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[3] = IntValue;
      }
      if (*column == "KILL_CHIP4")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[4] = IntValue;
      }
      if (*column == "KILL_CHIP5")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        kill_chip[5] = IntValue;
      }
      for (int chip = 0; chip < 6; chip++)
      {
        theDaqmb->SetKillChip(number, chip, kill_chip[chip]);
        for (int chan = 0; chan < 16; chan++)
        {
          unsigned short int mask = (1 << chan);
          if ((mask & kill_chip[chip]) == 0x0000)
          {
            theDaqmb->SetShiftArray(number, 5 - chip, chan, NORM_RUN);
          }
          else
          {
            theDaqmb->SetShiftArray(number, 5 - chip, chan, KILL_CHAN);
          }
        }
      }
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
    theDaqmb->cfebs_.push_back(cfeb);
  }
  copyCFEBToTable(results, theDaqmb);
  emu::db::ConfigEditor::setCachedTable(queryViewName, cacheIdentifier, results);
}


void PCConfigEditor::diffTMB(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_csc_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_csc_config_id,
    const std::string &chamber) throw (xcept::Exception)
{
  std::string queryViewName = "tmb";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;
  std::string old_tmb_config_id_, new_tmb_config_id_;

  //this selects TMBs which have either been changed themselves, or have had their ALCTs or anodechannels changed
  diff(connectionID, queryViewName, old_emu_config_id, old_csc_config_id, new_emu_config_id, new_csc_config_id, results);

  std::cout << "TMB  " << results.getRowCount() << " rows" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      StrgValue = value->toString();
      if (*column == "TMB_CONFIG_ID " + old_emu_config_id)
      {
        old_tmb_config_id_ = StrgValue;
      }
      else if (*column == "TMB_CONFIG_ID " + new_emu_config_id)
      {
        new_tmb_config_id_ = StrgValue;
      }
    }
    std::string identifier = chamber;
    diffALCT(connectionID, old_emu_config_id, old_tmb_config_id_, new_emu_config_id, new_tmb_config_id_, identifier);
    setCachedDiff(queryViewName, identifier, results);
  }
}


void PCConfigEditor::readTMB(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &csc_config_id,
    Crate * theCrate,
    Chamber * theChamber) throw (xcept::Exception)
{
  std::string queryViewName = "tmb";
  xdata::Table results;

  xdata::Serializable * value;
  int IntValue;
  long long int LongLongIntValue;
  std::string StrgValue;
  int slot = 0;
  std::string tmb_config_id_;

  query(connectionID, queryViewName, emu_config_id, csc_config_id, results);

  std::cout << "TMB  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      if (*column == "SLOT")
      {
        value = results.getValueAt(rowIndex, *column);
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if (i) slot = (int) *i;
      }
    }
    TMB * tmb_ = new TMB(theCrate, theChamber, slot);
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();
      
      if (*column == "TMB_FIRMWARE_MONTH"           ) tmb_->SetExpectedTmbFirmwareMonth(IntValue);
      if (*column == "TMB_FIRMWARE_DAY"             ) tmb_->SetExpectedTmbFirmwareDay(IntValue);
      if (*column == "TMB_FIRMWARE_YEAR"            ) tmb_->SetExpectedTmbFirmwareYear(IntValue);
      if (*column == "TMB_FIRMWARE_VERSION"         ) tmb_->SetExpectedTmbFirmwareVersion(IntValue);
      if (*column == "TMB_FIRMWARE_REVCODE"         ) tmb_->SetExpectedTmbFirmwareRevcode(IntValue);
      if (*column == "TMB_FIRMWARE_TYPE"            ) tmb_->SetExpectedTmbFirmwareType(IntValue);
      if (*column == "TMB_FIRMWARE_COMPILE_TYPE")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%x", &IntValue);
        tmb_->SetTMBFirmwareCompileType(IntValue);
      }
      if (*column == "RAT_FIRMWARE_MONTH"           ) tmb_->SetExpectedRatFirmwareMonth(IntValue);
      if (*column == "RAT_FIRMWARE_DAY"             ) tmb_->SetExpectedRatFirmwareDay(IntValue);
      if (*column == "RAT_FIRMWARE_YEAR"            ) tmb_->SetExpectedRatFirmwareYear(IntValue);
      if (*column == "ENABLE_ALCT_TX"               ) tmb_->SetEnableAlctTx(IntValue);
      if (*column == "ENABLE_ALCT_RX"               ) tmb_->SetAlctInput(IntValue);
      if (*column == "ALCT_POSNEG"                  ) tmb_->SetAlctPosNeg(IntValue);
      if (*column == "ALCT_CLEAR"                   ) tmb_->SetAlctClear(IntValue);
      if (*column == "ENABLECLCTINPUTS_REG42"       ) tmb_->SetEnableCLCTInputs(IntValue);
      if (*column == "RPC_EXISTS"                   ) tmb_->SetRpcExist(IntValue);
      if (*column == "RPC_READ_ENABLE"              ) tmb_->SetRpcReadEnable(IntValue);
      if (*column == "RPC_BXN_OFFSET"               ) tmb_->SetRpcBxnOffset(IntValue);
      if (*column == "RPC_MASK_ALL"                 ) tmb_->SetEnableRpcInput(IntValue);
      if (*column == "MATCH_PRETRIG_WINDOW_SIZE"    ) tmb_->SetAlctClctPretrigWidth(IntValue);
      if (*column == "MATCH_PRETRIG_ALCT_DELAY"     ) tmb_->SetAlctPretrigDelay(IntValue);
      //      if (*column == "LAYER_TRIG_DELAY"             ) tmb_->SetLayerTrigDelay(IntValue); // obsolete
      if (*column == "TMB_L1A_DELAY"                ) tmb_->SetL1aDelay(IntValue);
      if (*column == "TMB_L1A_WINDOW_SIZE"          ) tmb_->SetL1aWindowSize(IntValue);
      if (*column == "TMB_L1A_OFFSET"               ) tmb_->SetL1aOffset(IntValue);
      if (*column == "TMB_BXN_OFFSET"               ) tmb_->SetBxnOffset(IntValue);
      if (*column == "MATCH_TRIG_ALCT_DELAY"        ) tmb_->SetAlctVpfDelay(IntValue);
      if (*column == "MATCH_TRIG_WINDOW_SIZE"       ) tmb_->SetAlctMatchWindowSize(IntValue);
      if (*column == "MPC_TX_DELAY"                 ) tmb_->SetMpcTxDelay(IntValue);
      if (*column == "RPC0_RAW_DELAY"               ) tmb_->SetRpc0RawDelay(IntValue);
      if (*column == "RPC1_RAW_DELAY"               ) tmb_->SetRpc1RawDelay(IntValue);
      if (*column == "CLCT_PRETRIG_ENABLE"          ) tmb_->SetClctPatternTrigEnable(IntValue);
      if (*column == "ALCT_PRETRIG_ENABLE"          ) tmb_->SetAlctPatternTrigEnable(IntValue);
      if (*column == "MATCH_PRETRIG_ENABLE"         ) tmb_->SetMatchPatternTrigEnable(IntValue);
      if (*column == "CLCT_EXT_PRETRIG_ENABLE"      ) tmb_->SetClctExtTrigEnable(IntValue);
      if (*column == "ALL_CFEB_ACTIVE"              ) tmb_->SetEnableAllCfebsActive(IntValue);
      if (*column == "ENABLECLCTINPUTS_REG68"       ) tmb_->SetCfebEnable(IntValue);
      if (*column == "CFEB_ENABLE_SOURCE"           ) tmb_->Set_cfeb_enable_source(IntValue);
      if (*column == "CLCT_HIT_PERSIST"             ) tmb_->SetTriadPersistence(IntValue);
      if (*column == "CLCT_NPLANES_HIT_PRETRIG"     ) tmb_->SetHsPretrigThresh(IntValue);
      if (*column == "CLCT_NPLANES_HIT_PATTERN"     ) tmb_->SetMinHitsPattern(IntValue);
      if (*column == "CLCT_DRIFT_DELAY"             ) tmb_->SetDriftDelay(IntValue);
      if (*column == "TMB_FIFO_MODE"                ) tmb_->SetFifoMode(IntValue);
      if (*column == "CLCT_FIFO_TBINS"              ) tmb_->SetFifoTbins(IntValue);
      if (*column == "CLCT_FIFO_PRETRIG"            ) tmb_->SetFifoPreTrig(IntValue);
      if (*column == "MPC_SYNC_ERR_ENABLE"          ) tmb_->SetTmbSyncErrEnable(IntValue);
      if (*column == "ALCT_TRIG_ENABLE"             ) tmb_->SetTmbAllowAlct(IntValue);
      if (*column == "CLCT_TRIG_ENABLE"             ) tmb_->SetTmbAllowClct(IntValue);
      if (*column == "MATCH_TRIG_ENABLE"            ) tmb_->SetTmbAllowMatch(IntValue);
      if (*column == "ALCT_READOUT_WITHOUT_TRIG"    ) tmb_->SetAllowAlctNontrigReadout(IntValue);
      if (*column == "CLCT_READOUT_WITHOUT_TRIG"    ) tmb_->SetAllowClctNontrigReadout(IntValue);
      if (*column == "MATCH_READOUT_WITHOUT_TRIG"   ) tmb_->SetAllowMatchNontrigReadout(IntValue);
      if (*column == "MPC_RX_DELAY"                 ) tmb_->SetMpcRxDelay(IntValue);
      if (*column == "MPC_IDLE_BLANK"               ) tmb_->SetMpcIdleBlank(IntValue);
      if (*column == "MPC_OUTPUT_ENABLE"            ) tmb_->SetMpcOutputEnable(IntValue);
      if (*column == "MPC_BLOCK_ME1A"               ) tmb_->SetBlockME1aToMPC(IntValue);
      if (*column == "WRITE_BUFFER_REQUIRED"        ) tmb_->SetWriteBufferRequired(IntValue);
      if (*column == "VALID_CLCT_REQUIRED"          ) tmb_->SetRequireValidClct(IntValue);
      if (*column == "L1A_ALLOW_MATCH"              ) tmb_->SetL1aAllowMatch(IntValue);
      if (*column == "L1A_ALLOW_NOTMB"              ) tmb_->SetL1aAllowNoTmb(IntValue);
      if (*column == "L1A_ALLOW_NOL1A"              ) tmb_->SetL1aAllowNoL1a(IntValue);
      if (*column == "L1A_ALLOW_ALCT_ONLY"          ) tmb_->SetL1aAllowAlctOnly(IntValue);
      if (*column == "LAYER_TRIG_ENABLE"            ) tmb_->SetEnableLayerTrigger(IntValue);
      if (*column == "LAYER_TRIG_THRESH"            ) tmb_->SetLayerTriggerThreshold(IntValue);
      if (*column == "CLCT_BLANKING"                ) tmb_->SetClctBlanking(IntValue);
      //      if (*column == "CLCT_STAGGER"                 ) tmb_->SetClctStagger(IntValue); // obsolete
      if (*column == "CLCT_PID_THRESH_PRETRIG"      ) tmb_->SetClctPatternIdThresh(IntValue);
      if (*column == "AFF_THRESH"                   ) tmb_->SetActiveFebFlagThresh(IntValue);
      if (*column == "CLCT_MIN_SEPARATION"          ) tmb_->SetMinClctSeparation(IntValue);
      if (*column == "LAYER0_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(0, LongLongIntValue);
      }
      if (*column == "LAYER1_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(1, LongLongIntValue);
      }
      if (*column == "LAYER2_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(2, LongLongIntValue);
      }
      if (*column == "LAYER3_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(3, LongLongIntValue);
      }
      if (*column == "LAYER4_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(4, LongLongIntValue);
      }
      if (*column == "LAYER5_DISTRIP_HOT_CHANN_MASK")
      { // saved as string in Oracle since there is no Hex support
        sscanf(StrgValue.c_str(), "%Lx", &LongLongIntValue);
        tmb_->SetDistripHotChannelMask(5, LongLongIntValue);
      }
      if (*column == "REQUEST_L1A"                  ) tmb_->SetRequestL1a(IntValue);
      if (*column == "IGNORE_CCB_STARTSTOP"         ) tmb_->SetIgnoreCcbStartStop(IntValue);
      if (*column == "ALCT_CLOCK_EN_USE_CCB"        ) tmb_->SetEnableAlctUseCcbClock(IntValue);
      if (*column == "ALCT_TX_CLOCK_DELAY"          ) tmb_->SetAlctTXclockDelay(IntValue);
      if (*column == "ALCT_RX_CLOCK_DELAY"          ) tmb_->SetAlctRXclockDelay(IntValue);
      if (*column == "DMB_TX_DELAY"                 ) tmb_->SetDmbTxDelay(IntValue);
      if (*column == "RAT_TMB_DELAY"                ) tmb_->SetRatTmbDelay(IntValue);
      if (*column == "CFEB0DELAY"                   ) tmb_->SetCFEB0delay(IntValue);
      if (*column == "CFEB1DELAY"                   ) tmb_->SetCFEB1delay(IntValue);
      if (*column == "CFEB2DELAY"                   ) tmb_->SetCFEB2delay(IntValue);
      if (*column == "CFEB3DELAY"                   ) tmb_->SetCFEB3delay(IntValue);
      if (*column == "CFEB4DELAY"                   ) tmb_->SetCFEB4delay(IntValue);
      if (*column == "RPC0_RAT_DELAY"               ) tmb_->SetRpc0RatDelay(IntValue);
      if (*column == "RPC1_RAT_DELAY"               ) tmb_->SetRpc1RatDelay(IntValue);
      if (*column == "ADJACENT_CFEB_DISTANCE"       ) tmb_->SetAdjacentCfebDistance(IntValue);
      if (*column == "ALCT_BX0_DELAY"               ) tmb_->SetAlctBx0Delay(IntValue);
      if (*column == "ALCT_BX0_ENABLE"              ) tmb_->SetAlctBx0Enable(IntValue);
      if (*column == "CLCT_BX0_DELAY"               ) tmb_->SetClctBx0Delay(IntValue);
      if (*column == "CLCT_THROTTLE"                ) tmb_->SetClctThrottle(IntValue);
      if (*column == "RPC_FIFO_DECOUPLE"            ) tmb_->SetRpcDecoupleTbins(IntValue);
      if (*column == "RPC_FIFO_PRETRIG"             ) tmb_->SetFifoPretrigRpc(IntValue);
      if (*column == "RPC_FIFO_TBINS"               ) tmb_->SetFifoTbinsRpc(IntValue);
      if (*column == "WRITE_BUFFER_AUTOCLEAR"       ) tmb_->SetWriteBufferAutoclear(IntValue);
      if (*column == "WRITE_BUFFER_CONTINOUS_ENABLE") tmb_->SetClctWriteContinuousEnable(IntValue);
      if (*column == "TMB_FIFO_NO_RAW_HITS"         ) tmb_->SetFifoNoRawHits(IntValue);
      
      if (*column == "ALCT_TX_POSNEG"        ) tmb_->SetAlctTxPosNeg(IntValue);
      if (*column == "CFEB0POSNEG"           ) tmb_->SetCfeb0RxPosNeg(IntValue);
      if (*column == "CFEB1POSNEG"           ) tmb_->SetCfeb1RxPosNeg(IntValue);
      if (*column == "CFEB2POSNEG"           ) tmb_->SetCfeb2RxPosNeg(IntValue);
      if (*column == "CFEB3POSNEG"           ) tmb_->SetCfeb3RxPosNeg(IntValue);
      if (*column == "CFEB4POSNEG"           ) tmb_->SetCfeb4RxPosNeg(IntValue);
      if (*column == "MPC_SEL_TTC_BX0"       ) tmb_->SetSelectMpcTtcBx0(IntValue);
      if (*column == "ALCT_TOF_DELAY"        ) tmb_->SetAlctTOFDelay(IntValue);
      if (*column == "TMB_TO_ALCT_DATA_DELAY") tmb_->SetALCTTxDataDelay(IntValue);
      if (*column == "CFEB_TOF_DELAY"        ) tmb_->SetCfebTOFDelay(IntValue);
      if (*column == "CFEB0_TOF_DELAY"       ) tmb_->SetCfeb0TOFDelay(IntValue);
      if (*column == "CFEB1_TOF_DELAY"       ) tmb_->SetCfeb1TOFDelay(IntValue);
      if (*column == "CFEB2_TOF_DELAY"       ) tmb_->SetCfeb2TOFDelay(IntValue);
      if (*column == "CFEB3_TOF_DELAY"       ) tmb_->SetCfeb3TOFDelay(IntValue);
      if (*column == "CFEB4_TOF_DELAY"       ) tmb_->SetCfeb4TOFDelay(IntValue);
      if (*column == "CFEB_BADBITS_BLOCK"    ) tmb_->SetCFEBBadBitsBlock(IntValue);
      if (*column == "CFEB0_RXD_INT_DELAY"   ) tmb_->SetCFEB0RxdIntDelay(IntValue);
      if (*column == "CFEB1_RXD_INT_DELAY"   ) tmb_->SetCFEB1RxdIntDelay(IntValue);
      if (*column == "CFEB2_RXD_INT_DELAY"   ) tmb_->SetCFEB2RxdIntDelay(IntValue);
      if (*column == "CFEB3_RXD_INT_DELAY"   ) tmb_->SetCFEB3RxdIntDelay(IntValue);
      if (*column == "CFEB4_RXD_INT_DELAY"   ) tmb_->SetCFEB4RxdIntDelay(IntValue);
      if (*column == "CFEB_BADBITS_READOUT"  ) tmb_->SetCFEBBadBitsReadout(IntValue);
      if (*column == "L1A_PRIORITY_ENABLE"   ) tmb_->SetL1APriorityEnable(IntValue);
      if (*column == "MINISCOPE_ENABLE"      ) tmb_->SetMiniscopeEnable(IntValue);
      
      if (*column == "TMB_CONFIG_ID" ) tmb_config_id_ = StrgValue;
    }
    std::string identifier = DAQMBID(chamberID(theCrate->CrateID(), theChamber->GetLabel()), slot);
    readALCT(connectionID, emu_config_id, tmb_config_id_, tmb_, identifier);

    //this is because the results from TStore have the wrong column types
    copyTMBToTable(results, tmb_);
    setCachedTable(queryViewName, identifier, results);
  }
}


void PCConfigEditor::diffALCT(
    const std::string &connectionID,
    const std::string &old_emu_config_id,
    const std::string &old_tmb_config_id,
    const std::string &new_emu_config_id,
    const std::string &new_tmb_config_id,
    const std::string &chamber) throw (xcept::Exception)
{
  std::string queryViewName = "alct";
  xdata::Table results;

  xdata::Serializable * value;
  std::string StrgValue;
  std::string old_alct_config_id_;
  std::string new_alct_config_id_;

  //this selects any ALCTs which have either been changed themselves or which have changed anodechannels
  diff(connectionID, queryViewName, old_emu_config_id, old_tmb_config_id, new_emu_config_id, new_tmb_config_id, results);

  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      StrgValue = value->toString();

      if (*column == "ALCT_CONFIG_ID " + old_emu_config_id)
      {
        old_alct_config_id_ = StrgValue;
      }
      if (*column == "ALCT_CONFIG_ID " + new_emu_config_id)
      {
        new_alct_config_id_ = StrgValue;
      }
    }
    diffAnodeChannel(connectionID, old_emu_config_id, old_alct_config_id_,
        new_emu_config_id, new_alct_config_id_, chamber);
    setCachedDiff(queryViewName, chamber, results);
  }
}


void PCConfigEditor::readALCT(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &tmb_config_id,
    TMB * theTmb,
    const std::string &identifier) throw (xcept::Exception)
{
  std::string queryViewName = "alct";
  xdata::Table results;

  xdata::Serializable * value;
  int IntValue = 0;
  std::string StrgValue;
  std::string chamberType;
  std::string alct_config_id_;

  query(connectionID, queryViewName, emu_config_id, tmb_config_id, results);

  std::cout << "ALCT  " << std::endl;
  std::cout << "=========================================" << std::endl;
  results.writeTo(std::cout);std::cout << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      if (*column == "CHAMBER_TYPE")
      {
        chamberType = results.getValueAt(rowIndex, *column)->toString();
      }
    }
    ALCTController * alct_ = new ALCTController(theTmb, chamberType);
    RAT * rat_ = new RAT(theTmb);
    theTmb->SetAlct(alct_); //store alct_ pointer in theTmb
    theTmb->SetRat(rat_); //store rat_  in theTmb
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "ALCT_FIRMWARE_DAY"             ) alct_->SetExpectedFastControlDay(IntValue);
      if (*column == "ALCT_FIRMWARE_MONTH"           ) alct_->SetExpectedFastControlMonth(IntValue);
      if (*column == "ALCT_FIRMWARE_YEAR"            ) alct_->SetExpectedFastControlYear(IntValue);
      if (*column == "ALCT_FIRMWARE_BACKWARD_FORWARD") alct_->Set_fastcontrol_backward_forward_type(StrgValue);
      if (*column == "ALCT_FIRMWARE_NEGAT_POSIT"     ) alct_->Set_fastcontrol_negative_positive_type(StrgValue);
      if (*column == "ALCT_TRIG_MODE"                ) alct_->SetTriggerMode(IntValue);
      if (*column == "ALCT_EXT_TRIG_ENABLE"          ) alct_->SetExtTrigEnable(IntValue);
      if (*column == "ALCT_SEND_EMPTY"               ) alct_->SetSendEmpty(IntValue);
      if (*column == "ALCT_INJECT_MODE"              ) alct_->SetInjectMode(IntValue);
      if (*column == "ALCT_BXN_OFFSET"               ) alct_->SetBxcOffset(IntValue);
      if (*column == "ALCT_NPLANES_HIT_ACCEL_PRETRIG") alct_->SetAcceleratorPretrigThresh(IntValue);
      if (*column == "ALCT_NPLANES_HIT_ACCEL_PATTERN") alct_->SetAcceleratorPatternThresh(IntValue);
      if (*column == "ALCT_NPLANES_HIT_PATTERN"      ) alct_->SetPretrigNumberOfPattern(IntValue);
      if (*column == "ALCT_NPLANES_HIT_PRETRIG"      ) alct_->SetPretrigNumberOfLayers(IntValue);
      if (*column == "ALCT_DRIFT_DELAY"              ) alct_->SetDriftDelay(IntValue);
      if (*column == "ALCT_FIFO_TBINS"               ) alct_->SetFifoTbins(IntValue);
      if (*column == "ALCT_FIFO_PRETRIG"             ) alct_->SetFifoPretrig(IntValue);
      if (*column == "ALCT_FIFO_MODE"                ) alct_->SetFifoMode(IntValue);
      if (*column == "ALCT_L1A_DELAY"                ) alct_->SetL1aDelay(IntValue);
      if (*column == "ALCT_L1A_WINDOW_WIDTH"         ) alct_->SetL1aWindowSize(IntValue);
      if (*column == "ALCT_L1A_OFFSET"               ) alct_->SetL1aOffset(IntValue);
      if (*column == "ALCT_L1A_INTERNAL"             ) alct_->SetL1aInternal(IntValue);
      if (*column == "ALCT_CCB_ENABLE"               ) alct_->SetCcbEnable(IntValue);
      if (*column == "ALCT_CONFIG_IN_READOUT"        ) alct_->SetConfigInReadout(IntValue);
      if (*column == "ALCT_ACCEL_MODE"               ) alct_->SetAlctAmode(IntValue);
      if (*column == "ALCT_TRIG_INFO_EN"             ) alct_->SetTriggerInfoEnable(IntValue);
      if (*column == "ALCT_SN_SELECT"                ) alct_->SetSnSelect(IntValue);
      if (*column == "ALCT_TESTPULSE_AMPLITUDE"      ) alct_->SetTestpulseAmplitude(IntValue);
      if (*column == "ALCT_TESTPULSE_INVERT"         )
      {
        alct_->Set_InvertPulse(StrgValue);
        //std::cout << "ALCT_TESTPULSE_INVERT set to: " << StrgValue << std::endl;
        //std::cout << "ALCT_TESTPULSE_INVERT read as: " << alct_->Get_InvertPulse() << std::endl;
      }
      if (*column == "ALCT_TESTPULSE_DIRECTION"      ) alct_->Set_PulseDirection(StrgValue);
      if (*column == "ALCT_ZERO_SUPPRESS"            ) alct_->SetAlctZeroSuppress(IntValue);
      if (*column == "ALCT_CONFIG_ID"                ) alct_config_id_ = StrgValue;
      
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
    readAnodeChannel(connectionID, emu_config_id, alct_config_id_, alct_, identifier);
    //this is because the results from TStore have the wrong column types
    if (rowIndex == 0) copyALCTToTable(results, alct_);
    else
    { //as far as I can tell, there can only be one ALCT per TMB
      XCEPT_RAISE(xcept::Exception,"More than one ALCT for TMB");
    }
    setCachedTable(queryViewName, identifier, results);
  }
}


void PCConfigEditor::readAnodeChannel(
    const std::string &connectionID,
    const std::string &emu_config_id,
    const std::string &alct_config_id,
    ALCTController * theAlct,
    const std::string &identifier)
    throw (xcept::Exception)
{
  std::string queryViewName = "anodechannel";
  xdata::Table results;
  xdata::Serializable * value;
  int IntValue = 0;
  std::string StrgValue;
  int afeb_number = 0;

  query(connectionID, queryViewName/*"EMU_ANODECHANNEL"*/, emu_config_id, alct_config_id, results);

  std::cout << "ANODECHANNEL  " << std::endl;
  std::cout << "=========================================" << std::endl;
  std::vector<std::string> columns = results.getColumns();
  for (unsigned rowIndex = 0; rowIndex < results.getRowCount(); rowIndex++)
  {
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      if (*column == "AFEB_NUMBER")
      {
        value = results.getValueAt(rowIndex, *column);
        //std::cout << *column + ": " + value->toString() << std::endl;
        //std::cout << "type: " << results.getColumnType(*column) << std::endl;
        if (results.getColumnType(*column) == "int")
        {
          xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
          if (i) afeb_number = (int) *i;
        }
      }
    }
    theAlct->SetStandbyRegister_(afeb_number - 1, ON);
    for (std::vector<std::string>::iterator column = columns.begin(); column != columns.end(); ++column)
    {
      value = results.getValueAt(rowIndex, *column);
      //std::cout << "type: " << results.getColumnType(*column) << std::endl;
      if (results.getColumnType(*column) == "int")
      {
        xdata::Integer * i = dynamic_cast<xdata::Integer *> (value);
        if(i->isNaN()) IntValue=0;
        else IntValue=(int)*i;
      }
      StrgValue = value->toString();

      if (*column == "AFEB_FINE_DELAY")
      {
        theAlct->SetAsicDelay(afeb_number - 1, IntValue);
      }
      if (*column == "AFEB_THRESHOLD")
      {
        theAlct->SetAfebThreshold(afeb_number - 1, IntValue);
      }
      //std::cout << *column + ": " + StrgValue << std::endl;
    }
  }
  //this is because the results from TStore have the wrong column types
  copyAnodeChannelToTable(results, theAlct);
  setCachedTable(queryViewName, identifier, results);
}


std::string PCConfigEditor::displayName(
    const std::string &configName,
    const std::string &identifier,
    xdata::Table &data)
{
  //std::cout << "displayName for " << configName << std::endl;
  if (configName == topLevelTableName_ && tableHasColumn(data, "LABEL") && data.getRowCount() == 1)
  {
    xdata::String *label = dynamic_cast<xdata::String *> (data.getValueAt(0, "LABEL"));
    if (label) return (std::string) *label;
  }
  return ConfigEditor::displayName(configName, identifier, data);
}


}}
