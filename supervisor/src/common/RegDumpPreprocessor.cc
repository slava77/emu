#include "emu/supervisor/RegDumpPreprocessor.h"

// #include "emu/utils/IO.h"

#include "toolbox/regex.h"
#include "xcept/tools.h"
#include <sstream>

RegDumpPreprocessor::RegDumpPreprocessor()
  : options_      ( removeComments | expandRanges )
  , commentToken_ ( "#" )
  , title_        ( "Untitled register dump" )
  , messageStream_( NULL )
{}

std::string
RegDumpPreprocessor::process( const std::string& original, const std::string& substitutes ){
  currentTitle_ = title_ + " ( original )";
  std::string processedOriginal( process( original ) );
  if ( substitutes.length() == 0 ) return processedOriginal;
  currentTitle_ = title_ + " ( substitutes )";
  std::string processedSubstitutes( process( substitutes ) );
  return makeSubstitutions( processedOriginal, processedSubstitutes );
}

std::string
RegDumpPreprocessor::process( const std::string& regDump ){
  if ( commentToken_.length() == 0 ) options_ = ( options_ & ~removeComments );
  lineNumbers_.clear();
  currentLineNumber_ = 0;
  std::string result;
  std::istringstream iss( regDump );
  std::string line;
  while( std::getline( iss, line ) ){
    currentLineNumber_++;
    if ( options_ & expandRanges ) result.append( expandRange( line ) );
    else                           addLine( result, line );
  }
  return result;
}

std::string RegDumpPreprocessor::removeComment( const std::string& line ) const {
  return line.substr( 0, line.find( commentToken_ ) );
}

std::string RegDumpPreprocessor::getRegisterName( const std::string& line ) const {
  std::vector<std::string> matches;
  if ( toolbox::regx_match( line, "^[[:blank:]]*([^[:blank:]]+)", matches ) ) return matches.at(1);
  return std::string();
}

void RegDumpPreprocessor::checkForDuplicate( const std::string& line ){
  std::string registerName( getRegisterName( removeComment( line ) ) );
  if ( registerName.length() == 0 ) return;
  std::map<std::string, unsigned int>::const_iterator previousOccurence = lineNumbers_.find( registerName );
  if ( previousOccurence != lineNumbers_.end() && messageStream_ ){
    *messageStream_ << currentTitle_
		    << ", line "                   << currentLineNumber_
		    << ": Duplicate register "     << registerName
		    << ", first occurred at line " << previousOccurence->second
		    << std::endl;
  }
  else{
    lineNumbers_[registerName] = currentLineNumber_;
  }
}

void RegDumpPreprocessor::addLine( std::string& result, const std::string& line ){
  // Add line to result

  checkForDuplicate( line );

  if ( options_ & removeComments ){
    std::string bareLine( removeComment( line ) );
    if ( bareLine.length() > 0 ) result.append( bareLine + "\n" );
  }
  else{
    result.append( line + "\n" );
  }
}

std::string RegDumpPreprocessor::expandRange( const std::string& lines ){
  std::string result;
  std::istringstream iss( lines );
  bool continueExpanding = false;
  std::string line;
  while( std::getline( iss, line ) ){
    std::vector<std::string> matches;
    if ( toolbox::regx_match( line, "^[[:blank:]]*[^[:blank:]]+(\\[\\[[[:digit:]]+-[[:digit:]]+\\]\\])", matches ) ){
      //std::cout << "\nmatches: " << matches << std::endl;
      std::string range( matches.at(1) );
      std::size_t rangePos = line.rfind( range ); // The above regex matches the last range.
      std::vector<std::string> rolledOutRange( rollOutRange( range ) );
      for ( std::vector<std::string>::const_iterator i=rolledOutRange.begin(); i!=rolledOutRange.end(); ++i ){
	std::string newLine( line );
	newLine.replace( rangePos, range.length(), *i );
	result.append( newLine + "\n" ); // Don't check for duplicate here as newLine may need to be expanded further. There will be at least one more iteration.
	//std::cout << "result:\n" << result << std::endl;
      }
      continueExpanding = true; // Call for another iteration.
    }
    else{
      // This is the last iteration over this line as it seems fully expanded. Do the checks and add it to the result.
      addLine( result, line );
    }
  }
  if ( continueExpanding ) return expandRange( result ); // recursion
  return result;
}

