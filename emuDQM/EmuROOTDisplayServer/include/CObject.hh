//
// CObject class
//
// Tetsuo Arisawa 2000/06/16
// for HistoDisplay v.1.62 class
//
// Version 2.0,  2000/08/28
// Added Draw() and Paint().
// Version 3.0   2000/09/01
// Adding UpdateTObject().
// Version 3.1   2000/09/05
// free() is added. Care for deleting TPad.
// Version 3.3   2000/09/15
// Version 3.4   2001/02/27
// GetTitle() is added.

#ifndef _CObject_H_
#define _CObject_H_

#include "TObject.h"
#include "TString.h"
#include "TVirtualPad.h"

class CObject: public TObject {

private:
  TObject *GetNewObj( const char* , const TObject *);
  void free();
  void ResetAllkCanDelete( TVirtualPad *pad );

protected:
  TObject *fTObject; // Original TObject.
  const TObject *fInputSource; // Input source. TFile, TMapFile, TSocket.
  TString fName;
  TString fPath;

public:

  CObject();
  CObject( TObject *obj, const TObject *inputsource );
  CObject( const CObject &cobj );

  virtual ~CObject();

  CObject & operator=(const CObject &obj);

  //const char* GetName() const { return fTObject->GetName(); }
  const char* GetPath() const { return fPath.Data(); };
  const char* GetName() const;
  const char* GetTitle() const { return fTObject->GetTitle(); };

  Bool_t IsSortable() const {return kTRUE; }
#ifdef NO_ROOT_CONST
  ULong_t Hash();
  Bool_t IsEqual(TObject* obj2); 
  Int_t Compare(TObject *obj);
#else
  //ULong_t Hash() { return fTObject->Hash(); }
  ULong_t Hash() const;

  Bool_t IsEqual(const TObject* obj2) const; 
  Int_t Compare(const TObject *obj) const;
#endif

  TObject* GetTObject() const { return fTObject; }
  const TObject* GetInputSource() const { return fInputSource; }

  void SetPath(TString path) { fPath = path;};
  void SetTObject( TObject *obj );
  void SetInputSource( const TObject *obj ) { fInputSource = obj; }

  virtual void Draw( Option_t *option = "" );
  virtual void Paint( Option_t *option = "" );

  virtual void ReadObject();

  ClassDef(CObject,0)
};


#endif



