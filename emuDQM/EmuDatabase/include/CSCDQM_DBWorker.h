/*
 * =====================================================================================
 *
 *       Filename:  CSCDQM_DBWorker.h
 *
 *    Description:  CSCDQM Oracle Database worker object definition
 *
 *        Version:  1.0
 *        Created:  06/04/2009 06:18:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#ifndef CSCDQM_DBWORKER_H
#define CSCDQM_DBWORKER_H

#include <occi.h>
#include "DQM/CSCMonitorModule/interface/CSCDQM_Utility.h"
#include "DQM/CSCMonitorModule/interface/CSCDQM_Logger.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace cscdqm {

  class DBWorker {

    public:

      DBWorker(const oracle::occi::Connection *conn_);
      ~DBWorker();

      void setSQL(const std::string& sql);
      void setInt(const unsigned int number, const int unsigned value);

      template <typename T>
      void setVector(const unsigned int number, const std::vector<T> v, const std::string& type) {
        if (f_sql_set) {
          oracle::occi::setVector(stmt, number, v, type);
          const unsigned int i = v.size();
          parameters[number] = "vector of " + cscdqm::toString(i) + " values";
        }
      }

      unsigned int execSQL();
      bool execSQLResults(bool moveToFirst = false);
      bool resultsNext();
      unsigned int getInt(const unsigned int number);
      void execSQLResultsToVector(const unsigned int number, std::vector<unsigned int> &results);

      void commit() { 
        LOG_DEBUG << "Commit";
        conn->commit(); 
      }
      void rollback() { 
        LOG_DEBUG << "Rollback";
        conn->rollback(); 
      }

    private:

      void printParameters() const;

      oracle::occi::Connection *conn;
      oracle::occi::Statement *stmt;
      oracle::occi::ResultSet *rs;

      std::map<unsigned int, std::string> parameters;
      bool f_sql_set; 

  };

}

#endif
