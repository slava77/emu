#ifndef __emu_utils_SerializableFactory_h__
#define __emu_utils_SerializableFactory_h__

#include <string>
#include <map>

#include "xdata/Serializable.h"

namespace emu { namespace utils {


/** class SerializableFactory
 * Either I didn't notice it or xdaq does not provide convenient generic means to get a pointer
 * to Serializable by its type and string value.
 * This factory class serves exactly this purpose.
 */
class SerializableFactory
{
public:
  /// Constructor. It just fills the lookup table.
  SerializableFactory();

  /// The main factory method.
  /// Caller is responsible for deleting the returned object.
  xdata::Serializable * operator()(const std::string & type, const std::string & value);

private:

  /// the "meaningful" index for the type LUT
  enum SerializableIndex {
    UNSIGNEDLONG, UNSIGNEDINTEGER, UNSIGNEDINTEGER32, UNSIGNEDINTEGER64, UNSIGNEDSHORT,
    FLOAT, INTEGER, INTEGER32, INTEGER64, DOUBLE, BOOLEAN, TIMEVAL, STRING};

  /// type look-up table
  /// TODO: replace later with a faster hash-based unorderd_map when completely moved to gcc4
  std::map<std::string, SerializableIndex> lut_;
};

}}

#endif
