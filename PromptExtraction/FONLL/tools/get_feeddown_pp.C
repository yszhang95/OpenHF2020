static const std::vector<double> pts_Lc = {2, 3, 4, 5, 6, 8, 10};
static const std::vector<double> yMin {-3, -2, -1, 0, 1, 2};
static const std::vector<double> yMax {-2, -1, 0, 1, 2, 3};

static const std::vector<double> pts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28};
// obtained from HIN-22-001
// currently not used
static const std::vector<double> Bmodify = {2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5,
  1.6, 1.6, 1.6, 
  1.7, 1.7, 1.7, 1.7, 1.7,
  1.8, 1.8, 1.8,
  2.4, 2.4, 2.4, 2.4, 2.4
};
static const std::vector<double> yMin_cm {-3.46, -2.46, -1.46, -0.46, 0.54, 1.54};
static const std::vector<double> yMax_cm {-2.46, -1.46, -0.46, 0.54, 1.54, 2.54};

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

/*
 * calculate Lambda_b cross section using LHCb fraction; 6 rapidity unites; from FONLL
 *
 * I multiply the results by 2 becuase my yields does not take the average, (particle+anti-particle)/2.
 * Always check macros!
 * */
std::map<std::string, std::vector<TGraphAsymmErrors* > >
get_xsec(const bool modify, const std::string model = "FONLL", const bool modifyFF=false, const bool modifyFONLL = false, const std::string outputname="")
{
    /*
     * 0-4 are empty
     * 25-30 are empty
     * */
  const vector<double> pTMin_FF = {0, 1., 2., 3., 4., 5., 6., 7., 8., 9.,  10., 11., 12., 13., 14., 16., 18., 20, 25};
  const vector<double> pTMax_FF = {1, 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 16., 18., 20., 25, 30};
  std::vector<double> fb;
  {
    ifstream lbstream("../fb.lhcb");
    string lbtemp;
    while (getline(lbstream, lbtemp)) {
      //cout << lbtemp << endl;
      trim(lbtemp);
      istringstream lbstreamtemp(lbtemp);
      string numlb;
      {
        getline(lbstreamtemp, numlb, ' ');
        string temp1;
        string temp2;
        getline(lbstreamtemp, temp1, ' ');
        getline(lbstreamtemp, temp2, ' ');
        const double err = std::sqrt(std::pow(std::stof(temp1), 2) + std::pow(std::stof(temp2), 2));
        double ff = std::stof(numlb);
        if (modifyFF) ff += err;
        fb.push_back(ff);
        //cout << fb.back() << endl;
      }
    }
    auto fbfront = fb.front();
    auto fbback = fb.back();
    fb.insert(fb.begin(), fbfront);
    fb.insert(fb.begin(), fbfront);
    fb.insert(fb.begin(), fbfront);
    fb.insert(fb.begin(), fbfront);
    fb.push_back(fbback);
  }

  auto find_ff = [&](double pt) -> double {
    for (size_t ipt=0; ipt<pTMin_FF.size(); ++ipt) {
      if (pt >= pTMin_FF.at(ipt) && pt < pTMax_FF.at(ipt))
        return fb.at(ipt);
    }
    return -1;
  };

  /*
  std::cout << find_ff(0.5) << "\n";
  std::cout << find_ff(1.5) << "\n";
  std::cout << find_ff(2.5) << "\n";
  std::cout << find_ff(3.5) << "\n";
  std::cout << find_ff(4.5) << "\n";
  std::cout << find_ff(5.5) << "\n";
  */


  std::vector<TGraphAsymmErrors*> g_B{yMin.size(), nullptr};
  std::vector<TGraphAsymmErrors*> g_Lb{yMin.size(), nullptr};

  // read input from FONLL for B
  if (model == "FONLL") {
    double sum = 0;
    for (int i=0; i<yMin.size(); ++i) {
      std::string name = ::Form("output_pp_8p16TeV/pTspectra_y%.2fto%.2f", yMin_cm.at(i), yMax_cm.at(i));
      for (auto pos=name.find("."); pos!=std::string::npos; ) {
        name.replace(pos, 1, "p");
        pos = name.find(".");
      }
      name += ".txt";
      // std::cout << name << "\n";

      g_B.at(i) = new TGraphAsymmErrors(name.c_str());
      // g_B.at(i)->Print();
      // B+ 0.4, B0 0.4, Bs 0.1
      for (int ip=0; ip<g_B.at(i)->GetN(); ip++) {
        double y = g_B.at(i)->GetY()[ip];
        double yerr_h = g_B.at(i)->GetEYhigh()[ip];
        double yerr_l = g_B.at(i)->GetEYlow()[ip];

        if (modifyFONLL) {
          y = y + std::max(yerr_h, yerr_l);
        }

        if (modify) y = y * Bmodify.at(ip);
        if (modify) yerr_h = yerr_h * Bmodify.at(ip);
        if (modify) yerr_l = yerr_l * Bmodify.at(ip);

        // sum += y;

        g_B.at(i)->SetPoint(ip, g_B.at(i)->GetX()[ip], 0.8/0.9*y * 2);
        g_B.at(i)->SetPointEYhigh(ip, 8/9.*yerr_h * 2);
        g_B.at(i)->SetPointEYlow(ip, 8/9.*yerr_l * 2);

        // std::cout << g_B.at(i)->GetX()[ip] << g_B.at(i)->GetY()[ip ]<< "\n";
      }
    }
    // std::cout << "FONLL " << sum << "pb\n";
  } else if (model == "PYTHIA") {
    for (int i=0; i<yMin.size(); ++i) {
      TFile f("BuBd_pythia_spectra.root");
      TH1D* h = f.Get<TH1D>(::Form("spectra_y%d", i));
      g_B.at(i) = new TGraphAsymmErrors(h);
    }
  }

  // reweight
  for (int i=0; i<yMin.size(); ++i) {
    g_Lb.at(i) = new TGraphAsymmErrors(*g_B.at(i));
    for (int ip=0; ip<g_Lb.at(i)->GetN(); ++ip) {
      // const double pt = g_Lb.at(i)->GetX()[ip];
      // to update
      // double y = g_Lb.at(i)->GetY()[ip];
      double pt, y;
      g_Lb.at(i)->GetPoint(ip, pt, y);

      double yErrHigh = g_Lb.at(i)->GetErrorYhigh(ip);
      double yErrLow = g_Lb.at(i)->GetErrorYlow(ip);

      auto ff = find_ff(pt);

      std::cout << ff << "\n";
      ff = ff;

      y *= ff;
      yErrHigh *= ff;
      yErrLow *= ff;

      g_Lb.at(i)->SetPointY(ip, y);
      g_Lb.at(i)->SetPointEYhigh(ip, yErrHigh);
      g_Lb.at(i)->SetPointEYlow(ip, yErrLow);

    }
      std::cout << "B\n";
      g_B.at(i)->Print();
      std::cout << "Lb\n";
      g_Lb.at(i)->Print();
  }
  std::map<std::string, std::vector<TGraphAsymmErrors* > > output;
  output["B"] = g_B;
  output["Lb"] = g_Lb;

  if (!outputname.empty()) {
    TFile ofile(outputname.c_str(), "RECREATE");
    for (const auto & m : output) {
      for (int iy=0; iy<m.second.size(); ++iy) {
        const auto h = m.second.at(iy);
        h->SetTitle(::Form("%.1f<y_{lab}<%.1f ;p_{T};cross section (pb)", yMin[iy], yMax[iy]));
        h->Write(::Form("%sy%d", m.first.c_str(), iy));
      }
    }
  }
  return output;
}

