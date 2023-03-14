// what I want to do is to reweight the gen spectra with data.
// I might have the following in hands:
// - raw pT spectra without efficiency correction
// - efficiency table with fine binning
// I need to do the followings in order:
// - get efficiency-corrected spectra
// - fit the spectra with some function
// - reweight the gen in each bin
// - recalculate the efficiency table
// - go back to the first step

#include <vector>
#include <string>
#include <exception>

#include "TString.h"
#include "TPRegexp.h"
#include "TF1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TObjString.h"
/**
   To run, do the following:
   - initialize histograms in MC reco-level, iter0 -- init_recos
   - initialize histograms in MC gen-level, iter0 -- init_gens
   - initialize the histogram for raw spectra -- init_raw
   - compute initial efficiency -- compute_effs("iter0")
   - compute the initialized spectra iter0 -- compute_spectra("iter0")
   - initialize the initial list of parameters -- init_fit_func and set_init_pars
   - fit the initialized spectra iter0 -- fit_to_spectra("iter0")
   - iteratively do compute efficiency, compute spectra, fit
     - compute efficiency -- compute_effs("iter1") -- iterN
     - compute spectrum -- compute_spectra("iter1") -- iterN
     - fit the spectrum -- fit_to_spectra("iter1") -- iterN
 */

class reweight
{
public:

  unsigned short current_iter;
  unsigned short max_iters;

  reweight(const std::string);
  ~reweight();
  // current design only works when write() is called at the end of usage of reweight
  void write() { _fout->Write(); _has_write = true; }
  void init_fit_func(const std::string func);
  template<class iterator_type>
  void set_init_pars(iterator_type, iterator_type);
  void init_recos(const std::map<std::string, std::vector<std::string > >&);
  void init_gens(const std::map<std::string, std::vector<std::string > >&);
  void init_raw(const std::string, const std::string);
  void compute_effs(const std::string);
  void compute_spectra(const std::string);
  double fit_to_spectra(const std::string);
  double iter(unsigned int);
  const std::vector<double> get_pt_min() const { return _pt_min; }
  const std::vector<double> get_pt_max() const { return _pt_max; }
  const std::map<std::string, std::vector<TH1D*> >  get_recos() const { return _recos; }
  const std::map<std::string, std::vector<TH1D*> > get_gens() const { return _gens; }
  const std::map<std::string, TH1D*> get_effs() const { return _effs; }
  const TH1* get_raw() const { return _hraw; }
  const std::vector<double>::size_type n_pt_bins() const { return _pt_min.size(); }
  TDirectory* get_dir(const std::string&);

    std::vector<double> _pulls;

private:
  TH1D* init_hists(const std::string&, const std::string&);
  std::vector<TH1D*> init_hists(const std::map<std::string, std::vector<std::string> >&);
  void init_pts_from_raw();
  template<class iterator_type>
  void set_pts(iterator_type, iterator_type);

  std::pair<bool, std::string> get_prev_key(const std::string&);

  bool is_equal_floats(const double f1, const double f2,
                      const double precision=1E-6)
  {
    return std::abs((f1-f2)/f1) < precision;
  }

  std::map<std::string, TF1*> _funcs; // fit the spectra; owned by _spectra, does not delete twice
  std::map<std::string, TH1D*> _effs; // efficiency tables
  std::map<std::string, TH1D*> _spectra; // spectra after corrections
  std::map<std::string, std::vector<TH1D*> > _gens; // gen spectra for different MC samples
  std::map<std::string, std::vector<TH1D*> > _recos; // scale factor for each bin
  TH1D* _hraw;
  std::vector<double> _init_pars;
  std::vector<double> _pts;
  std::vector<double> _pt_min;
  std::vector<double> _pt_max;
  TFile* _fout;

  std::string _f_form;

  bool _has_write;

};

reweight::reweight(const std::string output):
  current_iter(0), max_iters(100), _hraw(nullptr), _has_write(0)
{
  _fout = new TFile(output.c_str(), "RECREATE");
}

