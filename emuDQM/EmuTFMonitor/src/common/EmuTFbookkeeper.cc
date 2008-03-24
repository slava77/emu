#include "EmuTFbookkeeper.h"

#include <TList.h>
#include "RegEx.h"
#include <sstream>
#include <math.h>

using namespace EmuTFxmlParsing;

bool EmuTFbookkeeper::book(unsigned short sp, unsigned short mpc, unsigned short csc){
	// Handle printout stream
	std::ostream &cerr = ( printout ? *printout : std::cerr );

	if( sp>12 || mpc>60 || csc>9 ) return false;
	// Iterate over all histogram types
	for(std::map<std::string,HistAttributes>::const_iterator iter=attributes.begin(); iter!=attributes.end(); iter++){
		// Unique identifier
		std::ostringstream identifier;
		// Select type of histograms to be created: sp level, pc level, or csc level
		if( sp && mpc && csc ){ //csc level
			if( iter->second.prefix == "TF_id_"    ) continue;
			if( iter->second.prefix == "TF_id_id_" ) continue;
			identifier<<"TF_"<<sp<<"_"<<mpc<<"_"<<csc<<"_";
		} else
		if( sp && mpc && !csc ){ //pc level
			if( iter->second.prefix == "TF_id_"       ) continue;
			if( iter->second.prefix == "TF_id_id_id_" ) continue;
			identifier<<"TF_"<<sp<<"_"<<mpc<<"_";
		} else
		if( sp && !mpc && !csc ){ //sp level
			if( iter->second.prefix == "TF_id_id_"    ) continue;
			if( iter->second.prefix == "TF_id_id_id_" ) continue;
			identifier<<"TF_"<<sp<<"_";
		} else
		if( !sp && !mpc && !csc ){ //run level
			if( iter->second.prefix == "TF_id_"       ) continue;
			if( iter->second.prefix == "TF_id_id_"    ) continue;
			if( iter->second.prefix == "TF_id_id_id_" ) continue;
			identifier<<"TF_";
		} else return false;
		std::map<std::string,TH1*> &hists = ( sp && mpc && csc ? cscHists : ( sp && mpc && !csc ? pcHists : ( sp && !mpc && !csc ? spHists : tfHists ) ) );

		// Assign full name to the histogram
		std::string hFullName = iter->second.type + identifier.str() + iter->second.name;
		// Create the histogram if it doesn't yet exist
		if( hists.find(hFullName) == hists.end() ){
			isModified = true;
			TH1 *hist = 0;
			if( iter->second.type == "h1_" )
				hist = new TH1F(hFullName.c_str(), iter->second.title.c_str(), iter->second.xNbins, iter->second.xMin, iter->second.xMax);
			if( iter->second.type == "h2_" )
				hist = new TH2F(hFullName.c_str(), iter->second.title.c_str(), iter->second.xNbins, iter->second.xMin, iter->second.xMax, iter->second.yNbins, iter->second.yMin, iter->second.yMax);
			if( hist )
				hists[hFullName] = hist;
			else
				return false;
			hist->GetXaxis()->SetLabelSize(0.05);
			hist->GetYaxis()->SetLabelSize(0.05);
			hist->GetYaxis()->SetTitleOffset(1.25);

			if( iter->second.xLabel.length() )
				hist->GetXaxis()->SetTitle(iter->second.xLabel.c_str());
			if( iter->second.yLabel.length() )
				hist->GetYaxis()->SetTitle(iter->second.yLabel.c_str());

			for(unsigned int index=0; index<iter->second.xBinLabels.size(); index++)
				hist->GetXaxis()->SetBinLabel(index+1,iter->second.xBinLabels[index].c_str());
			for(unsigned int index=0; index<iter->second.yBinLabels.size(); index++)
				hist->GetYaxis()->SetBinLabel(index+1,iter->second.yBinLabels[index].c_str());

			if( iter->second.yBinLabels.size() )
				hist->GetYaxis()->SetLabelSize(0.05);

			for(std::vector<std::string>::const_iterator property=iter->second.options.begin(); property!=iter->second.options.end(); property++){
				size_t pos=std::string::npos;
				if( (pos=property->find("LineColor")) != std::string::npos ) hist->SetLineColor(atoi(property->substr(pos+10).c_str()));
				if( (pos=property->find("FillColor")) != std::string::npos ) hist->SetFillColor(atoi(property->substr(pos+10).c_str()));
				if( (pos=property->find("bar1"     )) != std::string::npos ) hist->SetOption("bar1");
				if( (pos=property->find("textcolz" )) != std::string::npos ) hist->SetOption("textcolz");
				if( (pos=property->find("XnDivizions"))!= std::string::npos) hist->SetNdivisions(atoi(property->substr(pos+12).c_str()),"X");
				if( (pos=property->find("YnDivizions"))!= std::string::npos) hist->SetNdivisions(atoi(property->substr(pos+12).c_str()),"Y");
				if( (pos=property->find("XTitleOffset"))!= std::string::npos) hist->GetXaxis()->SetTitleOffset( atof(property->substr(pos+13).c_str()) );
				if( (pos=property->find("YTitleOffset"))!= std::string::npos) hist->GetYaxis()->SetTitleOffset( atof(property->substr(pos+13).c_str()) );
			}

			//Aliases
			if( sp>0&&sp<=12 && mpc>0&&mpc<=60 && csc>0&&csc<=9 )
				cscAlias[sp-1][mpc-1][csc-1][iter->second.name] = hist;
			if( sp>0&&sp<=12 && mpc>0&&mpc<=60 && csc==0 )
				pcAlias[sp-1][mpc-1][iter->second.name] = hist;
			if( sp>0&&sp<=12 && mpc==0 && csc==0 )
				spAlias[sp-1][iter->second.name] = hist;
			if( sp==0 ) tfAlias[iter->second.name] = hist;
			if( sp>12 || mpc>60 || csc>9 ) cerr<<"IDs are out or range!"<<std::endl;
//std::cout<<"Booked: "<<hist->GetName()<<std::endl;
		} else return false;
	}
	return true;
}

