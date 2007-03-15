void drawAll(const char *filename)
{
	bool eps=false;

	TFile f(filename);
	//TFile f("logs/RunNum499Evs300000to349999.bin.root");
	//f.ReadAll();
	//cout<<f.GetList()->GetSize()<<" objects read"<<endl;
	//TIterator *iter = f.GetList()->MakeIterator();

	gStyle->SetCanvasDefH(1000);
	gStyle->SetCanvasDefW(1200);

	gStyle->SetPalette(1,0);
	gStyle->SetTitleW(0.95);
	gStyle->SetTitleH(0.085);

	//TCanvas canvas;

	TString fn(filename);
	fn = fn(0,fn.Last('.'));
	fn.Append(".plots");
	//if( fn.Last('/')>0 && fn.Last('/')<fn.Length() )
	//	fn = fn(fn.Last('/')+1,fn.Length()-fn.Last('/')-1);

	char command[128];
	sprintf(command,"mkdir -p plots/%s",fn.Data());
	gSystem->Exec(command);
	filename = fn.Data();

	TIterator *keys = f.GetListOfKeys()->MakeIterator();

	TKey *key=0;
	while( ( key=(TKey*)keys->Next() ) ){
		if( strncmp(key->GetClassName(),"TCanvas",7) ) continue;
		TObject *obj = key->ReadObj();

		TString fullPath(obj->GetName()), basename, dirname;
		Ssiz_t pos=0;
		if( (pos = fullPath.Last('/')) >=0 ){
			basename = fullPath(pos+1,fullPath.Length()-pos-1);
			dirname  = fullPath(0,pos);
			cout<<"basename: "<<basename<<" dirname: "<<dirname<<endl;
			//char command[128];
			//sprintf(command,"mkdir -p plots/%s/%s",filename,dirname.Data());
			//gSystem->Exec(command);
		} else basename = fullPath;

		pos=0;
		while(
			basename[pos]=='1' ||
			basename[pos]=='2' ||
			basename[pos]=='3' ||
			basename[pos]=='4' ||
			basename[pos]=='5' ||
			basename[pos]=='6' ||
			basename[pos]=='7' ||
			basename[pos]=='8' ||
			basename[pos]=='9' ||
			basename[pos]=='0' ||
			basename[pos]=='_' ) pos++;

		if( pos > 0 ){
			TString location = basename(0,pos).Data();
			Ssiz_t space = location.First('_');
			TString sCrate(location(0,space));
			int crate = atoi(sCrate.Data());
			TString sSlot(location(space+1,pos-space-1));
			int slot  = atoi(sSlot.Data());
			if( crate==255 ) continue; 
			//cout<<"SLOT: "<<slot<<" CRATE: "<<crate<<endl;
			char command[128];
			sprintf(command,"mkdir -p plots/%s/crate%d/slot%d/%s",filename,crate,slot,dirname.Data());
			gSystem->Exec(command);
			obj->Draw();
			((TCanvas*)obj)->SetCanvasSize(1200,1000);
			if( eps ){
				((TCanvas*)obj)->Print("tmp.eps");
				char command2[512];
				sprintf(command2,"pstopnm -ppm -xborder 0 -yborder 0 -portrait -xsize 1200 -ysize 1000 tmp.eps && ppmtogif tmp.eps001.ppm>\"plots/%s/crate%d/slot%d/%s/%s.gif\" && rm tmp.eps*",filename,crate,slot,dirname.Data(),basename(pos,basename.Length()-pos).Data()); // ROOT crashes here if use same 'command' array
				if( gSystem->Exec(command2) ) exit(1);
			} else {
				char gifFile[512];
				sprintf(gifFile,"plots/%s/crate%d/slot%d/%s/%s.png",filename,crate,slot,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
			}
			delete obj;
		} else {
			char command[128];
			sprintf(command,"mkdir -p plots/%s/%s",filename,dirname.Data());
			gSystem->Exec(command);
			obj->Draw();
			((TCanvas*)obj)->SetCanvasSize(1200,1000);
			if( eps ){
				((TCanvas*)obj)->Print("tmp.eps");
				char command2[512];
				sprintf(command2,"pstopnm -ppm -xborder 0 -yborder 0 -portrait -xsize 1200 -ysize 1000  tmp.eps && ppmtogif tmp.eps001.ppm>\"plots/%s/%s.gif\" ",filename,fullPath.Data());
				if( gSystem->Exec(command2) ) exit(1);
			} else {
				char gifFile[512];
				sprintf(gifFile,"plots/%s/%s.png",filename,fullPath.Data());
				((TCanvas*)obj)->Print(gifFile);
			}

			delete obj;
		}
	}
exit(0);
}
