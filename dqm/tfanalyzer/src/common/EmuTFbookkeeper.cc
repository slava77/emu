#include "emu/dqm/tfanalyzer/EmuTFbookkeeper.h"
#include "emu/dqm/tfanalyzer/RegEx.h"

#include <TList.h>
#include <sstream>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

using namespace EmuTFxmlParsing;

// Default constructor
EmuTFbookkeeper::EmuTFbookkeeper(const std::map<std::string,HistAttributes>& histList, const std::map<std::string,CanvasAttributes>& canvList){
  generatingHistograms = histList;
  generatingCanvases   = canvList;
  printout   = 0;
  canvasList.clear();
  createDictionary();
}

void EmuTFbookkeeper::createDictionary(void){
  // Generate an association histogram->canvas
  quickCanvasSearchMap.clear();
  // Loop over cavases first
  std::map<std::string,CanvasAttributes>::const_iterator canvas_type = generatingCanvases.begin();
  while( canvas_type != generatingCanvases.end() )
    {
      // Iterate over items (histograms/pads) to be placed on this canvas
      std::map< std::string, std::vector<std::string> >::const_iterator item = canvas_type->second.items.begin();
      while ( item != canvas_type->second.items.end() )
        {
          // Truncate all the geometry information, following the name of histogram to be drawn
          size_t pos = item->first.find("(");
          if ( pos == std::string::npos ) pos = item->first.length();
          while ( isspace(item->first[pos-1]) ) pos--; 
          std::string histogram = item->first.substr(0,pos);
          // Look if we have this type of histogram
          std::map<std::string,HistAttributes>::const_iterator hist_type = generatingHistograms.find(histogram);
          if ( hist_type != generatingHistograms.end() )
            {
              // If we do, generate all possible instances of this type 
               if ( hist_type->second.prefix == "TF_" )
                {
                  std::ostringstream identifier; identifier<<"TF_";
                  // Assign full name to the instance of the histogram and to the canvas, the histogram goes on
                  std::string hFullName = hist_type  ->second.type + identifier.str() + hist_type  ->second.name;
                  std::string cFullName = canvas_type->second.type + identifier.str() + canvas_type->second.name;
                  quickCanvasSearchMap[hFullName] = cFullName;
                }
              if ( hist_type->second.prefix == "TF_id_" )
                {
                  for (int sp=1; sp<=12; sp++)
                    {
                      std::ostringstream identifier; identifier<<"TF_"<<sp<<"_";
                      // Assign full name to the instance of the histogram and to the canvas, the histogram goes on
                      std::string hFullName = hist_type  ->second.type + identifier.str() + hist_type  ->second.name;
                      std::string cFullName = canvas_type->second.type + identifier.str() + canvas_type->second.name;
                      quickCanvasSearchMap[hFullName] = cFullName;
                    }
                }
              if ( hist_type->second.prefix == "TF_id_id_" )
                {
                  for (int sp=1; sp<=12; sp++)
                    {
                      for(int mpc=1; mpc<=60; mpc++)
                        {
                          std::ostringstream identifier; identifier<<"TF_"<<sp<<"_"<<mpc<<"_";
                          // Assign full name to the instance of the histogram and to the canvas, it goes on
                          std::string hFullName = hist_type  ->second.type + identifier.str() + hist_type  ->second.name;
                          std::string cFullName = canvas_type->second.type + identifier.str() + canvas_type->second.name;
                          quickCanvasSearchMap[hFullName] = cFullName;
                        }
                    }
                }
              if ( hist_type->second.prefix == "TF_id_id_id_" )
                {
                  for (int sp=1; sp<=12; sp++)
                    {
                      for(int mpc=1; mpc<=60; mpc++)
                        {
                          for(int csc=1; csc<=9; csc++)
                            {
                              std::ostringstream identifier; identifier<<"TF_"<<sp<<"_"<<mpc<<"_"<<csc<<"_";
                              // Assign full name to the instance of the histogram and to the canvas, it goes on
                              std::string hFullName = hist_type  ->second.type + identifier.str() + hist_type  ->second.name;
                              std::string cFullName = canvas_type->second.type + identifier.str() + canvas_type->second.name;
                              quickCanvasSearchMap[hFullName] = cFullName;
                            }
                        }
                    }
                }
            }
          item++;
        }
      canvas_type++;
    }

}

