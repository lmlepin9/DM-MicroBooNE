#include "TFile.h"
#include "TH1F.h"
#include "TVector3.h"
#include "TMath.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"


// SCRIPT TO GET THE ENERGIES OF THE MUONS

// Draw histogram
void DrawHist(TH1D* hist, std::string name, std::string xaxis){

  TCanvas *canvas = new TCanvas(name.c_str(), "", 900, 600);

  hist->GetXaxis()->SetTitle(xaxis.c_str());
  hist->Draw();

  canvas->SaveAs((name+".png").c_str());
}

void get_dm_energy() {

  // create an ofstream for the file output (see the link on streams for
  // more info)
  ofstream outputFile;
  // create a name for the file output
  std::string filename = "dm_energies_cross_check.csv";
  // create and open the .csv file
  outputFile.open(filename);
  outputFile << "Energy electron" << "," << "Energy positron" << std::endl;

  // open the input file
  TFile f("./dm_cross_check_ana_tree.root");

  // setup a "TTreeReader" to read from the "anatree" Tree
  // Each entry is an art::Event
  TTreeReader anatree_reader("analysistree/anatree;1", &f);

  // Set up a reader for values in the tree
  // You can find the names by opening the tree in a TBrowser
  // It is important to request initial position at the vertex otherwise you can get anything 
  TTreeReaderValue<int> n_particles (anatree_reader, "geant_list_size"); // Number of particles simulated by g4
  TTreeReaderArray<int> pdg (anatree_reader, "pdg"); // Array of momenta
  TTreeReaderArray<float> energy (anatree_reader, "P"); // Array of energy 
  TTreeReaderArray<float> mass (anatree_reader, "Mass"); // Array of mass 
  TTreeReaderArray<float> momentum_x (anatree_reader, "Px"); // Array of momentum-x
  TTreeReaderArray<float> momentum_y (anatree_reader, "Py"); // Array of momentum-y 
  TTreeReaderArray<float> momentum_z (anatree_reader, "Pz"); // Array of momentum-z   
  TTreeReaderArray<float> start_x (anatree_reader, "StartPointx"); // Array of starting x positions
  TTreeReaderArray<float> start_y (anatree_reader, "StartPointy"); // Array of starting y positions
  TTreeReaderArray<float> start_z (anatree_reader, "StartPointz"); // Array of starting z positions

  // Define histograms
  TH1D* henergy = new TH1D("Particle Energy", "", 80, 0,1.0);
  int event = 1; 

  // Loop over the analysis tree
  while (anatree_reader.Next()) {
    std::cout<<"Event number: "<< event << std::endl;
    std::cout<<"\n"<<std::endl;
    float energy_e;
    float mass_e;
    float px_e;
    float py_e;
    float pz_e;  

    float energy_p;
    float mass_p;
    float px_p;
    float py_p;
    float pz_p;

    // Loop over the number of particles in the event
    for(size_t i = 0; i < *n_particles; i++){
      TVector3 start(start_x[i], start_y[i], start_z[i]);
      if(i ==1){
        mass_e = mass[i];
        px_e = momentum_x[i];
        py_e = momentum_y[i];
        pz_e = momentum_z[i];
        float total_p = TMath::Sqrt(pow(px_e,2) + pow(py_e,2) + pow(pz_e,2));
        energy_e = TMath::Sqrt( pow(total_p,2) + pow(mass_e,2) );
        std::cout<<"PDG CODE Electron: " << pdg[i] << std::endl;
        std::cout<<"Energy in GeV: " << energy_e << std::endl;
        std::cout<<"Total momentum: "<< total_p << std::endl;
        std::cout<<"Mass: "<<mass_e<<std::endl;
        std::cout<<"-----------------------"<<std::endl;
        std::cout<<"\n"<<std::endl;
      }

      if(i == 0){
        mass_p = mass[i];
        px_p = momentum_x[i];
        py_p = momentum_y[i];
        pz_p = momentum_z[i];
        float total_p = TMath::Sqrt(pow(px_p,2) + pow(py_p,2) + pow(pz_p,2));
        energy_p = TMath::Sqrt( pow(total_p,2) + pow(mass_p,2) );
        std::cout<<"PDG CODE Positron: " << pdg[i] << std::endl;
        std::cout<<"Energy in GeV: " << energy_p << std::endl;
        std::cout<<"Total momentum: "<< total_p << std::endl;
        std::cout<<"Mass: "<<mass_p<<std::endl;
        std::cout<<"-----------------------"<<std::endl;
        std::cout<<"\n"<<std::endl;
      }
    }
    outputFile << energy_e << "," << energy_p << std::endl;

    // Calculate the angle between them, convert to degrees
    // Fill the histogram
    henergy->Fill(TMath::Max(energy_e,energy_p));
    event +=1; 

  }

  // Draw the histogram
  DrawHist(henergy, "max_energy_truth_cross_check", "E [GeV]");
}
