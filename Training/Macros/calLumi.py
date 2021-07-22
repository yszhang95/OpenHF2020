#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('calLumi.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

ts = r.TStopwatch()
ts.Start()
lumi_Pbp = r.calLumi("../scripts/skimTree/data_Pbp.list", 'lambdacAna', '', '.', -1)
lumi_pPb = r.calLumi("../scripts/skimTree/data_pPb.list", 'lambdacAna', '', '.', -1)
ts.Stop()
ts.Print()
print (lumi_Pbp, lumi_pPb)
