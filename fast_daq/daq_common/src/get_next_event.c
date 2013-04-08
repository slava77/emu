/* get_next_event.c
 *
 * Author: David Chrisman
 * Date: 09 Sept 1999
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include "daq_module.h"
#include "daq_conf.h" /* to get the user name and site maximum lengths. */
#include "csc_event.h"

#define GNE_DEBUG

#ifdef GNE_DEBUG
#define dprint if (1) printf
#else
#define dprint if (0) printf
#endif

unsigned short int crcdivide(unsigned short int *datastream);
unsigned long combine_shorts(short upper, short lower);
/* int read_afeb_config(int config_file_num, int irun); */
void dump_block(short *buf);

#define DATA      2
#define BEGIN_RUN 1

char user_name_store[USER_LENGTH];
char site_store[SITE_LENGTH];
long version_store, revision_store;

extern int crate, chamber;

// return value flags:
// bit 0: wrong chamber or empty event
// bit 1: continue on the same buffer
// bit 2: bad ALCT or other defects
int get_next_event(short *buf, int first_time)
{
	unsigned long module_id;
	char config_file_name[30];
	int event_wc, block_wc;
	int got_event_head = 0;
	int rtval = 0;
	int j, res = 0;

	/* Zero the CSC event data structure (see csc_event.h) */
	memset(&(upevt_), 0x0, sizeof(csc_event_type));
	upevt_.num_wire_hits  = 0;
	upevt_.num_scint_hits = 0;
/*  upevt_.num_rpc_hits   = 0; */

	/* The event word count */
	event_wc     = buf[0];

	dprint ("inside get next event, first_time = %d\n", first_time);

	/* First get the event header */
	for (j = 1; j < event_wc - 1; j += block_wc) 
    {
		block_wc  = buf[j];
		if (first_time)
		{
			module_id = combine_shorts(buf[j+1], buf[j+2]);
 
			if (module_id == HEAD_ID) 
			{
				decode_EVTHEAD(buf + j);
				got_event_head = 1;
				break;
			}
		}
    } 
	if (first_time)
	{
		if (! got_event_head) 
		{
#ifdef OFFLINE_ONLY
			printf("EE get_next_event(): Something wrong. Can\'t find event header\n");
#else
			syslog(LOG_LOCAL1|LOG_ERR,"EE get_next_event(): Something wrong. Can\'t find event header\n");
#endif
			return 4;
		}
	}

/* Event dump (for debugging) *//*
								  v  printf("Dump of event %ld:\n", upevt_.event_number);
								  dump_block(buf);
								*/

	for (j = 1; j < event_wc - 1; j += block_wc) 
    {

		block_wc  = buf[j];
		module_id = combine_shorts(buf[j+1], buf[j+2]);

#ifdef GNE_DEBUG
		printf("get_next_event() module_id = %lx\n", module_id);
#endif

		/* Check to see if data was corrupted. The block word counts
		 * at the beginning and end of the event buffer must match.
		 */
		if (buf[j+block_wc-1] != block_wc) 
		{
#ifdef OFFLINE_ONLY
			printf("EE get_next_event(): Unmatched block word counts! "
				   "Event %ld, module ID = 0x%lx\n", upevt_.event_number, module_id);
#else
			syslog(LOG_LOCAL1|LOG_ERR,"EE get_next_event(): Unmatched block word counts! "
				   "Event %ld, module ID = 0x%lx\n", upevt_.event_number, module_id);
#endif
			return 4;
		}

		if (first_time)
		{
			switch(upevt_.event_type) 
			{
			case BEGIN_RUN: 
				switch (module_id)
				{
				case CABLE_MAP_ID:
					sprintf(config_file_name,
							"cable_map_%04d", upevt_.cable_map_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case MOD_CONF_ID:
					sprintf(config_file_name,
							"module_config_%04d", upevt_.module_config_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case TRIG_SETUP_ID:
					sprintf(config_file_name,
							"trig_setup_%04d", upevt_.trigger_setup_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case AFEB_CONF_ID:
					sprintf(config_file_name,
							"afeb_config_%04d", upevt_.afeb_config_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case ANODE_PATT_ID:
					sprintf(config_file_name,
							"anode_pattern_%04d",upevt_.anode_pattern_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case ANODE_HOT_ID:
					sprintf(config_file_name,
							"anode_hot_mask_%02d-%04d", upevt_.chamber_type_id,
							upevt_.anode_hot_mask_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case ANODE_THRES_ID:
					sprintf(config_file_name,
							"anode_threshold_%02d-%04d", upevt_.chamber_type_id,
							upevt_.anode_threshold_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case ANODE_DELAY_ID:
					sprintf(config_file_name,
							"anode_delay_%02d-%04d", upevt_.chamber_type_id,
							upevt_.anode_delay_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case CATH_HOT_ID:
					sprintf(config_file_name,
							"cathode_hot_mask_%02d-%04d",upevt_.chamber_type_id,
							upevt_.cathode_hot_mask_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case CATH_THRES_ID:
					sprintf(config_file_name,
							"cathode_threshold_%02d-%04d",upevt_.chamber_type_id,
							upevt_.cathode_threshold_id);
					rtval = extract_config_file(config_file_name, buf + j);
					if (rtval != 0) return rtval;
					break;
				case USER_NAME_ID:
					rtval = decode_STRING(buf+j, USER_LENGTH, user_name_store);
					if(rtval !=0) return rtval;
					memcpy(upevt_.user_name, user_name_store, USER_LENGTH);
					break;
				case SITE_ID:
					rtval = decode_STRING(buf+j, SITE_LENGTH, site_store);
					if(rtval !=0) return rtval;
					memcpy(upevt_.site, site_store, SITE_LENGTH);
					break;
				case DAQ_REV_ID:
					rtval = decode_DAQREV(buf+j);
					if(rtval !=0) return rtval;
					break;
				case HEAD_ID:
					break;
				default:
#ifdef OFFLINE_ONLY
					printf("WW get_next_event(): Unexpected module ID: 0x%lx at "
						   "start of run\n", module_id);
#else
					syslog(LOG_LOCAL1|LOG_WARNING,"WW get_next_event(): Unexpected module ID: 0x%lx at "
						   "start of run\n", module_id); 
#endif
					break;
				}
				break;
			case DATA: 
				switch (module_id) 
				{
				case DDU_ID:
					decode_DDU     (buf + j);
					break;
				case DDU2_ID:
					res = decode_DDU2    (buf + j, 1);
					break;
				case TDC2277_ID:
					decode_TDC2277 (buf + j);
					break;
				case TDC3377_ID:
					decode_TDC3377 (buf + j);
					break;
				case INREG_ID:
					break;
				case ALCTCAL_ID:
					decode_ALCTCAL (buf + j);
					break;
				case ALCT2_ID:
					decode_ALCT2(buf + j);
					break;
				case STRIPCAL_ID:
					decode_STRIPCAL(buf + j);
					break;
				case TMB_ID:
					decode_TMB     (buf + j);
					break;
				case SCL2551_ID:
/*          decode_SCL2551 (buf + j); */
					break;
				case SCL4434_ID:
					decode_SCL4434 (buf + j);
					break;
				case IOREG_ID:
					break;
				case ADC2249_ID:
					break;
				case DSC7106_ID:
					decode_DSC7106 (buf + j);
					break;
				case HEAD_ID:
					break;
				default:
#ifdef OFFLINE_ONLY
					printf("WW get_next_event(): Unknown module ID: 0x%lx\n", 
						   module_id);
#else
					syslog(LOG_LOCAL1|LOG_WARNING,"WW get_next_event(): Unknown module ID: 0x%lx\n", 
						   module_id); 
#endif
					break;
				}
				break;
			default:
#ifdef OFFLINE_ONLY
				printf("WW get_next_event(): Unknown event type: %d\n", 
					   upevt_.event_type);
#else
				syslog(LOG_LOCAL1|LOG_WARNING,"WW get_next_event(): Unknown event type: %d\n", 
					   upevt_.event_type); 
#endif
				break;
			}
		}
		else
		{
			// if in the process of decoding ddu, just call ddu directly
			res = decode_DDU2 (buf + j, 0);
		}
    }
	return res;
}

/*******************************************************************************
 
   Decode event header information
 
*******************************************************************************/

int decode_EVTHEAD(short *buf)
{
#ifdef GNE_DEBUG
  int j;
#endif

  upevt_.run_number       = buf[5];
  upevt_.event_number     = combine_shorts(buf[6], buf[7]);
  upevt_.event_type       = buf[8];
  upevt_.trigger_number   = combine_shorts(buf[9], buf[10]);
  upevt_.spill_number     = buf[11];
  upevt_.time_YYMM        = buf[12];
  upevt_.time_DDHH        = buf[13];
  upevt_.time_MMSS        = buf[14];
  upevt_.time_musec       = combine_shorts(buf[15], buf[16]);
  upevt_.module_config_id = buf[17];
  upevt_.cable_map_id     = buf[18];
  upevt_.trigger_setup_id = buf[19];
  upevt_.afeb_config_id   = buf[20];
  upevt_.anode_pattern_id = buf[21];
  upevt_.chamber_type_id  = buf[22];
  upevt_.anode_hot_mask_id = buf[23];
  upevt_.anode_threshold_id = buf[24];
  upevt_.anode_delay_id   = buf[25];
  upevt_.cathode_hot_mask_id = buf[26];
  upevt_.cathode_threshold_id = buf[27];

  if (upevt_.event_type == BEGIN_RUN)
    {
      memset(user_name_store,0,USER_LENGTH);
      memset(site_store,0,SITE_LENGTH);
      version_store = 0;
      revision_store = 0;
    }
  else if (upevt_.event_type == DATA)
    {
      memcpy(upevt_.user_name, user_name_store, USER_LENGTH);
      memcpy(upevt_.site, site_store, SITE_LENGTH);
      upevt_.version = version_store;
      upevt_.revision = revision_store;
    }

#ifdef GNE_DEBUG
  printf("Event header data:\n");
  for (j = 1; j < buf[0] - 1; j++) {
    printf (" %8x", (buf[j] & 0xffff));
    if (j%8 == 0 ) printf ("\n");
  }
  printf ("\n");
#endif

  return 0;

}


/*******************************************************************************
 
   Decode event header information
 
*******************************************************************************/

int decode_STRING(short *buf, int length, char* string)
{
  int max_len;
  /* do version checking */
  if ((buf[3] != 0 ) || (buf[4] != 1))
    {
#ifdef OFFLINE_ONLY
      printf(
#else
	     syslog(LOG_LOCAL1|LOG_WARNING, 
#endif
		    "WW get_next_event(): User/site version not supported: %d.%d\n", buf[3],buf[4]);
	     }
  else
    {
      max_len = (*buf-7)*2 < length ? (*buf-7)*2 : length; 
    memcpy(string,buf+5,max_len);
    }
return 0;
}


/******************************************************************************
 
   Decode event header information
 
*******************************************************************************/

int decode_DAQREV(short *buf)
{
  /* do version checking */
  if ((buf[3] != 0 ) || (buf[4] != 1))
    {
#ifdef OFFLINE_ONLY
      printf(
#else
      syslog(LOG_LOCAL1|LOG_WARNING, 
#endif
       	    "WW get_next_event(): User/site version not supported: %d.%d\n", buf[3],buf[4]);
    }
  else
    {
      upevt_.version = version_store = buf[5];
      upevt_.revision = revision_store = buf[6];
    }
return 0;
}

/******************************************************************************
 
   Decode Trigger Mother Board    
 
******************************************************************************/

int decode_TMB(short *buf)
{
  int ibuf = NMOD_HEAD + 1;

#ifdef GNE_DEBUG
  printf("=======================TMB FIFO Dump=======================\n");
  printf("CSR1 = %5hx CSR2 = %5hx\n", buf[ibuf], buf[ibuf + 1]); 
  printf("FIFO status = %hx\n", buf[ibuf + 2]); 
  printf("%5hx %5hx %5hx %5hx", 
    buf[ibuf + 3], buf[ibuf + 4], buf[ibuf + 5], buf[ibuf + 6]); 
  printf(" %5hx %5hx %5hx %5hx\n", 
    buf[ibuf + 7], buf[ibuf + 8], buf[ibuf + 9], buf[ibuf + 10]); 
  printf("FIFO status = %hx\n\n", buf[ibuf + 11]); 
#endif

  upevt_.tmb_alct_valid_patt[0]    =  buf[ibuf + 3] & 0x0001;
  upevt_.tmb_alct_patt_quality[0]  = (buf[ibuf + 3] & 0x0006) >>  1;
  upevt_.tmb_alct_accel_muon[0]    = (buf[ibuf + 3] & 0x0008) >>  3;
  upevt_.tmb_alct_wire_group[0]    = (buf[ibuf + 3] & 0x07f0) >>  4;
  upevt_.tmb_alct_bxn[0]           = (buf[ibuf + 3] & 0xf800) >> 11;
  upevt_.tmb_alct_full_bxn[0]      =  buf[ibuf + 4] & 0x0fff;

  upevt_.tmb_alct_valid_patt[1]    =  buf[ibuf + 5] & 0x0001;
  upevt_.tmb_alct_patt_quality[1]  = (buf[ibuf + 5] & 0x0006) >>  1;
  upevt_.tmb_alct_accel_muon[1]    = (buf[ibuf + 5] & 0x0008) >>  3;
  upevt_.tmb_alct_wire_group[1]    = (buf[ibuf + 5] & 0x07f0) >>  4;
  upevt_.tmb_alct_bxn[1]           = (buf[ibuf + 5] & 0xf800) >> 11;
  upevt_.tmb_alct_full_bxn[1]      =  buf[ibuf + 6] & 0x0fff;

  upevt_.tmb_clct_valid_patt[0]    =  buf[ibuf + 7] & 0x0001;
  /* For LCT99 pattern_number = pattern_quality  */
  upevt_.tmb_clct_patt_number[0]   = (buf[ibuf + 7] & 0x01fe) >>  1;
  upevt_.tmb_clct_bend[0]          = (buf[ibuf + 7] & 0x0200) >>  9;
  /* Half strip split between two words */
  upevt_.tmb_clct_half_strip[0]    = (buf[ibuf + 7] & 0xfc00) >> 10;
  upevt_.tmb_clct_half_strip[0]    = upevt_.tmb_clct_half_strip[0] |
                                     ((buf[ibuf + 8] & 0x0003) << 6);
  upevt_.tmb_clct_bxn[0]           = (buf[ibuf + 8] & 0x007c) >>  2;

  upevt_.tmb_clct_valid_patt[1]    =  buf[ibuf + 9] & 0x0001;
  /* For LCT99 pattern_number = patern_quality  */
  upevt_.tmb_clct_patt_number[1]   = (buf[ibuf + 9] & 0x01fe) >>  1;
  upevt_.tmb_clct_bend[1]          = (buf[ibuf + 9] & 0x0200) >>  9;
  /* Half strip split between two words */
  upevt_.tmb_clct_half_strip[1]    = (buf[ibuf + 9] & 0xfc00) >> 10;
  upevt_.tmb_clct_half_strip[1]    = upevt_.tmb_clct_half_strip[1] |
                                     ((buf[ibuf + 10] & 0x0003) << 6);
  upevt_.tmb_clct_bxn[0]           = (buf[ibuf + 10] & 0x007c) >>  2;

  return 0;
}

/******************************************************************************
 
   Decode Trigger Mother Board, 01 version
 
******************************************************************************/
//#define GNE_DEBUG
int decode_TMB2(short *buf, int ibuf, int version)
{

	int tmb_head_wc, ilink;
	unsigned long clct[2];
#ifdef GNE_DEBUG
	printf("=======================TMB2 FIFO Dump=======================\n");
#endif
  
	upevt_.tmb_sync    = 0; // not clear where to take this
//	upevt_.tmb_stat    = (buf[ibuf + 10] & 0x01f8) >>  5;
	upevt_.tmb_bxn     = (buf[ibuf + 3] & 0x0fff);
	upevt_.tmb_l1a_num = (buf[ibuf + 2] & 0x000f);
	tmb_head_wc = buf[ibuf + 4] & 0x1f;
  
	/* This is just some module number, so set it to 0. */
	upevt_.clct_lct_id        = (buf[ibuf+2] & 0x1f00) >> 8;
	upevt_.clct_csc_id        = (buf[ibuf+2] & 0x00f0) >> 4;
	upevt_.clct_l1a_num       =  buf[ibuf+5] & 0x000f;
//	upevt_.clct_l1a_lct_match = (buf[ibuf+3] & 0x2000) >> 13;
	upevt_.clct_nbucket       = (buf[ibuf+1] & 0x001f);
	upevt_.clct_tr_mode       =  1;
	upevt_.clct_full_bxn      =  buf[ibuf+3] & 0x0fff;
	upevt_.clct_active_febs   =  buf[ibuf+6] & 0x001f;
	upevt_.clct_febs_read     = (buf[ibuf+1] >> 5) & 0x1f;

#ifdef GNE_DEBUG
	printf("==============CLCT2 HEAD===================\n");
	printf("upevt_.clct_lct_id        = %d\n", upevt_.clct_lct_id);
	printf("upevt_.clct_csc_id        = %d\n", upevt_.clct_csc_id);
	printf("upevt_.clct_l1a_num       = %d\n", upevt_.clct_l1a_num);
	printf("upevt_.clct_l1a_lct_match = %d\n", upevt_.clct_l1a_lct_match);
	printf("upevt_.clct_nbucket       = %d\n", upevt_.clct_nbucket);
	printf("upevt_.clct_tr_mode       = %d\n", upevt_.clct_tr_mode);
	printf("upevt_.clct_full_bxn      = %#x\n", upevt_.clct_full_bxn);
	printf("upevt_.clct_active_febs   = %#x\n", upevt_.clct_active_febs);
	printf("upevt_.clct_febs_read     = %#x\n\n", upevt_.clct_febs_read);
//  printf("Cath LCT0                 = %04x\n", (buf[ibuf+6] & 0xff) | ((buf[ibuf+7] & 0xff) << 8));
//  printf("Cath LCT1                 = %04x\n", (buf[ibuf+8] & 0xff) | ((buf[ibuf+9] & 0xff) << 8));
//  printf("LCT0                      = %04x %04x\n", buf[ibuf+6], buf[ibuf+7]);
//  printf("LCT1                      = %04x %04x\n", buf[ibuf+8], buf[ibuf+9]);
		  
#endif

	// construct clcts from data words
	clct[0] = (buf[ibuf+8] & 0x7fff) | ((buf[ibuf+10] & 0x01f) << 15);
	clct[1] = (buf[ibuf+9] & 0x7fff) | ((buf[ibuf+10] & 0x3e0) << 10);

	// split them into fields
	for (ilink = 0; ilink < 2; ilink++) 
    {
		// = VB = Trying to read CLCT event parameters from Sequncer Latched CLCT frames
		upevt_.clct_valid_patt[ilink]   = clct[ilink] & 1;
		upevt_.clct_patt_number[ilink]  = (clct[ilink] >> 4) & 0xf;
		upevt_.clct_bend[ilink]         = (clct[ilink] >> 8) & 1;
		//  Multiplying CLCT Key half-strip on CLCT Key CFEB to get "absolute" halfstrip to satisfy test 26
		// formula = key_half_strip + cfeb*32
		// the bit fields are arranged in such a way that it is done automatically
		upevt_.clct_half_strip[ilink]   = (clct[ilink] >> 9) & 0xff;

		upevt_.clct_bxn[ilink]          = (clct[ilink] >> 17) & 3;
#ifdef GNE_DEBUG

		printf("upevt_.clct_valid_patt[%d]        = %d\n", ilink, upevt_.clct_valid_patt[ilink]);
		printf("upevt_.clct_patt_number[%d]       = %d\n", ilink, upevt_.clct_patt_number[ilink]);
		printf("upevt_.clct_bend[%d]              = %d\n", ilink, upevt_.clct_bend[ilink]);
		printf("upevt_.clct_half_strip[%d]        = %d\n", ilink, upevt_.clct_half_strip[ilink]);
//		printf("upevt_.clct_key_cfeb[%d]          = %d\n", ilink, ((buf[ibuf+7+ilink*2] & 0x0038) >> 3));
		printf("upevt_.clct_bxn[%d]               = %d\n\n", ilink, upevt_.clct_bxn[ilink]);
#endif
		    
    }

/*
  printf("decode_clct: LCT words for event %d are: %8hx %8hx %8hx %8hx\n", 
  upevt_.event_number, buf[ibuf+4], buf[ibuf+5],  buf[ibuf+6], buf[ibuf+7]);
*/

	return tmb_head_wc;
}
//#undef GNE_DEBUG
/******************************************************************************
 
   Extract configuration files from first event of the run
 
******************************************************************************/

int extract_config_file(char *file_name, short *buf)
{

#ifdef OFFLINE_ONLY
	int            fd;
	int            swap_bytes;
	int            iword;
	short          temp;
	struct utsname uname_buf;

	/*  See if analysis running on a little enden or big enden system */
	if (uname(&uname_buf) == -1) 
    {
		printf("Cannot get the system name.\n");
		exit(1);
    }
	if      (strcmp(uname_buf.sysname, "Linux") == 0) 
    {
		swap_bytes = 0;
    }
	else if (strcmp(uname_buf.sysname, "SunOS") == 0) 
    {
		swap_bytes = 1;
    }
	else if (strcmp(uname_buf.sysname, "HP-UX") == 0) 
    {
		swap_bytes = 1;
    }
	else 
    {
		printf("Analysis running on an unknown system.\n");
		printf("Can't decide whether to swap bytes or not.\n");
		exit(1);
    }

	printf("Extracting config file %s\n", file_name);
  
	if((fd = open( file_name, O_CREAT|O_WRONLY, 
				   S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1) 
    {
		printf("Failed to open config file %s for writing.\n", file_name);
		close(fd);
		return -1;
    }

	if (swap_bytes) 
    {
		for (iword = NMOD_HEAD + 1; iword < buf[0] - 1; iword++) 
		{
			temp       = buf[iword];
			buf[iword] = (temp & 0x00ff) << 8;
			buf[iword] = buf[iword] | ((temp & 0xff00) >> 8);
		}
    }

	if ( write(fd, buf + NMOD_HEAD + 1, (buf[0] - NMOD_HEAD - 2)*sizeof(short) ) 
		 != (int )((buf[0] - NMOD_HEAD - 2)*sizeof(short)) ) 
    {
		printf("Error writing config file %s \n", file_name);
		close(fd);
		return -1;
    }

	close(fd);

#endif

	return 0;

}

/******************************************************************************
 
   Decode DDU      
 
******************************************************************************/

int decode_DDU(short *buf)
  {
  int version;
  int alct_wc = 0; 
  int clct_wc = 0;
  int offset  = NMOD_HEAD + (NDDU_HEAD + 1) + 1;

  if (buf[0] <= 7)      /* Empty event */
    {
    return 0;
    }
  else if (buf[4] & 0x8000) 
    {
    #ifdef OFFLINE_ONLY
    printf(
    #else
    syslog(LOG_LOCAL1|LOG_WARNING, 
    #endif
      "WW decode_DDU(): Error flag set. event_number = %ld\n", 
	   upevt_.event_number);
    return 0;
    }

  /* Get the module data version number */
  version = buf[4] & 0xff;

  /* Get TMB words from DDU header */
  upevt_.tmb_sync    = (buf[NMOD_HEAD + 4] & 0x0800) >> 11;
  upevt_.tmb_stat    = (buf[NMOD_HEAD + 4] & 0x07e0) >>  5;
  upevt_.tmb_bxn     = (buf[NMOD_HEAD + 4] & 0x001f);
  upevt_.tmb_l1a_num = (buf[NMOD_HEAD + 7] & 0x0007);

  /* Decode ALCT data (if present) */
  if (buf[NMOD_HEAD + 2] & 0x400) 
    {
    upevt_.alct_active = 1;
    alct_wc = decode_ALCT(buf, offset, version);
    offset += alct_wc;
    }

  /* Decode CLCT data (if present) */
  if (buf[NMOD_HEAD + 2] & 0x800) 
    {
    upevt_.clct_active = 1;
    clct_wc = decode_CLCT(buf, offset, version);
    offset += clct_wc;   

    /* Hack to account for extra word after the CLCT data */
/*  if ((upevt_.run_number > 491) && (upevt_.run_number < 498))
 *    clct_wc++;
 */

    }
 
  /* Decode the CFEB data (if present) */
  if (buf[NMOD_HEAD + 2] & 0x01f) 
    {
    decode_CFEB(buf, offset, version);
    }

  return 0;

}

 void find_DMB_end(short *buf, int *index)
{
	int ew;
	// find dmb end
	for (ew = 0; ew < 30000; ew++)
	{
		if // last 4 DMB words start with 0xe
		(
			((buf[(*index)+0] >> 12) & 0xf) == 0xe &&
			((buf[(*index)+1] >> 12) & 0xf) == 0xe &&
			((buf[(*index)+2] >> 12) & 0xf) == 0xe &&
			((buf[(*index)+3] >> 12) & 0xf) == 0xe 
		) 
			break;
		else
			(*index)++;
	}
	if (ew >= 30000) printf("cannot find DMB block end\n");
	(*index) += 4; // index pointing at the word following this DMB block
}

int decode_DMB (short *buf, int * index)
{
	int dmb_l1a, dmb_bxn, ew, dmb_crate, dmb_id;
	int tmb_dav, alct_dav, cfeb_dav, offset, alct_wc, tmb_head_wc = 0, clct_wc;

	// protection against the extra words in DDU readout (???)
	for (ew = 0; ew < 16; ew++)
	{
		if // first 4 DMB words start with 9
		(
			((buf[(*index)+0] >> 12) & 0xf) == 9 &&
			((buf[(*index)+1] >> 12) & 0xf) == 9 &&
			((buf[(*index)+2] >> 12) & 0xf) == 9 &&
			((buf[(*index)+3] >> 12) & 0xf) == 9 
		) 
			break;
		else
			(*index)++;
	}
	if (ew == 16) // too many extra words, skip event...
	{
		syslog(LOG_LOCAL1|LOG_WARNING, 
			   "WW decode_DMB(): . Too many extra words in front of DMB block, skipping event number = %ld\n", 
			   upevt_.event_number);
		find_DMB_end(buf, index);
		return 0;
		
	}
	dmb_crate = (buf[(*index)+5] >> 4) & 0xff;
	dmb_id = buf[(*index)+5] & 0xf;

	if (crate >= 0 && chamber >= 0 && (crate != dmb_crate || chamber != dmb_id))
	{
		dprint("skip this event, wrong chamber\n"); 
		find_DMB_end(buf, index);
		return 1;
	}


	dprint ("dmb_crate: %d, dmb_id: %d\n", dmb_crate, dmb_id);

	dmb_l1a = (buf[(*index)+0] & 0xfff) | ((buf[(*index)+1] << 12) & 0xfff);
	dprint ("dmb_l1a: %d\n", dmb_l1a);

	dmb_bxn = buf[(*index)+3] & 0xfff;
	dprint ("dmb_bxn: %x\n", dmb_bxn);

	if (((buf[(*index)+0] >> 12) & 0xf) == 8)
	{
		dprint ("DMB lone word is received. No data\n");
		find_DMB_end(buf, index);
		return 1;
	}

	// these flags show whether tmb, alct and cfeb data are available
	tmb_dav  = (buf[(*index)+2] >> 11) & 1;
	alct_dav = (buf[(*index)+2] >> 10) & 1;
	cfeb_dav = buf[(*index)+2] & 0x1f;
	dprint ("tmb_dav: %x  alct_dav: %d  cfeb_dav: %x\n", tmb_dav, alct_dav, cfeb_dav);

	if ((!tmb_dav) && (!alct_dav) && (!cfeb_dav)) 
	{
		dprint("skipping event, no DAV flags\n");
		find_DMB_end(buf, index);
		return 1;
	}

	offset = 8;
	if (alct_dav)
	{
		upevt_.alct_active = 1;

		// check if we have DAQ01 or DAQ06 ALCT format
		if ((buf[(*index)+8] & 0xffff) == 0xdb0a) // word 0 of ALCT data (starting from word 0)
		{
			// DAQ06
			alct_wc = decode_ALCT4(buf+(*index), 8, 0);
		}
		else
		{
			// DAQ01
			alct_wc = decode_ALCT3(buf+(*index), 8, 0);
		}
		if (alct_wc == 0) {find_DMB_end(buf, index);return 4;}
		offset += alct_wc;
	}
	dprint ("ALCT wc : %d\n", alct_wc);

	if (tmb_dav)
	{
		dprint ("TMB header: %x\n", buf[(*index)+offset]);
		tmb_head_wc = decode_TMB2( buf+(*index), offset, 0);
		offset += tmb_head_wc;
		dprint ("TMB header wc : %d\n", tmb_head_wc);
		dprint ("TMB header end: %x\n", buf[(*index)+offset]);
		offset++; // to make it point to first CFEB word
	}

	if (tmb_dav)
	{
		upevt_.clct_active = 1;
		clct_wc = decode_CLCT2( buf+(*index), offset, 0);
		offset += clct_wc;
		dprint ("TMB end marker: %04x\n", buf[offset - 2+(*index)]);
	}


    if (cfeb_dav) 
    {
		decode_CFEB2(buf+(*index), offset, cfeb_dav);
    }

	find_DMB_end(buf, index);
	return 0;
}


int decode_DDU2 (short *buf, int first_time)
{
	int ddu_wc = buf[0];
	int ddu_l1a, i, j;
	static int index;
	int res;

	if (first_time)
	{
		if (ddu_wc <= 29) 
		{
			dprint("empty event, skip\n");
			return 1; // empty event
		}
	
		dprint("DDU word counts: front: %d, rear: %d\n", ddu_wc, buf[ddu_wc-1]);
		dprint ("First 20 words of DDU\n");
		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 4; j++)
				dprint("%04X ", buf[i*4 + j] & 0xffff);

			dprint("\n");
		}
		ddu_l1a = (buf [5] | ((buf[6] & 0xff) << 16)) & 0xffffff;
		dprint ("ddu_l1a: %d\n", ddu_l1a);

		dprint ("DDU header  words: ");
		for (i = 3; i <= 14; i++ ) 
			dprint ("%04X ", buf[i] & 0xffff);
		dprint ("\n");

		dprint ("DDU trailer words: ");
		for (i = ddu_wc - 13; i <= ddu_wc-2; i++ ) 
			dprint ("%04X ", buf[i] & 0xffff);
		dprint ("\n");

		index = 15;
	}

	dprint ("decode DMB index: %d\n", index);
	if (index < ddu_wc - 20)
	{
		res = decode_DMB (buf, &index); 
		return res | 2;
	}
	else return 1; // buffer empty, looks like wrong chamber 

	return 0;
}

/******************************************************************************
 
   Decode DDU2
 
******************************************************************************/

int decode_DDU2_old(short *buf) // used formerly for DDU2 
{
    int version;
    int alct_wc = 0; 
    int clct_wc = 0;
    int alct_block_count;
    /* we don't include NTMB2_HEAD because we need the CLCT stuff out
       of it for the CLCT */
    int offset  = NMOD_HEAD + (NDDU2_HEAD + 1) + 1;
    int ai;

	int ii;
	for (ii = 0; ii < 8; ii++) printf ("%04x ", buf[ii]);
	printf ("\n");

    if (buf[0] <= 7)      /* Empty event */
    {
		return 0;
    }
    else if (buf[4] & 0x8000) 
    {
#ifdef OFFLINE_ONLY
		printf("WW decode_DDU2(): Error flag set. event_number = %ld\n", 
			   upevt_.event_number);
#else
		syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_DDU2(): Error flag set. event_number = %ld\n", 
			   upevt_.event_number); 
#endif
		return 0;
    }

    /* Get the module data version number */
    version = buf[4] & 0xff;

    /* See if the TMB was read out */
    if (buf[ NMOD_HEAD + 2 ] & 0x400)
    {
		/* Get TMB words from DDU header */
		decode_TMB2( buf, offset, version );

		/* Decode the CLCT data (if present) */
		if (buf[ NMOD_HEAD + NDDU2_HEAD + 6] & 0x0f80)
		{
			upevt_.clct_active = 1;
			clct_wc = decode_CLCT2( buf, offset, version);
			offset += clct_wc;
		}
    
		/* Decode ALCT data... */
		alct_block_count = 0;
		/* the first test is the real start-of-data marker,
		   the second test is a kludge, perhaps a bug. */
		while (buf[offset] == (short)0x4b0a || (buf[offset]&0xf000) == 0x6000)
		{
			if ((buf[offset]&0xf000) != 0x6000) offset++;
			for (ai = 0; ai < 10000; ai++) 
			{
				if(buf[offset] == 0) offset++;
				else break;
			}
	    
			if ((buf[offset]&0xf000) != 0x6000)
			{
#ifdef OFFLINE_ONLY
				printf("II decode_DDU2(): ALCT block not found, event %ld\n", 
					   upevt_.event_number);
#else
				syslog(LOG_LOCAL1|LOG_INFO,"II decode_DDU2(): ALCT block not found, event %ld\n", 
					   upevt_.event_number); 
#endif
				break;
			}

			alct_block_count++;
			if (buf[offset] != (short)0x4dda)
			{
				upevt_.alct_active = 1;
				alct_wc = decode_ALCT3(buf, offset, version);
				if (alct_wc == 0) return -1;
				offset += alct_wc;
			}
		}
		if (alct_block_count > 1)
#ifdef OFFLINE_ONLY
			printf("II decode_DDU2(): %d ALCT blocks encountered, event %ld\n", 
				   alct_block_count,upevt_.event_number);
#else
	    syslog(LOG_LOCAL1|LOG_INFO,"II decode_DDU2(): %d ALCT blocks encountered, event %ld\n", 
			   alct_block_count,upevt_.event_number); 
#endif

		/* Do something here to take care of the DDA/DDB make-up patterns */
		if ((buf[offset] & 0xfff) == 0xdda)
		{
			offset ++;
		}
		if ((buf[offset] & 0xfff) == 0xddb)
		{
			offset ++;
		}

		if (buf[offset++] != (short)0xdaaa)
		{
#ifdef OFFLINE_ONLY
			printf("WW decode_DDU2(): DAAA TMB test bit marker failed, event %ld\n", 
				   upevt_.event_number);
#else
			syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_DDU2(): DAAA TMB test bit marker failed, event %ld\n", 
				   upevt_.event_number); 
#endif
			return 0;      
		}
		if (buf[offset++] != (short)0xd555)
		{
#ifdef OFFLINE_ONLY
			printf("WW decode_DDU2(): D555 TMB test bit marker failed, event %ld\n", 
				   upevt_.event_number);
#else
			syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_DDU2(): D555 TMB test bit marker failed, event %ld\n", 
				   upevt_.event_number); 
#endif
			return 0;      
		}

		/* Finish up the TMB scan */
		if (buf[offset++] != (short)0xde0f)
		{
#ifdef OFFLINE_ONLY
			printf("WW decode_DDU2(): Could not find end of TMB marker, event %ld\n", 
				   upevt_.event_number);
#else
			syslog(LOG_LOCAL1|LOG_WARNING, "WW decode_DDU2(): Could not find end of TMB marker, event %ld\n", 
				   upevt_.event_number);
#endif
			return 0;      
		}
		offset ++;
    }

    /* Decode the CFEB data (if present) */
    if (buf[NMOD_HEAD + 2] & 0x01f) 
    {
		decode_CFEB2(buf, offset, version);
    }

    return 0;
}



/******************************************************************************
 
   Decode ALCT99   
 
******************************************************************************/

int decode_ALCT(short *buf, int ibuf, int version)
{
  int j, ilink, ibit, kbit;
  int iafeb, iblock, ilayer, itime, kwire;
  int expected_wc;
  int alct_wc = 0;
  static int    first = 1, tbit[32];

#ifdef GNE_DEBUG
  int febid;
#endif

  upevt_.alct_lct_id        = (buf[ibuf+0] & 0x0700) >> 8;
  upevt_.alct_csc_id        = (buf[ibuf+0] & 0x00f0) >> 4;
  upevt_.alct_l1a_num       =  buf[ibuf+0] & 0x000f;

/* The next two lines work with ALCT 2000 */
   upevt_.alct_l1a_lct_match = (buf[ibuf+1] & 0x0080) >> 7;
   upevt_.alct_nbucket       = (buf[ibuf+1] & 0x007c) >> 2;
 
/* The next two lines work with ALCT 99 */ /*
 * upevt_.alct_l1a_lct_match = (buf[ibuf+1] & 0x0040) >> 6;
 * upevt_.alct_nbucket       = (buf[ibuf+1] & 0x003c) >> 2;
 */
  upevt_.alct_tr_mode       =  buf[ibuf+1] & 0x0003;
  upevt_.alct_full_bxn      =  buf[ibuf+2] & 0x0fff;
  upevt_.alct_active_febs   = (buf[ibuf+3] & 0x3f80) >> 7;
  upevt_.alct_febs_read     =  buf[ibuf+3] & 0x007f;           

/* Make an array of shifted bits to use later */
  if (first) {
    for (j = 0; j < 32; j++) tbit[j] = 1 << j;
    first = 0;
  }

/* Calculate the expected wordcount from the header info */
  expected_wc = ALCT_NO_DUMP_SIZE;
  if (upevt_.alct_tr_mode > 0) {
    for (j = 0; j < 4; j++) {
/*  for (j = 0; j < 7; j++) { */
      if (upevt_.alct_febs_read & (1 << j)) 
       expected_wc += 12 * upevt_.alct_nbucket;
    }
  }

#ifdef GNE_DEBUG
  printf("##############ALCT HEAD###################\n");
  printf("upevt_.alct_lct_id        = %d\n", upevt_.alct_lct_id);
  printf("upevt_.alct_csc_id        = %d\n", upevt_.alct_csc_id);
  printf("upevt_.alct_l1a_num       = %d\n", upevt_.alct_l1a_num);
  printf("upevt_.alct_l1a_lct_match = %d\n", upevt_.alct_l1a_lct_match);
  printf("upevt_.alct_nbucket       = %d\n", upevt_.alct_nbucket);
  printf("upevt_.alct_tr_mode       = %d\n", upevt_.alct_tr_mode);
  printf("upevt_.alct_full_bxn      = %#x\n", upevt_.alct_full_bxn);
  printf("upevt_.alct_active_febs   = %#x\n", upevt_.alct_active_febs);
  printf("upevt_.alct_febs_read     = %#x\n", upevt_.alct_febs_read);
  printf("expected alct word count  = %d\n\n", expected_wc);
#endif

  /* ilink=0 -> Best muon aLCT, ilink=1 -> Second Best muon aLCT */
  for (ilink = 0; ilink < 2; ilink++) {
    upevt_.alct_valid_patt[ilink]   =  buf[ibuf+4+ilink*2] & 0x0001;
    upevt_.alct_patt_quality[ilink] = (buf[ibuf+4+ilink*2] & 0x0006) >>  1;
    upevt_.alct_accel_muon[ilink]   = (buf[ibuf+4+ilink*2] & 0x0008) >>  3;
    upevt_.alct_wire_group[ilink]   = (buf[ibuf+4+ilink*2] & 0x07f0) >>  4;
    upevt_.alct_bxn[ilink]          = ((buf[ibuf+4+ilink*2] & 0x3800) >> 11)
                                    | ((buf[ibuf+5+ilink*2] & 0x0003) <<  3);

#ifdef GNE_DEBUG
    printf("LCT%d:\n", ilink);
    printf("upevt_.alct_valid_patt[%d]   = %d\n", ilink,
                                            upevt_.alct_valid_patt[ilink]);
    printf("upevt_.alct_patt_quality[%d] = %d\n", ilink,
                                            upevt_.alct_patt_quality[ilink]);
    printf("upevt_.alct_accel_muon[%d]   = %d\n", ilink,
                                            upevt_.alct_accel_muon[ilink]);
    printf("upevt_.alct_wire_group[%d]   = %d\n", ilink, 
                                            upevt_.alct_wire_group[ilink]);
    printf("upevt_.alct_bxn[%d]          = %#x\n", ilink, 
                                            upevt_.alct_bxn[ilink]);
#endif

  }

  if (upevt_.alct_tr_mode == 0) {  /* No-Dump mode */
    alct_wc = buf[ibuf+9] & 0x03ff;
    if (alct_wc != ALCT_NO_DUMP_SIZE) {
      #ifdef OFFLINE_ONLY
      printf(
      #else
      syslog(LOG_LOCAL1|LOG_WARNING, 
      #endif
       "WW decode_ALCT(): Incorrect frame count in event %ld for No-Dump mode: "
       "alct_wc = %d\n", upevt_.event_number, alct_wc);
    }
    return alct_wc;
  }

  /* If we get here we have Full-Dump or Local-Dump mode */
  alct_wc = ALCT_NO_DUMP_SIZE - 2;

  for (iafeb = 0; iafeb < NAFEB; iafeb++) {
    if (upevt_.alct_febs_read & (0x1 << iafeb)) {
      for (itime = 0; itime < upevt_.alct_nbucket; itime++) {
        for (ilayer = 0; ilayer < NLAYER; ilayer++) {
          for (iblock = 0; iblock < 2; iblock++) {
            for (ibit = 0; ibit < 8; ibit++) { 

              kbit = ibit;
              kwire = ibit + 8*iblock + 16*iafeb;

              #ifdef GNE_DEBUG
              printf("AFEB = %d, FEBID = %d, TBIN = %d, LAY = %d, WG = %d\n",
                iafeb, febid, itime, ilayer, kwire);
              #endif

/*
              if ((ilayer >= NLAYER) || (kwire >= NWIRE) ||
                  (itime  >= upevt_.alct_nbucket) ) {
                printf("DANGER: ilayer = %d, kwire = %d, itime = %d\n",
                  ilayer, kwire, itime);
                exit(1);
              }
*/
              if (buf[ibuf+alct_wc] & (0x1 << kbit)) {
                upevt_.alct_dump[ilayer][kwire] |= tbit[itime];
#ifdef GNE_DEBUG 
                if ((!(ilayer == 1 && kwire == 48)) && 
                    (!(ilayer == 1 && kwire == 49)) && 
                    (!(ilayer == 2 && kwire == 36))) {
/*              printf("Found a hit on layer %d, wire %d, in time bin %d; "
 *               "tbit=%#x, alct_dump=%#x\n", ilayer+1, kwire+1, itime, 
 *               tbit[itime], upevt_.alct_dump[ilayer][kwire] );
 */
                }
#endif 
              }
            }  
            alct_wc++;
          }
        }
      }
    }
  }

  /* Two Extra trailer words */
  if ((buf[ibuf+alct_wc] & 0xfff) != 0xe0d) {
    printf ("decode_alct: at end of data expected 0xe0d, found %#x\n", 
     buf[ibuf+alct_wc]);
    alct_wc++;
  }

  else {
    alct_wc++;
    if ((upevt_.alct_tr_mode == 1) &&  			/* Full-Dump mode */
     ((buf[ibuf+alct_wc] & 0x03ff) != (expected_wc & 0x3ff)) ) {  
      #ifdef OFFLINE_ONLY
      printf(
      #else
      syslog(LOG_LOCAL1|LOG_WARNING, 
      #endif
       "WW decode_ALCT(): Incorrect frame count in event %ld for Full-Dump "
       "mode: alct_wc = %d, Frame Count = %d, expected word count = %d\n", 
       upevt_.event_number, alct_wc+1, buf[ibuf+alct_wc] & 0x03ff, expected_wc);
    }
  }

  /* In order to return the correct word count */
  alct_wc++; 

  return alct_wc;

}

unsigned long crc22(unsigned short int *d, int length)
{
    int i, j, t;
    unsigned long crc = 0, ncrc;
    unsigned short w;

    for (j = 0; j < length; j++)
    {
		w = d[j]; // & 0xdfff;
		for (i = 15; i >= 0; i--)
		{
			t = ((w >> i) & 1) ^ ((crc >> 21) & 1);
			ncrc = (crc << 1) & 0x3ffffc;
			ncrc |= (t ^ (crc & 1)) << 1;
			ncrc |= t;
			crc = ncrc;
		}
    }
    return crc;
}



//#define GNE_DEBUG
//#define DEBUG_CRC
//#define MISSING_DATA_DEBUG
#define CRC_DEBUG

/*****************************************************************************
 
   Decode ALCT-DAQ06
 
*****************************************************************************/
int decode_ALCT4(short *buf, int ibuf, int version)
{
    int j, ilink;
    int expected_wc;
    int alct_wc = 0;
    int time_bit;
    unsigned long crc = 0, lcrc;
    int correct_wg = 0;
	static int event_num = 0;
	static int ignored_event_num = 0;

	int alct_wgn, alct_config_report, alct_lctbucket;
	int alct_lct_of;
	int alct_raw_of;
	int lctw, raww, lcti, crcw, tbi, lyi, lyp, w12, wi;

#ifdef MISSING_DATA_DEBUG
	FILE* out;
#endif

#ifdef CRC_DEBUG
	FILE* out;
	int ii;
#endif
    
#ifdef GNE_DEBUG
    int /*tb, ly, wg, */mwg = 0;
#endif

	// legacy active FEBs flags for all types of ALCTs
	int active_febs[]     = {0, 3,   7, 0xf, 0, 0x3f, 0x7f};
	// raw hit time bin lengths for all types of ALCTs
	int rawbin_length[]   = {0, 18, 24,  36, 0,   48,   60};
	// collision mask lengths for all types of ALCTs
	int collmask_length[] = {0, 8,  12,  16, 0,   24,   28};
	// number of layer parts for each ALCT type
	int ly_prt[] = {0, 3, 4, 6, 0, 8, 10};

	alct_wgn                  = buf[ibuf+6] & 0x7;
	alct_config_report        = (buf[ibuf+4] >> 14) & 1;
	alct_lct_of               = (buf[ibuf+4] >> 13) & 1;
	alct_raw_of               = (buf[ibuf+4] >> 12) & 1;
	alct_lctbucket           = (buf[ibuf+7] >> 5) & 0xf;

    upevt_.alct_lct_id        = 0;
    upevt_.alct_csc_id        = 0;
    upevt_.alct_l1a_num       = buf[ibuf+2] & 0x0fff;
    
    upevt_.alct_l1a_lct_match = 1;
    upevt_.alct_nbucket       = buf[ibuf+7] & 0x001f;
    upevt_.alct_tr_mode       = 0; // pick up from config register if available
    
    upevt_.alct_full_bxn      =  buf[ibuf+4] & 0x0fff;
    
    upevt_.alct_active_febs   = active_febs[alct_wgn];
    upevt_.alct_febs_read     = active_febs[alct_wgn];

	// determine chamber type from wiregroups (dirty, but there is no chamber type in datafile)
	switch(upevt_.alct_active_febs)
	{
	case 0x3:  upevt_.chamber_type_id = 13; mwg = 32; break;
	case 0x7:  upevt_.chamber_type_id = 11; mwg = 48; break;
	case 0xf:  upevt_.chamber_type_id = 22; mwg = 64; break;
	case 0x3f: upevt_.chamber_type_id = 31; mwg = 96; break;
	case 0x7f: upevt_.chamber_type_id = 21; mwg = 112; break;
	}
 
/* Calculate the expected wordcount from the header info */
    expected_wc = 8;

	if (alct_config_report)
		expected_wc += 
			3 + // virtex id
			5 + // config register
			collmask_length[alct_wgn] + 
			rawbin_length[alct_wgn];

	lctw = expected_wc; // here lct words start

	if (!alct_lct_of) // no LCT buffer overflow
		expected_wc += 2 * alct_lctbucket; // each lctbucket takes 2 words

	raww = expected_wc; // here raw data start

	if (!alct_raw_of) // no raw buffer overflow
		expected_wc += rawbin_length[alct_wgn] * upevt_.alct_nbucket;

	crcw = expected_wc + 1; // crc starts here

	expected_wc += 3; // skip CRC

	// end marker does not match - corrupt data - ignore
    if  ((buf[ibuf+expected_wc-3] & 0xfff) != 0xe0d) 
	{
		printf("decode_ALCT4(): Trailer is not E0D (%04x), event %ld, ignored: %d "
			   "\n", buf[ibuf+expected_wc-3] & 0xffff, upevt_.event_number, ignored_event_num++);

		// simply scan to find trailer (de0d), ignoring any data
		for (j = 0; j < 10000; j++)
		{
			if ((((unsigned short)buf[ibuf+ (++alct_wc)]) & 0xffff) == 0xde0d) break;
		}
		if (((unsigned short)buf[ibuf + alct_wc]) != 0xde0d || j == 10000)
		{
			dprint("decode_ALCT3: cannot find ALCT DE0D marker in corrupt data\n");
			return 0;
		}

		// found trailer, skip CRC and word count
		alct_wc += 4;
		return alct_wc;
	}
/*
	printf
    (
		"DAQ06 L1A#: %d, wgn: %d, cp-lof-rof: %d%d%d, rawbins: %d, trigbins: %d, bxn: %03x, expected_wc: %d raww :%d\n",
		upevt_.alct_l1a_num, 
		alct_wgn,
		alct_config_report,
		alct_lct_of,
		alct_raw_of,
		upevt_.alct_nbucket,
		alct_lctbucket,
		upevt_.alct_full_bxn,
		expected_wc,
		raww
	);
*/

#ifdef GNE_DEBUG
    printf("##############ALCT3 HEAD###################\n");
    printf("upevt_.chamber_type_id    = %d\n", upevt_.chamber_type_id);
    printf("upevt_.alct_lct_id        = %d\n", upevt_.alct_lct_id);
    printf("upevt_.alct_csc_id        = %d\n", upevt_.alct_csc_id);
    printf("upevt_.alct_l1a_num       = %d\n", upevt_.alct_l1a_num);
    printf("upevt_.alct_l1a_lct_match = %d\n", upevt_.alct_l1a_lct_match);
    printf("upevt_.alct_nbucket       = %d\n", upevt_.alct_nbucket);
    printf("upevt_.alct_tr_mode       = %d\n", upevt_.alct_tr_mode);
    printf("upevt_.alct_full_bxn      = %#x\n", upevt_.alct_full_bxn);
    printf("upevt_.alct_active_febs   = %#x\n", upevt_.alct_active_febs);
    printf("upevt_.alct_febs_read     = %#x\n", upevt_.alct_febs_read);
    printf("expected alct word count  = %d\n\n", expected_wc);
#endif

	// 08/09/07
	// the commented code below removed as the special ALCT versions for all types of chambers are used now
	// Madorsky 2/2/7
	// do this correction only if the firmware type used on chamber is wrong (i.e. 288 instead of 192)
    // Madorsky 05/29/03
    // little modification for chambers 1/3, 3/1, 4/1, where not all ALCT inputs are used
    // All wires must be shifted by 16 positions, so wire 16 becomes wire 1
/*    if ((upevt_.chamber_type_id == 13 && alct_wgn != 1) ||
		(upevt_.chamber_type_id == 31 && alct_wgn != 5) ||
		(upevt_.chamber_type_id == 41 && alct_wgn != 5))
    {
		correct_wg = 1;
    }
*/
	// number of output lcts in upevt_ is limited by 2 now (ilink < 2)
	// need to increase up to maximum possible
	for 
	(
		lcti = lctw, ilink = 0; 
		lcti < raww && ilink < 2; 
		lcti++
	)
	{
		if (buf[ibuf+lcti] & 0x0001) // if valid lct
		{
			upevt_.alct_valid_patt[ilink]   =  1;
			upevt_.alct_patt_quality[ilink] = (buf[ibuf+lcti] & 0x0006) >>  1;
			upevt_.alct_accel_muon[ilink]   = (buf[ibuf+lcti] & 0x0008) >>  3;
			upevt_.alct_wire_group[ilink]   = (buf[ibuf+lcti] & 0x0fe0) >>  5;

			// if ME13, 31, 41, correct wire group
			if (correct_wg) upevt_.alct_wire_group[ilink] -= 16;
			// bxn for an lct is calculated as full_bxn + number of lct bin
			// should take remainder from division of this number by number of LHC clocks per orbit
			upevt_.alct_bxn[ilink]          = upevt_.alct_full_bxn + lcti;
	
#ifdef GNE_DEBUG
			printf("LCT%d:\n", ilink);
			printf("upevt_.alct_valid_patt[%d]   = %d\n", ilink,
				   upevt_.alct_valid_patt[ilink]);
			printf("upevt_.alct_patt_quality[%d] = %d\n", ilink,
				   upevt_.alct_patt_quality[ilink]);
			printf("upevt_.alct_accel_muon[%d]   = %d\n", ilink,
				   upevt_.alct_accel_muon[ilink]);
			printf("upevt_.alct_wire_group[%d]   = %d\n", ilink, 
				   upevt_.alct_wire_group[ilink]);
			printf("upevt_.alct_bxn[%d]          = %#x\n", ilink, 
				   upevt_.alct_bxn[ilink]);
#endif
			ilink++; // take output parameters for next lct
		}
	}

    /* make copies for what the tmb used to present */
    /* Here is the philosophy.  This stuff  may be used somewhere in 
       analysis.  Although it is technically wrong to do this, we don't
       want to break the analysis code, so we use this patch. */
/*
    for (ilink = 0 ; ilink < 2 ; ilink ++)
    {
		upevt_.tmb_alct_valid_patt[ilink]    = upevt_.alct_valid_patt[ilink];
		upevt_.tmb_alct_patt_quality[ilink]  = upevt_.alct_patt_quality[ilink];
		upevt_.tmb_alct_accel_muon[ilink]    = upevt_.alct_accel_muon[ilink];
		upevt_.tmb_alct_wire_group[ilink]    = upevt_.alct_wire_group[ilink];
		upevt_.tmb_alct_bxn[ilink]           = upevt_.alct_bxn[ilink];
		upevt_.tmb_alct_full_bxn[ilink]      = upevt_.alct_full_bxn;
    }
*/
    /* If we get here we have Full-Dump or Local-Dump mode */
    alct_wc = raww;

	for (tbi = 0; tbi < upevt_.alct_nbucket; tbi++) // time bin loop
	{
		time_bit = 1 << tbi;
		for (lyi = 0; lyi < 6; lyi++) // layer loop
		{
			for (lyp = 0; lyp < ly_prt[alct_wgn]; lyp++) // layer part loop
			{
				// pick up 12 wires from raw data
				w12 = buf[ibuf + alct_wc + tbi*rawbin_length[alct_wgn] + lyi*ly_prt[alct_wgn] + lyp] & 0xfff;
				// loop over each wire
				for (wi = 0; wi < 12; wi++) // wire loop
				{
					if ((w12 >> wi) & 1)
						upevt_.alct_dump[lyi][lyp*12 + wi] |= time_bit;
				}
			}
		}
	}


// crc    
	alct_wc = crcw;

	crc = buf[ibuf+alct_wc] & 0x7ff;
	alct_wc++;

	crc |= ((unsigned long)(buf[ibuf+alct_wc] & 0x7ff)) << 11;
	alct_wc++;
	expected_wc += 2;

	lcrc = crc22(&buf[ibuf], alct_wc - 3); // don't take last 4 words into CRC calculation

	if (crc != lcrc) 
	{
		printf ("decode_ALCT4: CRC mismatch: hw = %08lx sw = %08lx event = %ld\n", crc, lcrc, upevt_.event_number);

#ifdef CRC_DEBUG
		out = fopen("bad_alct.txt", "a");
		for (ii = ibuf; ii < ibuf + alct_wc+1; ii++)
		{
			fprintf (out, "%04x\n", buf[ii] & 0xffff);
		}
		fclose(out);
//		exit(0);
#endif
	}

    /* Two Extra trailer words */
    if ((buf[ibuf+alct_wc-3] & 0xfff) != 0xe0d) 
    {
		printf 
			(
				"decode_ALCT4: at end of data expected 0xe0d, found %#x, event: %d\n", 
				buf[ibuf+alct_wc-3],
				event_num
				);
		alct_wc++;

#ifdef MISSING_DATA_DEBUG
		out = fopen("bad_alct.txt", "w");
		for (ii = ibuf; ii < ibuf + alct_wc; ii++)
		{
			fprintf (out, "%04x\n", buf[ii] & 0xffff);
		}
		fclose(out);
		exit(0);
#endif

    }
    else 
    {

#ifdef MISSING_DATA_DEBUG
		out = fopen("good_alct.txt", "w");
		for (ii = ibuf; ii < ibuf + alct_wc; ii++)
		{
			fprintf (out, "%04x\n", buf[ii] & 0xffff);
		}
		fclose(out);
#endif
//		alct_wc++;
		if ((upevt_.alct_tr_mode == 1) &&  			/* Full-Dump mode */
			((buf[ibuf+alct_wc] & 0x03ff) != (expected_wc & 0x3ff)) ) 
		{
			printf("decode_ALCT4(): Incorrect frame count in event %ld for "
				   "Full-Dump mode: alct_wc = %d, Frame Count = %d, expected "
				   "word count = %d\n", 
				   upevt_.event_number, alct_wc+1, 
				   buf[ibuf+alct_wc] & 0x03ff, expected_wc);
		}
    }

#ifdef NODEF //GNE_DEBUG

    for (tb = -1; tb < upevt_.alct_nbucket; tb++)
    {
		for (ly = 0; ly < 6; ly++)
		{
			for (wg = 0; wg < mwg; wg++)
			{
				if (tb == -1)
					printf ("%c", (upevt_.alct_dump[ly][wg] != 0) ? '#' : '.');
				else
					printf ("%c", (((upevt_.alct_dump[ly][wg] >> tb) & 1) == 1) ? '#' : '.');
			}
			printf("\n");
		}
		printf("\n");
    }
	printf ("nbucket: %d, mwg: %d\n", upevt_.alct_nbucket, mwg);
#endif


    /* In order to return the correct word count */
    alct_wc++; 
	event_num++;
    return alct_wc;
}



/*****************************************************************************
 
   Decode ALCT-DAQ01
 
*****************************************************************************/
int decode_ALCT3(short *buf, int ibuf, int version)
{
    int j, ilink, iword;
    int expected_wc;
    int alct_wc = 0;
    int wire_region, layer, time_bucket, time_bit, subwire;
    unsigned long crc = 0, lcrc;
    int correct_wg = 0;
	static int event_num = 0;
	static int ignored_event_num = 0;
#ifdef MISSING_DATA_DEBUG
	FILE* out;
#endif
    
#ifdef GNE_DEBUG
    int tb, ly, wg, mwg = 0;
#endif

    upevt_.alct_lct_id        = (buf[ibuf+0] & 0x0700) >> 8;
    upevt_.alct_csc_id        = (buf[ibuf+0] & 0x00f0) >> 4;
    upevt_.alct_l1a_num       =  buf[ibuf+0] & 0x000f;
    
    upevt_.alct_l1a_lct_match = (buf[ibuf+1] & 0x0080) >> 7;
    upevt_.alct_nbucket       = (buf[ibuf+1] & 0x007c) >> 2;
    upevt_.alct_tr_mode       =  buf[ibuf+1] & 0x0003;
    
    upevt_.alct_full_bxn      =  buf[ibuf+2] & 0x0fff;
    
    upevt_.alct_active_febs   = (buf[ibuf+3] & 0x3f80) >> 7;
    upevt_.alct_febs_read     =  buf[ibuf+3] & 0x007f;           
    
/* Calculate the expected wordcount from the header info */
    expected_wc = NLCT_HEAD + 2;
    if (upevt_.alct_tr_mode > 0) 
    {
		for (j = 0; j < 7; j++) 
		{
			if (upevt_.alct_febs_read & (1 << j)) 
			{
				expected_wc += 12 * upevt_.alct_nbucket;
#ifdef GNE_DEBUG
				mwg += 16; // max wiregroup number
#endif
			}
		}
    }

	// determine chamber type from wiregroups (dirty, but there is no chamber type in datafile)
	switch(upevt_.alct_active_febs)
	{
	case 0x3:  upevt_.chamber_type_id = 13; break;
	case 0x7:  upevt_.chamber_type_id = 11; break;
	case 0xf:  upevt_.chamber_type_id = 22; break;
	case 0x3f: upevt_.chamber_type_id = 31; break;
	case 0x7f: upevt_.chamber_type_id = 21; break;
	}

	// end marker does not match - corrupt data - ignore
    if  ((buf[ibuf+expected_wc] & 0xfff) != 0xe0d) 
	{
#ifdef OFFLINE_ONLY
		printf("WW decode_ALCT3(): Trailer is not E0D (%04x), event %ld, ignored: %d "
			   "\n", buf[ibuf+expected_wc] & 0xffff, upevt_.event_number, ignored_event_num++);
#else
		syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_ALCT3(): Trailer is not E0D (%04x), event %ld ignored: %d "
			   "\n", buf[ibuf+expected_wc] & 0xffff, upevt_.event_number, ignored_event_num++);
#endif
		// simply scan to find trailer (de0f), ignoring any data
		for (j = 0; j < 10000; j++)
		{
			if ((((unsigned short)buf[ibuf+ (++alct_wc)]) & 0xffff) == 0xde0d) break;
		}
		if (((unsigned short)buf[ibuf + alct_wc]) != 0xde0d || j == 10000)
		{
			dprint("decode_ALCT3: cannot find ALCT DE0D marker in corrupt data\n");
			return 0;
		}

		// found trailer, skip word count
		alct_wc += 2;
		return alct_wc;
	}
#ifdef GNE_DEBUG
    printf("##############ALCT3 HEAD###################\n");
    printf("upevt_.chamber_type_id    = %d\n", upevt_.chamber_type_id);
    printf("upevt_.alct_lct_id        = %d\n", upevt_.alct_lct_id);
    printf("upevt_.alct_csc_id        = %d\n", upevt_.alct_csc_id);
    printf("upevt_.alct_l1a_num       = %d\n", upevt_.alct_l1a_num);
    printf("upevt_.alct_l1a_lct_match = %d\n", upevt_.alct_l1a_lct_match);
    printf("upevt_.alct_nbucket       = %d\n", upevt_.alct_nbucket);
    printf("upevt_.alct_tr_mode       = %d\n", upevt_.alct_tr_mode);
    printf("upevt_.alct_full_bxn      = %#x\n", upevt_.alct_full_bxn);
    printf("upevt_.alct_active_febs   = %#x\n", upevt_.alct_active_febs);
    printf("upevt_.alct_febs_read     = %#x\n", upevt_.alct_febs_read);
    printf("expected alct word count  = %d\n\n", expected_wc);
#endif

	// 08/09/07
	// the commented code below removed as the special ALCT versions for all types of chambers are used now
	// Madorsky 05/29/03
    // little modification for chambers 1/3, 3/1, 4/1, where not all ALCT inputs are used
    // All wires must be shifted by 16 positions, so wire 16 becomes wire 1
/*    if (upevt_.chamber_type_id == 13 ||
		upevt_.chamber_type_id == 31 ||
		upevt_.chamber_type_id == 41)
    {
		correct_wg = 1;
    }
*/
    /* ilink=0 -> Best muon aLCT, ilink=1 -> Second Best muon aLCT */
    for (ilink = 0; ilink < 2; ilink++) 
    {
		upevt_.alct_valid_patt[ilink]   =  buf[ibuf+4+ilink*2] & 0x0001;
		upevt_.alct_patt_quality[ilink] = (buf[ibuf+4+ilink*2] & 0x0006) >>  1;
		upevt_.alct_accel_muon[ilink]   = (buf[ibuf+4+ilink*2] & 0x0008) >>  3;
		/* warning: which ALCT firmware do you have? 
		   The next line could be & 0x07f ) >> 4 ;  */
		upevt_.alct_wire_group[ilink]   = (buf[ibuf+4+ilink*2] & 0x0fe0) >>  5;

		// if ME13, 31, 41, correct wire group
		if (correct_wg) upevt_.alct_wire_group[ilink] -= 16;

		upevt_.alct_bxn[ilink]          = ((buf[ibuf+4+ilink*2] & 0x3800) >> 11)
			| ((buf[ibuf+5+ilink*2] & 0x0003) <<  3);
	
#ifdef GNE_DEBUG
		printf("LCT%d:\n", ilink);
		printf("upevt_.alct_valid_patt[%d]   = %d\n", ilink,
			   upevt_.alct_valid_patt[ilink]);
		printf("upevt_.alct_patt_quality[%d] = %d\n", ilink,
			   upevt_.alct_patt_quality[ilink]);
		printf("upevt_.alct_accel_muon[%d]   = %d\n", ilink,
			   upevt_.alct_accel_muon[ilink]);
		printf("upevt_.alct_wire_group[%d]   = %d\n", ilink, 
			   upevt_.alct_wire_group[ilink]);
		printf("upevt_.alct_bxn[%d]          = %#x\n", ilink, 
			   upevt_.alct_bxn[ilink]);
#endif
    }

    /* make copies for what the tmb used to present */
    /* Here is the philosophy.  This stuff  may be used somewhere in 
       analysis.  Although it is technically wrong to do this, we don't
       want to break the analysis code, so we use this patch. */
    for (ilink = 0 ; ilink < 2 ; ilink ++)
    {
		upevt_.tmb_alct_valid_patt[ilink]    = upevt_.alct_valid_patt[ilink];
		upevt_.tmb_alct_patt_quality[ilink]  = upevt_.alct_patt_quality[ilink];
		upevt_.tmb_alct_accel_muon[ilink]    = upevt_.alct_accel_muon[ilink];
		upevt_.tmb_alct_wire_group[ilink]    = upevt_.alct_wire_group[ilink];
		upevt_.tmb_alct_bxn[ilink]           = upevt_.alct_bxn[ilink];
		upevt_.tmb_alct_full_bxn[ilink]      = upevt_.alct_full_bxn;
    }

    if (upevt_.alct_tr_mode == 0)   /* No-Dump mode */
    {
		alct_wc = buf[ibuf+9] & 0x07ff; // word count is 11 bits now
		if (alct_wc != ALCT_NO_DUMP_SIZE)
		{
#ifdef OFFLINE_ONLY
			printf("WW decode_ALCT3(): Incorrect frame count in event %ld for No-Dump "
				   "mode: alct_wc = %d\n", upevt_.event_number, alct_wc);
#else
			syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_ALCT3(): Incorrect frame count in event %ld for No-Dump "
				   "mode: alct_wc = %d\n", upevt_.event_number, alct_wc); 
#endif
		}
		return alct_wc;
    }




    
    /* If we get here we have Full-Dump or Local-Dump mode */
    alct_wc = NLCT_HEAD;

    /* these people had a way freaky technique for decoding 
       things... using nested for loops ad nauseam...
       yes, it may be faster, but not really with today's
       cpu's and L2...
       
       here's the modulo way... */
    for (iword = 0 ; iword < expected_wc-NLCT_HEAD-2 ; iword ++ )
    {
		/* do some precalculations: */
		/* our wire_region is the region of wires described by
		   the data in question */
		wire_region = 
			(iword/(12*upevt_.alct_nbucket))*16 + (iword&0x1)*8;
		/* layer is obvious */
		layer = (iword/2) % 6;
		/* the time for this bucket is found in the upper half */
		time_bucket = (buf[ibuf+alct_wc]&0x1f00)>>8;
		/* the time bit we'll be setting is determined by the time
		   bucket */
		time_bit = 1 << time_bucket;
	
		// if ME13, 31, 41, correct wire group
		if (correct_wg) wire_region -= 16;
	
		if (wire_region >= 0)
		{
			/* finally, loop over the eight wires in this byte */
			for ( subwire = 0 ; subwire < 8 ; subwire ++ )
			{
				if (buf[ibuf+alct_wc]&(1<<subwire))
					upevt_.alct_dump[layer][wire_region+subwire] |= time_bit;
			}
		}

		alct_wc ++;

    }
// crc    
    if ((buf[ibuf+alct_wc] & 0xfff) != 0xe0d) 
    {
		crc = buf[ibuf+alct_wc] & 0x7ff;
		alct_wc++;

		crc |= ((unsigned long)(buf[ibuf+alct_wc] & 0x7ff)) << 11;
		alct_wc++;
		expected_wc += 2;

		lcrc = crc22(&buf[ibuf], alct_wc - 2);

#ifdef DEBUG_CRC
		printf ("crc  = %08lx\n", crc);
		printf ("lcrc = %08lx\n", lcrc);
#endif

    }

    /* Two Extra trailer words */
    if ((buf[ibuf+alct_wc] & 0xfff) != 0xe0d) 
    {
		printf 
			(
				"decode_ALCT3: at end of data expected 0xe0d, found %#x, event: %d\n", 
				buf[ibuf+alct_wc],
				event_num
				);
		alct_wc++;

#ifdef MISSING_DATA_DEBUG
		out = fopen("bad_alct.txt", "w");
		for (ii = ibuf; ii < ibuf + alct_wc; ii++)
		{
			fprintf (out, "%04x\n", buf[ii] & 0xffff);
		}
		fclose(out);
		exit(0);
#endif

    }
    else 
    {

#ifdef MISSING_DATA_DEBUG
		out = fopen("good_alct.txt", "w");
		for (ii = ibuf; ii < ibuf + alct_wc; ii++)
		{
			fprintf (out, "%04x\n", buf[ii] & 0xffff);
		}
		fclose(out);
#endif
		alct_wc++;
		if ((upevt_.alct_tr_mode == 1) &&  			/* Full-Dump mode */
			((buf[ibuf+alct_wc] & 0x03ff) != (expected_wc & 0x3ff)) ) 
		{
#ifdef OFFLINE_ONLY
			printf("WW decode_ALCT3(): Incorrect frame count in event %ld for "
				   "Full-Dump mode: alct_wc = %d, Frame Count = %d, expected "
				   "word count = %d\n", 
				   upevt_.event_number, alct_wc+1, 
				   buf[ibuf+alct_wc] & 0x03ff, expected_wc);
#else
            syslog(LOG_LOCAL1|LOG_WARNING,"WW decode_ALCT3(): Incorrect frame count in event %ld for "
				   "Full-Dump mode: alct_wc = %d, Frame Count = %d, expected "
				   "word count = %d\n", 
				   upevt_.event_number, alct_wc+1, 
				   buf[ibuf+alct_wc] & 0x03ff, expected_wc); 
#endif
		}
    }

#ifdef GNE_DEBUG
    for (tb = -1; tb < upevt_.alct_nbucket; tb++)
    {
		for (ly = 0; ly < 6; ly++)
		{
			for (wg = 0; wg < mwg; wg++)
			{
				if (tb == -1)
					printf ("%c", (upevt_.alct_dump[ly][wg] != 0) ? '#' : '.');
				else
					printf ("%c", (((upevt_.alct_dump[ly][wg] >> tb) & 1) == 1) ? '#' : '.');
			}
			printf("\n");
		}
		printf("\n");
    }
#endif


    /* In order to return the correct word count */
    alct_wc++; 
	event_num++;
    return alct_wc;
}
#undef  GNE_DEBUG
#undef  DEBUG_CRC
/******************************************************************************
 
   Decode CLCT99   
 
******************************************************************************/
  
int decode_CLCT(short *buf, int ibuf, int version)
{
  int j, iblock, ibit, kbit;
  int ilink, icfeb, itime, ilayer;
  int expected_wc, kdouble_strip;
  int clct_wc = 0;
  static int    first = 1, tbit[32];

  /* Valid for the 1999 beam test */
  int layer_map[6] = {2, 0, 4, 5, 3, 1};

  upevt_.clct_lct_id        = (buf[ibuf+0] & 0x0700) >> 8;
  upevt_.clct_csc_id        = (buf[ibuf+0] & 0x00f0) >> 4;
  upevt_.clct_l1a_num       =  buf[ibuf+0] & 0x000f;
  upevt_.clct_l1a_lct_match = (buf[ibuf+1] & 0x0040) >> 6;
  upevt_.clct_nbucket       = (buf[ibuf+1] & 0x003c) >> 2;
  upevt_.clct_tr_mode       =  buf[ibuf+1] & 0x0003;
  upevt_.clct_full_bxn      =  buf[ibuf+2] & 0x0fff;
  upevt_.clct_active_febs   = (buf[ibuf+3] & 0x0f80) >> 7;
  upevt_.clct_febs_read     =  buf[ibuf+3] & 0x001f;           

/* Make an array of shifted bits to use later */
  if (first) {
    for (j = 0; j < 32; j++) tbit[j] = 1 << j;
    first = 0;
  }

/* Calculate the expected wordcount from the header info */
  expected_wc = CLCT_NO_DUMP_SIZE;
  if (upevt_.clct_tr_mode > 0) {
    for (j = 0; j < 5; j++) {
      if (upevt_.clct_febs_read & (1 << j)) 
       expected_wc += 6 * upevt_.clct_nbucket;
    }
  }
  
#ifdef GNE_DEBUG
  printf("==============CLCT HEAD===================\n");
  printf("upevt_.clct_lct_id        = %d\n", upevt_.clct_lct_id);
  printf("upevt_.clct_csc_id        = %d\n", upevt_.clct_csc_id);
  printf("upevt_.clct_l1a_num       = %d\n", upevt_.clct_l1a_num);
  printf("upevt_.clct_l1a_lct_match = %d\n", upevt_.clct_l1a_lct_match);
  printf("upevt_.clct_nbucket       = %d\n", upevt_.clct_nbucket);
  printf("upevt_.clct_tr_mode       = %d\n", upevt_.clct_tr_mode);
  printf("upevt_.clct_full_bxn      = %#x\n", upevt_.clct_full_bxn);
  printf("upevt_.clct_active_febs   = %#x\n", upevt_.clct_active_febs);
  printf("upevt_.clct_febs_read     = %#x\n\n", upevt_.clct_febs_read);
#endif

  /* ilink=0 -> Best muon cLCT, ilink=1 -> Second Best muon cLCT */
  for (ilink = 0; ilink < 2; ilink++) {
    upevt_.clct_valid_patt[ilink]   =  buf[ibuf+4+ilink*2] & 0x0001;
    /* In LCT99 patt_number = patt_quality  */
    upevt_.clct_patt_number[ilink]  = (buf[ibuf+4+ilink*2] & 0x01fe) >> 1;
    upevt_.clct_bend[ilink]         = (buf[ibuf+4+ilink*2] & 0x0200) >> 9;

    /* 1/2-strip ID split between words */
    upevt_.clct_half_strip[ilink]  = (buf[ibuf+4+ilink*2] & 0x3c00) >> 10;
    upevt_.clct_half_strip[ilink]  = upevt_.clct_half_strip[ilink] | 
                                     ((buf[ibuf+5+ilink*2] & 0x000f) << 4);
    upevt_.clct_bxn[ilink]         = (buf[ibuf+5+ilink*2] & 0x01f0) >> 4;
  }
/*
  printf("decode_clct: LCT words for event %d are: %8x %8x %8x %8x\n", 
   upevt_.event_number, buf[ibuf+4], buf[ibuf+5],  buf[ibuf+6], buf[ibuf+7]);
*/

  if (upevt_.clct_tr_mode == 0) {  /* No-Dump mode */
    clct_wc = buf[ibuf+9] & 0x03ff;
    if (clct_wc != CLCT_NO_DUMP_SIZE) {
      #ifdef OFFLINE_ONLY
      printf(
      #else
      syslog(LOG_LOCAL1|LOG_WARNING, 
      #endif
      "WW decode_CLCT(): Incorrect frame count in event %ld for No-Dump mode: "
      "clct_wc = %d\n", upevt_.event_number, clct_wc);
    }
    return clct_wc;
  }


  /* If we get here we have Full-Dump or Local-Dump mode */
  clct_wc = CLCT_NO_DUMP_SIZE - 2;

  for (icfeb = 0; icfeb < NCFEB; icfeb++) {
    if (upevt_.clct_febs_read & (0x1 << icfeb)) {
      for (itime = 0; itime < upevt_.clct_nbucket; itime++) {
        for (iblock = 0; iblock < 2; iblock++) {
          for (ilayer = 0; ilayer < NLAYER; ilayer++) {
            for (ibit = 0; ibit < 4; ibit++) { 

              kbit = ((ilayer % 2) == 0) ? ibit : ibit + 4;
              kdouble_strip = ibit + 4*iblock + 8*icfeb;
                 
              if (buf[ibuf+clct_wc] & (0x1 << kbit)) {
                upevt_.clct_dump[layer_map[ilayer]][kdouble_strip] |= 
                 tbit[itime];
/*
                printf("decode_clct: found triad ON for layer %d distrip %d "
                 "timebin %d\n", layer_map[ilayer]+1, kdouble_strip+1, itime+1);
*/
              }
                       
            }
            if ((ilayer % 2) == 1) clct_wc++;
          }  
        }
      }
    }
  }

  /* Two Extra trailer words */
  if ((buf[ibuf+clct_wc] & 0xfff) != 0xe0d) {
    printf ("decode_clct: at end of data expected 0xe0d, found %#x\n", 
     buf[ibuf+clct_wc]);
    clct_wc++;
  }

  else {
    clct_wc++;
    if ((upevt_.clct_tr_mode == 1) &&  			/* Full-Dump mode */
     ((buf[ibuf+clct_wc] & 0x03ff) != (expected_wc & 0x3ff)) ) {  
      #ifdef OFFLINE_ONLY
      printf(
      #else
      syslog(LOG_LOCAL1|LOG_WARNING, 
      #endif
       "WW decode_CLCT(): Incorrect frame count in event %ld for Full-Dump "
       "mode: clct_wc = %d, Frame Count = %d, expected word count = %d\n", 
       upevt_.event_number, clct_wc+1, buf[ibuf+clct_wc] & 0x03ff, expected_wc);
    }
  }

  /* In order to return the correct word count */
  clct_wc++; 
  
  return clct_wc;

}

/******************************************************************************
 
   Decode CLCT01   
 
******************************************************************************/
// #define GNE_DEBUG  
int decode_CLCT2(short *buf, int ibuf, int version)
{
	int j, istrip;
	int icfeb, itime, ilayer; 
	int expected_wc;
	int clct_wc = 0;
#ifdef GNE_DEBUG
	int t, bcnt;
	char bsym[] = ".123456789ABCDEFGHIGKLMNPQRSTUVW";
#endif

/* Calculate the expected wordcount from the header info */
	expected_wc = 0;
	if (upevt_.clct_tr_mode > 0) 
    {
		for (j = 0; j < 5; j++) 
		{
			if (upevt_.clct_febs_read & (1 << j)) 
				expected_wc += 6 * upevt_.clct_nbucket;
		}
    }


	clct_wc = 0;
	for (icfeb = 0; icfeb < NCFEB; icfeb++) 
	{
		if (upevt_.clct_febs_read & (0x1 << icfeb)) 
		{
			for (itime = 0; itime < upevt_.clct_nbucket; itime++) 
			{
				for (ilayer = 0; ilayer < NLAYER; ilayer++) 
				{
					for ( istrip = 0 ; istrip < 8 ; istrip ++ )
					{
						if (buf[ibuf+clct_wc] & (0x1 << istrip))
						{
							upevt_.clct_dump[ilayer][istrip + (icfeb*8)] |= 1<<itime;
						}
					}
					clct_wc ++;
				}
			}
		}
	}

#ifdef GNE_DEBUG
	for (ilayer = 0; ilayer < NLAYER; ilayer++) 
	{
		for ( istrip = 0 ; istrip < 40 ; istrip ++ )
		{
			t = upevt_.clct_dump[ilayer][istrip];
			bcnt = 0;
			for (itime = 0; itime < upevt_.clct_nbucket; itime++)
			{
				if (t & 1) bcnt++;
				t >>= 1;
			}
			printf("%c", bsym[bcnt]);
		}
		printf ("\n");
	}
	printf ("\n----------------------------------------\n");

	for (itime = 0; itime < upevt_.clct_nbucket; itime++) 
	{
		for (ilayer = 0; ilayer < NLAYER; ilayer++) 
		{
			for ( istrip = 0 ; istrip < 40 ; istrip ++ )
			{
				if (((upevt_.clct_dump[ilayer][istrip] >> itime) & 1) == 1) printf ("#");
				else printf (".");
			}
			printf ("\n");
		}
		printf ("\n");
	}
#endif

	// clct trailer
	if ((buf[ibuf+clct_wc] & 0xffff) != 0x6b04) 
    {
		dprint ("decode_clct: at end of data expected 0x6b04, found %#x\n", 
				buf[ibuf+clct_wc]);
		clct_wc++;
    }

	// simply scan to find trailer (de0f), ignoring any RPC data
	for (j = 0; j < 10000; j++)
	{
		if ((((unsigned short)buf[ibuf+ (++clct_wc)]) & 0xffff) == 0xde0f) break;
	}
	if (((unsigned short)buf[ibuf + clct_wc]) != 0xde0f || j == 10000)
		dprint("decode_clct: cannot find TMB DE0F marker\n");

    // found trailer, skip word count
	clct_wc += 2;
	return clct_wc;
}
//#undef GNE_DEBUG
/******************************************************************************
 
   Decode CFEB     
 
******************************************************************************/

int decode_CFEB(short *buf, int ibuf, int version)
  {
  int   iddu, ifeb, itime, ifeb_chan;
  short ilayer = 0, istrip = 0;
  short crc15, checksum, dummy;
  short temp_strip;
  short temp[NLAYER][NCFEB][MAX_SCA_SAMPLE]; 

  upevt_.nsca_sample = (int )buf[NMOD_HEAD + 1];

  /* Zero temp */
  for (ifeb = 0; ifeb < NCFEB; ifeb++) 
    {
    for (ilayer = 0; ilayer < NLAYER; ilayer++) 
      {
      for (itime =0; itime < upevt_.nsca_sample; itime++) 
	{
        temp[ilayer][ifeb][itime] = 0;
	}
      }
    }

  for (ifeb = 0; ifeb < NCFEB; ifeb++) 
    {     
    if (buf[NMOD_HEAD + 2] & (0x1 << ifeb))    /* Is this CFEB present? */
      {
      upevt_.active_febs[ifeb] = 1;
      for (itime = 0; itime < upevt_.nsca_sample; itime++) 
	{
        for (iddu = 0; iddu < NCFEB_CHAN; iddu++) 
	  {

          ifeb_chan = 16*(iddu % 6) + iddu/6;

          /* Find layer and strip from CFEB cable map */
          ilayer = ((map_.CFEB[ifeb][ifeb_chan] & 0xff00)>>8) - 1;
          istrip =  (map_.CFEB[ifeb][ifeb_chan] & 0x00ff)     - 1;

          temp_strip = istrip % 16;

          /* Get the Serialized CFEB-SCA Controller Data */
          temp[ilayer][ifeb][itime] |= ((buf[ibuf] & 0x2000)>>13) 
							    << temp_strip;

          /* Record the ADC out-of-range flag */
          upevt_.adc_out_of_range[ilayer][istrip][itime] = 
            (buf[ibuf] & 0x1000) >> 12;

          /* Copy each time sample into upevt_. Take only first 12 bits */
          upevt_.sca[ilayer][istrip][itime] = buf[ibuf++] & 0xfff;
	  }

        /* See if we have a valid checksum */
        checksum = buf[ibuf++]; 
        dummy    = buf[ibuf++]; 
        crc15 = crcdivide(buf + ibuf - NCFEB_CHAN - 2);

        if (crc15 != checksum) 
	  {
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_ERR, 
          #endif
           "EE decode_CFEB(): Checksum error: FEB %d, Time sample %d Found "
           "%#hx, Expected %#hx event number %ld\n", ifeb, itime, checksum, 
           crc15, upevt_.event_number);

	  }
          
	}
      }  /* end if this FEB present */
    }

  /* Fill SCA block and trigger time arrays */
  for (ilayer = 0; ilayer < NLAYER; ilayer++) 
    {
    for (istrip = 0; istrip < NSTRIP; istrip++) 
      {
      ifeb = istrip / 16;
      for (itime = 0; itime < upevt_.nsca_sample; itime++) 
	{
        upevt_.sca_block[ilayer][istrip][itime]  = 
          (temp[ilayer][ifeb][itime] & 0x0f00) >> 8; 
        upevt_.sca_trig_time[ilayer][istrip][itime] = 
           temp[ilayer][ifeb][itime] & 0x00ff; 

	}
      }
    }

  return 0;

}
/******************************************************************************
 
   Decode CFEB for 2001 CFEBs
 
******************************************************************************/
//#define GNE_DEBUG
int decode_CFEB2(short *buf, int ibuf, int cfeb_active)
{
    int   iddu, ifeb, itime, ifeb_chan;
    short ilayer = 0, istrip = 0;
    short crc15, checksum, dummy;
    short temp_strip;
    unsigned short temp[NLAYER][NCFEB][MAX_SCA_SAMPLE]; 
    short cfeb_98, cfeb_99, cfeb_l1a;
//    int ibuf_old = ibuf;
    int crc_reported = 0;
//	int chan2ly[] = {2,0,4,5,3,1};
//	int chan2strip[] = {0,1,3,2,6,7,5,4,12,13,15,14,10,11,9,8};

	// extract ts_flag from cfeb word 95, determine how many samples
    upevt_.nsca_sample = ((buf[ibuf + 95] >> 13) & 1) ? 16 : 8;
	dprint ("decode_cfeb2: cfeb_active: %x  nsca_sample: %d buf[99]: %04x offset from DMB: %x, word_before: %04x\n", 
			cfeb_active, upevt_.nsca_sample, buf[ibuf+99], ibuf, buf[ibuf-1]);

    /* Zero temp */
    for (ifeb = 0; ifeb < NCFEB; ifeb++) 
    {
		for (ilayer = 0; ilayer < NLAYER; ilayer++) 
		{
			for (itime =0; itime < upevt_.nsca_sample; itime++) 
			{
				temp[ilayer][ifeb][itime] = 0;
			}
		}
    }

    for (ifeb = 0; ifeb < NCFEB; ifeb++) 
    {     
		if (cfeb_active & (0x1 << ifeb))    /* Is this CFEB present? */
		{
			upevt_.active_febs[ifeb] = 1;
			for (itime = 0; itime < upevt_.nsca_sample; itime++) 
			{
				for (iddu = 0; iddu < NCFEB_CHAN; iddu++) 
				{

					/* this reorders the ddu channel.  we don't use it in this
					   version.  See cable_map_4000. */
					/* ifeb_chan = 16*(iddu % 6) + iddu/6; */
					ifeb_chan = iddu;

					/* Find layer and strip from CFEB cable map */
					ilayer = ((map_.CFEB[ifeb][ifeb_chan] & 0xff00)>>8) - 1;
					istrip =  (map_.CFEB[ifeb][ifeb_chan] & 0x00ff)     - 1;

//					ilayer = chan2ly[ifeb_chan%6];
//					istrip = chan2strip[ifeb_chan/6] + 16*ifeb;

                    // modification below is to correctly calculate sca_block (Madorsky)
//                  temp_strip = istrip % 16;
					temp_strip = iddu / 6;
	      
					/* Get the Serialized CFEB-SCA Controller Data */
					temp[ilayer][ifeb][itime] |= ((buf[ibuf] & 0x2000)>>13) << temp_strip;

//	  dprint("ly:%d feb:%d time:%d temp:%x\n", ilayer, ifeb, itime, temp[ilayer][ifeb][itime]);

					/* Record the ADC out-of-range flag */
					upevt_.adc_out_of_range[ilayer][istrip][itime] = 
						(buf[ibuf] & 0x1000) >> 12;

					/* Copy each time sample into upevt_. Take only first 12 bits */
					upevt_.sca[ilayer][istrip][itime] = buf[ibuf++] & 0xfff;
				}

				/* See if we have a valid checksum */
				checksum = buf[ibuf++];
				cfeb_98    = buf[ibuf++]; 
				crc15 = crcdivide(buf + ibuf - NCFEB_CHAN - 2);

				if (crc15 != checksum && !crc_reported) 
				{
#ifdef OFFLINE_ONLY
					printf("EE decode_CFEB2(): Checksum error: FEB %d, Time sample %d Found "
						   "%#hx, Expected %#hx event number %ld offset %x\n", ifeb, itime, checksum, 
						   crc15, upevt_.event_number, ibuf);
#else
					syslog(LOG_LOCAL1|LOG_ERR,"EE decode_CFEB2(): Checksum error: FEB %d, Time sample %d Found "
						   "%#hx, Expected %#hx event number %ld offset %x\n", ifeb, itime, checksum, 
						   crc15, upevt_.event_number, ibuf); 
#endif

					crc_reported = 1;

				}
				/*
				 * There are lots of things in CFEB words 98 and 99, but
				 * we just skip them for FAST
				 */
				cfeb_99 = buf[ibuf++];
				cfeb_l1a = (cfeb_99 >> 6) & 0x3f;
//				dprint ("cfeb_l1a: %d\n", cfeb_l1a);
				dummy = buf[ibuf++];
			}
		}  /* end if this FEB present */
    }

    /* Fill SCA block and trigger time arrays */
    for (ilayer = 0; ilayer < NLAYER; ilayer++) 
    {
		for (istrip = 0; istrip < NSTRIP; istrip++) 
		{
			ifeb = istrip / 16;
			for (itime = 0; itime < upevt_.nsca_sample; itime++) 
			{
				upevt_.sca_block[ilayer][istrip][itime]  = 
					(temp[ilayer][ifeb][itime] & 0x0f00) >> 8; 
				upevt_.sca_trig_time[ilayer][istrip][itime] = 
					temp[ilayer][ifeb][itime] & 0x00ff; 
//	dprint("ly:%d strip:%d time:%d temp:%x\n", ilayer, istrip, itime, temp[ilayer][ifeb][itime]);
//	dprint("ly:%d strip:%d time:%d sca:%x\n", ilayer, istrip, itime, upevt_.sca_block[ilayer][istrip][itime]);

			}
		}
    }

    upevt_.sca_clock_phase = 0;
    for (ifeb = 0; ifeb < NCFEB; ifeb++)
    {
		upevt_.sca_clock_phase |= ((temp[0][ifeb][0] >> 12) & 1) << ifeb;
    }
//    dprint ("clock_phase: %x\n", upevt_.sca_clock_phase);

    return 0;
}
//#undef GNE_DEBUG

/*******************************************************************************
 
   Decode Wire JTAG calibration data -- ALCT version
   (For now, no calibration block is written, so this routine just calls
    decode_alct for the raw hits readout.)
 
*******************************************************************************/

int decode_ALCTCAL(short *buf)
{
  int            alct_wc, dav, empty, offset, rh_wc, version;
  short int      first_data_word;

  version = buf[NMOD_HEAD] & 0xff;
  if (version == 1) {
    first_data_word = buf[NMOD_HEAD+1];
    offset = NMOD_HEAD + 1 + (first_data_word >> 8);

/* Calibration unpacking */
    upevt_.alctcal_scan_mode = first_data_word & 0x7f;
    if (upevt_.alctcal_scan_mode > 0) 
      {
      upevt_.alctcal_current_value        = buf[NMOD_HEAD +  3];
      upevt_.alctcal_current_threshold    = buf[NMOD_HEAD +  8];
      upevt_.alctcal_trigger_per_step     = buf[NMOD_HEAD +  6];
      upevt_.alctcal_num_points           = buf[NMOD_HEAD +  7];
      upevt_.alctcal_num_points_2nd_param = buf[NMOD_HEAD + 11];
      }

/* Raw hits unpacking */
    if (first_data_word & 0x80) {
      rh_wc = buf[offset] & 0x0fff;
      empty = buf[offset] & 0x1000;
      dav   = buf[offset] & 0x2000;
      if (dav && !empty) {
        alct_wc = decode_ALCT(buf, offset+1, version);
        if (alct_wc != rh_wc) syslog(LOG_LOCAL1|LOG_WARNING, 
         "WW decode_ALCTCAL: mismatched word count in event %ld: alct_wc = %d, "
         "wc from raw hits status word = %d", upevt_.event_number, alct_wc, 
         rh_wc);
      }
    }
  }
  else syslog(LOG_LOCAL1|LOG_WARNING, 
   "WW decode_ALCTCAL: unknown data format version: %d", version);
  return 0;
}

/*****************************************************************************
   Decode ALCT-2001
*****************************************************************************/
//#define DEBUG_ALCT2
int decode_ALCT2(short *buf)
    {
	int            i, version, seq;
	short int      first_data_word;
	int            offset, track;
	int            subwire, wire_region, layer, time_bucket, time_bit;
	int usecrc = 0;
	unsigned long crc;

#ifdef DEBUG_ALCT2
	char           strue[2], sfalse[2];
  
	strcpy(strue,"T");
	strcpy(sfalse,"F");
#endif

	version = buf[NMOD_HEAD] & 0xff;
	if ((version == 1) || (version == 2))
	{
	    first_data_word = buf[NMOD_HEAD+1];
	    offset = NMOD_HEAD + 1 + (first_data_word >> 8);
	
	    if (version == 2)
	    {
		/* Calibration unpacking */
		upevt_.alctcal_scan_mode = first_data_word & 0x7f;
		if (upevt_.alctcal_scan_mode > 0) 
		{
		    upevt_.alctcal_current_value        = buf[NMOD_HEAD +  3];
		    upevt_.alctcal_current_threshold    = buf[NMOD_HEAD +  8];
		    upevt_.alctcal_trigger_per_step     = buf[NMOD_HEAD +  6];
		    upevt_.alctcal_num_points           = buf[NMOD_HEAD +  7];
		    upevt_.alctcal_num_points_2nd_param = buf[NMOD_HEAD + 11];
		}
	    }
	
	    upevt_.alct_active = 1;
	
#ifdef DEBUG_ALCT2
	    printf ("ALCT block: version %d, length %d\n",version,buf[0]);
#endif

	    seq = 0;
	    track = 0;
	
	    for ( i = offset ; i < buf[0]-1 ; i += 2 )
	    {
		switch (buf[i])
		{
		case 1:
#ifdef DEBUG_ALCT2
		    printf("Mark 1:   hfa = %s   h = %d   hn = %d   hpatb = %s\n",
			   (buf[i+1]&0x200)?strue:sfalse,
			   (buf[i+1]&0x180)>>7,
			   (buf[i+1]&0x07e)>>1,
			   (buf[i+1]&0x001)?strue:sfalse);
#endif
		    /* this routine unpacks the rest of the trigger info from
		       the lct0, lct1 words... see below, sequence 4-7 */
		    upevt_.alct_patb[0]=buf[i+1]&0x001;
		    break;
		case 2:
#ifdef DEBUG_ALCT2
		    printf("Mark 2:   lfa = %s   l = %d   ln = %d   lpatb = %s\n",
			   (buf[i+1]&0x200)?strue:sfalse,
			   (buf[i+1]&0x180)>>7,
			   (buf[i+1]&0x07e)>>1,
			   (buf[i+1]&0x001)?strue:sfalse);
#endif
		    /* this routine unpacks the rest of the trigger info from
		       the lct0, lct1 words... see below, sequence 4-7 */
		    upevt_.alct_patb[1]=buf[i+1]&0x001;
		    break;
		case 3:
#ifdef DEBUG_ALCT2
		    printf("Mark 3: seq=%d, ",seq);
#endif
		    if (seq == 0)
		    {
#ifdef DEBUG_ALCT2
			printf ("ddu=%s spcl=%s 1st=%s lst=%s abrt=%s bdid=%d csc=%d l1a=%d\n",
				(buf[i+1]&0x8000)?strue:sfalse,
				(buf[i+1]&0x4000)?strue:sfalse,
				(buf[i+1]&0x2000)?strue:sfalse,
				(buf[i+1]&0x1000)?strue:sfalse,
				(buf[i+1]&0x0800)?strue:sfalse,
				(buf[i+1]&0x0700)>>8,
				(buf[i+1]&0x00f0)>>4,
				(buf[i+1]&0x000f) );
#endif
			upevt_.alct_lct_id = (buf[i+1]&0x0700) >> 8;
			upevt_.alct_csc_id = (buf[i+1]&0x00f0) >> 4;
			upevt_.alct_l1a_num = (buf[i+1]&0x000f) ;
		    }
		    else if (seq == 1)
		    {
#ifdef DEBUG_ALCT2
			printf("ddu=%s spcl=%s 2nd=%s 1st=%s EXT=%s L1A=%s bins=%d fifo=%d\n",
			       (buf[i+1]&0x8000)?strue:sfalse,
			       (buf[i+1]&0x4000)?strue:sfalse,
			       (buf[i+1]&0x0400)?strue:sfalse,
			       (buf[i+1]&0x0200)?strue:sfalse,
			       (buf[i+1]&0x0100)?strue:sfalse,
			       (buf[i+1]&0x0080)?strue:sfalse,
			       (buf[i+1]&0x007c)>>2,
			       (buf[i+1]&0x0003) );
#endif
			upevt_.alct_l1a_lct_match = (buf[i+1]&0x0080) >> 7;
			upevt_.alct_nbucket       = (buf[i+1]&0x007c) >> 2;
			upevt_.alct_tr_mode       = (buf[i+1]&0x0003) ;
		    }
		    else if (seq == 2)
		    {
#ifdef DEBUG_ALCT2
			printf("ddu=%s spcl=%s BXN=%d\n",
			       (buf[i+1]&0x8000)?strue:sfalse,
			       (buf[i+1]&0x4000)?strue:sfalse,
			       (buf[i+1]&0x0fff) );
#endif
			upevt_.alct_full_bxn = buf[i+1]&0x0fff;		       
		    }
		    else if (seq == 3)
		    {
#ifdef DEBUG_ALCT2
			printf("ddu=%s spcl=%s active=%d lctro[6..0]=%d\n",
			       (buf[i+1]&0x8000)?strue:sfalse,
			       (buf[i+1]&0x4000)?strue:sfalse,
			       (buf[i+1]&0x3f80)>>7,
			       (buf[i+1]&0x007f) );		       
#endif
			upevt_.alct_active_febs = (buf[i+1]&0x3f80)>>7;
			upevt_.alct_febs_read = (buf[i+1]&0x007f);
		    }
		    else if ((seq >= 4) && ( seq <= 7 ))
		    {
#ifdef DEBUG_ALCT2
			printf("ddu=%s spcl=%s lct%d[%d..%d]=%d\n",
			       (buf[i+1]&0x8000)?strue:sfalse,
			       (buf[i+1]&0x4000)?strue:sfalse,
			       ((seq==4)||(seq==5))?0:1,
			       ((seq==4)||(seq==6))?13:27,
			       ((seq==4)||(seq==6))?0:14,
			       (buf[i+1]&0x3fff) );
#endif
			if (!(seq & 0x1))
			{
			    track = seq&0x2?1:0;
			    upevt_.alct_valid_patt[track]=buf[i+1]&0x0001;
			    upevt_.alct_patt_quality[track]=(buf[i+1]&0x0006)>>1;
			    upevt_.alct_accel_muon[track]=(buf[i+1]&0x0008)>>3;
			    /* TODO: sasha borrowed an extra bit in here...
			       buf[i+1]&0x0010 is actually patb, let's ignore */
			    upevt_.alct_wire_group[track]=(buf[i+1]&0x07e0)>>5;
			    upevt_.alct_bxn[track]=(buf[i+1]&0x3800)>>11;
			}
			else
			{
			    upevt_.alct_bxn[track]|=(buf[i+1]&0x0003)<<3;
			}
		    }
		    else if ((seq >= 8)
			     && ( seq <= (upevt_.alct_nbucket*48) + 7 ) )
		    {
#ifdef DEBUG_ALCT2
			printf("ddu=%s lct_chip=%d tbin=%d ly%d[%d..%d]=%d\n",
			       (buf[i+1]&0x8000) ? "T" : "F",
			       (buf[i+1]&0x6000) >> 13 , 
			       (buf[i+1]&0x1f00) >> 8 ,
			       ( ( seq - 8 ) / 2 ) % 6 ,  
			       ( seq & 0x1 ) ? 15 : 7 ,
			       ( seq & 0x1 ) ? 8 : 0,
			       (buf[i+1]&0x00ff) );
#endif
			/* these people had a way freaky technique for decoding 
			   things... using nested for loops ad nauseam...
			   yes, it may be faster, but not really with today's
			   cpu's and L2...

			   here's the modulo way... */

			/* do some precalculations: */
			/* our wire_region is the region of wires described by
			   the data in question */
			wire_region = 
			    ((seq-8)/(12*upevt_.alct_nbucket))*16 + (seq&0x1)*8;
			/* layer is obvious */
			layer = ((seq-8)/2) % 6;
			/* the time for this bucket is found in the upper half */
			time_bucket = (buf[i+1]&0x1f00)>>8;
			/* the time bit we'll be setting is determined by the time
			   bucket */
			time_bit = 1 << time_bucket;
			/* finally, loop over the eight wires in this byte */
			for ( subwire = 0 ; subwire < 8 ; subwire ++ )
			    if (buf[i+1]&(1<<subwire))
				upevt_.alct_dump[layer][wire_region+subwire] |= time_bit;
		    }
		    else if (seq == (upevt_.alct_nbucket*48) + 8 )
		    {
			if ((buf[i+1]&0xfff) != 0xe0d) 
			{
			    usecrc = 1;
			    crc = buf[i+1] & 0x7ff;
			}
			else
			{
#ifdef DEBUG_ALCT2
			    printf("ddu=%s evener = 0x%3.3x\n",
				   (buf[i+1]&0x8000)?strue:sfalse,
				   (buf[i+1]&0xfff) );
#endif
			}
		    }
		    else if (seq == (upevt_.alct_nbucket*48) + 9 )
		    {
			if (usecrc == 1)
			{
			    crc |= (buf[i+1] & 0x7ff) << 11;
#ifdef DEBUG_ALCT2
			    printf("crc = 0x%08lx\n", crc);
#endif
			}
			else
			{
#ifdef DEBUG_ALCT2
			    printf("ddu=%s spcl=%s lst=%s a=%s frame=%d\n",
				   (buf[i+1]&0x8000)?strue:sfalse,
				   (buf[i+1]&0x4000)?strue:sfalse,
				   (buf[i+1]&0x1000)?strue:sfalse,
				   (buf[i+1]&0x0400)?strue:sfalse,
				   (buf[i+1]&0x03ff) );
#endif
			}
		    }
		    else if (seq == (upevt_.alct_nbucket*48) + 10 )
		    {
			if (usecrc == 1)
			{
#ifdef DEBUG_ALCT2
			    printf("ddu=%s evener = 0x%3.3x\n",
				   (buf[i+1]&0x8000)?strue:sfalse,
				   (buf[i+1]&0xfff) );
#endif
			}
			else
			{
#ifdef DEBUG_ALCT2
			    printf(" sequence number invalid.\n");
#endif
			}
		    }
		    else if (seq == (upevt_.alct_nbucket*48) + 11 )
		    {
			if (usecrc == 1)
			{
#ifdef DEBUG_ALCT2
			    printf("ddu=%s spcl=%s lst=%s a=%s frame=%d\n",
				   (buf[i+1]&0x8000)?strue:sfalse,
				   (buf[i+1]&0x4000)?strue:sfalse,
				   (buf[i+1]&0x1000)?strue:sfalse,
				   (buf[i+1]&0x0400)?strue:sfalse,
				   (buf[i+1]&0x03ff) );
#endif
			}
			else
			{
#ifdef DEBUG_ALCT2
			    printf(" sequence number invalid.\n");
#endif
			}
		    }
		    else
		    {
#ifdef DEBUG_ALCT2
			printf(" sequence number invalid.\n");
#endif
		    }
		    seq++;
		    break;
		case 4:
#ifdef DEBUG_ALCT2
		    printf("Mark 4: buffer finished.\n");
#endif
		    break;
		default:
		    printf ("ALCT2 data contains bad marker: %d\n",buf[i]);
		    break;
		}
	    }

	}
	return 0;
    }

//#undef DEBUG_ALCT2

/******************************************************************************
 
   Decode Strip calibration data 
 
******************************************************************************/

int decode_STRIPCAL(short *buf)
{
  int            version;
  short int      first_data_word;

  version = buf[NMOD_HEAD] & 0xff;

  if (version == 1 || version == 2) {
    first_data_word = buf[NMOD_HEAD+1];
    upevt_.stripcal_scan_mode = (first_data_word >> 12) & 0xf;
    upevt_.stripcal_current_strip = buf[NMOD_HEAD + 4];
    if (upevt_.stripcal_scan_mode == 1 || upevt_.stripcal_scan_mode == 2 ||
     upevt_.stripcal_scan_mode == 4 || upevt_.stripcal_scan_mode == 5 ) 
      upevt_.stripcal_current_value = buf[NMOD_HEAD + 5];
    else upevt_.stripcal_current_value = buf[NMOD_HEAD + 6];
  }

  else if (version == 3) {
    first_data_word = buf[NMOD_HEAD+1];
    upevt_.stripcal_scan_mode = (first_data_word >> 12) & 0xf;
    upevt_.stripcal_current_strip = buf[NMOD_HEAD + 4];
    upevt_.stripcal_current_value = buf[NMOD_HEAD + 5];
    upevt_.stripcal_trigger_per_step = buf[NMOD_HEAD + 8];
    upevt_.stripcal_num_points    = buf[NMOD_HEAD + 9];
  }

  else if (version == 4) {
    first_data_word = buf[NMOD_HEAD+1];
    upevt_.stripcal_scan_mode = (first_data_word >> 12) & 0xf;
    upevt_.stripcal_current_strip = buf[NMOD_HEAD + 4];
    upevt_.stripcal_current_value = buf[NMOD_HEAD + 5];
    upevt_.stripcal_current_value_2 = buf[NMOD_HEAD + 6];
    upevt_.stripcal_trigger_per_step = buf[NMOD_HEAD + 9];
    upevt_.stripcal_num_points    = buf[NMOD_HEAD + 10];
  }
  else if (version == 5) {
    first_data_word = buf[NMOD_HEAD+1];
    upevt_.stripcal_scan_mode = (first_data_word >> 12) & 0xf;
    upevt_.stripcal_current_strip = buf[NMOD_HEAD + 4];
    upevt_.stripcal_current_value = buf[NMOD_HEAD + 5];
    upevt_.stripcal_current_value_2 = buf[NMOD_HEAD + 6];
    upevt_.stripcal_trigger_per_step = buf[NMOD_HEAD + 9];
    upevt_.stripcal_num_points    = buf[NMOD_HEAD + 10];
    upevt_.stripcal_num_points_turnoff  = buf[NMOD_HEAD + 11];
  }

  else syslog(LOG_LOCAL1|LOG_WARNING, 
   "WW decode_STRIPCAL: unknown data format version: %d", version);
  return 0;
}

/*******************************************************************************
 
   Decode TDC3377 
   This function currently only works for the case "leading_edge_only"
 
*******************************************************************************/

int decode_TDC3377(short *buf)
{
#define SINGLE_WORD 0
#define DOUBLE_WORD 1
  short tdc377_header;
/*int   got_leading = 0, got_trailing = 0;*/
  int   leading_edge, leading_edge_only;
  int   step, mode;
  int   nwire_hits, nscint_hits;
/* int   nrpc_hits; */
  int   channel, time;
  int   crate, slot, layer, wg, data_type;
  int   i;

  nscint_hits = upevt_.num_scint_hits;
  nwire_hits  = upevt_.num_wire_hits;
/* nrpc_hits   = upevt_.num_rpc_hits; */

/* NMOD_HEAD number of generic header words */
  crate = CRATE(buf[3]);
  slot  = SLOT(buf[3]);

/* First data word is 3377 header. See LeCroy catalog for details */
  tdc377_header = buf[NMOD_HEAD + 1];
  mode  = tdc377_header & 0x4000;

  if (mode == SINGLE_WORD) {
    leading_edge_only = !(tdc377_header & 0x400);
  }
  else {
    leading_edge_only = !(tdc377_header & 0x200);
  }

  #ifdef GNE_DEBUG
  printf("TDC3377: crate = %d slot = %d\n", crate, slot);
  #endif

  step = (mode == SINGLE_WORD) ? 1 : 2;

/* buf[0] is the data word count */ 
  for (i = NMOD_HEAD + 2; i < buf[0] - 1; i += step ) {

    if (mode == SINGLE_WORD) {

      if (leading_edge_only) {
        leading_edge = 1;
        time = buf[i] & 0x3ff;
      }
      else {
        leading_edge = (buf[i] & 0x200) >> 9;
        time = buf[i] & 0x1ff;
      }
      channel = (buf[i] & 0x7c00) >> 10;
    }

    else {     /* DOUBLE_WORD mode */
      leading_edge = ((buf[i] & 0x200) == 0);
      time = (buf[i] & 0xff)<<8 | (buf[i+1] & 0xff);
      channel = (buf[i] & 0x7c00) >> 10;

/* Do a few consistency checks on double-word format */
      if (((buf[i+1] & 0x100) != 0) || 
       ((buf[i] & 0x100) != 0x100))      {
        #ifdef OFFLINE_ONLY
        printf(
        #else
        syslog(LOG_LOCAL1|LOG_ERR, 
        #endif
         "EE decode_TDC3377(): Bad word identifiers, offset %x, offset + 1 "
         "%x crate %d slot %d\n", buf[i], buf[i+1], crate, slot); 
        continue;
      }
      if ((buf[i+1] & 0xfe00)!=(buf[i] & 0xfe00)) {
        #ifdef OFFLINE_ONLY
        printf(
        #else
        syslog(LOG_LOCAL1|LOG_ERR, 
        #endif
         "EE decode_TDC3377(): Bad channel numbers, crate %d, slot %d\n", 
         crate, slot);
        continue;
      }

    }  /* end if mode == SINGLE_WORD */


    /* Check to see if the TDC is mapped to chamber. If not then set
     * data_type = -1 (Unknown data type)
     */
    if(map_.TDC3377[crate][slot-1][channel] == 0) {
      data_type = -1;
      wg        = -1;
      layer     = -1;
    }
    else {
      data_type = (map_.TDC3377[crate][slot-1][channel] & 0xf000)>>12;
      layer     = (map_.TDC3377[crate][slot-1][channel] & 0x0f00)>>8;
      wg        = (map_.TDC3377[crate][slot-1][channel] & 0x00ff);
     
    }

    /* DEBUG 3377 map */
    #ifdef GNE_DEBUG
    printf("TDC3377: map = %x data_type = %d layer = %d wg = %d time = %d "
     "leading_edge_only = %d mode = %d\n", map_.TDC3377[crate][slot-1][channel],
     data_type, layer, wg, time, leading_edge_only, mode);
    #endif

    switch (data_type) {

      case 1:    /* Anode wire hits */
        if (nwire_hits == MAX_WIRE_HITS) {
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_WARNING, 
          #endif
           "WW decode_TDC3377(): TDC hits discarded: nwire_hits > "
           "MAX_WIRE_HITS\n");
        }
        else {
          if (leading_edge) {
            upevt_.wire_layers[nwire_hits] = layer;
            upevt_.wire_groups[nwire_hits] = wg;
            upevt_.wire_times_lead[nwire_hits] = time;
            nwire_hits++;

          } 
          else {   /* Trailing edge */
            upevt_.wire_times_trail[nwire_hits] = time;

            #ifdef GNE_DEBUG
            printf("TDC3377 TRAIL: layer = %d wg = %d time = %d leading_edge_only = %d mode = %d\n", layer, wg, time, leading_edge_only, mode);
            #endif

          }
        }
        break;

      case 0:    /* Trigger scintillator hits */
        if (nscint_hits == MAX_SCINT_HITS) {
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_WARNING, 
          #endif
           "WW decode_TDC3377(): TDC hits discarded: nscint_hits > "
           "MAX_SCINT_HITS WW\n");
        }
        else {
          if (leading_edge) {
            upevt_.scint_times [nscint_hits] = time;
            upevt_.scint_labels[nscint_hits] = 
              map_.TDC3377[crate][slot-1][channel];
            nscint_hits++;
          }
        }
        break;

//    case 3:    /* Kodel RPC */
//      if (nrpc_hits == NRPC_SECTOR*NRPC_STRIP*NRPC_HIT) {
//        #ifdef OFFLINE_ONLY
//        printf(
//        #else
//        syslog(LOG_LOCAL1|LOG_WARNING, 
//        #endif
//         "WW decode_TDC3377(): TDC hits discarded: nrpc_hits > "
//         "NRPC_SECTOR*NRPC_STRIP*NRPC_HIT\n");
//      }
//      else {
//        /* Only leading edge for RPC */
//        if (leading_edge) {
//          upevt_.rpc_sector[nrpc_hits]     = layer;
//          upevt_.rpc_strip[nrpc_hits]      = wg;
//          upevt_.rpc_times_lead[nrpc_hits] = time;
//          nrpc_hits++;
//
//          #ifdef GNE_DEBUG
//          printf("TDC3377 LEAD : sector = %d strip = %d time = %d "
//           "mode = %d\n", layer, wg, time, mode);
//          #endif
//
//        } 
//      }
//      break;

      default:   /* Unknown data type  */
        #ifdef GNE_DEBUG
        printf("TDC3377: Unknown module data type \n");
        #endif
        break;
      
    } /* end switch data_type */

  } /* end for i < buf[0] - 1 */  

  upevt_.num_wire_hits  = nwire_hits;
  upevt_.num_scint_hits = nscint_hits;
/* upevt_.num_rpc_hits   = nrpc_hits; */

  return 0;

}

/*******************************************************************************
 
   Decode TDC2277 
 
*******************************************************************************/

int decode_TDC2277(short *buf)
{
  int leading_edge;
  int nscint_hits, nwire_hits;
  int channel, time;
  int crate, slot, layer, wg;
  int nbeam_cham_hits[NBEAM_CHAM_LAYER][2];
  int data_type;
  int i;

  /* Zero nbeam_cham_hits */
  for (layer = 0; layer < NBEAM_CHAM_LAYER; layer++) {
    for (wg = 0; wg < 2; wg++) {
      nbeam_cham_hits[layer][wg] = 0;
    }
  }

  nscint_hits     = upevt_.num_scint_hits;
  nwire_hits      = upevt_.num_wire_hits;

  crate = CRATE(buf[3]);
  slot  = SLOT(buf[3]);

/*printf("decode 2277: buf[0] = %d\n", buf[0]);*/

  for (i = NMOD_HEAD + 1; i < buf[0] - 1; i += 2 ) {

    leading_edge =  buf[i] & 0x1;
    channel      = (buf[i] & 0x3e) >> 1;
    time         =  buf[i+1];

    if(map_.TDC2277[crate][slot-1][channel] == 0) {
      data_type = -1;
      wg        = -1;
      layer     = -1;
    }
    else {
      data_type = (map_.TDC2277[crate][slot-1][channel] & 0xf000)>>12;
      layer     = (map_.TDC2277[crate][slot-1][channel] & 0x0f00)>>8;
      wg        = (map_.TDC2277[crate][slot-1][channel] & 0x00ff);
    }

#ifdef GNE_DEBUG
    printf("TDC2277 crate = %d slot = %d data_type %d\n", crate, slot, data_type);
#endif

    switch (data_type) {

      case 1:    /* Anode wire hits */
        if (nwire_hits == MAX_WIRE_HITS) {
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_WARNING, 
          #endif
           "WW decode_TDC2277(): TDC hits discarded: nwire_hits > "
           "MAX_WIRE_HITS\n");
        }
        else {
          upevt_.wire_layers[nwire_hits] = layer;
          upevt_.wire_groups[nwire_hits] = wg;
          upevt_.wire_times_lead[nwire_hits] = time;
          nwire_hits++;

          #ifdef GNE_DEBUG
          printf("TDC2277 Anode Wire: layer = %d wg = %d time = %d\n", layer, wg, time);
          #endif

        }
        break;

      case 2:    /* Beam Chambers */
        nbeam_cham_hits[layer - 1][wg]++;
        if (nbeam_cham_hits[layer - 1][wg] > 1) {
/*
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_WARNING, 
          #endif
           "WW decode_TDC2277(): TDC hits discarded:  nbeam_cham_hits[%d][%d] "
           "> 1\n", layer, wg);
*/
        }
        else {
          upevt_.beam_cham_times[layer - 1][wg] = time;
        }
        #ifdef GNE_DEBUG
        printf("TDC2277 Beam Cham: layer = %d wg = %d time = %d\n", layer, wg, time);
        #endif
        break;

      case 0:    /* Trigger scintillator hits */
        if (nscint_hits == MAX_SCINT_HITS) {
          #ifdef OFFLINE_ONLY
          printf(
          #else
          syslog(LOG_LOCAL1|LOG_WARNING, 
          #endif
           "WW decode_TDC2277(): TDC hits discarded: nscint_hits > "
           "MAX_SCINT_HITS\n");
        }
        else {
          upevt_.scint_times [nscint_hits] = time;
          upevt_.scint_labels[nscint_hits] = 
            map_.TDC2277[crate][slot-1][channel];
          nscint_hits++;
        }
        break;

      default:   /* Unknown data type  */
        #ifdef GNE_DEBUG
        printf("TDC2277: Unknown module data type \n");
        #endif
        break;
      
    } /* end switch data_type */

  } /* end for offset < block_wc - 1 */

  upevt_.num_wire_hits      = nwire_hits;
  upevt_.num_scint_hits     = nscint_hits;

  return 0;

}


/*******************************************************************************
 
   Decode Phillips 7106 discriminator
 
*******************************************************************************/

int decode_DSC7106(short *buf) {
  int          ch, crate, slot, layer, scint;
  short int    data_word;

/* This module only has one word; unpack it into scint_hit array, according
 * to the cable map.
 */

  data_word = buf[NMOD_HEAD + 1];

  if (data_word != 0) {
    crate = (buf[NMOD_HEAD - 1] & 0x700) >> 8;
    slot = buf[NMOD_HEAD - 1] & 0xff;
    for (ch = 0; ch < 16; ch++) {
      if ((data_word & (1 << ch)) && map_.DSC7106[crate][slot-1][ch]) {
        layer = (map_.DSC7106[crate][slot-1][ch] >> 8) & 0xf;
        scint = map_.DSC7106[crate][slot-1][ch] & 0xff;
        upevt_.scint_on[layer-1][scint-1] = 1;
      }
    }
  }

  return 0;
}

/*******************************************************************************
 
   Decode LeCroy 4434 scaler
 
*******************************************************************************/

int decode_SCL4434(short *buf) {
  int          j, ch, crate, slot, temp;

/* The first two words are 32 bits specifying which channels are being read
 * out: first word = channels 0-15, second word = channels 16-31. These are 
 * followed by the data from those channels, with two words per channel: 
 * first word = 16 lowest bits, second word = 8 highest bits.
 *
 * Definitions of mapped data types:
 *    1 = ungated trigger input 
 *    2... not yet defined
 */

  temp = (buf[NMOD_HEAD + 1] & 0xffff) | (buf[NMOD_HEAD + 2] << 16);
  crate = (buf[NMOD_HEAD - 1] & 0x700) >> 8;
  slot = buf[NMOD_HEAD - 1] & 0xff;

  j = 2;
  ch = 0;

  while ((temp != 0) && (ch < 32)) {
    if (temp & 0x1) {
      if (map_.SCALER[crate][slot-1][ch] == 1) {
        upevt_.num_ungated_triggers = 
         (buf[NMOD_HEAD + j + 1] & 0xffff) | (buf[NMOD_HEAD + j + 2] << 16);
      }
      j += 2;
    }
    temp = temp >> 1;
    ch++;
  }
  return 0;
}

/*******************************************************************************
 
   Decode Lecroy 2551 scaler   
 
*******************************************************************************/
/*
int decode_SCL2551(short *buf) {
  int i, j;
  int ichan;	
  int interval_mode = 0;

  // See if in interval mode. This means that we count the number of
     hits out of spill and during spill, separately 
  if (buf[0] > (NMOD_HEAD + 26) ) {
    interval_mode = 1;
  }

  ichan = 0;
  j     = 0;
  for (i = NMOD_HEAD + 1; i < buf[0] - 1; i += 2 ) {
    if (interval_mode && (ichan > 11) ) {
      j     = 1;
      ichan = 0;
    }
    upevt_.scaler[j][ichan] = combine_shorts(buf[i], buf[i+1]);
    ichan++;

  }

  return 0;
}
*/

/*******************************************************************************

   Decode one module's worth of data (for debugging)

*******************************************************************************/

void dump_block(short *buf)
{
  int      j;
  short    *ptr;

  ptr = buf;
  for (j = 0; j < buf[0]; j++) { 
    printf(" %4hx", *ptr++);
    if (j % 16 == 15 || j == (buf[0]-1)) printf("\n");
  }
  printf("\n");
}

/*******************************************************************************
 
   Combine two short words into a long
 
*******************************************************************************/

unsigned long combine_shorts(short upper, short lower)
{
  unsigned long upper16, lower16;

  lower16 = 0x0000ffff & (unsigned long)lower;
  upper16 = ((unsigned long)upper << 16) & 0xffff0000; 

  return (upper16 | lower16);

}

