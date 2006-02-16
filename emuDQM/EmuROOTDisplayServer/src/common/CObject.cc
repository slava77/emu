//
// CObject class
//
// Tetsuo Arisawa 2000/06/16
// for HistoDisplay v.1.62 class
//
// Version 2.0,  2000/08/28
// Added Draw() and Paint().
// Version 3.0   2000/09/01
// Adding ReadObject().
// Version 3.1   2000/09/05
// free() is added. Care for deleting TPad.
// Version 3.2   2000/09/08
// Draw and Pain are updated.
// Version 3.3   2000/09/15
// Version 3.4   2001/02/27
// GetTitle() is added.
// Version 3.5   2002/09/04
// To handle Root v.3.03/08 TPaveStats destructor problem.
// 

#include <iostream>

#include "CObject.hh"
#include "TFile.h"
#include "TMapFile.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TObject.h" // kCanDelete, ResetBit, AppendPad
// tmp for v3.03/08
#include <TPaveStats.h>


#ifdef CDF
#include "EdmUtilities/CdfClassImp.hh"
CdfClassImp(CObject)
#else
ClassImp(CObject)
#endif

CObject::CObject()
  : TObject(),fTObject(NULL), fPath("") 
{
}

CObject::CObject( TObject *obj, const TObject *inputsource):
  fTObject(obj), fInputSource(inputsource), fPath("")
{

  if ( fTObject ) fName = fTObject->GetName();		     
}

CObject::CObject( const CObject &cobj):
  fTObject(cobj.fTObject), fInputSource(cobj.fInputSource), 
  fPath("")
{

  if ( fTObject ) fName = fTObject->GetName();
}


CObject::~CObject()
{
  free();
}

const char* CObject::GetName() const
{ 
  return fName.Data();
}
#ifndef NO_ROOT_CONST
ULong_t CObject::Hash() const
{
  if ( fTObject ) return fTObject->Hash();
  else return 0;
}


Bool_t CObject::IsEqual(const TObject* obj2) const
{
  //if ( !obj2 || ! obj2->InheritsFrom(CObject::Class()) ) return kFALSE;
  //return fTObject->IsEqual(((CObject*)obj2)->GetTObject());
  if ( obj2 && obj2->InheritsFrom( CObject::Class() ) ) {
    if ( fTObject &&
	 fTObject->IsEqual( ((CObject*)obj2)->GetTObject() ) )
      return kTRUE;
    else return kFALSE;
  }
  else {
    return kFALSE;
  }

}

Int_t CObject::Compare(const TObject *obj) const
{
  if (this == obj) return 0;
  if (CObject::Class() != obj->IsA()) return -1;
  return TString(GetName()).CompareTo(obj->GetName());
}
#else
ULong_t CObject::Hash()
{
  if ( fTObject ) return fTObject->Hash();
  else return 0;
}


Bool_t CObject::IsEqual(TObject* obj2)
{
  //if ( !obj2 || ! obj2->InheritsFrom(CObject::Class()) ) return kFALSE;
  //return fTObject->IsEqual(((CObject*)obj2)->GetTObject());
  if ( obj2 && obj2->InheritsFrom( CObject::Class() ) ) {
    if ( fTObject &&
	 fTObject->IsEqual( ((CObject*)obj2)->GetTObject() ) )
      return kTRUE;
    else return kFALSE;
  }
  else {
    return kFALSE;
  }

}

Int_t CObject::Compare(TObject *obj)
{
  if (this == obj) return 0;
  if (CObject::Class() != obj->IsA()) return -1;
  return TString(GetName()).CompareTo(obj->GetName());
}
#endif
void CObject::SetTObject( TObject *obj )
{
  if ( fTObject != obj ) {
    free();
    fTObject = obj;
  } //if ( fTObject != obj )
}


void CObject::Draw( Option_t *option )
{
  if ( ! fTObject ) return;

  ResetBit( kCanDelete );
  AppendPad( option );

}

void CObject::Paint( Option_t *option )
{
  if ( ! fTObject ) return;
  fTObject->Paint( option );
}



