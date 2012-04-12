// $Id: SerializableFactory.cc,v 1.2 2012/04/12 05:26:53 khotilov Exp $

#include "emu/utils/SerializableFactory.h"
#include "emu/exception/Exception.h"

#include "xdata/Table.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdata/TimeVal.h"
#include "toolbox/string.h"

#include <iostream>

namespace emu { namespace utils {

SerializableFactory::SerializableFactory()
{
  lut_.clear();
  lut_["unsigned long"] =   UNSIGNEDLONG;
  lut_["unsigned int"] =    UNSIGNEDINTEGER;
  lut_["unsigned int 32"] = UNSIGNEDINTEGER32;
  lut_["unsigned int 64"] = UNSIGNEDINTEGER64;
  lut_["unsigned short"] =  UNSIGNEDSHORT;
  lut_["float"] =           FLOAT;
  lut_["int"] =             INTEGER;
  lut_["int 32"] =          INTEGER32;
  lut_["int 64"] =          INTEGER64;
  lut_["double"] =          DOUBLE;
  lut_["bool"] =            BOOLEAN;
  lut_["time"] =            TIMEVAL;
  lut_["string"] =          STRING;
}


xdata::Serializable * SerializableFactory::operator()(const std::string & type, const std::string & value)
{
  xdata::Serializable * result = 0;

  std::map<std::string, SerializableIndex>::iterator idx = lut_.find(toolbox::tolower(type));
  if (idx == lut_.end()) return result;

  try
  {
    switch ((*idx).second)
    {
      case UNSIGNEDLONG:
        result = new xdata::UnsignedLong();
        result->fromString(value);
        return result;
      case UNSIGNEDINTEGER:
        result = new xdata::Integer();
        result->fromString(value);
        return result;
      case UNSIGNEDINTEGER32:
        result = new xdata::UnsignedInteger32();
        result->fromString(value);
        return result;
      case UNSIGNEDINTEGER64:
        result = new xdata::UnsignedInteger64();
        result->fromString(value);
        return result;
      case UNSIGNEDSHORT:
        result = new xdata::UnsignedShort();
        result->fromString(value);
        return result;
      case FLOAT:
        result = new xdata::Float();
        result->fromString(value);
        return result;
      case INTEGER:
        result = new xdata::Integer();
        result->fromString(value);
        return result;
      case INTEGER32:
        result = new xdata::Integer32();
        result->fromString(value);
        return result;
      case INTEGER64:
        result = new xdata::Integer64();
        result->fromString(value);
        return result;
      case DOUBLE:
        result = new xdata::Double(value);
        result->fromString(value);
        return result;
      case BOOLEAN:
        return new xdata::Boolean(value);
      case TIMEVAL:
        return new xdata::TimeVal(value);
      case STRING:
        return new xdata::String(value);
      default: return result;
    }
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RETHROW(xcept::Exception, "toSerializable: Cannot convert " + value + " to type " + type + " : " + e.what(), e);
  }
}

}}
