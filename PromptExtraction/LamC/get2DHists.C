#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TChain.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"
#include "TSystem.h"

#include "ROOT/RDataFrame.hxx"

#include <memory>
#include <string>
#include <stdexcept>

using TH2DMap = std::map<std::string, ROOT::RDF::RResultPtr<::TH2D > >;

/**
 * Return the node of MC samples. Simple cuts on pT and y applied.
 * New branches, DCA are created.
 */
ROOT::RDF::RNode get_node_MC(FitParConfigs configs, const std::string mc_type)
{
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  // MC
  // awaiting
  std::string cuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f && %s > %f",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax, mycuts._mvaName.c_str(), mycuts._mvaCut);
  std::string cuts_mc = cuts + " && cand_matchGEN > 0.5";

  const auto fileMC = inputConfigs.getPaths(mc_type);
  std::string treeMC = inputConfigs.getName(mc_type);

  ROOT::RDataFrame dfMC(treeMC, fileMC);
  auto dfSkimMC = dfMC.Filter(cuts_mc)
    .Define("DCA", [](const float dl, const float angle) {return dl*std::sin(angle); }, {"cand_decayLength3D", "cand_angle3D"});

  std::cout << "Booked cuts " << cuts_mc << " for MC\n";
  // std::cout << "Prepared MC\n";

  return dfSkimMC;
}

/**
 * Return the node of data. Simple cuts on pT and y applied.
 * New branches, CosAngle, proton_p are created.
 * The initial input must be given as the second parameter.
 */
ROOT::RDF::RNode get_node_Data(FitParConfigs configs, ROOT::RDF::RNode df)
{
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  std::cout << cutConfigs.getMvaName() << std::endl;

  const auto output = ::getNames(configs, mycuts);

  const bool useWeight = mycuts._useWeight;
  TGraph* g(0);
  EfficiencyTable<TGraph> effTab(g);
  try {
    const auto effFileName = inputConfigs.getPaths("MultEff").front();
    TFile fEff(effFileName.c_str());
    const auto effTabName = inputConfigs.getName("MultEff");
    TGraph* g = (TGraph*) fEff.Get(effTabName.c_str());
    effTab.setTable(g);
    g = nullptr;
    // useWeight = true;
  } catch (std::out_of_range& e) {
    std::cout << "Efficiency Table is not found. Do not use weight then.\n";
  }
  if (!g && !useWeight)
    std::cout << "MultEff option will be ignored because useWeight is disable.\n";

  int effTypeCode = 0;
  try {
   effTypeCode = std::stoi(output.at("effType"));
  } catch (std::out_of_range& e) {}
  const auto effType = EfficiencyTable<TGraph>::Value(effTypeCode);

  // data
  std::string cuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f && %s > %f",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax, mycuts._mvaName.c_str(), mycuts._mvaCut);
  std::string cuts_data = cuts;
  // event level
  // Ntrkoffline
  // multiplicity cuts
  // cuts_data += ::Form(" && Ntrkoffline >= %d && Ntrkoffline < %d", mycuts._NtrkofflineMin, mycuts._NtrkofflineMax);
  cuts_data += ::Form(" && cand_Ntrkoffline >= %d && cand_Ntrkoffline < %d", mycuts._NtrkofflineMin, mycuts._NtrkofflineMax);
  // HM trigger
  if (mycuts._useHM) cuts_data += " && trigBit_2 > 0.5";
  // MB trigger
  if (mycuts._useMB) cuts_data += " && trigBit_4 > 0.5";
  // pileup filter
  if (mycuts._usedz1p0) cuts_data += " && filterBit_4 > 0.5";
  // pileup filter
  if (mycuts._usegplus) cuts_data += " && filterBit_5 > 0.5";
  std::cout << "Booked cuts " << cuts_data << " for data\n";

  auto dfSkim = 
    mycuts._useWeight ? 
    df.Filter(cuts_data)
    .Define("weight", [&effTab, &effType](const unsigned short ntrk) { return effTab.getWeight(ntrk, effType); }, {"Ntrkoffline"})
    .Define("DCA", [](const float dl, const float angle) {return dl*std::sin(angle); }, {"cand_decayLength3D", "cand_angle3D"})
    : df.Filter(cuts_data)
    .Define("DCA", [](const float dl, const float angle) {return dl*std::sin(angle); }, {"cand_decayLength3D", "cand_angle3D"});

  return ROOT::RDF::RNode(dfSkim);

}

/**
 * Insert <key, 2D histogram> to the second input parameter hists.
 * The third parameter key can only be data or MC.
 */
void get_hists(ROOT::RDF::RNode dfSkim,
    std::map<std::string, TH2DMap>& hists, const std::string& key,
    const bool useWeight, const FitParConfigs& configs)
{
  const std::string postfix = key;
  if (useWeight) {
    // DCA vs mass
    hists["DCAVsMass"].insert({key, dfSkim
                                    .Histo2D({("DCAVsMass"+postfix).c_str(), ::Form("%s;Mass;DCA", key.c_str()),
                                        160, 2.08, 2.48, 400, 0, 0.2},
                                                   "cand_mass", "DCA", "weight")});
  } else {
    hists["DCAVsMass"].insert({key, dfSkim
                                    .Histo2D({("DCAVsMass"+postfix).c_str(), ::Form("%s;Mass;DCA", key.c_str()),
                                        160, 2.08, 2.48, 400, 0, 0.2},
                                                   "cand_mass", "DCA")});
  }
}

