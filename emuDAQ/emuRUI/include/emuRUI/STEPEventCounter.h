#ifndef __emuRUI_STEPEventCounter_h__
#define __emuRUI_STEPEventCounter_h__

#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Vector.h"
#include "xdata/include/xdata/Boolean.h"
#include <string>

namespace emuRUI{

  class STEPEventCounter{
    // Counts events on each input of the DDU. 
    // Info on live inputs and inputs with data are obtained from DDU header (3).
  public:
    enum { maxDDUInputs_ = 15 };
    STEPEventCounter();
    void         initialize( const unsigned int requestedEvents, char* const DDUHeader );
    bool         isInitialized() const { return isInitialized_; }
    void         reset();
    bool         isNeededEvent( char* const DDUHeader );
    unsigned int getLowestCount() const;
    unsigned int getCount( const int dduInputIndex ) const;
    unsigned int getNEvents() const{ return neededEvents_; }
    bool         isLiveInput( const int dduInputIndex ) const;
    bool         isMaskedInput( const int dduInputIndex ) const;
    void         maskInput( const int dduInputIndex );
    void         unmaskInput( const int dduInputIndex );
    std::string  print() const;

  private:
    enum { offsetNonEmptyDDUInputsField_  = 18, offsetLiveDDUInputsField_ = 22 }; // from the start of DDU header1 [bytes]
    unsigned int requestedEvents_;
    unsigned int neededEvents_; // the total number of events needed so far
    bool         isLiveInput_[maxDDUInputs_]; // this is obtained from the DDU header
    bool         isMaskedInput_[maxDDUInputs_]; // this the user can set
    unsigned int count_[maxDDUInputs_];
    bool         isInitialized_;
  };

}

#endif
