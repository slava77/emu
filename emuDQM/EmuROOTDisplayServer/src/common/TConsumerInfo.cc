//*************************************************************************
//             TConsumerInfo for the Consumer Project
//                   04/28/00 Harmut Stadie
//                   09/29/00 Tetsuo Arisawa
//*************************************************************************
// RCS Current Revision Record
//-------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/TConsumerInfo.cc,v $
// $Revision: 1.3 $
// $Date: 2007/11/27 15:27:24 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//**************************************************************************
#include "TConsumerInfo.hh"

#include "assert.h"

#include "TGListTree.h"
#include "TMessage.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TList.h"
#include "TSocket.h"
#include "TBuffer.h"

using std::cout;
using std::endl;
using std::pair;


#ifdef CDF
  #include "EdmUtilities/CdfClassImp.hh"
  CdfClassImp(TConsumerInfo)
#else
  ClassImp(TConsumerInfo)
#endif

TConsumerInfo::TConsumerInfo(const char* const consumername,int runnumber)
  : TNamed("ConsumerInfo",consumername), _list(new TList),
    _runnumber(runnumber),_nevents(0),
    _inputname(),_estimatedsize(100000)
{
}


TConsumerInfo::~TConsumerInfo()
{
  delete _list;
}

void TConsumerInfo::updateList(TList *list,TSocket *sock)
{
  std::cout << "TConsumerInfo: the deprecated function updateList has been called." << std::endl;
  /*
    TMessage *mess = NULL;
    TObject *obj;
    for(ConsumerInfoMapConstIter iter=_objectlist.begin() ;
    iter != _objectlist.end() ; ++iter)
    {
    //std::cout << "requesting " << iter->first << std::endl;
    sock->Send(iter->first);
    sock->Recv(mess);
    if (mess->What() == kMESS_OBJECT)
    {
    obj = (TObject*)mess->ReadObject(mess->GetClass());
    delete mess;
    if (obj->InheritsFrom(TH1::Class())||
    obj->InheritsFrom(TCanvas::Class()))
    list->Add(obj);
    }
    else delete mess;
    }
  */
}

void TConsumerInfo::setPath(const TString &name, const TString &path)
{
  if(_objectlist.count(name) == 1)
    {
      _objectlist.find(name)->second.path = path;
    }
  else
    std::cout << "TConsumerInfo: cannot set path. There is not exactly one element for " 
	 << name << " in the list." << std::endl;
  
} 

void TConsumerInfo::setPath(const TString &name, const TString &oldpath, const TString &newpath)
{
  std::pair<ConsumerInfoMapIter,ConsumerInfoMapIter> range = _objectlist.equal_range(name);
  
  for(ConsumerInfoMapIter iter = range.first ; iter != range.second ; ++iter)
    {
      if(iter->second.path == oldpath) iter->second.path = newpath;
    }
}

void TConsumerInfo::setStatus(const TString &name, int status)
{
  std::pair<ConsumerInfoMapIter,ConsumerInfoMapIter> range = _objectlist.equal_range(name);

  //std::cout << "test range" << _objectlist.count(name) << " " << std::endl;
  //int i = 0;
  for(ConsumerInfoMapIter iter = range.first ; iter != range.second ; ++iter)
    {
      iter->second.status = status;
      //++i;
    }
  //std::cout << i << std::endl;
}

void TConsumerInfo::setAddress(const TString &name, TObject *obj)
{
  std::pair<ConsumerInfoMapIter,ConsumerInfoMapIter> range = _objectlist.equal_range(name);

  //std::cout << "test range" << _objectlist.count(name) << " " << std::endl;
  //int i = 0;

  //update list!
  if(TObject* oldobj = _list->FindObject(name)) 
    {
      _list->Remove(oldobj);
      _list->Add(obj);
    }
  for(ConsumerInfoMapIter iter = range.first ; iter != range.second ; ++iter)
    {
      iter->second.address = obj;
      //++i;
    }
  //std::cout << i << std::endl;
} 

TString TConsumerInfo::getPath(const TString &name) const
{ 
  if(_objectlist.count(name) == 1)
    {
      return _objectlist.find(name)->second.path;
    }
  else
    std::cout << "TConsumerInfo: cannot get path. There is not exactly one element for " 
	 << name << " in the list." << std::endl;
  return "";
} 

Int_t TConsumerInfo::getStatus(const TString &name) const
{
  ConsumerInfoMapConstIter iter =_objectlist.find(name);
  if(iter != _objectlist.end())
    return _objectlist.find(name)->second.status;
  else return -1;
} 

TObject* TConsumerInfo::getAddress(const TString &name)
{  
  ConsumerInfoMapConstIter iter =_objectlist.find(name);
  if(iter != _objectlist.end())
    return _objectlist.find(name)->second.address;
  else return NULL;
} 

TString TConsumerInfo::getToolTip(const TString &name) const
{
  ConsumerInfoMapConstIter iter =_objectlist.find(name);
  if(iter != _objectlist.end())
    {
      TObject* obj = _objectlist.find(name)->second.address;
      if(obj) return obj->GetTitle();
    }
  return "";
}

void TConsumerInfo::setInputName( const TString &inputname )
{
  _inputname = inputname;
}


const TString& TConsumerInfo::getInputName() const 
{
  return _inputname;
}

