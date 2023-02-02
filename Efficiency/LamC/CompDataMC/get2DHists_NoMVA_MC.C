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
 * Used inside get_hists before Histo2D.
 * In the configuration, you must spell the cut variable as var::cut_var,
 * e.g., ProtonPt::cand_etaDau1, -1, 1, float.
 * var can only be ProtonPt, ProtonP, ProtonEta, ProtonDeDx, CosAngle
 * cut_var can only be cand_pTDau1, cand_etaDau1, trk_dau1_dEdxRes, CosAngle 
 */
ROOT::RDF::RNode apply_filters(ROOT::RDF::RNode df, const FitParConfigs& configs, const std::string& var)
{
  const auto cutConfigs = configs.getCutConfigs();
  // only suport float
  auto construct_filter = [&cutConfigs] (std::string name, std::string cut_var) {
    name += "::" + cut_var;
    try {
      const auto lower = cutConfigs.getFloatMin(name);
      const auto upper = cutConfigs.getFloatMax(name);
      return ::string_format("%f < %s && %s < %f", lower, cut_var.c_str(), cut_var.c_str(), upper);
    } catch (std::out_of_range& e) {
      return std::string{}; 
    }
  };
  // enumerate possibility
  const std::vector<std::string> cut_vars{"cand_pTDau1", "cand_etaDau1", "trk_dau1_dEdxRes", "CosAngle"};
  for (const auto& cut_var : cut_vars) {
    auto my_filter = construct_filter(var, cut_var);
    if (my_filter.empty()) continue;
    df = df.Filter(my_filter);
  }
  
  return ROOT::RDF::RNode(df);
}

/**
 * Return the node of MC samples. Simple cuts on pT and y applied.
 * New branches, CosAngle, proton_p are created.
 */
ROOT::RDF::RNode get_node_MC(FitParConfigs configs, int index)
{
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  // MC
  // awaiting
  std::string cuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax);
  std::string cuts_mc = cuts + " && cand_matchGEN > 0.5";

  std::string fileMC = inputConfigs.getPaths("MC").at(index);
  std::string treeMC = inputConfigs.getName("MC");

  ROOT::RDataFrame dfMC(treeMC, fileMC);
  auto dfSkimMC = dfMC.Filter(cuts_mc)
    .Define("CosAngle", [](const float angle) {return std::cos(angle); }, {"cand_angle3D"})
    .Define("proton_p", [](const float pt, const float eta) {return pt*std::cosh(eta); }, {"cand_pTDau1", "cand_etaDau1"});

  std::cout << "Booked cuts " << cuts_mc << " for MC\n";
  // std::cout << "Prepared MC\n";

  return dfSkimMC;
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
    // proton pT vs mass
    hists["ProtonPtVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonPt")
                                    .Histo2D({("ProtonPtVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton Pt", key.c_str()),
                                        160, 2.08, 2.48, 100, 0, 10},
                                                   "cand_mass", "cand_pTDau1", "weight")});
    // proton p vs mass
    hists["ProtonPVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonP")
                                    .Histo2D({("ProtonPVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton P", key.c_str()),
                                        160, 2.08, 2.48, 100, 0, 10},
                                                   "cand_mass", "proton_p", "weight")});
    // proton eta vs mass
    hists["ProtonEtaVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonEta")
                                    .Histo2D({("ProtonEtaVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton Eta", key.c_str()),
                                        160, 2.08, 2.48, 48, -2.4, 2.4},
                                                    "cand_mass", "cand_etaDau1", "weight")});
    // proton dE/dx vs mass
    hists["ProtonDeDxVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonDeDx")
                                    .Histo2D({("ProtonDeDxVsMass"+postfix).c_str(), ::Form("%s;Mass;Normalized proton dE/dx residual", key.c_str()),
                                        160, 2.08, 2.48, 50, -0.25, 0.25},
                                                    "cand_mass", "trk_dau1_dEdxRes", "weight")});
    // cosine angle
    hists["CosAngleVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "CosAngle")
                                    .Histo2D({("CosAngleVsMass"+postfix).c_str(), ::Form("%s;Mass;Cos Angle", key.c_str()),
                                        160, 2.08, 2.48, 200, -1, 1},
                                                    "cand_mass", "CosAngle", "weight")});
  } else {
    // proton pT vs mass
    hists["ProtonPtVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonPt")
                                    .Histo2D({("ProtonPtVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton Pt", key.c_str()),
                                        160, 2.08, 2.48, 100, 0, 10},
                                                   "cand_mass", "cand_pTDau1")});
    // prton p vs mass
    hists["ProtonPVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonP")
                                    .Histo2D({("ProtonPVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton P", key.c_str()),
                                        160, 2.08, 2.48, 100, 0, 10},
                                                   "cand_mass", "proton_p")});
    // proton eta vs mass
    hists["ProtonEtaVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonEta")
                                    .Histo2D({("ProtonEtaVsMass"+postfix).c_str(), ::Form("%s;Mass;Proton Eta", key.c_str()),
                                        160, 2.08, 2.48, 48, -2.4, 2.4},
                                                    "cand_mass", "cand_etaDau1")});
    // proton dE/dx vs mass
    hists["ProtonDeDxVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "ProtonDeDx")
                                    .Histo2D({("ProtonDeDxVsMass"+postfix).c_str(), ::Form("%s;Mass;Normalized proton dE/dx residual", key.c_str()),
                                        160, 2.08, 2.48, 50, -0.25, 0.25},
                                                    "cand_mass", "trk_dau1_dEdxRes")});
    // cosine angle
    hists["CosAngleVsMass"].insert({key,
                                    apply_filters(dfSkim, configs, "CosAngle")
                                    .Histo2D({("CosAngleVsMass"+postfix).c_str(), ::Form("%s;Mass;Cos Angle", key.c_str()),
                                        160, 2.08, 2.48, 200, -1, 1},
                                                    "cand_mass", "CosAngle")});
  }
}