std::vector<std::string>
RegDumpPreprocessor::rollOutRange( const std::string& range ) const {
  // Range is of the form [[n-m]]
  // Strip delimiters [[ and ]] to get a stripped range of the form n-m
  std::string strippedRange( range.substr( 2, range.length()-4 ) );
  std::size_t separatorPos = strippedRange.find_first_not_of( "0123456789" );
  if ( separatorPos == strippedRange.npos ) return std::vector<std::string>( 1, strippedRange );
  int from, to;
  std::istringstream( strippedRange.substr( 0             , separatorPos   ) ) >> from;
  std::istringstream( strippedRange.substr( separatorPos+1                 ) ) >> to;
  std::vector<std::string> v;
  for (int i=from; i<=to; ++i) v.push_back((dynamic_cast<std::ostringstream*>(&(std::ostringstream() << i)))->str());
  return v;
}    

std::string
RegDumpPreprocessor::makeSubstitutions( const std::string& original, const std::string& substitutes ){
  // If their register names match, replace the original line with that of the substitute.
  std::string result;
  std::istringstream originalStream( original );
  std::istringstream substitutesStream( substitutes );
  std::string originalLine;
  std::string substituteLine;
  std::vector< std::pair<std::string,bool> > substituteLines; // the bool means "this line has already been substituted"
  while( std::getline( substitutesStream, substituteLine ) ){
    std::string bareSubstituteLine( removeComment( substituteLine ) );
    // Only take this line if something other than white space is left after comments are removed:
    if ( bareSubstituteLine.find_first_not_of(" \t") != std::string::npos ) substituteLines.push_back( std::make_pair( bareSubstituteLine, false ) );
  }
  // Loop over the original lines:
  while( std::getline( originalStream, originalLine ) ){
    std::string bareOriginalLine( removeComment( originalLine ) );
    bool isReplaced = false;
    // Loop over the substitutes to see if any of them are needed to replace the original line:
    for ( std::vector< std::pair<std::string,bool> >::iterator sub=substituteLines.begin(); sub!= substituteLines.end(); ++sub ){
      std::string registerToSubstitute = getRegisterName( sub->first );
      if ( ! sub->second                     && // This substitute has not been used yet.
	   registerToSubstitute.length() > 0 && 
	   getRegisterName( bareOriginalLine ) == registerToSubstitute ){
	// This line has the same register name that this unused substitute line has. Add the substitute to the result:
	result.append( sub->first + "\n" );
	sub->second = true; // Mark this substitute as used.
	isReplaced = true; // This original line has been substituted for.
      }
    }
    // If no substitution was done, add the original line to the result:
    if ( ! isReplaced ) result.append( originalLine + "\n" );
  }
  // Look for unused substitute line(s) and report them:
  bool allUsed = true;
  for ( std::vector< std::pair<std::string,bool> >::iterator sub=substituteLines.begin(); sub!= substituteLines.end(); ++sub ) allUsed &= sub->second;
  if ( ! allUsed ){
    std::ostringstream oss;
    oss << title_ << ": The following lines have not been substituted. Perhaps these registers do not appear in the original?\n";
    for ( std::vector< std::pair<std::string,bool> >::iterator sub=substituteLines.begin(); sub!= substituteLines.end(); ++sub ){
      if ( ! sub->second ) oss << sub->first << "\n";
    }
    XCEPT_RAISE( xcept::Exception, std::string( "Error in TCDS register configuration: " ) + oss.str() );
  }
  return result;
}
