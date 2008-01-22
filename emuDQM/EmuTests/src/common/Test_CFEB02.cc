#include "Test_CFEB02.h"

using namespace XERCES_CPP_NAMESPACE;

Test_CFEB02::Test_CFEB02(std::string dfile): dataFile(dfile), testID("CFEB02") {
  init();
  nExpectedEvents = 10000;
}

Test_CFEB02::~Test_CFEB02() 
{
  for (cscTestCanvases::iterator itr=tcnvs.begin(); itr != tcnvs.end(); ++itr) {
    TestCanvases& cnvs = itr->second;
    TestCanvases::iterator c_itr;
    for (c_itr=cnvs.begin(); c_itr != cnvs.end(); ++c_itr) {
      delete c_itr->second;
    }
  }
	
}

void Test_CFEB02::init() {
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
}

int Test_CFEB02::loadTestCfg() 
{
  if (configFile == "") {
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
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("Booking") );
  if( l->getLength() != 1 ){
    //  LOG4CPLUS_ERROR (logger_, "There is not exactly one Booking node in configuration");
    return 1;
  }
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("TestResult") );
  if( itemList->getLength() == 0 ){
    //   LOG4CPLUS_ERROR (logger_, "There no histograms to book");
    return 1;
  }
  for(uint32_t i=0; i<itemList->getLength(); i++){
    std::map<std::string, std::string> obj_info;
    std::map<std::string, std::string>::iterator itr;
    DOMNodeList *children = itemList->item(i)->getChildNodes();
    for(unsigned int i=0; i<children->getLength(); i++){
      std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
      if ( children->item(i)->hasChildNodes() ) {
	std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
	obj_info[paramname] = param;
      }
    }
   
    itr = obj_info.find("Test"); 
    if ((itr != obj_info.end()) && (itr->second == testID)) {
	itr = obj_info.find("Name");
      if ((itr != obj_info.end()) && (itr->second != "")) {
	std::cout << "Found info for " << itr->second << std::endl;
	xmlCfg[itr->second] = obj_info;
      }
    }

    /*    if (obj_info.size() > 0) {
      params.clear();
      for (itr = obj_info.begin(); itr != obj_info.end(); ++itr) {
	params[itr->first] = itr->second;
      }
    }
    */
  }
  delete parser;
  return 0;
}

TestData2D parseMask(std::string s)
{

  TestData2D mask;
  mask.Nbins = 80;
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
      if (range_pair.find(":") != std::string::npos) {
        int layer = strtol(range_pair.substr(0,range_pair.find(":")).c_str(),  &stopstring, 10);
        std::string chans = range_pair.substr(range_pair.find(":")+1, range_pair.length());
	
        if (chans.find("-") != std::string::npos) {
	  int ch_start=0;
	  int ch_end=0;
	  ch_start = strtol(chans.substr(0,chans.find("-")).c_str(),  &stopstring, 10);
	  ch_end = strtol(chans.substr(chans.find("-")+1,chans.length()).c_str(),  &stopstring, 10);
          for (int i=ch_start; i<= ch_end; i++) {
	    mask.content[layer-1][i-1]=1;
	    std::cout << Form("mask chan %d:%d", layer, i) << std::endl; 

	  }
	} else {
	  int chan = strtol(chans.c_str(),  &stopstring, 10);
	  mask.content[layer-1][chan-1] = 1;
	  std::cout << Form("mask chan %d:%d", layer, chan) << std::endl;
	}
      }
      end_pos = tmp.find(")");
      start_pos = tmp.find("(");
      
    }
  return mask;
}


int Test_CFEB02::loadMasks() 
{
  if (masksFile == "") {
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
  if( l->getLength() != 1 ){
    //  LOG4CPLUS_ERROR (logger_, "There is not exactly one Booking node in configuration");
    return 1;
  }
  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("CSCMasks") );
  if( itemList->getLength() == 0 ){
    //   LOG4CPLUS_ERROR (logger_, "There no histograms to book");
    return 1;
  }
  for(uint32_t i=0; i<itemList->getLength(); i++){
    std::map<std::string, std::string> obj_info;
    std::map<std::string, std::string>::iterator itr;
    DOMNodeList *children = itemList->item(i)->getChildNodes();
    for(unsigned int i=0; i<children->getLength(); i++){
      std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
      if ( children->item(i)->hasChildNodes() ) {
	std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
	obj_info[paramname] = param;
      }
    }
    itr = obj_info.find("CSC");
    if (itr != obj_info.end()) {
      std::cout << "Found masks for " << itr->second << std::endl;
      if (obj_info["CFEBChans"] != "") {
	tmasks[itr->second]=parseMask(obj_info["CFEBChans"]);
      }
    }
  }
  delete parser;
  return 0;
}


void Test_CFEB02::setCSCMapFile(std::string filename)
{
  if (filename != "") {
    cscMapFile = filename;
    cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
  }


}

std::string Test_CFEB02::getCSCTypeLabel(int endcap, int station, int ring )
{
  std::string label = "Unknown";
  std::ostringstream st;
  if ((endcap > 0) && (station>0) && (ring>0)) {
    if (endcap==1) {
      st << "ME+" << station << "." << ring;
      label = st.str();
    } else if (endcap==2) {
      st << "ME-" << station << "." << ring;
      label = st.str();
    } else {
      label = "Unknown";
    }
  }
  return label;
}


std::string Test_CFEB02::getCSCFromMap(int crate, int slot)
{
  int iendcap = -1;
  int istation = -1;
  int iring = -1;
  int cscposition = -1;

  int id = cscMapping.chamber(iendcap, istation, crate, slot, -1);
  if (id==0) {
    return "";
  }
  CSCDetId cid( id );
  iendcap = cid.endcap();
  istation = cid.station();
  iring = cid.ring();
  cscposition = cid.chamber();

  std::string tlabel = getCSCTypeLabel(iendcap, istation, iring );
  return tlabel+"."+Form("%02d", cscposition);
}


