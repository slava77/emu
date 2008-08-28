#ifndef EmuTFbookkeeper_h
#define EmuTFbookkeeper_h

#include <string>
#include <ostream>
#include <list>
#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "EmuTFxmlParsing.h"
#include <ext/hash_map>

namespace __gnu_cxx {
  template<>
  struct hash<std::string>{
    hash<char*> hasher;
    size_t operator()(const std::string& s) const { return hasher(s.c_str()); }
  };
}

class EmuTFbookkeeper {
private:
	__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >  tfHists, spHists,     pcHists,         cscHists;
	__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >  tfAlias, spAlias[12], pcAlias[12][60], cscAlias[12][60][9];
	std::map<std::string,EmuTFxmlParsing::HistAttributes>    attributes;
	std::list<TObject*>                                      cleenupList;
	bool isModified;
	std::ostream *printout; // utilized for error messages only

public:
	// Create set of histograms for the particular:
	// Sector Processor  (one arg), Peripheral Crate (two args), chamber (three args)
	// return false in case of failure
	bool book(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0);
	// See if histograms for a specific sp/pc/csc were booked
	bool isBooked(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return false;
		__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> > &hists = ( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : ( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : ( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		return hists.size();
	}
	// Check if any new histograms were created since last resetModified() call:
	bool modified(void) const throw() { return  isModified; }
	// Whoever uses this class, synchronized its knowledge about currently existing hists:
	void resetModified(void)  throw() { isModified = false; }

	// Obtain final list of canvases with all histograms (costly, don't abuse this method):
	std::map<std::string,TCanvas*> wrapToCanvases(const std::map<std::string,EmuTFxmlParsing::CanvasAttributes>& canvasList);
	// Following function will delete all histograms (if any), created by previous function:
	void cleenupCanvases(void){
		for(std::list<TObject*>::iterator iter=cleenupList.begin(); iter!=cleenupList.end(); iter++)
			delete *iter;
		cleenupList.resize(0);
	}

	// Get histogram with specific name hname for a specific sp/pc/csc
	TH1* get(std::string hname, unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return 0;
		__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> > &hists = ( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : ( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : ( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::const_iterator entry = hists.find(hname);
		return ( entry==hists.end() ? 0 : entry->second );
	}
	// Get list of histograms with specific name
	std::list<const TH1*> get(std::string hname) const {
		std::list<const TH1*> retval;
		std::map<std::string,EmuTFxmlParsing::HistAttributes>::const_iterator iter = attributes.find(hname);
		if( iter == attributes.end() ) return retval;

		if( iter->second.prefix == "TF_" ){
			__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::const_iterator hist = tfAlias.find(hname);
			if( hist != tfAlias.end() ) retval.push_back( hist->second );
		}
		if( iter->second.prefix == "TF_id_" ){
			for(int sp=0; sp<12; sp++){
				__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::const_iterator hist = spAlias[sp].find(hname);
				if( hist != spAlias[sp].end() ) retval.push_back( hist->second );
			}
		}
		if( iter->second.prefix == "TF_id_id_" ){
			for(int sp=0; sp<12; sp++)
				for(int mpc=0; mpc<60; mpc++){
					__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::const_iterator hist = pcAlias[sp][mpc].find(hname);
					if( hist != pcAlias[sp][mpc].end() ) retval.push_back( hist->second );
				}
		}
		if( iter->second.prefix == "TF_id_id_id_" ){
			for(int sp=0; sp<12; sp++)
				for(int mpc=0; mpc<60; mpc++)
					for(int csc=0; csc<9; csc++){
						__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::const_iterator hist = cscAlias[sp][mpc][csc].find(hname);
						if( hist != cscAlias[sp][mpc][csc].end() ) retval.push_back( hist->second );
					}
		}
		return retval;
	}

	void setPrintout(std::ostream *str){ printout=str; }

	// Default constructor
	EmuTFbookkeeper(const std::map<std::string,EmuTFxmlParsing::HistAttributes>& histList){
		attributes=histList;
		isModified=true;
		printout  = 0;
	}
	// Cleanup
	~EmuTFbookkeeper(void){
		for(__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::iterator iter=tfHists.begin(); iter!=tfHists.end(); iter++)
			delete iter->second;
		for(__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::iterator iter=spHists.begin(); iter!=spHists.end(); iter++)
			delete iter->second;
		for(__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::iterator iter=pcHists.begin(); iter!=pcHists.end(); iter++)
			delete iter->second;
		for(__gnu_cxx::hash_map<std::string,TH1*,__gnu_cxx::hash<std::string> >::iterator iter=cscHists.begin(); iter!=cscHists.end(); iter++)
			delete iter->second;
		for(std::list<TObject*>::iterator iter=cleenupList.begin(); iter!=cleenupList.end(); iter++)
			delete *iter;
	}
};

#endif
