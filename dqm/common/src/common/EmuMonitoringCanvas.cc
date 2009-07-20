#include "emu/dqm/common/EmuMonitoringCanvas.h"
#include <sstream>

EmuMonitoringCanvas::EmuMonitoringCanvas(const EmuMonitoringCanvas& mo)
{
  // canvas = reinterpret_cast<MonitoringCanvas*>(mo.canvas->Clone());
  canvas = NULL;
  type = mo.type;
  prefix = mo.prefix;
  folder = mo.folder;
  name = mo.name;
  title = mo.title;
  params = mo.params;
  cnv_width = mo.cnv_width;
  cnv_height = mo.cnv_height;
  displayInWeb = mo.displayInWeb;
  summaryMap=mo.summaryMap;
  chamberMap=mo.chamberMap;
  runNumber=mo.runNumber;
}

EmuMonitoringCanvas& EmuMonitoringCanvas::operator=(const EmuMonitoringCanvas& mo)
{
  canvas = NULL;
  // canvas = reinterpret_cast<MonitoringCanvas*>(mo.canvas->Clone());
  type = mo.type;
  prefix = mo.prefix;
  folder = mo.folder;
  name = mo.name;
  title = mo.title;
  params = mo.params;
  cnv_width = mo.cnv_width;
  cnv_height = mo.cnv_height;
  displayInWeb = mo.displayInWeb;
  summaryMap=mo.summaryMap;
  chamberMap=mo.chamberMap;
  runNumber=mo.runNumber;
  return *this;
}

EmuMonitoringCanvas::EmuMonitoringCanvas() :
    type(""),
    prefix(""),
    folder(""),
    name(""),
    title(""),
    cnv_width(DEF_WIDTH),
    cnv_height(DEF_HEIGHT),
    runNumber("")

{
  canvas = NULL;
  displayInWeb = true;
  summaryMap=NULL;
  chamberMap=NULL;
  params.clear();
}

EmuMonitoringCanvas::EmuMonitoringCanvas(DOMNode* info)
{
  canvas = NULL;
  displayInWeb = true;
  parseDOMNode(info);
  summaryMap=NULL;
  chamberMap=NULL;
  Book();
}

int EmuMonitoringCanvas::Book()
{
  /*
    int nbinsx = 0, nbinsy = 0, nbinsz = 0;
    float xlow = 0, ylow = 0, zlow = 0;
    float xup = 0, yup = 0, zup = 0;
    char *stopstring;

    if (canvas != NULL) {
    delete canvas;
    canvas = NULL;
    }

    std::map<std::string, std::string> other_params;
    std::map<std::string, std::string>::iterator itr;
    if ((itr = params.find("NumPadsX")) != params.end()) {
    nbinsx = strtol( itr->second.c_str(), &stopstring, 10 );
    }
    if ((itr = params.find("NumPadsX")) != params.end()) {
    nbinsy = strtol( itr->second.c_str(), &stopstring, 10 );
    }

    // !!! TODO: Add canvas class check
    if (canvas != NULL) {

    }
  */
  return 0;
}


int EmuMonitoringCanvas::Book(DOMNode* info)
{

  parseDOMNode(info);
  Book();
  return 0;
}

void EmuMonitoringCanvas::Draw(ME_List& MEs, bool useDrawType)
{
  Draw(MEs, cnv_width, cnv_height, useDrawType);
}

std::map<std::string, std::string> EmuMonitoringCanvas::getListOfPads()
{
  int npadsx = 1, npadsy = 1;;
  char *stopstring;

  std::map<std::string, std::string> pads;
  std::map<std::string, std::string>::iterator itr;

  if ((itr = params.find("NumPadsX")) != params.end())
    {
      npadsx = strtol( itr->second.c_str(), &stopstring, 10 );
    }
  if ((itr = params.find("NumPadsY")) != params.end())
    {
      npadsy = strtol( itr->second.c_str(), &stopstring, 10 );
    }
  for (int i=0; i< npadsx*npadsy; i++)
    {
      std::stringstream st;
      st << "Pad" << i+1;
      std::string padTag = st.str();
      std::string objID = "";
      if ((itr = params.find(padTag)) != params.end())
        {
          objID = itr->second;
          pads[padTag] = objID;
        }
    }

  return pads;
}

