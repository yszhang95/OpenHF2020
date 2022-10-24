using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::getline;
using std::string;
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

void produceFONLL()
{
  const vector<double> pTMin = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 16., 18., 20.};
  const vector<double> pTMax = {2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 16., 18., 20., 25};
  const auto npt = pTMin.size();
  vector<vector<double > > values;
  vector<double > central;
  vector<double > errLw;
  vector<double > errUp;
  vector<double > pTMean;
  vector<double > pTMeanError;
  for (size_t ipt=0; ipt<npt; ++ipt) {
    std::string inname = Form("pT%.0fto%.0f.txt", pTMin[ipt], pTMax[ipt]);
    ifstream file(inname);
    std::string str;
    while(getline(file, str)) {
      if (str.at(0) == '#') continue;
      else break;
    }
    trim(str);
    //cout << str << endl;
    istringstream strstream(str);
    std::string num;
    vector<double> nums;
    while(getline(strstream, num, ' ')) {
      nums.push_back(std::stof(num));
      cout << nums.back() << "\t";
    }

    central.push_back(nums.at(1));
    //cout << endl;
    //cout << central.back() << endl;
    double errlwtemp = 0;
    for (int i=2; i<9; i=i+2) {
      const auto diff = nums.at(1) - nums.at(i);
      //cout << diff << endl;
      errlwtemp = std::pow(diff, 2);
    }
    errlwtemp = std::sqrt(errlwtemp);
    errLw.push_back(errlwtemp);

    double erruptemp = 0;
    for (int i=3; i<10; i=i+2) {
      const auto diff = nums.at(1) - nums.at(i);
      //cout << diff << endl;
      erruptemp = std::pow(diff, 2);
    }
    erruptemp = std::sqrt(erruptemp);
    errUp.push_back(erruptemp);

    values.push_back(nums);
    cout << endl;
    pTMean.push_back((pTMin[ipt]+pTMax[ipt])/2.);
    pTMeanError.push_back((pTMax[ipt]-pTMin[ipt])/2.);
  }
  vector<double> fs;
  vector<double> fb;
  {
    ifstream bsstream("fs.lhcb");
    string bstemp;
    while (getline(bsstream, bstemp)) {
      trim(bstemp);
      istringstream bsstreamtemp(bstemp);
      string numbs;
      /*
      while (getline(bsstreamtemp, numbs, " ") {
          fs.push_back(std::stof(numbs));
      }
      */
      {
        getline(bsstreamtemp, numbs, ' ');
        //cout << numbs << endl;
        fs.push_back(std::stof(numbs));
        //cout << fs.back() << endl;
      }
    }
    auto fsfront = fs.front();
    fs.insert(fs.begin(), fsfront);
    fs.insert(fs.begin(), fsfront);
    fs.insert(fs.begin(), fsfront);
  }
  {
    ifstream lbstream("fb.lhcb");
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
    fb.insert(fb.begin(), fbfront);
    fb.insert(fb.begin(), fbfront);
    fb.insert(fb.begin(), fbfront);
  }
  vector<double> frac;
  vector<double> central_lb;
  vector<double> errLw_lb;
  vector<double> errUp_lb;
  const double nevets = 8140017175.;
  //const auto lumi = 176.01482; // /nb
  //const auto lumi = 0.17601482; // /pb
  const auto lumi = .17601482/45.; // /pb
  const auto BR = 0.33;
  // fonll, dsigma/dpt pb/GeV
  for (size_t i=0; i<fb.size(); ++i) {
    const auto fractemp = fb.at(i)/(1+fs.at(i));
    cout << fractemp << endl;
    frac.push_back(fractemp);
    const auto yields = central.at(i) * BR * lumi/ nevets * fractemp;
    //cout << yields << endl;
    const auto errLwYields = errLw.at(i) * BR * lumi/ nevets * fractemp;
    const auto errUpYields = errUp.at(i) * BR * lumi/ nevets * fractemp;
    central_lb.push_back(yields);
    errLw_lb.push_back(errLwYields);
    errUp_lb.push_back(errUpYields);
  }
  TGraphAsymmErrors gB(npt, pTMean.data(), central.data(), pTMeanError.data(), pTMeanError.data(), errLw.data(), errUp.data());
  TGraphAsymmErrors gLb(npt, pTMean.data(), central_lb.data(), pTMeanError.data(), pTMeanError.data(), errLw_lb.data(), errUp_lb.data());
  TFile ofile("fonll.root", "recreate");
  gB.Write("btoBmeosns_dsigmaOverdPt");
  gLb.Write("btoLamBtoLamC_dNOverdLamBPt");
}
