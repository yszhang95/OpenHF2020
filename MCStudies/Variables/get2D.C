void get2D()
{
  gROOT->SetBatch(kTRUE);
  std::string fileMC = "/storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/TMVA_MCLambdaCKsP4to6MBApp.root";
  std::string treeMC = "lambdacAna_mc/ParticleNTuple";
  ROOT::RDataFrame df(treeMC, fileMC);
  auto h = df.Define("dlsig", [](float dl, float err){return dl/err;}, {"cand_decayLength3D", "cand_decayLengthError3D"})
    .Histo2D({"hAngleVsDLSig", "reconstructed MC matched #Lambda_{c}^{+};3D decay length significance;3D angle;", 50, 0, 10, 66, 0, 3.3}, "dlsig", "cand_angle3D");
  TFile fout("fAngleVsDLSig.root", "recreate");
  h->Write();

  TCanvas c("cAngleVsDLSig", "3D Angle vs. decay length significance", 800, 600);
  h->SetStats(0);
  h->Draw("COLZ");
  c.SetLogz();
  TLatex tex;
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.7, 0.8, "4 < p_{T} < 6 GeV");
  c.Draw();
  c.Print("cAngleVsDLSig.png");
}
