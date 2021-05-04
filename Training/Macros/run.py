#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('genMatch.cc+')

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
print Ks.longLived()
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

kinsCut = r.KineCut(0., 1000., 0., 1000)

ts = r.TStopwatch()
ts.Start()
#r.genMatchFS('lc.list', 'lambdacAna_mc', 'LambdaCKsProton', '/eos/cms/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/LambdaC-KsPr_LCpT-0p9_PbP-EmbEPOS_8p16_Pythia8/PA8TeV_PbP_LamC_pT0p9_MC_20210312', LambdaC, kinsCut, -1, True)
r.genMatchFS('lc.list', 'lambdacAna_mc', 'LambdaCKsProton', 'output', LambdaC, kinsCut, 100, True)
ts.Stop()
ts.Print()
