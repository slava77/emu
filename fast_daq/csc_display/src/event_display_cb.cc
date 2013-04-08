#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>

extern "C"
{
#include "daq_ipc.h"
//#include "forms.h"
//#include "event_display.h"
//#include "csc_display_config.h"
//#include "print.h"
#include "application.h"
#include "event_display_common.h"
#include "daq_module.h"
#include "csc_event.h"
#include "pedestal.h"

}

/// -- CMSSW Unpacker --
#include "csc_unpacker.h"

#include "emu/daq/reader/RawDataFile.h"
#include "j_common_data.h"
#include "J_Display.h"

extern "C" void make_fake_data();

extern emu::daq::reader::RawDataFile *fd;
extern char file_name[1000];
extern int rewind_comm;
j_common_type j_data;
int show_cnt = 1, skip_cnt = 1;
int show_index = 0; // how many events are shown before skipping
long event_num;

extern J_Display *jd;

/*
 * These menu items are defined in function get_run_list().
 */
/*
  Remove a large section of commented code
*/
void cbHelp(long data)
{
  std::cout << "Rewinding file\n";

  make_status_paused();

  requested_event = 0;
  show_index = 0;
  fd->close();
  delete fd;

  fd = new emu::daq::reader::RawDataFile (file_name, emu::daq::reader::RawDataFile::DDU);
  fd->open(file_name);
  event_num = 1;

  make_status_running();
}

