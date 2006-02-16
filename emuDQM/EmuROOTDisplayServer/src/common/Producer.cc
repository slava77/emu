//-------------------------------------------------------------------------
// File: Producer.cc
//
// This programm creates some histograms and sends them to the Display Server
// via ROOT sockets.
// The idea behind this program is to provide a stand alone test source for
// monitoring objects which does not depend on AC++ modules.
//
// To run this demo do the following:
//   - Start Producer such that histograms are produced and sent to the 
//     Display Server.
//   - The Server process is started by the Producer.
//   - Run HistoDisplayMain to open a socket connection to the Display Server 
//     and display the histograms.
//
//  Author list:
// 
//  Hans  Wenzel      IEKP Karlsruhe, now FNAL
//  Frank Hartmann    IEKP Karlsruhe
//  Hartmut Stadie    IEKP Karlsruhe 
//  Wolfgang Wagner   IEKP Karlsruhe
//
//-------------------------------------------------------------------------

#if defined( IRIX6_2 )
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE_EXTENDED==1)
//    static pid_t vfork (void); //hack hst 
#define __vfork vfork
#endif
#endif


//-------------------------------------------------------------------------
#include <iostream>

// Includes from ROOT
#include "TApplication.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TSocket.h"
#include "TServerSocket.h"
#include "TMath.h"
#include "TMessage.h"
#include "TMonitor.h"
#include "TFile.h"
#include "TCanvas.h"

// Includes from CDF offline
#include "TConsumerInfo.hh"
#include "ConsumerExport.hh"

#include "ConsumerCanvas.hh"

extern void InitGui(); 
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TROOT producer("producer","Simple histogram producer",initfuncs);

static Bool_t loopContinue=1;

extern "C" 
{
  void stoploop(int sig)
  {
    signal(SIGABRT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTRAP, SIG_DFL);
    signal(SIGKILL, SIG_DFL);
    std::cout << "Producer: stoploop(): ending loop with signal " << sig << std::endl;
    loopContinue=0; 
  } 
}


