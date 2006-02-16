////////////////////////////////////////
// HistoDisplay.hh
// Version 3.07
// Tetsuo Arisawa, 2002 8/29
//


#ifndef _HistoDisplay_H_
#define _HistoDisplay_H_

// HistoDisplay.H
//  Tetsuo Arisawa (Waseda Univ.)
//  1999 0716

#ifndef ROOT_TGMainFrame
#include <TGFrame.h>
#endif

#ifndef ROOT_TTimer
#include "TTimer.h"
#endif


class TGMenuBar;
class TGPopupMenu;
class TGHorizontal3DLine;

// Title --
class TGPictureButton;
class TGLabel;
class TGTextEntry; //GContext_t
class TGTextBuffer;
class TGTextButton;
class TGLayout; //TGLayoutHints
//--

class TGGroupFrame;
class TGCheckButton;
class TGRadioButton;

class TGListTree;
class TGListTreeItem;

class TRootEmbeddedCanvas;
class TGCanvas;


class TH1;
class TH1F;
class TPad;
class TCanvas;
class TVirtualPad;


class TSting;

class TFile;
class TMapFile;
class TSocket;

class TGStatusBar;
class TList;

class TCollection;
//class CList;

class TTimer;




class TConsumerInfo;

class ConsumerList; // Hartmut Stadie
class ConsumerListBox;


class CObject;

class HistoDisplayUpdateTimer;
class HistoDisplaySlideTimer;



class HistoDisplay : public TGMainFrame {

  friend class HistoDisplayUpdateTimer;
  friend class HistoDisplaySlideTimer;


private:

  // MenuBar --
  void MakeMenuBar();
  void CloseMenuBar();
  TGMenuBar *fMenuBar;
  TGLayoutHints *fMenuBarLayout;
  TGLayoutHints *fMenuBarItemLayout;
  TGLayoutHints *fMenuBarHelpLayout;
  TGPopupMenu *fFileMenu;
  TGPopupMenu *fNewCascadeMenu;
  TGPopupMenu *fListTreeMenu;
  TGPopupMenu *fSortCascadeMenu;
  TGPopupMenu *fConnectMenu;
  TGPopupMenu *fHelpMenu;

  TGHorizontal3DLine *fMenuBarSeparator;
  TGLayoutHints *fMenuBarSeparatorLayout;
  //--


  // Main Frames --
  TGCompositeFrame *fMainFrame;
  TGLayoutHints *fMainFrameLayout;
  //--


  // Color --
  //  ULong_t LightBlue;
  ULong_t LightYellow;
  //--


  // Title Frame --
  void MakeTitleFrame( const char* );
  void CloseTitleFrame();
  TGCompositeFrame   *fTitleFrame;
  TGLayoutHints      *fTitleFrameLayout;
  TGLabel            *fTitleLabel;
  TGLayoutHints      *fTitleLabelLayout;
  GContext_t          fTextGC;
  TGPictureButton    *fPicBut;
  TGLayoutHints      *fPictLayout;
  //--



  // Input Streem --
  void MakeInputFrame();
  void CloseInputFrame();

  //TGCompositeFrame *fFileNameFrame;
  TGGroupFrame  *fFileNameFrame;
  TGLayoutHints *fFileNameFrameLayout;

  TGRadioButton *fRaButMFile;
  TGRadioButton *fRaButRFile;
  TGRadioButton *fRaButSocket;
  TGRadioButton *fRaButSOAP;
  TGLayoutHints *fRaButLayout;


  TGLabel *fFileNameLabel;
  TGLayoutHints *fFileNameLabelLayout;

  TGCompositeFrame *fFileOpenFrame;
  TGLayoutHints *fFileOpenFrameLayout;

  TGTextEntry *fFileNameTextEntry;
  TGLayoutHints *fFileNameTextEntryLayout;
  TGTextBuffer *fFileNameBuffer;
  TGTextBuffer *fRefNameBuffer;  // Cavanaugh 03.04.2005
  TGTextBuffer *fDynRefNameBuffer;  // Cavanaugh 23.04.2005

