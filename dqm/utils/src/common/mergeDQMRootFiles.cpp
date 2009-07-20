/*

This macro will add histograms from a list of root files and write them
to a target root file. The target file is newly created and must not be
identical to one of the source files.

Author: Sven A. Schmidt, sven.schmidt@cern.ch
Date:   13.2.2001

This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
which had a problem with directories more than one level deep.
(see macro hadd_old.C for this previous implementation).
  
The macro from Sven has been enhanced by 
Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
to automatically add Trees (via a chain of trees).
  
To use this macro, modify the file names in function hadd.
  
NB: This macro is provided as a tutorial.
Use $ROOTSYS/bin/hadd to merge many histogram files

*/


#include <string>

#ifndef __CINT__

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "TList.h"



TList *FileList;
TFile *Target;


void MergeRootfiles( TDirectory *target, TList *sourcelist );

using namespace std;


void usage(char * name)
{
        cout << "Usage: " << name << " merged_file.root file1.root file2.root ..." << endl;
}

int main(int argc, char **argv)
{
  if (argc<3) {usage(argv[0]); return 0;};

  gSystem->Load("libHistPainter");


	  string merged_file = argv[1];
	  cout << "==> Output: " << merged_file << endl;
	  
	  FileList = new TList();

	  for (int i=2; i< argc; i++) {
		cout << "==> Input" << i-1 << ": " << argv[i] << endl;
		FileList->Add( TFile::Open(argv[i]) );	
	  }

	  Target = TFile::Open( merged_file.c_str(), "RECREATE" );
	  TDirectory *histos = Target->mkdir( "DQMData", "Histograms" );

  MergeRootfiles( histos, FileList );
	
}

#endif


void hadd() {
  // in an interactive ROOT session, edit the file names
  // Target and FileList, then
  // root > .L hadd.C
  // root > hadd()
   
  Target = TFile::Open( "run_00017247.root", "RECREATE" );
  TDirectory *histos = Target->mkdir( "DQMData", "Histograms" );
  
  FileList = new TList();
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-02.root") ); 
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-03.root") );
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-04.root") );
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-05.root") );
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-07.root") );
  FileList->Add( TFile::Open("run_00017247_csc-C2D07-09.root") );
//  FileList->Add( TFile::Open("RunNum2372_751.root") );
//  FileList->Add( TFile::Open("RunNum2372_750.root") );
  
  MergeRootfiles( histos, FileList );

}   


void MergeObjects( TDirectory *target, TFile *source)
{
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  source->cd( path );
  TDirectory *current_sourcedir = gDirectory;

  // loop over all keys in this directory
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key;

  while ( (key = (TKey*)nextkey())) {
//    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

    source->cd( path );

    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( "TH1" ) ) {

      TH1 *h1 = (TH1*)obj;

      if (target->cd()) {
	TH1 *h2 = (TH1*)target->Get( h1->GetName() );

	if ( h2 ) {
	  cout << "+Merging histogram " << obj->GetName() << endl;
	  h1->Add( h2 );
	  delete h2; // don't know if this is necessary, i.e. if

	} 
		
      }
    }
    else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
      // it's a subdirectory
      cout << "Found subdirectory " << obj->GetName() << endl;

      target->cd();
      if ( target->Get(obj->GetName()) == NULL)
	{
	  TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
	  MergeObjects( newdir, source );
	} else {

	  TDirectory *newdir =  (TDirectory *) (target->Get(obj->GetName()));
	  MergeObjects( newdir, source );
	}

    } 
    
    if ( obj ) {
      target->cd();
      obj->Write( key->GetName(), TObject::kOverwrite );
      delete obj;
    }
  }
  target->SaveSelf(kTRUE);
}


void MergeRootfiles( TDirectory *target, TList *sourcelist ) 
{

  TFile *source = (TFile*)sourcelist->First();

  while (source) {
    cout << "==> Processing " << source->GetName() << endl;;
    MergeObjects(target, source);
    source = (TFile*)sourcelist->After(source );
  }

  return;
}


