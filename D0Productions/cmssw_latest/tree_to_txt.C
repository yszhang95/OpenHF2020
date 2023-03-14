void tree_to_txt(std::string input, std::string treename)
{
  TFile f(input.c_str(), "READ");
  TTree* t;
  if (f.IsZombie()) {
    std::cout << "Bad name for input name: " << input << "\n";
    return;
  }
  f.GetObject(treename.c_str(), t);
  if (!t) {
    std::cout << "Bad name for input tree: " << treename << "\n";
    return;
  }
  nentries = t->GetEntries();

  auto pos = input.find(".root");
  if (pos == std::string::npos) {
    std::cout << "Wrong filename for ROOT: " << input << "\n";
    return;
  }
  std::string output = input.replace(pos, 5, ".txt");

  gSystem->Exec(::Form("touch %s", output.c_str()));
  gSystem->RedirectOutput(output.c_str(), "w");
  for (Long64_t ientry=0; ientry<nentries; ++ientry) {
    // t->GetEntry(ientry);
    t->Show(0);
  }
  gSystem->RedirectOutput(nullptr);
}