  TGTextButton *fReadFileButton;
  TGLayoutHints *fReadFileButtonLayout;

  TGCompositeFrame *fFileNameButtonFrame;
  TGLayoutHints *fFileNameButtonFrameLayout;

  TGTextButton *fChooseFileButton;
  TGLayoutHints *fChooseFileButtonLayout;
  TGTextButton *fClearFileNameButton;
  TGLayoutHints *fClearFileNameButtonLayout;

  //TGLayoutHints *fFileNameButtonLayout;
  //--


  //Draw ListTree Frame--
  void MakeTGListTreeFrame();
  void MakeTGRefListTreeFrame();      // Cavanaugh 03.04.2005
  void CloseTGListTreeFrame();

  TGCompositeFrame *fListTreeMainFrame;
  TGLayoutHints *fListTreeMainFrameLayout;

  /*
  TGLayoutHints *fMonitorNameLayout;
  TGLabel *fMonitorNameLabel;
  TGTextEntry *fMonitorNameTextEntry;
  TGTextBuffer *fMonitorNameBuffer;
  */


  TGCompositeFrame *fHistoListFrame;
  TGCompositeFrame *fRefListFrame;     // Cavanaugh 03.04.2005
  TGLayoutHints *fHistoListFrameLayout;
  TGLayoutHints *fRefListFrameLayout;  // Cavanaugh 03.04.2005

  TGCanvas *fTGCanvas;
  TGLayoutHints *fTGCanvasLayout;
  TGListTree *fHistoListTree;
  TGListTree *fRefListTree;  // Cavanaugh 03.04.2005
  int REF_FLAG;              // Cavanaugh 03.04.2005
  //--




  // Update selections --
  TGCompositeFrame *fListButtonFrame;
  TGLayoutHints *fListButtonFrameLayout;

  TGCheckButton *fChButtonAutoUpdate;
  TGLayoutHints *fChButtonAutoUpdateLayout;
  Bool_t AutoUpdate;


  TGCompositeFrame *fFrequencyFrame;
  TGLayoutHints *fFrequencyFrameLayout;

  TGLabel *fFrequencyLabel;
  TGTextEntry *fFrequencyTextEntry;
  TGTextBuffer *fFrequencyTextBuffer;
  //--







  // Control Buttons --
  TGCompositeFrame *fControlButtonFrame;
  TGLayoutHints *fControlButtonFrameLayout;


  TGCompositeFrame *fButtonFrame;
  TGLayoutHints *fButtonFrameLayout;
  TGLayoutHints *fButtonLayout;

  TGTextButton *fStartHistoButton;
  TGTextButton *fPauseHistoButton;
  TGTextButton *fRestartHistoButton;
  TGTextButton *fClearHistoButton;
  TGTextButton *fStopHistoButton;
  //TGTextButton *fExitButton;


  TGGroupFrame *fHistoButtonGFrame;
  TGLayoutHints *fHistoButtonGFrameLayout;
  TGCompositeFrame *fHistoButtonFrame;
  TGLayoutHints *fHistoButtonFrameLayout;
  TGLayoutHints *fHistoButtonLayout;

  TGTextButton *fZeroHistoButton;
  TGTextButton *fUnZeroHistoButton;
  TGTextButton *fRefHistoButton; // Cavanaugh 03.04.2005
  TGTextButton *fContentHistoButton;
  //--




  // Status Bar --
  void MakeStatusBar();
  void CloseStatusBar();
  TGStatusBar *fStatusBar;
  //--









  // File --
  /*
  enum EInputTypes {
    kTMapFile,
    kTFile,
    kTSocket,
    kTSOAP
  };

  TObject* OpenFile( const char *filename,
		     EInputTypes inputtype = kTFile,
		     Option_t *option = "READ" );

  TObject* CloseFile( const char *filename,
  	  EInputTypes inputtype = kTFile );

  TObject* CloseFile( TObject *inputobj );

  void CloseConnection();
  */


