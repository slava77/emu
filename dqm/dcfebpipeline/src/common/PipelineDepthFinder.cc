#include "emu/dqm/dcfebpipeline/PipelineDepthFinder.h"

#include "TMath.h"

#include "emu/dqm/dcfebpipeline/STL_ROOT.h"

#include <sstream>
#include <iomanip>

PipelineDepthFinder::PipelineDepthFinder()
  : referenceMeasurement_( vector<double>( referencePulseLength_, 0. ) )
  , referenceLandauSigma_( 120. )
  , referenceNoise_( 3. )
  , referencePeakTime_( 3.5 )
  , crate_( 0 )
  , slot_ ( 0 )
  , cfeb_ ( 0 )
  , measurementSize_( referencePulseLength_ )
{
  if ( measurementSize_ > maxMeasurementSize_ ) throw std::out_of_range( "Sample size is greater than max sample size." );

  // Initialize reference measurement with reference pulse and normalize it:
  std::copy( referencePulse_, referencePulse_ + referencePulseLength_, referenceMeasurement_.begin() );
  normalize<double>( referenceMeasurement_ );
}

PipelineDepthFinder::~PipelineDepthFinder(){
  if ( logFile_.is_open() ) logFile_.close();
  histogramFile_->Close();
  delete histogramFile_;
}

PipelineDepthFinder& PipelineDepthFinder::setId( const unsigned int crate, const unsigned int slot, const unsigned int cfeb ){
  crate_ = crate;
  slot_  = slot ;
  cfeb_  = cfeb ;
  openLogFiles();
  return *this;
}

PipelineDepthFinder& PipelineDepthFinder::setLogDir( const string& logDir ){
  logDir_ = logDir;
  openLogFiles();
  return *this;
}

void PipelineDepthFinder::openLogFiles(){
  if ( logFile_.is_open() ) return;
  if ( slot_ == 0 ) return;
  if ( logDir_.length() == 0 ) return;
  ostringstream fileName;
  fileName << logDir_ << "/"
	   << "crate" << setw(2) << setfill('0') << fixed << crate_
	   << "_slot" << setw(2) << setfill('0') << fixed << slot_
	   << "_cfeb" << setw(1) << setfill('0') << fixed << cfeb_;
  logFile_.open( (fileName.str() + ".log").c_str(), ofstream::trunc );
  histogramFile_ = new TFile( (fileName.str() + ".root").c_str(), "RECREATE", fileName.str().c_str(), 9 );
}

PipelineDepthFinder& PipelineDepthFinder::addDepth( const size_t depth, const size_t nSamples, const vector<double>& measurement ){
  if ( depth > maxDepth_ ) throw std::out_of_range( "Specified pipeline depth is greater than max pipeline depth." );
  if ( measurement.size() != measurementSize_ ) throw std::length_error( "Sample size mismatch." );
  map< int, vector<double> >::iterator m = measurements_.find( depth );
  if ( m == measurements_.end() ){
    measurements_[depth] = measurement;
    nSamples_[depth] = nSamples;
  }
  else{
    // m->second = m->second + measurement;
    // A second measurement should not be added as what we already have is an average.
    ostringstream oss;
    oss << "Measurement for pipeline depth " << depth <<  " has already been added.";
    throw std::runtime_error( oss.str() );
  }
  logFile_ << "Added crate "  << crate_
	   << "  dmb slot"    << slot_
	   << "  cfeb "       << cfeb_
	   << "  depth "      << depth 
	   << "  ADC counts " << measurements_[depth] << endl;
  return *this;
}

double PipelineDepthFinder::getBestDepth(){
  return getBestDepthA();
}

