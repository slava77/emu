//
// CHistogram class
//
// Tetsuo Arisawa 2000/08/28
// for HistoDisplay v.1.64 class
//
// 2000/09/01 T.A
// Version 2.0
// fSubtractHisto is cloned internally. Destructor is ok.
// Version 3.0
// Draw(), Paint() with UpdateTObject in CObject.
// Version 3.1 2000 09/15
//

#ifndef _CHistogram_H_
#define _CHistogram_H_

#include "TH1.h"
#include "TObject.h"
#include "CObject.hh"
#include "TList.h"

class CHistogram: public CObject {

protected:
  TH1 *fSubtractHisto; // histo to be subtracted after reset.
  TH1 *fRefHisto; // reference histo.

  TH1 *fHistoDrawn; // histo to be drawn. (TH1*)fTObject - fSubtractHisto

  virtual void UpdateHisto();

private:
  void free();
  void copy(const CHistogram &chisto);


public:
  CHistogram(TH1 *h1, const TObject *inputsource,
	     TH1 *subtracthisto = 0, TH1 *refhisto = 0 );
  CHistogram(const CHistogram &chisto);
  CHistogram(const CObject &cobj);
  CHistogram();


  //~CHistogram();
  virtual ~CHistogram();

 
  TH1* GetSubtractHisto() const { return fSubtractHisto; }
  TH1* GetRefHisto() const { return fRefHisto; }

  void SetSubtractHisto(TH1* );
  void SetRefHisto(TH1* );

  TH1* SubtractedHisto();


  virtual void Draw( Option_t *option = "" );
  virtual void Paint( Option_t *option = "" );

  TH1* GetHistoDrawn () const { return fHistoDrawn; }

  CHistogram & operator=(const CHistogram &chisto); 

  ClassDef(CHistogram,0)
};


#endif



