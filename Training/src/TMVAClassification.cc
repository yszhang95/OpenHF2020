// Original code can be found on 
// https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/TMVAClassification.C
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

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Config.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
#include "TMVA/DataLoader.h"
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

int TMVAClassification(const tmvaConfigs& configs);

int main( int argc, char** argv )
{
  gROOT->SetBatch(true);
  // Select methods (don't look at this code - not of interest)
  TString methodList;
  if( argc<2 || argc>3 ) {
    std::cout << "Usage: TMVAClassification configName [ DEBUG ]" << std::endl;
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

  return TMVAClassification(configs);
}

int TMVAClassification(const tmvaConfigs& configs)
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
  TString outfileName = configs.getOutFileName();
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  if(DEBUG) { cout << "Output file name: " << outfileName << endl; }

  // Create the factory object. Later you can choose the methods
  // whose performance you'd like to investigate. The factory is
  // the only TMVA object you have to interact with
  //
  // The first argument is the base of the name of all the
  // weightfiles in the directory weight/
  //
  // The second argument is the output file for the training results
  // All TMVA output can be suppressed by removing the "!" (not) in
  // front of the "Silent" argument in the option string
  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, configs.getFactoryConfig());

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
#else
  auto dataloader = factory;
#endif

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  if (outfileName.Length() > 9) {
    auto temp = outfileName(4, outfileName.Length()-9);
    (TMVA::gConfig().GetIONames()).fWeightFileDir = "weights" + temp;
  }

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

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

  // You can add so-called "Spectator variables", which are not used in the MVA training,
  // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
  // input variables, the response values of all trained MVAs, and the spectator variables

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

  // Tell the factory how to use the training and testing events
  //
  // If no numbers of events are given, half of the events in the tree are used 
  // for training, and the other half for testing:
  //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
  // To also specify the number of testing events, use:
  //    factory->PrepareTrainingAndTestTree( mycut,
  //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
  //  dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
  //  "nTest_Signal=1:nTest_Background=1:SplitMode=Random:NormMode=NumEvents:!V" );
  dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, configs.getDataLoaderConfig());


  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

  auto MethodCollection = setupMethodCollection();

  for (const auto& method : configs.getMethods()) {
    // if (DEBUG) { for (const auto& p : method) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
    factory->BookMethod(dataloader,
        MethodCollection.at(method.at("method").Data()),
        method.at("name"), method.at("config"));
#else
    factory->BookMethod(MethodCollection.at(method.at("method").Data()),
        method.at("name"), method.at("config"));
#endif
  }

  // ---- Now you can tell the factory to train, test, and evaluate the MVAs

  // Train MVAs using the set of training events
  factory->TrainAllMethods();

  // ---- Evaluate all MVAs using the set of test events
  factory->TestAllMethods();

  // ----- Evaluate and compare performance of all configured MVAs
  factory->EvaluateAllMethods();

  // --------------------------------------------------------------

  // Save the output
  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVAClassification is done!" << std::endl;

  delete factory;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  delete dataloader;
#endif

  return 0;
}
