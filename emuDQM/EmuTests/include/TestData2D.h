/////////////////////////////////////////////////
/// \struct TestData2D
/// \brief Structure of 2-dimensional data
/// \author Yuriy Pakhotin
/// \date Fri Jan 18 12:41:09 CET 2008
/////////////////////////////////////////////////
#ifndef TEST_DATA2D_H
#define TEST_DATA2D_H

#define TEST_DATA2D_NLAYERS 6
#define TEST_DATA2D_NBINS 112

struct TestData2D{
	int Nlayers; ///< Number of layers
	int Nbins;     ///< Number of bins
	double content[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS]; ///< Data content of structure
	int cnts[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS]; ///< Array of counters
};

#endif
