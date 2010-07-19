///
/// @file   Attributes.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 08:46:40 2010
/// 
/// @brief  A container class to pass multiple SOAP attributes and attributes in.
/// 
/// 
///
#ifndef __emu_soap_Attributes_h__
#define __emu_soap_Attributes_h__

#include <sstream>
#include <map>
#include "xdata/Serializable.h"

namespace emu{
  namespace soap{
    using namespace std;

    ///
    /// A container class to pass multiple SOAP attributes in.
    ///
    class Attributes : public map< string, xdata::Serializable* >{
    public:

      /// 
      /// Default ctor.
      ///
      Attributes();

      /// 
      /// Insertion operator for formatted text output.
      /// @param os The \c ostream object.
      /// @param attributes The \c Attributes object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os,  const emu::soap::Attributes& attributes );

      ///
      /// Sets whether namespace prefix is to be used.
      ///
      /// @param usePrefix True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      /// @return Ref. to the Attributes object itself.
      ///
      emu::soap::Attributes& setUsePrefix( bool usePrefix );

      /// 
      /// Gets whether namespace prefix is to be used.
      ///
      /// @return True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      bool getUsePrefix() const;

      /// 
      /// Adds an \c xdata object and its name.
      /// @param name The name of the data.
      /// @param value Pointer to the \c xdata object.
      ///
      /// @return Ref. to the Attributes object itself. 
      ///
      emu::soap::Attributes& add( const std::string &name, xdata::Serializable* value );

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

    private:
      bool usePrefix_; /// If true, the namespace prefix of the parent element will be used. Otherwise no namespace prefix will be used.
    };

  }
}

#endif