  //TString MonitorName; 
  //TString FileName;



  // Local File Name Buttons --
  const char* FileDialogWindow();
  void FileNameClear();

  void MessageBox(const char *msg);
  //--

  //--






  // Main -- //obsolete
  //void HistoDisplayMain();
  //void HistoDisplayMain( Int_t autoOpen = 0 );
  //void HistoDisplayMain( TObject *inputobj );






  // Histo TGListTree --

  //void AddToTree( TObject *inputobj );
  void AddToTree( const char* objname, TObject *inputobj );

  ////char* GetDirPath( const TString &);
  //TString GetDirPath( const TString &);

  //void MakeTGListTree( const TString &, TObject* );
  void MakeTGListTree( const TString &, TObject*, const char* tip = 0 );
  void RemoveTGListTree();
  void SortTGListTree();
  void DelTreeItemsOfInput( TObject *inputobj );
  //void AddTreeItemsOfInput( TObject *inputobj ); 
  void AddTreeItemsOfInput( TObject *inputobj
			    , Bool_t newlyOpened = kTRUE ); 

  void RefreshTreeItem( TGListTreeItem *item );

  void DeleteEmptyFolders( TGListTreeItem *item );

  TGListTreeItem *fSelectedItem;

  TString MakePathName( const char *dir, const char *objname );

  //void AddTreeItems( TConsumerInfo *info, TObject *inputobj );
  void AddTreeItems( TConsumerInfo *info, TObject *inputobj
		     , Bool_t newlyOpened = kTRUE );

  void SubtractTreeItems( TConsumerInfo *info );

  void DeleteTreeItem( TGListTreeItem *item );


  //--
  void AddRefTreeItemsOfInput( TObject *inputobj
			    , Bool_t newlyOpened = kTRUE );  // Cavanaugh 03.04.2005
  void MakeRefHistoList( TObject *inpuobj );
  void AddRefTreeItems( TConsumerInfo *info, TObject *inputobj
		     , Bool_t newlyOpened = kTRUE );         // Cavanaugh 03.04.2005
  void AddToRefTree( const char* objname, TObject *inputobj );//Cavanaugh 03.04.2005
  void MakeTGRefListTree( const TString &, TObject*, const char* tip = 0 ); // Cavanaugh 03.04.2005
  void SubtractRefTreeItems( TConsumerInfo *info );          // Cavanaugh 03.04.2005
  void DeleteEmptyRefFolders( TGListTreeItem *item );        // Cavanaugh 03.04.2005
  void DisplayRefHisto(TVirtualPad *currentPad);             // Cavanaugh 03.04.2005
  CObject* UpdateRefHistoList(const char *objPath, const char *objName, const TObject *inputobj); // Cavanaugh 03.04.2005
  void AddRefPadHistosToHistoList( TPad *pad );              // Cavanaugh 03.04.2005
  CObject* GetCObjFromRefListItem( TGListTreeItem *item );   // Cavanaugh 03.04.2005
  const char* PadName;                                       // Cavanaugh 21.04.2005





  // Get  ConsumerInfo --
  TConsumerInfo* MakeConsumerInfo( TObject *inputobj );
  TConsumerInfo* MakeConsumerInfo( const char *consumername, 
				   const char *filename );

  TList *fConsumerInfos;
  void DelTreeItemsOfInfo( TConsumerInfo *info );
  TConsumerInfo* GetConsumerInfoFromList( const char * consumer );
  TConsumerInfo* AddConsumerInfo( TConsumerInfo *info );
  void DelConsumerInfo( TConsumerInfo *info );


  ConsumerList *consumerlist;
  ConsumerListBox *fConsumerListBox;
  //--







  // Displayed List --

