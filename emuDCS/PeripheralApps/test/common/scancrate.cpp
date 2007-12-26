#include <iostream>
#include "VMEController.h"

using namespace std;

int main(int argc,char *argv[]){

  if(argc>2)
  {
   // create VME Controller
   VMEController *osucc = new VMEController();
   int port=atoi(argv[1]);
   osucc->init(argv[2], port);
   osucc->set_ErrorServer();
//   osucc->Debug(100);

   if(osucc->exist(1))
   {  cout << "Slot 1 active" << endl;
      for(int i=2;i<=21;i++)
      {
         cout << "Slot " << i << " ";
         if(osucc->exist(i)) cout << "active";
         cout << endl;
      }
   }
   else
   {   cout << "VMEController self test failed" << endl; 
   }
   delete(osucc);
  }
  else
  {
     cout << "Usage:     " << argv[0] << " <eth_port#> <controller_MAC_address>" << endl;
     cout << "  example: " << argv[0] << " 2 02:00:00:00:00:11" << endl;
  }
}

   
