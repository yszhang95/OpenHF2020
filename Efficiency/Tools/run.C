#include "reweight.C"

void run()
{
  reweight tool("output.root");
  tool.init_fit_func("[0]*exp([1]*x*x+[2]*x)");
  // tool.init_fit_func("gaus");
  std::vector<double> init_pars{0.000327293, -0.5, -0.5};
  tool.set_init_pars(init_pars.begin(), init_pars.end());
  tool.init_raw("../../Spectra/LcSpectra/yields_dz1p0.root", "Ntrk60to90");

  const auto n_pt_bins = tool.n_pt_bins();

  // initializeation;
  std::map<std::string, std::vector<std::string > > gen_hists;
  std::map<std::string, std::vector<std::string > > reco_hists;
  // std::map<std::string, std::vector<std::string > > raw_hists;
  gen_hists["../../Efficiency/LamC/gen_reweight_prep_pPb.root"]
    = std::vector<std::string >();
  reco_hists["../LamC/reco_reweight_prep_MB_pPb.root"] = std::vector<std::string >{};
  for (std::vector<double>::size_type i=0; i<n_pt_bins; ++i) {
    gen_hists.at("../../Efficiency/LamC/gen_reweight_prep_pPb.root").push_back(::Form("gen_pt%lu", i));
    reco_hists.at("../LamC/reco_reweight_prep_MB_pPb.root").push_back(::Form("hreco_pt%lu", i));
  }
  // raw_hists["../../Spectra/LcSpectra/yields_dz1p0.root"] = "Ntrk60to90";
  tool.init_gens(gen_hists);
  tool.init_recos(reco_hists);
  tool.iter(0);
  tool.iter(1);
  tool.iter(2);
  tool.iter(3);
  tool.iter(4);
  tool.iter(5);
  tool.write();
  for (const auto e : tool._pulls){
    std::cout << e << "\n";
  }
}
