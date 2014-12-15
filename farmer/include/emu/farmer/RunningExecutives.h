#ifndef __emu_farmer_RunningExecutives_h__
#define __emu_farmer_RunningExecutives_h__

#include <map>
#include <string>

using namespace std;

namespace emu { namespace farmer {

    class RunningExecutives : public map<string,string>{
    public:
      RunningExecutives& add( const string& URI, const string& ownerDuck );
      RunningExecutives& remove( const string& URI );
      bool isRunning( const string& URI ) const { return ( find( URI ) != end() ); }
      bool isActiveDuck( const string& duck ) const;
      size_t getExecutiveCount( const string& duck ) const;
      string getOwnerDuck( const string& URI ) const;
    };

}}
#endif
