
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

void printTab()
{
  TFile syst("syst.root");
  std::ofstream ofile("summary.txt");
  ofile  << "\\pt & ";
  for (int ibin=0; ibin<nbins; ++ibin) {
    ofile << "$\\noff$";
      if (ibin == nbins-1) ofile << "\\\\\n";
      else ofile << " & ";
  }
  ofile << "& ";
  for (int ibin=0; ibin<nbins; ++ibin) {
    ofile << Ntrks[ibin] <<  "--" << Ntrks[ibin+1];
      if (ibin == nbins-1) ofile << "\\\\\n";
      else ofile << " & ";
  }
  for (int ipt=0; ipt<nPt; ipt++) {
    ofile << ::Form("%g--%g", pts[ipt], pts[ipt+1]) << " & ";
    for (int ibin=0; ibin<nbins; ++ibin) {
      auto g = syst.Get<TGraphErrors>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
      auto deviation = std::round(g->GetY()[ipt] * 1000.)/10.;
      ofile << deviation << "\\%";
      if (ibin == nbins-1) ofile << "\\\\\n";
      else ofile << " & ";
      delete g;
    }
  }
}
