//-----------------------------------------------------------------------
// $Id: elong.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: elong.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef elong_h
#define elong_h

class elong {
 public:
  enum { ELLENGTH=22 };

  unsigned long r[ELLENGTH]; // value holder
  int l;               // current length
 
  void clear ();
  void cat (unsigned long arg, int bits);
  unsigned long split (int bits);
};

#endif
