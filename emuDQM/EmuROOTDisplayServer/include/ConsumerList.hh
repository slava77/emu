//*************************************************************************
//             ConsumerList for the Consumer Project
// the ConsumerList class contains a list with all consumers. This list can
// be displayed on the WWW,be read from a Webpage and sent/received via 
// Socket.  
//     (based on the DisplayServer by Frank Hartmann and Hans Wenzel)
//
//                    04/14/00 Harmut Stadie       first check in
//************************************************************************

#ifndef CONSUMERLIST_HH
#define CONSUMERLIST_HH

#include <vector>
#include <iostream>

#include "TString.h"
#include "TUrl.h"

class ConsumerList
{
public:
  ConsumerList(TString sitename="");
  ~ConsumerList();
  
  int nEntries() const;
  const TString& consumername(int i) const;
  const TString& hostname(int i) const;
  int port(int i) const;
  int nevents(int i) const;
  int runnumber(int i) const;
  int status(int i) const;
  const char* statusString(int i) const;
  const TString& updateTime(int i) const;
  int readWebPage();
  void setPort(int i, int port);
  void setNevents(int i, int nev);
  void setStatus(int i , int status);
  void setRunnumber(int i, int runn);
  void setUpdateTime(int i,char *tm);
  int watchServers(const int port,const char *filename); // command for the StateManager
  void test();
  void print(std::ostream& output = std::cout) const;
  void addEntry(TString nam, TString hostnam,int port,
		int nev,int runn,int stat,TString tm = "");
  void removeEntry(const char* hostname,int port);
  int getOldNEvents(const char* hostname,int port);
  int sendList(const char *host,const int port);
  enum 
  {
    Unknown    = 0, 
    Dead       = 1,
    Running    = 2,
    Stopped    = 3,
    Crashed    = 4,
    Finished   = 5,
    NotUpdated = 6
  };
  int writeWebPage(const char *filename);
private:
  //int writeWebPage(const char *filename);
  struct ConsumerEntry
  {
    TString name;
    TString hostname;
    int port;
    int nevents;
    int runnumber;
    int status;
    TString time;
    ConsumerEntry(TString nam, TString hostnam,int p,
		  int nev,int runn,int stat,TString tm = "");
    ConsumerEntry();
  }; 
  ConsumerList(const ConsumerList& sm);
  std::vector<ConsumerEntry> _consumerlist;
  TString _websitename;
  char *_statusnames[7];
public:
  static bool sortbyName(const ConsumerEntry a, const ConsumerEntry b);
};
  
#endif
