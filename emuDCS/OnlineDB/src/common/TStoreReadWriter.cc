// $Id: TStoreReadWriter.cc,v 1.5 2012/06/20 22:44:25 khotilov Exp $

#include "emu/db/TStoreReadWriter.h"
#include "emu/db/TStoreAgent.h"

#include "emu/utils/SimpleTimer.h"
#include "emu/utils/System.h"

#include "xdata/TableIterator.h"
#include "xdata/UnsignedShort.h"
#include "xdata/Integer.h"
#include "xdata/Vector.h"
#include "toolbox/TimeVal.h"
#include "xdata/TimeVal.h"

#define DBG 0

namespace emu {	namespace db {


TStoreReadWriter::TStoreReadWriter(ConfigHierarchy* hierarchy, xdaq::Application *application, const std::string & db_credentials, int instance)
: ConfigReadWriter()
, application_(application)
, db_credentials_(db_credentials)
, instance_(instance)
{
  hierarchy_ = hierarchy;

  if (hierarchy_->notForDB())
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "TStoreReadWriter was given ConfigHierarchy that was not supposed to be used with DB!");
  }

  if (db_credentials_.empty())
  {
    try
    {
      std::string home_dir = getenv("HOME");
      db_credentials_ = toolbox::trim(emu::utils::readFile(home_dir + "/dbuserfile.txt"));
    }
    catch (xcept::Exception & e)
    {
      XCEPT_RETHROW(emu::exception::FileException, "No db_credentials provided and cannot open $HOME/dbuserfile.txt " + std::string(e.what()), e);
    }
  }
}