void TConsumerInfo::addObject(const TString& name,const TString &path, int status,TObject *obj)
{
  // make entry in map
//  typedef std::multimap<TString,struct psa>::iterator MI;
//  pair<MI,MI> n = _objectlist.equal_range(name);
//  bool alreadyThere = false;
//  for (MI p = n.first; p!=n.second; ++p) {
    // std::cout << p->second.path << path << endl;
 //   if (p->second.path == path) alreadyThere = true;
//  }
  //if (!alreadyThere)  
   _objectlist.insert(std::pair<TString,psa>(name,psa(path,status,obj)));
  // only add the object once!
  if(obj && ! _list->FindObject(name))
    {
      _list->Add(obj);
      // increase estimatedsize
      TBufferFile testbuffer(TBufferFile::kWrite);
      obj->Streamer(testbuffer);
      _estimatedsize += testbuffer.BufferSize();
      _estimatedsize += 10000;
    } 
  _modified = true;
}
 
void TConsumerInfo::removeObject(const TString &name)
{
  TObject* add = getAddress(name);
  if(! add)
    {
      _objectlist.erase(name);
      _modified = true;
    }
  else removeObject(add);
}

void TConsumerInfo::removeObject(const TString &name, const TString &path)
{
  std::pair<ConsumerInfoMapIter,ConsumerInfoMapIter> range = _objectlist.equal_range(name);
  
  ConsumerInfoMapIter deliter = _objectlist.end();
  for(ConsumerInfoMapIter iter = range.first ; iter != range.second ; ++iter)
    {
      if(iter->second.path == path) deliter = iter;
    }
  _objectlist.erase(deliter);
  if(_objectlist.count(name) == 0) 
    if(TObject* obj = _list->FindObject(name))
      _list->Remove(obj);
  _modified = true;
}  

void TConsumerInfo::removeObject(TObject *obj, const TString &path)
{
  if (! obj) return; 
  removeObject(obj->GetName(),path);
}

void TConsumerInfo::removeObject(TObject *obj)
{
  if (! obj) return;
  //std::cout << "removing  object " << obj->GetName() << std::endl;
  // remove objects from the lists 
  _list->Remove(obj);
  _objectlist.erase(obj->GetName());
  //print();
  TBufferFile testbuffer(TBufferFile::kWrite);
  obj->Streamer(testbuffer);
  _estimatedsize -= testbuffer.BufferSize();
  _estimatedsize -= 10000;
  _modified = true;
}

void TConsumerInfo::emptyWarningFolder()
{
  //std::cout << "removing error objects" << std::endl;
  //print();
  for( ConsumerInfoMapIter iter = _objectlist.begin() ;
       iter != _objectlist.end() ; ++iter)
    {
      psa pns = iter->second;
      if(pns.path == "Warning!/")
	{
	  ConsumerInfoMapIter del = iter;
	  --iter;
	  _objectlist.erase(del);
	  _list->Remove(pns.address);

	  TBufferFile testbuffer(TBufferFile::kWrite);
	  pns.address->Streamer(testbuffer);
	  _estimatedsize -= testbuffer.BufferSize();
	  _estimatedsize -= 10000;
	  _modified = true;
	}
    }
}

void TConsumerInfo::setNevents(int nevents)
{
  _nevents = nevents;
}

void TConsumerInfo::setRunNumber(int number)
{
  _runnumber = number;
}

TConsumerInfo::psa::psa(TString p,int s,TObject *add)
  : path(p),status(s),address(add)
{
}

void TConsumerInfo::Streamer(TBuffer &b)
{
   // Stream an object of class TConsumerInfo.

   if (b.IsReading()) 
     {
       //std::cout << "reading" << std::endl;
       int size,s;
       TString p,n;
       Version_t v = b.ReadVersion(); 
       TNamed::Streamer(b);
       b >> _runnumber;
       b >> _nevents;
       b >>  size;
       //std::cout << size << std::endl;
       _objectlist.erase(_objectlist.begin(),_objectlist.end());
       for(int i = 0 ; i < size ; i++)
	 {
	   n.Streamer(b);
	   p.Streamer(b);
	   b >> s;
	   _objectlist.insert(std::pair<TString,psa>(n,psa(p,s,NULL)));
	 }
       //std::cout << "result:" << std::endl;
       //print();
     } 
   else 
     {
       //std::cout << " writing " << std::endl;
       //print();
       b.WriteVersion(TConsumerInfo::IsA());
       TNamed::Streamer(b);
       b << _runnumber;
       b << _nevents;
       b << _objectlist.size();
       //std::cout << _objectlist.size() << std::endl;
       for(ConsumerInfoMapConstIter iter=_objectlist.begin() ; iter != _objectlist.end() ; ++iter)
	 {
	   TString name = iter->first;
	   name.Streamer(b);
	   psa pns = iter->second;
	   pns.path.Streamer(b);
	   b << pns.status;
	 }
     }   
}

void TConsumerInfo::print(std::ostream& output) const
{
  output << fName << "  " << fTitle << std::endl;
  output << "run:" << _runnumber << "  number of events:" << _nevents << std::endl;
  std::cout << _objectlist.size() <<  " objects:" << std::endl;
  for(ConsumerInfoMapConstIter iter=_objectlist.begin() ; iter != _objectlist.end() ; ++iter)
    std::cout << iter->second.path << "/" << iter->first << "  " 
	 << iter->second.status << " " << iter->second.address << std::endl;
  std::cout << "list:" << std::endl;
  _list->ls();
  std::cout << endl << std::endl;
}

