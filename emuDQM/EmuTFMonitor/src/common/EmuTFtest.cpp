#include "RegEx.cc"
#include "EmuTFbookkeeper.h"
#include "EmuTFxmlParsing.h"
#include "EmuTFfiller.h"
#include "EmuTFvalidator.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1C.h"
#include "TROOT.h"
#include "TFile.h"
#include "TApplication.h"

#include "IORawData/CSCCommissioning/src/FileReaderDDU.cc"

#include "EventFilter/CSCTFRawToDigi/src/CSCTFEvent.h"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPTrailer.h"

#include <math.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

//#define BASE "/nfshome0/cscdqm/TriDAS/emu/emuDQM/"

//To compile on lxplus:   g++ -o test EmuTFtest.cpp -I/afs/cern.ch/cms/Releases/XDAQ/XDAQ_3_4/daq/extern/xerces/linuxx86/include/ -L/afs/cern.ch/cms/Releases/XDAQ/XDAQ_3_4/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/afs/cern.ch/user/k/kkotov/CMSSW_0_8_0_pre2/src/ -I../../src/common/ -I../../include/
//To compile on cmsdaq03: g++ -o test EmuTFtest.cpp -I$XDAQ_ROOT/daq/extern/xerces/linuxx86/include/ -L$XDAQ_ROOT/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/home/daq/kotov/CMSSW_0_5_1/src/ -I../../src/common/ -I../../include/
//To compile on slice11:  g++ -o test EmuTFtest.cpp -I/home/slice/DAQKit/v3.6/TriDAS/daq/extern/xerces/linuxx86/include/ -I../../src/common/ -I../../include/ -L/home/slice/DAQKit/v3.6/TriDAS/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/home/slice/kotov/CMSSW_0_8_0_pre2/src/
//To compile on emudqm: g++ -o test EmuTFtest.cpp -I/home/cscdqm/DAQKit/v4.2.1/TriDAS/daq/extern/xerces/linuxx86/include/ -I../../src/common/ -I../../include/ -L/home/cscdqm/DAQKit/v4.2.1/TriDAS/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/home/cscdqm/CMSSW_1_3_0_pre4/src
//To compile on csc-C2D07-10: g++ -o test EmuTFtest.cpp -DBASE="\"/nfshome0/cscdqm/TriDAS/emu/emuDQM/\"" -I/nfshome0/cscdqm/DAQKit/TriDAS/daq/extern/xerces/linuxx86/include/ -I../../src/common/ -I../../include/ -L/nfshome0/cscdqm/DAQKit/TriDAS/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/nfshome0/cscdqm/CMSSW/src
//With central installation of XDAQ: g++ -o test EmuTFtest.cpp -DBASE="\"/nfshome0/cscdqm/TriDAS/emu/emuDQM/\"" -I/opt/xdaq/include/ -I../../src/common/ -I../../include/ -L/opt/xdaq/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/nfshome0/cscdqm/CMSSW/src

