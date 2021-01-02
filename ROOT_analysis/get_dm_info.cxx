#include "TFile.h"
#include "TH1F.h"
#include "TVector3.h"
#include "TMath.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// SCRIPT TO GET THE STARTING RUN, SUBRUN and EVENT number 

void get_dm_info(){

  
  // create an ofstream for the file output (see the link on streams for
  // more info)
  ofstream outputFile;
  // create a name for the file output
  std::string filename = "dm_info_cross_check.csv";
  // create and open the .csv file
  outputFile.open(filename);
  // write the file headers
  outputFile << "run" << "," << "subrun" << "," << "event" << std::endl;
  
    

  // open the input file
  TFile f("./dm_cross_check_ana_tree.root");
  // setup a "TTreeReader" to read from the "anatree" Tree
  // Each entry is an art::Event
  TTreeReader anatree_reader("analysistree/anatree;1", &f);
  // Set up a reader for values in the tree
  TTreeReaderValue<Int_t> run (anatree_reader, "run"); // Array of momenta
  TTreeReaderValue<Int_t> subrun (anatree_reader, "subrun"); // Array of momenta
  TTreeReaderValue<Int_t> event (anatree_reader, "event"); // Array of momenta

  // Loop over the analysis tree
  while (anatree_reader.Next()) {
    std::cout<<"Run number: " << *run << std::endl;
    std::cout<<"Sub-run number: " << *subrun << std::endl;
    std::cout<<"Event number: "<< *event << std::endl;
    std::cout<<"\n"<<std::endl;

    // write the data to the output file
    outputFile << *run << "," << *subrun << "," << *event << std::endl;
  }

  // close the output file
  outputFile.close();
}
