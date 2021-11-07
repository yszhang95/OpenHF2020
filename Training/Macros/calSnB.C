#include "skimMass.C"
#include "fitHist.C"
#include "TString.h"
#include <string>
#include <vector>

void calSnBFromNTuple()
{
  // common cuts
  const float dlSig3DKs = 5.0;
  const float cosAngle3DKs = 0.999;
  const float dcaKs = 0.5;

  const std::string commonCuts = Form("abs(cand_y)<1 "
        "&& cand_dau0_decayLength3D/cand_dau0_decayLengthError3D > %f "
        "&& cos(cand_dau0_angle3D) > %f"
        "&& cos(cand_dau0_dca) > %f ",
        dlSig3DKs, cosAngle3DKs, dcaKs);

  // datasets for pT > 6 || pT>4
  std::vector<std::string> data6to20 = {
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB1_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB2_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB3_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB4_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB5_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB6_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB8_pPb_pT4p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",

        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB1_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB3_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB4_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB5_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB6_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB7_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB9_Pbp_pT6p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP_etaFlipped.root"
    };

  std::vector<std::string> MC5p9 = {
    "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP.root",
    "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_pT5p9_lambdacAna_mc_AllEntries_pT0p9to20p0_yAbs1p0_absMassDiff20p000_LamCKsP.root"
  };

  const std::string specialCuts = "true";

  const std::vector<float> pTs {4.0, 6.0, 8.0, 10.0};
  // 0 signal, 1 background, 2, eff_S, 3, eff_B
  std::vector<std::vector<double>> yields;

  for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
    const float pTMin = pTs.at(ipt-1);
    const float pTMax = pTs.at(ipt);
    auto ptStr = runSkimMass(pTMin, pTMax, "MB",
        data6to20, MC5p9, commonCuts, specialCuts);
    TFile data(Form("StoB/output_cand_mass_MB_%s.root", ptStr.c_str()));
    TFile mc(Form("StoB/output_cand_mass_MC_MB_%s.root", ptStr.c_str()));
    TH1D* hMass;
    TH1D* hMassMC;
    data.GetObject(Form("cand_mass_MB_%s", ptStr.c_str()), hMass);
    mc.GetObject(Form("cand_mass_MC_MB_%s", ptStr.c_str()), hMassMC);
    
    auto ret = simpleFit(hMass, hMassMC, pTMin, pTMax);

    TTree* tData; TTree* tMC;
    data.GetObject("counts", tData);
    mc.GetObject("counts", tMC);
    double effS, effB;
    tData->SetBranchAddress("eff", &effB);
    tMC->SetBranchAddress("eff", &effS);
    tData->GetEntry(0);
    tMC->GetEntry(0);

    ret.push_back(effS);
    ret.push_back(effB);

    yields.push_back(ret);
  }

  std::cout << "==============================" << std::endl;
  std::cout << "Summary" << std::endl;
  for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
    const float pTMin = pTs.at(ipt-1);
    const float pTMax = pTs.at(ipt);

    const auto ret = yields.at(ipt-1);

    const double EffS = ret.at(2);
    const double EffB = ret.at(3);
    const double S = ret.at(0);
    const double B = ret.at(1);
    const double sPrime = S/EffS;
    const double bPrime = B/EffB;

    std::cout << "MB: " << pTMin << " " << pTMax << std::endl;
    std::cout << "After cuts" << std::endl;
    std::cout << "EffS: " << EffS << std::endl;
    std::cout << "EffB: " << EffB << std::endl;
    std::cout << "S: " << S << std::endl;
    std::cout << "B: " << B << std::endl;
    std::cout << "Before cuts" << std::endl;
    std::cout << "S: " << sPrime << std::endl;
    std::cout << "B: " << bPrime << std::endl;
    std::cout << std::endl;
  }
}

