#include "../../../Spectra/ProduceWorkspace/D0/produceWorkspace.C"
using namespace RooFit;

void produceWS(std::string cut="std")
{
  // for loop
  const int nPt = 8;
  const float pts[nPt+1] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const float treeMin[nPt] = {2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 8.0, 8.0};
  const float treeMax[nPt] = {3.0, 4.0, 5.0, 6.0, 8.0, 8.0, 10., 10.};

  const int Ntrks[] = {0, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (unsigned int ibin=0; ibin<nbins; ++ibin) {
    for (int ipt=0; ipt<nPt; ++ipt) {
      const std::string configName = ::string_format("D0wKKPiPi_Ntrk%dto%d_%s.txt", Ntrks[ibin], Ntrks[ibin+1], cut.c_str());
      FitParConfigs configs(configName);
      auto inputConfigs = configs.getInputConfigs();
      auto outputConfigs = configs.getOutputConfigs();
      auto par = configs.getParConfigs();
      const auto cutConfigs = configs.getCutConfigs();
      // modify it later
      VarCuts mycuts(cutConfigs);
      mycuts._pTMin = pts[ipt];
      mycuts._pTMax = pts[ipt+1];
      mycuts._NtrkofflineMin = Ntrks[ibin];
      mycuts._NtrkofflineMax = Ntrks[ibin+1];
      const auto treePt = ::Form("D0_pT%.0fto%.0f", treeMin[ipt], treeMax[ipt]);
      prepareWS(configs, mycuts, treePt);
    }
  }
}
