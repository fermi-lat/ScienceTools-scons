#include <TTree.h>
#include <TFile.h>

#include<iostream>

#include "EvtUtils/EventClass.h"


int main(int argn, char** argc) {

 
  if (argn != 4) {
    std::cerr << "Usage: MakeEventClass.exe <xmlFile> <inputFile> <outputFile>" << std::endl;
  }

  std::string xmlFile(argc[1]);
  EvtUtils::EventClass* evtClass = EvtUtils::EventClass::loadFromXml(xmlFile);
  if ( evtClass == 0 ) {
    std::cerr << "Failed to load Event class definitions from " << xmlFile << std::endl;
    return 1;
  }

  TFile* fin = TFile::Open(argc[2]);
  if ( fin == 0 ) {
    std::cerr << "Failed to open " << argc[2] << std::endl;
    return 1;
  }
  
  TTree* tin = dynamic_cast<TTree*>(fin->Get("MeritTuple"));
  if ( tin == 0 ) {
    std::cerr << "Failed to get MeritTuple from " << argc[2] << std::endl;
    return 1;    
  }

  if ( ! evtClass->initializeShortCuts(*tin) ) {
    std::cerr << "Failed to initialize short cuts against input tree " << argc[2] << std::endl;
    return 1;       
  }

  if ( ! evtClass->initializeFullCuts(*tin) ) {
    std::cerr << "Failed to initialize full cuts against input tree " << argc[2] << std::endl;
    return 1;       
  }

  TFile* fout = TFile::Open(argc[3],"CREATE");
  if ( fout == 0 ) {
    std::cerr << "Failed to open output file " << argc[3] << std::endl;
    return 1;
  }

  TTree* tout = new TTree("MeritTuple","MeritTuple");
  if ( tout == 0 ) {
    std::cerr << "Failed to build output TTree " << argc[3] << std::endl;
    return 1;    
  }

  if ( ! evtClass->addShortBranchesToTTree(*tout) ) {
    std::cerr << "Failed to add short branches to output tree " << argc[3] << std::endl;
    return 1;       
  }

  if ( ! evtClass->addFullBranchesToTTree(*tout) ) {
    std::cerr << "Failed to add full branches to output tree " << argc[3] << std::endl;
    return 1;       
  }
 
  Long64_t nEvt = tin->GetEntries();
  std::cout << "Total Events : " << nEvt << std::endl;  
  for ( Long64_t iEvt(0); iEvt < nEvt; iEvt++ ) {
    if ( iEvt == 0 ) {;}
    else if ( iEvt % 1000000 == 0 ) { std::cout << "x " << iEvt / 1000000 << 'M' << std::endl; }
    else if ( iEvt % 100000 == 0 ) { std::cout << 'x' << std::endl; }
    else if ( iEvt % 10000 == 0 ) { std::cout << 'x' << std::flush; }
    else if ( iEvt % 1000 == 0 ) { std::cout << '.' << std::flush; }

    tin->LoadTree(iEvt);
    if ( ! evtClass->fillShortCutMaps() ) {
      std::cerr << "Failed to fill short cut maps on event " << iEvt << std::endl;
      return 1;             
    }
    if ( ! evtClass->fillFullCutMaps() ) {
      std::cerr << "Failed to fill full cut maps on event " << iEvt << std::endl;
      return 1;             
    }
    tout->Fill();
  }

  std::cout << std::endl;

  tout->AddFriend(tin,"input");

  std::cout << "Writing output to " << argc[3] << std::endl;
  tout->Print();

  tout->Write();
  fout->Write();
  delete fout;
  delete evtClass;

}
