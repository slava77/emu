/* decode_triads.c  
*
* Author: Fabian Heidrich-Meisner
* Date:  3 September 1999
*
* 
* Including the following functions:
*  - void decode_triads()
*/
#include <iostream>
#include <stdlib.h>
#include <syslog.h>
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "decode_triads.h"

/*---------------------------------------------------------*/

#define NSTRIP_FEB 16
#define MAX_HALFSTRIP_HITS 5000

/*------------------------------------------------------------------*/
/*                           decode triads                          */
/*------------------------------------------------------------------*/
/*
*  void  decode_triads()
* Author: Fabian Heidrich-Meisner
* date 2 September 1999
* 
* This function is supposed to decode triads into halfstrips.
  Input is the global variable upevt_.clct_dump[NLAYER][NSTRIP/2]
  Output will be in:
          clct_halfstrips[NLAYER][NSTRIP*2][NCLCT_BUCKET]
  Problems:
  If a triad occurs before pretrigger it may not be fully included 
  in the full dump. But a bit set to 1 belonging to such a triad can 
  mess up the following triads!
  
 
*/
/* function declaration */


using std::cout;
using std::endl;

//void init_array(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET]);
//int  calculate_halfstrip(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET],
//             		 int layer,int distrip, int  bucket); 
			 
void decode_triads_(int clct_halfstrips[NLAYER][NSTRIP*2][NCLCT_BUCKET])
{
 int ibucket;
 int ilayer;
 int idistrip;
 int halfstrip = 0;
 int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET]; 
 
 /* initialize working array */ 
  init_array(triads); 
  for(ilayer=0; ilayer < NLAYER; ilayer++){
       for(ibucket=0; ibucket < upevt_.clct_nbucket-2 ; ibucket ++){
           for(idistrip=0; idistrip < NSTRIP/2; idistrip++){
             /* initialize clct_halfstrips */
             /*
             clct_halfstrips[ilayer][((idistrip*2)+1)*2][ibucket]  = 0;
             clct_halfstrips[ilayer][((idistrip*2)+1)*2+1][ibucket]= 0;
             clct_halfstrips[ilayer][(idistrip*2)*2][ibucket]      = 0;
             clct_halfstrips[ilayer][(idistrip*2)*2+1][ibucket]    = 0;
             */
             if (triads[ilayer][idistrip][ibucket] == 1){
	       halfstrip =  calculate_halfstrip(triads,ilayer,idistrip, ibucket);
	         clct_halfstrips[ilayer][halfstrip][ibucket] = 1;
	      }/* if distrip hit */    
	   }/* idistrip */
       }/* ibucket */  
   }/* ilayer */
 

  
  
  for(ilayer=0; ilayer < NLAYER; ilayer++){
    for(idistrip=0; idistrip < NSTRIP/2; idistrip++){
      for(ibucket=0; ibucket < upevt_.clct_nbucket-2 ; ibucket ++){


	std::cout<<" TRIADS ARE ";
	std::cout<<triads[ilayer][idistrip][ibucket]<<" ";
	if(ibucket==9) std::cout<<" "<<std::endl; 

      }
    }
  }
  
// szs

}/* decode_triads */

/*----------------------------------------------------------*/
void init_array(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET])
{
 int ibucket;
 int ilayer;
 int idistrip;
  
   for (ilayer=0; ilayer < NLAYER; ilayer++){
     for (idistrip=0; idistrip < NSTRIP/2; idistrip++){
       for (ibucket=0; ibucket < upevt_.clct_nbucket; ibucket ++){
         if (upevt_.clct_dump[ilayer][idistrip] & (1 << ibucket)) 
          triads[ilayer][idistrip][ibucket] = 1;
         else triads[ilayer][idistrip][ibucket] = 0;
       } /* idistrip */
     }/* ibucket */
   }/* ilayer */  
}/* init array */

/*--------------------------------------------*/
int  calculate_halfstrip(int triads[NLAYER][NSTRIP/2][NCLCT_BUCKET],
                         int layer,int distrip, int  bucket)
{ 
  int strip;
  int halfstrip;
   
        if(triads[layer][distrip][bucket+1] == 1){
                 strip =  2 *distrip + 1;
        }/* right strip */
	else{
	         strip = 2*distrip;
        }/* left strip */		 
        if(triads[layer][distrip][bucket+2] == 1){
             halfstrip = 2*strip +1; 
       }/* right halfstrip */
       else{
             halfstrip = 2*strip;
       }/* left halfstrip */
       
       triads[layer][distrip][bucket+1] = 0;	
       triads[layer][distrip][bucket+2] = 0;
    
    return halfstrip;
    
}/* calculate_halfstrip */
		
