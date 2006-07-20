//-----------------------------------------------------------------------
// $Id: OwnIt.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: OwnIt.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef OwnIt_H
#define OwnIt_H
/** a very very simple auto_ptr.
    if static get destrying at the end avoiding 
    fake memory leaks...
 */
template<class T>
class OwnIt {
public:
  typedef OwnIt<T> self;
  OwnIt(T * p=0) : it(p){}
  
  ~OwnIt() { reset();}
  inline self& operator=(T * p) { if (it) delete it; it=p; return * this;}
  inline T * get() const { return it;}
  inline void reset() { if (it) { delete it; it=0; } }
  
private:
  T * it;
  
}; 

#endif // OwnIt_H
