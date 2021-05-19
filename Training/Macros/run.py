#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('genMatch.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')
a = r.gSystem.Load('libGenVector.so')
print a

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
r.genMatchFS('lc.list', 'lambdacAna_mc', 'LambdaCKsProton', '', LambdaC, kinsCut, -1, True)
ts.Stop()
ts.Print()
