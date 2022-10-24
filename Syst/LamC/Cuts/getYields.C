#include "utils.C"
#include "../../../Spectra/ProduceWorkspace/LamC/tools.C"

void getYields()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  for (const auto trig : {"MB", "HM"}) {
    TFile ofile(::Form("yields_%s.root", trig), "recreate");
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ipt ==0 && !strcmp(trig, "HM")) continue;
      std::cout << trig << " : " << pts[ipt] << " -- " << pts[ipt+1] << "\n";
      std::string ptbin = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      const std::vector<double> cuts = getCuts(trig, ptbin);
      std::string configName = ::string_format("configs/pT%.0fto%.0f_%s.conf", pts[ipt], pts[ipt+1], trig);
      FitParConfigs configs(configName);
      const auto cutConfigs = configs.getCutConfigs();
      // modify it later
      VarCuts mycuts(cutConfigs);
      std::vector<double> yields_raw;
      std::vector<double> errs_raw;
      for (const auto cut : cuts) {
        mycuts._mvaCut = cut;
        auto input = getOutNames(mycuts);
        std::string mvaStr = mycuts.getMva();
        input.at("outdir") = "./postfit/";
        input.at("fileName") = gSystem->BaseName(input.at("fileName").c_str());
        input.at("fileName") = input.at("outdir") + input.at("fileName");
        auto pos = input.at("fileName").find(".root");
        auto suffix = "_" + mvaStr + ".root";
        input.at("fileName").replace(pos, 5, suffix);
        double yields(0), err(0);
        // cout << input.at("fileName") << "\n";
        getFit(input, yields, err);
        double nEvents = countEvents(configName);
        yields /= nEvents;
        err /= nEvents;
        const double dPt = pts[ipt+1] - pts[ipt];
        yields /= dPt;
        err /= dPt;
        // cout << "pT: " << pts[ipt] << " " << pts[ipt+1] << " GeV: " << yields << " +/- " << err << "\n";
        yields_raw.push_back(yields);
        errs_raw.push_back(err);
      }
      auto input = getOutNames(mycuts);
      // cout << input.at("fileName") << "\n";
      TFile infile(input.at("fileName").c_str());
      auto eff = infile.Get<TGraph>("eff");
      TGraphErrors* g = new TGraphErrors(eff->GetN());
      for (int ip=0; ip<eff->GetN(); ++ip) {
        const double x = eff->GetX()[ip];
        const double y = yields_raw.at(ip)/eff->GetY()[ip];
        const double yerr = errs_raw.at(ip)/eff->GetY()[ip];
        g->SetPoint(ip, x, y);
        g->SetPointError(ip, 0, yerr);
      }
      ofile.cd();
      g->Write(::Form("pT%.0fto%.0f", pts[ipt], pts[ipt+1]));
      delete g;
    }
  }
}
