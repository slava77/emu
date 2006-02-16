//////////////////////////////////////////////////////
//
// HistoDisplay.C
// Tetsuo Arisawa, 1999 0715
// 
//
// modified by Hans Wenzel to make it work within 
// CDF environment 
// modified by Frank Hartmann
// to allow socket connection to DisplayServer
// modified by  Koji Ikado 
// to display member pictures
//
// Version 1.5 1999 11/18 Tetsuo.Arisawa 
//  TCanvas in addition to TH1F* can be read and drawn. 
// Version 1.51 2000 2/17 T.A
//  TH1F are drawn maintaining original options as being sent.
//  GXW are replaced by ViratualX for the new ROOT version (by Hans).
//  GetDirPath are modified to match new YMON histo names.
//  Canvas Window Resize is tuned by 0.844.( Do not know why.)
// Version 1.52 2000 3/  T.A
//  Any histograms which inherits from TH1, (not TH1F any more)
//  can be handled. For example TProfile are included.
//  (Now objects which inherit from TH1 or TCanvas are displayed.)
//  Canvas Window Resize is 1, not 0.844 any more.
// Version 1.53 2000 3/21 Koji Ikado
//  "Choose File" button & "Clear" button are added.
// Version 1.55 2000 3/24 T.A
//  Added a button to zero histograms.
//  Currently the no of entries are cleared but not other staticsics
//  as mean or rms. Waiting new version of ROOT in CDF. (v2.23.*)
//  Also the codes of drawing are improved.
// Version 1.56 2000 3/25 T.A
//  Added a button to draw a bin-content histogram of a selected histogram. 
//  Requested by LBNL, ANL (Young-Kee Kim, Larry Noodleman, Greg Veramendi). 
//  Control buttons work much better with abling and disabling.
// Version 1.561 2000 4/01 T.A
//  Some TCanvas were not be drawn such as from YMon.cc.
// (ROOT v2.22.06. KCC_3_3. ). Now fixed.
// Version 1.57 2000 4/19 Koji Ikado
//  Functions of help windows from menu bar are modified.
//  Some "WARNING" message windows are added.
// Version 1.58 2000 4/19 T.A
//  Removing automatical resizing and clearing of canvas settings.
//  Allowing to set additional delay time (msec) for  automatical update.
// Version 1.581 2000 4/28 T.A
//  Histogram control buttons (zero, unzero, content) are separated.
// Version 1.582 2000 5/12 T.A & K.I
//  Layout is changed. Debugged for displaying canvas. 
//  Status bar is added(K.I).
// Version 1.583 2000 6/01 T.A
//  Debugged status bar implimentation errors and modified.
//  Errors at closing file with the stop or exit button are fixed.
//  Quit clone of TH1. Delete TList. BinContentHisto modified.
// Version 1.584 2000 6/05 H.S
//  modified socket connection part to work with the new server
// Version 1.59 2000 6/02 T.A
//  Changed so that everything on a canvas is updated automatically.
//  But the only one input source is allowed now.
//  Statistics of subtracted hists are modified
//  for newer version than ROOT v2.23.12.
// Version 1.60 2000 6/8 T.A
//  Changed UpdateHistoList.
// Version 1.61 2000 6/9 T.A
// Version 1.62 2000 6/16 T.A
//  CObject, CHistogram class is added.
// Version 1.63 2000 8/03 T.A
//  Debugged. But if you open hsimple.map after SVXMon.root
//  segmentation fault occurs. If inverse the order, no error.
// Version 1.64 2000 8/25 T.A
//  Debugged. Canvas Update works fine. Draw option is taken from input.
//  Tries several times if it failed to receive an object from a socket.
//  Button to clear currently active pad is added.
// Version 1.641 2000 8/28 T.A
//  Minor change for removing unnecessary Clear().
// Version 1.642 2000 9/1 T.A
//  With updated CHistogram and CObject. Better memory management.
// Version 1.643 2000 9/5 T.A
//  Recursive pad clear is added for memory leak till the ROOT
//  is updated. 
// Version 1.644 2000 9/6 T.A
//  Fixed to draw after reconnection. Current pad is stored.
// Version 1.645 2000 9/9 T.A
//  Recursive pad clear is debugged. Current pad update is modified.
// Version 1.65 2000 9/9 T.A, Wolfgang and Hartmut,  
//  As suggested by Philip Canal, modified
//  error handling of socket connection,
//  update method of histogram maintaining changes by users on canvas.   
// Version 1.651 2000 9/15 T.A
//  Pad update restoring changing by users on canvas.
//  More attributes of pad or canvas are copied from the original 
//  input.
// Version 1.652 2000 9/21 T.A
//  Debugged so that histo in pad from static file can be drawn normally.
//  DrawHisto is divided for TList and TObject draw.
//  Double click on List Tree draws an object. 
//  ContentHisto is drawn for histo in selected pad.
// Version 1.653 2000 9/24 T.A
//  Testing to avoid segmentation fault that occurs 
//  when canvas is clicked while painting. (Igor pointed out.)
// Version 1.66 2000 9/29 T.A
//  TConsumerInfo is read from input and ListTree is made based on it.
//  Layout changes of main frame after its' resize look better now.
//  ( Fixed according to suggestion by Igor. )
// Version 1.661 2000 10/02 T.A
//  TConsumerInfo can be read from text file "ConsumerInfo.txt"
//  so that users can control what to be read from the inputs.
//  Buttons Start and Open are not disabled.
//  After the input connection closed, "Open" and "Start" able to update histos.
//  More precautions for cases where gPad !=0 with no opened canvases.
// Version 1.7 2000 10/06
//  Slide Show canvas is implemented.
//  CDF Consumer Display Canvas names and titles are changed with numbers.
// Version 1.71 2000 10/06
//  Debugging so that others than TH1 or TPad can be updated.
//  Socket becomes the default, not TFile for input.
// Version 1.72 2000 10/08
//  ListTree Item shows tips or a title of the object when mouse is on it.
//  ( For above ROOT v2.25/03 )
// Version 1.8 2000 10/13
//  Timer is added.
// Version 1.81 2000 10/13
//  Print function is being added. 
// Version 1.82 2000 10/27
//  Canvas name and title (which is dispayed on the top of the GUI canvas)
//  are dynamically changed according to the objects on the display.
// Version 1.83 2000 10/28
//  GUI Canvas is resized when a consumer canvas is displayed
//  duplicating its size. ( deactivated 2000/11/3. )
//  Quit to draw additional pad to draw consumer objects on it.
// Version 1.831 2000 11/3
//  Histogram axis labels are redrawn by update
//  because axis labels may be changed.
//  ( Some consumer put run no and no of events on the x axis label.)
// Version 1.832 2000/12/08
//  For ROOT 2.26.00 and for EGCS
// Version 1.84 2001/01/19
//  Popup a current canvas for slide show.
//  Cleaned up code for slide update.
// Version 1.85 2001/01/31
//  Slide show on one window.
// Version 1.852 2001/02/09
//  Slide show with timer update.
//  Slide shows resume on a canvas where the slide show was shown,
//  not on a current active canvas.
//  Actions of Pause and Update buttons are rearranged.
//  Cleared the use of varible Running:
//  Running = kTRUE means some timer is running.
//  By inserting Running = kFALSE, you can interrupped all timers.
//  Change default and min frequencies to 5sec and 800msec. (Wolfgang requests)
//  Included ServerProtocol.hh.
// Version 1.853 2001/02/11
//  Changed the Update Timer.
//  A pad where update timer started is continued to be updated
//  even after current active pad are changed.
// Version 1.854 2001/02/20
//  Debugged error which occured when you start slide show without a canvas
//  window. 
// Version 1.855 2001/02/22
//  Debugged that drawing option of objects in consumer canvases are
//  reproduced when they are displayed. Especially overlaid histos
//  are shown properly.
// Version 1.86 2001/03/08
//  ListTree updates. Redraw ListTree with new TConsumerInfo.
// Version 1.87 2001/03/12
//  Removing buttons. start, exit.
// Version 1.88 2001/03/26
//  Objects are not fetched from inputobj ( e.g. socket )
//  when open button is clicked
//  because it consumes long time.
//  Reading object is done each time when you tried to 
//  draw a List Tree item one by one.
//  Refresh is added to the List Tree menu item.
// Version 1.9 2001/03/28
//  PopUp warning/error slide show starts when new TConsumerInfo
//  that contains warning or error folder.
//  Debugged so that you can reconnect to the same socket
//  by pushing "open" button.
// Version 1.91 2001/04/06
//  Debugged so that canvases are drawn without bad X11 window allocation.
// Version 2.00 2001/05/15
//  AutoOpen, AutoOpenConsumer, HostPortFromID, ConsumerListBox class.
// Version 2.01 2001/06/13
//  Two modification according to Kaori-san's requests.
//  1.Auto Update mode becomes default.
//  2.Pull-down menu item of connection status opens the new ConsumerListBox.
//  To cope with TGraph update problem reported by Igor of SVXMon,
//  modified so that options of various objects are maintained at updates.
// Version 2.02 2001/06/15
//  Attributes of objects are changed correctly at update.
// Version 2.03 2001/
// Version 2.04 2001/09/20
//  1.Debugged so that slide show starts for one item 
//    in slide show and warning folders.
//  2.Text field for a file/socket name accepts spaces before or after
//    the name.
// Version 3.00 2001/12/11
//  1. List tree is updated according to new TConsumerInfo.
//  ( Warning canvas slide show starts and stops accordingly. ) 
//  2. TGraph can be updated even if the editable bit is not set on a pad.
//  
// Version 3.01 2002/4/11
//  Testing signal handling.
//  USR1 signal to this process make it to reconnect to all the consumers
//  which have been connected so far. The consumers should be in the 
//  ConsumerList in the web.
// Version 3.02 2002/5/13
//  After receiving  Open button event or USR1 signal to reconnect to the
//  display servers,
//  the Update button or double clicking on the list tree
//  draw new canvases as expected.
// Version 3.03 2002/5/15
//  Modified GetCObjFromListItem. The UserData of item changed to 
//  CObject. 
// Version 3.04 2002/6/5
//  Changing GUI layout to enlarge List Tree space
//  with other layout modifications.
// Version 3.05 2002/7/11
//  Socket error handling is updated.
// Version 3.06 2002/8/8
//  If WarningFolder becomes empty then the WarningCanvas is closed
//  automatically.
//  If the consumer status is "Old" in addition to "Running",
//  connection with HistoDisplay is considered.
// Version 3.07 2002/8/29
//  1. Handling the case when warning objects exist at the beginning 
//   of the connection.
//  2. Even Warning Canvas is selected as an active pad, double clicked items are
//  drawn on a newly opened canvas.
// Version 3.071 2002/9/05
//  To handle Root v.3.03/08 TPaveStats destructor problem, RecursivePadClear
//  is updated.
// Version 3.072 2002/9/06
//  Fix so that the consumer object with name including "slide" can be
//  drawn by double click.
// Version 3.073 2002/10/15
//  1. Because background pad color specified by YMon is not restored 
//   in the display side when you open ROOT files, (it was ok for socket connection), 
//   "gROOT->ForceStyle()" is commented out. 
//  2. There were reports that "YMon/SlideShowerMax" is not displayed by double click.
//    Changed so that slide show starts when the clicked folder name is 
//    just "Slides" and the folder has items in it.
// Version 3.074 2002/11/07
//  1. "std::" are added such as
//   std::endl;, std::ostream, std::ends, std::max.
//---------------------------------------------------------------------
// the following lines are a work around for a "known" bug in the IRIX6_2
// unistd.h header file
#include <sys/types.h>
#if defined( OSF1 )
#include <unistd.h>
#else
#include <sys/unistd.h>
#endif
//
// IRIXX 6.2 bug fix 
//
#if defined( IRIX6_2 )
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE_EXTENDED==1)
     static pid_t vfork (void);
#define __vfork vfork
#endif   
#endif
// end of work around 
//---------------------------------------------------------------------

/*
extern "C" {
#include <unistd.h>
}
*/


#ifdef ROOT_2_22
#include "TGXW.h"
#define gVirtualX gGXW
#else
#include "TVirtualX.h"
#endif

#include <TROOT.h> //gROOT 
#include <TStyle.h> //gStyle
#include <TApplication.h>
#include <TGClient.h>

#include <TString.h>
#include <TRegexp.h>

#include <TGMenu.h>

//#include <TGToolBar.h> //TGHorizontal3DLine
#ifndef ROOT_2_24
#include <TG3DLine.h> //TGHorizontal3DLine //2000/12/08 v2.26
#else
#include <TGSplitter.h> //TGHorizontal3DLine //2000/12/04 v2.25.03
#endif

#include <TRootHelpDialog.h> 
#include <HelpText.h> //gHelpAbout, gHelpCanvas
#include <TGTextView.h>

#include <TGLabel.h>
#include <TGTextEntry.h> //GContext_t
#include <TGLayout.h> //TGLayoutHints
#include <TGButton.h> //TGPictureButton, TGCheckButton

#include <TFile.h>
class TMapRec;
// needed by KCC 4.0 to correct problem in root v2_24_04c: 
// (Thanks Chris.)
class TGStatusBarPart; 
#include <TMapFile.h> //TMapFile, TMapRec

#include <TList.h>

#include <TCollection.h> // TIter
#include <TKey.h>
#include <TObject.h>
#include <TH1.h>
#include <TClass.h> // TH1F;;Class()

#include <TGListTree.h>

#include <TGWindow.h>
#include <TGCanvas.h>
#include <TCanvas.h>
#include <TTimer.h>
#include <TSystem.h> // TProcessEventTimer
#include <TVirtualPad.h> //gPad

#include <TGFileDialog.h> //TGFileInfo
#include <TGMsgBox.h>

#include <TGFrame.h>
#include <TGStatusBar.h>



#include <TUrl.h>

#include <TFrame.h>

#include <TTimer.h>
#include <TGTextEditDialogs.h> // TGPrintDialog

#include <TSocket.h>
#include <TMessage.h>

// tmp for v3.03/08
#include <TPaveStats.h>

#include <TPaveLabel.h> //04.21.2005 Cavanaugh


#include <string>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <fstream>
#include <ctype.h>

#include <sstream>


#include "HistoDisplay.hh"
#include "HistoDisplayHelpText.hh" //gHelpHistoDisplay
#include "HistoDisplayPhoto.hh"
#include "ConsumerList.hh"

#include "ConsumerListBox.hh"


#include "TConsumerInfo.hh"

#include "CObject.hh"
#include "CHistogram.hh"

//using std::string;
#include "ServerProtocol.hh"

#include "xoap.h"
/*
#include "xoap/SOAPMessage.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPName.h"
#include "xoap/AttachmentPart.h"
#include "xoap/SOAPAllocator.h"
#include "xoap/SOAPConnection.h"
*/
using namespace xoap;
using namespace std;
// #include "xdaqExecutive.h"

enum HistoDisplayMessageTypes {
  kM_MENU_FILE_NEWGUI,
  kM_MENU_FILE_NEWCANVAS,
  kM_MENU_FILE_NEWSLIDE,
  kM_MENU_FILE_CLOSE,
  kM_MENU_FILE_EXIT,
  kM_MENU_LISTTREE_SORTTREE,
  kM_MENU_LISTTREE_SORTFOLDER,
  kM_MENU_LISTTREE_DELETE,
  kM_MENU_LISTTREE_CLEAR,
  kM_MENU_LISTTREE_REFRESH,
  kM_MENU_CONNECT_STATUS,
  kM_MENU_HELP_GUI,
  kM_MENU_HELP_ROOT,
  kM_MENU_HELP_CANVAS,
  kM_PICTBUTTON,
  kM_RABUTTON_MFILE,
  kM_RABUTTON_RFILE,
  kM_RABUTTON_SOCKET,
  kM_RABUTTON_SOAP,
  kM_BUTTON_FILENAME,
  kM_BUTTON_REFNAME,  // Cavanaugh 03.04.2005
  kM_BUTTON_CHOOSEFILE,
  kM_BUTTON_CLEARFILENAME,
  kM_BUTTON_PAUSE,
  kM_BUTTON_CLEAR,
  kM_BUTTON_PRINT,
  kM_BUTTON_RESTART,
  kM_BUTTON_ZERO,
  kM_BUTTON_UNZERO,
  kM_BUTTON_REF,  // Cavanaugh 03.04.2005
  kM_BUTTON_CONTENT,
  kM_BUTTON_STOP,
  kM_CHBUTTON_AUTOUPDATE,
  kM_CHBUTTON_SLIDEUPDATE,
  kM_TEXTENTRY_FILENAME,
  kM_TEXTENTRY_MONITORNAME,
  kM_TEXTENTRY_FREQUENCY
};




// slide show starts when following names are included in 
// TGListTreeItem name.
//v.3072
//const char *kSlideShowFolder = "slide";
const char *kSlideShowFolder = "Slides";

//const char *kWarningFolder = "Warning!/";
//const char *kErrorFolder = "Warning!/";
const char *kWarningFolder = "Warning!";
const char *kErrorFolder = "Warning!";
const char *kWarningCanvasName = "WarningCanvas";
const char *kWarningCanvasTitle = "Warning Canvas";








#include <signal.h>
#include <TSysEvtHandler.h>

class HistoDisplaySigHandler: public TSignalHandler {

private:
  ESignals fHDSignal;
  HistoDisplay *fHistoDisplay;

public:
  HistoDisplaySigHandler( ESignals sig, HistoDisplay *histoDisplay);
  Bool_t Notify();
};


HistoDisplaySigHandler::
HistoDisplaySigHandler( ESignals sig, HistoDisplay *histodisplay )
  :TSignalHandler(sig)
{
  fHDSignal = sig;
  fHistoDisplay = histodisplay;
  Add();
}

Bool_t HistoDisplaySigHandler::
Notify()
{
  if ( fHistoDisplay ){
    if ( fHDSignal == kSigUser1 ) {
      std::cout<< " Handling USR1 " << std::endl;
      fHistoDisplay->AutoReconnect();
    }
    else if ( fHDSignal == kSigUser2 ) {
      std::cout<< " Handling SIGUSR2 " << std::endl;
    }
    else if ( fHDSignal == kSigPipe ) {
      std::cout<< " Handling SIGPIPE " << std::endl;
    }
    else
      std::cout << " Handler of the signal is not implemented" << std::endl;

  }//if ( fHistoDisplay )

  return kTRUE;

}




ClassImp(HistoDisplay)
  //ClassImp(HistoDisplayUpdateTimer)




HistoDisplay::HistoDisplay( const TGWindow *p, UInt_t w, UInt_t h)
: TGMainFrame(p, w, h)
  //, consumerlist(new ConsumerList("http://www-b0.fnal.gov:8000/consumer/consumer_status.html"))
  , consumerlist(new ConsumerList("http://www-cdfonline.fnal.gov/consumer/consumer_status.html"))
{
  //
  // hjw
  // first check if the enevironment is set correctly:
  //
  REF_FLAG = 0;         // Cavanaugh 03.04.2005
  char *c_ptr;
  c_ptr = getenv("CONSUMER_ICONS");
  if(c_ptr==NULL){
    std::cout<<"Unix logical CONSUMER_ICONS is not set, will not be able to load all icons"<<std::endl;
  }



  gStyle->SetPalette(1); //By Greg.


  //02/10/11 Commented out to restore pad filled color of such
  //as YMon/Slide/SlideCOT1 from ROOT file. ( It was ok from socket. ) 
  //gROOT->ForceStyle();


  gClient->GetColorByName("LightYellow",LightYellow);



  MakeMenuBar();

  fMainFrame =
    new TGCompositeFrame(this, 150, 300, kVerticalFrame); 

  fMainFrameLayout =
    new TGLayoutHints( kLHintsCenterX | kLHintsCenterY |
		       kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1); 


  char fTitle[] = "  EMU DQM Consumer Display ";
  MakeTitleFrame(fTitle);

  MakeInputFrame();

  MakeTGListTreeFrame();
  // MakeTGRefListTreeFrame();  // Cavanaugh 03.04.2005

  MakeStatusBar();

  AddFrame(fMainFrame,fMainFrameLayout); 

  SetWindowName(" CMS DQM Consumer Display Main Frame");

  Resize( GetDefaultSize() );
  Layout();

  MapSubwindows();
  MapWindow();


  fSelectedItem = 0; // to prevent segmentation fault

  fDisplayedList = new TList();
  fDisplayedRefList = new TList();  // Cavanaugh 03.04.2005
  fConsumerInfos = new TList();


  SlideShowUpdate = kFALSE;
  fSlideCanvasList = new TList();

  /*
  CanvasName = "CDFDisplayCanvas"; 
  CanvasTitle = "CDF Consumer Display Canvas ";
  CanvasNumber = 0;
  */


  fUpdateTimer = new HistoDisplayUpdateTimer( this, 100, kTRUE );
  fSlideTimer = new HistoDisplaySlideTimer( this, 2000, kTRUE );

  Running = kFALSE;

  fPrinter = 0;
  fPrinterCommand = 0;

  fConsumerListBox = 0;



  HistoDisplaySigHandler *handler = 
    new HistoDisplaySigHandler( kSigUser1, this );

    XMLPlatformUtils::Initialize();

}


void HistoDisplay::MakeMenuBar()
{
  // File Menu --
  fFileMenu = new TGPopupMenu( fClient->GetRoot() );
  fNewCascadeMenu = new TGPopupMenu( fClient->GetRoot() );
  fNewCascadeMenu->AddEntry( "&GUI", kM_MENU_FILE_NEWGUI );
  fNewCascadeMenu->AddEntry( "&Canvas", kM_MENU_FILE_NEWCANVAS );
  fNewCascadeMenu->AddEntry( "&Auto-Update Canvas", kM_MENU_FILE_NEWSLIDE );
  fFileMenu->AddPopup( "&New", fNewCascadeMenu );
  fFileMenu->AddSeparator();
  fFileMenu->AddEntry( "&Close", kM_MENU_FILE_CLOSE );
  fFileMenu->AddEntry( "&Exit", kM_MENU_FILE_EXIT );
  //--

  // List Tree --
  fListTreeMenu = new TGPopupMenu( fClient->GetRoot() );
  fSortCascadeMenu = new TGPopupMenu( fClient->GetRoot() );
  fSortCascadeMenu->AddEntry( "&Tree", kM_MENU_LISTTREE_SORTTREE );
  fSortCascadeMenu->AddEntry( "In &Folder", kM_MENU_LISTTREE_SORTFOLDER );
  fListTreeMenu->AddPopup( "&Sort", fSortCascadeMenu );
  fListTreeMenu->AddSeparator();
  fListTreeMenu->AddEntry( "&Delete", kM_MENU_LISTTREE_DELETE );
  fListTreeMenu->AddEntry( "&Clear", kM_MENU_LISTTREE_CLEAR );
  fListTreeMenu->AddEntry( "&Refresh", kM_MENU_LISTTREE_REFRESH );
  //--

  // Connection Menu --
  fConnectMenu = new TGPopupMenu( fClient->GetRoot() );
  fConnectMenu->AddLabel( "CMS Monitors");
  fConnectMenu->AddSeparator();
  fConnectMenu->AddEntry( "&Status...", kM_MENU_CONNECT_STATUS );
  //--

  // Help Menu --
  fHelpMenu = new TGPopupMenu( fClient->GetRoot() );
  fHelpMenu->AddEntry( "&About This GUI", kM_MENU_HELP_GUI );
  fHelpMenu->AddSeparator();
  fHelpMenu->AddEntry( "&About ROOT", kM_MENU_HELP_ROOT );
  fHelpMenu->AddEntry( "&About Canvas", kM_MENU_HELP_CANVAS );
  //--

  // Associate Menu Commands --
  fFileMenu->Associate(this);
  fNewCascadeMenu->Associate(this);
  fListTreeMenu->Associate(this);
  fSortCascadeMenu->Associate(this);
  fConnectMenu->Associate(this);
  fHelpMenu->Associate(this);
  //--

  // Create Menu Bar --
  fMenuBar = new TGMenuBar( this, 1, 1, kHorizontalFrame );
  fMenuBarLayout = 
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX,
		       0, 0, 1, 1);
  fMenuBarItemLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft, 0, 4, 0, 0 );
  fMenuBarHelpLayout =
    new TGLayoutHints( kLHintsTop | kLHintsRight );

  fMenuBar->AddPopup( "&File", fFileMenu, fMenuBarItemLayout );
  fMenuBar->AddPopup( "&List Tree", fListTreeMenu, fMenuBarItemLayout );
  fMenuBar->AddPopup( "&Connection", fConnectMenu, fMenuBarItemLayout );
  fMenuBar->AddPopup( "&Help", fHelpMenu, fMenuBarHelpLayout );

  AddFrame( fMenuBar, fMenuBarLayout );
  //--

  // Separator --
  fMenuBarSeparator = new TGHorizontal3DLine( this );
  fMenuBarSeparatorLayout =
    new TGLayoutHints( kLHintsTop | kLHintsExpandX );
  AddFrame( fMenuBarSeparator, fMenuBarSeparatorLayout );
  //--
}


void HistoDisplay::CloseMenuBar()
{
  delete fNewCascadeMenu;
  delete fFileMenu;
  delete fSortCascadeMenu;
  delete fListTreeMenu;
  delete fConnectMenu;
  delete fHelpMenu;
  delete fMenuBarItemLayout;
  delete fMenuBarHelpLayout;
  delete fMenuBar;
  delete fMenuBarLayout;

  delete fMenuBarSeparator;
  delete fMenuBarSeparatorLayout;
  delete consumerlist;
}



void HistoDisplay::MakeTitleFrame(const char *fTitle)
{
   // Title Frame with logo picture --

   fTitleFrame = new TGCompositeFrame(fMainFrame,100,30,kHorizontalFrame);
   fTitleFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX,
					 0, 0, 0, 0);
   FontStruct_t labelfont;
   labelfont = gClient->GetFontByName(
              "-*-times-bold-r-*-*-24-*-*-*-*-*-*-*");
   GCValues_t   gval;
   gval.fMask = kGCForeground | kGCFont;
   gval.fFont = gVirtualX->GetFontHandle(labelfont);
   gClient->GetColorByName("red", gval.fForeground);
   fTextGC = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval);

   fTitleLabel       = new TGLabel(fTitleFrame, fTitle , fTextGC, labelfont);
   fTitleLabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
					 20, 10, 10, 10);

   fTitleFrame->AddFrame(fTitleLabel, fTitleLabelLayout);

   fPicBut = 
     new TGPictureButton( fTitleFrame,
			  gClient-> GetPicture("${CONSUMER_ICONS}/cms_logo.xpm"),
			  kM_PICTBUTTON);

   fPicBut->Resize( 85, 85 );

   //fPictLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 4, 4, 0);
   fPictLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 4, 4, 4, 0);

   fPicBut->Associate(this);
   fTitleFrame->AddFrame(fPicBut, fPictLayout);

   fMainFrame->
     AddFrame(fTitleFrame,fTitleFrameLayout);
   //--
}


void HistoDisplay::CloseTitleFrame()
{
  gVirtualX->DeleteGC(fTextGC);
  delete fTitleLabel;
  delete fTitleLabelLayout;
  delete fPicBut;
  delete fPictLayout;
  delete fTitleFrame;
  delete fTitleFrameLayout;
}



