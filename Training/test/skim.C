void skim(const TString& name)
{
  TFile f(name+".root");
  TTree* t;
  f.GetObject("lambdacAna_mc/ParticleNTuple", t);
  TFile ofile(name+"_matched.root", "recreate");
  auto copytree = t->CopyTree("cand_matchGEN && abs(cand_y)<1.");
  copytree->Write();
}

void skim()
{
  skim("TMVA_MC_pPb_LambdaCKsP3to4App");
  skim("TMVA_MC_Pbp_LambdaCKsP3to4App_etaFlipped");
}
