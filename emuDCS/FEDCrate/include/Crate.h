#ifndef CRATE_h
#define CRATE_h

using namespace std;
#include <vector>
#include <iostream>

class VMEModule;
class VMEController;
class DDU;
class DCC;

class Crate {
public:
  Crate(int number, VMEController * controller);
  ~Crate();

  int number() const {return theNumber;}

  void addModule(VMEModule * module);

  VMEController * vmeController() const {return theController;}

  /// uses RTTI to find types
  vector<DDU *> ddus() const;
  vector<DCC *> dccs() const;

  void enable();
  void disable();
  void configure(int crate);
  void init(int crate);
  int irqtest(int crate,int ival);

private:

  template<class T> T * findBoard() const
  {
    for(unsigned i = 0; i < theModules.size(); ++i) {
      T * result = dynamic_cast<T *>(theModules[i]);
      if(result != 0) return result;
    }
    return 0;
  }

  int theNumber;
  /// indexed by slot 
  vector<VMEModule *> theModules;
  VMEController * theController;
};

#endif

