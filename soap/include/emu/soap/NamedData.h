///
/// @file   NamedData.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 08:46:40 2010
/// 
/// @brief  A container class to pass multiple SOAP parameters and attributes in.
/// 
/// 
///
#ifndef __emu_soap_NamedData_h__
#define __emu_soap_NamedData_h__

#include <sstream>
#include <map>
#include "xdata/Serializable.h"

namespace emu{
  namespace soap{
    using namespace std;

    ///
    /// A container class to pass multiple SOAP parameters and attributes in.
    ///
    class NamedData : public map< string, xdata::Serializable* >{
    public:
      /// 
      /// Insertion operator for formatted text output.
      /// @param os The \c ostream object.
      /// @param parameters The \c NamedData object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os,  const emu::soap::NamedData& parameters );

      /// 
      /// Adds an \c xdata object and its name.
      /// @param name The name of the data.
      /// @param value Pointer to the \c xdata object.
      ///
      /// @return Ref. to the NamedData object itself. 
      ///
      emu::soap::NamedData& add( const std::string &name, xdata::Serializable* value );

      /// 
      /// Gets the type of data named \c name .
      /// @param name Name of the data.
      ///
      /// @return Type of the data as returned by \c xdata.
      ///
      string getType( const string &name ) const;

      /// 
      /// Gets the \c xdata object holding data named \c name .
      /// @param name Name of the data.
      ///
      /// @return Pointer to the \c xdata object holding data named \c name .
      ///
      xdata::Serializable* getValue( const string &name );
    };

  }
}

#endif
