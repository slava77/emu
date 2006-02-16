//
// 
// ConsumerListBox.cc
// Tetsuo Arisawa (Waseda Univ.), 2001/5/3
//
// Display list of consumers.
// Enable to choose consumers.
//
//

#include <iostream>
#include <string>

#include <TROOT.h>
#include <TApplication.h>

#include <TGFrame.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGListBox.h>

#include <TString.h>

#include "ConsumerList.hh"
#include "ConsumerListBox.hh"

#include "HistoDisplay.hh"


ConsumerListBox::
ConsumerListBox(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h)
  : TGTransientFrame( p, main, w, h )
{

  fConsumerList = 0;
  fHistoDisplay = 0;

  fListBox = 0;

}


void ConsumerListBox::OpenWindow()
{

  //std::cout << " In OpenWindow " << std::endl;

  fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsTop | 
			      kLHintsExpandX,
			      0,0,5,0);
  fLayoutLT = new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 0, 5);


  fLabelFrame = new TGCompositeFrame(this, 400, 20, kHorizontalFrame);
  fLabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop );

  fLabel1 = new TGLabel(fLabelFrame, "  Consumer  " );//12
  fLabel2 = new TGLabel(fLabelFrame, "                    Host:port     " );
  //35
  fLabel3 = new TGLabel(fLabelFrame, "       Run" );//10
  fLabel4 = new TGLabel(fLabelFrame, "    Events" );//10
  fLabel5 = new TGLabel(fLabelFrame, "    Status" );//10

  fLabelFrame->AddFrame(fLabel1, fLabelLayout);
  fLabelFrame->AddFrame(fLabel2, fLabelLayout);
  fLabelFrame->AddFrame(fLabel3, fLabelLayout);
  fLabelFrame->AddFrame(fLabel4, fLabelLayout);
  fLabelFrame->AddFrame(fLabel5, fLabelLayout);

  /*
  fLabel = 
    new TGLabel(fLabelFrame, 
  " Consumer   Host:port                Run       Events    Status    ");

  fLabelFrame->AddFrame(fLabel,fLabelLayout);
  */

  AddFrame(fLabelFrame, fLayout);



  fListBox = new TGListBox(this, 3);
  fListBoxLayout = new TGLayoutHints(kLHintsCenterX | kLHintsTop | 
				     kLHintsExpandX | kLHintsExpandY,
				     2,2,2,0);

  AddFrame(fListBox, fListBoxLayout);
  fListBox->Associate(this);
  fListBox->Resize(400,100);


  fListBox->SetMultipleSelections(kTRUE);

  /*
  //tmp--
  fListBox->
    AddEntry(" YMon       b0dap24.fnal.gov:9091    12256     3315      Running   ", -1);
  fListBox->
    AddEntry(" TrigMon    b0dap25.fnal.gov:9091    12256     3318      Running   ", -1);
  //--
  */

  /*
  fConsumerList = new ConsumerList("http://www-cdfonline.fnal.gov/consumer/consumer_status.html");

  UpdateListBox();
  */


  fButtonFrame = new TGCompositeFrame(this, 400, 10, kHorizontalFrame);

  fButton1 = new TGTextButton(fButtonFrame, "Update", 1);
  fButtonFrame->AddFrame(fButton1, fLayoutLT);
  fButton1->Associate(this);

  fButton2 = new TGTextButton(fButtonFrame, "Open", 2);
  fButtonFrame->AddFrame(fButton2, fLayoutLT);
  fButton2->Associate(this);

  AddFrame(fButtonFrame, fLayout);

  MapSubwindows();
  Layout();
  SetWindowName("Consumer Lists and status");
  MapWindow();


}


void ConsumerListBox::CloseWindow()
{
  //std::cout << " In CloseWindow " << std::endl;

  // Called when window is closed via the window manager.
  delete this;

  //gApplication->Terminate(0);

}



ConsumerListBox::~ConsumerListBox()
{
  //std::cout << " In ~ConsumerListBox " << std::endl;

  delete fButton1;
  delete fButton2;
  delete fButtonFrame;

  delete fListBox; 
  delete fListBoxLayout;

  delete fLabel1;
  delete fLabel2;
  delete fLabel3;
  delete fLabel4;
  delete fLabel5;
  //delete fLabel;

  delete fLabelLayout;
  delete fLabelFrame;

  delete fLayoutLT;
  delete fLayout;

}

