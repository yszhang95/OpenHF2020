// Original code can be found on 
// https://github.com/root-project/root/blob/master/tutorials/tmva/TMVACrossValidation.C
// Modified by Yousen Zhang, Rice University, US
// Data: 2020 August 27
//

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "THashList.h"
#include "TFileCollection.h"

#include "Riostream.h"


#include "TMVA/Tools.h"
#include "TMVA/Config.h"
#include "TMVA/Factory.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
#include "TMVA/DataLoader.h"
#include "TMVA/CrossValidation.h"
#endif

#include "functions.h"

using std::map;
using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::istringstream;

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidation(const tmvaConfigs&);
#endif

int main( int argc, char** argv )
{
  gROOT->SetBatch(true);
  // Select methods (don't look at this code - not of interest)
  TString methodList;
  if( argc<2 || argc>3 ) {
    std::cout << "Usage: TMVACrossValidation configName [ DEBUG ]" << std::endl;
    return -1;
  }

  if (argc==3 && string(argv[2]).size()) { DEBUG = 1; }
  else DEBUG = 0;

  tmvaConfigs configs(argv[1], DEBUG);
  string tempOutFileName(gSystem->BaseName(argv[1]));
  {
    auto pos = tempOutFileName.find(".xml");
    if (pos!=string::npos) { tempOutFileName.erase(pos, 4); }
    if (tempOutFileName.size())
      tempOutFileName.insert(0, "_");
  }
  configs.setOutFileName("TMVA" + tempOutFileName +".root");

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  return TMVACrossValidation(configs);
#else
  cerr << "ROOT VERSION is too low. Please upgrade to root-6.12 or above" << endl;
  return -1;
#endif
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidation(const tmvaConfigs& configs)
{
  // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
  // if you use your private .rootrc, or run from a different directory, please copy the
  // corresponding lines from .rootrc

  //---------------------------------------------------------------
  // This loads the library
  gROOT->SetBatch(true);
  TMVA::Tools::Instance();

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassification" << std::endl;

  // ----------------------------------------------------------------------------------------------

  // --- Here the preparation phase begins

  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  //  TString outfileName = "TMVA" + configs.at("outfileName").at(0) + ".root";
  TString outfileName = configs.getOutFileName();
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  if(DEBUG) { cout << "Output file name: " << outfileName << endl; }

  TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  if (outfileName.Length() > 9) {
    auto temp = outfileName(4, outfileName.Length()-9);
    (TMVA::gConfig().GetIONames()).fWeightFileDir = "weights" + temp; //TString(temp.erase(0, 1).c_str());
  }


  // NOTE: Currently TMVA treats all input variables, spectators etc as
  //       floats. Thus, if the absolute value of the input is too large
  //       there can be precision loss. This can especially be a problem for
  //       cross validation with large event numbers.
  //       A workaround is to define your splitting variable as:
  //           `dataloader->AddSpectator("eventID := eventID % 4096", 'I');`
  //       where 4096 should be a number much larger than the number of folds
  //       you intend to run with.

  // varialbes without range specified
  for (const auto& var : configs.getTrainVarsWoRange()) {
    //    if (DEBUG) { for (const auto& p : var) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    dataloader->AddVariable(var.at("variable"),
        var.at("description"), var.at("unit"), var.at("type")[0]);
  }

  // varialbes with range specified
  for (const auto& var : configs.getTrainVarsWRange()) {
    //    if (DEBUG) { for (const auto& p : var) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    const double minValue = var.at("min") == "" ?
      std::numeric_limits<double>::min() : std::stod(var.at("min").Data());
    const double maxValue = var.at("max") == "" ?
      std::numeric_limits<double>::max() : std::stod(var.at("max").Data());

    dataloader->AddVariable(var.at("variable"),
                            var.at("description"), var.at("unit"), var.at("type")[0],
                            minValue, maxValue);
  }

  // varialbes without range specified
  for (const auto& var : configs.getSpecVarsWoRange()) {
    //    if (DEBUG) { for (const auto& p : var) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    dataloader->AddSpectator(var.at("variable"),
        var.at("description"), var.at("unit"), var.at("type")[0]);
  }
  // varialbes with range specified
  for (const auto& var : configs.getSpecVarsWRange()) {
    //    if (DEBUG) { for (const auto& p : var) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    const double minValue = var.at("min") == "" ?
      std::numeric_limits<double>::min() : std::stod(var.at("min").Data());
    const double maxValue = var.at("max") == "" ?
      std::numeric_limits<double>::max() : std::stod(var.at("max").Data());

    dataloader->AddSpectator(var.at("variable"),
                            var.at("description"), var.at("unit"),
                            minValue, maxValue);
  }

  // Read training and test data
  // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
  const auto signalFileList = configs.signalFileList();
  const auto backgroundFileList = configs.backgroundFileList();

  cout << "--- TMVAClassification       : Using signal file list:     " << signalFileList     << endl;
  cout << "--- TMVAClassification       : Using background file list: " << backgroundFileList << endl;

  // --- Register the training and test trees
  auto sTrain = std::make_shared<TChain>("lambdacAna_mc/ParticleNTuple");
  auto bTrain = std::make_shared<TChain>("lambdacAna/ParticleNTuple");
  auto sTest = std::make_shared<TChain>("lambdacAna_mc/ParticleNTuple");
  auto bTest = std::make_shared<TChain>("lambdacAna/ParticleNTuple");
  // global event weights per tree (see below for setting event-wise weights)
  // Double_t signalWeight     = 1.0;
  // Double_t backgroundWeight = 1.0;
  Double_t signalWeight = configs.signalWeight();
  Double_t backgroundWeight = configs.backgroundWeight();

  // prepare trees to read
  // You can add an arbitrary number of signal or background trees
  // But I only prepared one tree and add once
  const auto sNames = configs.getSignalFileNames();
  const auto bNames = configs.getBackgroundFileNames();
  // for signal
  if (sNames.size() == 2) {
    // train
    vector<string> fTrain(1, sNames.at(0));
    addFilesToChain(sTrain.get(), fTrain);
    dataloader->AddSignalTree(sTrain.get(), signalWeight, "Training");
    cout << "    Added the signal training tree from " << sNames.at(0) << endl;
    // test
    vector<string> fTest(1, sNames.at(1));
    addFilesToChain(sTest.get(), fTest);
    dataloader->AddSignalTree(sTest.get(), signalWeight, "Test");
    cout << "    Added the signal test tree from " << sNames.at(1) << endl;
  } else if (sNames.size() == 1) {
    addFilesToChain(sTrain.get(), sNames);
    dataloader->AddSignalTree(sTrain.get(), signalWeight);
  } else {
    cerr << "[ERROR] cannot find the file for the signal tree" << endl;
  }
  // for background
  if (bNames.size() == 2) {
    // train
    vector<string> fTrain(1, bNames.at(0));
    addFilesToChain(bTrain.get(), fTrain);
    dataloader->AddBackgroundTree(bTrain.get(), backgroundWeight, "Training");
    cout << "    Added the background training tree from " << bNames.at(0) << endl;
    // test
    vector<string> fTest(1, bNames.at(1));
    addFilesToChain(bTest.get(), fTest);
    dataloader->AddBackgroundTree(bTest.get(), backgroundWeight, "Test");
    cout << "    Added the background test tree from " << bNames.at(1) << endl;
  } else if (bNames.size() == 1) {
    addFilesToChain(bTrain.get(), bNames);
    dataloader->AddBackgroundTree(bTrain.get(), backgroundWeight);
  } else {
    cerr << "[ERROR] cannot find the file for the background tree" << endl;
  }

  if (configs.useEventWiseWeight()) {
    dataloader->SetSignalWeightExpression("weight");
    dataloader->SetBackgroundWeightExpression("weight");
  }
  // --- end of tree registration

  // Apply additional cuts on the signal and background samples (can be different)
  std::string common_cuts;
  for (const auto& cut : configs.getCommonCuts()) {
    common_cuts += cut;
    common_cuts += " && ";
  }
  if(common_cuts.size()>4) common_cuts.erase(common_cuts.size()-4, 4);
  TCut mycuts = TCut(common_cuts.c_str());
  //TCut mycutb( (common_cuts + "&& abs(cand_mass-2.29)>0.15 && abs(cand_mass-2.29)<0.20").c_str() ); // for example: TCut mycutb = "abs(var1)<0.5";
  TCut mycutb( (common_cuts).c_str() ); // for example: TCut mycutb = "abs(var1)<0.5";
  if (DEBUG) { cout << "Cut name:" << mycuts.GetName() << ", cut title: " << mycuts.GetTitle(); }
  if (DEBUG) { cout << "Cut name:" << mycutb.GetName() << ", cut title: " << mycutb.GetTitle(); }

  // The CV mechanism of TMVA splits up the training set into several folds.
  // The test set is currently left unused. The `nTest_ClassName=1` assigns
  // one event to the the test set for each class and puts the rest in the
  // training set. A value of 0 is a special value and would split the
  // datasets 50 / 50.
  dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, configs.getDataLoaderConfig());

  // --------------------------------------------------------------------------

  //
  // This sets up a CrossValidation class (which wraps a TMVA::Factory
  // internally) for 2-fold cross validation.
  //
  // The split type can be "Random", "RandomStratified" or "Deterministic".
  // For the last option, check the comment below. Random splitting randomises
  // the order of events and distributes events as evenly as possible.
  // RandomStratified applies the same logic but distributes events within a
  // class as evenly as possible over the folds.
  //
  //
  // One can also use a custom splitting function for producing the folds.
  // The example uses a dataset spectator `eventID`.
  //
  // The idea here is that eventID should be an event number that is integral,
  // random and independent of the data, generated only once. This last
  // property ensures that if a calibration is changed the same event will
  // still be assigned the same fold.
  //
  // This can be used to use the cross validated classifiers in application,
  // a technique that can simplify statistical analysis.
  //
  // If you want to run TMVACrossValidationApplication, make sure you have
  // run this tutorial with Deterministic splitting type, i.e.
  // with the option useRandomSPlitting = false
  //

  TMVA::CrossValidation cv{"TMVACrossValidation", dataloader, outputFile, configs.getFactoryConfig()};

  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

  auto MethodCollection = setupMethodCollection();
  for (const auto& method : configs.getMethods()) {
    //    if (DEBUG) { for (const auto& p : method) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    cv.BookMethod(
        MethodCollection.at(method.at("method").Data()),
        method.at("name"), method.at("config"));
  }

  // --------------------------------------------------------------------------

  //
  // Train, test and evaluate the booked methods.
  // Evaluates the booked methods once for each fold and aggregates the result
  // in the specified output file.
  //
  cv.Evaluate();

  // --------------------------------------------------------------------------

  //
  // Process some output programatically, printing the ROC score for each
  // booked method.
  //
  size_t iMethod = 0;
  for (auto && result : cv.GetResults()) {
    std::cout << "Summary for method " << cv.GetMethods()[iMethod++].GetValue<TString>("MethodName")
              << std::endl;
    for (UInt_t iFold = 0; iFold<cv.GetNumFolds(); ++iFold) {
      std::cout << "\tFold " << iFold << ": "
                << "ROC int: " << result.GetROCValues()[iFold]
                << ", "
                << "BkgEff@SigEff=0.3: " << result.GetEff30Values()[iFold]
                << std::endl;
    }
   }

  // --------------------------------------------------------------

  // Save the output
  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVACrossValidation is done!" << std::endl;

  // delete dataloader; // it seems that this line will lead to crash

  return 0;
}
#endif
