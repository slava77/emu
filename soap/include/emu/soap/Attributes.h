///
/// @file   Attributes.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 08:46:40 2010
/// 
/// @brief  A container class to pass multiple SOAP attributes in.
/// 
/// 
///
#ifndef __emu_soap_Attributes_h__
#define __emu_soap_Attributes_h__

#include <sstream>
#include <map>
#include "xdata/Serializable.h"
#include "emu/soap/QualifiedName.h"

namespace emu{
  namespace soap{
    using namespace std;

    class Attributes;

    /// 
    /// Insertion operator for formatted text output.
    /// @param os The \c ostream object.
    /// @param attributes The \c Attributes object to be serialized.
    ///
    /// @return Ref. to the \c ostream object itself.
    ///
    ostream& operator<<( ostream& os, const emu::soap::Attributes& attributes );

    ///
    /// A container class to pass multiple SOAP attributes in.
    ///
    class Attributes : public map< QualifiedName, xdata::Serializable*, less<QualifiedName> >{
    public:

      /// 
      /// Default ctor.
      ///
      Attributes();

      /// 
      /// Insertion operator for formatted text output is our friend.
      /// @param os The \c ostream object.
      /// @param attributes The \c Attributes object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os, const emu::soap::Attributes& attributes );

      ///
      /// Sets whether namespace prefix is to be used.
      ///
      /// @param usePrefixOfParent True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      /// @return Ref. to the Attributes object itself.
      ///
      emu::soap::Attributes& setUsePrefixOfParent( bool usePrefixOfParent );

      /// 
      /// Gets whether namespace prefix is to be used.
      ///
      /// @return True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      bool getUsePrefixOfParent() const;

      /// 
      /// Adds an \c xdata object and its name.
      /// @param name Qualified XML name of the data.
      /// @param value Pointer to the \c xdata object.
      ///
      /// @return Ref. to the Attributes object itself. 
      ///
      emu::soap::Attributes& add( const emu::soap::QualifiedName& name, 
				  xdata::Serializable* value );

      /// 
      /// Gets the type of data named \c name .
      /// @param name Qualified XML name of the data.
      ///
      /// @return Type of the data as returned by \c xdata.
      ///
      string getType( const emu::soap::QualifiedName& name ) const;

      /// 
      /// Gets the \c xdata object holding data named \c name .
      /// @param name Qualified XML name of the data.
      ///
      /// @return Pointer to the \c xdata object holding data named \c name .
      ///
      xdata::Serializable* getValue( const emu::soap::QualifiedName& name );

      static const emu::soap::Attributes none;  ///< An empty container of attributes.
    private:
      bool usePrefixOfParent_; ///< A flag to set the default namespace behavior. It is used if and only if no namespace is specified explicitly. If true, the namespace prefix of the parent element will be used. Otherwise no namespace prefix will be used.
    };

  }
}

#endif