void HistoDisplay::MakeInputFrame()
{
  // Input stream Frame --

  fFileNameFrame =
    new TGGroupFrame(fMainFrame, "Input Stream", kVerticalFrame);
  fFileNameFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX
		       //, 4, 4, 2, 2);
		       , 2, 2, 1, 1);

  // Radio Buttons --
  fRaButMFile =
    new TGRadioButton(fFileNameFrame,
		       "Memory Map File", kM_RABUTTON_MFILE);
  fRaButRFile =
    new TGRadioButton(fFileNameFrame, 
		       "Other ROOT File", kM_RABUTTON_RFILE);
  fRaButSocket =
    new TGRadioButton(fFileNameFrame,
		       "Socket Connection", kM_RABUTTON_SOCKET);
  fRaButSOAP =
    new TGRadioButton(fFileNameFrame,
                       "SOAP Connection", kM_RABUTTON_SOAP); 
   
  fRaButLayout =
    new TGLayoutHints(kLHintsTop | kLHintsLeft, 2,2,2,2);

  fRaButMFile->Associate(this);
  fRaButRFile->Associate(this);
  fRaButSocket->Associate(this);
  fRaButSOAP->Associate(this);

  fFileNameFrame->AddFrame(fRaButMFile, fRaButLayout);
  fFileNameFrame->AddFrame(fRaButRFile, fRaButLayout);
  fFileNameFrame->AddFrame(fRaButSocket, fRaButLayout);
  fFileNameFrame->AddFrame(fRaButSOAP, fRaButLayout);
  
  fRaButMFile->SetState(kButtonUp);
  fRaButRFile->SetState(kButtonUp);
  fRaButSocket->SetState(kButtonDown);

  //--


  // File Name --

  // Label --
  fFileNameLabel = 
    new TGLabel( fFileNameFrame, "Name of File / Socket Server:Port" );
  fFileNameLabelLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft, 3,3,2,2);
  fFileNameFrame->
    AddFrame( fFileNameLabel, fFileNameLabelLayout );
  //--

  // File Name Text Buffer --
  fFileOpenFrame =
    new TGCompositeFrame(fFileNameFrame, 60, 20, kHorizontalFrame);
  fFileOpenFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 1,1,2,2);

  fFileNameTextEntry =
    new TGTextEntry( fFileOpenFrame, 
		     //fFileNameBuffer = new TGTextBuffer(320),
		     fFileNameBuffer = new TGTextBuffer(220),
		     kM_TEXTENTRY_FILENAME );
  fFileNameTextEntryLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 2,2,2,2);

  fFileNameTextEntry->
    //Resize(340,fFileNameTextEntry->GetDefaultHeight());
    Resize(240,fFileNameTextEntry->GetDefaultHeight());
  fFileOpenFrame->
    AddFrame( fFileNameTextEntry, fFileNameTextEntryLayout );

  fReadFileButton =
    new TGTextButton(fFileOpenFrame,"  &Open  ", kM_BUTTON_FILENAME);
  fReadFileButtonLayout =
    new TGLayoutHints( kLHintsTop | kLHintsRight, 15,0,2,2);
  fReadFileButton->Associate(this);
  fReadFileButton->SetToolTipText("Open local/remote file or socket.");

  //gClient->GetColorByName("LightYellow",LightYellow);
  fReadFileButton->ChangeBackground(LightYellow);

  fFileOpenFrame->
    AddFrame( fReadFileButton, fReadFileButtonLayout );

  fFileNameFrame->
    AddFrame( fFileOpenFrame, fFileOpenFrameLayout );
  //--

//Begin Cavanaugh 03.04.2005 -----------------------------------------------------------------------
  // Label --
  fFileNameLabel = 
    new TGLabel( fFileNameFrame, "Name of Reference File / Socket Server:Port" );
  fFileNameLabelLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft, 3,3,2,2);
  fFileNameFrame->
    AddFrame( fFileNameLabel, fFileNameLabelLayout );
  //--

  // File Name Text Buffer --
  fFileOpenFrame =
    new TGCompositeFrame(fFileNameFrame, 60, 20, kHorizontalFrame);
  fFileOpenFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 1,1,2,2);

  fFileNameTextEntry =
    new TGTextEntry( fFileOpenFrame, 
		     //fFileNameBuffer = new TGTextBuffer(320),
		     fRefNameBuffer = new TGTextBuffer(220),
		     kM_TEXTENTRY_FILENAME );
  fFileNameTextEntryLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 2,2,2,2);

  fFileNameTextEntry->
    //Resize(340,fFileNameTextEntry->GetDefaultHeight());
    Resize(240,fFileNameTextEntry->GetDefaultHeight());
  fFileOpenFrame->
    AddFrame( fFileNameTextEntry, fFileNameTextEntryLayout );

  //fReadFileButton =
  //  new TGTextButton(fFileOpenFrame,"  &Open  ", kM_BUTTON_REFNAME);
  //fReadFileButtonLayout =
  //  new TGLayoutHints( kLHintsTop | kLHintsRight, 15,0,2,2);
  //fReadFileButton->Associate(this);
  //fReadFileButton->SetToolTipText("Open local/remote file or socket.");

  //gClient->GetColorByName("LightYellow",LightYellow);
  //fReadFileButton->ChangeBackground(LightYellow);

  fFileOpenFrame->
    AddFrame( fReadFileButton, fReadFileButtonLayout );

  fFileNameFrame->
    AddFrame( fFileOpenFrame, fFileOpenFrameLayout );

  // Label --
  fFileNameLabel = 
    new TGLabel( fFileNameFrame, "Name of Dynamic Rule/Ref Socket Server:Port" );
  fFileNameLabelLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft, 3,3,2,2);
  fFileNameFrame->
    AddFrame( fFileNameLabel, fFileNameLabelLayout );
  //--

  // File Name Text Buffer --
  fFileOpenFrame =
    new TGCompositeFrame(fFileNameFrame, 60, 20, kHorizontalFrame);
  fFileOpenFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 1,1,2,2);

  fFileNameTextEntry =
    new TGTextEntry( fFileOpenFrame, 
		     //fFileNameBuffer = new TGTextBuffer(320),
		     fDynRefNameBuffer = new TGTextBuffer(220),
		     kM_TEXTENTRY_FILENAME );
  fFileNameTextEntryLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft | kLHintsExpandX, 2,2,2,2);

  fFileNameTextEntry->
    //Resize(340,fFileNameTextEntry->GetDefaultHeight());
    Resize(240,fFileNameTextEntry->GetDefaultHeight());
  fFileOpenFrame->
    AddFrame( fFileNameTextEntry, fFileNameTextEntryLayout );

  //fReadFileButton =
  //  new TGTextButton(fFileOpenFrame,"  &Open  ", kM_BUTTON_REFNAME);
  //fReadFileButtonLayout =
  //  new TGLayoutHints( kLHintsTop | kLHintsRight, 15,0,2,2);
  //fReadFileButton->Associate(this);
  //fReadFileButton->SetToolTipText("Open local/remote file or socket.");

  //gClient->GetColorByName("LightYellow",LightYellow);
  //fReadFileButton->ChangeBackground(LightYellow);

  fFileOpenFrame->
    AddFrame( fReadFileButton, fReadFileButtonLayout );

  fFileNameFrame->
    AddFrame( fFileOpenFrame, fFileOpenFrameLayout );

  //--
  //End Cavanaugh 03.04.2005 -----------------------------------------------------------------------


  // Choose File, Clear buttons. --
  fFileNameButtonFrame =
    new TGCompositeFrame(fFileNameFrame, 60, 70, kHorizontalFrame);

  fFileNameButtonFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft, 2,2,2,0);



  fChooseFileButton =
    new TGTextButton(fFileNameButtonFrame,"&Choose...",
		     kM_BUTTON_CHOOSEFILE);
  fChooseFileButton->SetToolTipText("Choose Local file/ display servers");

  fChooseFileButton->Associate(this);

  fFileNameButtonFrame->
    AddFrame( fChooseFileButton,
	      ( fChooseFileButtonLayout =
		new TGLayoutHints(kLHintsTop | kLHintsCenterX, 3,5,2,0) )
	      );

  fChooseFileButton->ChangeBackground(LightYellow);




  fClearFileNameButton =
    new TGTextButton(fFileNameButtonFrame,"   &Clear   ",
		     kM_BUTTON_CLEARFILENAME);

  fClearFileNameButton->SetToolTipText("Clear Input Source Name");

  fClearFileNameButton->Associate(this);

  fFileNameButtonFrame->
    AddFrame( fClearFileNameButton, 
	      ( fClearFileNameButtonLayout =
		new TGLayoutHints(kLHintsTop | kLHintsRight, 3,5,2,0) )
	      );

  fClearFileNameButton->ChangeBackground(LightYellow);



  fFileNameFrame->
    AddFrame( fFileNameButtonFrame, fFileNameButtonFrameLayout );
  //--



  fMainFrame->
    AddFrame( fFileNameFrame, fFileNameFrameLayout  );
  //--
}


void HistoDisplay::CloseInputFrame()
{
  delete fRaButMFile;
  delete fRaButRFile;
  delete fRaButSocket;
  delete fRaButSOAP;
  delete fRaButLayout;

  delete fFileNameLabel; 
  delete fFileNameLabelLayout;

  delete fFileNameBuffer;
  delete fFileNameTextEntry;
  delete fFileNameTextEntryLayout;

  delete fReadFileButton;
  delete fReadFileButtonLayout;

  delete fFileOpenFrame;
  delete fFileOpenFrameLayout;

  delete   fChooseFileButton;
  delete   fChooseFileButtonLayout;
  delete   fClearFileNameButton;
  delete   fClearFileNameButtonLayout;
  delete   fFileNameButtonFrame;
  delete   fFileNameButtonFrameLayout;


  delete fFileNameFrameLayout;
  delete fFileNameFrame;
}



void HistoDisplay::MakeTGListTreeFrame()
{
  fListTreeMainFrame =
    new TGCompositeFrame(fMainFrame,150,50,kVerticalFrame);
  fListTreeMainFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY
		       , 4, 4, 4, 4);



  // Make List Tree Frame --

  fHistoListFrame =
    new TGCompositeFrame(fListTreeMainFrame, 150, 150, kHorizontalFrame);
  fHistoListFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY,
		       1, 1, 1, 1); 


  // Make List Tree --
  fTGCanvas =
    //new TGCanvas(fHistoListFrame, 210, 150, kSunkenFrame | kDoubleBorder);
    new TGCanvas(fHistoListFrame, 110, 100, kSunkenFrame | kDoubleBorder);
  fTGCanvasLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY );

  fHistoListTree = new TGListTree(fTGCanvas->GetViewPort()
				  //, 210, 250, kHorizontalFrame);
				  , 20, 20, kHorizontalFrame);

  fHistoListTree->Associate(this);
  fTGCanvas->SetContainer(fHistoListTree);

  //tmp 2000/12/08
  fHistoListTree->SetAutoTips();


  fHistoListFrame->AddFrame(fTGCanvas,fTGCanvasLayout);
  //--


  // Button Frame of List Tree --
  fListButtonFrame =
    new TGCompositeFrame(fHistoListFrame, 80, 100,
			 kVerticalFrame );
  fListButtonFrameLayout = 
    //new TGLayoutHints( kLHintsRight | kLHintsCenterY | kLHintsExpandX 
		       //, 2, 2, 5, 5);
    new TGLayoutHints( kLHintsRight | kLHintsCenterY
		       , 5, 5, 10, 0);



  // AutoUpdate Check Button --
  fChButtonAutoUpdate = 
    new TGCheckButton(fListButtonFrame, "Auto Update",
  	      kM_CHBUTTON_AUTOUPDATE);
  fChButtonAutoUpdateLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft,5,0,6,0);

  fChButtonAutoUpdate->Associate(this);
  fListButtonFrame->
    AddFrame( fChButtonAutoUpdate, fChButtonAutoUpdateLayout );
  fChButtonAutoUpdate->SetState(kButtonDown);
  AutoUpdate = kTRUE;
  //--


  // Slide Show Check Button --
  fChButtonSlideUpdate = 
    new TGCheckButton(fListButtonFrame, "Multi Canvas Update",
  	      kM_CHBUTTON_SLIDEUPDATE);
  fChButtonSlideUpdate->Associate(this);
  fListButtonFrame->
    AddFrame( fChButtonSlideUpdate, fChButtonAutoUpdateLayout );
  fChButtonSlideUpdate->SetState(kButtonDisabled);
  SlideShowUpdate = kFALSE;
  //--


  // Update Frequency --
  fFrequencyFrame = 
    new TGCompositeFrame(fListButtonFrame, 100, 50,
			 kHorizontalFrame );
  fFrequencyFrameLayout = 
    new TGLayoutHints( kLHintsTop | kLHintsLeft );

  fFrequencyLabel =
    //new TGLabel( fFrequencyFrame, "Update Delay (msec)" );
    new TGLabel( fFrequencyFrame, "Delay (msec)" );
  fFrequencyFrame->
    AddFrame( fFrequencyLabel, fChButtonAutoUpdateLayout);



  fFrequencyTextEntry =
    new TGTextEntry( fFrequencyFrame,
		     fFrequencyTextBuffer = new TGTextBuffer(40),
		     kM_TEXTENTRY_FREQUENCY );
  fFrequencyTextBuffer->AddText(0,  "5"); // 04.21.2005 Cavanaugh

  fFrequencyTextEntry->
    Resize( 50, fFrequencyTextEntry->GetDefaultHeight() );

  fFrequencyFrame->
    AddFrame( fFrequencyTextEntry, fChButtonAutoUpdateLayout);

  fListButtonFrame->
    AddFrame(fFrequencyFrame, fFrequencyFrameLayout);

  //--


  // Control Buttons --

  fControlButtonFrame =
    //new TGCompositeFrame( fListButtonFrame, 200, 300, kHorizontalFrame );
    new TGCompositeFrame( fListButtonFrame, 100, 50, kVerticalFrame );

  fControlButtonFrameLayout =
    new TGLayoutHints( kLHintsCenterX, 5,5,5,0);



  // List Tree Control Buttons --
  fButtonFrame =
    new TGCompositeFrame(fControlButtonFrame, 90, 50,
			 kVerticalFrame | kFixedWidth );
  fButtonFrameLayout = 
    new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 5, 5, 5);

  fButtonLayout =
    new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,
		      3, 3, 6, 0);

  fRestartHistoButton =
    new TGTextButton(fButtonFrame, "&Update", kM_BUTTON_RESTART);
  fRestartHistoButton->Associate(this);
  fRestartHistoButton->SetToolTipText("Update Current Pad");

  fPauseHistoButton = 
    new TGTextButton(fButtonFrame, "&Pause", kM_BUTTON_PAUSE); 
  fPauseHistoButton->Associate(this);
  fPauseHistoButton->SetToolTipText("Pause Updating Current Pad");

  fClearHistoButton = 
    new TGTextButton(fButtonFrame, "&Clear", kM_BUTTON_CLEAR); 
  fClearHistoButton->Associate(this);
  fClearHistoButton->SetToolTipText("Clear Current Pad");

  fPrintHistoButton = 
    new TGTextButton(fButtonFrame, "&Print...", kM_BUTTON_PRINT); 
  fPrintHistoButton->Associate(this);
  //fPrintHistoButton->SetToolTipText("Print Current Canvas");
  fPrintHistoButton->SetToolTipText("Print Current Pad");

  fStopHistoButton = 
    //new TGTextButton(fButtonFrame, "&Reset", kM_BUTTON_STOP);
    new TGTextButton(fButtonFrame, "&Reset All", kM_BUTTON_STOP);
  fStopHistoButton->Associate(this);
  fStopHistoButton->SetToolTipText("Reset All");


  fButtonFrame->AddFrame(fRestartHistoButton, fButtonLayout);
  fButtonFrame->AddFrame(fPauseHistoButton, fButtonLayout);
  fButtonFrame->AddFrame(fClearHistoButton, fButtonLayout);
  fButtonFrame->AddFrame(fPrintHistoButton, fButtonLayout);
  fButtonFrame->AddFrame(fStopHistoButton, fButtonLayout);

  fPauseHistoButton->SetState(kButtonDisabled);

  fPauseHistoButton->ChangeBackground(LightYellow);
  fClearHistoButton->ChangeBackground(LightYellow);
  fPrintHistoButton->ChangeBackground(LightYellow);
  fRestartHistoButton->ChangeBackground(LightYellow);
  fStopHistoButton->ChangeBackground(LightYellow);


  fControlButtonFrame->
    AddFrame(fButtonFrame, fButtonFrameLayout);
  //--


  // Histogram Control Buttons --
  fHistoButtonGFrame
    = new TGGroupFrame(fControlButtonFrame, "Histogram",
		       kVerticalFrame );

  fHistoButtonGFrameLayout = 
    new TGLayoutHints(kLHintsCenterX, 10, 10, 2, 0);


  fHistoButtonFrame =
    new TGCompositeFrame(fHistoButtonGFrame, 60, 30,
			 kVerticalFrame | kFixedWidth );


  fHistoButtonFrameLayout = 
    new TGLayoutHints(kLHintsRight | kLHintsCenterY); 
    //new TGLayoutHints(kLHintsCenterX);

  fHistoButtonLayout =
    new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX,
		      //1, 1, 5, 5);
		      1, 1, 6, 0);

  /*
  fZeroHistoButton = 
    new TGTextButton(fHistoButtonFrame, "&Zero", kM_BUTTON_ZERO);
  fZeroHistoButton->Associate(this);
  fZeroHistoButton->SetToolTipText("Reset selected Histograms");
  */ //Commented out by Cavanaugh 22.04.2005

  fUnZeroHistoButton = 
    new TGTextButton(fHistoButtonFrame, "&Online", kM_BUTTON_UNZERO);
  fUnZeroHistoButton->Associate(this);
  fUnZeroHistoButton
    ->SetToolTipText("Back before reseting selected Histograms");

  //  Begin Cavanaugh 03.04.2005
  fRefHistoButton = 
    new TGTextButton(fHistoButtonFrame, "&Ref-0", kM_BUTTON_REF);
  fRefHistoButton->Associate(this);
  fRefHistoButton->SetToolTipText("Reference for selected Histograms");
  //  End Cavanaugh 03.04.2005

  fZeroHistoButton = 
    new TGTextButton(fHistoButtonFrame, "&Ref-1", kM_BUTTON_ZERO);  // Cavanaugh
  fZeroHistoButton->Associate(this);
  fZeroHistoButton->SetToolTipText("Dynamic Reference for selected Histos");

  fContentHistoButton = 
    new TGTextButton(fHistoButtonFrame, "&Content", kM_BUTTON_CONTENT);
  fContentHistoButton->Associate(this);
  fContentHistoButton
    ->SetToolTipText("Draw Bin-Content Histo of a Selected Item");


  fHistoButtonFrame->AddFrame(fUnZeroHistoButton, fHistoButtonLayout);
  fHistoButtonFrame->AddFrame(fRefHistoButton, fHistoButtonLayout);  // Cavanaugh 03.04.2005
  fHistoButtonFrame->AddFrame(fZeroHistoButton, fHistoButtonLayout);
  fHistoButtonFrame->AddFrame(fContentHistoButton, fHistoButtonLayout);


  fZeroHistoButton->ChangeBackground(LightYellow);
  fUnZeroHistoButton->ChangeBackground(LightYellow);
  fRefHistoButton->ChangeBackground(LightYellow);  // Cavanaugh 03.04.2005
  fContentHistoButton->ChangeBackground(LightYellow);


  fHistoButtonGFrame->
    AddFrame(fHistoButtonFrame, fHistoButtonFrameLayout);

  fControlButtonFrame->
    AddFrame(fHistoButtonGFrame, fHistoButtonGFrameLayout);

  //--


  fListButtonFrame->
    AddFrame(fControlButtonFrame, fControlButtonFrameLayout);


  fHistoListFrame->
    AddFrame(fListButtonFrame, fListButtonFrameLayout);


  fListTreeMainFrame->
    AddFrame(fHistoListFrame,fHistoListFrameLayout);

  //--

  fMainFrame->
    AddFrame(fListTreeMainFrame,fListTreeMainFrameLayout);

  //--

}



void HistoDisplay::CloseTGListTreeFrame()
{



  delete fHistoListTree;
  delete fTGCanvas;
  delete fTGCanvasLayout;

  delete fChButtonAutoUpdate;
  delete fChButtonSlideUpdate;
  delete fChButtonAutoUpdateLayout;
  
  
  delete fFrequencyLabel;
  delete fFrequencyTextBuffer;
  delete fFrequencyTextEntry;
  delete fFrequencyFrame;
  delete fFrequencyFrameLayout;
  
  delete fPauseHistoButton;
  delete fRestartHistoButton;
  delete fClearHistoButton;
  delete fPrintHistoButton;
  delete fStopHistoButton;
  
  delete fButtonLayout;
  
  delete fButtonFrame;
  delete fButtonFrameLayout;
  
  
  
  delete fZeroHistoButton;
  delete fUnZeroHistoButton;
  delete fContentHistoButton;
  
  delete fHistoButtonLayout;
  
  delete fHistoButtonFrame;
  delete fHistoButtonFrameLayout;
  delete fHistoButtonGFrame;
  delete fHistoButtonGFrameLayout;
    
  
  delete fControlButtonFrame;
  delete fControlButtonFrameLayout;
  
  
  delete fListButtonFrame;
  delete fListButtonFrameLayout;
    
  delete fHistoListFrame;
  delete fHistoListFrameLayout;
  
  delete fListTreeMainFrame;
  delete fListTreeMainFrameLayout;
}

void HistoDisplay::MakeStatusBar()
{
  Int_t parts[] = {20, 80};
  fStatusBar = new TGStatusBar(fMainFrame, 50, 10, kHorizontalFrame);
  fStatusBar->SetParts(parts, 2);
  fMainFrame->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom| kLHintsExpandX));
}

void HistoDisplay::CloseStatusBar()
{
  delete fStatusBar;
}





//TObject *HistoDisplay::OpenFile( const char *filename )
TObject *HistoDisplay::OpenFile( const char *filename,
				 EInputTypes inputtype,
				 Option_t *option )
{
  //std::cout << " In OpenFile " << std::endl;

  TObject *InputObj = 0;

  if ( inputtype == kTMapFile ) {
    TObject *obj =gROOT->GetListOfMappedFiles()->FindObject(filename) ;
    TMapFile *MFile = 0;
    if ( obj ) {
      MFile = (TMapFile*)obj;
    }
    else {
      //TMapFile::SetMapAddress(0x40c0000);
      MFile = TMapFile::Create(filename, option);
    }

    if ( MFile &&
	 MFile->IsFolder() &&
	 (MFile->IsA() == TMapFile::Class())  ) {
      //MFile->ls();
      //InputOpen = kTRUE;
      InputObj = MFile;
    }
    else {
      std::cout << " Error Opening " << filename << std::endl;
      if ( MFile ) MFile->Close();
      //InputOpen = kFALSE;
    }
  } //if ( inputtype == kTMapFile )

  else if ( inputtype == kTFile ) {
    TObject *obj = gROOT->GetListOfFiles()->FindObject(filename);
    TFile *RFile = 0;
    if ( obj ) {
      RFile = (TFile*)obj;
    }
    else {
      RFile = TFile::Open(filename, option);
    }
    if ( RFile &&
	 RFile->IsOpen()  &&
	 RFile->IsFolder() &&
	 !(RFile->IsZombie()) &&
	 (RFile->IsA() == TFile::Class()) &&
	 (RFile->IsA() != TMapFile::Class())  ) {
      //RFile->ls();
      //InputOpen = kTRUE;
      InputObj = RFile;
    }
    else {
      std::cout << " Error Opening " << filename << std::endl;
      if ( RFile ) RFile->Close();
      //InputOpen = kFALSE;
    }
  } //else if ( inputtype == kTFile ) 


  else if ( inputtype == kTSocket ) {

    TSocket *sock = 0;
    //TObject *obj =gROOT->GetListOfSockets()->FindObject(filename);
    //if ( obj ) sock = (TSocket*)obj;
    //else {

    sock = (TSocket*)gROOT->GetListOfSockets()->FindObject(filename);
    //while ( sock && !sock->IsValid() ) {
    while ( sock ) {
      sock->Close();
      sock = (TSocket*)gROOT->GetListOfSockets()->FindObject(filename);
    }//while ( sock && !sock->IsValid() ) 

    if ( !sock ) {
      TUrl url(filename);
      sock = new TSocket(url.GetHost(),url.GetPort());
      //std::cout << " Socket Server Name = " << url.GetHost() << std::endl;
      //std::cout << " port = " << url.GetPort() << std::endl;
    } //if ( !sock ) 

    sock->SetName( filename );

    if ( sock && sock->IsValid() ) {
      std::cout << " Socket Server = " << filename << std::endl;
      //InputOpen = kTRUE;
      InputObj = sock;
    }
    else {
      std::cout << " Error Opening " << filename << std::endl;
      if ( sock ) sock->Close();
      //InputOpen = kFALSE;
    }
  } else if ( inputtype == kTSOAP ) {
     string soapCommand = "sendList";
     string targetAddrStr = "18";
     // TUrl url(filename);
     SOAPMessage request;
     SOAPEnvelope envelope = request.getSOAPPart().getEnvelope();
     SOAPBody body = envelope.getBody();

     // set the target method name and add the attributes
     SOAPName name = envelope.createName(soapCommand);
     SOAPElement command = body.addBodyElement(name);

     // Add tid of target application
     SOAPName targetAddr = envelope.createName("targetAddr");
     command.addAttribute (targetAddr, targetAddrStr);

     // Add tid of this application
     SOAPName originator = envelope.createName("originator");
     command.addAttribute(originator, "45");

     // cout << "send command " << i << " of " << count << endl;

     SOAPConnection connection;	 
     string urlStr = "http://";
     urlStr += filename;
     //urlStr += ":";
     //urlStr += portnumber;

     URLEndpoint url(urlStr);
     SOAPMessage reply = connection.call(request,url);
    /*
      try {
        SOAPMessage reply = connection.call(request,url);
     	} catch (SOAPException soape)
     	{
        	cout << "SOAP Exception: " << soape.what() << endl;
        	// return 1;
     	} catch (...)
     	{
        	cout << "Caught unspecified exception" << endl;
        	// return 1;
    	}
     */
	reply.writeTo(cout);
        cout<<" -------------------------------------- "<<endl;
  }


  return InputObj;

}



TObject *HistoDisplay::CloseFile( const char *filename,
				  EInputTypes inputtype )
{
  //std::cout << " In CloseFile " << std::endl;

  TObject *obj = 0;

  if ( inputtype == kTMapFile ) {

    while( ( obj = gROOT->GetListOfMappedFiles()->FindObject(filename) ) )
      ( (TMapFile*)obj )->Close();

  } //if ( inputtype == kTMapFile )
  else if ( inputtype == kTFile ) {

    while( ( obj = gROOT->GetListOfFiles()->FindObject(filename) ) )
      ( (TFile*)obj )->Close();

  } //else if ( inputtype == kTFile )
  else if ( inputtype == kTSocket ) {

    while( ( obj =gROOT->GetListOfSockets()->FindObject(filename) ) )
      ( (TSocket*)obj )->Close();

  } //else if ( inputtype == kTSocket ) 
 
  return obj;

}

TObject *HistoDisplay::CloseFile( TObject *obj )
{
  //std::cout << " In CloseFile ( obj )" << std::endl;

  if ( obj ) { 

    if ( obj->InheritsFrom( TMapFile::Class() ) ) 
      ( (TMapFile*)obj )->Close();

    else if ( obj->InheritsFrom( TMapFile::Class() ) ) 
      ( (TFile*)obj )->Close();

    else if ( obj->InheritsFrom( TSocket::Class() ) ) 
      ( (TSocket*)obj )->Close();
 
  } //if ( obj ) 

  return obj;

}


