#ifndef _Switch_h_
#define _Switch_h_

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

#define MAX_MACS 50

namespace emu{
  namespace pcsw{

class Switch
{
  public:

    // switch stuff
    int swadd;
    int swadd2;
    int init;

    int link[4];   

    typedef struct Mac{
      char mac[19];
      char status[24];
    }MAC;

    typedef struct side{
      char *name;
      std::string label;
      int nswitch;
      int nport;
      int vlan;
      MAC pmac;
    }SIDE;

    SIDE side[49];

    typedef struct Sw{
      // status info
      char port[5];
      char media[7];
      char link[5];
      // interface info
      int rx;
      int rx_error;
      int rx_broad;
      int tx;
      int tx_error;
      int collision;
      // mac info
      char vlan[2];
      int nmacs;
      MAC mac[MAX_MACS];
      int nmacs_expected;
      MAC mac_expected[30];
    }SW;

    SW sw[4][12];
    SW old[4][12];
    char *ip[8];
    
    //pc stuff
    typedef struct pc_eth{
      long int rx_bytes;
      long int rx;
      long int rx_err;
      long int rx_drop;
      long int rx_fifo;
      long int rx_frame;
      long int rx_compressed;
      long int rx_multicast;
      long int tx_bytes;
      long int tx;
      long int tx_err;
      long int tx_drop;
      long int tx_fifo;
      long int tx_colls;
      long int tx_carrier;
      long int tx_compressed;
    }PC_ETH;
    
    PC_ETH eth[2];
    PC_ETH ethold[2];
    
    typedef struct pc_stats{
      char machine[12];
      char eth[5];
    }PC_STATS;
    
    PC_STATS pc[2];
    
    std::ofstream LogFileSwitch;
    std::string filebuf;



// constructor destructor
   Switch();
  ~Switch();

// switch configure commands
   void ResetSwitch();
   void BackupSwitch();
   void ResetCounters(int swtch,int prt);
   void CLRcounters();

// switch read commands
   void fill_switch_statistics();
   void fill_switch_macs();
   void fill_pc_statistics();
   void fill_ping();
   void fill_problems();

// parse commands
   void parse_status(int swtch,int prt);
   void parse_status_all(int swtch);
   void parse_interface(int swtch,int prt);
   void parse_interface_all(int swtch);
   void parse_mac(int swtch);
   void parser_pc(int ieth);

// html routines
   std::string html_ping();
   std::string html_port_status();
   std::string html_pc_status();
   std::string html_mac_table();
   std::string html_parse_problems(int swt);


// utilities
   void fill_name(char *var, char *line);
   int compare(char *a,char *b,int begin,int length);
   void fill_char(char *var,char *line,int first,int length);
   void dump_expected_macs(int ioff);
   void fill_expected_mac_table();
   void copy_stats_new2old();

// other unrelated utilities that needed a home
   std::string dlink_stats();
   std::string eth_hook_stats();
   std::string switch_stats(int lasthex);

   std::ostringstream OutputSwitch;

private:

};

  } //namespace pcsw
} //namespace emu

#endif
