#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"

using namespace RooFit;

using std::cout;
using std::endl;

void fit()
{
  gROOT->SetBatch(kTRUE);
  FitParConfigs configs("test.txt");
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();

  std::string fileMC = inputConfigs.getPaths("PromptDMC").at(0);
  std::string fileD0toKK = inputConfigs.getPaths("KK").at(0);
  std::string fileD0toPiPi = inputConfigs.getPaths("PiPi").at(0);

  RooRealVar mass("mass", "mass", 1.725, 2.0);
  RooRealVar pT("pT", "pT", 0, 100);
  RooRealVar y("y", "rapidity", -2.4, 2.4);
  RooRealVar matchGEN("matchGEN", "matchGEN", -1, 2);
  RooRealVar isSwap("isSwap", "isSwap", -1, 2);
  RooRealVar dz1p0("dz1p0", "dz1p0", -2.4, 2.4);

  mass.setRange("full", 1.725, 2.0);
  mass.setRange("signal", 1.725, 2.0);
  mass.setRange("KK", 1.725, 1.82);
  mass.setRange("PiPi", 1.88, 2.0);

  // std::map<std::string, std::string> strs;
  // strs.insert( {"logfile", outputConfigs.getPath("log")} );
  // strs["xtitle"] = "M_{K#pi}";

  // // redirect messages
  // gSystem->RedirectOutput(strs.at("logfile").c_str(), "w", nullptr);

  // std::cout << ">>>>>>>> Fit true D\n" << std::endl;
  // RooDataSet dsMCSignal("dsMCSignal", "true D0 samples",
  //                       RooArgSet(mass, pT, y, matchGEN, isSwap),
  //                       ImportFromFile(fileMC.c_str(), "D0_pT2to3"),
  //                       Cut("isSwap<0.5"));
  // fitSignal(mass, dsMCSignal, par, strs);
  // std::cout << std::endl;

  // std::cout << ">>>>>>>> Fit swap\n" << std::endl;
  // RooDataSet dsMCSwap("dsMCSwap", "swap D0 samples",
  //                    RooArgSet(mass, pT, y, matchGEN, isSwap),
  //                    ImportFromFile(fileMC.c_str(), "D0_pT2to3"),
  //                    Cut("isSwap>0.5"));
  // fitSwap(mass, dsMCSwap, par);
  // std::cout << std::endl;

  // std::cout << ">>>>>>>> Fit true and swap D\n" << std::endl;
  // RooDataSet dsMCAll("dsMCAll", "true+swap D0 samples",
  //                    RooArgSet(mass, pT, y, matchGEN, isSwap),
  //                    ImportFromFile(fileMC.c_str(), "D0_pT2to3"));
  // fitSignalAndSwap(mass, dsMCAll, par);
  // std::cout << std::endl;

  // std::cout << ">>>>>>>> Fit D0->KK\n" << std::endl;
  // RooDataSet dsMCKK("dsMCKK", "MC D0->K+K0 samples",
  //                   RooArgSet(mass, pT, y, matchGEN, isSwap),
  //                   ImportFromFile(fileD0toKK.c_str(), "D0_pT2to3"));
  // fitCBShapeKK(mass, dsMCKK, par, strs);
  // std::cout << std::endl;

  // std::cout << ">>>>>>>> Fit D0->PiPi\n" << std::endl;
  // RooDataSet dsMCPiPi("dsMCPiPi", "MC D0->K+K0 samples",
  //                     RooArgSet(mass, pT, y, matchGEN, isSwap),
  //                     ImportFromFile(fileD0toPiPi.c_str(), "D0_pT2to3"));
  // fitCBShapePiPi(mass, dsMCPiPi, par, strs);
  // std::cout << std::endl;

  // std::cout << ">>>>>>>> Fit D0->KPi in data\n" << std::endl;
  // TChain chain(inputConfigs.getName("Data").c_str(), "D0NTuple");
  // const auto files = inputConfigs.getPaths("Data");
  // for (const auto & file : files) {
  //   const auto it = file.find(".list");
  //   if (it != std::string::npos) {
  //     TFileCollection tf("tf", "", file.c_str());
  //     chain.AddFileInfoList(tf.GetList());
  //   } else {
  //     chain.Add(file.c_str());
  //   }
  // }
  // RooDataSet dsDataPt2to3("dsDataPt2to3", "D0 pT 2to3", &chain,
  //                         RooArgSet(mass, pT, y),
  //                         "pT<3 && pT>2 && abs(y)<1");
  // fitD0(mass, dsDataPt2to3, par, strs);
  // std::cout << std::endl;


}

