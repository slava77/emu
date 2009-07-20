#ifndef EmuTFbookkeeper_h
#define EmuTFbookkeeper_h

#include <string>
#include <ostream>
#include <list>
#include <map>
#include <set>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "EmuTFxmlParsing.h"
// #include <ext/hash_map>


class EmuTFbookkeeper {
private:
	std::map<std::string,TH1*>  tfHists, spHists,     pcHists,         cscHists;
	std::map<std::string,TH1*>  tfAlias, spAlias[12], pcAlias[12][60], cscAlias[12][60][9];
	std::map<std::string, EmuTFxmlParsing::HistAttributes>   generatingHistograms;
	std::map<std::string, EmuTFxmlParsing::CanvasAttributes> generatingCanvases;

	std::map<std::string, TCanvas*>    canvasList;           // indexed by full canvase name
	std::map<std::string, bool>        isModified;           // tells if canvas or histogram needs to be updated
	std::map<std::string, std::string> quickCanvasSearchMap; // full histogram name is linked to the full canvas name
	std::ostream *printout; // utilized for error messages only

        // Build quickCanvasSearchMap from generatingHistograms and generatingCanvases
	void createDictionary(void);
        // Build a template canvas (factory), holding all the pads/histograms; it will be then instantiated for every specific sp/mpc/csc 
	TCanvas* createCanvasTemplate(const std::string &name, const EmuTFxmlParsing::CanvasAttributes &canvas_attributes) const ;
	// Create new canvas
	TCanvas* bookCanvasForHistogram(std::string hName);

public:
	// Create set of histograms for the particular:
	// Sector Processor  (one arg), Peripheral Crate (two args), chamber (three args)
	// return false in case of failure
	bool book(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0);
	// See if histograms for a specific sp/pc/csc were booked
	bool isBooked(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return false;
		std::map<std::string,TH1*> &hists = 
			( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : 
				( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : 
					( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		return hists.size();
	}
	// Check if any new histograms were created on the canvas since last resetModified() call:
	bool modified(std::string name) const throw() {
		std::map<std::string, bool>::const_iterator obj = isModified.find(name);
		if( obj != isModified.end() ) return obj->second;
		else return false;
	}
	// Whoever uses this class, he/she resets the flag below if the current version of histogram was taken:
	void resetModified(std::string name)  throw() { isModified[name] = false; }

	// Get map of canvases objects
//	std::map<std::string,TCanvas*> getCanvasList() const {return canvasList;}
	TCanvas* getCanvas(std::string cName);

	// Return names of canvases, holding non-empty pads/histograms
	std::set<std::string> getListOfAvailableCanvases(void) const ;

	// Following function will delete all canvases (if any), created by previous function:
	void cleanupCanvases(void){
		for(std::map<std::string, TCanvas*>::iterator iter=canvasList.begin(); iter!=canvasList.end(); iter++)
                        delete iter->second;
                canvasList.clear();
	}

	// Get histogram with specific name hname for a specific sp/pc/csc
	TH1* get(std::string hname, unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return 0;
		std::map<std::string,TH1*> &hists = ( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : ( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : ( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		std::map<std::string,TH1*>::const_iterator entry = hists.find(hname);
		return ( entry==hists.end() ? 0 : entry->second );
	}
	// Get list of histograms with specific name
	std::list<const TH1*> get(std::string hname) const {
		std::list<const TH1*> retval;
		std::map<std::string,EmuTFxmlParsing::HistAttributes>::const_iterator iter = generatingHistograms.find(hname);
		if( iter == generatingHistograms.end() ) return retval;

		if( iter->second.prefix == "TF_" ){
			std::map<std::string,TH1*>::const_iterator hist = tfAlias.find(hname);
			if( hist != tfAlias.end() ) retval.push_back( hist->second );
		}
		if( iter->second.prefix == "TF_id_" ){
			for(int sp=0; sp<12; sp++){
				std::map<std::string,TH1*>::const_iterator hist = spAlias[sp].find(hname);
				if( hist != spAlias[sp].end() ) retval.push_back( hist->second );
			}
		}
		if( iter->second.prefix == "TF_id_id_" ){
			for(int sp=0; sp<12; sp++)
				for(int mpc=0; mpc<60; mpc++){
					std::map<std::string,TH1*>::const_iterator hist = pcAlias[sp][mpc].find(hname);
					if( hist != pcAlias[sp][mpc].end() ) retval.push_back( hist->second );
				}
		}
		if( iter->second.prefix == "TF_id_id_id_" ){
			for(int sp=0; sp<12; sp++)
				for(int mpc=0; mpc<60; mpc++)
					for(int csc=0; csc<9; csc++){
						std::map<std::string,TH1*>::const_iterator hist = cscAlias[sp][mpc][csc].find(hname);
						if( hist != cscAlias[sp][mpc][csc].end() ) retval.push_back( hist->second );
					}
		}
		return retval;
	}

	void setPrintout(std::ostream *str){ printout=str; }

	// Default constructor
	EmuTFbookkeeper(const std::map<std::string,EmuTFxmlParsing::HistAttributes>& histList,const std::map<std::string,EmuTFxmlParsing::CanvasAttributes>& canvList);

	// Cleanup
	~EmuTFbookkeeper(void){
      	        for(std::map<std::string, TCanvas*>::iterator iter=canvasList.begin(); iter!=canvasList.end(); ++iter)
                        if (iter->second != NULL) delete iter->second;
                canvasList.clear();

		for(std::map<std::string,TH1*>::iterator iter=tfHists.begin(); iter!=tfHists.end(); ++iter)
			if (iter->second != NULL) delete iter->second;
		tfHists.clear();

		for(std::map<std::string,TH1*>::iterator iter=spHists.begin(); iter!=spHists.end(); ++iter)
			if (iter->second != NULL) delete iter->second;
		spHists.clear();

		for(std::map<std::string,TH1*>::iterator iter=pcHists.begin(); iter!=pcHists.end(); ++iter)
			if (iter->second != NULL) delete iter->second;
		pcHists.clear();

		for(std::map<std::string,TH1*>::iterator iter=cscHists.begin(); iter!=cscHists.end(); ++iter)			
			if (iter->second != NULL) delete iter->second;
		cscHists.clear();
	}
};

#endif
