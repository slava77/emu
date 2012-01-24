#ifndef __EMU_DB_XMLREADWRITER_H__
#define __EMU_DB_XMLREADWRITER_H__

#include <map>
#include <string>

#include "emu/db/ConfigReadWriter.h"
#include "emu/db/TranslatorKludgeXMLvsDB.h"
#include "emu/utils/SerializableFactory.h"

#include "xdata/UnsignedInteger64.h"

#include "xercesc/parsers/XercesDOMParser.hpp"

namespace emu {	namespace db {

/** @class XMLReadWriter
 *  An implementation of XML configuration reader/writer
 */
class XMLReadWriter : public ConfigReadWriter
{
public:

  /** Constructor
   * @hierarchy            Pointer to the object describing tables hierarchy.
   * @filename             XML file location.
   **/
  XMLReadWriter(ConfigHierarchy* hierarchy, const std::string &filename);

  /// Default destructor
  virtual ~XMLReadWriter() {};

  /** Change the name of XML file.
   * Is useful, e.g., when you read configuration from one file, and then want to write to a different one.
   */
  void setFileName(const std::string &filename) { filename_ = filename; }

  /** Read the configuration
   * @param id    configuration ID to assign to read in data
   * Note that it is sometimes important to follow conventions for configuration ID.
   * E.g., for PCrates, configuration ID is expected to be 7 digits with the 1st digit encoding the endcap side (1="plus", 2="minus").
   * And for PCrates the endcap side would matter when you would want to read configuration from XML and to write it into DB.
   */
  virtual bool read(xdata::UnsignedInteger64 id) throw (emu::exception::ConfigurationException);

  /** Write the configuration
   * \param data  a pointer to the ConfigTables map to write; if NULL, writes the current datamap_
   * Note: this function construct a ConfigTree internally. So if ConfigTree was already created, use write(ConfigTree *tree).
   */
  virtual bool write(std::map<std::string, ConfigTable> *data = 0) throw (emu::exception::ConfigurationException);

  /** Write the configuration
   * \param tree  a pointer to the ConfigTree to write; if NULL, writes ConfigTree filled from the current datamap_
   */
  virtual bool write(ConfigTree *tree) throw (emu::exception::ConfigurationException);

  /** Make XML configuration as a std::string
   * \param tree  a pointer to the ConfigTree to use; if NULL, uses ConfigTree filled from the current datamap_
   */
  std::string makeXML(ConfigTree *tree = 0) throw (emu::exception::ConfigurationException);

private:

  void cleanUpParserAndRaise(const std::string &msg) throw (emu::exception::ConfigurationException);

  /// recursively parse XML nodes
  void parseNode(xercesc::DOMNode *node, xdata::UnsignedInteger64 &parent_id, xdata::UnsignedInteger64 &number) throw (emu::exception::ConfigurationException);

  /// recursively fill XML node from a row of configuration
  void fillNode(xercesc::DOMNode *node, ConfigRow *row) throw (emu::exception::ConfigurationException);

  /// XML file location
  std::string filename_;

  /// Parser instance
  xercesc::XercesDOMParser *parser_;

  /// DOM document for making XML
  xercesc::DOMDocument *document_;

  /// Converter to Serializable
  emu::utils::SerializableFactory to_serializable_;

  /// Configuration ID to assign during reading. Default is 999.
  xdata::UnsignedInteger64 config_id_;

  /// A kludge to provide XML <-> DB correspondence
  TranslatorKludgeXMLvsDB translatorKludge_;
};

}}

#endif
