#include "TFile.h"
#include "TH1F.h"
#include "TVector3.h"
#include "TMath.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include <iostream>
#include <fstream>
#include <string>



/*
Script to extract information from single photon files. 
The output is a csv file with the relevant variables. 
There are two functions, one for signal files and another for background files 

Author: Luis Mora
*/ 




void SignalInfo(const std::string& input_file, const std::string& output_file){

  // Initialize output file and open .root file 
  ofstream outputFile;
  std::cout<<"Input file name: " << input_file << std::endl;
  std::cout<<"Output file name: " << output_file << std::endl;
  outputFile.open(output_file);
  outputFile << "run" << "," << "subrun" << "," << "event" << "," << "n_vertex" << "," << "op_angle" << "," << "e_min_E"<< "," << "e_plus_E" << ","
  "v_true_x" << "," << "v_true_y" << "," << "v_true_z" << "," << "shower_x" << "," << "shower_y" << "," << "shower_z" << ","
   "v_x" << "," << "v_y" << "," << "v_z" << "," << "n_tracks" << "," << "n_showers" << "," << "E_total" <<  "," << "shower_dir" << std::endl; 
  TFile f(input_file.c_str());


  // Extract relevant variables 
  TTreeReader anatree_reader("singlephotonana/vertex_tree;1", &f);
  TTreeReaderValue<Int_t> run (anatree_reader, "run_number"); 
  TTreeReaderValue<Int_t> subrun (anatree_reader, "subrun_number");
  TTreeReaderValue<Int_t> event (anatree_reader, "event_number");
  TTreeReaderValue<Int_t> nvertex (anatree_reader, "reco_vertex_size");
  TTreeReaderValue<Double_t> vertex_x (anatree_reader, "reco_vertex_x");
  TTreeReaderValue<Double_t> vertex_y (anatree_reader, "reco_vertex_y");
  TTreeReaderValue<Double_t> vertex_z (anatree_reader, "reco_vertex_z");
  TTreeReaderValue<Int_t> n_tracks (anatree_reader, "reco_asso_tracks");
  TTreeReaderValue<Int_t> n_showers (anatree_reader, "reco_asso_showers");
  TTreeReaderArray<Double_t> track_energies (anatree_reader, "reco_track_calo_energy_max");
  TTreeReaderArray<Double_t> shower_energies (anatree_reader, "reco_shower_energy_max");
  TTreeReaderArray<Double_t> dir_x (anatree_reader, "reco_shower_dirx");
  TTreeReaderArray<Double_t> dir_y (anatree_reader, "reco_shower_diry");
  TTreeReaderArray<Double_t> dir_z (anatree_reader, "reco_shower_dirz");
  TTreeReaderArray<Double_t> shower_start_x_arr (anatree_reader, "reco_shower_startx");
  TTreeReaderArray<Double_t> shower_start_y_arr (anatree_reader, "reco_shower_starty");
  TTreeReaderArray<Double_t> shower_start_z_arr (anatree_reader, "reco_shower_startz");
  TTreeReaderValue<Int_t> n_mctruth_daughters (anatree_reader, "mctruth_num_daughter_particles");
  TTreeReaderArray<Int_t> mctruth_pdg_arr (anatree_reader, "mctruth_daughters_pdg");
  TTreeReaderArray<Double_t> mctruth_E_arr (anatree_reader, "mctruth_daughters_E");
  TTreeReaderArray<Double_t> mctruth_start_x_arr (anatree_reader, "mctruth_daughters_startx");
  TTreeReaderArray<Double_t> mctruth_start_y_arr (anatree_reader, "mctruth_daughters_starty");
  TTreeReaderArray<Double_t> mctruth_start_z_arr (anatree_reader, "mctruth_daughters_startz");
  TTreeReaderArray<Double_t> mctruth_end_x_arr (anatree_reader, "mctruth_daughters_endx");
  TTreeReaderArray<Double_t> mctruth_end_y_arr (anatree_reader, "mctruth_daughters_endy");
  TTreeReaderArray<Double_t> mctruth_end_z_arr (anatree_reader, "mctruth_daughters_endz");
  TTreeReaderArray<Double_t> mctruth_p_x_arr (anatree_reader, "mctruth_daughters_px");
  TTreeReaderArray<Double_t> mctruth_p_y_arr (anatree_reader, "mctruth_daughters_py");
  TTreeReaderArray<Double_t> mctruth_p_z_arr (anatree_reader, "mctruth_daughters_pz");

  // Loop over the analysis tree
  while (anatree_reader.Next()) {

    /*
    std::cout<<"Run number: " << *run << std::endl;
    std::cout<<"Sub-run number: " << *subrun << std::endl;
    std::cout<<"Event number: "<< *event << std::endl; 
    std::cout<<"Number of vertexes: " << *nvertex << std::endl;
    std::cout<<"Number of tracks: " << *n_tracks << std::endl;
    std::cout<<"Number of showers: " << *n_showers << std::endl;
    std::cout<<"\n"<<std::endl;*/ 

    double total_energy = 0; 
    double dir_vect[3] = {0.462372, 0.0488541,0.885339};
    double shower_dir = -9999999.; 
    int pdg_code = 0; 
    double emin_true_E = 0;
    double eplus_true_E = 0; 
    double shower_start_x = -99999999;
    double shower_start_y = -99999999;
    double shower_start_z = -99999999;
    TVector3 emin_end;
    TVector3 emin_start;
    TVector3 emin_p; 
    TVector3 eplus_end;
    TVector3 eplus_start;
    TVector3 eplus_p; 


    // Currently using the first shower in the array, how to define leading shower (?)
    if(*n_showers >= 1){
      shower_start_x = shower_start_x_arr[0];
      shower_start_y = shower_start_y_arr[0];
      shower_start_z = shower_start_z_arr[0];
    }

    for(int i = 0; i < *n_tracks ; i++){
      total_energy += track_energies[i];

    }

    // Total reconstructed energy of showers, keeps the most extreme shower direction (needs to be proper defined)
    for(int j = 0; j < *n_showers ; j++){
      total_energy += shower_energies[j];
      double shower_dir_temp = dir_x[j]*dir_vect[0] + dir_y[j]*dir_vect[1] + dir_z[j]*dir_vect[2];
      if(shower_dir_temp > shower_dir){
        shower_dir = shower_dir_temp;
      }
      else{
        continue; 
      }
    }


    /*
    if(*n_tracks == 0 && *n_showers ==1){
      shower_dir = dir_x[0]*dir_vect[0] + dir_y[0]*dir_vect[1] + dir_z[0]*dir_vect[2];
    }*/ 


    // Truth info 
    for(int k = 0; k < *n_mctruth_daughters; k++){
      pdg_code = mctruth_pdg_arr[k];
      TVector3 start(mctruth_start_x_arr[k], mctruth_start_y_arr[k], mctruth_start_z_arr[k]);
      TVector3 end(mctruth_end_x_arr[k], mctruth_end_y_arr[k], mctruth_end_z_arr[k]);
      TVector3 p(mctruth_p_x_arr[k], mctruth_p_y_arr[k], mctruth_p_z_arr[k] );


      if(pdg_code == 11){
        emin_true_E = mctruth_E_arr[k];
        emin_start = start;
        emin_end = end; 
        emin_p = p;
      }

      if(pdg_code == -11){
        eplus_true_E = mctruth_E_arr[k];
        eplus_start = start;
        eplus_end = end; 
        eplus_p = p; 
      }
  
      //std::cout<< pdg_code << std::endl;
    }
    double angle = (emin_p).Angle(eplus_p) * 180./TMath::Pi();
    //std::cout << angle << std::endl;

    // write the data to the output file
    outputFile << *run << "," << *subrun << "," << *event << "," << *nvertex << "," << angle << "," << emin_true_E << "," << eplus_true_E << "," << emin_start.X() << "," << emin_start.Y() << "," << emin_start.Z() << ","  << *vertex_x 
    << "," << *vertex_y << "," << *vertex_z << "," << shower_start_x << "," << shower_start_y << "," << shower_start_z << "," << *n_tracks << "," << *n_showers << "," << total_energy << "," << shower_dir << std::endl;
    
    /*std::cout<<"==========================================="<<std::endl;
    std::cout<<"\n"<<std::endl;*/ 
  }

  // close the output file
  outputFile.close();
  
}