TestData Test_CFEB02::initCSC(std::string cscID) {
  nCSCEvents[cscID]=0;

  CFEBSCAData scadata;
  scadata.Nbins = 80;
  scadata.Nlayers = 6;
  memset(scadata.content, 0, sizeof (scadata.content));
  sdata[cscID] = scadata;

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = 80;
  cfebdata.Nlayers = 6;	
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));

  // Channels mask
  if (tmasks.find(cscID) != tmasks.end()) {
    cscdata["_MASK"]=tmasks[cscID];
  } else {
    cscdata["_MASK"]=cfebdata;
  }

  // mv0 - initial pedestals
  cscdata["_MV0"]=cfebdata;

  // rms0 - initial rms
  cscdata["_RMS0"]=cfebdata;

  // Q12
  cscdata["_Q12"]=cfebdata;

  // Q345
  cscdata["_Q345"]=cfebdata;

  // Q3
  cscdata["_Q3"]=cfebdata;

  // Q4
  cscdata["_Q4"]=cfebdata;

  // Q5
  cscdata["_Q5"]=cfebdata;

  // R01 - Overall pedestal
  cscdata["R01"]=cfebdata;

  // R02 - Overall noise
  cscdata["R02"]=cfebdata;

  // R03 - P12 pedestal
  cscdata["R03"]=cfebdata;

  // R04 - P12 noise
  cscdata["R04"]=cfebdata;

  // R05 - P345 pedestal
  cscdata["R05"]=cfebdata;

  // R06 - P345 noise
  cscdata["R06"]=cfebdata;
  
  // R07 - C(12)(12) covariance matrix element
  cscdata["R07"]=cfebdata;
  
  // R08 - C(12)3 covariance matrix element
  cscdata["R08"]=cfebdata;
 
  // R09 - C(12)4 covariance matrix element
  cscdata["R09"]=cfebdata;
  
  // R10 - C(12)5 covariance matrix element
  cscdata["R10"]=cfebdata;

  // R11 - C33 covariance matrix element
  cscdata["R11"]=cfebdata;

  // R12 - C34 covariance matrix element
  cscdata["R12"]=cfebdata;

  // R13 - C35 covariance matrix element
  cscdata["R13"]=cfebdata;

  // R14 - C44 covariance matrix element
  cscdata["R14"]=cfebdata;

  // R15 - C45 covariance matrix element
  cscdata["R15"]=cfebdata;

  // R16 - C45 covariance matrix element
  cscdata["R16"]=cfebdata;

  // R17 - RMS of SCA pedestals
  cscdata["R17"]=cfebdata;


  bookMonHistosCSC(cscID);
  
  return cscdata;
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
      if (label_pair.find("=") != std::string::npos) {
        int nbin = strtol(label_pair.substr(0,label_pair.find("=")).c_str(),  &stopstring, 10);
        std::string label = label_pair.substr(label_pair.find("=")+1, label_pair.length());
        while (label.find("\'") != std::string::npos) {
          label.erase(label.find("\'"),1);
        }
        labels[nbin] = label;
      }
      pos = tmp.find("|");
    }
  return labels;
}


