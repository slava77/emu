void drawAllSP(const char *filename){
	TFile f(filename);

	gStyle->SetCanvasDefH(1000);
	gStyle->SetCanvasDefW(1200);

	gStyle->SetPalette(1,0);
	gStyle->SetTitleW(0.95);
	gStyle->SetTitleH(0.085);

	gStyle->SetOptStat(0);

	TString fn(filename);
	fn = fn(0,fn.Last('.'));
	fn.Append(".plots");

	char command[1024];
	sprintf(command,"mkdir -p plots/%s",fn.Data());
	gSystem->Exec(command);
	filename = fn.Data();

	TIterator *keys = f.GetListOfKeys()->MakeIterator();

	TKey *key=0;
	TString summary_plain;
	TMap hist_ref;
	while( ( key=(TKey*)keys->Next() ) ){
		if( !strncmp(key->GetClassName(),"TH1C",4) ){
			TH1C *str = (TH1C*)key->ReadObj();
			if(strncmp(str->GetName(),"summary",7)) continue;
			summary_plain.Append(str->GetArray()+1,str->GetNbinsX());
		}

		if( strncmp(key->GetClassName(),"TCanvas",7) ) continue;
		TObject *obj = key->ReadObj();

		TString fullPath(obj->GetName()), basename, dirname;
		Ssiz_t pos=0;
		if( (pos = fullPath.Last('/')) >=0 ){
			basename = fullPath(pos+1,fullPath.Length()-pos-1);
			dirname  = fullPath(0,pos);
			cout<<"basename: "<<basename<<" dirname: "<<dirname<<endl;
		} else basename = fullPath;

		pos=5;
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

		if( pos > 5 ){
			TString location = basename(5,pos).Data();
			Ssiz_t space = location.First('_');
			TString sSP(location(0,space));
			int SP  = atoi(sSP.Data());
			int FN  = (pos>8?atoi(location(space+1,1).Data()):-1);
			int CSC = (pos>10?atoi(location(space+3,pos-space-3).Data()):-1);
			cout<<"SP: "<<SP<<" FN: "<<FN<<" CSC: "<<CSC<<endl;
			char command[1024];
			if(FN>0 && CSC>0){
				sprintf(command,"mkdir -p plots/%s/SP%d/F%d/CSC%d/%s",filename,SP,FN,CSC,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[1024];
				sprintf(gifFile,"plots/%s/SP%d/F%d/CSC%d/%s/%s.png",filename,SP,FN,CSC,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
				TList *pads = ((TCanvas*)obj)->GetListOfPrimitives();
				for(TObject *iter=(TObject*)pads->MakeIterator(),*pad=0; pad=((TIterator*)iter)->Next(); ){
					char hist_name[1024], link_name[1024];
					sprintf(hist_name,"%d_%d_%d_%s",SP,FN,CSC,pad->GetName());
					TNamed *full_hist_name = new TNamed(hist_name,gifFile);
					sprintf(link_name,"SP%d/F%d/CSC%d/%s",SP,FN,CSC,pad->GetName());
					TNamed *full_link_name = new TNamed(link_name,gifFile);
					hist_ref.Add((TObject*)full_hist_name,(TObject*)full_link_name);
				}
			}
			if(FN>0 && CSC<0){
				sprintf(command,"mkdir -p plots/%s/SP%d/F%d/%s",filename,SP,FN,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[1024];
				sprintf(gifFile,"plots/%s/SP%d/F%d/%s/%s.png",filename,SP,FN,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
				TList *pads = ((TCanvas*)obj)->GetListOfPrimitives();
				for(TObject *iter=(TObject*)pads->MakeIterator(),*pad=0; pad=((TIterator*)iter)->Next(); ){
					char hist_name[1024], link_name[1024];
					sprintf(hist_name,"%d_%d_%s",SP,FN,pad->GetName());
					TNamed *full_hist_name = new TNamed(hist_name,gifFile);
					sprintf(link_name,"%d_%d_%s",SP,FN,pad->GetName());
					TNamed *full_link_name = new TNamed(link_name,gifFile);
					hist_ref.Add((TObject*)full_hist_name,(TObject*)full_link_name);
				}
			}
			if(FN<0 && CSC<0){
				sprintf(command,"mkdir -p plots/%s/SP%d/%s",filename,SP,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[1024];
				sprintf(gifFile,"plots/%s/SP%d/%s/%s.png",filename,SP,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
				TList *pads = ((TCanvas*)obj)->GetListOfPrimitives();
				for(TObject *iter=(TObject*)pads->MakeIterator(),*pad=0; pad=((TIterator*)iter)->Next(); ){
					char hist_name[1024], link_name[1024];
					sprintf(hist_name,"%d_%s",SP,pad->GetName());
					TNamed *full_hist_name = new TNamed(hist_name,gifFile);
					sprintf(link_name,"%d_%s",SP,pad->GetName());
					TNamed *full_link_name = new TNamed(link_name,gifFile);
					hist_ref.Add((TObject*)full_hist_name,(TObject*)full_link_name);
				}
			}
			delete obj;
		} else {
			char command[1024];
			sprintf(command,"mkdir -p plots/%s/%s",filename,dirname.Data());
			gSystem->Exec(command);
			obj->Draw();
			((TCanvas*)obj)->SetCanvasSize(1200,1000);
			TList *pads = ((TCanvas*)obj)->GetListOfPrimitives();
			char gifFile[1024];
			sprintf(gifFile,"plots/%s/%s.png",filename,fullPath(5,basename.Length()-5).Data());
			((TCanvas*)obj)->Print(gifFile);
			TList *pads = ((TCanvas*)obj)->GetListOfPrimitives();
			for(TObject *iter=(TObject*)pads->MakeIterator(),*pad=0; pad=((TIterator*)iter)->Next(); ){
				TNamed *full_hist_name = new TNamed(pad->GetName(),gifFile);
				TNamed *full_link_name = new TNamed(pad->GetName(),gifFile);
				hist_ref.Add((TObject*)full_hist_name,(TObject*)full_link_name);
			}
			delete obj;
		}
	}

	TString summary_html(summary_plain);
	for(Ssiz_t from=summary_html.Index("Failed: ",8,0),to=summary_html.Index("\n",from); from>0&&to>0; from=summary_html.Index("Failed: ",to),to=summary_html.Index("\n",from)){
		TString name = summary_html(from+14,to-from-14);
		TPair *ref = (TPair*)hist_ref.FindObject(name);
		if(ref){
			const char *link = ref->Key()->GetTitle();
			link += strlen(filename)+7;
			char href[1024];
			sprintf(href,"&nbsp;&nbsp;&nbsp;<a href=\"%s\">%s</a>\n",link,ref->Value()->GetName());
			summary_html.Replace(from,to-from,href);
		} else cout<<"Can't find canvas for "<<name<<endl;
	}
	summary_html.ReplaceAll("\n",1,"<br>\n",5);

	for(Ssiz_t from=summary_html.Index("Timing",6,0),to=summary_html.Index("\n",from),first_time=1; from>0&&to>0; from=summary_html.Index("\n",from)+1,to=summary_html.Index("\n",from)){
		if( from == to ) continue;
		TString line = summary_html(from,to-from);
		line.ReplaceAll("<br>",4,"",0);
		if( first_time ){
                        char tab[1024];
			sprintf(tab,"<table><tr><td>%s</td></tr><tr><td>Chambers</td><td>Mean</td><td>RMS</td><td>nHits</td></tr>\n",line.Data());
			summary_html.Replace(from,to-from,tab);
			first_time = 0;
			continue;
		}
		Ssiz_t mean  = line.Index(" = ",     0);
		Ssiz_t rms   = line.Index("RMS = ",  0);
		Ssiz_t nhits = line.Index("nHits = ",0);
		if( mean<0 || rms<0 || nhits<0 ){
			char row[1024];
			sprintf(row,"<tr><td>%s</td></tr>\n",line.Data());
			summary_html.Replace(from,to-from,row); 			
		} else {
			char row[1024];
			TString _diff = line(0,17);
			TString _mean = line(mean+3,5);
			TString _rms  = line(rms+6,4);
			TString _nhits= line.Data()+nhits+8;
			sprintf(row,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",_diff.Data(),_mean.Data(),_rms.Data(),_nhits.Data());
			summary_html.Replace(from,to-from,row); 			
		}
	}
	summary_html.Append("</table>");

	summary_html.Prepend("<html>\n<head>\n</head>\n<body>\n");
	summary_html.Append("</body>\n<html>\n");
	sprintf(command,"mkdir -p plots/%s",filename);
	gSystem->Exec(command);
	FILE *file=0;
	char summary[1024];
	sprintf(summary,"plots/%s/summary.html",filename);
	if( (file=fopen(summary,"wt"))!=NULL )
		fwrite(summary_html.Data(),1,summary_html.Length(),file);
	else cout<<"Cannot dump summary"<<endl;

exit(0);
}
