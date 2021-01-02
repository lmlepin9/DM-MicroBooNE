void extract_tree() {

   //Get old file, old tree and set top branch address
   TFile *oldfile = new TFile("./ncpi0_larcv_fhc.root");
   TTree *oldtree = (TTree*)oldfile->Get("image2d_wire_tree;1");
  
   //Create a new file + a clone of old tree in new file
   TFile *newfile = new TFile("ncpi0_larcv_fhc_reduced.root","recreate");
   newfile->cd();
   oldtree->CloneTree()->Write();

}
