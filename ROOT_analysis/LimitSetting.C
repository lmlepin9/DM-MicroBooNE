//Root Headers
#include <TCanvas.h>
#include <TMinuit.h>
#include <TGraph2DErrors.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TF2.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>
#include <TGraphAsymmErrors.h>

#include "RooWorkspace.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooRandom.h"

#include "RooStats/ModelConfig.h"


#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/HybridCalculator.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestPlot.h"

#include "RooStats/NumEventsTestStat.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooStats/SimpleLikelihoodRatioTestStat.h"
#include "RooStats/RatioOfProfiledLikelihoodsTestStat.h"
#include "RooStats/MaxLikelihoodEstimateTestStat.h"
#include "RooStats/NumEventsTestStat.h"

#include "RooStats/HypoTestInverter.h"
#include "RooStats/HypoTestInverterResult.h"
#include "RooStats/HypoTestInverterPlot.h"


/* Macro to obtain limits on the dark trident parameter space
   Based on RooStats tutorials and Anyssa's Dark Side code 

*/ 



using namespace RooStats;
using namespace RooFit;

HypoTestInverterResult* SimpleHypoTestInv( RooWorkspace* w,
                     const char* modelConfigName,
                     const char* dataName );


void PLLTestLimits(){

    TFile *file = NULL;
    file = TFile::Open("./old_samples/stats_space_energy_0.01.root");

    TGraphErrors* gobs = new TGraphErrors;
    TGraph * g0 = new TGraph;
    TGraphAsymmErrors* g1 = new TGraphAsymmErrors;
    TGraphAsymmErrors* g2 = new TGraphAsymmErrors;

    RooWorkspace* w;
    RooRealVar* nsig_0;
    HypoTestInverterResult *r;

    w = (RooWorkspace*) file->Get("w_0.01");  
    w->Print();
    nsig_0 = (RooRealVar*) w->obj("nsig_0"); 
    r = SimpleHypoTestInv(w, "ModelConfig_0.01", "data");

    // Scale back to SM coupling  
    double norm = pow(1e-3,4)/nsig_0->getValV();

    double upperLimit = TMath::Sqrt(r->UpperLimit()*norm);
    double median = TMath::Sqrt(r->GetExpectedUpperLimit(0)*norm);
    double neg1sig =  TMath::Sqrt(r->GetExpectedUpperLimit(-1)*norm);
    double posi1sig =  TMath::Sqrt(r->GetExpectedUpperLimit(1)*norm);
    double neg2sig =  TMath::Sqrt(r->GetExpectedUpperLimit(-2)*norm);
    double posi2sig =  TMath::Sqrt(r->GetExpectedUpperLimit(2)*norm);
    
    std::cout << "Upper limit: " << upperLimit << std::endl;
    std::cout << "Expected limit: " << median << std::endl;
    std::cout << "Expected limit(+1 sig)" << posi1sig << std::endl;
    std::cout << "Expected limit(-1 sig)" << neg1sig << std::endl;

}





