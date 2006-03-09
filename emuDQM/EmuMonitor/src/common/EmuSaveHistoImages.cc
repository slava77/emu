#include "EmuLocalPlotter.h"



void EmuLocalPlotter::SaveImages(std::string path, std::string format, int width, int height) 
{
  LOG4CPLUS_INFO(logger_, "Generating Histo Images and saving them to " << path);
  if (format == "") { format = DEFAULT_IMAGE_FORMAT; }
  TString imgfile = "";

  std::ofstream folders_nav;
  std::ofstream canvas_nav;
  //  std::ofstream csc_nav;

  createHTMLNavigation(path);

  folders_nav.open((path+"/folders.html").c_str());
  

  folders_nav << "<html>\n"
	      << "<meta http-equiv=\"refresh\" content=\"10; URL=folders.html\">\n"
	      << "<body>";

  gStyle->SetPalette(1,0);
  gStyle->SetTitleW(0.95);
  gStyle->SetTitleH(0.085);
  /*
    string histodir = "histograms";
    TString command = Form("mkdir -p %s/%s",path.c_str(), histodir.c_str());
    gSystem->Exec(command.Data());
 
    LOG4CPLUS_DEBUG(logger_, " +++ Saving Histograms +++ ");

    for (map<int, map<string, TH1*> >::iterator itr = histos.begin(); itr != histos.end(); ++itr) {
    if (itr->first != SLIDES_ID) {
    for (map<string, TH1*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
    TString fullname = h_itr->second->GetName();
    // replace(fullname.begin(), fullname.end(), '/', '_');
    fullname.Replace(0, fullname.Last('/')+1,"");
    imgfile = Form ("%s/%s/%s.%s",path.c_str(), histodir.c_str(), fullname.Data(), format.c_str());

    LOG4CPLUS_DEBUG(logger_, imgfile);
    // string fullname = Form("%s_%s", (itr->first).c_str(), (h_itr->second->getFullName()).c_str());
    // imgfile = Form("%s/%s.%s", path, fullname, format);
    TCanvas *cnv = new TCanvas("cnv",fullname, width, height);
    cnv->cd();
    h_itr->second->Draw();
    cnv->Print(imgfile.Data());
    delete cnv;
    }
    }
    }
  */
  LOG4CPLUS_INFO(logger_, "Saving Canvases");
  for (map<int, map<string, ConsumerCanvas*> >::iterator itr = canvases.begin(); itr != canvases.end(); ++itr) {
    int id = itr->first;
    if (id != SLIDES_ID) 
      {
	if (id == 0) {
	  folders_nav << "<a href=\"canvases_common.html\" target=\"canvases_list\">Common DDU</a><br>\n"<< endl;
	  canvas_nav.open((path+"/canvases_common.html").c_str());
	  canvas_nav << "<html>\n"
		     << "<meta http-equiv=\"refresh\" content=\"10; URL=canvases_common.html\">\n"
		     << "<body>\n"
		     << "<h2>Common DDU Canvases</h2><hr>\n" << endl;
	} else {
	  int crate = (int)((id>>4) & 0xFF);
	  int slot =  (int)(id & 0xF);
	  string cscid = (Form("csc_%d_%d", crate, slot));
	  folders_nav << "<a href=\"canvases_" << cscid
		      << ".html\" target=\"canvases_list\">CSC crate" << crate << " slot" << slot << "</a><br>\n"<<endl;

	  canvas_nav.open((path+"/canvases_"+cscid+".html").c_str());	
	  canvas_nav << "<html>\n"
		     << "<meta http-equiv=\"refresh\" content=\"10; URL=canvases_"<<cscid<<".html\">\n"
		     << "<body>\n"
		     << "<h2>CSC crate" << crate << " slot" << slot <<" Canvases</h2><hr>\n" << endl;
	}
	for (map<string, ConsumerCanvas*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	  /*
	    string fullname = h_itr->second->GetName();
	    replace(fullname.begin(), fullname.end(), '/', '_');
	    imgfile =path + "/"+ fullname + "."+ format;
	    LOG4CPLUS_DEBUG(logger_, imgfile << " size" << width << ":" << height);
	    ((TCanvas *)(h_itr->second))->SetCanvasSize(width, height);
	    h_itr->second->Print(imgfile.c_str());
	  */
	  if( strncmp(h_itr->second->ClassName(),"TCanvas",7) ) continue;
	  TObject *obj = h_itr->second;

	  TString objPath(obj->GetName()), basename, dirname, fullPath, relPath;
	  Ssiz_t pos=0;
	  if( (pos = objPath.Last('/')) >=0 ){
	    basename = objPath(pos+1,objPath.Length()-pos-1);
	    dirname  = objPath(0,pos);

	  } else basename = objPath;
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
	    relPath = Form("crate%d/slot%d/%s", crate,slot,dirname.Data());
	    fullPath = Form("%s/%s",path.c_str(), relPath.Data());
	  } else {
	    relPath = dirname.Data();
	    fullPath = Form("%s/%s", path.c_str(),dirname.Data());
	  }
	  TString command = Form("mkdir -p %s",fullPath.Data());
	  gSystem->Exec(command.Data());
	  ((TCanvas*)obj)->SetCanvasSize(width, height);
	  ((TCanvas*)obj)->SetWindowSize(width, height);
	  obj->Draw();

	  imgfile = Form("%s/%s.%s", fullPath.Data(), basename(pos,basename.Length()-pos).Data(), format.c_str());
	  LOG4CPLUS_INFO(logger_, imgfile.Data());
	  ((TCanvas*)obj)->Print(imgfile.Data());
	  TString relfilename  = Form("%s/%s.%s", relPath.Data(), basename(pos,basename.Length()-pos).Data(), format.c_str()); 
	  canvas_nav << "<a href=\"" << relfilename << "\" target=\"imageout\">" << obj->GetName() << "</a><br>\n" << endl;
	  // delete obj;
	}
	canvas_nav << "</body>\n</html>\n" << endl;;
	canvas_nav.clear();
	canvas_nav.close();
      }
  }
  folders_nav << "</body>\n</html>\n";
  folders_nav.clear();
  folders_nav.close();

  LOG4CPLUS_INFO(logger_, "Done");
}



