#ifndef _Test_CFEB04_h_
#define _Test_CFEB04_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <vector>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

template< typename T >
std::vector< T > operator*( const std::vector< std::vector<T> >& M, const std::vector< T >& v ){
  if ( M.at( 0 ).size() != v.size() ) throw std::logic_error( "Cannot multiply matrix and vector of unmatched size." );
  typename std::vector< T > product( M.size(), 0 );
  for ( size_t i=0; i<M.size(); ++i ){
    T sum = 0;
    for ( size_t j=0; j<v.size(); ++j ) sum += M[i][j] * v[j];
    product[i] = sum;
  }
  return product;
}

#define MAX_VALID_SLOPE 12.
#define MIN_VALID_SLOPE 4.

class Test_CFEB04: public Test_Generic
{
public:
  Test_CFEB04(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
//	void finish();

protected:

  struct extremum_t {
    double x;
    double y;
    double x_corr;
    double y_corr;
  };

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  extremum_t correctedPulsePeak(vector<double>& points) const;
  extremum_t findMaximum( vector<double>& v ) const;
  std::vector< std::vector<double> > invert3x3Matrix( const std::vector< std::vector<double> >& m ) const;
  bool checkResults(std::string cscID);
  double fivePoleFitTime( int tmax,  double* adc, double t_peak);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<std::string, test_step> test_steps;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  std::map<int, ddu_stats> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  cscGainData gdata;
  cscTPeakData tpdata;	// Pulse Peaking Time 

};

#endif
