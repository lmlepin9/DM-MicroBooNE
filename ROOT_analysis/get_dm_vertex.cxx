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


void get_dm_vertex(){
  // create an ofstream for the file output (see the link on streams for
  // more info)
  ofstream outputFile;
  // create a name for the file output
  std::string filename = "dm_vertex_cross_check.csv";
  // create and open the .csv file
  outputFile.open(filename);
  outputFile << "v_x" << "," << "v_y" << "," << "v_z" << std::endl;

  // open the input file
  TFile f("./dm_cross_check_ana_tree.root");

  // setup a "TTreeReader" to read from the "anatree" Tree
  // Each entry is an art::Event
  TTreeReader anatree_reader("analysistree/anatree;1", &f);

  // Set up a reader for values in the tree
  // You can find the names by opening the tree in a TBrowser
  TTreeReaderValue<int> n_particles (anatree_reader, "geant_list_size"); // Number of particles simulated by g4
  TTreeReaderArray<int> pdg (anatree_reader, "pdg"); // Array of particles 
  TTreeReaderArray<float> start_x (anatree_reader, "StartPointx"); // Array of starting x positions
  TTreeReaderArray<float> start_y (anatree_reader, "StartPointy"); // Array of starting y positions
  TTreeReaderArray<float> start_z (anatree_reader, "StartPointz"); // Array of starting z positions

  // Loop over the analysis tree
  while (anatree_reader.Next()) {

    // Want to save the muon and proton end positions
    TVector3 emin_start;
    TVector3 eplus_start;

    // Loop over the number of particles in the event
    for(size_t i = 0; i < *n_particles; i++){
      TVector3 start(start_x[i], start_y[i], start_z[i]);
      
      // Get the electron end position
      if(i == 1){ 
          emin_start = start;
      }
      // Get the positron end position
      if(i == 0) {
          eplus_start = start;
      }
    }

     // write the data to the output file
   outputFile << emin_start.X() << "," << emin_start.Y() << "," << emin_start.Z() << std::endl;

  }
  outputFile.close();
}
