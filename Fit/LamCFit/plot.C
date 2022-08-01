using namespace RooFit;
void plot0to35()
{
  RooRealVar cand_mass("cand_mass", "cand_mass", 2.1, 2.47);
  const int nbins = 100;
  const double plot_lo = 2.12;
  const double plot_up = 2.47;
  cand_mass.setRange("full", 2.1, 2.47);
  cand_mass.setRange("signal", 2.165, 2.375);
  cand_mass.setRange("plotable", plot_lo, plot_up);

  auto f = TFile::Open("ofile_ws_pT3to4_yAbs1_mva0p0041_dz1p0_MB_Ntrk0to35.root");
  RooWorkspace* ws = (RooWorkspace*) f->Get("ws_pT3to4_yAbs1_mva0p0041_dz1p0_MB_Ntrk0to35");
  RooDataSet* ds = (RooDataSet*) ws->data("ds_pT3to4_yAbs1_mva0p0041_dz1p0_MB_Ntrk0to35");
  auto fpdf = TFile::Open("ofile_ws_pT3to4_yAbs1_mva0p0041_dz1p0_MB_Ntrk0to35_postfit.root");
  RooWorkspace* ws_postfit = (RooWorkspace*) fpdf->Get("ws_pT3to4_yAbs1_mva0p0041_dz1p0_MB_Ntrk0to35");
  RooAbsPdf* sum = ws_postfit->pdf("sum");
  TCanvas c;
  c.SetTopMargin(0.08);
  c.SetBottomMargin(0.12);
  c.SetLeftMargin(0.14);
  auto mass_frame = cand_mass.frame();
  mass_frame->GetXaxis()->SetRangeUser(plot_lo, plot_up);
  mass_frame->SetTitle(Form(";M_{K_{S}^{0}p} (GeV);Entries /%.03f MeV",
                       (plot_up-plot_lo)/ nbins));
  ds->plotOn(mass_frame, Binning(nbins, plot_lo, plot_up));
  sum->plotOn(mass_frame, NormRange("full"), Range("full"));
  mass_frame->Draw();
  c.Print("Ntrk0to35.png");
  delete f;
}
void plot185to250()
{
}
void plot()
{
  plot0to35();
}
