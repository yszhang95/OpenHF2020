#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

void calNtrk()
{
  const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;
  for (int ipt=0; ipt<nPt; ++ipt) {
    std::string configName = ::string_format("configForAlice/pT%gto%g.conf", pts[ipt], pts[ipt+1]);
    FitParConfigs configs(configName);
    auto inputConfigs = configs.getInputConfigs();
    const auto files = inputConfigs.getPaths("Data");
    double nevts = 0;
    for (const auto & file : files) {
      TFile infile(file.c_str());
      TH1D* h = (TH1D*) infile.Get("lambdacAna/hNtrkofflineDz1p0;1");
      auto n = h->GetEntries();
      //std::cout << n << "\n";
      //std::cout << n << "\t";
      nevts += n;
    }
    //std::cout << "\n";
    std::cout << pts[ipt] << " to " << pts[ipt+1] << ": " << nevts << "\n";
  }
}
