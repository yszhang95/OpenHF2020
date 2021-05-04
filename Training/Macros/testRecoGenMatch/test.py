#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('../genMatch.cc+')

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
r.genMatchFS('test.list', 'lambdacAna_mc', 'LambdaCKsProton', 'output', LambdaC, kinsCut, -1, False)
ts.Stop()
ts.Print()
