/*
  Daniel Aharoni- delay_tests.cpp
  DAharoni@ucla.edu
  Created summer 2007 during minus-side Slice Test on surface
  For information concerning this program see...
          http://twiki.cern.ch/twiki/bin/view/CMS/CSCTimingProgram

  Modifying the range of chambers you want to predict timing parameters 
    for can be done in set_range_to_all_chambers 
    
  The imputted configuration files can be changed in the top of the main() function.
    You can add as many read_in() functions as you would like.

  version record:
  * Modifications by Jay Hauser Sept. 2007 for new XML file format.

  * Modifications by Greg Rakness Oct. 2007
     - Moved constant parameters to the top of the file 
     - Cleaned up plotting, add option to print out plots to file
     - Fix average value computation of cfeb[0-4]delay to include wrap-around
     - Add option to compute predicted values using the _measured_ values.  This
       will prevent predictions which are off (e.g., match_trig_alct_delay + mpc_tx_delay)
       from tainting subsequent predictions.  This is especially useful for comparing
       each routine's computation with the measured values.

  *  Modifications by Greg Rakness Dec. 2007
     - add Global Run at the End of November data
     - organize the code by functionality
     - include TTCrxCoarseDelay and l1a_latency_from_mpc_back_to_ccb work for 
       tmb_lct_cable_delay, alct_l1a_delay, and tmb_l1a_delay
     - add clct_stagger for ME11 chambers

  *  Modifications by Greg Rakness 1 Feb. 2008, 
     - modify TTCrxCoarseDelay to (TTCrxCoarseDelay + fiber length) for the L1A delay parameters
     - fix bug to compute cable lengths in nsec before evaluating cfeb_cable_delay 
     - For xml output file:
       .add label and correct CrateID to the VMEController xml output file
       . remove CFEB Number=4 from ME1/3 chambers

  *  Modifications by Greg Rakness 18 Apr. 2008, 
     - creation of the predicted xml files
       . config_template.xml 
         * conform to the format for the plus side currently at point 5
         * remove Broadcast crate
       . add crate controller MAC addresses from 
         http://www.physics.ohio-state.edu/~cms/Netgear_Switch/VME_Crate2Switch_Port.pdf
       . ttc_fibers.txt
         * No fiber lengths available for generation.  Thus, simply copy the plus
           side values and use them for the minus side values.  Not right, but will
           at least follow trend...
	 * increase ttc_ccb_fiber_offset = 3 -> 5 to agree with the values we have at point 5
         * increase cfeb_dav_processing_time_offset = 5 -> 6 to make it not give negative values

  *  Modifications by Jay Hauser 23 May 2008, 
     - Updated ttc_fibers.txt to include measured lengths for minus-side fibers
     - Add echo of names of files read in that contain cable delays.
     - Rework program (a lot of small fixes) to allow this to compile without errors using g++.
     - Note that previously const int afeb_fine_delay_asic_nsec_per_setting = 2.2; obviously not an
       integer, convert to double (gcc changes 2.2 --> 2)
     - Concatenate minus and plus endcap files for cable lengths and afeb thresholds and delays.
     - Add reading in Dayong fine timing corrections file.

  *  Modifications by Jay Hauser 4 June 2008, 
     - Implemented Dayong LCT relative timing corrections by changing AFEB fine delays, and if
       over-range or negative, wrap around with subsequent corrections to mpc_tx_delay, mpc_rx_delay,
       tmb_l1a_delay.  See subsequent fixes described below on 18 Aug 2008.

  *  Modifications by Jay Hauser 27 June 2008, 
     - Reverse sign of fine timing correction back to positive "correction" in Dayong's files now.
     - Some changes to the XML file format, find out if we need to make other code corrections

  * Modifications by Jay Hauser 4 Aug 2008
     - Add new function (28) to adjust measured TTCrxCoarse delays and shift other parameters mpc_tx_delay
       tmb_l1a_delay, alct_l1a_delay, and tmb_lct_cable_delay, to accomodate it.
     - Hopefully this will work out with integer bunch crossing delays and the next step will
       be to program shifts for non-integer shifts to TTCrxCoarse delays.

  * Modifications by Jay Hauser 18 Aug 2008
     - Fix Dayong relative LCT timing correction algorithm to no longer shift mpc_rx_delay (a 
       fixed value of 6 since all versions of TMB firmware after ~Oct. 2007, downloaded ~June 2008).
     - Fix Dayong relative LCT timing correction algorithm to put in wrap-around shifts for alct_l1a_delay 
       and tmb_lct_cable_delay automatically so they don't need to be re-measured.
     - Reduce ttc_ccb_fiber_offset = 5 -> 2 to center range of TTCrxCoarseDelay
     - (in progress) Implement TTCrxFineDelay corrections inside adjust_ttc_delays:
       > Calculate integer number of ns delay.
       > Display the fine delay settings (ns), as well as the sub-ns deviations from perfection.
       > Read in measured values and keep separate from predicted values

  * Modifications by Jay Hauser 28 Aug 2008 fixed 16-Sep-2008 (TTCrxFineDelay values were not written!)
     - Put in TTCrxFineDelay corrections (in option 28). Plan is:
       > Add correction to TTCrxFineDelay by crate, afeb_fine_delay values by chamber
       > Calculate rollover in afeb_fine_delays
       > If roll-over, there is less ALCT delay to L1A, so just like situation with adjusting
         for Dayong's timing offsets (option 22), need to adjust this chamber's:
         1) mpc_tx_delay += 1
	 2) alct_l1a_delay +=1
	 3) tmb_l1a_delay +=1
	 4) tmb_lct_cable_delay +=1
       > Make sure the right parameters get written to the output XML file.

  * Modifications by Jay Hauser 3-Oct-2008 and 5-Oct-2008
     - Fix rounding of negative values for afeb_fine_delay values from TTCrx delay changes, 
       a la --> rounded = (int) (val>0 ? val+0.5 : val-0.5);
     - Put in bx number offsets for ALCT and CLCT (from Chad) as option 29

  * Modifications by Jay Hauser 14-Oct-2008 to 21-Oct-2008 
    - add save plot to file feature for numerous options.
    - Change calculations of cfeb and alct phases to keep floating point until the actual
      setting is calculated.  Tweak the predictions a little to improve them. Print out
      lists of chambers that disagree badly.  Fix the "wrapping" of CFEB phases at 12->0 boundary.
    - Change the calculations of alct_l1a_delay and tmb_l1a_delay to take advantage of 
      isochronous PC clocking--> big improvement.
    - Tweak many other options to match CMS conditions and to remove unnecessary (int)'s.
    - Fixed bug (+1) in calculation of number of bins in plots with integers

*/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
// 
#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <Tfile.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2f.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraph.h>
//
#include <vector>
#include <string>
#include <math.h>
using namespace std;

//structures--------------------------------------------------------------------

struct chambers { //Holds all information for a chamber
  int comp_timing;
  int alct_drift_delay;
  int clct_drift_delay;
  int match_trig_window_size;
  //
  bool has_measured_values;
  //
  double clock_phase_difference;
  double cable_difference_in_bx;
  double afeb_fine_delay_correction;
  //
  int mpc_tx_delay;
  int predicted_mpc_tx_delay;
  int mpc_rx_delay;
  int predicted_mpc_rx_delay;
  int tmb_lct_cable_delay;
  int predicted_tmb_lct_cable_delay;
  int tmb_l1a_delay;
  int predicted_tmb_l1a_delay;
  int tmb_bxn_offset;
  //CFEB -------------------------------------------
  int cfeb_cable_delay;
  int predicted_cfeb_cable_delay;
  double cfeb_tmb_skew_length;
  char cfeb_tmb_skew_revision;
  double clct_skewclear_delay;
  int cfeb[5];
  int predicted_cfeb_delay;
  int average_cfeb_delay;
  int cfeb_dav_cable_delay;
  int predicted_cfeb_dav_cable_delay;
  //ALCT -------------------------------------------
  int alct_l1a_delay;
  int predicted_alct_l1a_delay;
  int match_trig_alct_delay;                        //rounded to int
  double not_rounded_match_trig_alct_delay;         //not rounded
  int predicted_match_trig_alct_delay;
  double predicted_not_rounded_match_trig_alct_delay;
  double alct_skewclear_length;
  char alct_skewclear_revision;
  double alct_skewclear_delay;
  int alct_tx_clock_delay;
  int alct_rx_clock_delay;
  int predicted_alct_tx_clock_delay;
  int predicted_alct_rx_clock_delay;
  int alct_dav_cable_delay;
  int predicted_alct_dav_cable_delay;
  int alct_bxn_offset;
  //------- Calculated in match_trig_alct_delay() ---------
  int afeb_fine_delay_average;
  int init_afeb_fine_delay_average;
  int max_afeb_fine_delay;
  int init_max_afeb_fine_delay;
  double sp_bx_offset_sector;
  double sp_bx_offset_global;
  vector<int> afeb_fine_delay;
  vector<int> init_afeb_fine_delay;
  vector<int> init_afeb_thresh;
  double afeb_cable_adjustment;
  void clear_measured_timing_parameters() {
    has_measured_values = false;	
    comp_timing = -1;
    alct_drift_delay = -1;
    clct_drift_delay = -1;
    average_cfeb_delay = -1;
    cfeb[0] = cfeb[1] = cfeb[2] = cfeb[3] = cfeb[4] = -1;
    alct_tx_clock_delay = -1;
    alct_rx_clock_delay = -1;
    match_trig_alct_delay = -1;
    mpc_tx_delay = -1;
    mpc_rx_delay = -1;
    cfeb_cable_delay = -1;
    cfeb_dav_cable_delay = -1;
    alct_dav_cable_delay = -1;
    tmb_lct_cable_delay = -1;
    tmb_l1a_delay = -1;
    alct_l1a_delay = -1;
    tmb_bxn_offset = -1;
    alct_bxn_offset = -1;
    not_rounded_match_trig_alct_delay = -1;
    sp_bx_offset_sector = -999.;
    sp_bx_offset_global = -999;
  }
};
//
//struct crates { //Holds all information for a crate
//}
//
//function ---------------------------------------------------------------------
void options();                            //options of what type of data/delays to display
void output_data_to_file();                //used to import predicted timing parameters to excel or other type programs
void create_config_file();                 //creates a configuration .xml file for a defined trigger sector
void read_in_config(const char file_name[]); //reads in XML file (usually measured) data
void read_in_cable_lengths(const char file_name[]);              //reads in cable length ALCT and CFEB + revision
void read_in_afeb_fine_delays(const char file_name[]);
void read_in_afeb_thresh(const char file_name[]);
void set_range_to_all_chambers();          //opens up all chambers
void select_chamber();                     //allows the user to input desired chamber
void cfeb_delay_function();                //calculates predicted and measured cfeb_delay
void alct_tx_delay_function();     //calculates predicted and measured alct_tx_delay_setting
void alct_rx_delay_function();     //calculates predicted and measured alct_rx_delay
void alct_to_cfeb_skewclear_delay();       //calculates cfeb/alct skewdelay
void display_type();                       //Choose between text based or graphical based display of data
void clct_alct_match_window();             //calculates match window
void tmb_lct_cable_delay();                //calculates cable delay
void cfeb_dav_cable_delay();               //calculates predicted cfeb_dav_cable_delay
void alct_dav_cable_delay();               //calculates predicted alct_dav_cable_delay
void afeb_cable_adjustment();              
void cfeb_cable_delay();
void set_parameters();                     //user can adjust comp_timing, clct(alct)_drift_delay, match_trig_window_size, and z_side from here
void mpc_delays();                         //calculates mpc_tx_delay and mpc_rx_delay
void tmb_l1a_delay();                  
void alct_l1a_delay();
void adjust_LCT_fine_timing();
void adjust_ttc_delays();
void adjust_bx_offsets();
void adjust_measured_parameters();
void TTCrxDelayCalc();
void display_TTCrxDelayCalc();
double time_of_flight(int s, int z, int y, int x);
void output_measured_parameters(string file_name);
char difference_beyond_thresh(int measured, int predicted, int thresh, int z, int y, int x);
void display_data();                       //displays the user's choice
void display_graph();                      //function to display data in histograms
bool exit();                               //asks if user would like to quit and exit program
void predict_all_timing_parameters();      //calls all functions to calculate predicted timing parameters. 
void afeb_fine_delay_calculations();

//global variables --------------------------------------------------------------
int c[3] = {};                             //holds the value of a chamber #, ME(side)/c[0]/c[1]/c[2]
int side[2] = {};
int choice = 0;                            //user inputs his/her option choice to this variable
int display_choice = 0;                    //0 for texted based, 1 for graphical.  Given value in display_type() fucntion
int number_of_chambers = 0;                //holds the number of chambers with data in them (will calculate itself)
int number_of_chambers_of_interest = 0;    //number of chambers user is interested in (picked during select_chambers())
int start_chamber[5][4];                   //the starting chamber in the range of chambers in .txt file
int end_chamber[5][4];                     //the ending chamber in the range of chamers in .txt file
int chamber_limit[2][5][4] = {};           //holds the low and high chamber that the user is interested in.  First cell: 0 = low, 1 = high
chambers ME[2][5][4][37];                  //an array of all ME- and ME+ chambers
double fiber_length[2][5][13] = {};        //TTC to CCB fiber lengths
int TTCrxCoarseDelayPred[2][5][13] = {};   //units are bx
int TTCrxCoarseDelayMeas[2][5][13] = {};
int TTCrxFineDelayPred[2][5][13] = {};     //units are ns, not bx
int TTCrxFineDelayMeas[2][5][13] = {};
int ttc_rx_corr[2][5][13] = {};
int ttc_rx_corr_fine[2][5][13] = {};
//
unsigned ChamberToPCMap[5][4][37] = {};
//
//
//------YOU-------CAN--------MODIFY--------THESE-------
//
int alct_drift_delay = 2;
int clct_drift_delay = 2;
int comp_timing = 2;
int match_trig_window_size = 7;
//
//Show text values only for chambers with measurements:
bool display_only_measured_chambers = true;    
//
//Make a gif file as well whenever a plot is displayed:
bool send_plots_to_file = true;               
//
// When appropriate, use the measured values in the xml file so as not to propagate 
// a bad prediction of one value on to the next predicted value.  Especially 
// useful for match_trig_alct_delay, which is a tough value to predict:
bool use_measured_values = false;               
//
// ------ Set allowed difference between measured and predicted ------------
int cfeb_delay_thresh = 2;
int alct_tx_clock_delay_thresh = 2;
int alct_rx_clock_delay_thresh = 2;
int match_trig_alct_delay_thresh = 1;
int mpc_tx_delay_thresh = 1;
int mpc_rx_delay_thresh = 1;
int cfeb_cable_delay_thresh = 0;
int cfeb_dav_cable_delay_thresh = 1;
int alct_dav_cable_delay_thresh = 1;
int tmb_lct_cable_delay_thresh = 1;
int tmb_l1a_delay_thresh = 0;
int alct_l1a_delay_thresh = 0;
//
const double ddd_asic_nsec_per_setting = 2.;
const double afeb_fine_delay_asic_nsec_per_setting = 2.2;
//
// constant values to tune which affect the predictions:
//
// cfeb[0-4]delay (rx clock delay):
const double internal_cfeb_rx_clock_delay_offset = 17.0; //units are ns
//
// alct_tx_clock_delay:
const double internal_alct_tx_clock_delay_offset = 19.5; //units are ns
//
// alct_rx_clock_delay:
const int alct_tx_rx_offset = 3;
//
// mpc_tx_delay:
int clct_alct_latency_difference = 4;      
//
// mpc_rx_delay:
const int transmission_plus_mpc_processing_time = 4;
//
// TTCrxDelayCalc (tmb_l1a_delay, alct_l1a_delay, tmb_lct_cable_delay)
const int ttc_ccb_fiber_offset = 2;
//
// tmb_l1a_delay, alct_l1a_delay, tmb_lct_cable_delay:
const int l1a_latency_from_mpc_to_ttc_output = 101;  //predictions for TeamA ME+2 far side...
//                                                   These predictions dissociate the (fiber+TTCrxCoarseDelay) from the SP
//const int l1a_latency_from_mpc_to_ttc_output = 117;  //Global trigger path at end of November 2007
//                                                   and predictions for TeamA ME+1 far side...
//                                                   These predictions had the bug of the fiber time accounted 
//                                                   for as TTCrxCoarseDelay ONLY.
//const int l1a_latency_from_mpc_to_ttc_output = 118;  //Global trigger path at end of September 2007
//const int l1a_latency_from_mpc_to_ttc_output = 156;   //value for minus side slice test Summer 2007
//
// tmb_lct_cable_delay:
const int tmb_lct_cable_delay_offset = -118; // JH ad hoc needed for Oct 2008 data
//const int tmb_lct_cable_delay_offset = -117; // predictions for TeamA ME+2 far side...
//const int tmb_lct_cable_delay_offset = -118; //Value for xLatency = 0 (Global Run at End November and GRESeptember)
//const int tmb_lct_cable_delay_offset = 6; //value for minus side slice test Summer 2007
// 
// alct_l1a_delay:
const int tmb_to_alct_propagation = 0;
//Algo. #1: const double alct_l1a_delay_offset = 139.11;
//Algo. #2: JH big improvement seen:
const double alct_l1a_delay_offset = 135.22;
//
// cfeb_dav_cable_delay:
//const int cfeb_dav_processing_time_offset = 5;  
//const int cfeb_dav_processing_time_offset = 6;  
const double cfeb_dav_processing_time_offset = 5.4; //JH 15-Oct-08 make this a floating point knob (better results)

