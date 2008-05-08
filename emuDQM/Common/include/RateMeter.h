#ifndef _RateMeter_h_
#define _RateMeter_h_

#include <sys/time.h>
#include <iostream>
#include <map>
#include <string>

#include "toolbox.h"


using namespace toolbox;

template <typename T> class RateMeter: public Task
{
  typedef struct Sampler {
    T* ref;
    T old_val;
    T rate;
    struct timeval last_stamp;
  } Sampler;



 public:

  RateMeter(): Task("RateMeter"),
	Sem_(BSem::FULL) {
    Sem_.take();
    samplers.clear();
    Sem_.give();
    init();
  }
  ~RateMeter() { }

  void init() {
    Sem_.take();
    timerDelay=10;
    fActive= false;
    typename std::map< std::string, Sampler >::iterator itr;
    for (itr = samplers.begin(); itr != samplers.end(); ++itr) {
      Sampler& samp = itr->second;
      samp.old_val=T(0);
      samp.rate=T(0);
      gettimeofday(&(samp.last_stamp), NULL);
		
    }
    Sem_.give();
	
  }

  void clear() {
    Sem_.take();
    samplers.clear();
    Sem_.give();
  }

  void setTimer(int delay) { timerDelay = delay;}

  void addSampler(std::string id, T* s) {
    Sem_.take();
    Sampler samp;
    samp.ref = s;
    samp.old_val=T(0);
    gettimeofday(&(samp.last_stamp), NULL);
    samp.rate = T(0);
    samplers[id] = samp;
    Sem_.give();
  }

  bool isActive() const {return fActive;};

  T getRate(std::string id)  {     
    Sem_.take();
    T rate=T(0);
    typename std::map<std::string, Sampler >::const_iterator itr = samplers.find(id);
    if (itr != samplers.end()) {
      //      std::cout << itr->first << ": Read" <<  std::endl;
      // return itr->second.rate;
      rate = itr->second.rate;
    } 
    Sem_.give();
    return rate;
  }

  int svc() {
    struct timeval stamp;
    fActive=true;

    while (fActive) {
      Sem_.take();
      typename std::map<std::string, Sampler >::iterator itr;
      for (itr = samplers.begin(); itr != samplers.end(); ++itr) {
	Sampler& samp = itr->second;
	if (samp.ref) {
	  gettimeofday(&stamp, NULL);
	  T val=*(samp.ref);
	  T tdiff = stamp.tv_sec-samp.last_stamp.tv_sec;
	  if (tdiff>T(0)) samp.rate = (val-samp.old_val)/tdiff;
	  samp.last_stamp = stamp;
	  samp.old_val=val;
	  //  std::cout << itr->first << ": Set" <<  std::endl;
	} 
	
      }
      Sem_.give();
      usleep(timerDelay*1000*1000);
    }
    fActive=false;

    return 0;
  }

 private:

  bool fActive;
  int timerDelay;
  std::map<std::string, Sampler > samplers;
  BSem Sem_;
	

};


#endif
