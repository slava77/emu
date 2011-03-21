/*
 * =====================================================================================
 *
 *       Filename:  CSCDqmFact.h
 *
 *    Description:  Local DQM Fact used by client to DW
 *
 *        Version:  1.0
 *        Created:  02/05/2010 05:29:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#ifndef CSCDqmFact_h
#define CSCDqmFact_h

#include "emu/base/Fact.h"

class CSCDqmFact : public emu::base::Fact {

  public:

    CSCDqmFact(const std::string &run, const emu::base::Component &component, const std::string &name) {
      run_ = run;
      component_ = component;
      name_ = name;
    }
/*
    CSCDqmFact(const std::string &run, const std::string &component, const std::string &name) {
      run_ = run;
      component_.addId(component);
      name_ = name;
    }
*/
    CSCDqmFact& addParameter(const std::string &name, long value) {
      std::stringstream ss;
      ss << value;
      parameters_[name] = ss.str();
      return *this;
    }

    CSCDqmFact& addParameter(const std::string &name, const std::string value) {
      parameters_[name] = value;
      return *this;
    }

    CSCDqmFact& setSeverity(const std::string &sev) {
      severity_ = sev;
      return *this;
    }

    CSCDqmFact& setDescription(const std::string &descr) {
      description_ = descr;
      return *this;
    }

    CSCDqmFact& setRun(const std::string &run) {
      run_ = run;
      return *this;
    }

    CSCDqmFact& setTime(const std::string &time) {
      time_ = time;
      return *this;
    }



};

#endif