void EmuMonitoringCanvas::Draw(ME_List& MEs, int width, int height, bool useDrawType)
{
  int npadsx = 1, npadsy = 1;;
  char *stopstring;

  setCanvasWidth(width);
  setCanvasHeight(height);
  /*
    if (canvas != NULL) {
    delete canvas;
    canvas = NULL;
    }
  */
  gStyle->SetPalette(1,0);

  std::map<std::string, std::string> other_params;
  std::map<std::string, std::string>::iterator itr;
  if ((itr = params.find("NumPadsX")) != params.end())
    {
      npadsx = strtol( itr->second.c_str(), &stopstring, 10 );
    }
  if ((itr = params.find("NumPadsY")) != params.end())
    {
      npadsy = strtol( itr->second.c_str(), &stopstring, 10 );
    }
  if (canvas == NULL)
    {
      canvas = new MonitoringCanvas(getFullName().c_str(), getFullName().c_str(), getTitle().c_str(),
                                    npadsx, npadsy, getCanvasWidth(), getCanvasHeight());
      canvas->SetCanvasSize(width, height);
      for (int i=0; i< npadsx*npadsy; i++)
        {
          canvas->cd(i+1);
          std::stringstream st;
          st << "Pad" << i+1;
          std::string objname = "";
          if ((itr = params.find(st.str())) != params.end())
            {
              objname = itr->second;
            }
          if (!objname.empty() && !MEs.empty())
            {
              ME_List_iterator obj = MEs.find(objname);
              if (obj != MEs.end())
                {
                  // obj->second->Draw();
                  std::string leftMargin = obj->second->getParameter("SetLeftMargin");
                  if (leftMargin != "" )
                    {
                      gPad->SetLeftMargin(atof(leftMargin.c_str()));
                    }
                  std::string rightMargin = obj->second->getParameter("SetRightMargin");
                  if (rightMargin != "" )
                    {
                      gPad->SetRightMargin(atof(rightMargin.c_str()));
                    }

                  std::string logx = obj->second->getParameter("SetLogx");
                  if (logx!= "")
                    {
                      //  std::cout << "Logx " << ((double)(obj->second->getObject()->GetMaximum())) << std::endl;
                      gPad->SetLogx();
                    }
                  std::string logy = obj->second->getParameter("SetLogy");
                  if (logy!= "" && (obj->second->getObject()->GetMaximum()>0.))
                    {
                      // if (logy!= "") {
                      //  std::cout << "Logy " << ((double)(obj->second->getObject()->GetMaximum())) << std::endl;
                      gPad->SetLogy();
                    }

                  std::string logz = obj->second->getParameter("SetLogz");
                  if (logz!= "" && (obj->second->getObject()->GetMaximum()>0.) )
                    {
                      // if (logz!= "") {
                      //  std::cout << "Logz " << ((double)(obj->second->getObject()->GetMaximum())) << std::endl;
                      gPad->SetLogz();
                    }

                  std::string gridx = obj->second->getParameter("SetGridx");
                  if (gridx!= "" )
                    {
                      gPad->SetGridx();
                    }

                  std::string gridy = obj->second->getParameter("SetGridy");
                  if (gridy!= "" )
                    {
                      gPad->SetGridy();
                    }


                  if (obj->second->getParameter("SetStats") != "")
                    {
                      int stats = strtol( obj->second->getParameter("SetStats").c_str(), &stopstring, 10 );
                      obj->second->getObject()->SetStats(bool(stats));
                    }


                  if (useDrawType)
                    {
                      std::string drawtype=obj->second->getParameter("DrawType");

                      if ((drawtype.find("ChamberMap") != std::string::npos) && (chamberMap!=NULL))
                        {
                          TH2* tmp = dynamic_cast<TH2*>(obj->second->getObject());
                          chamberMap->draw(tmp);
                        }
                      else if ((drawtype.find("SummaryDetectorMap") != std::string::npos) && (summaryMap!=NULL))
                        {
                          TH2* tmp = dynamic_cast<TH2*>(obj->second->getObject());
                          summaryMap->drawDetector(tmp);
                        }
                      else if ((drawtype.find("SummaryStationMap") != std::string::npos) && (summaryMap!=NULL))
                        {
                          std::string station_str = obj->second->getName();
                          REREPLACE(".*Physics_ME([1234])$", station_str, "$1");
                          TH2* tmp = dynamic_cast<TH2*>(obj->second->getObject());
                          summaryMap->drawStation(tmp, atoi(station_str.c_str()));
                        }
                      else
                        {
                          obj->second->Draw();
                        }
                    }
                  else obj->second->Draw();


                  //	  obj->second->Draw();
                  /*
                    if (obj->second->getParameter("SetLabelSizeZ") != "") {
                    std::string st = obj->second->getParameter("SetlabelSizeZ");
                    double opt = atof(st.c_str()) ;
                    if (obj->second->getObject()) {
                    TPaletteAxis *palette = (TPaletteAxis*)(obj->second->getObject()->GetListOfFunctions()->FindObject("palette"));
                    if (palette != NULL) {
                    palette->SetLabelSize(opt);
                    } else {
                    std::cout << "Unable to find palette" << std::endl;
                    }

                    }
                    }
                  */
                  std::string statOpt = obj->second->getParameter("SetOptStat");
                  if (statOpt != "" )
                    {
                      gStyle->SetOptStat(statOpt.c_str());
                    }
                  else
                    {
                      //   gStyle->SetOptStat("e");
                    }

                }
            }
        }
    }//  else {
  // canvas->SetCanvasSize(width, height);
  canvas->SetRunNumber(runNumber.c_str());
  canvas->Draw();
  canvas->SetCanvasSize(width, height);
  //}

}


