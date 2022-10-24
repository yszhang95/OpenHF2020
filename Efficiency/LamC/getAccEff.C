// void setCanvasMargin(TCanvas* c)
void setCanvasMargin(TVirtualPad* c)
{
  c->SetLeftMargin(0.14);
  c->SetRightMargin(0.05);
  c->SetTopMargin(0.1);
  c->SetBottomMargin(0.12);
}
// void setCollision(TCanvas* c)
void setCollision(TVirtualPad* c)
{
  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextAlign(11);
  tex.DrawLatexNDC(0.14, 0.92, "CMS #it{preliminary}");
  tex.SetTextAlign(31);
  tex.DrawLatexNDC(0.95, 0.92, "pPb 8.16 TeV");
}
// void getAcc(TPad* pad)
void getAcc(TVirtualPad* pad)
{
  TFile* f_pPb = TFile::Open("gen_pPb.root");
  TFile* f_Pbp = TFile::Open("gen_Pbp.root");
  TH1D* hAll_pPb = f_pPb->Get<TH1D>("hAll");
  TH1D* hpass_pPb = f_pPb->Get<TH1D>("hPass");
  TH1D* hAll_Pbp = f_Pbp->Get<TH1D>("hAll");
  TH1D* hpass_Pbp = f_Pbp->Get<TH1D>("hPass");

  TGraphAsymmErrors g_pPb(hAll_pPb->GetNbinsX());
  g_pPb.Divide(hpass_pPb, hAll_pPb);

  TGraphAsymmErrors g_Pbp(hAll_Pbp->GetNbinsX());
  g_Pbp.Divide(hpass_Pbp, hAll_Pbp);

  pad->cd();
  auto hframe = pad->DrawFrame(0., 0., 10, 1.3);
  hframe->SetName(::Form("%s_frame", pad->GetName()));
  hframe->SetTitle(";p_{T} (GeV);Acc.");
  g_pPb.SetMarkerStyle(kFullCircle);
  g_pPb.SetMarkerColor(kRed);
  g_Pbp.SetMarkerStyle(kFullSquare);
  g_Pbp.SetMarkerColor(kBlue);
  g_pPb.DrawClone("P");
  g_Pbp.DrawClone("P");
  // g_pPb.Print();
  TLegend leg(0.15, 0.75, 0.35, 0.85);
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.AddEntry(&g_pPb, "pPb", "p");
  leg.AddEntry(&g_Pbp, "Pbp", "p");
  leg.DrawClone();
  TLatex tex;
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.15, 0.6, ::Form("#Lambda_{c}^{#pm}"));
}

// void getEff(TPad* pad, std::string trig)
void getEff(TVirtualPad* pad, std::string trig)
{
  TFile* f_pPb = TFile::Open("gen_pPb.root");
  TFile* f_Pbp = TFile::Open("gen_Pbp.root");
  TFile* freco_pPb = TFile::Open(::Form("reco_%s_pPb.root", trig.c_str()));
  TFile* freco_Pbp = TFile::Open(::Form("reco_%s_Pbp.root", trig.c_str()));
  TH1D* hpass_gen_pPb = f_pPb->Get<TH1D>("hPass");
  TH1D* hpass_gen_Pbp = f_Pbp->Get<TH1D>("hPass");
  TH1D* hpass_reco_pPb = freco_pPb->Get<TH1D>("hpass");
  TH1D* hpass_reco_Pbp = freco_Pbp->Get<TH1D>("hpass");
  // hpass_reco_Pbp->Print("All");
  // hpass_gen_Pbp->Print("All");

  TGraphAsymmErrors g_pPb(hpass_gen_pPb->GetNbinsX());
  g_pPb.Divide(hpass_reco_pPb, hpass_gen_pPb);

  TGraphAsymmErrors g_Pbp(hpass_gen_Pbp->GetNbinsX());
  g_Pbp.Divide(hpass_reco_Pbp, hpass_gen_Pbp);

  if (trig == "HM") {
    g_pPb.RemovePoint(0);
    g_Pbp.RemovePoint(0);
  }

  pad->cd();
  auto hframe = pad->DrawFrame(0., 0., 10, 0.2);
  hframe->SetName(::Form("%s_frame", pad->GetName()));
  hframe->SetTitle(";p_{T} (GeV);Eff.");
  g_pPb.SetMarkerStyle(kFullCircle);
  g_pPb.SetMarkerColor(kRed);
  g_Pbp.SetMarkerStyle(kFullSquare);
  g_Pbp.SetMarkerColor(kBlue);
  g_pPb.DrawClone("P");
  g_Pbp.DrawClone("P");
  // g_pPb.Print();
  TLegend leg(0.15, 0.75, 0.35, 0.85);
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.AddEntry(&g_pPb, "pPb", "p");
  leg.AddEntry(&g_Pbp, "Pbp", "p");
  leg.DrawClone();
  TLatex tex;
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.15, 0.6, ::Form("%s #Lambda_{c}^{#pm}", trig.c_str()));
}

