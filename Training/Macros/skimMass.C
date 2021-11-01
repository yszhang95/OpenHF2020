#include <string>
#include <vector>
#include "TROOT.h"
#include "TTree.h"
#include "ROOT/RDataFrame.hxx"

using namespace ROOT;

struct Pars
{
  std::vector<std::string> fileNames;
  std::string commonCuts;
  std::string ptCuts;
  std::string specialCuts;
  std::string histName;
  std::string histTitle;
  std::string outputFileName;
  bool isMC;

  Pars() {};
  Pars(const std::vector<std::string> fileNames_,
      const std::string commonCuts_,
      const std::string ptCuts_,
      const std::string specialCuts_,
      const std::string histName_,
      const std::string histTitle_,
      const std::string outputFileName_,
      const bool isMC_
      )
    : fileNames(fileNames_), commonCuts(commonCuts_),
    ptCuts(ptCuts_), specialCuts(specialCuts_),
    histName(histName_), histTitle(histTitle_),
    outputFileName(outputFileName_), isMC(isMC_) {}
};


void skimMass(Pars pars)
{
  EnableImplicitMT();
  const std::string treeName = pars.isMC ?
    "lambdacAna_mc/ParticleNTuple" : "lambdacAna/ParticleNTuple";
  const auto& fileNames = pars.fileNames;
  auto dfInput = RDataFrame(treeName, fileNames);

  const auto commonCuts = pars.commonCuts;
  const auto ptCuts = pars.ptCuts;
  auto df = dfInput.Filter(commonCuts).Filter(ptCuts);

  auto dfInitial = pars.isMC ? df.Filter("abs(cand_mass-2.2865)<0.3")
    :df.Filter("abs(cand_mass-2.2865)>0.06"
            " && abs(cand_mass-2.2865)>0.11");

  const auto specialCuts = pars.specialCuts;
  auto dfSkim = df.Filter(specialCuts);

  auto dfPass = dfInitial.Filter(specialCuts);

  auto total = dfInitial.Count();
  auto pass = dfPass.Count();

  const char* histName = pars.histName.c_str();
  const char* histTitle = pars.histTitle.c_str();
  auto hMass = dfSkim.Histo1D({histName, histTitle, 120, 2.15, 2.45}, "cand_mass");

  const char* outputFileName = pars.outputFileName.c_str();
  auto total_val = total.GetValue();
  auto pass_val = pass.GetValue();

  std::cout << pars.histName << std::endl;
  std::cout << "Total counts: " << total_val << std::endl;
  std::cout << "Passed counts: " << pass_val << std::endl;

  TTree t("counts", "counts");
  t.Branch("total", &total_val);
  t.Branch("pass", &pass_val);

  double eff = static_cast<double>(pass_val)/static_cast<double>(total_val);
  t.Branch("eff", &eff);
  t.Fill();

  TFile ofile(outputFileName, "RECREATE");
  hMass->Write();
  t.Write();
}