bool EmuTFbookkeeper::book(unsigned short sp, unsigned short mpc, unsigned short csc)
{
  // Handle printout stream
  std::ostream &cerr = ( printout ? *printout : std::cerr );

  if ( sp>12 || mpc>60 || csc>9 ) return false;
  // Iterate over all histogram types
  for (std::map<std::string,HistAttributes>::const_iterator iter=generatingHistograms.begin(); iter!=generatingHistograms.end(); iter++)
    {
      // Unique identifier
      std::ostringstream identifier;
      // Select type of histograms to be created: sp level, pc level, or csc level
      if ( sp && mpc && csc ) //csc level
        {
          if ( iter->second.prefix == "TF_"       ) continue;
          if ( iter->second.prefix == "TF_id_"    ) continue;
          if ( iter->second.prefix == "TF_id_id_" ) continue;
          identifier<<"TF_"<<sp<<"_"<<mpc<<"_"<<csc<<"_";
        }
      else if ( sp && mpc && !csc )  //pc level
        {
          if ( iter->second.prefix == "TF_"          ) continue;
          if ( iter->second.prefix == "TF_id_"       ) continue;
          if ( iter->second.prefix == "TF_id_id_id_" ) continue;
          identifier<<"TF_"<<sp<<"_"<<mpc<<"_";
        }
      else if ( sp && !mpc && !csc )  //sp level
        {
          if ( iter->second.prefix == "TF_"          ) continue;
          if ( iter->second.prefix == "TF_id_id_"    ) continue;
          if ( iter->second.prefix == "TF_id_id_id_" ) continue;
          identifier<<"TF_"<<sp<<"_";
        }
      else if ( !sp && !mpc && !csc )  //run level
        {
          if ( iter->second.prefix == "TF_id_"       ) continue;
          if ( iter->second.prefix == "TF_id_id_"    ) continue;
          if ( iter->second.prefix == "TF_id_id_id_" ) continue;
          identifier<<"TF_";
        }
      else return false;
      std::map<std::string,TH1*> &hists = ( sp && mpc && csc ? cscHists : ( sp && mpc && !csc ? pcHists : ( sp && !mpc && !csc ? spHists : tfHists ) ) );

      // Assign full name to the histogram
      std::string hFullName = iter->second.type + identifier.str() + iter->second.name;
      // Create the histogram if it doesn't yet exist
      if ( hists.find(hFullName) == hists.end() )
        {
          TH1 *hist = 0;
          if ( iter->second.type == "h1_" )
            hist = new TH1F(hFullName.c_str(), iter->second.title.c_str(), iter->second.xNbins, iter->second.xMin, iter->second.xMax);
          if ( iter->second.type == "h2_" )
            hist = new TH2F(hFullName.c_str(), iter->second.title.c_str(), iter->second.xNbins, iter->second.xMin, iter->second.xMax, iter->second.yNbins, iter->second.yMin, iter->second.yMax);
          if ( hist )
            {
              hists[hFullName] = hist;
              isModified[hFullName] = true;
              std::map<std::string, std::string>::const_iterator cFullName = quickCanvasSearchMap.find(hFullName);
              if( cFullName != quickCanvasSearchMap.end() )
                 isModified[cFullName->second] = true;
            }
          else
            return false;
          hist->GetXaxis()->SetLabelSize(0.05);
          hist->GetYaxis()->SetLabelSize(0.05);
          hist->GetYaxis()->SetTitleOffset(1.25);

          if ( iter->second.xLabel.length() )
            hist->GetXaxis()->SetTitle(iter->second.xLabel.c_str());
          if ( iter->second.yLabel.length() )
            hist->GetYaxis()->SetTitle(iter->second.yLabel.c_str());

          for (unsigned int index=0; index<iter->second.xBinLabels.size(); index++)
            hist->GetXaxis()->SetBinLabel(index+1,iter->second.xBinLabels[index].c_str());
          for (unsigned int index=0; index<iter->second.yBinLabels.size(); index++)
            hist->GetYaxis()->SetBinLabel(index+1,iter->second.yBinLabels[index].c_str());

          if ( iter->second.yBinLabels.size() )
            hist->GetYaxis()->SetLabelSize(0.05);

          for (std::vector<std::string>::const_iterator property=iter->second.options.begin(); property!=iter->second.options.end(); property++)
            {
              size_t pos=std::string::npos;
              if ( (pos=property->find("LineColor")) != std::string::npos ) hist->SetLineColor(atoi(property->substr(pos+10).c_str()));
              if ( (pos=property->find("FillColor")) != std::string::npos ) hist->SetFillColor(atoi(property->substr(pos+10).c_str()));
              if ( (pos=property->find("bar1"     )) != std::string::npos ) hist->SetOption("bar1");
              if ( (pos=property->find("textcolz" )) != std::string::npos ) hist->SetOption("textcolz");
              if ( (pos=property->find("XnDivizions"))!= std::string::npos) hist->SetNdivisions(atoi(property->substr(pos+12).c_str()),"X");
              if ( (pos=property->find("YnDivizions"))!= std::string::npos) hist->SetNdivisions(atoi(property->substr(pos+12).c_str()),"Y");
              if ( (pos=property->find("XTitleOffset"))!= std::string::npos) hist->GetXaxis()->SetTitleOffset( atof(property->substr(pos+13).c_str()) );
              if ( (pos=property->find("YTitleOffset"))!= std::string::npos) hist->GetYaxis()->SetTitleOffset( atof(property->substr(pos+13).c_str()) );
            }

          //Aliases
          if ( sp>0&&sp<=12 && mpc>0&&mpc<=60 && csc>0&&csc<=9 )
            cscAlias[sp-1][mpc-1][csc-1][iter->second.name] = hist;
          if ( sp>0&&sp<=12 && mpc>0&&mpc<=60 && csc==0 )
            pcAlias[sp-1][mpc-1][iter->second.name] = hist;
          if ( sp>0&&sp<=12 && mpc==0 && csc==0 )
            spAlias[sp-1][iter->second.name] = hist;
          if ( sp==0 ) tfAlias[iter->second.name] = hist;
          if ( sp>12 || mpc>60 || csc>9 ) cerr<<"IDs are out or range!"<<std::endl;

//std::cout<<"Booked: "<<hist->GetName()<<std::endl;
        }
      else return false;
    }
  return true;
}