void EmuLocalPlotter::SaveImagesFromROOTFile(std::string rootfile,std::string path, std::string format, int width, int height)
{
  if (rootfile == "") { return;}
  LOG4CPLUS_INFO(logger_, "Generating Histo Images from " << rootfile << " and saving them to " << path);
  if (format == "") { format = DEFAULT_IMAGE_FORMAT; }
  TString imgfile = "";
  TString filename(rootfile.c_str());
  TFile f(rootfile.c_str());
  gStyle->SetPalette(1,0);
  gStyle->SetTitleW(0.95);
  gStyle->SetTitleH(0.085);



  filename.ReplaceAll(".root", "");
  filename.Replace(0, filename.Last('/')+1,"");
  //  TString command = Form("rmdir -rf %s/%s",path.c_str(), filename.Data());
  //  gSystem->Exec(command.Data());

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
      fullPath = Form("%s/%s/crate%d/slot%d/%s",path.c_str(), filename.Data(),crate,slot,dirname.Data());
    } else {
      fullPath = Form("%s/%s/%s", path.c_str(), filename.Data(),dirname.Data());
    }
    TString command = Form("mkdir -p %s",fullPath.Data());
    gSystem->Exec(command.Data());
    // obj->Draw();
    ((TCanvas*)obj)->SetCanvasSize(width, height);
    imgfile = Form("%s/%s.%s", fullPath.Data(), basename(pos,basename.Length()-pos).Data(), format.c_str());
    LOG4CPLUS_INFO(logger_, imgfile.Data());	
    ((TCanvas*)obj)->Print(imgfile.Data());
    delete obj;
  }
  LOG4CPLUS_INFO(logger_, "Done...");
}

void EmuLocalPlotter::createHTMLNavigation(std:: string path) 
{

  std::ofstream indexfile;
  indexfile.open((path+"/index.html").c_str());

	
  indexfile << "<html>\n"
//	    << "<meta http-equiv=\"refresh\" content=\"10; URL=index.html\">\n"
	    << "<frameset rows=\"25%, 75%\">\n"
	    << "<frameset cols=\"25%, 70%\">\n"
	    << "<frame src=\"folders.html\">\n"
	    << "<frame src=\"canvases.html\" name=\"canvases_list\">\n"
	    << "</frameset>\n"
	    << "<frame src=\"imageout.html\" name=\"imageout\">\n"
	    << "</frameset>\n"
	    << "</html>\n";
  indexfile.clear();
  indexfile.close();
  
  indexfile.open((path+"/canvases.html").c_str());  
  indexfile << "<html>\n"
             << "<meta http-equiv=\"refresh\" content=\"10; URL=canvases.html\">\n"
             << "<body>"
             << "</body>\n</html>\n";
  indexfile.clear();
  indexfile.close();

  indexfile.open((path+"/imageout.html").c_str());
  indexfile << "<html>\n"
             << "<meta http-equiv=\"refresh\" content=\"10; URL=imageout.html\">\n"
             << "<body>"
             << "</body>\n</html>\n";
  indexfile.clear();
  indexfile.close(); 
  


}
