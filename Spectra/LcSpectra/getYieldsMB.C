#include "../ProduceWorkspace/LamC/tools.C"
void getYieldsMB()
{
  TFile ofile(::Form("yields_dz1p0_MB.root"), "recreate");
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h = new TH1D(::Form("MB"),
      ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", 0, 1000),
      nPt, pts);
  for (int ipt=0; ipt<nPt; ++ipt) {
    const char* useWeight = "unweighted";
    std::string configName = ::string_format("../ProduceWorkspace/LamC/dz1p0_%s/pT%.0fto%.0f_MB.conf", useWeight, pts[ipt], pts[ipt+1]);
    cout << configName << endl;
    configName = gSystem->ExpandPathName(configName.c_str());
    FitParConfigs configs(configName);
    auto outputConfigs = configs.getOutputConfigs();
    auto cutConfigs = configs.getCutConfigs();
    VarCuts mycuts(cutConfigs);
    auto names = ::getNames(configs, mycuts);
    names["outdir"] = outputConfigs.getPath("outdir");
    if (*names.at("outdir").rbegin() != '/') names.at("outdir") += "/";
    std::cout << "getNames\n";
    double yields = 0;
    double err = 0;
    getFit(names, yields, err);
    std::cout << "getFit\n";
    double nEvents = countEvents(configName);
    std::cout << nEvents << "\n";
    yields /= nEvents;
    err /= nEvents;
    const double dPt = pts[ipt+1] - pts[ipt];
    yields /= dPt;
    err /= dPt;
    h->SetBinContent(ipt+1, yields);
    h->SetBinError(ipt+1, err);
  }
  ofile.cd();
  h->Write();
  delete h;
}