double PipelineDepthFinder::getBestDepthA(){
  double bestDepth = -1.;

  if ( measurements_.size() == 0 ){
    logFile_ << "No data." << endl;
    return bestDepth;
  }

  ostringstream oss;
  oss << "crate "    << crate_
      << " dmb slot " << slot_
      << " cfeb "     << cfeb_;

  int nBinsX   = measurements_.rbegin()->first - measurements_.begin()->first + 1;
  double loX   = measurements_. begin()->first - 0.5; 
  double hiX   = measurements_.rbegin()->first + 0.5; 
  TH1D h_maxima( "maxima", (string("maxima from ") + oss.str()).c_str(), nBinsX, loX, hiX );
  h_maxima.SetXTitle( "pipeline depth" );
  h_maxima.SetYTitle( "maximum of average peak" );
  h_maxima.SetStats( kFALSE );
  TH1D h_intercepts( "intercepts", (string("maxima-depth from ") + oss.str()).c_str(), nBinsX, loX, hiX );
  h_intercepts.SetXTitle( "pipeline depth" );
  h_intercepts.SetYTitle( "intercept of max vs pipeline depth" );
  h_intercepts.SetStats( kFALSE );
  
  //
  // Find maximum at each pipeline depth
  //
  map< int, double > maxima; // Set pipeline depth --> Location of fitted maximum of average ADC counts
  map< int, double > nSamples; // Set pipeline depth --> number of samples that went into calculating the average ADC counts
  map< int, vector<double> >::const_iterator m = measurements_.begin();
  map< int, size_t >        ::const_iterator n = nSamples_    .begin();
  while ( m != measurements_.end() && n != nSamples_.end() ){

    // The sum of all samples will have a noise of sqrt( number_of_samples ) * single_sample_noise.
    // Since we subtract the minimal value of the samples as pedestal, the reference line drops about
    // 1.3 * noise below the mean (for number of samples=8). Let's discard samples whose integral is below pedestal + 5 * noise.
    const double minSumOfADCCounts = (1.3+5.)*TMath::Sqrt(referencePulseLength_)*referenceNoise_;
    if ( std::accumulate( m->second.begin(), m->second.end(), 0. ) > minSumOfADCCounts ){

      // First find the max average ADC count
      vector<double>::const_iterator iMax  = max_element( m->second.begin(), m->second.end() );
      double timeAtMax = double( iMax - m->second.begin() );
      logFile_ << "crate "    << crate_
	       << " dmb slot " << slot_
	       << " cfeb "     << cfeb_
	       << " depth "    << m->first
	       << " ADC "      << m->second
	       << " iMax "     << *iMax << "\n";
      vector<double>::const_iterator iNext = iMax;
      ++iNext;
      // Refine location of maximum (if maximum is not at the edge) by fitting a parabola to the maximum and its two neighbors
      if ( iMax != m->second.end() && iMax != m->second.begin() && iNext != m->second.end() ){
	// Sum_(i=0..3) p_i x^i = y_i
	// V * p = y   ==>  p = V^-1 * y
	// Maximum of parabola at p_1 / p_2 / 2
	vector<double> x( 3 );
	vector<double> y( 3 );
	vector< vector<double> > V( 3, vector<double>( 3, 0. ) ); // Vandermonde matrix
	x[2] = timeAtMax+1.; y[2] = *iNext;
	x[1] = timeAtMax   ; y[1] = *iMax ;
	--iMax; // from here on, iMax is the left neighbor of the maximum
	x[0] = timeAtMax-1.; y[0] = *iMax ;
	for ( size_t i=0; i<3; ++i ){
	  V[i][0] =        1.;
	  V[i][1] =      x[i];
	  V[i][2] = x[i]*x[i];
	}
	vector<double> p( invert3x3Matrix( V ) * y );
	if ( fabs( p[2] ) > std::numeric_limits<double>::epsilon() ){
	  maxima  [m->first] = - p[1]/(2*p[2]);
	  nSamples[m->first] = n->second;
	  int iBin = h_maxima.GetXaxis()->FindBin( m->first );
	  // Errors are relative only, just to show which point had what weight in the fit.
	  h_maxima.SetBinContent( iBin, maxima[m->first] );
	  h_maxima.SetBinError( iBin, n->second > 0 ? 1. / sqrt( (double) n->second ) : 1. );
	  h_intercepts.SetBinContent( iBin, maxima[m->first] - m->first );
	  h_intercepts.SetBinError( iBin, n->second > 0 ? 1./ sqrt( (double) n->second ) : 1. );
	}
	logFile_ << "p " << p
		 << " max="            << maxima[m->first]
		 << "    sample size=" << n->second << "\n";
      }
    }
    ++m;
    ++n;
  }
  h_maxima.Write();
  h_intercepts.Write();

  //
  // Fit the intercept (1D fit) in timeOfPeak = intercept + 1. * pipelineDepth
  //
  double sum_xw = 0., sum_w = 0.;
  map< int, double >::const_iterator x = maxima.begin();
  map< int, double >::const_iterator w = nSamples.begin();
  while( x != maxima.end() && w != nSamples.end() ){
    sum_xw += w->second * ( x->second - x->first );
    sum_w  += w->second;
    ++x;
    ++w;
  }
  if ( isnan( sum_xw ) || isnan( sum_w ) || sum_w <= 0. ){
    logFile_ << "Failed to fit best depth.\n";
    return bestDepth;
  }
  double intercept( sum_xw / sum_w );
  // timeOfPeak = intercept + 1. * pipelineDepth
  // Find the pipeline depth at which the pulse peaks at the ideal time referencePeakTime_.
  bestDepth = referencePeakTime_ - intercept;
  logFile_ << "intercept " << intercept 
	   << "   best depth " << bestDepth
	   << "\n";

  return bestDepth;
}