void HistoDisplay::CloseConnection() 
{

  //if (MFileOpen) {
  if ( gROOT->GetListOfMappedFiles() ) {
    //delete MFile;
    //gROOT->GetListOfMappedFiles()->Delete();
    // To avoid Error in <TMapFile::Close>: 
    //  shadow map == 0, should never happen!
    TIter next( gROOT->GetListOfMappedFiles() );
    TMapFile* mfile = 0; 
    while ( ( mfile = (TMapFile*)next() ) ) {
      mfile->Close();
      //gROOT->GetListOfMappedFiles()->Remove(mfile);
      // is called by close
    }
    //MFile = 0;
    //MFileOpen = kFALSE;
  }

  //if (RFileOpen) {
  if ( gROOT->GetListOfFiles() ) {
    //delete RFile;
    //gROOT->GetListOfFiles()->Delete();
    TIter next( gROOT->GetListOfFiles() );
    TFile* file = 0; 
    while ( ( file = (TFile*)next() ) ) {
      file->Close();
      //gROOT->GetListOfFiles()->Remove(file);
      // is called by close
    }
    //RFile = 0;
    //RFileOpen = kFALSE;
  }

  //if (sockOpen) {

  if ( gROOT->GetListOfSockets() ) {
    //gROOT->GetListOfSockets()->Delete();
    gROOT->GetListOfSockets()->ls();
    TIter next( gROOT->GetListOfSockets() );
    TSocket* socktmp = 0;
    while ( ( socktmp = (TSocket*)next() ) ) {
      //socktmp->Send("END CONNECTION");
      socktmp->Send( DspEndConnection.c_str() );
      socktmp->Close();
      //gROOT->GetListOfFiles()->Remove(socktmp);
      // is called by close
    }
    //sockOpen = kFALSE;
  }

  //InputOpen = kFALSE;

}








TObject* HistoDisplay::
AutoOpen( const char *servername, EInputTypes inputtype )
{
  //std::cout << " In AutoOpen " << std::endl;

  TObject *inputobj = OpenFile( servername, inputtype, "READ" );

  if ( inputobj ) {

    if( !REF_FLAG) AddTreeItemsOfInput( inputobj );  // Cavanaugh 03.04.2005
    //AddTreeItemsOfInput( inputobj );
    //RefreshTreeItem( fHistoListTree->GetFirstItem() );
    //fClient->NeedRedraw( fHistoListTree );

    char* str = new char[ 9 + strlen(servername) ];
    strcpy(str, " Opened ");
    fStatusBar
      ->SetText( strcat(str, servername), 1);
    delete [] str;
  } //if ( InputObj ) 
  else {

    //if ( inputtype == kTMapFile && !InputOpen ) {
    if ( inputtype == kTMapFile ) {
      const char *sg = "Failed to open memory map file.";
      MessageBox(sg);
    }
    //else if ( inputtype == kTFile && !InputOpen ) {
    else if ( inputtype == kTFile ) {
      const char *sg = "Failed to open ROOT file.";
      MessageBox(sg);
    }
    //else if ( inputtype == kTSocket && !InputOpen ) {
    else if ( inputtype == kTSocket ) {
      const char *sg = "Failed to open the socket connection.";
      MessageBox(sg);
    }

    char* str = new char[16+strlen(servername)];
    str = strcpy(str," Error Opening ");
    fStatusBar
      ->SetText( strcat(str, servername ), 1);
    delete [] str;
  }

  return inputobj;

}

TObject* HistoDisplay::
AutoClose( const char *servername, EInputTypes inputtype )
{
  //std::cout << " In AutoClose " << std::endl;
  TObject *inputobj = 0;

  return inputobj;

}




void  HistoDisplay::
AutoOpenConsumer( const char *consumername )
{
  //std::cout << " In AutoOpenConsumer " << std::endl;

  UInt_t count = 0;
  Int_t idsave = -99;

  if ( consumerlist ) {

    consumerlist->readWebPage();

    UInt_t nent = consumerlist->nEntries();

    if ( nent > 0 ) {
      for ( UInt_t i = 0; i < nent; i++ ) {
        if ( !strcmp( consumerlist->consumername(i), consumername) &&
	     // !strcmp( consumerlist->statusString(i), "Running" ) ) {
	     ( !strcmp( consumerlist->statusString(i), "Running" ) ||
	       !strcmp( consumerlist->statusString(i), "Old" ) ) ) {
          count++;
          idsave = i;
        } //if ( !strcmp( consumerlist->consumername(i), consumername) &&
      } //for ( int i = 0; i < nent; i++ )
    } //if ( nent > 0 )


    if ( count == 1 ) {
      char *hostport = HostPortFromID(idsave);

      if ( hostport ) {
	std::cout << " Opening hostport = \"" << hostport  << "\" ..." << std::endl;
	TObject *inputobj = AutoOpen( hostport, kTSocket );
	if ( inputobj )
	  std::cout << " done. " << std::endl;
	else
	  std::cout << " failed. " << std::endl;
	delete [] hostport;
      } //if ( hostport )

    } //if ( count == 1 )
    else if ( count > 1) {

      std::cout << " More than one " << consumername
	   << ". Choose one from ConsumerList Window." << std::endl;
      OpenConsumerListBox();

    } //else if ( count > 1) 
    else if ( count == 0 )
      std::cout << " No consumer " << consumername << std::endl;

  } //if ( consumerlist )
  else
    std::cout << " No ConsumerList. " << std::endl;


}




void HistoDisplay::OpenConsumerListBox()
{
  //std::cout << " In OpenConsumerListBox " << std::endl;

  //if ( !fConsumerListBox ) {

	fConsumerListBox =
	  new ConsumerListBox( gClient->GetRoot(), this, 500, 220);
	if ( consumerlist && fConsumerListBox ) {
	  fConsumerListBox->SetConsumerList( consumerlist );
	  fConsumerListBox->SetHistoDisplay( this );
	} //if ( consumerlist && fConsumerListBox )

	//} // if ( !fConsumerListBox )

      if ( fConsumerListBox ) {
	fConsumerListBox->OpenWindow();
	fConsumerListBox->UpdateListBox();
      } //if ( fConsumerListBox)

}




char* HistoDisplay::HostPortFromID( const UInt_t id ) const
{
  //std::cout << " In HostPortFromID " << std::endl;

  if ( !consumerlist ) {
    std::cout << " No connection to ConsumerList." << std::endl;
    return 0;
  } //if ( !consumerlist )

  char *hostport =
    new char[ strlen( consumerlist->hostname(id) ) + 8 + 1 ];

  hostport[0] = '\0';

  strcat( hostport, consumerlist->hostname(id) );

  char *tmp = new char[8];
  sprintf( tmp, ":%d", consumerlist->port(id) );

  strcat( hostport, tmp);

  //std::cout << " selected host:port " << hostport << std::endl;

  delete [] tmp;

  return hostport;
}






void HistoDisplay::AutoReconnect()
{
  //std::cout << " In AutoReconnect " << std::endl;

  if ( fConsumerInfos && !fConsumerInfos->IsEmpty() ) {

    TIter nextInfo( fConsumerInfos );
    TObject *obj = 0;

    while( ( obj = nextInfo() ) ) {
      if ( obj->InheritsFrom( TConsumerInfo::Class() ) ) {
	TConsumerInfo *info = ( TConsumerInfo* ) obj ;

	//std::cout << " consumer = " <<  info->consumername() << std::endl;

	AutoOpenConsumer( info->consumername() );
      } //if ( obj->InheritsFrom( TConsumerInfo::Class() )

    } //while( ( info = nextInfo() ) )

  } //if ( fConsumerInfos && !fConsumerInfos->IsEmpty() )

}








TConsumerInfo* HistoDisplay::MakeConsumerInfo( TObject *inputobj ) 
{
  //std::cout << " In Make ConsumerInfo( inputobj ) " << std::endl;

  if ( !inputobj ) return 0;

  TConsumerInfo *info = 0;
  TObject *obj = 0;


  obj = GetNewObj( "", "ConsumerInfo", inputobj );

  if ( obj && obj->InheritsFrom( TConsumerInfo::Class() ) ) {
    info = (TConsumerInfo*)obj;
    //info->print();
  } //if ( obj && obj->InheritsFrom( TConsumerInfo::Class() ) )



  return info;
}



TConsumerInfo* HistoDisplay::
MakeConsumerInfo( const char *TheConsumerName, const char *filename ) 
{
  //std::cout << " In Make ConsumerInfo( file ) " << std::endl;
  //std::cout << TheConsumerName << " " << filename << std::endl;

  //if ( !TheConsumerName || !filename ) return 0;
  if ( ( strlen( TheConsumerName ) == 0 ) || !filename ) return 0;

  TConsumerInfo *info = 0;

  const int maxsize = 256;
  std::ifstream infile( filename );

  if ( infile ) {

    do {
      char str[ maxsize ];
      infile.getline( str, maxsize - 1 );

      // ConsumerInfo --
      if ( char *pos = const_cast<char*>(strstr( str, "ConsumerInfo ")) ) {

        char *ConsumerName = 0;
        ConsumerName =  getword( pos + strlen( "ConsumerInfo " ) ); 

        if ( !strcmp( ConsumerName, TheConsumerName ) ) {

	  //tmp
          //std::cout << " ConsumerName = " << ConsumerName << std::endl;

	  info = new TConsumerInfo( "ConsumerInfo" );

	  do {
	    char str1[ maxsize ];
	    infile.getline( str1, maxsize - 1 );
	    // run: --
	    if ( strstr( str1, "run:" ) ) {
	      char *runstr = 0;
	      runstr = getword( str1 + strlen( "run:" ) );

	      int runnumber = 0;
	      if ( runstr ) {
		runnumber = atoi( runstr );
		delete [] runstr;
	      } //if ( runstr )

	      //std::cout << " runnumber = " << runnumber << std::endl;
	      info->setRunNumber( runnumber );

	      if ( char *pos = const_cast<char*>(strstr( str1, "number of events:" )) ) {
		char *nevstr = 0;

		nevstr = getword( pos + strlen( "number of events:" ) );

		int eventnumber = 0;
		if ( nevstr ) {
		  eventnumber = atoi( nevstr );
		  delete [] nevstr;
		} //if ( nevstr )

		//std::cout << " eventnumber = " << eventnumber << std::endl;
		info->setNevents( eventnumber );


	      } //if ( char *pos = strstr( str1, "number of events:" ) ) 

	    }// if ( strstr( str1, "run:" ) )

	    //--
	    // TMapFile, TFile, TSocket -- 
	    char *inputname = 0;

	    if ( strstr( str1, "TMapFile:" ) ) {

	      inputname = getword( str1 + strlen( "TMapFile:" ) ); 
	      info->setInputName( TString( "TMapFile:" ) +
				  TString( inputname ) );

	    }// if ( strstr( str1, "TMapFile:" ) )
	    else if  ( strstr( str1, "TFile:" ) ) {

	      inputname = getword( str1 + strlen( "TFile:" ) ); 
	      info->setInputName( TString( "TFile:" ) +
				  TString( inputname ) );

	    }// else if ( strstr( str1, "TFile:" ) )
	    else if  ( strstr( str1, "TSocket:" ) ) {

	      inputname = getword( str1 + strlen( "TSocket:" ) ); 
	      info->setInputName( TString( "TSocket:" ) +
				  TString( inputname ) );

	    } //else if  ( strstr( str1, "TSocket:" ) ) 

	    if ( inputname ) {

	      //std::cout << " InputName = " << inputname << std::endl;
	      //info->setInputName( TString( inputname ) );


	      delete [] inputname;
	    } //if ( inputname )

	    //--
	    // objects: --
	    if ( strstr( str1, "objects:" ) ) {

	      do {
		char str2[ maxsize ];
		infile.getline( str2, maxsize - 1 );

		if ( str2 && ( strlen( str2 ) > 0 ) 
		     && !strstr( str2, "ConsumerInfo" ) ) {

		  char *path = 0;
		  char *name = 0;

		  char *pathname = getword( str2 );

		  //if ( pathname && ( *pathname != commentchar ) ) {
		  if ( pathname ) {

		    char *begin = str2 + strlen( pathname ) + 1;

		    char *pos = 0;
		    while ( ( pos = const_cast<char*>(strchr( pathname, '\"' ) )) && 
			    ( strchr( pos + 1, '\"' ) ) ) {
		      char *tmp = 0;
		      tmp = const_cast<char*>(strdelete( pathname, "\"" ));
		      delete [] pathname;
		      pathname = strdelete( tmp, "\"" );
		      delete [] tmp;
		    } // while ( strchr( pathname, '\"') )


		    if ( char *pos = const_cast<char*>( strrchr( pathname, '/')) ) {
		      pos++;

		      name = new char[ strlen( pos ) + 1 ];
		      strcpy( name, pos );

		      char tmpchr = *pos;
		      *pos = '\0';

		      path = new char[ strlen( pathname ) + 1 ];
		      strcpy( path, pathname );

		      *pos = tmpchr;
        
		    } //if ( path = strrchr( pathname, '/') )
		    else {
		      path = new char;
		      path = "";

		      name = new char[ strlen( pathname ) + 1 ];
		      strcpy( name, pathname );

		    } //else //if ( path = strrchr( pathname, '/') )


		    int status = 999;
		    char *statusstr = getword( begin );
		    if ( statusstr ) {
		      delete [] statusstr;
		      status = atoi( statusstr );
		    } //if ( statusstr )

		    /*
		    std::cout << " path = " << path 
			 << " name = " << name 
			 << " status = " << status << std::endl;
		    */
		    info->addObject( TString( name ), TString( path ), status );

		    if ( pathname ) {
		      delete [] pathname;
		      pathname = 0;
		    }
		    if ( path ) {
		      delete [] path;
		      path = 0;
		    }
		    if ( name ) {
		      delete [] name;
		      name = 0;
		    }


		  } //if ( pathname && ( *pathname != commentchar ) )
		} // if ( str2 && ( strlen( str2 ) > 0 ) )
		else break;

	      } while( !infile.eof() ); //str2


	      break;

	    } //if ( strstr( str1, "objects:" ) )

	    //--


	  } while ( !infile.eof() ); //str1


	  if ( ConsumerName ) delete [] ConsumerName;
	  break;

        }//if ( !strcmp( ConsumerName, TheConsumerName ) )

      } //if ( strstr( str, "ConsumerInfo") ) 


    } while ( !infile.eof() ); //str

    infile.close();

    if ( !info ) std::cout << " No info of " << TheConsumerName 
		      << " in " << filename << std::endl;

  } //if ( infile )
  else
    std::cout << " Failed to open file " << filename << std::endl;

  return info;

}




char *getword( char *str , const char commentchar )
{
  //std::cout << " In getword " << std::endl;

  char *word = 0;

  if ( str && ( strlen( str ) > 0 ) ) {
    char *begin = str;
    while( isspace( *begin ) && ( *begin != '\0' ) ) begin++;

    //if ( *begin != '\0' ) {
    if ( ( *begin != '\0' ) &&
         ( *begin != commentchar ) ) {

      char *end = begin;

      //while( !isspace( *end ) ) end++;

      while( !isspace( *end ) ) {
        char *tmp = 0;
        if ( ( *end == '\"' ) && 
             ( tmp = const_cast<char*>(strchr( end + 1 , '\"' )) )  ) end = tmp;
        else end++;
      }

      char tmpchr = *end;
      *end = '\0';
 
      word = new char[ strlen( begin ) + 1 ];
      strcpy( word , begin );

      *end = tmpchr;

    } //if ( *begin != '\0' )

  } // if ( str && ( strlen( str ) > 0 ) )

  return word;

}


char *strdelete( char *str, char *del )
{
  //std::cout << " In strdelete " << std::endl;

  char *newword = 0;

  char *pos = 0;
  if ( ( pos = const_cast<char*>(strstr( str, del )) ) ) {
    newword = new char[ strlen( str ) - strlen( del ) + 1 ];
    if ( newword ) {
      char tmp = *pos;
      *pos = '\0';

      strcpy( newword, str );

      *pos = tmp;

      strcat( newword, pos + strlen( del ) );

    }//if ( newword )
  } //if ( pos = strstr( str, del ) )

  return newword;

}



const char* strstrn( const char *s, const char *k ) 
{
  //std::cout << "comparing: " << s << " and " << k << std::endl;
  const char *pk = k;

  while ( *s != '\0' ) {
    //std::cout << *s << " , " << *pk << std::endl;
    if ( tolower( *s ) == tolower( *pk ) ) {
      pk++;
      if ( *pk == '\0' ) 
	{
	  //std::cout << "result:" << k << std::endl;
	  return k;
	}
    } //if ( tolower( *ps ) == tolower( *pk ) )
    else {
      pk = k;
    } //else //if ( tolower( *ps ) == tolower( *pk ) )
    s++;
  } //while ( *s != '\0' ) 
  return  NULL ;
}





void HistoDisplay::MakeHistoList( TObject *inputobj )
{
  //std::cout << " In MakeHistoList " << std::endl;

  if ( ! inputobj ) return ;

  //if ( fHistoList && !( fHistoList->IsEmpty( )) ) fHistoList->Clear();

  if ( inputobj->IsA() == TMapFile::Class() ) {
    MakeHistoListM( (TMapFile*) inputobj );
  }
  else if ( inputobj->IsA() == TFile::Class() ) {
    MakeHistoListR( (TFile*) inputobj );
  }
  else if ( inputobj->IsA() == TSocket::Class() ) {
    MakeHistoListS( (TSocket*) inputobj );
  }

}

void HistoDisplay::MakeHistoListM( TMapFile *MFile )
{
  if ( ! MFile ) return ;

  TMapRec *mr = MFile->GetFirst();
  while( MFile->OrgAddress(mr) ) {
    TString name = mr->GetName();

    /*
    TObject * obj = 0;
    obj = (TObject*)MFile->Get(name.Data(),obj);
    if ( obj && obj->InheritsFrom(TH1::Class()) ||
	 //obj->InheritsFrom(TPad::Class()) ) {
	 obj->InheritsFrom(TCanvas::Class()) ) {
      fHistoList->Add(obj);
    }
    */

    AddToTree( name.Data(), MFile );

    mr = mr->GetNext();
  }
}

void HistoDisplay::MakeHistoListR( TFile *RFile )
{
  if ( ! RFile ) return ;

  TIter next(RFile->GetListOfKeys());
  TKey *key;
  while ( ( key = (TKey*)next() )  ) {

    /*
    TObject *obj = 0;
    obj = (TObject*)RFile->Get(key->GetName());
    if ( obj &&  obj->InheritsFrom(TH1::Class()) ||
	 //obj->InheritsFrom(TPad::Class()) ) {
	 obj->InheritsFrom(TCanvas::Class()) ) {
      fHistoList->Add(obj);
    }
    */

    AddToTree( key->GetName(), RFile );

  }
}

void HistoDisplay::MakeHistoListS( TSocket *sock )// added FH changed HS
{
  if ( ! sock ) return ;

  /*

  if ( sock->IsValid() ) {
      //request TConsumerInfo
      TMessage *mess;


      TConsumerInfo *info;
      sock->Send("ConsumerInfo");
      sock->Recv(mess);
      if ( mess ) {
      if ( mess->What() == kMESS_OBJECT)
        {
	  info = (TConsumerInfo*)mess->ReadObject(mess->GetClass());
	  //info->print();
	  if (info) 
	    {
	      info->updateList(fHistoList,sock);
	      delete info;
	    }
	  else
	    {
	    std::cout << " No TConsumerInfo from the server: " 
	    << sock->GetName() << std::endl;
	    }

	}
      delete mess;
      }
  }

  */

  std::cout << " No TConsumerInfo from the server: " 
       << sock->GetName() << std::endl;

}



void HistoDisplay::AddToTree( const char *objname, TObject *inputobj )
{

      char *filename;
      //filename = inputobj->GetName();

      char *tmpname; 
      if ( ( tmpname = strrchr( inputobj->GetName() , '/' ) ) )
	filename = tmpname;
      else
	strcpy( filename, inputobj->GetName() );

      /*
      TString PathName =
	"/" + TString( filename ) + 
	"/" + TString( objname );
      */

      const char *dir = filename; 

      TString PathName =
	MakePathName( dir, objname );


      MakeTGListTree( PathName, inputobj );
      //MakeTGListTree( PathName, inputobj, obj->GetTitle() );


      //fClient->NeedRedraw(fHistoListTree);

}



void HistoDisplay::MakeTGListTree( const TString &PathName, 
				   //TObject *inputobj )
				   TObject *inputobj,
				   const char* tip )
{
  //std::cout << " In MakeTGListTree " << std::endl;


  while ( PathName.Contains("//") ) {
    TRegexp re("//+");
    PathName(re) = "/";
  }

  //std::cout << " PathName = " << PathName.Data() << std::endl;

  const int treemax = 10;

  int index[treemax];

  int PathLength = PathName.Length();

  int ii;
  int ind;
  for ( ii=0, ind = PathName.Index("/",1,0,TString::kExact) ;
	0 <= ind && ind <= PathLength ; ++ii ) {
    index[ii] = ind;
    ind = PathName.Index("/",1,ind+1,TString::kExact);
  }

  int nindex = ii;
  index[nindex] = PathLength;

  TString DirName;
  TString HistoName;

  TString CurPathName;
  TString CurParentName;
  TString CurItemName;


  TGListTreeItem *itemOrg= 0;
  itemOrg = fHistoListTree->FindItemByPathname( PathName.Data() );

  //if ( !fHistoListTree
  //->FindItemByPathname( PathName.Data() ) ) {

  if ( !itemOrg ) {

    if ( 1 < nindex ) {

      DirName = PathName( 0, index[nindex-1] );
      HistoName = PathName( index[nindex-1]+1, index[nindex] );


      if ( !fHistoListTree
	  ->FindItemByPathname( DirName.Data() ) ) {

	for ( int i=0 ; i < nindex-1 ; ++i ) {
	  CurPathName = DirName(0,index[i+1]);

	  if ( !fHistoListTree
	      ->FindItemByPathname( CurPathName.Data() ) ) {

	    if ( i==0 ) { 
	      CurItemName = DirName(index[i]+1,index[i+1]-index[i]-1);

	      fHistoListTree
		->AddItem( 0, CurItemName.Data() );

	    } //if ( i==0 )
	    else {
	      CurParentName = DirName( 0, index[i]);
	      CurItemName = DirName(index[i]+1,index[i+1]-index[i]-1);

	      fHistoListTree
		->AddItem( fHistoListTree
			   ->FindItemByPathname( CurParentName.Data() ),
			   CurItemName.Data() );


	    } // else //if ( i==0 )
	  } // if ( !fHistoListTree...
	} //for ( int i=0 ; i < nindex-1 ; ++i )
      }//if ( !fHistoListTree...

      TGListTreeItem *item = 0;
      item = fHistoListTree
	->AddItem( fHistoListTree->FindItemByPathname(DirName.Data()),
		   HistoName.Data() );
      if ( item ) {
	item->SetUserData(inputobj);
	if ( tip ) fHistoListTree->SetToolTipItem( item, tip );
      } // if ( item )

    } //if ( 1 < nindex ) 
    else {

      HistoName = PathName( index[nindex-1]+1, index[nindex] );
      TGListTreeItem *item = 0;
      item = fHistoListTree
	->AddItem( 0, HistoName.Data() );
      if ( item ) {
	item->SetUserData(inputobj);
	if ( tip ) fHistoListTree->SetToolTipItem( item, tip );
      } // if ( item )


    } //else // if ( 1 < nindex )

  } //  if ( !itemOrg ) 
  else {
    TObject *inputobjOrg = (TObject*) ( itemOrg->GetUserData() );
    if ( !inputobj->IsEqual( inputobjOrg ) ) {
      itemOrg->SetUserData(inputobj);
      if ( tip ) fHistoListTree->SetToolTipItem( itemOrg, tip );
    } //if ( !inputobj->IsEqual( inputobjOrg ) ) 

  } //else // if ( !itemOrg )
}



void HistoDisplay::SortTGListTree()
{

  if ( fHistoListTree->GetFirstItem() ) {

  TGListTreeItem *item;
  TGListTreeItem **list;
  int ichild = 0;
  int iparent = 0;

  const int itemmax = 200;
  list = new TGListTreeItem* [itemmax];
  list[ichild] = fHistoListTree->GetFirstItem();

  do
  {
    if ( fHistoListTree->SortChildren( list[iparent] ) ) {
      if ( (item = list[iparent]->GetFirstChild()) ) {
	list[++ichild] = item;
	while ( ( item =  list[ichild]->GetNextSibling() )
		&& ichild < itemmax ) list[++ichild] = item;
      }
    }
    iparent++;

  } while ( iparent <= ichild && ichild < itemmax );
  
  delete [] list;

  } //if ( fHistoListTree->GetFirstItem() )

  fClient->NeedRedraw(fHistoListTree);

}


void HistoDisplay::RemoveTGListTree()
{
  //std::cout << " In RemoveTGListTree " << std::endl;
  // Remove all TGListTree Items.
  TGListTreeItem *item;
  if ( fHistoListTree ) {
    while ( (item = fHistoListTree->GetFirstItem()) ) {
      //fHistoListTree->DeleteItem( item );
      DeleteTreeItem( item );

    } //while ( (item = fHistoListTree->GetFirstItem()) )
    fClient->NeedRedraw(fHistoListTree);
    fSelectedItem = 0;
  }//if ( fHistoListTree ) 
}


void HistoDisplay::
DelTreeItemsOfInput( TObject *inputobj )
{
  //std::cout << " In DelTreeItemsOfInput " << std::endl;
  // Delete all TGListTree Items of a UserData inputobj.


  TGListTreeItem *item = fHistoListTree->GetFirstItem();

  while( item ) {


    DeleteTreeItem( item );

    fHistoListTree->RecursiveDeleteItem( item, inputobj );
    item = item->GetNextSibling();
  }//while( item )

}

//void HistoDisplay::AddTreeItemsOfInput( TObject *inputobj ) 
void HistoDisplay::AddTreeItemsOfInput( TObject *inputobj
					, Bool_t newlyOpened ) 
{
  //std::cout << " In AddTreeItemsOfInput " << std::endl;

  // Add TGListTreeItem of a inputobj to the TGListTree 
  // based on the current TConsumerInfo in the inputobj. 

  if ( !inputobj ) {
    std::cout << " No input is specified. " << std::endl;
    return;
  } //if ( !inputobj )


  TPad *padsav = 0;
  if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) )
    padsav = (TPad*) gPad;

  TConsumerInfo *info = MakeConsumerInfo( inputobj );

  if ( info ) {


    //AddTreeItems( info, inputobj );
    AddTreeItems( info, inputobj, newlyOpened );
    SubtractTreeItems( info );
    DeleteEmptyFolders( fHistoListTree->GetFirstItem() );

    AddConsumerInfo( info );



  } //if( info )
  else {

    MakeHistoList(inputobj);

  } //else //if( info )


  Bool_t FoundWarningFolder = kFALSE;

  //SearchWarningErrorFolder
  //( fHistoListTree->GetFirstItem(), inputobj );
  SearchWarningFolder
    ( fHistoListTree->GetFirstItem(), inputobj, FoundWarningFolder );

  if ( !FoundWarningFolder ) {
    TObject *obj;
    if ( ( obj = 
	   gROOT->GetListOfCanvases()
	   ->FindObject( kWarningCanvasName ) ) ) {
      TCanvas *canvas = (TCanvas*)obj;
      canvas->Close();
      std::cout 
	<< std::endl
	<< "\"" << kWarningCanvasTitle << "\""
	<< " is closed because the warning disappeared."
	<< std::endl
	<< std::endl;
    } //if ( ( obj = 
  } //if ( !FoundWarningFolder )


  fClient->NeedRedraw( fHistoListTree );



  if ( padsav ) padsav->cd();

}





TString HistoDisplay::MakePathName( const char *dir, const char *objname )
{
  //std::cout << " In MakePathName " << std::endl;
  //
  // Returns TString pathname of "/dir/objname".
  // "//+" are replaced by "/".

  TString pathname
    = "/" + TString( dir ) + "/" + TString( objname );

  while ( pathname.Contains( "//" ) ) {
    TRegexp re( "//+" );
    pathname( re ) = "/";
  } //while ( pathname.Contains( "//" ) )

  return pathname;

}