  TList *fDisplayedList;
  TList *fDisplayedRefList;  // Cavanaugh 03.04.2005
  //void AddToDisplayedList( const TObject *inputobj);

  CObject* UpdateHistoList(const char *objPath, const char *objName, const TObject *inputobj);
  TObject* GetNewObj(const char *objPath, const char *objName, const TObject *inputobj );

  CObject* GetCObjFromListItem( TGListTreeItem *item );

  // Histo List --
  //TList *fHistoList;

  void MakeHistoList( TObject *inpuobj );
  void MakeHistoListM( TMapFile *MFile );
  void MakeHistoListR( TFile *RFile );
  void MakeHistoListS( TSocket *sock );
  //--

  //--    








  // Draw --
  void DrawHisto( TObject *obj );
  void DrawHisto( TList *list );
  //void DrawHisto( const char* name );
  // inside DrawHisto

  void DrawTH1(TH1 *histo, Option_t* option, TVirtualPad *backpad);
  void DrawTPad(TPad *pad1, TVirtualPad *pad2);
  //--

  //--







  //Update --
  void UpdateTimerOn();
  void UpdateCanvas( TVirtualPad *currentpad = 0 );
  void UpdatePad( TVirtualPad *pad );

  void DrawUpdatedPad(TVirtualPad *currentPad);


  void ReplacePad( TVirtualPad *newpad, TVirtualPad *oldpad );

  void ReplaceHisto( TH1 *histo, TH1 *histoOld );
  void ReplaceHistoFunc( TH1 *histo, TH1 *histoOld );

  void AddPadHistosToHistoList( TPad *pad );
  // List of histograms to be subtracted.
  void AddSubtractHisto(TVirtualPad *currentPad, int option);

  // Clear Pad -
  void ClearCurrentPad();
  void RecursivePadClear( TVirtualPad *pad, Option_t *option = "" );


  // Update Canvas Timer -- 

  Bool_t Running;

  //TTimer *fUpdateTimer;
  HistoDisplayUpdateTimer *fUpdateTimer;
  //Bool_t HandleTimer( TTimer *timer );
  //--


  //--


  




  // Slide Show --
  TList *fSlideCanvasList;
  TGCheckButton *fChButtonSlideUpdate;
  Bool_t SlideShowUpdate;


  // Cycle Slide --
  //void CycleSlides( const TGListTreeItem *item,
  //	    TVirtualPad *currentpad );

  //v3.061
  /*
  void CycleSlides( const TGListTreeItem *item,
		    TVirtualPad *currentpad,
		    TGListTreeItem *displayingitem );
  */
  void CycleSlides( TGListTreeItem *item,
		    TVirtualPad *currentpad,
		    TGListTreeItem *displayingitem );


  void SlideTimerOn( TGListTreeItem *item );
  void StartSlides( TGListTreeItem *item );

  // Cycle Slide Timer --
  HistoDisplaySlideTimer *fSlideTimer;
  //--






  // Warning/Error folder slide show --   
  void PopUpSlides( TGListTreeItem *item,
		    const char* canvasname, const char *canvastitle );
  //void SearchWarningErrorFolder( TGListTreeItem *item, 
  //			 TObject *socket  );
  void SearchWarningFolder( TGListTreeItem *item, 
				 TObject *socket, Bool_t &FoundWarningFolder );
  void SearchWarningFolder( TConsumerInfo *info,
				 TObject *socket  );

  //--




  // Histo Content --
  TH1F* ContentHisto( TH1* histo);
  void DrawOnTheCanvas(
        const char* canvasname, const char* canvastitle, int padx, int pady, 
        TObject *obj );
  void DrawContentHisto(TH1* histo );

  TH1* HistoInPad( TVirtualPad *pad, Int_t &counthisto );
  //--





  // Print --
  char *fPrinter;
  char *fPrinterCommand;
  //void PrintCanvas( TCanvas *canvas );
  void PrintCanvas( TPad *canvas );
  TGTextButton *fPrintHistoButton;
  //--




