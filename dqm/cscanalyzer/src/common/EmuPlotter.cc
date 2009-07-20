
#include "emu/dqm/cscanalyzer/EmuPlotter.h"

EmuPlotter::EmuPlotter():logger_(Logger::getInstance("EmuPlotter")),appBSem_(toolbox::BSem::FULL)
{
  init();
}


EmuPlotter::EmuPlotter(Logger logger):logger_(logger),appBSem_(toolbox::BSem::FULL)
{
  init();
}

void EmuPlotter::init()
{
  debug = false;
  xmlHistosBookingCfgFile = "";
  xmlCanvasesCfgFile = "";
  cscMapFile = "";
  tmap = getCSCTypeToBinMap();
  dduCheckMask = 0xFFFFFFFF;
  binCheckMask = 0xFFFFFFFF;
  dduBinCheckMask = 0x02080016;
  reset();
}

void EmuPlotter::reset()
{
  appBSem_.take();
  fFirstEvent = true;
  nEvents = 0;
  nBadEvents = 0;
  nGoodEvents = 0;
  nCSCEvents = 0;
  nDetectedCSCs = 0;
  L1ANumber = 0;
  BXN = 0;
  eTag = "";
  runNumber="";
  bin_checker.output1().hide();
  bin_checker.output2().hide();
  bin_checker.crcALCT(true);
  bin_checker.crcTMB (true);
  bin_checker.crcCFEB(true);
  bin_checker.modeDDU(true);
//  bin_checker.output1().show();
//  bin_checker.output2().show();

  nDMBEvents.clear();
  L1ANumbers.clear();
  cscCounters.clear();

  clearCanvasesCollection(MECanvases);
  clearMECollection(MEs);
  appBSem_.give();
}


void EmuPlotter::clearMECollection(std::map<std::string, ME_List > & collection)
{
  std::map<std::string, ME_List >::iterator itr;
  for (itr = collection.begin(); itr != collection.end(); ++itr)
    {
      LOG4CPLUS_DEBUG(logger_,"Clean ME Collection " << itr->first );
      clearMECollection(itr->second);
    }

  collection.clear();
}