void HistoDisplay::
DeleteTreeItem( TGListTreeItem *item )
{
  //std::cout << " In DeleteTreeItem " << std::endl;

  if ( !item ) return;

  //std::cout << " item = " << item->GetText() << std::endl;

  if ( item == fSelectedItem ) fSelectedItem = 0;

  if ( fSlideTimer && (item == fSlideTimer->GetCurrentItem() ) ) 
    fSlideTimer->SetCurrentItem( 0 );
  if ( fSlideTimer && (item == fSlideTimer->GetDisplayingItem() ) ) 
    fSlideTimer->SetDisplayingItem( item->GetNextSibling() );

  fHistoListTree->DeleteItem( item );

  item = 0;

}



void HistoDisplay::
//AddTreeItems( TConsumerInfo *info, TObject *inputobj )
AddTreeItems( TConsumerInfo *info, TObject *inputobj, Bool_t newlyOpened )
{
  //std::cout << " In AddTreeItems " << std::endl;
  //
  // Add new TGListTreeItems to TGListTree *fHistoListTree
  // from TConsumerInfo *info.
  // If an object in TConsumerInfo is not in fHistoListTree,
  // corresponding item is added to it.
  // If an object in TConsumerInfo exists in fHistoListTree,
  // inputobj is set to an UserData of the item.
  //

  if ( !info ) return;

  TConsumerInfo::Iterator iter( info );

  while ( iter.is_valid() ) {

    TString pathname = MakePathName( iter.Path(), iter.Name() );

    TGListTreeItem *item
      = fHistoListTree->FindItemByPathname( pathname.Data() );

    if ( item ) {

      if ( newlyOpened ) {

	//std::cout << " Newly opened " << std::endl;

	item->SetUserData( inputobj );

	TObject *obj = fDisplayedList->FindObject( iter.Name() );
	if ( obj && obj->InheritsFrom( CObject::Class() ) ) {
	  CObject *cobj = (CObject*)obj;
	  cobj->SetInputSource( inputobj );
	} //if ( obj && obj->InheritsFrom( CObject::Class() ) )

      } //if ( newlyOpened )

    } //if ( item ) 
    else {
 
      //std::cout << " new item " << std::endl;

      //const char *tip = iter.Tip();
      //MakeTGListTree( pathname, inputobj, tip );
      MakeTGListTree( pathname, inputobj );
    } //else //if ( item ) 

    ++iter;

  } //while ( iter.is_valid() ) 

}





void HistoDisplay::
SubtractTreeItems( TConsumerInfo *info )
{
  //std::cout << " In SubtractTreeItems " << std::endl;
  //
  // Subtract TGListTreeItems from TGListTree *fHistoListTree
  // according to new TConsumerInfo *info.
  //
  // Items which do not exist in a new TConsumerInfo *info,
  // but did exist in an old TConsumerInfo "infoold"
  // of the same consumer name, 
  // then the items are deleted from the fHistoListTree.
  // 

  if ( !info ) return;

  TConsumerInfo *infoold = 
    GetConsumerInfoFromList( info->consumername() );

  if ( !infoold ) return;
  TConsumerInfo::Iterator iterold( infoold );



  while ( iterold.is_valid() ) {

    TConsumerInfo::Iterator iter( info );

    Bool_t found = kFALSE;
    Bool_t foundPlural = kFALSE;


    while ( iter.is_valid() ) {


      if ( !strcmp( iter.Name(), iterold.Name() ) ) {

	if ( !strcmp( iter.Path(), iterold.Path() ) ) {

	  found = kTRUE;
	  if ( foundPlural ) break;

	} //if ( !strcmp( iter.Path(), iterold.Path() ) ) 
	else {

	  foundPlural = kTRUE;
	  if ( found ) break;

	} //else //if ( !strcmp( iter.Path(), iterold.Path() ) ) 

      } //if ( !strcmp( iter.Name(), iterold.Name() ) ) 


      ++iter;

    } //while ( iter.is_valid() ) 


    if ( !found ) {

      TString pathname = 
	MakePathName( iterold.Path(), iterold.Name() );

      TGListTreeItem *item
	= fHistoListTree->FindItemByPathname( pathname.Data() );

      if ( item ) {
	
	TObject *obj = fDisplayedList->FindObject( item->GetText() );


	DeleteTreeItem( item );

	if ( obj && !foundPlural ) {
	  fDisplayedList->Remove( obj );
	  delete obj;
	} //if ( obj )

      } //if ( item ) 


    } //if ( !found ) 

    ++iterold;

  } //while ( iterold.is_valid() ) 


}














void HistoDisplay::RefreshTreeItem( TGListTreeItem *item ) 
{
  //std::cout << " In RefreshTreeItem " << std::endl;
  //
  // Refresh TGListTree:
  // Currently, only
  // TGListTreeItem's tip is set as an title of a corrsponding object
  // in fDisplayedList.
  //
  // The process is recursively done
  // for all items in the TGListTree 
  // following the item of the argument ( childrens and yonger siblings ).
  //


  while ( item ) {

    //tmp
    //std::cout << " item = " << item->GetText() <<std::endl;

    TGListTreeItem *sibling = item->GetNextSibling();

    TGListTreeItem *parent = item->GetParent();

    TGListTreeItem *firstchild = item->GetFirstChild();

    if ( firstchild ) {

      RefreshTreeItem( firstchild );

    } //if ( firstchild ) 
    else {


      TPad *padsav = 0;
      if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) )
	padsav = (TPad*) gPad;

      TObject *obj = fDisplayedList->FindObject( item->GetText() );

      if ( !obj ) {
	//if ( item == fSelectedItem ) fSelectedItem = 0;
	//fHistoListTree->DeleteItem( item );
      }
      else
	fHistoListTree->SetToolTipItem( item, obj->GetTitle() );

      if ( padsav ) padsav->cd();


    } //else //if ( firstchild )

    item = sibling;


  }  //while ( item ) 

  //fClient->NeedRedraw( fHistoListTree );

}



void HistoDisplay::
DeleteEmptyFolders( TGListTreeItem *item )
{
  //std::cout << " In DeleteEmptyFolders " << std::endl;

  // Recursively
  // delete empty TGListTree folder item
  // from the fHistoListTree.
  // The item is deleted as an empty folder if it
  // 1. does not have children.
  // 2. does not have userdata.

  while ( item ) {

    TGListTreeItem *firstchild = item->GetFirstChild();
    TGListTreeItem *sibling = item->GetNextSibling();

    if ( firstchild ) {

      DeleteEmptyFolders( firstchild );

      if ( !( item->GetFirstChild() ) ) {

	DeleteTreeItem( item );

      } //if ( !( item->GetFirstChild() ) )

    } //if ( firstchild )
    else {

      if ( !( item->GetUserData() ) ) {

	DeleteTreeItem( item );

      } //if ( !( item->GetUserData() ) )

    } //else //if ( firstchild )

    item = sibling;

  } //while ( item ) 

}


void HistoDisplay::
DelTreeItemsOfInfo( TConsumerInfo *info )
{
  //std::cout << " In DelTreeItemsOfInfo " << std::endl;

  // Delete TGListTreeItems of a TConsumerInfo info
  // from TGListTree fHistoListTree.
  // fDisplayedList is also cleaned.
  // 

  if ( !info ) return;

  TConsumerInfo::Iterator iter( info );

  while( iter.is_valid() ) {


    TString pathname = MakePathName( iter.Path(), iter.Name() );

    TGListTreeItem *item
      = fHistoListTree->FindItemByPathname( pathname.Data() );

    if ( item ) {

      TObject *obj = fDisplayedList->FindObject( item->GetText() );

      if ( obj ) {
	fDisplayedList->Remove( obj );
	delete obj;
      } //if ( obj )

      DeleteTreeItem( item );

    } // if ( item ) 

    ++iter;

  } //while( iter.is_valid() )


  DeleteEmptyFolders( fHistoListTree->GetFirstItem() );

  fClient->NeedRedraw( fHistoListTree );

}



TConsumerInfo* HistoDisplay::
GetConsumerInfoFromList( const char * consumer )
{
  //std::cout << " In GetConsumerInfoFromList " << std::endl;

  // Get TConsumerInfo from fConsumerInfos.


  TConsumerInfo * info = 0;

  if ( fConsumerInfos && !fConsumerInfos->IsEmpty() ) {

    TIter next( fConsumerInfos );

    TObject *obj;

    while ( ( obj = next() ) ) {

      if ( obj->InheritsFrom( TConsumerInfo::Class() ) ) {
	TConsumerInfo *infotmp = ( TConsumerInfo* ) obj;

	if ( !strcmp( infotmp->consumername(), consumer ) ) {
	  info = infotmp;
	  break;
	} //if ( !strcmp( infotmp->consumername(), consumer ) )

      } //if ( obj->InheritsFrom( TConsumerInfo::Class() ) )

    } //while ( ( TObject *obj = next() ) ) 

  } //  if ( fConsumerInfo && !fConsumerInfo->IsEmpty() ) 

  return info;

}





TConsumerInfo* HistoDisplay::
AddConsumerInfo( TConsumerInfo *info )
{
  //std::cout << " In AddConsumerInfo " << std::endl;

  //Add TConsumerInfo *info to fConsumerInfos.
  //If TConsumerInfo of the same consumer name exists
  //in the fConsumerInfos, it is removed and deleted.

  if ( fConsumerInfos && info ) {

    TConsumerInfo *oldinfo = 
      GetConsumerInfoFromList( info->consumername() );

    if ( oldinfo && fConsumerInfos->Remove( oldinfo ) )
      delete oldinfo;

    fConsumerInfos->Add( info );

    return 0;

  } //if ( info ) 
  else   return 0;

}


void HistoDisplay::
DelConsumerInfo( TConsumerInfo *info )
{
  //std::cout << " In DelConsumerInfo " << std::endl;

  //Delete TConsumerInfo *info to fConsumerInfos.
  //If TConsumerInfo of the same consumer name exists
  //in the fConsumerInfos, it is removed and deleted.

  if ( fConsumerInfos && info ) {

    TConsumerInfo *oldinfo = 
      GetConsumerInfoFromList( info->consumername() );

    if ( oldinfo && fConsumerInfos->Remove( oldinfo ) )
      delete oldinfo;

  } //if ( info ) 

}








void HistoDisplay::
//SearchWarningErrorFolder( TGListTreeItem *item, TObject *socket )
SearchWarningFolder( TGListTreeItem *item, TObject *socket,
 Bool_t &FoundWarningFolder )
{
  //std::cout << " In SearchWarningFolder " << std::endl;

  while ( item ) {

    //std::cout << item->GetText() << std::endl;


    //if ( ( strstrn( item->GetText(), kWarningFolder )  ||
    //   strstrn( item->GetText(), kErrorFolder ) ) &&
    if ( ( !strcmp( item->GetText(), kWarningFolder )  ||
	   !strcmp( item->GetText(), kErrorFolder ) ) &&
	 //( socket->IsEqual( (TObject*) item->GetUserData() ) ) ) {
	 //!strcmp( socket->GetName(), 
	 //	  ( (TObject*)item->GetUserData())->GetName() ) &&
	 item->GetFirstChild()
	 ) {

	//tmp
      //std::cout << "item->GetText()" << item->GetText() << std::endl;


	fHistoListTree->OpenItem( item );

	PopUpSlides( item,
		     kWarningCanvasName,
		     kWarningCanvasTitle );

	if ( !FoundWarningFolder ) FoundWarningFolder = kTRUE;


      } //if ( strstr( item->GetText(), "warning" ) ||


    TGListTreeItem *sibling = item->GetNextSibling();
    TGListTreeItem *firstchild = item->GetFirstChild();

    if ( firstchild ) {

      //SearchWarningErrorFolder( firstchild, socket );
      SearchWarningFolder( firstchild, socket, FoundWarningFolder );

    } //if ( firstchild ) 

    item = sibling;

  }  //while ( item ) 

}



void HistoDisplay::
SearchWarningFolder( TConsumerInfo *info, TObject *socket )
{
  //std::cout << " In SearchWarningFolder ( info )" << std::endl;

  


}





void HistoDisplay::PopUpSlides( TGListTreeItem *item,
const char *canvasname, const char *canvastitle ) 
{
  //std::cout << " In PopUpWarningSlides " << std::endl;

  TVirtualPad *padsav = 0;
  if ( gPad ) padsav = gPad;

  TCanvas *canvas = 0;

  canvas =
    (TCanvas*) 
    ( gROOT->GetListOfCanvases()->FindObject( canvasname ) );
	    
  if ( !canvas ) {
    canvas = 
      new TCanvas( canvasname, canvastitle, 400, 400 );
  } //if ( !canvas )

  if ( canvas ) {

    TCanvasImp *canvasimp = canvas->GetCanvasImp();
    if ( canvasimp ) canvasimp->Show();

    canvas->Clear();
    canvas->cd();


    StartSlides( item );

  } // if ( canvas ) 

  if ( padsav ) padsav->cd();


}









CObject* HistoDisplay::GetCObjFromListItem( TGListTreeItem *item ) 
{
  //std::cout << " In GetCObjFromListItem " << std::endl;

  if ( !item ) return 0;

  CObject *cobj = 0;


  /*

  TString objName = TString( item->GetText() );

  TObject *obj = fDisplayedList->FindObject( objName.Data() );

  if ( !obj ) {
    if ( !item->GetFirstChild() ) {
      TObject *inputobj = (TObject*)item->GetUserData();

      if ( inputobj ) {


	cobj =  UpdateHistoList( objName.Data(), inputobj );

	if ( !cobj ) 
	  std::cout <<  objName.Data() << " is not found." << std::endl;
	else
	  if ( item )
	    fHistoListTree->SetToolTipItem( item, cobj->GetTitle() );

      } //if ( inputobj )

    } //if ( !item->GetFirstChild() )

  } //if ( !obj ) 
  else {

    if ( obj->InheritsFrom( CObject::Class() ) ) {
      cobj = ( CObject* )obj;
      const TObject *inputobj = (TObject*)item->GetUserData();

      if ( cobj->GetInputSource() != inputobj ) {

	if ( inputobj ) {

	  cobj =  UpdateHistoList( objName.Data(), inputobj );

	  if ( !cobj ) 
	    std::cout <<  objName.Data() << " is not found." << std::endl;
	  else
	    if ( item )
	      fHistoListTree->SetToolTipItem( item, cobj->GetTitle() );

	} //if ( inputobj ) 

      } //if ( cobj->GetInputSource() != inputobj ) 
      else {


      }//else //if ( cobj->GetInputSource() != inputobj ) 


    } //if ( obj->InheritsFrom( CObject::Class() ) )

  } //else //if ( !obj )


  */



  TObject *obj;

  obj = (TObject*)item->GetUserData(); 


  if( obj ) {

    if ( obj->InheritsFrom( CObject::Class() ) ) {

      std::cout << "UserData is CObject" << std::endl;

      cobj = (CObject*)obj;

    }
    else {
      
      std::cout << "UserData is not CObject" << std::endl;

      if ( !item->GetFirstChild() ) {

	TObject *inputobj = obj;

	if ( inputobj ) {
          TString objname = item->GetText();
          TString objpath("");
          TGListTreeItem* iter_item = item;
          while (TGListTreeItem* parent = iter_item->GetParent()) {
//                std::cout << parent->GetText() << std::endl;
		objpath = parent->GetText() + TString("/") + objpath;
		iter_item = parent;
          }	  	
          // cobj =  UpdateHistoList( item->GetText(), inputobj );
	  cobj =  UpdateHistoList( objpath.Data(), objname.Data(), inputobj );

	  if ( !cobj ) 
	    std::cout <<  objname << " is not found." << std::endl;
	  else {
	    item->SetUserData( cobj );
	    if ( item )
	    fHistoListTree->SetToolTipItem( item, cobj->GetTitle() );
	  } //else //if ( !cobj )

	} //if ( inputobj )

      } //if ( !item->GetFirstChild() )

    } //else //if ( obj->InheritsFrom( CObject::Class() ) 


  } //if( obj )


  return cobj;

}














void HistoDisplay::DrawHisto( TObject *obj )
{
  //std::cout << " In DrawHisto obj " << std::endl;

  /*
  // For older version of ROOT, this is needed.
  static int icount = 0;
  if ( !(icount++) || gROOT->GetListOfCanvases()->IsEmpty() ) gPad = 0;
  */

  if ( gROOT->GetListOfCanvases()->IsEmpty() ) gPad = 0;

  if ( !gPad ) {
    //std::cout << " Current pad is not specified. " << std::endl;
    return;
  }

  if ( obj ) {

    //obj->ls();

    TCanvas *CurrentCanvas = 0;

    /*
    if ( !gPad ) {
      CanvasNumber++;
      char *num = new char[3];
      sprintf( num, "%d", CanvasNumber );
      CurrentCanvas =
	new TCanvas( CanvasName + TString( num ),
		     CanvasTitle + TString( num ) , 500, 500 );
      delete [] num;
      CurrentCanvas->cd();
    }
    */


    //if ( !gPad ) OpenDefaultCanvas();


    TPad *gPadSav = (TPad*)gPad;
    CurrentCanvas = gPadSav->GetCanvas();




    if ( obj->InheritsFrom( CHistogram::Class() ) ) {

      CHistogram *chisto = (CHistogram*) obj;
      TH1 *histo = chisto->SubtractedHisto();
      //TH1 *histo =  chisto->GetHistoDrawn();

      if ( histo ) {

	TObject *hobj = chisto->GetTObject();

	Option_t *option = 0;
	if ( hobj ) option = hobj->GetOption();

	//gPadSav->Clear();
	RecursivePadClear( gPadSav );
	gPadSav->cd();



	TString padname;
	TString padtitle;

	if ( gPadSav == CurrentCanvas ) {

	  //tmp
	  /*
	  CurrentCanvas->SetWindowSize( 500, 500 );
	  CurrentCanvas->Resize();
	  */

	  if ( strcmp( CurrentCanvas->GetName(),
		       kWarningCanvasName ) ) {

	  padname = "Canvas_of_" +  TString( histo->GetName() ) ;
	  if ( fSlideCanvasList->FindObject( CurrentCanvas ) )
	    //padtitle = "<Slide Canvas>" + TString( histo->GetTitle() );
	    padtitle = "<Auto Update>" + TString( histo->GetTitle() );
	  else
	    padtitle = TString( histo->GetTitle() );
	  CurrentCanvas->SetName( padname.Data() );
	  CurrentCanvas->SetTitle( padtitle.Data() );

	  } // if ( strcmp( CurrentCanvas->GetName(),


	  //gPadSav->cd();
	  //DrawTH1( histo, option, gPadSav );
	  DrawTH1( histo, option, CurrentCanvas );
	} //if ( gPadSav == gPadSav->GetCanvas() )
	else { 

	  padname = "Pad_of_" + TString( histo->GetName() );
	  padtitle = TString( histo->GetTitle() );

	  gPadSav->SetName( padname.Data() );
	  gPadSav->SetTitle( padtitle.Data() );
	  //gPadSav->cd();
	  DrawTH1( histo, option, gPadSav );

	  if ( strcmp( CurrentCanvas->GetName(),
		       kWarningCanvasName ) ) 
	    ChangeCanvasNameTitle( CurrentCanvas );

	} // else //if ( gPadSav == gPadSav->GetCanvas() )


      } //if ( histo )

    } //if ( obj->InheritsFrom( CHistogram::Class() ) ) 

    else if ( obj->InheritsFrom( CObject::Class() ) ) {
      CObject *cobj = (CObject*)obj;
      TObject *tobj = cobj->GetTObject();

      if ( tobj->InheritsFrom( TPad::Class() ) ) {

	TPad *c2 = (TPad*)tobj;

	//tmp
	//std::cout << " gPadSav->GetName() = " << gPadSav->GetName() << std::endl;

	gPadSav->cd(); // necessary to prevent bad x window allocation.

	if ( c2 ) {


	  if ( gPadSav == CurrentCanvas ) {
	    //gPadSav->Clear();
	    //RecursivePadClear( gPadSav );
	    gPadSav->cd();

	  //tmp
	  /*
	    if ( c2->InheritsFrom( TCanvas::Class() ) )
	      DuplicateCanvasSize( CurrentCanvas, (TCanvas*)c2 );
	  */

	  if ( strcmp( CurrentCanvas->GetName(),
		       kWarningCanvasName ) ) {

	    CurrentCanvas->SetName( c2->GetName() );
	    TString padtitle;
	    if ( fSlideCanvasList->FindObject( CurrentCanvas ) )
	      //padtitle = "<Slide Canvas>" + TString( c2->GetTitle() );
	      padtitle = "<Auto Update>" + TString( c2->GetTitle() );
	    else
	      padtitle = TString( c2->GetTitle() );
	    CurrentCanvas->SetTitle( padtitle );

	  } // if ( strcmp( CurrentCanvas->GetName(),


	    DrawTPad( c2, gPadSav );

	  } //if ( gPadSav == gPadSav->GetCanvas() )
	  else {
	    gPadSav->SetName( c2->GetName() );
	    gPadSav->SetTitle( c2->GetTitle() );
	    DrawTPad( c2, gPadSav );

	    if ( strcmp( CurrentCanvas->GetName(),
			 kWarningCanvasName ) ) 
	      ChangeCanvasNameTitle( CurrentCanvas );

	  } //else //if ( gPadSav == gPadSav->GetCanvas() ) 

	} //if (c2)

      } //if ( tobj->InheritsFrom( TPad::Class() ) )

    } //else if ( obj->InheritsFrom( CObject::Class() ) ) 


    //ChangeCanvasNameTitle( CurrentCanvas );

    gPadSav->cd();
    /*
    gPadSav->Modified();
    gPadSav->Update();
    */
    CurrentCanvas->Modified();
    CurrentCanvas->Update();


    //std::cout << "Drawing done" << std::endl << std::endl;
      
  } //if (obj)


}



void HistoDisplay::DrawHisto( TList *fSelectedList )
{
  //std::cout << " In DrawHisto list " << std::endl;

  // For older version of ROOT, this is needed.
  //static int icount = 0;
  if ( gROOT->GetListOfCanvases()->IsEmpty() ) gPad = 0;


  if ( !gPad ) {
    //std::cout << " Current pad is not specified. " << std::endl;
    return;
  }



    if ( fSelectedList && !fSelectedList->IsEmpty() ) {
      int nPad = fSelectedList->GetSize();
      int nPadX = (int) sqrt( (float)nPad );
      int nPadY = nPadX;

      if ( nPadX*nPadY < nPad ) {
	nPadX++;
	if ( nPadX * nPadY < nPad ) nPadY++;
      } //if ( nPadX*nPadY < nPad ) 


      // pad size ww,wh--
      //int ww, wh;
      //ww = wh = 300;

      TCanvas *CurrentCanvas = 0;

      /*
      if ( !gPad ) {
	CanvasNumber++;
	char *num = new char[3];
	sprintf( num, "%d", CanvasNumber);
	CurrentCanvas =
	  new TCanvas( CanvasName + TString( num ),
		       CanvasTitle + TString( num) , ww * nPadX, wh * nPadY );
	delete [] num;
	CurrentCanvas->cd();
      }//if (!gPad)
      */

      if ( !gPad ) {
	//OpenDefaultCanvas();
      }
      else {

	CurrentCanvas = (TCanvas*)gPad->GetCanvas();

	//CurrentCanvas->Clear();
	RecursivePadClear( CurrentCanvas );
	CurrentCanvas->cd();

	//tmp
	/*
	CurrentCanvas->SetWindowSize( ww * nPadX, wh * nPadY );
	CurrentCanvas->Resize();
	*/

      }//if (!gPad) else


      CurrentCanvas->Divide( nPadX, nPadY );


      int iPad = 0;

      TIter nextList(fSelectedList);

      TObject *obj = 0;
      while ( ( obj = nextList() ) ) {
	iPad++;

	if ( obj->InheritsFrom( CHistogram::Class() ) ) {

	  CurrentCanvas->cd(iPad);

	  CHistogram *chisto = (CHistogram*)obj;

	  TH1 *histo = chisto->SubtractedHisto();
	  //TH1 *histo = chisto->GetHistoDrawn();

	  if ( histo ) {
 
	    Option_t *option;
	    option = chisto->GetTObject()->GetOption();

	    TPad *gpad = (TPad*)gPad;

	    TString padname = "Pad_of_" +  TString( histo->GetName() ) ;
	    TString padtitle = TString( histo->GetTitle() ) ;

	    gpad->SetName( padname.Data() );
	    gpad->SetTitle( padtitle.Data() );

	    DrawTH1(histo, option, gpad);

	    gpad->Modified();
	    gpad->Update();

	  } //if( histo )

	} //if ( obj->InheritsFrom( CHistogram::Class() ) )
	    
	else if ( obj->InheritsFrom( CObject::Class() ) ) {

	  CObject *cobj = (CObject*)obj;
	  TObject *tobj = cobj->GetTObject();

	  if ( tobj && tobj->InheritsFrom( TPad::Class() ) ) {

	    CurrentCanvas->cd(iPad);

	    TPad *gpad = (TPad*)gPad;
	    TPad *c2 = (TPad*)tobj;

	    if (c2) {
	      gpad->SetName( c2->GetName() );
	      gpad->SetTitle( c2->GetTitle() );

	      DrawTPad(c2, gpad);
	      //gpad->Modified();
	      gpad->Update();

	    } //if (c2)

	  } //if ( tobj && tobj->InheritsFrom( TPad::Class() ) )

	} //else if ( obj->InheritsFrom( CObject::Class() ) )

      } // while ( obj = nextList() )


      ChangeCanvasNameTitle( CurrentCanvas );


      CurrentCanvas->cd();
      CurrentCanvas->Modified();
      CurrentCanvas->Update();

      //std::cout << "Drawing done" << std::endl << std::endl;

    } // if ( fSelectedList && !fSelectedList->IsEmpty() )
      

}





void HistoDisplay::ChangeCanvasNameTitle( TCanvas *canvas )
{
  //std::cout << " In ChangeCanvasNameTitle " << std::endl;

  if ( !canvas ) return;

  TList *list = canvas->GetListOfPrimitives();

  Int_t count = 0;
  TObject *obj = 0;


  TString canvasname = "";
  TString canvastitle = "";

  /*
  const char *name = 0;
  name = canvas->GetName();
  if ( name && fDisplayedList->FindObject( name ) ) {
    count++;
    canvasname = name;
    canvastitle = canvas->GetTitle();
  } //if ( name && fDisplayedList->FindObject( obj ) ) 
  */


  TIter next( list );

  while ( ( obj = next() ) ) {

    const char *name = 0;
    name = obj->GetName();

    if ( strstr( name, "Pad_of_" ) )
      name = &( name[ strlen( "Pad_of_" ) ] );

    //std::cout << " name " <<  name << std::endl;

    if ( name && fDisplayedList->FindObject( name ) ) {
      count++;
      if ( count == 1 ) {
	  canvasname = TString( name );
	  canvastitle = TString( obj->GetTitle() );
      } // if ( count == 1 )
      else if ( count == 2 ) {
	canvasname = canvasname + "__" + TString( name );
	canvastitle = canvastitle + ", " + TString( obj->GetTitle() );
      } //else if ( count == 2 )
      else if ( count == 3 ) {
	canvasname = canvasname + "__etc";
	canvastitle = canvastitle + ", etc.";
      } //else if ( count == 3 )
    } //if ( name && fDisplayedList->FindObject( obj ) ) 

  } //while ( obj = next() )

  if ( count ) {
    if ( fSlideCanvasList->FindObject( canvas->GetName() ) )
      //canvastitle =  "<Slide Canvas>" + canvastitle; 
      canvastitle =  "<Auto Update>" + canvastitle; 

    canvas->SetName( canvasname.Data() );
    canvas->SetTitle( canvastitle.Data() );
  } //if ( count )

}



void HistoDisplay::DuplicateCanvasSize( TCanvas *guicanvas,
				        TCanvas *consumercanvas )
{
  //std::cout << " In DuplicateCanvasSize " << std::endl;

  if ( !guicanvas || !consumercanvas ) return;
  Int_t ww, wh;

  ww = consumercanvas->GetWindowWidth();
  wh = consumercanvas->GetWindowHeight();

  if ( !ww || !wh ) {
    ww = consumercanvas->GetWw() + 4;
    wh = consumercanvas->GetWh() + 28;
  } //if ( !ww || !wh )

  if ( ww && wh ) {
    guicanvas->SetWindowSize( ww, wh );
    guicanvas->Resize();
  } //if ( ww && wh )

}