void BackInfo(const std::string& input_file, const std::string& output_file){

  ofstream outputFile;
  std::cout<<"Input file name: " << input_file << std::endl;
  std::cout<<"Output file name: " << output_file << std::endl;


  outputFile.open(output_file);
  outputFile << "run" << "," << "subrun" << "," << "event" << "," << "v_true_x" << "," << "v_true_y" << "," << "v_true_z" << "," 
  << "CC_or_NC" << "," << "n_pi0" << "," << "n_photons" << "," << "n_pipm" << "," << "n_vertex" <<  ","
  << "shower_x" << "," << "shower_y" << "," << "shower_z" << "," << "v_x" << "," << "v_y" << "," << "v_z" << ","
  << "n_tracks" << "," << "n_showers" << "," << "E_total" <<  "," << "shower_dir" << std::endl; 
  TFile f(input_file.c_str());

  TTreeReader anatree_reader("singlephotonana/vertex_tree;1", &f);
  TTreeReaderValue<Int_t> run (anatree_reader, "run_number"); 
  TTreeReaderValue<Int_t> subrun (anatree_reader, "subrun_number");
  TTreeReaderValue<Int_t> event (anatree_reader, "event_number");
  TTreeReaderValue<Int_t> nvertex (anatree_reader, "reco_vertex_size");
  TTreeReaderValue<Double_t> vertex_x (anatree_reader, "reco_vertex_x");
  TTreeReaderValue<Double_t> vertex_y (anatree_reader, "reco_vertex_y");
  TTreeReaderValue<Double_t> vertex_z (anatree_reader, "reco_vertex_z");
  TTreeReaderValue<Int_t> n_tracks (anatree_reader, "reco_asso_tracks");
  TTreeReaderValue<Int_t> n_showers (anatree_reader, "reco_asso_showers");
  TTreeReaderArray<Double_t> track_energies (anatree_reader, "reco_track_calo_energy_max");
  TTreeReaderArray<Double_t> shower_energies (anatree_reader, "reco_shower_energy_max");
  TTreeReaderArray<Double_t> dir_x (anatree_reader, "reco_shower_dirx");
  TTreeReaderArray<Double_t> dir_y (anatree_reader, "reco_shower_diry");
  TTreeReaderArray<Double_t> dir_z (anatree_reader, "reco_shower_dirz");
  TTreeReaderArray<Double_t> shower_start_x_arr (anatree_reader, "reco_shower_startx");
  TTreeReaderArray<Double_t> shower_start_y_arr (anatree_reader, "reco_shower_starty");
  TTreeReaderArray<Double_t> shower_start_z_arr (anatree_reader, "reco_shower_startz");
  TTreeReaderValue<Double_t> nu_vertex_x (anatree_reader, "mctruth_nu_vertex_x");
  TTreeReaderValue<Double_t> nu_vertex_y (anatree_reader, "mctruth_nu_vertex_y");
  TTreeReaderValue<Double_t> nu_vertex_z (anatree_reader, "mctruth_nu_vertex_z");
  TTreeReaderValue<Int_t> cc_or_nc (anatree_reader, "mctruth_cc_or_nc"); // 0 for CC and 1 for NC 
  TTreeReaderValue<Int_t> n_pi0 (anatree_reader, "mctruth_num_exiting_pi0");
  TTreeReaderValue<Int_t> n_pipm (anatree_reader, "mctruth_num_exiting_pipm");
  TTreeReaderValue<Int_t> n_photons (anatree_reader, "mctruth_num_exiting_photons");
  TTreeReaderValue<Int_t> n_mctruth_daughters (anatree_reader, "mctruth_num_daughter_particles");
  TTreeReaderArray<Int_t> mctruth_pdg_arr (anatree_reader, "mctruth_daughters_pdg");
  TTreeReaderArray<Double_t> mctruth_E_arr (anatree_reader, "mctruth_daughters_E");
  TTreeReaderArray<Double_t> mctruth_start_x_arr (anatree_reader, "mctruth_daughters_startx");
  TTreeReaderArray<Double_t> mctruth_start_y_arr (anatree_reader, "mctruth_daughters_starty");
  TTreeReaderArray<Double_t> mctruth_start_z_arr (anatree_reader, "mctruth_daughters_startz");
  TTreeReaderArray<Double_t> mctruth_end_x_arr (anatree_reader, "mctruth_daughters_endx");
  TTreeReaderArray<Double_t> mctruth_end_y_arr (anatree_reader, "mctruth_daughters_endy");
  TTreeReaderArray<Double_t> mctruth_end_z_arr (anatree_reader, "mctruth_daughters_endz");
  TTreeReaderArray<Double_t> mctruth_p_x_arr (anatree_reader, "mctruth_daughters_px");
  TTreeReaderArray<Double_t> mctruth_p_y_arr (anatree_reader, "mctruth_daughters_py");
  TTreeReaderArray<Double_t> mctruth_p_z_arr (anatree_reader, "mctruth_daughters_pz");

  // Loop over the analysis tree
  while (anatree_reader.Next()) {

    /*
    std::cout<<"Run number: " << *run << std::endl;
    std::cout<<"Sub-run number: " << *subrun << std::endl;
    std::cout<<"Event number: "<< *event << std::endl; 
    std::cout<<"Number of vertexes: " << *nvertex << std::endl;
    std::cout<<"Number of tracks: " << *n_tracks << std::endl;
    std::cout<<"Number of showers: " << *n_showers << std::endl;
    std::cout<<"\n"<<std::endl;*/ 

    double total_energy = 0; 
    double dir_vect[3] = {0.462372, 0.0488541,0.885339};
    double shower_dir = -9999999.; 
    int pdg_code = 0; 
    double emin_true_E = 0;
    double eplus_true_E = 0; 
    double shower_start_x = -99999999;
    double shower_start_y = -99999999;
    double shower_start_z = -99999999;
    TVector3 emin_end;
    TVector3 emin_start;
    TVector3 emin_p; 
    TVector3 eplus_end;
    TVector3 eplus_start;
    TVector3 eplus_p; 

    if(*n_showers >= 1){
      shower_start_x = shower_start_x_arr[0];
      shower_start_y = shower_start_y_arr[0];
      shower_start_z = shower_start_z_arr[0];
    }

    for(int i = 0; i < *n_tracks ; i++){
      total_energy += track_energies[i];

    }

    for(int j = 0; j < *n_showers ; j++){
      total_energy += shower_energies[j];
    }

    for(int j = 0; j < *n_showers ; j++){
      total_energy += shower_energies[j];
      double shower_dir_temp = dir_x[j]*dir_vect[0] + dir_y[j]*dir_vect[1] + dir_z[j]*dir_vect[2];
      if(shower_dir_temp > shower_dir){
        shower_dir = shower_dir_temp;
      }
      else{
        continue; 
      }
    }



    // write the data to the output file
    outputFile << *run << "," << *subrun << "," << *event << "," << *nu_vertex_x << "," << *nu_vertex_y << "," << *nu_vertex_z << "," 
    << *cc_or_nc << "," << *n_pi0 << "," << *n_photons << "," << *n_pipm << ","
    << *nvertex << "," << shower_start_x << "," << shower_start_y << "," << shower_start_z << ","  << *vertex_x << "," 
    << *vertex_y << "," << *vertex_z << "," << *n_tracks << "," << *n_showers << "," << total_energy << "," << shower_dir << std::endl;
  }

  // close the output file
  outputFile.close();
  
}
