#include<iostream>
#include<stdio.h>
#include<unistd.h> // for sleep()
#include "DDUTester.h"
#include "DDU.h"
#include "DCC.h"
#include "JTAG_constants.h"

#ifndef debugV //silent mode
#define PRINT(x)
#define PRINTSTRING(x)
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl;
#define PRINTSTRING(x) cout << #x << endl;
#endif

void DDUTester::all_chip_info()
{
  printf(" INPROM0 %08x \n",ddu_->inprom_idcode0());
}


void DDUTester::executeCommand(string command) {
    cout << "inside DDU executeCommand"<< endl;
  if(command == "allchipinfo")all_chip_info();
}






