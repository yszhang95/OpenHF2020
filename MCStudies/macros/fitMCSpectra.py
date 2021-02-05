#!/usr/bin/env python
import ROOT as r
print "imported ROOT"

r.gROOT.LoadMacro("MCLamC.C++")
print "ready for fit"
r.MCLamC("dataSet.root")
