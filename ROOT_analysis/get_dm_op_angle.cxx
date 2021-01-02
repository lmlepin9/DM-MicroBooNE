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

// Draw histogram
void DrawHist(TH1D* hist, std::string name, std::string xaxis){

  TCanvas *canvas = new TCanvas(name.c_str(), "", 900, 600);

  hist->GetXaxis()->SetTitle(xaxis.c_str());
  hist->Draw();

  canvas->SaveAs((name+".png").c_str());
}

void get_dm_op_angle(){
  // create an ofstream for the file output (see the link on streams for
  // more info)
  ofstream outputFile;
  // create a name for the file output
  std::string filename = "dm_angles_cross_check.csv";
  // create and open the .csv file
  outputFile.open(filename);
  outputFile << "angle" << std::endl;

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
  TTreeReaderArray<float> end_x (anatree_reader, "EndPointx"); // Array of end x positions
  TTreeReaderArray<float> end_y (anatree_reader, "EndPointy"); // Array of end y positions
  TTreeReaderArray<float> end_z (anatree_reader, "EndPointz"); // Array of end z positions

  // Define histograms
  TH1D* hAngle = new TH1D("hAngle", "", 60, 0,180);

  // Loop over the analysis tree
  while (anatree_reader.Next()) {

    // Want to save the muon and proton end positions
    TVector3 emin_end;
    TVector3 emin_start;
    TVector3 eplus_end;
    TVector3 eplus_start;

    // Loop over the number of particles in the event
    for(size_t i = 0; i < *n_particles; i++){
      TVector3 start(start_x[i], start_y[i], start_z[i]);
      TVector3 end(end_x[i], end_y[i], end_z[i]);
      
      // Get the electron end position
      if(i == 1){ 
          emin_end = end;
          emin_start = start;
      }
      // Get the positron end position
      if(i == 0) {
          eplus_start = start;
          eplus_end = end;
      }
    }

    // Calculate the angle between them, convert to degrees
    double angle = (emin_end - emin_start).Angle(eplus_end - eplus_start) * 180./TMath::Pi();
    // Fill the histogram
    std::cout<<"Opening angle:" << " " << angle << std::endl;
    outputFile << angle << std::endl;
    hAngle->Fill(angle);

  }
  // Draw the histogram
  DrawHist(hAngle, "opening_angle_truth_cross_check", "#theta_{e-e+} [deg]");
  outputFile.close();

}
