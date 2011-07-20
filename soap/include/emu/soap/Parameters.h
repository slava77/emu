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
#include <vector>
#include "xdata/Serializable.h"
#include "emu/soap/QualifiedName.h"
#include "emu/soap/Attributes.h"

namespace emu{
  namespace soap{
    using namespace std;

    class Parameters;
    /// 
    /// Insertion operator for formatted text output.
    /// @param os The \c ostream object.
    /// @param parameters The \c Parameters object to be serialized.
    ///
    /// @return Ref. to the \c ostream object itself.
    ///
    ostream& operator<<( ostream& os, const emu::soap::Parameters& parameters );

    ///
    /// A container class to pass multiple SOAP parameters with their attributes in.
    ///
    class Parameters : public vector< pair< QualifiedName, pair< xdata::Serializable*, const emu::soap::Attributes* > > >{
    public:

      /// 
      /// Default ctor.
      ///
      Parameters();

      /// 
      /// Insertion operator for formatted text output is our friend.
      /// @param os The \c ostream object.
      /// @param parameters The \c Parameters object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os, const emu::soap::Parameters& parameters );

      ///
      /// Sets whether namespace prefix is to be used.
      ///
      /// @param usePrefixOfParent True if namespace prefix is to be inherited from parent element. False if prefix is to be omitted.
      ///
      /// @return Ref. to the Parameters object itself.
      ///
      emu::soap::Parameters& setUsePrefixOfParent( bool usePrefixOfParent );

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
      /// @param attributes Pointer to the \c emu::soap::Attributes object.
      ///
      /// @return Ref. to the Parameters object itself. 
      ///
      emu::soap::Parameters& add( const emu::soap::QualifiedName& name, 
				  xdata::Serializable* value,
				  emu::soap::Attributes* attributes=NULL );

      /// 
      /// Gets the type of data named \c name .
      /// @param name index Index of the parameter.
      ///
      /// @return Type of the data as returned by \c xdata.
      ///
      string getType( const size_t index ) const;

      /// 
      /// Gets the \c xdata object holding data named \c name .
      /// @param name index Index of the parameter.
      ///
      /// @return Pointer to the \c xdata object holding data named \c name .
      ///
      xdata::Serializable* getValue( const size_t index );

      /// 
      /// Gets the attributes of the named parameter.
      /// @param index Index of the parameter.
      ///
      /// @return Pointer to the attributes of the named parameter.
      ///
      const emu::soap::Attributes* getAttributes( const size_t index ) const;

      static const emu::soap::Parameters none; ///< An empty container of parameters.

    private:
      bool usePrefixOfParent_; ///< A flag to set the default namespace behaviour. It is used if and only if no namespace is specified explicitly. If true, the namespace prefix of the parent element will be used. Otherwise no namespace prefix will be used.
    };

  }
}

#endif
