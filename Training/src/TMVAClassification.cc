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
using std::endl;
using std::istringstream;

int TMVAClassification(const map<string, vector<string>>& configs);

int main( int argc, char** argv )
{
  // Select methods (don't look at this code - not of interest)
  TString methodList; 
  if( argc<2 || argc>3 ) {
    std::cout << "Usage: TMVAClassification configName [ DEBUG ]" << std::endl;
    return -1;
  }

  if (argc==3 && string(argv[2]).size()) { DEBUG = 1; }

  auto configs = readConfigs(argv[1]);
  configs["outfileName"] = vector<string>(1, gSystem->BaseName(argv[1]));
  auto pos = configs.at("outfileName").at(0).find(".xml");
  if (pos!=string::npos) { (configs["outfileName"])[0].erase(pos, 4); }
  if (configs.at("outfileName").at(0).size()) { (configs["outfileName"])[0].insert(0, "_"); }

  return TMVAClassification(configs);
}

int TMVAClassification(const map<string, vector<string>>& configs)
{
  // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
  // if you use your private .rootrc, or run from a different directory, please copy the
  // corresponding lines from .rootrc

  //---------------------------------------------------------------
  // This loads the library
  TMVA::Tools::Instance();

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassification" << std::endl;

  // ----------------------------------------------------------------------------------------------

  // --- Here the preparation phase begins

  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfileName = "TMVA" + configs.at("outfileName").at(0) + ".root";
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
  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");
#else
  auto dataloader = factory;
#endif

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  if (configs.at("outfileName").at(0).size()) {
    auto temp = configs.at("outfileName").at(0);
    (TMVA::gConfig().GetIONames()).fWeightFileDir = "weights" + temp; //TString(temp.erase(0, 1).c_str());
  }

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

  // varialbes without range specified
  for (const auto& var : configs.at("training_variables_worange")) {
    const auto pars = getTrainPars(var, "training_variables_worange");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    dataloader->AddVariable(pars.at("variable"),
        pars.at("description"), pars.at("unit"), pars.at("type")[0]);
  }
  // varialbes with range specified
  for (const auto& var : configs.at("training_variables_wrange")) {
    const auto pars = getTrainPars(var, "training_variables_wrange");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    const double minValue = pars.at("min") == "" ?
      std::numeric_limits<double>::min() : std::stod(pars.at("min").Data());
    const double maxValue = pars.at("max") == "" ?
      std::numeric_limits<double>::max() : std::stod(pars.at("max").Data());
    dataloader->AddVariable(pars.at("variable"),
        pars.at("description"), pars.at("unit"), pars.at("type")[0],
        minValue, maxValue);
  }

  // You can add so-called "Spectator variables", which are not used in the MVA training,
  // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
  // input variables, the response values of all trained MVAs, and the spectator variables

  // varialbes without range specified
  for (const auto& var : configs.at("spectator_variables_worange")) {
    const auto pars = getTrainPars(var, "spectator_variables_worange");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    dataloader->AddSpectator(pars.at("variable"),
        pars.at("description"), pars.at("unit"), pars.at("type")[0]);
  }
  // varialbes with range specified
  for (const auto& var : configs.at("spectator_variables_wrange")) {
    const auto pars = getTrainPars(var, "spectator_variables_wrange");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    const double minValue = pars.at("min") == "" ?
      std::numeric_limits<double>::min() : std::stod(pars.at("min").Data());
    const double maxValue = pars.at("max") == "" ?
      std::numeric_limits<double>::max() : std::stod(pars.at("max").Data());
    dataloader->AddVariable(pars.at("variable"),
        pars.at("description"), pars.at("unit"), pars.at("type")[0],
        minValue, maxValue);
  }

  // Read training and test data
  // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
  const auto signalFileList = configs.at("signalFileList").at(0);
  const auto backgroundFileList = configs.at("backgroundFileList").at(0);

  cout << "--- TMVAClassification       : Using signal file list:     " << signalFileList     << endl;
  cout << "--- TMVAClassification       : Using background file list: " << backgroundFileList << endl;

  // --- Register the training and test trees
  TFileCollection fcSignal("signalFileCollection", "", signalFileList.c_str());
  TFileCollection fcBackground("backgroundFileCollection", "", backgroundFileList.c_str());

  auto signal = std::make_shared<TChain>("lambdacAna_mc/ParticleNTuple");
  auto background = std::make_shared<TChain>("lambdacAna/ParticleNTuple");

  signal->AddFileInfoList(fcSignal.GetList());
  background->AddFileInfoList(fcBackground.GetList());

  // global event weights per tree (see below for setting event-wise weights)
  Double_t signalWeight     = 1.0;
  Double_t backgroundWeight = 1.0;

  // You can add an arbitrary number of signal or background trees
  dataloader->AddSignalTree    ( signal.get(),     signalWeight     );
  dataloader->AddBackgroundTree( background.get(), backgroundWeight );

  // --- end of tree registration 

  // Apply additional cuts on the signal and background samples (can be different)
  std::string common_cuts;
  for (const auto& cut : configs.at("common_cuts")) {
    common_cuts += cut;
    common_cuts += " && ";
  }
  if(common_cuts.size()>4) common_cuts.erase(common_cuts.size()-4, 4);
  TCut mycuts = TCut(common_cuts.c_str());
  TCut mycutb( (common_cuts + "&& abs(cand_mass-2.29)>0.15").c_str() ); // for example: TCut mycutb = "abs(var1)<0.5";
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
  dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
      "SplitMode=Random:NormMode=NumEvents:!V" );

  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

  auto MethodCollection = setupMethodCollection();
  for (const auto& m : configs.at("methods")) {
    const auto method = getTrainPars(m, "methods");
    if (DEBUG) { for (const auto& p : method) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
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
