  
#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

const double pt_bins[] = {2, 3, 4, 5, 6, 8, 10};
const int npt = sizeof(pt_bins) /sizeof(double)- 1;

void getGen(std::string label, std::map<std::string, std::string> names, const double turnpoint = 6)
{
  std::vector<std::string> branches = {"gen_y", "gen_pT", "gen_dau0_eta", "gen_dau1_eta", "gen_dau0_pT", "gen_dau1_pT"};

  std::string ycut = "abs(gen_y)<1";

  // all of candidates
  std::cout << label << ": before filtering\n";

  auto dfLowPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", names.at("lowPt"), branches);
  auto dfLowPtMidY = dfLowPt.Filter(ycut).Filter("gen_pT<" + std::to_string(turnpoint));
  auto dfHighPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", names.at("highPt"), branches);
  auto dfHighPtMidY = dfHighPt.Filter(ycut).Filter("gen_pT>" + std::to_string(turnpoint));

  auto hlow_pt_all = dfLowPtMidY.Histo1D({"hLowPtAll", ";p_{T};counts", npt, pt_bins}, "gen_pT");
  auto hhigh_pt_all = dfHighPtMidY.Histo1D({"hHighPtAll", ";p_{T};counts", npt, pt_bins}, "gen_pT");

  TH1D* hAll = (TH1D*)hlow_pt_all->Clone("hAll");
  hAll->Add(hhigh_pt_all.GetPtr());

  std::cout << label << ": after filtering\n";
  // daughter kinematics
  std::string dauCuts = "abs(gen_dau0_eta) < 2.4 && abs(gen_dau1_eta) < 2.4 && gen_dau1_pT>0.3";
  auto dauPassLowPt = dfLowPtMidY.Filter(dauCuts);
  auto dauPassHighPt = dfHighPtMidY.Filter(dauCuts);

  auto hlow_pt_pass = dauPassLowPt.Histo1D({"hLowPtPass", ";p_{T};counts", npt, pt_bins}, "gen_pT");
  auto hhigh_pt_pass = dauPassHighPt.Histo1D({"hHighPtPass", ";p_{T};counts", npt, pt_bins}, "gen_pT");

  TH1D* hPass = (TH1D*)hlow_pt_pass->Clone("hPass");
  hPass->Add(hhigh_pt_pass.GetPtr());

  TFile ofile(::Form("gen_%s.root", label.c_str()), "recreate");
  hAll->Write();
  hPass->Write();
  TGraphAsymmErrors g(hAll->GetNbinsX());
  g.Divide(hPass, hAll);
  g.Write("gAcc");
}

void getGen_pPb()
{
  std::map<std::string, std::string> names;
  names["lowPt"] = "/storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/MC_pPb_lambdacAna_mc_AllEntries_Gen.root";
  names["highPt"] = "/storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/MC_pPb_pT5p9_lambdacAna_mc_AllEntries_Gen.root";
  getGen("pPb", names);
}

void getGen_Pbp()
{
  std::map<std::string, std::string> names;
  names["lowPt"] = "/storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/MC_Pbp_lambdacAna_mc_AllEntries_Gen_etaFlipped.root";
  names["highPt"] = "/storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/MC_Pbp_pT5p9_lambdacAna_mc_AllEntries_Gen_etaFlipped.root";
  getGen("Pbp", names);
}


void getRecoEachPt(TH1* h, std::string mvaName, std::string fileName, const double pTMin, const double pTMax, const double mvaCutVal)
{
  std::string kinematic = ::Form("cand_pT>%.3f && cand_pT<%.3f && abs(cand_y)<1.0", pTMin, pTMax);
  std::string mvaCut = ::Form("%s > %f", mvaName.c_str(), mvaCutVal);
  std::vector<std::string> branches = {"cand_pT", "cand_y", mvaName};
  auto df = ROOT::RDataFrame("lambdacAna_mc/ParticleNTuple", fileName, branches);

  auto hpass = df.Filter(kinematic).Filter(mvaCut).Histo1D({"pass", "", npt, pt_bins}, "cand_pT");

  h->Add(hpass.GetPtr());
}

void getReco(std::string boost, std::string mult)
{
  // std::string config = 
  std::string label = boost == "pPb" ? "" : "_etaFlipped";
  std::string ntrk = mult == "HM" ? "Ntrk185to250" : "Ntrk0to35";
  TFile ofile(::Form("reco_%s_%s.root", mult.c_str(), boost.c_str()), "recreate");
  ofile.cd();
  TH1D h("hpass", ";p_{T} (GeV);counts", npt, pt_bins);
  const std::string path = gSystem->ExpandPathName("${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_unweighted/");
  for (int ipt=0; ipt<npt; ++ipt) {
    if (ipt == 0 && mult == "HM") continue;
    std::string configName = ::string_format("%s/pT%.0fto%.0f_%s.conf", path.c_str(), pt_bins[ipt], pt_bins[ipt+1], ntrk.c_str());
    std::cout << configName <<"\n";
    FitParConfigs configs(configName);
    const auto cutConfigs = configs.getCutConfigs();
    const auto inputConfigs = configs.getInputConfigs();
    const int index = boost == "pPb" ? 0 : 1;
    std::string fileMC = inputConfigs.getPaths("MC").at(index);
    const VarCuts mycuts(cutConfigs);
    std::string fileName = fileMC;
    std::cout << fileName << "\n";
    getRecoEachPt(&h, mycuts._mvaName, fileName, mycuts._pTMin, mycuts._pTMax, mycuts._mvaCut);
  }
  ofile.cd();
  h.Write();
}

void getReco()
{
  for (const auto boost : {"pPb", "Pbp"}) {
    for (const auto mult : {"HM", "MB"}) {
      getReco(boost, mult);
    }
  }
}

void getCounts()
{
  getGen_pPb();
  getGen_Pbp();
  getReco();
}
