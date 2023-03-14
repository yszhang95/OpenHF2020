#include "reweight.C"

void run_LamC()
{
  const int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const int ntrk = sizeof(Ntrks)/sizeof(int) - 1;
  const std::vector<std::string > func_forms = {
    "[0]*exp([1]*x)", // 0 -- 35
    // "[0]*exp([1]*x + [2]*x*x)", // 0 -- 35
    //"[0]*exp([1]*x + [2]*x*x)", // 35 -- 60
    "[0]*exp([1]*x)", // 35 -- 60
    //"[0]*exp([1]*x+[2]*x*x)", // 60 -- 90
    "[0]*exp([1]*x)", // 60 -- 90
    //"[0]*exp([1]*x+[2]*x*x)", // 90 -- 120
    "[0]*exp([1]*x)", // 90 -- 120
    //"[0]*exp([1]*x+[2]*x*x)", // 120 --185
    "[0]*exp([1]*x)", // 120 --185
  };
  const std::vector<std::vector<double> > pars = {
    {0.000327293, -0.5}, // 0 -- 35
    // {0.000327293, -0.5, 0}, // 0 -- 35
    //{0.00327293, -0.8, -0.05}, // 35 -- 60
    {0.00327293, -0.5}, // 35 -- 60
    //{2E-5, 0, -0.5}, // 60 -- 90
    {2E-5, -0.5}, // 60 -- 90
    //{1E-4, 0, -0.5}, // 90 -- 120
    {1E-4, 0}, // 90 -- 120
    // {0.000327293, -0.2, -0.5} // 120 -- 185
    {0.00327293, -0.5} // 120 -- 185
  };
  //for (int itrk=0; itrk<ntrk; ++itrk) {
  // for (int itrk=0; itrk<1; ++itrk) {
  // for (int itrk=1; itrk<2; ++itrk) {
  // for (int itrk=2; itrk<3; ++itrk) {
  // for (int itrk=3; itrk<4; ++itrk) {
  for (int itrk=4; itrk<5; ++itrk) {
    std::string name = ::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]);
    std::cout << "Processing " << name << "\n";
    reweight tool(::Form("LamC_%s.root", name.c_str()));
    tool.init_fit_func(func_forms.at(itrk));
    // tool.init_fit_func("gaus");
    std::vector<double> init_pars = pars.at(itrk);
    tool.set_init_pars(init_pars.begin(), init_pars.end());
    tool.init_raw("../../Spectra/LcSpectra/yields_dz1p0.root", name.c_str());
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
    tool.init_gens(gen_hists);
    tool.init_recos(reco_hists);
    tool.iter(0);
    tool.iter(1);
    tool.iter(2);
    tool.iter(3);
    tool.iter(4);
    tool.iter(5);
    tool.iter(6);
    tool.write();
    for (const auto e : tool._pulls){
      std::cout << e << "\n";
    }
    std::cout << "Finished " << name << "\n";
  }
}
