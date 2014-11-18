#include "emu/utils/DOM.h"
#include "emu/utils/IO.h"
#include <iostream>
#include <string>

using namespace emu::utils;
using namespace std;


int main(int argc, char **argv)
{

  string XML( "<?xml version='1.0' encoding='UTF-8'?><td:A xmlns:td='http://testDOM' Aattr1='avalue'><B Battr1='bvalue'>Btext</B></td:A>" );

  string XPath( "/td:A/@Aattr1" );
  cout << XPath << ": " << getSelectedNodeValue( XML, XPath ) << endl;
  XPath = "//*/@*";
  cout << XPath << ": " << getSelectedNodesValues( XML, XPath ) << endl;
  cout << XPath << " removed:\n" << removeSelectedNode( XML, XPath ) << endl;
  XPath = "//td:A";
  XML = appendToSelectedNode( XML, XPath, "<C Cattr1='12345'/>" );
  cout << "Appended to " << XPath << ":\n" << XML << endl;
  XPath = "//C/@Cattr1";
  cout << XPath << ": " << setSelectedNodesValues( XML, XPath, "11111.7", emu::utils::add ) << endl;

  return 0;
}
