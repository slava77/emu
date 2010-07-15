//-----------------------------------------------------------------------
// $Id: DDU.cc,v 1.4 2010/07/15 11:41:08 liu Exp $
// $Log: DDU.cc,v $
// Revision 1.4  2010/07/15 11:41:08  liu
// update DIM data structure
//
// Revision 1.3  2009/10/14 17:33:09  liu
// allow X2P to start before monitoring processes, add new DIM commands
//
// Revision 1.2  2009/08/08 04:14:56  liu
// update protocol for DDU
//
// Revision 1.1  2009/07/20 12:41:34  liu
// add DDU info to X2P
//

#include "emu/x2p/DDU.h"

namespace emu {
  namespace x2p {

//
DDU::DDU():
label_("DDU0"), active_(0), ready_(false), corruption(false)
{
}

DDU::~DDU()
{
}

void DDU::Fill(char *buffer, int source)
{
//
// source==0  from Xmas
//       ==1  from file
//
   int idx=0, i;
   char *start = buffer, *item, *sep = " ";
   char *last=NULL;
   float y;

   item=strtok_r(start, sep, &last);
   while(item)
   {
       if(idx<3) 
       {
           i = atoi(item);
           states[idx] = i; 
       }
       else if(idx<12)
       {  
           y=strtof(item,NULL);
           if(idx<7) y=y*0.001;
           values[idx-3]=y;
       }
       idx++;
       item=strtok_r(NULL, sep, &last);
   };

   if(source)
   {   corruption = false;
       ready_ = true;
   }
   else if(idx!=12 || values[8]!=(48879.))
   {   std::cout << "BAD...total " << idx << " last one " << values[8] << std::endl;
       corruption = true;
   }
   else corruption = false;
}

//   hint (operation mode)
//    0        default, selected by EmuDim
//    1        file value, all good
//    2        reading error removed
//    3        true reading with all errors

void DDU::GetDimDDU(int hint, DDU_1_DimBroker *dim_return )
{
   int *info, this_st;
   float *data;
   //   float V15, V25A, V25B, V33, TD1, TD2, TD3, TD4;

      info = &(states[0]);
      data = &(values[0]);
         
      dim_return->v15 = data[0];
      dim_return->v25a = data[1];
      dim_return->v25b = data[2];
      dim_return->v33 = data[3];
      dim_return->td1 = data[4];
      dim_return->td2 = data[5];
      dim_return->td3 = data[6];
      dim_return->td4 = data[7];
      this_st = (info[1]?0:1);
      if(corruption) this_st=1;
      dim_return->status = this_st;
      dim_return->update_time = info[0];
//      memcpy(dim_return->QTAG, "DDU", 4);
}

  } // namespace emu::x2p
} // namespace emu
