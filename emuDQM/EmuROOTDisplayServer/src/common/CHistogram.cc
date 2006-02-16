//
// CHistogram class
//
// Tetsuo Arisawa 2000/06/16
// for HistoDisplay v.1.62 class
//
// 2000/09/01 T.A
// Version 2.0
// fSubtractHisto is cloned internally. Destructor is ok.
// Version 3.0
// Draw(), Paint() with UpdateTObject in CObject.
// Version 3.1 2000 09/15
//

#include "CHistogram.hh"
#include "iostream" // std::cout,std::endl

#include "TVirtualPad.h" //gPad

#ifdef CDF
  #include "EdmUtilities/CdfClassImp.hh"
  CdfClassImp(CHistogram)
#else
  ClassImp(CHistogram)
#endif
  
CHistogram::CHistogram(TH1 *h1, const TObject *inputsource,
		       TH1 *subtracthisto, TH1 *refhisto )
  : CObject(h1, inputsource), fRefHisto(refhisto),fSubtractHisto(NULL),
    fHistoDrawn(NULL)
{
  SetSubtractHisto( subtracthisto );
  UpdateHisto();
}

CHistogram::CHistogram(const CHistogram &chisto)
{
  if (this != &chisto) copy(chisto);
}


CHistogram::CHistogram(const CObject &cobj)
{
  TObject *obj = cobj.GetTObject();
  if ( obj && obj->InheritsFrom(TH1::Class()) ) 
    CHistogram( (TH1*)obj, cobj.GetInputSource(), 0, 0);
}

CHistogram::CHistogram()
  : CObject(),fRefHisto(NULL),fSubtractHisto(NULL),fHistoDrawn(NULL)
{
}


void CHistogram::copy(const CHistogram &chisto) 
{
  TH1 *histo = (TH1*) chisto.GetTObject();

  if ( !histo ) return;

  if ( fTObject && fTObject != chisto.fTObject ) delete fTObject;
  //fTObject = histo->Clone();
  fTObject = histo;

  fInputSource = chisto.GetInputSource();

  TH1 *subtracthisto =  (TH1*) chisto.GetSubtractHisto() ;
  SetSubtractHisto( subtracthisto );

  fRefHisto = chisto.GetRefHisto();

  UpdateHisto();

}

void CHistogram::free() 
{
  if ( fSubtractHisto ) {
    delete fSubtractHisto;
    fSubtractHisto = 0;
  }

  if ( fHistoDrawn ) {
    delete fHistoDrawn;
    fHistoDrawn = 0;
  }

  //if ( fTObject ) delete fTObject; // done by ~CObject()

}


CHistogram::~CHistogram(){
  free();
}


CHistogram& CHistogram::operator=(const CHistogram &chisto) {
  if (this != &chisto) {
    free();
    copy(chisto);
  }
  return *this;
}

void CHistogram::SetSubtractHisto(TH1* subtracthisto){
  // Set histo to be subtracted.
  // subtracthisto's clone is stored internally.

  if ( fSubtractHisto && fSubtractHisto != subtracthisto )
    delete fSubtractHisto;

  if ( subtracthisto ) {
    fSubtractHisto = (TH1*) subtracthisto->Clone();
  }
  else {
    fSubtractHisto = 0;
  }

  UpdateHisto();

}

void CHistogram::SetRefHisto(TH1* refhisto){
  fRefHisto = refhisto;
}


TH1* CHistogram::SubtractedHisto()
{
  // return subtracted histogram.

  TH1 *histo;
  if ( fTObject && fTObject->InheritsFrom( TH1::Class() ) ) {
    histo = (TH1*)fTObject;
  }
  else {
    return 0;
  }

  TH1* histonew = (TH1*)( histo->Clone() );

  TH1 *subtracthisto = fSubtractHisto;

  if ( histonew && subtracthisto) {

    histonew->Add(subtracthisto, -1.);

    histonew
      ->SetEntries( histo->GetEntries() 
		    - subtracthisto->GetEntries() );

   // After Root Version 2.23.12
    Stat_t s1[10], s2[10];
    Int_t i;
    for (i=0;i<10;i++) {s1[i] = s2[i] = 0;}
    histo->GetStats(s1);
    subtracthisto->GetStats(s2);
    for (i=0;i<10;i++) s1[i] -= s2[i];
    histonew->PutStats(s1);
    //-

  } //if (histonew && subtracthisto) 

  return histonew;
}


void CHistogram::UpdateHisto()
{
  if ( fHistoDrawn ) delete fHistoDrawn;
  fHistoDrawn = SubtractedHisto();
}


void CHistogram::Draw( Option_t *option )
{
  //if ( fHistoDrawn ) fHistoDrawn->Draw( option );

  if ( option == "" ) option = fTObject->GetOption();

  TString opt = option;
  opt.ToLower();
  if (gPad && !opt.Contains("same")) {
    //the following statement is necessary in case one attempts to draw
    //a temporary histogram already in the current pad
    if ( TestBit(kCanDelete) ) gPad->GetListOfPrimitives()->Remove(this);
    gPad->Clear();
  }
  AppendPad(opt.Data());

}


void CHistogram::Paint( Option_t *option )
{
  if ( fHistoDrawn ) fHistoDrawn->Paint( option );
}