int main(int argc, char *argv[]){
	using namespace std;
	using namespace EmuTFxmlParsing;
	// Root specific
	extern void InitGui();
	VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
	TROOT root("ROOTSYSTEM","ROOT_", initfuncs);
	int a=2;
	char *v[]={"qwe","-b"};
	TApplication app("qwe", &a, v);

	// Histograms/Canvases description
	std::map<std::string,HistAttributes>   histList   = parseHistXML  (BASE "EmuTFMonitor/xml/CSCTF_histograms.xml");
	std::map<std::string,CanvasAttributes> canvasList = parseCanvasXML(BASE "EmuTFMonitor/xml/CSCTF_canvases.xml");
	std::map<std::string,CheckAttributes>  checkList  = parseCheckXML (BASE "EmuTFMonitor/xml/CSCTF_checks.xml");

	EmuTFfiller filler(histList);

	// DDU File Reader
	FileReaderDDU reader;
	// Keep last event
	size_t last_size=0;
	const unsigned short *last_buf=0;
	// Keep track of dates
	map<string,string> history;

	for(int arg=1; arg<argc; arg++){
		struct stat info;
		if( stat(argv[arg],&info)==0 ){
			history[argv[arg]] = ctime(&info.st_ctime);
			size_t pos = history[argv[arg]].rfind('\n');
			if( pos!=string::npos ) history[argv[arg]].erase(pos);
		}
		else continue;

		reader.open(argv[arg]);
		//reader.reject(FileReaderDDU::DDUoversize|FileReaderDDU::FFFF);
		//reader.select(FileReaderDDU::Type1);
		reader.select(0);

		// Event buffer
		size_t size=0, nevents=0;
		const unsigned short *buf=0;

		// Main cycle
		while( (size = reader.read(buf)) /*&& nevents<117500*/ ){
			filler.fill(buf,size,reader.status());
			if( filler.last_event_status() == EmuTFfiller::CLEAR ){
				last_size = size;
				last_buf  = buf;
			}
			nevents++;
		}

	}



	// Summary Information
	std::ostringstream str;
	for(map<string,string>::const_iterator file=history.begin(); file!=history.end(); file++)
		str<<file->first<<"  ["<<file->second<<"]"<<endl;

	if( last_size && last_buf ){
		CSCTFEvent tfEvent;
		tfEvent.unpack(last_buf,last_size);
		vector<CSCSPEvent> SPs = tfEvent.SPs();
		for(vector<CSCSPEvent>::const_iterator spPtr=SPs.begin(); spPtr!=SPs.end(); spPtr++){
			unsigned short sp = spPtr->header().sector() + ( spPtr->header().endcap() ? 0 : 6 );
			str<<"SP"<<sp<<"  boar_id=0x"<<hex<<spPtr->trailer().board_id()<<dec<<" SP readout configuration: "<<spPtr->trailer().year()<<"/"<<spPtr->trailer().month()<<"/"<<spPtr->trailer().day()<<endl;
		}
	}

	EmuTFvalidator validator(filler.bookkeeper());
	str<<"nErrors: "<<validator.check(checkList)<<endl;
	list<string> failed = validator.falied();
	for(list<string>::const_iterator hist=failed.begin(); hist!=failed.end(); hist++)
		str<<" Failed: "<<*hist<<endl;
	list<string> passed = validator.passed();
	//for(list<string>::const_iterator hist=passed.begin(); hist!=passed.end(); hist++)
	//	str<<" Passed: "<<*hist<<endl;
	//str<<ends;

//	map<string,string> alias;
//	alias[""] = "";

	map<string,string> neighbour;
	neighbour["F2/CSC4"] = "F3/CSC7";
	neighbour["F2/CSC5"] = "F3/CSC8";
	neighbour["F2/CSC6"] = "F3/CSC9";
	neighbour["F2/CSC7"] = "F3/CSC7";
	neighbour["F2/CSC8"] = "F3/CSC8";
	neighbour["F2/CSC9"] = "F3/CSC9";

	neighbour["F1/CSC4"] = "F3/CSC4";
	neighbour["F1/CSC5"] = "F3/CSC5";
	neighbour["F1/CSC6"] = "F3/CSC6";
	neighbour["F1/CSC7"] = "F3/CSC4";
	neighbour["F1/CSC8"] = "F3/CSC5";
	neighbour["F1/CSC9"] = "F3/CSC6";

	neighbour["F4/CSC4"] = "F3/CSC4";
	neighbour["F4/CSC5"] = "F3/CSC5";
	neighbour["F4/CSC6"] = "F3/CSC6";
	neighbour["F4/CSC7"] = "F3/CSC7";
	neighbour["F4/CSC8"] = "F3/CSC8";
	neighbour["F4/CSC9"] = "F3/CSC9";

	neighbour["F4/CSC1"] = "F3/CSC1";
	neighbour["F4/CSC2"] = "F3/CSC2";
	neighbour["F4/CSC3"] = "F3/CSC3";

	neighbour["F1/CSC4"] = "F3/CSC4";
	neighbour["F1/CSC5"] = "F3/CSC5";
	neighbour["F1/CSC6"] = "F3/CSC6";

	neighbour["F2/CSC4"] = "F3/CSC7";
	neighbour["F2/CSC5"] = "F3/CSC8";
	neighbour["F2/CSC6"] = "F3/CSC9";

	neighbour["F1/CSC7"] = "F3/CSC4";
	neighbour["F1/CSC8"] = "F3/CSC5";
	neighbour["F1/CSC9"] = "F3/CSC6";

	neighbour["F2/CSC7"] = "F3/CSC7";
	neighbour["F2/CSC8"] = "F3/CSC8";
	neighbour["F2/CSC9"] = "F3/CSC9";

	neighbour["F1/CSC1"] = "F3/CSC1";
	neighbour["F1/CSC2"] = "F3/CSC2";
	neighbour["F1/CSC3"] = "F3/CSC2";

	neighbour["F2/CSC1"] = "F3/CSC2";
	neighbour["F2/CSC2"] = "F3/CSC2";
	neighbour["F2/CSC3"] = "F3/CSC3";

	str<<endl<<"Timing for neighbour chambers: "<<endl<<endl;
	for(int mpc=1; mpc<5; mpc++){
		for(int csc=1; csc<10; csc++){
			ostringstream label;
			label<<"F"<<mpc<<"/CSC"<<csc;
			TH2F *timing = (TH2F*)filler.bookkeeper().get("csc_timing",1,mpc,csc);
			if( !timing ) continue;
			for(int yBin=1; yBin<timing->GetNbinsY(); yBin++){
				if( neighbour[label.str()] != timing->GetYaxis()->GetBinLabel(yBin) ) continue;
				double m0=0, m1=0, m2=0;
				for(int xBin=1; xBin<timing->GetNbinsX(); xBin++){
					m0 += timing->GetBinContent(xBin,yBin);
					m1 += timing->GetBinContent(xBin,yBin)*(xBin-8);
					m2 += timing->GetBinContent(xBin,yBin)*(xBin-8)*(xBin-8);

				}
				str<<label.str()<<" - "<<neighbour[label.str()]<<(m1>0?" = +":" = ")<<m1/m0<<" RMS = "<<sqrt(m2/m0-m1*m1/m0/m0)<<" nHits = "<<m0<<endl;
			}

		}
	}

	map<string,string> overlap;
	overlap["F4/CSC4"] = "F4/CSC5";
	overlap["F4/CSC5"] = "F4/CSC6";
	overlap["F4/CSC6"] = "F4/CSC7";
	overlap["F4/CSC7"] = "F4/CSC8";
	overlap["F4/CSC8"] = "F4/CSC9";

	overlap["F3/CSC4"] = "F3/CSC5";
	overlap["F3/CSC5"] = "F3/CSC6";
	overlap["F3/CSC6"] = "F3/CSC7";
	overlap["F3/CSC7"] = "F3/CSC8";
	overlap["F3/CSC8"] = "F3/CSC9";

	overlap["F4/CSC1"] = "F4/CSC2";
	overlap["F4/CSC2"] = "F4/CSC3";

	overlap["F3/CSC1"] = "F3/CSC2";
	overlap["F3/CSC2"] = "F3/CSC3";

	overlap["F1/CSC1"] = "F1/CSC2";
	overlap["F1/CSC2"] = "F1/CSC3";
	overlap["F1/CSC3"] = "F2/CSC1";
	overlap["F1/CSC4"] = "F1/CSC5";
	overlap["F1/CSC5"] = "F1/CSC6";
	overlap["F1/CSC6"] = "F2/CSC4";
	overlap["F1/CSC7"] = "F1/CSC8";
	overlap["F1/CSC8"] = "F1/CSC9";

	overlap["F2/CSC1"] = "F2/CSC2";
	overlap["F2/CSC2"] = "F2/CSC3";
	overlap["F2/CSC4"] = "F2/CSC5";
	overlap["F2/CSC5"] = "F2/CSC6";
	overlap["F2/CSC7"] = "F2/CSC8";
	overlap["F2/CSC8"] = "F2/CSC9";

	str<<endl<<"Timing for overlapping chambers: "<<endl<<endl;
	for(int mpc=1; mpc<5; mpc++){
		for(int csc=1; csc<10; csc++){
			ostringstream label;
			label<<"F"<<mpc<<"/CSC"<<csc;
			TH2F *timing = (TH2F*)filler.bookkeeper().get("csc_timing",1,mpc,csc);
			if( !timing ) continue;
			for(int yBin=1; yBin<=timing->GetNbinsY(); yBin++){
				if( overlap[label.str()] != timing->GetYaxis()->GetBinLabel(yBin) ) continue;
				double m0=0, m1=0, m2=0;
				for(int xBin=1; xBin<timing->GetNbinsX(); xBin++){
					m0 += timing->GetBinContent(xBin,yBin);
					m1 += timing->GetBinContent(xBin,yBin)*(xBin-8);
					m2 += timing->GetBinContent(xBin,yBin)*(xBin-8)*(xBin-8);

				}
				str<<label.str()<<" - "<<overlap[label.str()]<<(m1>0?" = +":" = ")<<m1/m0<<" RMS = "<<sqrt(m2/m0-m1*m1/m0/m0)<<" nHits = "<<m0<<endl;
			}

		}
	}
	str<<endl;

	std::map<std::string,TCanvas*> rootCanvases = filler.bookkeeper().wrapToCanvases(canvasList);
	std::map<std::string,TCanvas*>::iterator iter = rootCanvases.begin();
	TFile file("qqq.root","RECREATE");
	while( iter != rootCanvases.end() ){
		//iter->second->Draw();
		iter->second->Write();
		iter++;
	}
	// Dump summary information
	string sum = str.str();
	TH1C summary("summary","Summary",sum.length(),0,1);
	for(unsigned int i=0;i<sum.length();i++) summary.SetBinContent(i+1,sum[i]);
	cout<<"Summary ["<<summary.GetNbinsX()<<"="<<sum.length()<<"]:"<<endl<<(summary.GetArray()+1)<<endl;
	summary.Write();
	file.Close();

	app.Terminate();
	return 0;
}