TCanvas* EmuTFbookkeeper::createCanvasTemplate(const std::string &name, const CanvasAttributes &canvas_attributes) const 
{
  // Handle printout streams
  std::ostream &cerr = ( printout ? *printout : std::cerr );
  // A template histogram (will be used as a factory to clone specific sp/pc/csc histograms later)
  TCanvas *canvas_template = new TCanvas("template",canvas_attributes.title.c_str());
  // Set pads for the template histogram
  std::map< std::string, std::vector<std::string> >::const_iterator item = canvas_attributes.items.begin();
  while ( item != canvas_attributes.items.end() )
    {
      // Parse item to be drawn and split it to histogram name and (possibly) its geometry/place
      std::map< std::string, std::vector<std::string> > constructor = RegExSearch("(\\w+)\\s*\\(?(.+)?\\)?",item->first.c_str());
      if ( constructor.size()!=1 ) // every item corresponds to one histogram only
          cerr<<"Cannot parse "<<item->first<<std::endl;
      else
        {
          // Create pad for each histogram dynamicaly (pad's name should match histogram name)
          // Once pad is drawn on a canvas, the canvas is resposible for copying/deleting it when needed
          TPad *pad=0;
          std::string padName  = constructor.begin()->second[0];
          std::string padTitle = constructor.begin()->second[0];
          std::string geometry =(constructor.begin()->second.size()==1?"Center":constructor.begin()->second[1]);
          if ( geometry=="Center"  ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.99, 0.99);
          if ( geometry=="Top)"    ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.505,0.99, 0.99);
          if ( geometry=="Bottom)" ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.99, 0.495);
          if ( geometry=="Left)"   ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.495,0.90);
          if ( geometry=="Right)"  ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.495,0.01, 0.99, 0.99);
          // Geometry doesn't match predefined patterns?
          if ( pad==0 )
            {
              // Look for explicit coordinates
              std::map< std::string, std::vector<std::string> > coordinates = RegExSearch("(\\w*.\\w*)\\s*x\\s*(\\w*.\\w*)\\s*\\+\\s*(\\w*.\\w*)\\s*\\+\\s*(\\w*.\\w*)",geometry.c_str());
              if ( coordinates.size()==1 ) // found a match
                {
                  double width  = atof(coordinates.begin()->second[0].c_str());
                  double height = atof(coordinates.begin()->second[1].c_str());
                  double x      = atof(coordinates.begin()->second[2].c_str());
                  double y      = atof(coordinates.begin()->second[3].c_str());
                  pad = new TPad(padName.c_str(),padTitle.c_str(),x,y,x+width,y+height);
                }
              else cerr<<"Wrong geometry: "<<geometry<<std::endl;
            }
          // Finally, pad was created?
          if ( pad!=0 )
            {
              // Put it on the canvas
              canvas_template->cd();
              pad->Draw();
              // Pads properties
              for (std::vector<std::string>::const_iterator property=item->second.begin(); property!=item->second.end(); property++)
                {
                  size_t pos=std::string::npos;
                  if ( (pos=property->find("LeftMargin")) != std::string::npos ) pad->SetLeftMargin(atof(property->substr(pos+12).c_str()));
                  if ( (pos=property->find("GridX"))      != std::string::npos ) pad->SetGridx(1);
                  if ( (pos=property->find("GridY"))      != std::string::npos ) pad->SetGridy(1);
                  if ( (pos=property->find("LogX"))       != std::string::npos ) pad->SetLogx(1);
                  if ( (pos=property->find("LogY"))       != std::string::npos ) pad->SetLogy(1);
                  if ( (pos=property->find("LogZ"))       != std::string::npos ) pad->SetLogz(1);
                  // Use pad's title since it desn't hold meaningful info anyway
                  if ( (pos=property->find("Normalize"))  != std::string::npos ) pad->SetTitle("norm");
                }
            }
        }
        item++;
    }
    return canvas_template;
}

