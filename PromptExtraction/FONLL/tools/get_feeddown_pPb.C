const double pb = 1;
const double nb = 1E3;
const double inv_pb = 1;
const double inv_nb = 1E-3;
const double lumi = 4.217 * inv_nb; // nb^-1
const double nevents = 8.7E9;
const double A = 208;
// table 3, https://www.hepdata.net/record/ins1829739
// Phys.Rev.Lett. 127 (2021) 202301
// Phys. Rev. C 104 (2021) 054905
// const double RpA[] = {1.10, 1.30, 1.60, 1.78,  1.66, 1.31};
//
// Scan from https://alice-figure.web.cern.ch/node/21660
// https://alice-figure.web.cern.ch/system/files/figures/akalteye/RpPb.pdf
// 3.01875  1.14149 0 0 0.63151 0.63151 
// 3.01875  1.14149 0 0 0.212674  0.212674  
// 6  1.16319 0 0 0.518663  0.518663  
// 6  1.15885 0 0 0.236545  0.236545  
// 9.99375  1.16319 0 0 0.859375  0.859375  
// 9.99375  1.16319 0 0 0.214844  0.214844  
const double RpA[] = {1.14, 1.163, 1.163, 1.163,  1.163, 1.163};
const double RpA_Err[] = {0.66, 0.57, 0.57, 0.57,  0.57, 0.885};

// modify is not useful yet. waiting for HIN-22-001
void get_feeddown_pPb(const std::string model="FONLL", const bool modify_RpA=false, const bool modifyFF=false,
    const bool modifyFONLL=false, const bool modify=false)
{
  std::string inname;
  std::string outname;
  if (model == "FONLL") {
    if (modifyFF) {
      inname = "pp_8p16TeV_b_to_Lambda_c_modifyFF.root";
      outname = modify_RpA ? "bdecay_frac_ModifyRpA_modifyFF.root" : "bdecay_frac_modifyFF.root";
    } else if (modifyFONLL) {
      inname = "pp_8p16TeV_b_to_Lambda_c_modifyFONLL.root";
      outname = "bdecay_frac_modifyFONLL.root";
      if (modify_RpA) {
        std::cout << "Cannot modify two options at the same time\n";
        return;
      }
    } else {
      inname = "pp_8p16TeV_b_to_Lambda_c.root";
      outname = modify_RpA ? "bdecay_frac_ModifyRpA.root" : "bdecay_frac.root";
    }
  }
  if (model == "PYTHIA") {
    inname = "pp_8p16TeV_b_to_Lambda_c_pythia.root";
    outname = "bdecay_frac_pythia.root";
  }
  TFile fbdecay_pp(inname.c_str());
  TFile finc("spectra_dz1p0_MB_LamC.root");
  auto hbdecay_pp = fbdecay_pp.Get<TH1D>("hLc_pt");
  auto hinc_pPb = finc.Get<TH1D>("MB");

  // TFile ofile("out_frac_pPb.root", "recreate");
  TFile ofile(outname.c_str(), "recreate");

  TH1D* h = (TH1D*)hbdecay_pp->Clone("hbdecay_pPb");
  TH1D* hfrac = (TH1D*)hbdecay_pp->Clone("hfrac");

  for (int ipt=0; ipt<hbdecay_pp->GetNbinsX(); ++ipt) {
    double dsigdpt_bdecay_pp = hbdecay_pp->GetBinContent(ipt+1) * pb;
    double err_dsigdpt_bdecay_pp = hbdecay_pp->GetBinError(ipt+1) * pb;
    const double R = modify_RpA ? RpA[ipt] + RpA_Err[ipt] : RpA[ipt];
    double dNdpt = R * A *dsigdpt_bdecay_pp * lumi / nevents;
    double err_dNdpt = R * A * err_dsigdpt_bdecay_pp * lumi / nevents;
    double dNdpt_inc = hinc_pPb->GetBinContent(ipt+1);
    double frac = dNdpt/dNdpt_inc;
    std::cout << dNdpt << "\t" << dNdpt_inc << "\t" << frac << "\n";
    h->SetBinContent(ipt+1, dNdpt);
    h->SetBinError(ipt+1, err_dNdpt);
    hfrac->SetBinContent(ipt+1, frac);
  }

  ofile.cd();
  h->Write();
  hfrac->Write();
}