std::vector<ConfigIDInfo> TStoreReadWriter::readIDInfos(const std::string &subsystem, int n)
throw (emu::exception::ConfigurationException)
{
  std::vector<ConfigIDInfo> result;

  // find out the query name from the top level table type:
  std::string query_name = toolbox::tolower(hierarchy_->typeOfHead());
  if (subsystem.empty()) query_name += "_ids";
  else query_name += "_ids_side";

  //set up parameter name and value
  std::map< std::string, std::string > parameters;
  if (!subsystem.empty())  parameters["SIDE"] = subsystem;

  std::ostringstream maxRows;
  maxRows << n;
  parameters["MAX"] = maxRows.str();

  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try  { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  xdata::Table table;
  try { table = tstore.query(query_name, parameters); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot get configuration IDs: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot get configuration IDs (unknown exception)");
  }
  tstore.disconnect();

  for (size_t row = 0; row < table.getRowCount(); ++row)
  {
    result.push_back( ConfigIDInfo(table, row, hierarchy_) );
  }

  return result;
}


std::vector<xdata::UnsignedInteger64> TStoreReadWriter::readIDs(const std::string &subsystem, int n)
throw (emu::exception::ConfigurationException)
{
  std::vector<xdata::UnsignedInteger64> result;

  std::vector<ConfigIDInfo> id_infos = readIDInfos(subsystem, n);

  for (size_t i = 0; i < id_infos.size(); ++i)
  {
    xdata::UnsignedInteger64 id;
    id.fromString(id_infos[i].id());
    result.push_back(id);
  }
  return result;
}


std::vector<std::string> TStoreReadWriter::readIDs(int side, int n)
throw (emu::exception::ConfigurationException)
{
  std::string subsystem = "";
  if (side==1) subsystem = "plus";
  if (side==2) subsystem = "minus";
  if (subsystem.empty())
    XCEPT_RAISE(emu::exception::ConfigurationException, "TStoreReadWriter::readIDs: side argument is not 1 or 2: " + toolbox::toString("%d", side));

  std::vector<ConfigIDInfo> id_infos = readIDInfos(subsystem, n);

  std::vector<std::string> result;
  for (size_t i = 0; i < id_infos.size(); ++i)
  {
    result.push_back(id_infos[i].id());
  }
  return result;
}


xdata::UnsignedInteger64 TStoreReadWriter::readMaxID(const std::string &subsystem)
throw (emu::exception::ConfigurationException)
{
  xdata::UnsignedInteger64 result;

  // find out the query name from the top level table type:
  std::string query_name = toolbox::tolower(hierarchy_->typeOfHead());
  if (subsystem.empty()) query_name += "_maxid";
  else query_name += "_maxid_side";

  //set up parameter name and value
  std::map< std::string, std::string > parameters;
  if (!subsystem.empty())  parameters["SIDE"] = subsystem;

  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try  { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  xdata::Table table;
  try  { table = tstore.query(query_name, parameters); }
  catch (emu::exception::DBException &e)
  {
    tstore.disconnect();
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot get configuration max ID: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot get configuration max ID (unknown exception)");
  }
  tstore.disconnect();

  std::string id_field_name = hierarchy_->idFieldNameOfHead();
  result.fromString(table.getValueAt(0, id_field_name)->toString());

  return result;
}


bool TStoreReadWriter::read(xdata::UnsignedInteger64 id)
throw (emu::exception::ConfigurationException)
{
  emu::utils::SimpleTimer timer;

  datamap_.clear();

  //set up parameter name and value
  std::map< std::string, std::string > parameters;
  parameters["ID"] = id.toString();

  // prepare TStoreAgent
  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try  { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  // prepare empty tables
  datamap_ = hierarchy_->definitionsTree();
  std::vector<std::string> types = hierarchy_->types();

  try
  {
    for (std::vector< std::string >::iterator t = types.begin(); t != types.end(); t++)
    {
      // get query name from the type of table:
      std::string query_name = toolbox::tolower(*t) + "_all";
      // query data from DB
      xdata::Table table_tstore = tstore.query(query_name, parameters);
      // shortcut for the table to fill
      ConfigTable &table_2fill = datamap_[*t];


      /* The situation is as follows:
       *  (1). we have table definitions as read from TStore configuration view (accessible through hierarchy_)
       *  (2). we have table definitions as read from DB by TStore
       *  (3). TStore or database messes up the case of field names
       *
       * We need to make sure that definitions in (1) are the same as definitions in (2).
       * And thit should be done in case-independent way b/c of (3).
       *
       * Also, b/c of (3) we want to have tables with definitions from (1) which have proper case.
       * We have already initialized the datamap_ with proper definitionsTree.
       * Now, after reading tables from DB, we need to copy them into our empty tables.
       */

      DefinitionType &def_tstore = table_tstore.getTableDefinition();
      DefinitionType &def_conf   = table_2fill.getTableDefinition();

      if (DBG)
      {
        std::cout<<"config names: "<<std::endl;
        for (DefinitionType::iterator field = def_conf.begin(); field != def_conf.end(); field++ )  std::cout<<(*field).first<<" "<<(*field).second<<std::endl;
        std::cout<<std::endl;
        std::cout<<"DB names: "<<std::endl;
        for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )  std::cout<<(*tfield).first<<" "<<(*tfield).second<<std::endl;
        std::cout<<std::endl;
      }

      // reserve the container size
      table_2fill.reserve(table_tstore.getRowCount());

      for (DefinitionType::iterator field = def_conf.begin(); field != def_conf.end(); field++ )
      {
        std::string name = (*field).first;
        std::string datatype = (*field).second;

        std::string name_tstore = "";
        std::string datatype_tstore = "";

        for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )
        {
          /// assert that table definitions as read from TStore are the same as read from TStore configuration view
          if (toolbox::toupper(name) == toolbox::toupper((*tfield).first))
          {
            name_tstore = (*tfield).first;
            datatype_tstore = (*tfield).second;
            break;
          }
        }
        if (name_tstore.empty())
        {
          std::ostringstream error;
          error<<"Config.view field name -> "<<name<<" is not in "<<std::endl;
          error<<"DB names: "<<std::endl;
          for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )
          {
            error<<(*tfield).first<<" ";
          }
          error<<std::endl;
          XCEPT_RAISE(emu::exception::ConfigurationException, std::string(
                      "Table definitions (above): as read from TStore != as read from TStore configuration view!!!"));
        }

        //      copy the data
        xdata::AbstractVector * v_tstore = table_tstore.columnData_.find(name_tstore)->second;
        xdata::AbstractVector * v_conf = table_2fill.columnData_.find(name)->second;
        for (size_t r = 0; r < table_tstore.getRowCount(); r++ )
        {
          xdata::Serializable * s = v_tstore->elementAt(r);

          // correct for this very popular datatype which TStore gets wrong as "int":
          if ( datatype == "unsigned short" && datatype_tstore == "int" )
          {
            xdata::Integer ival = *dynamic_cast<xdata::Integer*>(s);
            xdata::UnsignedShort val;
            val.fromString(ival.toString());
            *dynamic_cast<xdata::UnsignedShort*>(v_conf->elementAt(r)) = val;
          }
          else
          {
            table_2fill.setValueAt(r, name, *s );
          }
        }
      }

      table_2fill.setHierarchy(hierarchy_);
      table_2fill.setTableType(*t);
      //datamap_[*t].writeTo(std::cout); std::cout<<std::endl<<std::endl;
    }
  }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot read configuration: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot read configuration (unknown exception)");
  }
  tstore.disconnect();
  std::cout<<std::endl;

  std::cout<<"Endcap DB read time: "<<timer.sec()<<" sec"<<std::endl;
  return true;
}


