// Original code can be found on 
// https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/TMVAClassification.C
// Modified by Yousen Zhang, Rice University, US
// Data: 2020 August 27

// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 * You can also compile and run the example with the following commands           *
 *                                                                                *
 *    make                                                                        *
 *    ./TMVAClassification <Methods>                                              *
 *                                                                                *
 * where: <Methods> = "method1 method2"                                           *
 *        are the TMVA classifier names                                           *
 *                                                                                *
 * example:                                                                       *
 *    ./TMVAClassification Fisher LikelihoodPCA BDT                               *
 *                                                                                *
 * If no method given, a default set is of classifiers is used                    *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <memory>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "THashList.h"
#include "TFileCollection.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
//#include "TMVA/TMVAGui.h"

int TMVAClassification( TString signalFileList, TString backgroundFileList, TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   //
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 1; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   // 
   // --- Boosted Decision Trees
   Use["BDT"]             = 0; // uses Adaptive Boost
   Use["BDTG"]            = 1; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return 1;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

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

   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

   // Lc topo-info
   //factory->AddVariable( "cand_vtxChi2", "chi2 of vertex fit of Lc", "",   'F');
   factory->AddVariable( "cand_vtxProb", "vertex fit probability of Lc", "",   'F');

   //factory->AddVariable( "cand_angle2D", "2D pointing angle of Lc", "rad", 'F');
   factory->AddVariable( "cand_angle3D", "3D pointing angle of Lc", "rad", 'F');

   //factory->AddVariable( "cand_decayLength2D", "2D decay length w.r.t PV of Lc", "cm", 'F');
   factory->AddVariable( "cand_decayLength3D", "3D decay length w.r.t PV of Lc", "cm", 'F');

   //factory->AddVariable( "cand_dlsig2D := cand_decayLength2D/cand_decayLengthError2D", 
       //"2D decay length significance w.r.t PV of Lc", "", 'F');
   factory->AddVariable( "cand_dlsig3D := cand_decayLength3D/cand_decayLengthError3D", 
       "3D decay length significance w.r.t PV of Lc", "", 'F');

   //factory->AddVariable( "cand_pseudoDecayLengthError2D", "2D pseudo decay length error w.r.t PV of Lc", "", 'F');
   //factory->AddVariable( "cand_pseudoDecayLengthError3D", "3D pseudo decay length error w.r.t PV of Lc", "", 'F');

   // Lc daughters' kinematics
   factory->AddVariable( "cand_dau_eta_diff := cand_dau0_eta - cand_dau1_eta", "Eta difference between Ks and proton", "", 'F');
   factory->AddVariable( "cand_dau_eta_sum  := cand_dau0_eta + cand_dau1_eta", "Eta sum of Ks and proton",             "", 'F');
   factory->AddVariable( "cand_dau_pT_imba := (cand_dau0_pT - cand_dau1_pT)/(cand_dau0_pT + cand_dau1_pT)", 
       "imbalance between daughters' pT of Lc", "", 'F');

   // Ks topo-info
   //factory->AddVariable( "cand_dau0_vtxChi2", "chi2 of vertex fit of Ks from Lc", "",   'F');
   factory->AddVariable( "cand_dau0_vtxProb", "vertex fit probability of Ks from Lc", "",   'F');
   //factory->AddVariable( "cand_dau0_angle2D", "2D pointing angle of Ks from Lc", "rad", 'F');
   factory->AddVariable( "cand_dau0_angle3D", "3D pointing angle of Ks from Lc", "rad", 'F');

   //factory->AddVariable( "cand_dau0_decayLength2D", "2D decay length w.r.t PV of Ks from Lc", "cm", 'F');
   factory->AddVariable( "cand_dau0_decayLength3D", "3D decay length w.r.t PV of Ks from Lc", "cm", 'F');

   //factory->AddVariable( "cand_dau0_dlsig2D := cand_dau0_decayLength2D/cand_dau0_decayLengthError2D", 
       //"2D decay length significance w.r.t PV of Ks from Lc", "", 'F');
   factory->AddVariable( "cand_dau0_dlsig3D := cand_dau0_decayLength3D/cand_dau0_decayLengthError3D", 
       "3D decay length significance w.r.t PV of Ks from Lc", "", 'F');

   //factory->AddVariable( "cand_dau0_pseudoDecayLengthError2D", "2D pseudo decay length error w.r.t PV of Ks from Lc", "", 'F');
   //factory->AddVariable( "cand_dau0_pseudoDecayLengthError3D", "3D pseudo decay length error w.r.t PV of Ks from Lc", "", 'F');

   factory->AddVariable( "cand_dau0_dca", "DCA between pions from Ks", "cm", 'F');

   // Ks daughters' kinematics
   factory->AddVariable( "cand_dau0_etaDau_diff := cand_gdau0_eta - cand_dau1_eta", "Eta difference between pions from Ks", "", 'F');
   factory->AddVariable( "cand_dau0_etaDau_sum  := cand_gdau0_eta + cand_dau1_eta", "Eta sum of pions from Ks",             "", 'F');

   factory->AddVariable( "cand_dau0_pTdau_imba := (cand_gdau0_pT - cand_gdau1_pT)/(cand_gdau0_pT + cand_gdau1_pT)", 
       "imbalance between daughters' pT of Ks from Lc", "", 'F');

   // proton info
   factory->AddVariable( "cand_gdau1_p", "momentum of proton", "cm", 'F');
   // dedx awaiting

   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables
   //factory->AddSpectator( "spec1 := var1*2",  "Spectator 1", "units", 'F' );
   //factory->AddSpectator( "spec2 := var1*3",  "Spectator 2", "units", 'F' );
   factory->AddSpectator( "cand_mass",  "Lc mass", "GeV", 'F' );
   factory->AddSpectator( "cand_eta",  "Lc eta", "", 'F' );
   //factory->AddSpectator( "cand_dau0_mass",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_massDau0",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_massDau1",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_eta",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_etaDau0",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_etaDau1",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_pT",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_pTDau0",  "TBD?", "GeV", 'F' );
   //factory->AddSpectator( "cand_dau0_pTDau1",  "TBD?", "GeV", 'F' );

   // Read training and test data
   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
   //TString fname = "./tmva_class_example.root";
   
   //if (gSystem->AccessPathName( fname ))  // file does not exist in local directory
      //gSystem->Exec("curl -O http://root.cern.ch/files/tmva_class_example.root");
   
   //TFile *input = TFile::Open( fname );
   
   //std::cout << "--- TMVAClassification       : Using input file: " << input->GetName() << std::endl;
   std::cout << "--- TMVAClassification       : Using signal file list: " << signalFileList << std::endl;
   std::cout << "--- TMVAClassification       : Using background file list: " << backgroundFileList << std::endl;
   
   // --- Register the training and test trees
   TFileCollection fcSignal("signalFileCollection", "", signalFileList.Data());
   TFileCollection fcBackground("backgroundFileCollection", "", backgroundFileList.Data());

   //TTree *signal     = (TTree*)input->Get("TreeS");
   //TTree *background = (TTree*)input->Get("TreeB");
   auto signal = std::make_shared<TChain>("lambdacAna_mc/ParticleNTuple");
   auto background = std::make_shared<TChain>("lambdacAna_mc/ParticleNTuple");

   signal->AddFileInfoList(fcSignal.GetList());
   background->AddFileInfoList(fcBackground.GetList());
   
   // global event weights per tree (see below for setting event-wise weights)
   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;
   
   // You can add an arbitrary number of signal or background trees
   factory->AddSignalTree    ( signal.get(),     signalWeight     );
   factory->AddBackgroundTree( background.get(), backgroundWeight );
   
   // To give different trees for training and testing, do as follows:
   //    factory->AddSignalTree( signalTrainingTree, signalTrainWeight, "Training" );
   //    factory->AddSignalTree( signalTestTree,     signalTestWeight,  "Test" );
   
   // Use the following code instead of the above two or four lines to add signal and background
   // training and test events "by hand"
   // NOTE that in this case one should not give expressions (such as "var1+var2") in the input
   //      variable definition, but simply compute the expression before adding the event
   //
   //     // --- begin ----------------------------------------------------------
   //     std::vector<Double_t> vars( 4 ); // vector has size of number of input variables
   //     Float_t  treevars[4], weight;
   //     
   //     // Signal
   //     for (UInt_t ivar=0; ivar<4; ivar++) signal->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<signal->GetEntries(); i++) {
   //        signal->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < signal->GetEntries()/2.0) factory->AddSignalTrainingEvent( vars, signalWeight );
   //        else                              factory->AddSignalTestEvent    ( vars, signalWeight );
   //     }
   //   
   //     // Background (has event weights)
   //     background->SetBranchAddress( "weight", &weight );
   //     for (UInt_t ivar=0; ivar<4; ivar++) background->SetBranchAddress( Form( "var%i", ivar+1 ), &(treevars[ivar]) );
   //     for (UInt_t i=0; i<background->GetEntries(); i++) {
   //        background->GetEntry(i);
   //        for (UInt_t ivar=0; ivar<4; ivar++) vars[ivar] = treevars[ivar];
   //        // add training and test events; here: first half is training, second is testing
   //        // note that the weight can also be event-wise
   //        if (i < background->GetEntries()/2) factory->AddBackgroundTrainingEvent( vars, backgroundWeight*weight );
   //        else                                factory->AddBackgroundTestEvent    ( vars, backgroundWeight*weight );
   //     }
         // --- end ------------------------------------------------------------
   //
   // --- end of tree registration 

   // Set individual event weights (the variables must exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression    ("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
   //    factory->SetBackgroundWeightExpression( "weight" );

   // Apply additional cuts on the signal and background samples (can be different)
   std::string common_cuts = "cand_dau0_pdgId == 2212 && cand_dau1_pdgId == 310 && abs(trk_gdau0_zDCASignificance)>1 && abs(trk_gdau1_zDCASignificance)>1";
   TCut mycuts( (common_cuts+"&& cand_matchGEN").c_str() ); // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb( (common_cuts + "&& abs(cand_mass-2.29)>0.15").c_str() ); // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the factory how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut,
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );
   factory->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V" );

   // ---- Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   if (Use["MLP"])
      factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

   if (Use["MLPBFGS"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

   if (Use["MLPBNN"])
      factory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

   // CF(Clermont-Ferrand)ANN
   if (Use["CFMlpANN"])
      factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  

   // Tmlp(Root)ANN
   if (Use["TMlpANN"])
      factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

   // Boosted Decision Trees
   if (Use["BDTG"]) // Gradient Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );

   if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTB"]) // Bagging
      factory->BookMethod( TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
      factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
                           "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );

   // For an example of the category classifier usage, see: TMVAClassificationCategory

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   // factory->OptimizeAllMethods("SigEffAt001","Scan");
   // factory->OptimizeAllMethods("ROCIntegral","FitGA");

   // --------------------------------------------------------------------------------------------------

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

   // Launch the GUI for the root macros
   //if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );

   return 0;
}

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList; 
   if( argc<3 ) {
     std::cerr << "[ERROR] Users must give both signal file list and background file list!" << std::endl;
     std::cout << "Usage: tmvaClassfication signalFileList outputFileList [ options ] method1 method2 ..." << std::endl;
     std::cout << "Options:" << std::endl;
     std::cout << "  -b, --batch :    run without graphics"<< std::endl;
     return -1;
   }

   TString signalFileList(argv[1]);
   TString backgroundFileList(argv[2]);

   for (int i=3; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(","); 
      methodList += regMethod;
   }
   return TMVAClassification(signalFileList, backgroundFileList, methodList); 
}
