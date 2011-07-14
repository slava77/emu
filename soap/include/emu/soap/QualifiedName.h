#ifndef __emu_soap_QualifiedName_h__
#define __emu_soap_QualifiedName_h__

#include <string>
#include <ostream>

namespace emu{
  namespace soap{
    using namespace std;

    class QualifiedName{
    public:

      friend ostream& operator<<( ostream& os, const emu::soap::QualifiedName& qualifiedName ){
	os << qualifiedName.getPrefix() << ":" << qualifiedName.getName() 
	   << " xmlns:" << qualifiedName.getPrefix() << "=\"" << qualifiedName.getNamespaceURI() << "\"";
	return os;
      }

      QualifiedName( const string& name, const string& namespaceURI = emptyString, const string& prefix = emptyString );
      string getName() const { return name_; }
      string getNamespaceURI() const { return namespaceURI_; }
      string getPrefix() const { return prefix_; }
      int operator<( const QualifiedName& other ) const;
      int operator==( const QualifiedName& other ) const;
    private:
      string name_;
      string namespaceURI_;
      string prefix_;
      static const string emptyString;  ///< An empty string.
    };

  }
}
#endif
