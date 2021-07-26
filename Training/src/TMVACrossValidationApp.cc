// Original code can be found on
// https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/TMVAClassificationApplication.C
// Modified by Yousen Zhang, Rice University, US
// Data: 2020 August 27
//
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>

#include "TChain.h"
#include "TFile.h"
#include "TH3D.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "THashList.h"
#include "TFileCollection.h"

#include "Riostream.h"
#include "TString.h"
#include "TPRegexp.h"
#include "TClonesArray.h"
#include "TObjString.h"


#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Config.h"

#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
#include "TMVA/DataLoader.h"
#endif

#include "functions.h"
#include "TreeReader/ParticleTreeMC.hxx"
#include "TreeReader/ParticleTreeData.hxx"

using std::map;
using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::istringstream;

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidationApp(const map<string, vector<string>>& configs);
#endif

int main( int argc, char** argv )
{
  // Select methods (don't look at this code - not of interest)

  if( argc<2 || argc>3 ) {
    std::cout << "Usage: TMVAClassificationApp configName [ DEBUG ]" << std::endl;
    return -1;
  }

  if (argc==3 && string(argv[2]).size()) { DEBUG = 1; }

  auto configs = readConfigs(argv[1]);

  configs["outfileName"] = vector<string>(1, gSystem->BaseName(argv[1]));
  auto pos = configs.at("outfileName").at(0).find(".xml");
  if (pos!=string::npos) { (configs["outfileName"])[0].erase(pos, 4); }
  if (configs.at("outfileName").at(0).size()) { (configs["outfileName"])[0].insert(0, "_"); }
  //  (configs["outputDir"]).at(0) += "/";
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  int code = TMVACrossValidationApp(configs);
#else
  int code = -1;
#endif
  return code;
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidationApp(const map<string, vector<string>>& configs)
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
  TString outfileName = configs.at("outputDir").at(0) + "TMVA" + configs.at("outfileName").at(0) + ".root";
  TFile outputFile( outfileName, "RECREATE" );

  if(DEBUG) { cout << "Output file name: " << outfileName << endl; }

  // save output histogram
  TH3D hMassPtY("hMassPtY", ";Mass;Pt;y", 120, 2.15, 2.45, 50, 0, 10, 20, -2, 2);

  // Create the reader object.
  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

  // variables in training

  vector<vector<TString>> allTrainVars;
  vector<map<string, TString>> trainingVars;
  for (size_t ivar=0; ivar!=configs.at("training_variables").size(); ivar++) {
    const auto & var = configs.at("training_variables").at(ivar);
    const auto pars = getAppPars(var, "training_variables");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    auto v = splitTString(pars.at("eqn_vars"), ":");
    allTrainVars.push_back(v);
    trainingVars.push_back(pars);
  }

  vector<vector<TString>> allSpectatorVars;
  vector<map<string, TString>> spectatorVars;
  for (size_t ivar=0; ivar!=configs.at("spectator_variables").size(); ivar++) {
    const auto & var = configs.at("spectator_variables").at(ivar);
    const auto pars = getAppPars(var, "spectator_variables");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    auto v = splitTString(pars.at("eqn_vars"), ":");

    allSpectatorVars.push_back(v);
    spectatorVars.push_back(pars);
  }


  vector<vector<TString>> allCommonCuts;
  vector<map<string, TString>> commonCuts;
  for (size_t ivar=0; ivar!=configs.at("common_cuts").size(); ivar++) {
    const auto & var = configs.at("common_cuts").at(ivar);
    const auto pars = getAppPars(var, "common_cuts");
    if (DEBUG) { for (const auto& p : pars) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    auto v = splitTString(pars.at("eqn_vars"), ":");

    allCommonCuts.push_back(v);
    commonCuts.push_back(pars);
  }


  MVAHelper helper(trainingVars, spectatorVars, commonCuts);

  for (size_t ivar=0; ivar!=trainingVars.size(); ivar++) {
    const auto& pars = trainingVars.at(ivar);
    reader->AddVariable(pars.at("training_vars"), &helper.vars[ivar]);
  }
  for (size_t ivar=0; ivar!=spectatorVars.size(); ivar++) {
    const auto& pars = spectatorVars.at(ivar);
    // hard code, temporary use
    if (pars.at("spectator_vars") != "eventID := eventID % 8096")
      reader->AddSpectator(pars.at("spectator_vars"), &helper.specs[ivar]);
  }

  // hard code here, temporary use
  int eventID = 0;
  reader->AddSpectator("eventID := eventID % 8096", &eventID);

  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable
  TString dir;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  dir = "dataset/weights";
#else
  dir = "weights";
#endif
   TString prefix = "TMVACrossValidation";
   // Book method(s)
  auto MethodCollection = setupMethodCollection();
  vector<TString> methodNames;
  vector<TString> methodNames_copy;
  for (const auto& m : configs.at("methods")) {
    const auto method = getAppPars(m, "methods");
    if (DEBUG) { for (const auto& p : method) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    TString methodName = method.at("name");
    TString postfix = configs.at("trainXML").at(0);
    auto pos = postfix.Index(".xml");
    postfix.Remove(pos, 4);
    TString weightfile = dir + "_" + postfix + "/" + prefix + TString("_") + methodName + TString(".weights.xml");
    methodNames_copy.push_back(methodName);
    methodName += " method";
    reader->BookMVA( methodName, weightfile );
    methodNames.push_back(methodName);
  }

  // Read data
  // Prepare input tree (this must be replaced by your data source)

  const auto treeInfo = getAppPars((configs.at("treeInfo"))[0], "treeInfo");

  const auto treeDir = treeInfo.at("treeDir");
  const auto inputFileList = treeInfo.at("treeList");
  const auto particle_id = treeInfo.at("pdgId").Atoi();
  Long64_t nentries = treeInfo.at("nentries").Atoll();

  cout << "--- TMVAClassificationApp    : Using input file: " << inputFileList << endl;

  TFileCollection tf("tf", "", inputFileList.Data());
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTreeData p(&t); // temporary use

  outputFile.mkdir(treeDir);
  outputFile.cd(treeDir);
  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  ntp.dropDau = true; // hard code, temporary use
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initMVABranches(methodNames_copy);
  ntp.initNTuple();

  if(nentries < 0) nentries = p.GetEntries();
  // Event loop
  Long64_t num = 0;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    p.GetEntry(ientry);
    const auto recosize = p.cand_mass().size();
    const auto& pdgId = p.cand_pdgId();
  // Prepare the event tree
  // - Here the variable names have to corresponds to your tree
  // - You can use the same variables as above which is slightly faster,
  //   but of course you can use different ones and copy the values inside the event loop
  //
    vector<float> mvaValues(methodNames.size(), -1);
    for (size_t ireco=0; ireco<recosize; ireco++) {
      // begin LambdaC
      if (pdgId[ireco] == std::abs(particle_id)) {
        eventID = (num++) % 8192;
        ntp.retrieveTreeInfo(p, ireco);
        helper.GetValues(ntp, allTrainVars, allSpectatorVars, allCommonCuts);
        bool passCuts = true;
        for (size_t icut=0; icut<helper.nCuts; ++icut) {
          passCuts = passCuts && static_cast<int>(std::round(helper.cuts[icut]));
          if (!passCuts) break;
        }
        if (!passCuts) continue;
        for (size_t i=0; i<methodNames.size(); i++) {
          const auto& methodName = methodNames.at(i);
          mvaValues[i] = reader->EvaluateMVA(methodName);
        }
        ntp.setMVAValues(mvaValues);
        //ntp.fillNTuple();
        hMassPtY.Fill(ntp.cand_mass, ntp.cand_pT, ntp.cand_y);
      }
    }
  }
  // Save the output
  outputFile.Write();

  std::cout << "==> Wrote root file: " << outputFile.GetName() << std::endl;
  std::cout << "==> TMVAClassificationApplication is done!" << std::endl;

  delete reader;
  return 0;
}
#endif
