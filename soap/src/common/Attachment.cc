#include "emu/soap/Attachment.h"

const std::vector<emu::soap::Attachment> emu::soap::Attachment::none = std::vector<emu::soap::Attachment>();

using namespace std;

emu::soap::Attachment::Attachment() :
  content_( NULL ),
  contentLength_( 0 )
{}

emu::soap::Attachment::Attachment( const unsigned int length, const char* const content ) :
  content_( content ),
  contentLength_( length )
{}

const char*  emu::soap::Attachment::getContent()         const { return content_;         }
unsigned int emu::soap::Attachment::getContentLength()   const { return contentLength_;   }
string       emu::soap::Attachment::getContentType()     const { return contentType_;     }
string       emu::soap::Attachment::getContentEncoding() const { return contentEncoding_; }
string       emu::soap::Attachment::getContentId()       const { return contentId_;       }
string       emu::soap::Attachment::getContentLocation() const { return contentLocation_; }

emu::soap::Attachment& emu::soap::Attachment::setContent        ( const char* const content ){ content_         = content ; return *this; }
emu::soap::Attachment& emu::soap::Attachment::setContentLength  ( const unsigned int length ){ contentLength_   = length  ; return *this; }
emu::soap::Attachment& emu::soap::Attachment::setContentType    ( const string& type        ){ contentType_     = type    ; return *this; }
emu::soap::Attachment& emu::soap::Attachment::setContentEncoding( const string& encoding    ){ contentEncoding_ = encoding; return *this; }
emu::soap::Attachment& emu::soap::Attachment::setContentId      ( const string& id          ){ contentId_       = id      ; return *this; }
emu::soap::Attachment& emu::soap::Attachment::setContentLocation( const string& location    ){ contentLocation_ = location; return *this; }

ostream&
emu::soap::operator<<( ostream& os,  const emu::soap::Attachment& a ){
  std::string content(a.getContent(), a.getContentLength());
  os << "SOAP attachment type=" << a.getContentType()
     << " encoding="            << a.getContentEncoding()
     << " id="                  << a.getContentId()
     << " location="            << a.getContentLocation()
     << " length="              << a.getContentLength()
     << " :"                    << endl
     << a.getContent()          << endl;
  return os;
}
