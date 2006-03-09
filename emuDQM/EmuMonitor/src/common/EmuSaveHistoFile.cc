#include "EmuLocalPlotter.h"

void EmuLocalPlotter::save(const char *filename) {
	int i, j;
	stringstream stdir;
	string dir;
	string hname;
//	consinfo->clear();
	LOG4CPLUS_INFO(logger_, "Saving Histograms to " << filename);
	TFile f(filename, "recreate");

	f.cd();

	for (map<int, map<string, TH1*> >::iterator itr = histos.begin(); itr != histos.end(); ++itr) {
/*
	if (itr->first != 0) {
		stdir.clear(); stdir<< "CSC_" << ((itr->first>>4) & 0xFF) << "_" << (itr->first & 0xF);
		stdir >> dir;
		TDirectory * rdir = f.mkdir(dir.c_str());
		rdir->cd();
	}
*/
		if (itr->first != SLIDES_ID) {
			for (map<string, TH1*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
//				hname = "DQM/" + dir;
//				cout << h_itr->first << endl;
//				consinfo->addObject(TString(h_itr->first.c_str()), TString(dir.c_str()), 0, h_itr->second);
				h_itr->second->Write();
			}
		}
//		f.cd();
	}

	for (map<int, map<string, ConsumerCanvas*> >::iterator itr = canvases.begin(); itr != canvases.end(); ++itr) {
/*
	if (itr->first != 0) {
	stdir.clear(); stdir<< "CSC_" << ((itr->first>>4) & 0xFF) <<
	"_" << (itr->first & 0xF);
	stdir >> dir;
	TDirectory * rdir = f.mkdir(dir.c_str());
	rdir->cd();
	}
*/
		if (itr->first != SLIDES_ID) {
			for (map<string, ConsumerCanvas*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
				h_itr->second->Write();
			}
		}
	}
//	consTask->send(consinfo);
//	cout << "Saving Histograms to "<< filename << endl;
	fListModified = true;
	fSaveHistos = false;
	f.Close();
}

