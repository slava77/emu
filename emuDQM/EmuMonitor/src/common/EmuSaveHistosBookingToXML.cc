#include "EmuLocalPlotter.h"
#include "XMLUtils.h"

#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <TObject.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>


using namespace XERCES_CPP_NAMESPACE;

void EmuLocalPlotter::saveHistosBookingToXML(const char *filename) 
{
  string hname;
  LOG4CPLUS_INFO(logger_, "Saving Booking info to " << filename);

  if (filename == "") {
    LOG4CPLUS_ERROR (logger_, "Invalid output file: " << filename);
    return;
  }

  XMLPlatformUtils::Initialize();
  DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(X("Core"));
  DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();
  if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
                theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

  DOMDocument* doc = impl->createDocument(
					  0,                    // root element namespace URI.
					  X("Booking"),         // root element name
					  0);                   // document type object (DTD).

  DOMElement* rootElem = doc->getDocumentElement();
  /*
  DOMElement*  prodElem = doc->createElement(X("product"));
  rootElem->appendChild(prodElem);

  DOMText*    prodDataVal = doc->createTextNode(X("Xerces-C"));
  prodElem->appendChild(prodDataVal);

  DOMElement*  catElem = doc->createElement(X("category"));
  rootElem->appendChild(catElem);

  catElem->setAttribute(X("idea"), X("great"));

  DOMText*    catDataVal = doc->createTextNode(X("XML Parsing Tools"));
  catElem->appendChild(catDataVal);

  DOMElement*  devByElem = doc->createElement(X("developedBy"));
  rootElem->appendChild(devByElem);

  DOMText*    devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
  devByElem->appendChild(devByDataVal);
  */
  

  
  bool fSavedCommon = false;
  bool fSavedChamber = false;
  for (map<int, map<string, TH1*> >::iterator itr = histos.begin(); itr != histos.end(); ++itr) {    
    if (((itr->first == 0) && fSavedCommon) || ((itr->first != 0) && fSavedChamber) ) continue;
    
    for (map<string, TH1*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
      TH1* h = h_itr->second;
      
      DOMElement* histo = doc->createElement(X("Histogram"));
      std::string val = "";
      
      if (h->InheritsFrom(TH1::Class())) val = "h1_";
      if (h->InheritsFrom(TH2::Class())) val = "h2_";
      if (h->InheritsFrom(TH3::Class())) val = "h3_";
      if (h->InheritsFrom(TProfile::Class())) val = "hp_";
      if (h->InheritsFrom(TProfile2D::Class())) val = "hp2_";
      string type = val;
	  
      if (!val.empty()) histo->appendChild(makeElement(doc,"Type",val));

      val =h->GetName();
      if (itr->first == 0) { // Common or DDU histos
	histo->appendChild(makeElement(doc,"Prefix","DDU_id_"));
	std::string mask = "hist/h";
	if (val.find(mask) != std::string::npos) {
	  val.erase(val.find(mask), mask.length());
	}
	mask = "DDU_";
	if (val.find(mask) != std::string::npos) {
	  val.erase(val.find(mask), mask.length());
	}
	if (!val.empty()) histo->appendChild(makeElement(doc,"Name",val));
      } else { // Chamber histos
	histo->appendChild(makeElement(doc,"Prefix","CSC_id_id_"));
	std::string mask = "hist/h_";	
	if (val.find(mask) != std::string::npos) {
	  val = val.replace(val.find(mask),mask.length(),"");
	}

	mask = Form("CSC_%03d_%02d_", (itr->first>>4) &0xFF, itr->first & 0xF);
	if (val.find(mask) != std::string::npos) {
	  val = val.replace(val.find(mask),mask.length(),"");
	}
	
	if (!val.empty()) histo->appendChild(makeElement(doc,"Name",val));
      }

      val = h->GetTitle();
      if (!val.empty()) histo->appendChild(makeElement(doc,"Title",val));
      val = h->GetOption();
      if (!val.empty()) histo->appendChild(makeElement(doc,"SetOption",val));
      // val = Form("%d", gStyle->GetOptStat());
      histo->appendChild(makeElement(doc,"SetStats",h->TestBit(h->kNoStats)?"0":"1"));

      // if (!val.empty()) histo->appendChild(makeElement(doc,"SetOptStat",val));
	
      if (h->GetXaxis()) {
	val = Form("%.0f", h->GetXaxis()->GetXmin());
	histo->appendChild(makeElement(doc,"XMin",val));
	val = Form("%.0f", h->GetXaxis()->GetXmax());
	histo->appendChild(makeElement(doc,"XMax",val));
	val = Form("%d", h->GetXaxis()->GetNbins());
	histo->appendChild(makeElement(doc,"XBins",val));
	val = h->GetXaxis()->GetTitle();
	if (!val.empty()) histo->appendChild(makeElement(doc,"XTitle",val));

	val = "";
	for (int i=0; i<= h->GetXaxis()->GetNbins(); i++) {
	  string ref = Form("%d",i);
	  string label = h->GetXaxis()->GetBinLabel(i);	  
	  if (!label.empty() && (ref.compare(label) != 0)) {
	    val += ref+"=\'"+label+"\'|";
	  }
	}
	if (!val.empty()) histo->appendChild(makeElement(doc,"SetXLabels",val));

      }
      if (h->GetYaxis()) {
	if (type.find("h1_") == std::string::npos) {
	  val = Form("%.0f", h->GetYaxis()->GetXmin());
	  histo->appendChild(makeElement(doc,"YMin",val));
	  val = Form("%.0f", h->GetYaxis()->GetXmax());
	  histo->appendChild(makeElement(doc,"YMax",val));
	  val = Form("%d", h->GetYaxis()->GetNbins());
	  histo->appendChild(makeElement(doc,"YBins",val));	  
	}
	val = h->GetYaxis()->GetTitle();
	if (!val.empty()) histo->appendChild(makeElement(doc,"YTitle",val));

	val = "";
	for (int i=0; i<= h->GetYaxis()->GetNbins(); i++) {
	  string ref = Form("%d",i);
	  string label = h->GetYaxis()->GetBinLabel(i);	  
	  if (!label.empty() && (ref.compare(label) != 0)) {
	    val += ref+"=\'"+label+"\'|";
	  }
	}
	if (!val.empty()) histo->appendChild(makeElement(doc,"SetYLabels",val));

      }
      if (h->GetZaxis()) {
	if (type.find("h3_") != std::string::npos) {
	  val = Form("%.0f", h->GetZaxis()->GetXmin());
	  histo->appendChild(makeElement(doc,"ZMin",val));
	  val = Form("%.0f", h->GetZaxis()->GetXmax());
	  histo->appendChild(makeElement(doc,"ZMax",val));
	  val = Form("%d", h->GetZaxis()->GetNbins());
	  histo->appendChild(makeElement(doc,"ZBins",val));
	  val = h->GetZaxis()->GetTitle();
	  if (!val.empty()) histo->appendChild(makeElement(doc,"ZTitle",val));
	}
      }
	
      
      

      rootElem->appendChild(histo);
			     
      //h_itr->second->Write();
    }
    if (itr->first == 0) fSavedCommon = true;
    else fSavedChamber = true;
  }
  
  XMLFormatTarget *outTarget;

  outTarget = new LocalFileFormatTarget(filename);
  // DOMNode                     *doc = parser->getDocument();
  //
  // do the serialization through DOMWriter::writeNode();
  //
  theSerializer->writeNode(outTarget, *doc);

  doc->release();
  delete theSerializer;
  delete outTarget;
  XMLPlatformUtils::Terminate();

}