std::set<std::string> EmuTFbookkeeper::getListOfAvailableCanvases(void) const
{
    std::set<std::string> result; 
    // Loop over all types of canvases and check if related histograms has ever been modified (means created)
    std::map<std::string, std::string>::const_iterator hist = quickCanvasSearchMap.begin();
    while ( hist != quickCanvasSearchMap.end() )
      {
        if( isModified.find(hist->first) != isModified.end() ) result.insert(hist->second);
        hist++;
      }
    return result;
}

TCanvas* EmuTFbookkeeper::getCanvas(std::string cName){
    TCanvas *canvas = 0;
    // Had this canvas already been created?
    std::map<std::string,TCanvas*>::const_iterator created_canvas = canvasList.find(cName);
    if ( created_canvas != canvasList.end() ) canvas = created_canvas->second;
    // If there are no new histograms for the canvas (it is not modified), return it right away
    if( !isModified[cName] ) return canvas;

    // If we get so far, we either need to create a new canvas or draw new histograms on it
    // First, parse the cName and split it to id's and canvas type
    size_t break_pos = std::string::npos;
    unsigned int ids[3] = {0,0,0}, type=0;
    for ( size_t pos1 = cName.find("TF_")+3, pos2 = cName.find('_',pos1);
         (break_pos=pos1) && type < 3 && isdigit(cName[pos1]) && pos1 != std::string::npos && pos2 != std::string::npos;
         pos1 = pos2+1, pos2 = cName.find('_',pos1), type++ ) ids[type] = strtol( cName.substr(pos1,pos2-pos1).c_str(), 0, 10 );

    // Sanity check
    switch ( type )
      {
        case 3: if ( ids[0]<=0 || ids[0]>12 || ids[1]<=0 || ids[1]>60 || ids[2]<=0 || ids[2]>9 ) return 0; break;
        case 2: if ( ids[0]<=0 || ids[0]>12 || ids[1]<=0 || ids[1]>60 ) return 0; break;
        case 1: if ( ids[0]<=0 || ids[0]>12 ) return 0; break;
        case 0: break;
        default: return 0; break;
      }

    // Look for the attributes of the canvas of this type
    std::string canvas_type = cName.substr(break_pos);
    std::map<std::string, EmuTFxmlParsing::CanvasAttributes>::const_iterator match = generatingCanvases.find(canvas_type);
    if ( match == generatingCanvases.end() ) return canvas;

    // If the canvas was never created bofore, create it from a canvas template with all the histogram pads in it
    if( !canvas ){
       const CanvasAttributes &canvas_attributes = match->second;
       TCanvas *canvas_template = createCanvasTemplate(cName, canvas_attributes);
       canvas = (TCanvas*)canvas_template->Clone(cName.c_str());
       canvas->SetName(cName.c_str());
       canvasList[cName] = canvas;
       delete canvas_template;
    }

    // Based on canvas id's select set of histograms with the same id's
    const std::map<std::string,TH1*> &hists = 
      ( type == 3 ?      cscAlias[ ids[0]-1 ][ ids[1]-1 ][ ids[2]-1 ] : 
         ( type == 2 ?    pcAlias[ ids[0]-1 ][ ids[1]-1 ] : 
            ( type == 1 ? spAlias[ ids[0]-1 ] : tfAlias ) ) );

    // Loop over all pads on the canvas and find corresponding histograms 
    TPad *pad = 0;
    TIterator *iter = canvas->GetListOfPrimitives()->MakeIterator();
    while ( (pad = (TPad*)iter->Next()) )
      { 
        std::map<std::string,TH1*>::const_iterator hist = hists.find( pad->GetName() );
        if ( hist != hists.end() )
          {
            // Nice match, draw it
            pad->cd();
            if ( !strncmp(pad->GetTitle(),"norm",4) )
              {
                for (int bin=1; bin<=hist->second->GetNbinsX(); bin++)
                  hist->second->SetBinContent(bin,hist->second->GetBinContent(bin)/hist->second->GetEntries());
              }
            hist->second->Draw();
          }
      }
    delete iter;

    isModified[cName] = false;
    return canvas;
}