int main (int argc,char**argv)
{
  const int updatenum=5;
  const int printSupp=5;
  const int runNumber=1530; 
  //const int testAddNum = 300;
  const int testAddNum = 150;
  //const int testRemoveNum =600;
  const int testRemoveNum =300;
  const int testRemoveAfterAdd = 450; 
  const int s2nHistNum=15;
  const int histPerCanvas=4; 
  const int maxevnt=2000;
  const int maxPipeCell=46;
  const int maxChannel=640; 
  char histname[80];

  signal(SIGABRT, stoploop);
  signal(SIGTERM, stoploop);
  signal(SIGINT, stoploop);
  signal(SIGSEGV, stoploop);
  signal(SIGTRAP, stoploop);
  signal(SIGKILL, stoploop);


  gROOT->SetBatch(kTRUE);
  TApplication app("app",&argc,argv);
  // TMath locMath;
  ConsumerExport consExp(9050);

  //-----------------------------------------------------------------------
  TConsumerInfo *consinfo = new TConsumerInfo("Producer", runNumber);
  //-----------------------------------------------------------------------

  //-----------------------------------------------------------------------
  // Create  1d and  2d  histograms. These objects will
  // be automatically added to the current directory.
  Double_t **noiseArray1;  // Arrays for noise following
  Double_t **noiseArray2; 
  TH1F *histo1; 
  TH1F *histo2;
  TH2F *histo3;
  TH2F* noiseHist1 = 0;
  TH2F* noiseHist2 = 0;
  TH1F **s2nhistos;
  TCanvas **canvases;
  TCanvas* noiseCanvas1 = 0;
  TCanvas* noiseCanvas2 = 0;

  noiseArray1 = new Double_t*[maxChannel];
  noiseArray2 = new Double_t*[maxChannel];
  for (int iChan=0; iChan<maxChannel; iChan++) {
    noiseArray1[iChan] = new Double_t[maxPipeCell];
    noiseArray2[iChan] = new Double_t[maxPipeCell];
    // Initialize all this
    for (int iCell=0; iCell<maxPipeCell; iCell++) {
      noiseArray1[iChan][iCell] = 0.0;
    }
  }

  histo1 = new TH1F("Muon_Energy","Muon Energy",50,0,30);
  histo1->SetXTitle("muon energy");
  histo1->SetYTitle("event count");
  histo1->SetFillColor(48);
  
  histo2 = new TH1F("Muon_Pt","Muon Pt",50,0,30);
  histo2->SetXTitle("muon pt");
  histo2->SetYTitle("event count");
  histo2->SetFillColor(48);
  
  histo3 = new TH2F("Muon_py_vs_px","Muon py vs px",50,0,30,50,0,30);
  histo3->SetXTitle("muon px");
  histo3->SetYTitle("muon py");  

  TH2F* histo4 = new TH2F("Muon_py_vs_pxLarge","Muon py vs px",1000,0,30,1000,0,30);
  histo4->SetXTitle("muon px");
  histo4->SetYTitle("muon py");  

  consinfo->addObject("Muon_Energy", "Producer/Muon", 0, histo1);

  // test for nested canvases
  ConsumerCanvas *nestedtestcanv = new ConsumerCanvas("MuonCanv","Muon ConsumerCanvas","Muons",2,2);
  nestedtestcanv->cd(1);
  histo1->Draw();
  nestedtestcanv->cd(2);
  histo2->Draw();
  nestedtestcanv->cd(3);
  histo3->Draw();
  nestedtestcanv->cd(4);
  histo4->Draw();
  consinfo->addObject("MuonCanv","SlideShow",0,nestedtestcanv);
  consinfo->addObject("MuonCanv","Producer/Muon",0,nestedtestcanv);

  s2nhistos = new TH1F*[s2nHistNum];
  for (int ihist=0; ihist < s2nHistNum; ihist++) {
    sprintf(histname, "S2N_Barrel_%d", (400+ihist));
    s2nhistos[ihist] = new TH1F(histname, histname, 101, -0.5 , 100.5);
    if (ihist%2 == 0)  s2nhistos[ihist]->SetFillColor(3);
    else               s2nhistos[ihist]->SetFillColor(7);  
    consinfo->addObject(histname, "Producer/S2N", 0, s2nhistos[ihist]);
    s2nhistos[ihist]->SetXTitle("S/N");
    s2nhistos[ihist]->SetTitleOffset(1.4, "X");
  }

  // Calculate number of canvases needed
  Int_t nCanvas = s2nHistNum/histPerCanvas;
  if (nCanvas*histPerCanvas < s2nHistNum) nCanvas++;  

  canvases = new TCanvas*[nCanvas];
  for (Int_t iCan=0; iCan<nCanvas; iCan++) {
    sprintf(histname, "S2N_Canvas_%d", iCan);
    canvases[iCan] = new TCanvas(histname,histname,200,10,700,500);
    consinfo->addObject(histname, "SlideShow", 0, canvases[iCan]);
    canvases[iCan]->Divide(2,2);    // Carefull, hardcoded ! Clean this up later!
    canvases[iCan]->cd();
    for (Int_t iPad=0; iPad<histPerCanvas; iPad++) {
      canvases[iCan]->cd(iPad+1);
      Int_t ihist = iCan*histPerCanvas+iPad;
      if (ihist < s2nHistNum) s2nhistos[ihist]->Draw();
    } 
  }

  Float_t s2nMean[s2nHistNum];
  for (Int_t iHist=0; iHist<s2nHistNum; iHist++) {
    s2nMean[iHist] = (Float_t) gRandom->Gaus(25.0, 5.0);
  }

  // Define 2 big noise histograms
  sprintf(histname, 
  "Noise: cell number versus channel number, SVD 2c, Module ID: B3-067X-498");
  noiseHist1 = new TH2F (histname, histname, maxChannel, 0, (maxChannel-1),
                         maxPipeCell, 0, (maxPipeCell-1));
  consinfo->addObject(histname, "Producer/Noise", 0, noiseHist1);  
  noiseHist1->SetOption("COLZ");
  noiseHist1->SetStats(0);
  noiseHist1->SetXTitle("Channel No.");
  noiseHist1->SetYTitle("Cell No.");
  noiseHist1->SetTitleOffset(1.4, "X");
  noiseHist1->SetTitleOffset(1.4, "Y");

  sprintf(histname, 
  "Module ID: B3-067X-321: Noise: cell number versus channel number, SVD 2c");
  noiseHist2 = new TH2F (histname, histname, maxChannel, 0, (maxChannel-1), 
                         maxPipeCell, 0, (maxPipeCell-1));
  consinfo->addObject(histname, "Producer/Noise", 0, noiseHist2);
  noiseHist2->SetOption("COLZ"); 
  noiseHist2->SetStats(0);
  noiseHist2->SetXTitle("Channel No.");
  noiseHist2->SetYTitle("Cell No.");
  noiseHist2->SetTitleOffset(1.4, "X");
  noiseHist2->SetTitleOffset(1.4, "Y");
  
  // Define 2 big Canvases 
  sprintf(histname, "NoiseCanvas_1");
  noiseCanvas1 = new TCanvas(histname,histname,200,10,1000,700);
  consinfo->addObject(histname, "Producer/SlideShow", 0, noiseCanvas1);
  noiseCanvas1->cd();
  noiseHist1->Draw("COLZ");
  sprintf(histname, "NoiseCanvas_2");
  noiseCanvas2 = new TCanvas(histname,histname,200,10,1000,700);
  consinfo->addObject(histname, "Producer/Noise", 0, noiseCanvas2);
  noiseCanvas2->cd();
  noiseHist2->Draw("COLZ");
 

  // Define a hot channel and a hot cell list.
  // Doesn't catch the case that a number is selected twice.
  // They are also the same for both ladders.
  Int_t hotChan[10];
  std::cout << "Hot channels: ";
  for (Int_t iHot=0; iHot<10; iHot++) {
    hotChan[iHot] = gRandom->Integer(maxChannel);
    std::cout << hotChan[iHot] << ", ";
  }
  std::cout << std::endl;

  Int_t hotCell[5];
  std::cout << "Hot cells: "; 
  for (Int_t iHot=0; iHot<5; iHot++) {
    hotCell[iHot] = gRandom->Integer(maxPipeCell);
    std::cout << hotCell[iHot] << ", "; 
  }
  std::cout << std::endl;

  //-----------------------------------------------------------------------
  Float_t muon_Energy,muon_Pt,muon_Px,muon_Py;
  Double_t s2n;
  int count = 1;
  // now start std::endless loop
  
  while (loopContinue && (count <= maxevnt)) {
    // get random number for muon px and py:   
    gRandom->Rannor(muon_Px,muon_Py);
    muon_Px     = muon_Px + 10.;
    muon_Py     = muon_Py + 10.;
    muon_Pt     = sqrt(muon_Px*muon_Px+muon_Py*muon_Py);
    muon_Energy = sqrt(0.105*0.105+muon_Pt*muon_Pt);
    histo1->Fill(muon_Energy,1.0);
    histo2->Fill(muon_Pt,1.0);
    histo3->Fill(muon_Px,muon_Py,1.0);
    histo4->Fill(muon_Px,muon_Py);
    //---------------------------------------------------------------
    // Fill random numbers obeying a Landau distribution into the S/N 
    // histograms 
    for (int ihist=0; ihist<s2nHistNum; ihist++) {
      s2n = gRandom->Landau( s2nMean[ihist], 3.0);
      s2nhistos[ihist]->Fill((Float_t)s2n,1.0); 
    }
    //---------------------------------------------------------------
    // Now we slow down the program a bit and do some noise following.
    Double_t omega = 0.0;
    if (count < 250) omega = 1/((Double_t)count); // generic mean
    else             omega = 1/250.0;             // constant weight factor
    for (int iCell=0; iCell<maxPipeCell; iCell++) {
      Bool_t cellIsHot = kFALSE;
      for (int iHotCell=0; iHotCell<5; iHotCell++) { 
        if (hotCell[iHotCell] == iCell) {
          cellIsHot=kTRUE;
          continue;
        }
      }
      for (int iChan=0; iChan<maxChannel; iChan++) {
        Double_t sigma=3.0;
        for (int iHotChan=0; iHotChan<10; iHotChan++) {
          if (hotChan[iHotChan] == iChan) {
            sigma = 20.0;
            continue;
          } 
        }
        if (cellIsHot) sigma += 15.0;  
        Double_t pedSub = 0.0;
        Double_t noise  = 0.0;
        // All this is not optimized for speed. It's just a demo.
        pedSub = gRandom->Gaus(0.0, sigma);
        noise  = TMath::Abs(pedSub);
        noiseArray1[iChan][iCell] = noiseArray1[iChan][iCell] * (1 - omega) 
                                    + noise * omega; 
        pedSub = gRandom->Gaus(0.0, sigma);
        noise  = TMath::Abs(pedSub);
        noiseArray2[iChan][iCell] = noiseArray2[iChan][iCell] * (1 - omega) 
                                    + noise * omega; 
      }
    }  

    gSystem->Sleep(100);   // sleep for 0.1 seconds (don't swamp the system)
      
    if (count % updatenum == 0) {
      noiseHist1->Reset();
      noiseHist2->Reset();
      for (Int_t iChan=0; iChan<maxChannel; iChan++) {
        for (Int_t iCell=0; iCell<maxPipeCell; iCell++) {
          noiseHist1->Fill( (float)iChan, (float)iCell, 
                            (Float_t) noiseArray1[iChan][iCell]);
          noiseHist2->Fill( (float)iChan, (float)iCell, 
                            (Float_t) noiseArray2[iChan][iCell]); 
        }
      }
      // Histograms are sent every <updatenum> events.
      // You might want to change this when processing a lot of events.
      //
      // Here we test to add some histograms after a certain number
      // of events.
      if (count == testAddNum) {
        //consinfo->addObject("Muon_Pt", "Producer/Muon", 0, histo2);
        //consinfo->addObject("Muon_Pt", "Warning!", 0, histo2);
        //consinfo->addObject("Muon_py_vs_px", "Producer/Muon", 0, histo3);  
        //consinfo->addObject("Muon_py_vs_px", "Warning!", 0, histo3);
	consinfo->addObject("MuonCanv","Warning!",0,nestedtestcanv);
      }
      if (count == testRemoveAfterAdd) {
        consinfo->removeObject("MuonCanv","Warning!");
      }
      // Here we test to take some histograms away.  
      if (count == testRemoveNum) {
        for (int ihist=0; ihist < s2nHistNum; ihist++) {
          consinfo->removeObject(s2nhistos[ihist]);
        }
      }
      // Update the consumer info event number
      consinfo->setNevents(count);

      // Send out the consumer info and all objects in its list.
      consExp.send(consinfo);

      if (count%(updatenum*printSupp) == 0) { 
        std::cout << "Producer: Sent out objects to the Server. " 
             << "Event # "<< count << std::endl;
      }
    }
    ++count;
  }

  for (Int_t ihist=0; ihist<s2nHistNum; ihist++) delete s2nhistos[ihist];
  delete [] s2nhistos;

  for (Int_t iCan=0; iCan<nCanvas; iCan++) delete canvases[iCan];
  delete [] canvases;

  return 1;
}


