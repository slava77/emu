#include "emu/base/WebReportItem.h"

using namespace std;

emu::base::WebReportItem::WebReportItem( const string name,
					 const string value,
					 const string nameDescription,
					 const string valueDescription,
					 const string nameURL,
					 const string valueURL ) :
  name_            ( name ),
  value_           ( value ),
  nameDescription_ ( nameDescription ),
  valueDescription_( valueDescription ),
  nameURL_         ( nameURL ),
  valueURL_        ( valueURL ){}
string emu::base::WebReportItem::getName() const             { return name_;             }
string emu::base::WebReportItem::getValue() const            { return value_;            }
string emu::base::WebReportItem::getNameDescription() const  { return nameDescription_;  }
string emu::base::WebReportItem::getValueDescription() const { return valueDescription_; }
string emu::base::WebReportItem::getNameURL() const          { return nameURL_;          }
string emu::base::WebReportItem::getValueURL() const         { return valueURL_;         }
