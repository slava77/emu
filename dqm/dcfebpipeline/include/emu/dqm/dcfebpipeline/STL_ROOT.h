#ifndef __STL_ROOT_h__
#define __STL_ROOT_h__

#include <stdexcept>
#include "TH2D.h"
#include "TProfile.h"

template <typename Iterator, typename ROOTHistogram>
bool TH1From( const Iterator first, const Iterator last, ROOTHistogram& h, const int firstBin ){
  int count = 0;
  if ( h.IsA() != TH1F::Class() &&
       h.IsA() != TH1D::Class() &&
       h.IsA() != TH1C::Class() &&
       h.IsA() != TH1S::Class() &&
       h.IsA() != TProfile::Class() ) throw std::invalid_argument( "Histogram must be 1D." );
  Iterator i = first;
  for ( int iBinX = firstBin; iBinX <= h.GetNbinsX() && i != last; ++iBinX ){
    h.SetBinContent( iBinX, *(i++) );
    ++count;
  }
  return count;
}

template <typename Iterator, typename ROOTHistogram>
bool TH2From( const Iterator first, const Iterator last, ROOTHistogram& h, const int firstBin, const int atBin, bool fillColumn = false ){
  int count = 0;
  if ( h.IsA() != TH2F::Class() &&
       h.IsA() != TH2D::Class() &&
       h.IsA() != TH2C::Class() &&
       h.IsA() != TH2I::Class() &&
       h.IsA() != TH2S::Class()    ) throw std::invalid_argument( "Histogram must be 2D." );
  Iterator i = first;
  if ( fillColumn ){
    if ( atBin < 0 || h.GetNbinsX() < atBin ) throw std::out_of_range( "Column number out of range." );
    for ( int iBinY = firstBin; iBinY <= h.GetNbinsY() && i != last; ++iBinY ){
      h.SetBinContent( atBin, iBinY, *(i++) );
      ++count;
    }
  }
  else{
    if ( atBin < 0 || h.GetNbinsY() < atBin ) throw std::out_of_range( "Row number out of range." );
    for ( int iBinX = firstBin; iBinX <= h.GetNbinsX() && i != last; ++iBinX ){
      h.SetBinContent( iBinX, atBin, *(i++) );
      ++count;
    }
  }
  return count;
}

#endif
