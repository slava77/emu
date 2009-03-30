/////////////////////////////////////////////////
/// \struct CFEBData
/// \brief Structure of 2-dimensional data
/// \author Yuriy Pakhotin
/// \date Fri Jan 11 12:51:36 CET 2008
/////////////////////////////////////////////////
#ifndef CFEB_DATA_H
#define CFEB_DATA_H

#define CFEB_DATA_NLAYERS 6
#define CFEB_DATA_NBINS 80

struct CFEBData
{
  int Nlayers; ///< \brief Number of layers
  int Nbins;     ///< \brief Number of bins
  double content[CFEB_DATA_NLAYERS][CFEB_DATA_NBINS]; ///< \brief Data content of structure
  int cnts[CFEB_DATA_NLAYERS][CFEB_DATA_NBINS];
};

#endif
