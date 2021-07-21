#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('skimTree.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

kinsCut = r.KineCut(0., 1000., 0., 1000)

ts = r.TStopwatch()
ts.Start()
r.skimTree('lc.list', 'lambdacAna', 'LambdaCKsProton', '.', LambdaC, kinsCut, -1)
ts.Stop()
ts.Print()