reweight::~reweight()
{
  auto clear_hists = [](std::map<std::string, std::vector<TH1D*> > hs)
  {
    for (auto& p : hs) {
      for (auto& h : p.second) {
        delete h;
      }
    }
  };

  for (auto& p : _funcs) { delete p.second; }
  for (auto& p : _effs) { delete p.second; }

  if (!_has_write) _fout->Write();

  clear_hists(_gens);
  clear_hists(_recos);
  delete _hraw;
  delete _fout;
}

void reweight::init_fit_func(const std::string func)
{
  _f_form = func;
}

void reweight::init_pts_from_raw()
{
  std::vector<double> pts;
  if (!_hraw) {
    throw std::logic_error("Histogram for raw yields is not initialized");
  }
  const auto nbins = _hraw->GetNbinsX();
  for (int ibin=0; ibin<nbins+1; ++ibin) {
    pts.push_back(_hraw->GetXaxis()->GetBinUpEdge(ibin));
  }
  // for (unsigned int ibin=0; ibin<nbins; ++ibin) {
  //   if (_hraw->GetBinContent(ibin+1)) {
  //     break;
  //   } else {
  //     pts.erase(pts.begin() + ibin);
  //   }
  // }
  // for (int ibin=0; ibin<nbins; ++ibin) {
  //   if (_hraw->GetBinContent(nbins-ibin)) {
  //     break;
  //   } else {
  //     pts.erase(pts.begin() + nbins - 1 - ibin);
  //   }
  // }
  set_pts(pts.cbegin(), pts.cend());
}

void reweight::init_raw(const std::string file, const std::string name)
{
  TFile f(file.c_str(), "READ");
  f.GetObject(name.c_str(), _hraw);
  if (!_hraw) {
    throw std::logic_error("Bad file name or histogram name for raw");
  }
  _hraw->SetDirectory(_fout);
  init_pts_from_raw();
}

template<class iterator_type>
void reweight::set_pts(iterator_type pt_begin, iterator_type pt_end)
{
  // never use []
  _pts.assign(pt_begin, pt_end);
  const auto _pt_1 = _pts.begin();
  const auto _pt_2 = std::next(_pt_1);
  const auto _pt_r1 = _pts.end();
  const auto _pt_r2 = std::prev(_pt_r1);
  _pt_min.assign(_pt_1, _pt_r2);
  _pt_max.assign(_pt_2, _pt_r1);
}

template<class iterator_type>
void reweight::set_init_pars(iterator_type first, iterator_type last)
{
  _init_pars.assign(first, last);
}

// efficiency tables from different files and names must be provided
// files will be saved to fout
// only call this function at the beginning
void reweight::init_recos(const std::map<std::string, std::vector<std::string > >& input)
{
  const std::string key = ::Form("iter%u", 0);
  _recos[key] = init_hists(input);
}

// gen spectra from different files and names must be provided
// files will be saved to fout
// only call this function at the beginning
void reweight::init_gens(const std::map<std::string, std::vector<std::string > >& input)
{
  const std::string key = ::Form("iter%u", 0);
  _gens[key] = init_hists(input);
}

std::pair<bool, std::string>
reweight::get_prev_key(const std::string& iter_key)
{
  bool has_prev = true;
  TPRegexp pattern("(.*iter)(\\d*)");
  TObjArray *subStrL = pattern.MatchS(iter_key.c_str());
  const Int_t nrSubStr = subStrL->GetLast()+1;
  std::string prev_iter_key;
  if (nrSubStr > 2) {
    const TString prefix = ((TObjString *)subStrL->At(1))->GetString();
    const TString iter = ((TObjString *)subStrL->At(2))->GetString();
    int n = iter.Atoi();
    if (n>0) n -= 1;
    else has_prev = false;
    prev_iter_key = ::Form("%s%d", prefix.Data(), n);
  } else {
    throw std::logic_error("Check the iter_key format!");
  }
  delete subStrL;
  return std::pair<bool, std::string>{has_prev, prev_iter_key};
}

