// $Id: ConfigTree.cc,v 1.1 2011/09/09 16:04:44 khotilov Exp $

#include "emu/db/ConfigTree.h"
#include "emu/utils/SimpleTimer.h"

#include "toolbox/string.h"
#include "xdata/TableIterator.h"

namespace emu { namespace db {


ConfigTree::ConfigTree(std::map<std::string, ConfigTable> &datamap)
: hierarchy_(0)
{
  emu::utils::SimpleTimer timer;

  fillTreeData(datamap);

  std::cout<<"ConfigTree constructed in "<<timer.sec()<<" sec"<<std::endl;
}


ConfigTree::ConfigTree(const ConfigTree &t)
{
  ConfigRow *head = new ConfigRow(*t.head_);
  setHead(head);

  // set up head of the container
  MapUnsignedInteger64 innermap;
  innermap[head->id()] = head;
  data_[head->type()] = innermap;
}


ConfigTree::~ConfigTree()
{
  clear();
}


xdata::Table ConfigTree::table(const std::string &type)
{
  xdata::Table result;
  if (datamap_.find(type) == datamap_.end()) return result;
  return datamap_[type];
}


std::vector<ConfigRow*> ConfigTree::find(const std::string &type)
{
  std::vector<ConfigRow*> nodes;
  if (data_.count(type) == 0) return nodes;
  MapUnsignedInteger64::iterator innermap_it = data_[type].begin();
  for (; innermap_it != data_[type].end(); innermap_it++)
  {
    nodes.push_back(innermap_it->second);
  }
  return nodes;
}


ConfigRow* ConfigTree::find(const std::string &type, const xdata::UnsignedInteger64 &id)
{
  ConfigRow* node = 0;
  if (data_.count(type) == 0) return node;
  if ( (data_[type]).count(id) == 0) return node;
  return (data_[type])[id];
}


void ConfigTree::fillTreeData(std::map<std::string, ConfigTable> &datamap)
throw (emu::exception::ConfigurationException)
{
  // clean up
  clear();

  // copy data to the local datamap_
  for(std::map<std::string, ConfigTable>::iterator it = datamap.begin(); it != datamap.end(); it++)
  {
    std::cout<<"CT: type: "<<it->first<<std::endl;
    datamap_[it->first] = it->second;
    // check that all the tables have *the same* hierarchy pointer
    if (hierarchy_ != 0 && it->second.hierarchy() != hierarchy_)
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "ConfigTree constructor: ConfigTables have multiple hierarchy pointers!");
    }
    // update hierarchy_
    hierarchy_ = it->second.hierarchy();
  }
  std::cout<<"CT: filled"<<std::endl;

  // pick up hierarchy pointer from the first table (assume that they all have the same )

  // get the configuration head
  const std::string head_type = hierarchy_->typeOfHead();
  if (datamap_.find(head_type) == datamap_.end())
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "ConfigTree constructor: no top level table of type " + head_type);
  }

  if (datamap_[head_type].getRowCount() != 1)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "ConfigTree constructor: top level has !=1 rows: " +
                toolbox::toString("%d", datamap_[head_type].getRowCount()));
  }

  //xdata::UnsignedInteger64 config_id = head_row->getField(hierarchy->idFieldNameOfHead());
  //config_id.toString();

  ConfigRow * head = new ConfigRow(head_type, &(datamap_[head_type]), 0);
  setHead(head);
  std::cout<<"CT: head set"<<std::endl;

  // create ConfigRow objects for all table rows
  createConfigRows();

  // recursively (type by type) fill the parent-child relationships
  recursiveTreeFill(head_type);
}


void ConfigTree::setHead(ConfigRow* head)
{
  // all tree is erased first!
  if (head_) clear();

  head_ = head;

  if (head == 0) return;

  MapUnsignedInteger64 innermap;
  innermap[head->id()] = head;
  data_[head->type()] = innermap;
  order_.push_back(head);
}


void ConfigTree::createConfigRows()
{
  // do it for all table types except Configuration (as we have already made a head node out of it):
  std::vector<std::string> types = hierarchy_->types();
  for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); it++)
  {
    std::string t = *it;
    if (toolbox::tolower(t) == "configuration") continue;

    std::vector<ConfigRow * > empty_vec;
    config_raw_map_[*it] = empty_vec;
    for (size_t r = 0; r < datamap_[t].getRowCount(); r++)
    {
      ConfigRow * config_row = new ConfigRow(t, &(datamap_[t]), r);
      config_raw_map_[t].push_back(config_row);
    }
  }
}


void ConfigTree::recursiveTreeFill(const std::string &parent_type)
{
  // do it by types of children:
  std::vector<std::string> children_types = hierarchy_->typesOfChildern(parent_type);
  for (std::vector<std::string>::iterator it = children_types.begin(); it != children_types.end(); it++)
  {
    std::string child_type = *it;
    // add all children of this type
    std::vector<ConfigRow * >::iterator ir =  config_raw_map_[child_type].begin();
    while (ir != config_raw_map_[child_type].end())
    {
      if (addNode(*ir))
      {
        ir = config_raw_map_[child_type].erase(ir);
      }
      else
      {
        std::cout<<" * * * * * * WARNING: couldn't add the following node * * * * * *"<<std::endl;
        std::cout<<*ir<<std::endl;
        ir++;
      }
    }
    // recursively add children's children types
    recursiveTreeFill(child_type);
  }
}


bool ConfigTree::addNode(ConfigRow* node)
{
  // cannot add a node with no parent already present in the tree!
  ConfigRow* parent = find(hierarchy_->typeOfParent(node->type()), node->parentId());
  if (!parent) return false;

  // do not allow to add a node that already exists
  if ( find(node->type(), node->id()) ) return false;

  // append to data_ container
  if (data_.count(node->type()))
  {
    (data_[node->type()])[node->id()] = node;
  }
  else
  {
    MapUnsignedInteger64 innermap;
    innermap[node->id()] = node;
    data_[node->type()] = innermap;
  }
  // keep the order_ in parallel as well
  order_.push_back(node);

  // update parent-kids links
  parent->addChild(node);
  node->setParent(parent);

  return true;
}


void ConfigTree::clear()
{
  //return erase(head_->type(), head_->id());
  for (std::vector<ConfigRow*>::iterator n = order_.begin(); n != order_.end(); n++)  delete *n;
  order_.clear();
  data_.clear();
}

}}
