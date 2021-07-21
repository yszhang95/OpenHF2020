#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('calLumi.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

ts = r.TStopwatch()
ts.Start()
r.skimTree('lc.list', 'lambdacAna', 'LambdaCKsProton', '.', -1)
ts.Stop()
ts.Print()
