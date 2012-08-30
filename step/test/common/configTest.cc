//#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/System.h"
#include "emu/utils/String.h"
#include "emu/step/Test.h"

template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::map< T1, T2 >& t)
{
  std::cout << "(";
  typename std::map< T1, T2 >::const_iterator it, iti;
  for (it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti)
    std::cout << it->first << "->" << it->second << (iti != t.end() ? "," : "");
  std::cout << ")";
  return os;
}

using namespace std;
using namespace emu;

int main( int argc, char** argv ){

  cout << argv[1] << endl;
  string XML( utils::readFile( string( argv[1] ) ) );

  // string resultXML( utils::removeSelectedNode( XML, "//EmuSystem/PeripheralCrate/*" ) );
  // map<string, string> values;
  // values["//EmuSystem/PeripheralCrate[@crateID='41']/@label"] = "BLA";
  // values["//EmuSystem/PeripheralCrate[@crateID='31']/@*"] = "BLABLA";
  // cout << values << endl;
  // resultXML = utils::setSelectedNodesValues( resultXML, values );
  // resultXML = utils::setSelectedNodeValue( resultXML, "//EmuSystem/PeripheralCrate[@crateID='32']/@*", "BLABLABLA" );
  // utils::writeFile( "result.xml", resultXML );

  // Test getting test parameters:
  map<string,int> parameters;
  vector< pair< string, string > > par = utils::getSelectedNodesValues( XML, "//STEP_tests/test_config[translate(test,' ','')='17b']/*[name()!='test']" );
  for ( vector< pair< string, string > >::const_iterator p = par.begin(); p != par.end(); ++p ){
    parameters[p->first] = utils::stringTo<int>( p->second );
  }
  cout << parameters << endl;

  // Test creating endcap with test-specific VME settings:
  string generalSettingsXML( utils::readFile( "xml/VMEp2_1.xml" ) );
  string specialSettingsXML( utils::readFile( "xml/TestSpecificVME.xml" ) );
  step::Test t( "13", XML, generalSettingsXML, specialSettingsXML );

  return 0;
}