Bool_t ConsumerListBox::
ProcessMessage( Long_t msg, Long_t parm1, Long_t parm2 )
{
  switch (GET_MSG(msg))
    {
    case kC_COMMAND:
      switch (GET_SUBMSG(msg))
	{
        case kCM_BUTTON:
	  //printf("text button id %ld pressed\n", parm1);
	  switch ( parm1 ) {
	  case 1:

	    UpdateListBox();

	    break; //case 1:
	  case 2:
	    //OpenSelectedEntry();
	    OpenSelectedEntries();
	    break; //case 2:
	  default:
	    break;
	  } //switch ( parm1 ) 

          break;
        case kCM_LISTBOX:
          //printf("item id %ld selected from listbox id %ld \n"
	  //, parm2, parm1);
          break;
        default:
          break;
	}//switch (GET_SUBMSG(msg))

    default:
      break;
    }//switch (GET_MSG(msg))

  return kTRUE;
}


void ConsumerListBox::SetConsumerList( const char *website )
{
  //std::cout << " In SetConsumerList " << std::endl;

  if ( fConsumerList ) delete fConsumerList;
  fConsumerList = new ConsumerList( website );
}

void ConsumerListBox::SetConsumerList( ConsumerList *consumerlist )
{
  //std::cout << " In SetConsumerList " << std::endl;
  if ( consumerlist ) {
    if ( fConsumerList ) delete fConsumerList;
    fConsumerList = consumerlist;
  } //if ( consumerlist ) 
}


void ConsumerListBox::SetHistoDisplay( HistoDisplay *histodisplay )
{
  //std::cout << " In SetHistoDisplay " << std::endl;

  fHistoDisplay = histodisplay;

}
 



void ConsumerListBox::UpdateListBox()
{
  //std::cout << " In UpdateListBox " << std::endl;

  if ( fConsumerList && fListBox ) {

  if ( fConsumerList->nEntries() > 0 ) 
    fListBox->RemoveEntries( 0, fConsumerList->nEntries() - 1 );

  fConsumerList->readWebPage();

  //tmp
  //fConsumerList->print( std::cout );


  UInt_t nent = fConsumerList->nEntries();

  if ( nent > 0 ) {
    for ( int i = 0; i < nent; i++ ) {

      char *entry = new char[ 12+30+6+10+10+10+1 ];
      char *tmp = new char[31];

      entry[0] = '\0';

      sprintf( tmp, " %-11s", fConsumerList->consumername(i).Data() );
      //strcat(entry, tmp);
      strncat(entry, tmp, 12);
      
      sprintf( tmp, "%30s", fConsumerList->hostname(i).Data() );
      //strcat(entry, tmp);
      strncat(entry, tmp, 30);

      sprintf( tmp, ":%-5d", fConsumerList->port(i) );
      //strcat(entry, tmp);
      strncat(entry, tmp, 6);

      sprintf( tmp, "%10d", fConsumerList->runnumber(i) );
      //strcat(entry, tmp);
      strncat(entry, tmp, 10);

      sprintf( tmp, "%10d", fConsumerList->nevents(i) );
      //strcat(entry, tmp);
      strncat(entry, tmp, 10);

      sprintf( tmp, "%10s", fConsumerList->statusString(i) );
      //strcat(entry, tmp);
      strncat(entry, tmp, 10);

      //std::cout << " entry(" << i << ") = \""  << entry << "\"" << std::endl;

      fListBox->AddEntry(entry, i);
      /*
      TGTextLBEntry *lbe =
	new TGTextLBEntry(fListBox->GetContainer(), new TGString(entry), i);
      TGLayoutHints *lhints = 
	new TGLayoutHints(kLHintsLeft | kLHintsTop );
      fListBox->AddEntry(lbe, lhints);
      //delete lbe; delete lhints; //done by delete ~TGListBox()
      */

      delete [] tmp;
      delete [] entry;

    } //for ( int i = 0; i < nent; i++ )

  } //if ( nent > 0 )


  fListBox->MapSubwindows();
  fListBox->Layout();

  } //if ( fConsumerList && fListBox )
  else
    std::cout << " No ConsumerList or ListBox." << std::endl;

}


void ConsumerListBox::OpenSelectedEntries() const
{
  //std::cout << " In OpenSelectedEntries " << std::endl;

  if ( !fListBox ) return;

  TList *selected = new TList();
  fListBox->GetSelectedEntries(selected);

  if ( !selected->IsEmpty() ) {

    TIter next( selected );

    TGLBEntry *lbentry;
    char *hostport;

    while ( lbentry = (TGLBEntry*)next() ) {

      Int_t id = lbentry->EntryId();
      hostport = HostPortFromID( id );

      if ( hostport ) {
	std::cout << " Opening hostport = \"" << hostport  << "\" ..." << std::endl;


	if ( fHistoDisplay ) {
	  TObject *inputobj = 
	    fHistoDisplay->AutoOpen( hostport, HistoDisplay::kTSocket );
	  if ( inputobj )
	    std::cout << " done. " << std::endl;
	  else
	    std::cout << " failed. " << std::endl; 
	} //if ( fHistoDisplay ) 
	else
	  std::cout << " stopped. No HistoDisplay " << std::endl;


	delete hostport;
      } // if ( hostport ) 

    } //while ( lbentry = next() )

    //selected->Delete(); //never use Delete
    selected->Clear();

  } //if ( !selected->IsEmpty() ) 

  if ( selected ) delete selected;

}


