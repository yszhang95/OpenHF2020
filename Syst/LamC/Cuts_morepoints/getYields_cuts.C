#include "utils.C"
#include "../../../Spectra/ProduceWorkspace/LamC/tools.C"

void getYields_cuts()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  for (const auto trig : {"MB", "HM"}) {
  // for (const auto trig : {"HM"}) {
  // for (const auto trig : {"MB"}) {
    TFile ofile(::Form("yields_%s_cuts.root", trig), "recreate");
    for (int ipt=0; ipt<nPt; ++ipt) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
      if (std::abs(pts[ipt]-2)<1E-3 && !strcmp(trig, "HM")) continue;
      std::cout << trig << " : " << pts[ipt] << " -- " << pts[ipt+1] << "\n";
      std::string ptbin = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);

      std::string infile_name = ::string_format("output_workspace/ofile_dz1p0_%s_%s_yAbs1.root", trig, ptbin.c_str());
      TFile infile(infile_name.c_str());
      TGraphAsymmErrors* gEff{nullptr};
      if (infile.IsOpen()) {
        std::cout << "Opened " << infile_name << "\n";
        infile.GetObject("efficiency", gEff);
      }

      // const std::vector<double> cuts = getCuts(trig, ptbin);
      std::vector<double> cuts{gEff->GetX(), gEff->GetX()+gEff->GetN()};

      std::string configName = ::string_format("configs/pT%.0fto%.0f_%s.conf", pts[ipt], pts[ipt+1], trig);
      FitParConfigs configs(configName);
      const auto cutConfigs = configs.getCutConfigs();
      // modify it later
      VarCuts mycuts(cutConfigs);
      std::vector<double> yields_raw;
      std::vector<double> errs_raw;
      std::vector<double> eff_pass;
      std::vector<double> eff_pass_err;
      std::vector<double> cut_pass;
      // for (const auto cut : cuts) {
      for (int ip=0; ip<cuts.size(); ++ip) {
        const double cut = cuts.at(ip);

        mycuts._mvaCut = cut;
        auto input = getOutNames(mycuts);
        std::string mvaStr = mycuts.getMva();
        input.at("outdir") = "./postfit/";
        input.at("fileName") = gSystem->BaseName(input.at("fileName").c_str());
        input.at("fileName") = input.at("outdir") + input.at("fileName");
        auto pos = input.at("fileName").find(".root");
        auto suffix = "_" + mvaStr + ".root";
        input.at("fileName").replace(pos, 5, suffix);

        // clean
        // check chi2
        bool skip=false;
        bool found=false;
        std::string log_name = ::Form("dz1p0_output/LamCFit_pT%.1fto%.1f_%s_%s.log", pts[ipt], pts[ipt+1], trig, mvaStr.c_str());
        std::ifstream log_txt(::Form("dz1p0_output/LamCFit_pT%.1fto%.1f_%s_%s.log", pts[ipt], pts[ipt+1], trig, mvaStr.c_str()));
        for (std::string temp; std::getline(log_txt, temp); ) {
          auto ex = TPRegexp("chi\\^\\{2\\}/N\\.D\\.F = (\\d+.?\\d*)/(\\d+)");
          TObjArray *subStrL = ex.MatchS(temp.c_str());
          if (subStrL->GetEntries()) {
            const TString chi2str = ((TObjString *)subStrL->At(1))->GetString();
            const TString ndfstr = ((TObjString *)subStrL->At(2))->GetString();
            // std::cout << chi2str << "\n";
            const auto chi2 = chi2str.Atof();
            const auto ndf = ndfstr.Atoi();
            // std::cout << chi2/ndf << "\n";
            found=true;
            if (chi2/ndf > 1.3) {
              std::cout << "Removing\n";
              std::string f1 = ::Form("dz1p0_output/LamCFit_pT%.1fto%.1f_%s_%s.log", pts[ipt], pts[ipt+1], trig, mvaStr.c_str());
              std::string f2 = ::Form("dz1p0_output/pT%.1fto%.1f_%s_%s", pts[ipt], pts[ipt+1], trig, mvaStr.c_str());
              gSystem->Unlink(f1.c_str());
              gSystem->Unlink(::Form("%s_data.pdf", f2.c_str()));
              gSystem->Unlink(::Form("%s_data.png", f2.c_str()));
              gSystem->Unlink(::Form("%s_signal.png", f2.c_str()));
              skip=true;
            }
            break;
          }
        }
        if (!found) { std::cout << "Not found chi2 values in " << log_name << "\n"; }
        if (skip) continue;

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
        eff_pass.push_back(gEff->GetY()[ip]);
        // eff_pass_err.push_back(gEff->GetEY()[ip]);
        cut_pass.push_back(gEff->GetX()[ip]);
      }
      auto input = getOutNames(mycuts);
      // cout << input.at("fileName") << "\n";
      // TFile infile(input.at("fileName").c_str());
      // auto eff = infile.Get<TGraph>("eff");
      // auto eff = gEff;
      TGraphErrors* g = new TGraphErrors(cut_pass.size());
      TGraph* gEffNew = new TGraph(cut_pass.size());
      for (int ip=0; ip<eff_pass.size(); ++ip) {
        const double x = cut_pass.at(ip);
        const double y = yields_raw.at(ip)/eff_pass.at(ip);
        const double yerr = errs_raw.at(ip)/eff_pass[ip];
        g->SetPoint(ip, x, y);
        g->SetPointError(ip, 0, yerr);
        gEffNew->SetPoint(ip, x, eff_pass.at(ip));
        // gEffNew->SetPointError(ip, x, eff_pass_err.at(ip));
      }
      ofile.cd();
      g->Write(::Form("pT%.0fto%.0f", pts[ipt], pts[ipt+1]));
      gEffNew->Write(::Form("effNew_pT%.0fto%.0f", pts[ipt], pts[ipt+1]));
      delete g;
      delete gEffNew;
    }
  }
}
