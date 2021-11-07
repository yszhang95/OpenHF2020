#include <string>
#include <vector>
#include "TROOT.h"
#include "TTree.h"
#include "ROOT/RDataFrame.hxx"

#ifndef FIT_HIST
#include "fitHist.C"
#endif

#ifndef SKIM_MASS
#define SKIM_MASS

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

  Pars() : isMC(0) {};
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

std::string floatInStrToStr(std::string str)
{
  for (auto pos = str.find(".", 0); pos != std::string::npos; ) {
    str.replace(pos, 1, "p");
    pos = str.find(".", pos+1);
  }
  return str;
}

std::string runSkimMass(const float pTMin, const float pTMax, const char* dataset,
			const std::vector<std::string>& dataFiles,
			const std::vector<std::string>& mcFiles,
			const std::string commonCuts, const std::string specialCuts,
      const bool fromNTuple=false,
      const bool selectHM=false
			)
{
  const std::string ptCuts =
    Form("cand_pT > %.1f&& cand_pT < %.1f", pTMin, pTMax);
  const std::string ptStr = floatInStrToStr(
					    Form("pT%.1f_%.1f_y1p0", pTMin, pTMax));
  // data
  {
    std::string histName = 
      Form("cand_mass_%s_%s", dataset, ptStr.c_str());
    const std::string histTitle = Form("%s pT %.1f to %.1f GeV;"
				       "M_{K_{S}^{0}p} (GeV);Events per bin", dataset, pTMin, pTMax);
    const bool isMC = false;

    string newCommonCuts = commonCuts;
    if (selectHM) newCommonCuts = newCommonCuts + " && cand_Ntrkoffline >= 185 && cand_Ntrkoffline < 250";
    if (fromNTuple) histName = histName + "_fromNTuple";

    const std::string outputFileName =
      "StoB/output_" + histName + ".root";

    Pars pars(dataFiles, newCommonCuts, ptCuts, specialCuts,
	      histName, histTitle, outputFileName, isMC);

    skimMass(pars);
  }

  // MC
  {
    std::string histName = 
      Form("cand_mass_MC_%s_%s", dataset, ptStr.c_str());
    const std::string histTitle = Form("MB pT %.1f to %.1f GeV;"
				       "M_{K_{S}^{0}p} (GeV);Events per bin", pTMin, pTMax);
    const bool isMC = true;
    if (fromNTuple) histName = histName + "_fromNTuple";
    const std::string outputFileName = "StoB/output_" + histName + ".root";

    Pars pars(mcFiles, commonCuts, ptCuts, specialCuts,
	      histName, histTitle, outputFileName, isMC);

    skimMass(pars);
  }
  return ptStr;
}