int applyParameters(TH1* object, bookParams& params)
{
  char* stopstring;
  std::map<std::string, std::string>::iterator itr;
  // !!! TODO: Add object class check
  if (object != NULL) {
    // std::cout << "Booked " << getFullName() << std::endl;
    if (((itr = params.find("XTitle")) != params.end()) ||
        ((itr = params.find("XLabel")) != params.end())) {
      object->SetXTitle(itr->second.c_str());
    }
    if (((itr = params.find("YTitle")) != params.end()) ||
        ((itr = params.find("YLabel")) != params.end())) {
      object->SetYTitle(itr->second.c_str());
    }
    if (((itr = params.find("ZTitle")) != params.end()) ||
        ((itr = params.find("ZLabel")) != params.end())) {
      object->SetZTitle(itr->second.c_str());
    }

    if ((itr = params.find("SetOption")) != params.end()) {
      object->SetOption(itr->second.c_str());
      //object->Draw();
    }

    /*
      if ((itr = params.find("SetOptStat")) != params.end()) {
      gStyle->SetOptStat(itr->second.c_str());

      }
    */

    if ((itr = params.find("SetStats")) != params.end()) {
      int stats = strtol( itr->second.c_str(), &stopstring, 10 );
      object->SetStats(bool(stats));
    }


   object->SetFillColor(48);

    if ((itr = params.find("SetFillColor")) != params.end()) {
      int color = strtol( itr->second.c_str(), &stopstring, 10 );
      object->SetFillColor(color);
    }

    if ((itr = params.find("SetXLabels")) != params.end()) {
      std::map<int, std::string> labels = ParseAxisLabels(itr->second);
      for (std::map<int, std::string>::iterator l_itr = labels.begin(); l_itr != labels.end(); ++l_itr)
        {
          object->GetXaxis()->SetBinLabel(l_itr->first, l_itr->second.c_str());
        }


    }

    if ((itr = params.find("SetYLabels")) != params.end()) {
      std::map<int, std::string> labels = ParseAxisLabels(itr->second);
      for (std::map<int, std::string>::iterator l_itr = labels.begin(); l_itr != labels.end(); ++l_itr)
        {
          object->GetYaxis()->SetBinLabel(l_itr->first, l_itr->second.c_str());
        }
    }
    if ((itr = params.find("LabelsOption")) != params.end()) {
      std::string st = itr->second;
      if (st.find(",") != std::string::npos) {
        std::string opt = st.substr(0,st.find(",")) ;
        std::string axis = st.substr(st.find(",")+1,st.length());
        object->LabelsOption(opt.c_str(),axis.c_str());
      }
    }


    if ((itr = params.find("SetLabelSize")) != params.end()) {
      std::string st = itr->second;
      if (st.find(",") != std::string::npos) {
        double opt = atof(st.substr(0,st.find(",")).c_str()) ;
        std::string axis = st.substr(st.find(",")+1,st.length());
        object->SetLabelSize(opt,axis.c_str());
      }
    }
    if ((itr = params.find("SetTitleOffset")) != params.end()) {
      std::string st = itr->second;
      if (st.find(",") != std::string::npos) {
        double opt = atof(st.substr(0,st.find(",")).c_str()) ;
        std::string axis = st.substr(st.find(",")+1,st.length());
        object->SetTitleOffset(opt,axis.c_str());
      }
    }
    if ((itr = params.find("SetMinimum")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      object->SetMinimum(opt);
    }
    if ((itr = params.find("SetMaximum")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      object->SetMaximum(opt);
    }

    if ((itr = params.find("SetNdivisionsX")) != params.end()) {
      int opt = strtol( itr->second.c_str(), &stopstring, 10 );
      if (object) {
        object->SetNdivisions(opt,"X");
        object->GetXaxis()->CenterLabels(true);
      }

    }

    if ((itr = params.find("SetNdivisionsY")) != params.end()) {
      int opt = strtol( itr->second.c_str(), &stopstring, 10 );
      if (object) {
        object->SetNdivisions(opt,"Y");
        object->GetYaxis()->CenterLabels(true);
      }
    }
    if ((itr = params.find("SetTickLengthX")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      if (object) {
        object->SetTickLength(opt,"X");
      }
    }

    if ((itr = params.find("SetTickLengthY")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      if (object) {
        object->SetTickLength(opt,"Y");
      }
    }

    if ((itr = params.find("SetLabelSizeX")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      if (object) {
        object->SetLabelSize(opt,"X");

      }
    }

    if ((itr = params.find("SetLabelSizeY")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      if (object) {
	//        object->GetYaxis()->SetLabelSize(opt);
        object->SetLabelSize(opt,"Y");
      }
    }
    if ((itr = params.find("SetErrorOption")) != params.end()) {
      std::string st = itr->second;
      if (object) {
        reinterpret_cast<TProfile*>(object)->SetErrorOption(st.c_str());
      }
    }


    if ((itr = params.find("SetLabelSizeZ")) != params.end()) {
      std::string st = itr->second;
      double opt = atof(st.c_str()) ;
      if (object) {
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


void Test_CFEB02::bookMonHistosCSC(std::string cscID) {
  MonHistos cschistos;
  TestCanvases csccnvs;
  char *stopstring;
  for (testParamsCfg::iterator itr=xmlCfg.begin(); itr != xmlCfg.end(); ++itr) {
    bookParams& params = itr->second;
    if (params.find("Type") != params.end()) {
      std::string cnvtype = params["Type"];
	std::string name = cscID+"_"+testID+"_"+params["Name"];
	std::string title = cscID+": "+testID+" "+params["Title"];
	double xmin=0., xmax=0.; int xbins=0;
	double ymin=0., ymax=0.; int ybins=0;
	std::string xtitle = params["XTitle"];
	std::string ytitle = params["YTitle"];
	double low0limit=0., low1limit=0.;
	double high0limit=0., high1limit=0.;
	if (params["XMin"] != "") {
	  xmin = atof(params["XMin"].c_str());
	}
	if (params["XMax"] != "") {
	  xmax = atof(params["XMax"].c_str());
	}
	if (params["YMin"] != "") {
	  ymin = atof(params["YMin"].c_str());
	}
	if (params["YMax"] != "") {
	  ymax = atof(params["YMax"].c_str());
	}
	if (params["XBins"] != "") {
	  xbins = strtol(params["XBins"].c_str(), &stopstring, 10);
	}
	if (params["YBins"] != "") {
	  ybins = strtol(params["YBins"].c_str(), &stopstring, 10);
	}
      if (cnvtype == "cfeb_cnv") {
	if (params["Low0Limit"] != "") {
	  low0limit = atof(params["Low0Limit"].c_str());
	}
	if (params["Low1Limit"] != "") {
	  low1limit = atof(params["Low1Limit"].c_str());
	}
	if (params["High0Limit"] != "") {
	  high0limit = atof(params["High0Limit"].c_str());
	}
	if (params["High1Limit"] != "") {
	  high1limit = atof(params["High1Limit"].c_str());
	}

	// TestCanvas_6gr1h* cnv = new TestCanvas_6gr1h((cscID+"_CFEB02_R03").c_str(), (cscID+": CFEB02 R03").c_str(),80, 0.0, 80.0, 60, 0., 6.0);	
	TestCanvas_6gr1h* cnv = new TestCanvas_6gr1h(name, title,xbins, xmin, xmax, ybins, ymin, ymax);
	cnv->SetXTitle(xtitle);
	cnv->SetYTitle(ytitle);
	cnv->AddTextTest(testID);
	cnv->AddTextResult(params["Title"]);
	cnv->SetLimits(low1limit,low0limit, high0limit, high1limit);
	csccnvs[itr->first]=cnv;
      }

      if (cnvtype.find("h") == 0) {
	if (cnvtype.find("h1") != std::string::npos) {
	  cschistos[itr->first] = new TH1F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
	} else
	  if (cnvtype.find("h2") != std::string::npos) {
	    cschistos[itr->first] = new TH2F((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax, ybins, ymin, ymax);
	  } else
	    if (cnvtype.find("hp") != std::string::npos) {
	      cschistos[itr->first] = new TProfile((cnvtype+"_"+name).c_str(), title.c_str(), xbins, xmin, xmax);
	    }
	applyParameters(cschistos[itr->first], params);
      }


    } 
  }

  tcnvs[cscID] = csccnvs;

  cschistos["_V00"] = new TH2F((cscID+"_"+testID+"__V00").c_str(), "CSC Format Errors", 1, 0, 1, 20, 0, 20);
  cschistos["_V00"]->SetOption("textcolz");
  /*
  cschistos["V00"] = new TH1F((cscID+"_"+testID+"_V00").c_str(), "CSC Format Errors Frequency", 20, 0, 20);
  cschistos["V00"]->SetOption("texthbar1");
  cschistos["V01"] = new TH2F((cscID+"_"+testID+"_V01").c_str(), "Signal line", 16, 0, 16, 80, -20, 20);
  cschistos["V01"]->SetOption("colz");
  cschistos["V02"] = new TH1F((cscID+"_"+testID+"_V02").c_str(), "Q4 with dynamic ped substraction", 40, -20, 20);
  cschistos["V02"]->SetFillColor(48);
  cschistos["V03"] = new TH1F((cscID+"_"+testID+"_V03").c_str(), "SCA Cell Occupancy", 96, 0, 96);
  cschistos["V03"]->SetFillColor(48);
  cschistos["V04"] = new TH1F((cscID+"_"+testID+"_V04").c_str(), "SCA Block Occupancy", 12, 0, 12);
  cschistos["V04"]->SetFillColor(48);
  */
  mhistos[cscID]=cschistos;

  //  return cschistos;
}


void Test_CFEB02::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) {
  nTotalEvents++;

  //   uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }

  if(bin_checker.errors() != 0) {
    std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
    doBinCheck();
    return;
  }

  CSCDDUEventData dduData((uint16_t *) data);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();

  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); 
      chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    analyzeCSC(*chamberDataItr);
  }
	
}


void Test_CFEB02::doBinCheck() {

  std::map<int,long> checkerErrors = bin_checker.errorsDetailed();
  std::map<int,long>::const_iterator chamber = checkerErrors.begin();
  while( chamber != checkerErrors.end() ){
    // int ChamberID     = chamber->first;
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;
    if ((CrateID ==255) ||
        (chamber->second & 0x80)) { chamber++; continue;} // = Skip chamber detection if DMB header is missing (Error code 6)

    std::string cscID = getCSCFromMap(CrateID, DMBSlot);
  
    cscTestData::iterator td_itr = tdata.find(cscID);
    if ( (td_itr == tdata.end()) || (tdata.size() == 0) ) {
      std::cout << "Found " << cscID << std::endl;
      tdata[cscID] = initCSC(cscID);
      //      mhistos[cscID] = bookMonHistosCSC(cscID);
    }

    
    bool isCSCError = false;
    TH1* mo = mhistos[cscID]["_V00"];
    if (mo) {
       for(int bit=5; bit<24; bit++)
        if( chamber->second & (1<<bit) ) {
          isCSCError = true;
          mo->Fill(0.,bit-5);
	}
    }
    nCSCBadEvents[cscID]++;
    chamber++;
  }
}



void Test_CFEB02::analyzeCSC(const CSCEventData& data) {
  int conv_blk[16]={0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11};
  std::string cscID = getCSCFromMap(data.dmbHeader().crateID(), data.dmbHeader().dmbID());
  
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) ) {
    std::cout << "Found " << cscID << std::endl;
    tdata[cscID] = initCSC(cscID);
    //    mhistos[cscID] = bookMonHistosCSC(cscID);
  }
  nCSCEvents[cscID]++;
  uint32_t& nEvents=nCSCEvents[cscID];

  TestData& cscdata = tdata[cscID];
  TestData2D& _mv0 = cscdata["_MV0"];
  TestData2D& _rms0 = cscdata["_RMS0"];
  TestData2D& _q12 = cscdata["_Q12"];
  TestData2D& _q345 = cscdata["_Q345"];
  TestData2D& _q3 = cscdata["_Q3"];
  TestData2D& _q4 = cscdata["_Q4"];
  TestData2D& _q5 = cscdata["_Q5"];
  
  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];
  //  TestData2D& r03 = cscdata["R03"];
  TestData2D& r04 = cscdata["R04"];
  TestData2D& r05 = cscdata["R05"];
  TestData2D& r06 = cscdata["R06"];
  TestData2D& r07 = cscdata["R07"];
  TestData2D& r08 = cscdata["R08"];
  TestData2D& r09 = cscdata["R09"];
  TestData2D& r10 = cscdata["R10"];
  TestData2D& r11 = cscdata["R11"];
  TestData2D& r12 = cscdata["R12"];
  TestData2D& r13 = cscdata["R13"];
  TestData2D& r14 = cscdata["R14"];
  TestData2D& r15 = cscdata["R15"];
  TestData2D& r16 = cscdata["R16"];
  TestData2D& r17 = cscdata["R17"];

  CFEBSCAData& scadata = sdata[cscID];
  
  MonHistos& cschistos = mhistos[cscID];
  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
  TH1F* v02 = reinterpret_cast<TH1F*>(cschistos["V02"]);
  TH1F* v03 = reinterpret_cast<TH1F*>(cschistos["V03"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);

  if (data.dmbHeader().cfebAvailable()){
    for (int icfeb=0; icfeb<5;icfeb++) {//loop over cfebs in a given chamber
      CSCCFEBData * cfebData =  data.cfebData(icfeb);
      if (!cfebData) continue;

      for (unsigned int layer = 1; layer <= 6; layer++){
	int nTimeSamples= cfebData->nTimeSamples();
	for(int strip = 1; strip <= 16; ++strip) {
	  double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts 
		      + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	  double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
	  if(v02) v02->Fill(Q4-Q12);

	  // int offset=0;
	  int blk_strt=0;
	  int cap=0;

	  for (int itime=0;itime<nTimeSamples;itime++){
	    CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	    int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	    if (v01) v01->Fill(itime, Qi-Q12);

	    if (nEvents <= 1000) {
	      _mv0.content[layer-1][icfeb*16+strip-1] += Qi;
	      _mv0.cnts[layer-1][icfeb*16+strip-1]++;
	      _rms0.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
              _rms0.cnts[layer-1][icfeb*16+strip-1]++;
	    }
	  }
	  if (nEvents > 1000) {
	    bool fEventValid = true;
	    double mv0 = _mv0.content[layer-1][icfeb*16+strip-1];
	    double rms0 = _rms0.content[layer-1][icfeb*16+strip-1];
	    // == Check that charges in first 5 timebins satisfy |Qi - mv0| < 3*rms0
	    for (int itime=0;itime<nTimeSamples;itime++){
	      CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	      int ADC = (int) ((timeSample->adcCounts)&0xFFF);
	      if ( fabs(ADC-mv0) >= 3*rms0) { 
		fEventValid=false; 
		/*std::cout << "Evt" <<nEvents << " " << cscID << " " << layer << ":"<< (icfeb*16+strip) << " skipped " 
		  <<  fabs(ADC-mv0) << ">=" << (3*rms0) <<  std::endl;
		*/
		break;
	      }
	    }
	    if (fEventValid) {
	      double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts 
			  + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	      double Q345=((cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts 
			   + (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts
			   + (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts)/3.;
	      double Q3 = (cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts;
	      double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
	      double Q5 = (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts;

	      _q12.content[layer-1][icfeb*16+strip-1] += Q12;
	      _q12.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q345.content[layer-1][icfeb*16+strip-1] += Q345;
	      _q345.cnts[layer-1][icfeb*16+strip-1]++;

	      _q3.content[layer-1][icfeb*16+strip-1] += Q3;
	      _q3.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q4.content[layer-1][icfeb*16+strip-1] += Q4;
	      _q4.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q5.content[layer-1][icfeb*16+strip-1] += Q5;
	      _q5.cnts[layer-1][icfeb*16+strip-1]++;

	      r04.content[layer-1][icfeb*16+strip-1] += Q12;
	      r04.cnts[layer-1][icfeb*16+strip-1]++;

	      r05.content[layer-1][icfeb*16+strip-1] += pow(Q12,2);
	      r05.cnts[layer-1][icfeb*16+strip-1]++;

	      r06.content[layer-1][icfeb*16+strip-1] += Q345;
	      r06.cnts[layer-1][icfeb*16+strip-1]++;

	      r07.content[layer-1][icfeb*16+strip-1] += pow(Q345,2);
	      r07.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      r08.content[layer-1][icfeb*16+strip-1] += Q12*Q12;
	      r08.cnts[layer-1][icfeb*16+strip-1]++;

	      r09.content[layer-1][icfeb*16+strip-1] += Q12*Q3;
	      r09.cnts[layer-1][icfeb*16+strip-1]++;

	      r10.content[layer-1][icfeb*16+strip-1] += Q12*Q4;
	      r10.cnts[layer-1][icfeb*16+strip-1]++;

	      r11.content[layer-1][icfeb*16+strip-1] += Q12*Q5;
	      r11.cnts[layer-1][icfeb*16+strip-1]++;

	      r12.content[layer-1][icfeb*16+strip-1] += Q3*Q3;
	      r12.cnts[layer-1][icfeb*16+strip-1]++;

	      r13.content[layer-1][icfeb*16+strip-1] += Q3*Q4;
	      r13.cnts[layer-1][icfeb*16+strip-1]++;

	      r14.content[layer-1][icfeb*16+strip-1] += Q3*Q5;
	      r14.cnts[layer-1][icfeb*16+strip-1]++;

	      r15.content[layer-1][icfeb*16+strip-1] += Q4*Q4;
	      r15.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      r16.content[layer-1][icfeb*16+strip-1] += Q4*Q5;
	      r16.cnts[layer-1][icfeb*16+strip-1]++;

	      r17.content[layer-1][icfeb*16+strip-1] += Q5*Q5;
	      r17.cnts[layer-1][icfeb*16+strip-1]++;

	      /*
		offset = 0;
		// == Find offset in sca block
		for (int itime=0;itime<nTimeSamples-1;itime++){
		if ( (cfebData->timeSlice(itime+1))->scaControllerWord(layer).sca_blk != 
		(cfebData->timeSlice(itime))->scaControllerWord(layer).sca_blk )  
		{
		std::cout << Form("%d:%d block %d change in sample %d", 
		layer, icfeb*16+strip, 
		(int)((cfebData->timeSlice(itime))->scaControllerWord(layer).sca_blk), itime+1 ) << std::endl;
		offset = nTimeSamples-itime-1;
		break;
		}
		}
	      */
	      for (int itime=0;itime<nTimeSamples;itime++){
		CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
		int Qi = (int) ((timeSample->adcCounts)&0xFFF);


		CSCCFEBSCAControllerWord scaControllerWord = (cfebData->timeSlice(itime))->scaControllerWord(layer);
		int scaBlock  = (int)(scaControllerWord.sca_blk);
		int trigTime = (int)(scaControllerWord.trig_time);
		int lctPhase = (int)(scaControllerWord.lct_phase);

		for(int pos=0;pos<8;pos++){if(trigTime==(1<<pos)) lctPhase=pos;}

		if(trigTime!=0){
		  cap = lctPhase+itime;
		  blk_strt =itime;
		}else{
		  cap=itime-blk_strt-1;
		}
		
		int scaNumber=8*conv_blk[scaBlock]+cap;

		// int scaNumber = 8*conv_blk[scaBlock]+(offset+itime)%8;

		if (scaNumber >= 96) {
		  std::cout << "Invalid SCA cell" << std::endl;
		}

		scadata.content[layer-1][icfeb*16+strip-1][scaNumber].value += Qi;
		scadata.content[layer-1][icfeb*16+strip-1][scaNumber].cnt++;
		v03->Fill(scaNumber);
		v04->Fill(conv_blk[scaBlock]);

		r01.content[layer-1][icfeb*16+strip-1] += Qi;
		r01.cnts[layer-1][icfeb*16+strip-1]++;
		r02.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
		r02.cnts[layer-1][icfeb*16+strip-1]++;
	      }
	    }
	  }
	}

      } // 1000 events
    }

    // Calculate mv0 and rms0
    if (nEvents == 1000) {
      for (int i=0; i<_mv0.Nlayers; i++) {
	for (int j=0; j<_mv0.Nbins; j++) {
	  //	  std::cout << (i+1) << ":" << (j+1) << " " << r01.content[i][j] << ", " <<r01.cnts[i][j]
	  //	    << ", " << r02.content[i][j] << ", " << r02.cnts[i][j] << ": ";
	  if (_mv0.cnts[i][j]==0) {
	    // std::cout << cscID << " " << i << ":" << j << " r01entries=0"<< std::endl;
	  } else {
	    _mv0.content[i][j] /= (double)(_mv0.cnts[i][j]);
	  }
	  if (_rms0.cnts[i][j]==0) {
	    // std::cout << cscID << " " << i << ":" << j << " r02 entries=0"<< std::endl;
	  } else {
	    double rms = sqrt(((_rms0.content[i][j])/((double)(_rms0.cnts[i][j]))- pow(_mv0.content[i][j],2)) );
	    _rms0.content[i][j]=rms;
	    //std:: cout << r01.content[i][j] << ", " << r02.content[i][j] << std::endl;
	  }
	
	}
      }
    }


  } // CFEB data available
  
}


void Test_CFEB02::finishCSC(std::string cscID) 
{
  
  if (nCSCEvents[cscID] < nExpectedEvents/2) {
    std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
    // = Set error 
    return;
  }
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end()) {
  
    TestData& cscdata= td_itr->second;

    // TestData2D& mask = cscdata["_MASK"];
    TestData2D& _q12 = cscdata["_Q12"];
    TestData2D& _q3 = cscdata["_Q3"];
    TestData2D& _q4 = cscdata["_Q4"];
    TestData2D& _q5 = cscdata["_Q5"];
  
    TestData2D& r01 = cscdata["R01"];
    TestData2D& r02 = cscdata["R02"];
    TestData2D& r03 = cscdata["R03"];
    TestData2D& r04 = cscdata["R04"];
    TestData2D& r05 = cscdata["R05"];
    TestData2D& r06 = cscdata["R06"];
    TestData2D& r07 = cscdata["R07"];
    TestData2D& r08 = cscdata["R08"];
    TestData2D& r09 = cscdata["R09"];
    TestData2D& r10 = cscdata["R10"];
    TestData2D& r11 = cscdata["R11"];
    TestData2D& r12 = cscdata["R12"];
    TestData2D& r13 = cscdata["R13"];
    TestData2D& r14 = cscdata["R14"];
    TestData2D& r15 = cscdata["R15"];
    TestData2D& r16 = cscdata["R16"];
    TestData2D& r17 = cscdata["R17"];

    CFEBSCAData& scadata = sdata[cscID];
    
    double rms = 0.;
    double covar = 0;

    for (int i=0; i<r01.Nlayers; i++) {
      for (int j=0; j<r01.Nbins; j++) {
	
	if (_q12.cnts[i][j]) _q12.content[i][j] /= (double)(_q12.cnts[i][j]);
	if (_q3.cnts[i][j]) _q3.content[i][j] /= (double)(_q3.cnts[i][j]);
	if (_q4.cnts[i][j]) _q4.content[i][j] /= (double)(_q4.cnts[i][j]);
	if (_q5.cnts[i][j]) _q5.content[i][j] /= (double)(_q5.cnts[i][j]);

	// == Calculate Overall pedestals and noise
	if (r01.cnts[i][j]) {
	  r01.content[i][j] /= (double)(r01.cnts[i][j]);

	  rms = sqrt( ((r02.content[i][j])/((double)(r02.cnts[i][j]))- pow(r01.content[i][j],2)) );
	  r02.content[i][j]=rms;
	}

	// == Calculate RMS of SCA pedestala
	double sca_mean=0.;
	double sca_mean_sum=0.;
	double sca_mean_sq_sum=0;
	int cells_cnt=0;
	for (int k=0; k<96;k++) {
	  if (scadata.content[i][j][k].cnt) {
	    cells_cnt++;
	    sca_mean=scadata.content[i][j][k].value / scadata.content[i][j][k].cnt;
	    sca_mean_sum+=sca_mean;
	    sca_mean_sq_sum+=pow(sca_mean,2);
	  }
	}
	sca_mean = sca_mean_sum/cells_cnt;
	rms = sqrt(sca_mean_sq_sum/cells_cnt - pow(sca_mean,2));

	// std::cout << Form("%s %d:%d %d %.2f",cscID.c_str(),i,j,cells_cnt, sca_mean) << std::endl;

	r03.content[i][j] = rms;

	// == Calculate time samples 1,2 pedestals and noise
	if (r04.cnts[i][j]) {
	  r04.content[i][j] /= (double)(r04.cnts[i][j]);

	  rms = sqrt( ((r05.content[i][j])/((double)(r05.cnts[i][j]))- pow(r04.content[i][j],2)) );
	  r05.content[i][j]=rms;
	}

	// == Calculate time samples 3,4,5 pedestals and noise
	if (r06.cnts[i][j]) {
	  r06.content[i][j] /= (double)(r06.cnts[i][j]);
	  rms = sqrt( ((r07.content[i][j])/((double)(r07.cnts[i][j]))- pow(r06.content[i][j],2)) );

	  r07.content[i][j]=rms;
	}

	// == Calculate covariance matrix elements

	// = C12|12
	if (r08.cnts[i][j]) {
	  covar  =  ((r08.content[i][j]) /(double)(r08.cnts[i][j])) - _q12.content[i][j]*_q12.content[i][j];
	  r08.content[i][j] = covar;
	}

	// = C12|3
	if (r09.cnts[i][j]) {
	  covar =  ((r09.content[i][j]) /(double)(r09.cnts[i][j])) - _q12.content[i][j]*_q3.content[i][j];
	  r09.content[i][j] = covar;
	}

	// = C12|4
	if (r10.cnts[i][j]) {
	  covar =  ((r10.content[i][j]) /(double)(r10.cnts[i][j])) - _q12.content[i][j]*_q4.content[i][j];
	  r10.content[i][j] = covar;
	}

	// = C12|5
	if (r11.cnts[i][j]) {
	  covar =  ((r11.content[i][j]) /(double)(r11.cnts[i][j])) - _q12.content[i][j]*_q5.content[i][j];
	  r11.content[i][j] = covar;
	}

	// = C3|3
	if (r12.cnts[i][j]) {
	  covar =  ((r12.content[i][j]) /(double)(r12.cnts[i][j])) - _q3.content[i][j]*_q3.content[i][j];
	  r12.content[i][j] = covar;
	}

	// = C3|4
	if (r13.cnts[i][j]) {
	  covar =  ((r13.content[i][j]) /(double)(r13.cnts[i][j])) - _q3.content[i][j]*_q4.content[i][j];
	  r13.content[i][j] = covar;
	}
	     
	// = C3|5
	if (r14.cnts[i][j]) {
	  covar =  ((r14.content[i][j]) /(double)(r14.cnts[i][j])) - _q3.content[i][j]*_q5.content[i][j];
	  r14.content[i][j] = covar;
	}

	// = C4|4
	if (r15.cnts[i][j]) {
	  covar =  ((r15.content[i][j]) /(double)(r15.cnts[i][j])) - _q4.content[i][j]*_q4.content[i][j];
	  r15.content[i][j] = covar;
	}

	// = C4|5
	if (r16.cnts[i][j]) {
	  covar =  ((r16.content[i][j]) /(double)(r16.cnts[i][j])) - _q4.content[i][j]*_q5.content[i][j];
	  r16.content[i][j] = covar;
	}
	  
	// = C5|5
	if (r17.cnts[i][j]) {
	  covar =  ((r17.content[i][j]) /(double)(r17.cnts[i][j])) - _q5.content[i][j]*_q5.content[i][j];
	  r17.content[i][j] = covar;	
	}

      }
    }

    // == Save results to text files
    std::string rpath = "Test_"+testID+"/"+outDir;
    std::string path = rpath+"/"+cscID+"/";

    std::ofstream res_out((path+cscID+"_CFEB02_01.results").c_str());
    res_out << "Layer Strip Pedestal Noise SCA_rms P12_Ped P12_rms P345_Ped P345_rms" << std::endl;
    for (int i=0; i<r03.Nlayers; i++) {
      for (int j=0; j<r03.Nbins; j++) {
	res_out << std::fixed << std::setprecision(2) << std::setw(5) << (i+1) << std::setw(6) << (j+1) 
		<< std::setw(9) << r01.content[i][j]  << std::setw(6) << r02.content[i][j]
		<< std::setw(8) << r03.content[i][j]  << std::setw(8) << r04.content[i][j]
		<< std::setw(8) << r05.content[i][j]  << std::setw(9) << r06.content[i][j]
		<< std::setw(9) << r07.content[i][j]
		<< std::endl;
      }
    }
    res_out.close();

    res_out.open((path+cscID+"_CFEB02_02.results").c_str());
    res_out << "Layer Strip C12|12 C12|3 C12|4 C12|5  C3|3  C3|4  C3|5  C4|4  C4|5  C5|5" << std::endl;
    for (int i=0; i<r03.Nlayers; i++) {
      for (int j=0; j<r03.Nbins; j++) {
	res_out << std::fixed << std::setprecision(2) << std::setw(5) << (i+1) << std::setw(6) << (j+1) 
		<< std::setw(7) << r08.content[i][j] << std::setw(6) << r09.content[i][j]  
		<< std::setw(6) << r10.content[i][j]
		<< std::setw(6) << r11.content[i][j]  << std::setw(6) << r12.content[i][j]  
		<< std::setw(6) << r13.content[i][j]  
		<< std::setw(6) << r14.content[i][j]  << std::setw(6) << r15.content[i][j]  << 
	  std::setw(6) << r16.content[i][j]
		<< std::setw(6) << r17.content[i][j]  << std::endl;
      }
    }
    res_out.close();
  }
}


void Test_CFEB02::finish() {

  char* stopstring;
  struct tm* clock;
  struct stat attrib;
  stat(dataFile.c_str(), &attrib);
  clock = localtime(&(attrib.st_mtime));
  std::string dataTime=asctime(clock);
  time_t now = time(NULL);  
  clock = localtime(&now);
  std::string testTime=asctime(clock);

  std::string rpath = "Test_"+testID+"/"+outDir;
  TString command = Form("mkdir -p %s", rpath.c_str());
  gSystem->Exec(command.Data());
  std::ofstream fres((rpath+"/test_results.js").c_str());
  int res=0;
  int sum_res=res;

  gStyle->SetPalette(1,0);

  std::string filename="Test_"+testID+"/"+rootFile;
  TFile root_res(filename.c_str(), "recreate");
  if (!root_res.IsZombie()) {
    root_res.cd();
    TDirectory* f = root_res.mkdir(("Test_"+testID).c_str());

    
    for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr) {
      bool fEnoughData = true;
      sum_res=0;
      std::string cscID = td_itr->first;
      fres << "['"+cscID+"',[" << std::endl;

      TDirectory * rdir = f->mkdir((cscID).c_str());
      std::string path = rpath+"/"+cscID+"/";
      TString command = Form("mkdir -p %s", path.c_str());
      gSystem->Exec(command.Data());

      rdir->cd();

      TH2F* mo = reinterpret_cast<TH2F*>(mhistos[cscID]["_V00"]);
      TH1F* mof = reinterpret_cast<TH1F*>(mhistos[cscID]["V00"]);
      double max_freq=0.;
      if (mo && mof) {
      	uint32_t nTotalEvents = nCSCEvents[cscID]+nCSCBadEvents[cscID];
      	for(int bit=1; bit<=20; bit++) {
          double freq = (100.0*mo->GetBinContent(1,bit))/nTotalEvents;
	  if (freq>0) mof->SetBinContent(bit, freq);
	  if (freq>max_freq) max_freq=freq;
      	}
      	mof->SetEntries(nTotalEvents);
	if (max_freq>0) mof->SetMaximum(max_freq);
	else mof->SetMaximum(1);
      }
   
      if (nCSCEvents[cscID] >= nExpectedEvents/2) {
	finishCSC(cscID);
      } else {
	std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
	// = Set error 
	sum_res=4;
	fres << "\t['V00','" << 4 << "']," << std::endl;
	fEnoughData = false;
	/*
	if (mhistos[cscID]["V00"]) {

	}	
	*/
      }



      TestData& cscdata= td_itr->second;
      TestData2D& mask = cscdata["_MASK"];

      TestCanvases& cnvs = tcnvs[cscID];
      TestCanvases::iterator c_itr;
      for (c_itr=cnvs.begin(); c_itr != cnvs.end(); ++c_itr) {
	std::string subtestID = c_itr->first;
	TestCanvas_6gr1h* cnv = c_itr->second;
	TestData::iterator itr = cscdata.find(subtestID);
	if (itr != cscdata.end()) {
	  TestData2D& data = itr->second;
	  cnv->AddTextDatafile(dataFile);
	  cnv->AddTextRun(dataTime);
	  cnv->AddTextAnalysis(testTime +", version " + ANALYSIS_VER);
	  if (fEnoughData) {
	    res=cnv->Fill(data,mask);
	    if (res>sum_res) sum_res=res;
	    fres << "\t['" << itr->first << "','" << res << "']," << std::endl;
	  }
	  cnv->SetCanvasSize(imgW, imgH);
	  cnv->Draw();
	  cnv->SaveAs(path+cscID+"_"+testID+"_"+subtestID);
	  cnv->Write();		
	}
      }      

      MonHistos& monhistos = mhistos[cscID];
      MonHistos::iterator m_itr;
      for (m_itr=monhistos.begin(); m_itr!=monhistos.end(); ++m_itr) {
	std::string subtestID = m_itr->first;
	bookParams& params =  xmlCfg[subtestID];
	std::string descr = params["Title"];

	TStyle defStyle(*gStyle);
	MonitoringCanvas* cnv= new MonitoringCanvas((cscID+"_"+testID+"_"+subtestID).c_str(), (cscID+"_"+testID+"_"+subtestID).c_str(), 
						    (cscID + " "+testID+" "+descr).c_str() ,
						    1, 1, imgW, imgH);
	cnv->SetCanvasSize(imgW, imgH);
	cnv->cd(1);
	TVirtualPad* cPad = cnv->GetPad(1);	

	std::string leftMargin = params["SetLeftMargin"];
	if (leftMargin != "" ) {
	  cPad->SetLeftMargin(atof(leftMargin.c_str()));
	}
	std::string rightMargin = params["SetRightMargin"];
	if (rightMargin != "" ) {
	  cPad->SetRightMargin(atof(rightMargin.c_str()));
	}

	std::string logx = params["SetLogx"];
	if (logx!= "") {
	  cPad->SetLogx();
	}
	std::string logy = params["SetLogy"];
	if (logy!= "" && (m_itr->second->GetMaximum()>0.)) {
       	  cPad->SetLogy();
	}

	std::string logz = params["SetLogz"];
	if (logz!= "" && (m_itr->second->GetMaximum()>0.) ) {
       	  cPad->SetLogz();
	}

	std::string gridx = params["SetGridx"];
	if (gridx!= "" ) {
	  cPad->SetGridx();
	}

	std::string gridy = params["SetGridy"];
	if (gridy!= "" ) {
	  cPad->SetGridy();
	}

	if (params["SetStats"] != "") {
	  int stats = strtol( params["SetStats"].c_str(), &stopstring, 10 );
	  m_itr->second->SetStats(bool(stats));
	}

	std::string statOpt = params["SetOptStat"];
	//if (statOpt != "" ) {
            gStyle->SetOptStat(statOpt.c_str());
	    // } 


        gStyle->SetPalette(1,0);
	m_itr->second->Draw();
	cnv->Draw();
	cnv->Print((path+cscID+"_"+testID+"_"+subtestID+".png").c_str());
	m_itr->second->Write();
	cnv->Write();
	delete cnv;
	defStyle.cd();
      }


      f->cd();
      fres << "\t['SUMMARY','" << sum_res << "']" << std::endl;
      fres << "]],"<< std::endl;
    }
    root_res.Close();
  }
  saveCSCList();

}



void Test_CFEB02::saveCSCList()
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
  for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr) {
    std::string cscID = td_itr->first;
    csc_list << "'"<< cscID << "'," << std::endl;
  }
  csc_list<< "]]" << std::endl;
}

