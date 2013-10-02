//-----------------------------------------------------------------------
// $Id: Chamber.cc,v 1.20 2011/03/11 14:05:32 liu Exp $
// $Log: Chamber.cc,v $
// Revision 1.20  2011/03/11 14:05:32  liu
// update comments
//
// Revision 1.19  2011/03/11 13:43:13  liu
// added 7 volts and CCB configuration bits
//
// Revision 1.18  2010/08/26 19:16:45  liu
// add TMB voltages
//
// Revision 1.17  2010/07/18 16:41:47  liu
// new PVSS-X2P protocol
//
// Revision 1.16  2010/07/15 11:41:08  liu
// update DIM data structure
//
// Revision 1.15  2010/07/07 21:56:41  liu
// update
//
// Revision 1.14  2010/05/27 12:38:18  liu
// add error flags for bad DMB reading
//
// Revision 1.13  2010/02/23 14:32:01  liu
// add debug info
//
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
label_("CSC"), active_(0), dataok_(true), ready_(false), corruption(false)
{
   type_=0;
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
       if(idx<5) 
       {
           i = atoi(item);
           states[idx] = i; 
       }
       else if(idx<69 || (type_==2 && idx<286))
       {  
           y=strtof(item,NULL);
           values[idx-5]=y;
       }
       idx++;
       item=strtok_r(NULL, sep, &last);
   };

   if(source==1)  type_=states[0];

   if(source==0)
   {  
      if(type_<=1)
      {
        if(idx!=68 || values[62]!=(-50.))
        {   std::cout << label_ << " (type 1) BAD...total " << idx << " last one " << values[62] << std::endl;
            corruption = true;
        }
        else 
        {   corruption = false;
        }
      }
      else if(type_==2)
      {
        if(idx!=286 || values[280]!=(-50.))
        {   std::cout << label_ << " (type 2) BAD...total " << idx << " last one " << values[280] << std::endl;
            corruption = true;
        }
        else 
        {   corruption = false;
        }
      }

      if (states[0]!=0)
      {   // DEBUG: print a bad reading, but only the first time it goes bad
         if(dataok_) std::cout << label_ << " " <<  states[0] << " " << states[1] << std::endl;
         dataok_ = false;
      }
      else
      {   // DEBUG: print if a bad reading changes back to good
         if(ready_ && !dataok_) std::cout << "OK now: " << label_ << " " << states[1] << std::endl;
         dataok_ = true;
      }
      ready_=true;
   }
}

//   hint (operation mode)
//    0        default, selected by EmuDim
//    1        file value, all good
//    2        reading error removed
//    3        true reading with all errors

void Chamber::GetDimLV(int hint, LV_1_DimBroker *dim_lv )
{
   if(type_==2) return;

   int *info, this_st;
   float *data;
   //   float V33, V50, V60, C33, C50, C60, V18, V55, V56, C18, C55, C56;

   info = &(states[0]);
   data = &(values[0]);

   this_st = info[0];
   if(corruption)
   {
       this_st |= 4;
       for(int i=0; i<38; i++) data[i] = -2.;
   }
   for(int i=0; i<CFEB_NUMBER; i++)
   {
      dim_lv->cfeb.v33[i] = data[19+3*i];
      dim_lv->cfeb.v50[i] = data[20+3*i];
      dim_lv->cfeb.v60[i] = data[21+3*i];
      dim_lv->cfeb.c33[i] = data[ 0+3*i];
      dim_lv->cfeb.c50[i] = data[ 1+3*i];
      dim_lv->cfeb.c60[i] = data[ 2+3*i];
   }
      dim_lv->alct.v18 = data[35];
      dim_lv->alct.v33 = data[34];
      dim_lv->alct.v55 = data[36];
      dim_lv->alct.v56 = data[37];
      dim_lv->alct.c18 = data[16];
      dim_lv->alct.c33 = data[15];
      dim_lv->alct.c55 = data[17];
      dim_lv->alct.c56 = data[18];
   
      dim_lv->tmb.v50  = data[47];
      dim_lv->tmb.v33  = data[48];
      dim_lv->tmb.v15C = data[49];
      dim_lv->tmb.v15T = data[50];
      dim_lv->tmb.v10T = data[51];
      dim_lv->tmb.c50  = data[52];
      dim_lv->tmb.c33  = data[53];
      dim_lv->tmb.c15C = data[54];
      dim_lv->tmb.c15T = data[55];
      dim_lv->tmb.cRAT = data[56];
      dim_lv->tmb.vRAT = data[57];
      dim_lv->tmb.vREF = data[58];
      dim_lv->tmb.vGND = data[59];
      dim_lv->tmb.vMAX = data[60];

   dim_lv->A7v = data[38];
   dim_lv->D7v = data[39];
   dim_lv->CCB_bits = info[3];
   dim_lv->FPGA_bits = info[4];
   dim_lv->update_time = info[1];
   dim_lv->status = this_st;

}

