#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RResultPtr.hxx"
#include "TSystem.h"
#include "TFile.h"

#include <vector>

const double pt_bins[] = {2, 3, 4, 5, 6, 8, 10};
const int npt = sizeof(pt_bins) /sizeof(double)- 1;

const double pt_low = 2;
const double pt_upper = 10;
const int nbins = 80;

void getGen(std::string label, std::map<std::string, std::string> names, const double turnpoint = 6)
{
  std::vector<std::string> branches = {"gen_y", "gen_pT", "gen_dau0_eta", "gen_dau1_eta", "gen_dau0_pT", "gen_dau1_pT"};

  std::string ycut = "abs(gen_y)<1";

  // all of candidates
  // std::cout << label << ": before filtering\n";

  std::vector<ROOT::RDF::RResultPtr<::TH1D > > hists;

  auto dfLowPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", names.at("lowPt"), branches);
  auto dfLowPtMidY = dfLowPt.Filter(ycut).Filter("gen_pT<" + std::to_string(turnpoint));
  auto dfHighPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", names.at("highPt"), branches);
  auto dfHighPtMidY = dfHighPt.Filter(ycut).Filter("gen_pT>" + std::to_string(turnpoint));

  for (unsigned int ipt=0; ipt<npt; ++ipt) {
    const double ptmin = pt_bins[ipt];
    const double ptmax = pt_bins[ipt+1];
    auto df = (ptmax-0.0001) < turnpoint ? dfLowPtMidY : dfHighPtMidY;
    const std::string cut = ::Form("gen_pT > %.2f && gen_pT < %.2f", ptmin, ptmax);
    const std::string histname = ::Form("gen_pt%d", ipt);
    const std::string histtitle = ::Form("%.1f < gen p_{T} < %.1f GeV;p_{T} (GeV);Entries", ptmin, ptmax);
    hists.push_back(df.Filter(cut)
                    .Histo1D({histname.c_str(), histtitle.c_str(), nbins, pt_low, pt_upper}, "gen_pT"));
  }

  TFile ofile(::Form("gen_reweight_prep_%s.root", label.c_str()), "recreate");
  for (auto& h : hists) {
    h->Write();
  }
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


void getRecoEachPt(TFile* f, std::string mvaName, std::string fileName, const double pTMin, const double pTMax, const double mvaCutVal, const int ipt)
{
  std::string kinematic = ::Form("cand_pT>%.3f && cand_pT<%.3f && abs(cand_y)<1.0", pTMin, pTMax);
  std::string mvaCut = ::Form("%s > %f", mvaName.c_str(), mvaCutVal);
  std::vector<std::string> branches = {"cand_pT", "cand_y", mvaName};
  auto df = ROOT::RDataFrame("lambdacAna_mc/ParticleNTuple", fileName, branches);

  std::string histname = ::Form("hreco_pt%d", ipt);
  auto hpass = df.Filter(kinematic).Filter(mvaCut).Histo1D({histname.c_str(), "", nbins, pt_low, pt_upper}, "cand_pT");

  f->cd();
  hpass->Write();
}

void getReco(std::string boost, std::string mult)
{
  std::string label = boost == "pPb" ? "" : "_etaFlipped";
  std::string ntrk = mult == "HM" ? "Ntrk185to250" : "Ntrk0to35";
  TFile ofile(::Form("reco_reweight_prep_%s_%s.root", mult.c_str(), boost.c_str()), "recreate");
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
    getRecoEachPt(&ofile, mycuts._mvaName, fileName, mycuts._pTMin, mycuts._pTMax, mycuts._mvaCut, ipt);
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

void reweight_prep()
{
  getGen_pPb();
  getGen_Pbp();
  getReco();
}
