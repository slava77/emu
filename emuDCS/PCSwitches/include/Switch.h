#ifndef _Switch_h_
#define _Switch_h_

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

#include "xdata/String.h"

#define MAX_MACS 50

namespace emu{
  namespace pcsw{

class Switch
{
  public:

    // switch stuff
    std::string sidelabel;
    int link[4];   

    int num_pcs;
    int num_connections;
    int num_switches;

    typedef struct Mac{
      std::string mac;
      std::string status;
    }MAC;

    typedef struct side{
      std::string name;
      std::string label;
      int nswitch;
      int nport;
      int vlan;
      std::string ipaddr;
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

    typedef struct ip{
      std::string label;
      std::string ipaddr;
    } IP;

    IP ip_addresses[4];
    
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
      std::string machine;
      std::string eth;
    }PC_STATS;
    
    PC_STATS pc[2];
    
    std::string filebuf;

// constructor destructor
   Switch();
  ~Switch();
   void initialize();
// switch configure commands
   void ResetSwitch(std::string switchTelnet);
   void PowerSwitch(std::string upsTelnet, std::string switchNum, std::string switchOperation);
   void BackupSwitch(std::string switchTelnet, std::string BackupDir);
   void ResetCounters(int swtch,int prt, std::string switchTelnet);
   void CLRcounters(std::string switchTelnet);

// switch read commands
   void fill_switch_statistics(std::string switchTelnet);
   void fill_switch_macs(std::string switchTelnet);
   void fill_pc_statistics();
   void fill_ping(std::string switchTelnet);
   void fill_problems(std::string switchTelnet);

// parse commands
//   void parse_status(int swtch,int prt);
   void parse_status_all(int swtch);
//   void parse_interface(int swtch,int prt);
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
   void fill_char(char *var,char *line,int first,int length);
   void dump_expected_macs();
   void fill_expected_mac_table();
   void copy_stats_new2old();

// other unrelated utilities that needed a home
   std::string dlink_stats();
   std::string eth_hook_stats();
   std::string switch_stats(int lasthex, std::string testScript);


private:

};

  } //namespace pcsw
} //namespace emu

#endif
