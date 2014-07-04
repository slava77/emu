#ifndef __PipelineDepthFinder_h__
#define __PipelineDepthFinder_h__

#include "TFile.h"

#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>    // std::out_of_range
#include <numeric>      // std::accumulate
#include <cmath>        // fabs
#include <limits>       // std::numeric_limits
#include <fstream>

template< typename T >
std::ostream& operator<<(std::ostream& os, const std::vector< T >& t)
{
  os << "[";
  typename std::vector< T >::const_iterator it;
  for (it = t.begin(); it != t.end(); ++it)
    os << *it << (it + 1 != t.end() ? "," : "");
  os << "]";
  return os;
}

template< typename T >
std::ostream& operator<<(std::ostream& os, std::vector< T >& t)
{
  os << "[";
  typename std::vector< T >::iterator it;
  for (it = t.begin(); it != t.end(); ++it)
    os << *it << (it + 1 != t.end() ? "," : "");
  os << "]";
  return os;
}

template< typename T1, typename T2 >
std::ostream& operator<<(std::ostream& os, const std::map< T1, T2 >& t)
{
  os << "(";
  if ( t.size() )
    {
      typename std::map< T1, T2 >::const_iterator it, iti;
      for (it = iti = t.begin(), ++iti; it != t.end(); ++it, ++iti)
        os << it->first << "->" << it->second << (iti != t.end() ? "," : "");
    }
  os << ")";
  return os;
}

template< typename T >
std::vector< T > operator+( const std::vector< T >& v1, const std::vector< T >& v2 )
{
  if ( v1.size() != v2.size() ) throw std::logic_error( "Cannot add vectors of unuqual sizes." );
  std::vector< T > sum( v1 );
  typename std::vector< T >::iterator isum = sum.begin();
  typename std::vector< T >::const_iterator iv2  = v2 .begin();
  while ( isum != v1.end() ){
    *isum += *iv2;
    ++isum;
    ++ iv2;
  }
  return sum;
}

template< typename T >
T normalize( std::vector< T >& v ){
  T sum = std::accumulate( v.begin(), v.end(), T(0) );
  if ( std::fabs( sum ) <=  std::numeric_limits<T>::epsilon() ) return T(0);
  for ( typename std::vector< T >::iterator i=v.begin(); i!=v.end(); ++i ) *i /= sum;
  return sum;
}

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

using namespace std;

class PipelineDepthFinder{
public:
  PipelineDepthFinder();
  ~PipelineDepthFinder();
  PipelineDepthFinder& setId( const unsigned int crate, const unsigned int slot, const unsigned int cfeb );
  PipelineDepthFinder& setLogDir( const string& logDir );
  PipelineDepthFinder& addDepth( const size_t depth, const size_t nSamples, const vector<double>& measurement );
  double getBestDepth();
  double getBestDepthA();
  double getBestDepthB();
  vector< vector<double> > invert3x3Matrix( const vector< vector<double> >& m ) const;

private:
  void openLogFiles();

  static const size_t maxDepth_ = 511; ///< Maximum pipeline depth.
  static const size_t maxMeasurementSize_ = 128; ///< Maximum number of time samples.

  static const size_t referencePulseLength_ = 8;	///< Number of time samples for the reference pulse.
  static const int    referencePulse_[referencePulseLength_]; ///< ADC counts of a reference pulse to compare the measurements to.
  vector<double>      referenceMeasurement_; ///< Normalized ADC counts of a reference pulse.
  const double        referenceLandauSigma_; ///< Sigma parameter of the typical (reference) pulse signal.
  const double        referenceNoise_; ///< The std dev of the pedestal.
  const double        referencePeakTime_; ///< The ideally timed pulse should peak at this time sample.
  
  unsigned int crate_;		///< Crate number.
  unsigned int slot_;		///< DMB slot.
  unsigned int cfeb_;		///< CFEB number.
  size_t measurementSize_;	///< Number of time samples in a measurement.
  map< int, vector<double> > measurements_; ///< Set pipeline depth --> Average ADC counts.
  map< int, size_t > nSamples_; ///< Set pipeline depth --> Number of samples that were averaged.
  string   logDir_;
  ofstream logFile_;
  TFile *histogramFile_;
};

#endif
