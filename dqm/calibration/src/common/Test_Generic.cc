#include "emu/dqm/calibration/Test_Generic.h"
#include "emu/utils/IO.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace emu::utils;

time_sample Test_Generic::CalculateCorrectedPulseAmplitude(pulse_fit& fit)
{
  time_sample peak_time;

  double pulse_width=50.; //

  double x1 = fit.left.tbin*pulse_width;
  double x2 = fit.max.tbin*pulse_width;
  double x3 = fit.right.tbin*pulse_width;
  double y1 = fit.left.value;
  double y2 = fit.max.value;
  double y3 = fit.right.value;

  double d21 = (y2-y1)/(x2-x1);
  double d32 = (y3-y2)/(x3-x2);
  double c = (d21-d32)/(x3-x1);
  double x0 = (x1 + 2*x2 + x3)/4 + (d21+d32)/(4*c);
  double A = y2 + c*pow(x2-x0,2);
  double dX=x0-x2;

  double kp[4] = {1.022, -0.027, 7.6, 63.};
  double taup[4] = {-1.5, -2.5, -5, 50};

  double k = kp[0] + kp[1]*cos(2*M_PI*((dX-kp[2])/kp[3]));
  double tau = taup[0] + taup[1]*cos(2*M_PI*((dX-taup[2])/taup[3]));

  peak_time.tbin = (int)(x0+tau);
  peak_time.value = (int)(A*k);
  return peak_time;
}

/*
int Test_Generic::getNumStrips(std::string cscID)
{
  if ((cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") ==0 )) return 64;
  else return 80;
}
*/