// pt -> data/MC -> <name, 2D hist>
template<class It_pt, class It_Ntrk>
void  get2D_NoMVA(It_pt pts_beg, It_pt pts_end,
    It_Ntrk Ntrks_beg, It_Ntrk Ntrks_end)
{
  ROOT::EnableImplicitMT();

  std::vector<std::map<std::string, TH2DMap > >  vec_hist_maps;
  std::vector<std::string> output_file_names;
  // data

  const std::vector<float> pts(pts_beg, pts_end);
  const auto nPt = pts.size() - 1;
  const std::vector<int> Ntrkofflines(Ntrks_beg, Ntrks_end);
  const int nNtrkofflines = Ntrkofflines.size() - 1;

  ULong64_t nevents_1percent = ULong64_t(-1);

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      // prepare names of configurations
      if (std::abs(pts[ipt]-2)<1E-3 && Ntrkofflines[itrk+1] == 250) continue;
      std::string configName = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      if (Ntrkofflines[itrk+1] == 250)
        configName = "configs/" + configName + "_HM.conf";
      else
        configName = "configs/" + configName + "_MB.conf";
      configName = gSystem->ExpandPathName(configName.c_str());
      std::cout << "Reading " << configName << "\n";

      // prepre configurations
      FitParConfigs configs(configName.c_str());
      auto inputConfigs = configs.getInputConfigs();
      auto outputConfigs = configs.getOutputConfigs();
      const auto cutConfigs = configs.getCutConfigs();
      const VarCuts mycuts(cutConfigs);

      std::cout << mycuts._mvaName << std::endl;

      std::vector<std::string> vars {"cand_pT", "cand_y",
                                 "cand_angle3D", "cand_decayLength3D", 
                                  "cand_mass", "trigBit_2", "trigBit_4", "cand_Ntrkoffline",
                                  mycuts._mvaName};

      ROOT::RDataFrame df_data("lambdacAna/ParticleNTuple", inputConfigs.getPaths("Data"), vars);

      auto nevents = *df_data.Count();
      nevents_1percent = nevents / 100;

      // prepare nodes
      auto node_data = get_node_Data(configs, df_data);
      auto node_MC_p = get_node_MC(configs, "PromptMC");
      auto node_MC_np = get_node_MC(configs, "NonPromptMC");

      // prepare hists
      std::map<std::string, TH2DMap > hists;
      get_hists(node_data, hists, "data", mycuts._useWeight, configs);
      get_hists(node_MC_p,   hists, "PromptMC",   false,             configs);
      get_hists(node_MC_np,   hists, "NonPromptMC",   false,             configs);
      std::cout << "Prepared nodes" << " for pT " << mycuts._pTMin << " " << mycuts._pTMax << "\n";

      // collect hists
      vec_hist_maps.push_back(hists);

      // parepare output
      const auto output = ::getNames(configs, mycuts);
      std::string ofileName = output.at("fileName");

      // collect output names
      output_file_names.push_back(ofileName);
    }
  }

  // execution
  int i=0; 
  auto nthreads = ROOT::GetThreadPoolSize();
  TStopwatch sw;
  auto print_progress = [&i, &sw, nthreads](TH2D& h) {
    std::cout << "Processed " << ++i * nthreads << " percents of ProtonPtVsMass. ";
    std::cout << "Elapsed " << sw.RealTime() << " seconds\n";
    sw.Continue();
  };
  std::cout << "Will execut with " << nthreads << " threads\n";
  vec_hist_maps.front().at("DCAVsMass").at("data").OnPartialResult(nevents_1percent, print_progress);
  // trigger the execution
  // vec_hist_maps.front().at("ProtonPtVsMass").at("data")->GetEntries();

  const auto n_output = output_file_names.size();
  for (size_t i=0; i<n_output; ++i) {
    auto ofileName = output_file_names.at(i);
    // I do not know to write using xroot is safe or not
    // I disable xrootd by creating an object instead of using pointer.
    // remember that the AccessPathName returns false it the path exits!
    ofileName = gSystem->BaseName(ofileName.c_str());
    ofileName = "output/" + ofileName;
    const auto out_dir = gSystem->DirName(ofileName.c_str());
    if (gSystem->AccessPathName(out_dir)) {
      gSystem->mkdir(out_dir, kTRUE);
    }
    TFile ofile(ofileName.c_str(), "recreate");

    auto hists = vec_hist_maps.at(i);
    ofile.cd();
    for (auto& e : hists) { for (auto& ee : e.second) ee.second->Write(); }
    for (auto& e : hists) {
      for (auto& ee : e.second) {
        std::string temp = ofileName;
        auto index = temp.find(".root");
        if (index != std::string::npos) {
          temp.replace(index, 5, "_"+e.first+"_"+ee.first+".dot");
        }
        // not thread-safe
        // ROOT::RDF::SaveGraph(ee.second, temp);
      }
    }
  }
  std::cout << "Finished\n";
  sw.Print();

}

void get2DHists()
{
  // const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  // const int Ntrkofflines[] = {0, 185};
  // std::vector<float> pts{6, 8, 10};
  std::vector<float> pts{2, 3, 4, 5, 6, 8, 10};
  // std::vector<float> pts{8, 10};
  // std::vector<float> pts{6, 8};
  std::vector<float> Ntrkofflines{0, 185};
  get2D_NoMVA(pts.begin(), pts.end(), Ntrkofflines.begin(), Ntrkofflines.end());
}


void get2DHists(const float pTMin, const float pTMax)
{
  std::vector<float> pts{pTMin, pTMax};
  std::vector<float> Ntrkofflines{0, 185};
  get2D_NoMVA(pts.begin(), pts.end(), Ntrkofflines.begin(), Ntrkofflines.end());
}

