#include <string>
#include <vector>
#include "TROOT.h"
#include "ROOT/RDataFrame.hxx"
#include "TParameter.h"

#pragma link C++ class TParameter<unsigned long long>+;

using namespace ROOT;
struct Pars
{
  std::vector<std::string> fileNames;
  std::string cuts;
  std::string histName;
  std::string histTitle;
  std::string outputFileName;
  bool isMC;
};

void skimMass(Pars pars)
{
  EnableImplicitMT();
  const std::string treeName = pars.isMC ?
    "lambdacAna_mc/ParticleNTuple" : "lambdacAna/ParticleNTuple";
  const auto& fileNames = pars.fileNames;
  auto df = RDataFrame(treeName, fileNames);

  const auto& cuts = pars.cuts;

  auto dfInitial = pars.isMC ? df.Filter("abs(cand_mass-2.2865)<0.3")
    :df.Filter("abs(cand_mass-2.2865)>0.06"
            " && abs(cand_mass-2.2865)>0.11");

  auto dfSkim = df.Filter(cuts);
  auto dfPass = dfInitial.Filter(cuts);

  auto total = dfInitial.Count();
  auto pass = dfPass.Count();

  const char* histName = pars.histName.c_str();
  const char* histTitle = pars.histTitle.c_str();
  auto hMass = dfSkim.Histo1D({histName, histTitle, 120, 2.15, 2.45});

  const char* outputFileName = pars.outputFileName.c_str();
  TFile ofile(outputFileName, "RECREATE");
  hMass->Write();
  const auto total_val = total.GetValue();
  const auto pass_val = pass.GetValue();
  TParameter<unsigned long long> total_par("total", total_val);
  TParameter pass_par("pass", pass_val);
  total_par.Write();
  pass_par.Write();
}