bool TStoreReadWriter::write(std::map<std::string,ConfigTable> *data)
throw (emu::exception::ConfigurationException)
{
  emu::utils::SimpleTimer timer;

  if (data == 0) data = &datamap_;

  std::string head_type = hierarchy_->typeOfHead();
  ConfigTable & head = (*data)[head_type];
  if (head.getRowCount() != 1)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "TStoreReadWriter::write: Head ConfigTable has number of rows not equal one: " +
                toolbox::toString("%d",head.getRowCount()));
  }

  toolbox::TimeVal currentTime;
  xdata::TimeVal   config_time = (xdata::TimeVal) currentTime.gettimeofday();
  //head.writeTo(std::cout); std::cout<<std::endl;
  head.setValueAt(0, hierarchy_->timeFieldName(), config_time);

  // find out the subsystem
  std::string subsystem = hierarchy_->determineSubSystem(head);

  // get max ID in DB for a specific subsystem (e.g., endcap side) and increment it by 1
  xdata::UnsignedInteger64 new_id = readMaxID(subsystem) + 1;

  // prepare TStoreAgent
  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try  { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  std::vector<std::string> types = hierarchy_->types();
  try
  {
    for (std::vector< std::string >::iterator t = types.begin(); t != types.end(); t++)
    {
      // get insert query name from the type of table:
      std::string insert_name = toolbox::tolower(*t);;

      // table which contains information that needs to be written
      xdata::Table & table = (*data)[*t];

      // by using the table definition from TStore for a table to write, we can ensure the field name case safety
      // but we do check furcher that table definition are the same (case-insensitive)
      xdata::Table table_2write = tstore.definition(*t);

      DefinitionType &def_tstore = table_2write.getTableDefinition();
      DefinitionType &def_conf   = table.getTableDefinition();

      if (DBG)
      {
        std::cout<<"config names: "<<std::endl;
        for (DefinitionType::iterator field = def_conf.begin(); field != def_conf.end(); field++ )  std::cout<<(*field).first<<" "<<(*field).second<<std::endl;
        std::cout<<std::endl;
        std::cout<<"DB names: "<<std::endl;
        for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )  std::cout<<(*tfield).first<<" "<<(*tfield).second<<std::endl;
        std::cout<<std::endl;
      }

      // reserve the container size
      table_2write.reserve(table.getRowCount());

      for (DefinitionType::iterator field = def_conf.begin(); field != def_conf.end(); field++ )
      {
        std::string name = (*field).first;
        std::string datatype = (*field).second;

        std::string name_tstore = "";
        std::string datatype_tstore = "";

        for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )
        {
          /// need assert that table definitions as read from TStore are the same as read from TStore configuration view
          if (toolbox::toupper(name) == toolbox::toupper((*tfield).first))
          {
            name_tstore = (*tfield).first;
            datatype_tstore = (*tfield).second;
            break;
          }
        }
        if (name_tstore.empty())
        {
          std::ostringstream error;
          error<<"Config.view field name -> "<<name<<" is not in "<<std::endl;
          error<<"DB names: "<<std::endl;
          for (DefinitionType::iterator tfield = def_tstore.begin(); tfield != def_tstore.end(); tfield++ )
          {
            error<<(*tfield).first<<" ";
          }
          error<<std::endl;
          XCEPT_RAISE(emu::exception::ConfigurationException, error.str() +
                      "Table definitions (above): as read from TStore != as read from TStore configuration view!!!");
        }

        //      copy the data
        xdata::AbstractVector * v_conf = table.columnData_.find(name)->second;
        for (size_t r = 0; r < table.getRowCount(); r++ )
        {
          xdata::Serializable * s = v_conf->elementAt(r);
          table_2write.setValueAt(r, name_tstore, *s );
        }
      }

      // update ID's in this table with new common configuration ID
      //hierarchy_->updateIds(new_id, table);
      xdata::AbstractVector * ids_own    = table_2write.columnData_.find(hierarchy_->idFieldName(*t))->second;
      xdata::AbstractVector * ids_parent = table_2write.columnData_.find(hierarchy_->idFieldNameOfParent(*t))->second;
      for (size_t r = 0; r < table_2write.getRowCount(); r++ )
      {
        table_2write.setValueAt(r, hierarchy_->idFieldNameOfHead(), new_id);

        // the only ID that head table is supposed to have
        if (*t == head_type) continue;

        xdata::UnsignedInteger64 id_own    = *dynamic_cast<xdata::UnsignedInteger64*>(ids_own->elementAt(r));
        xdata::UnsignedInteger64 id_parent = *dynamic_cast<xdata::UnsignedInteger64*>(ids_parent->elementAt(r));

        xdata::UnsignedInteger64 new_id_own    = hierarchy_->updateConfigId(id_own, new_id);
        xdata::UnsignedInteger64 new_id_parent = hierarchy_->updateConfigId(id_parent, new_id);

        table_2write.setValueAt(r, hierarchy_->idFieldName(*t), new_id_own);
        table_2write.setValueAt(r, hierarchy_->idFieldNameOfParent(*t), new_id_parent);
      }

      std::cout<<"Writing new id="<<new_id.toString()<<"  for "<<*t<<" in subsystem="<<subsystem<<"..."<<std::endl;
      //table_2write.writeTo(std::cout); std::cout<<std::endl;
      tstore.insert(insert_name, table_2write);
    }
  }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Error during writing to DB: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error during writing to DB (unknown exception)");
  }
  tstore.disconnect();

  std::cout<<"Endcap DB write time: "<<timer.sec()<<" sec"<<std::endl;
  return true;
}


