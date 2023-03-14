void run_weighted(const std::string& trig="dzp10")
{
  const float pts[] = {2, 3};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  const int Ntrkofflines[] = {0, 35};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      std::string config = ::string_format("pT%.0fto%.0f_Ntrk%dto%d.conf", pts[ipt], pts[ipt+1], Ntrkofflines[itrk], Ntrkofflines[itrk+1]);
      config = trig + "_weighted/" + config;
      if (trig != "dz1p0" && itrk == nNtrkofflines-1) continue;
      std::cout << config << "\n";
      produceWorkspaceLamC(config);
    }
  }
}

void run0to35_pT2to3(const std::string& trig="dz1p0")
{
  run_weighted(trig);
}