char* ConsumerListBox::HostPortFromID( const UInt_t id ) const
{
  //std::cout << " In HostPortFromID " << std::endl;

  /*
  if ( fListBox && !fListBox->GetSelection(id) ) {
    std::cout << " Not Selected " << std::endl;
    return 0;
  } //if ( !fListBox->GetSelection(id) )
  */

  if ( !fConsumerList ) {
    std::cout << " No connection to ConsumerList." << std::endl;
    return 0;
  } //if ( !fConsumerList )


  char *hostport = 
    new char[ strlen( fConsumerList->hostname(id) ) + 8 + 1 ];

  hostport[0] = '\0';

  strcat( hostport, fConsumerList->hostname(id) );

  char *tmp = new char[8];
  sprintf( tmp, ":%d", fConsumerList->port(id) );

  strcat( hostport, tmp);

  //std::cout << " selected host:port " << hostport << std::endl; 

  delete [] tmp;
  //delete [] hostport;

  /*
  std::cout << " The number of running consumer of the same name is "
       << NRunningConsumer( fConsumerList->consumername(id) )
       << std::endl;
  */

  return hostport;
}


char*  ConsumerListBox::
GetHostPort( const char *consumername ) const
{
  //std::cout << " In GetHostPort " << std::endl;

  char *hostport = 0;

  UInt_t count = 0;
  Int_t idsave = -99; 

  if ( fConsumerList ) {

    UInt_t nent = fConsumerList->nEntries();

    if ( nent > 0 ) {
      for ( UInt_t i = 0; i < nent; i++ ) {
	if ( !strcmp( fConsumerList->consumername(i), consumername) && 
	     !strcmp( fConsumerList->statusString(i), "Running" ) ) {
	  count++;
	  idsave = i;
	} //if ( !strcmp( fConsumerList->consumername(i), consumername) &&
      } //for ( int i = 0; i < nent; i++ )
    } //if ( nent > 0 )

  } //if ( fConsumerList )

  if ( count == 1 ) {
    if ( fListBox ) fListBox->Select(idsave);
    hostport = HostPortFromID(idsave);
  } //if ( count == 1 )
  else if ( count == 0 ) std::cout << " No consumer " << consumername << std::endl;
  else if ( count > 1) std::cout << " More than one " << consumername << std::endl; 

  return hostport;
}


int ConsumerListBox::
NRunningConsumer( const char *consumername ) const
{
  //std::cout << " In NRunningConsumer " << std::endl;

  UInt_t count = 0;

  if ( fConsumerList ) {

  UInt_t nent = fConsumerList->nEntries();

  if ( nent > 0 ) {
    for ( UInt_t i = 0; i < nent; i++ ) {

      if ( !strcmp( fConsumerList->consumername(i), consumername) && 
	   !strcmp( fConsumerList->statusString(i), "Running" ) ) 
	count++;

    } //for ( int i = 0; i < nent; i++ )
  } //if ( nent > 0 )

  } //if ( fConsumerList )

  return count;
} 


/*


//---- Main program ----------------------------------------------------------

TROOT root("GUI", "GUI test environement");

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   } //if (gROOT->IsBatch()) 

   ConsumerListBox mainWindow(gClient->GetRoot(), 
			      gClient->GetRoot(), 400, 220);

   mainWindow.SetConsumerList("http://www-cdfonline.fnal.gov/consumer/consumer_status.html");

   mainWindow.OpenWindow();

   mainWindow.UpdateListBox();

   if ( argc > 0 ) {
     for( int i = 1; i < argc ; i++ ) {
       std::cout << " Consumer: " << argv[i] << std::endl;
       char *hostport = mainWindow.GetHostPort( argv[i] );
       if ( hostport ) {
	 std::cout << " GetHostPort " << hostport << std::endl;
	 delete [] hostport;
       } //if ( hostport )
     } //for( int i = 1; i < argc ; i++ )
   } //if ( argc > 0 ) 

   theApp.Run();

   return 0;
}

*/

