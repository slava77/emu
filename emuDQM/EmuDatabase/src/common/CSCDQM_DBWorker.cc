/*
 * =====================================================================================
 *
 *       Filename:  CSCDQM_DBWorker.cc
 *
 *    Description:  CSCDQM Oracle Database Worker object
 *
 *        Version:  1.0
 *        Created:  06/04/2009 06:17:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#include "CSCDQM_DBWorker.h"

namespace cscdqm {

  DBWorker::DBWorker(const oracle::occi::Connection *conn_) {
    conn = const_cast<oracle::occi::Connection*>(conn_);
    stmt = conn->createStatement();
    stmt->setAutoCommit(false);
    rs = 0;
    f_sql_set = false;
  }

  DBWorker::~DBWorker() {
    if (rs) stmt->closeResultSet(rs);
  }

  void DBWorker::printParameters() const {
    if (parameters.size() > 0) {
      ostringstream sout;
      for (std::map<unsigned int, std::string>::const_iterator it = parameters.begin(); it != parameters.end(); it++) {
        sout << ":" << it->first << " = " << it->second << " ";
      }
      LOG_DEBUG << sout.str();
    }
  }

  void DBWorker::setSQL(const std::string& sql) {
    if (f_sql_set && rs != 0) {
      stmt->closeResultSet(rs);
      rs = 0;
    }
    LOG_DEBUG << sql;
    stmt->setSQL(sql);
    parameters.clear();
    f_sql_set = true;
  }

  unsigned int DBWorker::execSQL() {
    if (f_sql_set) {
      printParameters();
      const unsigned int rows = stmt->executeUpdate();
      LOG_DEBUG << "Rows affected: " << rows;
      return rows;
    }
    return 0;
  }

  void DBWorker::setInt(const unsigned int number, const unsigned int value) {
    if (f_sql_set) {
      stmt->setInt(number, value);
      parameters[number] = cscdqm::toString(value);
    }
  }

  bool DBWorker::execSQLResults(bool moveToFirst) {
    if (f_sql_set) {
      printParameters();
      rs = stmt->executeQuery();
      if (moveToFirst) return rs->next();
    } else {
      return false;
    }
    return true;
  }

  void DBWorker::execSQLResultsToVector(const unsigned int number, std::vector<unsigned int> &results) {
    if (execSQLResults(false)) {
      while (resultsNext()) {
        results.push_back(getInt(number));
      }
    }
  }

  bool DBWorker::resultsNext() {
    if (!rs) return false;
    return rs->next();
  }

  unsigned int DBWorker::getInt(const unsigned int number) {
    if (!rs) return 0;
    return (unsigned int) rs->getNumber(number);
  }

};

