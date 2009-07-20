/////////////////////////////////////////////////
/// \struct TestData1D
/// \brief Structure of 1-dimensional data
/// \author Yuriy Pakhotin
/// \date Fri Jan 18 12:41:09 CET 2008
/////////////////////////////////////////////////
#ifndef TEST_DATA1D_H
#define TEST_DATA1D_H

#define TEST_DATA1D_NBINS 112

struct TestData1D
{
  int Nbins;    ///< Number of bins
  double content[TEST_DATA1D_NBINS]; ///< Data content of structure
  int cnts[TEST_DATA1D_NBINS]; ///< Array of counters
};

#endif
