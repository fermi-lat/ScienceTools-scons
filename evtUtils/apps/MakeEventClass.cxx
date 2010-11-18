#include <TTree.h>
#include <TFile.h>

#include<iostream>

#include "evtUtils/EventClass.h"

#ifndef WIN32
#include <unistd.h>
#else
#include "facilities/XGetopt.h"
#endif


void usage(std::ostream& os) {
  os << std::endl
     << "Usage:  MakeEventClass.exe [options] <inputFile>" << std::endl
     << "\tOptions: " << std::endl
     << "\t-h\t                      Print this message and exit." << std::endl
     << "\t-s\t<suffix>              Append suffix to output file name [_cuts.root]." << std::endl
     << "\t-x\t<xml>                 Use class definitions from xml file [EvtClassDefs.xml]." << std::endl    
     << "\t-t\t<tree>                Name of input tree [MeritTuple]." << std::endl
     << "\t-T\t<tree>                Name of output tree [MeritTuple]." << std::endl
     << std::endl;
}


int main(int argn, char** argc) {

  // parse options
  // char* endPtr;  
  int opt;

  std::string xmlFile("EvtClassDefs.xml");
  std::string suffix("_cuts.root");
  std::string inTree("MeritTuple");
  std::string outTree("MeritTuple");
  
#ifdef WIN32
  while ( (opt = facilities::getopt(argn, argc, "hs:x:t:T:")) != EOF ) {
#else
  while ( (opt = getopt(argn, argc, "hs:x:t:T:")) != EOF ) {
#endif
    switch (opt) {
    case 'h':   // help      
      usage(std::cout);
      return 1;
    case 's':   // suffix for output file
      suffix = optarg;
      break;
    case 'x':   // xml file with cut definitions
      xmlFile = optarg;
      break;
    case 't':   //  inpput tree
      inTree = optarg;
      break;
    case 'T':   //  output tree
      outTree = optarg;
      break;
    case '?':
      usage(std::cout);
      return 2;
    default:
      std::cerr << opt << " not parsable..." << std::endl;
      usage(std::cerr);
      return 2;  
    }
#ifdef WIN32
  }
#else
  }
#endif

  if ( optind != argn - 1 ) {
    usage(std::cerr);
    return 1;    
  }
  
  evtUtils::EventClass* evtClass = evtUtils::EventClass::loadFromXml(xmlFile);
  if ( evtClass == 0 ) {
    std::cerr << "Failed to load Event class definitions from " << xmlFile << std::endl;
    return 1;
  }

  std::string inFileName = argc[optind];
  std::string outFileName = inFileName;
  size_t find = outFileName.find(".root");
  outFileName.replace(find,5,suffix);

  TFile* fin = TFile::Open(inFileName.c_str());
  if ( fin == 0 ) {
    std::cerr << "Failed to open " << inFileName << std::endl;
    return 1;
  }
  
  TTree* tin = dynamic_cast<TTree*>(fin->Get(inTree.c_str()));
  if ( tin == 0 ) {
    std::cerr << "Failed to get tree " << inTree << " from " << inFileName << std::endl;
    return 1;    
  }

  if ( ! evtClass->initializeShortCuts(*tin) ) {
    std::cerr << "Failed to initialize short cuts against input tree " << inFileName << std::endl;
    return 1;       
  }

  if ( ! evtClass->initializeFullCuts(*tin) ) {
    std::cerr << "Failed to initialize full cuts against input tree " << inFileName << std::endl;
    return 1;       
  }

  TFile* fout = TFile::Open(outFileName.c_str(),"CREATE");
  if ( fout == 0 ) {
    std::cerr << "Failed to open output file " << outFileName << std::endl;
    return 1;
  }

  TTree* tout = new TTree(outTree.c_str(),outTree.c_str());
  if ( tout == 0 ) {
    std::cerr << "Failed to build output TTree " << outFileName << std::endl;
    return 1;    
  }

  if ( ! evtClass->addShortBranchesToTTree(*tout) ) {
    std::cerr << "Failed to add short branches to output tree " << outFileName << std::endl;
    return 1;       
  }

  if ( ! evtClass->addFullBranchesToTTree(*tout) ) {
    std::cerr << "Failed to add full branches to output tree " << outFileName << std::endl;
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

  std::cout << "Writing output to " << outFileName << std::endl;
  tout->Print();

  tout->Write();
  fout->Write();
  delete fout;
  delete evtClass;

}
