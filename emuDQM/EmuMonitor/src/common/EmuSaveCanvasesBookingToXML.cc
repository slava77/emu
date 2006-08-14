#include "EmuLocalPlotter.h"
#include "XMLUtils.h"

#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <TObject.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>

using namespace XERCES_CPP_NAMESPACE;

std::string ParseHistoName(int folder, std::string name)
{
  std::string val = name;
  if (folder == 0) { // Common or DDU histos
        std::string mask = "hist/h";
        if (val.find(mask) != std::string::npos) {
          val.erase(val.find(mask), mask.length());
        }
        mask = "DDU_";
        if (val.find(mask) != std::string::npos) {
          val.erase(val.find(mask), mask.length());
        }
      } else { // Chamber histos
        std::string mask = "hist/h_";
        if (val.find(mask) != std::string::npos) {
          val = val.replace(val.find(mask),mask.length(),"");
        }

        mask = Form("CSC_%03d_%02d_", (folder>>4) &0xFF, folder & 0xF);
        if (val.find(mask) != std::string::npos) {
          val = val.replace(val.find(mask),mask.length(),"");
        }
      }
  return val;

}

void EmuLocalPlotter::saveCanvasesBookingToXML(const char *filename) 
{
  string hname;
  LOG4CPLUS_INFO(logger_, "Saving Canvases Booking info to " << filename);

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
					  X("Canvases"),         // root element name
					  0);                   // document type object (DTD).

  DOMElement* rootElem = doc->getDocumentElement();
  
  bool fSavedCommon = false;
  bool fSavedChamber = false;
  for (map<int, map<string, ConsumerCanvas*> >::iterator itr = canvases.begin(); itr != canvases.end(); ++itr) {
    if (((itr->first == 0) && fSavedCommon) || ((itr->first != 0) && fSavedChamber) ) continue;
    
    for (map<string, ConsumerCanvas*>::iterator c_itr = itr->second.begin(); c_itr != itr->second.end(); ++c_itr) {
      ConsumerCanvas* cnv = c_itr->second;
      map<string, TH1*>& h_list =  histos[itr->first];
      
      DOMElement* canvas = doc->createElement(X("Canvas"));
      std::string val = "";
      canvas->appendChild(makeElement(doc,"Type","cnv_"));
      
      val =cnv->GetName();
      if (itr->first == 0) { // Common or DDU canvases 
	canvas->appendChild(makeElement(doc,"Prefix","DDU_"));
	std::string mask = "EMU/";
	if (val.find(mask) != std::string::npos) {
	  canvas->appendChild(makeElement(doc,"Folder","DDU"));
          val.erase(val.find(mask), mask.length());
        }
	mask = "DDU: ";
        if (val.find(mask) != std::string::npos) {
          val.erase(val.find(mask), mask.length());
        }

      } else { // Chamber canvases
	canvas->appendChild(makeElement(doc,"Prefix","CSC_id_id_"));
        if (val.rfind("/") != std::string::npos) {
          std::string folder = val.substr(0, val.rfind("/"));
	  if (!folder.empty()) canvas->appendChild(makeElement(doc,"Folder",folder));
        }

        std::string mask = Form("%03d_%02d_", (itr->first>>4) &0xFF, itr->first & 0xF);
        if (val.find(mask) != std::string::npos) {
          val = val.erase(0, val.find(mask)+mask.length());
        }

      }
      if (!val.empty()) canvas->appendChild(makeElement(doc,"Name",val));

      // val = cnv->GetTitle();
      if (!val.empty()) canvas->appendChild(makeElement(doc,"Title",val));
      
      val = Form("%d",cnv->GetNumPadsX());
      if (!val.empty()) canvas->appendChild(makeElement(doc,"NumPadsX",val));
      val = Form("%d",cnv->GetNumPadsY());
      if (!val.empty()) canvas->appendChild(makeElement(doc,"NumPadsY",val));

      for (int i = 1; i<= cnv->GetNumPads(); i++) 
      {
	string padID = Form("Pad%d", i);
	for (map<string, TH1*>::iterator h_itr = h_list.begin(); h_itr != h_list.end(); ++h_itr) {	
		TObject* obj = cnv->GetPad(i)->FindObject(h_itr->second);
		if ((obj != NULL) && (obj->InheritsFrom(TH1::Class())) ) {				
			val = ParseHistoName(itr->first, ((TH1*) obj)->GetName());
			
	        	if (!val.empty()) canvas->appendChild(makeElement(doc,padID,val));
		}
	}
      }

      rootElem->appendChild(canvas);
			     
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

