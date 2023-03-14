#include "reweight.C"

void run_D0()
{
  const int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const int ntrk = sizeof(Ntrks)/sizeof(int) - 1;
  /*
  const std::vector<std::string > func_forms = {
    "[0]*exp([1]*x)", // 0 -- 35
    // "[0]*exp([1]*x + [2]*x*x)", // 0 -- 35
    "[0]*exp([1]*x + [2]*x*x)", // 35 -- 60
    "[0]*exp([1]*x*x+[2]*x)", // 60 -- 90
    "[0]*exp([1]*x*x+[2]*x)", // 90 -- 120
    "[0]*exp([1]*x*x+[2]*x)", // 120 --185
  };
  const std::vector<std::vector<double> > pars = {
    {0.000327293, -0.5}, // 0 -- 35
    // {0.000327293, -0.5, 0}, // 0 -- 35
    {0.00327293, -0.8, -0.05}, // 35 -- 60
    {0.000327293, -0.5, -0.5}, // 60 -- 90
    {0.000327293, -0.5, -0.5}, // 90 -- 120
    {0.000327293, -0.5, -0.5} // 120 -- 185
  };
  */
  const std::vector<std::string > func_forms = {
    "[0]*exp([1]*x)", // 0 -- 35
    // "[0]*exp([1]*x + [2]*x*x)", // 0 -- 35
    "[0]*exp([1]*x)", // 35 -- 60
    "[0]*exp([1]*x)", // 60 -- 90
    "[0]*exp([1]*x)", // 90 -- 120
    "[0]*exp([1]*x)", // 120 --185
  };
  const std::vector<std::vector<double> > pars = {
    {0.000327293, -0.5}, // 0 -- 35
    // {0.000327293, -0.5, 0}, // 0 -- 35
    {0.00327293, -0.8}, // 35 -- 60
    {0.000327293, -0.5}, // 60 -- 90
    {0.000327293, -0.5}, // 90 -- 120
    {0.000327293, -0.5} // 120 -- 185
  };
  for (int itrk=0; itrk<ntrk; ++itrk) {
    std::string name = ::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]);
    std::cout << "Processing " << name << "\n";
    reweight tool(::Form("D0_%s.root", name.c_str()));
    tool.init_fit_func(func_forms.at(itrk));
    // tool.init_fit_func("gaus");
    std::vector<double> init_pars = pars.at(itrk);
    tool.set_init_pars(init_pars.begin(), init_pars.end());
    tool.init_raw("../../Spectra/D0Spectra/yields_dz1p0.root", name.c_str());
    const auto n_pt_bins = tool.n_pt_bins();
    // initializeation;
    std::map<std::string, std::vector<std::string > > gen_hists;
    std::map<std::string, std::vector<std::string > > reco_hists;
    // std::map<std::string, std::vector<std::string > > raw_hists;
    gen_hists["../../Efficiency/D0/AccEff_PromptMC_mva0p560000_reweight.root"]
      = std::vector<std::string >();
    reco_hists["../D0/AccEff_PromptMC_mva0p560000_reweight.root"] = std::vector<std::string >{};
    for (std::vector<double>::size_type i=0; i<n_pt_bins; ++i) {
      gen_hists.at("../../Efficiency/D0/AccEff_PromptMC_mva0p560000_reweight.root").push_back("gen");
      reco_hists.at("../D0/AccEff_PromptMC_mva0p560000_reweight.root").push_back("reco_std");
    }
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
    std::cout << "Finished " << name << "\n";
  }
}
