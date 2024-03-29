static const std::vector<double> pts_Lc = {2, 3, 4, 5, 6, 8, 10};
static const std::vector<double> yMin {-3, -2, -1, 0, 1, 2};
static const std::vector<double> yMax {-2, -1, 0, 1, 2, 3};

static const std::vector<double> pts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28};
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
 * */
std::map<std::string, std::vector<TGraphAsymmErrors* > > get_xsec()
{
    /*
     * 0-4 are empty
     * 25-30 are empty
     * */
  const vector<double> pTMin_FF = {0, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 16., 18., 20., 25};
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
        fb.push_back(std::stof(numlb));
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
      if (pt > pTMin_FF.at(ipt) && pt < pTMax_FF.at(ipt))
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

      y *= ff;
      yErrHigh *= ff;
      yErrLow *= ff;

      g_Lb.at(i)->SetPointY(ip, y);
      g_Lb.at(i)->SetPointEYhigh(ip, yErrHigh);
      g_Lb.at(i)->SetPointEYlow(ip, yErrLow);

      // std::cout << "B\n";
      // g_B.at(i)->Print();
      // std::cout << "Lb\n";
      // g_Lb.at(i)->Print();
    }
  }
  std::map<std::string, std::vector<TGraphAsymmErrors* > > output;
  output["B"] = g_B;
  output["Lb"] = g_Lb;
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
get_feeddown_pp(std::string name, const double BR)
{
  std::vector<std::vector<TH1D* > > output;
  auto output1 = get_xsec();
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
void get_feeddown_pp()
{
  TFile ofile("pp_8p16TeV_b_to_Lambda_c.root", "recreate");
  auto Lb = get_feeddown_pp("Lb", 0.33);
  auto B = get_feeddown_pp("B", 0.05);
  auto hLc = new TH1D(::Form("hLc_pt"), "b #rightarrow #Lambda_{c}^{+};p_{T} (GeV);d#sigma^{pp}/dp_{T} (pb/GeV)",
      pts_Lc.size()-1, pts_Lc.data());
  for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
    auto h = new TH1D(::Form("hbtoLc_pt%d", ipt), "", pts.size()-1, pts.data());
    for (int iy=0; iy<yMin.size(); ++iy) {
      h->Add(Lb.at(ipt).at(iy));
      h->Add(B.at(ipt).at(iy));
    }
    const double y = h->Integral("width");
    const double width = hLc->GetBinWidth(ipt+1);
    hLc->SetBinContent(ipt+1, y/width);
  }
  // hLc->Print("all");
  ofile.cd();
  hLc->Write();
}