std::map<std::string,TCanvas*> EmuTFbookkeeper::wrapToCanvases(const std::map<std::string,CanvasAttributes>& canvasList){
	// Handle printout streams
	std::ostream &cerr = ( printout ? *printout : std::cerr );
	// Canvases; do not delete anything from this list other than calling cleenupCanvases():
	std::map<std::string,TCanvas*> result;
	// Loop over provided canvases
	std::map<std::string,CanvasAttributes>::const_iterator iter = canvasList.begin();
	while( iter != canvasList.end() ){
		// Create an alias to attributes
		const CanvasAttributes &canvas_attributes = iter->second;
		// A template histogram (will be used as a factory to clone specific sp/pc/csc histograms)
		TCanvas canvas_template("template",canvas_attributes.title.c_str());
		// Set pads for the template histogram
		std::map< std::string, std::vector<std::string> >::const_iterator item = canvas_attributes.items.begin();
		while( item != canvas_attributes.items.end() ){
			// Parse item to be draw and split it to histogram name and (possibly) its location
			std::map< std::string, std::vector<std::string> > constructor = RegExSearch("(\\w+)\\s*\\(?(.+)?\\)?",item->first.c_str());
			if( constructor.size()==1 ){ // every item corresponds to one histogram only
				// Create pad for each histogram dynamicaly (pad's name should match histogram name)
				// Draw it on a static canvas and it gets automatically deleted on leaving of the scope
				TPad *pad=0;
				//std::string padName  = "p_"+canvas_attributes.prefix+constructor.begin()->second[0];
				std::string padName  = constructor.begin()->second[0];
				std::string padTitle = constructor.begin()->second[0];
				std::string geometry = (constructor.begin()->second.size()==1?"Center":constructor.begin()->second[1]);
				if( geometry=="Center"  ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.99, 0.99);
				if( geometry=="Top)"    ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.505,0.99, 0.99);
				if( geometry=="Bottom)" ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.99, 0.495);
				if( geometry=="Left)"   ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.01, 0.01, 0.495,0.90);
				if( geometry=="Right)"  ) pad = new TPad(padName.c_str(),padTitle.c_str(),0.495,0.01, 0.99, 0.99);
				// Non of code words match geometry?
				if( pad==0 ){
					// Look for explicit coordinates
					std::map< std::string, std::vector<std::string> > coordinates = RegExSearch("(\\w*.\\w*)\\s*x\\s*(\\w*.\\w*)\\s*\\+\\s*(\\w*.\\w*)\\s*\\+\\s*(\\w*.\\w*)",geometry.c_str());
					if( coordinates.size()==1 ){ // found a match
						double width  = atof(coordinates.begin()->second[0].c_str());
						double height = atof(coordinates.begin()->second[1].c_str());
						double x      = atof(coordinates.begin()->second[2].c_str());
						double y      = atof(coordinates.begin()->second[3].c_str());
//std::cout<<"padName: "<<padName<<" title: "<<padTitle<<" "<<width<<" "<<height<<" "<<x<<" "<<y<<std::endl;
						pad = new TPad(padName.c_str(),padTitle.c_str(),x,y,x+width,y+height);
					} else cerr<<"Wrong geometry: "<<geometry<<std::endl;
				}
				// Finally, pad was created?
				if( pad!=0 ){
					canvas_template.cd();
					pad->Draw();
					// Pads properties
					for(std::vector<std::string>::const_iterator property=item->second.begin(); property!=item->second.end(); property++){
						size_t pos=std::string::npos;
						if( (pos=property->find("LeftMargin")) != std::string::npos ) pad->SetLeftMargin(atof(property->substr(pos+12).c_str()));
						if( (pos=property->find("GridX"))      != std::string::npos ) pad->SetGridx(1);
						if( (pos=property->find("GridY"))      != std::string::npos ) pad->SetGridy(1);
						if( (pos=property->find("LogX"))       != std::string::npos ) pad->SetLogx(1);
						if( (pos=property->find("LogY"))       != std::string::npos ) pad->SetLogy(1);
						if( (pos=property->find("LogZ"))       != std::string::npos ) pad->SetLogz(1);
						// Use pad's title since it desn't hold meaningful info anyway
						if( (pos=property->find("Normalize"))  != std::string::npos ) pad->SetTitle("norm");
					}
				}
			} else cerr<<"Cannot parse "<<item->first<<std::endl;
			item++;
		}

		// Now clone canvas_template for particular sp, pc, and csc ids and draw available histograms on it

		// Define type of canvas first (sp level, pc level, or csc level)
		std::vector<std::string> ids;
		for(size_t pos1=canvas_attributes.prefix.find("TF_")+3,pos2=canvas_attributes.prefix.find('_',pos1); pos1!=std::string::npos&&pos2!=std::string::npos; pos1=pos2+1,pos2=canvas_attributes.prefix.find('_',pos1))
			ids.push_back(canvas_attributes.prefix.substr(pos1,pos2-pos1));
		// Check that type is one of three types (sp/pc/csc)
		if( ids.size()!=0 && ids.size()!=1 && ids.size()!=2 && ids.size()!=3 ){
			cerr<<"Cannot parse "<<canvas_attributes.prefix<<std::endl;
			iter++;
			continue;
		}
		// Based on canvas type, select set of histograms of the same type
		const std::map<std::string,TH1*> &hists = ( ids.size()==3 ? cscHists : ( ids.size()==2 ? pcHists : ( ids.size()==1 ? spHists : tfHists ) ) );
		// From this set choose histograms that go on this particular canvas
		for(std::map<std::string,TH1*>::const_iterator hist=hists.begin(); hist!=hists.end(); hist++){
			// Define histogram ids first
			std::vector<std::string> hist_ids;
			size_t name_start=std::string::npos;
			for(size_t pos1=hist->first.find("TF_")+3,pos2=hist->first.find('_',pos1),id=0; (name_start=pos1)&&id<ids.size()&&pos1!=std::string::npos&&pos2!=std::string::npos; pos1=pos2+1,pos2=hist->first.find('_',pos1),id++)
				hist_ids.push_back(hist->first.substr(pos1,pos2-pos1));
//std::cout<<"pos1: "<<pos1<<" pos2: "<<pos2<<" id: "<<hist->first.substr(pos1,pos2-pos1)<<std::endl;
			// This is what remains:
			std::string hist_name = hist->first.substr(name_start);
			// Check for consistency
			if( hist_ids.size() != ids.size() ){ cerr<<"It cannot happen ever!!!"<<std::endl; continue; }
			// If any of canvas ids were explicitly specified, choose only hists that suffice those ids
			if(	( ids.size()>0 && ids[0]!="id" && ids[0]!=hist_ids[0] ) ||
				( ids.size()>1 && ids[1]!="id" && ids[1]!=hist_ids[1] ) ||
				( ids.size()>2 && ids[2]!="id" && ids[2]!=hist_ids[2] ) ) continue;
			// Form actual canvas with specific ids from the nameless template
			std::string fullCanvasName = canvas_attributes.type+"TF_";
			if( hist_ids.size()>0 ) fullCanvasName.append(hist_ids[0]+"_");
			if( hist_ids.size()>1 ) fullCanvasName.append(hist_ids[1]+"_");
			if( hist_ids.size()>2 ) fullCanvasName.append(hist_ids[2]+"_");
			fullCanvasName.append(canvas_attributes.name);
//std::cout<<"Clonning: "<<fullCanvasName<<std::endl;
			TCanvas *canvas = 0;
			// Had this canvas already been created?
			std::map<std::string,TCanvas*>::const_iterator created_canvas = result.find(fullCanvasName);
			if( created_canvas!=result.end() )
				canvas = created_canvas->second;
			else {
				canvas = (TCanvas*)canvas_template.Clone(fullCanvasName.c_str());
				canvas->SetName(fullCanvasName.c_str());
				cleenupList.push_back(canvas);
				result[fullCanvasName] = canvas;
			}
			// Match histogram name agains all pads of this canvas
			TPad *pad = (TPad*)canvas->GetListOfPrimitives()->FindObject(hist_name.c_str());
			// Current histogram is not for this canvas
			if(pad==0) continue;
			else {
				// Nice match, draw it
				pad->cd();
				if( !strncmp(pad->GetTitle(),"norm",4) ){
					for(int bin=1; bin<=hist->second->GetNbinsX(); bin++)
						hist->second->SetBinContent(bin,hist->second->GetBinContent(bin)/hist->second->GetEntries());
				}
				hist->second->Draw();
			}
		}
		iter++;
	}
	return result;
}