void EmuPlotter::clearMECollection(ME_List & collection)
{

  if (collection.size() > 0)
    {
      for (ME_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
        {
          if (itr->second) delete itr->second;
        }
      collection.clear();
    }

}
void EmuPlotter::printMECollection(ME_List & collection)
{
  int i = 0;
  for (ME_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
    {
      LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
    }

}

void EmuPlotter::clearCanvasesCollection(std::map<std::string, MECanvases_List > & collection)
{
  std::map<std::string, MECanvases_List >::iterator citr;
  for (citr = collection.begin(); citr != collection.end(); ++citr)
    {
      LOG4CPLUS_DEBUG(logger_,"Clean Canvases Collection " << citr->first );
      clearCanvasesCollection(citr->second);
    }

  collection.clear();

}

void EmuPlotter::printCanvasesCollection(std::map<std::string, MECanvases_List > & collection)
{
  std::map<std::string, MECanvases_List >::iterator citr;
  for (citr = collection.begin(); citr != collection.end(); ++citr)
    {
      LOG4CPLUS_DEBUG(logger_,"Print Canvases Collection " << citr->first );
      printCanvasesCollection(citr->second);
    }

}


void EmuPlotter::clearCanvasesCollection(MECanvases_List & collection)
{

  if (collection.size() > 0)
    {
      for (MECanvases_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
        {
          LOG4CPLUS_DEBUG(logger_,"Clean " << itr->first );
          if (itr->second) delete itr->second;
        }
      collection.clear();
    }

}

void EmuPlotter::printCanvasesCollection(MECanvases_List & collection)
{
  int i = 0;
  for (MECanvases_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
    {
      LOG4CPLUS_DEBUG(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
    }

}



void EmuPlotter::cleanup()
{
  appBSem_.take();  
  LOG4CPLUS_WARN(logger_,"Cleanup Called" );
  clearCanvasesCollection(MECanvases);
  clearMECollection(MEs);

  clearCanvasesCollection(MECanvasFactories);
  clearMECollection(MEFactories);
  appBSem_.give();
}


EmuPlotter::~EmuPlotter()
{
  cleanup();
}

void EmuPlotter::book()
{
/*  appBSem_.take();
  clearCanvasesCollection(MECanvases);
  clearMECollection(MEs);
  appBSem_.give();
*/
  reset();
  if (loadXMLBookingInfo(xmlHistosBookingCfgFile) == 0)
    {
//      reset();
      loadXMLCanvasesInfo(xmlCanvasesCfgFile);
    }


}




void EmuPlotter::setCSCMapFile(std::string filename)
{
  if (filename != "")
    {
      cscMapFile = filename;
      cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
    }


}

std::map<std::string, int> EmuPlotter::getCSCTypeToBinMap()
{
  std::map<std::string, int> tmap;
  tmap["ME-4/2"] = 0;
  tmap["ME-4/1"] = 1;
  tmap["ME-3/2"] = 2;
  tmap["ME-3/1"] = 3;
  tmap["ME-2/2"] = 4;
  tmap["ME-2/1"] = 5;
  tmap["ME-1/3"] = 6;
  tmap["ME-1/2"] = 7;
  tmap["ME-1/1"] = 8;
  tmap["ME+1/1"] = 9;
  tmap["ME+1/2"] = 10;
  tmap["ME+1/3"] = 11;
  tmap["ME+2/1"] = 12;
  tmap["ME+2/2"] = 13;
  tmap["ME+3/1"] = 14;
  tmap["ME+3/2"] = 15;
  tmap["ME+4/1"] = 16;
  tmap["ME+4/2"] = 17;
  return tmap;

}

std::string EmuPlotter::getCSCTypeLabel(int endcap, int station, int ring )
{
  std::string label = "Unknown";
  std::ostringstream st;
  if ((endcap > 0) && (station>0) && (ring>0))
    {
      if (endcap==1)
        {
          st << "ME+" << station << "/" << ring;
          label = st.str();
        }
      else if (endcap==2)
        {
          st << "ME-" << station << "/" << ring;
          label = st.str();
        }
      else
        {
          label = "Unknown";
        }
    }
  return label;
}


std::string EmuPlotter::getCSCFromMap(int crate, int slot, int& csctype, int& cscposition)
{
  //  LOG4CPLUS_INFO(logger_, "========== get CSC from Map crate" << crate << " slot" << slot);
  int iendcap = -1;
  int istation = -1;
  int iring = -1;
  // TODO: Add actual Map conversion
  int id = cscMapping.chamber(iendcap, istation, crate, slot, -1);
  std::string cscname="";
  if (id==0)
    {
      return cscname;
    }
  CSCDetId cid( id );
  iendcap = cid.endcap();
  istation = cid.station();
  iring = cid.ring();
  cscposition = cid.chamber();

  //  std::map<std::string, int> tmap = getCSCTypeToBinMap();
  std::string tlabel = getCSCTypeLabel(iendcap, istation, iring );
  std::map<std::string,int>::const_iterator it = tmap.find( tlabel );
  if (it != tmap.end())
    {
      csctype = it->second;
      //      LOG4CPLUS_INFO(logger_, "========== get CSC from Map label:" << tlabel << "/" << cscposition);
      cscname=std::string(Form("%s/%02d",tlabel.c_str(),cscposition));
    }
  else
    {
      //      LOG4CPLUS_INFO(logger_, "========== can not find map");
      csctype = 0;
    }

  // return bin number which corresponds for CSC Type (ex. ME+4/2 -> bin 18)
  return cscname;

}


bool EmuPlotter::isMEvalid(ME_List& MEs, std::string name, EmuMonitoringObject*& me)
{
  ME_List_iterator res = MEs.find(name);
  if (res != MEs.end() && (res->second != 0))
    {
      me = res->second;
      // cout << "Found " << me->getName() << endl;
      return true;
    }
  else
    {
      if (debug) LOG4CPLUS_WARN(logger_, "Plotter can not find ME: '" << name << "'");
      me = 0;
      return false;
    }

}

ME_List EmuPlotter::bookMEs(std::string factoryID, std::string prefix)
{
  ME_List mes;
  ME_List_iterator itr;

  std::map<std::string, ME_List >::iterator f_itr = MEFactories.find(factoryID);
  if (f_itr != MEFactories.end())
    {
      ME_List& factory = f_itr->second;
      for (itr = factory.begin(); itr != factory.end(); ++itr)
        {
          EmuMonitoringObject * obj = new EmuMonitoringObject(*itr->second);
          obj->setPrefix(prefix);
          mes[obj->getName()] = obj;
        }
    }
  return  mes;
}

CSCCounters EmuPlotter::bookCounters()
{
  CSCCounters trigCnts;
  trigCnts["ALCT"] = 0;
  trigCnts["CLCT"] = 0;
  trigCnts["CFEB"] = 0;
  trigCnts["DMB"] = 0;
  trigCnts["BAD"] = 0;
  nDetectedCSCs++;
  return trigCnts;
}



MECanvases_List EmuPlotter::bookMECanvases(std::string factoryID, std::string prefix, std::string title)
{
  MECanvases_List mes;
  MECanvases_List_iterator itr;

  std::map<std::string, MECanvases_List >::iterator f_itr = MECanvasFactories.find(factoryID);
  if (f_itr != MECanvasFactories.end())
    {
      MECanvases_List& factory = f_itr->second;
      for (itr = factory.begin(); itr != factory.end(); ++itr)
        {
          EmuMonitoringCanvas * obj = new EmuMonitoringCanvas(*itr->second);
          obj->setPrefix(prefix);
          std::string new_title = obj->getTitle() + title;
          obj->setTitle(new_title);
          mes[obj->getName()] = obj;
        }
    }
  return  mes;
}