bool Test_Generic::isME11(std::string cscID)
{
  return ((cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") == 0));
}

Test_Generic::Test_Generic(std::string dfile): dataFile(dfile), logger(Logger::getInstance("Generic"))
{
  binCheckMask=0xFFFFFFFF;
  cratemap = new CSCCrateMap();
  // fillCrateMap(cratemap);
  // init();
}

Test_Generic::~Test_Generic()
{
  for (cscTestCanvases::iterator itr=tcnvs.begin(); itr != tcnvs.end(); ++itr)
    {
      TestCanvases& cnvs = itr->second;
      TestCanvases::iterator c_itr;
      for (c_itr=cnvs.begin(); c_itr != cnvs.end(); ++c_itr)
        {
          delete c_itr->second;
        }
    }

  if (cratemap) delete cratemap;
  // if (map) delete map;

}

std::string timestr(time_t* t)
{
  char buf[255];
  // time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(t));
  std::string time = std::string(buf);
  return time.substr(0,time.find("\n",0));

};

/*
std::map<std::string, int> getCSCTypeToBinMap()
{
  std::map<std::string, int> tmap;
  tmap["ME-4.2"] = 0;
  tmap["ME-4.1"] = 1;
  tmap["ME-3.2"] = 2;
  tmap["ME-3.1"] = 3;
  tmap["ME-2.2"] = 4;
  tmap["ME-2.1"] = 5;
  tmap["ME-1.3"] = 6;
  tmap["ME-1.2"] = 7;
  tmap["ME-1.1"] = 8;
  tmap["ME+1.1"] = 9;
  tmap["ME+1.2"] = 10;
  tmap["ME+1.3"] = 11;
  tmap["ME+2.1"] = 12;
  tmap["ME+2.2"] = 13;
  tmap["ME+3.1"] = 14;
  tmap["ME+3.2"] = 15;
  tmap["ME+4.1"] = 16;
  tmap["ME+4.2"] = 17;
  return tmap;

}
*/
void Test_Generic::fillCrateMap(CSCCrateMap* mapobj)
{
  if (!fillCrateMapSQLite(mapobj))
    LOG4CPLUS_ERROR (logger,"Unable to fill CSCCrateMap from  " << SQLiteDB);

  // if (!fillCrateMapSQLite(mapobj)) fillCrateMapOracle(mapobj);
}

bool Test_Generic::fillCrateMapSQLite(CSCCrateMap* mapobj)
{
  LOG4CPLUS_DEBUG(logger, "Trying to fill CSCCrateMap using SQLite from " << SQLiteDB);
  CSCMapItem::MapItem item;
  sqlite3 *db;
  //  char *zErrMsg = 0;
  int rc;
  int          col_cnt, row_cnt;
  const char   *sqst_end;
  sqlite3_stmt *sqst;

  std::string sql_st = "SELECT * FROM csc_map;";

  rc = sqlite3_open_v2(SQLiteDB.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
  if ( rc )
    {
      LOG4CPLUS_ERROR (logger,"Can't open database: " << sqlite3_errmsg(db));
      sqlite3_close(db);
      return false;
    }


  /*Compile the SQL.*/
  rc = sqlite3_prepare_v2(db, sql_st.c_str(), -1, &sqst, &sqst_end);
  if (rc != SQLITE_OK)
    {
      LOG4CPLUS_ERROR (logger,"SQL compile error: " << sqlite3_errmsg(db));
      return false;
    }  /*if*/

  /*Execute the SQL.  It could be busy.  Count busy polls to break
    a deadlock.  When busy reset the current step, pause and relaunch it.*/

  /*Count columns in statement and access declared types.*/
  col_cnt = sqlite3_column_count(sqst);

  bool finished = false;
  while (!finished)
    {
      rc = sqlite3_step(sqst);
      switch (rc)
        {
        case SQLITE_DONE:     /*Execution finished.*/
          finished = true;
          sqlite3_reset(sqst);  /*Resets the compiled statement for re-use.*/

          /*Execute logic for end of data set.*/
          /*!!!*/
          break;
        case SQLITE_ROW:      /*We have a row.*/
          if (row_cnt == 0)
            {
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

          if (mapobj) mapobj->crate_map[item.crateid*10+item.cscid] = item;

          row_cnt++;
          break;
        default:    /*A nasty error.*/
          sqlite3_finalize(sqst);
          LOG4CPLUS_ERROR (logger,"fatal SQL EXEC error: " << sqlite3_errmsg(db));
          // wrapup("9: Fatal SQL EXEC error");
          break;
        }  /*switch*/
    }    /*while*/


  sqlite3_close(db);
  LOG4CPLUS_DEBUG(logger, "CSCCrateMap filled using SQLite");

  return true;
}

/*
bool Test_Generic::fillCrateMapOracle(CSCCrateMap* mapobj)
{
  LOG4CPLUS_DEBUG(logger, "Trying to CSCCrateMap using Oracle");
  cscmap1 *map = new cscmap1 ();
  CSCMapItem::MapItem item;

  int i,j,k,l; //i - endcap, j - station, k - ring, l - chamber.
  int r,c;     //r - number of rings, c - number of chambers.
  int count=0;
  int chamberid;
  int crate_cscid;

  // This is version for 540 chambers.
  for (i=1; i<=2; ++i)
    {
      for (j=1; j<=4; ++j)
        {
          if (j==1) r=3;
          //else if(j==4) r=1;
          else r=2;
          for (k=1; k<=r; ++k)
            {
              if (j>1 && k==1) c=18;
              else c=36;
              for (l=1; l<=c; ++l)
                {
                  chamberid=i*100000+j*10000+k*1000+l*10;
                  map->chamber(chamberid,&item);
                  crate_cscid=item.crateid*10+item.cscid;
                  if (mapobj)
                    {
                      mapobj->crate_map[crate_cscid]=item;
                    }
                  count=count+1;
                }
            }
        }
    }
  delete map;
  return true;
}
*/


void Test_Generic::init()
{
  nTotalEvents = 0;
  nBadEvents = 0;
  imgW = DEF_WIDTH;
  imgH = DEF_HEIGHT;
  bin_checker.output1().hide();
  bin_checker.output2().hide();
  bin_checker.crcALCT(true);
  bin_checker.crcTMB (true);
  bin_checker.crcCFEB(true);
  bin_checker.modeDDU(true);
  emucnvs.clear();
  tmap = getCSCTypeToBinMap();
  //  map = new cscmap1();
  //  cratemap = new CSCCrateMap();
  fillCrateMap(cratemap);
}



int Test_Generic::loadTestCfg()
{
  if (configFile == "")
    {
      return 1;
    }

  XMLPlatformUtils::Initialize();
  XercesDOMParser *parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);
  parser->setDoSchema(true);
  parser->setValidationSchemaFullChecking(false); // this is default
  parser->setCreateEntityReferenceNodes(true);  // this is default
  parser->setIncludeIgnorableWhitespace (false);

  parser->parse(configFile.c_str());
  DOMDocument *doc = parser->getDocument();
  if (!doc)
    {
      LOG4CPLUS_ERROR (logger, "Failed to load or parse config file: tried "<<configFile);
    }
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Booking") );
  if ( l->getLength() != 1 )
    {
      LOG4CPLUS_ERROR (logger, "There is not exactly one Booking node in configuration");
      return 1;
    }

  ///* Find and parse Test results plots definitions and configurations
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("TestResult") );
  if ( itemList->getLength() == 0 )
    {
      LOG4CPLUS_ERROR (logger, "There no histograms to book");
      return 1;
    }
  for (uint32_t j=0; j<itemList->getLength(); j++)
    {
      std::map<std::string, std::string> obj_info;
      std::map<std::string, std::string>::iterator itr;
      DOMNodeList *children = itemList->item(j)->getChildNodes();
      for (uint32_t i=0; i<children->getLength(); i++)
        {
          std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
          if ( children->item(i)->hasChildNodes() )
            {
              std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
              obj_info[paramname] = param;
            }
        }

      itr = obj_info.find("Test");
      if ((itr != obj_info.end()) && ((itr->second == testID) || (itr->second == "ALL")))
        {
          itr = obj_info.find("Name");
          if ((itr != obj_info.end()) && (itr->second != ""))
            {
              LOG4CPLUS_DEBUG(logger, "Found info for " << itr->second);
              xmlCfg[itr->second] = obj_info;
            }
        }

    }


  ///* Find and parse Test configuration parameters
  test_params.clear();
  itemList = doc->getElementsByTagName( XMLString::transcode("Config") );
  if ( itemList->getLength() == 0 )
    {
      LOG4CPLUS_INFO (logger, "There is no Config section found. Using defaults.");
    }
  else
    {
      for (uint32_t j=0; j<itemList->getLength(); j++)
        {
          std::map<std::string, std::string> obj_info;
          std::map<std::string, std::string>::iterator itr;
          DOMNodeList *children = itemList->item(j)->getChildNodes();
          for (unsigned int i=0; i<children->getLength(); i++)
            {
              std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
              if ( children->item(i)->hasChildNodes() )
                {
                  std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
                  obj_info[paramname] = param;
                  test_params[paramname] = param;
                }
            }

          ///* Change default test name with a value from the xml config file
          itr = obj_info.find("TestName");
          if (itr != obj_info.end() )
            {
              testID = (itr->second).c_str();
              LOG4CPLUS_INFO (logger, "Test Name: " << testID);
            }

          ///* Change default the number of expected events in data file paramater with a value from the xml config file
          itr = obj_info.find("ExpectedEvents");
          if (itr != obj_info.end() )
            {
              nExpectedEvents = atoi((itr->second).c_str());
              LOG4CPLUS_INFO (logger, "Number of Expected Events: " << nExpectedEvents);
            }
         
          n_ME11_DCFEBs = -1;
          itr = obj_info.find("n_ME11_DCFEBs");
          if (itr != obj_info.end() )
            {
              n_ME11_DCFEBs = atoi((itr->second).c_str());
              LOG4CPLUS_INFO (logger, "n_ME11_DCFEBs: " << n_ME11_DCFEBs);
            }
        }
    }


  //if we got to this point, the config was loaded OK
  LOG4CPLUS_INFO (logger, "Done loading test config file: " << configFile);
  delete parser;

  bookCommonHistos();

  return 0;
}

void addMasks(TestData2D& current_mask, TestData2D new_mask)
{
  for (int i = 0; i < current_mask.Nlayers; i++)
    for (int j = 0; j < current_mask.Nbins; j++)
      if (new_mask.content[i][j] > 0 ) current_mask.content[i][j] = new_mask.content[i][j];
}

TestData2D parseMask(std::string s)
{

  TestData2D mask;
  mask.Nbins = 112;
  mask.Nlayers = 6;
  memset(mask.content, 0, sizeof (mask.content));
  memset(mask.cnts, 0, sizeof (mask.cnts));

  std::string tmp = s;
  std::string::size_type start_pos = tmp.find("(");
  std::string::size_type end_pos = tmp.find(")");
  char* stopstring = NULL;

  while ((end_pos != std::string::npos) && (start_pos != std::string::npos))
    {
      std::string range_pair = tmp.substr(start_pos+1, end_pos);
      tmp.replace(start_pos,end_pos+1,"");
      if (range_pair.find(":") != std::string::npos)
        {
          std::string layers = range_pair.substr(0,range_pair.find(":"));
          std::string chans = range_pair.substr(range_pair.find(":")+1, range_pair.length());
          int ly_start=0;
          int ly_end=0;
          int ch_start=0;
          int ch_end=0;

          // Parse layers
          if (layers.find("-") != std::string::npos)
            {
              ly_start = strtol(layers.substr(0,layers.find("-")).c_str(),  &stopstring, 10);
              ly_end = strtol(layers.substr(layers.find("-")+1,layers.length()).c_str(),  &stopstring, 10);
            }
          else
            {
              ly_start = strtol(layers.c_str(),  &stopstring, 10);
              ly_end = ly_start;
            }

          // Parse channels
          if (chans.find("-") != std::string::npos)
            {
              ch_start = strtol(chans.substr(0,chans.find("-")).c_str(),  &stopstring, 10);
              ch_end = strtol(chans.substr(chans.find("-")+1,chans.length()).c_str(),  &stopstring, 10);
            }
          else
            {
              ch_start = strtol(chans.c_str(),  &stopstring, 10);
              ch_end = ch_start;
            }


          for (int i=ly_start; i<=ly_end; i++)
            {
              for (int j=ch_start; j<= ch_end; j++)
                {
                  mask.content[i-1][j-1]=1;
                  //       std::cout << Form("mask chan %d:%d", i, j) << std::endl;
                }
            }

        }
      end_pos = tmp.find(")");
      start_pos = tmp.find("(");

    }
  return mask;
}

/***
 * Load channels mask from the masking file
 ***/
int Test_Generic::loadMasks(std::string testID)
{
  if (masksFile == "")
    {
      return 1;
    }

  XMLPlatformUtils::Initialize();
  XercesDOMParser *parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);
  parser->setDoSchema(true);
  parser->setValidationSchemaFullChecking(false); // this is default
  parser->setCreateEntityReferenceNodes(true);  // this is default
  parser->setIncludeIgnorableWhitespace (false);

  parser->parse(masksFile.c_str());
  DOMDocument *doc = parser->getDocument();
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Masks") );
  if ( l->getLength() != 1 )
    {
      //  LOG4CPLUS_ERROR (logger_, "There is not exactly one Booking node in configuration");
      return 1;
    }
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("CSCMasks") );
  if ( itemList->getLength() == 0 )
    {
      //   LOG4CPLUS_ERROR (logger_, "There no histograms to book");
      return 1;
    }
  for (uint32_t i=0; i<itemList->getLength(); i++)
    {
      std::map<std::string, std::string> obj_info;
      std::map<std::string, std::string>::iterator itr;
      DOMNodeList *children = itemList->item(i)->getChildNodes();
      for (unsigned int i=0; i<children->getLength(); i++)
        {
          std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
          if ( children->item(i)->hasChildNodes() )
            {
              std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
              obj_info[paramname] = param;
            }
        }

      /// Find chamber entry with masked channels
      itr = obj_info.find("CSC");

      /// Load cathode channels masks
      /// Filter masks using test ID and <Test> tag in xml file
      /// If there is no <Test> tag entry then load common mask
      if (itr != obj_info.end())
        {
          if ((obj_info["CFEBChans"] != "") &&
              ( (testID == "") || (obj_info["Test"] == "") || (testID == obj_info["Test"])))
            {
              LOG4CPLUS_DEBUG(logger, "Found cathode masks for " << testID << " " << itr->second << ": " << obj_info["CFEBChans"]);
              if (tmasks.find(itr->second) == tmasks.end())
                {
                  tmasks[itr->second] = parseMask(obj_info["CFEBChans"]);
                }
              else
                {
                  addMasks(tmasks[itr->second], parseMask(obj_info["CFEBChans"]));
                }
            }
        }

      /// Load anode channels masks
      /// Filter masks using test ID and <Test> tag in xml file
      /// If there is no <Test> tag entry then load common mask
      if (itr != obj_info.end())
        {
          if ((obj_info["AFEBChans"] != "") &&
              ( (testID == "") || (obj_info["Test"] == "") || (testID == obj_info["Test"]) ) )
            {
              LOG4CPLUS_DEBUG(logger, "Found anode masks for " << testID << " " << itr->second << ": " << obj_info["AFEBChans"]);
              if (amasks.find(itr->second) == amasks.end())
                {
                  amasks[itr->second] = parseMask(obj_info["AFEBChans"]);
                }
              else
                {
                  addMasks(amasks[itr->second], parseMask(obj_info["AFEBChans"]));
                }
            }
        }

    }
  delete parser;
  return 0;
}


