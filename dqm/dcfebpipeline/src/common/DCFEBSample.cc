#include "emu/dqm/dcfebpipeline/DCFEBSample.h"

#include <algorithm>

using namespace std;

DCFEBSampleId::DCFEBSampleId( unsigned int cfeb_number, unsigned int depth )
  : cfeb( cfeb_number )
  , pipelineDepth( depth ){}

// bool DCFEBSampleId::operator<( const DCFEBSampleId& other ) const {
//   if      ( crate <  other.crate ) return true;
//   else if ( crate == other.crate ){
//     if      ( slot <  other.slot ) return true;
//     else if ( slot == other.slot ){
//       if      ( cfeb <  other.cfeb ) return true;
//       else if ( cfeb == other.cfeb ) return pipelineDepth < other.pipelineDepth;
//     }
//   }
//   return false;
// }

bool DCFEBSampleId::operator<( const DCFEBSampleId& other ) const {
  if      ( cfeb <  other.cfeb ) return true;
  else if ( cfeb == other.cfeb ) return pipelineDepth < other.pipelineDepth;
  return false;
}

DCFEBSample::DCFEBSample()
  : nSamples_( 0 ){}

DCFEBSample::DCFEBSample( const vector<int>& sample )
  : nSamples_( 0 ){
  add( sample );
}

DCFEBSample& DCFEBSample::add( const vector<int>& sample ){
  if ( sample_.size() < sample.size() ) sample_.resize( sample.size() );
  int minimum = *min_element( sample .begin(), sample.end() );
  vector<int>::iterator        i = sample_.begin();
  vector<int>::const_iterator ci = sample .begin();
  while( i != sample_.end() && ci != sample.end() ){
    *i += *ci - minimum; // subtract pedestal
    ++i;
    ++ci;
  }
  nSamples_++;
  return *this;
}

vector<double> DCFEBSample::getAverageSample() const {
  vector<double> average( sample_.size() );
  if ( nSamples_ == 0 ) return average;
  vector<double>::iterator    ia = average.begin();
  vector<int>::const_iterator is = sample_.begin();
  while( is != sample_.end() && ia != average.end() ){
    *ia = double( *is ) / nSamples_;
    ++ia;
    ++is;
  }
  return average;
}
