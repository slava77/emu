#ifndef _HistoDisplayPhoto_H_
#define _HistoDisplayPhoto_H_

//******************** Modified by Koji (1999/12/2) ******************* 
class PhotoWindow : public TGTransientFrame {

private:
  ULong_t            LightYellow,LightBlue, Cyan;
  TGTextButton       *fButton;
  TGPictureButton    *fPicBut0, *fPicBut1;
  TGLabel            *fTitleLabel, *fTextLabel0, *fTextLabel1, *fTextLabel2;
  TGLabel            *fTextLabel3, *fTextLabel4, *fTextLabel5;
  TGLabel            *fTextLabel41;
  TGCompositeFrame   *fTitleFrame, *fTextFrame0, *fTextFrame1, *fTextFrame2;
  GContext_t         fTextGC0, fTextGC1, fTextGC2;

public:
    PhotoWindow(const TGWindow *p, const TGWindow *main,  UInt_t w, UInt_t h,
                UInt_t options = kMainFrame | kVerticalFrame);
    virtual ~PhotoWindow();
    virtual void ClosePhotoWindow();
    virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};
//***********************************************************************

#endif
