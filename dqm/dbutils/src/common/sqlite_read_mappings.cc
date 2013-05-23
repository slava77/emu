#include <iostream>
#include <sstream>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include "emu/dqm/dbutils/cscmap1.h"
#include "CondFormats/CSCObjects/interface/CSCMapItem.h"
#include "CondFormats/CSCObjects/interface/CSCCrateMap.h"

using namespace std;

void printMapItem(CSCMapItem::MapItem& item) {
    cout<<"chamberId: "<<item.chamberId<<endl;

    cout<<"chamberLabel: "<<item.chamberLabel<<endl;

    cout<<"endcap: "<<item.endcap<<endl;
    cout<<"station: "<<item.station<<endl;
    cout<<"ring: "<<item.ring<<endl;
    cout<<"chamber: "<<item.chamber<<endl;
    cout<<"cscIndex: "<<item.cscIndex<<endl;
    cout<<"layerIndex: "<<item.layerIndex<<endl;
    cout<<"stripIndex: "<<item.stripIndex<<endl;
    cout<<"anodeIndex: "<<item.anodeIndex<<endl;
    cout<<"strips: "<<item.strips<<endl;
    cout<<"anodes: "<<item.anodes<<endl;
    cout<<"crateLabel: "<<item.crateLabel<<endl;
    cout<<"crateid: "<<item.crateid<<endl;
    cout<<"sector: "<<item.sector<<endl;
    cout<<"trig_sector: "<<item.trig_sector<<endl;
    cout<<"dmb: "<<item.dmb<<endl;
    cout<<"cscid: "<<item.cscid<<endl;
    cout<<"ddu: "<<item.ddu<<endl;
    cout<<"ddu_input: "<<item.ddu_input<<endl;
    cout<<"slink: "<<item.slink<<endl;
    cout<<"fed_crate: "<<item.fed_crate<<endl;
    cout<<"ddu_slot: "<<item.ddu_slot<<endl;
    cout<<"dcc_fifo: "<<item.dcc_fifo<<endl;
    cout<<"fiber_crate: "<<item.fiber_crate<<endl;
    cout<<"fiber_pos: "<<item.fiber_pos<<endl;
    cout<<"fiber_socket: "<<item.fiber_socket<<endl;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}


int main(int argc, char **argv)
{

  CSCMapItem::MapItem item;
  CSCCrateMap* cratemap = new CSCCrateMap();
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if( argc!=2 ){
    fprintf(stderr, "Usage: %s DATABASE\n", argv[0]);
    exit(1);
  }



  rc = sqlite3_open(argv[1], &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }

  std::string sql_st = "SELECT * FROM csc_map;";


  int          col_cnt, row_cnt; 
  const char   *sqst_end;
  sqlite3_stmt *sqst;
  
  /*Compile the SQL.*/
  rc = sqlite3_prepare_v2(db, sql_st.c_str(), -1, &sqst, &sqst_end);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL compile error: %s\n", sqlite3_errmsg(db));
    exit(1);
  }  /*if*/

  /*Execute the SQL.  It could be busy.  Count busy polls to break 
  a deadlock.  When busy reset the current step, pause and relaunch it.*/

  /*Count columns in statement and access declared types.*/
  col_cnt = sqlite3_column_count(sqst);

  bool finished = false;
  while (!finished) {
    rc = sqlite3_step(sqst);
    switch (rc) {
      case SQLITE_DONE:     /*Execution finished.*/
        finished = true;
        sqlite3_reset(sqst);  /*Resets the compiled statement for re-use.*/

        /*Execute logic for end of data set.*/
        /*!!!*/
        break;
      case SQLITE_ROW:      /*We have a row.*/
        if (row_cnt == 0) {
          /*Execute code for start of data set*/
          /*!!!!*/
        }

    	item.chamberId = sqlite3_column_int(sqst, 0);
        item.chamberLabel = (const char *)sqlite3_column_text(sqst, 1);
	item.endcap = sqlite3_column_int(sqst, 2); 
    item.station = sqlite3_column_int(sqst, 3);
    item.ring = sqlite3_column_int(sqst, 4);
    item.chamber = sqlite3_column_int(sqst, 5);
    item.cscIndex = sqlite3_column_int(sqst, 6);
    item.layerIndex = sqlite3_column_int(sqst, 7);
    item.stripIndex = sqlite3_column_int(sqst, 8);
    item.anodeIndex = sqlite3_column_int(sqst, 9);
    item.strips = sqlite3_column_int(sqst, 10);
    item.anodes = sqlite3_column_int(sqst, 11);
    item.crateLabel = (const char *)sqlite3_column_text(sqst, 12);
    item.crateid  = sqlite3_column_int(sqst, 13);
    item.sector = sqlite3_column_int(sqst, 14);
    item.trig_sector = sqlite3_column_int(sqst, 15);
    item.dmb = sqlite3_column_int(sqst, 16);
    item.cscid = sqlite3_column_int(sqst, 17);
    item.ddu = sqlite3_column_int(sqst, 18);
    item.ddu_input = sqlite3_column_int(sqst, 19);
    item.slink = sqlite3_column_int(sqst, 20);
    item.fed_crate = sqlite3_column_int(sqst, 21);
    item.ddu_slot = sqlite3_column_int(sqst, 22);
    item.dcc_fifo = (const char *)sqlite3_column_text(sqst, 23);
    item.fiber_crate = sqlite3_column_int(sqst, 24);
    item.fiber_pos = sqlite3_column_int(sqst, 25);
    item.fiber_socket = (const char *)sqlite3_column_text(sqst, 26);

    if (cratemap) cratemap->crate_map[item.crateid*10+item.cscid];

         
	printMapItem(item);
        std::cout << "-------------------------------------"<< std::endl;
        /*Scan all the columns.  Return value in "strg"*/
/*
        for (int a = 0; a < col_cnt; a++) {
          std::cout << sqlite3_column_name(sqst, a) << ": ";
          switch (sqlite3_column_type(sqst, a)) {
            case SQLITE_INTEGER:
              int result = sqlite3_column_int(sqst, a);
              // sprintf(strg, "%d", result);
	      std::cout << result << std::endl;
              break;
            case SQLITE_FLOAT:
              {
                double  dub;
                dub = sqlite3_column_double(sqst, a);
		std::cout << dub << std::endl;
              }
              break;
            case SQLITE_TEXT:
//              std::string txt(sqlite3_column_text(sqst, a));
	      std::cout << sqlite3_column_text(sqst, a) << std::endl;
              break;
            case SQLITE_BLOB:
              break;
            case SQLITE_NULL:
              std::cout << "NULL" << std::endl;
              break;
          }  
        }    
*/
        row_cnt++;
        break;
      default:    /*A nasty error.*/
        sqlite3_finalize(sqst);
        // wrapup("9: Fatal SQL EXEC error");
        break;
    }  /*switch*/
  }    /*while*/




  // CSCMapItem::MapItem item;


  sqlite3_close(db);

  delete cratemap;
  return 0;
	
}