short buf[100000];
void cbShowEvent()
{
  int    istat, rtval;
  int    offset;
  short  event_wc;
  static int my_i_last_event =  -1;
  static int first = 1;
  static float mean[NSTRIP*NLAYER], rms[NSTRIP*NLAYER];
  static int oneliner = 0;
  //	FL_Coord  x, y, w, h;
  char   message[60];
  unsigned int evnum;
  static int first_time = 1;


  int block_wc;
  // fake header, to satisfy the old unpacker
  short fake_head[] = {
    0x001d, 0x4845, 0x4144, 0x0000, 0x0003, 0x04d2, 0x0000, 0x0000,
    0x0002, 0x0000, 0x5a01, 0x0000, 0x6803, 0x100b, 0x201d,	0x000a,
    0x9a58, 0x102c, 0x0fa0, 0x0fa0, 0x102c, 0x100e, 0x001f, 0x001a,
    0x00c8, 0x001a, 0x0001, 0x0001, 0x001d
  };
  
  // Read in the default pedestals
  if (first || re_read_peds)
    {
      printf("Read peds IS NOT WORKING -- NEED TO FIX.\n");
//		if (get_default_peds(mean, rms) != 0)
//		{
//			syslog(LOG_LOCAL1|LOG_ERR, "cbShowEvent(): Failed to get default pedestals. Event display ending. CC");
//			exit(1);
//		}
      first     = 0;
      re_read_peds = 0;
    }

  if (need_event && !disp_paused)
    {

      // RawDataFile
      do
        {
          if ((fd != NULL && first_time) || rewind_comm)
            {

              if (rewind_comm)
                {
                  // RawDataFile
                  fd->close();
                  // RawDataFile
                  fd->open(file_name);
                  event_num = 1;
                  rewind_comm = 0;
                  first_time = 1;
                }
              // RawDataFile
              rtval = fd->readNextEvent();
              // std::cout<<"event display rtval next event "<<rtval<<endl;

              upevt_.event_number = evnum = event_num++;

//			std::cout << "event number 189: " << upevt_.event_number << std::endl;

              // run number
              fake_head[5] = run_num;

              // event number
              fake_head[6] = (evnum << 16) & 0xffff; // MSB first (f@&k)
              fake_head[7] = evnum & 0xffff;
	      
              // chamber type cannot be determined at this point. Could also be different chambers in one file.
              // event display just shows the available number of wiregroups for each event
	      
	      // RawDataFile
	      block_wc = fd->dataLength()/sizeof(short) + 4; // add 4 for DDU2 header, block_wc in front and rear
	      event_wc = block_wc + 200; // add size of header (roughly)
	      if( rtval == 0 ) 
		{
		  disp_paused = 1; // end of file, pause
		}
	      //				buf = (short int*)malloc(event_wc*sizeof(short));      
	      int i = 1;
	      memcpy(buf + i, fake_head, sizeof(fake_head)); // fake event header
	      i += sizeof(fake_head)/sizeof(short); // move index
	      buf[i++] = block_wc; // event block word count
	      buf[i++] = 0x4444; // 'DD' header
	      buf[i++] = 0x5532; // 'U2' header
	      // RawDataFile
	      memcpy(buf + i, fd->data(), fd->dataLength()); // event itself
	      // RawDataFile
	      i +=  fd->dataLength() / sizeof(short); // move index				
	      buf[i++] = block_wc; // duplicate block_wc at the end of data block
	      
              // printf ("BLOCK WORDS: %d, event read result: %d\n", block_wc, rtval);
	      
              event_wc = i;
              buf[0] = event_wc; // total event word count;

            }

          // Unpack the event and release its buffer space
          if (upevt_.event_number >= requested_event)
            {
              std::cout << "==> Calling get_next_event event number/req event "<<upevt_.event_number<<"/"<<requested_event<<endl;
              // istat = get_next_event(buf, first_time); // unpack only if needed
              istat = get_next_event_cmssw(fd->data(), fd->dataLength(), first_time); // unpack only if needed
              upevt_.event_number = event_num;
              // std::cout << "get next event return status: " << istat << endl;

              if (istat & 2) first_time = 0;
              else
                {
                  // std::cout <<  "starting new event, istat: "<< istat <<"\n";
                  first_time = 1;
                }
            }
          else
            istat = 0;

          // RawDataFile
//			if (fd != NULL && first_time) free(buf);
        }
      while (istat & 1); // if required chamber not found, just get the next event.

      // calculate next requested event
      if (show_index++ == show_cnt)
        {
          // requested_event = upevt_.event_number + skip_cnt - show_cnt;
          show_index = 0;
        }

      if (istat & 4)
        {
          syslog(LOG_LOCAL1+LOG_ERR, "Could not get next event. EE");
          // set timer here
	  //			fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);
          need_event = 1;
          return;
        }
      else
        {
          if (upevt_.event_number < requested_event)
            {

              printf("==> Fast forward to event %d\n", requested_event);

              do
                {
                  // RawDataFile
                  rtval = fd->readNextEvent();
                  upevt_.event_number = evnum = event_num++;

//					std::cout << "event number 265: " << upevt_.event_number << std::endl;

                  if ( rtval == 0 )
                    {
                      /* If at end of file then set fd back to the beginning */
                      printf("==> Reached end of data file. Rewinding...\n");
                      requested_event = 0;
                      // RawDataFile
                      fd->close();
                      // RawDataFile
                      fd->open(file_name);
                      event_num = 1;
                      // set timer here
		      //						fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);
                      break;
                    }
                }
              while (evnum < requested_event-1);
              // set timer here
	      //				fl_set_timer(fd_event_display->show_event_timer, 0.1);
              need_event = 1;
              return;
            }
          else
            {
              need_event = 0;
	      //				subtract_monitor_peds(mean, rms);

              // Copy data from up upevt_ into common blocks used by event display
	      //				unpack_data_();
              unpack_data_cc(); //added 10-30-07
            }
        }

    } /* end if need_event */

// show events here
//	j_display_cc();
  jd->layout();

  // set timer here
//	fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);
/*
  Removed large section of commented code
*/
  disp_wire_strip = wire_strip_active;
  disp_alct_time = alct_time_active;
  disp_clct_time = clct_time_active;
  disp_sca        = sca_active;
  disp_atrig      = atrig_active;
  disp_ctrig      = ctrig_active;
  disp_sci_strips = sci_strips_active;
  disp_sci_wires  = sci_wires_active;
  need_event = 1;
  // set timer here
//	fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);

}

/*
  Removed large section of commented code
*/
