//-----------------------------------------------------------------------
// $Id: PROBLEM.cc,v 1.1 2011/03/06 13:51:28 liu Exp $
// $Log: PROBLEM.cc,v $
// Revision 1.1  2011/03/06 13:51:28  liu
// new class PROBLEM
//
// Revision 1.0  2011/03/04 13:14:01  liu
// create PROBLEM class

//
// This class should not use any external functions or references
// from other classes, including those in the standard C/C++ libs
//
#include "emu/pc/PROBLEM.h"

namespace emu {
  namespace pc {


PROBLEM::PROBLEM()
:crate_(0), module_(0), chamber_(0), sub_mod_(0),
 source_(0), index_(0), type_(0), sub_type_(0)
{};

PROBLEM::PROBLEM(int error)
{
   crate_ = error&0x3F;
   module_ = (error>>6)&7;
   chamber_ = (error>>9)&0xF;
   sub_mod_ = (error>>13)&7;

   source_ = (error>>16)&0xF;
   index_ = (error>>20)&0xF;
   type_ = (error>>24)&0xF;
   sub_type_ = (error>>28)&7;
}

PROBLEM::~PROBLEM() {}

int PROBLEM::comb() 
{
   int combined_;

   if(crate_==0 || crate_>60) return 0;
   combined_ = crate_+(module_<<6)+(chamber_<<9)+(sub_mod_<<13);
   combined_ += (source_<<16)+(index_<<20)+(type_<<24)+(sub_type_<<28);
   return combined_;
}

void PROBLEM::clear()
{
   crate_ = 0;
   module_ = 0;
   chamber_ = 0;
   sub_mod_ = 0;
   source_ = 0;
   index_ = 0;
   type_ = 0;
   sub_type_ = 0;
}

bool PROBLEM::error()
{
   if(crate_==0) return false;
   if(source_==0) return false;
   if(type_<4) return false;
   return true;
}

bool PROBLEM::known()
{
   if(type_>=1 && type_<=3) return true;
   else return false;
}

  } // namespace emu::pc
  } // namespace emu