HypoTestInverterResult* SimpleHypoTestInv( RooWorkspace* w,
                     const char* modelConfigName,
                     const char* dataName )
{

  /////////////////////////////////////////////////////////////
  // First part is just to access the workspace file 
  ////////////////////////////////////////////////////////////

  std::cout << "Running HypoTestInverter on the workspace " << w->GetName() << std::endl;
  w->Print();

  // get the modelConfig out of the file
  RooStats::ModelConfig* mc = (RooStats::ModelConfig*) w->obj(modelConfigName);

  // get the modelConfig out of the file
  RooAbsData* data = w->data(dataName);
  
  //check the data
  if (!data) {
    Error("StandardHypoTestDemo","Not existing data %s",dataName);
    return 0;
  }
  else
    std::cout << "Using data set " << dataName << std::endl;

  ModelConfig*  sbModel = (RooStats::ModelConfig*) w->obj(modelConfigName);
  ModelConfig*  bModel  = (RooStats::ModelConfig*) w->obj(modelConfigName);


  //check the model
  if(!sbModel){
    Error("PLLTestLimits","Not existing ModelConfig %s",modelConfigName);
  }
  if(!sbModel->GetPdf()){
    Error("PLLTestLimits","Model %s has no pdf", modelConfigName);
  }

  if (!sbModel->GetSnapshot() ) {
    Info("PLLTestLimits","Model %s has no snapshot  - make one using model poi",modelConfigName);
    sbModel->SetSnapshot( *sbModel->GetParametersOfInterest() );
  }


  RooRealVar* poi = (RooRealVar*) sbModel->GetParametersOfInterest()->first();
  double orPoi = poi->getVal(); 
  cout << "POI original value: " << poi->GetName() << " = " << poi->getVal() << endl;  


  if (!bModel || bModel == sbModel) {
    Info("PLLTestLimits","The background model %s does not exist",modelConfigName);
    Info("PLLTestLimits","Copy it from ModelConfig %s and set POI to zero",modelConfigName);
    bModel = (ModelConfig*) sbModel->Clone();
    bModel->SetName(TString(modelConfigName)+TString("_with_poi_0"));
    poi->setVal(0);
    bModel->SetSnapshot( *poi );
    poi->setVal(orPoi); 
  }
  else{

    if (!bModel->GetSnapshot() ) {
      Info("PLLTestLimits","B model has no snapshot  - make one using model poi and 0 values ");
      poi->setVal(0);
      bModel->SetSnapshot( *poi);
      poi->setVal(orPoi);
    }
        
  }


  // create HypoTest calculator (data, alt model , null model)

  //FrequentistCalculator  fc(*data, *bModel, *sbModel);
  //fc.SetToys(1000,1000);

  // asymptotic calculator
  AsymptoticCalculator  ac(*data, *bModel, *sbModel, true);


  ac.SetOneSided(true);  // for one-side tests (limits)
  //  ac->SetQTilde(true);
  AsymptoticCalculator::SetPrintLevel(-1);

  cout << "Calculator def" << endl;
  
#define UseAsym
  // create hypotest inverter 
  // passing the desired calculator 
#ifdef UseAsym
  HypoTestInverter calc(ac);    // for asymptotic
#else
  HypoTestInverter calc(fc);  // for frequentist
#endif
  // set confidence level (e.g. 95% upper limits)
  calc.SetConfidenceLevel(0.9);

  cout << "CL set" <<endl;

  // for CLS
  bool useCLs = true;
  calc.UseCLs(useCLs);
  calc.SetVerbose(true);

//  calc.SetParameter("EnableDetailedOutput", true);

#ifndef UseAsym
  // configure ToyMC Samler (needed only for frequentit calculator)
  ToyMCSampler *toymcs = (ToyMCSampler*)calc.GetHypoTestCalculator()->GetTestStatSampler();
#endif

  cout <<"Test statistics " << endl;

//  // profile likelihood test statistics
  ProfileLikelihoodTestStat profll(*sbModel->GetPdf());
//  // for CLs (bounded intervals) use one-sided profile likelihood
  if (useCLs) profll.SetOneSided(true);

  profll.EnableDetailedOutput(); //detailed outputs

#ifndef UseAsym
  // set the test statistic to use 
  toymcs->SetTestStatistic(&profll);

  // if the pdf is not extended (e.g. in the Poisson model) 
  // we need to set the number of events
  if (!sbModel->GetPdf()->canBeExtended())
     toymcs->SetNEventsPerToy(1);
#endif

  int npoints = 100;  // number of points to scan
  // min and max (better to choose smaller intervals)
  double poimin = poi->getMin();
  double poimax = 10; //poi->getMax();
  //poimin = 0; poimax=10;

  std::cout << "Doing a fixed scan  in interval : " << poimin << " , " << poimax << std::endl;
  calc.SetFixedScan(npoints,poimin,poimax);
  

  HypoTestInverterResult * r = calc.GetInterval();

  double upperLimit = r->UpperLimit();

  std::cout << "The computed upper limit is: " << upperLimit << std::endl;
  RooRealVar* nsig_0 = (RooRealVar*) w->obj("nsig_0");
  std::cout << "The computed upper limit divide by nominal value: "<< nsig_0->getValV()<<" is: " << upperLimit/nsig_0->getValV() << std::endl;

  // compute expected limit
  std::cout << "Expected upper limits, using the B (alternate) model : " << std::endl;
  std::cout << " expected limit (median) " << r->GetExpectedUpperLimit(0) << std::endl;
  std::cout << " expected limit (-1 sig) " << r->GetExpectedUpperLimit(-1) << std::endl;
  std::cout << " expected limit (+1 sig) " << r->GetExpectedUpperLimit(1) << std::endl;
  
//  profll.GetDetailedOutput();

  // plot now the result of the scan 

  HypoTestInverterPlot *plot = new HypoTestInverterPlot("HTI_Result_Plot","HypoTest Scan Result",r);

  // plot in a new canvas with style
  TCanvas * c1 = new TCanvas("HypoTestInverter Scan"); 
  c1->SetLogy(false);

  plot->Draw("CLb 2CL");  // plot also CLb and CLs+b
//  plot->Draw("OBS");  // plot only observed p-value


  // plot also in a new canvas the test statistics distributions 
  
  // plot test statistics distributions for the two hypothesis
  // when distribution is generated (case of FrequentistCalculators)
  const int n = r->ArraySize();
  if (n> 0 &&  r->GetResult(0)->GetNullDistribution() ) { 
     TCanvas * c2 = new TCanvas("Test Statistic Distributions","",2);
     if (n > 1) {
        int ny = TMath::CeilNint( sqrt(n) );
        int nx = TMath::CeilNint(double(n)/ny);
        c2->Divide( nx,ny);
     }
     for (int i=0; i<n; i++) {
        if (n > 1) c2->cd(i+1);
        SamplingDistPlot * pl = plot->MakeTestStatPlot(i);
        pl->SetLogYaxis(true);
        pl->Draw();
     }
  }

  return r;
}

