void run_unweighted(const std::string& trig="dzp10")
{
  const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  const int Ntrkofflines[] = {0, 35, 60, 90, 120, 185};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      if (ipt ==0 && itrk == nNtrkofflines-1) continue;
      std::string config = ::string_format("pT%.0fto%.0f_Ntrk%dto%d.conf", pts[ipt], pts[ipt+1], Ntrkofflines[itrk], Ntrkofflines[itrk+1]);
      config = trig + "_unweighted/" + config;
      if (trig != "dz1p0" && itrk == nNtrkofflines-1) continue;
      std::cout << config << "\n";
      produceWorkspaceLamC(config);
    }
  }
}

void run_weighted(const std::string& trig="dz1p0", const int weightType=0)
{
  const float pts[] = {3, 4, 5, 6, 8, 10};
  // const float pts[] = {3, 4};
  // const float pts[] = {4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  const int Ntrkofflines[] = {185, 250};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      std::string config = ::string_format("pT%.0fto%.0f_Ntrk%dto%d.conf", pts[ipt], pts[ipt+1], Ntrkofflines[itrk], Ntrkofflines[itrk+1]);
      switch (weightType) {
        case -1 : 
          config = trig + "_weighted_lo/" + config; break;
        case 0 :
          config = trig + "_weighted/" + config; break;
        case 1:
          config = trig + "_weighted_up/" + config; break;
        default:
          return;
      }
      std::cout << config << "\n";
      produceWorkspaceLamC(config);
    }
  }
}

void run(const std::string& trig="dz1p0")
{
  run_unweighted(trig);
  run_weighted(trig);
  run_weighted(trig, -1);
  run_weighted(trig, 1);
}
