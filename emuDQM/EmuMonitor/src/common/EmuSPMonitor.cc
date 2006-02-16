#include <stdio.h>
#include <cstdarg>
#include "EmuSPMonitor.h"
#include "TVirtualPad.h"
#include "TStyle.h"
using namespace std;
//#include "TrackFinderDataFormat/include/SRSPEventData.h"
#include "Trigger/L1CSCTBRawFormat/interface/L1MuCSCEventData.h"

EmuSPMonitor::EmuSPMonitor(void):
	hTF_LCTs   ("hTF_LCTs",   "Any LCTs in Chamber",       16,0,16),
	hTF_CLCTs  ("hTF_CLCTs",  "Only CLCTs in Chamber",     16,0,16),
	hTF_ALCTs  ("hTF_ALCTs",  "Only ALCTs in Chamber",     16,0,16),
	hTF_CORLCTs("hTF_CORLCTs","Correlated LCTs in Chamber",16,0,16),
	hTF_Eff    ("hTF_Eff",    "LCT Eff vs. Chamber",       16,0,16),
	hTF_L1As   ("hTF_L1As",   "L1As",                      16,0,16),
	hTF_ModeProblem("hTF_ModeProblem","Reported modes match",3,0,3),
	hTF_nTracks("hTF_nTracks","Number of Tracks per event",21,0,21),
	hTF_bxTrack("hTF_bxTracks","Time distribution of tracks",7,0,7),
	hTF_bxFirstTrack ("hTF_bxFirstTrack", "Time when first track appeared",  7,0,7),
	hTF_bxSecondTrack("hTF_bxSecondTrack","Time when second track appeared", 7,0,7),
	hTF_bxThirdTrack ("hTF_bxThirdTrack", "Time when third track appeared",  7,0,7),
	hTF_BXA    ("hTF_BXA", "BX analizer statistics",  3,-1,2),
	hTF_SEQ    ("hTF_SEQ", "LCT Sequence cases",  3,-1,2),
	hTF_bxaEff ("hTF_bxaEff","Track Finding efficiency",3,-1,2),
	hTF_L1AvsBX("hTF_L1AvsBX","L1A vs. BX",     128,0,128,128,0,128),
	hTF_VP     ("hTF_VP",     "Reported Valid Pattern Error ",1,0,1,15,0,15),
	hTF_TrkPhi ("hTF_TrkPhi", "#phi_{track}", 32, 0, 32),
	hTF_TrkEta ("hTF_TrkEta", "#eta_{track}", 32, 0, 32),
	hTF_Pt     ("hTF_Pt",     "p_{T}", 256, 0, 256),
	cnv1       ("cTF1",       "Number of Charged Tracks in chambers"),
	cnv2       ("cTF2",       "Some TF plots"),
	cnv3       ("cTF3",       "BX distribution"),
	cnv4       ("cTF4",       ""),
	cnv5       ("cTF5",       "Tracks"),
	cnv6       ("cTF6",       "")
{
	cnv1.Divide(2,2);

	hTF_LCTs.SetXTitle("CSC");
	hTF_LCTs.SetYTitle("Events");
	hTF_LCTs.SetFillColor(15);
	hTF_LCTs.SetStats(false);
	hTF_LCTs.GetXaxis()->SetLabelSize(0.05);
	hTF_LCTs.GetYaxis()->SetLabelSize(0.05);
	hTF_LCTs.GetXaxis()->SetTitleSize(0.05);
	hTF_LCTs.GetYaxis()->SetTitleSize(0.05);
	hTF_LCTs.GetXaxis()->SetTitleOffset(0.9);
	hTF_LCTs.GetYaxis()->SetTitleOffset(1.2);
	hTF_LCTs.GetXaxis()->SetNdivisions(520);
	//hTF_LCTs.GetXaxis()->SetTitleSize(0.06);
	//hTF_LCTs.GetYaxis()->SetTitleSize(0.06);
	cnv1.cd(4);
	hTF_LCTs.Draw();

	hTF_CLCTs.SetXTitle("CSC");
	hTF_CLCTs.SetYTitle("Events");
	hTF_CLCTs.SetFillColor(15);
	hTF_CLCTs.SetStats(false);
	hTF_CLCTs.GetXaxis()->SetLabelSize(0.05);
	hTF_CLCTs.GetYaxis()->SetLabelSize(0.05);
	hTF_CLCTs.GetXaxis()->SetTitleSize(0.05);
	hTF_CLCTs.GetYaxis()->SetTitleSize(0.05);
	hTF_CLCTs.GetXaxis()->SetTitleOffset(0.9);
	hTF_CLCTs.GetYaxis()->SetTitleOffset(1.2);
	hTF_CLCTs.GetXaxis()->SetNdivisions(520);
	cnv1.cd(2);
	hTF_CLCTs.Draw();

	hTF_ALCTs.SetXTitle("CSC");
	hTF_ALCTs.SetYTitle("Events");
	hTF_ALCTs.SetFillColor(15);
	hTF_ALCTs.SetStats(false);
	hTF_ALCTs.GetXaxis()->SetLabelSize(0.05);
	hTF_ALCTs.GetYaxis()->SetLabelSize(0.05);
	hTF_ALCTs.GetXaxis()->SetTitleSize(0.05);
	hTF_ALCTs.GetYaxis()->SetTitleSize(0.05);
	hTF_ALCTs.GetXaxis()->SetTitleOffset(0.9);
	hTF_ALCTs.GetYaxis()->SetTitleOffset(1.2);
	hTF_ALCTs.GetXaxis()->SetNdivisions(520);
	cnv1.cd(1);
	hTF_ALCTs.Draw();

	hTF_CORLCTs.SetXTitle("CSC");
	hTF_CORLCTs.SetYTitle("Events");
	hTF_CORLCTs.SetFillColor(15);
	hTF_CORLCTs.SetStats(false);
	hTF_CORLCTs.GetXaxis()->SetLabelSize(0.05);
	hTF_CORLCTs.GetYaxis()->SetLabelSize(0.05);
	hTF_CORLCTs.GetXaxis()->SetTitleSize(0.05);
	hTF_CORLCTs.GetYaxis()->SetTitleSize(0.05);
	hTF_CORLCTs.GetXaxis()->SetTitleOffset(0.9);
	hTF_CORLCTs.GetYaxis()->SetTitleOffset(1.2);
	hTF_CORLCTs.GetXaxis()->SetNdivisions(520);
	cnv1.cd(3);
	hTF_CORLCTs.Draw();

	TIterator *iter = cnv1.GetListOfPrimitives()->MakeIterator();
	TVirtualPad *_pad;
	while( (_pad=(TVirtualPad*)iter->Next()) ) _pad->SetLeftMargin(0.13);
	delete iter;

	cnv2.Divide(2,2);

	hTF_L1As.SetXTitle("CSC");
	hTF_L1As.SetYTitle("Num of L1As");
	hTF_L1As.SetFillColor(15);
	hTF_L1As.SetStats(false);
	hTF_L1As.GetXaxis()->SetLabelSize(0.05);
	hTF_L1As.GetYaxis()->SetLabelSize(0.05);
	hTF_L1As.GetXaxis()->SetTitleSize(0.05);
	hTF_L1As.GetYaxis()->SetTitleSize(0.05);
	hTF_L1As.GetXaxis()->SetTitleOffset(0.9);
	hTF_L1As.GetYaxis()->SetTitleOffset(1.2);
	hTF_L1As.GetXaxis()->SetNdivisions(520);
	cnv2.cd(1);
	((TVirtualPad*)cnv2.GetListOfPrimitives()->At(0))->SetLeftMargin(0.13);
 	hTF_L1As.Draw();

	hTF_Eff.SetXTitle("CSC");
	hTF_Eff.SetYTitle("Efficiency");
	hTF_Eff.SetFillColor(15);
	hTF_Eff.SetStats(false);
	hTF_Eff.GetXaxis()->SetLabelSize(0.05);
	hTF_Eff.GetYaxis()->SetLabelSize(0.05);
	hTF_Eff.GetXaxis()->SetTitleSize(0.05);
	hTF_Eff.GetYaxis()->SetTitleSize(0.05);
	hTF_Eff.GetXaxis()->SetTitleOffset(0.9);
	hTF_Eff.GetYaxis()->SetTitleOffset(0.9);
	hTF_Eff.GetXaxis()->SetNdivisions(520);
	cnv2.cd(2);
	hTF_Eff.Draw();

	hTF_L1AvsBX.SetXTitle("L1A");
	hTF_L1AvsBX.SetYTitle("BX");
	hTF_L1AvsBX.SetFillColor(15);
	hTF_L1AvsBX.SetStats(false);
	hTF_L1AvsBX.GetXaxis()->SetLabelSize(0.05);
	hTF_L1AvsBX.GetYaxis()->SetLabelSize(0.05);
	hTF_L1AvsBX.GetXaxis()->SetTitleSize(0.05);
	hTF_L1AvsBX.GetYaxis()->SetTitleSize(0.05);
	hTF_L1AvsBX.GetXaxis()->SetTitleOffset(0.9);
	hTF_L1AvsBX.GetYaxis()->SetTitleOffset(0.9);
	//hTF_L1AvsBX.GetXaxis()->SetNdivisions(520);
	cnv2.cd(3);
	hTF_L1AvsBX.Draw();

	hTF_VP.SetFillColor(15);
	hTF_VP.SetStats(false);
	cnv2.cd(4);
	TVirtualPad *pad = (TVirtualPad*)cnv2.GetListOfPrimitives()->At(3);
	hTF_VP.GetXaxis()->SetBinLabel(1,"Number of events");
	for(int link=1; link<=15; link++){ char buff[128]; sprintf(buff,"Link %d",link); hTF_VP.GetYaxis()->SetBinLabel(link,buff); }
	hTF_VP.SetOption("textcolz");
	hTF_VP.GetXaxis()->SetLabelSize(0.07);
	hTF_VP.GetYaxis()->SetLabelSize(0.07);
	hTF_VP.GetXaxis()->SetTitleSize(0.05);
	hTF_VP.GetYaxis()->SetTitleSize(0.05);
	hTF_VP.GetXaxis()->SetTitleOffset(0.9);
	hTF_VP.GetYaxis()->SetTitleOffset(0.9);
	hTF_VP.SetMarkerSize(2.9);
	pad->SetLeftMargin(0.2);
	pad->SetRightMargin(0.13);
	//pad->SetFillColor(0);
	pad->SetGridy(1);
	hTF_VP.Draw();

	cnv3.Divide(3,3);

	for(int cscId=1; cscId<11; cscId++){
		// Hist name and title
		if( cscId==7 ) continue; // On Test Beams no CSC7
		char hname[256], htitle[256];
		sprintf(hname,"hTF_CSC%dBX",cscId);
		sprintf(htitle,"BX Distribution for CSC%d",cscId);
		hTF_CSCBX[cscId].SetBins(7,0,7);
		hTF_CSCBX[cscId].SetName (hname);
		hTF_CSCBX[cscId].SetTitle(htitle);
		hTF_CSCBX[cscId].SetXTitle("BX");
		hTF_CSCBX[cscId].SetYTitle("Number of events");
		hTF_CSCBX[cscId].SetFillColor(15);
		hTF_CSCBX[cscId].SetStats(false);
		cnv3.cd((cscId<7?cscId:cscId-1));

		hTF_CSCBX[cscId].GetXaxis()->SetLabelSize(0.06);
		hTF_CSCBX[cscId].GetYaxis()->SetLabelSize(0.06);
		hTF_CSCBX[cscId].GetXaxis()->SetTitleSize(0.06);
		hTF_CSCBX[cscId].GetYaxis()->SetTitleSize(0.06);
		hTF_CSCBX[cscId].GetXaxis()->SetTitleOffset(0.9);
		hTF_CSCBX[cscId].GetYaxis()->SetTitleOffset(1.2);
		//hTF_CSCBX[cscId].SetTitleSize(1.);
		TVirtualPad *pad_ = (TVirtualPad*)cnv3.GetListOfPrimitives()->At((cscId<7?cscId-1:cscId-2));
		pad_->SetLeftMargin(0.15);
		hTF_CSCBX[cscId].Draw();
	}

	hTF_ModeProblem.SetXTitle("Track ID");
	hTF_ModeProblem.SetYTitle("Error rate");
	hTF_ModeProblem.SetFillColor(15);
	hTF_ModeProblem.SetStats(false);
	cnv4.cd();
	hTF_ModeProblem.Draw();

	cnv5.Divide(2,2);

	hTF_bxTrack.SetXTitle("Number of tracks");
	hTF_bxTrack.SetYTitle("Events");
	hTF_bxTrack.SetFillColor(15);
	hTF_bxTrack.SetStats(false);
	hTF_bxTrack.GetXaxis()->SetLabelSize(0.05);
	hTF_bxTrack.GetYaxis()->SetLabelSize(0.05);
	hTF_bxTrack.GetXaxis()->SetTitleSize(0.05);
	hTF_bxTrack.GetYaxis()->SetTitleSize(0.05);
	hTF_bxTrack.GetXaxis()->SetTitleOffset(0.9);
	hTF_bxTrack.GetYaxis()->SetTitleOffset(1.2);
	hTF_bxTrack.GetXaxis()->SetNdivisions(510);
	cnv5.cd(1);
	((TVirtualPad*)cnv5.GetListOfPrimitives()->At(0))->SetLeftMargin(0.13);
	hTF_bxTrack.Draw();

	hTF_bxFirstTrack.SetXTitle("First Tracks");
	hTF_bxFirstTrack.SetYTitle("BX");
	hTF_bxFirstTrack.SetFillColor(15);
	hTF_bxFirstTrack.SetStats(false);
	hTF_bxFirstTrack.GetXaxis()->SetLabelSize(0.05);
	hTF_bxFirstTrack.GetYaxis()->SetLabelSize(0.05);
	hTF_bxFirstTrack.GetXaxis()->SetTitleSize(0.05);
	hTF_bxFirstTrack.GetYaxis()->SetTitleSize(0.05);
	hTF_bxFirstTrack.GetXaxis()->SetTitleOffset(0.9);
	hTF_bxFirstTrack.GetYaxis()->SetTitleOffset(1.2);
	hTF_bxFirstTrack.GetXaxis()->SetNdivisions(510);
	cnv5.cd(2);
	((TVirtualPad*)cnv5.GetListOfPrimitives()->At(1))->SetLeftMargin(0.13);
	hTF_bxFirstTrack.Draw();

	hTF_bxSecondTrack.SetXTitle("Second Tracks");
	hTF_bxSecondTrack.SetYTitle("BX");
	hTF_bxSecondTrack.SetFillColor(15);
	hTF_bxSecondTrack.SetStats(false);
	hTF_bxSecondTrack.GetXaxis()->SetLabelSize(0.05);
	hTF_bxSecondTrack.GetYaxis()->SetLabelSize(0.05);
	hTF_bxSecondTrack.GetXaxis()->SetTitleSize(0.05);
	hTF_bxSecondTrack.GetYaxis()->SetTitleSize(0.05);
	hTF_bxSecondTrack.GetXaxis()->SetTitleOffset(0.9);
	hTF_bxSecondTrack.GetYaxis()->SetTitleOffset(1.2);
	hTF_bxSecondTrack.GetXaxis()->SetNdivisions(510);
	cnv5.cd(3);
	((TVirtualPad*)cnv5.GetListOfPrimitives()->At(2))->SetLeftMargin(0.13);
	hTF_bxSecondTrack.Draw();

	hTF_bxThirdTrack.SetXTitle("Third Tracks");
	hTF_bxThirdTrack.SetYTitle("BX");
	hTF_bxThirdTrack.SetFillColor(15);
	hTF_bxThirdTrack.SetStats(false);
	hTF_bxThirdTrack.GetXaxis()->SetLabelSize(0.05);
	hTF_bxThirdTrack.GetYaxis()->SetLabelSize(0.05);
	hTF_bxThirdTrack.GetXaxis()->SetTitleSize(0.05);
	hTF_bxThirdTrack.GetYaxis()->SetTitleSize(0.05);
	hTF_bxThirdTrack.GetXaxis()->SetTitleOffset(0.9);
	hTF_bxThirdTrack.GetYaxis()->SetTitleOffset(1.2);
	hTF_bxThirdTrack.GetXaxis()->SetNdivisions(510);
	cnv5.cd(4);
	((TVirtualPad*)cnv5.GetListOfPrimitives()->At(3))->SetLeftMargin(0.13);
	hTF_bxThirdTrack.Draw();

	cnv6.Divide(1,2);

	hTF_nTracks.SetXTitle("Events");
	hTF_nTracks.SetYTitle("Number of tracks");
	hTF_nTracks.SetFillColor(15);
	hTF_nTracks.SetStats(false);
	hTF_nTracks.GetXaxis()->SetLabelSize(0.05);
	hTF_nTracks.GetYaxis()->SetLabelSize(0.05);
	hTF_nTracks.GetXaxis()->SetTitleSize(0.05);
	hTF_nTracks.GetYaxis()->SetTitleSize(0.05);
	hTF_nTracks.GetXaxis()->SetTitleOffset(0.9);
	hTF_nTracks.GetYaxis()->SetTitleOffset(1.);
	hTF_nTracks.GetXaxis()->SetNdivisions(510);
	cnv6.cd(1);
	//((TVirtualPad*)cnv6.GetListOfPrimitives()->At(0))->SetLeftMargin(0.13);
	hTF_nTracks.Draw();

	//hTF_SEQ.SetXTitle("Situation");
	hTF_SEQ.SetYTitle("Number of cases");
	hTF_SEQ.SetFillColor(15);
	hTF_SEQ.SetStats(false);
	hTF_SEQ.GetXaxis()->SetLabelSize(0.07);
	hTF_SEQ.GetYaxis()->SetLabelSize(0.05);
	hTF_SEQ.GetXaxis()->SetTitleSize(0.07);
	hTF_SEQ.GetYaxis()->SetTitleSize(0.05);
	hTF_SEQ.GetXaxis()->SetTitleOffset(0.9);
	hTF_SEQ.GetYaxis()->SetTitleOffset(1.);
	hTF_SEQ.GetXaxis()->SetNdivisions(505);
	hTF_SEQ.GetXaxis()->SetBinLabel(1,"Consequent multiple LCTs");
	hTF_SEQ.GetXaxis()->SetBinLabel(2,"Multiple LCTs in same BX");
	hTF_SEQ.GetXaxis()->SetBinLabel(3,"LCTs in neighbour BXs");

	//hTF_bxaEff.SetXTitle("Situation");
	hTF_bxaEff.SetYTitle("Efficiency [%]");
	hTF_bxaEff.SetFillColor(15);
	hTF_bxaEff.SetStats(false);
	hTF_bxaEff.GetXaxis()->SetLabelSize(0.07);
	hTF_bxaEff.GetYaxis()->SetLabelSize(0.05);
	hTF_bxaEff.GetXaxis()->SetTitleSize(0.07);
	hTF_bxaEff.GetYaxis()->SetTitleSize(0.05);
	hTF_bxaEff.GetXaxis()->SetTitleOffset(0.9);
	hTF_bxaEff.GetYaxis()->SetTitleOffset(1.);
	hTF_bxaEff.GetXaxis()->SetNdivisions(505);
	hTF_bxaEff.GetXaxis()->SetBinLabel(1,"ID info is ambigious");
	hTF_bxaEff.GetXaxis()->SetBinLabel(2,"Both LCTs in same BX");
	hTF_bxaEff.GetXaxis()->SetBinLabel(3,"LCTs in neighbour BXs");
	((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->Divide(2,1);
	((TVirtualPad*)((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->GetListOfPrimitives()->At(0))->SetBottomMargin(0.2);
	((TVirtualPad*)((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->GetListOfPrimitives()->At(0))->SetRightMargin(0.2);
	((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->cd(1);
	hTF_SEQ.Draw();
	((TVirtualPad*)((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->GetListOfPrimitives()->At(1))->SetBottomMargin(0.2);
	((TVirtualPad*)((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->GetListOfPrimitives()->At(1))->SetRightMargin(0.2);
	((TVirtualPad*)((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->GetListOfPrimitives()->At(1))->SetLogy();
	((TVirtualPad*)cnv6.GetListOfPrimitives()->At(1))->cd(2);
	hTF_bxaEff.Draw();

	refreshCnt = 0;
	seqAmbigious = 0;

	bzero(Quality,sizeof(Quality));
	bzero(StripID,sizeof(StripID));
	bzero(ClctPattern,sizeof(ClctPattern));
	bzero(WireGroup,sizeof(WireGroup));
	bzero(BX,sizeof(BX));
	bzero(Bits,sizeof(Bits));

	bzero(hTF_Quality,sizeof(void*)*11);//sizeof(hTF_Quality));
	bzero(hTF_halfStripID,sizeof(void*)*11);//,sizeof(hTF_halfStripID));
	bzero(hTF_diStripID,sizeof(void*)*11);//,sizeof(hTF_diStripID));
	bzero(hTF_StripID,sizeof(void*)*11);//,sizeof(hTF_StripID));
	bzero(hTF_ClctPattern,sizeof(void*)*11);//,sizeof(hTF_ClctPattern));
	bzero(hTF_WireGroup,sizeof(void*)*11);//,sizeof(hTF_WireGroup));
	bzero(hTF_BX,sizeof(void*)*11);//,sizeof(hTF_BX));
	bzero(hTF_bc0,sizeof(void*)*11);//,sizeof(hTF_bc0));
	bzero(hTF_bx0,sizeof(void*)*11);//,sizeof(hTF_bx0));
	bzero(hTF_lr,sizeof(void*)*11);//,sizeof(hTF_lr));
	bzero(hTF_vp,sizeof(void*)*11);//,sizeof(hTF_vp));

	bzero(cntCSC,sizeof(cntCSC));
	bookCSC(0);
	bookCSC(1);
	bookCSC(2);
	bookCSC(3);
	bookCSC(4);
	bookCSC(5);
	bookCSC(6);
	bookCSC(7);
	bookCSC(8);
	bookCSC(9);
	bookCSC(10);
	bookCSC(11);
	bookCSC(12);
	bookCSC(13);
	bookCSC(14);
	bookCSC(15);

	TrackGeom = new TCanvas("TrackGeom","TrackGeom");
	TrackGeom->Divide(1,2);
	TrackGeom->cd(1); hTF_TrkPhi.Draw();
	TrackGeom->cd(2); hTF_TrkEta.Draw();
	TrackPt = new TCanvas("TrackPt","TrackPt");
	hTF_Pt.Draw();

	hTF_TrkPhi.SetFillColor(15);
	hTF_TrkPhi.SetStats(false);
	hTF_TrkPhi.SetXTitle("#phi");
	//hTF_TrkPhi.SetYTitle("");
	hTF_TrkPhi.GetXaxis()->SetLabelSize(0.05);
	hTF_TrkPhi.GetYaxis()->SetLabelSize(0.05);
	hTF_TrkPhi.GetXaxis()->SetTitleSize(0.05);
	hTF_TrkPhi.GetYaxis()->SetTitleSize(0.05);
	hTF_TrkPhi.GetXaxis()->SetTitleOffset(0.9);
	hTF_TrkPhi.GetYaxis()->SetTitleOffset(1.2);

	hTF_TrkEta.SetFillColor(15);
	hTF_TrkEta.SetStats(false);
	hTF_TrkEta.SetXTitle("#eta");
	//hTF_TrkEta.SetYTitle("");
	hTF_TrkEta.GetXaxis()->SetLabelSize(0.05);
	hTF_TrkEta.GetYaxis()->SetLabelSize(0.05);
	hTF_TrkEta.GetXaxis()->SetTitleSize(0.05);
	hTF_TrkEta.GetYaxis()->SetTitleSize(0.05);
	hTF_TrkEta.GetXaxis()->SetTitleOffset(0.9);
	hTF_TrkEta.GetYaxis()->SetTitleOffset(1.2);

	hTF_Pt.SetFillColor(15);
	hTF_Pt.SetStats(false);
	hTF_Pt.SetXTitle("p_{T}");
	//hTF_Pt.SetYTitle("");
	hTF_Pt.GetXaxis()->SetLabelSize(0.05);
	hTF_Pt.GetYaxis()->SetLabelSize(0.05);
	hTF_Pt.GetXaxis()->SetTitleSize(0.05);
	hTF_Pt.GetYaxis()->SetTitleSize(0.05);
	hTF_Pt.GetXaxis()->SetTitleOffset(0.9);
	hTF_Pt.GetYaxis()->SetTitleOffset(1.2);
}

void EmuSPMonitor::bookCanvas(TCanvas* &canvas, const char* name, const char* title, int cscId) const {
	if( canvas ){
		canvas->Clear();
	} else {
		char _name[1000], __name[1000], _title[1000];
		sprintf(__name,name,cscId);
		sprintf(_name,"CSC%d/%s",cscId,__name);
		sprintf(_title,title,cscId);
		canvas = new TCanvas(_name,_title);
	}
}

void EmuSPMonitor::book2Dhist(TH1F* &hist, const char* name, const char* title, int nBins, int max, int min, int cscId /*, ...*/) const {
	//va_list ap;
	//va_start(ap,title);
	//strrchr(title,'%')+1;
	//int   arg1 = va_arg(ap,int);
	//char* arg2 = va_arg(ap,char*);
	//va_end(ap);
	if( hist ){
		hist->Clear();
	} else {
		char _name[1000], _title[1000];
		sprintf(_name,name,cscId);
		sprintf(_title,title,cscId);
		hist = new TH1F(_name,_title,nBins,max,min);
	}
	hist->SetFillColor(15);
	hist->SetStats(false);
	//gStyle->SetOptStat("e");
	hist->GetXaxis()->SetLabelSize(0.05);
	hist->GetYaxis()->SetLabelSize(0.05);
	hist->GetXaxis()->SetTitleSize(0.05);
	hist->GetYaxis()->SetTitleSize(0.05);
	hist->GetXaxis()->SetTitleOffset(0.9);
	hist->GetYaxis()->SetTitleOffset(1.2);
	//hist->GetXaxis()->SetNdivisions(520);
}

bool EmuSPMonitor::bookCSC(int cscId){
	if( cscId<1 || cscId>10 ) return false;
	bookCanvas(    Quality[cscId],"Quality_CSC%d", "LCT Quality for CSC%d",cscId);
	book2Dhist(hTF_Quality[cscId],"hTF_Quality_CSC%d", "LCT Quality",16,0,16,cscId);

	Quality[cscId]->cd(); hTF_Quality[cscId]->Draw();
	bookCanvas(        StripID[cscId],"StripID_CSC%d","StripID for CSC%d",cscId);
	book2Dhist(hTF_halfStripID[cscId],"hTF_halfStripID_CSC%d","Half-Strip",160,0,160,cscId);
	book2Dhist(  hTF_diStripID[cscId],"hTF_diStripID_CSC%d","Di-Strip",160,0,160,cscId);
	book2Dhist(    hTF_StripID[cscId],"hTF_StripID%d","Raw data field of CLCT Pattern ID",256,0,256,cscId);
	StripID[cscId]->Divide(1,2);
	StripID[cscId]->cd(1); hTF_halfStripID[cscId]->Draw(); hTF_halfStripID[cscId]->GetXaxis()->SetLabelSize(0.08); hTF_halfStripID[cscId]->GetYaxis()->SetLabelSize(0.08);
	StripID[cscId]->cd(2); hTF_diStripID[cscId]->Draw();   hTF_diStripID[cscId]->GetXaxis()->SetLabelSize(0.08);   hTF_diStripID[cscId]->GetYaxis()->SetLabelSize(0.08);
	//StripID[cscId]->cd(3); hTF_StripID[cscId]->Draw();     hTF_StripID[cscId]->GetXaxis()->SetLabelSize(0.08);     hTF_StripID[cscId]->GetYaxis()->SetLabelSize(0.08);

	bookCanvas(    ClctPattern[cscId],"ClctPattern_CSC%d","CLCT Pattern # for CSC%d",cscId);
	book2Dhist(hTF_ClctPattern[cscId],"hTF_ClctPattern_CSC%d","CLCT Pattern #",8,0,8,cscId);
	ClctPattern[cscId]->cd(); hTF_ClctPattern[cscId]->Draw();

	bookCanvas(    WireGroup[cscId],"WireGroup_CSC%d","Wire Group for CSC%d",cscId);
	book2Dhist(hTF_WireGroup[cscId],"hTF_WireGroup_CSC%d","Wire Group",128,0,128,cscId);
	WireGroup[cscId]->cd(); hTF_WireGroup[cscId]->Draw();

	//bookCanvas(    BX[cscId],"BX_CSC%d","BX for CSC%d",cscId);
	//book2Dhist(hTF_BX[cscId],"hTF_BX_CSC%d","BX",7,0,7,cscId);
	//BX[cscId].cd(); hTF_BX[cscId]->Draw();

	bookCanvas(   Bits[cscId],"Bits_CSC%d","Wire Group for CSC%d", cscId);
	book2Dhist(hTF_vp [cscId],"hTF_vp_CSC%d","Valid Pattern",2,0,2,cscId);
	book2Dhist(hTF_bc0[cscId],"hTF_bc0_CSC%d","BC0",2,0,2,cscId);
	book2Dhist(hTF_bx0[cscId],"hTF_bx0_CSC%d","BX0",2,0,2,cscId);
	book2Dhist(hTF_lr [cscId],"hTF_lr_CSC%d","L/R",2,0,2,cscId);
	Bits[cscId]->Divide(2,2);
	Bits[cscId]->cd(1); hTF_vp [cscId]->SetMinimum(0); hTF_vp [cscId]->Draw(); hTF_vp [cscId]->GetXaxis()->SetNdivisions(503); hTF_vp [cscId]->GetXaxis()->SetLabelSize(0.08); hTF_vp [cscId]->GetYaxis()->SetLabelSize(0.08);
	Bits[cscId]->cd(2); hTF_bc0[cscId]->SetMinimum(0); hTF_bc0[cscId]->Draw(); hTF_bc0[cscId]->GetXaxis()->SetNdivisions(503); hTF_bc0[cscId]->GetXaxis()->SetLabelSize(0.08); hTF_bc0[cscId]->GetYaxis()->SetLabelSize(0.08);
	Bits[cscId]->cd(3); hTF_bx0[cscId]->SetMinimum(0); hTF_bx0[cscId]->Draw(); hTF_bx0[cscId]->GetXaxis()->SetNdivisions(503); hTF_bx0[cscId]->GetXaxis()->SetLabelSize(0.08); hTF_bx0[cscId]->GetYaxis()->SetLabelSize(0.08);
	Bits[cscId]->cd(4); hTF_lr [cscId]->SetMinimum(0); hTF_lr [cscId]->Draw(); hTF_lr [cscId]->GetXaxis()->SetNdivisions(503); hTF_lr [cscId]->GetXaxis()->SetLabelSize(0.08); hTF_lr [cscId]->GetYaxis()->SetLabelSize(0.08);
}
/*
void EmuSPMonitor::fill(const unsigned short* data){
	SRSPEventData* TFevent = new SRSPEventData(const_cast<unsigned short*>(data));

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
				hTF_L1As.Fill(lct->getCSCID());
				hTF_CSCBX[lct->getCSCID()].Fill(bx);
				if( lct->getQuality()==4 || lct->getQuality()==5 ) {
					hTF_LCTs .Fill(lct->getCSCID());
					hTF_CLCTs.Fill(lct->getCSCID());
				} else
				if( lct->getQuality()==1 || lct->getQuality()==3 ) {
					hTF_ALCTs.Fill(lct->getCSCID());
				} else hTF_CORLCTs.Fill(lct->getCSCID());
				lct++;
			}
			sr++;
		}
	}
	if( refreshCnt++ % 100 ){
		//hTF_LCTs.Sumw2();
		//hTF_L1As.Sumw2();
		hTF_Eff.Divide(&hTF_LCTs,&hTF_L1As);
	}
	hTF_L1AvsBX.Fill(TFevent->spVMEHeader().getLvl1num(),TFevent->spVMEHeader().numBX());
	//hTF_L1AvsBX.Fill(TFevent->eventHeader().getLvl1num(),TFevent->eventHeader().getBXnum());
}
*/
void EmuSPMonitor::fill(const unsigned short* data){
	L1MuCSCEventData SRSPEvent(const_cast<unsigned short*>(data));
	// Cycle over 7 BXs of SP window ( relative bx )

	int nTracks=0;
	L1MuCSCCorrelatedLCT LCTs_cur[15], LCTs_prev[15];
	for(int bx=0; bx<7; bx++){
		const unsigned short IDcode[15] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x8, 0x10, 0x18, 0x20, 0x40, 0x60, 0x80, 0x100, 0x180 };
		const unsigned short IDmask[15] = { 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x18,0x18, 0x18, 0x60, 0x60, 0x60, 0x180,0x180, 0x180 };

		unsigned short ME_IDs[3] = { 0, 0, 0 };
		for(int trk=0; trk<SRSPEvent.spDatum(bx+1).spData().size(); trk++){
			ME_IDs[trk] = SRSPEvent.spDatum(bx+1).spData()[trk].trackId();
			switch( nTracks ){
				case 0 : hTF_bxFirstTrack. Fill(bx); break;
				case 1 : hTF_bxSecondTrack.Fill(bx); break;
				case 2 : hTF_bxThirdTrack. Fill(bx); break;
				default: break;
			}
			hTF_bxTrack.Fill(bx);
			nTracks++;

			hTF_TrkPhi.Fill(SRSPEvent.spDatum(bx+1).spData()[trk].phiPacked());
			hTF_TrkEta.Fill(SRSPEvent.spDatum(bx+1).spData()[trk].etaPacked());
			if( SRSPEvent.spDatum(bx+1).spData()[trk].ptData() )
				hTF_Pt.Fill(SRSPEvent.spDatum(bx+1).spData()[trk].ptData()->ptLUT(SRSPEvent.spDatum(bx+1).spData()[trk].frPacked()));
		}

		unsigned short vpbits = 0;
		for(unsigned int sr=0; sr<SRSPEvent.frontDatum(bx+1).frontData().size(); sr++){
			// Cycle over received LCTs
			vector<L1MuCSCCorrelatedLCT> lctData = SRSPEvent.frontDatum(bx+1).lctData(sr+1);
			for(unsigned int link=0; link<lctData.size(); link++){
				// Main cycle begins
				L1MuCSCCorrelatedLCT lct = lctData[link];
				// Fill general hists with input values
				if(lct.getStripType()) hTF_halfStripID[lct.getCSCID()]->Fill( lct.getStrip() );
				else                     hTF_diStripID[lct.getCSCID()]->Fill( lct.getStrip() );
				hTF_StripID[lct.getCSCID()]->Fill( 0 );
				if( lct.getQuality()!=4 && lct.getQuality()!=5 )
					hTF_WireGroup[lct.getCSCID()]->Fill( lct.getWireGang() );
                else if( lct.getWireGang() ) cerr<<"Error (1)"<<endl;
				hTF_Quality    [lct.getCSCID()]->Fill( lct.getQuality() );
				hTF_ClctPattern[lct.getCSCID()]->Fill( lct.getCathodePattern() );
				hTF_bc0        [lct.getCSCID()]->Fill( lct.getAnodeBX()>>1  );
				hTF_bx0        [lct.getCSCID()]->Fill( lct.getAnodeBX()&0x1 );
				hTF_lr         [lct.getCSCID()]->Fill( lct.getBend() );
				//hTF_vp         [lct.getCSCID()]->Fill( lct.isValid() );

				//
				cntCSC[lct.getCSCID()]++;

				// Geographical position of the link
				unsigned int glink = (SRSPEvent.frontDatum(bx+1).frontData(sr+1,link+1).getMPC()-1)*3+link;
				if( glink>14 ){ cout<<"KARAUL!!!"<<endl; continue; }
				// Save this lct for further needs
				LCTs_cur[glink] = lct;
				// Make bit array of LCTs from Front Block ( should match VP word in Front Header
				vpbits |= 1<<((SRSPEvent.frontDatum(bx+1).frontData(sr+1,link+1).getMPC()-1)*3+link);
				// Try to skip all LCTs haven't caused a Track
				if( (ME_IDs[0]&IDmask[glink])!=IDcode[glink] && (ME_IDs[1]&IDmask[glink])!=IDcode[glink] && (ME_IDs[2]&IDmask[glink])!=IDcode[glink] ) continue;

				//
				hTF_L1As.Fill(lct.getCSCID());
				if( lct.getQuality()!=4 && lct.getQuality()!=5 && lct.getQuality()>7 )
					hTF_CSCBX[lct.getCSCID()].Fill(bx);
				if( lct.getQuality()==4 || lct.getQuality()==5 ) {
					hTF_CLCTs.Fill(lct.getCSCID());
				} else
				if( lct.getQuality()==1 || lct.getQuality()==3 ) {
					hTF_ALCTs.Fill(lct.getCSCID());
				} else hTF_CORLCTs.Fill(lct.getCSCID());
				hTF_LCTs.Fill(lct.getCSCID());
			}
		}

		bool cur1=false, cur2=false, prev=false;
		for(int lnk=0; lnk<15; lnk++){
			if( LCTs_cur [lnk].isValid() ) if( cur1 ){ cur2=true; } else { cur1=true; }
			if( LCTs_prev[lnk].isValid() ) prev=true;
			if( LCTs_cur [lnk].isValid() && LCTs_prev[lnk].isValid() ) seqAmbigious++;
		}
		if( cur1 && cur2 && !prev ) hTF_SEQ.Fill(0);
		if( cur1 && prev && !cur2 ) hTF_SEQ.Fill(1);
		if( cur1 && prev &&  cur2 ) hTF_SEQ.Fill(-1);

		for(int trk=0; trk<3; trk++){
			bool curBXA=false, prevBXA=false, skip=false;
			for(int lnk=0; lnk<15; lnk++)
				if( (ME_IDs[trk] & IDmask[lnk]) == IDcode[lnk] ){
					if( LCTs_cur[lnk].isValid() && LCTs_prev[lnk].isValid() ){
						//cout<<"KARAUL"<<endl;
						skip=true;
					} else {
						if( LCTs_cur [lnk].isValid() ) curBXA =true;
						if( LCTs_prev[lnk].isValid() ) prevBXA=true;
					}
				}

			if(  skip ) hTF_BXA.Fill(-1);
			if( !skip &&  curBXA && !prevBXA ) hTF_BXA.Fill(0);
			if( !skip &&  curBXA &&  prevBXA ) hTF_BXA.Fill(1);
		}
		for(int lnk=0; lnk<15; lnk++) LCTs_prev[lnk] = LCTs_cur[lnk];

		for(int trk=0; trk<SRSPEvent.spDatum(bx+1).spData().size(); trk++)
			if( SRSPEvent.spDatum(bx+1).spData()[trk].spTrack().trackId().mode() !=
				SRSPEvent.spDatum(bx+1).spHeader().getTrackMode(trk+1) ) hTF_ModeProblem.Fill(trk);


		for(unsigned int frontFPGA=0; frontFPGA<5; frontFPGA++)
			for(unsigned int link=0; link<3; link++)
				if( bool(vpbits&(1<<(frontFPGA*3+link))) != bool(SRSPEvent.frontDatum(bx+1).frontHeader().getVPBit(frontFPGA+1,link+1)) ) hTF_VP.Fill((double)0,frontFPGA*3+link);
	}

	if( refreshCnt++ % 100 ){
		//hTF_LCTs.Sumw2();
		//hTF_L1As.Sumw2();
		hTF_Eff.Divide(&hTF_CORLCTs,&hTF_L1As);
		hTF_bxaEff.SetBinContent(1,hTF_BXA.GetBinContent(1)*100./float(seqAmbigious));
		hTF_bxaEff.SetBinContent(2,hTF_BXA.GetBinContent(2)*100./hTF_SEQ.GetBinContent(2));
		hTF_bxaEff.SetBinContent(3,hTF_BXA.GetBinContent(3)*100./hTF_SEQ.GetBinContent(3));
	}
	hTF_L1AvsBX.Fill(SRSPEvent.eventHeader().getLvl1num()%128,SRSPEvent.eventHeader().getBXnum()%128);
	hTF_nTracks.Fill(nTracks);

}

list<TCanvas*> EmuSPMonitor::getListOfCanvases(void){
	list<TCanvas*> retval;
	retval.push_back(&cnv1);
	retval.push_back(&cnv2);
	retval.push_back(&cnv3);
	retval.push_back(&cnv4);
	retval.push_back(&cnv5);
	retval.push_back(&cnv6);
	retval.push_back(TrackGeom);
	retval.push_back(TrackPt);

	for(int cscId=0; cscId<11; cscId++)
		if( cntCSC[cscId] ){
			//cout<<"Found: "<<cscId<<"("<<
				//cntCSC[cscId]<<") "<<(void*)Quality[cscId]<<" "<<(void*)StripID[cscId]<<
				//" "<<(void*)ClctPattern[cscId]<<" "<<(void*)WireGroup[cscId]<<
				///*" 0x"<<(void*)BX[cscId]<<*/" "<<(void*)Bits[cscId]<<endl;
			retval.push_back(Quality[cscId]);
			retval.push_back(StripID[cscId]);
			retval.push_back(ClctPattern[cscId]);
			retval.push_back(WireGroup[cscId]);
			//retval.push_back(BX[cscId]);
			retval.push_back(Bits[cscId]);
		}

	for(int cscId=1; cscId<11; cscId++){
		TVirtualPad *pad = (TVirtualPad*)cnv3.GetListOfPrimitives()->At((cscId<7?cscId-1:cscId-2));
		if( hTF_CSCBX[cscId].GetEntries() ) pad->SetLogy();
	}

	if( hTF_nTracks.GetEntries() ) cnv6.SetLogy();
	if( hTF_bxTrack.GetEntries() ) ((TVirtualPad*)cnv5.GetListOfPrimitives()->At(0))->SetLogy();
	if( hTF_bxFirstTrack .GetEntries() ) ((TVirtualPad*)cnv5.GetListOfPrimitives()->At(1))->SetLogy();
	if( hTF_bxSecondTrack.GetEntries() ) ((TVirtualPad*)cnv5.GetListOfPrimitives()->At(2))->SetLogy();
	if( hTF_bxThirdTrack. GetEntries() ) ((TVirtualPad*)cnv5.GetListOfPrimitives()->At(3))->SetLogy();

	return retval;
}
