import ROOT as r
#r.gSystem.Load("libMyTreeReader")
ts = r.TStopwatch()
ts.Start()
r.gROOT.LoadMacro("helpers.cc++")
r.MCMatch.genMultipleMatch("../Lists/20210217/Merged_PA8TeV_Pbp_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217.list")
r.MCMatch.genMultipleMatch("../Lists/20210217/Merged_PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217.list")
ts.Stop()
ts.Print()
