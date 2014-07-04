#ifndef __PipelineAnalyzer_h__
#define __PipelineAnalyzer_h__

#include "emu/dqm/dcfebpipeline/DCFEBSample.h"
#include "emu/dqm/dcfebpipeline/PipelineDepthFinder.h"

#include "EventFilter/CSCRawToDigi/interface/CSCDCCExaminer.h"

#include <sstream>

/// Convert type T to binary number in ASCI string.
///
/// @param t Number of type T.
///
/// @return  Binary number in ASCI string.
///
template <typename T>
std::string binaryFrom( const T& t ){
  std::stringstream ss;
  const int nBits( sizeof( T ) * 8 );
  const T mask( 1 );
  for ( int i=nBits-1; i>=0; --i ) ss << ((t>>i) & mask);
  return ss.str();
}

/// Convert binary number in ASCI string to type T.
/// Spaces, underscores, etc. can be used, it ignores everything that is not '0' or '1'.
///
/// @param s Binary number in ASCI string.
///
/// @return  Number of type T.
///
template <typename T>
T binaryTo( const std::string s )
{
  T t( 0 );
  const T one( 1 );
  const size_t nBits( sizeof( T ) * 8 );
  size_t iBit( 0 );
  for ( std::string::const_reverse_iterator i=s.rbegin(); i!=s.rend() && iBit<nBits; ++i ){
    if      ( *i == '0' ) ++iBit;
    else if ( *i == '1' ){
      t |= one<<iBit;
      ++iBit;
    }
  }
  return t;
}

using namespace std;

class PipelineAnalyzer{
public:
  PipelineAnalyzer( const string& dataDir );
private:
  void analyze();
  /// Unpack ODMB DDU tx FIFO data files, and return the DCFEB ADC counts.
  ///
  /// @param fileNames Key: set pipeline depth. Value: files containing the data from the ODMB DDU tx FIFO taken at this pipeline depth.
  ///
  /// @return The DCFEB ADC counts
  ///
  map<DCFEBSampleId,DCFEBSample> getADCCounts( const multimap<unsigned int, string>& fileNames );
  vector<string> execShellCommand( const string &shellCommand ) const;
  void openResults();
  void addToResults( const unsigned int crate, const unsigned int slot, const unsigned int cfeb, 
			 const double depth );
  void closeResults();
  string dataDir_;
  ofstream logFile_;
  ofstream resultsTextFile_;
  ofstream resultsHTMLFile_;
  ofstream resultsXSLTFile_;
  static const ExaminerMaskType examinerMask_;
  static const unsigned int maxCFEBs_ = 7; ///< Maximum number of CFEBs per DMB.
  static const unsigned int maxDMBs_ = 10; ///< Maximum number of DMBs per peripheral crate.
};

#endif