/*
 * calculate acceptance for each rapidity, given pT; from MC
 * */
std::map<std::string, std::vector<std::vector<TH1D* > > > 
get_acc()
{
  TFile fin("EvtGen.root");
  std::vector<TH1D*> hLb_all;
  std::vector<TH1D*> hB_all;
  // std::vector<std::vector<TH1D* > > hLb_pass;
  // std::vector<std::vector<TH1D* > > hB_pass;

  std::vector<std::vector<TH1D* > > hLb_acc;
  std::vector<std::vector<TH1D* > > hB_acc;

  for (int iy=0; iy<yMin.size(); ++iy) {
    std::string Lb_name = ::Form("hLb_all_y%d", iy);
    std::string B_name = ::Form("hB_all_y%d", iy);
    hLb_all.push_back(fin.Get<TH1D>(Lb_name.c_str()));
    hB_all.push_back(fin.Get<TH1D>(B_name.c_str()));
  }
  for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
    std::vector<TH1D*> hLbs;
    std::vector<TH1D*> hBs;
    for (int iy=0; iy<yMin.size(); ++iy) {
      std::string Lb_name = ::Form("hLb_pass_pt%d_y%d", ipt, iy);
      std::string B_name = ::Form("hB_pass_pt%d_y%d", ipt, iy);
      hLbs.push_back(fin.Get<TH1D>(Lb_name.c_str()));
      hLbs.back()->SetDirectory(0);
      hLbs.back()->Divide(hLb_all.at(iy));
      hBs.push_back(fin.Get<TH1D>(B_name.c_str()));
      hBs.back()->SetDirectory(0);
      hBs.back()->Divide(hB_all.at(iy));
    }
    hLb_acc.push_back(hLbs);
    hB_acc.push_back(hBs);
  }
  std::map<std::string, std::vector<std::vector<TH1D* > > > output;
  output["Lb"] = hLb_acc;
  output["B"] = hB_acc;
  return output;
}

/*
 * xsec * BR * acc
 * */
