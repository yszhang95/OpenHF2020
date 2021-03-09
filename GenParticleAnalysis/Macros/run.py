#!/usr/bin/env python
inList = 'PA8TeV_pPb_LamC_MC_20210302.list'

import ROOT
ROOT.gROOT.LoadMacro("helpers.cc+")
ROOT.gROOT.SetBatch()

tsw = ROOT.TStopwatch()
tsw.Start()
#ROOT.MCMatch.genLcMatchInfo(inList)
ROOT.MCMatch.genKsMatchInfo(inList)
tsw.Stop()
tsw.Print()
