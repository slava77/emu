// $Id: PCConfigHierarchy.cc,v 1.3 2012/04/30 23:43:56 khotilov Exp $

#include "emu/db/PCConfigHierarchy.h"
#include "emu/db/ConfigTable.h"

#include "toolbox/string.h"


namespace emu { namespace db {

// static members definitions:

std::string PCConfigHierarchy::prefix_ = "EMU_"; // initialize only this one here
// all the other are initialized later in init()
std::string PCConfigHierarchy::head_;
std::string PCConfigHierarchy::head_xml_;
std::vector<std::string> PCConfigHierarchy::types_;
std::map<std::string, std::vector<std::string> > PCConfigHierarchy::typeChildren_;
std::map<std::string, std::string > PCConfigHierarchy::typeParent_;
std::string PCConfigHierarchy::idFieldNameHead_;
std::map<std::string, std::string > PCConfigHierarchy::idFieldNames_;
std::string PCConfigHierarchy::timeFieldName_;
std::string PCConfigHierarchy::typeOfFlashTable_;
std::string PCConfigHierarchy::idFieldNameOfFlashTable_;
std::string PCConfigHierarchy::timeFieldNameOfFlashTable_;
std::string PCConfigHierarchy::descriptionFieldName_;


PCConfigHierarchy::PCConfigHierarchy(const std::string & tstore_conf, TruncateLevel truncate_level)
: ConfigHierarchy()
, has_DDU_(0)
{
  def_ = new TableDefinitions(tstore_conf, prefix_);
  if (!def_->valid())
  {
    std::cout<<std::endl<<" * * * * * * WARNING: TableDefinitions is not vaild!!! * * * * * *"<<std::endl<<std::endl;
  }

  // for the test-stand configuration, TStore configuration must have DDU table definition
  if (def_->tableDefinition("DDU") != 0)  has_DDU_ = true;
  if (has_DDU_) setNotForDB();

  truncate_level_ = truncate_level;
  init();
}


PCConfigHierarchy::PCConfigHierarchy()
: ConfigHierarchy()
{
  init();
}


PCConfigHierarchy::~PCConfigHierarchy()
{
  if (def_) delete def_;
}


std::string PCConfigHierarchy::prefix() const
{
  if(def_) return def_->prefix();
  return prefix_;
}


std::vector<std::string> PCConfigHierarchy::typesOfChildern(const std::string & type) const
{
  return typeChildren_[type];
}


std::string PCConfigHierarchy::typeOfParent(const std::string & type) const
{
  return typeParent_[type];
}


std::string PCConfigHierarchy::idFieldName(const std::string & type) const
{
  return idFieldNames_[type];
}


std::string PCConfigHierarchy::idFieldNameOfParent(const std::string &type) const
{
  std::string parent_type = typeOfParent(type);
  if (parent_type.empty()) return "";
  return idFieldName(parent_type);
}


std::map<std::string, ConfigTable> PCConfigHierarchy::definitionsTree()
{
  std::map<std::string, ConfigTable> result;

  ConfigTable head_table(head_, this);
  head_table = *(def_->tableDefinition(head_));
  result[head_] = head_table;

  definitionsTreeRecursive(head_, result);

  return result;
}


void PCConfigHierarchy::definitionsTreeRecursive(const std::string &type, std::map<std::string, ConfigTable> &table_tree)
{
  std::vector<std::string> kid_types = typesOfChildern(type);
  std::vector< std::string >::iterator it = kid_types.begin();
  for (; it != kid_types.end(); it++)
  {
    std::string kid_type = *it;
    ConfigTable t(kid_type, this);
    t = *(def_->tableDefinition(kid_type));
    table_tree[kid_type] = t;
    table_tree[type].addChild(&(table_tree[kid_type]));
  }
  // do the recursion in a second loop to preserve the order
  for (it = kid_types.begin(); it != kid_types.end(); it++)
  {
    definitionsTreeRecursive(*it, table_tree);
  }
}


xdata::UnsignedInteger64 PCConfigHierarchy::id(const std::string &type, xdata::UnsignedInteger64 &parent_id, xdata::UnsignedInteger64 &number)
{
  if (type == "Configuration") return number;
  if (type == "PeripheralCrate") return parent_id * 100000000 + (number + 1) * 1000000;
  if (type == "CCB")   return parent_id + 110000;
  if (type == "MPC")   return parent_id + 120000;
  if (type == "VCC")   return parent_id + 130000;
  if (type == "CSC")   return parent_id + (number + 1) * 10000;
  if (type == "DAQMB") return parent_id + 1000;
  if (type == "CFEB")  return parent_id + number;
  if (type == "TMB")   return parent_id + 2000;
  if (type == "ALCT")  return parent_id + 100;
  if (type == "AnodeChannel") return parent_id + (number+1);
  if (type == "DDU")   return parent_id + 140000;
  return 0;
}


xdata::UnsignedInteger64 PCConfigHierarchy::updateConfigId(xdata::UnsignedInteger64 &id_to_update, xdata::UnsignedInteger64 &new_config_id) const
{
  xdata::UnsignedInteger64 result = id_to_update;

  std::string str_val = id_to_update.toString();
  size_t len = str_val.length();

  std::string str_new = new_config_id.toString();
  size_t len_new = str_new.length();

  long unsigned int val_new = new_config_id;

  if (len < 7 ) return false;

  // always require new CONFIGURATION ID to be 7 digits!
  if (len_new != 7 )
  {
    std::cout<<"WARNING:updateConfigId: new configuration ID has more then 7 digits: "<<new_config_id<<std::endl;
    return false;
  }
  str_val.replace(0, 7, toolbox::toString("%07d",val_new));

  result.fromString(str_val);

  return result;
}


std::string PCConfigHierarchy::determineSubSystem(ConfigTable & table) const
{
  if (table.getRowCount() == 0) return "";

  // for PCrate tables, first digit of config id is either 1 (plus) or 2 (minus endcap)
  xdata::UnsignedInteger64 id = table.getCastValueAt<xdata::UnsignedInteger64>(0, idFieldNameOfHead());
  std::string str_id = id.toString();
  if (str_id.substr(0,1) == "1") return "plus";
  if (str_id.substr(0,1) == "2") return "minus";
  return "";
}


xdata::UnsignedInteger64 PCConfigHierarchy::defaultIdForSubsystem(const std::string &subsystem) const
{
  if (subsystem == "plus") return 1000000;
  if (subsystem == "minus") return 2000000;
  return 0;
}


bool PCConfigHierarchy::isFieldDBOnly(const std::string &type, const std::string &field_name) const
{
  // all id fields are DB only
  if (field_name.find("config_id") != std::string::npos) return true;
  // currently, no XML attributes are written from "Configuration" fields
  if (type == "Configuration") return true;
  //if (type == "Configuration" && field_name != "emu_endcap_side") return true;
  return false;
}


void PCConfigHierarchy::init()
{
  int tr = truncate_level_;

  // initialize the statics
  if (types_.empty())
  {
    head_ = "Configuration";
    head_xml_ = "EmuSystem";

    types_.push_back("Configuration");
    if (tr > 0) types_.push_back("PeripheralCrate");
    if (tr > 1) types_.push_back("CCB");
    if (tr > 1) types_.push_back("MPC");
    if (tr > 1) types_.push_back("VCC");
    if (tr > 1) types_.push_back("CSC");
    if (tr > 1 && has_DDU_) types_.push_back("DDU");
    if (tr > 2) types_.push_back("DAQMB");
    if (tr > 3) types_.push_back("CFEB");
    if (tr > 2) types_.push_back("TMB");
    if (tr > 3) types_.push_back("ALCT");
    if (tr > 4) types_.push_back("AnodeChannel");

    // define the parent-children hierarchy:
    std::vector<std::string> emptyVec;
    typeChildren_["Configuration"] = emptyVec;
    if (tr > 0) addChildTypeName("Configuration", "PeripheralCrate");
    if (tr > 1) addChildTypeName("PeripheralCrate", "CCB");
    if (tr > 1) addChildTypeName("PeripheralCrate", "MPC");
    if (tr > 1) addChildTypeName("PeripheralCrate", "VCC");
    if (tr > 1) addChildTypeName("PeripheralCrate", "CSC");
    if (tr > 1 && has_DDU_) addChildTypeName("PeripheralCrate", "DDU");
    if (tr > 2) addChildTypeName("CSC", "DAQMB");
    if (tr > 3) addChildTypeName("DAQMB", "CFEB");
    if (tr > 2) addChildTypeName("CSC", "TMB");
    if (tr > 3) addChildTypeName("TMB", "ALCT");
    if (tr > 4) addChildTypeName("ALCT", "AnodeChannel");

    typeParent_["Configuration"] = "";
    if (tr > 0) typeParent_["PeripheralCrate"] = "Configuration";
    if (tr > 1) typeParent_["CCB"] = "PeripheralCrate";
    if (tr > 1) typeParent_["MPC"] = "PeripheralCrate";
    if (tr > 1) typeParent_["VCC"] = "PeripheralCrate";
    if (tr > 1) typeParent_["CSC"] = "PeripheralCrate";
    if (tr > 1 && has_DDU_) typeParent_["DDU"] = "PeripheralCrate";
    if (tr > 2) typeParent_["DAQMB"] = "CSC";
    if (tr > 3) typeParent_["CFEB"] = "DAQMB";
    if (tr > 2) typeParent_["TMB"] = "CSC";
    if (tr > 3) typeParent_["ALCT"] = "TMB";
    if (tr > 4) typeParent_["AnodeChannel"] = "ALCT";

    idFieldNameHead_ = "emu_config_id";
    idFieldNames_["Configuration"] = "emu_config_id";
    if (tr > 0) idFieldNames_["PeripheralCrate"] = "periph_config_id";
    if (tr > 1) idFieldNames_["CCB"] = "ccb_config_id";
    if (tr > 1) idFieldNames_["MPC"] = "mpc_config_id";
    if (tr > 1) idFieldNames_["VCC"] = "vcc_config_id";
    if (tr > 1) idFieldNames_["CSC"] = "csc_config_id";
    if (tr > 1 && has_DDU_) idFieldNames_["DDU"] = "ddu_config_id";
    if (tr > 2) idFieldNames_["DAQMB"] = "daqmb_config_id";
    if (tr > 3) idFieldNames_["CFEB"] = "cfeb_config_id";
    if (tr > 2) idFieldNames_["TMB"] = "tmb_config_id";
    if (tr > 3) idFieldNames_["ALCT"] = "alct_config_id";
    if (tr > 4) idFieldNames_["AnodeChannel"] = "afeb_config_id";

    timeFieldName_ = "emu_config_time";
    descriptionFieldName_ = "description";

    typeOfFlashTable_ = "FLASH_WRITE";
    idFieldNameOfFlashTable_ = "config_id";
    timeFieldNameOfFlashTable_ = "write_date";

    // Make sure that the hardcoded types are the same as in definitions that were read in from TStore configuration
    if (def_)
    {
      std::vector<std::string> def_types = def_->types();
      for (std::vector< std::string >::iterator t = types_.begin(); t != types_.end(); t++)
      {
        bool found = false;
        for (std::vector< std::string >::iterator dt = def_types.begin(); dt != def_types.end(); dt++)
        {
          if (toolbox::tolower(*t) == toolbox::tolower(*dt))
          {
            found = true;
            if (*t != *dt) {
              std::cout<<std::endl<<" * * * WARNING: hardcoded "<<*t<<" type has different case then "<<*dt<<" !!! * * *"<<std::endl<<std::endl;
            }
          }
        }
        if (!found)
        {
          std::cout<<std::endl<<" * * * WARNING: hardcoded "<<*t<<" type was not found in TableDefinitions!!! * * *"<<std::endl<<std::endl;
        }
      }
    }
  }
}


void PCConfigHierarchy::addChildTypeName(const std::string &parent, const std::string &child)
{
  typeChildren_[parent].push_back(child);
  std::vector<std::string> emptyVec;
  typeChildren_[child] = emptyVec;
}


}}