//
// alct_dav_cable_delay:
//const int alct_dav_processing_time_offset = 6;
const double alct_dav_processing_time_offset = 6.8;
//
//
// Here we go: start by clearing the chamber parameter structure
//---------------------------------------------------------------------------------------------------------
//
int main() {
  //
  int x, y, z;
  int z_side = 0;
  int key = 0;
  set_range_to_all_chambers();
  cout << endl << "Resetting all timing parameters" << endl;
  for (z_side = 0; z_side < 2; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { 
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 
	  ME[z_side][z][y][x].clear_measured_timing_parameters();
	}
      }
    }    
  }    
  //
  // Read in additional information from a few files:
  //
  cout << "Reading in cable lengths, afeb_fine_delays, and afeb_thresholds for the + side" << endl;  
  //-------------------------------------------------------------------------------------    
  string filename1="skewclear_cable_lengths_all.txt";
  cout << "Read in cable lengths from " << filename1 << endl;
  read_in_cable_lengths(filename1.c_str());
  //
  string filename2="afeb_fine_delays_all.txt";
  cout << "Read in afeb_fine_delays from " << filename2 << endl;
  read_in_afeb_fine_delays(filename2.c_str());
  //
  string filename3="afeb_thresholds_all.txt";
  cout << "Read in afeb_thresholds from " << filename3 << endl;
  read_in_afeb_thresh(filename3.c_str());
  //
  // Read in the best available XML files
  //
  cout << "Reading in measured timing parameters" << endl;
  //
  //June 2008: well advanced with commissioning of both disks, best available files:  
  //string filename4="measured_xml/VME_MEm1234_20080625.xml";
  //string filename5="measured_xml/VME_MEm1234_20080625.xml";
  //Aug 2008: new files
  //string filename4="measured_xml/VME_minus_20080731_midweek_global.xml";
  //string filename5="measured_xml/VME_plus_20080731_midweek_global.xml";
  //  string filename4="measured_xml/VME_minus_20080813_midweek_global_day2.xml";
  // string filename5="measured_xml/VME_plus_20080813_midweek_global_day2.xml";
  //
  //1-crate test xml files:
  //
  //string filename4="measured_xml/am.xml";
  //string filename5="measured_xml/ap.xml";
  //
  //Oct 2008: recent files
  // string filename4="VME_minus_20080915_LHC.xml";
  // string filename5="VME_plus_20080915_LHC.xml";
  string filename4="VME_minus_TTCrxFineDelays_bxn_LHC.xml";
  string filename5="VME_plus_TTCrxFineDelays_bxn_LHC.xml";
  //
  cout << "Reading in measured XML from " << filename4 << " ..." << endl;
  read_in_config(filename4.c_str());
  //
  cout << "Reading in measured XML from " << filename5 << " ..." << endl;
  read_in_config(filename5.c_str());
  //
  // Make predictions of the parameters:
  //
  cout << "Predicting timing parameters" << endl << endl;
  //---------------------------------------
  predict_all_timing_parameters();         //calculates values for each chamber
  //
  // Now ask the user what he/she would like to do:
  //
  while (1) {                              //start of user interface
    // 
    options();                             //show current settings and options menu, return user menu choice
    //
    display_choice = 0;                    //default is text display of results
    //
    if ((choice >= 1) && (choice <= 29) || (choice == 99) ) {  //if choice picked is an allowed choice
      //
      if((choice >= 1) && (choice <= 17)) { //these require display of some sort
	if (choice != 5) 	  
	  display_type();                 //user picks how to display the data (text or graphic)
	select_chamber();                 //allows user to select chamber(s) of interest
	//
	if (display_choice == 1) {	      //if users picks to display graphically
	  cout << "DEBUG: calling display_graph();" << endl;
	  display_graph();                
	} else if (display_choice == 0) {     //if user picks to display text based
	  display_data(); 
	}
      }
      //
      if (choice == 18) {                //output predicted parameters to an XML file
	select_chamber();
	output_data_to_file();
      }
      //
      if (choice == 19)                  //create XML file (one crate?)
	create_config_file();
      //
      if (choice == 20)    //Modify comp_timing, alct/clct_drift_delay, match_trig_window_size predicted values
	set_parameters();
      //
      if (choice == 21) {                //change between text display for all chambers and only measured chambers
	if (display_only_measured_chambers == true)
	  cout << endl << endl << "Allowing only measured chambers to be displayed" << endl;
	else if (display_only_measured_chambers == false)
	  cout << endl << endl << "Allowing all chambers to be displayed" << endl;
	cout << endl << "1: Allow display for all chambers" << endl;
	cout << "2: Allow display for only measured chambers" << endl << endl;
	cout << "===>";
	cin >> key;
	display_only_measured_chambers = key - 1;
	if (display_only_measured_chambers == true)
	  cout << endl << endl << "Allowing only measured chambers to be displayed" << endl;
	else if (display_only_measured_chambers == false)
	  cout << endl << endl << "Allowing all chambers to be displayed" << endl;
      }
      //
      if (choice == 22)     //adjust fine timing mpc/afeb_fine delays to align LCTs at SP (Dayong corrections)
	                    // or to adjust TTCrxFineDelay values, then write new XML config file
	adjust_LCT_fine_timing();
      //
      if (choice == 23)                 //change comp_timing, clct_drift_delay, and/or alct_drift_delay for MEASURED
	                                //values, then write config XML file
	adjust_measured_parameters();
      //
      if (choice == 24)                 //calculate and display the TTCrxCoarseDelays based on fiber lengths
	display_TTCrxDelayCalc();
      //
      if (choice == 25) {               //"chamber line up at MPC" -JH what does that mean??
	select_chamber();
	display_data();
      }
      //
      if (choice == 26) {               //Send plots to output files
	//
	if (send_plots_to_file == true) {
	  cout << endl << endl << "Currently sending plots to files" << endl;
	} else if (send_plots_to_file == false) {
	  cout << endl << endl << "Currently NOT sending plots to files..." << endl;
	}
	//
	cout << endl;
	cout << "1: No, do not send plots to files" << endl;
	cout << "2: Yes, do send plots to files"    << endl << endl;
	cout << "===>";
	cin >> key;
	send_plots_to_file = key - 1;
	if (send_plots_to_file == true) {
	  cout << endl << endl << "OK, sending plots to files" << endl;
	} 
	else if (send_plots_to_file == false) {
	  cout << endl << endl << "OK, NOT sending plots to files" << endl;
	}
      }
      //
      if (choice == 27) {                //"Use measured values for subsequent predictions"
	//
	if (use_measured_values == true) {
	  cout << endl << endl;
	  cout << "Currently using the measured values as basis to predict the following items:" << endl;
	  cout << "- mpc_rx_delay" << endl;
	  cout << "- tmb_l1a_delay" << endl;
	  cout << "- alct_l1a_delay" << endl;
	} 
	else if (use_measured_values == false) {
	  cout << endl << endl << "Currently predicting all values..." << endl;
	}
	//
	cout << endl;
	cout << "1: Predict all values" << endl;
	cout << "2: Use measured values as basis for next-step predictions (good for display)" << endl << endl;
	cout << "===>";
	cin >> key;
	use_measured_values = key - 1;
	if (use_measured_values == true) {
	  cout << endl << endl << "OK, using measured values" << endl;
	} else if (use_measured_values == false) {
	  cout << endl << endl << "OK, predicting all values" << endl;
	}
	//
	predict_all_timing_parameters();  //calculate again using the desired input values
	//
      }
      if (choice == 28) 
	adjust_ttc_delays();
      //
      if (choice == 29)
	adjust_bx_offsets();
      //
      if ((choice != 20) && (choice != 21) && (choice != 99))
	if (exit())                         //if exit returns 1 then breaks the loop and exits program 
	  break;
      //
      if (choice == 99)
	break;
    }
    //
  } // end of repeat while loop
  
  // --------- clears vectors ----------------------------------------------
  for (z_side = 0; z_side < 2; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { 
        for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 
	      ME[z_side][z][y][x].afeb_fine_delay.clear();
	      ME[z_side][z][y][x].init_afeb_fine_delay.clear();
	      ME[z_side][z][y][x].init_afeb_thresh.clear();
		}
      }
    }
  }
  return 0;
}
//
//---------------------------------------------------------------------------------------------------------
//
void predict_all_timing_parameters() {
  //
  // **********FUNCTIONS NEED TO STAY IN THIS ORDER******************************
  //
  // compute delays depending strictly on the cable length
  //
  //First the TTCrx{Coarse/Fine}Delay calculations from optical fibers:
  TTCrxDelayCalc();
  //
  //Then other cable delays from copper cable lengths and propagation speeds:
  alct_to_cfeb_skewclear_delay();
  cfeb_cable_delay();
  afeb_cable_adjustment();
  //
  // compute communication phases
  cfeb_delay_function();
  alct_rx_delay_function(); //tx called in this function
  //
  // Align the ALCT with the CLCT, assuming that the ALCT (critical latency) has been lined up at the SP
  mpc_delays(); 
  clct_alct_match_window();
  //
  // receive the L1A back at the components
  tmb_lct_cable_delay();
  tmb_l1a_delay();
  alct_l1a_delay();
  //
  // receive the Data AVailable bits at the DMB
  cfeb_dav_cable_delay();
  alct_dav_cable_delay();
}
//
// Compute delays depending strictly on cable lengths
//
//---------------------------------------------------------------------------------------------------------
//
void TTCrxDelayCalc() {
  char VME[11] = {};
  double length;
  double max_length = 0;
  int station;
  // int min_coarse = 15;
  // int max_coarse = 0;
  int x, y;
  int z_side=0;
  char cz_side;
  int crate;
  int number_of_crates[5] = {0,12,6,6,6};
  //
  ifstream fiber("ttc_fibers.txt", ios::in);
  while (fiber >> VME >> length) {
    if (VME[3] == '-') {
      cz_side = '-';
      z_side = 0;
    }
    else if (VME[3] == '+') {
      cz_side = '+';
      z_side = 1;
    }
    station = (int) VME[5] - 48;
    crate = 10 * ((int) VME[8] - 48) + ((int) VME[9] - 48);
    //   cout << endl << station << " " << crate;
    //
    //4.9 ns/m in fiber according to Bruce Taylor article, also 20-Feb-08 email from Matveev.
    //4.960 ns/m using Casenove number for refractive index n=1.487
    //
    fiber_length[z_side][station][crate] = (length * 4.960) / 25; // using bx units
  }
  fiber.close();
  //
  for (z_side = 0; z_side < 2; z_side++) {
    for (x = 1; x <= 4; x++) {
      for (y = 1; y <= number_of_crates[x]; y++) {
	if((fiber_length[z_side][x][y]) && (fiber_length[z_side][x][y] > max_length)) 
	  max_length = fiber_length[z_side][x][y];
      }
    }
  }
  //
  //Calc Delay setting longest fiber to zero initially:
  //
  for (z_side = 0; z_side < 2; z_side++) {  
    for (x = 1; x <= 4; x++) {
      for (y = 1; y <= number_of_crates[x]; y++) {
	if (fiber_length[z_side][x][y])
	  TTCrxCoarseDelayPred[z_side][x][y] = int(0.5 + (max_length - fiber_length[z_side][x][y]));
	  TTCrxFineDelayPred[z_side][x][y] = int( 25*( (0.5 + (max_length - fiber_length[z_side][x][y]))
	    -TTCrxCoarseDelayPred[z_side][x][y]) );
      }
    }
  }
  //  
  //Apply a global shift:
  //
  for (z_side = 0; z_side < 2; z_side++) {  
    for (x = 1; x <= 4; x++) {
      for (y = 1; y <= number_of_crates[x]; y++) {
	if (fiber_length[z_side][x][y]) {
	  TTCrxCoarseDelayPred[z_side][x][y] += ttc_ccb_fiber_offset;
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void cfeb_cable_delay(){
  /*      Calculates cfeb_cable_delay based on clct_skewclear_delay
	  If skew_delay is below 53 then make cfeb_cable_delay = 1
	  else make cfeb_cable_delay = 0.
	  Twiki page said cut off value should be 50 not 53 but it
	  seems like 53 works better.  This is the spot in the largest
          gap between cable lengths
  */
  int x, y, z;
  int z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  if (ME[z_side][z][y][x].clct_skewclear_delay > 53)
	    ME[z_side][z][y][x].predicted_cfeb_cable_delay = 0;
	  else
	    ME[z_side][z][y][x].predicted_cfeb_cable_delay = 1;
	  //	  if (z==1 && y==1) 
	  //	    std::cout << "CFEB cable length E" << z_side 
	  //		      << " S" << z
	  //		      << " R" << y
	  //		      << " C" << x
	  //		      << " = " << ME[z_side][z][y][x].cfeb_tmb_skew_length
	  //		      << "m takes " << ME[z_side][z][y][x].clct_skewclear_delay 
	  //		      << "nsec" << std::endl;  
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
// 
void alct_to_cfeb_skewclear_delay() { 
  /*
    Calculates the alct and cfeb skewclear delay based on
    cable length and cable revision.  This calculation is also
    done in a different function.  Might be able to delete this.
  */
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { 
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  if (ME[z_side][z][y][x].cfeb_tmb_skew_revision  == 'N')
	    ME[z_side][z][y][x].clct_skewclear_delay = ME[z_side][z][y][x].cfeb_tmb_skew_length * (4.7);
	  else
	    ME[z_side][z][y][x].clct_skewclear_delay = ME[z_side][z][y][x].cfeb_tmb_skew_length * (5.2);
	  //
	  //	  if (z==1 && y==1) 
	  //	    std::cout << "CFEB cable length E" << z_side
	  //		      << " S" << z
	  //		      << " R" << y
	  //		      << " C" << x
	  //		      << " = " << ME[z_side][z][y][x].cfeb_tmb_skew_length
	  //		      << "m takes " << 	    ME[z_side][z][y][x].clct_skewclear_delay 
	  //		      << "nsec" << std::endl;  
	}
      }
    }
  }
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 
	  if (ME[z_side][z][y][x].alct_skewclear_revision == 'N')
	    ME[z_side][z][y][x].alct_skewclear_delay = ME[z_side][z][y][x].alct_skewclear_length * (4.7);
	  else
	    ME[z_side][z][y][x].alct_skewclear_delay = ME[z_side][z][y][x].alct_skewclear_length * (5.2);
	  //
	  //	  if (z==1 && y==1) 
	  //	    std::cout << "ALCT cable length E" << z_side 
	  //		      << " S" << z
	  //		      << " R" << y
	  //		      << " C" << x
	  //		      << " = " << ME[z_side][z][y][x].alct_skewclear_length
	  //		      << "m takes " << ME[z_side][z][y][x].alct_skewclear_delay 
	  //		      << "nsec" << std::endl;  
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void afeb_cable_adjustment(){
  /*
    Calculates an adjustment parameter for the difference in ME-X/1 and ME-X/2 afeb cable lengths
    It seems that for ME-X/2 chambers, the shortest cable length is longer than in ME-X/1.
  */
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 
	  if (y == 2)
	    ME[z_side][z][y][x].afeb_cable_adjustment = (1 + 5) / afeb_fine_delay_asic_nsec_per_setting;
	  if (y == 1)
	    ME[z_side][z][y][x].afeb_cable_adjustment = (1 + 0) / afeb_fine_delay_asic_nsec_per_setting;
	}
      }
    }
  }
}
//
// Compute communication phases
//---------------------------------------------------------------------------------------------------------
//
void cfeb_delay_function(){                                  
  /*
    Loops through all chambers and calculates the average and predicted cfeb_delay.
    average_cfeb_delay = (the sum of cfeb0-4) / 5
    predicted_cycle = number of ns after last BX (should change the name to something different)
    predicted_cfeb_delay = the number of ns needed to delay the signal to reach the begining of the next BX (in setting steps not ns)
  */
  int x, y, z, z_side = 0;                                                          //used for for loops
  double dt;
  int meas, pred, delta;
  char cz_side[2];
  cz_side[0]='-';
  cz_side[1]='+';
  //
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){    
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { 
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {  
	  if (ME[z_side][z][y][x].cfeb[0] == -1)         //case that data not read in
	    ME[z_side][z][y][x].average_cfeb_delay = -1;
	  else {                                         //have timing data
	    //
	    // correct for wrapping, if needed
	    bool perform_wrap = false;
	    for (int label=1; label<5; label++) {
	      double diff=abs(ME[z_side][z][y][x].cfeb[0] - ME[z_side][z][y][x].cfeb[label]) ;
	      if (diff>6.) perform_wrap = true;
	    }
	    //
	    double tmp[5] = {};
	    //
	    for (int label=0; label<5; label++) {   //Get all the values before averaging:
	      //
	      if (perform_wrap) {
		//		cout << " before cfeb[" << label << "] = " << ME[z_side][z][y][x].cfeb[label] << "... ";
		tmp[label] = (ME[z_side][z][y][x].cfeb[label] + 12.5);
		if (tmp[label] > 15.) 
		  tmp[label] -= 12.5;
		//		cout << " after tmp[" << label << "] = " << tmp[label] << "... " << endl;
	      } else {
		//			cout << "...NOT wrapping" << endl;
		tmp[label] = ME[z_side][z][y][x].cfeb[label];
	      }
	      //
	    }
	    ME[z_side][z][y][x].average_cfeb_delay = (int) (0.5 + (tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4])/5.);
	  }
	  //JH new calculation: calculate relevant floating point time interval
	  dt = internal_cfeb_rx_clock_delay_offset + 2*ME[z_side][z][y][x].clct_skewclear_delay;
	  //JH then calculate time remainder, in base 25ns (one clock cycle):
	  dt = 25 - (dt - 25*int(dt/25)); 
	  //JH then calculate the setting of the DDD delay device (2ns steps, range 0-12):
	  ME[z_side][z][y][x].predicted_cfeb_delay = ( (int) (dt/ddd_asic_nsec_per_setting) ) %13;
	  //old DA formula:
	  //    ((int) (0.5 + (25 - (int) ( 0.5 + internal_cfeb_rx_clock_delay_offset 
	  //		  + 2 * ME[z_side][z][y][x].clct_skewclear_delay) % 25)/ddd_asic_nsec_per_setting)) % 13;
	  //
	  //JH Print out details if a bad time match:
	  //
	  meas = ME[z_side][z][y][x].average_cfeb_delay;
	  pred = ME[z_side][z][y][x].predicted_cfeb_delay;
	  delta = meas-pred;
	  //cout << "JH DEBUG delta =" << delta << " from meas=" << meas << " and pred=" << pred << endl;
	  delta = ( abs(delta+13) %13);
	  if (delta > 6) delta=13-delta;
	  //cout << "         ------>" << delta << endl;
	  //JH print out chambers with bad matches right now:
	  if (delta > 3) {
	    cout << "cfeb_delay_function: bad match, delta=" << delta 
		 << " for ME" << cz_side[z_side] << z << "/" << y << "/" << x 
		 << ": Meas=" << meas << ", Pred=" << pred << endl;
	  }
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void alct_tx_delay_function() {                     
  /*       Function calculates the predicted_alct_tx_clock_delay, predicted_alct_tx_cycle
	   and alct_skewclear_delay.
	   alct_skewclear delay is calculated from the alct_skewclear_length and 
	   alct_skewclear_revision.
	   predicted_alct_tx_cycle = (internal_alct_tx_clock_delay_offset + 2 * alct_skewclear_delay) MOD 25.  
	   The predicted alct_tx_cycle is the number of ns past the last BX.
	   alct_skewclear_delay = (25 - predicted_alct_tx_cycle)/2 .  Inside the ()
	   gives the number of ns till next BX.  We divide by 2 to convert to setting.
  */
  int x, y, z, z_side;
  double dt;
  int meas, pred, delta;
  char cz_side[2];
  cz_side[0]='-';
  cz_side[1]='+';
  //
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 

	  //JH new calculation:
	  dt = internal_alct_tx_clock_delay_offset + 2*ME[z_side][z][y][x].alct_skewclear_delay;
	  //JH then calculate time remainder, in base 25ns (one clock cycle):
	  dt = 25 - (dt - 25*int(dt/25)); 
	  //JH then calculate the setting of the DDD delay device (2ns steps, range 0-12):
	  ME[z_side][z][y][x].predicted_alct_tx_clock_delay = ( (int) (dt/ddd_asic_nsec_per_setting) ) %13;
	  //Old DA calculation:
	  //  ME[z_side][z][y][x].predicted_alct_tx_clock_delay = 
	  //  ((int) (0.5 + 
	  //	    (25 - (int)(0.5 + internal_alct_tx_clock_delay_offset
	  //	     + 2 * ME[z_side][z][y][x].alct_skewclear_delay) % 25)/ddd_asic_nsec_per_setting)) % 13;
	  //
	  //JH Print out details if a bad time match:
	  //
	  meas = ME[z_side][z][y][x].alct_tx_clock_delay;
	  pred = ME[z_side][z][y][x].predicted_alct_tx_clock_delay;
	  delta = meas-pred;
	  //cout << "JH DEBUG delta =" << delta << " from meas=" << meas << " and pred=" << pred << endl;
	  delta = ( abs(delta+13) %13);
	  if (delta > 6) delta=13-delta;
	  //cout << "         ------>" << delta << endl;
	  //JH print out chambers with bad matches right now:
	  if (delta > 3) {
	    cout << "alct_tx_delay_function: bad match, delta=" << delta 
		 << " for ME" << cz_side[z_side] << z << "/" << y << "/" << x 
		 << ": Meas=" << meas << ", Pred=" << pred << endl;
	  }

	}
      }
    }
  }
}
//
void alct_rx_delay_function() {
  int x, y, z, z_side = 0;
  //
  alct_tx_delay_function();
  //
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {
	  //
	  if (use_measured_values) {
	    ME[z_side][z][y][x].predicted_alct_rx_clock_delay = (ME[z_side][z][y][x].alct_tx_clock_delay           + alct_tx_rx_offset) % 13;
	  } else {
	    ME[z_side][z][y][x].predicted_alct_rx_clock_delay = (ME[z_side][z][y][x].predicted_alct_tx_clock_delay + alct_tx_rx_offset) % 13;
	  }
	  //
	}
      }
    }
  }
}
//
// Align the ALCT with the CLCT
//
//---------------------------------------------------------------------------------------------------------

