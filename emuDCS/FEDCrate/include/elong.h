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