void Test_Generic::setCSCMapFile(std::string filename)
{
  if (filename != "")
    {
      cscMapFile = filename;
      cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
    }


}


std::string Test_Generic::getCSCTypeLabel(int endcap, int station, int ring )
{
  std::string label = "Unknown";
  std::ostringstream st;
  if ((endcap > 0) && (station>0) && (ring>0))
    {
      if (endcap==1)
        {
          st << "ME+" << station << "." << ring;
          label = st.str();
        }
      else if (endcap==2)
        {
          st << "ME-" << station << "." << ring;
          label = st.str();
        }
      else
        {
          label = "Unknown";
        }
    }
  return label;
}


std::string Test_Generic::getCSCFromMap(int crate, int slot, int& csctype, int& cscposition)
{
  std::string cscid = "";
  int iendcap = -1;
  int istation = -1;
  int iring = -1;


  /// This is csc_map.txt based mapping access
  /*
  int id = cscMapping.chamber(iendcap, istation, crate, slot, -1);

  if (id==0)
  {
    return "";
  }

  CSCDetId cid( id );
  */

  ///* Mapping access replaced with slqite-based access
  ///!!! This original CMSSW code throws exception
  try
    {
      CSCDetId cid = cratemap->detId(crate, slot, 0, 0);

      iendcap = cid.endcap();
      istation = cid.station();
      iring = cid.ring();
      cscposition = cid.chamber();
    }
  catch (...)
    {
      LOG4CPLUS_ERROR (logger, "Invalid CSC Id detected -> VME crate:" << crate << ", DMB slot" << slot);
      return "";
    }

  /// Get chamber label by constructing it with endcap/station/ring/position (txt and sqlite-based mapping)
  /*
  std::string tlabel = getCSCTypeLabel(iendcap, istation, iring );
  std::map<std::string,int>::const_iterator it = tmap.find( tlabel );
  if (it != tmap.end())
  {
    csctype = it->second;
  }
  else
  {
    csctype = 0;
  }


  return tlabel+"."+Form("%02d", cscposition);
  */

  /// Get chamber label from the mapping entry chamberLabel field directly (sqlite-based mapping)
  try
    {
      int dmb = slot;
      if (dmb >= 6) --dmb;
      int id = 10*crate+dmb;
      CSCMapItem::MapItem mapitem = cratemap->item(id);
      cscid = mapitem.chamberLabel;
      std::replace(cscid.begin(), cscid.end(), '/', '.'); /// Can't use '/' in CSC name -> invalid folder name
    }
  catch (...)
    {
      LOG4CPLUS_ERROR (logger, "Invalid CSC Id detected -> VME crate:" << crate << ", DMB slot" << slot);
      return "";
    }

  return cscid;
}


std::map<int, std::string> ParseAxisLabels(std::string s)
{

  std::map<int, std::string> labels;
  std::string tmp = s;
  std::string::size_type pos = tmp.find("|");
  char* stopstring = NULL;

  while (pos != std::string::npos)
    {
      std::string label_pair = tmp.substr(0, pos);
      tmp.replace(0,pos+1,"");
      if (label_pair.find("=") != std::string::npos)
        {
          int nbin = strtol(label_pair.substr(0,label_pair.find("=")).c_str(),  &stopstring, 10);
          std::string label = label_pair.substr(label_pair.find("=")+1, label_pair.length());
          while (label.find("\'") != std::string::npos)
            {
              label.erase(label.find("\'"),1);
            }
          labels[nbin] = label;
        }
      pos = tmp.find("|");
    }
  return labels;
}

int applyCanvasParameters(TVirtualPad* cPad, TH1* h, bookParams& params)
{
  char* stopstring;
  if (cPad != NULL)
    {
      // TVirtualPad* cPad = cnv->GetUserPad();

      std::string leftMargin = params["SetLeftMargin"];
      if (leftMargin != "" )
        {
          cPad->SetLeftMargin(atof(leftMargin.c_str()));
        }
      std::string rightMargin = params["SetRightMargin"];
      if (rightMargin != "" )
        {
          cPad->SetRightMargin(atof(rightMargin.c_str()));
        }

      std::string logx = params["SetLogx"];
      if (logx!= "")
        {
          cPad->SetLogx();
        }
      std::string logy = params["SetLogy"];
      if (logy!= "" && (h->GetMaximum()>0.))
        {
          cPad->SetLogy();
        }

      std::string logz = params["SetLogz"];
      if (logz!= "" && (h->GetMaximum()>0.) )
        {
          cPad->SetLogz();
        }

      std::string gridx = params["SetGridx"];
      if (gridx!= "" )
        {
          cPad->SetGridx();
        }

      std::string gridy = params["SetGridy"];
      if (gridy!= "" )
        {
          cPad->SetGridy();
        }

      if (params["SetStats"] != "")
        {
          int stats = strtol( params["SetStats"].c_str(), &stopstring, 10 );
          h->SetStats(bool(stats));
        }

      std::string statOpt = params["SetOptStat"];
      //if (statOpt != "" ) {
      gStyle->SetOptStat(statOpt.c_str());
      // }
      return 0;
    }
  return -1;

}