// pt -> pPb/Pbp -> <name, 2D hist>
template<class It_pt, class It_Ntrk>
void  get2D_NoMVA(It_pt pts_beg, It_pt pts_end,
    It_Ntrk Ntrks_beg, It_Ntrk Ntrks_end)
{
  ROOT::EnableImplicitMT();

  std::vector<std::map<std::string, TH2DMap > >  vec_hist_maps;
  std::vector<std::string> output_file_names;
  // data
  std::vector<std::string> vars {"cand_pT", "cand_y", "trk_dau1_dEdxRes",
                                 "cand_angle3D", "cand_etaDau1", "cand_pTDau1",
                                  "cand_mass", "trigBit_2", "trigBit_4", "cand_Ntrkoffline"};
  FitParConfigs configs("configs/no_mva.conf");
  auto inputConfigs = configs.getInputConfigs();

  std::vector<std::string> files;
  for (const auto& l : inputConfigs.getPaths("Data")) {
    std::string f;
    std::ifstream ll(l);
    while(std::getline(ll, f)){
      if (f.at(0) == '#') continue;
      files.push_back(f);
    }
  }

  ROOT::RDataFrame df_data("lambdacAna/ParticleNTuple", files, vars);

  auto nevents = *df_data.Count();
  auto nevents_1percent = nevents / 100;

  const std::vector<float> pts(pts_beg, pts_end);
  const auto nPt = pts.size() - 1;
  const std::vector<int> Ntrkofflines(Ntrks_beg, Ntrks_end);
  const int nNtrkofflines = Ntrkofflines.size() - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      // prepare names of configurations
      if (ipt ==0 && Ntrkofflines[itrk+1] == 250) continue;
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

      // prepare nodes
      auto node_MC_pPb   = get_node_MC(configs, 0);
      auto node_MC_Pbp   = get_node_MC(configs, 1);

      // prepare hists
      std::map<std::string, TH2DMap > hists;
      get_hists(node_MC_pPb, hists, "pPb", false, configs);
      get_hists(node_MC_Pbp, hists, "Pbp", false, configs);
      std::cout << "Prepared nodes" << " for pT " << mycuts._pTMin << " " << mycuts._pTMax << "\n";

      // collect hists
      vec_hist_maps.push_back(hists);

      // parepare output
      const auto output = ::getNames(configs, mycuts);
      std::string ofileName = output.at("fileName");
      auto index = ofileName.find(".root");
      ofileName.replace(index, 5, "_mc.root");

      // collect output names
      output_file_names.push_back(ofileName);
    }
  }

  // combine
  std::map<std::string, std::vector<double>> lumis;
  lumis["HM"] = {28917.856684357, 68876.167756928}; // ub
  // lumis["HM"] = {68876.167756928, 28917.856684357}; // ub
  lumis["MB"] = {1113.004725970, 3103.762734886}; // ub
  // lumis["MB"] = {3103.762734886, 1113.004725970}; // ub

  std::cout << "Starting combining\n";

  std::vector<std::map<std::string, TH2D* > > combined_hists;
  for (auto& hists : vec_hist_maps) {
    combined_hists.emplace_back();
    for (auto& hist : hists) {
      const auto key = hist.first;
      auto val_pPb = hist.second.at("pPb");
      auto val_Pbp = hist.second.at("Pbp");
      val_pPb->Sumw2();
      val_Pbp->Sumw2();
      val_pPb->Scale(lumis["MB"].at(0)/val_pPb->Integral());
      val_Pbp->Scale(lumis["MB"].at(1)/val_Pbp->Integral());
      auto h = (TH2D*) val_pPb->Clone();
      std::string name {h->GetName()};
      auto index = name.find("pPb");
      name.replace(index, 3, "MC");
      h->SetName(name.c_str());
      h->Add(val_Pbp.GetPtr());
      h->SetDirectory(0);
      combined_hists.back()[key] = h;
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
  std::cout << "Will execute with " << nthreads << " threads\n";


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
    auto c_hists = combined_hists.at(i);
    ofile.cd();
    for (auto& e : hists) { for (auto& ee : e.second) ee.second->Write(); }
    for (auto& e : c_hists) { e.second->Write(); }
  }
  std::cout << "Finished\n";
  sw.Print();

}

void get2DHists_NoMVA_MC()
{
  // const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  // const int Ntrkofflines[] = {0, 185};
  // std::vector<float> pts{6, 8, 10};
  // std::vector<float> pts{2, 3, 4, 5, 6, 8, 10};
  std::vector<float> pts{4, 5};
  // std::vector<float> pts{8, 10};
  std::vector<float> Ntrkofflines{0, 185};
  get2D_NoMVA(pts.begin(), pts.end(), Ntrkofflines.begin(), Ntrkofflines.end());
}