void HistoDisplay::OpenDefaultCanvas()
{
  //std::cout << " In OpenDefaultCanvas()" << std::endl;

  const TString canvasName = "CMSDisplayCanvas";
  const TString canvasTitle = "CMS DQM Consumer Display Canvas ";
  static int canvasNumber = 0;


  canvasNumber++;

  char *num = new char[3];
  sprintf( num, "%d", canvasNumber );
  TCanvas *canvas = new TCanvas( canvasName + TString(num), 
				 canvasTitle + TString( num ), 500, 500 );
  delete [] num;
  canvas->cd();

}









void HistoDisplay::DrawTH1(TH1 *histo, Option_t *option
			   , TVirtualPad *BackPad )
{
  //std::cout << " In DrawTH1" << std::endl;

  if ( !histo || !BackPad ) return;

  BackPad->cd();

  //BackPad->Clear(); // Done by TH1::Draw()
  //RecursivePadClear( BackPad );


  BackPad->SetTickx( gStyle->GetPadTickX() );
  BackPad->SetTicky( gStyle->GetPadTickY() );
  BackPad->SetGridx( gStyle->GetPadGridX() );
  BackPad->SetGridy( gStyle->GetPadGridY() );
  BackPad->SetLogx( gStyle->GetOptLogx() );
  BackPad->SetLogy( gStyle->GetOptLogy() );
  BackPad->SetLogz( gStyle->GetOptLogz() );
  BackPad->SetBorderSize( gStyle->GetPadBorderSize() );
  BackPad->SetBorderMode( gStyle->GetPadBorderMode() );
  BackPad->SetGridy( gStyle->GetPadGridY() );

  BackPad->ResetAttLine();
  //BackPad->ResetAttFill(); // Black?
  //BackPad->SetFillColor(10); // White
  BackPad->SetFillColor( gStyle->GetPadColor() );
  BackPad->SetFillStyle( gStyle->GetFillStyle() );
  BackPad->ResetAttPad();

  //histo->Draw(option);
  histo->Draw();
  histo->SetDrawOption( option );

  //std::cout << " End of DrawTH1 " << std::endl;

}





void HistoDisplay::DrawTPad( TPad *DrawnPad, TVirtualPad *BackPad )
{
  //std::cout << " In DrawTPad  " << std::endl;

  if ( !DrawnPad || !BackPad ) return;

  //copy pad attributes
  BackPad->Range( DrawnPad->GetX1(), DrawnPad->GetY1(),
		  DrawnPad->GetX2(), DrawnPad->GetY2() );
  BackPad->SetTickx( DrawnPad->GetTickx() );
  BackPad->SetTicky( DrawnPad->GetTicky() );
  BackPad->SetGridx( DrawnPad->GetGridx() );
  BackPad->SetGridy( DrawnPad->GetGridy() );
  BackPad->SetLogx( DrawnPad->GetLogx() );
  BackPad->SetLogy( DrawnPad->GetLogy() );
  BackPad->SetLogz( DrawnPad->GetLogz() );
  BackPad->SetBorderSize( DrawnPad->GetBorderSize() );
  BackPad->SetBorderMode( DrawnPad->GetBorderMode() );
  DrawnPad->TAttLine::Copy( (TAttLine&)*BackPad );
  DrawnPad->TAttFill::Copy( (TAttFill&)*BackPad );
  DrawnPad->TAttPad::Copy( (TAttPad&)*BackPad );


  TList *list = DrawnPad->GetListOfPrimitives();
  TIter next( list );
  TObject *objInPad;

  //BackPad->Clear();
  RecursivePadClear( BackPad );


  BackPad->cd();


  while( ( objInPad = next() ) ) {
    //BackPad->cd(); // this is absolutely necessary in compiled mode.
    // and just before Draw(). Learned hard way.
    // Seemed not true for newer ROOT version.

    gROOT->SetSelectedPad( BackPad ); 
    // not BackPad->cd() but this for primitiveList->Add

    //objInPad->DrawClone(); // Best choise to draw all in DrawnPad.
    // But does not draw them on additional pad made here.
    // Not allowed to change pad size. 

    //objInPad->Clone()->Draw();

    BackPad->GetListOfPrimitives()
      //->Add( objInPad->Clone(), objInPad->GetDrawOption() );
      ->Add( objInPad->Clone(), next.GetOption() );

  } // while( objInPad = next() )

  BackPad->Modified();

}


void HistoDisplay::RecursivePadClear( TVirtualPad *pad, Option_t *option )
{
  //std::cout << " In RecursivePadClear " << std::endl;
  if ( !pad ) return;
  //std::cout << pad->GetName() << std::endl;


  TList *primitivelist = pad->GetListOfPrimitives();
  TFrame *frame = pad->GetFrame();

  TIter next( primitivelist );

  TObject *obj;
  while ( ( obj = next() ) ) {

      if ( !( obj->TestBit( kCanDelete ) ) &&
	   !( frame && frame->IsEqual( obj ) ) ) obj->SetBit( kCanDelete );


      //tmp v.3071 for ROOT v.3.03/07 --
      if ( obj->InheritsFrom( TPaveStats::Class() ) ) {
	TPaveStats *stats = (TPaveStats*) obj;

	if ( stats && !stats->GetParent() )
	  stats->SetParent( pad->GetListOfPrimitives() );

      }//if ( obj->InheritsFrom( TPaveStats::Class() ) ) 
      //--


      if ( obj->InheritsFrom( TPad::Class() )  ) {
	TPad *padInside = (TPad*) obj;
	RecursivePadClear( padInside, option );
      } // if ( obj->InheritsFrom( TPad::Class() )  )

  } //while ( obj = next() )

  pad->Clear( option );

}




void HistoDisplay::UpdateTimerOn()
{
  //std::cout << " In UpdateTimerOn " << std::endl;

  TVirtualPad *currentPad;

  if ( !gPad || ( gROOT->GetListOfCanvases()->IsEmpty() ) )
    return;
  else
    currentPad = gPad;


  Int_t sleeptime  = 0;

  if ( AutoUpdate ) {

    sleeptime =  atoi( fFrequencyTextBuffer->GetString() );

    if ( fUpdateTimer ) {



      if ( fUpdateTimer && !fUpdateTimer->HasTimedOut() )
	fUpdateTimer->TurnOff();

      if ( fSlideTimer && 
	   ( fSlideTimer->GetCurrentPad() == gPad ) &&
	   !fSlideTimer->HasTimedOut() )
	fSlideTimer->TurnOff();



      fUpdateTimer->SetTime( std::max( sleeptime, 800 ) );
      //std::cout << " Timer Time = " << fUpdateTimer->GetTime() << std::endl;

      fUpdateTimer->SetCurrentPad( currentPad );

      Running = kTRUE;
      fUpdateTimer->TurnOn();

      //return;
    } //if ( fUpdateTimer )

  } //if ( AutoUpdate )


  //std::cout << "Update done" << std::endl << std::endl;

}



Bool_t HistoDisplayUpdateTimer::Notify()
{
  //std::cout << " In HistoDisplayUpdateTimer::Notify " << std::endl; 
  if ( this &&
       ( !fHistoDisplay->AutoUpdate ||
         !fHistoDisplay->Running ) ) {

    fHistoDisplay->Running = kFALSE;
    TurnOff();   // same as Remove();

    fHistoDisplay->fPauseHistoButton->SetState( kButtonDisabled );
    fHistoDisplay->fRestartHistoButton->SetState( kButtonUp );

    fHistoDisplay->fHistoListTree
      ->HighlightItem( fHistoDisplay->fSelectedItem );
    fHistoDisplay->fClient->NeedRedraw( fHistoDisplay->fHistoListTree );

    //std::cout << "Update done" << std::endl << std::endl;
    std::cout << "Update stopped" << std::endl << std::endl;
    return kTRUE;
  } //if ( this &&



  /*
  //-- usr signal handling

  if ( gReconnectFlag ) {

    std::cout<< "Reconnect to all consumers signal" << std::endl;

    fHistoDisplay->AutoReconnect();
    gReconnectFlag = false;

  }

  //--
  */



  //fHistoDisplay->UpdateCanvas();
  fHistoDisplay->UpdateCanvas( fCurrentPad );
  Reset();
  return kFALSE;

}




//void HistoDisplay::UpdateCanvas() {
void HistoDisplay::UpdateCanvas( TVirtualPad *currentpad ) {

  //std::cout << " In UpdateCanvas " << std::endl;

  if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) ) {

    TCanvas *canvas = 0;
    static TObjLink *link = 0;

    TPad *gPadSav = (TPad*) gPad;

    if ( AutoUpdate &&
	SlideShowUpdate &&
	fSlideCanvasList &&
	!( fSlideCanvasList->IsEmpty() ) ) {

      if ( !link ) link = fSlideCanvasList->FirstLink();

      if ( link ) {

	TObject *ocanv = 0;
	if ( ( ocanv = link->GetObject() ) &&
	     ( gROOT->GetListOfCanvases()->FindObject( ocanv ) ) &&
	     ( ocanv->InheritsFrom( TCanvas::Class() ) ) ) {
	  canvas = (TCanvas*) ocanv;

	  //canvas->cd();
	  UpdatePad( canvas );

	  link = link->Next();
	} //if ( ( ocanv = link->GetObject() ) &&
	else {
	  TObjLink *linktmp = link;
	  link = link->Next();
	  fSlideCanvasList->Remove( linktmp );
	  return;
	} // else //if ( ( ocanv = link->GetObject() ) &&

      } //if ( link ) 


    } //if ( AutoUpdate &&
    else {

      link = 0;
      //canvas = gPad->GetCanvas();

      if ( !currentpad ||
	   !gROOT->GetListOfCanvases()->FindObject( currentpad ) )
	currentpad = gPadSav;

      UpdatePad( currentpad );

      canvas = currentpad->GetCanvas();

    } //else //if ( AutoUpdate && 

    /*
    if ( canvas ) {
      TCanvasImp *canvasimp = canvas->GetCanvasImp();
      if ( canvasimp ) canvasimp->Show();
    } // if ( canvas ) 
    */

    //TPad *gpad = (TPad*) gPad;
    //UpdatePad( gpad );

    gPadSav->cd();

  } //if( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) ) 



}




//void HistoDisplay::StartSlides( const TGListTreeItem *selectedItem )
void HistoDisplay::StartSlides( TGListTreeItem *selectedItem )
{
  //std::cout << " In StartSlides " << std::endl;

  if ( selectedItem &&
       selectedItem->GetFirstChild() &&
       //selectedItem->GetFirstChild()->GetNextSibling() &&
       selectedItem->IsOpen() ) {

    //Running = kTRUE;

    fPauseHistoButton->SetState( kButtonUp );
    fRestartHistoButton->SetState( kButtonDisabled );




    /*
    //if ( fUpdateTimer && !fUpdateTimer->HasTimedOut() )
    if ( fUpdateTimer )
      fUpdateTimer->TurnOff();
    */


    //SlideTimerOn();
    SlideTimerOn( selectedItem );

  } //if ( selectedItem &&


}


//void HistoDisplay::SlideTimerOn()
//void HistoDisplay::SlideTimerOn( cosnt TGListTreeItem *item )
void HistoDisplay::SlideTimerOn( TGListTreeItem *item )
{
  //std::cout << " In SlideTimerOn " << std::endl;

  if ( !item ) return;

  //if ( !gPad || ( gROOT->GetListOfCanvases()->IsEmpty() ) ) return;

  Int_t sleeptime  = 0;

  sleeptime =  atoi( fFrequencyTextBuffer->GetString() );

  if ( fSlideTimer ) {


    if ( fUpdateTimer && 
	 ( fUpdateTimer->GetCurrentPad() == gPad ) &&
	 !fUpdateTimer->HasTimedOut() )
      fUpdateTimer->TurnOff();

    if ( fSlideTimer && !fSlideTimer->HasTimedOut() )
      fSlideTimer->TurnOff();






    fSlideTimer->SetTime( std::max( sleeptime, 2000 ) );
    //std::cout << " Timer Time = " << fSlideTimer->GetTime() << std::endl;

    //fSlideTimer->SetCurrentItem( fSelectedItem ); 
    fSlideTimer->SetCurrentItem( item ); 


    //if ( item->GetFirstChild() ) 
    //fSlideTimer->SetDisplayingItem( item->GetFirstChild() );
  

    TGListTreeItem *ditem = fSlideTimer->GetDisplayingItem(); 
    if ( !ditem ||
	 ( ditem && ( ditem->GetParent() != item ) ) )
      fSlideTimer->SetDisplayingItem( item->GetFirstChild() );
    

    TVirtualPad *pad = 0;
    //if ( gPad && !gROOT->GetListOfCanvases()->IsEmpty() ) pad = gPad;
    if ( gROOT->GetListOfCanvases()->IsEmpty() ) gPad = 0;
    pad = gPad;
    fSlideTimer->SetCurrentPad( pad );




    /*
    if ( fUpdateTimer
	 && ( fUpdateTimer->GetCurrentPad() == gPad )
	 && !fUpdateTimer->HasTimedOut() )
      //if ( fUpdateTimer )
      fUpdateTimer->TurnOff();
    */


    Running = kTRUE;
    fSlideTimer->TurnOn();

  } // if ( fSlideTimer ) {

}



Bool_t HistoDisplaySlideTimer::Notify()
{
  //std::cout << " In Notify (Slide Timer ) " << std::endl;


  //if ( this &&
  //   !fHistoDisplay->Running  ) {
  if ( this &&
       ( !fHistoDisplay->Running || !fCurrentItem ) ) {



    fHistoDisplay->Running = kFALSE;
    fCurrentItem = 0;

    TurnOff();   // same as Remove();

    fHistoDisplay->fPauseHistoButton->SetState( kButtonDisabled );
    fHistoDisplay->fRestartHistoButton->SetState( kButtonUp );

    fHistoDisplay->fHistoListTree
      ->HighlightItem( fHistoDisplay->fSelectedItem );
    fHistoDisplay->fClient->NeedRedraw( fHistoDisplay->fHistoListTree );

    //std::cout << "Update done" << std::endl << std::endl;
    std::cout << "Slide show stopped" << std::endl << std::endl;
    return kTRUE;

  } //if ( this &&


  /*
  //-- usr signal handling

  if ( gReconnectFlag ) {

    std::cout<< "Reconnect to all consumers signal" << std::endl;

    fHistoDisplay->AutoReconnect();
    gReconnectFlag = false;

  }

  //--
  */




  //fHistoDisplay->CycleSlides( fCurrentItem );
  //fHistoDisplay->CycleSlides( fCurrentItem, fCurrentPad );
  fHistoDisplay->CycleSlides( fCurrentItem, fCurrentPad, fDisplayingItem );
  Reset();
  return kFALSE;

}




//void HistoDisplay::CycleSlides( const TGListTreeItem *currentitem )
//void HistoDisplay::CycleSlides( const TGListTreeItem *currentitem,
//			TVirtualPad *currentpad )
//void HistoDisplay::CycleSlides( const TGListTreeItem *currentitem,
//				TVirtualPad *currentpad,
//				TGListTreeItem *item )
void HistoDisplay::CycleSlides( TGListTreeItem *currentitem,
				TVirtualPad *currentpad,
				TGListTreeItem *item )
{
  //std::cout << " In CycleSlides " << std::endl;

  //static TGListTreeItem *item = 0;


  if ( !currentitem ) {
    Running = kFALSE;
    return;
  } //if ( !currentitem ) 


  TString objname;
  TVirtualPad *padsav = 0;


  if ( !item || ( item && ( item->GetParent() != currentitem ) ) ) 
    item = currentitem->GetFirstChild();


  if ( item ) {

    objname = item->GetText();
    TGListTreeItem *next = item->GetNextSibling();

    fHistoListTree->HighlightItem( item );
    fClient->NeedRedraw( fHistoListTree );


    //if ( !gROOT->GetListOfCanvases()->IsEmpty() && gPad )
    if ( gROOT->GetListOfCanvases()->IsEmpty() ) gPad = 0;
    padsav =  gPad;




    //TObject *obj = fDisplayedList->FindObject( item->GetText() );
    TObject *obj = GetCObjFromListItem( item );

    if( obj ) {
      if ( obj->InheritsFrom( CObject::Class() ) ) {

	const TObject *inputobj = 
	  ( (CObject*)obj )->GetInputSource();

	//std::cout << " item text in cycle slides " << item->GetText() << std::endl;


	//TObject *cobj = 
	//UpdateHistoList( item->GetText(), inputobj );

	//const char *objname = item->GetText();
	//TObject *cobj =  UpdateHistoList( objname, inputobj );

	TObject *cobj =  UpdateHistoList( ((CObject*)obj)->GetPath(), objname.Data(), inputobj );


	//fHistoListTree->HighlightItem( item );
	//fClient->NeedRedraw( fHistoListTree );

	if ( currentpad &&
	     gROOT->GetListOfCanvases()->FindObject( currentpad ) )
	  currentpad->cd();
	else if ( padsav ) {
	  padsav->cd();
	  fSlideTimer->SetCurrentPad( padsav );
	}

	DrawHisto( cobj );

	if ( padsav ) padsav->cd();

      } //if ( obj->InheritsFrom( CObject::Class() ) )
    }//if( obj )
    else
      std::cout << " No object: " << objname.Data() << std::endl;


    /*
    item = item->GetNextSibling();

    if ( !item ) 
      if ( AutoUpdate && fSelectedItem ) 
	item = fSelectedItem->GetFirstChild();
      else
	Running = kFALSE;

    fSlideTimer->SetDisplayingItem( item );
    */


    /*
    if ( !next ) 
      if ( AutoUpdate && fSelectedItem ) 
	next = fSelectedItem->GetFirstChild();
      else
	Running = kFALSE;
    */

    if ( !next )
      if ( AutoUpdate ) {
	if ( !fSelectedItem && currentitem ) fSelectedItem = currentitem;
	next = fSelectedItem->GetFirstChild();
      } //if ( AutoUpdate )
      else
	Running = kFALSE;


    fSlideTimer->SetDisplayingItem( next );




  } // if ( item )
  else {
    Running = kFALSE;
  } //else //if ( item )


}






void HistoDisplay::UpdatePad( TVirtualPad *pad )
{
  //std::cout << " In UpdatePad " << std::endl;

  if ( !pad ) return;

  DrawUpdatedPad( pad );
  pad->cd();
  pad->Modified();
  pad->Update();

}



void HistoDisplay::DrawUpdatedPad( TVirtualPad *currentPad )
{
  //std::cout << " In DrawUpdatedPad " << std::endl;

  if ( ! currentPad ) return;

  //if ( currentPad != currentPad->GetCanvas() ) {
  
  const char *name = currentPad->GetName();
  
  TObject *objP = fDisplayedList->FindObject( name );
  
  if ( objP && objP->InheritsFrom( CObject::Class() ) ) {
    
    const TObject *inputObj = ( (CObject*)objP )->GetInputSource();
    CObject *objNew = UpdateHistoList( ( (CObject*)objP )->GetPath(), name, inputObj );
    
    if ( objNew ) {
      TObject *tobj = objNew->GetTObject();

      if ( tobj->InheritsFrom( TPad::Class() ) ) {
	//DrawTPad( (TPad*)tobj, currentPad );
	ReplacePad( (TPad*)tobj, currentPad );
	return;
	} //if ( tobj->InheritsFrom( TPad::Class() ) )
      
    } //if ( objNew )
    
  } //if ( objP && objP->InheritsFrom( CObject::Class() ) )
  
  //} //if ( currentPad != currentPad->GetCanvas() )
  
  

  TObjOptLink *lnk = (TObjOptLink*)currentPad
    ->GetListOfPrimitives()
    ->FirstLink();

  TFrame *frame = currentPad->GetFrame();

  while( lnk ) {

    TObject *obj = lnk->GetObject();

    if ( obj ) {

      if ( obj->InheritsFrom(TPad::Class()) ) {
	DrawUpdatedPad( (TPad*)obj );
      } //if ( obj->InheritsFrom(TPad::Class()) )

      else {

      //if ( obj->InheritsFrom(TH1::Class()) ) {

	TObject *obj2 = fDisplayedList->FindObject(obj->GetName());
	
	if ( obj2 && obj2->InheritsFrom(CObject::Class()) ) {

	  const TObject *inputObj = ((CObject*)obj2)->GetInputSource();
	  //CObject *objNew = UpdateHistoList(obj2->GetName(), inputObj);
	  const char *objname = obj2->GetName();
	  CObject *objNew =  UpdateHistoList(((CObject*)obj2)->GetPath(), objname, inputObj );

	  //objNew->ls();

	  //if ( objNew && objNew->InheritsFrom(CHistogram::Class()) ) {

	  if ( objNew ) {

	    if ( objNew->InheritsFrom(CHistogram::Class()) ) {

	      //gROOT->SetSelectedPad( currentPad );

	      TH1 *histo = (TH1*) (((CHistogram*)objNew)->SubtractedHisto());
	      //TH1 *histo =  ( (CHistogram*)objNew )->GetHistoDrawn();

	      if ( histo ) {

		//tmp
		if ( ! currentPad->GetPadPaint() ) {

		  TH1 *histoOld = (TH1*)obj;

		  ReplaceHisto( histo, histoOld );
		  delete histo;

		  currentPad->Modified();

		} //if ( ! currentPad->GetPadPaint() )
		else
		  std::cout << " Pad is being painted. Not updated. " << std::endl;

	      } //if ( histo ) 

	      //} //if (objNew && objNew->InheritsFrom(CHistogram::Class()) )
	    } //if ( objNew->InheritsFrom(CHistogram::Class()) )

	    else {

	      if ( ! currentPad->GetPadPaint() ) {

		if ( obj->IsEqual( frame ) ) {

		  lnk->SetObject( objNew->Clone() );
		  if ( obj ) delete obj;

		} //if ( obj->IsEqual( frame ) )

		currentPad->Modified();

	      } //if ( ! currentPad->GetPadPaint() )
	      else
		std::cout << " Pad is being painted. Not updated. " << std::endl;


	    } //else //if ( objNew->InheritsFrom(CHistogram::Class()) )


	  } //if ( objNew )


	} //if ( obj2 && obj2->InheritsFrom(CObject::Class()) )

	//} //if ( obj->InheritsFrom(TH1::Class()) )

      } //else //if ( obj->InheritsFrom(TPad::Class()) )

    } //if ( obj )

    lnk = (TObjOptLink*) ( lnk->Next() );

  } //  while( lnk ) 

  //std::cout << " End of DrawUpdatedPad " << std::endl;
}



void HistoDisplay::ReplacePad( TVirtualPad *newpad, TVirtualPad *oldpad )
{
  //std::cout << " In ReplacePad " << std::endl;

  if ( !newpad || !oldpad ) return;


  newpad->TAttLine::Copy( (TAttLine&)*oldpad );
  newpad->TAttFill::Copy( (TAttFill&)*oldpad );
  newpad->TAttPad::Copy( (TAttPad&)*oldpad );


  TFrame *frame = newpad->GetFrame();

  TList *oldPrimitiveList = oldpad->GetListOfPrimitives();
  TList *newPrimitiveList = newpad->GetListOfPrimitives(); 

  TIter next( newPrimitiveList );

  TObject *obj;
  while( ( obj = next() ) ) {

      TObject *objOld = 
	oldPrimitiveList->FindObject( obj->GetName() );
      TObject *cobj =
	fDisplayedList->FindObject( obj->GetName() );

      if ( obj->InheritsFrom( TPad::Class() ) &&
	   objOld &&
	   objOld->InheritsFrom( TPad::Class() ) )

	ReplacePad( (TPad*)obj, (TPad*)objOld );

      else if ( obj->InheritsFrom( TH1::Class() ) 
		&& objOld
		&& objOld->InheritsFrom( TH1::Class() ) ) {

	TH1 *histo;
	TH1 *histoOld = (TH1*)objOld;

	if ( cobj && cobj->InheritsFrom( CHistogram::Class() ) ) {
	  CHistogram *chisto = (CHistogram*)cobj;
	  //chisto->SetTObject( obj );
	  histo = chisto->SubtractedHisto();
	} //if ( cobj && cobj->InheritsFrom( CHistogram::Class() ) )
	else {
	  histo = (TH1*)( obj->Clone() );
	} //else //if ( cobj && cobj->InheritsFrom( CHistogram::Class() ) )

	if ( histo && histoOld
	     && ! obj->IsEqual( objOld )  ) {
	  //tmp
	  if ( ! oldpad->GetPadPaint() ) {

	    ReplaceHisto( histo, histoOld );
	    delete histo;

	    oldpad->Modified();

	  } //if ( ! oldpad->GetPadPaint() )
	  else
	    std::cout << " Pad is being painted. Not updated. " << std::endl;

	} //if ( histo && histoOld )

      }  //else if ( obj->InheritsFrom( TH1::Class() ) && 

      //tmp
      // Should be comment out the following to avoid double TFrames.
      else {

	if ( !( frame->IsEqual( obj ) ) ) {

	  if ( objOld ) {
	       //&& !obj->IsEqual(objOld) ) {

	    // tmp v.3.071
	    // For Root TPaveStats destructor --

	    if ( objOld->InheritsFrom( TPaveStats::Class() ) ) {
	      TPaveStats *stats = (TPaveStats*)objOld;

	      if ( stats && !stats->GetParent() )
		stats->SetParent( oldpad->GetListOfPrimitives() );

	    } //if ( objOld->InheritsFrom( TPaveStats::Class() ) )
	    //--


	    TObject *objtmp = oldPrimitiveList->Remove( objOld );
	    if ( objtmp ) delete objOld;
	  } //if ( objOld ) 

	  //std::cout << " obj name " << obj->GetName() << std::endl;

	  //oldpad->cd();
	  //obj->Clone()->Draw( next.GetOption() );

	  oldPrimitiveList
	    ->Add( obj->Clone(), next.GetOption() );

	  oldpad->Modified();


	} //if ( !( frame->IsEqual( obj ) ) )

      } //else //if ( obj->InheritsFrom( TPad::Class() ) &&

  } //while ( obj = next() )


}
 

void HistoDisplay::ReplaceHisto( TH1 *histo, TH1 *histoOld )
{
  //std::cout << " In ReplaceHisto " << std::endl;

  if ( !histo || !histoOld ) return;

  histoOld->Reset();

  histo->TAttLine::Copy( (TAttLine&)*histoOld );
  histo->TAttMarker::Copy( (TAttMarker&)*histoOld );
  histo->TAttFill::Copy( (TAttFill&)*histoOld );

  histoOld->Add( histo );

  TAxis *xaxis = histo->GetXaxis();
  if ( xaxis ) {
    char *title = new char[ strlen( xaxis->GetTitle() ) + 1 ];
    strcpy( title, xaxis->GetTitle() ); 
    histoOld->SetXTitle( title );
    delete [] title;
  } //if ( xaxis )
  TAxis *yaxis = histo->GetYaxis();
  if ( yaxis ) {
    char *title = new char[ strlen( yaxis->GetTitle() ) + 1 ];
    strcpy( title, yaxis->GetTitle() ); 
    histoOld->SetYTitle( title );
    delete [] title;
  } //if ( yaxis )
  TAxis *zaxis = histo->GetZaxis();
  if ( zaxis ) {
    char *title = new char[ strlen( zaxis->GetTitle() ) + 1 ];
    strcpy( title, zaxis->GetTitle() ); 
    histoOld->SetZTitle( title );
    delete [] title;
  } //if ( zaxis ) 

  //delete histo;

}



