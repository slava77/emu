///
/// @file   QualifiedName.h
/// @author  <banicz@vpcf1>
/// @date   Tue Jul 19 22:29:00 2011
/// 
/// @brief  A fully qualified XML name with namespace and prefix.
/// 
/// 
///
#ifndef __emu_soap_QualifiedName_h__
#define __emu_soap_QualifiedName_h__

#include <string>
#include <ostream>

namespace emu{
  namespace soap{
    using namespace std;

    class QualifiedName;

    /// 
    /// Insertion operator for formatted text output.
    /// @param os  The \c ostream object.
    /// @param qualifiedName The \c qualifiedName object to be serialized.
    ///
    /// @return  Ref. to the \c ostream object itself.
    ///
    ostream& operator<<( ostream& os, const emu::soap::QualifiedName& qualifiedName );

    /// 
    ///  A fully qualified XML name with namespace and prefix.
    /// 
    class QualifiedName{
    public:

      /// 
      /// Insertion operator for formatted text output is our friend.
      /// @param os  The \c ostream object.
      /// @param qualifiedName The \c qualifiedName object to be serialized.
      ///
      /// @return  Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os, const emu::soap::QualifiedName& qualifiedName );

      /// 
      /// Ctor.
      /// @param name XML tag name.
      /// @param namespaceURI Optional XML namespace URI.
      /// @param prefix Optional XML namespace prefix.
      ///
      QualifiedName( const string& name, const string& namespaceURI = emptyString, const string& prefix = emptyString );

      /// 
      /// Constructor from simple character string if no namespace is to be specified.
      /// @param name XML tag name.
      ///
      QualifiedName( const char* name );

      /// 
      /// Get XML name.
      ///
      /// @return XML name.
      ///
      string getName() const { return name_; }

      /// 
      /// Get XML namespace URI.
      ///
      /// @return XML namespace URI.
      ///
      string getNamespaceURI() const { return namespaceURI_; }

      /// 
      /// Get XML namespace prefix.
      ///
      /// @return XML namespace prefix.
      ///
      string getPrefix() const { return prefix_; }

      /// 
      /// Less-than operator, needed for the STL containers.
      /// @param other The other \c QualifiedName to compare this with.
      ///
      /// @return 1 if this is less than the other, 0 otherwise.
      ///
      int operator<( const QualifiedName& other ) const;

      /// 
      /// Equality operator.
      /// @param other The other \c QualifiedName to compare this with.
      ///
      /// @return 1 if this is equal to the other, 0 otherwise.
      ///
      int operator==( const QualifiedName& other ) const;

      /// 
      /// Set the namespace URI.
      /// @param namespaceURI Namespace URI.
      ///
      /// @return Reference to this instance of QualifiedName.
      ///
      QualifiedName& setNamespaceURI( const string& namespaceURI ){ namespaceURI_ = namespaceURI; return *this; }

    private:
      string name_;		///< XML name.
      string namespaceURI_;	///< XML namespace URI.
      string prefix_;		///< XML namespace prefix.
      static const string emptyString;  ///< An empty string.
    };

  }
}

#endif