bool TStoreReadWriter::write(ConfigTree *tree)
throw (emu::exception::ConfigurationException)
{
  std::map<std::string,ConfigTable> *data = 0;
  if (tree) data = &(tree->datamap());
  return write(data);
}


xdata::UnsignedInteger64 TStoreReadWriter::readLastConfigIdFlashed(const std::string &subsystem)
throw (emu::exception::ConfigurationException)
{
  xdata::UnsignedInteger64 result;

  // find out the query name from the top level table type:
  std::string query_name = toolbox::tolower(hierarchy_->typeOfFlashTable());
  //if (side.empty()) query_name = "latest_" + query_name;
  //else query_name += ;
  query_name = "latest_" + query_name;

  //set up parameter name and value
  std::map< std::string, std::string > parameters;
  if (!subsystem.empty())  parameters["SIDE"] = subsystem;

  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  xdata::Table table;
  try { table = tstore.query(query_name, parameters); }
  catch (emu::exception::DBException &e)
  {
    tstore.disconnect();
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot get last configuration ID flashed: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot get last configuration ID flashed (unknown exception)");
  }
  tstore.disconnect();

  std::string id_field_name = hierarchy_->idFieldNameOfFlashTable();
  result.fromString(table.getValueAt(0, id_field_name)->toString());

  return result;
}