void HistoDisplay::ClearCurrentPad() {

  //std::cout << " In ClearCurrentPad " << std::endl; 

  //TPad *padsav = (TPad*) gPad;
  TPad *pad = (TPad*)gROOT->GetSelectedPad();

  if ( !pad ) return;

  //pad->Clear();
  RecursivePadClear( pad );
  pad->cd();

  
  /*
    if ( pad != pad->GetCanvas() ) {
    pad->SetName("VOID");
    }
  */
  pad->SetName("VOID");

  pad->SetTitle("Cleared");

  pad->Modified();
  pad->Update();
  //padsav->cd();

  //std::cout << "Cleared " << std::endl;

}






CObject* HistoDisplay::
UpdateHistoList(const char* objPath, const char* objName, const TObject *inputobj )
{
  std::cout << " In UpdateHistoList " << std::endl;
  std::cout << " updating " << objName << std::endl;

  /*
  if ( !inputobj ) {
    std::cout << " Input Object is not set. " << std::endl; 
    return 0;
  }
  */

  //const char *path = gROOT->GetPath(); 
  TVirtualPad *padsav = 0;
  if ( gPad && !gROOT->GetListOfCanvases()->IsEmpty() ) padsav = gPad;



  TObject *obj = GetNewObj( objPath, objName, inputobj );
  CObject *cobj = 0; 

  if ( obj ) {

    TObject *objold = fDisplayedList->FindObject( objName );

    /*
    TObjLink *lnk = fDisplayedList->FirstLink();
    while (lnk) {
      TObject *objtmp = lnk->GetObject();
      if (objtmp && 
	  objtmp->GetName() && !strcmp(objName, objtmp->GetName())) {

	if ( objtmp->InheritsFrom( CObject::Class() ) ) {
	  CObject *cobjtmp = (CObject*)objtmp;
	  TObject *inputtmp = cobjtmp->GetInputSource();
	  if ( ( inputtmp && inputtmp->GetName() ) &&
	       ( inputobj && inputobj->GetName() ) &&
	       ( !strcmp( inputtmp->GetName(),
			  inputobj->GetName() ) ) ) {
	    //objold = objtmp;
	    cobj = cobjtmp;
	    cobj->SetTObject( obj );
	    if ( cobj->GetInputSource() != inputobj )
	      cobj->SetInputSource( inputobj );
	      
	  } //if ( ( inputtmp && inputtmp->GetName() ) &&


	}//if ( objtmp->InheritsFrom( CObject::Class() ) ) 


      } //if ( objtmp && objtmp->GetName() && ... )

      lnk = lnk->Next();
    } //while (lnk) 

    */


    if ( objold && objold->InheritsFrom( CObject::Class() ) ) {

      cobj = (CObject*) objold;

      if ( cobj ) {
	cobj->SetTObject( obj );
        cobj->SetPath(TString(objPath));
	//if ( cobj->GetInputSource() != inputobj )
	if ( inputobj != cobj->GetInputSource()  ) {
	  cobj->SetInputSource( inputobj );
	  //tmp
	  //std::cout << " InputObj is updated. " << std::endl;
	} // if ( inputobj != cobj->GetInputSource()  )  
      } // if ( cobj )

    } //if ( objold && objold->InheritsFrom( CObject::Class() ) )

    else {

      //if ( !cobj ) {

      if ( obj->InheritsFrom(TH1::Class()) ) {

	CHistogram *cobjnew =
	  new CHistogram( (TH1*)obj, inputobj, 0, 0 );        
	fDisplayedList->Add( cobjnew );

	cobj = cobjnew;
	
      } //if ( obj->InheritsFrom(TH1::Class()) )
      else {

	CObject *cobjnew =
	  new CObject( obj, inputobj );
	
	fDisplayedList->Add( cobjnew );

	cobj = cobjnew;

      } // else //if ( obj->InheritsFrom(TH1::Class()) )

      //} // if ( !cobj ) 
	cobj->SetPath(TString(objPath));
    } //else //if ( objold && objold->InheritsFrom( CObject::Class() ) )

    if ( obj->InheritsFrom( TPad::Class() ) )
      AddPadHistosToHistoList( (TPad*)obj );

  } //if ( obj )

  else {

    //tmp
    /*
    if ( objold && objold->InheritsFrom( CObject::Class() ) ) 
      cobj = (CObject*) objold;
    */
    //tmp
    //std::cout << " obj is not got from the input stream. " << std::endl;

  } //else //if(obj)

  //tmp
  if ( !cobj ) std::cout << " Not updated. " << std::endl;


  //gROOT->Cd( path );
  if ( padsav ) padsav->cd(); 


  return cobj;

}



void HistoDisplay::AddPadHistosToHistoList( TPad *pad )
{
  //std::cout << " In AddPadHistosToHistoList " << std::endl; 

  if ( !pad ) return;

  TObjLink *link = pad->GetListOfPrimitives()->FirstLink();

  while ( link ) {

    TObject *obj = link->GetObject();

    if ( obj ) {

      if ( obj->InheritsFrom( TPad::Class() ) ) 
	AddPadHistosToHistoList( (TPad*)obj ); 

      else if ( obj->InheritsFrom( TH1::Class() ) ) {

	TObject *objold = fDisplayedList->FindObject( obj->GetName() );

	if ( objold && objold->InheritsFrom( CObject::Class())  ) {

	  CObject *cobj = (CObject*)objold;
	  //cobj->SetTObject( obj );
	  cobj->SetTObject( obj->Clone() );

	} //if ( objold && objold->InheritsFrom( CObject::Class()) )
	else {

	  CHistogram *cobjnew =
	    //new CHistogram( (TH1*)obj, 0, 0, 0 );
	    new CHistogram( (TH1*)obj->Clone(), 0, 0, 0 );

	  fDisplayedList->Add( cobjnew );

	} //else //if ( objold && objold->InheritsFrom( CObject::Class()) ) )

      } //else if ( obj->InheritsFrom( TH1::Class() ) )

    } //if ( obj ) 

    link = link->Next();

  } //while ( link )

  //std::cout << " End of AddPadHistosToHistoList " << std::endl; 

}



TObject* HistoDisplay::
//GetNewObj(const char* objName, const TObject* inputobj)
GetNewObj(const char* objpath, const char* objname, const TObject* inputobj)
{
  //std::cout << " In GetNewObj" << std::endl;
  //std::cout << " objname = " << objname << std::endl;

  TObject *obj = 0;
  string value(objname);
  string oname = value;
  int pos = value.rfind("/", value.size());
  if (pos != string::npos)
      oname = value.substr(pos+1, value.size());
  TString objName = TString( objname );
  TString objPath = TString(objpath) + TString(oname.c_str());

  if (inputobj) {

    if ( inputobj->IsA() == TMapFile::Class() )   {
      TMapFile *mfile = (TMapFile*)inputobj;
      //if (mfile) obj = (TObject*)mfile->Get( objName );
      if (mfile) obj = (TObject*)mfile->Get( objName.Data() );
    }
    else if ( inputobj->IsA() == TFile::Class() )   {
      TFile *rfile = (TFile*)inputobj;
      //if (rfile) obj = (TObject*)rfile->Get( objName );
      if (rfile) obj = (TObject*)rfile->Get( objName.Data() );
    }
    else if ( inputobj->IsA() == TSocket::Class() )   {
      TSocket *socket = (TSocket*)inputobj;

      if ( socket && socket->IsValid() ) {

	Int_t count = 0;
	Int_t maxcount = 8; 

	while ( !obj && count < maxcount ) {

	  //socket->Send( objName );
	  // socket->Send( objName.Data() );

	  Int_t resSend;
	  resSend = socket->Send( objPath.Data() );
	  // resSend = socket->Send( objName.Data() );


	  if ( resSend == -1 ) {
	    std::cerr << " HistoDisplay: ERROR:"
		      << " Socket->Send failed.  resSend = " << resSend << ".\n" 
		      << "    ==> Closing the socket connection." << ".\n"
		      << "    ==> Please open again. "
		      << std::endl;
	    socket->Close();
	    Running = kFALSE;  
	    count = maxcount;      // Stop the request loop
	  } //if ( resSend == -1 )
	  if ( resSend == -4 ) {
	    std::cerr << " HistoDisplay: ERROR:"
		      << " Socket->Send failed.  resSend = " << resSend << ".\n" 
		      << " Nothing to be read. "
		      << std::endl;
	  } //if ( resSend == -4 )
	  else {



	  TMessage *message = NULL;

	  Int_t resRecv;
	  resRecv = socket->Recv(message);
	  //std::cout << "resRecv: " << resRecv << std::endl;

	  if (resRecv == -1) {
	    std::cerr << " HistoDisplay: ERROR:"
		      << " Socket->Receive failed. resRecv = " << resRecv << ".\n" 
		      << "    ==> Closing the socket connection." << ".\n"
		      << "    ==> Please open again. "
		      << std::endl;
	    socket->Close();
	    Running = kFALSE;  
	    count = maxcount;      // Stop the request loop
	  } //if (resRecv == -1)
	  else if (resRecv == -4) {
	    std::cerr << " HistoDisplay: ERROR:"
		      << " Socket->Receive failed. resRecv = " << resRecv << ".\n" 
		      << " Nothing to be read. "
		      << std::endl;
	  } //if (resRecv == -4)
	  else if (resRecv == 0 ) {
	    std::cerr << " HistoDisplay: ERROR:"
		      << " Socket->Receive failed. resRecv = " << resRecv << ".\n" 
		      << " The other side is closed connection. "
		      << std::endl;
	    socket->Close();
	    Running = kFALSE;  
	    count = maxcount;      // Stop the request loop
	  } //if (resRecv == 0 )
	  else {


	    if (message) {
              if (message->What() == kMESS_OBJECT) {
	        obj = (TObject*)message->ReadObject(message->GetClass());
	        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	        // The following was added: //2001/01/16 Wolfgang Wagner
                if (obj) {
		  string v = obj->GetName();
	          string name = v;
                  int pos = v.rfind("/", v.size());
                  if (pos != string::npos) 
			name = v.substr(pos+1, v.size()); 
		  
                  if (TString(name.c_str()) != TString(oname.c_str()))
		    std::cerr << "ERROR: Got object: " <<  obj->GetName()
		      //<< " but requested " << objName << "." << std::endl;
			 << " but requested " << objName.Data() 
			 << "." << std::endl;
                } //if (obj)
	        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              } //if (message->What() == kMESS_OBJECT)
              else {
                if (message->What() == kMESS_STRING) {          
                  int messLength = message->BufferSize() + 10; 
                  char* keyString = new char[messLength];
                  message->ReadString(keyString,messLength);                  
                  string serverString(keyString);
                  delete keyString;
                  keyString = 0;
                  // We got a message string from Display Server 
                  // indicating a special request.
                  // Now we need to decode the string.
                  // The key strings are defined in ServerProtocol.hh
                  //
                  bool localDebug = false;
                  if (localDebug) {
                    std::cout << "HistoDisplay: got string: " << serverString
		         << "." << std::endl;
                  } 
                  if (serverString == DspEndConnection) {
                    std::cout << "HistoDisplay: The Display Server ended the "
			 << "connection." << std::endl;
                    count = maxcount;
                    socket->Close();
         	    Running = kFALSE;  
                  }
                  if (serverString == DspObjectNotFound) {
                    std::cerr << "HistoDisplay: The requested object '"
		      // << objName << "' is not available from the "
			 << objName.Data() << "' is not available from the "
                         << std::endl;
                    std::cerr << "Display Server." << std::endl;
                    count = maxcount;                      
                  }
		  if (serverString == DspRequestNewInfo) {
                    if (localDebug) {
                      std::cout << "HistoDisplay: The Display Server suggests "
		           << "to request the TConsumerInfo." << std::endl;
                    } //if (localDebug)
		    //AddTreeItemsOfInput( socket );
		    AddTreeItemsOfInput( socket, kFALSE );
                  } //if (serverString == DspRequestNewInfo)
                } //if (message->What() == kMESS_STRING)
	      } //else //if (message->What() == kMESS_OBJECT)
	    } //if (message)
	  } //else //if (resRecv == -1) //if (resRecv == -4, 0 ) 


	  //delete message;
	  if ( message ) delete message;
	  count++;

	  } //else //if ( resSend == -1, -4 )

	       
	} //while ( !obj && count < maxcount )




	if ( ! obj ) {
//	  std::cout << " Failed to get " << objName << std::endl;
	  std::cout << " Failed to get " << objPath << std::endl;
	} //if ( ! obj )
	else {
	  //if ( count > 1 ) std::cout << " The object " << objName 
//	  if ( count > 1 ) std::cout << " The object " << objName.Data() 
          if ( count > 1 ) std::cout << " The object " << objPath.Data()
				<< " is sent from the socket by " << count
				<< " tries." << std::endl;
	} //else //if ( ! obj )



      } //if ( socket && socket->IsValid() )
      else {
	std::cout << "no connection to socket..."
	     << socket->GetName() << std::endl;
	Running = kFALSE;
      } //else //if ( socket && socket->IsValid() )

    } // TMapFile,TFile, TSocket 
    else {

      std::cout << " No inputobj " << std::endl;

    } //else // TMapFile,TFile, TSocket

  } // if(inputobj)

  //tmp
  //if ( !obj ) std::cout << " No object found " << std::endl;

  //std::cout << " End of GetNewObj for objname " << objName << std::endl;
  //std::cout << " End of GetNewObj for objname " << objName.Data() << std::endl;



  return obj;

}







void HistoDisplay::AddSubtractHisto( TVirtualPad *currentPad , int option)
{
  // Zero or Restore all histos on the currentPad.
  // int option = 0    ; unzero
  //            = else ;   zero 

  //std::cout << " In AddSubtractHisto" << std::endl;

  if (!currentPad) return;

  TList *primitiveList = currentPad->GetListOfPrimitives();
  TIter next(primitiveList);
  TObject *obj = 0;

  while ( ( obj = next() ) ) {
    if ( obj->InheritsFrom(TPad::Class()) ) {

      AddSubtractHisto( (TPad*)obj , option);

    } //if ( obj->InheritsFrom(TPad::Class()) )
    else {

      if ( obj->InheritsFrom(TH1::Class()) ) {

	TObject *obj2 = 
	  fDisplayedList->FindObject(obj->GetName());

	if( obj2 && obj2->InheritsFrom(CHistogram::Class()) ) {

	  CHistogram *chisto = (CHistogram*)obj2;
	  TH1 *histo = (TH1*) chisto->GetTObject();

	  if (option) {
	    if (histo) chisto->SetSubtractHisto( histo );
	  } //if (option)
	  else {
	    chisto->SetSubtractHisto(0);
	  } //else //if (option)


	}//if( obj2 && obj2->InheritsFrom(CHistogram::Class()) )


      } //if ( obj->InheritsFrom(TH1::Class()) )


    } //else//if ( obj->InheritsFrom(TPad::Class()) )


  } //while ( obj = next() )

}



TH1* HistoDisplay::HistoInPad( TVirtualPad *pad, Int_t &counthisto ) 
{
  //std::cout << " In HistoInPad " << std::endl;

  if ( !pad ) return 0;

  TList *primitiveList = pad->GetListOfPrimitives();
  TIter next( primitiveList );

  TH1 *histo = 0;

  TObject *obj;
  while ( ( obj = next() ) && ( counthisto <= 1 ) ) {

    if ( obj->InheritsFrom( TH1::Class() ) ) {
      counthisto++ ;
      if ( counthisto == 1 ) histo = (TH1*)obj;
    } //if ( obj->InheritsFrom( TH1::Class() ) )

    if ( obj->InheritsFrom( TPad::Class() ) )
      histo = HistoInPad( (TPad*)obj , counthisto );

  } //while ( ( obj = next() ) && ( counthisto <= 1 ) )

  if ( counthisto >= 2 ) return 0;

  return histo;

}




void HistoDisplay::DrawContentHisto(TH1* histo)
{
  //std::cout << " In DrawContentHisto" << std::endl;
  if ( !histo ) return;

  DrawOnTheCanvas( "ContentHistoCanvas", "Bin-Content Histogram Canvas",
		   300, 300,
		   ContentHisto(histo) ) ;
}



TH1F* HistoDisplay::ContentHisto( TH1* histo )
{
  //std::cout << " In ContentHisto" << std::endl;
  if (!histo) return 0;

  float min, max;
  min = histo->GetMinimum();
  max = histo->GetMaximum();

  if ( min == max ) return 0;

  int nbin;
  nbin = 50;
  float binsize;
  binsize = (max - min)/nbin;

  TH1F* contenthisto = 
    new TH1F("BinContenHisto", histo->GetTitle(), nbin,
	    min - 2*binsize, max + 2*binsize );
  // oldone is removed by canvas->Clear() in DrawOntheCanvas.

  contenthisto->SetXTitle("Hits in a bin");
  contenthisto->SetYTitle("Number of bins");

  Int_t nbinsx = histo->GetNbinsX();
  Int_t nbinsy = histo->GetNbinsY();
  Int_t nbinsz = histo->GetNbinsZ();


   Int_t bin, binx, biny, binz;
   double content;
   for (binz=0;binz<=nbinsz+1;binz++) {
     for (biny=0;biny<=nbinsy+1;biny++) {
       for (binx=0;binx<=nbinsx+1;binx++) {
            bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
	    content = histo->GetBinContent(bin);
	    if (content) contenthisto->Fill(content);
       }
     }
   }

   return contenthisto;
}


void HistoDisplay::DrawOnTheCanvas(
 const char* canvasname, const char* canvastitle, int padx, int pady, 
 TObject *obj )
{
  //std::cout << " In DrawOnTheCanvas" << std::endl;
  if (!obj) return;

  TPad *padsav  = (TPad*)gPad;

  TCanvas *canvas;

  canvas = 
    (TCanvas*) ( gROOT->GetListOfCanvases()->FindObject( canvasname ) );

  if (!canvas) {
    canvas = new TCanvas( canvasname, canvastitle, padx, pady);
  }
  else {
    //canvas->SetWindowSize( padx, pady);
    canvas->cd();
    canvas->Clear();
  } //if (!canvas)

  obj->Draw();

  canvas->Modified();
  canvas->Update();

  padsav->cd();

}



void HistoDisplay::PrintCanvas( TPad *canvas )
{
  //std::cout << " In PrintCanvas " << std::endl;
  // From TGTextEdit.cxx
    Int_t ret;
    static char *gPrinter = 0;
    static char *gPrintCommand = 0;

    if (!gPrinter) {
      //gPrinter = StrDup(""); // use gEnv
      //gPrintCommand = StrDup("lpr");
      gPrinter = StrDup("b0ctl_tcolor"); // use gEnv
      gPrintCommand = StrDup("flpr -q");
    } //if (!gPrinter)

    TGPrintDialog *printdialog =
      new TGPrintDialog( fClient->GetRoot(), this, 400, 150,
			&gPrinter, &gPrintCommand, &ret );

    if ( ret ) {
      if ( canvas ) {

	const char *canvasname = canvas->GetName();

	char *msg = 0;
	if ( strlen( gPrinter ) ) {
	  /*
	  msg = new char[ strlen( gPrintCommand ) + 3 + strlen( gPrinter ) 
			+ 1 + strlen( canvasname ) + 4 + 1 ];
	  sprintf( msg, "%s -P%s %s.ps\n", gPrintCommand,
		   gPrinter, canvasname );
	  */
	  msg = new char[ strlen( gPrintCommand ) + 1 + strlen( gPrinter ) 
			+ 1 + strlen( canvasname ) + 6 + 1 ];
	  sprintf( msg, "%s %s \"%s.ps\"\n", gPrintCommand,
		   gPrinter, canvasname );

	} //if ( gPrinter )
	else {
	  msg = new char[ strlen( gPrintCommand )
			+ 1 + strlen( canvasname ) + 6 + 1 ];
	  sprintf( msg, "%s \"%s.ps\"\n", gPrintCommand, canvasname );  
	} //else //if ( gPrinter ) 


	canvas->Print( 0, "ps" );

	std::cout << msg << std::endl;
	gSystem->Exec( msg );

	if ( msg ) delete [] msg;

      } //if ( canvas )
    } //if (ret)

    // do not need delete printdialog
    // because done by closing the windown.
}


void HistoDisplay::CloseWindow()
{
  gApplication->Terminate(0);
  //delete this;

}

void HistoDisplay::ResetObjects()
{
  //std::cout << " In ResetObjects " << std::endl;

  RemoveTGListTree();

  if ( fDisplayedList ) fDisplayedList->Delete();

  CloseConnection();
}


HistoDisplay::~HistoDisplay()
{

  ResetObjects();

  //delete fHistoList;

  delete fConsumerInfos;

  delete fDisplayedList;
  delete fSlideCanvasList;

  delete fUpdateTimer;
  delete fSlideTimer;

  CloseMenuBar();
  CloseTitleFrame();
  CloseTGListTreeFrame();
  CloseInputFrame();
  CloseStatusBar();

  delete fMainFrame;
  delete fMainFrameLayout;
}


void HistoDisplay::MenuConnectStatus()
{
  /*
	TGTransientFrame *fTransientFrame;
	fTransientFrame =
	  new TGTransientFrame(gClient->GetRoot(), this, 500, 200);
	TGLayoutHints *fLayout;
	fLayout =
	  new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);

	TGTextView *fTextView;
        fTextView =
	  new TGTextView( fTransientFrame, 600, 200);
			  //,kSunkenFrame | kDoubleBorder );

	TString StatusTxt = "\n";
	consumerlist->readWebPage();
	std::ostrstream ostr;
	consumerlist->print(ostr);
	ostr << std::ends;
	StatusTxt.Append(ostr.str());


	if ( fTextView->LoadBuffer( StatusTxt.Data() ) ) {
	  fTextView->MapWindow();
        }
	else {
	  std::cout << " Failed to load Socket status text info."  << std::endl;
	}

	fTransientFrame->AddFrame(fTextView,fLayout);
	fTransientFrame->SetWindowName("Connection Status");
	fTransientFrame->MapSubwindows();
	fTransientFrame->MapWindow();
  */

  OpenConsumerListBox();

}


//Help menu contents modified by Koji (2000/04/19)

void HistoDisplay::MenuHelp(char *s, const char *helpText)
{
  //std::cout << " In MenuHelp " << std::endl;

  TRootHelpDialog  *fHelpDialog;
  fHelpDialog =
    new TRootHelpDialog( this, s, 500, 400);
  {
    fHelpDialog->SetText(helpText);
    fHelpDialog->Popup();
  }
}

void HistoDisplay::MessageBox(const char *msg)
{
  //std::cout << " In MessageBox " << std::endl;

        int retval;

        EMsgBoxIcon icontype = kMBIconExclamation;
        //Int_t buttons = kMBOk;
        int buttons = kMBOk;

        new TGMsgBox(fClient->GetRoot(), this, "WARNING", msg, 
		     icontype, buttons, &retval);
}


// Choose File Name buttons. --

const char* HistoDisplay::FileDialogWindow()
{ 

  //const char *filetypes[] = { "All files",     "*",
  //		      "ROOT files",    "*.root",
  //		      "MAP files",     "*.map",
  //		      0,               0 };


  static TGFileInfo fi;
  //fi.fFileTypes = (char **)filetypes;


  //EInputTypes inputtype;

  if ( fRaButMFile->GetState() == kButtonDown ) {
    //inputtype = kTMapFile;
    const char* filetypes[] = { "MAP files",     "*.map",
		    "All files",     "*",
		    "ROOT files",    "*.root",
		    0,               0 };
#ifndef NO_ROOT_CONST
    fi.fFileTypes = (const char **)filetypes;
#else
    fi.fFileTypes = (char **)filetypes;
#endif
  }
  else if ( fRaButRFile->GetState() == kButtonDown ) {
    //inputtype = kTFile;
    const char* filetypes[] = {"ROOT files",    "*.root",
		   "All files",     "*",
		   "MAP files",     "*.map",
		   0,               0 };
#ifndef NO_ROOT_CONST
    fi.fFileTypes = (const char **)filetypes;
#else
    fi.fFileTypes = (char **)filetypes;
#endif 
  }
  else if ( fRaButSocket->GetState() == kButtonDown ) {
    //inputtype = kTSocket;
    const char* filetypes[] = { "All files",     "*",
		    "ROOT files",    "*.root",
		    "MAP files",     "*.map",
		    0,               0 };
#ifndef NO_ROOT_CONST
    fi.fFileTypes = (const char **)filetypes;
#else
    fi.fFileTypes = (char **)filetypes;
#endif 
  }
  else if ( fRaButSOAP->GetState() == kButtonDown ) {
    //inputtype = kTSocket;
    const char* filetypes[] = { "All files",     "*",
                    "ROOT files",    "*.root",
                    "MAP files",     "*.map",
                    0,               0 };
#ifndef NO_ROOT_CONST
    fi.fFileTypes = (const char **)filetypes;
#else
    fi.fFileTypes = (char **)filetypes;
#endif
  }



  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fi);

  return fi.fFilename;
}

void HistoDisplay::FileNameClear()
{
  //std::cout << " In File Name Clear " << std::endl;
  fFileNameBuffer->Clear();
  fFileNameTextEntry->SetState(kTRUE);
}

//--



