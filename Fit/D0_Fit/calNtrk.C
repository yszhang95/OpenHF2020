#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

void calNtrk()
{
  FitParConfigs configs("test.txt");
  auto inputConfigs = configs.getInputConfigs();
  const auto files = inputConfigs.getPaths("Data");
  double nevts = 0;
  for (const auto & file : files) {
    TFile infile(file.c_str());
    TH1D* h = (TH1D*) infile.Get("hNtrkofflineDz1p0;1");
    auto n = h->GetEntries();
    std::cout << n << std::endl;
    nevts += n;
  }
  std::cout << nevts << std::endl;
}
