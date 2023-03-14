
void run_unweighted(const float pTMin, const float pTMax)
{
  const std::string trig="dz1p0";

  std::string config = ::string_format("pT%.0fto%.0f_MB.conf", pTMin, pTMax);
  config = trig + "_unweighted/" + config;
  std::cout << config << "\n";
  produceWorkspaceLamC(config);
}


void run_unweighted()
{
  const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    run_unweighted(pts[ipt], pts[ipt+1]);
  }
}

void runMB(const float pTMin, const float pTMax)
{
  run_unweighted(pTMin, pTMax);
}

void runMB()
{
  run_unweighted();
}
