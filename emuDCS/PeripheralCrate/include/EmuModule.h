#ifndef EmuModule_h
#define EmuModule_h


#include <iostream>

class EmuModule
{
public:
  //
  EmuModule();
  virtual ~EmuModule() {};
  
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  
  void SendOutput(std::string,std::string = "INFO");
  
  std::ostream * MyOutput_ ;

private:
  //
  //
};

#endif
