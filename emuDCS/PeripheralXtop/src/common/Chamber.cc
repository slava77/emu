//-----------------------------------------------------------------------
// $Id: Chamber.cc,v 1.1 2008/10/12 11:55:49 liu Exp $
// $Log: Chamber.cc,v $
// Revision 1.1  2008/10/12 11:55:49  liu
// new release of e2p code
//

#include "Chamber.h"

namespace emu {
  namespace e2p {

//
Chamber::Chamber():
label_("CSC"), active_(0), ready_(false)
{
}

Chamber::~Chamber()
{
}

void Chamber::Fill(char *buffer, int source)
{
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
           if(source) states_bk[idx] = i; 
           else if(idx<2) states[idx] = i; 
       }
       else if(idx<52)
       {  
           y=strtof(item,NULL);
           if(source) values_bk[idx-3]=y;
           else values[idx-3]=y;
       }
       idx++;
       item=strtok_r(NULL, sep, &last);
   };
   if(source && idx==51) ready_ = true;
   if(idx!=51 || (source==0 && values[47]!=(-50.))) 
      std::cout << "Total " << idx << " last one " << values[47] << std::endl;
}

void Chamber::GetDimLV(int hint, LV_1_DimBroker *dim_lv )
{
   int *info;
   float *data;
   char *vcc_ip = "02:00:00:00:00:00";

   if(hint==0)
   {  info = &(states[0]);
      data = &(values[0]);
   }      
   else
   {  info = &(states_bk[0]);
      data = &(values_bk[0]);
   }
   for(int i=0; i<CFEB_NUMBER; i++)
   {
      dim_lv->cfeb.v33[i] = data[19+3*i];
      dim_lv->cfeb.v50[i] = data[20+3*i];
      dim_lv->cfeb.v60[i] = data[21+3*i];
      dim_lv->cfeb.c33[i] = data[ 0+3*i];
      dim_lv->cfeb.c50[i] = data[ 1+3*i];
      dim_lv->cfeb.c60[i] = data[ 2+3*i];
      dim_lv->cfeb.status[i] = info[0];
   }
      dim_lv->alct.v18 = data[35];
      dim_lv->alct.v33 = data[34];
      dim_lv->alct.v55 = data[36];
      dim_lv->alct.v56 = data[37];
      dim_lv->alct.c18 = data[16];
      dim_lv->alct.c33 = data[15];
      dim_lv->alct.c55 = data[17];
      dim_lv->alct.c56 = data[18];
      dim_lv->alct.status = info[0];
   
   dim_lv->update_time = info[1];
   dim_lv->slot = (states_bk[2]>>8)&0xFF;

   memcpy(dim_lv->VCCMAC, vcc_ip, 18);
   sprintf(dim_lv->VCCMAC+15, "%02X", (states_bk[2]&0xFF));
}

void Chamber::GetDimTEMP(int hint, TEMP_1_DimBroker *dim_temp )
{
   int *info;
   float *data;
   char *vcc_ip = "02:00:00:00:00:00";

   if(hint==0)
   {  info = &(states[0]);
      data = &(values[0]);
   }      
   else
   {  info = &(states_bk[0]);
      data = &(values_bk[0]);
   }
      dim_temp->t_daq = data[40];
      dim_temp->t_cfeb1 = data[41];
      dim_temp->t_cfeb2 = data[42];
      dim_temp->t_cfeb3 = data[43];
      dim_temp->t_cfeb4 = data[44];
      dim_temp->t_cfeb5 = data[45];
      dim_temp->t_alct = data[46];
   
   dim_temp->update_time = info[1];
   dim_temp->slot = (states_bk[2]>>8)&0xFF;

   memcpy(dim_temp->VCCMAC, vcc_ip, 18);
   sprintf(dim_temp->VCCMAC+15, "%02X", (states_bk[2]&0xFF));
}

  } // namespace emu::e2p
} // namespace emu
