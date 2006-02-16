#include "EmuLocalPlotter.h"
//TF
#include "TrackFinderDataFormat/include/SRSPEventData.h"
void EmuLocalPlotter::fillTF(unsigned char * data, int dataLength) {
	map<string, TH1*> h = histos[0];
	// get Track Finder event
	SRSPEventData::setCDF(true);  // for using with XDAQ ONLY
	//SRSPEventData::setCDF(false); // for all other cases
	SRSPEventData* TFevent = new SRSPEventData(reinterpret_cast<unsigned short*>(data));

	//if(debug)cout<<TFevent->spVMEHeader()<<endl;
	// split it into up to 7 BXs
	vector<SRBxData> SRdata = TFevent->srBxData();
	for(int bx=0; bx<SRdata.size(); bx++){
		// each of 5 Frond FPGAs may present in this BX
		vector<SREventData>	srFPGAs = SRdata[bx].srData();
		vector<SREventData>::const_iterator sr = srFPGAs.begin();
		while( sr != srFPGAs.end() ){
			// each Front FPGA has 3 MPC input links ( 3 LCT on each BX )
			vector<L1MuCSCCorrelatedLCT> LCTs = sr->corrLCTData();
			vector<L1MuCSCCorrelatedLCT>::const_iterator lct = LCTs.begin();
			while( lct != LCTs.end() ){
				switch( lct->getCSCID() ){
					case 4 : h["hTF_ch4BX"]->Fill(bx); break;
					case 5 : h["hTF_ch5BX"]->Fill(bx); break;
					case 8 : h["hTF_ch8BX"]->Fill(bx); break;
					case 9 : h["hTF_ch9BX"]->Fill(bx); break;
					default : break;
				}
				if( lct->getQuality()==4 || lct->getQuality()==5 ) {
					h["hTF_LCTs" ]->Fill(lct->getCSCID());
					h["hTF_CLCTs"]->Fill(lct->getCSCID());
				} else
				if( lct->getQuality()==1 || lct->getQuality()==3 ) {
					h["hTF_ALCTs"]->Fill(lct->getCSCID());
				} else h["hTF_CORLCTs"]->Fill(2);
				lct++;
			}
			sr++;
		}
	}
}
//TF end

