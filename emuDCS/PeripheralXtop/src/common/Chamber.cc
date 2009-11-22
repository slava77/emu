//-----------------------------------------------------------------------
// $Id: Chamber.cc,v 1.12 2009/11/22 13:45:37 liu Exp $
// $Log: Chamber.cc,v $
// Revision 1.12  2009/11/22 13:45:37  liu
// debug message  for bad readings
//
// Revision 1.11  2009/10/14 17:33:09  liu
// allow X2P to start before monitoring processes, add new DIM commands
//
// Revision 1.10  2009/08/08 04:14:56  liu
// update protocol for DDU
//
// Revision 1.9  2009/06/03 01:37:50  liu
// don't send data if can't read the crate or chamber
//
// Revision 1.8  2009/05/30 09:14:44  liu
// update
//
// Revision 1.7  2009/05/20 11:00:05  liu
// update
//
// Revision 1.6  2009/03/25 12:06:24  liu
// change namespace from emu::e2p:: to emu::x2p::
//
// Revision 1.5  2009/03/20 13:08:14  liu
// move include files to include/emu/pc
//
// Revision 1.4  2008/11/03 20:00:13  liu
// do not update timestamp for all-zero chamber
//
// Revision 1.3  2008/10/18 18:18:44  liu
// update
//
// Revision 1.2  2008/10/13 12:14:28  liu
// update
//
// Revision 1.1  2008/10/12 11:55:49  liu
// new release of e2p code
//

#include "emu/x2p/Chamber.h"

namespace emu {
  namespace x2p {

//
Chamber::Chamber():
label_("CSC"), active_(0), ready_(false), corruption(false)
{
}

Chamber::~Chamber()
{
}

void Chamber::Fill(char *buffer, int source)
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
       else if(idx<52)
       {  
           y=strtof(item,NULL);
           values[idx-3]=y;
       }
       idx++;
       item=strtok_r(NULL, sep, &last);
   };
   if(source==0 && states[0]==0)
   {   // DEBUG: print a bad reading, but only the first time it goes to bad
       if(ready_) std::cout << label_ << " " << states[1] << std::endl;
       ready_ = false;
   }
   else
   {   // DEBUG: print if a bad reading changes back to good
       if(source==0 && !ready_) std::cout << "OK now: " << label_ << " " << states[1] << std::endl;
       ready_ = true;
   }
   if(source==0 && (idx!=51 || values[47]!=(-50.)))
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

void Chamber::GetDimLV(int hint, LV_1_DimBroker *dim_lv )
{
   int *info, this_st, total_st=0;
   float *data;
   char *vcc_ip = "02:00:00:00:00:00";
   //   float V33, V50, V60, C33, C50, C60, V18, V55, V56, C18, C55, C56;

   info = &(states[0]);
   data = &(values[0]);

   for(int i=0; i<CFEB_NUMBER; i++)
   {
      dim_lv->cfeb.v33[i] = data[19+3*i];
      dim_lv->cfeb.v50[i] = data[20+3*i];
      dim_lv->cfeb.v60[i] = data[21+3*i];
      dim_lv->cfeb.c33[i] = data[ 0+3*i];
      dim_lv->cfeb.c50[i] = data[ 1+3*i];
      dim_lv->cfeb.c60[i] = data[ 2+3*i];
      this_st=info[0];
      if(data[19+3*i]==0. && data[20+3*i]==0. && data[21+3*i]==0.) this_st=0;
      if(corruption) this_st=0;
      dim_lv->cfeb.status[i] = this_st;
      total_st += this_st;
   }
      dim_lv->alct.v18 = data[35];
      dim_lv->alct.v33 = data[34];
      dim_lv->alct.v55 = data[36];
      dim_lv->alct.v56 = data[37];
      dim_lv->alct.c18 = data[16];
      dim_lv->alct.c33 = data[15];
      dim_lv->alct.c55 = data[17];
      dim_lv->alct.c56 = data[18];
      this_st=info[0];
      if(data[34]==0. && data[35]==0. && data[36]==0. && data[37]==0. ) this_st=0;
      if(corruption) this_st=0;
      dim_lv->alct.status = this_st;
      total_st += this_st;
   
   dim_lv->update_time = info[1];
   dim_lv->slot = (states[2]>>8)&0xFF;

   memcpy(dim_lv->VCCMAC, vcc_ip, 18);
   sprintf(dim_lv->VCCMAC+15, "%02X", (states[2]&0xFF));
}

void Chamber::GetDimTEMP(int hint, TEMP_1_DimBroker *dim_temp )
{
   int *info;
   float *data, total_temp;
   char *vcc_ip = "02:00:00:00:00:00";

      info = &(states[0]);
      data = &(values[0]);

      dim_temp->t_daq = (data[40]<(-30)) ? 0.0 :data[40];
      dim_temp->t_cfeb1 = (data[41]<(-30)) ? 0.0 :data[41];
      dim_temp->t_cfeb2 = (data[42]<(-30)) ? 0.0 :data[42];
      dim_temp->t_cfeb3 = (data[43]<(-30)) ? 0.0 :data[43];
      dim_temp->t_cfeb4 = (data[44]<(-30)) ? 0.0 :data[44];
      // 1/3 chambers have no CFEB5
      dim_temp->t_cfeb5 = (data[45]<(-30)) ? 0.0 : data[45];
      dim_temp->t_alct = (data[46]<(-30)) ? 0.0 : data[46];
   
   total_temp = dim_temp->t_daq + dim_temp->t_cfeb1 + dim_temp->t_cfeb2
            + dim_temp->t_cfeb3 + dim_temp->t_cfeb4 + dim_temp->t_alct;

   dim_temp->update_time = info[1];
   dim_temp->slot = (states[2]>>8)&0xFF;

   memcpy(dim_temp->VCCMAC, vcc_ip, 18);
   sprintf(dim_temp->VCCMAC+15, "%02X", (states[2]&0xFF));
}

  } // namespace emu::x2p
} // namespace emu
