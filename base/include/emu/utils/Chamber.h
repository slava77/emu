///
/// @file   Chamber.h
/// @author K. Banicz
/// @date   Tue Nov 20 16:58:48 2012
/// 
/// @brief  Class to check the validity of a chamber and generate its canonical name.
/// 
/// 
///
#ifndef __emu_utils_Chamber_h__
#define __emu_utils_Chamber_h__

#include <string>

using namespace std;

namespace emu { namespace utils {

      class Chamber{
      private:
	int endcap_;
	int station_;
	int ring_;
	int chamber_;
	string name_;	///< Canonical chamber name, i.e., ME[+-][1-4]/[1-3]/[0-3][0-9]
	string canonicalName() const;
	int endcapNumber( const int endcap ) const;
	int sgn( const int i ){
	  if      ( i >  0 ) return 1; 
	  else if ( i == 0 ) return 0; 
	  return -1;
	}
      public:

	/// Default ctor.
	///
	///
	Chamber();

	/// Ctor.
	///
	/// @param endcap Plus side if greater than 0, minus side if less than 0.
	/// @param station Station number.
	/// @param ring Ring number.
	/// @param chamber Chamber number.
	///
	Chamber( const int endcap, const int station, const int ring, const int chamber );

	/// Ctor.
	///
	/// @param endcap Plus side if any of '+Pp', minus side if any of '-Mm'.
	/// @param station Station number.
	/// @param ring Ring number.
	/// @param chamber Chamber number.
	///
	Chamber( const char endcap, const int station, const int ring, const int chamber );

	/// Ctor from name (not necessarily the canonical one).
	///
	/// @param name Can be of any reasonable format, e.g., "MEm1.2.3" or "me-1_2_3" or the canonical "ME-1/2/03".
	///
	Chamber( const string& name );

	int endcap() const { return endcap_; }

	int station() const { return station_; }

	int ring() const { return ring_; }

	int chamber() const { return chamber_; }

	/// Get the canonical chamber name, i.e., ME[+-][1-4]/[1-3]/[0-3][0-9]
	///
	///
	/// @return The canonical chamber name if it is a valid chamber; empty string or the original name (if that was the argument of the ctor) otherwise.
	///
	string name() const { return name_; }

	/// Check if this is a formally valid chamber.
	///
	///
	/// @return \e true if this is a formally valid chamber, \e false otherwise.
	///
	bool isValid() const;
      };

}} // namespace emu::utils

#endif