void Chamber::GetDimLV2(int hint, LV_2_DimBroker *dim_lv )
{
   if(type_<=1) return;

   int *info, this_st;
   float *data;
   //   float V33, V50, V60, C33, C50, C60, V18, V55, V56, C18, C55, C56;

   info = &(states[0]);
   data = &(values[0]);

   this_st = info[0];
   if(corruption)
   {
       this_st |= 4;
       for(int i=0; i<38; i++) data[i] = -2.;
   }
   for(int i=0; i<DCFEB_NUMBER; i++)
   {
      dim_lv->dcfeb.v30[i] = data[25+3*i];
      dim_lv->dcfeb.v40[i] = data[26+3*i];
      dim_lv->dcfeb.v55[i] = data[27+3*i];
      dim_lv->dcfeb.c30[i] = data[ 0+3*i];
      dim_lv->dcfeb.c40[i] = data[ 1+3*i];
      dim_lv->dcfeb.c55[i] = data[ 2+3*i];
   }
      dim_lv->alct.v18 = data[47];
      dim_lv->alct.v33 = data[46];
      dim_lv->alct.v55 = data[48];
      dim_lv->alct.v56 = data[49];
      dim_lv->alct.c18 = data[22];
      dim_lv->alct.c33 = data[21];
      dim_lv->alct.c55 = data[23];
      dim_lv->alct.c56 = data[24];
   
      dim_lv->tmb.v50  = data[65];
      dim_lv->tmb.v33  = data[66];
      dim_lv->tmb.v15C = data[67];
      dim_lv->tmb.v15T = data[68];
      dim_lv->tmb.v10T = data[69];
      dim_lv->tmb.c50  = data[70];
      dim_lv->tmb.c33  = data[71];
      dim_lv->tmb.c15C = data[72];
      dim_lv->tmb.c15T = data[73];
      dim_lv->tmb.cRAT = data[74];
      dim_lv->tmb.vRAT = data[75];
      dim_lv->tmb.vREF = data[76];
      dim_lv->tmb.vGND = data[77];
      dim_lv->tmb.vMAX = data[78];

   for(int i=0; i<DCFEB_NUMBER; i++)
   {
      dim_lv->dsys.vcore[i] = data[81+27*i];
      dim_lv->dsys.vaux1[i] = data[82+27*i];
   }

   dim_lv->A7v = data[50];
   dim_lv->D7v = data[51];
   dim_lv->CCB_bits = info[3];
   dim_lv->FPGA_bits = info[4];
   dim_lv->update_time = info[1];
   dim_lv->status = this_st;

}

void Chamber::GetDimTEMP(int hint, TEMP_1_DimBroker *dim_temp )
{
   if(type_==2) return;

   int *info, this_st;
   float *data, total_temp;

      info = &(states[0]);
      data = &(values[0]);

   this_st = info[0];
   if(corruption)
   {
       this_st |= 4;
       for(int i=38; i<48; i++) data[i] = -2.;
   }
      dim_temp->t_daq = (data[40]<(-30)) ? -3.0 :data[40];
      dim_temp->t_cfeb1 = (data[41]<(-30)) ? -3.0 :data[41];
      dim_temp->t_cfeb2 = (data[42]<(-30)) ? -3.0 :data[42];
      dim_temp->t_cfeb3 = (data[43]<(-30)) ? -3.0 :data[43];
      dim_temp->t_cfeb4 = (data[44]<(-30)) ? -3.0 :data[44];
      // 1/3 chambers have no CFEB5
      dim_temp->t_cfeb5 = (data[45]<(-30)) ? -3.0 : data[45];
      dim_temp->t_alct = (data[46]<(-30)) ? -3.0 : data[46];
   
   total_temp = dim_temp->t_daq + dim_temp->t_cfeb1 + dim_temp->t_cfeb2
            + dim_temp->t_cfeb3 + dim_temp->t_cfeb4 + dim_temp->t_alct;

   dim_temp->update_time = info[1];
   dim_temp->status = this_st;
}

void Chamber::GetDimTEMP2(int hint, TEMP_2_DimBroker *dim_temp )
{
   if(type_<=1) return;

   int *info, this_st;
   float *data, total_temp;

      info = &(states[0]);
      data = &(values[0]);

   this_st = info[0];
   if(corruption)
   {
       this_st |= 4;
       for(int i=38; i<48; i++) data[i] = -2.;
   }
      dim_temp->t_odmb = data[80+27*DCFEB_NUMBER];
      dim_temp->t_otmb = data[57];
      dim_temp->t_alct = data[56];
      dim_temp->t_lvdb = data[55];
      
      for(int i=0; i<DCFEB_NUMBER; i++)
      {
         dim_temp->t_fpga[i] = data[80+27*i];
         dim_temp->t_pcb1[i] = data[80+27*i+22];
         dim_temp->t_pcb2[i] = data[80+27*i+23];
      }

   dim_temp->update_time = info[1];
   dim_temp->status = this_st;
}

  } // namespace emu::x2p
} // namespace emu
