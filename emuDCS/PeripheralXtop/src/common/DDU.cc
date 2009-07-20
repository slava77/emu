//-----------------------------------------------------------------------
// $Id: DDU.cc,v 1.1 2009/07/20 12:41:34 liu Exp $
// $Log: DDU.cc,v $
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
           if(source) 
             {  states_bk[idx] = i; 
             }
           states[idx] = i; 
       }
       else if(idx<12)
       {  
           y=strtof(item,NULL);
           if(idx<7) y=y*0.001;
           if(source) values_bk[idx-3]=y;
           values[idx-3]=y;
       }
       idx++;
       item=strtok_r(NULL, sep, &last);
   };
   if(idx==11) 
   {   
       if(source) 
       {
          ready_ = false;
       }
       else
       {
          ready_ = states[1];
       }
   }
   if(source)
   {   corruption = false;
   }
   else if(idx!=11 || values[8]!=(48879.))
   {   std::cout << "BAD...total " << idx << " last one " << values[47] << std::endl;
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
   int *info;
   float *data;
   //   float V15, V25A, V25B, V33, TD1, TD2, TD3, TD4;

   if(corruption || hint==1)
   {  info = &(states_bk[0]);
      data = &(values_bk[0]);
   }
   else
   {  info = &(states[0]);
      data = &(values[0]);
   }      
      dim_return->v15 = data[0];
      dim_return->v25a = data[1];
      dim_return->v25b = data[2];
      dim_return->v33 = data[3];
      dim_return->td1 = data[4];
      dim_return->td2 = data[5];
      dim_return->td3 = data[6];
      dim_return->td4 = data[7];
      dim_return->status = info[1];
      dim_return->update_time = info[0];

}

  } // namespace emu::x2p
} // namespace emu
