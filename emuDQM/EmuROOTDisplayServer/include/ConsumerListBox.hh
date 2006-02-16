//
// 
// ConsumerListBox.hh
// Tetsuo Arisawa (Waseda Univ.), 2001/5/3
//
// Display list of consumers.
// Enable to choose consumers.
//
//

#ifndef _ConsumerListBox_H_
#define _ConsumerListBox_H_


class TGTextButton;
class TGListBox; 
class TGCompositeFrame;
class TGLayoutHints;
class TGLablel;


class ConsumerList; // Hartmut Stadie
class HistoDisplay; // T.A



class ConsumerListBox : public TGTransientFrame
{

private:

  ConsumerList *fConsumerList;
  HistoDisplay *fHistoDisplay;


  TGLayoutHints *fLayout;
  TGLayoutHints *fLayoutLT;

  TGCompositeFrame *fLabelFrame;
  TGLayoutHints *fLabelLayout;
  TGLabel *fLabel1,*fLabel2,*fLabel3,*fLabel4,*fLabel5;
  //TGLabel *fLabel;

  TGListBox *fListBox;
  TGLayoutHints *fListBoxLayout;

  TGCompositeFrame *fButtonFrame;
  TGTextButton *fButton1, *fButton2;


public:
  ConsumerListBox(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
  virtual ~ConsumerListBox();
  virtual Bool_t ProcessMessage( Long_t msg, Long_t parm1, Long_t parm2 );

  virtual void CloseWindow();

  void SetConsumerList( const char *website );
  void SetConsumerList( ConsumerList *consumerlist );

  void SetHistoDisplay( HistoDisplay *histodisplay );

  void OpenWindow();
  void UpdateListBox();

  void OpenSelectedEntries() const;

  char* GetHostPort( const char *consumername ) const;
  char *HostPortFromID( const UInt_t id ) const;
  int NRunningConsumer( const char *consumername ) const;

  //ClassDef( ConsumerListBox, 0 )
};


#endif
