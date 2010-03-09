#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <sstream>
#include <ostream>
#include <set>
#include <vector>

namespace emu { namespace base {
  class Component;
  using namespace std;

  class Component{

  public:

    friend ostream& operator<<( ostream& os,       emu::base::Component& c );
    friend ostream& operator<<( ostream& os, const emu::base::Component& c );

    Component(){}
    Component( const string& id ){ addId( id ); }

    emu::base::Component& addId( const string& id ){ ids_.insert( id ); return *this; }
    const set<string>& getIds() const { return ids_; }
    bool operator==( const emu::base::Component& other ) const;
    bool isMatchedBy( const string& regex, vector<string>& matches ) const;

  private:
    set<string> ids_;
    
  };
}} // namespace emu::base
#endif  // ifndef __COMPONENT_H__
