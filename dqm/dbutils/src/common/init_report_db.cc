#include <iostream>
#include <sstream>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

/*
void insert_into_sqlite(sqlite3 *db, CSCMapItem::MapItem& item) 
{
  char *zErrMsg = 0;
  int rc;

//  char *buf = (char *)sqlite3_malloc(256);
  char *buf = sqlite3_mprintf("INSERT INTO csc_map (chamberId,chamberLabel,endcap,station,ring,chamber,cscIndex,layerIndex,stripIndex,\
        anodeIndex,strips,anodes,crateLabel,crateid,sector,trig_sector,dmb,cscid,ddu,ddu_input,slink,fed_crate,\
        ddu_slot,dcc_fifo,fiber_crate,fiber_pos,fiber_socket) values (%d,'%q',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%q',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%q',%d,%d,'%q');",
	item.chamberId, item.chamberLabel.c_str(), item.endcap, item.station, item.ring, item.chamber,
	item.cscIndex, item.layerIndex, item.stripIndex, item.anodeIndex, item.strips,item.anodes,
    	item.crateLabel.c_str(), item.crateid, item.sector, item.trig_sector, item.dmb, item.cscid, item.ddu,
	item.ddu_input, item.slink, item.fed_crate, item.ddu_slot, item.dcc_fifo.c_str(), item.fiber_crate, item.fiber_pos, 
	item.fiber_socket.c_str());
  

   std::cout  << string(buf) << std::endl;

  rc = sqlite3_exec(db, buf, 0, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
//  sqlite3_free(buf);
}
*/

int main(int argc, char **argv)
{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if( argc!=2 ){
    fprintf(stderr, "Usage: %s DATABASE\n", argv[0]);
    exit(1);
  }

  std::string sql_st_test_types = "CREATE TABLE IF NOT EXISTS test_types (test_idx INTEGER PRIMARY KEY AUTOINCREMENT, testID TEXT, testPlot TEXT, testScope TEXT);";

  std::string sql_st_runs_list = "CREATE TABLE IF NOT EXISTS runs_list (run_idx INTEGER PRIMARY KEY AUTOINCREMENT, runID TEXT NOT NULL, genDate TEXT, runNum INTEGER);";

  std::string sql_st_obj_types = "CREATE TABLE IF NOT EXISTS obj_types (obj_idx INTEGER PRIMARY KEY AUTOINCREMENT, objID TEXT NOT NULL, name TEXT);";

  std::string sql_st_report_entries = "CREATE TABLE IF NOT EXISTS report_entries (run_idx INTEGER, obj_idx INTEGER, test_idx INTEGER, descr TEXT, severity INTEGER);";

  rc = sqlite3_open(argv[1], &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }


  rc = sqlite3_exec(db, sql_st_test_types.c_str(), callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  rc = sqlite3_exec(db, sql_st_runs_list.c_str(), callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  rc = sqlite3_exec(db, sql_st_obj_types.c_str(), callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  rc = sqlite3_exec(db, sql_st_report_entries.c_str(), callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sqlite3_close(db);
  return 0;
	
}