Bool_t HistoDisplay::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

  //std::cout << " In ProcessMessage " << std::endl;

  //static TObject *inputobj = 0;
  //TObject *inputobj2 =0;
  TPad *gpad = 0;

  switch ( GET_MSG(msg) ) {
  case kC_COMMAND:
    switch ( GET_SUBMSG(msg) ) {
    case kCM_MENU:
      switch ( parm1 ) {
      case kM_MENU_FILE_NEWGUI:
	new HistoDisplay(gClient->GetRoot(), 200, 400);
	break;
      case kM_MENU_FILE_NEWCANVAS:
	//gROOT->GetMakeDefCanvas()();
	/*
	{
	  CanvasNumber++;
	  char *num = new char[3];
	  sprintf( num, "%d", CanvasNumber);
	  TCanvas *CurrentCanvas =
	    new TCanvas( CanvasName + TString( num ),
			 CanvasTitle + TString( num) , 500, 500 );
	  delete [] num;
	  CurrentCanvas->cd();
	}
	*/
	OpenDefaultCanvas();
	break;
      case kM_MENU_FILE_NEWSLIDE:
	{
	  static int i = 0;
	  i++;
	  //TString name = "CDFSlideShowCanvas";
	  //TString title = "CDF Slide Show Canvas ";
	  TString name = "AutoUpdateCanvas";
	  TString title = "Auto Update Canvas ";
	  char *num = 0;
	  num = new char[ 3 ];
	  sprintf( num, "%d", i );
	  name = name + TString( num );
	  title = title + TString( num );
	  delete [] num;
	  //tmp
	  //std::cout << name << " " << title << std::endl; 
	  TCanvas *slideCanvas = new TCanvas( name, title, 500, 500 );
	  slideCanvas->cd();
	  if ( fSlideCanvasList ) {
	    if ( !( fSlideCanvasList->IsEmpty() ) )  {
	      TObjLink *link = fSlideCanvasList->FirstLink();
	      while ( link ) {
		TObject *obj = 0;
		obj = link->GetObject();
		link = link->Next();
		if ( !obj ||
		     !( gROOT->GetListOfCanvases()->FindObject( obj ) ) )
		  fSlideCanvasList->Remove( obj );
	      } //while ( link )
	    } //if ( !( fSlideCanvasList->IsEmpty() )
	    fSlideCanvasList->Add( slideCanvas );
	  } //if ( fSlideCanvasList )
	} //{
	break;
      case kM_MENU_FILE_CLOSE:
	// Do not know how to close one window.
	DestroyWindow(); // in TGWindow
	//std::cout << " Sorry. Not yet. " << std::endl;
	break;
      case kM_MENU_FILE_EXIT:
	Running = kFALSE;
	ResetObjects();

	CloseWindow();
	break;
      case kM_MENU_LISTTREE_SORTTREE:
	SortTGListTree();
	break;
      case kM_MENU_LISTTREE_SORTFOLDER:
	if ( fSelectedItem ) {
	  fHistoListTree->SortChildren(fSelectedItem);
	  fClient->NeedRedraw(fHistoListTree);
	}
	break;
      case kM_MENU_LISTTREE_DELETE:
	if ( fSelectedItem ) {
	  CObject *cobj = 
	    (CObject*) fDisplayedList
	    ->FindObject(fSelectedItem->GetText()); 

	  if ( fDisplayedList->Remove( cobj ) ) delete cobj;

	  /*
	  if ( fSlideTimer && (fSelectedItem == fSlideTimer->fCurrentItem ) ) 
	    fSlideTimer->fCurrentItem = 0;

	  fHistoListTree->DeleteItem( fSelectedItem );
	  */
	  DeleteTreeItem( fSelectedItem );

	  fClient->NeedRedraw(fHistoListTree);
	}
	fSelectedItem = 0;
	break;
      case kM_MENU_LISTTREE_CLEAR:

	if ( fDisplayedList ) fDisplayedList->Clear();
	RemoveTGListTree();
	break;
      case kM_MENU_LISTTREE_REFRESH:

	if ( fSelectedItem ) 
	  {
	    RefreshTreeItem( fSelectedItem );
	  }
	else
	  {
  	    RefreshTreeItem( fHistoListTree->GetFirstItem() );
	    //RefreshTreeItem( fRefListTree->GetFirstItem() ); // Cavanaugh 03.04.2005
	  }

	fClient->NeedRedraw( fHistoListTree );
	//fClient->NeedRedraw( fRefListTree );  // Cavanaugh 03.04.2005

	break;
	/*
      case kM_MENU_CONNECT_MON1:
	std::cout << " Sorry. Not yet implemented. " << std::endl;
	break;
      case kM_MENU_CONNECT_MON2:
	std::cout << " Sorry. Not yet implemented. " << std::endl;
	break;
      case kM_MENU_CONNECT_MON3:
	std::cout << " Sorry. Not yet implemented. " << std::endl;
	break;
      case kM_MENU_CONNECT_MON4:
	std::cout << " Sorry. Not yet implemented. " << std::endl;
	break;
      case kM_MENU_CONNECT_MON5:
	std::cout << " Sorry. Not yet implemented. " << std::endl;
	break;
	*/

      case kM_MENU_CONNECT_STATUS:
        MenuConnectStatus();
	break;

      case kM_MENU_HELP_GUI:
        MenuHelp("Help on this GUI...", gHelpHistoDisplay);
	break;

      case kM_MENU_HELP_ROOT:
        MenuHelp("Help on ROOT...", gHelpAbout);
        break;

      case kM_MENU_HELP_CANVAS:
        MenuHelp("Help on Canvas...", gHelpCanvas);
	break;
      } // switch( parm1 )
      break; //kCM_MENU


    case kCM_BUTTON:
      switch ( parm1 ) {

      case kM_PICTBUTTON:
	//PopupWindow();
        new PhotoWindow(fClient->GetRoot(), this, 400, 500);
	break;


      case kM_BUTTON_FILENAME:
	{
	  TString FileName = TString( fFileNameBuffer->GetString() );

	  TRegexp re_first("^ +");
	  FileName(re_first) = "";
	  TRegexp re_last("  *$");
	  FileName(re_last) = "";


	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;
	  AutoOpen( FileName.Data(), inputtype );
          break;  // Cavanaugh 03.04.2005
	}
       // Begin Cavanaugh 03.04.2005 ------------------------
      case kM_BUTTON_REFNAME: 
	{
          TString RefName  = TString( fRefNameBuffer->GetString() );

	  TRegexp re_first("^ +");
          RefName(re_first)  = ""; 
	  TRegexp re_last("  *$");
          RefName(re_last)  = ""; 

	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;
          REF_FLAG = 1;
	  TObject* InputObject = AutoOpen( RefName.Data(), inputtype );
          if(InputObject) AddRefTreeItemsOfInput( InputObject );
          REF_FLAG = 0;

          //RefFile = (TFile*)AutoOpen( RefName.Data(), inputtype ); 
          break;
	}
       // End Cavanaugh 03.04.2005 ------------------------

	break;


      case kM_BUTTON_CHOOSEFILE:
        if( (fRaButMFile->GetState() == kButtonDown) ||
            (fRaButRFile->GetState() == kButtonDown) ) {

	  const char *file = FileDialogWindow(); 
	  if ( file ) { 

	    fFileNameBuffer->Clear();

	    // Older version of  ROOT.
	    /*
	    char s[80] = "";
	    strcat(s, gSystem->WorkingDirectory()); 
	    strcat(s, "/"); 
	    strcat(s, file); 

	    fFileNameBuffer->AddText(0, s);
	    */

	    fFileNameBuffer->AddText(0, file);

	    gClient->NeedRedraw(fFileNameTextEntry);

	    //T.A
	    delete [] file;
	  } //if ( file )

	} //if( (fRaButMFile->GetState() == kButtonDown) ||
	else if( fRaButSocket->GetState() == kButtonDown ) { 

	  OpenConsumerListBox();

	}//if( fRaButSocket->GetState() == kButtonDown )
	else if( fRaButSOAP->GetState() == kButtonDown ) {

          OpenConsumerListBox();

        }//if( fRaButSOAP->GetState() == kButtonDown )

        break;
            
      case kM_BUTTON_CLEARFILENAME:

        FileNameClear();

        break;

      case kM_BUTTON_PAUSE:

	if ( Running ) {
	  fPauseHistoButton->SetState( kButtonDisabled );
	  fRestartHistoButton->SetState( kButtonUp );
	  Running = kFALSE;
	} //if ( Running )



	break;
      case kM_BUTTON_CLEAR:

        ClearCurrentPad();
	break;

      case kM_BUTTON_PRINT:

	if ( gPad || !( gROOT->GetListOfCanvases()->IsEmpty() ) ) {
	  //PrintCanvas( gPad->GetCanvas() );
	  PrintCanvas( (TPad*)gPad );
	}//if ( gPad || !( gROOT->GetListOfCanvases()->IsEmpty() ) ) 
	  
	break;

      case kM_BUTTON_RESTART:

	if ( fSelectedItem ) {


	  /*
	  //-- AutoReconnect
  
	  if ( gReconnectFlag ) {
	    AutoReconnect();
	    gReconnectFlag = false;
	  } //if ( gReconnectFlag )

	  //--
	  */



	  //if ( strstrn( fSelectedItem->GetText(), kSlideShowFolder ) ) {
	  if ( !strcmp( fSelectedItem->GetText(), kSlideShowFolder ) &&
	       fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	    if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		   ( gPad &&
		     !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		 && fSelectedItem->IsOpen() )
	      OpenDefaultCanvas();

	    StartSlides( fSelectedItem );

	  } //if ( strstr( fSelectedItem->GetText() ) ||
	  //else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) ) {
	  else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) &&
		    fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	    PopUpSlides( fSelectedItem,
			 kWarningCanvasName,
			 kWarningCanvasTitle );

	  } // else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) )
	  else {


	    if ( fSlideTimer && 
		 ( fSlideTimer->GetCurrentPad() == gPad ) &&
		 !fSlideTimer->HasTimedOut() )
	      //if ( fSlideTimer )
	      fSlideTimer->TurnOff();


	    if ( AutoUpdate ) {
	      fPauseHistoButton->SetState( kButtonUp );
	      fRestartHistoButton->SetState( kButtonDisabled );
	      UpdateTimerOn();
	    } // if ( AutoUpdate ) {
	    else {
	      UpdateCanvas();
	    } //else //if ( AutoUpdate ) {

	  } //else // if ( strstr( fSelectedItem->GetText(), "slide" ) ||

	} //if( fSelectedItem )  
	else {
	  std::cout << "No item in List Tree is selected. " << std::endl;
	} //else //if( fSelectedItem )

	break;
      case kM_BUTTON_ZERO:
        { //Cavanaugh 22.04.2005
	  //gpad = (TPad*)gPad;
	  //AddSubtractHisto(gpad, 1);

	  //UpdateCanvas();
          TString DynRefName  = TString( fDynRefNameBuffer->GetString() );

	  TRegexp re_first("^ +");
          DynRefName(re_first)  = ""; 
	  TRegexp re_last("  *$");
          DynRefName(re_last)  = ""; 

	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;
          REF_FLAG = 1;
	  TObject* InputObject = AutoOpen( DynRefName.Data(), kTSocket );
          if(InputObject) AddRefTreeItemsOfInput( InputObject );
          gpad = (TPad*) gPad;
          PadName = gpad->GetTitle();
          gpad->SetTitle("DYNAMIC RULE/REFERENCE");
          //gpad->Draw();
          //gpad->SetTitle(PadName);
          //DisplayRefHisto(gpad);
          //((TCanvas*) gpad)->Draw();
          UpdateCanvas();
          REF_FLAG = 0;

          //RefFile = (TFile*)AutoOpen( RefName.Data(), inputtype ); 
          break;


	}

	break;
      case kM_BUTTON_UNZERO:
	{
	  TString FileName = TString( fFileNameBuffer->GetString() );

	  TRegexp re_first("^ +");
	  FileName(re_first) = "";
	  TRegexp re_last("  *$");
	  FileName(re_last) = "";


	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;
	  AutoOpen( FileName.Data(), inputtype );
          gpad = (TPad*) gPad; // Cavanaugh 21.04.2005
          gpad->SetTitle("ONLINE"); // Cavanaugh 21.04.2005
          //cout << "-------------------- Restore PadName = " << PadName << endl;
          UpdateCanvas(); // Cavanaugh 21.04.2005
          break;  // Cavanaugh 03.04.2005
	}

	//	gpad = (TPad*)gPad;
	//	AddSubtractHisto(gpad, 0);
	//	UpdateCanvas();

	break;
	// Begin Cavanaugh 03.04.2005
      case kM_BUTTON_REF:
	{
	//gpad = (TPad*)gPad;
	//DisplayRefHisto(gpad);
	//UpdateCanvas();  Do not update canvas...it overwrites the Reference histogram
          TString RefName  = TString( fRefNameBuffer->GetString() );

	  TRegexp re_first("^ +");
          RefName(re_first)  = ""; 
	  TRegexp re_last("  *$");
          RefName(re_last)  = ""; 

	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;
          REF_FLAG = 1;
	  TObject* InputObject = AutoOpen( RefName.Data(), inputtype );
          if(InputObject) AddRefTreeItemsOfInput( InputObject );
          gpad = (TPad*) gPad;
          PadName = gpad->GetTitle();
          gpad->SetTitle("REFERENCE");
          //gpad->Draw();
          //gpad->SetTitle(PadName);
          //DisplayRefHisto(gpad);
          UpdateCanvas();
          REF_FLAG = 0;

          //RefFile = (TFile*)AutoOpen( RefName.Data(), inputtype ); 
          break;
	}
	break;
	// End Cavanaugh 03.04.2005
      case kM_BUTTON_CONTENT:
	/*
	if ( fSelectedItem ) {
	  TObject *obj = 
	    fDisplayedList->FindObject( fSelectedItem->GetText() );
	  if ( obj && obj->InheritsFrom(CHistogram::Class()) ) {
	    TH1 *histo =
	      (TH1*) ( ((CHistogram*)obj)->SubtractedHisto() );
	    //TH1 *histo =  ( (CHistogram*)obj )->GetHistoDrawn();

	    DrawContentHisto( histo );
	    delete histo;
	  }
	}
	*/

	gpad = (TPad*)gPad;
	if ( gpad ) {

	  Int_t counthisto = 0;
	  TH1 *histo = HistoInPad( gpad , counthisto );
	  if ( counthisto == 1 ) DrawContentHisto( histo ); 
	  else if ( counthisto >= 2 )
	    std::cout << " Select one histo on canvas. " << std::endl;

	} //if ( gpad )


	break;
      case kM_BUTTON_STOP:
	/*
	fPauseHistoButton->SetState(kButtonDisabled);
	fRestartHistoButton->SetState(kButtonDisabled);
	fZeroHistoButton->SetState(kButtonDisabled);
	fUnZeroHistoButton->SetState(kButtonDisabled);
	fContentHistoButton->SetState(kButtonDisabled);
	fStopHistoButton->SetState(kButtonDisabled);
	*/

	fReadFileButton->SetState(kButtonUp);

	Running = kFALSE;

	fSelectedItem = 0;

	ResetObjects();

	// Delete and Remove files, socket, and lists, ListTrees --
	if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) ) {
	  TCanvas *CurrentCanvas = 0;
	  CurrentCanvas = gPad->GetCanvas();
	  if ( CurrentCanvas ) {
	    //CurrentCanvas->Clear();
	    RecursivePadClear( CurrentCanvas );
	    CurrentCanvas->cd();
	    CurrentCanvas->Modified();
	    CurrentCanvas->Update();
	  } //if ( CurrentCanvas )
	} //if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) )


	//--

	break;
	/*
      case kM_BUTTON_EXIT:
	Running = kFALSE;

	ResetObjects();

	CloseWindow();
	break;
	*/

      } //switch parm1
      break; //kCM_BUTTON

    case kCM_CHECKBUTTON:
      switch ( parm1 ) {
      case kM_CHBUTTON_AUTOUPDATE:
	if ( fChButtonAutoUpdate->GetState() == kButtonDown ) {
	  AutoUpdate = kTRUE;

	  fChButtonSlideUpdate->SetState(kButtonUp);
	}
	else {
	  AutoUpdate = kFALSE;
	  fPauseHistoButton->SetState(kButtonDisabled);
	  fRestartHistoButton->SetState(kButtonUp);
	  Running = kFALSE;

	  fChButtonSlideUpdate->SetState(kButtonUp);
	  fChButtonSlideUpdate->SetState(kButtonDisabled);
	  SlideShowUpdate = kFALSE;
	}
	break;

      case kM_CHBUTTON_SLIDEUPDATE:
	if ( fChButtonSlideUpdate->GetState() == kButtonDown ) 
	  SlideShowUpdate = kTRUE;
	else 
	  SlideShowUpdate = kFALSE;

	break;
      } // switch parm1 of Check Button

      break; //kCM_CHECKBUTTON

    case kCM_RADIOBUTTON:
      switch ( parm1 ) {
      case kM_RABUTTON_MFILE:
	fRaButMFile->SetState(kButtonDown);
	fRaButRFile->SetState(kButtonUp);
	fRaButSocket->SetState(kButtonUp);
	fRaButSOAP->SetState(kButtonUp);
	break; // kM_RABUTTON_MFILE
      case kM_RABUTTON_RFILE:
	fRaButMFile->SetState(kButtonUp);
	fRaButRFile->SetState(kButtonDown);
	fRaButSocket->SetState(kButtonUp);
	fRaButSOAP->SetState(kButtonUp);
	break; // kM_RABUTTON_RFILE
      case kM_RABUTTON_SOCKET:
	fRaButMFile->SetState(kButtonUp);
	fRaButRFile->SetState(kButtonUp);	
	fRaButSocket->SetState(kButtonDown);
	fRaButSOAP->SetState(kButtonUp);
	break; // kM_RABUTTON_SOCKET
      case kM_RABUTTON_SOAP:
        fRaButMFile->SetState(kButtonUp);
        fRaButRFile->SetState(kButtonUp);
        fRaButSocket->SetState(kButtonUp);
        fRaButSOAP->SetState(kButtonDown);
	break; // kM_RABUTTON_SOAP
      } // switch parm1 of Radio Button

      break; //kCM_RADIOBUTTON


    } //switch submsg of kC_COMMAND
    break; // kC_COMMAND

  case kC_LISTTREE:
    switch ( GET_SUBMSG(msg) ) {

    case kCT_ITEMCLICK:
      if ( parm1 == kButton1 || parm1 == kButton2 ) {
	TGListTreeItem *item;
	if ( (item = fHistoListTree->GetSelected()) !=0 &&
	    item != fSelectedItem) {
	  fClient->NeedRedraw(fHistoListTree);
	  fSelectedItem = item;
	}
      } //if parm1 of kCT_ITEMCLICK

      else if ( parm1 == kButton3 ) {
	TGListTreeItem *item;
	if ( (item = fHistoListTree->GetSelected()) !=0 ) {
	  fClient->NeedRedraw(fHistoListTree);
	  fSelectedItem = item;


	  if ( fSelectedItem ) {


	    //if ( strstrn( fSelectedItem->GetText(), kSlideShowFolder ) ) {
	    if ( !strcmp( fSelectedItem->GetText(), kSlideShowFolder ) &&
		 fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	      if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		     ( gPad &&
		       !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		   && fSelectedItem->IsOpen() )
		OpenDefaultCanvas();

	      StartSlides( fSelectedItem );

	    } //if ( strstr( fSelectedItem->GetText() ) ||

	    //else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) ) {
	    else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) &&
		      fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	      PopUpSlides( fSelectedItem,
			   kWarningCanvasName,
			   kWarningCanvasTitle );

	    } // else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) )
	    else {

	      if ( fSlideTimer && 
		   ( fSlideTimer->GetCurrentPad() == gPad ) &&
		   !fSlideTimer->HasTimedOut() )
		//if ( fSlideTimer )
		fSlideTimer->TurnOff();


	      if ( fSelectedItem->GetFirstChild() &&
		   fSelectedItem->GetFirstChild()->GetNextSibling() &&
		   fSelectedItem->IsOpen() ) {

		TList *selectedList = new TList();

		TGListTreeItem *item2;
		item2 = fSelectedItem->GetFirstChild();

		while ( item2 ) {
		  //TObject *obj2 = 0;
		  //obj2 = fDisplayedList->FindObject( item2->GetText() );

		  TObject *obj2 = GetCObjFromListItem( item2 );

		  if( obj2 ) {
		    selectedList->Add( obj2 );
		    //obj2->ls();
		  }//if( obj2 )
		  item2 = item2->GetNextSibling();
		}// while( item2 )

		if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		       ( gPad && !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		     && !selectedList->IsEmpty() )
		  OpenDefaultCanvas();

		DrawHisto( selectedList );
		delete selectedList;

	      } //if ( selectedItem->GetFirstChild() &&
	      else {

		//TObject *obj =
		//fDisplayedList->FindObject( fSelectedItem->GetText() );

		TObject *obj = GetCObjFromListItem( item );


		if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		       ( gPad && !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		     && obj )
		  OpenDefaultCanvas();

		DrawHisto( obj );


	      } //else //if ( selectedItem->GetFirstChild() &&


	      if ( AutoUpdate ) {
		fPauseHistoButton->SetState( kButtonUp );
		fRestartHistoButton->SetState( kButtonDisabled );
	      
		UpdateTimerOn();

	      } //if (AutoUpdate)
	      else {

		//if ( !Running ) {
		  fPauseHistoButton->SetState( kButtonDisabled );
		  fRestartHistoButton->SetState( kButtonUp );
		  //} //if ( !Running ) {

	      } //else //if (AutoUpdate)


	    } //else //if ( strstr( fSelectedItem->GetText() ) ||



	  } //if ( fSelectedItem )


	} //if ( (item = fHistoListTree->GetSelected()) !=0 )
      } // else if parm1 == kButton3 of kCT_ITEMCLICK
      break; // kCT_ITEMCLICK


    case kCT_ITEMDBLCLICK:
      //if ( parm1 == kButton1 ) {
      if ( parm1 ) {
	TGListTreeItem *item;
	if ( (item = fHistoListTree->GetSelected()) !=0 ) {
	  fClient->NeedRedraw(fHistoListTree);
	  fSelectedItem = item;

	  if ( fSelectedItem ) {

	    /*
	  //-- AutoReconnect
  
	  if ( gReconnectFlag ) {
	    AutoReconnect();
	    gReconnectFlag = false;
	  } //if ( gReconnectFlag )

	  //--
	  */

	    /*

	    //TObject *obj =
	    //fDisplayedList->FindObject( fSelectedItem->GetText() );

	    TObject *obj = GetCObjFromListItem( item );

	    if ( !obj ) {
 
	      //if ( strstrn( fSelectedItem->GetText(), kSlideShowFolder ) ) {
	      if ( !strcmp( fSelectedItem->GetText(), kSlideShowFolder ) ) {

		StartSlides( fSelectedItem );

	      } // if ( ( strstr( fSelectedItem->GetText(), "slide" ) ||

	    } //if ( !obj )  

	    else {

	      if ( fSlideTimer &&
		   ( fSlideTimer->GetCurrentPad() == gPad ) &&
		   !fSlideTimer->HasTimedOut() )
		//if ( fSlideTimer )
		fSlideTimer->TurnOff();

	      DrawHisto( obj );

	      if ( AutoUpdate ) {
		fPauseHistoButton->SetState( kButtonUp );
		fRestartHistoButton->SetState( kButtonDisabled );

		UpdateTimerOn();

	      } //if ( AutoUpdate )
	      else {

		//if ( !Running ) {
		  fPauseHistoButton->SetState( kButtonDisabled );
		  fRestartHistoButton->SetState( kButtonUp );
		  //} //if ( !Running ) {

	      } //else //if (AutoUpdate)


	    } //else //if ( !obj )
	    */



	    //if ( strstrn( fSelectedItem->GetText(), kSlideShowFolder ) ) {
	    if ( !strcmp( fSelectedItem->GetText(), kSlideShowFolder ) &&
		 fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	      if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		     ( gPad && !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		   && fSelectedItem->IsOpen() )
		OpenDefaultCanvas();

	      StartSlides( fSelectedItem );

	    } //if ( strstr( fSelectedItem->GetText() ) ||

	    //else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) ) {
	    else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) &&
		      fSelectedItem->IsOpen() && fSelectedItem->GetFirstChild() ) {

	      PopUpSlides( fSelectedItem,
			   kWarningCanvasName,
			   kWarningCanvasTitle );

	    } // else if ( !strcmp( fSelectedItem->GetText(), kWarningFolder ) )

	    else {

	      if ( fSlideTimer && 
		   ( fSlideTimer->GetCurrentPad() == gPad ) &&
		   !fSlideTimer->HasTimedOut() )
		//if ( fSlideTimer )
		fSlideTimer->TurnOff();


	      if ( fSelectedItem->GetFirstChild() &&
		   fSelectedItem->GetFirstChild()->GetNextSibling()
		   //&& fSelectedItem->IsOpen()
		   ) {

		/*

		TList *selectedList = new TList();

		TGListTreeItem *item2;
		item2 = fSelectedItem->GetFirstChild();

		while ( item2 ) {
		  //TObject *obj2 = 0;
		  //obj2 = fDisplayedList->FindObject( item2->GetText() );

		  TObject *obj2 = GetCObjFromListItem( item2 );

		  if( obj2 ) {
		    selectedList->Add( obj2 );
		    //obj2->ls();
		  }//if( obj2 )
		  item2 = item2->GetNextSibling();
		}// while( item2 )

		DrawHisto( selectedList );
		delete selectedList;

		*/


	      } //if ( selectedItem->GetFirstChild() &&
	      else {


		//TObject *obj =
		//fDisplayedList->FindObject( fSelectedItem->GetText() );

		TObject *obj = GetCObjFromListItem( item );

		if ( ( !gPad || gROOT->GetListOfCanvases()->IsEmpty() ||
		       ( gPad &&
			 !strcmp( gPad->GetCanvas()->GetName(), kWarningCanvasName ) ) )
		     && obj )
		  OpenDefaultCanvas();

		DrawHisto( obj );

	      } //else //if ( selectedItem->GetFirstChild() &&


	      if ( AutoUpdate ) {
		fPauseHistoButton->SetState( kButtonUp );
		fRestartHistoButton->SetState( kButtonDisabled );
	      
		UpdateTimerOn();

	      } //if (AutoUpdate)
	      else {

		//if ( !Running ) {
		  fPauseHistoButton->SetState( kButtonDisabled );
		  fRestartHistoButton->SetState( kButtonUp );
		  //} //if ( !Running ) {

	      } //else //if (AutoUpdate)



	    } //else //if ( strstr( fSelectedItem->GetText() ) ||



	  } //if ( fSelectedItem )


	} //if ( (item = fHistoListTree->GetSelected()) !=0 )
      } //if ( parm1 == kButton1 ) of kCT_ITEMDBLCLICK
      break; // kCT_ITEMDBLCLICK

    } // switch submsg of kC_LISTTREE
    break; //case kC_LISTTREE

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {

    case kTE_ENTER:

      switch ( parm1 ) {

      case kM_TEXTENTRY_FILENAME:

	{
	  TString FileName = TString( fFileNameBuffer->GetString() );

	  TRegexp re_first("^ +");
	  FileName(re_first) = "";
	  TRegexp re_last("  *$");
	  FileName(re_last) = "";


	  EInputTypes inputtype;

	  if ( fRaButMFile->GetState() == kButtonDown )
	    inputtype = kTMapFile;
	  else if ( fRaButRFile->GetState() == kButtonDown )
	    inputtype = kTFile;
	  else if ( fRaButSocket->GetState() == kButtonDown )
	    inputtype = kTSocket;
          else if ( fRaButSOAP->GetState() == kButtonDown )
            inputtype = kTSOAP;

	  AutoOpen( FileName.Data(), inputtype );
	}
	break;//  kM_TEXTENTRY_FILENAME
    
      }//switch ( parm1 )

      break; //case kTE_ENTER:

    default:
      break;

    }//switch (GET_SUBMSG(msg)) 

    break; //case kC_TEXTENTRY:

  default:
    break;

  } //switch ( GET_MSG(msg) )

  return kTRUE;

}




//******************** Modified by Koji (1999/12/2) ************************

