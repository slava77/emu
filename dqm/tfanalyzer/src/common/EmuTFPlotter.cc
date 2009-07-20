#include "emu/dqm/tfanalyzer/EmuTFPlotter.h"
#include <TFile.h>
#include <ctype.h>

void EmuTFPlotter::reset()
{
  if ( filler ) delete filler;
  filler = new EmuTFfiller(histList,canvasList);
}

void EmuTFPlotter::setXMLHistosBookingCfgFile(std::string filename)
{
  histList = EmuTFxmlParsing::parseHistXML(filename.c_str());
  if ( filler ) delete filler;
  filler = new EmuTFfiller(histList,canvasList);
}

void EmuTFPlotter::setXMLCanvasesCfgFile(std::string filename)
{
  canvasList = EmuTFxmlParsing::parseCanvasXML(filename.c_str());
  if ( filler ) delete filler;
  filler = new EmuTFfiller(histList,canvasList);
}

void EmuTFPlotter::saveToROOTFile(std::string outputRootFile)
{
  std::set<std::string> canvases = filler->bookkeeper().getListOfAvailableCanvases();
  std::set<std::string>::const_iterator cnv = canvases.begin();
  TFile file(outputRootFile.c_str(),"RECREATE");
  while( cnv != canvases.end() )
    {
      TCanvas *canvas = filler->bookkeeper().getCanvas(*cnv);
      canvas->Write();
      cnv++;
    }
  file.Write();
  file.Close();
}

TCanvas* EmuTFPlotter::getCanvas(std::string name)
{
   TCanvas * cnv = NULL;
   if ( filler ) 
     {
       cnv = filler->bookkeeper().getCanvas(name);
     }
   return cnv;
}

void EmuTFPlotter::updateLists(void)
{
  if ( filler )
    {
      std::set<std::string> canvases = filler->bookkeeper().getListOfAvailableCanvases();
      for ( std::set<std::string>::iterator iter=canvases.begin(); iter!=canvases.end(); iter++)
        {
          // Take care of canvases first
          // obscure, but robust way to parse canvas name:
          size_t break_point = std::string::npos;
          for (size_t pos1=iter->find("TF_")+3, pos2=iter->find('_',pos1), id=0;
               (break_point=pos1) && id<3 && isdigit((*iter)[pos1]) && pos1!=std::string::npos && pos2!=std::string::npos;
               pos1=pos2+1, pos2=iter->find('_',pos1), id++);
          std::string prefix = iter->substr(0,break_point);
          // std::string folder = iter->first.substr(folder_start,name_start);
          // create new EmuMonitoringCanvas only for new canvases
          if ( MECanvases[prefix].find(*iter) == MECanvases[prefix].end() )
            {
              TCanvas *cnv = filler->bookkeeper().getCanvas(*iter);
              if ( cnv ) 
                {
                  EmuMonitoringCanvas *obj = new EmuMonitoringCanvas(); // from TCanvas = iter->second
                  obj->setPrefix(prefix);
                  obj->setName  (*iter);
                  obj->setTitle (cnv->GetTitle());
                  MECanvases[prefix][*iter] = obj;
                
                  // Go into each canvas and see what histograms (pads) it has
                  TList *hisoPads = cnv->GetListOfPrimitives();
                  TIterator *iter = hisoPads->MakeIterator();
                  TPad *pad = 0;
                  while ( (pad=(TPad*)iter->Next()) )
                    {
                      TH1 *hist = (TH1*)pad->GetListOfPrimitives()->At(0);
                      if ( !hist ) continue;
                      std::string name = hist->GetName();
                      // parse histogram name:
                      // size_t folder_start = name.find("TF_");
                      for (size_t pos1=name.find("TF_")+3, pos2=name.find('_',pos1), id=0;
                           (break_point=pos1) && id<3 && isdigit(name[pos1]) && pos1!=std::string::npos && pos2!=std::string::npos;
                           pos1=pos2+1, pos2=name.find('_',pos1), id++);
                      std::string prefix = name.substr(0,break_point);
                      // std::string folder = name.substr(folder_start,name_start);
                      if ( MEs[prefix].find(name) == MEs[prefix].end() )
                        {
                          EmuMonitoringObject *obj = new EmuMonitoringObject(); // from TH1 hist
                          obj->setPrefix(prefix);
                          obj->setName  (name);
                          obj->setTitle (hist->GetTitle());
                          MEs[prefix][name] = obj;
                       }
                    }
                }
            }
        }
    }
}

void EmuTFPlotter::processEvent(const char *data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) /*throw (std::exception)*/
{
  if ( filler )
    {
      /*		// Prepare printouts first:
      		class shut_up : public std::ostream {
      			class dummy_buffer : public std::streambuf{} db;
      			public: shut_up():std::ostream(db){}
      		} null;
      		filler->setOutputStream(&null);
      		ostringstream cerr;
      		filler->setErrorStream (&cerr);
      		// run TF DQM
      */
      // filler->fill((const unsigned short*)data,dataSize/sizeof(const unsigned short*),errorStat);
      filler->fill((const unsigned short*)data,dataSize,errorStat);
      // anything to say?
//		std::cerr<<cerr.str()<<std::endl;
      // If any new histograms were booked while processing event, regenerate MEs and MECanvases lists:
    }
  else
    {
      // throw std::exception();
    }
  nevents++;
}
