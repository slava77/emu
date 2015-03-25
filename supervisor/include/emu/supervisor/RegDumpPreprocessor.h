#ifndef __emu_supervisor_RegDumpPreprocessor_h__
#define __emu_supervisor_RegDumpPreprocessor_h__

#include <string>
#include <iostream>
#include <vector>
#include <map>

///
/// @file   RegDumpPreprocessor.h
/// @author cscdev common account <cscdev@srv-s2g18-19-01.cms>
/// @date   Mon Oct 27 15:15:48 2014
/// 
/// @brief  TCDS register dump preprocessor
/// 
/// Optionally removes comments and empty/black lines and/or expands ranges of the form [[n-m]].
/// Checks for duplicate register names.


class RegDumpPreprocessor{
public:
  enum { 
    removeComments = 1,		///< Remove comments and empty/blank lines.
    expandRanges   = 2		///< Expand ranges of the form [[n-m]]
//  newFeature     = 4
  };

  /// Default ctor.
  ///
  /// Example:
  /// \code
  /// RegDumpPreprocessor pp;
  /// ostringstream msg;    // Use this to collect messages in.
  /// pp.setOptions( RegDumpPreprocessor::expandRanges ).setMessageStream( msg );
  /// cout << "Unprocessed regDump:\n"     << ss.str()
  ///      << "\nPreprocessed regDump:\n"  << pp.process( ss.str() );
  /// cout << "\nPreprocessor messages:\n" << msg.str() << endl;
  /// /// \endcode
  /// This will result in, for example:
  /// \code
  /// Unprocessed regDump:
  /// aaa[[1-3]].bbb[[2-4]]       0x00000001
  /// aaa2.bbb4                   0x00000002 # this will be a duplicate
  /// aaa[[[6-8]]].bbb[[[5-7]]]   0x00000003 # characters [ and ] can still be used
  /// 
  /// Preprocessed regDump:
  /// aaa1.bbb2       0x00000001
  /// aaa2.bbb2       0x00000001
  /// aaa3.bbb2       0x00000001
  /// aaa1.bbb3       0x00000001
  /// aaa2.bbb3       0x00000001
  /// aaa3.bbb3       0x00000001
  /// aaa1.bbb4       0x00000001
  /// aaa2.bbb4       0x00000001
  /// aaa3.bbb4       0x00000001
  /// aaa2.bbb4                   0x00000002 # this will be a duplicate
  /// aaa[6].bbb[5]   0x00000003 # characters [ and ] can still be used
  /// aaa[7].bbb[5]   0x00000003 # characters [ and ] can still be used
  /// aaa[8].bbb[5]   0x00000003 # characters [ and ] can still be used
  /// aaa[6].bbb[6]   0x00000003 # characters [ and ] can still be used
  /// aaa[7].bbb[6]   0x00000003 # characters [ and ] can still be used
  /// aaa[8].bbb[6]   0x00000003 # characters [ and ] can still be used
  /// aaa[6].bbb[7]   0x00000003 # characters [ and ] can still be used
  /// aaa[7].bbb[7]   0x00000003 # characters [ and ] can still be used
  /// aaa[8].bbb[7]   0x00000003 # characters [ and ] can still be used
  /// 
  /// Preprocessor messages:
  /// Line 2: Duplicate register aaa2.bbb4, first occurred at line 1
  /// \endcode
  RegDumpPreprocessor();

  /// Set options if they are to be other than the default 'removeComments' and 'expandRanges'.
  ///
  /// @param o Options should be the bitwise OR of any of removeComments, expandRanges
  ///
  /// @return Reference to this object.
  ///
  RegDumpPreprocessor& setOptions      ( const unsigned int o ){ options_ = o;         return *this; }

  /// Set comment token if it is to be othen than the default '#'.
  ///
  /// @param c Comment token, may be multicharacter.
  ///
  /// @return Reference to this object.
  ///
  RegDumpPreprocessor& setCommentToken ( const std::string& c ){ commentToken_ = c;    return *this; }

  /// Set message stream.
  ///
  /// @param ms Message stream, e.g. std::cerr or a ostringstream object.
  ///
  /// @return Reference to this object.
  ///
  RegDumpPreprocessor& setMessageStream( std::ostream& ms     ){ messageStream_ = &ms; return *this; }


  unsigned int  getOptions()       const { return options_;       }
  std::string   getCommentToken()  const { return commentToken_;  }
  std::ostream* getMessageStream() const { return messageStream_; }

  /// Process register dump.
  ///
  /// @param regDump Register dump that may contain comments and/or ranges.
  ///
  /// @return Processed register dump.
  ///
  std::string process( const std::string& regDump );

  /// Process original register dump while replacing registers in the original with matching registers from the substitutes.
  ///
  /// @param original Register dump that may contain comments and/or ranges.
  /// @param substitutes Registers that may contain comments and/or ranges and are to be substituted for matching registers in the original register dump.
  ///
  /// @return Processed register dump after substitutions.
  ///
  std::string process( const std::string& original, const std::string& substitutes );
private:
  std::string removeComment    ( const std::string& line  ) const;
  std::string expandRange      ( const std::string& lines );
  std::string getRegisterName  ( const std::string& line  ) const;
  void        checkForDuplicate( const std::string& line  );
  void        addLine          ( std::string& result, const std::string& line );
  std::vector<std::string> rollOutRange( const std::string& range ) const;
  std::string makeSubstitutions( const std::string& original, const std::string& substitutes );
  unsigned int                         options_;
  std::string                          commentToken_;
  unsigned int                         currentLineNumber_;
  std::map<std::string, unsigned int>  lineNumbers_; ///< register name --> line number
  std::ostream                        *messageStream_;
};

#endif
