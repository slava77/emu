///
/// @file   Parameters.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 08:46:40 2010
/// 
/// @brief  A container class to pass multiple SOAP parameters with their attributes in.
/// 
/// 
///
#ifndef __emu_soap_Parameters_h__
#define __emu_soap_Parameters_h__

#include <sstream>
#include <map>
#include "xdata/Serializable.h"
#include "emu/soap/Attributes.h"

namespace emu{
  namespace soap{
    using namespace std;

    ///
    /// A container class to pass multiple SOAP parameters with their attributes in.
    ///
    class Parameters : public map< string, pair< xdata::Serializable*, const emu::soap::Attributes* > >{
    public:

      /// 
      /// Default ctor.
      ///
      Parameters();

      /// 
      /// Insertion operator for formatted text output.
      /// @param os The \c ostream object.
      /// @param parameters The \c Parameters object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os,  const emu::soap::Parameters& parameters );

      ///
      /// Sets whether namespace prefix is to be used.
      ///
      /// @param usePrefix True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      /// @return Ref. to the Parameters object itself.
      ///
      emu::soap::Parameters& setUsePrefix( bool usePrefix );

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
      /// @param attributes Pointer to the \c emu::soap::Attributes object.
      ///
      /// @return Ref. to the Parameters object itself. 
      ///
      emu::soap::Parameters& add( const std::string &name, xdata::Serializable* value, emu::soap::Attributes* attributes=NULL );

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

      /// 
      /// Gets the attributes of the named parameter.
      /// @param name Name of parameter.
      ///
      /// @return Pointer to the attributes of the named parameter.
      ///
      const emu::soap::Attributes* getAttributes( const string &name ) const;

      static const emu::soap::Parameters none; ///< An empty container of parameters.

    private:
      bool usePrefix_; ///< If true, the namespace prefix of the parent element will be used. Otherwise no namespace prefix will be used.
    };

  }
}

#endif
