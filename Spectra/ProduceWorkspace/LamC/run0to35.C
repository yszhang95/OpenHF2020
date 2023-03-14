void run_weighted(const std::string& trig="dzp10", const std::string type="")
{
  const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  const int Ntrkofflines[] = {0, 35};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      std::string config = ::string_format("pT%.0fto%.0f_Ntrk%dto%d.conf", pts[ipt], pts[ipt+1], Ntrkofflines[itrk], Ntrkofflines[itrk+1]);
      config = trig + "_weighted" + type + "/" + config;
      std::cout << config << "\n";
      produceWorkspaceLamC(config);
    }
  }
}

void run0to35(const std::string& trig="dz1p0")
{
  run_weighted(trig);
  run_weighted(trig, "_lo");
  run_weighted(trig, "_up");
}
