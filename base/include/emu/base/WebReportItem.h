#ifndef __WEBREPORTITEM_H__
#define __WEBREPORTITEM_H__

#include <string>

namespace emu { namespace base {
  using namespace std;
  ///
  /// Class for an item to be reported to Emu Page 1
  ///
  class WebReportItem {
  public:
    /// Constructor.
    ///
    /// @param name name of parameter, to be shown as an html link 
    /// @param value value of parameter, to be shown as an html link
    /// @param nameDescription one-line description of the parameter, to be shown in a tooltip for the \c name link
    /// @param valueDescription one-line description of the value, to be shown in a tooltip for the \c value link
    /// @param nameURL \c name will link to this URL, say, a documentation twiki page (may be null string)
    /// @param valueURL \c value will link to this URL, say, the detailed monitor page (may be null string)
    ///
    WebReportItem( const string name,
		   const string value,
		   const string nameDescription,
		   const string valueDescription,
		   const string nameURL,
		   const string valueURL );
    string getName() const;	///< accessor of name
    string getValue() const;	///< accessor of value
    string getNameDescription() const; ///< accessor of name description
    string getValueDescription() const;	///< accessor of value description
    string getNameURL() const;	///< accessor of name URL
    string getValueURL() const;	///< accessor of value URL

  private:
    string name_;              ///< name of parameter, to be shown as an html link
    string value_;	       ///< value of parameter, to be shown as an html link
    string nameDescription_;   ///< one-line description of the parameter, to be shown in a tooltip for \c name link
    string valueDescription_;  ///< one-line description of the value, to be shown in a tooltip for \c value link
    string nameURL_;	       ///< \c name  will link to this URL, say, a documentation twiki page (may be null string)
    string valueURL_;	       ///< \c value will link to this URL, say, the detailed monitor page (may be null string)
  };
}} // namespace emu::base

#endif  // ifndef __WEBREPORTITEM_H__
