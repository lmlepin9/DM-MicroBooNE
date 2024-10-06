#include <string>
#include <vector>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"



void check_dm_flux(){
    std::string pion_file{"/home/lmlepin/Desktop/dm_sets/bdnmc_events_0.05_pi0_ratio_0.6_1000_alD_0.1_fermion_test_11-04-2023_0.root"}; 
    std::string eta_file{"/home/lmlepin/Desktop/dm_sets/bdnmc_events_0.05_eta_100_alD_0.1_fermion_test_11-04-2023_02.root"};

    TFile *f1 = new TFile(pion_file.c_str(),"READ");
    TFile *f2 = new TFile(eta_file.c_str(),"READ");

    TTree *t1 = (TTree *) f1->Get("event_tree");
    TTree *t2 = (TTree *) f2->Get("event_tree");

    std::cout << "Number of entries in pi0 file: " << t1->GetEntries() << std::endl;
    std::cout << "Number of entries in eta file: " << t2->GetEntries() << std::endl;

    double pi0_energy, pi0_dm_energy, pi0_dm_weight; 
    double eta_energy, eta_dm_energy, eta_dm_weight;

    t1->SetBranchAddress("parent_energy",&pi0_energy);
    t1->SetBranchAddress("dm_energy",&pi0_dm_energy);
    t1->SetBranchAddress("dm_weight",&pi0_dm_weight);


    t2->SetBranchAddress("parent_energy",&eta_energy);
    t2->SetBranchAddress("dm_energy",&eta_dm_energy);
    t2->SetBranchAddress("dm_weight",&eta_dm_weight);


    TH1D *h_pi0[3];
    TH1D *h_eta[3];

    h_pi0[0] = new TH1D("pi0_dm_energy", " Dark matter energy distribution; Energy[GeV]; A.U",50, 0., 4.);
    h_eta[0] = new TH1D("eta_dm_energy", " ",50, 0., 4.);

    h_pi0[1] = new TH1D("pi0_energy", " Parent meson energy; Energy[GeV]; A.U",50, 0., 10.);
    h_eta[1] = new TH1D("eta_energy", " ",50, 0., 10.);

    h_pi0[2] = new TH1D("pi0_dm_weight", " Weight distribution; Weight; A.U",50, 0., 40);
    h_eta[2] = new TH1D("eta_dm_weight", " ",50, 0., 40);

    TLegend *l[3];

    for(int j{0}; j < 3; j++){
        l[j] = new TLegend(0.7,0.7,0.88,0.88);
        h_pi0[j]->SetLineColor(kBlue);
        h_eta[j]->SetLineColor(kRed);
        l[j]->AddEntry(h_pi0[j],"#pi^{0}");
        l[j]->AddEntry(h_eta[j],"#eta");

    }


    for(int i{0}; i < 2000; i++){
        t1->GetEntry(i);
        t2->GetEntry(i);
        h_pi0[0]->Fill(pi0_dm_energy);
        h_eta[0]->Fill(eta_dm_energy);
        h_pi0[1]->Fill(pi0_energy);
        h_eta[1]->Fill(eta_energy);
        h_pi0[2]->Fill(pi0_dm_weight);
        h_eta[2]->Fill(eta_dm_weight);
    }


    gStyle->SetOptStat(0);

    TCanvas *c1 = new TCanvas();
    h_pi0[0]->Draw("HIST");
    h_eta[0]->Draw("HIST SAME");
    l[0]->Draw("SAME");

    TCanvas *c2 = new TCanvas(); 
    h_pi0[1]->Draw("HIST");
    h_eta[1]->Draw("HIST SAME");
    l[1]->Draw("SAME");
    

    TCanvas *c3 = new TCanvas();
    h_pi0[2]->Draw("HIST");
    h_eta[2]->Draw("HIST SAME");
    l[2]->Draw("SAME");

}