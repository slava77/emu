//-----------------------------------------------------------------------
// $Id: PROBLEM.h,v 1.1 2011/03/06 13:51:28 liu Exp $
// $Log: PROBLEM.h,v $
// Revision 1.1  2011/03/06 13:51:28  liu
// new class PROBLEM
//
// Revision 1.0  2011/03/06 13:14:01  liu
// create PROBLEM class

//
// This class should not have any external references
// to other classes, including those in the standard C/C++ libs
//
#ifndef PROBLEM_h
#define PROBLEM_h

namespace emu {
  namespace pc {

const int ProbModule_CRATE = 0;
const int ProbModule_VCC = 1;
const int ProbModule_CCB = 2;
const int ProbModule_MPC = 3;
const int ProbModule_TMB = 4;
const int ProbModule_DMB = 5;
const int ProbModule_FEB = 6;

const int ProbSubMod_ALL = 0;
const int ProbSubMod_CFEB1 = 1;
const int ProbSubMod_CFEB2 = 2;
const int ProbSubMod_CFEB3 = 3;
const int ProbSubMod_CFEB4 = 4;
const int ProbSubMod_CFEB5 = 5;
const int ProbSubMod_ALCT = 6;
const int ProbSubMod_CFEBALL = 7;

const int ProbSource_CONFDB = 1;
const int ProbSource_CCBBIT = 2;
const int ProbSource_DCSREAD = 3;
const int ProbSource_TMBCOUNT = 4;
const int ProbSource_CONFCHK = 5;
const int ProbSource_FPGACHK = 6;
const int ProbSource_VME = 7;
const int ProbSource_VMESP = 8;
const int ProbSource_USER = 9;
// ...

const int ProbType_OK = 0;
const int ProbType_OFF = 1;
const int ProbType_SKIP = 2;
const int ProbType_KNOWN = 3;
const int ProbType_ACCESS = 4;
const int ProbType_HIGH = 5;
const int ProbType_LOW = 6;
const int ProbType_DIFF = 7;
//...
const int ProbType_EXCEPTION = 14;
const int ProbType_UNKNOWN = 15;


class PROBLEM
{
public:
  PROBLEM();
  PROBLEM(int error);
  virtual ~PROBLEM();

  inline void crate(int value)   {crate_ = value & 0x3F;}
  inline int  crate()   {return (int)crate_;}
  inline void module(int value)   {module_ = value & 7;}
  inline int  module()   {return (int)module_;}
  inline void chamber(int value)   {chamber_ = value & 0xF;}
  inline int  chamber()   {return (int)chamber_;}
  inline void sub_mod(int value)   {sub_mod_ = value & 7;}
  inline int  sub_mod()   {return (int)sub_mod_;}
  inline void source(int value)   {source_ = value & 0xF;}
  inline int  source()   {return (int)source_;}
  inline void index(int value)   {index_ = value & 0xF;}
  inline int  index()   {return (int)index_;}
  inline void type(int value)   {type_ = value & 0xF;}
  inline int  type()   {return (int)type_;}
  inline void sub_type(int value)   {sub_type_ = value & 7;}
  inline int  sub_type()   {return (int)sub_type_;}
  inline void vme_add(int value)  {index_ = value&0xF;  type_ = (value>>4)&0xF; sub_type_ = (value>>8)&7;}
  inline int vme_add()  {return ((int)sub_type_<<8)+((int)type_<<4)+index_; }
  inline void reset() { type_ = 0; sub_type_ = 0;}
  
  int comb();
  void clear();
  bool error();
  bool known();

private:
  char crate_;     // 6 bits
  char module_;    // 3 bits
  char chamber_;   // 4 bits
  char sub_mod_;   // 3 bits

  char source_;    // 4 bits
  char index_;     // 4 bits   vme_add lowest 4 bits
  char type_;      // 4 bits   vme_add middle 4 bits
  char sub_type_;  // 3 bits   vme_add highest 3 bits
         // total == 31 bits
};


  } // namespace emu::pc
  } // namespace emu
#endif

