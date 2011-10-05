///
/// @file   Attachment.h
/// @author cscdaq common account <cscdaq@csc-C2D08-11.cms>
/// @date   Thu Jul 15 09:06:40 2010
/// 
/// @brief  An attachment to a SOAP message.
/// 
/// 
///
#ifndef __emu_soap_Attachment_h__
#define __emu_soap_Attachment_h__

#include <sstream>
#include <vector>

namespace emu{
  namespace soap{
    using namespace std;

    class Attachment;
    /// Insertion operator for formatted text output.
    ///
    /// @param os The \c ostream object.
    /// @param attachment The \c Attachment object to be serialized.
    ///
    /// @return Ref. to the \c ostream object itself.
    ///
    ostream& operator<<( ostream& os,  const emu::soap::Attachment& attachment );

    ///
    /// An attachment to a SOAP message.
    ///
    class Attachment{
    public:

      /// Insertion operator for formatted text output.
      ///
      /// @param os The \c ostream object.
      /// @param attachment The \c Attachment object to be serialized.
      ///
      /// @return Ref. to the \c ostream object itself.
      ///
      friend ostream& operator<<( ostream& os,  const emu::soap::Attachment& attachment );

      /// 
      /// Default ctor.
      ///
      Attachment();

      /// Ctor from data and its length.
      ///
      /// @param length The length of data in bytes.
      /// @param content Pointer to the data.
      ///
      Attachment( const unsigned int length, const char* const content );

      /// 
      /// Gets the data.
      ///
      /// @return Pointer to the data.
      ///
      const char*  getContent() const;

      /// 
      /// Gets the data length.
      ///
      /// @return The data length in bytes.
      ///
      unsigned int getContentLength() const;

      /// 
      /// Gets the content type in the MIME header.
      ///
      /// @return Content type in the MIME header.
      ///
      string       getContentType() const;

      /// 
      /// Gets the content encoding in the MIME header.
      ///
      /// @return Content encoding in the MIME header.
      ///
      string       getContentEncoding() const;

      /// 
      /// Gets the content id in the MIME header.
      ///
      /// @return Content id in the MIME header.
      ///
      string       getContentId() const;

      /// 
      /// Gets the content location in the MIME header.
      ///
      /// @return Content location in the MIME header.
      ///
      string       getContentLocation() const;

      /// 
      /// Sets pointer to the data.
      /// @param content Pointer to the data.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContent        ( const char* const content );

      /// 
      /// Sets the data length.
      /// @param length The length of data in bytes.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContentLength  ( const unsigned int length );

      /// 
      /// Sets the content type in the MIME header.
      /// @param type Content type in the MIME header.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContentType    ( const string& type        );

      /// 
      /// Sets the content encoding in the MIME header.
      /// @param encoding Content encoding in the MIME header.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContentEncoding( const string& encoding    );

      /// 
      /// Sets the content id in the MIME header.
      /// @param id Content id in the MIME header.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContentId      ( const string& id          );

      /// 
      /// Sets the content location in the MIME header.
      /// @param location Content location in the MIME header.
      ///
      /// @return The \c Attachment object itself.
      ///
      emu::soap::Attachment& setContentLocation( const string& location    );

      static const std::vector<emu::soap::Attachment> none; ///< An empty container of attachments.

    private:
      const char   *content_; ///< Pointer to the data.
      unsigned int  contentLength_; ///< Data length in bytes.
      string        contentType_; ///< Content type in MIME header.
      string        contentEncoding_; ///< Content encoding in MIME header.
      string        contentId_; ///< Content id in MIME header.
      string        contentLocation_; ///< Content location in MIME header.
    };

  }
}

#endif
