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

  RateMeter(): Task("RateMeter") {
    samplers.clear();
    init();
  }
  ~RateMeter() { }

  void init() {
    timerDelay=10;
    fActive= false;
    typename std::map< std::string, Sampler >::iterator itr;
    for (itr = samplers.begin(); itr != samplers.end(); ++itr) {
      Sampler& samp = itr->second;
      samp.old_val=T(0);
      samp.rate=T(0);
      gettimeofday(&(samp.last_stamp), NULL);
		
    }
	
  }

  void clear() {
    samplers.clear();
  }

  void setTimer(int delay) { timerDelay = delay;}

  void addSampler(std::string id, T* s) {
    Sampler samp;
    samp.ref = s;
    samp.old_val=T(0);
    gettimeofday(&(samp.last_stamp), NULL);
    samp.rate = T(0);
    samplers[id] = samp;
  }

  bool isActive() const {return fActive;};

  T getRate(std::string id) const { 
    typename std::map<std::string, Sampler >::const_iterator itr = samplers.find(id);
    if (itr != samplers.end()) {
      //      std::cout << itr->first << ": Read" <<  std::endl;
      return itr->second.rate;
    } else return T(0);
  }

  int svc() {
    struct timeval stamp;
    fActive=true;

    while (fActive) {
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
      usleep(timerDelay*1000*1000);
    }
    fActive=false;

    return 0;
  }

 private:

  bool fActive;
  int timerDelay;
  std::map<std::string, Sampler > samplers;
	

};


#endif
