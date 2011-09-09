#ifndef __EMU_DB_CONFIGTREE_H__
#define __EMU_DB_CONFIGTREE_H__

#include "emu/exception/Exception.h"
#include "emu/db/ConfigTable.h"
#include "emu/db/ConfigRow.h"
#include "emu/db/ConfigHierarchy.h"

#include <vector>
#include <map>
#include <string>

namespace emu { namespace db {


/** @class ConfigTree
 *
 * A data container which maps the tree-like hierarchy of the configuration data  over the data tables,
 * and provides convenient interfaces for access.
 * Each row of a each data table becomes referenced by a tree node of ConfigRow type.
 * The parent-children relationships of ConfigRow's are filled during ConfigTree construction using appropriate ID fields.
 */
class ConfigTree
{
public:
  /** Constructors
  * @param datamap      the map of data stored in ConfigTables with their table type used as a key.
  *                     It has to have at least one table!
  *                     All ConfigTables have to have the same ConfigHierarchy pointer!
  *
  * Note: the tables are copied to the datamap_ class member.
  * To keep memory in neat state, the original datamap that was used as an argument, might need to be cleared.
  */
  ConfigTree(std::map<std::string, ConfigTable> &datamap);
  
  /** Copy constructor
   *  creates a deep copy
   */
  ConfigTree(const ConfigTree &t);

  /** Destructor
   * deletes all the nodes.
   */
  ~ConfigTree();

  // ----- accessors -----

  /** pointer to the hierarchy object  */
  const ConfigHierarchy * hierarchy() const {return hierarchy_;}

  /** head node  */
  ConfigRow* head() {return head_;}

  /** current ConfigTable data map */
  std::map<std::string, ConfigTable> & datamap() { return datamap_;}

  /** retrieve all nodes of a type as a Table
   * Trivial Table is returned if there's no nodes of "type".
   */
  xdata::Table table(const std::string &type);

  /** retrieve all nodes of a type  */
  std::vector<ConfigRow*> find(const std::string &type);

  /** find a node by its type and id  */
  ConfigRow* find(const std::string &type, const xdata::UnsignedInteger64 &id);

  /** Initialize tree with new data.
   * @param datamap      the map of data stored in ConfigTables with their table type used as a key.
   *                     It has to have at least one table!
   *                     All ConfigTables have to have the same ConfigHierarchy pointer!
   *
   * Note: the tables are copied to the datamap_ class member.
   * To keep memory in neat state, the original datamap that was used as an argument, might need to be cleared.
   */
  void fillTreeData(std::map<std::string, ConfigTable> &datamap) throw (emu::exception::ConfigurationException);

  /** Iterators for traversing a tree:
   * start from the head node and follow the order in which the nodes were added
   */
  typedef std::vector<ConfigRow*>::const_iterator const_iterator;
  const_iterator begin() const { return order_.begin() ;}
  const_iterator end() const { return order_.end() ;}

  typedef std::vector<ConfigRow*>::iterator iterator;
  iterator begin() { return order_.begin() ;}
  iterator end() { return order_.end() ;}

  /** Comparison functor required to make maps with xdata::UnsignedInteger64 keys.
   *  The xdata::UnsignedInteger64 doesn't have const version of comparison operators that is needed by map
   **/
  struct CompareUnsignedInteger64
  {
    bool operator() (const xdata::UnsignedInteger64 &lhs, const xdata::UnsignedInteger64 &rhs) const
    {
      return *(const_cast<xdata::UnsignedInteger64 *>(&lhs)) < *(const_cast<xdata::UnsignedInteger64 *>(&rhs));
    }
  };

protected:

  // ----- modifiers -----

  /** Set head node
   * warning: this method clears/deletes all the other tree nodes!
   */
  void setHead(ConfigRow* head);

  /** creates ConfigRow objects for all table rows during initialization */
  void createConfigRows();

  /** recursively (type by type) fills the parent-child relationships during initialization */
  void recursiveTreeFill(const std::string &parent_type);

  /** Add a node (used by recursiveTreeFill)
   * Node's parent has to be already present in the tree!
   * \returns if insertion was successful
   */
  bool addNode(ConfigRow* node);

  /** call delete for all tree nodes  */
  void clear();

  // ----- members -----

  /** keeps a copy of configuration tables accessible by a type key */
  std::map<std::string, ConfigTable> datamap_;

  /** pointer to a system hierarchy object */
  ConfigHierarchy* hierarchy_;

  /** the head node of a tree  */
  ConfigRow* head_;

  /** Internal data as a lookup table for speeding up the access.
   * index of the outer map is type
   * index of the inner map is uint64 id
   */
  typedef std::map<xdata::UnsignedInteger64, ConfigRow*, CompareUnsignedInteger64 > MapUnsignedInteger64;
  std::map<std::string, MapUnsignedInteger64 > data_;

  /** Maintain this vector in parallel with the map.
   * The order of elements is the order in which the nodes were added.
   * It is useful for easy tree traversal with iterators.
   */
  std::vector<ConfigRow*> order_;

  /** temporary container used for the initial tree filling */
  std::map< std::string, std::vector<ConfigRow * > > config_raw_map_;
};

}}

#endif
