#include "EmuMonitoringCanvas.h"
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
	return *this;
}

EmuMonitoringCanvas::EmuMonitoringCanvas() :
	type(""),
	prefix(""),
	folder(""),
	name(""),
	title(""),
	cnv_width(DEF_WIDTH),
	cnv_height(DEF_HEIGHT)
	
{
	canvas = NULL;
	params.clear();
}

EmuMonitoringCanvas::EmuMonitoringCanvas(DOMNode* info) 
{
	canvas = NULL;
	parseDOMNode(info);
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

void EmuMonitoringCanvas::Draw(ME_List& MEs) 
{
	Draw(MEs, cnv_width, cnv_height);
}

void EmuMonitoringCanvas::Draw(ME_List& MEs, int width, int height)
{
  int npadsx = 1, npadsy = 1;;
  char *stopstring;

  setCanvasWidth(width);
  setCanvasHeight(height);
  if (canvas != NULL) {
    delete canvas;
    canvas = NULL;
  }
  gStyle->SetPalette(1,0);

  std::map<std::string, std::string> other_params;
  std::map<std::string, std::string>::iterator itr;
  if ((itr = params.find("NumPadsX")) != params.end()) {
    npadsx = strtol( itr->second.c_str(), &stopstring, 10 );
  }
  if ((itr = params.find("NumPadsY")) != params.end()) {
    npadsy = strtol( itr->second.c_str(), &stopstring, 10 );
  }
  if (canvas == NULL)
	  canvas = new MonitoringCanvas(getFullName().c_str(), getFullName().c_str(), getTitle().c_str(), npadsx, npadsy, getCanvasWidth(), getCanvasHeight());
  for (int i=0; i< npadsx*npadsy; i++) {
    canvas->cd(i+1);
    std::stringstream st;
    st << "Pad" << i+1;
    std::string objname = "";
    if ((itr = params.find(st.str())) != params.end()) {
      objname = itr->second;
    }
    if (!objname.empty() && !MEs.empty()) {
      ME_List_iterator obj = MEs.find(objname);
      if (obj != MEs.end()) {
	std::string statOpt = obj->second->getParameter("SetOptStat");
        if (statOpt != "" ) {
	  gStyle->SetOptStat(statOpt.c_str());
        }
	std::string leftMargin = obj->second->getParameter("SetLeftMargin");
        if (leftMargin != "" ) {
	  gPad->SetLeftMargin(atof(leftMargin.c_str()));
          // gStyle->SetOptStat(statOpt.c_str());
        }
	std::string rightMargin = obj->second->getParameter("SetRightMargin");
        if (rightMargin != "" ) {
          gPad->SetRightMargin(atof(rightMargin.c_str()));
          // gStyle->SetOptStat(statOpt.c_str());
        }

	std::string logx = obj->second->getParameter("SetLogx");
        if (logx!= "" ) {
          gPad->SetLogx(atoi(logx.c_str()));
          // gStyle->SetOptStat(statOpt.c_str()); 
        }
	std::string logy = obj->second->getParameter("SetLogy");
        if (logx!= "" ) {
          gPad->SetLogy(atoi(logy.c_str()));
          // gStyle->SetOptStat(statOpt.c_str()); 
        }


	if (obj->second->getParameter("SetStats") != "") {
	  int stats = strtol( obj->second->getParameter("SetStats").c_str(), &stopstring, 10 );
	  obj->second->getObject()->SetStats(bool(stats));
	}

	obj->second->Draw();
      }      
    }
  }
}


EmuMonitoringCanvas::~EmuMonitoringCanvas()
{
	if (canvas != NULL) {
		delete canvas;
		canvas = NULL;
	}
}

void EmuMonitoringCanvas::setName(std::string newname)
{
	name = newname;
	if (canvas != NULL) {
		canvas->SetName(getFullName().c_str());
	}
}

void EmuMonitoringCanvas::setPrefix(std::string newprefix)
{
        prefix = newprefix;
        if (canvas != NULL) {
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
        if (canvas != NULL) {
                canvas->SetTitle(getTitle().c_str());
        }
}

int EmuMonitoringCanvas::setParameter(std::string parname, std::string parvalue)
{
        if (canvas != NULL) {
		params[parname] = parvalue;
		return 0;
        } else return 1;
	
}


int EmuMonitoringCanvas::setParameters(std::map<std::string, std::string> newparams, bool resetParams)
{
	std::map<std::string, std::string>::iterator itr;
	if (resetParams) { 
		params.clear();
		params = newparams;
	} else {
		// == Append to parameters list
		for (itr = newparams.begin(); itr != newparams.end(); ++itr) {
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
  for(unsigned int i=0; i<children->getLength(); i++){
	std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
	if ( children->item(i)->hasChildNodes() ) {
		std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
		obj_info[paramname] = param;
	}
  }
  /*
  for  (itr = obj_info.begin(); itr != obj_info.end(); ++itr) {
	std::cout << itr->first << ":" << itr->second << std::endl;
  }
  */

	if (obj_info.size() > 0) {
	  // == Construct Monitoring Canvas Name
	  std::string objname = "";
	  if ((itr = obj_info.find("Type")) != obj_info.end()) {
		// string typestr = itr->second;
		objname += itr->second;
		type = itr->second;
		obj_info.erase("Type");
		
	  }
	  if ((itr = obj_info.find("Prefix")) != obj_info.end()) {
                objname += itr->second;
		prefix = itr->second;
		obj_info.erase("Prefix");		
          }
	  if ((itr = obj_info.find("Name")) != obj_info.end()) {
                objname += itr->second;
		name = itr->second;
		obj_info.erase("Name");
          }
	  if ((itr = obj_info.find("Folder")) != obj_info.end()) {
                objname += itr->second;
		folder = itr->second;
		obj_info.erase("Folder");
          }
	  // name = objname;
	  
	  // == Get Monitoring Canvas Title
	  if ((itr = obj_info.find("Title")) != obj_info.end()) {
                title = itr->second;
		obj_info.erase("Title");
          }


	  // == Create Monitoring Canvas Parameters map
	  params.clear();
	  for (itr = obj_info.begin(); itr != obj_info.end(); ++itr) {
		params[itr->first] = itr->second; 
		// std::cout << "\t" << itr->first << ":" << itr->second << std::endl;
	  }
        }
	return 0;
}
