#include "emu/dqm/cscdisplay/EmuDisplayClient.h"

void EmuDisplayClient::clearMECollection(std::map<std::string, ME_List > & collection)
{
  std::map<std::string, ME_List >::iterator itr;
  for (itr = collection.begin(); itr != collection.end(); ++itr)
  {
    LOG4CPLUS_DEBUG(logger_,"Clean ME Collection " << itr->first );
    clearMECollection(itr->second);
  }

  collection.clear();
}


void EmuDisplayClient::clearMECollection(ME_List & collection)
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
void EmuDisplayClient::printMECollection(ME_List & collection)
{
  int i = 0;
  for (ME_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
  {
    LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
  }

}

void EmuDisplayClient::clearCanvasesCollection(std::map<std::string, MECanvases_List > & collection)
{
  std::map<std::string, MECanvases_List >::iterator citr;
  for (citr = collection.begin(); citr != collection.end(); ++citr)
  {
    LOG4CPLUS_DEBUG(logger_,"Clean Canvases Collection " << citr->first );
    clearCanvasesCollection(citr->second);
  }

  collection.clear();

}

void EmuDisplayClient::printCanvasesCollection(std::map<std::string, MECanvases_List > & collection)
{
  std::map<std::string, MECanvases_List >::iterator citr;
  for (citr = collection.begin(); citr != collection.end(); ++citr)
  {
    LOG4CPLUS_DEBUG(logger_,"Print Canvases Collection " << citr->first );
    printCanvasesCollection(citr->second);
  }

}


void EmuDisplayClient::clearCanvasesCollection(MECanvases_List & collection)
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

void EmuDisplayClient::printCanvasesCollection(MECanvases_List & collection)
{
  int i = 0;
  for (MECanvases_List_iterator itr = collection.begin(); itr != collection.end(); ++itr)
  {
    LOG4CPLUS_DEBUG(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
  }

}




void EmuDisplayClient::book()
{

  clearCanvasesCollection(MECanvases);
  clearMECollection(MEs);

  if (loadXMLBookingInfo(xmlHistosBookingCfgFile_.toString()) == 0)
  {
    loadXMLCanvasesInfo(xmlCanvasesCfgFile_.toString());
  }

}

/*
bool EmuPlotter::isMEvalid(ME_List& MEs, std::string name, EmuMonitoringObject*& me)
{
  ME_List_iterator res = MEs.find(name);
  if (res != MEs.end() && (res->second != 0)) {
    me = res->second;
    // cout << "Found " << me->getName() << endl;
    return true;
  } else {
    LOG4CPLUS_WARN(logger_, "Plotter can not find ME: '" << name << "'");
    me = 0;
    return false;
  }

}
*/


bool EmuDisplayClient::isMEValid(std::map<std::string, ME_List >&  List, std::string Folder, std::string Name, EmuMonitoringObject*& mo)
{
  if (List.empty())
  {
    mo=0;
    return false;
  }

  std::map<std::string, ME_List >::iterator f_itr = List.find(Folder);
  if (f_itr != List.end())
  {
    // LOG4CPLUS_INFO(logger_, "Found Folder: '" << Folder << "'");
    ME_List& OList = f_itr->second;
    if (OList.empty())
    {
      mo = 0;
      return false;
    }

    ME_List::iterator o_itr = OList.find(Name);
    if (o_itr != OList.end() && (o_itr->second != 0))
    {
      mo = o_itr->second;
      // LOG4CPLUS_INFO(logger_, "Found Object: '" << Name << "'");
      return true;
    }
    else
    {
      // LOG4CPLUS_DEBUG(logger_, "Can not find Object: '" << Name << "'");
      mo = 0;
      return false;
    }
  }
  else
  {
    // LOG4CPLUS_DEBUG(logger_, "Can not find Folder: '" << Folder << "'");
    mo = 0;
    return false;
  }
  mo = 0;
  return false;
}


bool EmuDisplayClient::bookME(std::map<std::string, ME_List >&  List,
                              std::string Folder, std::string Name, std::string Title, EmuMonitoringObject*& mo )
{
  if (isMEValid(List, Folder, Name, mo)) return true;

  EmuMonitoringObject * obj = 0;
  std::string Scope="";
  std::string Prefix=Folder;
  if (Folder.find("EMU") ==0)
  {
    Scope = "EMU";
    Prefix = "EMU_Summary";
  }
  else if (Folder.find("DDU") ==0) Scope = "DDU";
  else if (Folder.find("CSC") ==0) Scope = "CSC";
  if (Scope == "")
  {
    mo = 0;
    return false;
  }

  std::map<std::string, ME_List >& FList = MEFactories;
  std::map<std::string, ME_List >::iterator fact_itr = FList.find(Scope);
  if (fact_itr != FList.end())
  {

    ME_List::iterator fact_obj_itr = fact_itr->second.find(Name);
    if (fact_obj_itr != fact_itr->second.end() && (fact_obj_itr->second != 0))
    {
      obj = new EmuMonitoringObject(*fact_obj_itr->second);
      obj->setPrefix(Prefix);
      std::string new_title = obj->getTitle() + Title;
      obj->setTitle(new_title);
      mo = obj;

      std::map<std::string, ME_List >::iterator f_itr = List.find(Folder);
      if (f_itr != List.end())
      {
        ME_List& FList=f_itr->second;
        FList[Name]=obj;
      }
      else
      {
        ME_List newFList;
        newFList[Name]=obj;
        List.insert(make_pair(Folder, newFList));
      }
      //  LOG4CPLUS_INFO(logger_, "Booked Object: '" << Folder << " " << Name << "'");
      return true;
    }
  }

  mo = 0;
  return false;

}


bool EmuDisplayClient::isCanvasValid(std::map<std::string, MECanvases_List >&  List, std::string Folder, std::string Name, EmuMonitoringCanvas*& cnv)
{
  if (List.empty())
  {
    cnv=0;
    return false;
  }

  std::map<std::string, MECanvases_List >::iterator f_itr = List.find(Folder);
  if (f_itr != List.end())
  {
    // LOG4CPLUS_INFO(logger_, "Found Folder: '" << Folder << "'");
    MECanvases_List& CList = f_itr->second;
    if (CList.empty())
    {
      cnv = 0;
      return false;
    }

    MECanvases_List::iterator c_itr = CList.find(Name);
    if (c_itr != CList.end() && (c_itr->second != 0))
    {
      cnv = c_itr->second;
      // LOG4CPLUS_INFO(logger_, "Found Canvas: '" << Name << "'");
      return true;
    }
    else
    {
      // LOG4CPLUS_DEBUG(logger_, "Can not find Canvas: '" << Name << "'");
      cnv = 0;
      return false;
    }
  }
  else
  {
    // LOG4CPLUS_DEBUG(logger_, "Can not find Folder: '" << Folder << "'");
    cnv = 0;
    return false;
  }
  cnv = 0;
  return false;
}

bool EmuDisplayClient::bookCanvas(std::map<std::string, MECanvases_List >&  List,
                                  std::string Folder, std::string Name, std::string Title, EmuMonitoringCanvas*& cnv )
{
  if (isCanvasValid(List, Folder, Name, cnv)) return true;

  EmuMonitoringCanvas * obj = 0;
  std::string Scope="";
  if (Folder.find("EMU") ==0) Scope = "EMU";
  else if (Folder.find("DDU") ==0) Scope = "DDU";
  else if (Folder.find("CSC") ==0) Scope = "CSC";
  if (Scope == "")
  {
    cnv = 0;
    return false;
  }

  std::map<std::string, MECanvases_List >& FList = MECanvasFactories;
  std::map<std::string, MECanvases_List >::iterator fact_itr = FList.find(Scope);
  if (fact_itr != FList.end())
  {

    MECanvases_List::iterator fact_cnv_itr = fact_itr->second.find(Name);
    if (fact_cnv_itr != fact_itr->second.end() && (fact_cnv_itr->second != 0))
    {
      obj = new EmuMonitoringCanvas(*fact_cnv_itr->second);
      obj->setPrefix(Folder);
      std::string new_title = obj->getTitle() + Title;
      obj->setTitle(new_title);
      cnv = obj;

      std::map<std::string, MECanvases_List >::iterator f_itr = List.find(Folder);
      if (f_itr != List.end())
      {
        MECanvases_List& FList=f_itr->second;
        FList[Name]=obj;
      }
      else
      {
        MECanvases_List newFList;
        newFList[Name]=obj;
        List.insert(make_pair(Folder, newFList));
      }
      // LOG4CPLUS_INFO(logger_, "Booked Canvas: '" << Folder << " " << Name << "'");
      return true;
    }
  }

  cnv = 0;
  return false;
}

bool EmuDisplayClient::readME(std::string Folder, std::string Name, EmuMonitoringObject*& mo, TFile* rootsrc)
{
  if (!rootsrc)
  {
    // LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return false;
  }

  if (!rootsrc->cd("DQMData"))
  {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return false;
  }
  LOG4CPLUS_DEBUG(logger_, "Trying to Read " << Folder << "/" << mo->getFullName() << " object" );
  TDirectory *sourcedir = gDirectory;

  TObject *obj = sourcedir->Get((Folder+"/"+mo->getFullName()).c_str());
  if (obj != NULL)
  {
    LOG4CPLUS_DEBUG(logger_, "Successfully Read " << Folder << "/" << mo->getFullName() << " object");
    mo->setObject(reinterpret_cast<MonitorElement*>(obj->Clone()));
  }
  else
  {
    LOG4CPLUS_WARN(logger_, "Unable to Read " << Folder << "/" << mo->getFullName() << " object");
  }

  return true;
}

bool EmuDisplayClient::updateME(std::string Folder, std::string Name, EmuMonitoringObject*& mo, TFile* runname)
{

  // if ((runname != "Online") && (runname != ""))
  if (runname != NULL)
  {
    return readME(Folder, Name, mo, runname);
  }

  std::map<std::string, std::set<int> >::iterator itr = foldersMap.find(Folder);
  if ((itr == foldersMap.end()) || itr->second.empty())
  {
    if (debug) LOG4CPLUS_WARN (logger_, "Can not locate request node for " << Folder);
    return false;
  }


  // == Send request to nodes from list
  std::set<int>::iterator nitr;
  std::vector<TObject*> objects;
  objects.clear();

  TObject* obj = NULL;
  for (nitr = itr->second.begin(); nitr != itr->second.end(); ++nitr)
  {
    int nodeID = *nitr;
    TMessage* msgbuf =requestObjects(nodeID, Folder, Name);

    if (msgbuf != NULL)
    {
      msgbuf->Reset();
      msgbuf->SetReadMode();
      if (msgbuf->What() == kMESS_OBJECT)
      {
        // std::cout << ((msgbuf->GetClass())->ClassName()) << std::endl;
        obj = reinterpret_cast<TObject*>(msgbuf->ReadObjectAny(msgbuf->GetClass()));
        if ((obj != NULL) && obj->InheritsFrom(TH1::Class()))
        {
          objects.push_back(obj);
          // delete obj;
          obj=NULL;
        }
      }

      delete msgbuf;
      msgbuf=NULL;
    }
    else
    {
      if (debug) LOG4CPLUS_WARN(logger_, "Can not update Objects: '" << Folder << " " << Name << "' from DQM Node #" << nodeID  );
    }
  }

  if (objects.size())
  {
    MonitorElement* me = mergeObjects(objects);
    if (me) mo->setObject(me);
    for (uint32_t i=0; i<objects.size(); i++)
    {
      delete objects[i];
    }
    objects.clear();
    return true;
  }
  return false;
}


MonitorElement* EmuDisplayClient::mergeObjects(std::vector<TObject*>& olist)
{
  MonitorElement* me = NULL;
  if (olist.size())
  {
    me = reinterpret_cast<MonitorElement*>(olist[0]->Clone());
    for (uint32_t i=1; i<olist.size(); i++)
    {
      (reinterpret_cast<MonitorElement*>(me))->Add(reinterpret_cast<MonitorElement*>(olist[i]));
    }
  }
  return me;
}

void EmuDisplayClient::updateEfficiencyHistos(TFile* runname)
{

  std::string nodeTag = "EMU";
  std::string title = "";

  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mo1 = NULL;
  //
  // Set detector information
  //

  summary.Reset();

  //  LOG4CPLUS_INFO(logger_, "Updating Efficiency Histos");

  if (bookME(nodeTag, "CSC_Reporting",title, mo))
  {
    updateME(nodeTag, "CSC_Reporting", mo, runname);
    if (bookME(nodeTag, "CSC_Format_Errors",title, mo1))
    {
      updateME(nodeTag, "CSC_Format_Errors",mo1, runname);
    }
    if (bookME(nodeTag, "CSC_L1A_out_of_sync",title, mo1))
    {
      updateME(nodeTag, "CSC_L1A_out_of_sync",mo1, runname);
    }
    if (bookME(nodeTag, "CSC_DMB_input_fifo_full",title, mo1))
    {
      updateME(nodeTag, "CSC_DMB_input_fifo_full",mo1, runname);
    }
    if (bookME(nodeTag, "CSC_DMB_input_timeout",title, mo1))
    {
      updateME(nodeTag, "CSC_DMB_input_timeout", mo1, runname);
    }
  }

  if (bookME(nodeTag, "Physics_ME1",title, mo))
  {
    updateME(nodeTag, "Physics_ME1",mo, runname);
  }

  if (bookME(nodeTag, "Physics_ME2",title, mo))
  {
    updateME(nodeTag, "Physics_ME2",mo, runname);
  }

  if (bookME(nodeTag, "Physics_ME3",title, mo))
  {
    updateME(nodeTag, "Physics_ME3",mo, runname);
  }

  if (bookME(nodeTag, "Physics_ME4",title, mo))
  {
    updateME(nodeTag, "Physics_ME4",mo, runname);
  }

  if (bookME(nodeTag, "Physics_EMU",title, mo))
  {
    updateME(nodeTag, "Physics_EMU",mo, runname);
  }


  if (isMEValid(nodeTag, "CSC_Reporting", mo))
  {

    TH2* rep = dynamic_cast<TH2*>(mo->getObject());
    summary.ReadReportingChambers((const TH2*&)rep, 1.0);

    if (isMEValid(nodeTag, "CSC_Format_Errors", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::FORMAT_ERR, 0.1, 5.0);
    }

    if (isMEValid(nodeTag, "CSC_L1A_out_of_sync", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::L1SYNC_ERR, 0.1, 5.0);
    }

    if (isMEValid(nodeTag, "CSC_DMB_input_fifo_full", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::FIFOFULL_ERR, 0.1, 5.0);
    }

    if (isMEValid(nodeTag, "CSC_DMB_input_timeout", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::INPUTTO_ERR, 0.1, 5.0);
    }

  }

  //
  // Write summary information
  //


  if (isMEValid(nodeTag, "Physics_ME1", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 1);
  }

  if (isMEValid(nodeTag, "Physics_ME2", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 2);
  }

  if (isMEValid(nodeTag, "Physics_ME3", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 3);
  }

  if (isMEValid(nodeTag, "Physics_ME4", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 4);
  }


  if (isMEValid(nodeTag, "Physics_EMU", mo))
  {

    TH2* tmp=dynamic_cast<TH2*>(mo->getObject());
    // float rs =
    summary.WriteMap(tmp);
    // float he = summary.GetEfficiencyHW();
    // TString title = Form("EMU Status: Physics Efficiency %.2f", rs);
    // tmp->SetTitle(title);

  }

}


/*
ME_List EmuPlotter::bookMEs(std::string factoryID, std::string prefix)
{
        ME_List mes;
        ME_List_iterator itr;

        std::map<std::string, ME_List >::iterator f_itr = MEFactories.find(factoryID);
        if (f_itr != MEFactories.end()) {
                ME_List& factory = f_itr->second;
                for (itr = factory.begin(); itr != factory.end(); ++itr) {
                        EmuMonitoringObject * obj = new EmuMonitoringObject(*itr->second);
                        obj->setPrefix(prefix);
                        mes[obj->getName()] = obj;
                }
        }
        return  mes;
}

MECanvases_List EmuPlotter::bookMECanvases(std::string factoryID, std::string prefix, std::string title)
{
        MECanvases_List mes;
        MECanvases_List_iterator itr;

        std::map<std::string, MECanvases_List >::iterator f_itr = MECanvasFactories.find(factoryID);
        if (f_itr != MECanvasFactories.end()) {
                MECanvases_List& factory = f_itr->second;
                for (itr = factory.begin(); itr != factory.end(); ++itr) {
                        EmuMonitoringCanvas * obj = new EmuMonitoringCanvas(*itr->second);
                        obj->setPrefix(prefix);
                        std::string new_title = obj->getTitle() + title;
                        obj->setTitle(new_title);
                        mes[obj->getName()] = obj;
                }
        }
        return  mes;
}

*/