PhotoWindow::PhotoWindow(const TGWindow *p, const TGWindow *main, UInt_t w,
                         UInt_t h, UInt_t options)
  :TGTransientFrame(p, main, w, h, options)
{
    gClient->GetColorByName("Cyan",Cyan);
    ChangeBackground(Cyan);

 fTitleFrame = new TGCompositeFrame(this,800,100);
 FontStruct_t labelfont0, labelfont1;
 labelfont0 = gClient->GetFontByName("-adobe-helvetica-medium-r-*-*-18-*-*-*-*-*-iso8859-1");
   GCValues_t   gval0, gval1, gval2;
   gval0.fMask = kGCForeground | kGCFont;
   gval0.fFont = gVirtualX->GetFontHandle(labelfont0);
   gClient->GetColorByName("red", gval0.fForeground);
   fTextGC0 = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval0);
   fTitleLabel = new TGLabel(fTitleFrame, "Online Consumer Team for CDF Run II", fTextGC0, labelfont0);
   fTitleLabel->ChangeBackground(Cyan);
   fTitleFrame->AddFrame(fTitleLabel, new TGLayoutHints(kLHintsTop | kLHintsCenterX));

 fPicBut0 = new TGPictureButton(this, gClient->GetPicture("${CONSUMER_ICONS}/photo.xpm"), 1);

 fPicBut1 = new TGPictureButton(this, gClient->GetPicture("${CONSUMER_ICONS}/ta.xpm"), 2);

 fButton = new TGTextButton(this, "OK", 3);
 fButton->Associate(this);
 // fButton->SetCommand(".q" );
 gClient->GetColorByName("LightYellow",LightYellow);
 fButton->ChangeBackground(LightYellow);


   labelfont1 = gClient->GetFontByName("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
   gval1.fMask = kGCForeground | kGCFont;
   gval1.fFont = gVirtualX->GetFontHandle(labelfont1);
   gClient->GetColorByName("blue", gval1.fForeground);
   fTextGC1 = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval1);

   gval2.fMask = kGCForeground | kGCFont;
   gval2.fFont = gVirtualX->GetFontHandle(labelfont1);
   gClient->GetColorByName("magenta", gval2.fForeground);
   fTextGC2 = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval2);

 fTextFrame0 = new TGCompositeFrame(this,100,50);
 fTextFrame1 = new TGCompositeFrame(this,100,50);
 fTextFrame2 = new TGCompositeFrame(this,100,50);

 fTextLabel0 = new TGLabel(fTextFrame0, "H. Wenzel", fTextGC1, labelfont1);
 fTextLabel1 = new TGLabel(fTextFrame0, "IEKP Karlsruhe", fTextGC2, labelfont1);
 fTextLabel2 = new TGLabel(fTextFrame1, "K. Maeshima                   K. Ikado        ", fTextGC1, labelfont1);
 fTextLabel3 = new TGLabel(fTextFrame1, "     Fermilab                 Waseda Univ.     ", fTextGC2, labelfont1);
 //fTextLabel4 = new TGLabel(fTextFrame2, "F. Hartmann                T. Arisawa                 G. Veramendi", fTextGC1, labelfont1);
 fTextLabel4 = new TGLabel(fTextFrame2, "H.Stadie,W.Wagner         T. Arisawa                 G. Veramendi", fTextGC1, labelfont1);
 fTextLabel41 = new TGLabel(fTextFrame2, " F.Hartmann", fTextGC1, labelfont1);
 fTextLabel5 = new TGLabel(fTextFrame2, "IEKP Karlsruhe              Waseda Univ.                   LBNL        ", fTextGC2, labelfont1);

 fTextLabel0->ChangeBackground(Cyan);
 fTextLabel1->ChangeBackground(Cyan);
 fTextLabel2->ChangeBackground(Cyan);
 fTextLabel3->ChangeBackground(Cyan);
 fTextLabel4->ChangeBackground(Cyan);
 fTextLabel41->ChangeBackground(Cyan);
 fTextLabel5->ChangeBackground(Cyan);

 fTextFrame0->AddFrame(fTextLabel0,new TGLayoutHints(kLHintsCenterX) );
 fTextFrame0->AddFrame(fTextLabel1,new TGLayoutHints(kLHintsCenterX) );
 fTextFrame1->AddFrame(fTextLabel2,new TGLayoutHints(kLHintsCenterX));
 fTextFrame1->AddFrame(fTextLabel3,new TGLayoutHints(kLHintsCenterX));
 fTextFrame2->AddFrame(fTextLabel4,new TGLayoutHints(kLHintsCenterX));
 fTextFrame2->AddFrame(fTextLabel41,new TGLayoutHints(kLHintsLeft));
 fTextFrame2->AddFrame(fTextLabel5,new TGLayoutHints(kLHintsCenterX));

 fTitleFrame->ChangeBackground(Cyan);
 fTextFrame0->ChangeBackground(Cyan);
 fTextFrame1->ChangeBackground(Cyan);
 fTextFrame2->ChangeBackground(Cyan);

    AddFrame(fTitleFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX,10,10,10,10));
    AddFrame(fPicBut0, new TGLayoutHints(kLHintsTop | kLHintsCenterX,10,10,0));
    AddFrame(fTextFrame0, new TGLayoutHints(kLHintsTop | kLHintsCenterX));
    AddFrame(fTextFrame1, new TGLayoutHints(kLHintsTop | kLHintsCenterX));
    AddFrame(fPicBut1, new TGLayoutHints(kLHintsTop | kLHintsCenterX,0,0,10));
    AddFrame(fTextFrame2, new TGLayoutHints(kLHintsTop | kLHintsCenterX));
    AddFrame(fButton, new TGLayoutHints(kLHintsBottom | kLHintsCenterX));

    MapSubwindows();
    Layout();
    SetWindowName("Online Consumer Team");
    MapWindow();

}
 
PhotoWindow::~PhotoWindow()
{
  delete fButton;  delete fPicBut0;  delete fPicBut1;
  delete fTitleLabel;  delete fTextLabel0;  delete fTextLabel1;
  delete fTextLabel2;  delete fTextLabel4;  delete fTextLabel5;
  delete fTextLabel41;
  delete fTitleFrame;  delete fTextFrame0;  delete fTextFrame1;
  delete fTextFrame2;
}

void PhotoWindow::ClosePhotoWindow()
{
  delete this;
}

Bool_t PhotoWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
  switch ( GET_MSG(msg) ) {
  case kC_COMMAND:
    switch ( GET_SUBMSG(msg) ) {
    case kCM_BUTTON:
      switch ( parm1 ) {
      case 3:
	{
	  ClosePhotoWindow();
	}
	break; //case 3:
      default:
	break;
      } //switch ( parm1 ) 
      break; //case kCM_BUTTON:
    default:
      break;
    } // switch ( GET_SUBMSG(msg) ) 
    break; //case kC_COMMAND:
  default:
    break;
  } //switch ( GET_MSG(msg) )
  return kTRUE;
}

//***********************************************************************
// Cavanaugh 03.04.2005

//void HistoDisplay::AddTreeItemsOfInput( TObject *inputobj ) 
void HistoDisplay::AddRefTreeItemsOfInput( TObject *inputobj
					, Bool_t newlyOpened ) 
{
  //std::cout << " In AddRefTreeItemsOfInput " << std::endl;

  // Add TGListTreeItem of a inputobj to the TGListTree 
  // based on the current TConsumerInfo in the inputobj. 

  if ( !inputobj ) {
    std::cout << " No input is specified. " << std::endl;
    return;
  } //if ( !inputobj )
 
  TPad *padsav = 0;
  if ( gPad && !( gROOT->GetListOfCanvases()->IsEmpty() ) )
    padsav = (TPad*) gPad;

  TConsumerInfo *info = MakeConsumerInfo( inputobj );

  if ( info ) {


    //AddTreeItems( info, inputobj );
    AddRefTreeItems( info, inputobj, newlyOpened );
    SubtractRefTreeItems( info );
    DeleteEmptyRefFolders( fHistoListTree->GetFirstItem() );

    AddConsumerInfo( info );



  } //if( info )
  else {

    MakeRefHistoList(inputobj);

  } //else //if( info )


  Bool_t FoundWarningFolder = kFALSE;

  //SearchWarningErrorFolder
  //( fHistoListTree->GetFirstItem(), inputobj );
  SearchWarningFolder
    ( fHistoListTree->GetFirstItem(), inputobj, FoundWarningFolder );

  if ( !FoundWarningFolder ) {
    TObject *obj;
    if ( ( obj = 
	   gROOT->GetListOfCanvases()
	   ->FindObject( kWarningCanvasName ) ) ) {
      TCanvas *canvas = (TCanvas*)obj;
      canvas->Close();
      std::cout 
	<< std::endl
	<< "\"" << kWarningCanvasTitle << "\""
	<< " is closed because the warning disappeared."
	<< std::endl
	<< std::endl;
    } //if ( ( obj = 
  } //if ( !FoundWarningFolder )


  fClient->NeedRedraw( fHistoListTree );



  if ( padsav ) padsav->cd();

}

void HistoDisplay::MakeRefHistoList( TObject *inputobj )
{
  //std::cout << " In MakeHistoList " << std::endl;

  if ( ! inputobj ) return ;

  //if ( fHistoList && !( fHistoList->IsEmpty( )) ) fHistoList->Clear();

  if ( inputobj->IsA() == TMapFile::Class() ) 
  {
    TMapFile *MFile = (TMapFile*) inputobj;
    if( MFile )
      {
        TMapRec *mr = MFile->GetFirst();
        while( MFile->OrgAddress(mr) ) 
        {
          TString name = mr->GetName();
          AddToRefTree( name.Data(), MFile );
          mr = mr->GetNext();
        }
      }
  }
  else if ( inputobj->IsA() == TFile::Class() ) 
  {
    TFile *RFile = (TFile*) inputobj;
    if ( RFile )
      {
        TIter next(RFile->GetListOfKeys());
        TKey *key;
        while ( ( key = (TKey*)next() )  ) 
        {
          AddToRefTree( key->GetName(), RFile );

        }
      }
  }
  else if ( inputobj->IsA() == TSocket::Class() ) 
  {
    TSocket *sock = (TSocket*) inputobj;
    if ( sock ) 
      {
        std::cout << " No TConsumerInfo from the server: " 
                  << sock->GetName() << std::endl;
      }
  }
}

void HistoDisplay::
//AddTreeItems( TConsumerInfo *info, TObject *inputobj )
AddRefTreeItems( TConsumerInfo *info, TObject *inputobj, Bool_t newlyOpened )
{
  //std::cout << " In AddRefTreeItems " << std::endl;
  //
  // Add new TGListTreeItems to TGListTree *fHistoListTree
  // from TConsumerInfo *info.
  // If an object in TConsumerInfo is not in fHistoListTree,
  // corresponding item is added to it.
  // If an object in TConsumerInfo exists in fHistoListTree,
  // inputobj is set to an UserData of the item.
  //

  if ( !info ) return;

  TConsumerInfo::Iterator iter( info );

  while ( iter.is_valid() ) {

    TString pathname = MakePathName( iter.Path(), iter.Name() );

    TGListTreeItem *item
      = fHistoListTree->FindItemByPathname( pathname.Data() );

    if ( item ) {

      if ( newlyOpened ) {

	//std::cout << " Newly opened " << std::endl;

	item->SetUserData( inputobj );

	TObject *obj = fDisplayedList->FindObject( iter.Name() );
	if ( obj && obj->InheritsFrom( CObject::Class() ) ) {
	  CObject *cobj = (CObject*)obj;
	  cobj->SetInputSource( inputobj );
	} //if ( obj && obj->InheritsFrom( CObject::Class() ) )

      } //if ( newlyOpened )

    } //if ( item ) 
    else {
 
      //std::cout << " new item " << std::endl;

      //const char *tip = iter.Tip();
      //MakeTGListTree( pathname, inputobj, tip );
      MakeTGRefListTree( pathname, inputobj );
    } //else //if ( item ) 

    ++iter;

  } //while ( iter.is_valid() ) 

}

void HistoDisplay::AddToRefTree( const char *objname, TObject *inputobj )
{

      char *filename;
      //filename = inputobj->GetName();

      char *tmpname; 
      if ( ( tmpname = strrchr( inputobj->GetName() , '/' ) ) )
	filename = tmpname;
      else
	strcpy( filename, inputobj->GetName() );

      /*
      TString PathName =
	"/" + TString( filename ) + 
	"/" + TString( objname );
      */

      const char *dir = filename; 

      TString PathName =
	MakePathName( dir, objname );


      MakeTGRefListTree( PathName, inputobj );
      //MakeTGListTree( PathName, inputobj, obj->GetTitle() );


      //fClient->NeedRedraw(fHistoListTree);

}


void HistoDisplay::MakeTGRefListTree( const TString &PathName, 
				   //TObject *inputobj )
				   TObject *inputobj,
				   const char* tip )
{
  std::cout << " In MakeTGRefListTree : " ;


  while ( PathName.Contains("//") ) {
    TRegexp re("//+");
    PathName(re) = "/";
  }

  //std::cout << " PathName = " << PathName.Data() << " : " << endl;

  const int treemax = 10;

  int index[treemax];

  int PathLength = PathName.Length();

  int ii;
  int ind;
  for ( ii=0, ind = PathName.Index("/",1,0,TString::kExact) ;
	0 <= ind && ind <= PathLength ; ++ii ) {
    index[ii] = ind;
    ind = PathName.Index("/",1,ind+1,TString::kExact);
  }

  int nindex = ii;
  index[nindex] = PathLength;

  TString DirName;
  TString HistoName;

  TString CurPathName;
  TString CurParentName;
  TString CurItemName;


  TGListTreeItem *itemOrg= 0;
  itemOrg = fHistoListTree->FindItemByPathname( PathName.Data() );

  //if ( !fHistoListTree
  //->FindItemByPathname( PathName.Data() ) ) {

  if ( !itemOrg ) 
    {

    if ( 1 < nindex ) 
      {

      DirName = PathName( 0, index[nindex-1] );
      HistoName = PathName( index[nindex-1]+1, index[nindex] );

      if ( !fHistoListTree->FindItemByPathname( DirName.Data() ) ) 
	{

	  for ( int i=0 ; i < nindex-1 ; ++i ) 
	    {
	      CurPathName = DirName(0,index[i+1]);  //cout << " CurPathName  " << CurPathName << endl;
	      if ( !fHistoListTree->FindItemByPathname( CurPathName.Data() ) ) 
		{
		  if ( i==0 ) 
		    { 
		      CurItemName = DirName(index[i]+1,index[i+1]-index[i]-1);  //cout << " CurItemName  " << CurItemName << endl;
		      fHistoListTree->AddItem( 0, CurItemName.Data() );
		    } //if ( i==0 )
		  else 
		    {
		      CurParentName = DirName( 0, index[i]);
		      CurItemName = DirName(index[i]+1,index[i+1]-index[i]-1);  //cout << " CurItemName  " << CurItemName << endl;
		      fHistoListTree->AddItem( fHistoListTree->FindItemByPathname( CurParentName.Data() ),CurItemName.Data() );
		    } // else //if ( i==0 )
		} // if ( !fHistoListTree...
	    } //for ( int i=0 ; i < nindex-1 ; ++i )
	}//if ( !fHistoListTree...

      TGListTreeItem *item = 0; //cout << " :: " << DirName.Data() << " ::: " << HistoName.Data() << " :::: " ;
      item = fHistoListTree->AddItem( fHistoListTree->FindItemByPathname(DirName.Data()),HistoName.Data() );
      if ( item ) // item is added to fHistoListTree above
	{
	  item->SetUserData(inputobj);  
          TObject *obj = (TObject*) ( item->GetUserData() ); //cout << obj->GetName() << endl;
	  if ( tip ) fHistoListTree->SetToolTipItem( item, tip );
	} // if ( item )
    } //if ( 1 < nindex ) 
    else 
    {
      HistoName = PathName( index[nindex-1]+1, index[nindex] );
      TGListTreeItem *item = 0;
      item = fHistoListTree->AddItem( 0, HistoName.Data() );
      if ( item ) 
	{
	  item->SetUserData(inputobj);
	  if ( tip ) fHistoListTree->SetToolTipItem( item, tip );
	} // if ( item )
    } //else // if ( 1 < nindex )
  } //  if ( !itemOrg ) 
  else 
  {
    TObject *inputobjOrg = (TObject*) ( itemOrg->GetUserData() ); //cout << inputobjOrg->GetName() << endl;
    if ( !inputobj->IsEqual( inputobjOrg ) ) 
      {
	itemOrg->SetUserData(inputobj);
	if ( tip ) fHistoListTree->SetToolTipItem( itemOrg, tip );
      } //if ( !inputobj->IsEqual( inputobjOrg ) ) 
  } //else // if ( !itemOrg )
}




void HistoDisplay::
SubtractRefTreeItems( TConsumerInfo *info )
{
  //std::cout << " In SubtractTreeItems " << std::endl;
  //
  // Subtract TGListTreeItems from TGListTree *fHistoListTree
  // according to new TConsumerInfo *info.
  //
  // Items which do not exist in a new TConsumerInfo *info,
  // but did exist in an old TConsumerInfo "infoold"
  // of the same consumer name, 
  // then the items are deleted from the fHistoListTree.
  // 

  if ( !info ) return;

  TConsumerInfo *infoold = 
    GetConsumerInfoFromList( info->consumername() );

  if ( !infoold ) return;
  TConsumerInfo::Iterator iterold( infoold );



  while ( iterold.is_valid() ) {

    TConsumerInfo::Iterator iter( info );

    Bool_t found = kFALSE;
    Bool_t foundPlural = kFALSE;


    while ( iter.is_valid() ) {


      if ( !strcmp( iter.Name(), iterold.Name() ) ) {

	if ( !strcmp( iter.Path(), iterold.Path() ) ) {

	  found = kTRUE;
	  if ( foundPlural ) break;

	} //if ( !strcmp( iter.Path(), iterold.Path() ) ) 
	else {

	  foundPlural = kTRUE;
	  if ( found ) break;

	} //else //if ( !strcmp( iter.Path(), iterold.Path() ) ) 

      } //if ( !strcmp( iter.Name(), iterold.Name() ) ) 


      ++iter;

    } //while ( iter.is_valid() ) 


    if ( !found ) {

      TString pathname = 
	MakePathName( iterold.Path(), iterold.Name() );

      TGListTreeItem *item
	= fHistoListTree->FindItemByPathname( pathname.Data() );

      if ( item ) {
	
	TObject *obj = fDisplayedList->FindObject( item->GetText() );


	DeleteTreeItem( item );

	if ( obj && !foundPlural ) {
	  fDisplayedList->Remove( obj );
	  delete obj;
	} //if ( obj )

      } //if ( item ) 


    } //if ( !found ) 

    ++iterold;

  } //while ( iterold.is_valid() ) 


}

void HistoDisplay::
DeleteEmptyRefFolders( TGListTreeItem *item )
{
  //std::cout << " In DeleteEmptyFolders " << std::endl;

  // Recursively
  // delete empty TGListTree folder item
  // from the fHistoListTree.
  // The item is deleted as an empty folder if it
  // 1. does not have children.
  // 2. does not have userdata.

  while ( item ) {

    TGListTreeItem *firstchild = item->GetFirstChild();
    TGListTreeItem *sibling = item->GetNextSibling();

    if ( firstchild ) {

      DeleteEmptyRefFolders( firstchild );

      if ( !( item->GetFirstChild() ) ) {

	DeleteTreeItem( item );

      } //if ( !( item->GetFirstChild() ) )

    } //if ( firstchild )
    else {

      if ( !( item->GetUserData() ) ) {

	DeleteTreeItem( item );

      } //if ( !( item->GetUserData() ) )

    } //else //if ( firstchild )

    item = sibling;

  } //while ( item ) 

}

void HistoDisplay::DisplayRefHisto( TVirtualPad *currentPad) 
{

  if (!currentPad) return;
  //gDirectory->pwd(); 
  //RefFile->cd();
  //gDirectory->pwd();
  currentPad->cd();

  TList *primitiveList = currentPad->GetListOfPrimitives();
  //cout << currentPad->GetName() << endl; 
  TIter next(primitiveList);
  TObject *obj = 0;

  while ( ( obj = next() ) ) {
    if ( obj->InheritsFrom(TPad::Class()) ) {

      DisplayRefHisto( (TPad*)obj );

    } 
    else {

      if ( obj->InheritsFrom(TH1::Class()) ) {

	TObject *obj2 = 
	  fDisplayedList->FindObject(obj->GetName()); 

        char *dir;
        char *tmpname; 
	//        if ( ( tmpname = strrchr( RefFile->GetName() , '/' ) ) )
  	  dir = tmpname;
	  //        else
	  //	  strcpy( dir, RefFile->GetName() );
        const char *objname = obj->GetName();
        //gDirectory->pwd();

        //cout << "objname = " << objname <<  endl;
        //TGListTreeItem *item = fRefListTree->FindItemByPathname(objname);
        //if(item) TObject *myobj = GetCObjFromRefListItem( item );
        //if(myobj) cout << "RefListItem Name " << myobj->GetName() << endl;
        TH1F *h = (TH1F*) fDisplayedList->FindObject( objname ); 
        TCanvas *hcanvas = new TCanvas(objname,"Reference", 500, 500);
        //gPad->cd(); gPad->Clear();
        if(h) h->Draw();  //cout << "Drawing Ref Histo : " << h->GetName() << endl;                      
        //gPad->Modified(); gPad->Update();
      } //if ( obj->InheritsFrom(TH1::Class()) )


    } //else//if ( obj->InheritsFrom(TPad::Class()) )


  } //while ( obj = next() )

  //  RefFile->cd("temp");
}

CObject* HistoDisplay::
UpdateRefHistoList(const char* objPath, const char* objName, const TObject *inputobj )
{
  //std::cout << " In UpdateRefHistoList " << std::endl;
  //std::cout << " updating reference " << objName << std::endl;

  /*
  if ( !inputobj ) {
    std::cout << " Input Object is not set. " << std::endl; 
    return 0;
  }
  */

  //const char *path = gROOT->GetPath(); 
  TVirtualPad *padsav = 0;
  if ( gPad && !gROOT->GetListOfCanvases()->IsEmpty() ) padsav = gPad;



  TObject *obj = GetNewObj( objPath, objName, inputobj );
  CObject *cobj = 0; 

  if ( obj ) {

    TObject *objold = fDisplayedList->FindObject( objName );

    /*
    TObjLink *lnk = fDisplayedList->FirstLink();
    while (lnk) {
      TObject *objtmp = lnk->GetObject();
      if (objtmp && 
	  objtmp->GetName() && !strcmp(objName, objtmp->GetName())) {

	if ( objtmp->InheritsFrom( CObject::Class() ) ) {
	  CObject *cobjtmp = (CObject*)objtmp;
	  TObject *inputtmp = cobjtmp->GetInputSource();
	  if ( ( inputtmp && inputtmp->GetName() ) &&
	       ( inputobj && inputobj->GetName() ) &&
	       ( !strcmp( inputtmp->GetName(),
			  inputobj->GetName() ) ) ) {
	    //objold = objtmp;
	    cobj = cobjtmp;
	    cobj->SetTObject( obj );
	    if ( cobj->GetInputSource() != inputobj )
	      cobj->SetInputSource( inputobj );
	      
	  } //if ( ( inputtmp && inputtmp->GetName() ) &&


	}//if ( objtmp->InheritsFrom( CObject::Class() ) ) 


      } //if ( objtmp && objtmp->GetName() && ... )

      lnk = lnk->Next();
    } //while (lnk) 

    */


    if ( objold && objold->InheritsFrom( CObject::Class() ) ) {

      cobj = (CObject*) objold;

      if ( cobj ) {
	cobj->SetTObject( obj );
        cobj->SetPath(TString(objPath));
	//if ( cobj->GetInputSource() != inputobj )
	if ( inputobj != cobj->GetInputSource()  ) {
	  cobj->SetInputSource( inputobj );
	  //tmp
	  //std::cout << " InputObj is updated. " << std::endl;
	} // if ( inputobj != cobj->GetInputSource()  )  
      } // if ( cobj )

    } //if ( objold && objold->InheritsFrom( CObject::Class() ) )

    else {

      //if ( !cobj ) {

      if ( obj->InheritsFrom(TH1::Class()) ) {

	CHistogram *cobjnew =
	  new CHistogram( (TH1*)obj, inputobj, 0, 0 );        
	fDisplayedList->Add( cobjnew );

	cobj = cobjnew;
	
      } //if ( obj->InheritsFrom(TH1::Class()) )
      else {

	CObject *cobjnew =
	  new CObject( obj, inputobj );
	
	fDisplayedList->Add( cobjnew );

	cobj = cobjnew;

      } // else //if ( obj->InheritsFrom(TH1::Class()) )

      //} // if ( !cobj ) 
	cobj->SetPath(TString(objPath));
    } //else //if ( objold && objold->InheritsFrom( CObject::Class() ) )

    if ( obj->InheritsFrom( TPad::Class() ) )
      AddRefPadHistosToHistoList( (TPad*)obj );

  } //if ( obj )

  else {

    //tmp
    /*
    if ( objold && objold->InheritsFrom( CObject::Class() ) ) 
      cobj = (CObject*) objold;
    */
    //tmp
    //std::cout << " obj is not got from the input stream. " << std::endl;

  } //else //if(obj)

  //tmp
  if ( !cobj ) std::cout << " Not updated. " << std::endl;


  //gROOT->Cd( path );
  if ( padsav ) padsav->cd(); 


  return cobj;

}

void HistoDisplay::AddRefPadHistosToHistoList( TPad *pad )
{
  //std::cout << " In AddPadHistosToHistoList " << std::endl; 

  if ( !pad ) return;

  TObjLink *link = pad->GetListOfPrimitives()->FirstLink();

  while ( link ) {

    TObject *obj = link->GetObject();

    if ( obj ) {

      if ( obj->InheritsFrom( TPad::Class() ) ) 
	AddPadHistosToHistoList( (TPad*)obj ); 

      else if ( obj->InheritsFrom( TH1::Class() ) ) {

	TObject *objold = fDisplayedList->FindObject( obj->GetName() );

	if ( objold && objold->InheritsFrom( CObject::Class())  ) {

	  CObject *cobj = (CObject*)objold;
	  //cobj->SetTObject( obj );
	  cobj->SetTObject( obj->Clone() );

	} //if ( objold && objold->InheritsFrom( CObject::Class()) )
	else {

	  CHistogram *cobjnew =
	    //new CHistogram( (TH1*)obj, 0, 0, 0 );
	    new CHistogram( (TH1*)obj->Clone(), 0, 0, 0 );

	  fDisplayedList->Add( cobjnew );

	} //else //if ( objold && objold->InheritsFrom( CObject::Class()) ) )

      } //else if ( obj->InheritsFrom( TH1::Class() ) )

    } //if ( obj ) 

    link = link->Next();

  } //while ( link )

  //std::cout << " End of AddPadHistosToHistoList " << std::endl; 

}

CObject* HistoDisplay::GetCObjFromRefListItem( TGListTreeItem *item ) 
{
  //std::cout << " In GetCObjFromRefListItem " << std::endl;

  if ( !item ) return 0;

  CObject *cobj = 0;

  TObject *obj;

  obj = (TObject*)item->GetUserData(); 


  if( obj ) {

    if ( obj->InheritsFrom( CObject::Class() ) ) {

      //std::cout << "Ref: UserData is CObject" << std::endl;

      cobj = (CObject*)obj;

    }
    else {
      
      std::cout << "Ref: UserData is not CObject" << std::endl;

      if ( !item->GetFirstChild() ) {

	TObject *inputobj = obj;

	if ( inputobj ) {
          TString objname = item->GetText();
          TString objpath("");
          TGListTreeItem* iter_item = item;
          while (TGListTreeItem* parent = iter_item->GetParent()) {
//                std::cout << parent->GetText() << std::endl;
		objpath = parent->GetText() + TString("/") + objpath;
		iter_item = parent;
          }	  	
          // cobj =  UpdateHistoList( item->GetText(), inputobj );
	  cobj =  UpdateRefHistoList( objpath.Data(), objname.Data(), inputobj );

	  if ( !cobj ) 
	    std::cout <<  objname << " is not found." << std::endl;
	  else {
	    item->SetUserData( cobj );
	    if ( item )
	    fHistoListTree->SetToolTipItem( item, cobj->GetTitle() );
	  } //else //if ( !cobj )

	} //if ( inputobj )

      } //if ( !item->GetFirstChild() )

    } //else //if ( obj->InheritsFrom( CObject::Class() ) 


  } //if( obj )


  return cobj;

}


void HistoDisplay::MakeTGRefListTreeFrame()
{
  //cout << "Now in MakeTGRefListTreeFrame ..." << endl;
  fListTreeMainFrame =
    new TGCompositeFrame(fMainFrame,20,20,kVerticalFrame);
  fListTreeMainFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY
		       , 4, 4, 4, 4);



  // Make List Tree Frame --

  fRefListFrame =
    new TGCompositeFrame(fListTreeMainFrame, 150, 150, kHorizontalFrame);
  fRefListFrameLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY,
		       1, 1, 1, 1); 


  // Make List Tree --
  fTGCanvas =
    //new TGCanvas(fHistoListFrame, 210, 150, kSunkenFrame | kDoubleBorder);
    new TGCanvas(fRefListFrame, 110, 100, kSunkenFrame | kDoubleBorder);
  fTGCanvasLayout =
    new TGLayoutHints( kLHintsTop | kLHintsLeft |
		       kLHintsExpandX | kLHintsExpandY );

  fRefListTree = new TGListTree(fTGCanvas->GetViewPort()
				//, 210, 250, kHorizontalFrame);
				  , 20, 20, kHorizontalFrame);

  fRefListTree->Associate(this);
  fTGCanvas->SetContainer(fRefListTree);

  //tmp 2000/12/08
  fRefListTree->SetAutoTips();


  fRefListFrame->AddFrame(fTGCanvas,fTGCanvasLayout);
  //--


  // Button Frame of List Tree --
  fListButtonFrame =
    new TGCompositeFrame(fRefListFrame, 60, 80,
			 kVerticalFrame );
  fListButtonFrameLayout = 
    //new TGLayoutHints( kLHintsRight | kLHintsCenterY | kLHintsExpandX 
		       //, 2, 2, 5, 5);
    new TGLayoutHints( kLHintsRight | kLHintsCenterY
		       , 5, 5, 10, 0);

}