void fitAlice()
{
  FitParConfigs configs("test.txt");
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();

  std::string fileMC = inputConfigs.getPaths("PromptDMC").at(0);
  std::string fileD0toKK = inputConfigs.getPaths("KK").at(0);
  std::string fileD0toPiPi = inputConfigs.getPaths("PiPi").at(0);

  RooRealVar mass("mass", "mass", 1.725, 2.0);
  RooRealVar pT("pT", "pT", 0, 100);
  RooRealVar y("y", "rapidity", -2.4, 2.4);
  RooRealVar matchGEN("matchGEN", "matchGEN", -1, 2);
  RooRealVar isSwap("isSwap", "isSwap", -1, 2);
  RooRealVar dz1p0("dz1p0", "dz1p0", -1, 2);
  RooRealVar mva("mva", "mva", -1, 1);

  mass.setRange("full", 1.725, 2.0);
  mass.setRange("signal", 1.725, 2.0);
  mass.setRange("KK", 1.725, 1.82);
  mass.setRange("PiPi", 1.88, 2.0);

  // for loop
  std::map<std::string, std::string> strs;
  const int nPt = 14;
  const float pts[nPt+1] =   {2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 9.0, 10.0};
  const float treeMin[nPt] = {2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 6.0, 6.0, 6.0, 8.0, 8.0};
  const float treeMax[nPt] = {3.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 6.0, 8.0, 8.0, 8.0, 8.0, 10., 10.};
  for (int ipt=0; ipt<nPt; ++ipt) {
    // redirect messages, do not forget to get the stream back
    std::string path_name_log = Form("output/D0Fit_pT%.1fto%.1f.log",
                                     pts[ipt], pts[ipt+1]);
    std::string treePt = Form("D0_pT%.0fto%.0f", treeMin[ipt], treeMax[ipt]);
    std::string fig_path = Form("output/pT%.1fto%.1f", pts[ipt], pts[ipt+1]);
    strs["fig_path"] = fig_path;
    strs["alignment"] = "22";
    strs["pT"] = Form("%.1f<p_{T}<%.1f", pts[ipt], pts[ipt+1]);
    strs["y"] = "|y|<0.5";
    // I take cuts as weights because
    // if pass cuts, weight = 1, otherwise weight = 0
    std::string cuts = Form("pT > %.1f && pT < %.1f", pts[ipt], pts[ipt+1]);
    cuts += " && dz1p0 > 0.5";
    cuts += " && abs(y)< 0.5";
    cuts += " && mva > 0.56";
    if (!gSystem->AccessPathName(path_name_log.c_str())) {
      gSystem->Unlink(path_name_log.c_str());
    }
    // remember that the AccessPathName returns false it the path exits!
    const auto out_dir = gSystem->DirName(path_name_log.c_str());
    if (gSystem->AccessPathName(out_dir)) {
      gSystem->mkdir(out_dir, kTRUE);
    }
    gSystem->RedirectOutput(path_name_log.c_str(), "a", nullptr);

    strs["xtitle"] = "M_{K#pi}";
    std::cout << ">>>>>>>> Fit true D\n" << std::endl;
    RooDataSet dsMCSignal("dsMCSignal", "true D0 samples",
                          RooArgSet(mass, pT, y, matchGEN, isSwap, mva),
                          ImportFromFile(fileMC.c_str(), treePt.c_str()),
                          Cut("isSwap<0.5 && mva>0.56"));
    fitSignal(mass, dsMCSignal, par, strs);
    std::cout << std::endl;

    std::cout << ">>>>>>>> Fit swap\n" << std::endl;
    RooDataSet dsMCSwap("dsMCSwap", "swap D0 samples",
                        RooArgSet(mass, pT, y, matchGEN, isSwap, mva),
                        ImportFromFile(fileMC.c_str(), treePt.c_str()),
                        Cut("isSwap>0.5 && mva>0.56"));
    fitSwap(mass, dsMCSwap, par, strs);
    std::cout << std::endl;

    std::cout << ">>>>>>>> Fit true and swap D\n" << std::endl;
    RooDataSet dsMCAll("dsMCAll", "true+swap D0 samples",
                       RooArgSet(mass, pT, y, matchGEN, isSwap, mva),
                       ImportFromFile(fileMC.c_str(), treePt.c_str()),
                       Cut("mva>0.56"));
    fitSignalAndSwap(mass, dsMCAll, par, strs);
    std::cout << std::endl;

    std::cout << ">>>>>>>> Fit D0->KK\n" << std::endl;
    RooDataSet dsMCKK("dsMCKK", "MC D0->K+K0 samples",
                      RooArgSet(mass, pT, y, matchGEN, isSwap, mva),
                      ImportFromFile(fileD0toKK.c_str(), treePt.c_str()),
                      Cut("mva>0.56"));
    fitCBShapeKK(mass, dsMCKK, par, strs);
    std::cout << std::endl;

    std::cout << ">>>>>>>> Fit D0->PiPi\n" << std::endl;
    RooDataSet dsMCPiPi("dsMCPiPi", "MC D0->K+K0 samples",
                        RooArgSet(mass, pT, y, matchGEN, isSwap, mva),
                        ImportFromFile(fileD0toPiPi.c_str(), treePt.c_str()),
                        Cut("mva>0.56"));
    fitCBShapePiPi(mass, dsMCPiPi, par, strs);
    std::cout << std::endl;

    std::cout << ">>>>>>>> Fit D0->KPi in data\n" << std::endl;
    TChain chain(inputConfigs.getName("Data").c_str(), "D0NTuple");
    const auto files = inputConfigs.getPaths("Data");
    for (const auto & file : files) {
      const auto it = file.find(".list");
      if (it != std::string::npos) {
        TFileCollection tf("tf", "", file.c_str());
        chain.AddFileInfoList(tf.GetList());
      } else {
        chain.Add(file.c_str());
      }
    }
    // cuts as weight
    RooDataSet dsData("dsData", treePt.c_str(), &chain,
                      RooArgSet(mass, pT, y, dz1p0, mva),
                      cuts.c_str());
    cout << cuts << endl;
    // special for data
    strs["mult"] = "MB";
    fitD0(mass, dsData, par, strs);
    std::cout << std::endl;

    // get the stream back
    gSystem->RedirectOutput(nullptr);
  }
}