double PipelineDepthFinder::getBestDepthB(){
  double bestDepth = -1.;

  if ( measurements_.size() == 0 ){
    logFile_ << "No data." << endl;
    return bestDepth;
  }

  ostringstream oss;
  oss << "crate "    << crate_
      << " dmb slot " << slot_
      << " cfeb "     << cfeb_;

  int nBinsX   = measurements_.rbegin()->first - measurements_.begin()->first + 1;
  double loX   = measurements_. begin()->first - 0.5; 
  double hiX   = measurements_.rbegin()->first + 0.5; 
  TH1D h_chi2( "chi2", (string("chi2 from ") + oss.str()).c_str(), nBinsX, loX, hiX );
  h_chi2.SetXTitle( "pipeline depth" );
  h_chi2.SetYTitle( "#chi^{2}" );
  h_chi2.SetStats( kFALSE );
  TH1D h_nSample( "nSamples", (string("Number of samples from ") + oss.str()).c_str(), nBinsX, loX, hiX );
  h_nSample.SetXTitle( "pipeline depth" );
  h_nSample.SetYTitle( "samples" );
  h_nSample.SetStats( kFALSE );
  // TH1D h_pKol( "pKol", "pKol", nBinsX, loX, hiX );
  
  int nBinsY   = measurements_.rbegin()->second.size();
  double loY   =                                       + 0.5;
  double hiY   = measurements_.rbegin()->second.size() + 0.5;
  TH2D h_AverADC( "averageADC"   , (string("Average ADC from "    ) + oss.str()).c_str(), nBinsX, loX, hiX, nBinsY, loY, hiY );
  TH2D h_NormADC( "normalizedADC", (string("Normalized ADC from " ) + oss.str()).c_str(), nBinsX, loX, hiX, nBinsY, loY, hiY );
  h_AverADC.SetXTitle( "pipeline depth" );
  h_AverADC.SetYTitle( "time sample" );
  h_AverADC.SetZTitle( "average ADC count" );
  h_AverADC.SetStats( kFALSE );
  h_NormADC.SetXTitle( "pipeline depth" );
  h_NormADC.SetYTitle( "time sample" );
  h_NormADC.SetZTitle( "normalized ADC count" );
  h_NormADC.SetStats( kFALSE );

  map< int, double > chi2s;
  int depthAtMin = -1;
  double minChi2 = std::numeric_limits<double>::max();
  map< int, vector<double> >::const_iterator dm = measurements_.begin();
  map< int, size_t         >::const_iterator dn = nSamples_    .begin();
  while ( dm!=measurements_.end() && dn!=nSamples_.end() ){
    vector<double> normalizedMeasurement( dm->second );
    normalize<double>( normalizedMeasurement );

    // TH1D h_r( "h_r", "h_r", nBinsY, loY, hiY );
    // TH1D h_m( "h_m", "h_m", nBinsY, loY, hiY );
    // TH1From<vector<double>::const_iterator,TH1D>( referenceMeasurement_.begin(), referenceMeasurement_.end(), h_r, 1 );
    // TH1From<vector<double>::iterator,TH1D>( normalizedMeasurement.begin(), normalizedMeasurement.end(), h_m, 1 );
    // h_pKol.Fill( dm->first, h_r.KolmogorovTest( &h_m, "M" ) );

    h_nSample.Fill( dn->first, dn->second );

    double chi2 = 0.;
    vector<double>::const_iterator r = referenceMeasurement_.begin();
    vector<double>::const_iterator m = normalizedMeasurement.begin();
    while ( r != referenceMeasurement_.end() && m != normalizedMeasurement.end() ){
      vector<double>::difference_type i = m-normalizedMeasurement.begin();
      chi2 += ( *m - *r ) * ( *m - *r );
      h_AverADC.Fill( dm->first, i+1, dm->second.at( i ) );
      h_NormADC.Fill( dm->first, i+1, *m                 );
      ++r;
      ++m;
    }
    // Try ROOT's chi2 test:
    // chi2 = h_m.Chi2Test( &h_r, "WW CHI2" );
    
    // Penalize low-ADC-count samples
    // The sum of all samples will have a noise of sqrt( number_of_samples ) * single_sample_noise.
    // Since we subtract the minimal value of the samples as pedestal, the reference line drops about
    // 1.3 * noise below the mean (for number of samples=8). Let's start penalizing 5 * noise above the mean, 
    // adding one unit of chi2 with every multiple of the reference Landau's sigma by which 
    // the sum of all samples falls below this limit:
    double sumOfADCCounts = std::accumulate( dm->second.begin(), dm->second.end(), 0. );
    chi2 += std::max( 0., 
		      ( (1.3+5.)*TMath::Sqrt(referencePulseLength_)*referenceNoise_ - sumOfADCCounts ) 
		      / referenceLandauSigma_ );

    if ( chi2 < minChi2 ){
      minChi2 = chi2;
      depthAtMin = dm->first;
    }

    chi2s[dm->first] = chi2;
    h_chi2.Fill( dm->first, chi2 );

    ++dm;
    ++dn;
  }
  logFile_ << "depth->chi2: " << chi2s << endl;
  h_chi2.Write();
  // h_pKol.Write();
  h_nSample.Write();
  h_AverADC.Write();
  h_NormADC.Write();

  // Refine locataion of minimum (if minimum is not at the edge) by fitting a parabola to the minimum and its two neighbors
  map<int,double>::iterator iMin  = chi2s.find( depthAtMin );
  map<int,double>::iterator iNext = iMin;
  ++iNext;
  if ( iMin != chi2s.end() && iMin != chi2s.begin() && iNext != chi2s.end() ){
    // Sum_(i=0..3) p_i x^i = y_i
    // V * p = y   ==>  p = V^-1 * y
    // Minimum of parabola at p_1 / p_2 / 2
    vector<double> x( 3 );
    vector<double> y( 3 );
    vector< vector<double> > V( 3, vector<double>( 3, 0. ) ); // Vandermonde matrix
    x[2] = iNext->first; y[2] = iNext->second;
    x[1] = iMin ->first; y[1] = iMin ->second;
    --iMin; // from here on, iMin is the left neighbor of the minimum
    x[0] = iMin ->first; y[0] = iMin ->second;
    for ( size_t i=0; i<3; ++i ){
      V[i][0] =        1.;
      V[i][1] =      x[i];
      V[i][2] = x[i]*x[i];
    }
    vector<double> p( invert3x3Matrix( V ) * y );
    if ( fabs( p[2] ) > std::numeric_limits<double>::epsilon() ) return - p[1]/(2*p[2]);
  }
  logFile_ << "Refinement of minimum failed." << endl;
  return bestDepth;
}

vector< vector<double> > PipelineDepthFinder::invert3x3Matrix( const vector< vector<double> >& m ) const {
  vector< vector<double> > inverse( 3, vector<double>( 3, 0. ) );
  double det = 
    + m[0][0] * ( m[1][1]*m[2][2] - m[1][2]*m[2][1] )
    + m[0][1] * ( m[1][2]*m[2][0] - m[1][0]*m[2][2] )
    + m[0][2] * ( m[1][0]*m[2][1] - m[1][1]*m[2][0] );

  if ( fabs( det ) < std::numeric_limits<double>::epsilon() ) return inverse;

  inverse[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1]; inverse[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2]; inverse[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
  inverse[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2]; inverse[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0]; inverse[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
  inverse[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0]; inverse[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1]; inverse[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

  for ( int i=0; i<3; ++i ) for ( int j=0; j<3; ++j ) inverse[i][j] /= det;

  return inverse;
}

const int PipelineDepthFinder::referencePulse_[referencePulseLength_] = { 0, 0, 1, 8, 6, 3, 2, 1 };