std::vector<std::vector<TH1D* > >
get_feeddown_pp(std::string name, const double BR, const bool modify, std::string model, const bool modifyFF, const bool modifyFONLL)
{
  std::vector<std::vector<TH1D* > > output;
  auto output1 = get_xsec(modify, model, modifyFF, modifyFONLL);
  auto output2 = get_acc();
  auto fonlls = output1.at(name);
  auto accs = output2.at(name);
  for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
    std::vector<TH1D* > v;
    for (int iy=0; iy<yMin.size(); ++iy) {
      auto acc = accs.at(ipt).at(iy);
      auto fonll = fonlls.at(iy);
      std::string name = acc->GetName();
      auto pos = name.find("pass");
      name.replace(pos, 4, "BR_acc");
      auto h = (TH1D*) acc->Clone(name.c_str());
      for (int ip=0; ip<fonll->GetN(); ++ip) {
        h->SetBinContent(ip+1, fonll->GetPointY(ip));
      }
      h->Scale(BR);
      h->Multiply(acc);
      h->SetDirectory(0);
      v.push_back(h);
    }
    output.push_back(v);
  }
  return output;
}
// modify is useless now, temporarily
void get_feeddown_pp(const std::string model="FONLL", const bool modifyFF = false, const bool modifyFONLL=false, const bool modify=false)
{
  std::string outputname;
  std::string outname;
  if (model == "FONLL") {
    if (modifyFF) {
      outputname = "pp_8p16TeV_bhadron_spectra_modifyFF.root";
      outname = "pp_8p16TeV_b_to_Lambda_c_modifyFF.root";
    } else if (modifyFONLL){
      outputname = "pp_8p16TeV_bhadron_spectra_modifyFONLL.root";
      outname = "pp_8p16TeV_b_to_Lambda_c_modifyFONLL.root";
    }else {
      outputname = "pp_8p16TeV_bhadron_spectra.root";
      outname = "pp_8p16TeV_b_to_Lambda_c.root";
    }
  }
  if (model == "PYTHIA") {
    outputname = "pp_8p16TeV_bhadron_pythia_spectra.root";
    outname = "pp_8p16TeV_b_to_Lambda_c_pythia.root";
  }
  get_xsec(false, model, modifyFF, modifyFONLL, outputname);
  // std::string outname = modify ? "pp_8p16TeV_b_to_Lambda_c_modify.root" : "pp_8p16TeV_b_to_Lambda_c.root";
  TFile ofile(outname.c_str(), "recreate");
  // auto Lb = get_feeddown_pp("Lb", 0.33);
  auto Lb = get_feeddown_pp("Lb", 0.83, modify, model, modifyFF, modifyFONLL);
  auto B = get_feeddown_pp("B", 0.05, modify, model, modifyFF, modifyFONLL);
  auto hLc = new TH1D(::Form("hLc_pt"), "b #rightarrow #Lambda_{c}^{+};p_{T} (GeV);d#sigma^{pp}/dp_{T} (pb/GeV)",
      pts_Lc.size()-1, pts_Lc.data());
  auto hBtoLc = new TH1D(::Form("hBtoLc_pt"), "B^{+} and B^{0} #rightarrow #Lambda_{c}^{+};p_{T} (GeV);d#sigma^{pp}/dp_{T} (pb/GeV)",
      pts_Lc.size()-1, pts_Lc.data());
  auto hLbtoLc = new TH1D(::Form("hLbtoLc_pt"), "#Lambda_{b}^{0} #rightarrow #Lambda_{c}^{+};p_{T} (GeV);d#sigma^{pp}/dp_{T} (pb/GeV)",
      pts_Lc.size()-1, pts_Lc.data());
  for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
    auto h = new TH1D(::Form("hbtoLc_pt%d", ipt), "", pts.size()-1, pts.data());
    auto hB = new TH1D(::Form("hBtoLc_pt%d", ipt), "", pts.size()-1, pts.data());
    auto hLb = new TH1D(::Form("hLbtoLc_pt%d", ipt), "", pts.size()-1, pts.data());

    for (int iy=0; iy<yMin.size(); ++iy) {
      h->Add(Lb.at(ipt).at(iy));
      h->Add(B.at(ipt).at(iy));
      hB->Add(B.at(ipt).at(iy));
      hLb->Add(Lb.at(ipt).at(iy));

    }
    const double y = h->Integral("width");
    const double yB = hB->Integral("width");
    const double yLb = hLb->Integral("width");
    const double width = hLc->GetBinWidth(ipt+1);
    hLc->SetBinContent(ipt+1, y/width);
    hLbtoLc->SetBinContent(ipt+1, yLb/width);
    hBtoLc->SetBinContent(ipt+1, yB/width);
  }
  // hLc->Print("all");
  ofile.cd();
  hLc->Write();
  hLbtoLc->Write();
  hBtoLc->Write();
  for (const auto& hs : Lb) {
    for (const auto& h : hs ) {
      h->Write();
    }
  }
  for (const auto& hs : B) {
    for (const auto& h : hs ) {
      h->Write();
    }
  }
}