void calSnBFromNTupleHM()
{
  // common cuts
  const float dlSig3DKs = 5.0;
  const float cosAngle3DKs = 0.999;
  const float dcaKs = 0.5;

  const std::string commonCuts = Form("abs(cand_y)<1 "
        "&& cand_dau0_decayLength3D/cand_dau0_decayLengthError3D > %f "
        "&& cos(cand_dau0_angle3D) > %f"
        "&& cos(cand_dau0_dca) > %f ",
        dlSig3DKs, cosAngle3DKs, dcaKs);

  // datasets for pT > 6 || pT>4
  std::vector<std::string> data6to20 = {
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM1_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM2_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM3_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM4_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM5_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root",
        "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM6_pPb_pT2p0to20p0_yAbs1p0_absMassDiff0p150_LamCKsP.root"
    };

  std::vector<std::string> MC5p9 = {
    "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP.root",
    "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_pT5p9_lambdacAna_mc_AllEntries_pT0p9to20p0_yAbs1p0_absMassDiff20p000_LamCKsP.root"
  };

  const std::string specialCuts = "abs(cand_etaDau0)<1.4 && abs(cand_etaDau1)<1.4 && cand_pTDau0>0.5 && cand_pTDau1>1.5";

  const std::vector<float> pTs {4.0, 6.0, 8.0, 10.0};
  // 0 signal, 1 background, 2, eff_S, 3, eff_B
  std::vector<std::vector<double>> yields;

  for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
    const float pTMin = pTs.at(ipt-1);
    const float pTMax = pTs.at(ipt);
    auto ptStr = runSkimMass(pTMin, pTMax, "HM",
        data6to20, MC5p9, commonCuts, specialCuts, true, true);
    TFile data(Form("StoB/output_cand_mass_HM_%s_fromNTuple.root", ptStr.c_str()));
    TFile mc(Form("StoB/output_cand_mass_MC_HM_%s_fromNTuple.root", ptStr.c_str()));
    TH1D* hMass;
    TH1D* hMassMC;
    data.GetObject(Form("cand_mass_HM_%s_fromNTuple", ptStr.c_str()), hMass);
    mc.GetObject(Form("cand_mass_MC_HM_%s_fromNTuple", ptStr.c_str()), hMassMC);
 
    auto ret = simpleFit(hMass, hMassMC, pTMin, pTMax);

    TTree* tData; TTree* tMC;
    data.GetObject("counts", tData);
    mc.GetObject("counts", tMC);
    double effS, effB;
    tData->SetBranchAddress("eff", &effB);
    tMC->SetBranchAddress("eff", &effS);
    tData->GetEntry(0);
    tMC->GetEntry(0);

    ret.push_back(effS);
    ret.push_back(effB);

    yields.push_back(ret);
  }

  std::cout << "==============================" << std::endl;
  std::cout << "Summary" << std::endl;
  for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
    const float pTMin = pTs.at(ipt-1);
    const float pTMax = pTs.at(ipt);

    const auto ret = yields.at(ipt-1);

    const double EffS = ret.at(2);
    const double EffB = ret.at(3);
    const double S = ret.at(0);
    const double B = ret.at(1);
    const double sPrime = S/EffS;
    const double bPrime = B/EffB;

    std::cout << "HM: " << pTMin << " " << pTMax << std::endl;
    std::cout << "After cuts" << std::endl;
    std::cout << "EffS: " << EffS << std::endl;
    std::cout << "EffB: " << EffB << std::endl;
    std::cout << "S: " << S << std::endl;
    std::cout << "B: " << B << std::endl;
    std::cout << "Before cuts" << std::endl;
    std::cout << "S: " << sPrime << std::endl;
    std::cout << "B: " << bPrime << std::endl;
    std::cout << std::endl;
  }
}

void calSnBFromHist()
{
  std::map<std::string, std::map<std::string, std::string>> inputs;
  std::map<std::string, std::vector<float>> pTVecs;

  inputs["MB"]["data"] = "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataMB_CutBase.root";
  inputs["MB"]["MC"] = "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP_Cutbase.root";
  pTVecs["MB"] = {2.0, 3.0, 4.0};

  inputs["HM"]["data"] = "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/dataHM_CutBase.root";
  inputs["HM"]["MC"] = "root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP_Cutbase.root";
  pTVecs["HM"] = {2.0, 3.0, 4.0, 6.0, 8.0, 10.0};
  // 0 signal, 1 background, 2, eff_S, 3, eff_B

  for (const auto e : inputs) {
    std::vector<std::vector<double>> yields;

    const auto label = e.first.c_str();
    const auto dataFile = e.second.at("data");
    const auto mcFile = e.second.at("MC");
    const auto& pTs = pTVecs.at(label);

    //cout << label << endl;

    for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
      //cout << "ipt: " << ipt<< endl;
      const float pTMin = pTs.at(ipt-1);
      const float pTMax = pTs.at(ipt);
      const std::string ptStr = floatInStrToStr(
          Form("pT%.1f_%.1f_y1p0", pTMin, pTMax));
      skimMass(dataFile.c_str(), mcFile.c_str(),
          pTMin, pTMax, 
          Form("cand_mass_%s_%s", label, ptStr.c_str()), label);
      TFile data(Form("StoB/output_cand_mass_%s_%s.root", label, ptStr.c_str()));
      TFile mc(Form("StoB/output_cand_mass_MC_%s_%s.root", label, ptStr.c_str()));
      TH1D* hMass;
      TH1D* hMassMC;
      data.GetObject(Form("cand_mass_%s_%s", label, ptStr.c_str()), hMass);
      mc.GetObject(Form("cand_mass_MC_%s_%s", label, ptStr.c_str()), hMassMC);

      auto ret = simpleFit(hMass, hMassMC, pTMin, pTMax);

      TTree* tData; TTree* tMC;
      data.GetObject("counts", tData);
      mc.GetObject("counts", tMC);
      double effS, effB;
      tData->SetBranchAddress("eff", &effB);
      tMC->SetBranchAddress("eff", &effS);
      tData->GetEntry(0);
      tMC->GetEntry(0);

      ret.push_back(effS);
      ret.push_back(effB);

      yields.push_back(ret);
    }

    std::cout << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Summary for " << label << std::endl;
    for (size_t ipt=1; ipt < pTs.size() ; ++ipt) {
      const float pTMin = pTs.at(ipt-1);
      const float pTMax = pTs.at(ipt);

      const auto ret = yields.at(ipt-1);

      const double EffS = ret.at(2);
      const double EffB = ret.at(3);
      const double S = ret.at(0);
      const double B = ret.at(1);
      const double sPrime = S/EffS;
      const double bPrime = B/EffB;

      std::cout << label << ": " << pTMin << " " << pTMax << std::endl;
      std::cout << "After cuts" << std::endl;
      std::cout << "EffS: " << EffS << std::endl;
      std::cout << "EffB: " << EffB << std::endl;
      std::cout << "S: " << S << std::endl;
      std::cout << "B: " << B << std::endl;
      std::cout << "Before cuts" << std::endl;
      std::cout << "S: " << sPrime << std::endl;
      std::cout << "B: " << bPrime << std::endl;
      std::cout << std::endl;
    }
  }
}
