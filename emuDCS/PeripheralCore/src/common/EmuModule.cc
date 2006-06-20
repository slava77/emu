//----------------------------------------------------------------------
#include "EmuModule.h"
//
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#include <log4cplus/logger.h>

EmuModule::EmuModule()
{
  //
  MyOutput_ = &std::cout ;
  //
}

//
void EmuModule::SendOutput(std::string Output){
  //
  log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuPeripheralCrate");
  LOG4CPLUS_INFO(logger,Output);
  //
}