void reweight::compute_effs(const std::string iter_key)
{
  // let us assume
  // - pt_min and pt_max falls on the edges of the binning of hist. efficiency
  // - and each element in _gens[key] and _recos[keys] represents one pt bin
  const std::string effName = ::Form("eff_%s", iter_key.c_str());
  if (_pts.empty()) {
    throw std::logic_error("Pt binnings was not initialized."
                           " Please try reweight:set_pts");
  }
  TH1D* heff = new TH1D(effName.c_str(), ";p_{T};Eff. #times Acc.",
                        n_pt_bins(), _pts.data());
  heff->Sumw2();

  const auto _reco = _recos.at("iter0");
  const auto _gen = _gens.at("iter0");

  auto temp_key = get_prev_key(iter_key);
  const auto has_prev = temp_key.first;
  std::string prev_iter_key = temp_key.second;
  std::string curr_iter_key = iter_key;

  const TF1* _f = _funcs.find(prev_iter_key) != _funcs.end() ?
    _funcs.at(prev_iter_key) : nullptr;
  const auto n = _pts.size() - 1;
  for (unsigned int ipt=0; ipt<n; ++ipt) {
    const auto hgen = _gen.at(ipt);
    const auto hreco = _reco.at(ipt);
    const auto nbins = hgen->GetNbinsX();
    auto hgen_clone = (TH1D*) hgen->Clone("gen_clone");
    auto hreco_clone = (TH1D*) hreco->Clone("reco_clone");
    for (int ibin=0; ibin<nbins; ++ibin) {
      const double x = hgen_clone->GetBinCenter(ibin+1);
      const double y_gen = hgen_clone->GetBinContent(ibin+1);
      const double y_reco = hreco_clone->GetBinContent(ibin+1);
      const double y_fit = _f ? _f->Eval(x) : y_gen;
      // keep y_gen / y_reco the same for this bin
      const double y_gen_new = y_gen / y_fit;
      const double y_reco_new = y_reco / y_fit;
      hgen_clone->SetBinContent(ibin+1, y_gen_new);
      hreco_clone->SetBinContent(ibin+1, y_reco_new);
    }
    // may be redundant
    // assume pt_min and pt_max falls on the bin edges;
    auto get_integral = [=](TH1* h, double pt_min, double pt_max)
      -> double {
      auto bw = h->GetBinWidth(1);
      auto bin1 = h->FindBin(pt_min + 0.5 * bw);
      auto bin2 = h->FindBin(pt_max - 0.5 * bw);
      return h->Integral(bin1, bin2);
    };
    const auto gen_part = get_integral(hgen_clone,
                                       _pt_min.at(ipt), _pt_max.at(ipt));
    const auto gen_full = hgen_clone->Integral();
    const auto reco_part = get_integral(hreco_clone,
                                       _pt_min.at(ipt), _pt_max.at(ipt));
    const auto reco_full = hreco_clone->Integral();

    const double y_gen = is_equal_floats(gen_part, gen_full) ?
      gen_full : gen_part;
    const double y_reco = is_equal_floats(reco_part, reco_full) ?
      reco_full : reco_part;

    const double eff = y_reco / y_gen;

    heff->SetBinContent(ipt+1, eff);

    delete hgen_clone;
    delete hreco_clone;
  }
  auto dir = get_dir(iter_key.c_str());
  heff->SetDirectory(dir);
  if (_effs.find(iter_key) != _effs.end()) delete _effs.at(iter_key);
  _effs[iter_key] = heff;
}

void reweight::compute_spectra(const std::string iter_key)
{
  if (_pts.empty()) {
    throw std::logic_error("Pt binnings was not initialized."
                           " Please try reweight:set_pts");
  }
  const std::string hname = ::Form("spectra_%s", iter_key.c_str());
  const std::string titles = ::Form("%s;p_{T} (GeV)"
                                    ";dN^{raw}/dp_{T}/(Eff.#timesAcc.)",
                                    iter_key.c_str());
  auto h = (TH1D*) _hraw->Clone(hname.c_str());
  h->Sumw2();
  h->SetTitle(titles.c_str());
  h->Divide(_effs.at(iter_key));
  auto dir = get_dir(iter_key);
  h->SetDirectory(dir);
  if (_spectra.find(iter_key) != _spectra.end())
    delete _spectra.at(iter_key);
  _spectra[iter_key] = h;
}

