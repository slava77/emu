#include <iostream>
#include <sstream>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#include "emu/dqm/dbutils/cscmap1.h"

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

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

//  sql_st = "INSERT INTO csc_map (chamberId,chamberLabel,endcap,station,ring,chamber,cscIndex,layerIndex,stripIndex,\
        anodeIndex,strips,anodes,crateLabel,crateid,sector,trig_sector,dmb,cscid,ddu,ddu_input,slink,fed_crate,\
        ddu_slot,dcc_fifo,fiber_crate,fiber_pos,fiber_socket) values ("+st.str()+");";
  rc = sqlite3_exec(db, buf, 0, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
//  sqlite3_free(buf);
}

int main(int argc, char **argv)
{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if( argc!=2 ){
    fprintf(stderr, "Usage: %s DATABASE\n", argv[0]);
    exit(1);
  }

  std::string sql_st = "CREATE TABLE csc_map (chamberId INTEGER PRIMARY KEY,chamberLabel TEXT,endcap INTEGER,\
    	station INTEGER,ring INTEGER,chamber INTEGER,cscIndex INTEGER,layerIndex INTEGER,stripIndex INTEGER,\
	anodeIndex INTEGER,strips INTEGER,anodes INTEGER,crateLabel TEXT,crateid INTEGER,sector INTEGER,\
	trig_sector INTEGER,dmb INTEGER,cscid INTEGER,ddu INTEGER,ddu_input INTEGER,slink INTEGER,fed_crate INTEGER,\
	ddu_slot INTEGER,dcc_fifo TEXT,fiber_crate INTEGER,fiber_pos INTEGER,fiber_socket TEXT);";




  rc = sqlite3_open(argv[1], &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }


  rc = sqlite3_exec(db, sql_st.c_str(), callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

//  sqlite3_close(db);
//  return 0;

  CSCMapItem::MapItem item;

  cscmap1 *map = new cscmap1 ();
  cout << " Connected cscmap ... " << endl;

//  map->chamber(122360,&item);
//  insert_into_sqlite(db, item); 

  int r,c, count=0;
  for(int i=1;i<=2;++i){
    for(int j=1;j<=4;++j){
      if(j==1) r=3;
      //else if(j==4) r=1;
      else r=2;
      for(int k=1;k<=r;++k){
        if(j>1 && k==1) c=18;
        else c=36;
        for(int l=1;l<=c;++l){
          int chamberid=i*100000+j*10000+k*1000+l*10;
          map->chamber(chamberid,&item);
          int crate_cscid=item.crateid*10+item.cscid;
	  
	  insert_into_sqlite(db, item);
          count++;
	  std::cout << count << " " << item.chamberLabel << " " << item.chamberId << std::endl;


        }
      }
    }
  }

  delete map;

  sqlite3_close(db);
  return 0;
	
}

