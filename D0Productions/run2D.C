void run2D(std::string dspat, int istart=1, int nds=0)
{
  gROOT->ProcessLine(".L skim2D.C+");
  if (nds==0) {
    gROOT->ProcessLine(::Form("skim2D(\"%s\")", dspat.c_str()));
  }
  int iend = istart + nds;
  for (int i=istart; i<iend; ++i) {
    // std::cout << ::Form(dspat.c_str(), i) << "\n";
    std::string ds = ::Form(dspat.c_str(), i);
    std::cout << ds << "\n";
    gROOT->ProcessLine(::Form("skim2D(\"%s\")", ds.c_str()));
  }
}