void CObject::ReadObject()
{
  SetTObject( GetNewObj( GetName(), fInputSource ) );
}


 
TObject* CObject::GetNewObj(const char* objName, const TObject* inputobj)
{

  //std::cout << " In GetNewObj" << std::endl;

  TObject *obj = 0;

  if (inputobj) {

    if ( inputobj->IsA() == TMapFile::Class() ) {
      TMapFile *mfile = (TMapFile*)inputobj;
      if (mfile) obj = (TObject*)mfile->Get( objName );
    }
    else if ( inputobj->IsA() == TFile::Class() ) {
      TFile *rfile = (TFile*)inputobj;
      if (rfile) obj = (TObject*)rfile->Get( objName );
    }
    else if ( inputobj->IsA() == TSocket::Class() ) {
      TSocket *socket = (TSocket*)inputobj;

      if ( socket && socket->IsValid() ) {

        Int_t count = 0;
        Int_t maxcount = 5;

        while ( !obj && count < maxcount ) {

          socket->Send( objName );
          TMessage *message = NULL;
          socket->Recv(message);
          if ( message && message->What() == kMESS_OBJECT) {
            obj = (TObject*)message->ReadObject(message->GetClass());
          }
          delete message;

          count++;
        } //while ( !obj && count < maxcount )


        if ( ! obj ) {
          std::cout << " Failed to get " << objName
               << " after " << maxcount << " tries." << std::endl;
        } //if ( ! obj )
        else {
          if ( count > 1 ) std::cout << " The object " << objName
                                << " is sent from the socket by " << count
                                << " tries." << std::endl;
        } //else of if ( ! obj )

      } //if ( socket && socket->IsValid() )
      else {
        std::cout << "no connection to socket..." << std::endl;
      } //else // if ( socket && socket->IsValid() )

    } // TMapFile,TFile, TSocket

    else {

      std::cout << " No inputobj " << std::endl;

    } // else // TMapFile,TFile, TSocket

  } // if(inputobj)

  return obj;

}


void CObject::free()
{
  //std::cout << " In CObject free " << std::endl;

  if ( !fTObject ) return;

  if ( fTObject->InheritsFrom( TPad::Class() ) )
    ResetAllkCanDelete( (TPad*)fTObject );

  //std::cout << "before delete fTObject:" << fTObject->GetName() << std::endl;
  delete fTObject;
  fTObject = 0;

  //std::cout << "end of free " << std::endl;

}


void CObject::ResetAllkCanDelete( TVirtualPad *pad )
{
  //std::cout << " In CObject ResetAllkCanDelete " << std::endl;

  if ( !pad ) return;

  TObjLink *link = pad->GetListOfPrimitives()->FirstLink();

  while( link ) {
    TObject *obj = link->GetObject();
    if ( obj ) { 

      //tmp for v.3.03/07 --

      //if ( ! obj->TestBit( kCanDelete ) ) obj->SetBit( kCanDelete );

      if ( ! obj->TestBit( kCanDelete ) 
	   && !obj->InheritsFrom( TPaveStats::Class() ) ) obj->SetBit( kCanDelete );

      if ( obj->InheritsFrom( TPaveStats::Class() ) ) {
	TPaveStats *stats = (TPaveStats*) obj;
	if ( stats && !stats->GetParent() ) 
	  stats->SetParent( pad->GetListOfPrimitives() );

      }//if ( obj->InheritsFrom( TPaveStats::Class() ) ) 
      //--



      if ( obj->InheritsFrom( TPad::Class() ) ) 
	ResetAllkCanDelete( (TPad*) obj );
    }
    link = link->Next();
  } // while( link )

  //std::cout << " End of ResetAllkCanDelete of " << pad->GetName() << std::endl;

}

CObject &CObject::operator=( const CObject &cobj )
{
  if (this != &cobj) {
    //if ( fTObject ) delete fTObject;
    //fTObject = cobj.fTObject;
    SetTObject( cobj.fTObject );
    SetPath(TString(cobj.GetPath()));
    fInputSource = cobj.fInputSource;
  }
  return *this;
}