std::vector<ConfigIDInfo> TStoreReadWriter::readFlashIDInfos(const std::string &subsystem)
throw (emu::exception::ConfigurationException)
{
  std::vector<ConfigIDInfo> result;

  // find out the query name from the top level table type:
  std::string query_name = toolbox::tolower(hierarchy_->typeOfFlashTable());
  //if (side.empty()) query_name = "latest_" + query_name;
  //else query_name += ;
  query_name = "list_" + query_name;

  //set up parameter name and value
  std::map< std::string, std::string > parameters;
  if (!subsystem.empty())  parameters["SIDE"] = subsystem;

  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  xdata::Table table;
  try { table = tstore.query(query_name, parameters); }
  catch (emu::exception::DBException &e)
  {
    tstore.disconnect();
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot read flash IDInfos: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot read flash IDInfos (unknown exception)");
  }
  tstore.disconnect();

  for (size_t row = 0; row < table.getRowCount(); ++row)
  {
    result.push_back( ConfigIDInfo(table, row, hierarchy_) );
  }

  return result;
}


std::vector<std::pair< std::string, std::string> > TStoreReadWriter::readFlashList(const std::string &subsystem)
throw (emu::exception::ConfigurationException)
{
  std::vector<std::pair< std::string, std::string> > result;

  std::vector<ConfigIDInfo> id_infos = readFlashIDInfos(subsystem);

  for (size_t i = 0; i < id_infos.size(); ++i)
  {
    result.push_back( std::make_pair(id_infos[i].id(), id_infos[i].timeOfFlash() ));
  }
  return result;
}


bool TStoreReadWriter::writeFlashTime(xdata::UnsignedInteger64 id)
throw (emu::exception::ConfigurationException)
{
  // type-name of flash table
  std::string type = hierarchy_->typeOfFlashTable();
  // get insert query name from the type of table:
  std::string insert_name = toolbox::tolower(type);

  // obtain current time
  xdata::TimeVal   config_time = (xdata::TimeVal) toolbox::TimeVal::gettimeofday();
  xdata::Table table = *(hierarchy_->definitions()->tableDefinition(type));
  table.setValueAt(0, hierarchy_->idFieldNameOfFlashTable(), id);
  table.setValueAt(0, hierarchy_->timeFieldNameOfFlashTable(), config_time);

  // prepare TStoreAgent
  TStoreAgent tstore(application_, hierarchy_->tstoreViewID(), instance_);
  try  { tstore.connect(db_credentials_); }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Cannot connect to TStore: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot connect to TStore (unknown exception)");
  }

  xdata::Table dtable = tstore.definition(type);
  dtable.setValueAt(0, toolbox::toupper(hierarchy_->idFieldNameOfFlashTable()), id);
  dtable.setValueAt(0, toolbox::toupper(hierarchy_->timeFieldNameOfFlashTable()), config_time);

  std::cout<<"Config def: "<<std::endl;
  table.writeTo(std::cout); std::cout<<std::endl;
  std::cout<<"TStore def: "<<std::endl;
  dtable.writeTo(std::cout); std::cout<<std::endl;

  try
  {
    std::cout<<"Writing firmware flash timestamp into "<<type<<": id "<<id.toString()<<"  time "<<config_time.toString()<<std::endl;
    dtable.writeTo(std::cout); std::cout<<std::endl;
    tstore.insert(insert_name, dtable);
  }
  catch (emu::exception::DBException &e)
  {
    XCEPT_RETHROW(emu::exception::ConfigurationException, "Error during writing flash time to DB: " + std::string(e.what()), e);
  }
  catch (...)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error during writing flash time to DB (unknown exception)");
  }
  tstore.disconnect();

  return true;
}


}}