TH1D*
reweight::init_hists(const std::string& filename,
                     const std::string& histname)
{
  _fout->cd();
  TH1D* htemp;
  unsigned int iter = 0;
  const std::string dir_name = ::Form("iter%u", iter);
  auto dir = get_dir(dir_name);
  dir->cd();
  auto f = TFile(filename.c_str());
  f.GetObject(histname.c_str(), htemp);
  if (htemp) {
    auto hclone = (TH1D*)htemp->Clone(::Form("%s_iter%u",
                                             htemp->GetName(), iter));
    hclone->SetDirectory(dir);
    return hclone;
  } else {
    throw std::logic_error("Cannot find histgram " + histname + " in " + filename);
  }

  return nullptr;
}


double reweight::fit_to_spectra(const std::string iter_key)
{
  auto dir = get_dir(iter_key);
  // if (_funcs.find(iter_key) != _funcs.end())
  //   delete _funcs.at(iter_key);
  // _funcs[iter_key] = new TF1(iter_key.c_str(), _f_form.c_str(),
  //                            _pts.front(), _pts.back());
  // _funcs.at(iter_key)->SetDirectory(dir);
  TF1* ftemp = new TF1(iter_key.c_str(), _f_form.c_str(),
                             _pts.front(), _pts.back());

  auto temp_key = get_prev_key(iter_key);
  const auto has_prev = temp_key.first;
  std::string prev_iter_key = temp_key.second;
  std::string curr_iter_key = iter_key;

  if (!has_prev) {
    if (_init_pars.size() == ftemp->GetNpar()) {
      ftemp->SetParameters(_init_pars.data());
    } else {
      throw std::logic_error("Check the size of initialized parameters");
    }
  } else {
    ftemp->SetParameters(_funcs.at(prev_iter_key)->GetParameters());
  }

  _spectra.at(curr_iter_key)->Fit(ftemp,
                                  "WQNR", "", _pts.front(), _pts.back());
  _spectra.at(curr_iter_key)->Fit(ftemp,
                                  "WQNR", "", _pts.front(), _pts.back());
  auto result = _spectra.at(curr_iter_key)->Fit(ftemp,
                                  "WERSQ", "", _pts.front(), _pts.back());
  result->Print();
  std::cout << "Chi2: " << result->Chi2() << "\n";

  // delete ftemp;
  // _funcs[iter_key] = dynamic_cast<TF1*>(_spectra.at(curr_iter_key)
  //                                       ->GetListOfFunctions()
  //                                       ->FindObject(iter_key.c_str()));
  if (_funcs.find(iter_key) != _funcs.end()) delete _funcs.at(iter_key);
  _funcs[iter_key] = ftemp;

  double pull = 0;
  if (has_prev) {
    for(int i=0; i<_funcs.at(curr_iter_key)->GetNpar(); ++i) {

      pull += std::pow( (_funcs.at(prev_iter_key)->GetParameter(i)
                         - _funcs.at(curr_iter_key)->GetParameter(i))/
                        _funcs.at(prev_iter_key)->GetParError(i)
                       , 2);
    }
  }

  return pull;
}

double reweight::iter(unsigned int iter)
{
  std::string iter_key = ::Form("iter%u", iter);
  // if (iter>0) fit_to_spectra(iter_key);
  compute_effs(iter_key);
  compute_spectra(iter_key);
  const auto pull = fit_to_spectra(iter_key);
  _pulls.push_back(pull);
  return pull;
}

std::vector<TH1D*> reweight::init_hists(const std::map<std::string, std::vector<std::string> >& input)
{
  std::vector<TH1D*> hs;
  for (const auto& p : input) {
    const auto& filename = p.first;
    for (const auto& h : p.second) {
      auto hptr = init_hists(filename, h);
      hs.push_back(hptr);
    }
  }
  return hs;
}

TDirectory* reweight::get_dir(const std::string& dir_name)
{
  auto dir = _fout->GetDirectory(dir_name.c_str());
  if (!dir) {
    dir = _fout->mkdir(dir_name.c_str(), dir_name.c_str());
  }
  return dir;
}
