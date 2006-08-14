#include "EmuLocalPlotter.h"

void EmuLocalPlotter::testHistos(const char *filename) {
	int i, j;
	stringstream stdir;
	string dir;
	string hname;
//	consinfo->clear();
	// LOG4CPLUS_INFO(logger_, " Histograms to " << filename);

	for (map<int, map<string, TH1*> >::iterator itr = histos.begin(); itr != histos.end(); ++itr) {
			for (map<string, TH1*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	// Put checks here 
			}
	}

}