  // Canvas name, title, number --
  /*
  TString CanvasName;
  TString CanvasTitle;
  Int_t CanvasNumber;
  */

  void OpenDefaultCanvas();

  // Change canvas name and title, size --
  void ChangeCanvasNameTitle( TCanvas *canvas );
  void DuplicateCanvasSize( TCanvas *guicanvas, TCanvas *consumercanvas );
  //--

  //--



  // Close --
  void CloseWindow();
  void ResetObjects();
  //--

  void MenuConnectStatus();
  void MenuHelp(char *s, const char *helpText);
  //void PopupWindow();


  char* HostPortFromID( const UInt_t id ) const;


public:

  HistoDisplay( const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~HistoDisplay();
  virtual Bool_t ProcessMessage( Long_t msg, Long_t parm1, Long_t parm2 );
  //Bool_t HandleUpdateTimer( TTimer *timer );



  // Open Input --
  enum EInputTypes {
    kTMapFile,
    kTFile,
    kTSocket,
    kTSOAP
  };
  TObject* OpenFile( const char *filename,
		     EInputTypes inputtype = kTFile,
		     Option_t *option = "READ" );
  TObject* CloseFile( const char *filename,
  	  EInputTypes inputtype = kTFile );
  TObject* CloseFile( TObject *inputobj );
  void CloseConnection();


  TObject* AutoOpen( const char *servername,
		     EInputTypes inputtype = kTSocket );

  TObject* AutoClose( const char *filename,
		      EInputTypes inputtype = kTSocket );

  void AutoOpenConsumer( const char *consumername );

  void OpenConsumerListBox();


  void AutoReconnect();

  //--


  ClassDef( HistoDisplay, 0 )

};






class HistoDisplayUpdateTimer : public TTimer {
private:
  HistoDisplay *fHistoDisplay;
  TVirtualPad *fCurrentPad;
public:
  HistoDisplayUpdateTimer( HistoDisplay *histodisplay, Long_t msec = 100,
			   Bool_t mode = kTRUE )
    : TTimer( msec, mode ) 
  { 
    fHistoDisplay = histodisplay;
    fCurrentPad = 0;
  }

  Bool_t Notify();

  void SetCurrentPad( TVirtualPad *pad )
  { fCurrentPad = pad; } 

  TVirtualPad* GetCurrentPad() const
  { return fCurrentPad; } 

  //ClassDef( HistoDisplayUpdateTimer, 0 )
};









class HistoDisplaySlideTimer : public TTimer {
private:
  HistoDisplay *fHistoDisplay;
  TGListTreeItem *fCurrentItem;
  TGListTreeItem *fDisplayingItem;
  TVirtualPad *fCurrentPad;
public:
  HistoDisplaySlideTimer( HistoDisplay *histodisplay, Long_t msec = 2000,
			   Bool_t mode = kTRUE )
    : TTimer( msec, mode ) 
  { 
    fHistoDisplay = histodisplay; 
    fCurrentItem = 0;
    fDisplayingItem = 0;
    fCurrentPad = 0;
  }

  Bool_t Notify();

  void SetCurrentItem( TGListTreeItem *item )
  { fCurrentItem = item; }

  TGListTreeItem* GetCurrentItem() const
  { return fCurrentItem; }

  void SetDisplayingItem( TGListTreeItem *item )
  { fDisplayingItem = item; }

  TGListTreeItem* GetDisplayingItem() const
  { return fDisplayingItem; }

  void SetCurrentPad( TVirtualPad *pad )
  { fCurrentPad = pad; } 

  TVirtualPad* GetCurrentPad() const
  { return fCurrentPad; } 

  //ClassDef( HistoDisplayUpdateTimer, 0 )
};




char *getword( char *str, const char commentchar = '#' );
char *strdelete( char *str, char *del );

const char *strstrn( const char *s, const char *k);

#endif