// void getAccEff(TPad* pad, std::string trig)
void getAccEff(TVirtualPad* pad, std::string trig)
{
  TFile* f_pPb = TFile::Open("gen_pPb.root");
  TFile* f_Pbp = TFile::Open("gen_Pbp.root");
  TFile* freco_pPb = TFile::Open(::Form("reco_%s_pPb.root", trig.c_str()));
  TFile* freco_Pbp = TFile::Open(::Form("reco_%s_Pbp.root", trig.c_str()));
  TH1D* hAll_gen_pPb = f_pPb->Get<TH1D>("hAll");
  TH1D* hAll_gen_Pbp = f_Pbp->Get<TH1D>("hAll");
  TH1D* hpass_reco_pPb = freco_pPb->Get<TH1D>("hpass");
  TH1D* hpass_reco_Pbp = freco_Pbp->Get<TH1D>("hpass");
  // hpass_reco_Pbp->Print("All");
  // hpass_gen_Pbp->Print("All");

  TGraphAsymmErrors g_pPb(hAll_gen_pPb->GetNbinsX());
  g_pPb.Divide(hpass_reco_pPb, hAll_gen_pPb);
  g_pPb.SetName(::Form("AccEff_pPb_%s", trig.c_str()));

  TGraphAsymmErrors g_Pbp(hAll_gen_Pbp->GetNbinsX());
  g_Pbp.Divide(hpass_reco_Pbp, hAll_gen_Pbp);
  g_Pbp.SetName(::Form("AccEff_Pbp_%s", trig.c_str()));

  if (trig == "HM") {
    g_pPb.RemovePoint(0);
    g_Pbp.RemovePoint(0);
  }

  pad->cd();
  auto hframe = pad->DrawFrame(0., 0., 10, 0.2);
  hframe->SetName(::Form("%s_frame", pad->GetName()));
  hframe->SetTitle(";p_{T} (GeV);Acc. #times Eff.");
  g_pPb.SetMarkerStyle(kFullCircle);
  g_pPb.SetMarkerColor(kRed);
  g_Pbp.SetMarkerStyle(kFullSquare);
  g_Pbp.SetMarkerColor(kBlue);
  g_pPb.DrawClone("P");
  g_Pbp.DrawClone("P");
  // g_pPb.Print();
  TLegend leg(0.15, 0.75, 0.35, 0.85);
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.AddEntry(&g_pPb, "pPb", "p");
  leg.AddEntry(&g_Pbp, "Pbp", "p");
  leg.DrawClone();
  TLatex tex;
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.15, 0.6, ::Form("%s #Lambda_{c}^{#pm}", trig.c_str()));
}

void getAccEffPads(TCanvas* c, std::string trig)
{
  c->Divide(3, 1);

  auto pad1 = c->cd(1);
  pad1->SetName("acc");
  setCanvasMargin(pad1);
  getAcc(pad1);
  setCollision(pad1);

  auto pad2 = c->cd(2);
  pad1->SetName("eff");
  setCanvasMargin(pad2);
  getEff(pad2, trig);
  setCollision(pad2);

  auto pad3 = c->cd(3);
  pad1->SetName("acceff");
  setCanvasMargin(pad3);
  getAccEff(pad3, trig);
  setCollision(pad3);
}

void getAccEff()
{
  TFile ofile("eff.root", "recreate");
  TCanvas* cHM = new TCanvas("cHM", "", 1800, 500);
  cHM->SetLeftMargin(0);
  cHM->SetRightMargin(0);
  cHM->SetTopMargin(0);
  cHM->SetBottomMargin(0);
  getAccEffPads(cHM, "HM");
  cHM->Print("LcEff_HM.png");
  cHM->Print("LcEff_HM.pdf");
  auto g_pPb_HM = (TGraphAsymmErrors*)gROOT->FindObject("AccEff_pPb_HM");
  auto g_Pbp_HM = (TGraphAsymmErrors*)gROOT->FindObject("AccEff_Pbp_HM");
  // g_pPb_HM->Print();
  
  TCanvas* cMB = new TCanvas("cMB", "", 1800, 500);
  cMB->SetLeftMargin(0);
  cMB->SetRightMargin(0);
  cMB->SetTopMargin(0);
  cMB->SetBottomMargin(0);
  getAccEffPads(cMB, "MB");
  cMB->Print("LcEff_MB.png");
  cMB->Print("LcEff_MB.pdf");
  auto g_pPb_MB = (TGraphAsymmErrors*)gROOT->FindObject("AccEff_pPb_MB");
  auto g_Pbp_MB = (TGraphAsymmErrors*)gROOT->FindObject("AccEff_Pbp_MB");

  ofile.cd();
  g_pPb_HM->Write();
  g_pPb_MB->Write();
  g_Pbp_HM->Write();
  g_Pbp_MB->Write();

  delete cHM;
  delete cMB;
}
