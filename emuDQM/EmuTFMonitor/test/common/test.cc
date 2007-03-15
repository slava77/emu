#include <string.h>
#include <iostream>
#include <iomanip>
#include <set>
#include "EventFilter/CSCTFRawToDigi/src/CSCTFEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPRecord.cc"
#include "IORawData/CSCCommissioning/src/FileReaderDDU.cc"
//g++ -o _test test.cc -I/home/slice/kotov/CMSSW_0_8_0_pre2/src`
//#include "TROOT.h"
//#include "TTree.h"
//#include "TFile.h"
//g++ -o _test test.cc -I/home/slice/kotov/CMSSW_0_8_0_pre2/src `root-config --cflags` `root-config --glibs`

int main(int argc, char *argv[]){
	using namespace std;

	pair<int,int> coincidence[6]; // Num. of coincidence and num. of hits in the middle chamber

	// DDU File Reader
	FileReaderDDU reader;
	reader.open(argv[1]);

	// Event buffer
	size_t size, nevents=0;
	const unsigned short *buf=0;

	// Main cycle
	while( (size = reader.read(buf)) /*&& nevents<100*/ ){
		unsigned short event[size];

		// Swep out C-words
		unsigned int index1=12, index2=12;
		memcpy(event,buf,12*sizeof(unsigned short));
		while( index1 < size ){
			if( (buf[index1]&0xF000)!=0xC000 ){
				event[index2] = buf[index1];
				index1++;
				index2++;
			} else {
				index1++;
			}
		}

		// Numeration 1->6 accounts for following cases:
		// 1) F1/CSC7 -> F4/CSC4
		// 2) F1/CSC8 -> F4/CSC5
		// 3) F1/CSC9 -> F4/CSC6
		// 4) F2/CSC7 -> F4/CSC7
		// 5) F2/CSC8 -> F4/CSC8
		// 6) F2/CSC9 -> F4/CSC9

		CSCSP_MEblock middle_csc[6]; // Want not to doublecount ghosts
		pair<CSCSP_MEblock,CSCSP_MEblock> edges[6]; // Fired CSC on both edges for 6 different positions
		bzero(middle_csc,sizeof(middle_csc));
		bzero(edges,sizeof(edges));

		CSCTFEvent tfEvent, qwe;
		cout<<"Event: "<<nevents<<" Unpack: "<<
		tfEvent.unpack(event,index2)
		<<endl;

		int q;
		if( q=qwe.unpack(event,index2) ){
			cout<<"Error: "<<q<<endl;
			for(int i=0; i<index2/4; i++)
				cout<<hex<<setw(8)<<event[i*4]<<" "<<setw(8)<<event[i*4+1]<<" "<<setw(8)<<event[i*4+2]<<" "<<setw(8)<<event[i*4+3]<<dec<<endl;
		}

		vector<CSCSPEvent> SPs = tfEvent.SPs();
		if( SPs.size() ){
			cout<<" L1A="<<SPs[0].header().L1A()<<endl;
			for(unsigned int tbin=0; tbin<SPs[0].header().nTBINs(); tbin++){
				cout<<"  tbin: "<<tbin<<"  nLCTs: "<<SPs[0].record(tbin).LCTs().size()<<" (";//<<endl;
				vector<CSCSP_MEblock> lct = SPs[0].record(tbin).LCTs();
				for(std::vector<CSCSP_MEblock>::const_iterator i=lct.begin(); i!=lct.end(); i++){
					cout<<" F"<<((i->spInput()-1)/3+1)<<"/CSC"<<i->csc();
					switch( i->spInput() ){
						case 1 : if( i->csc()>6 && i->csc()<10 ) edges[i->csc()-7].first = *i; break;
						case 4 : if( i->csc()>6 && i->csc()<10 ) edges[i->csc()-4].first = *i; break;
						case 7 : if( i->csc()>3 && i->csc()<10 && middle_csc[i->csc()-4].quality()==0 ) middle_csc[i->csc()-4] = *i; break;
						case 10: if( i->csc()>3 && i->csc()<10 ) edges[i->csc()-4].second = *i; break;
					}

				}
				cout<<" )"<<endl;
			}
		} else {
			cout<<"Empty record"<<endl;
		}

		for(int pos=0; pos<6; pos++){
			if( edges[pos].first.quality()>10 && edges[pos].second.quality()>10 ){
			//if( edges[pos].first.quality()>12 && edges[pos].second.wireGroup()>0 ){
			if( (edges[pos].second.wireGroup()-edges[pos].first.wireGroup())>=16 ){
				coincidence[pos].first++;
				if( middle_csc[pos].quality()>0 ){
                                	coincidence[pos].second++;
cout<<"Found three of a kind"<<endl;
				} else {
cout<<"Missing middle CSC"<<endl;
				}
			}}
		}

		nevents++;
	}

	for(int pos=0; pos<6; pos++)
		cout<<"pos: "<<pos<<" Num. of coincidence: "<<coincidence[pos].first<<" middle chamber: "<<coincidence[pos].second<<" eff= "<<(coincidence[pos].second/float(coincidence[pos].first))<<endl;

	return 0;
}
