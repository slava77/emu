//********************************************************************************
//             TConsumerInfo for the Consumer Project
// the TConsumerInfo class contains a list with all objects and their status
// and other information about each monitor. This list is added to the mapfile
//   
//     
//                    05/11/00 Harmut Stadie       first check in
//                    09/29/00 Tetsuo Arisawa  
//                    08/24/01 Hartmut Stadie      change to multimap, 
//                                                 new Iterator for display 
//*******************************************************************************
// RCS Current Revision Record
//-------------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/include/TConsumerInfo.hh,v $
// $Revision: 1.3 $
// $Date: 2007/03/22 17:42:18 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*******************************************************************************
#ifndef TCONSUMERINFO_HH
#define TCONSUMERINFO_HH 1


#include <map>
#include <iostream>

#include "TString.h"
#include "TNamed.h"
#include "TList.h"

class TList;
class TSocket;
class TGListTree;


class TConsumerInfo : public TNamed
{ 
public:
  TConsumerInfo(const char* const consumername="",int runnumber=0);
  ~TConsumerInfo();
 
  const char *consumername() const {return GetTitle();}; 
  void clear() { _list->Clear();}; 
  void setConsumername(const char *name) {SetTitle(name);};
 
  void addObject(const TString &name,const TString &path, int status,
		 TObject *obj = NULL);
  void removeObject(const TString &name);
  void removeObject(const TString &name, const TString &path);
  void removeObject(TObject *obj);
  void removeObject(TObject *obj, const TString &path);
  void emptyWarningFolder();
  void setPath(const TString &name, const TString &path);
  void setPath(const TString &name, const TString &oldpath, const TString &newpath);
  void setStatus(const TString &name,int status);
  void setAddress(const TString &name, TObject *obj);

  //note: the following accessors do not necessarily work with more than one entry for a name
  TString getPath(const TString &name) const;
  Int_t getStatus(const TString &name) const;
  TObject* getAddress(const TString &name);
  TString getToolTip(const TString &name) const;

  bool hasEntry(const TString &name) { return _objectlist.count(name) != 0;}
  void setNevents(int nevents);
  void setRunNumber(int number);
  int runnumber() const {return _runnumber;}
  int nevents() const {return _nevents;}
  bool isModified() const { return _modified;}
  void setModified(bool modified) { _modified = modified;}
  void setInputName( const TString &inputname );
  const TString& getInputName() const;
  long estimatedSize() const {return _estimatedsize;}
  TList *list() {return _list;}

  void updateList(TList *list,TSocket *sock);// note: only for the GUI

  void print(std::ostream& output = std::cout) const;
  //object status
  enum
  {
    Okay,
    Warning,
    Error
  };

private: 
  struct psa
  {
    TString path;
    Int_t status;
    TObject *address;
    psa(TString p = "", int s = 0, TObject *add = NULL);
  };
  bool _modified;
  Int_t _runnumber;
  Int_t _nevents;
  long _estimatedsize;

  TString _inputname;
  TList *_list;
 
#ifndef __CINT__
  typedef std::multimap<TString,psa> ConsumerInfoMap;
  typedef std::multimap<TString,psa>::iterator ConsumerInfoMapIter;
  typedef std::multimap<TString,psa>::const_iterator ConsumerInfoMapConstIter;
  ConsumerInfoMap _objectlist; 
  
  //nested class Iterator
public:
  class Iterator;
  friend class Iterator;
  class Iterator
  {
    
  private:
    ConsumerInfoMapConstIter _iter;
    const TConsumerInfo *_info;
    
  public:
    Iterator(const TConsumerInfo *info) :  _iter(info->_objectlist.begin()), _info(info) {} 
    ~Iterator() {}
    
    void next() { ++_iter;}
    void prev() { --_iter;}
    const TString& Name() const { return _iter->first;} 
    const TString& Path() const { return _iter->second.path;}
    TObject* Address() {return _iter->second.address;}
    TString ToolTip() const { return (_iter->second.address) ? _iter->second.address->GetTitle() : "" ;}
    int Status()  const { return _iter->second.status;}
    void operator++() { ++_iter;}
    bool is_valid() { return _iter != _info->_objectlist.end();}
  };
#endif

  ClassDef(TConsumerInfo,1)  

};

#endif


