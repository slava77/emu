#include "emu/dqm/dcfebpipeline/PipelineAnalyzer.h"

#include <iostream>
#include <iomanip>
#include <fstream>

int main( int argc, char** argv ){

  if ( argc != 2 ){
    std::cout << "Needs exactly one argument: the data file directory for this scan. Exiting." << std::endl;
    return 1;
  }

  PipelineAnalyzer pa( argv[1] );

  return 0;
}