void mpc_delays() {
  /*
    Predicts relative MPC tx delays
    Aligns all chambers by shifting afeb_fine_delays and calculating mpc_tx_delay
    Smallest mpc_tx_delay is set to 1
    Adjustments to mpc_tx_delay due to match_trig_alct_delay are done in clct_alct_match_delay() function     
  */
  int x, y, z, z_side;
  unsigned int w = 0;
  int correction;
  int correction2, correction3;
  int largest;
  int smallest;
  double longest_delay = 0;
  double change = 0;
  //
  afeb_fine_delay_calculations();
  //
  int max_endcap;
  int max_station;
  int max_ring;
  int max_chamber;
  //
  // ---------- Finds longest delay as a reference point -----------------
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {
	  //
	  ME[z_side][z][y][x].predicted_mpc_tx_delay = 0;
	  //
	  //	  double current_length = 
	  //	    afeb_fine_delay_asic_nsec_per_setting * ME[z_side][z][y][x].init_max_afeb_fine_delay 
	  //	    + ME[z_side][z][y][x].alct_skewclear_delay 
	  //	    + time_of_flight(z_side,z,y,x);
	  //
	  // Since all the afeb_fine delays have been normalized to the same average value, 
	  // we take the length difference to be corrected for to be purely from the time of flight 
	  // and the ALCT-TMB skewclear cable length...
	  //
	  double current_length = ME[z_side][z][y][x].alct_skewclear_delay + time_of_flight(z_side,z,y,x);
	  //
	  if (longest_delay < current_length) {
	    max_endcap  = z_side;
	    max_station = z;
	    max_ring    = y;
	    max_chamber = x;
	    longest_delay = current_length;
	  }
	  //
	}
      }
    }
  }
  //
  //  std::cout << "Maximum length = " << longest_delay << "nsec at E = " << max_endcap 
  //	    << " S = " << max_station << " R = " << max_ring << " C = " << max_chamber << std::endl;
  //
  longest_delay += 25; // this sets the smallest mpc_tx_delay to be 1
  //
  //std::cout << "CAREFUL:  Re-aligning all chambers to " << longest_delay << "nsec" << std::endl;
  //
  // ------- shift afeb_fine_delays and calculate mpc_tx_delay such that all chambers are aligned ------------
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {
	  //
	  //	  double current_length = 
	  //	    afeb_fine_delay_asic_nsec_per_setting * ME[z_side][z][y][x].init_max_afeb_fine_delay 
	  //	    + ME[z_side][z][y][x].alct_skewclear_delay 
	  //	    + time_of_flight(z_side,z,y,x);
	  //
	  double current_length = ME[z_side][z][y][x].alct_skewclear_delay + time_of_flight(z_side,z,y,x);
	  //
	  change = longest_delay - current_length;
	  correction = 0;
	  largest = 15;
	  smallest = 0;
	  //
	  //std::cout << "Need to add " << change << " to length = " << current_length << "nsec in E = " << z_side
	  //          << " S = " << z << " R = " << y << " C = " << x << std::endl;
	  //
	  for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
	    //
	    //std::cout << "Before AFEB = " << w << " = " << ME[z_side][z][y][x].init_afeb_fine_delay[w];
	    //
	    int afeb_delay_correction = (int) (0.5 + (change / afeb_fine_delay_asic_nsec_per_setting)); 
	    //
	    //std::cout << "... add " << afeb_delay_correction;
	    //
	    // add the full correction to the all of the AFEB fine delays to line up the signals at TMB
	    ME[z_side][z][y][x].init_afeb_fine_delay[w] += afeb_delay_correction;
	    //
	    //std::cout << " to get " << ME[z_side][z][y][x].init_afeb_fine_delay[w] << std::endl;
	    //
	    if ((ME[z_side][z][y][x].init_afeb_fine_delay[w] < 0) && (ME[z_side][z][y][x].init_afeb_fine_delay[w] < smallest)) {
	      smallest = ME[z_side][z][y][x].init_afeb_fine_delay[w];
	      correction = -(int)((smallest - 11.4) / 11.4);
	    } 
	    //
	    if ((ME[z_side][z][y][x].init_afeb_fine_delay[w] > 15) && (ME[z_side][z][y][x].init_afeb_fine_delay[w] > largest)) {
	      largest = ME[z_side][z][y][x].init_afeb_fine_delay[w];
	      correction = -(int) (((largest + 11.4) - 16) / 11.4);
	    }
	    //
	  }
	  //
	  int int_afeb_delay_recorrection = 0;
	  double afeb_delay_recorrection = correction * 11.4;
	  //
	  if (afeb_delay_recorrection > 0) {
	    int_afeb_delay_recorrection = (int) (0.5 + afeb_delay_recorrection);
	  } else {
	    int_afeb_delay_recorrection = (int) (-0.5 + afeb_delay_recorrection);
	  }
	  //
	  //std::cout << "Largest AFEB value = " << largest << " needs to be corrected by " << afeb_delay_recorrection 
	  //    << "(" << int_afeb_delay_recorrection << ")" << std::endl;
	  //std::cout << "... with a corresponding correction to mpc_tx_delay of - " << correction << " gives... " << std::endl;
	  //
	  for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
	    //
	    ME[z_side][z][y][x].init_afeb_fine_delay[w] += int_afeb_delay_recorrection;
	    //std::cout << "AFEB = " << w << " = " << ME[z_side][z][y][x].init_afeb_fine_delay[w] << std::endl;
	    //
	  }	
	  //
	  ME[z_side][z][y][x].predicted_mpc_tx_delay -= correction;
	  //std::cout << "... and mpc_tx_delay = " << ME[z_side][z][y][x].predicted_mpc_tx_delay << std::endl;
	  //
	  //
	}
      }
    }
  }
  //
  afeb_fine_delay_calculations();
  //
  // ------------- Finds which side to adjust out of range afeb_fine_delay values
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {
	  correction2 = 0;
	  correction3 = 0;
	  for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
	    if (ME[z_side][z][y][x].init_afeb_fine_delay[w] < 0) {
	      correction2++;
	    } 
	    if (ME[z_side][z][y][x].init_afeb_fine_delay[w] > 15) {
	      correction3++;
	    }
	  }
	  // ----------- if chamber has negative afeb delays -----------------
	  if (correction2 > 0) {
	    for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
	      if ((ME[z_side][z][y][x].init_afeb_fine_delay[w] + 11) > 15)
		correction3++;
	    }
	    if (correction2 > correction3) {
	      for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
		ME[z_side][z][y][x].init_afeb_fine_delay[w] += 11;
		if (ME[z_side][z][y][x].init_afeb_fine_delay[w] > 15)
		  ME[z_side][z][y][x].init_afeb_fine_delay[w] = 15;
	      }
	      ME[z_side][z][y][x].predicted_mpc_tx_delay--;
	    }
	    else {
	      for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
		if (ME[z_side][z][y][x].init_afeb_fine_delay[w] < 0)
		  ME[z_side][z][y][x].init_afeb_fine_delay[w] = 0;
	      }
	    }
	    correction3 = 0;
	  }
	  // ------------------ if chamber has afeb delays > 15 -------------
	  if (correction3 > 0) {
	    for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
	      if ((ME[z_side][z][y][x].init_afeb_fine_delay[w] - 11) < 0)
		correction2++;
	    }
	    if (correction2 < correction3) {
	      for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
		ME[z_side][z][y][x].init_afeb_fine_delay[w] -= 11;
		if (ME[z_side][z][y][x].init_afeb_fine_delay[w] < 0)
		  ME[z_side][z][y][x].init_afeb_fine_delay[w] = 0;
	      }
	      ME[z_side][z][y][x].predicted_mpc_tx_delay++;
	    }
	    else {
	      for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
		if (ME[z_side][z][y][x].init_afeb_fine_delay[w] > 15)
		  ME[z_side][z][y][x].init_afeb_fine_delay[w] = 15;
	      }
	    }
	    correction2 = 0;
	  }   
	}
      }
    }
  }
}
//
double time_of_flight(int z_side, int z, int y, int x) {
  /*
    returns time of flight for a specific chamber
    inputs: z_side is 0 or 1, z is station, y is ring, x is chamber number
    function is called in mpc_delays()

    ******************************** RETURNS IN NS *****************************
  */
  
  //        DO  STUFF


  return 0;
}
//---------------------------------------------------------------------------------------------------------
//
void afeb_fine_delay_calculations() {
  /*
    calculates the max afeb and average afeb for the current afeb_fine_delays
    when afebs are adjusted, this function should be called to recalculate which is the max afeb_fine_delay
  */
  int x, y, z, z_side;
  unsigned int w = 0;
  int afeb_fine_delay_sum = 0;
  int counter = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number  
	  //
	  if (ME[z_side][z][y][x].init_afeb_fine_delay.size() == 0) 
	    ME[z_side][z][y][x].init_afeb_fine_delay.push_back(0);
	  ME[z_side][z][y][x].init_max_afeb_fine_delay = 0;
	  ME[z_side][z][y][x].max_afeb_fine_delay = 0;
	  afeb_fine_delay_sum = 0;
	  counter = 0;
	  for(w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++){
	    //
	    if(ME[z_side][z][y][x].init_afeb_fine_delay[w] > ME[z_side][z][y][x].init_max_afeb_fine_delay)
	      ME[z_side][z][y][x].init_max_afeb_fine_delay = ME[z_side][z][y][x].init_afeb_fine_delay[w];
	    //
	    afeb_fine_delay_sum += ME[z_side][z][y][x].init_afeb_fine_delay[w];
	    //
	  }
	  //
	  for(w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++){
	    //
	    if(ME[z_side][z][y][x].afeb_fine_delay[w] > ME[z_side][z][y][x].max_afeb_fine_delay)
	      ME[z_side][z][y][x].max_afeb_fine_delay = ME[z_side][z][y][x].afeb_fine_delay[w];
	    //
	  }
	  ME[z_side][z][y][x].init_afeb_fine_delay_average = (int) (0.5 + (afeb_fine_delay_sum)/ME[z_side][z][y][x].init_afeb_fine_delay.size());
	  //
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void clct_alct_match_window() {
  /*    
	afeb_fine_delay_average is computed here.
	Calculates match_trig_alct_delay and mpc_tx_delay
        match_trig_alct_delay is a function of clock_phase_difference, cable_length_difference_in_bx, and afeb_fine_delay_correction.
	not_rounded_alst_match_window_delay is used to compare predicted values to the weigted average of match_trig_alct_delay timing test
	mpc_tx_delay + match_trig_alct_delay = the sum of these two variables from .txt file
  */
  int x, y, z, z_side;
  //
  afeb_fine_delay_calculations();
  //
  for (z_side = 0; z_side <= 1; z_side++) {                                                   //endcap
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //station
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //ring 
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber 
	  //  
	  ME[z_side][z][y][x].afeb_fine_delay_correction = 
	    afeb_fine_delay_asic_nsec_per_setting * 
	    (ME[z_side][z][y][x].afeb_cable_adjustment + (double) ME[z_side][z][y][x].init_max_afeb_fine_delay) / 25;   
	  //
	  ME[z_side][z][y][x].clock_phase_difference = 
	    ((double)
	     ( (int)(0.5 + 2 * ME[z_side][z][y][x].predicted_cfeb_delay + ME[z_side][z][y][x].clct_skewclear_delay) % 25 
	       - (int)(0.5 + 2 * ME[z_side][z][y][x].predicted_alct_tx_clock_delay + ME[z_side][z][y][x].alct_skewclear_delay) % 25))/25;
	  //
	  ME[z_side][z][y][x].cable_difference_in_bx = 
	    (int)( (ME[z_side][z][y][x].alct_skewclear_delay-ME[z_side][z][y][x].clct_skewclear_delay) / 25);
	  //   
	  ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay = 
	    clct_alct_latency_difference 
	    + comp_timing 
	    + clct_drift_delay 
	    - alct_drift_delay 
	    + (match_trig_window_size - 1)/2 
	    - ME[z_side][z][y][x].clock_phase_difference 
	    - ME[z_side][z][y][x].cable_difference_in_bx 
	    - ME[z_side][z][y][x].afeb_fine_delay_correction;
	  //
	  //	  ME[z_side][z][y][x].predicted_match_trig_alct_delay = (int)(0.5 + ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay);
	  ME[z_side][z][y][x].predicted_match_trig_alct_delay = (int)ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay;
	  //
	  /*
	    ------ Use if you do not want to align chambers at MPC --- Need to comment out lines below --------
	    for (z_side = 0; z_side <= 1; z_side++) {
	    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
	    for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	    for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {
	    //------------------------------------------------
	    //ME[z_side][z][y][x].predicted_mpc_tx_delay = 5 - ME[z_side][z][y][x].predicted_match_trig_alct_delay + alct_drift_delay;
	    //------------------------------------------------
	    
	    ME[z_side][z][y][x].predicted_mpc_rx_delay = ME[z_side][z][y][x].predicted_mpc_tx_delay + 5;
	    }
	    }
	    }
	    }
	  */
	  // ---------- Adjust MPC tx (after alignment) and rx due to match_trig_alct_delay and alct_drift_delay --------
	  if (use_measured_values) {
	    //
	    ME[z_side][z][y][x].predicted_mpc_rx_delay = ME[z_side][z][y][x].mpc_tx_delay           + transmission_plus_mpc_processing_time;
	    //
	  } else {
	    //
	    // Since the predictions want to be aligning the LCT at the SP, we don't want to touch the mpc_tx delay
	    // after it has been predicted.
	    //
	    // N.B. This is a potential problem in versions before 14 December 2007
	    //
	    //ME[z_side][z][y][x].predicted_mpc_tx_delay += 4 + alct_drift_delay - ME[z_side][z][y][x].predicted_match_trig_alct_delay;
	    //
	    ME[z_side][z][y][x].predicted_mpc_rx_delay = ME[z_side][z][y][x].predicted_mpc_tx_delay + transmission_plus_mpc_processing_time;
	    //
	  }
	}
      }
    }
  }
}
//
// Receive the L1A at the components
//---------------------------------------------------------------------------------------------------------
//
void tmb_lct_cable_delay() {
  /*      
	  Calculates tmb_lct_cable_delay based on clct_skewclear_delay and mpc_tx_delay
	  Depending on what range the clct_skewclear_delay falls into, we give the
	  tmb_lct_cable_delay a value of 0 to 3.  The ranges in the 'if' statements
	  have been adjusted to maximize the correlation between measured and predicted tmb_lct_cable_delay
	  +11 FROM TWIKI PAGE
  */
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {  
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  //
	  ME[z_side][z][y][x].predicted_tmb_lct_cable_delay = 
	                                                     //JH add pos. number inside int then subtract:
	    (int)(1000+tmb_lct_cable_delay_offset-ME[z_side][z][y][x].clct_skewclear_delay /25.)-1000 
	    + l1a_latency_from_mpc_to_ttc_output
	    + (TTCrxCoarseDelayPred[z_side][z][ChamberToPCMap[z][y][x]] + (int) fiber_length[z_side][z][ChamberToPCMap[z][y][x]])
	  - comp_timing;
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void tmb_l1a_delay() {
  //
  // Calculates the amount of time to wait between the LCT (sent to the MPC) and the L1A
  //
  //  double clct_l1a_delay_offset = 120.2;    //Positive CLCT to L1A return bx plus phase adjustment
  int clct_l1a_delay_offset = 120;    //Option 2: if just depends on ALCT time only
  double alct_l1a_delay_offset = 0.11;     //Phase adjustment only
  int x, y, z, z_side = 0;
  //JH try to find optimum phases by scanning, count discrepancies:
  //
  //for(double cval = 120.0; cval<121.1; cval=cval+0.1) {
  //  clct_l1a_delay_offset=cval;
  //for(double aval = 0.0; aval<1.1; aval=aval+0.1) {
  //  alct_l1a_delay_offset=aval;
    int discreps=0;
    int nchambers=0;
  for (z_side = 0; z_side <= 1; z_side++) {                                                   //endcap
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //station
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //ring
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  //
	  //	  std::cout << "endcap = " << z_side 
	  //		    << " station = " << z 
	  //		    << " ring = " << y
	  //		    << " chamber = " << x
	  //		    << " PC = " << ChamberToPCMap[z][y][x]
	  //		    << " TTCrxCoarseDelay = " << TTCrxCoarseDelayPred[z_side][z][ChamberToPCMap[z][y][x]] 
	  //		    << std::endl; 
	  //
	  if (use_measured_values) {
	    //
	    ME[z_side][z][y][x].predicted_tmb_l1a_delay = 
	      ME[z_side][z][y][x].mpc_tx_delay           
	      + l1a_latency_from_mpc_to_ttc_output
	      + (TTCrxCoarseDelayPred[z_side][z][ChamberToPCMap[z][y][x]] 
		 + (int) fiber_length[z_side][z][ChamberToPCMap[z][y][x]]);
	    //
	  } else {
	    //
	    // ME[z_side][z][y][x].predicted_tmb_l1a_delay = 
	    //  ME[z_side][z][y][x].predicted_mpc_tx_delay 
	    //  + l1a_latency_from_mpc_to_ttc_output
	    //  + (TTCrxCoarseDelayPred[z_side][z][ChamberToPCMap[z][y][x]] 
	    //	 + (int) fiber_length[z_side][z][ChamberToPCMap[z][y][x]]);
	    //
	    //JH 20-Oct-08 Think a little harder: it seems that the total delays to add are:
	    //	1) (CLCT-ALCT) mean time in the TMB  depends on Skewclear lengths
	    //	2) ALCT variations in time from TMB up to MPC (mpc_tx_delay, integer)
	    //	The rest is isochronous to MPC, and SP, and L1, and TTC, and back to TMB.
	    //	The first part is the hardest:
	    //	CLCT has CFEB Skewclear delay, use (int) (const_c + Skewclear_c)
	    //	ALCT has ALCT Skewclear delay, use (int) (const_a + Skewclear_a)
	    //	So there are two phase wrapping parameters to be understood. 
	    //The ALCT phase wrapping may be 0.11 bx = 2.75ns/
	    //Delay is the difference between clct-finding (varies) and L1A being returned (same for every chamber)
	    //
	    //Version 2: if the LCT goes out relative to ALCT, then just need mpc_tx_delay correction
	      ME[z_side][z][y][x].predicted_tmb_l1a_delay = 
		//        (int) (clct_l1a_delay_offset + ME[z_side][z][y][x].clct_skewclear_delay/25)
		//- (int) (alct_l1a_delay_offset + ME[z_side][z][y][x].alct_skewclear_delay/25)
		clct_l1a_delay_offset
		+ ME[z_side][z][y][x].mpc_tx_delay;
	      //JH debug
	      int pred = ME[z_side][z][y][x].predicted_tmb_l1a_delay;
	      int meas = ME[z_side][z][y][x].tmb_l1a_delay;
	      nchambers++;
	      if (pred != meas) {
		discreps++;
		//      cout << "tmb_l1a_delay: diff for side " << z_side <<", station " << z << ", ring " << y
		// << ", phi=" << x << " pred=" << pred << " and meas=" << meas << endl;
	      }
	    }
	  }
	}
      }
    }
  cout << "tmb_l1a_delay: using {clct/alct}_l1a_delay_offsets=" << clct_l1a_delay_offset 
       << ", " << alct_l1a_delay_offset << " find discreps=" << discreps << "/" << nchambers << endl;
  //  }}
}
//---------------------------------------------------------------------------------------------------------
//
void alct_l1a_delay() {
   /*
     Calculates the alct_l1a_delay based on and alct_skewclear_delay and alct_drift_delay.
     alct_drift_delay is subtracted to compensate for the mpc_tx_delay staying the same.
     Assumes that all other delays are timed in correctly such as TTCrxCoarseDelay
     N.B. mpc_tx_delay is already included in tmb_l1a_delay
   */
  //
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { 
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { 
	  //
	  if (use_measured_values) {
	    //
	    ME[z_side][z][y][x].predicted_alct_l1a_delay = 
	      ME[z_side][z][y][x].tmb_l1a_delay 
	      + tmb_to_alct_propagation 
	      - alct_drift_delay                                    
	      + ME[z_side][z][y][x].match_trig_alct_delay
	      + (int)(0.5 + (ME[z_side][z][y][x].alct_skewclear_delay / 25.) );
	      // + (int)(0.5 + 2.*ME[z_side][z][y][x].alct_skewclear_delay / 25. );
	    //
	  } else {
	    ME[z_side][z][y][x].predicted_alct_l1a_delay = 
	      //JH 19-Oct-2008 previous "general" calculation
	      //	      ME[z_side][z][y][x].predicted_tmb_l1a_delay 
	      //+ tmb_to_alct_propagation 
	      //- alct_drift_delay                                    
	      //+ ME[z_side][z][y][x].predicted_match_trig_alct_delay
	      //+ (int)(2.*ME[z_side][z][y][x].alct_skewclear_delay / 25. );
	      //JH 19-Oct-2008 new "simple" calculation with isochronous peripheral crate clocking
	      //Algo. #1:	      (int)(alct_l1a_delay_offset + ME[z_side][z][y][x].alct_skewclear_delay/25);
	      //Algo. #2: JH 19-Oct-2008 spectacular improvement
	      (int)(alct_l1a_delay_offset + 2*ME[z_side][z][y][x].alct_skewclear_delay/25
		    +ME[z_side][z][y][x].mpc_tx_delay);
		    }
	  //
	}
      }
    }
  }
}
//
//
// Receive the Data AVailable bits at the DMB
//
//---------------------------------------------------------------------------------------------------------
//
void cfeb_dav_cable_delay() {
  /*      Calculates cfeb_dav_cable_delay based on clct_skewclear_delay and cfeb_cable_delay
	  Depending on what range the clct_skewclear_delay falls into, we give the
	  cfeb_dav_cable_delay a value of 0 to 2.  We then subtract the cfeb_cable_delay
	  value.  The ranges in the 'if' statements have been adjusted to maximize
	  the corrilation between measured and predicted cfeb_dav_cable_delay.
	  +5 FROM TWIKI PAGE
  */
  //
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  //
	  // N.B.  Since there is no measured cfeb_cable_delay, the predicted_cfeb_cable_delay is all we have:
	  //
	  ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay = 
	    (int)(cfeb_dav_processing_time_offset-(2 * ME[z_side][z][y][x].clct_skewclear_delay)/25) //JH new calculation method
	    - ME[z_side][z][y][x].predicted_cfeb_cable_delay;  //JH is this the right thing to subtract?
	  //    	    cfeb_dav_processing_time_offset
	  //- (int)(0.6+(2 * ME[z_side][z][y][x].clct_skewclear_delay)/25) //JH can add "vernier" change here
	  //- ME[z_side][z][y][x].predicted_cfeb_cable_delay;
	    //JH: Previous calculation:
	    //6
	    //- (int)((2 * ME[z_side][z][y][x].clct_skewclear_delay)/25)
	    //- ME[z_side][z][y][x].predicted_cfeb_cable_delay;
	  //
	}
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------
//
void alct_dav_cable_delay() {
  /*      
	  Calculates alct_dav_cable_delay based on alct_skewclear_delay.
	  Depending on what range the alct_skewclear_delay falls into, we give the
	  alct_dav_cable_delay a value of 0 to 3. The ranges in the 'if' statements 
	  have been adjusted to maximize the correlation between measured and predicted
	  alct_dav_cable_delay.
  */
  //
  int x, y, z, z_side = 0;
  for (z_side = 0; z_side <= 1; z_side++) {  
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){        //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {     //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {   //chamber number
	  //
	  ME[z_side][z][y][x].predicted_alct_dav_cable_delay = 
	    //    alct_dav_processing_time_offset
	    //    - (int)(0.5 + 2.*ME[z_side][z][y][x].alct_skewclear_delay / 25. );
	    //JH tweak a little to optimize the agreement with measured alct_dav delays
	    // - (int)(0.2 + 2.*ME[z_side][z][y][x].alct_skewclear_delay / 25. );
	    //JH put the offset inside the (int)
	    (int)(alct_dav_processing_time_offset - 2.*ME[z_side][z][y][x].alct_skewclear_delay / 25. );
	  //
	}
      }
    }
  }
}
//
//
// Several routines for reading in ascii files follow
//---------------------------------------------------------------------------------------------------------
//
void read_in_cable_lengths(const char file_name[]) {
  /*
    reads in and stores the cable length and cable revisions for CFEB and ALCT for all chambers
  */
  int number;                                                      
  double cfeb_length;
  double alct_length;
  double delay;
  char cfeb_rev;
  char alct_rev;
  char chamber[9];
  int cell[3] = {};
  int z_side = 0;
  ifstream cable_length(file_name, ios::in);
  ifstream match_file("weighted_match_trig_alct_delay.txt", ios::in);
  // Read in data from cable_length file
  while(cable_length >> number >> chamber >> cfeb_length >> cfeb_rev >> alct_length >> alct_rev) {

    if (chamber[2] == '+')   z_side = 1;
    if (chamber[2] == '-')   z_side = 0;
    cell[0] = (int) chamber[3] - 48;
    cell[1] = (int) chamber[5] - 48;
    cell[2] = 10 * ((int) chamber[7] - 48) + ((int) chamber[8] - 48);

    ME[z_side][cell[0]][cell[1]][cell[2]].cfeb_tmb_skew_length = cfeb_length;
    ME[z_side][cell[0]][cell[1]][cell[2]].cfeb_tmb_skew_revision = cfeb_rev;
    ME[z_side][cell[0]][cell[1]][cell[2]].alct_skewclear_length = alct_length;
    ME[z_side][cell[0]][cell[1]][cell[2]].alct_skewclear_revision = alct_rev;
  }
  if (match_file) {
    //chamber numbers must be in the form ME-X/X/XX
    while (match_file >> chamber >> delay) {
      if (chamber[2] == '-')
	z_side = 0;
      else if (chamber[2] == '+')
	z_side = 1;
      cell[0] = (int) chamber[3] - 48;
      cell[1] = (int) chamber[5] - 48;
      cell[2] = 10 * ((int) chamber[7] - 48) + ((int) chamber[8] - 48);
      ME[z_side][cell[0]][cell[1]][cell[2]].not_rounded_match_trig_alct_delay = delay;//current as of July 5th, 2007
    }
  }
  
  else
    cout << "No not_rounded_match_trig_alct_delays read in" << endl << endl;
  cable_length.close();
  match_file.close();
}
//
// Time offsets calculated by Dayong
//---------------------------------------------------------------------------------------------------------
//
void read_in_sp_offsets(const char file_name[]) {
  /*
    reads in and stores the bx time offsets for all chambers
  */
  int chamberID;
  char chamber[9];
  double sector_corr;
  double global_corr;
  //
  int z_side = 0;
  char cz_side;
  int cell[3] = {};
  //
  cout << "read_in_sp_offsets: read from SP bx offset file name " << file_name << endl;
  ifstream sp_offsets(file_name, ios::in);
  // Read in data line:
  while(sp_offsets >> chamberID >> chamber >> sector_corr >> global_corr ) {
    cz_side = chamber[2];
    if (cz_side == '+')   z_side = 1;
    if (cz_side == '-')   z_side = 0;
    //
    cell[0] = (int) chamber[3] - 48;
    cell[1] = (int) chamber[5] - 48;
    cell[2] = 10 * ((int) chamber[7] - 48) + ((int) chamber[8] - 48);

    ME[z_side][cell[0]][cell[1]][cell[2]].sp_bx_offset_sector = sector_corr;
    ME[z_side][cell[0]][cell[1]][cell[2]].sp_bx_offset_global = global_corr;
    cout << "SP bx offsets for side " << cz_side <<", station " << cell[0] << ", ring " << cell[1]
	 << ", phi=" << cell[2] << " are " << sector_corr << " and " << global_corr << endl;
  }
  
    cout << "SP bx offset file was read in" << endl << endl;
  sp_offsets.close();
}
//
// TTCrxCoarseDelay adjustments read in (may be precalculated by this program based on fiber lengths, in fact)
//---------------------------------------------------------------------------------------------------------
void read_in_ttc_delays(const char file_name[]) {
  /*
    reads in and stores the bx time offsets for all crates
  */
  int ttc_corr;
  int ttc_corr_fine;
  char VME[8];
  //
  int z_side = 0;
  char cz_side;
  int station;
  int crate;
  //
  cout << "read_in_ttc_delays: read from TTC delay file name " << file_name << endl;
  ifstream ttc_offsets(file_name, ios::in);
  if (ttc_offsets.fail())
    cout << file_name << " not found.  Measured timing parameters not read in" << endl << endl;
  //
  // Read in each data line:
  while(ttc_offsets >> VME >> ttc_corr >> ttc_corr_fine ) {
    cz_side = VME[3];
    if (cz_side == 'p')   z_side = 1;
    if (cz_side == 'm')   z_side = 0;
    //
    //Figure out which crate:
    station = (int) VME[4] - 48;        // hacky character to integer conversions
    crate = 10 * ((int) VME[6] - 48) + ((int) VME[7] - 48);
    //
    //Store the TTC delay information:
    ttc_rx_corr[z_side][station][crate] = ttc_corr;
    ttc_rx_corr_fine[z_side][station][crate] = ttc_corr_fine;
    //
    //Debug:
    cout << "read_in_ttc_delays: VME" << cz_side << station << "/" << crate 
	 << ": bx=" << ttc_corr << " fine=" << ttc_corr_fine << endl;
  }
  //
  //Cleanup:
  cout << "TTC bx offset file has been read in." << endl << endl;
  ttc_offsets.close();
}
//
// bx number offsets adjustments read in
//--------------------------------------
void read_in_bx_values(const char file_name[]) {
  /*
    reads in and stores the bx time offsets for all crates
  */
  char chamber[9];
  int alct_bx;
  int clct_bx;
  //
  int z_side = 0;
  char cz_side;
  int cell[3] = {};
  //
  cout << "read_in_bx_values: read from bx number file name " << file_name << endl;
  ifstream bx_values(file_name, ios::in);
  // Read in data line:
  while(bx_values >> chamber >> alct_bx >> clct_bx ) {
    cz_side = chamber[2];
    if (cz_side == '+')   z_side = 1;
    if (cz_side == '-')   z_side = 0;
    //
    cell[0] = (int) chamber[3] - 48;
    cell[1] = (int) chamber[5] - 48;
    cell[2] = 10 * ((int) chamber[7] - 48) + ((int) chamber[8] - 48);

    ME[z_side][cell[0]][cell[1]][cell[2]].alct_bxn_offset = alct_bx;
    ME[z_side][cell[0]][cell[1]][cell[2]].tmb_bxn_offset = clct_bx;
    //cout << "DEBUG: Bunch crossing bxn offsets for side " << cz_side <<", station " << cell[0] << ", ring " << cell[1]
    // << ", phi=" << cell[2] << " are (ALCT) " << alct_bx << " and (CLCT) " << clct_bx << endl;
  }
  
    cout << "read_in_bx_values: bx number offset file was read in" << endl << endl;
  bx_values.close();
}
//---------------------------------------------------------------------------------------------------------
//
void read_in_afeb_thresh(const char file_name[]) {
  int number;
  char chamber[10];
  char temp[3];
  char waste[9];
  char date[9];
  int number_of_delays;
  int cell[3] = {};
  int delay;
  int w = 0;
  int z_side = 0;
  ifstream in_file(file_name, ios::in);
  if (in_file.fail())
    cout << file_name << " not found.  afeb threshold not read in" << endl << endl;
  while (in_file >> number >> chamber >> temp >> waste >> date >> number_of_delays) {
    if (chamber[2] == '-')
      z_side = 0;
    else if (chamber[2] == '+')
      z_side = 1;
    cell[0] = (int) chamber[3] - 48;
    cell[1] = (int) chamber[5] - 48;
    cell[2] = 100 * ((int) chamber[7] - 48) + 10 * ((int) chamber[8] - 48) + ((int) chamber[9] - 48);
    for (w = 1; w <= number_of_delays; w++) {
      in_file >> delay;
      ME[z_side][cell[0]][cell[1]][cell[2]].init_afeb_thresh.push_back(delay);
    }
  }
  in_file.close();
}
//---------------------------------------------------------------------------------------------------------
//
void read_in_afeb_fine_delays(const char file_name[]) {
  /*
    reads in afeb_fine_delays from corrected file from
    http://www.uscms.org/LPC/lpc_muon/teren/Pages/DelaySX5ISR/
    these afeb_fine_delays are not the final fine_delays that will be used 
  */
  ifstream in_file(file_name, ios::in);
  int number;
  char chamber[10];
  char temp[3];
  char waste[9];
  char date[9];
  int number_of_delays;
  int cell[3] = {};
  int delay;
  int w = 0;
  int z_side = 0;
  if (in_file.fail())
    cout << file_name << " not found.  afeb_fine_delays not read in" << endl << endl;
  //
  while (in_file >> number >> chamber >> temp >> waste >> date >> number_of_delays) {
    //
    if (chamber[2] == '-')
      z_side = 0;
    else if (chamber[2] == '+')
      z_side = 1;
    //
    cell[0] = (int) chamber[3] - 48;
    cell[1] = (int) chamber[5] - 48;
    cell[2] = 100 * ((int) chamber[7] - 48) + 10 * ((int) chamber[8] - 48) + ((int) chamber[9] - 48);
    //
    for (w = 1; w <= number_of_delays; w++) {
      in_file >> delay;
      ME[z_side][cell[0]][cell[1]][cell[2]].init_afeb_fine_delay.push_back(delay);
    }
    //
  }
  in_file.close();
}
//---------------------------------------------------------------------------------------------------------
//
void read_in_config(const char file_name[]) {                 
  //
  //Reads in data from xml file and stores it in appropriate chamber ME[][][]
  //
  string line;
  int x = 0, y = 0, z = 0, z_side = 0;
  int z_crate = 0, x_crate = 0;
  int place = 0;
  int temp = 0;
  char quote = '"';
  ifstream config (file_name , ios::in);
  if (config.fail())
    cout << file_name << " not found.  Measured timing parameters not read in" << endl << endl;
  while (getline(config, line)) {
    //
    //By crate: just the TTCrxCoarseDelay parameter is relevant
    //
    if (line.find("<PeripheralCrate") != string::npos) {   // look for crate location
      place = line.find("VME");                           //in the form VMEp1_12 etc
      if (line[place + 3] == 'm') {
	z_side = 0;
      }
      else if (line[place + 3] == 'p') {
	z_side = 1;
      }
      z_crate = (int) line[place + 4] - 48;// station
                                           // no y (ring for peripheral crates)
      if (line[place + 7] != '"')          // handle 1 or 2 digits for phi of crate
	x_crate = ((int) line[place + 6] - 48) * 10 + (int) line[place + 7] - 48;
      else
	x_crate = (int) line[place +6] - 48;
      //     cout << "DEBUG read_in_config z_side, z_crate, x_crate: " << z_side << ", " << z_crate << ", " << x_crate << endl;
    }
      //
      //The only parameters that go with crate:
      //
      if (line.find("TTCrxCoarseDelay") != string::npos){
	TTCrxCoarseDelayMeas[z_side][z_crate][x_crate] = 0;
	place = line.find(quote) + 1;
	while (line[place] != '"') {
	  TTCrxCoarseDelayMeas[z_side][z_crate][x_crate] = 10 * TTCrxCoarseDelayMeas[z_side][z_crate][x_crate] + (int) line[place] - 48;
	  place++;
	}
	//cout << "read_in_config:: DEBUG read in value of TTCrxCoarseDelayMeas " 
	//    << TTCrxCoarseDelayMeas[z_side][z_crate][x_crate] << endl;
      }
      if (line.find("TTCrxFineDelay") != string::npos){
	TTCrxFineDelayMeas[z_side][z_crate][x_crate] = 0;
	place = line.find(quote) + 1;
	while (line[place] != '"') {
	  TTCrxFineDelayMeas[z_side][z_crate][x_crate] = 10 * TTCrxFineDelayMeas[z_side][z_crate][x_crate] + (int) line[place] - 48;
	  place++;
	}
	//cout << "read_in_config:: DEBUG read in value of TTCrxFineDelayMeas " 
	//   << TTCrxFineDelayMeas[z_side][z_crate][x_crate] << endl;
      }
    //
    //Parameters by chamber below - find out which chamber:
    //
      if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
	place = line.find("ME");                   //in the form ME-_/_/_
	if (line[place + 2] == '-')
	  z_side = 0;
	else if (line[place + 2] == '+')
	  z_side = 1;
      z = (int) line[place + 3] - 48;
      y = (int) line[place + 5] - 48;
      if (line[place + 8] != '"')
	x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
      else
	x = (int) line[place +7] - 48;
      //  cout << z << " " << y << " " << x << endl;
      //
      ME[z_side][z][y][x].has_measured_values = true;
      ME[z_side][z][y][x].afeb_fine_delay.clear();
      //
      }
    //
    //Now the chamber-by-chamber parameters:
    //
    if (line.find("comp_timing") != string::npos){
      // cout << endl << endl << "CFEB0" << endl;
      ME[z_side][z][y][x].comp_timing = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].comp_timing = 10 * ME[z_side][z][y][x].comp_timing + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("alct_drift_delay") != string::npos){
      // cout << endl << endl << "CFEB0" << endl;
      ME[z_side][z][y][x].alct_drift_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_drift_delay = 10 * ME[z_side][z][y][x].alct_drift_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("clct_drift_delay") != string::npos){
      // cout << endl << endl << "CFEB0" << endl;
      ME[z_side][z][y][x].clct_drift_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].clct_drift_delay = 10 * ME[z_side][z][y][x].clct_drift_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("match_trig_window_size") != string::npos){
      // cout << endl << endl << "CFEB0" << endl;
      ME[z_side][z][y][x].match_trig_window_size = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].match_trig_window_size = 10 * ME[z_side][z][y][x].match_trig_window_size + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb0delay") != string::npos){
      // cout << endl << endl << "CFEB0" << endl;
      ME[z_side][z][y][x].cfeb[0] = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb[0] = 10 * ME[z_side][z][y][x].cfeb[0] + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb1delay") != string::npos){
      //cout << endl << endl << "CFEB1" << endl;
      ME[z_side][z][y][x].cfeb[1] = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb[1] = 10 * ME[z_side][z][y][x].cfeb[1] + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb2delay") != string::npos){
      //cout << endl << endl << "CFEB2" << endl;
      ME[z_side][z][y][x].cfeb[2] = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb[2] = 10 * ME[z_side][z][y][x].cfeb[2] + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb3delay") != string::npos){
      //cout << endl << endl << "CFEB3" << endl;  
      ME[z_side][z][y][x].cfeb[3] = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb[3] = 10 * ME[z_side][z][y][x].cfeb[3] + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb4delay") != string::npos){
      //cout << endl << endl << "CFEB4" << endl;     
      ME[z_side][z][y][x].cfeb[4] = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb[4] = 10 * ME[z_side][z][y][x].cfeb[4] + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("alct_tx_clock_delay") != string::npos){
      //cout << endl << endl << "ALCT TX" << endl;      
      ME[z_side][z][y][x].alct_tx_clock_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_tx_clock_delay = 10 * ME[z_side][z][y][x].alct_tx_clock_delay + (int) line[place] - 48;
	place++;
      }
    }	
    if (line.find("alct_rx_clock_delay") != string::npos){
      //cout << endl << endl << "ALCT RX" << endl;
      ME[z_side][z][y][x].alct_rx_clock_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_rx_clock_delay = 10 * ME[z_side][z][y][x].alct_rx_clock_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("match_trig_alct_delay") != string::npos){
      //cout << endl << endl << "MATCH WINDOW" << endl;
      ME[z_side][z][y][x].match_trig_alct_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].match_trig_alct_delay = 10 * ME[z_side][z][y][x].match_trig_alct_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("mpc_tx_delay") != string::npos){
      //cout << endl << endl << "MPC TX" << endl;
      ME[z_side][z][y][x].mpc_tx_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].mpc_tx_delay = 10 * ME[z_side][z][y][x].mpc_tx_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("mpc_rx_delay") != string::npos){
      //cout << endl << endl << "MPC RX" << endl;
      ME[z_side][z][y][x].mpc_rx_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].mpc_rx_delay = 10 * ME[z_side][z][y][x].mpc_rx_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("tmb_lct_cable_delay") != string::npos){
      //cout << endl << endl << "TMB LCT" << endl;
      ME[z_side][z][y][x].tmb_lct_cable_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].tmb_lct_cable_delay = 10 * ME[z_side][z][y][x].tmb_lct_cable_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb_dav_cable_delay") != string::npos){
      //cout << endl << endl << "CFEB DAV" << endl;
      ME[z_side][z][y][x].cfeb_dav_cable_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	  ME[z_side][z][y][x].cfeb_dav_cable_delay = 10 * ME[z_side][z][y][x].cfeb_dav_cable_delay + (int) line[place] - 48;
	  place++;
      }
    }
    if (line.find("alct_dav_cable_delay") != string::npos){
      //cout << endl << endl << "ALCT DAV" << endl;
      ME[z_side][z][y][x].alct_dav_cable_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_dav_cable_delay = 10 * ME[z_side][z][y][x].alct_dav_cable_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("cfeb_cable_delay") != string::npos){
      //cout << endl << endl << "CFEB CABLE" << endl;
      ME[z_side][z][y][x].cfeb_cable_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].cfeb_cable_delay = 10 * ME[z_side][z][y][x].cfeb_cable_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("tmb_l1a_delay") != string::npos){
      //cout << endl << endl << "TMB L1A" << endl;
      ME[z_side][z][y][x].tmb_l1a_delay = 0;
      place = line.find(quote) + 1;
      while (line[place] != '"') {
	ME[z_side][z][y][x].tmb_l1a_delay = 10 * ME[z_side][z][y][x].tmb_l1a_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("alct_l1a_delay") != string::npos){
      //cout << endl << endl << "ALCT L1A" << endl;
      ME[z_side][z][y][x].alct_l1a_delay = 0;
      place = line.find(quote) + 1;     
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_l1a_delay = 10 * ME[z_side][z][y][x].alct_l1a_delay + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("afeb_fine_delay=") != string::npos){
      temp = 0;
      place = line.find("afeb_fine_delay=") + 17;
      while (line[place] != '"') {
	temp = 10 * temp + (int) line[place] - 48;
	place++;
      }
      ME[z_side][z][y][x].afeb_fine_delay.push_back(temp);
    }
    //
    //06-Oct-08 JH added:
    //
    if (line.find("tmb_bxn_offset") != string::npos){
      ME[z_side][z][y][x].tmb_bxn_offset = 0;
      place = line.find(quote) + 1;     
      while (line[place] != '"') {
	ME[z_side][z][y][x].tmb_bxn_offset = 10 * ME[z_side][z][y][x].tmb_bxn_offset + (int) line[place] - 48;
	place++;
      }
    }
    if (line.find("alct_bxn_offset") != string::npos){
      ME[z_side][z][y][x].alct_bxn_offset = 0;
      place = line.find(quote) + 1;     
      while (line[place] != '"') {
	ME[z_side][z][y][x].alct_bxn_offset = 10 * ME[z_side][z][y][x].alct_bxn_offset + (int) line[place] - 48;
	place++;
      }
    }
  }
  config.close();
}
//
//---------------------------------------------------------------------------------------------------------
//
// create and modify xml files
//
void create_config_file() {
  /*
    Create a config.xml file to be used with a sector.
    User defines each chamber and then function will create and store the config set up.
  */
  char replace = 'n';
  int station, check_station = 0;
  int crate, check_crate = 0;
  int x, y, z, w;
  unsigned int i;
  int line_number = 0;
  int number_of_chambers_in_sector = 0;
  int c[9] = {};
  int ring[9] = {};
  int z_side = 0;
  char cz_side, ci_side;
  string MAC  = "00";
  string iMAC = "00";
  int istation, icrate, icrate_number, crate_number=0;
  char bf, np;
  //
  string file_name;
  string new_file_name;
  string line;
  //
  ifstream config_template;
  ifstream check_if_exists;
  ofstream config;
  ifstream trigger_sectors("trigger_sectors.txt", ios::in);
  //-------------------------------------------
  ifstream crate_MAC("crate_MAC.txt", ios::in);
  //-------------------------------
  //
  cout << endl << endl << "Please enter the desired name of config file you would like to create" << endl;
  cout << "===>";
  cin >> file_name;
  if (file_name.find(".xml") == string::npos) file_name += ".xml";
  //
  check_if_exists.open(file_name.c_str(), ios::in);
  //
  if (check_if_exists.fail()) {
    //
    config.open(file_name.c_str(), ios::out); 
    cout << endl << "The file name will be named " << file_name << endl;
    //
  } else {
    //
    cout << endl << file_name << " already exists. Would you like to replace it? (y or n)" << endl;
    cout << "===>";
    cin >> replace;
    if ((replace == 'y') || (replace == 'Y'))
      new_file_name = file_name;
    else
      new_file_name = "new_" + file_name;
    config.open(new_file_name.c_str(), ios::out);
    cout << endl << "The file will be named " << new_file_name << endl;      
  }	
  //
  cout << endl << "The top part of the config file will not be created.  You must manually edit this part." << endl << endl;
  cout << "Please enter + or - for correct side" << endl;
  cout << "===> ";
  cin >> cz_side;
  //
  if (cz_side == '-')
    z_side = 0;
  else if (cz_side == '+')
    z_side = 1;
  //
  cout << "Please enter station number (1 - 4)" << endl;
  cout << "===>";
  cin >> station;
  if (station == 1)
    cout << endl << "Please enter crate index number (1 - 12)" << endl;
  else
    cout << endl << "Please enter crate index number (1 - 6)" << endl;
  cout << "===>";
  cin >> crate;
  //------------------------ Find out which ethernet MAC address to use -----------
  // 
  // Read in data from crate_MAC file
  //
  MAC  = "00";
  crate_number = 0;
  while( crate_MAC >> ci_side >> istation >> icrate >> icrate_number >> iMAC) {
    if ( ci_side == cz_side && istation == station && icrate == crate ) {
      crate_number = icrate_number;
      MAC = iMAC;
    } 
  }
  cout << "For this trigger sector the crate number = " << crate_number << " and ethernet MAC = " << MAC << endl;
  //
  //------------------------ Defines trigger sector ------------------  
  cout << "Compiling a list of the chambers that belong to this trigger sector" << endl;
  if (station == 1)
    line_number = crate;
  else if (station == 2)
    line_number = 12 + crate;
  else if (station == 3)
    line_number = 12 + 6 + crate;
  else if (station == 4)
    line_number = 12 + 6 + 6 + crate;
  for (w = 1; w < line_number; w++)
    getline(trigger_sectors, line);
  //
  if (station == 4) {
    trigger_sectors >> check_station >> check_crate >> c[0] >> c[1] >> c[2];
    number_of_chambers_in_sector = 3;
    ring[0] = 1;
    ring[1] = 1;
    ring[2] = 1;
    //
  } else if (station == 1){
    //
    trigger_sectors >> check_station >> check_crate >> c[0] >> c[1] >> c[2] >> c[3] >> c[4] >> c[5] >> c[6] >> c[7] >> c[8];
    number_of_chambers_in_sector = 9;
    ring[0] = 1;
    ring[1] = 1;
    ring[2] = 1;
    ring[3] = 2;
    ring[4] = 2;
    ring[5] = 2;
    ring[6] = 3;
    ring[7] = 3;
    ring[8] = 3;
    //
  } else {
    //
    trigger_sectors >> check_station >> check_crate >> c[0] >> c[1] >> c[2] >> c[3] >> c[4] >> c[5] >> c[6] >> c[7] >> c[8];
    number_of_chambers_in_sector = 9;
    ring[0] = 1;
    ring[1] = 1;
    ring[2] = 1;
    ring[3] = 2;
    ring[4] = 2;
    ring[5] = 2;
    ring[6] = 2;
    ring[7] = 2;
    ring[8] = 2;
  }
  //
  cout << endl << "The configuration file will be made for VME" << z_side << "<" << check_station << "><" << check_crate << ">" << endl;
  //  cout << "   0% Written" << endl;
  //  config << "<EmuSystem>" << endl;
  
  //
  std::ostringstream endcap_side;
  if (z_side == '-')
    endcap_side << "m";
  else if (z_side == '+')
    endcap_side << "p";
  //
  std::ostringstream dummy;
  dummy << "VME" << endcap_side.str() << check_station << "_" << check_crate;
  //
  config << "  <PeripheralCrate crateID=\"" << crate_number 
	 << "\" label=\"" << dummy.str()
	 << "\" VMEaddress=\"02:00:00:00:00:" << MAC << "\" port=\"2\">" << endl << endl;
  //
  bool skip_cfeb4 = false;
  //
  for (w = 0; w < number_of_chambers_in_sector; w++) {
    config_template.open("config_template.xml", ios::in);
    x = c[w];
    y = ring[w];
    z = station;
    //    config << "<!-- ME" << z_side << z << "/" << y << "/" << x << " chamber -->" << endl; 
    config << "    <CSC label=\"" << "ME" << z_side << z << "/" << y << "/" << x << "\">" << endl;
    if (w < 5)
      config << "    <TMB slot=\"" << (1 + w) * 2 << "\"" << endl;
    else
      config << "    <TMB slot=\"" << 2 + (1 + w) * 2 << "\"" << endl;
    //
    //loop over lines in the template file, replacing only the ones of interest:
    //
    while (getline(config_template, line)) {
      if (line.find("comp_timing") != string::npos) {
	//
	config << "           comp_timing=\"" << comp_timing << "\"" << endl;
	//
      } else if (line.find("match_trig_window_size") != string::npos) {
	config << "         match_trig_window_size=\"" << match_trig_window_size << "\"" << endl;
	//
      } else if (line.find("<ALCT") != string::npos) {
	//
	// A bit of a mess, the following for the start of the ALCT section:
	//
	config << "       <ALCT chamber_type=\"ME" << z << y << "\"" << endl;
	//
	if( z==1 && y==1) {   // special lines for ME11 only
	  np = 'n';
	  if ( z_side == '+' ) np='p';  // np (negative/positive) refers to endcap
	  config << "             alct_firmware_negat_posit=\"" << np <<"\"" << endl;
	  bf = 'b';
	  if ( (x % 2) == 0 ) bf = 'f'; // bf (backward/forward) is forward if chamber is even in phi
	  config << "             alct_firmware_backward_forward=\"" << bf << "\"" << endl;
	}
	//
	if( z==1 && y==3) {   // skip CFEB4 for ME13
	  //
	  skip_cfeb4 = true;
	  //
	}
	//
      } else if (line.find("chamber_type=") != string::npos) {  //do nothing because handled by previous lines of code
	//
	//
      } else if (line.find("alct_drift_delay") != string::npos) { // Back to simple stuff...
	//
	config << "             alct_drift_delay=\"" << alct_drift_delay << "\"" << endl;
	//
      } else if (line.find("clct_stagger") != string::npos) {
	//
	if( z==1 && y==1) {   //tell TMB to not stagger the strips for ME11
	  //
	  config << "         clct_stagger=\"0\"" << endl;
	  //
	} else {
	  //
	  config << "         clct_stagger=\"1\"" << endl;
	  //
	}
	//
      } else if (line.find("enableCLCTInputs_reg42") != string::npos) {
	//
	if( z==1 && y==3) {   //mask off the 4th AFEB for ME1/3 chambers
	  //
	  config << "         enableCLCTInputs_reg42=\"15\"" << endl;
	  //
	} else {
	  //
	  config << "         enableCLCTInputs_reg42=\"31\"" << endl;
	  //
	}
	//
      } else if (line.find("clct_drift_delay") != string::npos) {
	//
	config << "         clct_drift_delay=\"" << clct_drift_delay << "\"" << endl;
	//
      } else if (line.find("cfeb0delay") != string::npos){
	//
	config << "         cfeb0delay=\"" << ME[z_side][z][y][x].predicted_cfeb_delay << "\"" << endl;
	//
      } else if (line.find("cfeb1delay") != string::npos){
	//
	config << "         cfeb1delay=\"" << ME[z_side][z][y][x].predicted_cfeb_delay << "\"" << endl;
	//
      } else if (line.find("cfeb2delay") != string::npos){
	//
	config << "         cfeb2delay=\"" << ME[z_side][z][y][x].predicted_cfeb_delay << "\"" << endl;
	//
      } else if (line.find("cfeb3delay") != string::npos){
	//
	config << "         cfeb3delay=\"" << ME[z_side][z][y][x].predicted_cfeb_delay << "\"" << endl;
	//
      } else if (line.find("cfeb4delay") != string::npos){
	//
	config << "         cfeb4delay=\"" << ME[z_side][z][y][x].predicted_cfeb_delay << "\"" << endl;
	//
      } else if (line.find("alct_tx_clock_delay") != string::npos){
	//
	config << "         alct_tx_clock_delay=\"" << ME[z_side][z][y][x].predicted_alct_tx_clock_delay << "\"" << endl;
	//
      }	else if (line.find("alct_rx_clock_delay") != string::npos){
	//
	config << "         alct_rx_clock_delay=\"" << ME[z_side][z][y][x].predicted_alct_rx_clock_delay << "\"" << endl;
	//
      } else if (line.find("match_trig_alct_delay") != string::npos){
	//
	config << "         match_trig_alct_delay=\"" << ME[z_side][z][y][x].predicted_match_trig_alct_delay << "\"" << endl;
	//
      } else if (line.find("mpc_tx_delay") != string::npos){
	//
	config << "         mpc_tx_delay=\"" << ME[z_side][z][y][x].predicted_mpc_tx_delay << "\"" << endl;
	//      
      } else if (line.find("mpc_rx_delay") != string::npos){
	//
	config << "         mpc_rx_delay=\"" << ME[z_side][z][y][x].predicted_mpc_rx_delay << "\"" << endl;
	//
      } else if (line.find("tmb_lct_cable_delay") != string::npos){
	//
	config << "           tmb_lct_cable_delay=\"" << ME[z_side][z][y][x].predicted_tmb_lct_cable_delay << "\"" << endl;
	//
      } else if (line.find("cfeb_dav_cable_delay") != string::npos){
	//
	config << "           cfeb_dav_cable_delay=\"" << ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay << "\"" << endl;
	//
      } else if (line.find("alct_dav_cable_delay") != string::npos){
	//
	config << "           alct_dav_cable_delay=\"" << ME[z_side][z][y][x].predicted_alct_dav_cable_delay << "\"" << endl;
	//
      } else if (line.find("cfeb_cable_delay") != string::npos){
	//
	config << "           cfeb_cable_delay=\"" << ME[z_side][z][y][x].predicted_cfeb_cable_delay << "\"" << endl;
	//
      } else if (line.find("tmb_l1a_delay") != string::npos){
	//
	config << "         tmb_l1a_delay=\"" << ME[z_side][z][y][x].predicted_tmb_l1a_delay << "\"" << endl;
	//
      } else if (line.find("alct_l1a_delay") != string::npos){
	//
	config << "             alct_l1a_delay=\"" << ME[z_side][z][y][x].predicted_alct_l1a_delay << "\"" << endl;
	//
      } else if (line.find("afeb_fine_delay=") != string::npos){
	//
	for (i = 0; i < ME[z_side][z][y][x].init_afeb_fine_delay.size(); i++)
	  config << "          <AnodeChannel afeb_number=\"" << i + 1 << "\"  afeb_fine_delay=\"" 
		 << ME[z_side][z][y][x].init_afeb_fine_delay[i] 
		 << "\"  afeb_threshold=\"" << ME[z_side][z][y][x].init_afeb_thresh[i] << "\"> </AnodeChannel>" << endl;
	//
      } else if (line.find("DAQMB slot=") != string::npos){
	//
	if (w < 5)
	  config << "    <DAQMB slot=\"" << (w * 2) + 3 << "\"" << endl; 
	else
	  config << "    <DAQMB slot=\"" << (w * 2) + 5<< "\"" << endl;
	//
      } else if ( (line.find("<CFEB cfeb_number=\"4\">") != string::npos) && skip_cfeb4 ) {  
	//
	// remove this line from the output if this is an ME1/3 chamber....
	//
	// (27-June-2008) Following appears outdated?
	//      } else if (line.find("crate_id=") != string::npos){  //Following is part of the DAQMB setup
	//
	// config << "           crate_id=\"" << crate_number << "\"" << endl;
	//
	//
      } else if (line.find("<CCB ") != string::npos) {// JH 27-June-2008 this may be an unnecessary antique
	//      } else if (line.find("<!-- Broadcast") != string::npos) {	// A little bit of funny business to skip over the Broadcast part here
	//
	config_template.close();
	break;
	//
      } else {
	//
	config << line << endl;
	//
      }
    }
    //    cout << setw(3) << (int) ((double) (w + 1)/number_of_chambers_in_sector * 100) << "% Written" << endl; 
  } 
  //  config_template.close();
  //
  // And pick up from the Broadcast part here...
  // 27-June-2008 JH probably following code needs to be junked?
  //
  config_template.open("config_template.xml", ios::in);
  //
  while (getline(config_template, line)){
    if (line.find("<CCB ") != string::npos) {
    //    if (line.find("<!-- Broadcast") != string::npos) {
      //
      config << line << endl;
      //
      while (getline(config_template, line)) {
	//
	if ((line.find("TTCrxCoarseDelay=") != string::npos) && (fiber_length)) {
	  //
	  config << "      TTCrxCoarseDelay=\"" << TTCrxCoarseDelayPred[z_side][station][crate] << "\"" << endl;
	  //
	} else if (line.find("BoardID=") != string::npos){ 	// Following is part of the MPC setup that is after the Broadcast part
	  //
	  config << "         BoardID=\"" << crate_number << "\"" << endl;
	  //
	} else {
	  //
	  config << line << endl;
	  //
	}
      } 
    } 
  }
  //
  trigger_sectors.close();
  config.close();
  config_template.close();
  check_if_exists.close();
}
//---------------------------------------------------------------------------------------------------------
//
void adjust_LCT_fine_timing() {
  /*
    Allows user to align LCT timing at SP.
    Reads in an XML file, shifts afeb/mpc_tx_delays
    ...then changes mpc_rx and tmb_l1a_delay values
    ...then outputs to a revised XML file
  */
  string file_name;
  string offset_file_name;
  bool use_offset_file;               
  int correction;
  int smallest, largest;
  double change = 0;
  string line;
  ifstream infile;
  int x, y, z;
  int z_side = 0;
  char cz_side = '-';
  unsigned int w;
  int place;
  //
  cout << endl << "Warning: Changes made here will be stored in the measured timing parameter variables until the program is closed" << endl;
  //
  // here either run interactively (chamber by chamber) or read in file with SP offsets
  //
  cout << "Enter I for Interactive or the <filename> of the file containing SP bx offsets" << endl;
  cout << "===> ";
  cin >> offset_file_name;
    if ((offset_file_name == 'i') || (offset_file_name == 'I'))
      use_offset_file = false;
    else {
      use_offset_file = true;
      read_in_sp_offsets(offset_file_name.c_str());
    }
  //
  cout << "What is the file name you wish to modify?" << endl;
  cout << "===> ";
  cin >> file_name;
  //
  if (file_name.find(".xml") == string::npos) file_name += ".xml";
  infile.open(file_name.c_str(), ios::in);
  if (infile.fail() == 1)
    cout << endl << file_name << " NOT FOUND" << endl;
  //  
  read_in_config(file_name.c_str());
  //
  // Parse the xml file: find which chamber
  //
  while (getline(infile, line)) {
    if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
      place = line.find("ME");                   //in the form ME-_/_/_
      if (line[place + 2] == '-') {
    	cz_side ='-';
	z_side = 0;
      }
      else if (line[place + 2] == '+') {
	cz_side = '+';
	z_side = 1;
      }
      z = (int) line[place + 3] - 48;
      y = (int) line[place + 5] - 48;
      if (line[place + 8] != '"')
	x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
      else
	x = (int) line[place +7] - 48;
      //
      // Apply the appropriate correction:
      //
      if(use_offset_file) { 
	change = ME[z_side][z][y][x].sp_bx_offset_global;
	//
	// Was done temporarily for plus side to flip the sign from what Dayong has in the file
	//change = -ME[z_side][z][y][x].sp_bx_offset_global;
      }
      else {
	cout << endl << "How much would you like to shift by for ME" << cz_side << z << "/" << y << "/" << x << " in bx?" << endl;
	cout << "===> ";
	cin >> change;
      }
      //
      cout << "ME" << cz_side << z << "/" << y << "/" << x << " Changing by " << change << " bx" << endl;
      if(change==-999.) {
      // TEMPORARY? sign flip
      //     if(change==999.) {
	cout << "XXX No correction found so reset change to 0.0 bx" << endl; 
	change = 0.;
      }

      //
      // Find the smallest and the largest settings:
      //
      correction = 0;
      smallest   = 0;
      largest    = 15;
      //
      for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) {
	//
	double setting_change = change*25 /  afeb_fine_delay_asic_nsec_per_setting;
	int fine_change = int(setting_change<0 ? setting_change+0.5 : setting_change-0.5);  //rounding that works for negative numbers
	ME[z_side][z][y][x].afeb_fine_delay[w] += fine_change; 
	//ME[z_side][z][y][x].afeb_fine_delay[w] += floor(0.5 + (change * 25 / afeb_fine_delay_asic_nsec_per_setting)); 
	//
	if ((ME[z_side][z][y][x].afeb_fine_delay[w] < 0) && (ME[z_side][z][y][x].afeb_fine_delay[w] < smallest)) {
	  smallest = ME[z_side][z][y][x].afeb_fine_delay[w];
	  correction = -(int)((smallest - 11.4) / 11.4);                 //If we have a negative smallest
	} 
	//
	if ((ME[z_side][z][y][x].afeb_fine_delay[w] > 15) && (ME[z_side][z][y][x].afeb_fine_delay[w] > largest)) {
	  largest = ME[z_side][z][y][x].afeb_fine_delay[w];
	  correction = -(int) (((largest + 11.4) - 16) / 11.4);          //If we have a too-big largest
	}
      }
      //
      for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) {
	ME[z_side][z][y][x].afeb_fine_delay[w] += (int) (correction * 11.4);  //Apply the wrap-around correction if any
      }	
      ME[z_side][z][y][x].mpc_tx_delay -= correction; //Correct these parameters for wrap-around
      //
      //18Aug2008 No longer needed:
      //      ME[z_side][z][y][x].mpc_rx_delay -= correction;
      ME[z_side][z][y][x].tmb_l1a_delay -= correction;
      //
      //18Aug2008 add these corrections so don't need to remeasure:
      //
      ME[z_side][z][y][x].alct_l1a_delay -= correction;
      ME[z_side][z][y][x].tmb_lct_cable_delay -= correction;

      //
      //Complain if we are totally out of range on mpc_tx_delay (user intervention will be required):
      //
      if (ME[z_side][z][y][x].mpc_tx_delay < 0)
	cout << "ERROR: The adjustment by " << change 
	     << " bx shifted the mpc_tx_delay below 0 for chamber ME" << z_side << z << "/" << y << "/" << x << endl;
      //
      //Complain if we are out of range on AFEB fine delays (some plateauing of optimal values):
      //
      for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) {
	// if (ME[z_side][z][y][x].afeb_fine_delay[w] > 15 || ME[z_side][z][y][x].afeb_fine_delay[w] < 0)
	if (ME[z_side][z][y][x].afeb_fine_delay[w] > 15) {
	  cout << "WARNING: The adjustment by " << change << " bx shifted afeb_fine_delay " << w + 1 
	       << " to a value of " << ME[z_side][z][y][x].afeb_fine_delay[w] << " for chamber ME" 
	       << cz_side << z << "/" << y << "/" << x << endl;
	  ME[z_side][z][y][x].afeb_fine_delay[w] = 15;
	  cout << "  value CUTOFF at 15" << endl;
	}
	if(ME[z_side][z][y][x].afeb_fine_delay[w] < 0) {
	  cout << "WARNING: The adjustment by " << change << " bx shifted afeb_fine_delay " << w + 1 
	       << " to a value of " << ME[z_side][z][y][x].afeb_fine_delay[w] << " for chamber ME" 
	       << cz_side << z << "/" << y << "/" << x << endl;
	  ME[z_side][z][y][x].afeb_fine_delay[w] = 0;
	  cout << "  value CUTOFF at 0" << endl;
	}
      } 
    }
  } 
  //
  output_measured_parameters(file_name);
}
//---------------------------------------------------------------------------------------------------------
// Adjust TTC delays and other related parameters:
//
void adjust_ttc_delays() {
  /*
    Allows user to change TTC delays in the CCB without ruining other timing by reading an offsets file.
    There is another option for calculating TTC delays based on optical fiber lengths that
    should be run to determine and display those delays.

    Procedure:
	Reads in a .txt file containing desired TTC timing shifts (by crate) using read_in_ttc_delays function.
        Reads in an XML file to be modified.
	For each crate:
	...shifts TTCrxCoarseDelay (CCB)
	...shifts TTCrxFineDelay (CCB)
	Also for each chamber
	...shifts afeb_fine_delay values the same as the TTCrxFineDelay change (ALCT)
	...if rollover, need to adjust the following +1 beyond adjustment for TTCrxCoarseDelay
        ...shifts up to 9 mpc_tx_delay values (TMB - note range is 0-15)
	...shifts up to 9 tmb_l1a_delay values (TMB)
	...shifts up to 9 alct_l1a_delay values (TMB--> ALCT)
	...shifts up to 9 tmb_lct_cable_delay values (DAQMB - note range is 0-7)
        Then outputs to a revised XML file
  */
  int user_choice;
  string ttc_file_name;
  string config_file_name;
  bool use_deltas; 
  int ichange = 0;
  string line;
  ifstream infile;
  int x=0, y=0, z=0;
  int z_side = 0;
  int z_crate=0;
  int x_crate=0;
  char cz_side = '-';
  int place;
  //
  //New for proper TTCrxFineDelay adjustment
  int smallest, largest;
  int correction;
  unsigned int w; //AFEB index
  double change = 0;
  //
  cout << endl << "Warning: Changes made here will be stored in the measured timing parameter variables until the program is closed" << endl;
  //
  //User may choose either (1) calculated ttc coarse, fine delays, or (2) changes to ttc measured delays
  //
  cout << endl << "Enter 1 (...dont...) to apply TTC delays directly, or 2 to apply CORRECTIONS" << endl;
  cout << "===> ";
  cin >> user_choice;
  cout << "User choice = " << user_choice;
  use_deltas = false;
  if( user_choice == 1) {
    cout << "You chose calculations directly based on TTC fiber lengths" << endl;
    use_deltas = false;
    cout << "Option not yet completely implemented (JH, 19Aug2008)!!! Return." << endl;
    return;}
  else if (user_choice==2) {
    cout << "You chose to apply delta corrections" << endl;
    use_deltas = true;
    cout << endl << user_choice << " is your option.  Please continue..." << endl << endl;}
  else {
    cout << "INVALID CHOICE, no operation performed, return to menu." << endl;
    return;}
  //
  //Get name of file with TTC offsets or values and read it in:
  //
  cout << "Enter the <filename> of the file containing TTC delays data" << endl
       << "             (e.g. ttc_delays.txt or ttc_offsets.txt)" << endl;
  cout << "===> ";
  cin >> ttc_file_name;
  read_in_ttc_delays(ttc_file_name.c_str());
  //
  //Get name of XML configuration file and start reading it in:
  //
  cout << "What is the XML configuration file name you wish to modify?" << endl;
  cout << "===> ";
  cin >> config_file_name;
  if (config_file_name.find(".xml") == string::npos) config_file_name += ".xml";
  infile.open(config_file_name.c_str(), ios::in);
  if (infile.fail() == 1) {
    cout << endl << config_file_name << " NOT FOUND, no operations performed" << endl;
    return;}
  read_in_config(config_file_name.c_str());
  //
  // Parse the xml file: find which Peripheral Crate and apply correction to TTCrxCoarseDelayMeas
  //
  while (getline(infile, line)) {
    //
    //Look for *crate* location in the form VMEp1_12, etc. and calculated TTCrx corrections
    //
    if (line.find("<PeripheralCrate") != string::npos) {
      place = line.find("VME");
      if (line[place + 3] == 'm') {
    	cz_side ='-';
	z_side = 0;
      }
      else if (line[place + 3] == 'p') {
	cz_side = '+';
	z_side = 1;
      }
      z_crate = (int) line[place + 4] - 48;// station
                                           // no y (i.e. no ring index for peripheral crates)
      if (line[place + 7] != '"')          // handle 1 or 2 digits for phi of crate
	x_crate = ((int) line[place + 6] - 48) * 10 + (int) line[place + 7] - 48;
      else
	x_crate = (int) line[place +6] - 48;
      //
      //TTCrx{Coarse,Fine}Delay changes applied:
      //
      if(ttc_rx_corr[z_side][z_crate][x_crate] != 0 ||
	 ttc_rx_corr_fine[z_side][z_crate][x_crate] != 0) {
	if(use_deltas) {
	  TTCrxCoarseDelayPred[z_side][z_crate][x_crate] 
	    = TTCrxCoarseDelayMeas[z_side][z_crate][x_crate] + ttc_rx_corr[z_side][z_crate][x_crate];
	  TTCrxFineDelayPred[z_side][z_crate][x_crate] 
	    = TTCrxFineDelayMeas[z_side][z_crate][x_crate] + ttc_rx_corr_fine[z_side][z_crate][x_crate];
	}
	else {
	  TTCrxCoarseDelayPred[z_side][z_crate][x_crate] = ttc_rx_corr[z_side][z_crate][x_crate];
	  TTCrxFineDelayPred[z_side][z_crate][x_crate]   = ttc_rx_corr_fine[z_side][z_crate][x_crate];
	}    
	//   (these are important, so let the user know about it)
	cout << endl  << "adjust_ttc_delays: VME" << cz_side << z_crate << "/" << x_crate 
	     << " change TTCrx{Coarse,Fine}Delays to values ={" 
	     << TTCrxCoarseDelayPred[z_side][z_crate][x_crate] << ", "
	     << TTCrxFineDelayPred[z_side][z_crate][x_crate]
	     << " ns}" << endl;
      }
    }
    //
    //Look for *chamber* location in the form ME+1/1/23 etc. and apply other corrections
    //
    if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
      place = line.find("ME");                   //in the form ME-_/_/_
      if (line[place + 2] == '-') {
    	cz_side ='-';
	z_side = 0;
      }
      else if (line[place + 2] == '+') {
	cz_side = '+';
	z_side = 1;
      }
      z = (int) line[place + 3] - 48;
      y = (int) line[place + 5] - 48;
      if (line[place + 8] != '"')
	x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
      else
	x = (int) line[place +7] - 48;
      //
      // Apply the appropriate TTCrxCoarseDelay corrections:
      //
      ichange = ttc_rx_corr[z_side][z_crate][x_crate];
      //
      if (ichange!=0) {
	ME[z_side][z][y][x].mpc_tx_delay        += ichange;
	ME[z_side][z][y][x].tmb_l1a_delay       += ichange;
	ME[z_side][z][y][x].alct_l1a_delay      += ichange;
	ME[z_side][z][y][x].tmb_lct_cable_delay += ichange;

	cout << "ME" << cz_side << z << "/" << y << "/" << x << " Changing "
	     << "  mpc_tx_delay to " << ME[z_side][z][y][x].mpc_tx_delay 
	     << ", tmb_l1a_delay to " << ME[z_side][z][y][x].tmb_l1a_delay
	     << ", alct_l1a_delay to " << ME[z_side][z][y][x].alct_l1a_delay
	     << ", tmb_lct_cable_delay to " << ME[z_side][z][y][x].tmb_lct_cable_delay << endl;
      } 
      //
      //NEW: Apply the appropriate TTCrxFineDelay corrections if non-zero
      //
      change = ttc_rx_corr_fine[z_side][z_crate][x_crate]; //Recall units are ns
      if (change !=0) {
	//
	// Loop over AFEBs, applying fine delay corrections, and find the smallest and the largest settings per chamber:
	//
	correction = 0; //This is an integer that will be -1 bx if we wrap around on high side, +1 bx if wrap low side
	smallest   = 0;
	largest    = 15;
	//
	for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) { //Loop over AFEBs on this chamber, apply shifts
	  //
	  //Calculate the AFEB delay setting based on the timing change:
	  double setting_change = change /  afeb_fine_delay_asic_nsec_per_setting;
	  int fine_change = (int)(setting_change>0 ? setting_change+0.5 : setting_change-0.5);
	  if (w==0) cout << "First AFEB this chamber, change in ns=" << change 
			 << ", setting change float, int " << setting_change << ", " << fine_change << endl;
	  ME[z_side][z][y][x].afeb_fine_delay[w] += fine_change; 
	  //
	  //ME[z_side][z][y][x].afeb_fine_delay[w] += (int) (0.5 + (change / afeb_fine_delay_asic_nsec_per_setting)); 
	  // cout << "DEBUG: AFEB #" << w << ", new fine delay =" << ME[z_side][z][y][x].afeb_fine_delay[w] << endl; 
	  //
	  //Calculate what to do if get negative values (have to add a bx to delays):
	  if ((ME[z_side][z][y][x].afeb_fine_delay[w] < 0) && (ME[z_side][z][y][x].afeb_fine_delay[w] < smallest)) {
	    smallest = ME[z_side][z][y][x].afeb_fine_delay[w];
	    correction = -(int)((smallest - 11.4) / 11.4);                 //If we have a negative smallest
	  }
	//
	//Calculate what to do if get values too large (have to subtract a bx to delays):
	  if ((ME[z_side][z][y][x].afeb_fine_delay[w] > 15) && (ME[z_side][z][y][x].afeb_fine_delay[w] > largest)) {
	    largest = ME[z_side][z][y][x].afeb_fine_delay[w];
	    correction = -(int) (((largest + 11.4) - 16) / 11.4);          //If we have a too-big largest
	  }
	}
	//cout << "DEBUG: for this chamber, AFEB smallest=" << smallest << ", large=" << largest << ", correction=" << correction
	//     << endl;
	//
	//Ended 1st loop over AFEBs applying shifts, Now if non-zero correction we must apply wrap-around shifts
	//
	for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) {
	  //  cout << "DEBUG: must apply wraparound correction on AFEB " << w << " by (minus) " << correction << endl; 
	  ME[z_side][z][y][x].afeb_fine_delay[w] += (int) (correction * 11.4);  //Apply the wrap-around correction if any
	  // cout << "  --> fixed AFEB " << w << " delay, new value=" << ME[z_side][z][y][x].afeb_fine_delay[w] << endl;
	}	
	ME[z_side][z][y][x].tmb_l1a_delay -= correction;
	ME[z_side][z][y][x].mpc_tx_delay -= correction; //Correct these parameters for wrap-around
	ME[z_side][z][y][x].alct_l1a_delay -= correction;
	ME[z_side][z][y][x].tmb_lct_cable_delay -= correction;
      } // End check if non-zero fine delay correction
      //
      //Check for errors in the parameter settings:
      //
      if (ME[z_side][z][y][x].mpc_tx_delay < 0)
	cout << "ERROR: The adjustment by " << change 
	     << " bx shifted the mpc_tx_delay below 0 for chamber ME" << z_side << z << "/" << y << "/" << x << endl;
      //
      if (ME[z_side][z][y][x].tmb_lct_cable_delay < 0)
	cout << "ERROR: The adjustment by " << change 
	     << " bx shifted the tmb_lct_cable_delay below 0 for chamber ME" << z_side << z << "/" << y << "/" << x << endl;
      //
      if (ME[z_side][z][y][x].tmb_lct_cable_delay > 7)
	cout << "ERROR: The adjustment by " << change 
	     << " bx shifted the tmb_lct_cable_delay above 7 for chamber ME" << z_side << z << "/" << y << "/" << x << endl;
      //
      //Complain if we are out of range on AFEB fine delays (some plateauing of optimal values):
      //
      for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++) {
	// if (ME[z_side][z][y][x].afeb_fine_delay[w] > 15 || ME[z_side][z][y][x].afeb_fine_delay[w] < 0)
	if (ME[z_side][z][y][x].afeb_fine_delay[w] > 15) {
	  cout << "WARNING: The adjustment by " << change << " ns shifted afeb_fine_delay " << w + 1 
	       << " to a value of " << ME[z_side][z][y][x].afeb_fine_delay[w] << " for chamber ME" 
	       << cz_side << z << "/" << y << "/" << x << endl;
	  ME[z_side][z][y][x].afeb_fine_delay[w] = 15;
	  cout << "  value CUTOFF at 15" << endl;
	}
	if(ME[z_side][z][y][x].afeb_fine_delay[w] < 0) {
	  cout << "WARNING: The adjustment by " << change << " ns shifted afeb_fine_delay " << w + 1 
	       << " to a value of " << ME[z_side][z][y][x].afeb_fine_delay[w] << " for chamber ME" 
	       << cz_side << z << "/" << y << "/" << x << endl;
	  ME[z_side][z][y][x].afeb_fine_delay[w] = 0;
	  cout << "  value CUTOFF at 0" << endl;
	}
      } //End loop over check AFEB fine delay range

    } //End handling of this chamber encountered during XML file parsing
  } //End loop over XML file lines
  //
  output_measured_parameters(config_file_name);
} //End this routine
//---------------------------------------------------------------------------------------------------------
// Adjust bunch crossing offsets for ALCT and CLCT:
//
void adjust_bx_offsets() {
  /*
    Allows user to change TTC delays in the CCB without ruining other timing by reading an offsets file.
    There is another option for calculating TTC delays based on optical fiber lengths that
    should be run to determine and display those delays.

    Procedure:
        Reads in an XML file, then applies bx offsets for alct and clct data.
        ...then outputs to a revised XML file


  */
  int user_choice;
  string bx_file_name;
  string config_file_name;
  bool use_deltas; 
  string line;
  ifstream infile;
  int x=0, y=0, z=0;
  int z_side = 0;
  char cz_side = '-';
  int place;
  //
  //User may choose to read in file with either (1) bx offsets, or (2) bx offset corrections
  //
  cout << endl << "Enter 1 to apply bx offsets directly, or 2 (DONT) to apply CORRECTIONS" << endl;
  cout << "===> ";
  cin >> user_choice;
  cout << "User choice = " << user_choice;
  use_deltas = false;
  if( user_choice == 1) {
    cout << "You chose to apply bx offset numbers directly" << endl;
    use_deltas = false;
    cout << endl << user_choice << " is your option.  Please continue..." << endl << endl;
    }
  else if (user_choice==2) {
    cout << "You chose to apply delta corrections" << endl;
    use_deltas = true;
    cout << "Option not yet implemented (JH, 03-Oct-2008)!!! Return to menu." << endl;
    return;}
  else {
    cout << "INVALID CHOICE, no operation performed, return to menu." << endl;
    return;}
  //
  //Get name of file with bx number values or offsets and read it in:
  //
  cout << "Enter the <filename> of the file containing bx number offset data" << endl
       << "             (e.g. bxn_values.txt)" << endl;
  cout << "===> ";
  cin >> bx_file_name;
  read_in_bx_values(bx_file_name.c_str());
  //
  //Apply global bx number corrections?
  //
  int user_offsets[2]; //ALCT, then CLCT global offsets to apply
  user_offsets[0]=0;
  user_offsets[1]=0;
  cout << "Enter global bx number correction to apply to ALCT values:" << endl;
  cin >> user_offsets[0]; 
  cout << "Enter global bx number correction to apply to CLCT values:" << endl;
  cin >> user_offsets[1]; 
  cout << "We apply global bx number offsets of " << user_offsets[0] << " (ALCT), and " 
       << user_offsets[1] << " (CLCT)" << endl;
  //
  //Get name of XML configuration file and start reading it in:
  //
  cout << "What is the XML configuration file name you wish to modify?" << endl;
  cout << "===> ";
  cin >> config_file_name;
  if (config_file_name.find(".xml") == string::npos) config_file_name += ".xml";
  infile.open(config_file_name.c_str(), ios::in);
  if (infile.fail() == 1) {
    cout << endl << config_file_name << " NOT FOUND, no operations performed" << endl;
    cout << "ERROR!!!" << endl << "ERROR!!!" << endl << endl;
    return;}
  read_in_config(config_file_name.c_str());
  //
  // Parse the xml file: find which Peripheral Crate and apply correction to TTCrxCoarseDelayMeas
  //
  while (getline(infile, line)) {
    //
    //Look for *chamber* location in the form ME+1/1/23 etc. and apply other corrections
    //
    if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
      place = line.find("ME");                   //in the form ME-_/_/_
      if (line[place + 2] == '-') {
    	cz_side ='-';
	z_side = 0;
      }
      else if (line[place + 2] == '+') {
	cz_side = '+';
	z_side = 1;
      }
      z = (int) line[place + 3] - 48;
      y = (int) line[place + 5] - 48;
      if (line[place + 8] != '"')
	x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
      else
	x = (int) line[place +7] - 48;
      //
      // Apply the appropriate bx_offset_{alct,clct}_{pred,meas} corrections:
      //   user_offsets : user-selected global bx number offsets
      //   -ME[]etc : these are the values Chad determined from "monster" events September 2008, negative sign needed
      //   +1 for ALCT: because ALCT bxn offsets somehow need to start at 1, not 0 (don't understand it...)
      //
      int alct_val = user_offsets[0] - ME[z_side][z][y][x].alct_bxn_offset +1;
      int clct_val = user_offsets[1] - ME[z_side][z][y][x].tmb_bxn_offset;
      if (alct_val < 0) cout << "ERROR!!! ALCT bx offset <0 detected";
      if (clct_val < 0) cout << "ERROR!!! CLCT bx offset <0 detected";
      //
      if (alct_val !=0 || clct_val!=0) {
	ME[z_side][z][y][x].alct_bxn_offset = alct_val;
	ME[z_side][z][y][x].tmb_bxn_offset = clct_val;
	cout << endl  << "adjust_bx_offsets: ME" << cz_side << z << "/" << y << "/" << x 
	     <<     " Changing alct bx numbers to/by " << alct_val 
	     << " bx, changing clct bx numbers to/by " << clct_val << endl;
	//
      } 
      else {
	cout << " adjust_bx_offsets: no correction required for this chamber" << endl;
      }
    } //End handling of this chamber encountered during XML file parsing
  } //End loop over XML file lines
  //
  output_measured_parameters(config_file_name);
} //End this routine
//
//---------------------------------------------------------------------------------------------------------
//
void adjust_measured_parameters() {
  string file_name;
  string line;
  ifstream infile;
  int place;
  int x, y, z;
  int z_side = 0;
  char cz_side = '-';
  int comp_change = 0;
  int alct_change = 0;
  int clct_change = 0;
  int window_change = 0;
  cout << endl << "Warning: Changes made here will be stored in the measured timing parameter variables until the program is closed" << endl << endl;
  cout << "What is the file name you wish to modify?" << endl;
  cout << "===> ";
  cin >> file_name;
  if (file_name.find(".xml") == string::npos)
    file_name += ".xml";
  infile.open(file_name.c_str(), ios::in);
  if (infile.fail() == 1) {
    //
    cout << endl << file_name << " NOT FOUND" << endl;
    //
  } else {
    //
    read_in_config(file_name.c_str());
    cout << "Change comp_timing by ===> ";
    cin >> comp_change;
    cout << "Change clct_drift_delay by ===> ";
    cin >> clct_change;
    cout << "Change alct_drift_delay by ===> ";
    cin >> alct_change;
    cout << "Change match_trig_window_size by ===>";
    cin >> window_change;
    //
    cout << endl << endl << "Changing:" << endl;
    cout << "    match_trig_alct_delay  by " << comp_change - alct_change + clct_change + window_change/2 << endl;
    cout << "    mpc_tx_delay           by " << -comp_change - clct_change - window_change/2 << endl;
    cout << "    mpc_rx_delay           by " << -comp_change - clct_change - window_change/2 << endl;
    cout << "    tmb_lct_cable_delay    by " << -comp_change << endl;
    cout << "    tmb_l1a_delay          by " << -comp_change - clct_change - window_change/2 << endl;
    cout << "    alct_l1a_delay         by " << -alct_change << endl;
    while (getline(infile, line)) {
      if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
	place = line.find("ME");                   //in the form ME-_/_/_
	if (line[place + 2] == '-') {
	  cz_side = '-';
	  z_side = 0;
	}
	else if (line[place + 2] == '+') {
	  cz_side = '+';
	  z_side = 1;
	}
	z = (int) line[place + 3] - 48;
	y = (int) line[place + 5] - 48;
	if (line[place + 8] != '"')
	  x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
	else
	  x = (int) line[place +7] - 48;
	//
	ME[z_side][z][y][x].comp_timing += comp_change;
	ME[z_side][z][y][x].match_trig_alct_delay += comp_change;
	ME[z_side][z][y][x].mpc_tx_delay -= comp_change;
	ME[z_side][z][y][x].mpc_rx_delay -= comp_change;
	ME[z_side][z][y][x].tmb_lct_cable_delay -= comp_change;
	ME[z_side][z][y][x].tmb_l1a_delay -= comp_change;
	//
	ME[z_side][z][y][x].alct_drift_delay += alct_change;
	ME[z_side][z][y][x].match_trig_alct_delay -= alct_change;
	ME[z_side][z][y][x].alct_l1a_delay -= alct_change;
	//
	ME[z_side][z][y][x].clct_drift_delay += clct_change;
	ME[z_side][z][y][x].match_trig_alct_delay += clct_change;
	ME[z_side][z][y][x].mpc_tx_delay -= clct_change;
	ME[z_side][z][y][x].mpc_rx_delay -= clct_change;
	ME[z_side][z][y][x].tmb_l1a_delay -= clct_change;
	//
	ME[z_side][z][y][x].match_trig_window_size += window_change;
	ME[z_side][z][y][x].match_trig_alct_delay += window_change / 2;
	ME[z_side][z][y][x].mpc_tx_delay -= window_change / 2;
	ME[z_side][z][y][x].mpc_rx_delay -= window_change / 2;
	ME[z_side][z][y][x].tmb_l1a_delay -= window_change / 2;
	//
	if (ME[z_side][z][y][x].mpc_tx_delay < 0)
	  cout << endl << "WARNING: These adjustments shifted the mpc_tx_delay below 0 for chamber ME" << cz_side << z << "/" << y << "/" << x;	
	if (ME[z_side][z][y][x].tmb_lct_cable_delay < 0)
	  cout << endl << "WARNING: These adjustments shifted the tmb_lct_cable_delay below 0 for chamber ME" << cz_side << z << "/" << y << "/" << x;	
      }
    }
    output_measured_parameters(file_name);
  }
}
//
//---------------------------------------------------------------------------------------------------------
void output_measured_parameters(string file_name) {
  ifstream infile(file_name.c_str(), ios::in);
  string line;
  int x = 0, y = 0, z = 0, z_side = 0;
  int x_crate=0, z_crate=0;
  int temp;
  int place = 0;
  int afeb_number;
  int thresh;
  //  string new_file_name = "new_" + file_name;
  string new_file_name = file_name + ".new";
  ofstream config(new_file_name.c_str(), ios:: out);
  cout << endl << "The new config file will be named " << new_file_name;
  while (getline(infile, line)) {
    //
    //Find crate:
    //
    if (line.find("PeripheralCrate") != string::npos) {   // look for crate location
      place = line.find("VME");                           //in the form VMEp1_12 etc
      if (line[place + 3] == 'm') {
	z_side = 0;
      }
      else if (line[place + 3] == 'p') {
	z_side = 1;
      }
      z_crate = (int) line[place + 4] - 48;// station
                                           // no y (ring for peripheral crates)
      if (line[place + 7] != '"')          // handle 1 or 2 digits for phi of crate
	x_crate = ((int) line[place + 6] - 48) * 10 + (int) line[place + 7] - 48;
      else
	x_crate = (int) line[place +6] - 48;
    }
    //
    //Find chamber:
    //
    if (line.find("CSC label") != string::npos && line.find("Broadcast") == string::npos) {
      //
      place = line.find("ME");                   //in the form ME-_/_/_
      if (line[place + 2] == '-') {
	z_side = 0;
      }
      else if (line[place + 2] == '+') {
	z_side = 1;
      }      
      z = (int) line[place + 3] - 48;
      y = (int) line[place + 5] - 48;
      if (line[place + 8] != '"')
	x = ((int) line[place + 7] - 48) * 10 + (int) line[place + 8] - 48;
      else
	x = (int) line[place +7] - 48;
      config << line << endl;
      //
    }
    //
    //Now specific lines that need to be set:
    // 
    //CCB parameters:
    //
    else if (line.find("TTCrxCoarseDelay") != string::npos) {
      config << "        TTCrxCoarseDelay=\"" << TTCrxCoarseDelayPred[z_side][z_crate][x_crate] << "\"" << endl;
    } 
    //Oops, need to add similar for TTCrxFineDelay
    else if (line.find("TTCrxFineDelay=") != string::npos) {
      //
      config << "        TTCrxFineDelay=\"" << TTCrxFineDelayPred[z_side][z_crate][x_crate] << "\"" << endl;
    }
    //
    //TMB parameters:
    //
    else if (line.find("mpc_tx_delay") != string::npos){
      config << "         mpc_tx_delay=\"" << ME[z_side][z][y][x].mpc_tx_delay << "\"" << endl;
    } 
    else if (line.find("mpc_rx_delay") != string::npos){
      config << "         mpc_rx_delay=\"" << ME[z_side][z][y][x].mpc_rx_delay << "\"" << endl;
    } 
    else if (line.find("tmb_l1a_delay") != string::npos){
      config << "         tmb_l1a_delay=\"" << ME[z_side][z][y][x].tmb_l1a_delay << "\"" << endl;
    } 
    else if (line.find("clct_drift_delay") != string::npos) {
      config << "         clct_drift_delay=\"" << ME[z_side][z][y][x].clct_drift_delay << "\"" << endl;
    } 
    else if (line.find("match_trig_window_size") != string::npos) {
      config << "         match_trig_window_size=\"" << ME[z_side][z][y][x].match_trig_window_size << "\"" << endl;
       } 
    else if (line.find("tmb_bxn_offset") != string::npos){
      config << "         tmb_bxn_offset=\"" << ME[z_side][z][y][x].tmb_bxn_offset << "\"" << endl;
    } 
    //
    //ALCT parameters:
    //
    else if (line.find("alct_drift_delay") != string::npos) {
      config << "             alct_drift_delay=\"" << ME[z_side][z][y][x].alct_drift_delay << "\"" << endl;
    } 
    else if (line.find("alct_l1a_delay") != string::npos){
      config << "             alct_l1a_delay=\"" << ME[z_side][z][y][x].alct_l1a_delay << "\"" << endl;
    }
    else if (line.find("alct_bxn_offset") != string::npos){
      config << "             alct_bxn_offset=\"" << ME[z_side][z][y][x].alct_bxn_offset << "\"" << endl;
    } 
    //
    //DAQMB parameters:
    //
    else if (line.find("comp_timing") != string::npos) {
      config << "           comp_timing=\"" << ME[z_side][z][y][x].comp_timing << "\"" << endl;
    } 
    else if (line.find("tmb_lct_cable_delay") != string::npos){
      config << "           tmb_lct_cable_delay=\"" << ME[z_side][z][y][x].tmb_lct_cable_delay << "\"" << endl;
    } 
    //
    // More involved computation on the AFEB fine delays:
    //
    else if (line.find("afeb_fine_delay=") != string::npos) {
      temp = 0;
      //      place = line.find("AnodeChannel Number=") + 21;
      place = line.find("AnodeChannel afeb_number=") + 26;
      while (line[place] != '"') {
	temp = 10 * temp + (int) line[place] - 48;
	place++;
      }
      afeb_number = temp;
      temp = 0;
      place = line.find("afeb_threshold=") + 16;
      while (line[place] != '"') {
	temp = 10 * temp + (int) line[place] - 48;
	place++;
      }
      thresh = temp;
      config << "          <AnodeChannel afeb_number=\"" << afeb_number << "\"";
      if (afeb_number < 10)
	config<< " ";
      config << " afeb_fine_delay=\"" << ME[z_side][z][y][x].afeb_fine_delay[afeb_number - 1] << "\"";
      if (ME[z_side][z][y][x].afeb_fine_delay[afeb_number -1] < 10)
	config << " "; 
      config << " afeb_threshold=\"" << thresh << "\"";
      if (thresh < 10)
	config << " ";
      config << "> </AnodeChannel>" << endl;
    } else {
      config << line << endl;
    }
  }
}
//
// User interface subroutines
//
//Display options and return menu selection in integer choice
//---------------------------------------------------------------------------------------------------------
//
void options() {         //options of what type of data/delays to display
  //
  //N.B. If you add an option increase number in if statement below and also in if statement in main() ---
  //
  while (1) {
    cout << "Settings: comp_timing=" << comp_timing << ", alct/clct_drift_delay=" 
	 << alct_drift_delay << "/" << clct_drift_delay;
    if (display_only_measured_chambers == true)
      cout << ", display only chambers w/measured values" << endl << endl;
    else
      cout << ", display all chambers" << endl << endl;
    cout << "Please make a selection and then hit return:" << endl;
    cout << "1 : cfeb_rx_delay" << endl;
    cout << "2 : alct_tx_delay" << endl;
    cout << "3 : Compare alct_skewclear_delay to clct_skewclear_delay" << endl; 
    cout << "4 : alct_rx_clock_delay" << endl;
    cout << "5 : Display properties of chamber{s}" << endl;
    cout << "6 : Compare measured alct_rx_clock_delay to alct_tx_clock_delay" << endl; 
    cout << "7 : Compare predicted_ not_rounded_match_trig_alct_delay to other parameters" << endl;
    cout << "8 : cfeb_cable_delay" << endl;
    cout << "9 : alct_dav_cable_delay" << endl;
    cout << "10: cfeb_dav_cable_delay" << endl;
    cout << "11: afeb_fine_delays" << endl;
    cout << "12: match_trig_alct_delay"<< endl;
    cout << "13: tmb_lct_cable_delay a.k.a. AFF-L1A cable delay" << endl;
    cout << "14: alct_l1a_delay" << endl;
    cout << "15: tmb_l1a_delay" << endl;
    cout << "16: mpc_tx_delay" << endl;
    cout << "17: mpc_rx_delay" << endl;
    cout << "18: Output all predicted timing parameters to file" << endl;
    cout << "19: Create config.xml file" << endl;
    cout << "20: Modify comp_timing, alct/clct_drift_delay, match_trig_window_size (affects only PREDICTED values)" << endl;
    cout << "21: Change between text display for all chambers and only measured chambers" << endl;
    cout << "22: Adjust mpc/afeb_fine_delays to align chambers at SP then write config file" << endl;
    cout << "23: Change comp_timing, clct_drift_delay, and/or alct_drift_delay for MEASURED values then write config file" << endl;
    cout << "24: TTCrxCoarseDelay calculation based on optical fiber lengths" << endl;
    cout << "25: Chamber line up at MPC" << endl;
    cout << "26: Send plots to output files" << endl;
    cout << "27: Use measured values for subsequent predictions" << endl;
    cout << "28: Apply various corrections when adjusting TTCrx delays" << endl;
    cout << "29: Apply corrections to bx number offsets" << endl;
    cout << "99: Exit" << endl;
   
    cout << "===> ";
    cin >> choice; //user inputs his/her choice
    
    if ((choice < 1) || (choice > 29 && choice != 99) ) //if picked option is not allowed
      cout << endl << endl << choice << " is not an option.  Please pick again." << endl << endl;
    else 
      break;
  }
}
//
bool exit() { 
  //
  // function which returns 1 to exit, 0 to repeat main()
  //
  bool repeat = 0;             //return value
  bool ask_again = 1;          
  char key = 0;                //input key
  cout << endl << endl << "Would you like to continue? (y/n)";
  while (ask_again) {
    cin >> key;
    if ((key == 'n') || (key == 'N')) {
      repeat = 1;
      ask_again = 0;
    }
    else {
      if ((key == 'y') || (key == 'Y')) {
	repeat = 0;
	ask_again = 0;
      }
      else
	ask_again = 1;     //if 'n' or 'y' was not the key that was inputted by user
    }
  } // end of ask_again while loop
  cout << endl << endl;
  return repeat;
}
//
//---------------------------------------------------------------------------------------------------------
void set_parameters() {
  //
  //Used for PREDICTED values
  // Allows the user to define the values for
  //   . comp_timing
  //   . alct(clct)_drift_delay
  //   . match_trig_window_size
  //
  int repeat = 1;
  char key = '0';
  char temp = '0';
  while (repeat) {
    cout << endl << "Changes made here only affect the predicted timing parameter values";
    cout << endl << endl << "Select an option" << endl;
    cout << "1: Set comp_timing.  Currently set to " << comp_timing << endl;
    cout << "2: Set alct_drift_delay.  Currently set to " << alct_drift_delay << endl;
    cout << "3: Set clct_drift_delay.  Currently set to " << clct_drift_delay << endl;
    cout << "4: Set match_trig_window_size.  Currently set to " << match_trig_window_size << endl;
    cout << "5: Return to main menu" << endl << endl;
    cout << "===>";
    cin >> key;
    switch ((int) key - 48) {
    case (1):
      cout << endl << "Set comp_timing to ===>";
      cin >> temp;
      comp_timing = (int) temp - 48;
      break;
    case (2):
      cout << endl << "Set alct_drift_delay to ===>";
      cin >> temp;
      alct_drift_delay = (int) temp - 48;
      
      break;
    case (3):
      cout << endl << "Set clct_drift_delay to ===>";
      cin >> temp;
      clct_drift_delay = (int) temp - 48;
      
      break;
    case (4):
      cout << endl << "Set match_trig_window_size to ===>";
      cin >> temp;
      match_trig_window_size = (int) temp - 48;
      
      break;
    case (5):
      repeat = 0;
      break;
    default:
      break;
    }
  }
  predict_all_timing_parameters();
}
//
//---------------------------------------------------------------------------------------------------------
void display_graph() {
  /*
    When user selects data to be displayed as graph/histogram this function is called.
    For each option there is a switch and case.
    If histogram already exists it will be deleted and a new one will be made.
    The graphs/histograms/canvas for each case are named using the number of case.
   */

  char key = 0;
  int x, y, z;
  unsigned int w = 0;
  int z_side;
  //  int color = 2; //unused??
  int number = 0;
  double x2[2], y2[2];   //used to plot a straight line on graphs
  //double x_mid[2], y_mid[2];                       //unused??
  //double x_bot[2], y_bot[2], x_top[2], y_top[2];//unused??
  int nbinsx, nbinsy;
  int temp = 0;
  //int temp2 =0;
  //
  cout << "DEBUG have entered display_graph() with choice=" << choice << endl;
  gStyle->SetOptStat(11);   
  //        i = 1 = integral of bins     printed
  //        o = 1 = number of overflows  printed
  //        u = 1 = number of underflows printed
  //        r = 1 = rms                  printed
  //        m = 1 = mean value           printed
  //        e = 1 = number of entries    printed
  //        n = 1 = name of histogram    printed
  cout << "Number of chambers of interest --> " << number_of_chambers_of_interest << endl;
  //
  //-----------------------------------------------------------------------------
  switch (choice) {//displays graph and histograms for each choice
    //
  case 1:
    cout << "DEBUG have chosen to look at CFEB rx_delays" << endl;
    cout << endl << endl << "Which CFEB would you like to view? (0-4, X for average)" << endl;
    cout << "===> ";
    cin >> key;
    TH2F *old_object1 = (TH2F*) gROOT->FindObject("h1");
    if (old_object1) old_object1->Delete();
    //---------------------------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1","Delay Graph",200,10,600,400);
    //    c1->Divide(2,1);
    x2[0] = y2[0] = -0.5;
    x2[1] = y2[1] = 12.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    TH2F * h1 = new TH2F("h1","CFEB rx clock delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph = new TGraph(number_of_chambers_of_interest); 
    //--------------------------------------------------------------------------
    if ((key >= '0') && (key <= '4')) {
      cout << endl << endl << "You have selected cfeb[" << (int) key - 48 <<"]_delay" << endl; 
      for (z_side = side[0]; z_side <= side[1]; z_side++) {
	for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	  for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	    for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	      if (ME[z_side][z][y][x].has_measured_values == true) {
		graph->SetPoint(temp, ME[z_side][z][y][x].predicted_cfeb_delay, ME[z_side][z][y][x].cfeb[key - 48]);
		h1->Fill(ME[z_side][z][y][x].predicted_cfeb_delay, ME[z_side][z][y][x].cfeb[key - 48]);
		temp++;
	      }
	    }
	  }
	}
      }
    } else {
      cout << endl << endl <<"You have selected average_cfeb_delay" << endl;
      for (z_side = side[0]; z_side <= side[1]; z_side++) {
	for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	  for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	    for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	      if (ME[z_side][z][y][x].has_measured_values == true) {
		graph->SetPoint(temp, ME[z_side][z][y][x].predicted_cfeb_delay, ME[z_side][z][y][x].average_cfeb_delay);
		h1->Fill(ME[z_side][z][y][x].predicted_cfeb_delay, ME[z_side][z][y][x].average_cfeb_delay);
		temp++;
	      }
	    }
	  }
	}
      }
    }
    //----------------------------------------------------
    h1->GetXaxis()->SetTitle("predicted values");
    h1->GetYaxis()->SetTitle("measured values");
    h1->Draw("BOX");
    //
    TGraph *graph_line1 = new TGraph(2, x2, y2); 
    graph_line1->Draw("L");
    c1->Update();
    //c1->Print("asdf.gif");
    if (send_plots_to_file)
      c1->SaveAs("pictures/cfeb_rx_clock_delay.gif");
    break;
    //
  case 2:
    TH2F *old_object2 = (TH2F*) gROOT->FindObject("h2");
    if (old_object2) old_object2->Delete();
    //-----------------------------------------------------------------------
    TCanvas *c2 = new TCanvas("c2","Delay Graph",200,10,600,400);
    //
    x2[0] = y2[0] = -0.5;
    x2[1] = y2[1] = 12.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);

    TH2F *h2 = new TH2F("h2","ALCT tx clock delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    //
    TGraph *graph2 = new TGraph(number_of_chambers_of_interest);
    //-----------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ //ME number
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { //n number
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph2->SetPoint(temp, ME[z_side][z][y][x].predicted_alct_tx_clock_delay, ME[z_side][z][y][x].alct_tx_clock_delay);
	      h2->Fill(ME[z_side][z][y][x].predicted_alct_tx_clock_delay, ME[z_side][z][y][x].alct_tx_clock_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //--------------------------------------------------
    h2->GetXaxis()->SetTitle("predicted value");
    h2->GetYaxis()->SetTitle("measured value");
    h2->Draw("BOX");
    //
    TGraph * graph_line2 = new TGraph(2, x2, y2); 
    graph_line2->Draw("L");
    //--------------------------------------------------
    c2->Update();
    if (send_plots_to_file)
      c2->SaveAs("pictures/alct_tx_clock_delay.gif");
    break;
    //
  case 3:
    TH2F *old_object3 = (TH2F*) gROOT->FindObject("h3");
    if (old_object3) old_object3->Delete();
    //-----------------------------------------------------------------------------
    TCanvas *c3 = new TCanvas("c3","Delay Graph",200,10,600,400);
    c3->Divide(2,1);
    TH2F *h3 = new TH2F("h3","Number of chambers for each delay",65,20,65,65,0,65);
    TGraph *graph3 = new TGraph(468);
    //-----------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ //ME number
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { //n number
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    //    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph3->SetPoint(temp, ME[z_side][z][y][x].clct_skewclear_delay, ME[z_side][z][y][x].alct_skewclear_delay);
	      h3->Fill(ME[z_side][z][y][x].clct_skewclear_delay, ME[z_side][z][y][x].alct_skewclear_delay);
	      temp++;
	      // }
	  }
	}
      }
    }
    //-------------------------------------------------
    x2[0] = y2[0] = 20;
    x2[1] = y2[1] = 85;
    TGraph *graph_line3 = new TGraph(2,x2,y2);
    //-------------------------------------------------
    graph3->SetMarkerStyle(8);
    graph3->SetMarkerColor(4);
    graph_line3->SetTitle("alct_skewclear_delay to clct_skewclear_delay");
    graph_line3->GetXaxis()->SetTitle("clct_skewclear_delay (ns)");
    graph_line3->GetYaxis()->SetTitle("alct_skewclear_delay (ns)");
    //-------------------------------------------------
    c3->cd(1);
    graph_line3->Draw("AL");
    graph3->Draw("P");
    //-------------------------------------------------
    c3->cd(2);
    h3->Draw("BOX");
    graph_line3->Draw("");
    //-------------------------------------------------
    c3->Update();
    if (send_plots_to_file)
      c3->SaveAs("pictures/cfeb_vs_alct_skewclear.gif");
    break;
    //
  case 4:
    TH2F *old_object4 = (TH2F*) gROOT->FindObject("h4");
    if (old_object4) old_object4->Delete();
    //-------------------------------------------------------------------------
    TCanvas *c4 = new TCanvas("c4","Delay Graph",200,10,600,400);
    //
    x2[0] = y2[0] = -0.5;
    x2[1] = y2[1] = 12.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    TH2F *h4 = new TH2F("h4","ALCT rx clock delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    //
    TGraph *graph4 = new TGraph(number_of_chambers_of_interest);
    //-----------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) { 
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ //ME number
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { //n number
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph4->SetPoint(temp, ME[z_side][z][y][x].predicted_alct_rx_clock_delay, ME[z_side][z][y][x].alct_rx_clock_delay);
	      h4->Fill(ME[z_side][z][y][x].predicted_alct_rx_clock_delay, ME[z_side][z][y][x].alct_rx_clock_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------
    h4->GetXaxis()->SetTitle("Predicted values");
    h4->GetYaxis()->SetTitle("Measured values");
    h4->Draw("BOX");
    //
    TGraph * graph_line4 = new TGraph(2, x2, y2); 
    graph_line4->Draw("L");
    //------------------------------------------------
    c4->Update();
    if (send_plots_to_file)
      c4->SaveAs("pictures/alct_rx_clock_delay.gif");
    break;
    //
  case 5: //This is a listing of chamber parameters, nothing to plot.
    break;
    //
  case 6:
    TH2F *old_object6 = (TH2F*) gROOT->FindObject("h6");
    if (old_object6) old_object6->Delete();
    //--------------------------------------------------------------------------
    TCanvas *c6 = new TCanvas("c6","Delay Graph",200,10,600,400);
    c6->Divide(2,1);
    TH2F *h6 = new TH2F("h6","Number of chambers for each delay",13,0,13,13,0,13);
    TGraph *graph6 = new TGraph(number_of_chambers_of_interest);
    //---------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ //ME number
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { //n number
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph6->SetPoint(temp, ME[z_side][z][y][x].alct_tx_clock_delay, ME[z_side][z][y][x].alct_rx_clock_delay);
	      h6->Fill(ME[z_side][z][y][x].alct_tx_clock_delay, ME[z_side][z][y][x].alct_rx_clock_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //--------------------------------------------------
    x2[0] =-2.5; 
    y2[0] =-0.5;
    x2[1] = 12.5;
    y2[1] = 14.5;
    TGraph *graphf = new TGraph(2, x2, y2);
    //-------------------------------------------------
    x2[0] = 8.5; 
    y2[0] = -0.5;
    x2[1] = 12.5;
    y2[1] = 3.5;;
    TGraph *graphf2 = new TGraph(2, x2, y2);
    //-------------------------------------------------
    graph6->SetMarkerStyle(8);
    graph6->SetMarkerColor(4);
    graphf->SetTitle("Measured alct_rx_clock_delay to alct_tx_clock_delay");
    graphf->GetXaxis()->SetTitle("measured alct_tx_delay");
    graphf->GetYaxis()->SetTitle("measured alct_rx_delay");
    //------------------------------------------------
    c6->cd(1);
    graphf->Draw("AL");
    graphf2->Draw();
    graph6->Draw("P");
    //------------------------------------------------
    c6->cd(2);
    h6->Draw("BOX");
    graphf2->Draw();
    graphf->Draw("");
    //------------------------------------------------
    c6->Update();
    if (send_plots_to_file)
      c6->SaveAs("pictures/alct_meas_tx_vs_rx_delays.gif");
    break;
  case 7:
    TCanvas *c7 = new TCanvas("c7","Delay Graph",200,10,600,400);
    c7->Divide(2,2);
    TGraph *graph7 = new TGraph(number_of_chambers_of_interest); 
    TGraph *graph7a = new TGraph(number_of_chambers_of_interest); 
    TGraph *graph7b = new TGraph(number_of_chambers_of_interest); 
    TGraph *graph7c = new TGraph(number_of_chambers_of_interest);
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ //ME number
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) { //n number
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {  
	      graph7->SetPoint(temp, ME[z_side][z][y][x].cable_difference_in_bx, ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay);
	      
	      graph7a->SetPoint(temp, ME[z_side][z][y][x].clock_phase_difference, ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay);
	      
	      graph7b->SetPoint(temp, ME[z_side][z][y][x].afeb_fine_delay_correction, ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay);
	      
	      graph7c->SetPoint(temp, ME[z_side][z][y][x].clock_phase_difference + ME[z_side][z][y][x].afeb_fine_delay_correction, ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay);	
	      
	      temp++;
	    }
	  }
	}
      }
    }
    x2[0] = y2[0] = 4;
    x2[1] = y2[1] = 7.5;
    // JH following line somehow unused (why?)
    //TGraph *graph_line7 = new TGraph(2,x2, y2);
    graph7->SetMarkerStyle(8);
    graph7a->SetMarkerStyle(8);
    graph7b->SetMarkerStyle(8);
    graph7c->SetMarkerStyle(8);
    graph7->SetMarkerColor(4);
    graph7a->SetMarkerColor(4);
    graph7b->SetMarkerColor(4);
    graph7c->SetMarkerColor(4);
    
    graph7->SetTitle("measured match_trig_alct_delay to cable difference(bx)");
    graph7->GetYaxis()->SetTitle("match_trig_alct_delay");
    graph7->GetXaxis()->SetTitle("cable difference");
    graph7a->SetTitle("measured match_trig_alct_delay to clock_phase_difference(bx)");
    graph7b->SetTitle("measured match_trig_alct_delay to afeb_fine_delay_corretion(bx)");
    graph7c->SetTitle("measured match_trig_alct_delay to clock_phase + afeb_correction");
    c7->cd(1);
    graph7->Draw("AP");
    c7->cd(2);
    graph7a->Draw("AP");
    c7->cd(3);
    graph7b->Draw("AP");
    c7->cd(4);
    graph7c->Draw("AP");
    c7->Update(); 
    if (send_plots_to_file)
      c7->SaveAs("pictures/match_trig_alct_delay_contributions.gif");
    break;
  case 8:
    TH2F *old_object8 = (TH2F*) gROOT->FindObject("h8");
    if (old_object8) old_object8->Delete();
    //-----------------------------------------------------------------------
    TCanvas *c8 = new TCanvas("c8","Delay Graph",200,10,600,400);
    //c8->Divide(2,1);
    x2[0] = -1.5;
    y2[0] = -1.5;
    x2[1] =  3.5;
    y2[1] =  3.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    TH2F *h8 = new TH2F("h8","Chamber Distribution",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph8 = new TGraph(number_of_chambers_of_interest); 
    //---------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph8->SetPoint(temp, ME[z_side][z][y][x].predicted_cfeb_cable_delay, ME[z_side][z][y][x].cfeb_cable_delay);
	      h8->Fill( ME[z_side][z][y][x].predicted_cfeb_cable_delay, ME[z_side][z][y][x].cfeb_cable_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //----------------------------------------------
    //x2[0] = y2[0] = -1.5;
    //x2[1] = y2[1] =  3.5;
    //TGraph *graph_line8 = new TGraph(2, x2, y2); 
    //----------------------------------------------
    //graph8->SetMarkerStyle(8);
    //graph8->SetMarkerColor(4);
    //graph_line8->SetTitle("CFEB Cable Delay");
    //graph_line8->GetXaxis()->SetTitle("predicted_cfeb_cable_delay");
    //graph_line8->GetYaxis()->SetTitle("set_cfeb_cable_delay");
    //-----------------------------------------------
    //c8->cd(1);
    //graph_line8->Draw("AL");
    //graph8->Draw("P");
    //-----------------------------------------------
    //c8->cd(2);
    h8->GetXaxis()->SetTitle("predicted_cfeb_cable_delay");
    h8->GetYaxis()->SetTitle("set_cfeb_cable_delay");
    h8->Draw("BOX");
    TGraph *graph_line8 = new TGraph(2, x2, y2); 
    graph_line8->Draw("L");
    //-----------------------------------------------
    c8->Update();
    if (send_plots_to_file)
      c8->SaveAs("pictures/cfeb_cable_delay_dmb.gif");
    break;
 case 9:
    TH2F *old_object9 = (TH2F*) gROOT->FindObject("h9");
    if (old_object9) old_object9->Delete();
    //-------------------------------------------------------------------------------
    TCanvas *c9 = new TCanvas("c9","Delay Graph",200,10,600,400);
    //    c9->Divide(2,1);
    x2[0] = -0.5;
    y2[0] = -0.5;
    x2[1] =  5.5;
    y2[1] =  5.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    //
    TH2F *h9 = new TH2F("h9","ALCT DAV cable delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph9 = new TGraph(number_of_chambers_of_interest);
    //---------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number   
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph9->SetPoint(temp, ME[z_side][z][y][x].predicted_alct_dav_cable_delay, ME[z_side][z][y][x].alct_dav_cable_delay);
	      h9->Fill(ME[z_side][z][y][x].predicted_alct_dav_cable_delay, ME[z_side][z][y][x].alct_dav_cable_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------
    //    graph9->SetMarkerStyle(8);
    //    graph9->SetMarkerColor(4);
    //    graph_line9->SetTitle("Predicted to Measured alct_dav_cable_delay");
    //----------------------------------------------
    //    c9->cd(1);
    //    graph_line9->Draw("LA");
    //    graph9->Draw("P");
    //-----------------------------------------------
    //    c9->cd(2);
    h9->GetXaxis()->SetTitle("predicted values");
    h9->GetYaxis()->SetTitle("measured values");
    h9->Draw("BOX");
    TGraph *graph_line9 = new TGraph(2, x2, y2); 
    graph_line9->Draw("L");
    //-----------------------------------------------
    c9->Update();
    if (send_plots_to_file)
      c9->SaveAs("pictures/alct_dav_cable_delay.gif");
    break;
  case 10:
    TH2F *old_object10 = (TH2F*) gROOT->FindObject("h10");
    if (old_object10) old_object10->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c10 = new TCanvas("c10","Delay Graph",200,10,600,400);
    //    c10->Divide(2,1);
    x2[0] = -1.5;
    y2[0] = -1.5;
    x2[1] =  3.5;
    y2[1] =  3.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    //
    TH2F *h10 = new TH2F("h10","CFEB DAV cable delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph10 = new TGraph(number_of_chambers_of_interest);
    //-----------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph10->SetPoint(temp, ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay, ME[z_side][z][y][x].cfeb_dav_cable_delay);
	      h10->Fill(ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay, ME[z_side][z][y][x].cfeb_dav_cable_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------
    //-----------------------------------------------
    //    graph10->SetMarkerStyle(8);
    //    graph10->SetMarkerColor(4);
    //    graph_line10->SetTitle("Prediected to measured cfeb_dav_cable_delay");
    //-----------------------------------------------
    ////    c10->cd(2);
    h10->GetXaxis()->SetTitle("predicted values");
    h10->GetYaxis()->SetTitle("measured values");
    h10->Draw("BOX");
    TGraph *graph_line10 = new TGraph(2, x2, y2);
    graph_line10->Draw("L");
    //----------------------------------------------
    c10->Update();
    if (send_plots_to_file)
      c10->SaveAs("pictures/cfeb_dav_cable_delay.gif");
    break;    
  case 11:
    number = 0;
    TCanvas *c11 = new TCanvas("c11","Delay Graph",200,10,600,400);
    // TCanvas *c11a = new TCanvas("c11a","Delay Graph",200,10,600,400);
    c11->Divide(2,1);
    TGraph* graph11;
    TGraph* graph11a;
    //------------------------------------------------
    x2[0] = 0;
    y2[0] = -5;
    x2[1] = 45;
    y2[1] = 20;
    TGraph *graph_bounds = new TGraph(2, x2, y2);
    TGraph *graph_bounds2 = new TGraph(2, x2, y2);
    //-----------------------------------------------
    graph_bounds2->SetTitle("AFEB Fine Delay Distrabution (Inputted from Config)");
    graph_bounds2->GetYaxis()->SetTitle("afeb_fine_delay");
    graph_bounds2->GetXaxis()->SetTitle("afeb #");
    c11->cd(2);
    graph_bounds2->Draw("AP");
    c11->cd(1);
    graph_bounds->SetTitle("AFEB Fine Delay Distrabution (Inputted from initial values)");
    graph_bounds->GetYaxis()->SetTitle("afeb_fine_delay");
    graph_bounds->GetXaxis()->SetTitle("afeb #");
    graph_bounds->Draw("AP");
    //--------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number  
	    if (((ME[z_side][z][y][x].has_measured_values == true) && (display_only_measured_chambers == true)) || (display_only_measured_chambers == false)) {
	      graph11 = new TGraph(ME[z_side][z][y][x].init_afeb_fine_delay.size());  
	      for (w = 0; w < ME[z_side][z][y][x].init_afeb_fine_delay.size(); w++) {
		graph11->SetPoint(w, w, ME[z_side][z][y][x].init_afeb_fine_delay[w]);
	      }
	      graph11->SetMarkerStyle(8);
	      graph11->SetMarkerColor((number + 2) % 9);
	      graph11->SetLineColor((number + 2) % 9);
	      c11->cd(1);
	      graph11->Draw("CP");
	      number++;
	    }
	  }
	}
      }
    }
    number = 0;
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){ 
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number  
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      
	      graph11a = new TGraph(ME[z_side][z][y][x].afeb_fine_delay.size());  
	      for (w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++)
		graph11a->SetPoint(w, w, ME[z_side][z][y][x].afeb_fine_delay[w]);
	      graph11a->SetMarkerStyle(8);
	      graph11a->SetMarkerColor((number + 2) % 9);
	      graph11a->SetLineColor((number + 2) % 9);
	      c11->cd(2);
	      graph11a->Draw("CP");		
	      number++;
	    }
	  }
	}
      }
    }
    c11->Update();
    
    // graph11->Draw("APL");
    // c11->Update();
    
    if (send_plots_to_file)
      c11->SaveAs("pictures/afeb_fine_delays.gif");
   break;
  case 12:
    TH2F *old_object12 = (TH2F*) gROOT->FindObject("h12");
    if (old_object12) old_object12->Delete();
    //-------------------------------------------------------------------------------
    TCanvas *c12 = new TCanvas("c12","Delay Graph",200,10,600,400);
    //    c9->Divide(2,1);
    x2[0] = 0;
    y2[0] = 0;
    x2[1] = 11;
    y2[1] = 11;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    //
    TH2F *h12 = new TH2F("h12","ALCT delay to match CLCT",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph12 = new TGraph(number_of_chambers_of_interest);
    //---------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) { //chamber number   
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph12->SetPoint(temp, ME[z_side][z][y][x].predicted_match_trig_alct_delay, ME[z_side][z][y][x].match_trig_alct_delay);
	      h12->Fill(ME[z_side][z][y][x].predicted_match_trig_alct_delay, ME[z_side][z][y][x].match_trig_alct_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------
  
    //-----------------------------------------------
    //    graph12->SetMarkerStyle(8);
    //    graph12->SetMarkerColor(4);
    //    graph_line12->SetTitle("Predicted to Measured match_trig_alct_delay");
    //----------------------------------------------
    //    c12->cd(1);
    //    graph_line12->Draw("LA");
    //    graph12->Draw("P");
    //-----------------------------------------------
    //    c12->cd(2);
    h12->GetXaxis()->SetTitle("predicted values");
    h12->GetYaxis()->SetTitle("measured values");
    h12->Draw("BOX");
    TGraph *graph_line12 = new TGraph(2, x2, y2); 
    graph_line12->Draw("L");
    //-----------------------------------------------
    c12->Update();
    if (send_plots_to_file)
      c12->SaveAs("pictures/match_trig_alct_delay.gif");
    //
    //    TH2F *old_object12 = (TH2F*) gROOT->FindObject("h12");
    //    if (old_object12) old_object12->Delete();
    //    TH2F *old_object12b = (TH2F*) gROOT->FindObject("h12b");
    //    if (old_object12b) old_object12b->Delete();
    //    
    //    TCanvas *c12 = new TCanvas("c12","Delay Graph",200,10,600,400);
    //    c12->Divide(2,2);
    //
    //    TH2F *h12 = new TH2F("h12","Number of chambers for each delay",5,4,9,5,4,9); 
    //    TGraph *graph12 = new TGraph(number_of_chambers_of_interest);                  
    //    TH2F *h12b = new TH2F("h12b","Number of chambers for each delay",5,4,9,5,4,9);
    //    TGraph *graph12b = new TGraph(number_of_chambers_of_interest);    
    //    
    //    for (z_side = side[0]; z_side <= side[1]; z_side++) {
    //      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
    //	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
    //	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
    //	    if (ME[z_side][z][y][x].has_measured_values == true) {
    //	      graph12b->SetPoint(temp, ME[z_side][z][y][x].predicted_match_trig_alct_delay, ME[z_side][z][y][x].match_trig_alct_delay);
    //	      h12b->Fill(ME[z_side][z][y][x].predicted_match_trig_alct_delay, ME[z_side][z][y][x].match_trig_alct_delay);
    //	      temp++;
    //	      //--------------------------------------------------------------------------------------------------------
    //	      if (ME[z_side][z][y][x].not_rounded_match_trig_alct_delay > -1) {
    //		graph12->SetPoint(temp2, ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay, ME[z_side][z][y][x].not_rounded_match_trig_alct_delay);
    //		h12->Fill(ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay, ME[z_side][z][y][x].not_rounded_match_trig_alct_delay);
    //		temp2++;
    //	      }
    //	      //-------------------------------------------------------------------------------------------------------
    //	    }
    //	  }
    //	}
    //      }
    //    }
    //    x2[0] = y2[0] = 0;
    //    x2[1] = y2[1] = 10;
    //    
    //    TGraph *graphm = new TGraph(2, x2, y2); 
    //    TGraph *graphm2 = new TGraph(2, x2, y2);
    //    graph12->SetMarkerStyle(8);
    //    graph12->SetMarkerColor(4); 
    //    graph12b->SetMarkerStyle(8);
    //    graph12b->SetMarkerColor(4);
    //    graphm->SetTitle("CLCT ALCT Match Window Parameters");
    //    graphm->GetXaxis()->SetTitle("predicted_alct_match_windwo_delay");
    //    graphm->GetYaxis()->SetTitle("measured match_trig_alct_delay");
    //    c12->cd(1);
    //    graphm->Draw("AL");
    //    graph12->Draw("P");
    //    c12->cd(2);
    //    h12->Draw("BOX");
    //    graphm->Draw();
    //    c12->cd(3);
    //    graphm2->SetTitle("CLCT ALCT Match Window Parameters (Rounded to nearest int)");
    //    graphm2->GetXaxis()->SetTitle("predicted_match_trig_alct_delay");
    //    graphm2->GetYaxis()->SetTitle("measured match_trig_alct_delay");
    //    graphm2->Draw("AL");
    //    graph12b->Draw("P");
    //    c12->cd(4);
    //    h12b->Draw("BOX");
    //    graphm2->Draw();
    // 
    //    c12->Update();
    break;
  case 13:
    TH2F *old_object13 = (TH2F*) gROOT->FindObject("h13");
    if (old_object13) old_object13->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c13 = new TCanvas("c1","Delay Graph",200,10,600,400);
    //    c13->Divide(2,1);
    x2[0] = y2[0] = -0.5;
    x2[1] = y2[1] =  7.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(x2[1] - x2[0]);
    //
    TH2F *h13 = new TH2F("h13","AFF-L1A cable delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph13 = new TGraph(number_of_chambers_of_interest); 
    //-------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph13->SetPoint(temp, ME[z_side][z][y][x].predicted_tmb_lct_cable_delay, ME[z_side][z][y][x].tmb_lct_cable_delay);
	      h13->Fill(ME[z_side][z][y][x].predicted_tmb_lct_cable_delay, ME[z_side][z][y][x].tmb_lct_cable_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------------
    //-----------------------------------------------------
    //    graph13->SetMarkerStyle(8);
    //    graph13->SetMarkerColor(4);
    //    graph_line13->SetTitle("TMB LCT Cable Delay");
    //------------------------------------------------------
    //    c13->cd(1);
    //
    //    graph13->Draw("P");
    //-----------------------------------------------------
    h13->GetXaxis()->SetTitle("predicted values");
    h13->GetYaxis()->SetTitle("measured values");
    h13->Draw("BOX");
    TGraph *graph_line13 = new TGraph(2, x2, y2); 
    graph_line13->Draw("L");
    //-----------------------------------------------------
    c13->Update();
    if (send_plots_to_file)
      c13->SaveAs("pictures/aff_l1a_cable_delay.gif");
    break;
  case 14:
    TH2F *old_object14 = (TH2F*) gROOT->FindObject("h14");
    if (old_object14) old_object14->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c14 = new TCanvas("c1","Delay Graph",200,10,600,400);
    //    c14->Divide(2,1);
    x2[0] = y2[0] = 138.5;
    x2[1] = y2[1] = 144.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    TH2F *h14 = new TH2F("h14","ALCT L1A delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    TGraph *graph14 = new TGraph(number_of_chambers_of_interest); 
    //-------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    if (ME[z_side][z][y][x].has_measured_values == true) {	  
	      graph14->SetPoint(temp, ME[z_side][z][y][x].predicted_alct_l1a_delay, ME[z_side][z][y][x].alct_l1a_delay);
	      h14->Fill(ME[z_side][z][y][x].predicted_alct_l1a_delay, ME[z_side][z][y][x].alct_l1a_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------------
    h14->GetXaxis()->SetTitle("predicted values");
    h14->GetYaxis()->SetTitle("measured values");
    h14->Draw("BOX");
    TGraph *graph_line14 = new TGraph(2, x2, y2); 
    graph_line14->Draw("L");
    //-----------------------------------------------------
    c14->Update();
    if (send_plots_to_file)
      c14->SaveAs("pictures/alct_l1a_delay.gif");
    break;
  case 15:
    TH2F *old_object15 = (TH2F*) gROOT->FindObject("h15");
    if (old_object15) old_object15->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c15 = new TCanvas("c15","Delay Graph",200,10,600,400);
    //
    x2[0] = y2[0] = 119.5;
    x2[1] = y2[1] = 124.5;
    nbinsx = int(x2[1] - x2[0]);
    nbinsy = int(y2[1] - y2[0]);
    //JH: option to plot difference meas-pred versus various things such as Skewclear lengths (ns)
    //x2[0] = 25.;
    //y2[0] = -3.;
    //x2[1] = 85.;
    //y2[1] =  3.;
    //nbinsx = 60;
    //nbinsy = 30;

    TH2F *h15 = new TH2F("h15","TMB L1A delay",nbinsx,x2[0],x2[1],nbinsy,y2[0],y2[1]);
    //
    TGraph *graph15 = new TGraph(number_of_chambers_of_interest); 
    //-------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      //graph15->SetPoint(temp, ME[z_side][z][y][x].clct_skewclear_delay,
	      //		ME[z_side][z][y][x].tmb_l1a_delay-ME[z_side][z][y][x].predicted_tmb_l1a_delay); 
	      // h15->Fill(ME[z_side][z][y][x].clct_skewclear_delay,
	      //		ME[z_side][z][y][x].tmb_l1a_delay-ME[z_side][z][y][x].predicted_tmb_l1a_delay); 
	      graph15->SetPoint(temp, ME[z_side][z][y][x].predicted_tmb_l1a_delay, ME[z_side][z][y][x].tmb_l1a_delay);
	      h15->Fill(ME[z_side][z][y][x].predicted_tmb_l1a_delay, ME[z_side][z][y][x].tmb_l1a_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //JH option:
    //h15->GetXaxis()->SetTitle("clct_skewclear_delay (ns)");
    //h15->GetYaxis()->SetTitle("tmb_l1a_delay (meas-pred) (bx)");
    h15->GetXaxis()->SetTitle("predicted values");
    h15->GetYaxis()->SetTitle("measured values");
    h15->Draw("BOX");
    //
    TGraph *graph_line15 = new TGraph(2, x2, y2); 
    graph_line15->Draw("L");
    //-----------------------------------------------------
    c15->Update();
    if (send_plots_to_file)
      c15->SaveAs("pictures/tmb_l1a_delay.gif");
    break;
    //
 case 16:
    TH2F *old_object16 = (TH2F*) gROOT->FindObject("h16");
    if (old_object16) old_object16->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c16 = new TCanvas("c16","Delay Graph",200,10,600,400);
    c16->Divide(2,1);
    TH2F *h16 = new TH2F("h16","Number of chambers for each delay",5,0,5,5,0,5);
    TGraph *graph16 = new TGraph(number_of_chambers_of_interest); 
    //-------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph16->SetPoint(temp, ME[z_side][z][y][x].predicted_mpc_tx_delay, ME[z_side][z][y][x].mpc_tx_delay);
	      h16->Fill(ME[z_side][z][y][x].predicted_mpc_tx_delay, ME[z_side][z][y][x].mpc_tx_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------------
    x2[0] = y2[0] = 0;
    x2[1] = y2[1] = 5;
    TGraph *graph_line16 = new TGraph(2, x2, y2); 
    //-----------------------------------------------------
    graph16->SetMarkerStyle(8);
    graph16->SetMarkerColor(4);
    graph_line16->SetTitle("mpc_tx_delay");
    graph_line16->GetXaxis()->SetTitle("predicted_mpc_tx_delay");
    graph_line16->GetYaxis()->SetTitle("measured_mpc_tx_delay");
    //------------------------------------------------------
    c16->cd(1);
    graph_line16->Draw("AL");
    //JH   graph_line->Draw("AL");
    graph16->Draw("P");
    //-----------------------------------------------------
    c16->cd(2);
    h16->Draw("BOX");
    graph_line16->Draw("");
    //-----------------------------------------------------
    c16->Update();
    if (send_plots_to_file)
      c16->SaveAs("pictures/mpc_tx_delay.gif");
    break;
 case 17:
    TH2F *old_object17 = (TH2F*) gROOT->FindObject("h17");
    if (old_object17) old_object17->Delete();
    //------------------------------------------------------------------------------
    TCanvas *c17 = new TCanvas("c17","Delay Graph",200,10,600,400);
    c17->Divide(2,1);
    TH2F *h17 = new TH2F("h17","Number of chambers for each delay",5,5,10,5,5,10);
    TGraph *graph17 = new TGraph(number_of_chambers_of_interest); 
    //-------------------------------------------------------------------------------
    for (z_side = side[0]; z_side <= side[1]; z_side++) {    
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    if (ME[z_side][z][y][x].has_measured_values == true) {
	      graph17->SetPoint(temp, ME[z_side][z][y][x].predicted_mpc_rx_delay, ME[z_side][z][y][x].mpc_rx_delay);
	      h17->Fill(ME[z_side][z][y][x].predicted_mpc_rx_delay, ME[z_side][z][y][x].mpc_rx_delay);
	      temp++;
	    }
	  }
	}
      }
    }
    //------------------------------------------------------
    x2[0] = y2[0] = 5;
    x2[1] = y2[1] = 10;
    TGraph *graph_line17 = new TGraph(2, x2, y2); 
    //-----------------------------------------------------
    graph17->SetMarkerStyle(8);
    graph17->SetMarkerColor(4);
    graph_line17->SetTitle("mpc_rx_delay");
    graph_line17->GetXaxis()->SetTitle("predicted_mpc_rx_delay");
    graph_line17->GetYaxis()->SetTitle("measured_mpc_rx_delay");
    //------------------------------------------------------
    c17->cd(1);
    graph_line17->Draw("AL");
    graph17->Draw("P");
    //-----------------------------------------------------
    c17->cd(2);
    h17->Draw("BOX");
    graph_line17->Draw("");
    //-----------------------------------------------------
    c17->Update();
    if (send_plots_to_file)
      c17->SaveAs("pictures/mpc_rx_delay.gif");
    break;
    
  default:
    ; }
}
//
char difference_beyond_thresh(int measured, int predicted, int thresh, int z, int y, int x) {
  /*
    Determines if predicted value is close to measured value.  
    Set the threshold cut range using int thresh
  */
  if ((abs(measured - predicted) > thresh) && (measured != -1)) {
    return 'X';
  }
  else
    return ' ';
}
//
//---------------------------------------------------------------------------------------------------------
void display_data() {                       
  /*  
      Texted based display of data.  Function options() assigns a value to variable choice.
      This loops through all chambers of interest and displays the choice.
  */
  int x, y, z;
  unsigned int w = 0;                           //for loop variables
  int display_text = 1;
  int z_side;
  char cz_side = '-';
  for (z_side = side[0]; z_side <= side[1]; z_side++) {
    //
    if (z_side == 0)
      cz_side = '-';
    else if (z_side == 1)
      cz_side = '+';
    //
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){       //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {    //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {  //chamber number 
	  if (((display_only_measured_chambers == true) && (ME[z_side][z][y][x].has_measured_values == true)) || display_only_measured_chambers == false) {
	    switch (choice) {
	    case (1):
	      cout << "ME" << cz_side  << z << "/" << y << "/" << x << " : " <<  endl;
	      cout << "     Predicted cfeb0-4_delay            : " << ME[z_side][z][y][x].predicted_cfeb_delay << endl;
	      cout << "     Average cfeb0-4_delay              : " << ME[z_side][z][y][x].average_cfeb_delay << endl;
	      cout << "     measured cfeb0                     : " << ME[z_side][z][y][x].cfeb[0] << endl;
	      cout << "     measured cfeb1                     : " << ME[z_side][z][y][x].cfeb[1] << endl;
	      cout << "     measured cfeb2                     : " << ME[z_side][z][y][x].cfeb[2] << endl;
	      cout << "     measured cfeb3                     : " << ME[z_side][z][y][x].cfeb[3] << endl;
	      cout << "     measured cfeb4                     : " << ME[z_side][z][y][x].cfeb[4] << endl;
	      break;
	    case (2): 
	      if (display_text == 1) {
		cout << "alct_tx_clock_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side  << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].alct_tx_clock_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_alct_tx_clock_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_tx_clock_delay, ME[z_side][z][y][x].predicted_alct_tx_clock_delay, alct_tx_clock_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (3):
	      if (display_text == 1) {
		cout << "alct_skewclear_delay to clct_skewclear_delay : " << endl;
		cout << "            |    ALCT    |     CFEB    |" << endl;
	      }
	      cout << "ME" << cz_side  << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(5) << ME[z_side][z][y][x].alct_skewclear_delay
		   << "   |     " << setw(5) <<  ME[z_side][z][y][x].clct_skewclear_delay << "   |" << endl;
	      break;
	    case (4):
	      if (display_text == 1) {
		cout << "alct_rx_clock_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].alct_rx_clock_delay << "     |     "  
		   << setw(3) <<  ME[z_side][z][y][x].predicted_alct_rx_clock_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_rx_clock_delay, ME[z_side][z][y][x].predicted_alct_rx_clock_delay, alct_rx_clock_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (5):
	      cout << "ME" << cz_side << z << "/" << y << "/" << x << " : " << endl;
	      cout << "     alct_skewclear_delay (ns)          |          " << setw(5) << ME[z_side][z][y][x].alct_skewclear_delay << "           |" << endl;
	      cout << "     clct_skewclear_delay  (ns)          |          " << setw(5) << ME[z_side][z][y][x].clct_skewclear_delay << "           |" <<  endl;
	      cout << "                                        |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      cout << "     comp_timing                        |    " << setw(3) << ME[z_side][z][y][x].comp_timing << "     |     " 
		   << setw(3) << comp_timing << "     |" << endl;
	      cout << "     alct_drift_delay                   |    " << setw(3) << ME[z_side][z][y][x].alct_drift_delay << "     |     " 
		   << setw(3) << alct_drift_delay << "     |" << endl;
	      cout << "     clct_drift_delay                   |    " << setw(3) << ME[z_side][z][y][x].clct_drift_delay << "     |     " 
		   << setw(3) << clct_drift_delay << "     |" << endl;
	      cout << "     match_trig_window_size             |    " << setw(3) << ME[z_side][z][y][x].match_trig_window_size << "     |     " 
		   << setw(3) << match_trig_window_size << "     |" << endl;
	      cout << "     cfeb0_delay                        |    " << setw(3) << ME[z_side][z][y][x].cfeb[0] << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb[0], ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb1_delay                        |    " << setw(3) << ME[z_side][z][y][x].cfeb[1] << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb[1], ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb2_delay                        |    " << setw(3) << ME[z_side][z][y][x].cfeb[2] << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb[2], ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb3_delay                        |    " << setw(3) << ME[z_side][z][y][x].cfeb[3] << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb[3], ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb4_delay                        |    " << setw(3) << ME[z_side][z][y][x].cfeb[4] << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb[4], ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     Average cfeb0-4_delay              |    " << setw(3) << ME[z_side][z][y][x].average_cfeb_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].average_cfeb_delay, ME[z_side][z][y][x].predicted_cfeb_delay, cfeb_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     alct_tx_clock_delay                |    " << setw(3) << ME[z_side][z][y][x].alct_tx_clock_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_alct_tx_clock_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_tx_clock_delay, ME[z_side][z][y][x].predicted_alct_tx_clock_delay, alct_tx_clock_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     alct_rx_clock_delay                |    " << setw(3) << ME[z_side][z][y][x].alct_rx_clock_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_alct_rx_clock_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_rx_clock_delay, ME[z_side][z][y][x].predicted_alct_rx_clock_delay, alct_rx_clock_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     match_trig_alct_delay              |    " << setw(3) << ME[z_side][z][y][x].match_trig_alct_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_match_trig_alct_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].match_trig_alct_delay, ME[z_side][z][y][x].predicted_match_trig_alct_delay, match_trig_alct_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     mpc_tx_delay                       |    " << setw(3) << ME[z_side][z][y][x].mpc_tx_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_mpc_tx_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].mpc_tx_delay, ME[z_side][z][y][x].predicted_mpc_tx_delay, mpc_tx_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     mpc_rx_delay                       |    " << setw(3) << ME[z_side][z][y][x].mpc_rx_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_mpc_rx_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].mpc_rx_delay, ME[z_side][z][y][x].predicted_mpc_rx_delay, mpc_rx_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb_cable_delay                   |    " << setw(3) << ME[z_side][z][y][x].cfeb_cable_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb_cable_delay, ME[z_side][z][y][x].predicted_cfeb_cable_delay, cfeb_cable_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     cfeb_dav_cable_delay               |    " << setw(3) << ME[z_side][z][y][x].cfeb_dav_cable_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb_dav_cable_delay, ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay, cfeb_dav_cable_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     tmb_lct_cable_delay                |    " << setw(3) << ME[z_side][z][y][x].tmb_lct_cable_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_tmb_lct_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].tmb_lct_cable_delay, ME[z_side][z][y][x].predicted_tmb_lct_cable_delay, tmb_lct_cable_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     alct_dav_cable_delay               |    " << setw(3) << ME[z_side][z][y][x].alct_dav_cable_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_alct_dav_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_dav_cable_delay, ME[z_side][z][y][x].predicted_alct_dav_cable_delay, alct_dav_cable_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     tmb_l1a_delay                      |    " << setw(3) << ME[z_side][z][y][x].tmb_l1a_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_tmb_l1a_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].tmb_l1a_delay, ME[z_side][z][y][x].predicted_tmb_l1a_delay, tmb_l1a_delay_thresh, z, y, x) << "      |" << endl;
	      
	      cout << "     alct_l1a_delay                     |    " << setw(3) << ME[z_side][z][y][x].alct_l1a_delay << "     |     " 
		   << setw(3) << ME[z_side][z][y][x].predicted_alct_l1a_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_l1a_delay, ME[z_side][z][y][x].predicted_alct_l1a_delay, alct_l1a_delay_thresh, z, y, x) << "      |" << endl;
	      
	      break;
	    case (6):
	      if (display_text == 1) {
		cout << "compare alct_rx_clock_delay to alct_tx_clock_delay : " << endl;
		cout << "            |     rx     |      tx     |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].alct_rx_clock_delay
		   << "     |     " << setw(3) <<  ME[z_side][z][y][x].alct_tx_clock_delay << "     |" << endl;
	      break;
	    case (7):
	      if (display_text == 1) {
		cout << "not_rounded_match_trig_alct_delay : " << endl;
		cout << "            |  Measured  |  Predicted  |" << endl;
	      }	  
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(5) << ME[z_side][z][y][x].not_rounded_match_trig_alct_delay 
                   << "   |    " << setw(5) <<  ME[z_side][z][y][x].predicted_not_rounded_match_trig_alct_delay << "    |" << endl;
	      break;
	    case (8):
	      if (display_text == 1) {
		cout << "cfeb_cable_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }	  
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].cfeb_cable_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_cfeb_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb_cable_delay, ME[z_side][z][y][x].predicted_cfeb_cable_delay, cfeb_cable_delay_thresh, z, y, x) 
		   << "      |" << endl;
	      break;
	    case (9):
	      if (display_text == 1) {
		cout << "alct_dav_cable_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].alct_dav_cable_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_alct_dav_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_dav_cable_delay, ME[z_side][z][y][x].predicted_alct_dav_cable_delay, alct_dav_cable_delay_thresh, z, y, x) 
		   << "      |" << endl;
	      break;
	    case (10):
	      if (display_text == 1) {
		cout << "cfeb_dav_cable_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].cfeb_dav_cable_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].cfeb_dav_cable_delay, ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay, cfeb_dav_cable_delay_thresh, z, y, x) 
		   << "      |" << endl;
	      break;
	    case (11):
	      cout << "            |  Measured  |  Predicted  |" << endl;
	      cout << "ME" << cz_side << z << "/" << y << "/" << x << " : " <<  endl;
	      for(w = 0; w < ME[z_side][z][y][x].afeb_fine_delay.size(); w++)
		cout << "            |   " << setw(2) << ME[z_side][z][y][x].afeb_fine_delay[w] << "    |    " << setw(2) << ME[z_side][z][y][x].init_afeb_fine_delay[w] << "    |" <<  endl;
	      break;
	    case (12):
	      if (display_text == 1) {
		cout << "match_trig_alct_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].match_trig_alct_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_match_trig_alct_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].match_trig_alct_delay, ME[z_side][z][y][x].predicted_match_trig_alct_delay, match_trig_alct_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (13):
	      if (display_text == 1) {
		cout << "tmb_lct_cable_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }	  
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].tmb_lct_cable_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_tmb_lct_cable_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].tmb_lct_cable_delay, ME[z_side][z][y][x].predicted_tmb_lct_cable_delay, tmb_lct_cable_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (14):
	      if (display_text == 1) {
		cout << "alct_l1a_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }	  
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].alct_l1a_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_alct_l1a_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].alct_l1a_delay, ME[z_side][z][y][x].predicted_alct_l1a_delay, alct_l1a_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (15):
	      if (display_text == 1) {
		cout << "tmb_l1a_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].tmb_l1a_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_tmb_l1a_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].tmb_l1a_delay, ME[z_side][z][y][x].predicted_tmb_l1a_delay, tmb_l1a_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (16):
	      if (display_text == 1) {
		cout << "mpc_tx_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].mpc_tx_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_mpc_tx_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].mpc_tx_delay, ME[z_side][z][y][x].predicted_mpc_tx_delay, mpc_tx_delay_thresh, z, y, x) << "      |" << endl;
	      break;
	    case (17):
	      if (display_text == 1) {
		cout << "mpc_rx_delay : " << endl;
		cout << "            |  Measured  |  Predicted  | Out Of Thr  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |    " << setw(3) << ME[z_side][z][y][x].mpc_rx_delay << "     |     " 
		   << setw(3) <<  ME[z_side][z][y][x].predicted_mpc_rx_delay << "     |      " 
		   << difference_beyond_thresh(ME[z_side][z][y][x].mpc_rx_delay, ME[z_side][z][y][x].predicted_mpc_rx_delay, mpc_rx_delay_thresh, z, y, x) << "      |" << endl; 
	      break;
	    case (26):
	      if (display_text == 1) {
		cout << "MPC alignment : " << endl;
		cout << "            |ALCT Skew(bx)| Max  AFEB(bx) |mpc_tx_delay |    Sum(bx)  |" << endl;
	      }
	      cout << "ME" << cz_side << z << "/" << y << "/" << setw(2) << x << " : |   " << setw(7) << ME[z_side][z][y][x].alct_skewclear_delay / 25.0 << "   |    " 
		   << setw(7) <<  ME[z_side][z][y][x].init_max_afeb_fine_delay * afeb_fine_delay_asic_nsec_per_setting/25 << "    | " 
		   << setw(7) <<  ME[z_side][z][y][x].predicted_mpc_tx_delay << "     |   "
		   << setw(7) <<  ME[z_side][z][y][x].alct_skewclear_delay / 25.0 + ME[z_side][z][y][x].init_max_afeb_fine_delay * afeb_fine_delay_asic_nsec_per_setting/25 + ME[z_side][z][y][x].predicted_mpc_tx_delay 
		   << "   |" << endl; 
	      break;
	    default:
	      break;
	    }
	    display_text = 0;
	  }
	}
      }
    }
  }
}
//
//---------------------------------------------------------------------------------------------------------
void select_chamber() {
  /*
    User can pick from one to all chambers allowed.  The selection is
    entered in as an array of char.  The form is ME-_/_/_ where the blanks (_) 
    will be filled in.  If an X or x is entered into a blank it means uses all
    allowed chambers.
    
    NOT SET UP TO HANDLE INCORRECT ENTRIES
  */
  
  char temp[9] = {};             //holds inputted value
  int z, y, x, z_side =0;                //loop variables
  bool ask_again = 1;
  bool good_value = 1;
  while (ask_again == 1) {
    
    set_range_to_all_chambers();
    number_of_chambers_of_interest = 0;
    
    
    cout << "Please enter the desired chambers, e.g. ME+1/2/4 (one) or MExX/X/X (all):" << endl;
    cout << "Enter X's to see all chambers :" << endl << endl;
    cout << "===> ME"; 
    cin >> temp;
    if (temp[0] == '-')
      side[0] = side[1] = 0;
    else if (temp[0] == '+')
      side[0] = side[1] = 1;
    else {
      side[0] = 0;
      side[1] = 1;
    }
    c[0] = (int) temp[1] - 48;  //converts to int
    c[1] = (int) temp[3] - 48;  //converts to int
    if (temp[6]){               //takes care of double digit chambers-------------
      c[2] = 10*((int) temp[5] - 48) + ((int) temp[6] - 48);
      temp[6] = 0;
    }
    else
      c[2] = (int) temp[5] - 48;//---------------------------------------------------------
    
    //determines which chambers are of interest to user --------------------------------------
    if ((temp[1] == 'X') || (temp[1] == 'x')) {
      cout << endl << endl <<"You have selected ME" << temp[0] << "X";
    }
    else {
      if ((c[0] >= chamber_limit[0][0][0]) && (c[0] <= chamber_limit[1][0][0])) {
	good_value = 1;
	chamber_limit[0][0][0] = c[0];
	chamber_limit[1][0][0] = c[0];
	cout << endl << endl << "You have selected ME" << temp[0] << c[0];
      }
      else {
	good_value = 0;
	cout << "JH DEBUG fail test 1" << endl;
      }
    }
    
    if ((temp[3] == 'X') || (temp[3] == 'x')) {
      cout << "/X";
    }
    else {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++) {
	if ((good_value = 1) && (c[1] >= chamber_limit[0][z][0]) && (c[1] <= chamber_limit[1][z][0])) {
	  good_value = 1;
	  chamber_limit[0][z][0] = c[1];
	  chamber_limit[1][z][0] = c[1];
	}
	else {
	  good_value = 0;
	  cout << "JH DEBUG fail test 2" << endl;
	}
      }
      if (good_value == 1)
	cout << "/" << c[1];
    }
    
    if ((temp[5] == 'X') || (temp[5] == 'x')) {
      cout << "/X" << endl << endl;
    }
    else {
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++) {
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  if ((good_value = 1) && (c[2] >= chamber_limit[0][z][y]) && (c[2] <= chamber_limit[1][z][y])) {
	    good_value = 1;
	    chamber_limit[0][z][y] = c[2];
	    chamber_limit[1][z][y] = c[2];
	  }
	  else {
	    good_value = 0;
	    cout << "JH DEBUG fail test 3" << endl;
	  }
	}
      }
      if (good_value == 1)
	cout << "/" << c[2] << endl << endl;
    }
    if (good_value == 1)
      ask_again = 0;
  }
  //calculates number of chambers user is interested in.  Value will be used in creating graphs-----
  for (z_side = side[0]; z_side <= side[1]; z_side++) {
    for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){       //ME number
      for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {    //n number
	for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {  //chamber number
	  if (((ME[z_side][z][y][x].has_measured_values == true) && (display_only_measured_chambers == true)) || (display_only_measured_chambers == false))
	    number_of_chambers_of_interest++;
	}
      }
    }
  }
}
//
//---------------------------------------------------------------------------------------------------------
void display_type() {
  /*
    Function which gives the option to display data as a graph or text.
    User enters y to display as a graph/histogram.
    Only y and n are accepted, any other character will cause the function to repeat
  */
  char key_hit = 0;
  while (1) {//asks user how he/she would like data displayed
    cout << endl << endl << "Would you like the data to be displayed as a graph? (y/n)" << endl << endl;
    cout << "===> ";
    cin >> key_hit;
    if ((key_hit == 'y') || (key_hit == 'Y')) {
      display_choice = 1;
      break;
    }
    else {
      if ((key_hit == 'n') || (key_hit == 'N')) {
	display_choice = 0;
	break;
      }
      else
	cout << endl << "Please enter y or n." << endl;
    }
  }
}
//
//---------------------------------------------------------------------------------------------------------
void display_TTCrxDelayCalc() {
  int number_of_crates[5] = {0,12,6,6,6};
  int x, y;
  int z_side;
  char cz_side;
  cout << "TTCrxCoarseDelay    : " << endl;
  cout << "              | Length(bx)   |  Delay(bx) | FineDel(ns) |  Sum (bx)   |" << endl;
  cz_side = '-';
  for (z_side = 0; z_side < 2; z_side++) {
    if (z_side == 1) cz_side = '+';
    for (x = 1; x <= 4; x++) {
      for (y = 1; y <= number_of_crates[x]; y++) {  
	if (fiber_length[z_side][x][y])
	  cout << "VME" << cz_side << "<" << x << "><" 
	       << setw(2) <<  y 
	       << "> : |  " << setw(7) << fiber_length[z_side][x][y] 
	       << "     |    " << setw(3) <<  TTCrxCoarseDelayPred[z_side][x][y] 
	       << "     |    " << setw(4)  <<  TTCrxFineDelayPred[z_side][x][y] 
	       << "     |    " << setw(7) << fiber_length[z_side][x][y] + double(TTCrxCoarseDelayPred[z_side][x][y])
	    +double(TTCrxFineDelayPred[z_side][x][y])/25
	       << "  |" <<  endl;
      }
    }
  }
}
//
//---------------------------------------------------------------------------------------------------------
void output_data_to_file() {
  /*
    Asks user to define file to store data in.
    Then stores predicted timing parameter data.
  */
  int x, y, z, w = 0;
  int z_side;
  string output_file_name;
  char output_choice = 'y';
  char cz_side = '-';
  int ask_again = 1;
  int number_of_output_parameters = 16;
  ofstream out_file;
  ifstream check_if_exists;
  while(ask_again) {
    cout << endl << "Would you like to define the file name which stores the predicted timing parameters? (y or n)" << endl;
    cout << "If not, the predicted timing parametrers will be stored in predicted_timing_parameters.txt" << endl << endl;
    cout << "FILE WILL BE OVERWRITTEN IF IT ALREADY EXISTS" << endl << endl;
    cout << "===> ";
    cin >> output_choice;
    if (output_choice == 'n' || output_choice == 'N') {
      output_file_name = "predicted_timing_parameters.txt";
      ask_again = 0;
    }
    else {
      if (output_choice == 'y' || output_choice == 'Y') {
	cout << endl << endl << "Please enter in desired file name with the correct extension (.txt, .xml, ...)" << endl << endl;
	cout << "===> ";
	cin >> output_file_name;
	cout << endl << endl << "The file will be named " << output_file_name << endl;
	ask_again = 0;
      }
      else {
	cout << endl << "Invalid input, please enter y or n" << endl;
	ask_again = 1;
      }
    }
  }
  out_file.open(output_file_name.c_str(), ios::out);
  cout << "  0% Written" << endl;
  out_file << "Predicted timing parameter values using..." << endl;
  out_file << "comp_timing\t\t" << comp_timing << endl;
  out_file << "clct_drift_delay\t\t" << clct_drift_delay << endl;
  out_file << "alct_drift_delay\t\t" << alct_drift_delay << endl;
  out_file << "match_trig_window_size\t\t" << match_trig_window_size << endl << endl;
  out_file << "For chambers with no afeb_fine_delay values, the prediction for match_trig_alct_delay, mpc_tx(rx)_delay, and tmb_l1a_delay is not accurate" << endl << endl;
  for (w = 1; w <= number_of_output_parameters; w++) {
    switch (w) {
    case (1):
      out_file << "Chamber                  " << "\t\t";
      break;
    case (2):
      out_file << "clct_skewclear_delay" << "\t\t";
      break;
    case (3):
      out_file << "alct_skewclear_delay" << "\t\t";
      break;
    case (4):
      out_file << "Predicted cfeb 0-4 delay" << "\t\t";
      break;
    case (5):
      out_file << "Predicted alct_tx_clock_delay" << "\t\t";
      break;
    case (6):
      out_file << "Predicted alct_rx_clock_delay" << "\t\t";
      break;
    case (7):
      out_file << "Predicted match_trig_alct_delay" << "\t\t";
      break;
    case (8):
      out_file << "Predicted mpc_tx_delay" << "\t\t";
      break;
    case (9):
      out_file << "Predicted mpc_rx_delay" << "\t\t";
      break;
    case (10):
      out_file << "Predicted tmb_lct_cable_delay" << "\t\t";
      break;
    case (11):
      out_file << "Predicted cfeb_cable_delay" << "\t\t";
      break;
    case (12):
      out_file << "Predicted cfeb_dav_cable_delay" << "\t\t";
      break;
    case (13):
      out_file << "Predicted alct_dav_cable_delay" << "\t\t";
      break;
    case (14):
      out_file << "Predicted tmb_l1a_delay" << "\t\t";
      break;
    case (15):
      out_file << "Predicted alct_l1a_delay" << "\t\t";
      break;
    case (16):
      out_file << "Chamber has afeb_fine_delays" << "\t\t";
    }
    for (z_side = side[0]; z_side <= side[1]; z_side++) { 
      //
      if (z_side == 0)
	cz_side = '-';
      else if (z_side == 1)
	cz_side = '+';
      //
      for (z = chamber_limit[0][0][0]; z <= chamber_limit[1][0][0]; z++){
	for (y = chamber_limit[0][z][0]; y <= chamber_limit[1][z][0]; y++) {
	  for (x = chamber_limit[0][z][y]; x <= chamber_limit[1][z][y]; x++) {    
	    switch (w){
	    case (1):
	      out_file << "ME" << cz_side << z << "/" << y << "/" << x << "\t";
	      break;
	    case (2):
	      out_file << ME[z_side][z][y][x].clct_skewclear_delay << "\t";
	      break;
	    case (3):
	      out_file << ME[z_side][z][y][x].alct_skewclear_delay << "\t";
	      break;
	    case (4):
	      out_file << ME[z_side][z][y][x].predicted_cfeb_delay << "\t";
	      break;
	    case (5):
	      out_file << ME[z_side][z][y][x].predicted_alct_tx_clock_delay << "\t";
	      break;
	    case (6):
	      out_file << ME[z_side][z][y][x].predicted_alct_rx_clock_delay << "\t";
	      break;
	    case (7):
	      out_file << ME[z_side][z][y][x].predicted_match_trig_alct_delay << "\t";
	      break;
	    case (8):
	      out_file << ME[z_side][z][y][x].predicted_mpc_tx_delay << "\t";
	      break;
	    case (9):
	      out_file << ME[z_side][z][y][x].predicted_mpc_rx_delay << "\t";
	      break;
	    case (10):
	      out_file << ME[z_side][z][y][x].predicted_tmb_lct_cable_delay << "\t";
	      break;
	    case (11):
	      out_file << ME[z_side][z][y][x].predicted_cfeb_cable_delay << "\t";
	      break;
	    case (12):
	      out_file << ME[z_side][z][y][x].predicted_cfeb_dav_cable_delay << "\t";
	      break;
	    case (13):
	      out_file << ME[z_side][z][y][x].predicted_alct_dav_cable_delay << "\t";
	      break;
	    case (14):
	      out_file << ME[z_side][z][y][x].predicted_tmb_l1a_delay << "\t";
	      break;
	    case (15):
	      out_file << ME[z_side][z][y][x].predicted_alct_l1a_delay << "\t";
	      break;
	    case (16):
	      if (ME[z_side][z][y][x].init_afeb_fine_delay.size() > 1)
		out_file << "YES" << "\t";
	      else
		out_file << "NO" << "\t";
	      
	    }
	  }
	}
      }
    }
    cout << setw(3) << (int) ((double) w/number_of_output_parameters * 100) << "% Written" << endl; 
    out_file << "\n";
  }
  out_file.close();
}
//
//-------------------------------------------------------
// internal setup functions...
//-------------------------------------------------------
void set_range_to_all_chambers () {     //defines all ME- chambers
  /*
    Sets the range of possible chambers to all chambers in ME-.  This
    is used when reading in cable values from cable_length.txt as well
    as can be used to define the largest range of user selectable chambers.
  */
  
  start_chamber[0][0] = chamber_limit[0][0][0] = 1;
  end_chamber[0][0]   = chamber_limit[1][0][0] = 4;
  
  start_chamber[1][0] = chamber_limit[0][1][0] = 1; // ME-1
  end_chamber[1][0]   = chamber_limit[1][1][0] = 3;
  start_chamber[2][0] = chamber_limit[0][2][0] = 1; // ME-2
  end_chamber[2][0]   = chamber_limit[1][2][0] = 2;
  start_chamber[3][0] = chamber_limit[0][3][0] = 1 ;// ME-3
  end_chamber[3][0]   = chamber_limit[1][3][0] = 2;
  start_chamber[4][0] = chamber_limit[0][4][0] = 1; // ME-4
  end_chamber[4][0]   = chamber_limit[1][4][0] = 1;
  
  start_chamber[1][1] = chamber_limit[0][1][1] = 1; // ME-1
  end_chamber[1][1] = chamber_limit[1][1][1] = 36;
  start_chamber[1][2] = chamber_limit[0][1][2] = 1;
  end_chamber[1][2] = chamber_limit[1][1][2] = 36;
  start_chamber[1][3] = chamber_limit[0][1][3] = 1;
  end_chamber[1][3] = chamber_limit[1][1][3] = 36;
  start_chamber[2][1] = chamber_limit[0][2][1] = 1; // ME-2
  end_chamber[2][1] = chamber_limit[1][2][1] = 18;
  start_chamber[2][2] = chamber_limit[0][2][2] = 1;
  end_chamber[2][2] = chamber_limit[1][2][2] = 36;
  start_chamber[3][1] = chamber_limit[0][3][1] = 1; // ME-3
  end_chamber[3][1] = chamber_limit[1][3][1] = 18;
  start_chamber[3][2] = chamber_limit[0][3][2] = 1;
  end_chamber[3][2] = chamber_limit[1][3][2] = 36;
  start_chamber[4][1] = chamber_limit[0][4][1] = 1; // ME-4
  end_chamber[4][1] = chamber_limit[1][4][1] = 18;
  
  // label for the Peripheral Crate is [station][ring][chamber]
  ChamberToPCMap[1][1][36] = 1;
  ChamberToPCMap[1][1][1]  = 1;
  ChamberToPCMap[1][1][2]  = 1;
  ChamberToPCMap[1][2][36] = 1;
  ChamberToPCMap[1][2][1]  = 1;
  ChamberToPCMap[1][2][2]  = 1;
  ChamberToPCMap[1][3][36] = 1;
  ChamberToPCMap[1][3][1]  = 1;
  ChamberToPCMap[1][3][2]  = 1;
  //
  ChamberToPCMap[1][1][3]  = 2;
  ChamberToPCMap[1][1][4]  = 2;
  ChamberToPCMap[1][1][5]  = 2;
  ChamberToPCMap[1][2][3]  = 2;
  ChamberToPCMap[1][2][4]  = 2;
  ChamberToPCMap[1][2][5]  = 2;
  ChamberToPCMap[1][3][3]  = 2;
  ChamberToPCMap[1][3][4]  = 2;
  ChamberToPCMap[1][3][5]  = 2;
  //
  ChamberToPCMap[1][1][6]  = 3;
  ChamberToPCMap[1][1][7]  = 3;
  ChamberToPCMap[1][1][8]  = 3;
  ChamberToPCMap[1][2][6]  = 3;
  ChamberToPCMap[1][2][7]  = 3;
  ChamberToPCMap[1][2][8]  = 3;
  ChamberToPCMap[1][3][6]  = 3;
  ChamberToPCMap[1][3][7]  = 3;
  ChamberToPCMap[1][3][8]  = 3;
  //
  ChamberToPCMap[1][1][9]  = 4;
  ChamberToPCMap[1][1][10] = 4;
  ChamberToPCMap[1][1][11] = 4;
  ChamberToPCMap[1][2][9]  = 4;
  ChamberToPCMap[1][2][10] = 4;
  ChamberToPCMap[1][2][11] = 4;
  ChamberToPCMap[1][3][9]  = 4;
  ChamberToPCMap[1][3][10] = 4;
  ChamberToPCMap[1][3][11] = 4;
  //
  ChamberToPCMap[1][1][12] = 5;
  ChamberToPCMap[1][1][13] = 5;
  ChamberToPCMap[1][1][14] = 5;
  ChamberToPCMap[1][2][12] = 5;
  ChamberToPCMap[1][2][13] = 5;
  ChamberToPCMap[1][2][14] = 5;
  ChamberToPCMap[1][3][12] = 5;
  ChamberToPCMap[1][3][13] = 5;
  ChamberToPCMap[1][3][14] = 5;
  //
  ChamberToPCMap[1][1][15] = 6;
  ChamberToPCMap[1][1][16] = 6;
  ChamberToPCMap[1][1][17] = 6;
  ChamberToPCMap[1][2][15] = 6;
  ChamberToPCMap[1][2][16] = 6;
  ChamberToPCMap[1][2][17] = 6;
  ChamberToPCMap[1][3][15] = 6;
  ChamberToPCMap[1][3][16] = 6;
  ChamberToPCMap[1][3][17] = 6;
  //
  ChamberToPCMap[1][1][18] = 7;
  ChamberToPCMap[1][1][19] = 7;
  ChamberToPCMap[1][1][20] = 7;
  ChamberToPCMap[1][2][18] = 7;
  ChamberToPCMap[1][2][19] = 7;
  ChamberToPCMap[1][2][20] = 7;
  ChamberToPCMap[1][3][18] = 7;
  ChamberToPCMap[1][3][19] = 7;
  ChamberToPCMap[1][3][20] = 7;
  //
  ChamberToPCMap[1][1][21] = 8;
  ChamberToPCMap[1][1][22] = 8;
  ChamberToPCMap[1][1][23] = 8;
  ChamberToPCMap[1][2][21] = 8;
  ChamberToPCMap[1][2][22] = 8;
  ChamberToPCMap[1][2][23] = 8;
  ChamberToPCMap[1][3][21] = 8;
  ChamberToPCMap[1][3][22] = 8;
  ChamberToPCMap[1][3][23] = 8;
  //
  ChamberToPCMap[1][1][24] = 9;
  ChamberToPCMap[1][1][25] = 9;
  ChamberToPCMap[1][1][26] = 9;
  ChamberToPCMap[1][2][24] = 9;
  ChamberToPCMap[1][2][25] = 9;
  ChamberToPCMap[1][2][26] = 9;
  ChamberToPCMap[1][3][24] = 9;
  ChamberToPCMap[1][3][25] = 9;
  ChamberToPCMap[1][3][26] = 9;
  //
  ChamberToPCMap[1][1][27] = 10;
  ChamberToPCMap[1][1][28] = 10;
  ChamberToPCMap[1][1][29] = 10;
  ChamberToPCMap[1][2][27] = 10;
  ChamberToPCMap[1][2][28] = 10;
  ChamberToPCMap[1][2][29] = 10;
  ChamberToPCMap[1][3][27] = 10;
  ChamberToPCMap[1][3][28] = 10;
  ChamberToPCMap[1][3][29] = 10;
  //
  ChamberToPCMap[1][1][30] = 11;
  ChamberToPCMap[1][1][31] = 11;
  ChamberToPCMap[1][1][32] = 11;
  ChamberToPCMap[1][2][30] = 11;
  ChamberToPCMap[1][2][31] = 11;
  ChamberToPCMap[1][2][32] = 11;
  ChamberToPCMap[1][3][30] = 11;
  ChamberToPCMap[1][3][31] = 11;
  ChamberToPCMap[1][3][32] = 11;
  //
  ChamberToPCMap[1][1][33] = 12;
  ChamberToPCMap[1][1][34] = 12;
  ChamberToPCMap[1][1][35] = 12;
  ChamberToPCMap[1][2][33] = 12;
  ChamberToPCMap[1][2][34] = 12;
  ChamberToPCMap[1][2][35] = 12;
  ChamberToPCMap[1][3][33] = 12;
  ChamberToPCMap[1][3][34] = 12;
  ChamberToPCMap[1][3][35] = 12;
  //
  for (int station=2; station<=4; station++) {
    //
    ChamberToPCMap[station][1][2]  = 1;
    ChamberToPCMap[station][1][3]  = 1;
    ChamberToPCMap[station][1][4]  = 1;
    ChamberToPCMap[station][2][3]  = 1;
    ChamberToPCMap[station][2][4]  = 1;
    ChamberToPCMap[station][2][5]  = 1;
    ChamberToPCMap[station][2][6]  = 1;
    ChamberToPCMap[station][2][7]  = 1;
    ChamberToPCMap[station][2][8]  = 1;
    //
    ChamberToPCMap[station][1][5]  = 2;
    ChamberToPCMap[station][1][6]  = 2;
    ChamberToPCMap[station][1][7]  = 2;
    ChamberToPCMap[station][2][9]  = 2;
    ChamberToPCMap[station][2][10] = 2;
    ChamberToPCMap[station][2][11] = 2;
    ChamberToPCMap[station][2][12] = 2;
    ChamberToPCMap[station][2][13] = 2;
    ChamberToPCMap[station][2][14] = 2;
    //
    ChamberToPCMap[station][1][8]  = 3;
    ChamberToPCMap[station][1][9]  = 3;
    ChamberToPCMap[station][1][10] = 3;
    ChamberToPCMap[station][2][15] = 3;
    ChamberToPCMap[station][2][16] = 3;
    ChamberToPCMap[station][2][17] = 3;
    ChamberToPCMap[station][2][18] = 3;
    ChamberToPCMap[station][2][19] = 3;
    ChamberToPCMap[station][2][20] = 3;
    //
    ChamberToPCMap[station][1][11] = 4;
    ChamberToPCMap[station][1][12] = 4;
    ChamberToPCMap[station][1][13] = 4;
    ChamberToPCMap[station][2][21] = 4;
    ChamberToPCMap[station][2][22] = 4;
    ChamberToPCMap[station][2][23] = 4;
    ChamberToPCMap[station][2][24] = 4;
    ChamberToPCMap[station][2][25] = 4;
    ChamberToPCMap[station][2][26] = 4;
    //
    ChamberToPCMap[station][1][14] = 5;
    ChamberToPCMap[station][1][15] = 5;
    ChamberToPCMap[station][1][16] = 5;
    ChamberToPCMap[station][2][27] = 5;
    ChamberToPCMap[station][2][28] = 5;
    ChamberToPCMap[station][2][29] = 5;
    ChamberToPCMap[station][2][30] = 5;
    ChamberToPCMap[station][2][31] = 5;
    ChamberToPCMap[station][2][32] = 5;
    //
    ChamberToPCMap[station][1][17] = 6;
    ChamberToPCMap[station][1][18] = 6;
    ChamberToPCMap[station][1][1]  = 6;
    ChamberToPCMap[station][2][33] = 6;
    ChamberToPCMap[station][2][34] = 6;
    ChamberToPCMap[station][2][35] = 6;
    ChamberToPCMap[station][2][36] = 6;
    ChamberToPCMap[station][2][1]  = 6;
    ChamberToPCMap[station][2][2]  = 6;
    //
  }
}
