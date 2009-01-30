///
/// @file   DDUHeader.h
/// @author A.Tumanov 
/// @date   4/24/03
/// 
/// @brief documented at http://www.physics.ohio-state.edu/~cms/ddu/ddu2.html
/// 
/// 
///


#ifndef _DDUHeader_h_
#define _DDUHeader_h_

class DDUHeader {

 public:
  DDUHeader();

  int s_link_status() const { return s_link_status_;}
  int format_version() const { return format_version_;}
  int source_id() const { return source_id_;}
  int bxnum() const { return bxnum_;}
  int lvl1num() const { return lvl1num_;}
  int event_type() const { return event_type_;}
  int ncsc() const { return ncsc_;}
  int dmb_dav() const { return dmb_dav_;}
  int errorstat() const {return errorstat_;}
  static unsigned sizeInWords() {return 12;}

  bool check() const;

  // gets some data filled by the event data
  friend class MuEndDDUEventData;
 private:
   
  unsigned s_link_status_  : 4;
  unsigned format_version_ : 4;
  unsigned source_id_      : 12;
  unsigned bxnum_          : 12;

  unsigned lvl1num_        : 24;
  unsigned event_type_     : 4;
  /// should always be 5
  unsigned bit64_          : 4;

  /// should be 8000/0001/8000/8000
  unsigned header2_1_ : 16;
  unsigned header2_2_ : 16;
  unsigned header2_3_ : 16;
  unsigned header2_4_ : 16;

  unsigned ncsc_      : 16;
  unsigned dmb_dav_   : 16;
  unsigned errorstat_ : 32;

};
#endif
