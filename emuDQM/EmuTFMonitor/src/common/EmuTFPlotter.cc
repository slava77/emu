#include "EmuTFPlotter.h"

EmuTFPlotter::EmuTFPlotter(void):filler(0){
// Histogram configuration defaults given that BASE in correctly set during compilation with -DBASE key
#ifdef BASE
	histList   = EmuTFxmlParsing::parseHistXML  (BASE "EmuTFMonitor/xml/CSCTF_histograms.xml";);
	canvasList = EmuTFxmlParsing::parseCanvasXML(BASE "EmuTFMonitor/xml/CSCTF_canvases.xml");
	checkList  = EmuTFxmlParsing::parseCheckXML (BASE "EmuTFMonitor/xml/CSCTF_checks.xml");
	filler     = new EmuTFfiller(histList);
#endif
	nevents = 0;
}

void EmuTFPlotter::setXMLHistosBookingCfgFile(string filename){
	histList = EmuTFxmlParsing::parseHistXML(filename);
	if( filler ) delete filler;
	filler = new EmuTFfiller(histList);
}

void EmuTFPlotter::setXMLCanvasesCfgFile(string filename){
	canvasList = EmuTFxmlParsing::parseCanvasXML(filename);
}

void EmuTFPlotter::processEvent(const char *data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber=0){
	if( filler ){
		// Prepare printouts first:
		class shut_up : public std::ostream {
			class dummy_buffer : public std::streambuf{} db;
			public: shut_up():std::ostream(db){}
		} null;
		filler->setOutputStream(&null);
		ostringstream cerr;
		filler->setErrorStream (&cerr);
		// run TF DQM
		filler->fill((const unsigned short*)data,dataSize/sizeof(const unsigned short*),errorStat);
		// anything to say?
		std::cerr<<cerr.str()<<std::endl;
	}
	// If any new histograms were booked while processing event, regenerate MEs and MECanvases lists:
	if( filler->bookkeeper().modified() ){ //&& nevents>1000 ){
		filler.bookkeeper().cleenupCanvases();
		std::map<std::string,TCanvas*> rootCanvases = filler.bookkeeper().wrapToCanvases(canvasList);
		for( std::map<std::string,TCanvas*>::iterator iter=rootCanvases.begin(); iter!=rootCanvases.end(); iter++){

			// Take care of canvases first
			// obscure, but robust way to parse canvas name:
			size_t name_start=std::string::npos;
			for(size_t pos1=iter->first.find("TF_")+3, pos2=iter->first.find('_',pos1), id=0;
				(name_start=pos1) && id<3 && pos1!=std::string::npos && pos2!=std::string::npos;
				pos1=pos2+1, pos2=iter->first.find('_',pos1), id++);
			std::string prefix = iter->first.substr(0,name_start);
			// create new EmuMonitoringCanvas only for new canvases
			if( MECanvases[prefix].find(iter->first) == MECanvases[prefix].end() ){
				EmuMonitoringCanvas *obj = new EmuMonitoringCanvas(); // from TCanvas = iter->second
				obj->setPrefix(prefix);
				obj->setName  (iter->first);
				obj->setTitle (iter->second->GetTitle());
				MECanvases[prefix][iter->first] = obj;
			}

			// Go into each canvas and see what histograms (pads) it has
			TList hisoPads = iter->second->GetListOfPrimitives();
			TIterator *iter = hisoPads->MakeIterator();
			TPad *pad = 0;
			while( (pad=(TPad*)iter->Next()) ){
				TH1 *hist = pad->GetListOfPrimitives()->At(0);
				if( !hist ) continue;
				std::string name = hist->GetName();
				// parse histogram name:
				for(size_t pos1=name.find("TF_")+3, pos2=name.find('_',pos1), id=0;
					(name_start=pos1) && id<3 && pos1!=std::string::npos && pos2!=std::string::npos;
					pos1=pos2+1, pos2=name.find('_',pos1), id++);
				std::string prefix = iter->first.substr(0,name_start);
				if( MEs[prefix].find(name) == MEs[prefix].end() ){
					EmuMonitoringObject *obj = new EmuMonitoringObject(); // from TH1 hist
					obj->setPrefix(prefix);
					obj->setName  (name);
					obj->setTitle (hist->GetTitle());
					MEs[prefix][name] = obj;
				}
			}

		}
		filler->bookkeeper().resetModified()
	}
	nevents++;
}