EmuMonitoringCanvas::~EmuMonitoringCanvas()
{
  if (canvas != NULL)
    {
      delete canvas;
      canvas = NULL;
    }

}

void EmuMonitoringCanvas::setName(std::string newname)
{
  name = newname;
  if (canvas != NULL)
    {
      canvas->SetName(getFullName().c_str());
    }
}

void EmuMonitoringCanvas::setPrefix(std::string newprefix)
{
  prefix = newprefix;
  if (canvas != NULL)
    {
      canvas->SetName(getFullName().c_str());
    }
}

void EmuMonitoringCanvas::setFolder(std::string newfolder)
{
  folder = newfolder;
}


void EmuMonitoringCanvas::setTitle(std::string newtitle)
{
  title = newtitle;
  if (canvas != NULL)
    {
      canvas->SetTitle(getTitle().c_str());
    }
}

int EmuMonitoringCanvas::setParameter(std::string parname, std::string parvalue)
{
  if (canvas != NULL)
    {
      params[parname] = parvalue;
      return 0;
    }
  else return 1;

}

int EmuMonitoringCanvas::setParameters(std::map<std::string, std::string> newparams, bool resetParams)
{
  std::map<std::string, std::string>::iterator itr;
  if (resetParams)
    {
      params.clear();
      params = newparams;
    }
  else
    {
      // == Append to parameters list
      for (itr = newparams.begin(); itr != newparams.end(); ++itr)
        {
          params[itr->first] = itr->second;
        }
    }
  return 0;
}

std::string EmuMonitoringCanvas::getParameter(std::string paramname)
{
  std::map<std::string, std::string>::iterator itr;
  if ((itr = params.find(paramname)) != params.end())
    return itr->second;
  else
    return "";
}

int EmuMonitoringCanvas::parseDOMNode(DOMNode* info)
{
  std::map<std::string, std::string> obj_info;
  std::map<std::string, std::string>::iterator itr;
  DOMNodeList *children = info->getChildNodes();
  for (unsigned int i=0; i<children->getLength(); i++)
    {
      char * pChar = XMLString::transcode(children->item(i)->getNodeName());
      std::string paramname = std::string(pChar);
      delete[] pChar;
      if ( children->item(i)->hasChildNodes() )
        {
          char * pChar = XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue());
          std::string param = std::string(pChar);
          obj_info[paramname] = param;
          delete[] pChar;
        }
    }

  /*
    for(unsigned int i=0; i<children->getLength(); i++){
      std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
      if ( children->item(i)->hasChildNodes() ) {
        std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
        obj_info[paramname] = param;
      }
    }
  */
  /*
    for  (itr = obj_info.begin(); itr != obj_info.end(); ++itr) {
    std::cout << itr->first << ":" << itr->second << std::endl;
    }
  */

  if (obj_info.size() > 0)
    {
      // == Construct Monitoring Canvas Name
      std::string objname = "";
      if ((itr = obj_info.find("Type")) != obj_info.end())
        {
          // std::string typestr = itr->second;
          objname += itr->second;
          type = itr->second;
//      obj_info.erase("Type");

        }
      if ((itr = obj_info.find("Prefix")) != obj_info.end())
        {
          objname += itr->second;
          prefix = itr->second;
//      obj_info.erase("Prefix");
        }
      if ((itr = obj_info.find("Name")) != obj_info.end())
        {
          objname += itr->second;
          name = itr->second;
//      obj_info.erase("Name");
        }
      if ((itr = obj_info.find("DisplayInWeb")) != obj_info.end())
        {
          objname += itr->second;
          displayInWeb = (bool) atoi(itr->second.c_str());
//      obj_info.erase("DisplayInWeb");
        }
      if ((itr = obj_info.find("Folder")) != obj_info.end())
        {
          objname += itr->second;
          folder = itr->second;
//      obj_info.erase("Folder");
        }
      // name = objname;

      // == Get Monitoring Canvas Title
      if ((itr = obj_info.find("Title")) != obj_info.end())
        {
          title = itr->second;
//      obj_info.erase("Title");
        }


      // == Create Monitoring Canvas Parameters map
      params.clear();
      for (itr = obj_info.begin(); itr != obj_info.end(); ++itr)
        {
          params[itr->first] = itr->second;
          // std::cout << "\t" << itr->first << ":" << itr->second << std::endl;
        }
    }
  return 0;
}