void skimMass(const char* inData, const char* inMC,
	      const double pTMin, const double pTMax,
	      const char* histName, const char* label)
{
  TFile* dataFile = TFile::Open(inData);
  TFile* MCFile = TFile::Open(inMC);
  TH3D *hData3D, *hMC3D, *hData3DNoCut, *hMC3DNoCut;
  dataFile->GetObject("lambdacAna/hMassPtY;1", hData3D);
  MCFile->GetObject("lambdacAna_mc/hMassPtY;1", hMC3D);
  dataFile->GetObject("lambdacAna/hMassPtYNoCut;1", hData3DNoCut);
  MCFile->GetObject("lambdacAna_mc/hMassPtYNoCut;1", hMC3DNoCut);

  const auto pTBinMin = hData3D->GetYaxis()->FindBin(pTMin);
  const auto pTBinMax = hData3D->GetYaxis()->FindBin(pTMax) - 1;

  const auto yBinMin = hData3D->GetZaxis()->FindBin(-1.);
  const auto yBinMax = hData3D->GetZaxis()->FindBin(1.) - 1;

  auto hData = hData3D->ProjectionX(histName,
				    pTBinMin, pTBinMax, yBinMin, yBinMax);
  hData->SetTitle( Form("%s pT %.1f to %.1f", label, pTMin, pTMax) );

  auto hDataNoCut = hData3DNoCut->ProjectionX(Form("%s_NoCut", histName),
					      pTBinMin, pTBinMax, yBinMin, yBinMax);
  hDataNoCut->SetTitle(
		       Form("NoCut %s pT %.1f to %.1f", label, pTMin, pTMax) );

  TString MCName(histName);
  MCName.Replace(0, 9, "cand_mass_MC");
  auto hMC = hMC3D->ProjectionX(MCName,
				pTBinMin, pTBinMax, 6, 15);
  hMC->SetTitle(Form("MC %s %.1fto%.1f", label, pTMin, pTMax));

  auto hMCNoCut = hMC3DNoCut->ProjectionX(MCName+"_NoCut",
					  pTBinMin, pTBinMax, 6, 15);
  hMCNoCut->SetTitle(Form("MC NoCut %s %.1fto%.1f", label, pTMin, pTMax));

  // fit for w/o cuts is not stable
  /*
  const double lw = 2.176;
  const double up = 2.42;
  const double bkgvars[] = {40000., -500, 50, 50};
  auto fbkgReject = make_shared<TF1>
    ("fbkgReject", bkgRejctPeak, lw, up, 4);
  auto fbkgRejectNoCut = make_shared<TF1>
    ("fbkgRejectNoCut", bkgRejctPeak, lw, up, 4);
  TCanvas c("c", "", 600*2, 480);
  c.Divide(2, 1);
  c.cd(1);
  fbkgReject->SetParameters(bkgvars);
  hData->Fit(fbkgReject.get(), "QRN", "", lw, up);
  hData->Fit(fbkgReject.get(), "MRN", "", lw, up);
  hData->Draw("E");
  c.cd(2);
  fbkgRejectNoCut->SetParameters(bkgvars);
  hDataNoCut->Fit(fbkgRejectNoCut.get(), "QRN", "", lw, up);
  hDataNoCut->Fit(fbkgRejectNoCut.get(), "MRN", "", lw, up);
  hDataNoCut->Draw("E");
  ROOT::Math::ChebyshevPol chebyshev(3);
  auto fbkg = make_shared<TF1>("fbkg", &chebyshev, lw, up, 4);
  auto fbkgNoCut = make_shared<TF1>("fbkgNoCut", &chebyshev, lw, up, 4);
  fbkg->SetParameters(fbkgReject->GetParameters());
  fbkgNoCut->SetParameters(fbkgRejectNoCut->GetParameters());
  c.cd(1);
  fbkg->Draw("SAME");
  c.cd(2);
  fbkgNoCut->Draw("SAME");
  TString pdfFileName = TString(histName) + "_sideband.pdf";
  c.Print(pdfFileName);
  */

  TString outputFileName = "StoB/output_" + TString(histName) + ".root";
  TString outputFileNameMC = "StoB/output_" + MCName + ".root";

  double eff = 0;

  /*
  const double cand_mass = 2.2865;
  double left = fbkg->Integral(cand_mass-0.11, cand_mass-0.6);
  double leftNoCut = fbkgNoCut->Integral(cand_mass-0.11, cand_mass-0.6);
  double right = fbkg->Integral(cand_mass+0.06, cand_mass+0.11);
  double rightNoCut = fbkgNoCut->Integral(cand_mass+0.06, cand_mass+0.11);
  eff = (left+right) / (leftNoCut + rightNoCut);
  */

  {
    const double cand_mass = 2.2865;
    const auto leftBinMin = hDataNoCut->FindBin(cand_mass - 0.11);
    const auto leftBinMax = hDataNoCut->FindBin(cand_mass - 0.06);
    const auto rightBinMin = hDataNoCut->FindBin(cand_mass + 0.06);
    const auto rightBinMax = hDataNoCut->FindBin(cand_mass + 0.11);
    double left = hData->Integral(leftBinMin, leftBinMax);
    double leftNoCut = hDataNoCut->Integral(leftBinMin, leftBinMax);
    double right = hData->Integral(rightBinMin, rightBinMax);
    double rightNoCut = hDataNoCut->Integral(rightBinMin, rightBinMax);
    eff = (left+right) / (leftNoCut + rightNoCut);
  }

  TTree* t, *tMC;
  TFile ofile(outputFileName, "RECREATE");
  t = new TTree("counts", "counts");
  auto bEff = t->Branch("eff", &eff, "eff/D");
  t->Fill();
  hData->Write();
  hDataNoCut->Write();
  t->Write();
  delete t;
  ofile.Close();
  cout << "Data done" << endl;

  eff = hMC->Integral()/ hMCNoCut->Integral();
  TFile ofileMC(outputFileNameMC, "RECREATE");
  tMC = new TTree("counts", "counts");
  tMC->Branch("eff", &eff);
  tMC->Fill();
  hMC->Write();
  hMCNoCut->Write();
  tMC->Write();
  delete tMC;
  ofileMC.Close();
  cout << "MC done" << endl;

  delete dataFile;
  delete  MCFile;
}

#endif