int applyParameters(TH1* object, bookParams& params)
{
  char* stopstring;
  std::map<std::string, std::string>::iterator itr;
  // !!! TODO: Add object class check
  if (object != NULL)
    {
      // std::cout << "Booked " << getFullName() << std::endl;
      if (((itr = params.find("XTitle")) != params.end()) ||
          ((itr = params.find("XLabel")) != params.end()))
        {
          object->SetXTitle(itr->second.c_str());
        }
      if (((itr = params.find("YTitle")) != params.end()) ||
          ((itr = params.find("YLabel")) != params.end()))
        {
          object->SetYTitle(itr->second.c_str());
        }
      if (((itr = params.find("ZTitle")) != params.end()) ||
          ((itr = params.find("ZLabel")) != params.end()))
        {
          object->SetZTitle(itr->second.c_str());
        }

      if ((itr = params.find("SetOption")) != params.end())
        {
          object->SetOption(itr->second.c_str());
          //object->Draw();
        }

      /*
        if ((itr = params.find("SetOptStat")) != params.end()) {
        gStyle->SetOptStat(itr->second.c_str());

        }
      */

      if ((itr = params.find("SetStats")) != params.end())
        {
          int stats = strtol( itr->second.c_str(), &stopstring, 10 );
          object->SetStats(bool(stats));
        }


      object->SetFillColor(48);

      if ((itr = params.find("SetFillColor")) != params.end())
        {
          int color = strtol( itr->second.c_str(), &stopstring, 10 );
          object->SetFillColor(color);
        }

      if ((itr = params.find("SetXLabels")) != params.end())
        {
          std::map<int, std::string> labels = ParseAxisLabels(itr->second);
          for (std::map<int, std::string>::iterator l_itr = labels.begin(); l_itr != labels.end(); ++l_itr)
            {
              object->GetXaxis()->SetBinLabel(l_itr->first, l_itr->second.c_str());
            }


        }

      if ((itr = params.find("SetYLabels")) != params.end())
        {
          std::map<int, std::string> labels = ParseAxisLabels(itr->second);
          for (std::map<int, std::string>::iterator l_itr = labels.begin(); l_itr != labels.end(); ++l_itr)
            {
              object->GetYaxis()->SetBinLabel(l_itr->first, l_itr->second.c_str());
            }
        }
      if ((itr = params.find("LabelsOption")) != params.end())
        {
          std::string st = itr->second;
          if (st.find(",") != std::string::npos)
            {
              std::string opt = st.substr(0,st.find(",")) ;
              std::string axis = st.substr(st.find(",")+1,st.length());
              object->LabelsOption(opt.c_str(),axis.c_str());
            }
        }


      if ((itr = params.find("SetLabelSize")) != params.end())
        {
          std::string st = itr->second;
          if (st.find(",") != std::string::npos)
            {
              double opt = atof(st.substr(0,st.find(",")).c_str()) ;
              std::string axis = st.substr(st.find(",")+1,st.length());
              object->SetLabelSize(opt,axis.c_str());
            }
        }
      if ((itr = params.find("SetTitleOffset")) != params.end())
        {
          std::string st = itr->second;
          if (st.find(",") != std::string::npos)
            {
              double opt = atof(st.substr(0,st.find(",")).c_str()) ;
              std::string axis = st.substr(st.find(",")+1,st.length());
              object->SetTitleOffset(opt,axis.c_str());
            }
        }
      if ((itr = params.find("SetMinimum")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          object->SetMinimum(opt);
        }
      if ((itr = params.find("SetMaximum")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          object->SetMaximum(opt);
        }

      if ((itr = params.find("SetNdivisionsX")) != params.end())
        {
          int opt = strtol( itr->second.c_str(), &stopstring, 10 );
          if (object)
            {
              object->SetNdivisions(opt,"X");
              object->GetXaxis()->CenterLabels(true);
            }

        }

      if ((itr = params.find("SetNdivisionsY")) != params.end())
        {
          int opt = strtol( itr->second.c_str(), &stopstring, 10 );
          if (object)
            {
              object->SetNdivisions(opt,"Y");
              object->GetYaxis()->CenterLabels(true);
            }
        }


      if ((itr = params.find("SetTickLengthX")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          if (object)
            {
              object->SetTickLength(opt,"X");
            }
        }

      if ((itr = params.find("SetTickLengthY")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          if (object)
            {
              object->SetTickLength(opt,"Y");
            }
        }

      if ((itr = params.find("SetLabelSizeX")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          if (object)
            {
              object->SetLabelSize(opt,"X");

            }
        }

      if ((itr = params.find("SetLabelSizeY")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          if (object)
            {
              //        object->GetYaxis()->SetLabelSize(opt);
              object->SetLabelSize(opt,"Y");
            }
        }
      if ((itr = params.find("SetErrorOption")) != params.end())
        {
          std::string st = itr->second;
          if (object)
            {
              reinterpret_cast<TProfile*>(object)->SetErrorOption(st.c_str());
            }
        }


      if ((itr = params.find("SetLabelSizeZ")) != params.end())
        {
          std::string st = itr->second;
          double opt = atof(st.c_str()) ;
          if (object)
            {
              //        object->GetZaxis()->SetLabelSize(opt);
              object->SetLabelSize(opt,"Z");
              /*
                TPaletteAxis *palette = (TPaletteAxis*)object->GetListOfFunctions()->FindObject("palette");
                if (palette != NULL) {
                palette->SetLabelSize(opt);
                }
              */
            }
        }

    }
  return 0;
}

void Test_Generic::bookCommonHistos()
{
  MonHistos emuhistos;
  emucnvs.clear();
  tlimits.clear();
  char *stopstring;
  for (testParamsCfg::iterator itr=xmlCfg.begin(); itr != xmlCfg.end(); ++itr)
    {
      bookParams& params = itr->second;
      if (params.find("Type") != params.end())
        {
          std::string cnvtype = params["Type"];
          std::string scope = params["Prefix"];
          std::string tname = params["Name"];

          std::string name = "sum_"+testID+"_"+tname;
          std::string title = "Summary: "+testID+" "+params["Title"];
          double xmin=0., xmax=0.;
          int xbins=0;
          double ymin=0., ymax=0.;
          int ybins=0;
          std::string xtitle = params["XTitle"];
          std::string ytitle = params["YTitle"];
          double low0limit=0., low1limit=0.;
          double high0limit=0., high1limit=0.;
          if (params["XMin"] != "")
            {
              xmin = atof(params["XMin"].c_str());
            }
          if (params["XMax"] != "")
            {
              xmax = atof(params["XMax"].c_str());
            }
          if (params["YMin"] != "")
            {
              ymin = atof(params["YMin"].c_str());
            }
          if (params["YMax"] != "")
            {
              ymax = atof(params["YMax"].c_str());
            }
          if (params["XBins"] != "")
            {
              xbins = strtol(params["XBins"].c_str(), &stopstring, 10);
            }
          if (params["YBins"] != "")
            {
              ybins = strtol(params["YBins"].c_str(), &stopstring, 10);
            }

          if ((cnvtype == "strips_cnv") || (cnvtype == "wires_cnv") || (cnvtype == "cfeb_cnv") || (cnvtype == "halfstrips_cnv") || (cnvtype == "mwires_cnv") || (cnvtype == "gasgain_cnv"))
            {
              /*
                  // = Set actual number of strips depending on Chamber type
                  xbins = getNumStrips(cscID);
                  xmax = getNumStrips(cscID);
              */

              if (params["Low0Limit"] != "")
                {
                  low0limit = atof(params["Low0Limit"].c_str());
                }
              if (params["Low1Limit"] != "")
                {
                  low1limit = atof(params["Low1Limit"].c_str());
                }
              if (params["High0Limit"] != "")
                {
                  high0limit = atof(params["High0Limit"].c_str());
                }
              if (params["High1Limit"] != "")
                {
                  high1limit = atof(params["High1Limit"].c_str());
                }

              tlimits[tname].low0 = low0limit;
              tlimits[tname].low1 = low1limit;
              tlimits[tname].high0 = high0limit;
              tlimits[tname].high1 = high1limit;


              // TestCanvas_6gr1h* cnv = new TestCanvas_6gr1h((cscID+"_CFEB02_R03").c_str(), (cscID+": CFEB02 R03").c_str(),80, 0.0, 80.0, 60, 0., 6.0);
              // TH1F* h = new TH1F(name.c_str(), title.c_str(), ybins, ymin, ymax);
              TestCanvas_1h* cnv = new TestCanvas_1h(name.c_str(), title.c_str(), ybins, ymin, ymax);
              cnv->SetXTitle(xtitle.c_str());
              cnv->SetYTitle(ytitle.c_str());
              cnv->AddTextTest(testID);
              cnv->AddTextResult(params["Title"]);
              cnv->SetLimits(low1limit,low0limit, high0limit, high1limit);

              emucnvs[itr->first]=cnv;
            }
          if ((cnvtype.find("h") == 0 && !(cnvtype.find("half") == 0)) && (scope=="EMU"))
            {
              // std::cout << "Booking " << name << std::endl;
              if (cnvtype.find("h1") != std::string::npos)
                {
                  emuhistos[itr->first] = new TH1F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
                }
              else if (cnvtype.find("h2") != std::string::npos)
                {
                  emuhistos[itr->first] = new TH2F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);
                }
              else if (cnvtype.find("hp") != std::string::npos)
                {
                  emuhistos[itr->first] = new TProfile((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
                }
              applyParameters(emuhistos[itr->first], params);
            }


        }
    }
  /*
    hSummaryFormatErrors = new TH2F(("sum_"+testID+"_FormatErrors").c_str(), "CSCs with Format Errors", 36, 1, 37, 18, 0, 16);
    hSummaryFormatErrors->SetOption("colz");
    hSummaryFormatErrors->SetNdivisionsX(36);
    hSummaryFormatErrors->SetLabelSizeX(0.02);
    hSummaryFormatErrors->SetLabelSizeZ(0.02);
  */

  mhistos["EMU"] = emuhistos;

}

void Test_Generic::bookTestsForCSC(std::string cscID)
{
  MonHistos cschistos;
  cschistos.clear();
  TestCanvases csccnvs;
  char *stopstring;

  ResultsCodes rcodes;

  for (testParamsCfg::iterator itr=xmlCfg.begin(); itr != xmlCfg.end(); ++itr)
    {
      bookParams& params = itr->second;
      if (params.find("Type") != params.end())
        {
          std::string cnvtype = params["Type"];
          std::string scope = params["Prefix"];
          std::string name = cscID+"_"+testID+"_"+params["Name"];
          std::string title = cscID+": "+testID+" "+params["Title"];
          double xmin=0., xmax=0.;
          int xbins=0;
          double ymin=0., ymax=0., ymax2=0.;
          int ybins=0;
          std::string xtitle = params["XTitle"];
          std::string ytitle = params["YTitle"];
          double low0limit=0., low1limit=0.;
          double high0limit=0., high1limit=0.;
          double high0limit2=0., high1limit2=0.;

          if (params["XMin"] != "")
            {
              xmin = atof(params["XMin"].c_str());
            }
          if (params["XMax"] != "")
            {
              xmax = atof(params["XMax"].c_str());
            }
          if (params["YMin"] != "")
            {
              ymin = atof(params["YMin"].c_str());
            }
          if (params["YMax"] != "")
            {
              ymax = atof(params["YMax"].c_str());
            }
          if (params["XBins"] != "")
            {
              xbins = strtol(params["XBins"].c_str(), &stopstring, 10);
            }
          if (params["YBins"] != "")
            {
              ybins = strtol(params["YBins"].c_str(), &stopstring, 10);
            }
          if (params["YMax2"] != "")
            {
              ymax2 = atof(params["YMax2"].c_str());
            }
          else
            {
              ymax2 = 0;
            }

          if(isME11(cscID))
            {
              if (params["XMin_ME11"] != "")
                {
                  xmin = atof(params["XMin_ME11"].c_str());
                }
              if (params["XMax_ME11"] != "")
                {
                  xmax = atof(params["XMax_ME11"].c_str());
                }
              if (params["YMin_ME11"] != "")
                {
                  ymin = atof(params["YMin_ME11"].c_str());
                }
              if (params["YMax_ME11"] != "")
                {
                  ymax = atof(params["YMax_ME11"].c_str());
                }
              if (params["XBins_ME11"] != "")
                {
                  xbins = strtol(params["XBins_ME11"].c_str(), &stopstring, 10);
                }
              if (params["YBins_ME11"] != "")
                {
                  ybins = strtol(params["YBins_ME11"].c_str(), &stopstring, 10);
                }
            }

          if (params["XScale"] != "")
            {
              if (params["XScale"] == "wires")
                {
                  xmin=0;
                  xmax=getNumWireGroups(cscID);
                  xbins=getNumWireGroups(cscID);
                }
              if (params["XScale"] == "strips")
                {
                  xmin=0;
                  xmax=getNumStrips(cscID);
                  xbins=getNumStrips(cscID);
                }
              if (params["XScale"] == "halfstrips")
                {
                  xmin=0;
                  xmax=getNumStrips(cscID)*2;
                  xbins=getNumStrips(cscID)*2;
                }
              // params["SetNdivisionsX"]=Form("%d",(int)xbins);;
            }

          if (params["YScale"] != "")
            {
              if (params["YScale"] == "wires")
                {
                  ymin=0;
                  ymax=getNumWireGroups(cscID);
                  ybins=getNumWireGroups(cscID);
                }
              if (params["YScale"] == "strips")
                {
                  ymin=0;
                  ymax=getNumStrips(cscID);
                  ybins=getNumStrips(cscID);
                }
              if (params["YScale"] == "halfstrips")
                {
                  ymin=0;
                  ymax=getNumStrips(cscID)*2;
                  ybins=getNumStrips(cscID)*2;
                }
              // params["SetNdivisionsY"]=Form("%d",(int)ybins);
            }

          if ((cnvtype == "strips_cnv") || (cnvtype == "wires_cnv") || (cnvtype == "cfeb_cnv") || (cnvtype == "halfstrips_cnv") || (cnvtype == "mwires_cnv") || (cnvtype == "gasgain_cnv"))
            {
              if (cnvtype == "strips_cnv")
                {
                  // = Set actual number of strips depending on Chamber type
                  xbins = getNumStrips(cscID);
                  xmax = getNumStrips(cscID);
                  if (isME11(cscID) && n_ME11_DCFEBs > 0)
                    {
                      int nDCFEBs =  n_ME11_DCFEBs;
                      if (nDCFEBs > 7 ) nDCFEBs = 7;
                      if (nDCFEBs > 0 ) { xbins = nDCFEBs*16; xmax = xbins; }
                    }
                }
              if (cnvtype == "halfstrips_cnv")
                {
                  // = Set actual number of strips depending on Chamber type
                  xbins = getNumStrips(cscID)*2;
                  xmax = getNumStrips(cscID)*2;
                  if (isME11(cscID) && params["n_ME11_TMB_DCFEBs"] != "")
                    {
                      int nDCFEBs =  atof(params["n_ME11_TMB_DCFEBs"].c_str());
                      LOG4CPLUS_INFO(logger, "Detected n_ME11_TMB_DCFEBs and it's ME11, use the value "<< nDCFEBs);
                      if (nDCFEBs >7 ) nDCFEBs = 7;
                      if (nDCFEBs > 0 )
                        {
                          xbins = nDCFEBs*16*2;
                          xmax = xbins;
                        }
                    }
                }
              if (cnvtype == "wires_cnv" || (cnvtype == "mwires_cnv"))
                {
                  // = Set actual number of wiregroups depending on Chamber type
                  xbins = getNumWireGroups(cscID);
                  xmax = getNumWireGroups(cscID);
                }
              if (cnvtype == "cfeb_cnv")
                {
                  // = Set actual number of wiregroups depending on Chamber type
                  xbins = getNumStrips(cscID)/16;
                  xmax = getNumStrips(cscID)/16;
                }
              if(cnvtype == "gasgain_cnv")
                {
                  int ncfebs=getNumStrips(cscID)/16;
                  int nsegments=5;
                  xbins=ncfebs*nsegments;
                  xmax=ncfebs*nsegments;
                }



              if (params["Low0Limit"] != "")
                {
                  low0limit = atof(params["Low0Limit"].c_str());
                }
              if (params["Low1Limit"] != "")
                {
                  low1limit = atof(params["Low1Limit"].c_str());
                }
              if (params["High0Limit"] != "")
                {
                  high0limit = atof(params["High0Limit"].c_str());
                }
              if (params["High1Limit"] != "")
                {
                  high1limit = atof(params["High1Limit"].c_str());
                }
              if (params["High0Limit2"] != "")
                {
                  // for Test 25
                  high0limit2 = atof(params["High0Limit2"].c_str());
                }
              else high1limit2 = 0;
              if (params["High0Limit2"] != "")
                {
                  // for Test 25
                  high1limit2 = atof(params["High1Limit2"].c_str());
                }
              else high1limit2 = 0;


              if(isME11(cscID))
                {
                  if (params["Low0Limit_ME11"] != "")
                    {
                      low0limit = atof(params["Low0Limit_ME11"].c_str());
                    }
                  if (params["Low1Limit_ME11"] != "")
                    {
                      low1limit = atof(params["Low1Limit_ME11"].c_str());
                    }
                  if (params["High0Limit_ME11"] != "")
                    {
                      high0limit = atof(params["High0Limit_ME11"].c_str());
                    }
                  if (params["High1Limit_ME11"] != "")
                    {
                      high1limit = atof(params["High1Limit_ME11"].c_str());
                    }
                }

              // TestCanvas_6gr1h* cnv = new TestCanvas_6gr1h((cscID+"_CFEB02_R03").c_str(), (cscID+": CFEB02 R03").c_str(),80, 0.0, 80.0, 60, 0., 6.0);
              TestCanvas_6gr1h* cnv = new TestCanvas_6gr1h(name, title,xbins, xmin, xmax, ybins, ymin, ymax, cnvtype, ymax2);
              cnv->SetCanvasType(cnvtype);
              cnv->SetXTitle(xtitle);
              cnv->SetYTitle(ytitle);
              cnv->AddTextTest(testID);
              cnv->AddTextResult(params["Title"]);
              cnv->SetLimits(low1limit,low0limit, high0limit, high1limit, high0limit2, high1limit2);
              csccnvs[itr->first]=cnv;
            }

          if ((cnvtype.find("h") == 0 && !(cnvtype.find("half") == 0)) && (scope=="CSC"))
            {
              if (cnvtype.find("h1") != std::string::npos)
                {
                  cschistos[itr->first] = new TH1F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
                }
              else if (cnvtype.find("h2") != std::string::npos)
                {
                  cschistos[itr->first] = new TH2F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);
                }
              else if (cnvtype.find("hp") != std::string::npos)
                {
                  cschistos[itr->first] = new TProfile((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
                }
              applyParameters(cschistos[itr->first], params);
            }


        }
    }

  tcnvs[cscID] = csccnvs;

  hFormatErrors[cscID] = new TH2F((cscID+"_"+testID+"_FormatErrors").c_str(), "CSC Format Errors", 1, 0, 1, 20, 0, 20);
  hFormatErrors[cscID]->SetOption("textcolz");

  mhistos[cscID]=cschistos;

  rescodes[cscID] = rcodes;

  //  return cschistos;
}


void Test_Generic::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{
  nTotalEvents++;

  //   uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  if ( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 )
    {
      //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
      const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
      tmp = dduTrailer;
      bin_checker.check(tmp,uint32_t(4));
    }

  if (bin_checker.errors() != 0)
    {
      // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
      doBinCheck();
      return;
    }

  CSCDDUEventData dduData((uint16_t *) data);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();

  for (std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
       chamberDataItr != chamberDatas.end(); ++chamberDataItr)
    {
      analyzeCSC(*chamberDataItr);
    }

}


void Test_Generic::doBinCheck()
{

  std::map<CSCIdType, ExaminerStatusType> checkerErrors = bin_checker.errorsDetailed();
  std::map<CSCIdType, ExaminerStatusType>::const_iterator chamber = checkerErrors.begin();
  while ( chamber != checkerErrors.end() )
    {
      // int ChamberID     = chamber->first;
      int CrateID = (chamber->first>>4) & 0xFF;
      int DMBSlot = chamber->first & 0xF;
      if ((CrateID ==255) ||
          (chamber->second & 0x80))
        {
          chamber++;    // = Skip chamber detection if DMB header is missing (Error code 6)
          continue;
        }

      int CSCtype   = 0;
      int CSCposition = 0;
      std::string cscID = getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
      if (cscID == "")
        {
          chamber++;
          continue;
        }

      addCSCtoMap(cscID, CrateID, DMBSlot);

      cscTestData::iterator td_itr = tdata.find(cscID);
      if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
        {
          LOG4CPLUS_INFO(logger, "Found " << cscID);
          initCSC(cscID);
          //      mhistos[cscID] = bookMonHistosCSC(cscID);
        }

      bool isCSCError = false;
      if (hFormatErrors[cscID])
        {
          for (int bit=5; bit<24; bit++)
            if ( chamber->second & (1<<bit) )
              {
                isCSCError = true;
                hFormatErrors[cscID]->Fill(0.,bit-5);
              }
        }

      if (isCSCError && CSCtype && CSCposition && mhistos["EMU"]["E00"])
        {

          mhistos["EMU"]["E00"]->Fill(CSCposition, CSCtype);
          //    mo->SetEntries(nBadEvents);

        }

      if (isCSCError)
        {
          LOG4CPLUS_WARN(logger, "Evt#" << std::dec << nTotalEvents << "> " << cscID
                         << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << chamber->second << std::dec);
          nCSCBadEvents[cscID]++;
        }
      chamber++;
    }
}


void Test_Generic::finish()
{

  // char* stopstring;
  struct tm* clock;
  struct stat attrib;
  stat(dataFile.c_str(), &attrib);
  clock = localtime(&(attrib.st_mtime));
  time_t now = mktime(clock);
  std::string dataTime=timestr(&now); //asctime(clock);
  now = time(NULL);
  //  clock = localtime(&now);
  std::string testTime=timestr(&now);//  asctime(clock);

  std::string rpath = "Test_"+testID+"/"+outDir;
  TString command = Form("mkdir -p %s", rpath.c_str());
  gSystem->Exec(command.Data());
  // == Need to clean directory from old chamber data
  command = Form("rm -rf %s/ME*", rpath.c_str());
  gSystem->Exec(command.Data());
  // std::ofstream fres((rpath+"/test_results.js").c_str());
  int res=0;
  int sum_res=res;

  gStyle->SetPalette(1,0);

  std::string filename="Test_"+testID+"/"+rootFile;
  TFile root_res(filename.c_str(), "recreate");
  if (!root_res.IsZombie())
    {
      root_res.cd();
      TDirectory* f = root_res.mkdir(("Test_"+testID).c_str());


      for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr)
        {
          bool fEnoughData = true;
          sum_res=0;
          std::string cscID = td_itr->first;

          ResultsCodes& rcodes = rescodes[cscID];
          ResultsCodes::iterator r_itr;

          TDirectory * rdir = f->mkdir((cscID).c_str());
          std::string path = rpath+"/"+cscID+"/";
          TString command = Form("mkdir -p %s", path.c_str());
          gSystem->Exec(command.Data());
          std::ofstream csc_fres((path+"/test_results.js").c_str());

          // fres << "['"+cscID+"',[" << std::endl;
          csc_fres << "['"+cscID+"',[" << std::endl;

          rdir->cd();

          TH1F* mo = reinterpret_cast<TH1F*>(mhistos[cscID]["V00"]);
          double max_freq=0.;
          if (mo && hFormatErrors[cscID])
            {
              uint32_t nTotalEvents = nCSCEvents[cscID]+nCSCBadEvents[cscID];
              for (int bit=1; bit<=20; bit++)
                {
                  double freq = (100.0*hFormatErrors[cscID]->GetBinContent(1,bit))/nTotalEvents;
                  if (freq>0) mo->SetBinContent(bit, freq);
                  if (freq>max_freq) max_freq=freq;
                }
              mo->SetEntries(nTotalEvents);
              if (max_freq>0) mo->SetMaximum(max_freq);
              else mo->SetMaximum(1);
            }

          LOG4CPLUS_INFO(logger, "Finalize " << testID << " analysis for " << cscID);
          finishCSC(cscID);


          LOG4CPLUS_INFO(logger, "Save " << testID << " results for " << cscID);

          if (nCSCEvents[cscID] < nExpectedEvents/2)
            {
              sum_res = 4;
              fEnoughData = false;
              rcodes["V00"] = sum_res;
            }

          TestData& cscdata= td_itr->second;
          TestData2D& mask = cscdata["_MASK"];

          TestCanvases& cnvs = tcnvs[cscID];
          TestCanvases::iterator c_itr;
          for (c_itr=cnvs.begin(); c_itr != cnvs.end(); ++c_itr)
            {
              std::string subtestID = c_itr->first;
              TestCanvas_6gr1h* cnv = dynamic_cast<TestCanvas_6gr1h*>(c_itr->second);
              TestData::iterator itr = cscdata.find(subtestID);
              if (itr != cscdata.end())
                {
                  TestData2D& data = itr->second;

                  cnv->AddTextDatafile(dataFile);
                  cnv->AddTextRun(dataTime);
                  cnv->AddTextAnalysis(testTime +", version " + ANALYSIS_VER);
                  if (fEnoughData)
                    {
                      res=cnv->Fill(data,mask);
                      if (res>sum_res) sum_res=res;
                      // fres << "\t['" << itr->first << "','" << res << "']," << std::endl;
                      csc_fres << "\t['" << itr->first << "','" << res << "']," << std::endl;
                    }
                  cnv->Draw();
                  cnv->SetCanvasSize(imgW, imgH);
                  cnv->SaveAs(path+cscID+"_"+testID+"_"+subtestID+".png");
                  // cnv->Write();
                  if (emucnvs[subtestID] != NULL)
                    {
                      TestCanvas_1h* emucnv = dynamic_cast<TestCanvas_1h*>(emucnvs[subtestID]);
                      emucnv->GetHisto()->Add(cnv->GetHisto());
                    }

                  // == Save results to text file
                  int l0_cnt=0, l1_cnt=0, h0_cnt=0, h1_cnt=0;
                  std::vector<double> limits = cnv->GetLimits();
                  std::ofstream text_res((path+cscID+"_"+testID+"_"+subtestID+".results").c_str());
                  // text_res << cscID << " " << testID << " " << subtestID << std::endl;
                  text_res << cnv->GetTitle() << std::endl;
                  text_res << "Datafile: " << dataFile << " (" << dataTime << ")" << std::endl;
                  text_res << "Analysis ver." << ANALYSIS_VER << " Time: " << testTime <<  std::endl;
                  text_res << "Total Events: " << nCSCEvents[cscID] << " Rejected: " << nCSCBadEvents[cscID] << std::endl;
                  text_res << "Limits: L1=" << limits[0] << ", L0="<< limits[1] << ", H0="<< limits[2]<<", H1=" << limits[3] << std::endl;
                  //    if (fEnoughData) {
                  std::string cnvtype = cnv->GetCanvasType();
                  if (cnvtype == "strips_cnv") text_res <<  "Layer Strip    Value Status Masked" << std::endl;
                  else if (cnvtype == "halfstrips_cnv") text_res <<  "Layer  HalfStrip    Value Status Masked" << std::endl;
                  else if (cnvtype == "wires_cnv") text_res <<  "Layer  Wire    Value Status Masked" << std::endl;
                  else if (cnvtype == "mwires_cnv") text_res <<  "m/6 Planes  Wire    Value Status Masked" << std::endl;
                  else if (cnvtype == "cfeb_cnv") text_res <<  "Layer  CFEB    Value Status Masked" << std::endl;
                  else if (cnvtype == "afeb_cnv") text_res <<  "AFEB    Value Status Masked" << std::endl;
                  else if (cnvtype == "gasgain_cnv") text_res <<  "GasGain    Value Status Masked" << std::endl;
                  else text_res <<  "Layer  Chan    Value Status Masked" << std::endl;
                  for (int i=0; i<data.Nlayers; i++)
                    {
                      for (int j=0; j<data.Nbins; j++)
                        {
                          //          text_res << std::fixed << std::setprecision(2) << std::setw(5) << (i+1) << std::setw(6) << (j+1)
                          //      << std::setw(9) << data.content[i][j] << std::endl;
                          std::string validity="OK";
                          if (data.content[i][j] < limits[0])
                            {
                              validity="L1";
                              if (!mask.content[i][j]) l0_cnt++;
                            }
                          else if (data.content[i][j] < limits[1])
                            {
                              validity="L0";
                              if (!mask.content[i][j]) l1_cnt++;
                            }
                          else if (data.content[i][j] > limits[3])
                            {
                              validity="H1";
                              if (!mask.content[i][j]) h1_cnt++;
                            }
                          else if (data.content[i][j] > limits[2])
                            {
                              validity="H0";
                              if (!mask.content[i][j]) h0_cnt++;
                            }
                          // if (validity != "OK") failed_cnt++;
                          int prec=2;
                          if (fabs(data.content[i][j]) < 1) prec = 3;
                          if (fabs(data.content[i][j]) < 0.01) prec = 5;
                          text_res << std::fixed << std::setprecision(prec) << std::setw(5) << (i+1) << std::setw(6) << (j+1)
                                   << std::setw(9) << data.content[i][j] << std::setw(7) << validity << std::setw(7) << (int)(mask.content[i][j]) << std::endl;
                        }
                    }
                  text_res <<  "Out of range counters: L1=" << l1_cnt << ", L0="<< l0_cnt << ", H0="<< h0_cnt <<", H1=" << h1_cnt << std::endl;
                  //    }
                  text_res << "TEST STATUS: ";
                  if (!fEnoughData)
                    {
                      text_res << "FAILED NOT ENOUGH DATA";
                    }
                  else if ((l0_cnt+h0_cnt)>0)
                    {
                      text_res << "FAILED";
                    }
                  else if ((l1_cnt+h1_cnt)>0)
                    {
                      text_res << "PASSED WITH WARNINGS";
                    }
                  else
                    {
                      text_res << "PASSED";
                    }
                  text_res << std::endl;

                }
            }

          MonHistos& monhistos = mhistos[cscID];
          MonHistos::iterator m_itr;
          for (m_itr=monhistos.begin(); m_itr!=monhistos.end(); ++m_itr)
            {
              std::string subtestID = m_itr->first;
              bookParams& params =  xmlCfg[subtestID];
              std::string descr = params["Title"];

              TStyle defStyle(*gStyle);
              /*  MonitoringCanvas* cnv= new MonitoringCanvas((cscID+"_"+testID+"_"+subtestID).c_str(), (cscID+"_"+testID+"_"+subtestID).c_str(),
                (cscID + " "+testID+" "+descr).c_str() ,
                1, 1, imgW, imgH);

              */
              TestCanvas_userHisto* cnv = new TestCanvas_userHisto((cscID+"_"+testID+"_"+subtestID).c_str(),
                  (cscID + " "+testID+" "+descr).c_str());
              /*
                TestCanvas_1h* cnv = new TestCanvas_1h((cscID+"_"+testID+"_"+subtestID).c_str(),
                (cscID + " "+testID+" "+descr).c_str(), 70, 300.0, 1000.0);
              */
              cnv->SetCanvasSize(imgW, imgH);
              cnv->AddTextTest(testID);
              cnv->AddTextResult(params["Title"]);
              cnv->AddTextDatafile(dataFile);
              cnv->AddTextRun(dataTime);
              cnv->AddTextAnalysis(testTime +", version " + ANALYSIS_VER);
              cnv->AddTextEvents(Form("%d",nCSCEvents[cscID]));
              cnv->AddTextBadEvents(Form("%d",nCSCBadEvents[cscID]));

              TVirtualPad* cPad = cnv->GetUserPad();
              applyCanvasParameters(cPad, m_itr->second, params);

              gStyle->SetPalette(1,0);

              //std::cout << "subtestID: " << subtestID << std::endl;
              //std::cout << "params: " << params << std::endl;
              //  m_itr->second->Draw();

              // if (m_itr->second != NULL)
              cnv->SetHistoObject(m_itr->second);

              // Set results code for user histograms
              r_itr = rcodes.find(subtestID);
              if (r_itr != rcodes.end())
                {
                  res = r_itr->second;
                  csc_fres << "\t['"<< subtestID << "','" << res << "']," << std::endl;
                  cnv->SetResultCode(res);
                  // if (res > sum_res) sum_res = res;
                }

              cnv->Draw();
              cnv->SaveAs((path+cscID+"_"+testID+"_"+subtestID+".png").c_str());
              //  cnv->Print((path+cscID+"_"+testID+"_"+subtestID+".png").c_str());
              m_itr->second->Write();
              // cnv->Write();
              delete cnv;
              defStyle.cd();
            }

          nCSCEvents["EMU"] += nCSCEvents[cscID];
          nCSCEvents["EMU"] += nCSCBadEvents[cscID];

          f->cd();


          // fres << "\t['SUMMARY','" << sum_res << "']" << std::endl;
          // fres << "]],"<< std::endl;
          csc_fres << "\t['SUMMARY','" << sum_res << "']" << std::endl;
          csc_fres << "]],"<< std::endl;


        }

      // == Save common/summary histos
      TDirectory * rdir = f->mkdir("EMU");
      std::string path = rpath+"/EMU/";
      TString command = Form("mkdir -p %s", path.c_str());
      gSystem->Exec(command.Data());
      rdir->cd();
      for (TestCanvases::iterator c_itr=emucnvs.begin(); c_itr != emucnvs.end(); ++c_itr)
        {
          std::string subtestID = c_itr->first;
          TestCanvas_1h* cnv = dynamic_cast<TestCanvas_1h*>(c_itr->second);
          if ((cnv != NULL) && (cnv->GetHisto() != NULL))
            {
              cnv->SetHistoObject(cnv->GetHisto());
              cnv->AddTextDatafile(dataFile);
              cnv->AddTextRun(dataTime);
              cnv->AddTextAnalysis(testTime +", version " + ANALYSIS_VER);
              cnv->Draw();
              cnv->SetCanvasSize(imgW, imgH);
              cnv->SaveAs(path+"sum_"+testID+"_"+subtestID+".png");
              // cnv->Write();
              cnv->GetHisto()->Write();
            }

        }

      MonHistos& emuhistos = mhistos["EMU"];
      for (MonHistos::iterator h_itr=emuhistos.begin(); h_itr!=emuhistos.end(); ++h_itr)
        {
          std::string subtestID = h_itr->first;
          bookParams& params =  xmlCfg[subtestID];
          std::string descr = params["Title"];

          TStyle defStyle(*gStyle);
          TestCanvas_userHisto* cnv = new TestCanvas_userHisto(("Sum_"+testID+"_"+subtestID).c_str(),
              ("Summary "+testID+" "+descr).c_str());
          cnv->SetCanvasSize(imgW, imgH);
          cnv->AddTextTest(testID);
          cnv->AddTextResult(params["Title"]);
          cnv->AddTextDatafile(dataFile);
          cnv->AddTextRun(dataTime);
          cnv->AddTextAnalysis(testTime +", version " + ANALYSIS_VER);
          cnv->AddTextEvents(Form("%d",nCSCEvents["EMU"]));
          cnv->AddTextBadEvents(Form("%d",nCSCBadEvents["EMU"]));
          TVirtualPad* cPad = cnv->GetUserPad();
          applyCanvasParameters(cPad, h_itr->second, params);
          gStyle->SetPalette(1,0);

          cnv->SetHistoObject(h_itr->second);
          cnv->Draw();
          cnv->SaveAs((path+"sum_"+testID+"_"+subtestID+".png").c_str());
          h_itr->second->Write();
          // cnv->Write();
          delete cnv;

        }
      f->cd();
      root_res.Close();
    }
  //  saveCSCList();

}

int Test_Generic::checkChannel(TestData& cscdata, std::vector<std::string>& tests, int layer, int strip)
{
  for (unsigned i = 0; i < tests.size(); i++)
    {
      if ((cscdata[tests[i]].content[layer][strip] > tlimits[tests[i]].high1)
          || (cscdata[tests[i]].content[layer][strip] < tlimits[tests[i]].low1))
        return 1;
    }

  return 0;
}


double Test_Generic::checkChannelConstant(std::string test, double value, double threshold)
{
  if ((value > threshold*tlimits[test].high1)
      || (value < threshold*tlimits[test].low1))  return BAD_VALUE;
  return value;
}

void Test_Generic::addCSCtoMap(std::string cscid, int dmbcrate, int dmbslot)
{
  if (cscid != "")
    {
      CSCtoHWmap::iterator itr = cscmap.find(cscid);
      if (itr==cscmap.end() || cscmap.size() == 0)
        {
          cscmap[cscid]=std::make_pair(dmbcrate, dmbslot);
        }
    }

}


void Test_Generic::saveCSCList()
{
  //   struct tm* clock;
  struct stat attrib;
  stat(dataFile.c_str(), &attrib);
  // clock = localtime(&(attrib.st_mtime));
  std::string dataTime=ctime(&(attrib.st_mtime));//asctime(clock);
  dataTime = dataTime.substr(0,dataTime.find("\n",0));

  std::string path = "Test_"+testID+"/"+outDir;

  std::ofstream csc_list((path+"/csc_list.js").c_str());
  csc_list << "var CSC_LIST=[\n\t['"<< dataFile << "','"<< dataTime << "',"<< std::endl;
  for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr)
    {
      std::string cscID = td_itr->first;
      csc_list << "'"<< cscID << "'," << std::endl;
    }
  csc_list<< "]]" << std::endl;
}

