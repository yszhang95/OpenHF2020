const double pb = 1;
const double nb = 1E3;
const double inv_pb = 1;
const double inv_nb = 1E-3;
const double lumi = 4.217 * inv_nb; // nb^-1
const double nevents = 8.1E9;
const double A = 208;
// table 3, https://www.hepdata.net/record/ins1829739
// Phys.Rev.Lett. 127 (2021) 202301
// Phys. Rev. C 104 (2021) 054905
const double RpA[] = {1.10, 1.30, 1.60, 1.78,  1.66, 1.31};

void cal_frac(const bool modify=false)
{
  TFile fbdecay_pp("pp_8p16TeV_b_to_Lambda_c.root");
  TFile finc("merged_spectra_Ntrk0_185.root");
  auto hbdecay_pp = fbdecay_pp.Get<TH1D>("hLc_pt");
  auto hinc_pPb = finc.Get<TH1D>("Ntrk0to185_merge");

  TFile ofile("out_frac.root", "recreate");

  TH1D* h = (TH1D*)hbdecay_pp->Clone("hbdecay_pPb");

  for (int ipt=0; ipt<hbdecay_pp->GetNbinsX(); ++ipt) {
    double dsigdpt_bdecay_pp = hbdecay_pp->GetBinContent(ipt+1) * pb;
    const double R = modify ? RpA[ipt] : 1;
    double dNdpt = R * A *dsigdpt_bdecay_pp * lumi / nevents;
    double dNdpt_inc = hinc_pPb->GetBinContent(ipt+1);
    double frac = dNdpt/dNdpt_inc;
    std::cout << dNdpt << "\t" << dNdpt_inc << "\t" << frac << "\n";
    h->SetBinContent(ipt+1, dNdpt);
  }

  ofile.cd();
  h->Write();

  TFile previous("../scaledgen.root");
  auto hprev = previous.Get<TH1D>("hLcGen");
  hprev->Scale(208);
  h->Divide(hprev);
  h->Print("all");
}
